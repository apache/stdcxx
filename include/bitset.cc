/***************************************************************************
 *
 * bitset - definitions of out-of-line bitset members
 *
 * $Id: //stdlib/dev/include/bitset.cc#44 $
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

_RWSTD_NAMESPACE (__rw) { 


_EXPORT
template <class _CharT, class _Traits>
void __rw_bitset (unsigned long *__bits, _RWSTD_SIZE_T __maxbits,
                  const _CharT *__str, _RWSTD_SIZE_T __slen,
                  const _Traits*, _CharT __b0, _CharT __b1,
                  _RWSTD_SIZE_T __pos, _RWSTD_SIZE_T __n,
                  const char *__file, const char *__fun)
{
    const _RWSTD_SIZE_T __wordbits = sizeof *__bits * _RWSTD_CHAR_BIT;

    if (_RWSTD_SIZE_MAX == __slen)
        __slen = _Traits::length (__str);

    // verify that `pos' is a valid offset into `str'
    _RWSTD_REQUIRES (__pos <= __slen,
                     (_RWSTD_ERROR_OUT_OF_RANGE, __file, __fun, __pos, __slen));

    // compute the number of characters in `str' past the offset `pos'
    // that will be checked (although not all of them may necessarily
    // be used)
    const _RWSTD_SIZE_T __nchars =
        __n < (__slen - __pos) ? __n : __slen - __pos;

    // compute the number of bits to initialize from `str'
    const _RWSTD_SIZE_T __nbits = __nchars < __maxbits ? __nchars : __maxbits;

    __str += __pos;

    // compute the number of bytes occupied by `bits'
    const _RWSTD_SIZE_T __nbytes =
        (__maxbits + sizeof *__bits * _RWSTD_CHAR_BIT - 1) / _RWSTD_CHAR_BIT;

    _RWSTD_C::memset (__bits, 0, __nbytes);

    // set all bits but also check any extra characters as required
    for (_RWSTD_SIZE_T __i = 0; __i != __nchars; ++__i) {

        if (_Traits::eq (__b1, __str [__i])) {

            const _RWSTD_SIZE_T __bitno = __nbits - __i - 1;

            if (__i < __nbits)
                __bits [__bitno / __wordbits] |= 1UL << __bitno % __wordbits;
        }
        else {
            // verify that the character is valid
            _RWSTD_REQUIRES (_Traits::eq (__b0, __str [__i]),
                             (_RWSTD_ERROR_INVALID_ARGUMENT, __file, __fun));
        }
    }
}


_EXPORT
template <_RWSTD_SIZE_T _Size, class _CharT, class _Traits> 
_STD::basic_istream<_CharT, _Traits>&  
__rw_extract_bitset (_STD::basic_istream<_CharT, _Traits> &__strm,
                     _STD::bitset<_Size>                  &__x)
{
    typedef _STD::basic_istream<_CharT, _Traits> _Istream;

    _TYPENAME _Istream::iostate __err (__strm.goodbit);


    _STD::bitset<_Size> __tmp;

    _RWSTD_SIZE_T __bit = _Size;

    _TRY {

        // sentry may throw an exception
        const _TYPENAME _Istream::sentry __ipfx (__strm);

        if (__ipfx) {

            // 23.3.5.3, p5 - extract at most _Size chars

            // get next char without extracting
            _TYPENAME _Traits::int_type __c = __strm.rdbuf ()->sgetc ();

            for ( ; __bit; --__bit) {

                if (_Traits::eq_int_type (_Traits::eof (), __c)) {
                    __err = __strm.eofbit;
                    break;
                }

                // convert to char_type and narrow to char
                const char __ch =
                    __strm.narrow (_Traits::to_char_type (__c), _CharT ());

                if ('1' == __ch)
                    __tmp [__bit - 1] = 1;
                else if ('0' != __ch)
                    break;

                __c = __strm.rdbuf ()->snextc ();
            }

            // shift tmp right by the number of outstanding bits
            __tmp >>= __bit;
        }
    }
    _CATCH (...) {
        __strm.setstate (__strm.badbit | _RW::__rw_rethrow);
    }

    if (_Size && _Size == __bit)
        __err |= __strm.failbit;
    else
        __x = __tmp;

    if (__err)
        __strm.setstate (__err);

    return __strm;
}

}   // namespace __rw
