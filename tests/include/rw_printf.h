/************************************************************************
 *
 * printf.h - declarations of the rw_printf family of functions
 *
 * $Id$
 *
 ************************************************************************
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

#ifndef RW_PRINTF_H_INCLUDED
#define RW_PRINTF_H_INCLUDED

#include <rw_testdefs.h>

struct rw_file;

// the equivalent of stdout and stderr
extern _TEST_EXPORT rw_file* const rw_stdout;
extern _TEST_EXPORT rw_file* const rw_stderr;


/************************************************************************
 * Formatted file output.
 ************************************************************************/

/**
 *  Prints to rw_stdout.
 */
_TEST_EXPORT int
rw_printf (const char*, ...);

/**
 * Prints to a file.
 */
_TEST_EXPORT int
rw_fprintf (rw_file*, const char*, ...);


/************************************************************************
 * Formatted string output.
 ************************************************************************/

/**
 * Prints to a character buffer.
 */
_TEST_EXPORT int
rw_sprintf (char*, const char*, ...);

/**
 * Prints to a character buffer of given size.
 */
_TEST_EXPORT int
rw_snprintf (char*, _RWSTD_SIZE_T, const char*, ...);


/************************************************************************
 * Formatted string output into a dynamically allocated buffer.
 ************************************************************************/

/**
 * Prints to a dynamically allocated character buffer.
 *
 * @param fmt  Format specifier.  See rwtest-fmtspec page for details.
 *
 * @return  On success, returns a pointer to the dynamically allocated
 *          character buffer. Otherwise, returns 0.
 */
_TEST_EXPORT char*
rw_sprintfa (const char* fmt, ...);


/**
 * Prints to a dynamically allocated character buffer.
 *
 * @param buf  A pointer to character buffer where the function should
 *        store its output.
 * @param bufise  The size of the character buffer in bytes.
 * @param fmt  Format specifier.  See rwtest-fmtspec page for details.
 *
 * @return  On success, if the size of the supplied buffer was sufficient
 *          to format all characters including the terminating NUL, returns
 *          buf. Otherwise, if the size of the supplied buffer was not
 *          sufficient, returns a pointer to the newly allocated character
 *          buffer of sufficient size. Returns 0 on failure.
 */
_TEST_EXPORT char*
rw_snprintfa (char *buf, _RWSTD_SIZE_T bufsize, const char* fmt, ...);


/**
 * Prints to a dynamically allocated character buffer of sufficient size.
 * Provided for portability with the BSD and GNU C libraries:
 *
 * http://www.freebsd.org/cgi/man.cgi?query=asprintf
 * http://www.openbsd.org/cgi-bin/man.cgi?query=asprintf
 * http://netbsd.gw.com/cgi-bin/man-cgi?asprintf++NetBSD-current
 * http://www.gnu.org/software/libc/manual/html_node/Dynamic-Output.html
 *
 * @param pbuf  Pointer to a pointer to character set by the caller to
 *        to address of the inital character buffer, or 0 of no such
 *        buffer exists. The function sets the pointer to a the address
 *        of the dynamically allocated character buffer, or leaves it
 *        unchanged if it doesn't allocate any buffer.
 * @param pbufsize  Pointer to a size_t set by the caller to the size of
 *        the inital character buffer. The function sets the value pointed
 *        to by this argument to the size of the dynamically allocated
 *        character buffer, or leaves it unchanged if it doesn't allocate
 *        any buffer.
 * @param fmt  Format specifier.  See rwtest-fmtspec page for details.
 *
 * @return  On success, returns the number of characters formatted into
 *          the buffer, otherwise -1.
 */
_TEST_EXPORT int
rw_asnprintf (char** pbuf, _RWSTD_SIZE_T *pbufsize, const char *fmt, ...);


/**
 * @page rwtest-fmtspec Formated Output Directives and Conversion Specifiers
 *
 * The RWTest library supports all formatted output directives specified
 * by the C99 standard as well as many other directive extensions.  Only
 * deviations, extensions, and other implmementation-defined behavior are
 * described below.  Consult your system and/or compiler documentation
 * for specifications of the standard formatting output directives.
 *
 * Unless specified otherwise, the following caveats apply to all
 * directives and conversions.  Directives that accept a pointer argument
 * will convert a null pointer into the string literal <tt>(null)</tt>.
 *
 * Specifiers within formatting directives are denoted by angle brackets.
 * The specifiers are placeholders whose range of possible values are
 * listed in the description of the directive.
 *
 * @section rwtest-fmtspec-c89 C89 Directives
 *
 * @subsection rwtest-fmtspec-c89-ptr Pointer \%p \%\#p
 *
 * The argument is a pointer to \c void.  The value of the pointer is
 * converted to a sequence of hexadecimal digits.  The number of
 * converted digits depends on the size of the \c void* type.  The
 * "alternate form" expressed by \c \%\#p immediately precedes converted
 * digits with a \c 0x prefix.
 *
 *
 * @section rwtest-fmtspec-c99 C99 Directives
 *
 * No known deviations or implementation-defined behavior.
 *
 *
 * @section rwtest-fmtspec-ext RWTest Extensions
 *
 * @subsection rwtest-fmtspec-ext-bool Boolean \%b
 *
 * The argument is a value convertible to \c bool type.  If the value
 * of the argument is nonzero, the value is converted to the string
 * literal \c true.  Otherwise, the value converts to \c false.
 *
 * @subsection rwtest-fmtspec-ext-funptr Function Pointer \%{f}
 *
 * The argument is a pointer to a function.  The conversion and formatting
 * is performed as specified by the \c \%p directive.
 *
 * @subsection rwtest-fmtspec-ext-memptr Member Pointer \%{M}
 *
 * The argument is a pointer to a composite member.  The conversion and
 * formatting is performed as specified by the \c \%p directive.
 *
 * @subsection rwtest-fmtspec-ext-esc-c Escaped Character \%{c} \%{\#c}
 *
 * The argument is an integer value converted to <tt>unsigned char</tt>
 * type.  For non-printable characters, the resulting output is formatted
 * as the corresponding escape sequence.  (The value for a horizontal
 * tab character for example is formatted as the string literal
 * <tt>\\t</tt> and a zero value may be formatted as the string literal
 * <tt>\\x00</tt>.  In the alternate form, the resulting output is
 * immediately surrounded with a single quotation mark <tt>'</tt>.
 *
 * @subsection rwtest-fmtspec-ext-esc-lc Escaped Wide Character \%{lc} \%{\#lc}
 *
 * Similar to the escaped character directive <tt>%{c}</tt> and quoted
 * escaped character directive <tt>%{\#c}</tt> respectively, except
 * that the argument is a value of <tt>wchar_t</tt> type.
 *
 * @subsection rwtest-fmtspec-ext-s Quoted Character String \%{\#s}
 *
 * Similar to the character string directive <tt>%s</tt> except that the
 * resulting output is surrounded with a double quotation mark <tt>"</tt>.
 *
 * @subsection rwtest-fmtspec-ext-ls Quoted Wide Character String \%{\#ls}
 *
 * Similar to the quoted character string directive <tt>%{\#s}</tt>
 * except that the argument is a pointer to a null-terminate sequence of
 * <tt>wchar_t</tt> type.
 *
 * @subsection rwtest-fmtspec-ext-err Error Code \%m \%{\#m}
 *
 * The argument is an integer value representing a system-dependent error
 * code.  In the first form, the value is interpreted as an argument of
 * the \c strerror() function.  The formatted result is the return value
 * of this function using this parameter value.  In the second form, the
 * formatted result is a character sequence representing the
 * corresponding error name (e.g. \c ENOMEM) if applicable or equivalent
 * numeric sequence in decimal notation otherwise.
 *
 * @subsection rwtest-fmtspec-ext-sig Signal \%K
 *
 * The argument is an integer value representing a system-dependent
 * signal.  The resulting output is a character sequence representing
 * the name of corresponding signal (e.g. \c SIGABRT) if
 * applicable or equivalent numeric sequence in decimal notation
 * otherwise.
 *
 * @subsection rwtest-fmtspec-ext-bits Bitset \%{b}
 *
 * The argument is a pointer to an array of \c char.  Each bit in the
 * array, ordered from MSB of the first element to LSB of the last
 * element is converted into a sequence of '0' or '1' characters.
 *
 * @subsection rwtest-fmtspec-ext-Ai Integer Array \%{Ac} \%{Ao} \%{Ad} \%{Ax}
 *
 * The argument is a pointer to an array of integer type.  The resulting
 * output is formatted as a string of comma-separated integer values.
 * The notation of each integer value is specified by <tt>c</tt> for
 * characters, <<tt>o</tt> for octal, <tt>d</tt> for decimal, or
 * <tt>x</tt> for hexadcimal.  An optional field width specifies the
 * size of elements in the array (defaults to 1).  An optional precision
 * specifies the length of the array.  In the alternate form, the
 * resulting output for octal and hexidecimal integer values are
 * prefixed with string literals <tt>0</tt> and <tt>0x</tt> respectively.
 *
 * @subsection rwtest-fmtspec-ext-I Fixed-Width Integers \%{I<wn>}
 *
 * The argument is a value of an integer type.  The directive requires
 * two specifiers: the <tt>w</tt> specifier is one of 8, 16, 32, or 64
 * specifying the width of the integer value.  The <tt>n</tt> specifier
 * is one of \c d, \c o, \c x, or \c X specifying the notation of the
 * resulting output.
 *
 * @subsection rwtest-fmtspec-ext-Is Stream State \%{Is} \%{\#Is}
 *
 * The argument is a value of type <tt>std::ios_base::iostate</tt>.  In
 * the plain form, the resulting ouput is formatted as a sequence of one
 * or more unqualified names corresponding to the \c iostate enumerators,
 * separated by the OR operator (<tt>|</tt>).  In the extended form, the
 * resulting output is prefixed with the string literal
 * <tt>std::ios_base::</tt> indicating the qualfied namespace.
 *
 * @subsection rwtest-fmtspec-ext-Io Open Mode \%{Io} \%{\#Io}
 *
 * Similar to the <tt>%{Is}</tt> directive except the argument is a
 * value of the <tt>std::ios_base::openmode</tt> enumeration type and
 * names in the resulting output correspond to enumerators of this type.
 *
 * @subsection rwtest-fmtspec-ext-Iw Seek Direction \%{Iw} \%{\#Iw}
 *
 * The argument is a value of type <tt>std::ios_base::seekdir</tt>.  In
 * the plain form, the resulting ouput is formatted as the unqualified
 * name corresponding to one of the \c seekdir enumerators.  In the
 * extended form, the enumerator is qualified with the enclosing
 * namespace.
 *
 * @subsection rwtest-fmtspec-ext-If Format Flag \%{If} \%{\#Iw}
 *
 * Similar to the <tt>%{Is}</tt> directive except the argument is a
 * value of the <tt>std::ios_base::fmtflags</tt> enumeration type and
 * names in the resulting output correspond to enumerators of this type.
 *
 * @subsection rwtest-fmtspec-ext-Lc Locate Category \%{Lc} \%{\#Lc}
 *
 * The argument is an integer type representing a constant defined by
 * the C <tt>&lt;locale.h&gt;</tt> header (identified by an <tt>LC_</tt>
 * prefix) or a <tt>std::locale::category</tt> constant defined by
 * the C++ <tt>&lt;locale&gt;</tt> header.  The resulting output is
 * formatted as a character string representing the name of the C or C++
 * constant enclosed in square brackets (<tt>[</tt>) and <tt>]</tt>).
 * In the alternate form, the formatted output for C++ constants is
 * prefixed with the string literal <tt>std::locale::</tt> indicating
 * the qualified namespace.
 *
 * @subsection rwtest-fmtspec-ext-LC Character Classifiers \%{LC} \%{\#LC}
 *
 * The argument is an integer type representing enumerators of the
 * <tt>std::ctype_base::mask</tt> enumeration.  The resulting output is
 * formatted as a character string representing the name of the
 * enumerator enclosed in square brackets (<tt>[</tt>) and <tt>]</tt>).
 * In the alternate form, the enumerator is prefixed with the string
 * literal <tt>std::ctype_base::</tt> indicating the qualified
 * namespace.
 *
 * @subsection rwtest-fmtspec-ext-pid Process Identifier \%{P}
 *
 * The argument is a value of the portable \c rw_pid_t type.  The
 * resulting output is formatted as a numeric sequence of digits
 * representing the corresponding process identifier.  The \c P
 * specifier may be preceded by optional width, justification, and
 * precision qualifiers.
 * 
 * @subsection rwtest-fmtspec-ext-str String \%{S}
 *
 * The argument is a pointer to an object of the \c std::string class.
 * Otherwise, the pointer \c P is converted by calling the \c P->c_str()
 * function and the result is formatted identical to the <tt>%s</tt>
 * character string directive.
 *
 * @subsection rwtest-fmtspec-ext-wstr Wide String \%{lS}
 *
 * The argument is a pointer to an object of the \c std::wstring class.
 * Conversion and formatting is performed as stated for the \c \%{S}
 * directive.
 *
 * @subsection rwtest-fmtspec-ext-tm Time Structure \%{t} \%{\#t}
 *
 * The argument is a pointer to a \c tm structure.  If the pointer is
 * null, the resulting output is the string literal \c (null).  In the
 * plain form, the resulting output for a valid pointer is formatted
 * similar (but not necessarily identical) to the return value of the
 * \c asctime() function.  If the value of any structure member is
 * outside its valid range, the resulting output is formatting using the
 * alternate form.  In the alternate form, the resulting output is
 * formatted as a comma-separated sequence of members enclosed in braces
 * where each member is of the format "name=value [range]".  The
 * "[range]" is only shown if the value is not in the valid range for
 * the respective member.
 *
 * @subsection rwtest-fmtspec-ext-other Other Directives
 *
 * The following directives are also recognized by RWTest output
 * functions but are heretofore undocumented.
 *
 * <ul>
 * <li> \%{<n>} buffer size
 * <li> \%{<n>$} positional parameter
 * <li> \%{$<s>} environment variable
 * <li> \%{?} \%{:} \%{;} conditionals
 * <li> \%{@} nested format
 * <li> \%{!} user-defined format
 * </ul>
 */

#endif   // RW_PRINTF_H_INCLUDED
