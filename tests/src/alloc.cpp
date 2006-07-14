/************************************************************************
 *
 * alloc.cpp - definitions of rw_alloc and rw_free
 *
 * $Id: alloc.cpp
 *
 ************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <assert.h>   // for assert()
#include <stdlib.h>   // for atoi(), getenv(), malloc(), free()
#include <string.h>   // for memset()

#include <algorithm>

#ifdef __CYGWIN__
// use the Windows API on Cygwin
#  define _WIN32
#endif

#if !defined (_WIN32) && !defined (_WIN64)
#  ifdef __SUNPRO_CC
// working around SunOS bug #568
#    include <time.h>
#  endif
#  include <unistd.h>     // for getpagesize(), sysconf()
#  include <sys/mman.h>   // for mmap()
#  include <sys/types.h>

#  ifndef _SC_PAGE_SIZE
// fall back on the alternative macro if it exists,
// or use getpagesize() otherwise
#    ifndef _SC_PAGESIZE
#      define GETPAGESIZE()   getpagesize ()
#    else
#      define GETPAGESIZE()   sysconf (_SC_PAGESIZE)
#    endif
#  else
#      define GETPAGESIZE()   sysconf (_SC_PAGE_SIZE)
#  endif   // _SC_PAGE_SIZE

#else   // defined (_WIN32) || defined (_WIN64)

#  include <windows.h>    // for everything (ugh)
#  include <sys/types.h>  // for off_t
#  include <errno.h>      // for errno

#  define GETPAGESIZE()   getpagesize ()

static long getpagesize ()
{
    static long pagesize_ = 0;

    if (0 == pagesize_) {
        SYSTEM_INFO info;
        GetSystemInfo (&info);
        pagesize_ = long (info.dwPageSize);
    }

    return pagesize_;
}

enum {
    PROT_NONE  = 0,
    PROT_READ  = 1 << 0,
    PROT_WRITE = 1 << 1,
    PROT_RDWR  = PROT_READ | PROT_WRITE,
    PROT_EXEC  = 1 << 2,
};

#define MAP_PRIVATE   0
#define MAP_ANONYMOUS 0

static void* const MAP_FAILED = (void*)-1;

static const DWORD prots[8] = {
    PAGE_NOACCESS,
    PAGE_READONLY,
    PAGE_READWRITE,
    PAGE_READWRITE,
    PAGE_EXECUTE,
    PAGE_EXECUTE_READ,
    PAGE_EXECUTE_READWRITE,
    PAGE_EXECUTE_READWRITE
};

static inline DWORD translate_prot(int prot)
{
    if ((0 <= prot) && (sizeof(prots) / sizeof(prots[0]) > prot))
        return prots[prot];

    return PAGE_NOACCESS;
}

static inline void* mmap(void* addr, size_t len, int prot, int, int, off_t)
{
    addr = VirtualAlloc (addr, len, MEM_RESERVE | MEM_COMMIT,
        translate_prot (prot));

    if (addr)
        return addr;

    errno = EINVAL;
    return MAP_FAILED;
}

static inline int munmap(void* addr, size_t)
{
    if (VirtualFree (addr, 0, MEM_RELEASE))
        return 0;
    
    errno = EINVAL;
    return -1;
}

static inline int mprotect(void *addr, size_t len, int prot)
{
    DWORD flOldProt;
    if (VirtualProtect (addr, len, translate_prot (prot), &flOldProt))
        return 0;
    
    errno = EINVAL;
    return -1;
}

#endif   // _WIN{32,64}

#include <rw/_defs.h>
#include <rw_alloc.h>

/************************************************************************/

struct BlockInfo
{
    void*  addr_;   // address of the allocated block
                    // if addr_ == 0 block is unused
    size_t size_;   // size of the allocated block
    void*  data_;   // address of the user data
    size_t udsz_;   // size of the user data
    int    flags_;  // memory protection flags
};

struct Stats
{
    size_t blocks_;     // number of the current allocated blocks
    size_t maxblocks_;  // max number of the allocated blocks
};

static Stats stats_;

struct Blocks
{
    Blocks*   next_;
    size_t    nblocks_;
    BlockInfo blocks_[1];
};

static Blocks* first_ = 0;  // pointer to the first Blocks in list
static Blocks* last_ = 0;   // pointer to the last Blocks in list

struct Pair
{
    void*      addr_;       // pointer to the user data in block
    BlockInfo* info_;       // pointer to the corresponding BlockInfo
};

static Pair* table_            = 0; // pointer to the table
static size_t table_size_      = 0; // size of table in bytes
static size_t table_max_size_  = 0; // max number of items in table

/************************************************************************/

static Pair* _rw_lower_bound (Pair* first, Pair* last, void* addr)
{
    for (size_t dist = last - first; dist > 0; ) {

        const size_t half = dist / 2;
        Pair* const middle = first + half;

        if (middle->addr_ < addr) {
            first = middle + 1;
            dist -= half + 1;
        }
        else
            dist = half;
    }

    return first;
}

static inline Pair* _rw_binary_search (Pair* first, Pair* last, void* addr)
{
    Pair* it = _rw_lower_bound (first, last, addr);
    return (it != last && it->addr_ == addr) ? it : 0;
}

/************************************************************************/

// constructor sets r/w access to the specified memory pages
// destructor sets r/o access to the specified memory pages
class MemRWGuard
{
private:
    void*  addr_;
    size_t size_;

public:
    MemRWGuard (void* addr, size_t size)
    {
        static const size_t pagemask = GETPAGESIZE () - 1;

        // check that pagesize is power of 2
        assert (0 == ((pagemask + 1) & pagemask));
        // addr_ should be aligned to memory page boundary
        size_t off = size_t (addr) & pagemask;
        addr_ = _RWSTD_STATIC_CAST(char*, addr) - off;
        size_ = size + off;

        int res = mprotect (addr_, size, PROT_READ | PROT_WRITE);
        assert (0 == res);
    }

    ~MemRWGuard ()
    {
        int res = mprotect (addr_, size_, PROT_READ);
        assert (0 == res);
    }

private:
    // not defined
    MemRWGuard (const MemRWGuard&);
    MemRWGuard& operator= (const MemRWGuard&);
};

/************************************************************************/

static void _rw_table_free ()
{
    if (!table_)
        return;

    int res = munmap (table_, table_size_);
    assert (0 == res);

    table_          = 0;
    table_size_     = 0;
    table_max_size_ = 0;
}

static bool _rw_table_grow ()
{
    // table_max_size_ cannot be less than allocated blocks
    assert (table_max_size_ >= stats_.blocks_);

    // check for free space in current table
    if (table_max_size_ == stats_.blocks_) {
        // realloc more memory
        static const size_t pagesize = GETPAGESIZE ();

        const size_t new_table_size = table_size_ + pagesize;

        void* new_table = mmap (0, new_table_size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (MAP_FAILED == new_table)
            // no memory available
            return false;

        // copy old table
        memcpy (new_table, table_, stats_.blocks_ * sizeof (Pair));

        // protect the new table
        int res = mprotect (new_table, new_table_size, PROT_READ);
        assert (0 == res);

        // free old table
        _rw_table_free ();

        table_          = _RWSTD_STATIC_CAST (Pair*, new_table);
        table_size_     = new_table_size;
        table_max_size_ = new_table_size / sizeof (Pair);
    }

    return true;
}

// inserts info about newly created BlockInfo
// increments the number of the allocated blocks
static void _rw_table_insert (BlockInfo& info)
{
    Pair* end = table_ + stats_.blocks_;
    Pair* it = _rw_lower_bound (table_, end, info.data_);

    {
        MemRWGuard guard (table_, table_size_);

        // move items [it, end) to the end of table
        memmove (it + 1, it, (end - it) * sizeof (Pair));

        it->addr_ = info.data_;
        it->info_ = &info;
    }

    ++stats_.blocks_;
    if (stats_.blocks_ > stats_.maxblocks_)
        stats_.maxblocks_ = stats_.blocks_;
}

// removes the specified item from table
// decrements the number of the allocated blocks
static void _rw_table_remove (Pair* it)
{
    assert (table_ <= it && table_ + stats_.blocks_ > it);
    size_t index = it - table_;

    MemRWGuard guard (table_, table_size_);
    memmove (it, it + 1, (--stats_.blocks_ - index) * sizeof (Pair));
}

/************************************************************************/

// allocate more blocks (allocates one memory page)
static bool _rw_allocate_blocks ()
{
    // count of the blocks per memory page
    static size_t blocks_per_page = 0;

    static const size_t pagesize = GETPAGESIZE ();

    if (0 == blocks_per_page)
        blocks_per_page = (pagesize - sizeof (Blocks)) / sizeof (BlockInfo) + 1;

    void* buf = mmap (0, pagesize, PROT_READ,
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (MAP_FAILED != buf) {

        MemRWGuard guard (buf, pagesize);

        memset (buf, 0, pagesize);

        Blocks* blocks = _RWSTD_STATIC_CAST(Blocks*, buf);
        blocks->nblocks_ = blocks_per_page;

        if (0 == first_)
            first_ = blocks;
        else
            last_->next_ = blocks;

        last_ = blocks;

        return true;
    }

    return false;
}

// free allocated blocks
// should be called when all user memory allocated
// by rw_alloc were freed by rw_free
static void _rw_free_blocks ()
{
    assert (0 == stats_.blocks_);

    static const size_t pagesize = GETPAGESIZE ();

    while (first_) {
        Blocks* it = first_;
        first_ = first_->next_;
        int res = munmap (it, pagesize);
        assert (0 == res);
    }

    last_ = 0;
}

static BlockInfo * _rw_find_unused_from (Blocks* it)
{
    while (it) {
        for (size_t i = 0; i < it->nblocks_; ++i) {
            BlockInfo & info = it->blocks_ [i];
            if (0 == info.addr_)
                return &info;
        }

        it = it->next_;
    }

    return 0;
}

// returns pointer to the first unused BlockInfo
// if none unused items tries allocate more blocks
// returns 0 if no memory
static BlockInfo * _rw_find_unused ()
{
    BlockInfo * res = _rw_find_unused_from (first_);

    if (!res && _rw_allocate_blocks ()) {
        // find the unused block from newly allocated blocks
        // res = _rw_find_unused_from (last_);
        res = last_->blocks_;
        // res should be != 0
        assert (0 != res);
    }

    return res;
}

// returns pointer to the Pair which corresponds to addr
// returns 0 if addr is not valid pointer, returned by rw_alloc
static Pair * _rw_find_by_addr (void* addr)
{
    Pair* end = table_ + stats_.blocks_;
    return _rw_binary_search (table_, end, addr);
}

static inline int _rw_get_prot (int flags)
{
    return (flags & RW_PROT_READ  ? PROT_READ  : 0)
         | (flags & RW_PROT_WRITE ? PROT_WRITE : 0)
         | (flags & RW_PROT_EXEC  ? PROT_EXEC  : 0);
}

static inline int getenvflags ()
{
    if (const char * envvar = getenv ("RWSTD_ALLOC_FLAGS"))
        return atoi (envvar);

    return 0;
}

/************************************************************************/

_TEST_EXPORT void*
rw_alloc(size_t nbytes, int flags/* = -1*/)
{
    static const int RWSTD_ALLOC_FLAGS = getenvflags ();

    // redefine flags if environment variable was set
    if (-1 == flags && 0 != RWSTD_ALLOC_FLAGS)
        flags = RWSTD_ALLOC_FLAGS;

    // make sure that table has free space
    if (!_rw_table_grow ())
        return 0;

    if (BlockInfo * info = _rw_find_unused ()) {

        BlockInfo newinfo = BlockInfo ();

        newinfo.udsz_ = nbytes;
        newinfo.flags_ = flags;

        if (-1 == flags) {

            newinfo.addr_ = malloc (nbytes);

            if (newinfo.addr_) {
                newinfo.size_ = nbytes;
                newinfo.data_ = newinfo.addr_;
            }
        }
        else {

            static const size_t pagesize = GETPAGESIZE ();

            size_t size = nbytes + pagesize;

            // check that pagesize is power of 2
            assert (0 == (pagesize & (pagesize - 1)));
            size_t offset = size & (pagesize - 1);

            if (offset) {
                offset = pagesize - offset;
                size += offset;
            }

            newinfo.addr_ = mmap (0, size, _rw_get_prot(flags),
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (MAP_FAILED != newinfo.addr_) {

                char* data = _RWSTD_STATIC_CAST (char*, newinfo.addr_);
                char* guard = data;
                
                if (RW_PROT_BELOW & flags)
                    offset = pagesize;
                else
                    guard += size - pagesize;

                // deny access to the guard page
                int res = mprotect (guard, pagesize, PROT_NONE);
                assert (0 == res);

                newinfo.size_ = size;
                newinfo.data_ = data + offset;
            }
        }

        if (newinfo.data_) {

            {
                MemRWGuard guard (info, sizeof (*info));
                *info = newinfo;
            }

            _rw_table_insert (*info);

            return info->data_;
        }
    }

    return 0;
}

_TEST_EXPORT void
rw_free(void* addr)
{
    if (Pair * it = _rw_find_by_addr (addr)) {

        BlockInfo & info = *it->info_;

        if (-1 == info.flags_)
            free (addr);
        else {
            int res = munmap (info.addr_, info.size_);
            assert (0 == res);
        }

        {
            MemRWGuard guard (&info, sizeof (info));
            info = BlockInfo ();
        }

        _rw_table_remove (it);

        if (0 == stats_.blocks_) {
            _rw_free_blocks ();
            _rw_table_free ();
        }
    }
    else
        assert (!"Invalid addr passed to the rw_free");
}
