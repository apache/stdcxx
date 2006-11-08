// checking for extern template extension

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// establish dependencies on the config tests and define config
// macros used in the header below (the are not autodetected
// in headers)
#ifdef _RWSTD_NO_INLINE_MEMBER_TEMPLATE
#  define NO_INLINE_MEMBER_TEMPLATE
#endif   // _RWSTD_NO_INLINE_MEMBER_TEMPLATE

#ifdef _RWSTD_NO_MEMBER_TEMPLATE
#  define NO_MEMBER_TEMPLATE
#endif   // _RWSTD_NO_MEMBER_TEMPLATE

// include a file containing the definition of a template
// and an extern template directive referencing an explicit
// instantiation of the same template in extern_template_imp.o
#include "extern_template_imp.h"

// establish a dependency on extern_template_imp.o to make sure
// the extern_template_imp.cpp is compiled before this file
#ifndef _RWSTD_NO_extern_template_imp

// link with the object file below
// LDOPTS = ./extern_template_imp.o

#endif   // _RWSTD_NO_extern_template_imp

int main ()
{
    S<int> s;

    // call member template functions on a specialization of
    // a class template explicitly instantiated in one file
    // and declared extern template in extern_template_imp.h
    // to detect if the extern template declaration prevents
    // the implicit instantiation of the member templates
    // HP aCC 3 and 5 bug -- see STDCXX-270:
    // http://issues.apache.org/jira/browse/STDCXX-270
    return s.inline_member_template (0) + s.member_template (0);
}
