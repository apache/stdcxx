// checking for std::mbstate_t in <wchar.h> and <wctype.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif

// compile-only test:
// check to see if the headers above declared mbstate_t in namespace std
std::mbstate_t dummy_state;
