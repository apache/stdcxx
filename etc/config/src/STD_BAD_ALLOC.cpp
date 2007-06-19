// checking for class std::bad_alloc

#include "config.h"

#if !defined (_RWSTD_NO_NEW_THROWS) && !defined (_RWSTD_NO_NEW_OFLOW_SAFE)
#  include <stddef.h>   // for size_t
#endif   // !_RWSTD_NO_NEW_THROWS && !_RWSTD_NO_NEW_OFLOW_SAFE

#ifndef _RWSTD_NO_SETRLIMIT
// test for setrlimit() presence before compiling current file
#endif

// proclimits.h must be included only after #if[n]def _RWSTD_NO_SETRLIMIT
#include "proclimits.h"

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

loop:

    while ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();            // recurse infinitely
    }

    goto loop;
}

}   // namespace std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD

// will fail if namespaces aren't supported
namespace std {

struct bad_alloc { };

}

// force a failure if namespace std isn't honored
enum bad_alloc { };


#if defined (_MSC_VER)

#include <crtdbg.h>  // for _CrtSetReportMode()

void disable_msc_gui ()
{
    _CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
}

#else   // if !defined (_MSC_VER)

void disable_msc_gui () { /* no-op */ }

#endif   // _MSC_VER


int main ()
{
    // prevent MSVC's libc from popping up a dialog box
    // with an out-of-memory message
    disable_msc_gui ();

    try {

#ifndef _RWSTD_NO_NEW_THROWS
#  ifndef _RWSTD_NO_NEW_OFLOW_SAFE

        void *p = ::operator new (size_t (-1));

        if (!p)
            return 1;

#  else   // if defined (_RWSTD_NO_NEW_OFLOW_SAFE)

        limit_memory (0);

        for (unsigned long n = 1UL << (sizeof (long) * 8 - 1);
             0 != n; n |= (n >> 1)) {

            // try to allocate a huge amount of memory
            void *p = ::operator new (n);

            if (!p)
                return 1;
        }

#  endif   // _RWSTD_NO_NEW_OFLOW_SAFE
#else   // if defined (_RWSTD_NO_NEW_THROWS)

        return 4;

#endif

    }
    catch (std::bad_alloc&) {
        return 0;
    }
    catch (...) {
        return 2;
    }

    return 3;
}
