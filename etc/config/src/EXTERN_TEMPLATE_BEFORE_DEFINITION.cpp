// checking for extern template before definition

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_extern_template_before_definition_imp
  // establish a dependency on the source file to get it
  // compiled first and  link with the object file
  // LDOPTS = ./extern_template_before_definition_imp.o
#endif

#ifdef _RWSTD_NO_IMPLICIT_INCLUSION
   // tell "instantiation_before_definition.h" to #include its
   // implementation file, instantiation_before_definition.cc
#  define INCLUDE_CC_FILE
#endif   // _RWSTD_NO_IMPLICIT_INCLUSION

#ifdef _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE
   // tell "instantiation_before_definition.h" to use explicit
   // function template specialization instead of the extern
   // template extension
#  define SPECIALIZE_FUNCTION_TEMPLATE
#endif   // _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE

#define EXTERN_TEMPLATE
#include "instantiation_before_definition.h"

// test exercises the ability to declare an explicit instantiation
// of a template using the "extern template" syntax and lexically
// before the definition of the template (exercises, for example,
// gcc 4.0.2 bug #24511:
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=24511)

int main ()
{
    InstantiatedBeforeDefined<int> ibd;

    return   ibd.defined_first (0)
           + ibd.instantiated_first (0)
           + instantiated_before_defined (0);
}
