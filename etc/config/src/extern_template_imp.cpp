
#include <config.h>

// establish dependencies on the config tests and define config
// macros used in the header below (the are not autodetected
// in headers)
#ifdef _RWSTD_NO_INLINE_MEMBER_TEMPLATE
#  define NO_INLINE_MEMBER_TEMPLATE
#endif   // _RWSTD_NO_INLINE_MEMBER_TEMPLATE

#ifndef _RWSTD_NO_MEMBER_TEMPLATE
#  define NO_MEMBER_TEMPLATE
#endif   // _RWSTD_NO_MEMBER_TEMPLATE

// explicitly instantiate the template defined in the header
#define INSTANTIATE
#include "extern_template_imp.h"
