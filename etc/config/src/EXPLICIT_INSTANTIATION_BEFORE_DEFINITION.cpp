// checking for instantiation before definition

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

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

#define INSTANTIATE_TEMPLATE
#include "instantiation_before_definition.h"

// test exercises the ability to explicitly instantiate a template
// lexically before the definition of the template (exercises, for
// example, IBM VAC++ 7.0 bug #448:
// http://iwww.cvo.roguewave.com/bugzilla/show_bug.cgi?id=448)

int main ()
{
    InstantiatedBeforeDefined<int> ibd;

    return   ibd.defined_first (0)
           + ibd.instantiated_first (0)
           + instantiated_before_defined (0);
}
