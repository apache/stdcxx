/**************************************************************************
 *
 * rw_char.h - defines a User Defined Character type and its traits
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2004-2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

#ifndef RW_CHAR_INCLUDED
#define RW_CHAR_INCLUDED


// avoid dependencies on any library headers
// to prevent the pollution of the namespace(s)
#include <rw/_traits.h>
#include <testdefs.h>


struct UserChar   // user-defined character type (must be POD)
{
#if    !defined (_RWSTD_NO_LONG_DOUBLE) \
    && !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x540
    long double f;    // exercise correct alignment
#else
    // cannot use long double with SunPro due to a compiler
    // bug that prevents assignments of UserChar() (PR #28328)
    double f;
#endif   // _RWSTD_NO_LONG_DOUBLE

    unsigned char c;   // underlying character representation

    static UserChar eos () {
        // use UserChar::eos() instead of UserChar() to work around broken
        // compilers (e.g., MSVC 6) that do not zero out POD structs
        // on default construction
        const UserChar tmp = { 0, 0 };
        return tmp;
    }

    // definition of a POD-struct (9, p4):
    // A POD-struct is an aggregate class that has no non-static data
    // members of type pointer to member, non-POD-struct, non-POD-union
    // (or array of such types) or reference, and has no user-defined
    // copy assign-ment operator and no user-defined destructor.

    // definition of an aggregate (8.5.1, p1):
    // An aggregate is an array or a class (clause 9) with no user-declared
    // constructors (12.1), no private or protected non-static data members
    // (clause 11), no base classes (clause 10), and no virtual functions
    // (10.3).
};


inline UserChar make_char (char c, UserChar*)
{
    const UserChar ch = { 0.0, c };
    return ch;
}

inline char make_char (char c, char*)
{
    return c;
}

#ifndef _RWSTD_NO_WCHAR_T

inline wchar_t make_char (char c, wchar_t*)
{
    typedef unsigned char UChar;
    return wchar_t (UChar (c));
}

#endif   // _RWSTD_NO_WCHAR_T

///////////////////////////////////////////////////////////////////////////
// 21.1.2, p2 of C++ 03:
//
//   typedef INT_T int_type;
//
// -2- Requires: For a certain character container type char_type, a related
//     container type INT_T shall be a type or class which can represent all
//     of the valid characters converted from the corresponding char_type
//     values, as well as an end-of-file value, eof(). The type int_type
//     represents a character container type which can hold end-of-file to
//     be used as a return type of the iostream class member functions.224)
// ____________
// Footnote 224: If eof() can be held in char_type then some iostreams
//               operations may give surprising results.

class UserInt
{
    void* ptr_;   // to check validity
    int   i_;     // underlying int representation

    // valid values are 0 through 256 with 256 representing EOF
public:

    // default ctor creates a valid object but assigns it an invalid
    // value so that the only legal operations on it are assignment
    // and copy
    UserInt (): ptr_ (&i_ /* valid */), i_ (-1 /* invalid */) { /* empty */ }

    UserInt (const UserInt &rhs)
        : ptr_ (&i_), i_ (rhs.i_) {
        RW_ASSERT (rhs.ptr_ == &rhs.i_);             // verify rhs is valid 
        RW_ASSERT (-1 <= rhs.i_  && rhs.i_ < 257);   // i may be invalid
    }

    // (almost) assignable; i.e., returns void instead of UserInt&
    // for extra robustness
    void operator= (const UserInt &rhs) {
        RW_ASSERT (ptr_     == &i_);                // verify *this is valid
        RW_ASSERT (rhs.ptr_ == &rhs.i_);            // verify rhs is valid
        RW_ASSERT (-1 <= i_     && i_     < 257);   // i may be invalid
        RW_ASSERT (-1 <  rhs.i_ && rhs.i_ < 257);   // rhs.i must ve valid

        i_ = rhs.i_;
    }

    static UserInt eof () {
        UserInt tmp;
        tmp.i_ = 256;   // EOF
        return tmp;
    }

    static UserInt from_char (const UserChar &c) {
        UserInt tmp;
        tmp.i_ = int (c.c);
        return tmp;
    }

    UserChar to_char () const {
        const UserChar tmp = { 0, i_ };
        return tmp;
    }

    ~UserInt () {
        RW_ASSERT (ptr_ == &i_);            // verify *this is valid
        RW_ASSERT (-1 <= i_ == i_ < 257);   // i may be invalid

        i_   = _RWSTD_INT_MIN;   // invalidate
        ptr_ = &ptr_;            // invalidate
    }

    bool equal (const UserInt &rhs) const {
        RW_ASSERT (ptr_     == &i_);               // verify *this is valid
        RW_ASSERT (rhs.ptr_ == &rhs.i_);           // verify rhs is valid
        RW_ASSERT (-1 < i_     && i_     < 257);   // i must ve valid
        RW_ASSERT (-1 < rhs.i_ && rhs.i_ < 257);   // rhs.i must be valid

        return i_ == rhs.i_;
    }
};


// user-defined character traits
template <class charT>
struct UserTraits: std::char_traits<charT>
{
    typedef typename std::char_traits<charT>::int_type int_type;

    static int_type eof_;

    static int_type eof () {
        return eof_;
    }
};

template <class charT>
typename UserTraits<charT>::int_type
UserTraits<charT>::eof_ = std::char_traits<charT>::eof ();


_TEST_EXPORT
_RWSTD_SPECIALIZED_CLASS
struct UserTraits<UserChar>   // user-defined character traits
{
    typedef UserChar char_type;
    typedef UserInt  int_type;

    // avoid any dependency on the library
    typedef int                   off_type;     // std::streamoff
    typedef int                   state_type;   // std::mbstate_t
    typedef std::fpos<state_type> pos_type;     // std::fpos<state_type>

    // accesses to the char_type::f member may trigger a SIGBUS
    // on some architectures (e.g., PA or SPARC) if the member
    // isn't appropriately aligned
    static void assign (char_type &c1, const char_type &c2) {
        c1.f = c2.f;
        c1.c = c2.c;
    }

    static bool eq (const char_type &c1, const char_type &c2) {
        return c1.f == c2.f && c1.c == c2.c;
    }

    static bool lt (const char_type &c1, const char_type &c2) {
        return c1.f < c2.f || c1.f == c2.f && c1.c < c2.c;
    }

    static int
    compare (const char_type*, const char_type*, _RWSTD_SIZE_T);
        
    static _RWSTD_SIZE_T
    length (const char_type*);
 
    static const char_type*
    find (const char_type*, _RWSTD_SIZE_T, const char_type&);

    static char_type*
    copy (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    move (char_type*, const char_type*, _RWSTD_SIZE_T);

    static char_type*
    assign (char_type*, _RWSTD_SIZE_T, char_type);

    static int_type
    not_eof (const int_type &i) {
        if (eq_int_type (i, int_type::eof ())) {
            const char_type c = { 0, 0 };
            return int_type::from_char (c);
        }
        return i;
    }

    static char_type to_char_type (const int_type &i) {
        return i.to_char ();
    }
      
    static int_type to_int_type (const char_type &c) {
        return int_type::from_char (c);
    }

    static bool eq_int_type (const int_type &i1, const int_type &i2) {
        return i1.equal (i2);
    }

    static int_type eof () {
        return int_type::eof ();
    }
};


_TEST_EXPORT
char* rw_widen (char*, const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

#ifndef _RWSTD_WCHAR_T

_TEST_EXPORT
wchar_t* rw_widen (wchar_t*, const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

#endif   // _RWSTD_WCHAR_T

_TEST_EXPORT
UserChar* rw_widen (UserChar*, const char*, _RWSTD_SIZE_T = _RWSTD_SIZE_MAX);

#endif   // RW_CHAR_INCLUDED
