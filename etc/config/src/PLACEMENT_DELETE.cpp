// checking for placement delete

#include "config.h"

#ifndef _RWSTD_NO_EXCEPTION_SPECIFICATION_ON_NEW
#  define _THROWS() throw ()
#else
#  define _THROWS()
#endif

void operator delete (void*) _THROWS ();
void operator delete (void*, void*) _THROWS () { }
