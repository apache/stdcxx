/***************************************************************************
 *
 * string.cc - definitions of the C++ Standard Library string members
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifdef _RWSTD_MSVC
   // shut up the dumb MSVC warning C4345:
   // behavior change: an object with POD type constructed with
   // an initializer of the form () will be default-initialized
#  pragma warning (push)
#  pragma warning (disable: 4345)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) {


#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT
#  if !defined (_RWSTD_MSVC) || _RWSTD_MSVC > 1300
   // MSVC 7.0 allows initializers for static const integral members
   // but out-of-line definitions cause multiply defined symbol errors
   // (see PR #26562)

#    ifndef _RWSTD_NO_STRING_NPOS_TYPE

template <class _CharT, class _Traits, class _Allocator>
const typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
npos /* = SIZE_MAX */;

#    else   // if defined (_RWSTD_NO_STRING_NPOS_TYPE)

template <class _CharT, class _Traits, class _Allocator>
const _RWSTD_SIZE_T
basic_string<_CharT, _Traits, _Allocator>::
npos /* = SIZE_MAX */;

#    endif   // _RWSTD_NO_STRING_NPOS_TYPE
#  endif   // MSVC > 7.0
#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_INIT


#ifndef _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS

template <class _CharT, class _Traits, class _Allocator>
_RW::__null_ref<_CharT, _Traits, _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
_C_null_ref;

#endif   // _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::_C_string_ref_type*
basic_string<_CharT, _Traits, _Allocator>::
_C_get_rep (size_type __cap, size_type __len)
{
    if (__cap > max_size ()) {
        __cap = __len;

        _RWSTD_REQUIRES (__cap <= max_size (),
                         (_RWSTD_ERROR_LENGTH_ERROR,
                          _RWSTD_FUNC ("basic_string::_C_get_rep (size_type, "
                                       "size_type)"),
                          __cap, max_size ()));
    }

    _RWSTD_REQUIRES (__len <= __cap,
                     (_RWSTD_ERROR_LENGTH_ERROR,
                      _RWSTD_FUNC ("basic_string::_C_get_rep (size_type, "
                                   "size_type)"),
                      __len, __cap));

    if (!__cap) {
        _RWSTD_ASSERT (!__len);
        return _C_nullref ();
    }

    // allocate, initialize the __string_ref, and initialize each character
    _C_string_ref_type* const __ret =
        _RWSTD_STATIC_CAST (_C_string_ref_type*,
            _RWSTD_STATIC_CAST (void*,
                _RWSTD_VALUE_ALLOC (_C_value_alloc_type, *this,
                    allocate (  __cap + sizeof (_C_string_ref_type)
                              / sizeof (value_type) + 2))));

    // avoid copy construction (mutex isn't copy-constructible)
    // _C_ref_alloc_type (*this).construct (__ret, _C_string_ref_type ());
    new (__ret) _C_string_ref_type ();

    // set initial reference count, capacity, and length
    __ret->_C_init (__cap, __len);

    _RWSTD_VALUE_ALLOC (_C_value_alloc_type, *this,
                        construct (__ret->data () + __len, value_type ()));

    return __ret;
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
basic_string (const basic_string &__s, size_type __pos, size_type __n,
              const allocator_type& __alloc /* = allocator_type () */)
    : allocator_type (__alloc)
{
    _RWSTD_REQUIRES (__pos <= __s.size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE,
                      _RWSTD_FUNC ("basic_string::basic_string (const "
                                   "basic_string&, size_type, size_type)"),
                      __pos, __s.size ()));

    size_type __slen = __s.size () - __pos;
    size_type __rlen = __n < __slen ? __n : __slen; 
    size_type __nlen =  __n == npos ? 0 : __n;
    size_type __maxlen = __nlen > __rlen ? __nlen : __rlen; 
    if (__maxlen) 
        _C_data = _C_get_rep (_C_grow (size_type (), __maxlen),
                              __rlen)->data ();
    else
        _C_data = _C_get_rep (__maxlen, __rlen)->data ();

    traits_type::copy (_C_data, &__s._C_data [__pos], __rlen);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
basic_string (size_type __n, value_type __c, 
              const allocator_type &__alloc /* = allocator_type () */)
    : allocator_type (__alloc)
{
    _RWSTD_REQUIRES (__n <= max_size (),
                     (_RWSTD_ERROR_LENGTH_ERROR,
                      _RWSTD_FUNC ("basic_string::basic_string (size_type, "
                                   "value_type)"),
                      __n, max_size ()));

    if (__n)
        _C_data = _C_get_rep (_C_grow (size_type (), __n), __n)->data ();
    else
        _C_data = _C_nullref ()->data ();

    traits_type::assign (_C_data, __n, __c);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
basic_string (const_pointer __s, size_type __n,
              const allocator_type &__alloc /* = allocator_type () */)
    : allocator_type (__alloc)
{
    // extension: if `s' is 0 then `n' uninitialized elements are allocated

    _RWSTD_REQUIRES (__n <= max_size (),
                     (_RWSTD_ERROR_LENGTH_ERROR,
                      _RWSTD_FUNC ("basic_string::basic_string (const_pointer,"
                                   "size_type, const allocator_type&)"),
                      __n, max_size ()));

    _C_data = __n ?
        _C_get_rep (_C_grow (size_type (), __n), __n)->data ()
      : _C_nullref ()->data ();

    if (__s)
        traits_type::copy (_C_data, __s, __n);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
basic_string (const_pointer __s,
              const allocator_type &__alloc /* = allocator_type () */)
    : allocator_type (__alloc)
{     
    _RWSTD_ASSERT (__s != 0);

    const size_type __n = traits_type::length (__s);

    _C_data = __n ?
        _C_get_rep (_C_grow (size_type (), __n), __n)->data ()
      : _C_nullref ()->data ();

    traits_type::copy (_C_data, __s, __n);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
basic_string (const_pointer __first, const_pointer __last, 
              const allocator_type &__alloc /* = allocator_type () */)
    : allocator_type (__alloc)
{
    const size_type __n = size_type (__last - __first);

    _C_data = __n ?
        _C_get_rep (_C_grow (size_type (), __n), __n)->data ()
      : _C_nullref ()->data ();

    traits_type::copy (_C_data, __first, __n);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>&
basic_string<_CharT, _Traits, _Allocator>::
operator= (const basic_string &__rhs)
{
    if (size_type (0) < size_type (__rhs._C_pref ()->_C_get_ref ())) {
        // `rhs' has reference counting enabled
        __rhs._C_pref ()->_C_inc_ref ();
        _C_unlink (__rhs._C_data);
    }
    else if (this != &__rhs)
        // `rhs' has reference counting disabled
        replace (size_type (), size (), __rhs.data (), __rhs.size ());

    return *this;
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>&
basic_string<_CharT, _Traits, _Allocator>::
assign (const basic_string &__str, size_type __pos, size_type __n)
{
    _RWSTD_REQUIRES (__pos <= __str.size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE,
                      _RWSTD_FUNC ("basic_string::assign (basic_string&, "
                                   "size_type, size_type)"),
                      __pos, __str.size ()));

    const size_type __rlen = _C_min (__str.size () - __pos, __n);

    return replace (size_type (), size (), __str, __pos, __rlen);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>&
basic_string<_CharT, _Traits, _Allocator>::
insert (size_type __pos1,
        const basic_string& __str, size_type __pos2, size_type __n)
{
    _RWSTD_REQUIRES (__pos1 <= size () && __pos2 <= __str.size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE,
                      _RWSTD_FUNC ("basic_string::insert (size_type, const "
                                   "basic_string&, size_type, size_type)"), 
                      __pos1 > size () ? __pos1:__pos2, __str.size ()));
    
    const size_type __rlen = _C_min (__str.size () - __pos2, __n);

    _RWSTD_REQUIRES (size () <= max_size () - __rlen,
                     (_RWSTD_ERROR_LENGTH_ERROR,
                      _RWSTD_FUNC ("basic_string::insert (size_type, const "
                                   "basic_string&, size_type, size_type)"), 
                      size (), max_size () - __rlen));

    return replace (__pos1, size_type (), __str, __pos2, __n);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>&
basic_string<_CharT, _Traits, _Allocator>::
insert (size_type __pos1, const basic_string &__str)
{
    _RWSTD_REQUIRES (__pos1 <= size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE,
                      _RWSTD_FUNC ("basic_string::insert (size_type, const "
                                   "basic_string&)"),
                      __pos1, size ()));
    
    _RWSTD_REQUIRES (size () <= max_size () - __str.size (),
                     (_RWSTD_ERROR_LENGTH_ERROR,
                      _RWSTD_FUNC ("basic_string::insert (size_type, "
                                   "const basic_string&)"),
                      size (), max_size () - __str.size ()));

    return replace (__pos1, size_type (), __str);
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>&
basic_string<_CharT, _Traits, _Allocator>::
replace (size_type __pos1, size_type __n1, const_pointer __s, size_type __n2)
{
    const size_type __size0 = size ();

    if (npos == __n2)
        __n2 = traits_type::length (__s);

    _RWSTD_REQUIRES (__pos1 <= __size0,
                     (_RWSTD_ERROR_OUT_OF_RANGE, 
                     _RWSTD_FUNC ("basic_string::replace (size_type, size_type"
                                  ", const_pointer, size_type)"), 
                      __pos1, __size0 > __n2 ? __size0 : __n2));

    // number of characters to delete
    const size_type __xlen = _C_min (__n1, __size0 - __pos1);

    _RWSTD_REQUIRES (__n2 <= max_size (),
                     (_RWSTD_ERROR_LENGTH_ERROR,
                     _RWSTD_FUNC ("basic_string::replace (size_type, size_type"
                                  ", const_pointer, size_type)"), 
                     __n2, max_size ()));

    _RWSTD_REQUIRES (__size0 - __xlen <= max_size () - __n2,
                     (_RWSTD_ERROR_LENGTH_ERROR,
                     _RWSTD_FUNC ("basic_string::replace (size_type, size_type"
                                  ", const_pointer, size_type)"), 
                      __size0 - __xlen, max_size () - __n2));

    // compute the resulting string size
    const size_type __size1 = __size0 - __xlen + __n2;

    if (__size1) {

        // compute the length of the bit at the end
        const size_type __rem = __size0 - __xlen - __pos1;

        // check for shared representation, insufficient capacity,
        // and overlapping regions
        if (   size_type (1) < size_type (_C_pref ()->_C_get_ref ())
            || capacity () < __size1
            || __s >= data () && __s < data () + __size0) {

            // Need to allocate a new reference.
            const size_type __cap = _C_grow (__size0, __size1);

            const pointer __data = _C_get_rep (__cap, __size1)->data ();

            traits_type::copy (__data, _C_data, __pos1);

            traits_type::copy (__data + __pos1, __s, __n2);

            traits_type::copy (__data + __pos1 + __n2,
                               _C_data + __pos1 + __n1, __rem);

            _C_unlink (__data);
        }
        else {
            // current reference has enough space

            const pointer __beg = _C_data + __pos1;

            traits_type::move (__beg + __n2, __beg + __n1, __rem);

            traits_type::copy (__beg, __s, __n2);

            traits_type::assign (_C_data [__size1], value_type ());

            _C_pref ()->_C_size._C_size = __size1;
        }
    }
    else {
        // special case a substitution that leaves the string empty.
        clear ();
    }

    return *this;
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>&
basic_string<_CharT, _Traits, _Allocator>::
replace (size_type __pos, size_type __len, size_type __count, value_type __val)
{
    // replaces `len' characters at position `pos'
    // with `count' copies of the character `val'

    const size_type __size0 = size ();

    _RWSTD_REQUIRES (__pos <= __size0,
                     (_RWSTD_ERROR_OUT_OF_RANGE, 
                      _RWSTD_FUNC ("basic_string::replace (size_type, "
                                   "size_type, size_type, value_type)"),
                      __pos, __size0));

    const size_type __xlen = _C_min (__size0 - __pos, __len);

    _RWSTD_REQUIRES (__size0 - __xlen <= max_size () - __count,
                     (_RWSTD_ERROR_LENGTH_ERROR,
                      _RWSTD_FUNC ("basic_string::replace (size_type, "
                                   "size_type, size_type, value_type)"), 
                      __size0 - __xlen, max_size () - __count));

    // compute the resulting string size
    const size_type __size1 = __size0 - __xlen + __count;

    if (__size1) {

        // compute the length of the bit at the end
        const size_type __rem = __size0 - __xlen - __pos;

        // check for shared representation, insufficient capacity
        if (   capacity () < __size1
            || size_type (1) < size_type (_C_pref ()->_C_get_ref ())) {

            // need to allocate a new reference
            const size_type __cap = _C_grow (__size0, __size1);

            const pointer __data = _C_get_rep (__cap, __size1)->data ();

            traits_type::copy (__data, _C_data, __pos);

            traits_type::assign (__data + __pos, __count, __val);

            traits_type::copy (__data + (__pos + __count),
                               _C_data + (__pos + __len), __rem);

            _C_unlink (__data);
        }
        else {
            // current reference is not shared and has enough space

            const pointer __beg = _C_data + __pos;

            traits_type::move (__beg + __count, __beg + __len, __rem);

            traits_type::assign (__beg, __count, __val);

            // append the terminating NUL character
            traits_type::assign (_C_data [__size1], value_type ());

            _C_pref ()->_C_size._C_size = __size1;
        }
    }
    else {
        // special case a substitution that leaves the string empty.
        clear ();
    }

    return *this;
}


#ifdef _RWSTD_NO_STRING_OUTLINED_MEMBER_TEMPLATES

_EXPORT
template <class _CharT, class _Traits, class _Alloc,
          class _StringIter, class _InputIter>
_STD::basic_string<_CharT, _Traits, _Alloc>& 
__rw_replace (_STD::basic_string<_CharT, _Traits, _Alloc> &__s, 
              _StringIter __first1, _StringIter __last1,
              _InputIter __first2, _InputIter __last2) {

    typedef _Traits                               traits_type;
    typedef typename traits_type::char_type      value_type;
    typedef _Alloc                                allocator_type;
    typedef typename allocator_type::size_type   size_type;

    typedef _STD::basic_string<_CharT, _Traits, _Alloc> _C_string_type;

    typedef _RW::__string_ref<value_type, traits_type, allocator_type>
     _C_string_ref_type;


#else   // !defined (_RWSTD_NO_STRING_OUTLINED_MEMBER_TEMPLATES)

template<class _CharT, class _Traits, class _Allocator>
template<class _InputIter>
basic_string<_CharT, _Traits, _Allocator>& 
basic_string<_CharT, _Traits, _Allocator>::
replace (iterator __first1, iterator __last1,
         _InputIter __first2, _InputIter __last2, void*)
{
    typedef basic_string _C_string_type;

    basic_string &__s = *this;

#endif   // _RWSTD_NO_STRING_OUTLINED_MEMBER_TEMPLATES

    _RWSTD_ASSERT_RANGE (__first1,
                         __s._C_make_iter (__s._C_data + __s.size ()));
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    if (__first2 == __last2) {
        if (__first1 == __last1)
            return __s;

        const size_type __pos = __s._C_off (__first1);
        const size_type __n = __s._C_off (__first1, __last1);
        return __s.replace (__pos, __n, size_type (), value_type ());
    }

     // use a (probably) faster algorithm if possible
    if (_STD::__is_bidirectional_iterator (_RWSTD_ITERATOR_CATEGORY(_InputIter,
                                                                    __last2)))
        return __s.__replace_aux (__first1, __last1, __first2, __last2);

    _C_string_type __s3;
    typename _C_string_type::iterator __first3 = __s3.begin ();
    
    for ( ; !(__first2 == __last2); ++__first2, ++__first3) {

        const size_type __off = __s3._C_off (__first3);

        _RWSTD_REQUIRES (__off <= __s3.max_size (),
                         (_RWSTD_ERROR_LENGTH_ERROR,
                          _RWSTD_FUNC ("basic_string::replace (iterator, "
                                       "iterator, InputIterator, "
                                       "InputIterator)"),
                          __s3._C_off (__first3), __s3.max_size ()));
         
        // extend the string if necessary
        if (__s3.end () == __first3) {
            // compute the size of new buffer
            const size_type __cap = __s3._C_grow (__s3.size (), size_type ());

            // allocate a new buffer
            _C_string_ref_type *__tmp = __s3._C_get_rep (__cap, __cap);

            // copy data from old to new, leaving a hole for additions
            traits_type::copy (__tmp->data (), __s3._C_data, __off);
            __s3._C_unlink (__tmp->data ());
            __first3 = __s3._C_make_iter (__s3._C_data + __off);
        }

        // copy data over
        traits_type::assign (*__first3, *__first2);
    }

    const size_type __size = __s3._C_off (__first3);
    traits_type::assign (__s3._C_data [__size], value_type ());
    __s3._C_pref ()->_C_size._C_size = __size;

    return __s.__replace_aux (__first1, __last1, __s3.begin (), __s3.end ());
}

// Special function for random access and bi-directional iterators
// Avoids the possibility of multiple allocations
// We still have to copy characters over one at a time.

#ifdef _RWSTD_NO_STRING_OUTLINED_MEMBER_TEMPLATES

_EXPORT
template <class _CharT, class _Traits, class _Alloc,
          class _StringIter, class _InputIter>
_STD::basic_string<_CharT, _Traits, _Alloc>& 
__rw_replace_aux (_STD::basic_string<_CharT, _Traits, _Alloc> &__s, 
                  _StringIter __first1, _StringIter __last1,
                  _InputIter __first2, _InputIter __last2)
{
    typedef _Traits                                   traits_type;
    typedef typename traits_type::char_type          value_type;
    typedef _Alloc                                    allocator_type;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::const_reference const_reference;

    typedef _RW::__string_ref<value_type, traits_type, allocator_type>
    _C_string_ref_type;

#else   // if !defined (_RWSTD_NO_STRING_OUTLINED_MEMBER_TEMPLATES)

template<class _CharT, class _Traits, class _Allocator>
template<class _InputIter>
basic_string<_CharT, _Traits, _Allocator>& 
basic_string<_CharT, _Traits, _Allocator>::
__replace_aux (iterator __first1, iterator __last1, 
               _InputIter __first2, _InputIter __last2)
{
    basic_string &__s = *this;

#endif  // _RWSTD_NO_STRING_OUTLINED_MEMBER_TEMPLATES

    _RWSTD_ASSERT_RANGE (__first1, __s._C_make_iter (__s._C_data 
                                                     + __s.size ()));
    _RWSTD_ASSERT_RANGE (__first1, __last1);
    _RWSTD_ASSERT_RANGE (__first2, __last2);

    const size_type  __n2 = _DISTANCE (__first2, __last2, size_type);
    const size_type  __n  = __s._C_off (__first1, __last1);
    const size_type __pos = __s._C_off (__first1);
    const size_type __ssize = __s.size ();

    _RWSTD_REQUIRES (__pos <= __ssize,
                     (_RWSTD_ERROR_OUT_OF_RANGE,
                      _RWSTD_FUNC ("basic_string::__replace_aux (iterator, "
                                   "iterator, InputIterator, InputIterator)"),
                      __pos, __ssize));

    size_type __slen = __ssize - __pos;
    size_type __xlen = __n < __slen ? __n : __slen; 

    _RWSTD_REQUIRES (__ssize - __xlen <= __s.max_size () - __n2,
                     (_RWSTD_ERROR_LENGTH_ERROR, 
                      _RWSTD_FUNC ("basic_string::__replace_aux (iterator, "
                                   "iterator, InputIterator, InputIterator)"),
                      __ssize - __xlen, __s.max_size () - __n2));

    size_type __len = __ssize - __xlen + __n2;  // Final string length.

    if (!__len) {
        // Special case a substitution that leaves the string empty.
        __s._C_unlink (__s._C_nullref ()->data ());
    }
    else {
        size_type __d = 0;
        // length of bit at the end
        size_type __rem = __ssize - __xlen - __pos;
        // Check for shared representation, insufficient capacity
        if (   __s.capacity () < __len
            || size_type (1) < size_type (__s._C_pref ()->_C_get_ref ()))
        {
            // Need to allocate a new reference.
            const size_type __cap = __s._C_grow (__ssize, __len);

            _C_string_ref_type * __temp = __s._C_get_rep (__cap, __len);
            if (__pos)
                traits_type::copy (__temp->data (), __s._C_data, __pos);
            for (__d = 0; __d < __n2; __d++)
                traits_type::assign (*(__temp->data ()+__pos + __d),
                                     *__first2++);
            if (__rem)
                traits_type::copy (__temp->data () + __pos + __n2,
                                   __s._C_data + __pos + __n, __rem);
            __s._C_unlink (__temp->data ());
        }
        else {
            // Current reference has enough room.
            if (__rem)
                traits_type::move (__s._C_data + __pos + __n2,
                                   __s._C_data + __pos + __n, 
                                   __rem);

            for (__d = 0; __d < __n2; __d++)
                traits_type::assign (*(__s._C_data + __pos + __d),
                                     *__first2++);

            __s._C_pref ()->_C_size._C_size = __len;
            traits_type::assign (__s._C_data [__len], value_type ());
        }
    }
    return __s;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
copy (pointer __s, size_type __n, size_type __pos) const
{
    _RWSTD_REQUIRES (__pos <= size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE, 
                      _RWSTD_FUNC ("basic_string::copy (pointer, size_type, "
                                   "size_type)"),
                      __pos, size ()));

    const size_type __rlen = _C_min (size () - __pos, __n);

    traits_type::copy (__s, _C_data + __pos, __rlen);

    return __rlen;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
find (const_pointer __seq, size_type __off, size_type __len) const
{
    const size_type __size = size ();

    if (__size < __off)
        return npos;

    _RWSTD_ASSERT (__seq != 0 || __len == 0);

    const const_pointer __end     = _C_data + __size;
    const const_pointer __seq_end = __seq + __len;

    // `first' is set to point to the first occurrence of the first
    // element of the sought sequence in the controlling sequence
    // if one exists, otherwise to 0
    const_pointer __first = const_pointer ();

    for (const_pointer __next = _C_data + __off; ; __next = __first) {

        // compute the legth of the rest of the controlling sequene
        // and break out when it's shorter than the sought sequence
        const size_type __ext = size_type (__end - __next);

        if (__ext < __len)
            break;

        __first = const_pointer ();

        for (const_pointer __n = __next, __s = __seq; ; ++__n, ++__s) {

            if (__seq_end == __s)
                return size_type (__next - _C_data);

            if (traits_type::eq (*__n, *__s)) {
                if (const_pointer () == __first && __n != __next && traits_type::eq (*__n, *__seq))
                    __first = __n;
            }
            else {
                if (const_pointer () == __first) {
                    // look for the first occurrence of the first element
                    // of the sought sequence in the rest of the cotrolling
                    // sequence
                    __first = traits_type::find (__n, __end - __n, *__seq);

                    if (const_pointer () == __first)
                        return npos;
                }
                break;
            }
        }
    }

    return npos;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
rfind (const_pointer __s, size_type __pos, size_type __n) const
{
    _RWSTD_ASSERT (__s != 0);

    if (size () < __n)
        return npos;
    
    size_type __slen = size () - __n;
    size_type __xpos_start = __slen < __pos ? __slen : __pos; 

    for (size_type __xpos = __xpos_start + 1; __xpos != 0 ; __xpos--) {
        if (!traits_type::compare (_C_data + __xpos - 1, __s, __n))
            return __xpos - 1;
    }

    return npos;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
find_first_of (const_pointer __s, size_type __pos, size_type __n) const
{
    _RWSTD_ASSERT (__s != 0);

    for (size_type __xpos = __pos; __xpos < size () ; __xpos++) {
        for (size_type __i = 0; __i < __n ; __i++)
            if (traits_type::eq (_C_data [__xpos], __s [__i]))
                return __xpos;
    }

    return npos;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
find_last_of (const_pointer __s, size_type __pos, size_type __n) const
{
    _RWSTD_ASSERT (__s != 0);

    if (size ()) {
        size_type __slen = size () - 1;
        size_type __xpos_start = __pos < __slen ? __pos : __slen; 
        for (size_type __xpos = __xpos_start+1; __xpos != 0 ; __xpos--) {
            for (size_type __i = 0; __i < __n ; __i++)
                if (traits_type::eq (_C_data [__xpos - 1], __s [__i]))
                    return __xpos - 1;
        }
    }

    return npos;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
find_first_not_of (const_pointer __s, size_type __pos, size_type __n) const
{
    _RWSTD_ASSERT (__s != 0);

    for (size_type __xpos = __pos; __xpos < size () ; __xpos++) {
        bool __found = false;
        for (size_type __i = 0; __i < __n ; __i++) {
            if (traits_type::eq (_C_data [__xpos], __s [__i])) {
                __found = true;
                break;
            }
        }
        if (!__found)
            return __xpos;
    }

    return npos;
}


template <class _CharT, class _Traits, class _Allocator>
typename basic_string<_CharT, _Traits, _Allocator>::size_type
basic_string<_CharT, _Traits, _Allocator>::
find_last_not_of (const_pointer __s, size_type __pos, size_type __n) const
{
    _RWSTD_ASSERT (__s != 0);

    if (size ()) {
        size_type __slen = size () - 1;
        size_type __xpos_start = __pos < __slen ? __pos : __slen; 
        for (size_type __xpos = __xpos_start + 1; __xpos != 0 ; __xpos--) {
            bool __found = false;
            for (size_type __i = 0; __i < __n ; __i++) {
                if (traits_type::eq (_C_data [__xpos - 1], __s [__i])) {
                    __found = true;
                    break;
                }
            }
            if (!__found)
                return __xpos - 1;
        }
    }

    return npos;
}


template <class _CharT, class _Traits, class _Allocator>
basic_string<_CharT, _Traits, _Allocator>
basic_string<_CharT, _Traits, _Allocator>::
substr (size_type __pos, size_type __n) const
{
    _RWSTD_REQUIRES (__pos <= size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE, 
                      _RWSTD_FUNC ("basic_string::substr (size_type, "
                                   "size_type) const"),
                      __pos, size ()));

    size_type __slen = size () - __pos;
    size_type __rlen = __n < __slen ? __n : __slen;
    return basic_string (_C_data + __pos, __rlen);
}


template <class _CharT, class _Traits, class _Allocator>
int basic_string<_CharT, _Traits, _Allocator>::
compare (size_type __pos1, size_type __n1, 
         const basic_string& __str, size_type __pos2, size_type __n2) const
{
    _RWSTD_REQUIRES (__pos2 <= __str.size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE, 
                      _RWSTD_FUNC ("basic_string::compare (size_type, "
                                   "size_type, const basic_string&, "
                                   "size_type, size_type) const"),
                      __pos2, __str.size ()));

    if (__str.size () - __pos2 < __n2)
        __n2 = __str.size () - __pos2;

    return compare (__pos1, __n1, __str.c_str () + __pos2, __n2);
}


template <class _CharT, class _Traits, class _Allocator>
int basic_string<_CharT, _Traits, _Allocator>::
compare (size_type __pos, size_type __n1,
         const_pointer __s, size_type __n2) const
{
    _RWSTD_REQUIRES (__pos <= size (),
                     (_RWSTD_ERROR_OUT_OF_RANGE, 
                      _RWSTD_FUNC ("basic_string::compare (size_type, "
                                   "size_type, const const_pointer, "
                                   "size_type) const"),
                      __pos, size ()));

    if (size () - __pos < __n1)
        __n1 = size () - __pos;
    size_type __rlen = __n1 < __n2 ? __n1 : __n2; 
    int __res = traits_type::compare (_C_data + __pos, __s, __rlen);

    if (__res == 0)
        __res = (__n1 < __n2) ? -1 : (__n1 != __n2);

    return __res;
}


}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC
