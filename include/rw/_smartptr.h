// -*- C++ -*-
/***************************************************************************
 *
 * _smartptr.h - definition of class template shared_ptr and weak_ptr
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_SMARTPTR_INCLUDED
#define _RWSTD_SMARTPTR_INCLUDED


#include <rw/_autoptr.h>
#include <rw/_exception.h>
#include <rw/_mutex.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {


struct __rw_ptr_base
{
    __rw_ptr_base ()
        : _C_ref (1), _C_weak_ref (0) { }

    virtual ~__rw_ptr_base () {
#ifdef _RWSTDDEBUG
        _C_ref      = 0xdeadbeefL;
        _C_weak_ref = 0xdeadbeefL;
#endif   // _RWSTDDEBUG
    }

    virtual void* _C_get_deleter () const {
        return 0;
    }

    long _C_get_ref () {
        _RWSTD_ATOMIC_PREINCREMENT (_C_ref, false);
        return _RWSTD_ATOMIC_PREDECREMENT (_C_ref, false);
    }

    void _C_inc_ref () {
        _RWSTD_ATOMIC_PREINCREMENT (_C_ref, false);
    }

    void _C_inc_ref_weak () {
        _RWSTD_ATOMIC_PREINCREMENT (_C_weak_ref, false);
    }

    void _C_unref () {
        const long __ref = _RWSTD_ATOMIC_PREDECREMENT (_C_ref, false);
        if (0 == __ref)
            _C_delete ();

        if (__ref <= 0) {
            if (0 > _RWSTD_ATOMIC_PREDECREMENT (_C_weak_ref, false))
                delete this;
            else
                _RWSTD_ATOMIC_PREINCREMENT (_C_weak_ref, false);
        }
    }

    void _C_unref_weak () {
        if (0 >= _RWSTD_ATOMIC_PREDECREMENT (_C_weak_ref, false)) {
            if (0 > _RWSTD_ATOMIC_PREDECREMENT (_C_ref, false))
                delete this;
            else
                _RWSTD_ATOMIC_PREINCREMENT (_C_ref, false);
        }
    }

private:

    virtual void _C_delete () = 0;

    long  _C_ref;
    long  _C_weak_ref;
};


template <class _TypeU>
struct __rw_ptr_body: __rw_ptr_base
{
    __rw_ptr_body (_TypeU *__ptr)
        : __rw_ptr_base (), _C_ptr (__ptr) { }

protected:

    _TypeU* const _C_ptr;

private:

    virtual void _C_delete () {
        delete _C_ptr;
    }
};


template <class _TypeU, class _Deleter>
struct __rw_ptr_deleter: __rw_ptr_body<_TypeU>
{
    __rw_ptr_deleter (_TypeU *__ptr, const _Deleter &__del)
        : __rw_ptr_body<_TypeU>(__ptr), _C_del (__del) { }

private:

    virtual void _C_delete () {
        _C_del (this->_C_ptr);
    }

    virtual void* _C_get_deleter () const {
        return _RWSTD_CONST_CAST (_Deleter*, &_C_del);
    }

    _Deleter _C_del;
};


}   // namespace __rw


_RWSTD_NAMESPACE (std) {


// [util.smartptr.weakptr] - class bad_weak_ptr
struct bad_weak_ptr: _RW::__rw_exception
{
    bad_weak_ptr (): _RW::__rw_exception () { }
};


template <class _TypeT>
class weak_ptr;


// [util.smartptr.shared] - class template shared_ptr
template <class _TypeT>
class shared_ptr
{
    // declare every specialization a friend and grant
    // template members access to _C_ptr
    template <class _TypeU>
    friend class shared_ptr;

    template <class _TypeU>
    friend class weak_ptr;

    template <class _TypeU, class _TypeV>
    friend shared_ptr<_TypeU>
    dynamic_pointer_cast (const shared_ptr<_TypeV>&);

    typedef _RW::__rw_ptr_base _C_base;

    _C_base *_C_pbody;
    _TypeT  *_C_ptr;

    template <class _TypeU>
    explicit
    shared_ptr (const shared_ptr<_TypeU> &__rhs, const _TypeT*)
        : _C_pbody (__rhs._C_pbody) {
        if (_C_pbody)
            _C_pbody->_C_inc_ref ();
    }

public:    

    typedef _TypeT element_type;

    // [util.smartptr.shared.const] constructors

    shared_ptr (): _C_pbody (0), _C_ptr (0) { }

    template <class _TypeU>
    explicit
    shared_ptr (_TypeU *__ptr)
        : _C_pbody (__ptr ? new _RW::__rw_ptr_body<_TypeU>(__ptr) : 0),
          _C_ptr (__ptr) { /* empty */ }

    template <class _TypeU, class _Deleter>
    shared_ptr (_TypeU *__ptr, _Deleter __del)
        : _C_pbody (new _RW::__rw_ptr_deleter<_TypeU, _Deleter>(__ptr, __del)),
          _C_ptr (__ptr) { /* empty */ }

    shared_ptr (const shared_ptr &__rhs)
        : _C_pbody (__rhs._C_pbody), _C_ptr (__rhs._C_ptr) {
        if (_C_pbody)
            _C_pbody->_C_inc_ref ();
    }

    template <class _TypeU>
    shared_ptr (const shared_ptr<_TypeU> &__rhs)
        : _C_pbody (__rhs._C_pbody), _C_ptr (__rhs._C_ptr) {
        if (_C_pbody)
            _C_pbody->_C_inc_ref ();
    }

    template <class _TypeU>
    explicit
    shared_ptr (const weak_ptr<_TypeU> &__rhs)
        : _C_pbody (__rhs._C_pbody), _C_ptr (__rhs._C_ptr) {
        if (_C_pbody) {
            if (0 >= _C_pbody->_C_get_ref ())
                throw bad_weak_ptr ();
            _C_pbody->_C_inc_ref ();
        }
    }

    template <class _TypeU>
    explicit
    shared_ptr (auto_ptr<_TypeU>&);

    // [util.smartptr.shared.dest] destructor
    ~shared_ptr () {
        if (_C_pbody)
            _C_pbody->_C_unref ();

#ifdef _RWSTDDEBUG
        _C_pbody = (_C_base*)0xdeadbeefL;
        _C_ptr   = (element_type*)0xdeadbeefL;
#endif   // _RWSTDDEBUG
    }

    // [util.smartptr.shared.assign] assignment

    shared_ptr& operator= (const shared_ptr &__rhs) {
        if (__rhs._C_pbody)
            __rhs._C_pbody->_C_inc_ref ();
        if (_C_pbody)
            _C_pbody->_C_unref ();
        _C_pbody = __rhs._C_pbody;
        _C_ptr   = __rhs._C_ptr;
        return *this;
    }

    template <class _TypeU>
    shared_ptr& operator= (const shared_ptr<_TypeU> &__rhs) {
        _C_is_convertible ((_TypeU*)0);
        if (__rhs._C_pbody)
            __rhs._C_pbody->_C_inc_ref ();
        if (_C_pbody)
            _C_pbody->_C_unref ();
        _C_pbody = __rhs._C_pbody;
        _C_ptr   = __rhs._C_ptr;
        return *this;
    }

    template <class _TypeU>
    shared_ptr& operator= (auto_ptr<_TypeU> &__rhs) {
        _RW::__rw_ptr_body<_TypeU>* const __tmp =
            new _RW::__rw_ptr_body<_TypeU>(__rhs.get ());
        if (_C_pbody)
            _C_pbody->_C_unref ();
        _C_pbody = __tmp;
        _C_ptr   = __rhs.release ();
        return *this;
    }

    // [util.smartptr.shared.mod] modifiers

    void swap (shared_ptr &__other) {
        _C_base* const __body = _C_pbody;
        element_type* const __ptr = _C_ptr;
        _C_pbody = __other._C_pbody;
        _C_ptr   = __other._C_ptr;
        __other._C_pbody = __body;
        __other._C_ptr   = __ptr;
    }

    void reset () {
        if (_C_pbody) {
            _C_pbody->_C_unref ();
            _C_pbody = 0;
            _C_ptr   = 0;
        }
    }

    template <class _TypeU>
    void reset (_TypeU *__ptr) {
        shared_ptr<_TypeU>(__ptr).swap (*this);
    }

    template <class _TypeU, class _Deleter>
    void reset (_TypeU *__ptr, _Deleter __del) {
        shared_ptr<_TypeU>(__ptr, __del).swap (*this);
    }

    // [util.smartptr.shared.obs] observers

    element_type* get () const {
        return _C_ptr;
    }

    typename _RW::__rw_nonvoid_ref<element_type>::_C_ref
    operator*() const {
        _RWSTD_ASSERT (0 != get ());
        return *get ();
    }

    element_type* operator->() const {
        return &**this;
    }

    long use_count () const {
        return _C_pbody ? _C_pbody->_C_get_ref () : 0;
    }

    bool unique () const {
        return 1 == use_count ();
    }

    operator bool () const {
        return 0 != get ();
    }

    template <class _Deleter>
    _Deleter* _C_get_deleter () const {
        return _RWSTD_STATIC_CAST (_Deleter*,
            _C_pbody ? _C_pbody->_C_get_deleter () : 0);
    }

    template <class _TypeU>
    bool _C_less (const shared_ptr<_TypeU> &__rhs) const {
        return _C_pbody < __rhs._C_pbody;
    }

private:

    // helper to check whether one pointer is convertible to another
    // and to enforce the convertibility requirements
    static void _C_is_convertible (element_type*) { }
};


// [util.smartptr.shared.cmp] - shared_ptr comparisons
template <class _TypeT, class _TypeU>
inline bool
operator== (const shared_ptr<_TypeT> &__lhs, const shared_ptr<_TypeU> &__rhs)
{
    return __lhs.get () == __rhs.get ();
}


template <class _TypeT, class _TypeU>
inline bool
operator!= (const shared_ptr<_TypeT> &__lhs, const shared_ptr<_TypeU> &__rhs)
{
    return __lhs.get () != __rhs.get ();
}


template <class _TypeT, class _TypeU>
inline bool
operator< (const shared_ptr<_TypeT> &__lhs, const shared_ptr<_TypeU> &__rhs)
{
    return __lhs._C_less (__rhs);
}


// [util.smartptr.shared.spec] - shared_ptr specialized algorithms
template <class _TypeT>
inline void
swap (shared_ptr<_TypeT> &__lhs, shared_ptr<_TypeT> &__rhs)
{
    __lhs.swap (__rhs);
}


// [util.smartptr.shared.cast] - shared_ptr casts
template <class _TypeT, class _TypeU>
inline shared_ptr<_TypeT>
static_pointer_cast (const shared_ptr<_TypeU> &__src)
{
    _TypeT* const __ptr = _RWSTD_STATIC_CAST (_TypeT*, __src.get ());
    if (__ptr)
        return shared_ptr<_TypeT>(__src, __ptr);

    return shared_ptr<_TypeT>();
}


template <class _TypeT, class _TypeU>
inline shared_ptr<_TypeT>
dynamic_pointer_cast (const shared_ptr<_TypeU> &__src)
{
    _TypeT* const __ptr = _RWSTD_DYNAMIC_CAST (_TypeT*, __src.get ());
    if (__ptr)
        return shared_ptr<_TypeT>(__src, __ptr);

    return shared_ptr<_TypeT>();
}


template <class _TypeT, class _TypeU>
inline shared_ptr<_TypeT>
const_pointer_cast (const shared_ptr<_TypeU> &__src)
{
    _TypeT* const __ptr = _RWSTD_CONST_CAST (_TypeT*, __src.get ());
    if (__ptr)
        return shared_ptr<_TypeT>(__src, __ptr);

    return shared_ptr<_TypeT>();
}


#if 0   // NOT IMPLEMENTED YET

// [util.smartptr.shared.io] - shared_ptr I/O
template <class _CharT, class _Traits, class _TypeT>
inline basic_ostream<_CharT, _TypeT>&
operator<< (basic_ostream<_CharT, _Traits>&, const shared_ptr<_TypeT>&);

#endif   // 0/1


// [util.smartptr.shared.getdeleter] - shared_ptr get_deleter
template <class _Deleter, class _TypeT>
_Deleter*
get_deleter (const shared_ptr<_TypeT> &__ptr)
{
    return __ptr._C_get_deleter ();
}


// [util.smartptr.weak] - class template weak_ptr
template <class _TypeT>
class weak_ptr
{
    // declare every specialization a friend and grant
    // template members access to _C_pbody
    template <class _TypeU>
    friend class weak_ptr;

    template <class _TypeU>
    friend class shared_ptr;

    typedef _RW::__rw_ptr_base _C_base;

    _C_base *_C_pbody;
    _TypeT  *_C_ptr;

public:

    typedef _TypeT element_type;

    // constructors
    weak_ptr (): _C_pbody (0), _C_ptr (0) { }

    template <class _TypeU>
    weak_ptr (const shared_ptr<_TypeU> &__ptr)
        : _C_pbody (__ptr._C_pbody), _C_ptr (__ptr._C_ptr) {
        if (_C_pbody)
            _C_pbody->_C_inc_ref_weak ();
    }

    weak_ptr (const weak_ptr &__rhs)
        : _C_pbody (__rhs._C_pbody), _C_ptr (__rhs._C_ptr) {
        if (_C_pbody)
            _C_pbody->_C_inc_ref_weak ();
    }

    template <class _TypeU>
    weak_ptr (const weak_ptr<_TypeU> &__rhs)
        : _C_pbody (__rhs._C_pbody), _C_ptr (__rhs._C_ptr) {
        if (_C_pbody)
            _C_pbody->_C_inc_ref_weak ();
    }

    // destructor
    ~weak_ptr () {
        if (_C_pbody)
            _C_pbody->_C_unref_weak ();

#ifdef _RWSTDDEBUG
        _C_pbody = (_C_base*)0xdeadbeefL;
        _C_ptr   = (element_type*)0xdeadbeefL;
#endif   // _RWSTDDEBUG
    }

    // assignment
    weak_ptr& operator= (const weak_ptr &__rhs) {
        if (_C_pbody)
            _C_pbody->_C_unref_weak ();
        if ((_C_pbody = __rhs._C_pbody))
            _C_pbody->_C_inc_ref_weak ();
        _C_ptr = __rhs._C_ptr;
        return *this;
    }

    template <class _TypeU>
    weak_ptr& operator= (const weak_ptr<_TypeU> &__rhs) {
        _C_is_convertible ((_TypeU*)0);
        if (_C_pbody)
            _C_pbody->_C_unref_weak ();
        if ((_C_pbody = __rhs._C_pbody))
            _C_pbody->_C_inc_ref_weak ();
        _C_ptr = __rhs._C_ptr;
        return *this;
    }

    template<class _TypeU>
    weak_ptr& operator=(const shared_ptr<_TypeU> &__rhs) {
        _C_is_convertible ((_TypeU*)0);
        if (_C_pbody)
            _C_pbody->_C_unref_weak ();
        if ((_C_pbody = __rhs._C_pbody))
            _C_pbody->_C_inc_ref_weak ();
        _C_ptr = __rhs._C_ptr;
        return *this;
    }

    // modifiers

    void swap (weak_ptr &__other) {
        _C_base* const __body = _C_pbody;
        element_type* const __ptr = _C_ptr;
        _C_pbody = __other._C_pbody;
        _C_ptr   = __other._C_ptr;
        __other._C_pbody = __body;
        __other._C_ptr   = __ptr;
    }

    void reset () {
        if (_C_pbody) {
            _C_pbody->_C_unref_weak ();
            _C_pbody = 0;
            _C_ptr   = 0;
        }
    }

    // observers
    long use_count () const {
        return _C_pbody ? _C_pbody->_C_get_ref () : 0;
    }

    bool expired () const {
        return 0 == use_count ();
    }

    shared_ptr<element_type>
    lock () const {
        return expired () ? shared_ptr<element_type>()
                          : shared_ptr<element_type>(*this);
    }

    // implements 20.6.12.3.6 - non-member template operator<
    template <class _TypeU>
    bool operator< (const weak_ptr<_TypeU> __rhs) const {
        return _C_pbody < __rhs._C_pbody;
    }

private:

    // helper to check whether one pointer is convertible to another
    // and to enforce the convertibility requirements
    static void _C_is_convertible (element_type*) { }
};


// [util.smartptr.weak.cmp] - weak_ptr comparison
template <class _TypeT, class _TypeU>
inline bool
operator< (const weak_ptr<_TypeT> &__lhs, const weak_ptr<_TypeU> &__rhs)
{
    return __lhs.operator< (__rhs);
}


// [util.smartptr.weak.spec] - weak_ptr specialized algorithms
template <class _TypeT>
inline void
swap (weak_ptr<_TypeT> &__lhs, weak_ptr<_TypeT> &__rhs)
{
    __lhs.swap (__rhs);
}


// [util.smartptr.enab] - class enable_shared_from_this
template <class _TypeT>
class enable_shared_from_this;


}   // namespace std


#endif   // _RWSTD_SMARTPTR_INCLUDED
