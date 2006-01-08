/***************************************************************************
 *
 * _messages.cc - definition of std::messages members
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include <loc/_ctype.h>


_RWSTD_NAMESPACE (_V3_LOCALE) { 


template <class _CharT>
_RW::__rw_facet_id messages<_CharT>::id;

template <class _CharT>
_TYPENAME messages<_CharT>::string_type
messages<_CharT>::do_get (messages_base::catalog __cat,
                          int                    __set,
                          int                    __msgid,
                          const string_type     &__dfault) const
{
    const char* const __text = _RW::__rw_get_message (__cat, __set, __msgid);
 
    if (__text) {
        typedef _V3_LOCALE::codecvt<_CharT, char, _RWSTD_C::mbstate_t>
            _CodeCvt;

        typedef _TYPENAME _CodeCvt::state_type state_type;

        const _CodeCvt& __codecvt =
            _V3_USE_FACET (_CodeCvt, _RW::__rw_get_locale (__cat));

        if (sizeof (_CharT) == sizeof (char) && __codecvt.always_noconv ()) {
            // lwg issue 19: always_noconv() is only allowed to return
            // true if extern_type and intern_type are one and the same
            return _RWSTD_REINTERPRET_CAST (const _CharT*, __text);
        }

        typedef char_traits<char> CharTraits;

        const _RWSTD_SIZE_T __src_len = CharTraits::length (__text);
        const char* const __src_first = __text;
        const char* const __src_last  = __text + __src_len;
        const char*       __src_next  = __src_first;

        // allocate a [wide] string large enough to hold at least
        // as many internal characters as there are bytes in the
        // multibyte characater message
        string_type __result_str ((const _CharT*)0, __src_len);

        _CharT* const __dst_first = &__result_str [0];
        _CharT* const __dst_last  = __dst_first + __src_len;
        _CharT*       __dst_next  = __dst_first;

        state_type __state = state_type ();

        const codecvt_base::result __res =
            __codecvt.in (__state, __src_first, __src_last, __src_next,
                          __dst_first, __dst_last, __dst_next);

        switch (__res) {
        case codecvt_base::ok:
            // shrink the converted string accordingly
            __result_str.resize (__dst_next - __dst_first);
            return __result_str;

        case codecvt_base::noconv:
            if (sizeof (_CharT) == sizeof (char)) {
                // lwg issue 19: in() is only allowed to return noconv
                // if extern_type and intern_type are one and the same
                return _RWSTD_REINTERPRET_CAST (const _CharT*, __text);
            }
            else {
                // should not happen - bad codecvt implementation

                typedef ctype<_CharT> _Ctype;

                const _Ctype& __ctp =
                    _V3_USE_FACET (_Ctype, _RW::__rw_get_locale (__cat));

                for (_RWSTD_SIZE_T __i = 0; __i != __src_len; ++__i)
                    __dst_first [__i] = __ctp.widen (__text [__i]);

                return __result_str;
            }

        case codecvt_base::partial:
            // should not happen (bad catalog?)
        default:
            break;
        }
    }
    return __dfault;
}


}   // namespace _V3_LOCALE
