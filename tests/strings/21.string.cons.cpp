/***************************************************************************
 *
 * 21.string.cons.cpp - test exercising string constructors [lib.string.cons]
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

#include <stdexcept>    // for length_error
#include <string>
#include <cstring>

#include <cmdopt.h>
#include <driver.h>

#include <alg_test.h>      // for InputIterator

#ifndef _RWSTD_NO_REPLACEABLE_NEW_DELETE
#  include <rw_new.h>    // for replacing operator new/delete
#else   // if defined (_RWSTD_NO_REPLACEABLE_NEW_DELETE)
   // e.g., AIX, Win32
#  define rwt_check_leaks(pint, ignore) (pint ? *((std::size_t*)pint) = 0 : 0)
#endif   // _RWSTD_NO_REPLACEABLE_NEW_DELETE


template<class charT>
class Lit { };

_RWSTD_SPECIALIZED_CLASS
class Lit<char>
{
public:
    static const char null[];
    static const char space[];
    static const char aString[];
    static const char abcdef[];
    static const char abcdefg[];
    static const char abcnulldefg[];
    static const char nullchar[];
    static const char a[];
    static const char b[];
    static const char c[];
    static const char d[];
    static const char ef[];
    static const char f[];
    static const char cd[];
    static const char abc[];
    static const char ddddd[];
};

const char Lit<char>::null[] = "\0";
const char Lit<char>::space[] = " ";
const char Lit<char>::aString[] = "A String";
const char Lit<char>::abcdef[] = "abcdef";
const char Lit<char>::abcdefg[] = "abcdefg";
const char Lit<char>::abcnulldefg[] = "abc\0defg";
const char Lit<char>::nullchar[] = " ";
const char Lit<char>::a[] = "a";
const char Lit<char>::b[] = "b";
const char Lit<char>::c[] = "c";
const char Lit<char>::d[] = "d";
const char Lit<char>::ef[] = "ef";
const char Lit<char>::f[] = "f";
const char Lit<char>::cd[] = "cd";
const char Lit<char>::abc[] = "abc";
const char Lit<char>::ddddd[] = "ddddd";


#ifndef _RWSTD_NO_WCHAR_T
_RWSTD_SPECIALIZED_CLASS
class Lit<wchar_t>
{
public:
    static const wchar_t null[];
    static const wchar_t space[];
    static const wchar_t aString[];
    static const wchar_t abcdef[];
    static const wchar_t abcdefg[];
    static const wchar_t abcnulldefg[];
    static const wchar_t nullchar[];
    static const wchar_t a[];
    static const wchar_t b[];
    static const wchar_t c[];
    static const wchar_t d[];
    static const wchar_t ef[];
    static const wchar_t f[];
    static const wchar_t cd[];
    static const wchar_t abc[];
    static const wchar_t ddddd[];
};

const wchar_t Lit<wchar_t>::null[] = L"\0";
const wchar_t Lit<wchar_t>::space[] = L" ";
const wchar_t Lit<wchar_t>::aString[] = L"A String";
const wchar_t Lit<wchar_t>::abcdef[] = L"abcdef";
const wchar_t Lit<wchar_t>::abcdefg[] = L"abcdefg";
const wchar_t Lit<wchar_t>::abcnulldefg[] = L"abc\0defg";
const wchar_t Lit<wchar_t>::nullchar[] = L" ";
const wchar_t Lit<wchar_t>::a[] = L"a";
const wchar_t Lit<wchar_t>::b[] = L"b";
const wchar_t Lit<wchar_t>::c[] = L"c";
const wchar_t Lit<wchar_t>::d[] = L"d";
const wchar_t Lit<wchar_t>::ef[] = L"ef";
const wchar_t Lit<wchar_t>::f[] = L"f";
const wchar_t Lit<wchar_t>::cd[] = L"cd";
const wchar_t Lit<wchar_t>::abc[] = L"abc";
const wchar_t Lit<wchar_t>::ddddd[] = L"ddddd";

#endif

/**************************************************************************/

/* extern */ int rw_opt_no_copy_ctor;   // for --no-copy-ctor
/* extern */ int rw_opt_no_def_ctor;    // for --no-default-ctor
/* extern */ int rw_opt_no_ptr_ctor;    // for --no-pointer-ctor
/* extern */ int rw_opt_no_fill_ctor;   // for --no-fill-ctor


template <class T>
void test_signatures (T, const char* tname)
{
    // verify that constructs below compile ad run (no assertions)

    typedef std::char_traits<T>                      Traits;
    typedef std::allocator<T>                        Allocator;
    typedef std::basic_string<T, Traits, Allocator>  String;

    typedef typename String::size_type SizeT;

    rw_info (0, 0, 0, "std::basic_string<%s> constructors signatures", tname);

    // 21.3.1, p2
    String s0 /* (Allocator ()) */;
    String s1 = String (Allocator ());

    // lwg issue #42
    String s2 = String (String ());

    // 21.3.1, p3
    String s3 = String (String (), SizeT (), SizeT () /* , Allocator () */);
    String s4 = String (String (), SizeT (), SizeT (), Allocator ());

    // 21.3.1, p6
    const T c = T ();
    String s5 (&c, 1 /* , Allocator () */);
    String s6 (&c, 1, Allocator ());

    // 21.3.1, p9
    String s7 (&c /* , Allocator () */);
    String s8 (&c, Allocator ());

    // 21.3.1, p12
    String s9  = String (SizeT (), c /* , Allocator () */);
    String s10 = String (SizeT (), c, Allocator ());

    // 21.3.1, p15 - template ctor

    // check basic_string::const_pointer
    String s11 = String ((const T*)0, (const T*)0 /* , Allocator () */);
    String s12 = String ((const T*)0, (const T*)0, Allocator ());

    _RWSTD_UNUSED (s0);  _RWSTD_UNUSED (s1);  _RWSTD_UNUSED (s2);
    _RWSTD_UNUSED (s3);  _RWSTD_UNUSED (s4);  _RWSTD_UNUSED (s5);
    _RWSTD_UNUSED (s6);  _RWSTD_UNUSED (s7);  _RWSTD_UNUSED (s8);
    _RWSTD_UNUSED (s9);  _RWSTD_UNUSED (s10); _RWSTD_UNUSED (s11);
    _RWSTD_UNUSED (s12);

#ifndef _RWSTD_NO_INLINE_MEMBER_TEMPLATES
#  if _MSC_VER >= 1300   // MSVC 6.0 and prior are too dumb to handle this

    // check a pointer other than basic_string::const_pointer
    String s13 = String ((const int*)0, (const int*)0 /* , Allocator () */);
    String s14 = String ((const int*)0, (const int*)0, Allocator ());

    // check integral types
    String s15 = String (true, false /* , Allocator () */);
    String s16 = String (true, false, Allocator ());
    String s17 = String (2, 1 /* , Allocator () */);
    String s18 = String (2, 1, Allocator ());
    String s19 = String (2L, 1L /* , Allocator () */);
    String s20 = String (2L, 1L, Allocator ());

    _RWSTD_UNUSED (s13); _RWSTD_UNUSED (s14); _RWSTD_UNUSED (s15);
    _RWSTD_UNUSED (s16); _RWSTD_UNUSED (s17); _RWSTD_UNUSED (s18);
    _RWSTD_UNUSED (s19); _RWSTD_UNUSED (s20);

#  endif   // MSVC > 6.0
#endif   // _RWSTD_NO_INLINE_MEMBER_TEMPLATES

}

/**************************************************************************/

template <class T>
void test_ctors (T, const char* tname)
{
    typedef std::char_traits<T>                      Traits;
    typedef std::allocator<T>                        Allocator;
    typedef std::basic_string<T, Traits, Allocator>  String;

    typedef typename String::size_type SizeT;

    static const char* const sname = "basic_string";

    SizeT nbytes = 0;
    SizeT nblocks = 0;

    // establish a checkpoint for memory leaks
    rwt_check_leaks (0, 0);

    //////////////////////////////////////////////////////////////
    if (rw_opt_no_def_ctor) {
        rw_note (0, 0, 0, "std::%s<%s>::%1$s (const %s&) test disabled",
                 sname, tname, "allocator_type");
    }
    else {
        rw_info (0, 0, 0, "std::%s<%s>::%1$s (const allocator_type&)",
                 sname, tname);

        // 21.3.1, p2 - create an empty string
        const String s0;

        // 21.3.1, p2 - effects table, row 1
        rw_assert (0 != s0.data (), 0, __LINE__,
                   "%s<%s>::%1$s ().data () != 0", sname, tname);

        // 21.3.1, p2 - effects table, row 2
        rw_assert (0 == s0.size (), 0, __LINE__,
                   "%s<%s>::%1$s().size () == 0", sname, tname);

        // 21.3.1, p2 - effects table, row 3
        rw_assert (s0.size () <= s0.capacity (), 0, __LINE__,
                   "%s<%s>::%1$s(const char_type*).capacity() >= %zu, got %zu",
                   sname, tname, s0.size (), s0.capacity ());

        rw_assert (0 != s0.c_str (), 0, __LINE__,
                   "%s<%s>().c_str () != 0", sname, tname);

        rw_assert (Traits::eq (*s0.c_str(), T ()), 0, __LINE__,
                   "%s<%s>().c_str ()[0] == '\\0'", sname, tname);
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "%s<%s>() leaked %zu bytes in %zu block(s)",
               sname, tname, nbytes, nblocks);


    //////////////////////////////////////////////////////////////
    if (rw_opt_no_ptr_ctor) {
        rw_note (0, 0, 0,
                 "std::%s<%s>::%1$s (const_pointer %s&) test disabled",
                 sname, tname);
    }
    else {
        rw_info (0, 0, 0, "std::%s<%s>::%1$s (const_pointer)", sname, tname);
        
        const T ca [] = { '\0' };

        // 21.3.1, p9 - create an empty string using a (non-null) pointer
        const String s0 (ca + 0);
    
        // 21.3.1, p10 - effects table, row 1
        rw_assert (0 != s0.data (), 0, __LINE__,
                   "%s<%s>::%1$s(const_pointer = %{*Ac}).data() != 0",
                   sname, tname, int (sizeof *ca), ca);

        // 21.3.1, p10 - effects table, row 2
        rw_assert (Traits::length (ca) == s0.size (), 0, __LINE__,
                   "%s<%s>::%1$s(const_pointer = %{*Ac}).size() "
                   "== 0, got %zu",
                   sname, tname, int (sizeof *ca), ca, s0.size ());

        // 21.3.1, p10 - effects table, row 3
        rw_assert (s0.size () <= s0.capacity (), 0, __LINE__,
                   "%s<%s>::%1$s(const_pointer= %{*Ac}).capacity() "
                   ">= %zu, got %zu",
                   sname, tname, int (sizeof *ca), ca,
                   s0.size (), s0.capacity ());

        rw_assert (0 != s0.c_str (), 0, __LINE__,
                   "%s<%s>::%1$s(const_pointer = %{*Ac}).c_str () != 0",
                   sname, tname, int (sizeof *ca), ca);

        rw_assert (Traits::eq (*s0.c_str(), T ()), 0, __LINE__,
                   "%s<%s>::%1$s(const_pointer = %{*Ac})"
                   ".c_str()[0] == '\\0'",
                   sname, tname, int (sizeof *ca), ca);
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "%s<%s>(\"\") leaked %d bytes in %d block(s)",
               sname, tname, nbytes, nblocks);

#ifndef _RWSTD_NO_EXCEPTIONS

    //////////////////////////////////////////////////////////////
    if (rw_opt_no_fill_ctor) {
        rw_note (0, 0, 0,
                 "std::%s<%s>::%1$s (size_type, value_type) test disabled",
                 sname, tname);
    }
    else {
        rw_info (0, 0, 0,
                 "std::%s<%s>::%1$s (size_type, value_type)", sname, tname);

        bool thrown = false;
    
        try {
            String ts (String::npos, Lit<T>::space[0]);
        }
        catch (std::length_error) {
            thrown = true;
        }
        catch (...) {
        }
    
        rw_assert (thrown, 0, __LINE__,
                   "%s<%s>::%1$s (size_type = %zu, char_type = %#lc) "
                   "threw std::length_error",
                   sname, tname, String::npos, Lit<T>::space[0]);
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "%s<%s>(size_type, char_type) leaked %d bytes "
               "in %d block(s) after an exception",
               sname, tname, nbytes, nblocks);

#endif   // _RWSTD_NO_EXCEPTIONS

    {
        const SizeT siz = 120;
    
        const String ts (siz, Lit<T>::space[0] );

        // Length set to siz when reserving
        rw_assert ( (ts.length() == siz), 0, __LINE__, "C9");
    
        // Allocated size at least as large as requested
        rw_assert ( (ts.capacity() >= siz), 0, __LINE__, "C10");
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "%s<%s>(size_type, char_type) leaked %d bytes "
               "in %d block(s)", sname, tname, nbytes, nblocks);

    //////////////////////////////////////////////////////////////
    if (rw_opt_no_copy_ctor) {
        rw_note (0, 0, 0, "%s<%s>::%1$s (const %1$&) test disabled",
                 sname, tname);
    }
    else {
        rw_info (0, 0, 0, "std::%s<%s>::%1$s (const %1$s&)", sname, tname);

        const String s1 (Lit<T>::aString);
        const String s2 (s1);

#ifndef _RWSTD_NO_STRING_REF_COUNT

        // verify that the two strings share the same data
        rw_assert (s1.data () == s2.data (), 0, __LINE__,
                   "s1.data () == s2.data (); strings NOT reference counted");

#else   // if defined (_RWSTD_NO_STRING_REF_COUNT)

        // verify that the two strings share the same data
        rw_assert (s1.data () != s2.data (), 0, __LINE__,
                   "s1.data () != s2.data (); strings reference counted");

#endif   // _RWSTD_NO_STRING_REF_COUNT

        // 21.3.1, p5 - effects table, row 1
        rw_assert (&s2 [0] == s2.data (), 0, __LINE__,
                   "%s<%s>::%1$s(const %1$s&)", sname, tname);

        // 21.3.1, p5 - effects table, row 2
        rw_assert (s1.size () == s2.size (), 0, __LINE__,
                   "%s<%s>::%1$s(const %1$s&).size () == %zu, got %zu",
                   sname, tname, s1.size (), s2.size ());

        // 21.3.1, p5 - effects table, row 3
        rw_assert (s2.size () <= s2.capacity (), 0, __LINE__,
                   "%s<%s>::%1$s(%1$s&).capacity () >= %zu, got %zu",
                   sname, tname, s2.size (), s2.capacity ());

        // verify that both strings are the same
        rw_assert (s2 == s1
                   && 0 == Traits::compare (s1.data (), s2.data (), s1.size ()),
                   0, __LINE__,
                   "std::operator==(const %s<%s>&, const %1$s<%2$s>&)",
                   sname, tname);
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "%s<%s>(const %1$s&) leaked %d bytes in %d block(s)",
               sname, tname, nbytes, nblocks);

    if (!rw_opt_no_copy_ctor) {
        String s1;
        String s2(s1);
    
        // Empty copies have same lengths
        rw_assert((s1.length() == s2.length()), 0, __LINE__, "C19");

        // Empty copies compare equal
        rw_assert ( (s1.compare(s2) == 0), 0, __LINE__, "C16");
    
        // Empty copies return same data string.
        rw_assert (!Traits::compare (s1.data (), s2.data (), s1.size ()), 
                   0, __LINE__, "C17");

        // Empty copies return same c_str string.
        rw_assert (!Traits::compare (s1.c_str (), s2.c_str (), s1.size()), 
                   0, __LINE__, "C18");
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "%s<%s>(const basic_string&) leaked %d bytes "
               "in %d block(s)", tname, nbytes, nblocks);


#ifndef _RWSTD_NO_EXCEPTIONS

    {   //////////////////////////////////////////////////////////////
        rw_info (0, 0, 0, "std::%s<%s>::%1$s (const %1$s&, size_type)",
                 sname, tname);
    
        bool thrown = false;
        String t1( Lit<T>::abcdef );

        try {
            String t2(t1, 7);
        }
        catch (std::out_of_range) {
            thrown = true;
        }
        catch (...) {
        }
    
        // Threw exception when pos exceeded length of str
        rw_assert ( ( thrown==true ) , 0, __LINE__, "C20");
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 6);
    
        // Correct single character beyond end value
        rw_assert ( t2 == Lit<T>::null, 0, __LINE__, "C21");

        // Correctly references nullRef
        rw_assert ( t2.data() == String ().data(), 0, __LINE__, "C22");
    
        // Correct single character beyond end length
        rw_assert ( (t2.length() == 0), 0, __LINE__, "C23");
    }
    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 5);
    
        // Correct single character at end value
        rw_assert ( (t2 == Lit<T>::f), 0, __LINE__, "C24");
    
        // Correct single character at end length
        rw_assert ( (t2.length() == 1), 0, __LINE__, "C25");
    }
    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 4);
    
        // Correct last two characters value
        rw_assert ( (t2 == Lit<T>::ef), 0, __LINE__, "C26");
    
        // Correct last two characters length
        rw_assert ( (t2.length() == 2), 0, __LINE__, "C27");
    }
    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 0);
    
        // Correct entire String value
        rw_assert ( (t2 == t1), 0, __LINE__, "C28");
    
        // Correct entire String length
        rw_assert ( (t2.length() == t1.length()), 0, __LINE__, "C29");
    }
    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 0, 0);
    
        // Correct zero-length string off front value
        rw_assert ( t2 == Lit<T>::null, 0, __LINE__, "C30");
    
        // Correctly references nullRef
        rw_assert ( t2.data () == String ().data (), 0, __LINE__, "C31");
    
        // Correct zero-length string off front length
        rw_assert ( (t2.length() == 0), 0, __LINE__, "C32");
    }
    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 0, 1);
    
        // Correct one character off front value
        rw_assert ( (t2 == Lit<T>::a), 0, __LINE__, "C33");
    
        // Correct one character off front length
        rw_assert ( (t2.length() == 1), 0, __LINE__, "C34");
    }
    {
        String t1( Lit<T>::abcdef );
        String t2(t1, 2, 2);
    
        // Correct subString from middle value
        rw_assert ( (t2 == Lit<T>::cd), 0, __LINE__, "C35");
    
        // Correct subString from middle length
        rw_assert ( (t2.length() == 2), 0, __LINE__, "C36");
    }
    {
        rw_info (0, 0, 0, "std::%s<%s>::%1$s (const_pointer, size_type)",
                 sname, tname);
    
        String t1( Lit<T>::abcdefg, (SizeT)0);
        String t2( Lit<T>::abcdefg, 1 );
        String t3( Lit<T>::abcdefg, 3 );
        String t4( Lit<T>::abcnulldefg, 8 ); //Embedded null
    
        // Correct value when n is zero
        rw_assert ( t1 == Lit<T>::null, 0, __LINE__, "C37");
    
        // Correctly references nullRef when n is zero
        rw_assert ( t1.data () == String ().data (), 0, __LINE__, "C38");
    
        // Correct value when n is one
        rw_assert ( (t2 == Lit<T>::a), 0, __LINE__, "C39");
    
        // Correct value when n is three
        rw_assert ( (t3 == Lit<T>::abc), 0, __LINE__, "C40");
    
        // Correct value with embedded null
        rw_assert (!std::memcmp(t4.c_str(), Lit<T>::abcnulldefg, 8), 
                   0, __LINE__, "C41");
    
        // Lengths are n that was passed in
        rw_assert (((t1.length()==0)&&(t2.length()==1) &&
                    (t3.length()==3)&&(t4.length()==8)), 
                   0, __LINE__, "C42");
    }

    if (!rw_opt_no_ptr_ctor) {
        rw_info (0, 0, 0, "std::%s<%s>::"
                 "basic_string (const_pointer)", tname);
    
        String t1( Lit<T>::abcdefg );
        String t2( Lit<T>::abcnulldefg );
    
        // Correct value with normal string
        rw_assert ( (t1 == Lit<T>::abcdefg), 0, __LINE__, "C43");
    
        // Correct value with _EXPLICIT embedded null
        rw_assert ( (t2 == Lit<T>::abc), 0, __LINE__, "C44");
    
        // Lengths are n that was passed in
        rw_assert ( ((t1.length() == 7) && (t2.length() == 3)), 
                    0, __LINE__, "C45");
    }

#ifndef _RWSTD_NO_EXCEPTIONS
    {
        rw_info (0, 0, 0, "std::%s<%s>::%1$s (size_type, char_type)",
                 sname, tname);

        bool thrown = false;
    
        try { String ts(String::npos, Lit<T>::a[0] ); }
        catch (std::length_error) { thrown = true; }
    
        rw_assert ( (thrown==true), 0, __LINE__, "C46");
    }
#endif

    {
        String t1( (SizeT)0, Lit<T>::b[0] );
        String t2( (SizeT)1, Lit<T>::c[0] );
        String t3( (SizeT)5, Lit<T>::d[0] );
    
        // Correct value for no repetitions
        rw_assert ( (t1 == Lit<T>::null), 0, __LINE__, "C47");

        // Correctly references nullRef when no reps
        rw_assert ( t1.data () == String ().data (), 0, __LINE__, "C48");
    
        // Correct value for default repetitions
        rw_assert ( (t2 == Lit<T>::c ), 0, __LINE__, "C49");
    
        // Correct value for five repetitions
        rw_assert ( (t3 == Lit<T>::ddddd), 0, __LINE__, "C50");
    
        // Correct lengths for all
        rw_assert (((t1.length() == 0) && (t2.length() == 1) &&
                    (t3.length() == 5) ), 0, __LINE__, "C51");
    }

    {
        // extension (for compilers with no member templates)
        rw_info (0, 0, 0, "std::%s<%s>::%1$s(const_pointer, const_pointer)",
                 sname, tname);

        const T ca [] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };

        SizeT size = sizeof ca / sizeof *ca;

        // 21.3.1, p16 - construct a string using two pointers
        const String s1 (ca, ca + 0);

        // verify that the constructed string is empty
        rw_assert (s1.size () == 0, 0, __LINE__,
                   "%s (const_pointer, const_pointer).size ()"
                   " == 0, got %zu", sname, s1.size ());

        rw_assert (s1.data () == String ().data (), 0, __LINE__,
                   "%s (const_pointer, const_pointer).data () == "
                   "%1$s().data ()", sname);

        const String s2 (ca, ca + size);

        // verify that both strings are the same
        rw_assert (s2.size () == size, 0, __LINE__,
                   "%s (const_pointer, const_pointer).size ()"
                   " == %zu, got %zu", sname, size, s2.size ());

        rw_assert (0 == Traits::compare (s2.data (), ca, s2.size ()), 
                   0, __LINE__, "%s (const_pointer, const_pointer)", sname);
    }

#ifndef _RWSTD_NO_INLINE_MEMBER_TEMPLATES
#  if !defined (_MSC_VER) || _MSC_VER >= 1300
 
    //////////////////////////////////////////////////////////////
    if (rw_opt_no_fill_ctor) {
        rw_note (0, 0, 0, "template <class %s> "
                 "std::%s<%s>::%2$s (%1$s, %1$s) test disabled",
                 "InputIterator", "basic_string", sname, tname);
    }
    else {
        rw_info (0, 0, 0, "template <class %s> "
                 "std::%s<%s>::%2$s (%1$s, %1$s)",
                 "InputIterator", "basic_string", sname, tname);

        const T ca [] = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };
        const int ia []   = { 'a', 'b', 'c', 'd', 'e', 'f', '\0' };

        SizeT size = sizeof ia / sizeof *ia;

        // 21.3.1, p16 - construct a string using two InputIterators
        const String s1 =
            String (InputIter<int>(ia, ia, ia + size),
                    InputIter<int>(ia + size, ia + size, ia + size));

        // verify that both strings are the same
        rw_assert (s1.size () == size, 0, __LINE__,
                   "%s (InputIterator, InputIterator).size ()"
                    " == %zu, got %zu", sname, size, s1.size ());

        // verify that both strings are the same
        rw_assert (0 == Traits::compare (s1.data (), ca, s1.size ()), 
                   0, __LINE__,
                   "%s (InputIterator, InputIterator)", sname);

        const T ca2 [] = { 'a', 'a', 'a', 'a', 'a', 'a', '\0' };

        // 21.3.1, p15 - construct a string using a template ctor
        //               with InputIterator being an integral type
        const String s2 = String (6, int (ca2 [0]));

        // effects: same as basic_string (size_type (begin) size_type (end))
        rw_assert (s2.size () == 6, 0, __LINE__,
                   "%s (InputIterator, InputIterator).size ()"
                   " == %zu, got %zu", sname, 6, s1.size ());

        // verify that both strings are the same
        rw_assert (0 == Traits::compare (s2.data (), ca2, s2.size ()), 
                   0, __LINE__,
                   "%s (InputIterator, InputIterator)", sname);
    }

    nblocks = rwt_check_leaks (&nbytes, 0);

    rw_assert (!nblocks && !nbytes, 0, __LINE__,
               "template <class %s> %s::%2$s"
               "(%1$s, %1$s) leaked %d bytes in %d block(s)",
               "InputIterator", tname, sname, nbytes, nblocks);

#  endif   // !MSVC ||  MSVC > 6.0
#endif   // _RWSTD_NO_INLINE_MEMBER_TEMPLATES


#if 0

    {   // FIXME: implement and enable

        rw_info (0, 0, 0, 
                 "dynamic allocation, construction, destruction, "
                 "and deallocation of string objects across "
                 "library boundaries");

        typedef std::allocator<T>                       Allocator;
        typedef std::basic_string<T, Traits, Allocator> String;

        const T eos = T ();
        const T* const empty = &eos;
        const T non_empty[] = { ' ', '\0' };

        // exercise the ability to dynamically allocate, construct
        // destroy, and deallocate std::string and std::wstring objects
        // across library boundaries; especially important when strings
        // are reference counted and _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
        // is #defined
        String *ps = copy_string ((String*)0);
        delete ps;

        ps = copy_string (new String ());
        delete ps;

        ps = copy_string (new String (empty));
        delete ps;

        ps = copy_string (new String (0, eos));
        delete ps;

        ps = copy_string (new String (non_empty));
        delete ps;

        ps = copy_string (copy_string ((String*)0));
        delete ps;

        ps = copy_string (copy_string (new String (empty)));
        delete ps;

        ps = copy_string (copy_string (new String (0, eos)));
        delete ps;

        ps = copy_string (copy_string (new String (non_empty)));
        delete ps;
    }

#endif   // 0/1

}

/**************************************************************************/

int run_test (int, char**)
{
    if (rw_enabled ("char")) {
        test_signatures (char (), "char");
        test_ctors (char (), "char");
    }
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t")) {
        test_signatures (wchar_t (), "wchar_t");
        test_ctors (wchar_t (), "wchar_t");
    }
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.cons",
                    0 /* no comment */,
                    run_test,
                    "|-no-copy-ctor# "
                    "|-no-default-ctor# "
                    "|-no-pointer-ctor# ",
                    &rw_opt_no_copy_ctor,
                    &rw_opt_no_def_ctor,
                    &rw_opt_no_ptr_ctor);
}
