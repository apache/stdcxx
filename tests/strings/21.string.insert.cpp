/***************************************************************************
 *
 * 21.string.insert.cpp - test exercising [lib.string.insert]
 *
 * $Id: //stdlib/dev/tests/stdlib/string/insert.cpp#12 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx, Inc. All Rights Reserved.
 *
 * This computer software  is owned by Quovadx, Inc.  and is protected by
 * U.S.  copyright laws  and other  laws and  by  international treaties.
 * This computer  software is furnished  by Quovadx, Inc., pursuant  to a
 * written license  agreement and may  be used, copied,  transmitted, and
 * stored only in accordance with the terms of such license agreement and
 * with  the inclusion  of  the above  copyright  notice.  This  computer
 * software or any other copies  thereof may not be provided or otherwise
 * made available to any other person.
 *
 *
 * U.S. Government Restricted Rights.
 *
 * This computer software: (a) was developed at private expense and is in
 * all respects the proprietary information of Quovadx, Inc.; (b) was not
 * developed with  government funds;  (c) is a  trade secret  of Quovadx,
 * Inc. for all purposes of the  Freedom of Information Act; and (d) is a
 * commercial item  and thus, pursuant  to Section 12.212 of  the Federal
 * Acquisition  Regulations (FAR) and  DFAR Supplement  Section 227.7202,
 * Government's use,  duplication or disclosure of  the computer software
 * is subject to the restrictions set forth by Quovadx, Inc.
 *
 **************************************************************************/

#include <stdexcept>   // for length_error, out_of_range
#include <string>

#include <cmdopt.h>
#include <driver.h>
#include <valcmp.h>

/**************************************************************************/

template <class charT>
struct Lit
{
    static const charT null[];
    static const charT space[];
    static const charT a[];
    static const charT n[];
    static const charT s[];
    static const charT x[];
    static const charT st[];
    static const charT abc[];
    static const charT tes[];
    static const charT xxx[];
    static const charT string[];
    static const charT firstString[];
    static const charT firstSecondString[];
    static const charT firstStringSecond[];
    static const charT firFirstString[];
    static const charT firstFirstStString[];
    static const charT firstStndring[];
    static const charT firstnString[];
    static const charT firstStrings[];
    static const charT sFirstString[];
    static const charT secondFirstString[];
    static const charT second[];
    static const charT coFirstString[];
    static const charT testString[];
    static const charT longTest[];
};

#define LIT(member) \
    template <class charT> const charT Lit<charT>::member[]


LIT (null) = { 0 };
LIT (space) = { ' ', 0 };
LIT (a) = { 'a', 0 };
LIT (n) = { 'n', 0 };
LIT (s) = { 's', 0 };
LIT (x) = { 'x', 0 };
LIT (st) = { 's', 't', 0 };
LIT (abc) = { 'a', 'b', 'c', 0 };
LIT (tes) = { 't', 'e', 's', 0 };
LIT (xxx) = { 'x','x','x', 0 };
LIT (string) = {' ', 's', 't', 'r', 'i', 'n', 'g', 0 };
LIT (firstString) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstSecondString) = {
    'F', 'i', 'r', 's', 't', 'S', 'e', 'c', 'o', 'n', 'd', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstStringSecond) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g',
    'S', 'e', 'c', 'o', 'n', 'd', 0
};
LIT (firFirstString) = {
    'F', 'i', 'r', 'F', 'i', 'r', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstFirstStString) = {
    'F', 'i', 'r', 's', 't', 'F', 'i', 'r', 's', 't', ' ', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstStndring) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'n', 'd', 'r', 'i', 'n', 'g', 0
};
LIT (firstnString) = {
    'F', 'i', 'r', 's', 't', 'n', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (firstStrings) = {
    'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 's',  0
};
LIT (sFirstString) = {
    's', 'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (secondFirstString) = {
    'S', 'e', 'c', 'o', 'n', 'd', 'F', 'i', 'r', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (second) = { 'S', 'e', 'c', 'o', 'n', 'd', 0 };
LIT (coFirstString) = {
    'c', 'o', 'F', 'i', 'r', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (testString) = {
    't', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g', 0
};
LIT (longTest) = {
    'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ',
    'v', 'e', 'r', 'y', ' ',  'l', 'o', 'n', 'g', ' ',
    't', 'e', 's', 't', ' ',
    's', 't', 'r', 'i', 'n', 'g', '.', 0
};

/**************************************************************************/

template <class charT>
void test_insert_into_self (charT, const char *cname)
{
    typedef std::char_traits<charT> Traits;
    typedef std::basic_string<charT, Traits, std::allocator<charT> > String;

    const charT ab[] = { 'a', 'b', '\0' };
    const charT result[] = { 'b', 'a', 'a', 'b', 'b', '\0' };

    rw_info (0, 0, 0,
             "template <class InputIterator> "
             "std::basic_string<%s>::insert(iterator, "
             "InputIterator, InputIterator): insertion into self",
             cname);

    {    // exercise PR #23293

        rw_info (0, 0, 0,
                 "std::basic_string<%s>::insert(iterator, "
                 "iterator, iterator)", cname);

        String s (ab);

        // insert string into the middle of itself
        s.insert (s.begin () + 1, s.begin (), s.end ());

        rw_assert (!Traits::compare (s.data (), result + 1, s.size ()),
                   0, __LINE__,
                   "basic_string<%s>::insert (iterator, "
                   "iterator, iterator)", cname);

        // insert last char of string to the biginning of itself
        s.insert (s.begin (), s.end () - 1, s.end ());

        rw_assert (!Traits::compare (s.data (), result, s.size ()),
                   0, __LINE__,
                   "basic_string<%s>::insert (iterator, "
                   "iterator, iterator)", cname);
    }

    {
        rw_info (0, 0, 0,
                 "std::basic_string<%s>::insert(iterator, "
                 "const_pointer, const_pointer): insertion into self", cname);

        String s (ab);

        // insert string into the middle of itself
        s.insert (s.begin () + 1, s.data (), s.data () + s.length ());

        rw_assert (!Traits::compare (s.data (), result + 1, s.size ()),
                   0, __LINE__,
                   "basic_string<%s>::insert (iterator, const_pointer, "
                   "const_pointer) expected \"aabb\", got %{*Ac}",
                   cname, sizeof (charT), s.data ());

        // insert last char of string to the biginning of itself
        s.insert (s.begin (),
                  s.data () + s.length () - 1, s.data () + s.length ());

        rw_assert (!Traits::compare (s.data (), result, s.size ()),
                   0, __LINE__,
                   "basic_string<%s>::insert (iterator, const_pointer, "
                   "const_pointer) expected \"baabb\", got %{*Ac}",
                   cname, sizeof (charT), s.data ());
    }

    {
        rw_info (0, 0, 0,
                 "std::basic_string<%s>::insert(size_type, "
                 "const_pointer): insertion into self", cname);

        String s (ab);

        // insert string into the middle of itself
        s.insert (1, s.data ());

        rw_assert (!Traits::compare (s.data (), result + 1, s.size ()),
                   0, __LINE__,
                   "basic_string<%s>::insert (iterator, const_pointer), "
                   "expected \"aabb\", got %{*Ac}",
                   cname, sizeof (charT), s.data ());

        // insert last char of string to the biginning of itself
        s.insert (0, s.data () + s.size () - 1);

        rw_assert (!Traits::compare (s.data (), result, s.size ()),
                   0, __LINE__,
                   "std::basic_string<%s>::insert (iterator, const_pointer), "
                   "expected \"baabb\", got %{*Ac}",
                   cname, sizeof (charT), s.data ());
    }
}

/**************************************************************************/

template <class charT>
void test_insert_size_string (charT, const char *cname)
{
    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    typedef typename String::size_type size_type;

    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(size_type, const basic_string&)",
             cname);

    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        ts1.insert((size_type)0, ts2);

        // Insert whole string at beginning
        rw_assert ((ts1 == Lit<charT>::secondFirstString), 0, __LINE__, "I5");
    }
    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        ts1.insert(5, ts2);

        // Insert whole string in middle
        rw_assert ((ts1 == Lit<charT>::firstSecondString), 0, __LINE__, "I6");
    }
    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        ts1.insert(ts1.length(), ts2);

        // Insert whole string at end
        rw_assert ((ts1 == Lit<charT>::firstStringSecond), 0, __LINE__, "I7");
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        const char *error = "expected exception not thrown";

        String ts (Lit<charT>::testString);
        String nl (Lit<charT>::abc);

        try {
            // 21.3.5.4, p1
            // basic_string<>::insert (size_type pos1,
            //                         const basic_string<>& str)
            ts.insert (ts.length() + 1, nl);
        }
        catch (const std::out_of_range&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::out_of_range, caught unknown";
        }

        // Threw exception when pos1 too large
        rw_assert (0 == error, 0, __LINE__, error);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

template <class charT>
void test_insert_size_string_size_size (charT, const char *cname)
{
    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    typedef typename String::size_type size_type;

    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(size_type, const basic_string&, "
             "size_type, size_type)", cname);

    {
        String ts1, ts2;
        ts1.insert(0, ts2, 0, String::npos);

        // Null string insertion gives proper result
        rw_assert (ts1 == Lit<charT>::null, 0, __LINE__, "I4");
    }

    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        ts1.insert(0, ts2, 0, 0);

        // Insert zero-length of string
        rw_assert ((ts1 == Lit<charT>::firstString), 0, __LINE__, "I8");
    }
    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        ts1.insert(0, ts2, 2, 2);

        // Insertion into middle of string
        rw_assert ((ts1 == Lit<charT>::coFirstString), 0, __LINE__, "I9");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(0, ts1, 0, 3);

        // Insert from same string with overlapping copy
        rw_assert ((ts1 == Lit<charT>::firFirstString), 0, __LINE__, "I10");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(5, ts1, 0, 8);

        // Insert from same string with forward copy
        rw_assert ((ts1 == Lit<charT>::firstFirstStString), 0, __LINE__, "I11");
    }
    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        String rslt = String (ts1).insert(8, ts2, 4,4);

        // Insertion returns proper string
        rw_assert (rslt == Lit<charT>::firstStndring, 0, __LINE__,
                   "std::basic_string<%s>(%{*Ac}).insert (8, "
                   "std::basic_string<%s>(%{*Ac}), 4, 4) "
                   "== %{*Ac}; got %{*Ac}",
                   cname, sizeof (charT), ts1.data (),
                   cname, sizeof (charT), ts2.data (),
                   sizeof (charT), Lit<charT>::firstStndring,
                   sizeof (charT), rslt.data ());
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        // same as obove but for pos2 == basic_string<>::npos
        const char *error = "expected exception not thrown";

        String ts (Lit<charT>::testString);
        String nl (Lit<charT>::abc);

        try {
            // 21.3.5.4, p2
            // basic_string<>::insert (size_type pos1,
            //                         const basic_string<>& str,
            //                         size_type pos2, size_type n)
            // Requires: pos1 <= size () && pos2 <= str.size ()
            // Throws: out_of_range if the above condition is violated
            ts.insert (ts.length (), nl, nl.length() + 1, String::npos);
        }
        catch (const std::out_of_range&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::out_of_range, caught unknown";
        }

        // Threw exception when pos2 too large
        rw_assert (0 == error == true, 0, __LINE__, error);

        // original string unmodified
        rw_assert (ts == Lit<charT>::testString, 0, __LINE__,
                   "object modified after an exception");
    }

    {
        const char *error = "expected exception not thrown";

        String ts (Lit<charT>::testString);
        String nl (Lit<charT>::abc);

        try {
            // 21.3.5.4, p2
            // basic_string::insert (size_type pos1, const basic_string& str,
            //                       size_type pos2, size_type n)
            // Requires: pos1 <= size () && pos2 <= str.size ()
            // Throws: out_of_range if the above condition is violated
            ts.insert (ts.length (), nl, String::npos, String::npos);
        }

        catch (const std::out_of_range&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::out_of_range, caught unknown";
        }

        // Threw exception when pos2 too large
        rw_assert (0 == error == true, 0, __LINE__, error);

        // original string unmodified
        rw_assert (ts == Lit<charT>::testString, 0, __LINE__,
                   "object modified after an exception");
    }

#endif   // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

template <class charT>
void test_insert_size_pointer_size (charT, const char *cname)
{
    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    typedef typename String::size_type size_type;

    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(size_type, const_pointer, "
             "size_type)", cname);

    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(0, Lit<charT>::second, 2);

        // Insertion into middle of string
        rw_assert ((ts1 == String(Lit<charT>::firstString).insert(0, String(Lit<charT>::second, 2))),
                   0, __LINE__, "I20");
    }
    {
        String ts1(Lit<charT>::firstString);
        String ts2(Lit<charT>::second);
        String rslt = ts1.insert(8, Lit<charT>::second, 4);

        // Insertion returns proper string
        rw_assert ((rslt == String(Lit<charT>::firstString).insert(8, String(Lit<charT>::second, 4))),
                   0, __LINE__, "I21");
    }

    {
        String ts1;
        ts1.insert(0, Lit<charT>::null, 0, Lit<charT>::space[0]);

        // Null string insertion gives proper result
        rw_assert (   (ts1 == Lit<charT>::null)
                   && (ts1 == String().insert(0, String(Lit<charT>::null, 0, Lit<charT>::null[0]))),
                   0, __LINE__, "I16");
    }
}

/**************************************************************************/

template <class charT>
void test_insert_size_pointer (charT, const char *cname)
{
    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    typedef typename String::size_type size_type;

    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(size_type, const_pointer)",
             cname);

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        const char *error = "expected exception not thrown";

        String ts(Lit<charT>::testString);
        try {
            ts.insert(ts.length()+1, Lit<charT>::abc);
        }
        catch (const std::out_of_range&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::out_of_range, caught unknown";
        }

        // Threw exception when pos1 too large
        rw_assert (0 == error, 0, __LINE__, error);
    }

    {
        const char *error = "expected exception not thrown";

        String ts(Lit<charT>::testString);
        try {
            ts.insert(ts.length() + 1, Lit<charT>::abc, 4);
        }
        catch (const std::out_of_range&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::out_of_range, caught unknown";
        }

        // Does NOT throw exception when pos2 too large
        rw_assert (0 == error, 0, __LINE__, error);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(0, Lit<charT>::second);

        // Insert whole string at beginning
        rw_assert (   (ts1 == Lit<charT>::secondFirstString)
                   && (ts1 == String(Lit<charT>::firstString).insert(0, String(Lit<charT>::second))), 0, __LINE__, "I17");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(5, Lit<charT>::second);

        // Insert whole string in middle
        rw_assert (   (ts1 == Lit<charT>::firstSecondString)
                   && (ts1 == String(Lit<charT>::firstString).insert(5, String(Lit<charT>::second))), 0, __LINE__, "I18");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(ts1.length(), Lit<charT>::second);

        // Insert whole string at end
        rw_assert (   (ts1 == Lit<charT>::firstStringSecond)
                   && (ts1 == String(Lit<charT>::firstString).insert(12, String(Lit<charT>::second))), 0, __LINE__, "I19");
    }
}

/**************************************************************************/

template <class charT>
void test_insert_size_size_value (charT, const char *cname)
{
    typedef std::char_traits<charT>                     Traits;
    typedef std::allocator<charT>                       Allocator;
    typedef std::basic_string<charT, Traits, Allocator> String;

    typedef typename String::size_type size_type;

    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(size_type, size_type, "
             "value_type)", cname);

#ifndef _RWSTD_NO_EXCEPTIONS

    {
        const char *error = "expected exception not thrown";

        String ts(Lit<charT>::testString);
        try {
            ts.insert(ts.length()+1, 1, Lit<charT>::a[0]);
        }
        catch (const std::out_of_range&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::out_of_range, caught unknown";
        }

        // Threw exception when pos1 too large
        rw_assert (0 == error, 0, __LINE__, error);
    }

    {
        const char *error = "expected exception not thrown";

        String ts1(Lit<charT>::longTest);
        try {
            ts1.insert (size_type (0),
                        ts1.max_size (),
                        Lit<charT>::a[0]);
        }
        catch (const std::length_error&) {
            error = 0;
        }
        catch (...) {
            error = "expected std::length_error, caught unknown";
        }

        // Threw exception when possible length of string exceeded
        rw_assert (0 == error, 0, __LINE__, error);
    }

#endif   // _RWSTD_NO_EXCEPTIONS


    {
        String ts1;
        ts1.insert((size_type)0, (size_type)0, Lit<charT>::a[0]);

        // Null string insertion gives proper result
        rw_assert (   (ts1 == Lit<charT>::null)
                   && (ts1 == String().insert(0, String(0, Lit<charT>::a[0]))),
                   0, 0, "I24");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert((size_type)0, 1, Lit<charT>::s[0]);

        // Insert whole string at beginning
        rw_assert (   (ts1 == Lit<charT>::sFirstString)
                   && (ts1 == String(Lit<charT>::firstString).insert(0, String(1, Lit<charT>::s[0]))),
                   0, __LINE__, "I25");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(5, 1, Lit<charT>::n[0]);

        // Insert whole string in middle
        rw_assert (   (ts1 == Lit<charT>::firstnString)
                   && (ts1 == String(Lit<charT>::firstString).insert(5, String(1, Lit<charT>::n[0]))),
                   0, __LINE__, "I26");
    }
    {
        String ts1(Lit<charT>::firstString);
        ts1.insert(ts1.length(), 1, Lit<charT>::s[0]);

        // Insert whole string at end
        rw_assert (   (ts1 == Lit<charT>::firstStrings)
                   && (ts1 == String(Lit<charT>::firstString).insert(12, String(1, Lit<charT>::s[0]))),
                   0, __LINE__, "I27");
    }
    {
        String ts1(Lit<charT>::firstString);
        String rslt = ts1.insert(8, 4, Lit<charT>::x[0]);

        // Insertion returns proper string
        rw_assert ((rslt == String(Lit<charT>::firstString).insert( 8, String(4, Lit<charT>::x[0]))),
                   0, __LINE__, "I28");
    }
}

/**************************************************************************/

template <class charT>
void test_insert_iterator_value (charT, const char *cname)
{
    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(iterator, value_type)", cname);

    rw_warn (0, 0, __LINE__,
             "std::basic_string<%s>::insert(iterator, value_type) "
             "not exercised", cname);
}

/**************************************************************************/

template <class charT>
void test_insert_iterator_size_value (charT, const char *cname)
{
    rw_info (0, 0, 0,
             "std::basic_string<%s>::insert(iterator, size_type, "
             "value_type)", cname);

    rw_warn (0, 0, __LINE__,
             "std::basic_string<%s>::insert(iterator, size_type, "
             "value_type) not exercised", cname);
}

/**************************************************************************/

template <class charT>
void test_insert_iterator_range (charT, const char *cname)
{
    rw_info (0, 0, 0,
             "template <class InputIterator> "
             "std::basic_string<%s>::insert(iterator, "
             "InputIterator, InputIterator)",
             cname);

    rw_warn (0, 0, __LINE__,
             "template <class InputIterator> "
             "std::basic_string<%s>::insert(iterator, "
             "InputIterator, InputIterator) insufficiently exercised",
             cname);

}

/**************************************************************************/

template <class charT>
void test_insert (charT, const char *cname)
{
    test_insert_size_string (charT (), cname);
    test_insert_size_string_size_size (charT (), cname);
    test_insert_size_pointer_size (charT (), cname);
    test_insert_size_pointer (charT (), cname);
    test_insert_size_size_value (charT (), cname);
    test_insert_iterator_value (charT (), cname);
    test_insert_iterator_size_value (charT (), cname);
    test_insert_iterator_range (charT (), cname);

    test_insert_into_self (charT (), cname);
}

/**************************************************************************/

int run_test (int, char*[])
{
    if (rw_enabled ("char"))
        test_insert (char (), "char");
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        test_insert (wchar_t (), "wchar_t");
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;

}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.string.insert",
                    0 /* no comment */, run_test,
                    0 /* co command line options */);
}
