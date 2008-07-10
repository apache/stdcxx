/***************************************************************************
 *
 * _tree.h - Declarations for the Standard Library tree classes
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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
 ***************************************************************************
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 **************************************************************************/

/***************************************************************************
 *
 * Red-black tree class, designed for use in implementing associative
 * containers (set, multiset, map, and multimap). The insertion and
 * deletion algorithms are based on those in Cormen, Leiserson, and
 * Rivest, Introduction to Algorithms (MIT Press, 1990), except that:
 * 
 * (1) the header cell is maintained with links not only to the root
 * but also to the leftmost node of the tree, to enable constant time
 * begin(), and to the rightmost node of the tree, to enable linear time
 * performance when used with the generic set algorithms (set_union,
 * etc.);
 * 
 * (2) when a node being deleted has two children its successor node
 * is relinked into its place, rather than copied, so that the only
 * iterators invalidated are those referring to the deleted node.
 *
 **************************************************************************/

#ifndef _RWSTD_RW_TREE_H_INCLUDED
#define _RWSTD_RW_TREE_H_INCLUDED

#ifndef _RWSTD_RW_ALGOBASE_H_INCLUDED
#  include <rw/_algobase.h>
#endif   // _RWSTD_RW_ALGOBASE_H_INCLUDED

#ifndef _RWSTD_RW_ITERATOR_H_INCLUDED
#  include <rw/_iterator.h>
#endif   // _RWSTD_RW_ITERATOR_H_INCLUDED


_RWSTD_NAMESPACE (__rw) { 


template <class _Alloc, class _Val, class _Key, class _KeyOf>
struct __rw_rb_tree_node
{
    enum _C_color_t { _C_red, _C_black };

    typedef _Val&                                          reference;
    typedef _Alloc                                         allocator_type;

    typedef _RWSTD_REBIND (allocator_type, __rw_rb_tree_node) _C_node_alloc_t;
    typedef _RWSTD_REBIND (allocator_type, _Key)              _C_key_alloc_t;

    typedef typename _C_node_alloc_t::pointer             _C_link_t;
    typedef typename _C_key_alloc_t::const_reference      _C_const_key_ref;

    _C_color_t _C_color; 
    _C_link_t  _C_parent;
    _C_link_t  _C_child [2];   // left (0) and right (1) children
    _Val       _C_value;

    static _C_link_t _C_minmax (_C_link_t __lnk, bool __do_max) {
        _RWSTD_ASSERT (_C_link_t () != __lnk);
        while (_C_link_t () != __lnk->_C_child [__do_max])
            __lnk = __lnk->_C_child [__do_max];
        return __lnk;
    }

    static _C_link_t _C_min (_C_link_t __lnk) {
        return _C_minmax (__lnk, false);
    }
    
    static _C_link_t _C_max (_C_link_t __lnk) {
        return _C_minmax (__lnk, true);
    }

    _C_const_key_ref _C_key () const {
        return _KeyOf ()(_C_value);
    }
};


// iterator implements inorder traversal; i.e., nodes are visited
// recursively in this order: left subtree, root, right subtree
template <class _TypeT, class _DiffT,
          class _Pointer, class _Reference, class _Node>
class __rw_tree_iter
    : public _STD::iterator <_STD::bidirectional_iterator_tag,
                            _TypeT, _DiffT, _Pointer, _Reference>
{
    typedef _STD::iterator <_STD::bidirectional_iterator_tag,
                           _TypeT, _DiffT, _Pointer, _Reference> _C_iter_base;
public:

    typedef typename _C_iter_base::value_type        value_type;
    typedef typename _C_iter_base::difference_type   difference_type;
    typedef typename _C_iter_base::pointer           pointer;
    typedef typename _C_iter_base::reference         reference;
    typedef typename _C_iter_base::iterator_category iterator_category;
    typedef _Node                                     _C_node_t;
    typedef typename _C_node_t::allocator_type       allocator_type;
    typedef typename _C_node_t::_C_link_t            _C_link_t;

    typedef const value_type*                         const_pointer; 
    typedef const value_type&                         const_reference; 

    typedef __rw_tree_iter<_TypeT, _DiffT, value_type*, value_type&, _C_node_t>
    _C_iterator;

    _C_link_t _C_node;

    __rw_tree_iter () { /* empty */ }

    // no copy ctor other than the one below is defined
    // will use a compiler generated one if __rw_tree_iter != _C_iterator
    __rw_tree_iter (const _C_iterator &__rhs)
        : _C_node (__rhs._C_node) { }

    template <class _Ptr, class _Ref>
    __rw_tree_iter (const __rw_tree_iter<_TypeT, _DiffT, _Ptr, _Ref, _Node>&
                    __rhs)
        : _C_node (__rhs._C_node) { }

    __rw_tree_iter (_C_link_t __lnk)
        : _C_node (__lnk) {}        

#ifdef SNI
    difference_type operator- (const __rw_tree_iter&) const {
        return 0;
    }
#endif
    
    __rw_tree_iter& operator++ () {
        if (_C_link_t () != _C_node->_C_child [1]) {
            _C_node = _C_node_t::_C_min (_C_node->_C_child [1]);
        }
        else {
            _C_link_t __tmp = _C_node->_C_parent;

            while (_C_node == __tmp->_C_child [1]) {
                _C_node = __tmp;
                __tmp     = __tmp->_C_parent;
            }

            if (_C_node->_C_child [1] != __tmp)
                _C_node = __tmp;
        }
        return *this;
    }
    
    __rw_tree_iter& operator-- () {
        if (   _C_node->_C_color == _C_node_t::_C_red
            && _C_node->_C_parent->_C_parent == _C_node)  
            //
            // Check for header.
            //
            _C_node = _C_node->_C_child [1];   // Return rightmost.
        else if (_C_link_t () != _C_node->_C_child [0]) {
            _C_node = _C_node_t::_C_max (_C_node->_C_child [0]);
        }
        else {
            _C_link_t __tmp = _C_node->_C_parent;

            while (_C_node == __tmp->_C_child [0]) {
                _C_node = __tmp;
                __tmp     = __tmp->_C_parent;
            }
            _C_node = __tmp;
        }
        return *this;
    }

    __rw_tree_iter operator++ (int) {
        __rw_tree_iter __tmp (*this);
        return ++*this, __tmp;
    }
    
    __rw_tree_iter operator-- (int) {
        __rw_tree_iter __tmp (*this);
        return --*this, __tmp;
    }

    reference operator* () const {
        return _C_node->_C_value;
    }

    _RWSTD_OPERATOR_ARROW (pointer operator-> () const);
};


#define _RWSTD_TREE_ITER(n) \
        __rw_tree_iter <_TypeT, _DiffT, _Ptr##n, _Ref##n, _Node>

template <class _TypeT, class _DiffT,
          class _Ptr1, class _Ref1, class _Ptr2, class _Ref2, class _Node>
inline bool
operator== (const _RWSTD_TREE_ITER (1) &__lhs,
            const _RWSTD_TREE_ITER (2) &__rhs)
{
    return __lhs._C_node == __rhs._C_node;
}

template <class _TypeT, class _DiffT,
          class _Ptr1, class _Ref1, class _Ptr2, class _Ref2, class _Node>
inline bool
operator!= (const _RWSTD_TREE_ITER (1) &__lhs,
            const _RWSTD_TREE_ITER (2) &__rhs)
{
    return !(__lhs == __rhs);
}


#undef _RWSTD_TREE_ITER

// for convenience
#undef _ITER_NODE
#define _ITER_NODE(it)   (_ITER_BASE (it)._C_node)


_EXPORT
template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
class __rb_tree : private _Alloc
{
private:
    
    typedef __rw_rb_tree_node<_Alloc,_Val,_Key,_KeyOf> _C_node_t; 
    typedef _RWSTD_ALLOC_TYPE (_Alloc,_Val)            _C_val_alloc_t;
    typedef _RWSTD_REBIND (_Alloc, _Key)               _C_key_alloc_t;
    typedef _RWSTD_REBIND (_Alloc, _C_node_t)          _C_node_alloc_t;
    typedef typename _C_node_alloc_t::pointer         _C_link_t;

public:
    
    typedef _Key                                       key_type;
    typedef _Val                                       value_type;
    typedef _Comp                                      key_compare;
    typedef _Alloc                                     allocator_type;
      
    typedef typename _C_val_alloc_t::pointer          pointer;
    typedef typename _C_val_alloc_t::const_pointer    const_pointer;
    typedef typename allocator_type::size_type        size_type;
    typedef typename allocator_type::difference_type  difference_type;
    typedef typename _C_val_alloc_t::reference        reference;
    typedef typename _C_val_alloc_t::const_reference  const_reference;
    
private:

    typedef __rw_tree_iter<value_type, difference_type, pointer,
                              reference, _C_node_t>        _C_tree_iter;

    typedef __rw_tree_iter<value_type, difference_type, const_pointer,
                              const_reference, _C_node_t>  _C_tree_citer;

public:

#ifndef _RWSTD_NO_DEBUG_ITER

    typedef __rw_debug_iter <__rb_tree, _C_tree_iter, _C_tree_iter>
    iterator;

    typedef __rw_debug_iter <__rb_tree, _C_tree_citer, _C_tree_iter>
    const_iterator;

    iterator _C_make_iter (_C_link_t __node) {
        return iterator (*this, _C_tree_iter (__node));
    }

    const_iterator _C_make_iter (_C_link_t __node) const {
        return const_iterator (*this, _C_tree_citer (__node));
    }

#else   // if defined (_RWSTD_NO_DEBUG_ITER)

    typedef _C_tree_iter         iterator;
    typedef _C_tree_citer        const_iterator;

    iterator _C_make_iter (_C_link_t __node) {
        return iterator (__node);
    }

    const_iterator _C_make_iter (_C_link_t __node) const {
        return const_iterator (__node);
    }

#endif   // _RWSTD_NO_DEBUG_ITER

private:

#ifdef _RWSTD_NO_NESTED_CLASS_ACCESS

    // allow _C_node_buf access to __rb_tree's private type(s)
    // if the resolution of cwg issue 45 is not yet implemented
    struct _C_node_buf;
    friend struct _C_node_buf;
    
#endif   // _RWSTD_NO_NESTED_CLASS_ACCESS

    struct _C_node_buf {
        typedef _RWSTD_REBIND (allocator_type, _C_node_buf) _C_buf_alloc_t;
        typedef typename _C_buf_alloc_t::pointer           _C_buf_ptr_t;
        
        _C_buf_ptr_t _C_next_buffer;
        size_type    size;
        _C_link_t    _C_buffer;
    };

    typedef typename _C_node_buf::_C_buf_alloc_t _C_buf_alloc_t;
    typedef typename _C_node_buf::_C_buf_ptr_t   _C_buf_ptr_t;

    _C_buf_ptr_t _C_buf_list;
    _C_link_t    _C_free_list;
    _C_link_t    _C_next_avail;
    _C_link_t    _C_last;
    
    void _C_add_new_buffer ();

    void _C_deallocate_buffers ();
    
    // 
    // Return a node from the free list or new storage
    //
    _C_link_t _C_get_link () {
        _C_link_t __tmp = _C_free_list;
        _C_link_t __tmp2 = (void*)_C_free_list ? 
            (_C_free_list = _RWSTD_STATIC_CAST (_C_link_t,(_C_free_list->_C_child [1])), __tmp) 
            : (_C_next_avail == _C_last ? (_C_add_new_buffer (), _C_next_avail++) 
               : _C_next_avail++);
        __tmp2->_C_parent    = 0;
        __tmp2->_C_child [0] = 0;
        __tmp2->_C_child [1] = 0;
        __tmp2->_C_color     = _C_node_t::_C_red;
        return __tmp2;
    }

    //
    // Return a node from the free list or new storage with
    // the _Val __v constructed on it.  Every call to _C_get_node
    // must eventually be followed by a call to _C_put_node.
    //
    _C_link_t _C_get_node (const_reference __v) {
        _C_link_t __tmp2 = _C_get_link ();

        _TRY {
            _RWSTD_VALUE_ALLOC (_C_val_alloc_t, *this,
                 construct (_RWSTD_VALUE_ALLOC (_C_val_alloc_t, *this,
                                                address (__tmp2->_C_value)),
                            __v));
        }
        _CATCH (...) {
            _C_put_node (__tmp2, false);
            _RETHROW;
        }      
        return __tmp2;
    }

    _C_link_t _C_get_node () {
        return _C_get_link ();
    }
    
    // 
    // Return a node to the free list and destroy the value in it.
    //
    void _C_put_node (_C_link_t __p, bool __destroy = true) { 
        __p->_C_child [1] = _C_free_list; 
        if (__destroy)  {
            _RWSTD_VALUE_ALLOC (_C_val_alloc_t, *this,
                destroy (_RWSTD_VALUE_ALLOC (_C_val_alloc_t, *this,
                    address (__p->_C_value))));
        }
        _C_free_list = __p; 
    }
    
private:

    // _C_end is end()
    // tree root is _C_end->_C_parent
    // the leftmost node, i.e., begin(), is _C_end->_C_child [0]
    // the rightmost node, i.e., end() - 1, is _C_end->_C_child [1]
    // all three pointers are null (0) when the tree is empty
    // both child pointers of each leaf node are null (0)
    // the parent pointer of the root node points to *_C_end
    _C_link_t _C_end;

    size_type   _C_size;  // number of nodes
    key_compare _C_cmp;   // comparison object

public:
    

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

    typedef _STD::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef _STD::reverse_iterator<iterator>       reverse_iterator;

#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

  typedef std::reverse_iterator<const_iterator, 
        _STD::bidirectional_iterator_tag, value_type, 
        const_reference, const_pointer, difference_type>
    const_reverse_iterator;

    typedef std::reverse_iterator<iterator, 
        _STD::bidirectional_iterator_tag, value_type, 
        reference, pointer, difference_type>
    reverse_iterator;

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC

private:

    iterator _C_insert (_C_link_t, _C_link_t, const value_type&);

    _C_link_t _C_copy (_C_link_t, _C_link_t);

    void _C_erase (_C_link_t);

    void _C_erase_leaf (_C_link_t);

    void _C_init () {
      _C_buf_list          = 0;
      _C_free_list         =
      _C_next_avail        =
       _C_last             = 0;
      _C_end               = _C_get_node ();
      _C_end->_C_parent    = 0;
      _C_end->_C_child [0] =
      _C_end->_C_child [1] = _C_end;
    }

public:

    __rb_tree (const key_compare& = key_compare (),
               const allocator_type& = allocator_type ());

    template<class _InputIter>
    __rb_tree (_InputIter __first, _InputIter __last,
               const key_compare &__cmp,
               const allocator_type &__alloc, bool __dup)
        : allocator_type (__alloc), _C_buf_list (0),
          _C_end (0), _C_size (0), 
          _C_cmp (__cmp) { 

        _C_init ();

        _TRY {
            insert (__first, __last, __dup);
        }
        _CATCH (...) {
            _C_deallocate_buffers ();
            _RETHROW;
        }
    }

    __rb_tree (const __rb_tree&);

    ~__rb_tree ();

    __rb_tree& operator= (const __rb_tree&);

    key_compare key_comp () const {
        return _C_cmp;
    }

    _C_val_alloc_t get_allocator () const {
        return _C_val_alloc_t (*this);
    }

    iterator begin () {
        return _C_make_iter (_C_end->_C_child [0]);
    }

    const_iterator begin () const {
        return _C_make_iter (_C_end->_C_child [0]);
    }

    iterator end () {
        return _C_make_iter (_C_end);
    }

    const_iterator end () const {
        return _C_make_iter (_C_end);
    }

    reverse_iterator rbegin () { 
        return reverse_iterator (end ());
    }
    
    reverse_iterator rend () { 
        return reverse_iterator (begin ());
    } 

    const_reverse_iterator rbegin () const { 
        return const_reverse_iterator (end ());
    }
    
    const_reverse_iterator rend () const { 
        return const_reverse_iterator (begin ());
    } 

    bool empty () const {
        return 0 == _C_size;
    }

    size_type size () const {
        return _C_size;
    }

    size_type max_size () const { 
      return _C_node_alloc_t (*this).max_size (); 
    }
    
    void swap (__rb_tree &__rhs) {
      if (get_allocator () == __rhs.get_allocator ()) {
          _STD::swap (_C_buf_list, __rhs._C_buf_list);
          _STD::swap (_C_free_list, __rhs._C_free_list);
          _STD::swap (_C_next_avail, __rhs._C_next_avail);
          _STD::swap (_C_last, __rhs._C_last);
          _STD::swap (_C_end, __rhs._C_end);
          _STD::swap (_C_size, __rhs._C_size);
          _STD::swap (_C_cmp, __rhs._C_cmp);
      }
      else {
          __rb_tree __tmp = *this;
          *this = __rhs;
          __rhs = __tmp;
      } 
    }

    void _C_insert (const value_type&, _STD::pair<iterator, bool>&, bool);

    _STD::pair<iterator, bool>
    insert (const value_type &__val, bool __dup) {
        _STD::pair<iterator, bool> __ret;
        return _C_insert (__val, __ret, __dup), __ret;
    }

    iterator insert (iterator, const value_type&, bool);

    template<class _Iterator>
    void insert (_Iterator __first, _Iterator __last, bool __dup) {
        for (; __first != __last; ++__first)
            insert (*__first, __dup);
    }

    iterator erase (iterator);

    size_type erase (const key_type&);

    iterator erase (iterator, iterator);

// MSVC 6.0 thinks S<const T*> is the same as S<T*>...
#if !defined (_MSC_VER) || _MSC_VER > 1300

    // map and set's iterator may be defined to be tree::const_iterator
    iterator insert (const_iterator __it, const value_type &__x, bool __dup) {
        return insert (_C_make_iter (_ITER_NODE (__it)), __x, __dup);
    }

    // map and set's iterator may be defined to be tree::const_iterator
    iterator erase (const_iterator __it) {
        return erase (_C_make_iter (_ITER_NODE (__it)));
    }

    // map and set's iterator may be defined to be tree::const_iterator
    iterator erase (const_iterator __first, const_iterator __last) {
        return erase (_C_make_iter (_ITER_NODE (__first)),
                      _C_make_iter (_ITER_NODE (__last)));
    }

#endif   // _MSC_VER <= 1300

    void erase (const key_type*, const key_type*);

    void clear () {
        erase (begin (), end ());
    }

    iterator find (const key_type&);

    const_iterator find (const key_type& __key) const {
        return _RWSTD_CONST_CAST (__rb_tree*, this)->find (__key);
    }

    size_type count (const key_type&) const;

    iterator lower_bound (const key_type&);

    const_iterator lower_bound (const key_type& __key) const {
        return _RWSTD_CONST_CAST (__rb_tree*, this)->lower_bound (__key);
    }

    iterator upper_bound (const key_type&);

    const_iterator upper_bound (const key_type& __key) const {
        return _RWSTD_CONST_CAST (__rb_tree*, this)->upper_bound (__key);
    }

    _STD::pair<iterator, iterator> equal_range (const key_type&);

    _STD::pair<const_iterator, const_iterator>
    equal_range (const key_type& __key) const {
        _STD::pair<iterator, iterator> __tmp =
            _RWSTD_CONST_CAST (__rb_tree*, this)->equal_range (__key);
        return _STD::pair<const_iterator, const_iterator>
            (__tmp.first, __tmp.second);
    }

#ifndef _RWSTD_NO_OPTIMIZE_SPEED

    void _C_rol (_C_link_t);
    void _C_ror (_C_link_t);

#else   // if defined (_RWSTD_NO_OPTIMIZE_SPEED)

    void _C_rotate  (_C_link_t, bool);

#endif   // _RWSTD_NO_OPTIMIZE_SPEED

    size_type _C_level (const_iterator) const;

    // depth guaranteed to be <= 2 * log2(size() + 1)
    size_type _C_depth (const_iterator, size_type* = 0) const;

    size_type _C_depth () const {
        return _C_depth (_C_make_iter (_C_end->_C_parent));
    }
};


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline bool
operator== (const __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>& __lhs,
            const __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>& __rhs)
{
    return    __lhs.size () == __rhs.size ()
           && _STD::equal (__lhs.begin (), __lhs.end (), __rhs.begin ());
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline bool
operator< (const __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>& __lhs,
           const __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>& __rhs)
{
    return _STD::lexicographical_compare (__lhs.begin (), __lhs.end (),
                                          __rhs.begin (), __rhs.end ());
}


template <class _Key,class _Val,class _KeyOf,class _Comp,class _Alloc>
inline void   
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_erase_leaf (_C_link_t __lnk)
{
    // remove a leaf node from the tree
    const _C_link_t __parent = __lnk->_C_parent;

    if (__parent == _C_end) {
        _C_end->_C_parent    = 0;
        _C_end->_C_child [0] =
        _C_end->_C_child [1] = __parent;
    }

#ifndef _RWSTD_NO_OPTIMIZE_SPEED

    else if (__parent->_C_child [0] == __lnk) {
        __parent->_C_child [0] = 0;
        if (_C_end->_C_child [0] == __lnk)
            _C_end->_C_child [0] = __parent;
    }
    else {
        __parent->_C_child [1] = 0;
        if (_C_end->_C_child [1] == __lnk)
            _C_end->_C_child [1] = __parent;
    }

#else   // if !defined (_RWSTD_NO_OPTIMIZE_SPEED)

    else {
        const bool __right = __parent->_C_child [0] != __lnk;
        __parent->_C_child [__right] = 0;
        if (_C_end->_C_child [__right] == __lnk)
            _C_end->_C_child [__right] = __parent;
    }

#endif   // _RWSTD_NO_OPTIMIZE_SPEED
}


#ifndef _RWSTD_NO_OPTIMIZE_SPEED

template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline void 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_rol (_C_link_t __lnk)
{
    _RWSTD_ASSERT (_C_link_t () != __lnk);

    _C_link_t __tmp = __lnk->_C_child [1];
    __lnk->_C_child [1] = __tmp->_C_child [0];

    if (_C_link_t () != __tmp->_C_child [0])
        __tmp->_C_child [0]->_C_parent = __lnk;

    __tmp->_C_parent = __lnk->_C_parent;

    if (__lnk == _C_end->_C_parent)
        _C_end->_C_parent = __tmp;
    else if (__lnk == __lnk->_C_parent->_C_child [0])
        __lnk->_C_parent->_C_child [0] = __tmp;
    else
        __lnk->_C_parent->_C_child [1] = __tmp;

    __tmp->_C_child [0] = __lnk;
    __lnk->_C_parent    = __tmp;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline void 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_ror (_C_link_t __lnk)
{
    _RWSTD_ASSERT (_C_link_t () != __lnk);
      
    _C_link_t __tmp = __lnk->_C_child [0];
    __lnk->_C_child [0] = __tmp->_C_child [1];

    if (_C_link_t () != __tmp->_C_child [1])
        __tmp->_C_child [1]->_C_parent = __lnk;

    __tmp->_C_parent = __lnk->_C_parent;
    
    if (__lnk == _C_end->_C_parent)
        _C_end->_C_parent = __tmp;
    else if (__lnk == __lnk->_C_parent->_C_child [1])
        __lnk->_C_parent->_C_child [1] = __tmp;
    else
        __lnk->_C_parent->_C_child [0] = __tmp;

    __tmp->_C_child [1] = __lnk;
    __lnk->_C_parent    = __tmp;
}

#else   // if defined (_RWSTD_NO_OPTIMIZE_SPEED)

template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline void 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_rotate (_C_link_t __lnk, bool __right)
{
    _RWSTD_ASSERT (_C_link_t () != __lnk);

    _C_link_t __tmp = __lnk->_C_child [!__right];
    __lnk->_C_child [!__right] = __tmp->_C_child [__right];

    if (_C_link_t () != __tmp->_C_child [__right])
        __tmp->_C_child [__right]->_C_parent = __lnk;

    __tmp->_C_parent = __lnk->_C_parent;

    if (__lnk == _C_end->_C_parent)
        _C_end->_C_parent = __tmp;
    else {
        const bool __rt = __lnk == __lnk->_C_parent->_C_child [1];
        __lnk->_C_parent->_C_child [__rt] = __tmp;
    }

    __tmp->_C_child [__right] = __lnk;
    __lnk->_C_parent          = __tmp;
}

#endif   // _RWSTD_NO_OPTIMIZE_SPEED


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline void __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
erase (const _Key* __first, const _Key* __last)
{
    for (; __first != __last; ++__first)
        erase (*__first);
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::size_type 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
count (const _Key& __k) const
{
    _STD::pair<const_iterator, const_iterator> __p = equal_range (__k);
    size_type __n = _DISTANCE (__p.first, __p.second, size_type);
    return __n;
}


#define _RWSTD_RB_TREE_ITER \
        typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator
                                               
template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
inline _STD::pair<_RWSTD_RB_TREE_ITER , _RWSTD_RB_TREE_ITER >
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
equal_range (const _Key& __k)
{
    return _STD::pair<iterator, iterator>(lower_bound (__k), upper_bound(__k));
}


#undef _RWSTD_RB_TREE_ITER


}   // namespace __rw


#ifdef _RWSTD_NO_IMPLICIT_INCLUSION
#  include <rw/_tree.cc>
#endif

#endif   // _RWSTD_RW_TREE_H_INCLUDED
