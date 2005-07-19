// checking for exception specification on new

#include <stdlib.h>
#include "config.h"


#ifndef _RWSTD_NO_NAMESPACE

namespace std {

struct bad_alloc { };

}

void* operator new (size_t) throw (std::bad_alloc);

#else   // if defined (_RWSTD_NO_NAMESPACE)

struct bad_alloc { };

void* operator new (size_t) throw (bad_alloc);

#endif   // _RWSTD_NO_NAMESPACE


void* foo ()
{
    void *p = operator new (1);
    return p;
}
