// checking for cv qualifiers on type void

#include "config.h"

#ifdef _RWSTD_NO_NEW_CLASS_TEMPLATE_SYNTAX
#  define _RWSTD_SPECIALIZED_CLASS
#else
#  define _RWSTD_SPECIALIZED_CLASS template<>
#endif

template <class T>
struct S {};

_RWSTD_SPECIALIZED_CLASS
struct S<void> {};

_RWSTD_SPECIALIZED_CLASS
struct S<const void> {};

_RWSTD_SPECIALIZED_CLASS
struct S<volatile void> {};

_RWSTD_SPECIALIZED_CLASS
struct S<const volatile void> {};
