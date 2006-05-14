/**************************************************************************
 *
 * rw_allocator.h - user-defined allocator type
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
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

#ifndef RW_ALLOCATOR_INCLUDED
#define RW_ALLOCATOR_INCLUDED


#include <testdefs.h>


struct _TEST_EXPORT SharedAlloc
{
    // identifies each member function of a standard allocator class
    enum MemFun {
        m_ctor, m_cvt_ctor, m_cpy_ctor,
        m_dtor,
        m_allocate, m_deallocate,
        m_construct, m_destroy,
        m_address, m_max_size,
        n_funs
    };

    // constructs an allocator object managing a memory pool
    // of the specified size (bytes and blocks)
    SharedAlloc (_RWSTD_SIZE_T /* max_bytes  */ = _RWSTD_SIZE_MAX,
                 _RWSTD_SIZE_T /* max_blocks */ = _RWSTD_SIZE_MAX);

    virtual ~SharedAlloc ();

    // attempts to allocate storage for nelems objects of elemsize
    // bytes each and returns a pointer to the allocated storage
    // on success; throws std::bad_alloc on failure
    virtual void*
    allocate (_RWSTD_SIZE_T /* nelems */,
              _RWSTD_SIZE_T /* elemsize */,
              const void*   /* hint */);

    // deallocates storage at ptr allocated by a call to allocate
    // for nelems objects of elemsize bytes each
    virtual void
    deallocate (void*         /* ptr */,
                _RWSTD_SIZE_T /* nelems */,
                _RWSTD_SIZE_T /* elemsize */);

    // returns the maximum number of objects of elemsize each
    // that can be allocated from the pool managed by *this
    virtual _RWSTD_SIZE_T
    max_size (_RWSTD_SIZE_T /* elemsize */);

    // records a call to the allocator member function fun and
    // throws an exception derived from std::bad_alloc if the
    // number of calls to the member function reaches the limit
    // specified by throw_at_calls_
    virtual void funcall (MemFun /* fun */);

    // gets or sets a pointer to the global allocator object
    static SharedAlloc*
    instance (SharedAlloc* = 0);

    // returns a unique id of this allocator object
    int id () const { return id_; }

    _RWSTD_SIZE_T max_bytes_;    // memory pool size
    _RWSTD_SIZE_T max_blocks_;   // memory pool size

    _RWSTD_SIZE_T n_bytes_;     // number of allocated bytes
    _RWSTD_SIZE_T n_blocks_;    // number of allocated blocks

    _RWSTD_SIZE_T n_refs_;      // number of references

    // counter of the number of calls to each allocator member function
    _RWSTD_SIZE_T n_calls_ [n_funs];

    // member function counter value that, when reached, will cause
    // an exception derived from std::bad_alloc to be thrown
    _RWSTD_SIZE_T throw_at_calls_ [n_funs];

private:
    int id_;
};


// allocator types for easy specialization
template <class T>
struct AllocTypes
{
    typedef _RWSTD_SIZE_T    size_type;
    typedef _RWSTD_PTRDIFF_T difference_type;
    typedef T*               pointer;
    typedef const T*         const_pointer;
    typedef T&               reference;
    typedef const T&         const_reference;
};


template <class T, class Types = AllocTypes<T> >
struct UserAlloc
{
// private:
    SharedAlloc* const pal_;

public:
    typedef T                               value_type;
    typedef typename Types::size_type       size_type;
    typedef typename Types::difference_type difference_type;
    typedef typename Types::pointer         pointer;
    typedef typename Types::const_pointer   const_pointer;
    typedef typename Types::reference       reference;
    typedef typename Types::const_reference const_reference;

    UserAlloc (SharedAlloc *pal = 0) _THROWS (())
        : pal_ (pal ? pal : SharedAlloc::instance ()) {
        RW_ASSERT (0 != pal_);
        pal_->funcall (pal_->m_ctor);
    }

    UserAlloc (const UserAlloc &rhs) _THROWS (())
        : pal_ (rhs.pal_) {
        RW_ASSERT (0 != pal_);
        pal_->funcall (pal_->m_cpy_ctor);
    }

#ifndef _RWSTD_NO_INLINE_MEMBER_TEMPLATES

    template <class U>
    UserAlloc (const UserAlloc<U> &rhs) _THROWS (())
        : pal_ (rhs.pal_) {
        RW_ASSERT (0 != pal_);
        pal_->funcall (pal_->m_cvt_ctor);
    }

    template <class U>
    struct rebind { typedef UserAlloc<U> other; };

#endif   // _RWSTD_NO_INLINE_MEMBER_TEMPLATES

    ~UserAlloc () {
        pal_->funcall (pal_->m_dtor);
    }

    pointer allocate (size_type nelems, const void *hint = 0) {
        void* const ptr = pal_->allocate (nelems, sizeof (value_type), hint);
        return _RWSTD_STATIC_CAST (pointer, ptr);
    }

    void deallocate (pointer ptr, size_type nelems) {
        pal_->deallocate (ptr, nelems, sizeof (value_type));
    }

    void construct (pointer ptr, const_reference val) {
        pal_->funcall (pal_->m_construct);
        new (ptr) value_type (val);
    }

    void destroy (pointer ptr) { 
        pal_->funcall (pal_->m_destroy);
        ptr->~T ();
    }

    pointer address (reference ref) const {
        pal_->funcall (pal_->m_address);
        return &ref;
    }

    const_pointer address (const_reference ref) const {
        pal_->funcall (pal_->m_address);
        return &ref;
    }

    size_type max_size () const _THROWS (()) {
        return pal_->max_size (sizeof (value_type));
    }

    bool operator== (const UserAlloc &rhs) const {
        return pal_->id () == rhs.pal_->id ();
    }

    bool operator!= (const UserAlloc &rhs) const {
        return !operator== (rhs);
    }
};


#endif   // RW_ALLOCATOR_INCLUDED
