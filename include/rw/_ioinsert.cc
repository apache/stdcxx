/***************************************************************************
 *
 * _ioinsert.cc - definitions for the C++ Standard Library inserter
 *                helper templates
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_select.h>


_RWSTD_NAMESPACE (__rw) { 


_EXPORT
template <class _CharT, class _Traits, class _NativeType>
_STD::basic_ostream<_CharT, _Traits>&
__rw_insert (_STD::basic_ostream<_CharT, _Traits> &__strm,
             _NativeType                           __val)
{
    const typename _STD::basic_ostream<_CharT, _Traits>::sentry
        __opfx (__strm);

    _TRY {

        typedef _STD::ostreambuf_iterator<_CharT, _Traits> _Iter;
        typedef _STD::num_put<_CharT, _Iter>               _NumPut;

        if (__opfx && _STD_USE_FACET (_NumPut, __strm.getloc ())
            .put (_Iter (__strm), __strm, __strm.fill (), __val).failed ()) {

            // this may throw but the exception will just be caught
            // and rethrown below; this optimizes for the common case
            // (i.e., success) but pessimizes the case of failure
            __strm.setstate (_STD::ios_base::badbit);
        }
    }
    _CATCH (...) {
        __strm.setstate (_STD::ios_base::badbit | _RW::__rw_rethrow);
    }

    return __strm;
}


// optimized for when stream's char_type is the same
// as the type of the character string being inserted,
// i.e., ostream and char* and wostream and wchar_t*
template <class _CharT, class _Traits>
inline _RWSTD_STREAMSIZE
__rw_sputn (_STD::basic_ostream<_CharT, _Traits> &__strm,
            const _CharT *__s, _RWSTD_STREAMSIZE __len, __rw_true_t)
{
    _RWSTD_ASSERT (!__len || __len > 0 && 0 != __s);

    if (1 == __len) {
        typedef typename _Traits::int_type _IntT;

        const _IntT __ic = __strm.rdbuf ()->sputc (*__s);

        return 0 + !_Traits::eq_int_type (_Traits::eof (), __ic);
    }

    return __strm.rdbuf ()->sputn (__s, __len);
}


// special case for char* insertion into a stream whose
// char_type is not char (e.g., wostream or user-defined)
template <class _CharT, class _Traits>
inline _RWSTD_STREAMSIZE
__rw_sputn (_STD::basic_ostream<_CharT, _Traits> &__strm,
            const char *__s, _RWSTD_STREAMSIZE __len, __rw_false_t)
{
    _RWSTD_ASSERT (!__len || __len > 0 && 0 != __s);

    const _STD::ctype<_CharT>& __ctp =
        _STD_USE_FACET (_STD::ctype<_CharT>, __strm.getloc ());

    for (_RWSTD_STREAMSIZE __i = 0; __i < __len; ++__i) {

        typedef typename _Traits::int_type _IntT;

        const _IntT __ic = __strm.rdbuf ()->sputc (__ctp.widen (__s [__i]));

        if (_Traits::eq_int_type (__ic, _Traits::eof ()))
            return __i;
    }

    return __len;
}


_EXPORT
template<class _CharT, class _Traits, class _StringT>
_STD::basic_ostream<_CharT, _Traits>&
__rw_insert (_STD::basic_ostream<_CharT, _Traits> &__strm,
             const _StringT                       *__s,
             _RWSTD_STREAMSIZE                     __len,
             _RWSTD_STREAMSIZE                     __width)
{
    _RWSTD_ASSERT (0 != __strm.rdbuf ());
    _RWSTD_ASSERT (0 != __s);
      
    _STD::ios_base::iostate __err = _STD::ios_base::goodbit;

    // writes out max(len, width) characters, padding appropriately
    // with the fill character if (len < width);
    // calls width(0) if and only if the write has been successful
    // (i.e., will leave it unchanged if sputn fails or throws an
    // exception)

    _TRY {
  
        const typename _STD::basic_ostream<_CharT, _Traits>::sentry
            __opfx (__strm);

        if (__opfx) {

            // compute the number of fill chars to pad with
            // according to the rules described in 22.2.2.2.2, p19
            const _RWSTD_STREAMSIZE __pad = __width - __len;

            typedef typename
                __rw_same_type<_CharT, _StringT>::_C_type _Same;

            if (__pad > 0) {
                const int __padbits =
                    _STD::ios_base::adjustfield & __strm.flags ();

                // output left padding (output is right-aligned by default)
                if (   _STD::ios_base::left != __padbits
                    && __pad != __strm._C_pad (__pad)
                    // write out (not necessarily null-terminated) string
                    || __len != __rw_sputn (__strm, __s, __len, _Same ())
                    // output right padding (output is left-aligned)
                    || _STD::ios_base::left == __padbits
                    && __pad != __strm._C_pad (__pad))
                    __err = _STD::ios_base::badbit;
#ifndef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
                else
                    __strm.width (0);   // reset width only on success
#else   // if defined (_RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE)
                __strm.width (0);   // reset width unconditionally
#endif   // _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
            }
            else if (__len == __rw_sputn (__strm, __s, __len, _Same ())) {
                __strm.width (0);   // reset width only on success
            }
            else {
                __err = _STD::ios_base::badbit;

#ifdef _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
                __strm.width (0);
#endif   // _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
            }
        }
    }
    _CATCH (...) {
        // set badbit and rethrow the caught exception if badbit
        // is also set in exceptions()
        __strm.setstate (_STD::ios_base::badbit | _RW::__rw_rethrow);
    }

    if (__err) {
        // set badbit which may throw ios_base::failure
        __strm.setstate (__err);
    }

    return __strm;
}


}   // namespace __rw
