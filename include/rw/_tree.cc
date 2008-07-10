/***************************************************************************
 *
 * _tree.cc - Non-inline tree definitions for the Standard Library
 *
 * $Id$
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


_RWSTD_NAMESPACE (__rw) { 


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
__rb_tree (const key_compare    &__cmp /* = key_compare () */,
           const allocator_type &__alloc /* = allocator_type () */) 
    : allocator_type (__alloc),
    _C_buf_list (0),
    _C_end (0),
    _C_size (0),
    _C_cmp (__cmp)
{
    _C_init ();
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>& 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
operator= (const __rb_tree &__x)
{
    if (this != &__x) {
        erase (begin (), end ());

        // set the root of the tree
        _C_end->_C_parent = _C_copy (__x._C_end->_C_parent, _C_end);

        if (_C_link_t () == _C_end->_C_parent) {
            // set the child pointers of an empty tree to point to the header
            _C_end->_C_child [0] =
            _C_end->_C_child [1] = _C_end;
        }
        else {
            // set the leftmost and the rightmost nodes
            _C_end->_C_child [0] = _C_node_t::_C_min (_C_end->_C_parent);
            _C_end->_C_child [1] = _C_node_t::_C_max (_C_end->_C_parent);
        }
        _C_size = __x._C_size;
        _C_cmp  = __x._C_cmp;
    }
    return *this;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
__rb_tree (const __rb_tree &__lnk)
    : allocator_type (__lnk.get_allocator ()), _C_buf_list (0), _C_end (0),
      _C_size (__lnk._C_size),
      _C_cmp (__lnk._C_cmp)
{
    _C_free_list     = _C_next_avail = _C_last = 0;
    _C_end           = _C_get_node ();
    _C_end->_C_color = _C_node_t::_C_red;
    _TRY { 
        _C_end->_C_parent = _C_copy (__lnk._C_end->_C_parent, _C_end);
    }
    _CATCH (...) {
        _C_deallocate_buffers ();
        _RETHROW;
    }
    if (_C_link_t () == _C_end->_C_parent) {
        _C_end->_C_child [0] =
            _C_end->_C_child [1] = _C_end;
    }
    else {
        _C_end->_C_child [0] = _C_node_t::_C_min (_C_end->_C_parent);
        _C_end->_C_child [1] = _C_node_t::_C_max (_C_end->_C_parent);
    }
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
~__rb_tree ()
{
    if (_C_link_t () != _C_end) {
        erase (begin (), end ());
        _C_put_node (_C_end, false);
        _C_deallocate_buffers ();
    }
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
void __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_add_new_buffer ()
{
    const size_type __bufsize =
        _C_buf_ptr_t () == _C_buf_list ? 0 : _C_buf_list->size;

    size_type __newsize = _RWSTD_NEW_CAPACITY (__rb_tree, this, __bufsize);
    if (__newsize <= __bufsize)
        __newsize = __bufsize + 1;

    _C_buf_ptr_t __buf = 
        _C_buf_alloc_t (*this).allocate (size_type (1), (void*)_C_buf_list);

    _TRY {
        __buf->_C_buffer = 
            _C_node_alloc_t (*this).allocate (__newsize, (void*)_C_last);
    }
    _CATCH (...) {
        _C_buf_alloc_t (*this).deallocate (__buf, 1);
        _RETHROW;
    } 
    __buf->_C_next_buffer = _C_buf_list;
    __buf->size           = __newsize;
    _C_buf_list           = __buf;
    _C_next_avail         = _C_buf_list->_C_buffer;
    _C_last               = _C_next_avail + __newsize;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
void __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_deallocate_buffers ()
{
    while ((void*)_C_buf_list) {
        _C_buf_ptr_t __tmp = _C_buf_list;
        _C_buf_list        = (_C_buf_ptr_t)(_C_buf_list->_C_next_buffer);
        _C_node_alloc_t(*this).deallocate(__tmp->_C_buffer,__tmp->size);
        _C_buf_alloc_t(*this).deallocate(__tmp,1);
    }
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_insert (_C_link_t __x, _C_link_t __y, const value_type &__v)
{
    _C_link_t __z = _C_get_node (__v);
    ++_C_size;

    // for notational convenience
    const typename _C_node_t::_C_color_t _Red   = _C_node_t::_C_red;
    const typename _C_node_t::_C_color_t _Black = _C_node_t::_C_black;
    const _C_link_t                       _Null  = _C_link_t ();

    if (   __y == _C_end || _Null != __x
        || _C_cmp (_KeyOf()(__v), __y->_C_key ())) {

        __y->_C_child [0] = __z;
        if (__y == _C_end) {

            // set the root node and the rightmost node
            _C_end->_C_parent    = __z;
            _C_end->_C_child [1] = __z;
        }
        else if (__y == _C_end->_C_child [0]) {
            // set the leftmost node
            _C_end->_C_child [0] = __z;
        }
    }
    else {
        __y->_C_child [1] = __z;
        if (__y == _C_end->_C_child [1]) {
            // set the rightmost node
            _C_end->_C_child [1] = __z;
        }
    }

    __z->_C_parent = __y;
    __x            = __z;   // recolor and rebalance the tree

    while (   __x != _C_end->_C_parent
           && __x->_C_parent->_C_color == _Red) {

#ifndef  _RWSTD_NO_OPTIMIZE_SPEED

        if (__x->_C_parent == __x->_C_parent->_C_parent->_C_child [0]) {

            __y = __x->_C_parent->_C_parent->_C_child [1];

            if (_Null != __y && __y->_C_color == _Red) {
                __x->_C_parent->_C_color            = _Black;
                __y->_C_color                       = _Black;
                __x->_C_parent->_C_parent->_C_color = _Red;
                __x                                 = __x->_C_parent->_C_parent;
            }
            else {
                if (__x == __x->_C_parent->_C_child [1]) {
                    __x = __x->_C_parent; 
                    _C_rol (__x);
                }
                __x->_C_parent->_C_color            = _Black;
                __x->_C_parent->_C_parent->_C_color = _Red;
                _C_ror (__x->_C_parent->_C_parent);
            }
        }
        else {
            __y = __x->_C_parent->_C_parent->_C_child [0];

            if (_Null != __y && __y->_C_color == _Red) {
                __x->_C_parent->_C_color            = _Black;
                __y->_C_color                       = _Black;
                __x->_C_parent->_C_parent->_C_color = _Red;
                __x                                 = __x->_C_parent->_C_parent;
            }
            else {
                if (__x == __x->_C_parent->_C_child [0]) {
                    __x = __x->_C_parent; 
                    _C_ror (__x);
                }
                __x->_C_parent->_C_color            = _Black;
                __x->_C_parent->_C_parent->_C_color = _Red;
                _C_rol (__x->_C_parent->_C_parent);
            }
        }

#else   // if defined (_RWSTD_NO_OPTIMIZE_SPEED)

        const bool __left =
            __x->_C_parent == __x->_C_parent->_C_parent->_C_child [0];
        const bool __right = !__left;

        __y = __x->_C_parent->_C_parent->_C_child [__left];

        if (_Null != __y && _Red == __y->_C_color) {
            __y->_C_color                       =
            __x->_C_parent->_C_color            = _Black;
            __x->_C_parent->_C_parent->_C_color = _Red;
            __x                                 = __x->_C_parent->_C_parent;
        }
        else {
            if (__x == __x->_C_parent->_C_child [__left]) {
                __x = __x->_C_parent; 
                _C_rotate (__x, __right);
            }
            __x->_C_parent->_C_color            = _Black;
            __x->_C_parent->_C_parent->_C_color = _Red;
            _C_rotate (__x->_C_parent->_C_parent, __left);
        }

#endif   // _RWSTD_NO_OPTIMIZE_SPEED

    }

    // set the color of the root node
    _C_end->_C_parent->_C_color = _Black;

    return _C_make_iter (__z);
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
void __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_insert (const value_type &__v, _STD::pair<iterator, bool> &__ret,
           bool __dup)
{
    _C_link_t __y = _C_end;              // end
    _C_link_t __x = _C_end->_C_parent;   // root node

    bool __right  = true;

    while (_C_link_t () != __x) {
        __y     = __x;
        __right = _C_cmp (_KeyOf ()(__v), __x->_C_key ());
        __x     = __x->_C_child [!__right];
    }

    typedef _STD::pair<iterator, bool> _IterPair;

    if (__dup) {
        // allow insertion of duplicate keys
        __ret = _IterPair (_C_insert (__x, __y, __v), true);
        return;
    }

    iterator __j = _C_make_iter (__y);   
    if (__right) {
        if (__j == begin ()) {
            __ret = _IterPair (_C_insert (__x, __y, __v), true);
            return;
        }
        --__j;
    }

    if (_C_cmp (_ITER_NODE (__j)->_C_key (), _KeyOf ()(__v)))
        __ret = _IterPair (_C_insert (__x, __y, __v), true);
    else
        __ret = _IterPair (__j, false);
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
insert (iterator __it, const value_type &__v, bool __dup)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);

#ifdef _RWSTDDEBUG

    {   // verify the consistency of the tree
        size_type __two_logN = 0;
        for (size_type __i = size () + 1; __i >>= 1; ++__two_logN);

        __two_logN *= 2;

        const size_type __depth = _C_depth ();
        _RWSTD_ASSERT (__depth <= __two_logN);
    }

#endif   // _RWSTDDEBUG

    const _C_link_t __hint = _ITER_NODE (__it);

    // if __hint is the right most child, or tree is empty
    if (__hint == _C_end->_C_child [1]) {

        // if tree is not empty and __key is greater
        // then insert on the right
        if (_C_size && _C_cmp (__hint->_C_key (), _KeyOf ()(__v)))
            return _C_insert (0, __hint, __v);

        // otherwise just insert
        return insert (__v, __dup).first;
    }

    // if __hint is past the end and __key is greater,
    // then insert on the right
    if (__hint == _C_end) {
        if (_C_cmp (__hint->_C_child [1]->_C_key(), _KeyOf ()(__v)))
            return _C_insert (0, __hint->_C_child [1], __v);
        return insert (__v, __dup).first;
    }

    // if __hint is the left most child and __key is less
    // then insert on the left
    if (__hint == _C_end->_C_child [0]) {
        if (_C_cmp (_KeyOf ()(__v), __hint->_C_key ()))
            return _C_insert (__hint, __hint, __v);
        return insert (__v, __dup).first;
    }

    const iterator __prev = __it++;

    // if __v falls between __prev and __it, then insert it there
    if (   _C_cmp (_ITER_NODE (__prev)->_C_key (), _KeyOf ()(__v))
        && _C_cmp (_KeyOf ()(__v), _ITER_NODE (__it)->_C_key ())) {

        // if there is no right child of __prev, then __prev is the
        // left child of __it and we insert to right of __prev
        if (_C_link_t () == _ITER_NODE (__prev)->_C_child [1])
            return _C_insert (0, _ITER_NODE (__prev), __v); 

        // otherwise we insert on the left of __it
        return _C_insert (_ITER_NODE (__it), _ITER_NODE (__it), __v);
    }

    // otherwise, do a full traversal
    return insert (__v, __dup).first;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::erase (iterator __it)
{
    _RWSTD_ASSERT_RANGE (begin (), __it);

#ifdef _RWSTDDEBUG

    {   // verify the consistency of the tree
        size_type __two_logN = 0;
        for (size_type __i = size () + 1; __i >>= 1; ++__two_logN);

        __two_logN *= 2;

        const size_type __depth = _C_depth ();
        _RWSTD_ASSERT (__depth <= __two_logN);
    }

#endif   // _RWSTDDEBUG

    if (__it == end ())
        return end ();

    // for notational convenience
    const typename _C_node_t::_C_color_t _Red   = _C_node_t::_C_red;
    const typename _C_node_t::_C_color_t _Black = _C_node_t::_C_black;
    const _C_link_t                       _Null  = _C_link_t ();

    // returned iterator pointing to the element just past `it'
    const iterator __next = ++iterator (__it); 

    _C_link_t __z (_ITER_NODE (__it));   // `it's node
    _C_link_t __y (__z);                 // node to be erased
    _C_link_t __x;

    _C_link_t __x_parent = 0;


    _RWSTD_ASSERT (_Null != __z);

    if (_Null == __y->_C_child [0]) { 
        // __z might have one non-null child. x 
        // holds that child (might be null though).
        __x = __y->_C_child [1]; 
    }
    else if (_Null == __y->_C_child [1]) {
        // __z HAS one non-null child and __x points to it now.
        __x = __y->_C_child [0];
    }
    else {
        // __z has two non-null children; make __y point to its immediate
        // predecessor; __x will point to that predecessor's immediate
        // predecessor.
        __y = _C_node_t::_C_min (__y->_C_child [1]);
        __x = __y->_C_child [1];
    }


    if (__y != __z) {
        // __y != __z ( means __y is in __z's left subtree )

        // __y REPLACES __z

        // relink `y' in place of `z' (at this point __y 
        // is __z's immediate predecessor)
        __z->_C_child [0]->_C_parent = __y; 
        __y->_C_child [0]            = __z->_C_child [0];

        if (__y != __z->_C_child [1]) {
            // __y is predecessor but is not __z's direct child (right)

            // save y's former parent
            __x_parent = __y->_C_parent;

            if (_Null != __x)
                // if __x exists, __x's parent becomes __y's parent
                __x->_C_parent = __y->_C_parent;

            // __y's former parent becomes __x's parent
            __y->_C_parent->_C_child [0] = __x;

            // __y's right child gets __z's former right child
            __y->_C_child [1]            = __z->_C_child [1];

            // __z's former right child gets __y as parent
            __z->_C_child [1]->_C_parent = __y;
        }
        else {

            // in the case when __y is a direct child of __z, 
            // save __y value as __x's parent 
            __x_parent = __y;  

        }

        if (_C_end->_C_parent == __z) {
            // when deleted (__z) is root/end, then __y replaces root/end
            _C_end->_C_parent = __y;
        }
        else {
            // nice shot; 
            // when __z is not root/end, then its parent's
            // child on __z's side becomes __y
            __z->_C_parent->_C_child [__z->_C_parent->_C_child [0] != __z]
                = __y;
        }

        // Finally, y's parent changes to be z's parent;
        // y is completely severed from its place
        __y->_C_parent = __z->_C_parent;

        // swap y and z colors
        _STD::swap (__y->_C_color, __z->_C_color);

        // __y now points to the deleted node 
        __y = __z;
    }
    else {
        // (__y == __z)  ( __z has had only one child (or maybe none) )

        // __x REPLACES __z

        // save __y's/__z's parent (will later become x's parent)
        __x_parent = __y->_C_parent;
        
        if (_Null != __x) {
            // for a non-null __x, set its parent to point to __y's parent
            __x->_C_parent = __y->_C_parent;
        }

        if (_C_end->_C_parent == __z) {
            // if __z was root, __x becomes new root...
            _C_end->_C_parent = __x;
        }
        else
            // ... otherwise, __x simply replaces __z and as such,
            // __z's parent gets __x as right *or* left child
            __z->_C_parent->_C_child [__z->_C_parent->_C_child [0] != __z]
                = __x;

        // ADJUST TREE'S CORNERS
        if (_C_end->_C_child [0] == __z)  {
            // __z is the left corner of the tree (smallest element)

            if (_Null == __z->_C_child [1])
                // when __z leaves the tree, leftmost will be invalidated; 
                // adjust left corner to point to __z's  parent
                _C_end->_C_child [0] = __z->_C_parent;
            else {
                // finding the successor of __z when __z's right subtree
                // is not null; leftmost becomes that minimum/sucessor
                _C_end->_C_child [0] = _C_node_t::_C_min (__x);
            }
        }

        if (_C_end->_C_child [1] == __z)   {
            // __z is in the right corner of the tree

            if (_Null == __z->_C_child [0])
                // when __z leaves the tree, right corner will be invalidated; 
                // adjust right corner to point to __z's  parent
                _C_end->_C_child [1] = __z->_C_parent;
            else {
                // otherwise, find z's predecessor in z's 
                // left subtree; that becomes the new rightmost element
                _C_end->_C_child [1] = _C_node_t::_C_max (__x);
            }
        }
    }


    // COLOR SWAPPING
    // (At this point __y has replaced __z and __z has been removed.)

    if (__y->_C_color != _Red) {

            // __y's color is not red; had it been red, we would have been 
            // done because it would not have altered the "blackness"
            

            // At the loop's beginning, __x can be one of the two:
            // 1. either the strict predecessor of __z's strict predecessor
            //    and in this case is down the subtree on z's left
            // 2. is the node that replaced __z when __z has had only one 
            //    child; that child has replaced __z and its changes
            //    will be propagated upward


        while (   __x != _C_end->_C_parent
               && (_Null == __x || __x->_C_color == _Black)) {

#ifndef _RWSTD_NO_OPTIMIZE_SPEED

            if (__x == __x_parent->_C_child [0]) {

                // X ON THE LEFT SIDE OF ITS PARENT

                // __w is __x's brother/sibling
                _C_link_t __w = __x_parent->_C_child [1];

                if (_Red == __w->_C_color) {
                    __w->_C_color        = _Black;
                    __x_parent->_C_color = _Red;

                    _C_rol (__x_parent);

                    __w = __x_parent->_C_child [1];
                }

                if ((_Null  == __w->_C_child [0] || 
                     _Black == __w->_C_child [0]->_C_color) && 
                    (_Null  == __w->_C_child [1] || 
                     _Black == __w->_C_child [1]->_C_color)) {
                    // __x's sibling (__w) has black children (or null);
                    // color __x's sibling as RED and move up in the tree
                    __w->_C_color = _Red;
                    __x           = __x_parent;
                    __x_parent    = __x_parent->_C_parent;
                }
                else {
                    // __w (__x's sibling) has a child which is RED

                    if (_Null  == __w->_C_child [1] || 
                        _Black == __w->_C_child [1]->_C_color) {

                        // __w's left is RED

                        if (_Null != __w->_C_child [0])
                            // the left RED child becomes BLACK
                            __w->_C_child [0]->_C_color = _Black;

                        // __w (x's sibling) becomes RED - color propagates up
                        __w->_C_color = _Red;

                        _C_ror (__w);

                        __w = __x_parent->_C_child [1];
                    }

                    __w->_C_color        = __x_parent->_C_color;
                    __x_parent->_C_color = _Black;

                    if (_Null != __w->_C_child [1])
                        __w->_C_child [1]->_C_color = _Black;

                    _C_rol (__x_parent);

                    break;
                }
            }
            else {

                // same as the if clause with "right" and "left" exchanged

                _C_link_t __w = __x_parent->_C_child [0];

                if (_Red == __w->_C_color) {
                    __w->_C_color        = _Black;
                    __x_parent->_C_color = _Red;

                    _C_ror (__x_parent);

                    __w = __x_parent->_C_child [0];
                }

                if ((_Null  == __w->_C_child [1] || 
                     _Black == __w->_C_child [1]->_C_color) && 
                    (_Null  == __w->_C_child [0] || 
                     _Black == __w->_C_child [0]->_C_color)) {
                    __w->_C_color = _Red;
                    __x           = __x_parent;
                    __x_parent    = __x_parent->_C_parent;
                }
                else {
                    if (_Null  == __w->_C_child [0] || 
                        _Black == __w->_C_child [0]->_C_color) {
                        if (_Null != __w->_C_child [1])
                            __w->_C_child [1]->_C_color = _Black;

                        __w->_C_color = _Red;

                        _C_rol (__w);

                        __w = __x_parent->_C_child [0];
                    }

                    __w->_C_color        = __x_parent->_C_color;
                    __x_parent->_C_color = _Black;

                    if (_Null != __w->_C_child [0])
                        __w->_C_child [0]->_C_color = _Black;

                    _C_ror (__x_parent);

                    break;
                }
            }

#else   // if defined (_RWSTD_NO_OPTIMIZE_SPEED)

            const bool __left  = (__x == __x_parent->_C_child [0]);
            const bool __right = !__left;

            _C_link_t __w = __x_parent->_C_child [__left];

            if (_Red == __w->_C_color) {
                __w->_C_color            = _Black;
                __x_parent->_C_color = _Red;

                // rotate right if `x' is a right child, otherwise right
                _C_rotate (__x_parent, __right);
                __w = __x_parent->_C_child [__left];
            }

            if ((_Null  == __w->_C_child [__right] || 
                 _Black == __w->_C_child [__right]->_C_color) &&
                (_Null  == __w->_C_child [__left ] || 
                 _Black == __w->_C_child [__left ]->_C_color)) {
                __w->_C_color = _Red;
                __x           = __x_parent;
                __x_parent    = __x->_C_parent;
            }
            else {
                if (_Null  == __w->_C_child [__left] || 
                    _Black == __w->_C_child [__left]->_C_color) {

                    if (_Null != __w->_C_child [__right])
                        __w->_C_child [__right]->_C_color = _Black;

                    __w->_C_color = _Red;
                    _C_rotate (__w, __left);
                    __w = __x_parent->_C_child [__left];
                }

                if (_Null != __w) {
                    __w->_C_color        = __x_parent->_C_color;
                    __x_parent->_C_color = _Black;

                    if (_Null != __w->_C_child [__left])
                        __w->_C_child [__left]->_C_color = _Black;

                    _C_rotate (__x_parent, __right);
                }
                break;
            }

#endif   // _RWSTD_NO_OPTIMIZE_SPEED

        }
         
        if (_Null != __x)
            __x->_C_color = _Black;
        
    }
    
    _C_put_node (__y);
    
    --_C_size;
    
    return __next;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::size_type 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
erase (const key_type &__x)
{
    const _STD::pair<iterator, iterator> __p = equal_range(__x);

    const size_type __n = _DISTANCE (__p.first, __p.second, size_type);

    erase (__p.first, __p.second);

    return __n;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::_C_link_t 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_copy (_C_link_t __x, _C_link_t __p)
{
    // recursive structural copy
    _C_link_t __res = __x;

    while (_C_link_t () != __x) {
        const _C_link_t __y = _C_get_node (__x->_C_value);
        if (__res == __x)
            __res = __y;  // save for return value

        __y->_C_color     = __x->_C_color;
        __y->_C_parent    = __p;
        __p->_C_child [0] = __y;
        __y->_C_child [1] = _C_copy (__x->_C_child [1], __y);
        __p               = __y;
        __x               = __x->_C_child [0];
    }
    __p->_C_child [0] = 0;

    return __res;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
void __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_erase (_C_link_t __x)
{
    // recursively erase without rebalancing

    while (_C_link_t () != __x) {
        _C_erase (__x->_C_child [1]);
        _C_link_t __y = __x->_C_child [0];
        _C_put_node (__x);
        __x = __y;
    }
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
erase (iterator __first, iterator __last)
{
    _RWSTD_ASSERT_RANGE (begin (), __first);
    _RWSTD_ASSERT_RANGE (__first, __last);

    iterator __tmp;

    if (__first == begin () && __last == end () && _C_size) {
        // erase the root node
        _C_erase (_C_end->_C_parent);

        // set the parent pointer of an empty tree to null
        _C_end->_C_parent = 0;

        // set the child pointers of an empty tree to end()
        _C_end->_C_child [0] =
        _C_end->_C_child [1] = _C_end;

        // set the size of the tree to 0
        _C_size = 0;

        // return end()
        __tmp = end ();
    } else
        for (__tmp = end (); !(__first == __last); __tmp = erase (__first++));

    return __tmp;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
find (const key_type &__k)
{
    const iterator __j = lower_bound (__k);

    return __j == end () || _C_cmp (__k, _ITER_NODE (__j)->_C_key ()) ?
        end () : __j;
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
lower_bound (const key_type &__k)
{
    _C_link_t __y = _C_end;

    for (_C_link_t __x = __y->_C_parent; _C_link_t () != __x; ) {
        if (_C_cmp (__x->_C_key (), __k)) {
            __x = __x->_C_child [1];
        }
        else {
            __y = __x;
            __x = __x->_C_child [0];
        }
    }

    return _C_make_iter (__y);
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::iterator 
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
upper_bound (const key_type &__k)
{
    _C_link_t __y = _C_end;

    for (_C_link_t __x = __y->_C_parent; _C_link_t () != __x; ) {
        if (_C_cmp (__k, __x->_C_key ())) {
            __y = __x;
            __x = __x->_C_child [0];
        }
        else
            __x = __x->_C_child [1];
    }

    return _C_make_iter (__y);
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::size_type
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_depth (const_iterator __it, size_type *__rank /* = 0 */) const
{
    // for notational convenience
    const typename _C_node_t::_C_color_t _Black = _C_node_t::_C_black;
    const _C_link_t                       _Null  = _C_link_t ();

    const _C_link_t __node = _ITER_NODE (__it);

    if (_Null == __node)
        return 0;

#ifdef _RWSTDDEBUG

    const typename _C_node_t::_C_color_t _Red = _C_node_t::_C_red;

    if (_Red == __node->_C_color) {

        // both children of every red node must be black

        const bool __left_child_black =
               _Null == __node->_C_child [0]
            || _Black == __node->_C_child [0]->_C_color;

        const bool __right_child_black =
               _Null == __node->_C_child [1]
            || _Black == __node->_C_child [1]->_C_color;

        _RWSTD_ASSERT (__left_child_black);
        _RWSTD_ASSERT (__right_child_black);
    }

#endif   // _RWDSDEBUG

    // every simple path from every node must contain the same number
    // of black nodes; that means that every black node must either
    // have zero or two children (of either color), or a single red
    // child

    size_type __ranks [2];

    if (   _Null == __node->_C_child [0]
        || _Black == __node->_C_child [0]->_C_color)
        __ranks [0] = 1;
    else
        __ranks [0] = 0;

    if (   _Null == __node->_C_child [1]
        || _Black == __node->_C_child [1]->_C_color)
        __ranks [1] = 1;
    else
        __ranks [1] = 0;

    const size_type __depth [] = {
        _Null == __node->_C_child [0] ?
        0 : 1 + _C_depth (_C_make_iter (__node->_C_child [0]), __ranks + 0),
        _Null == __node->_C_child [1] ?
        0 : 1 + _C_depth (_C_make_iter (__node->_C_child [1]), __ranks + 1)
    };

    _RWSTD_ASSERT (__ranks [0] == __ranks [1]);

    if (__rank)
        *__rank += __ranks [0];

    return __depth [__depth [0] < __depth [1]];
}


template <class _Key, class _Val, class _KeyOf, class _Comp, class _Alloc>
typename __rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::size_type
__rb_tree<_Key, _Val, _KeyOf, _Comp, _Alloc>::
_C_level (const_iterator __it) const
{
    if (_ITER_NODE (__it) == _C_end->_C_parent)
        return 0;

    return 1 + _C_level (_C_make_iter (_ITER_NODE (__it)->_C_parent));
}


}   // namespace __rw
