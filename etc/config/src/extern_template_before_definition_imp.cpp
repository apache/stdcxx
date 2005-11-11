
#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifdef _RWSTD_NO_IMPLICIT_INCLUSION
   // tell "instantiation_before_definition.h" to #include its
   // implementation file, instantiation_before_definition.cc
#  define INCLUDE_CC_FILE
#endif   // _RWSTD_NO_IMPLICIT_INCLUSION

#define INSTANTIATE_TEMPLATE
#include "instantiation_before_definition.h"
