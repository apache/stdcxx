// -*- C++ -*-
/***************************************************************************
 *
 * complex.cc - definitions of the template complex and specializations
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
 * Copyright 2000-2006 Rogue Wave Software.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (std) { 


// complex number `a' raised to an integer power `n'
//     a**n = r**n * (cos(n theta) + i sin (n theta))
_EXPORT
template <class _TypeT>
complex<_TypeT>
pow (const complex<_TypeT>& __a, int __n)
{
    if (__a == complex<_TypeT>())
        return complex<_TypeT>(0 == __n);

    if (_TypeT () == __a.imag ()) {
        if (__a.real () < _TypeT ())
            return pow (__a, complex<_TypeT>(__n));

        return complex<_TypeT>(_RWSTD_C::pow (__a.real (), __n));
    }

    _TypeT __r  = _RWSTD_C::pow (_TypeT (abs (__a)), __n);
    _TypeT __th = __n * arg (__a);

    return complex<_TypeT>(__r * _RWSTD_C::cos (__th),
                           __r * _RWSTD_C::sin (__th));
}


// complex number `a' raised to a real power `s'
//     a**s = exp(s * log(a))
_EXPORT
template <class _TypeT>
complex<_TypeT>
pow (const complex<_TypeT> &__a, const _TypeT &__s)
{
    if (__a == complex<_TypeT>())
        return complex<_TypeT>(_TypeT () == __s);

    if (_TypeT () == __a.imag ()) {
        if (__a.real () < _TypeT ())
            return pow (__a, complex<_TypeT>(__s));

        return complex<_TypeT>(_RWSTD_C::pow (__a.real (), __s));
    }
    return exp (__s * log (__a));
}


// real number `s' raised to a complex power `a'
//     s**a = exp(a * log (s))
_EXPORT
template <class _TypeT>
complex<_TypeT>
pow (const _TypeT& __s, const complex<_TypeT>& __a)
{
    if (_TypeT() == __s)
        return complex<_TypeT>(complex<_TypeT>() == __a);

    if (__s < _TypeT ())
        return pow (complex<_TypeT>(__s), __a);

    if (_TypeT () == __a.imag ())
        return complex<_TypeT>(_RWSTD_C::pow (__s, __a.real ()));

    return complex<_TypeT>(exp(__a * _TypeT (_RWSTD_C::log (__s))));
}


// complex<T> number `a1' raised to `a' complex<T> power `a2'
//     a1**a2 = rho * (cos(phi) + i sin(phi))
//     rho = r1 ** u2   *  exp (-v2 * theta1)
//     phi = v2 * log(r1) + u2 * theta1
_EXPORT
template <class _TypeT>
complex<_TypeT>
pow (const complex<_TypeT>& __x, const complex<_TypeT>& __y)
{
    if (complex<_TypeT>() == __x)
        return complex<_TypeT>(complex<_TypeT>() == __y);

    _TypeT __r1  = abs (__x);
    _TypeT __u2  = real (__y);
    _TypeT __v2  = imag (__y);
    _TypeT __th1 = arg (__x);
    _TypeT __rho = _RWSTD_C::pow (__r1, __u2) * _RWSTD_C::exp (-__v2 * __th1);
    _TypeT __phi = __v2 * _RWSTD_C::log (__r1) + __u2 * __th1;

    return complex<_TypeT>(__rho * _RWSTD_C::cos (__phi),
                           __rho * _RWSTD_C::sin (__phi));
}      


_EXPORT
template <class _TypeT, class _CharT, class _Traits>
basic_istream<_CharT, _Traits >&
operator>> (basic_istream<_CharT, _Traits>& __strm, complex<_TypeT> &__val)
{
    // read a complex number in one of the following forms:
    // "r", "(r)", "(r, i)"

    // lwg issue 156: extraction is performed as a series of simpler
    // extractions. Therefore, the skipping of whitespace is specified
    // to be the same for each of the simpler extractions.

    _TypeT __re = _TypeT (),
           __im = _TypeT ();

    _CharT __ch;

    if (!(__strm >> __ch))
        return __strm;

    if ('(' == __ch) {
        if (__strm >> __re && __strm >> __ch) {
            if (',' == __ch && __strm >> __im)
                __strm >> __ch;

            // avoid setting failbit if either badbit
            // or failbit is already set in the stream
            if (')' != __ch && __strm)
                __strm.setstate (__strm.failbit);
        }
    }
    else if (__strm.putback (__ch)) {
        __strm >> __re;
    }

    if (__strm)
      __val = complex<_TypeT>(__re, __im);

    return __strm;
}


_EXPORT
template <class _TypeT, class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>&
operator<< (basic_ostream<_CharT, _Traits >& __strm,
            const complex<_TypeT>&           __val)
{
    basic_ostringstream<_CharT, _Traits, allocator<_CharT> > __ss;
    __ss.flags (__strm.flags ());
    __ss.imbue (__strm.getloc ());
    __ss.precision (__strm.precision ());
    __ss << '(' << __val.real () << ',' << __val.imag () << ')';
    return __strm << __ss.str ();
}


}   // namespace std
