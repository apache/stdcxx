/***************************************************************************
 *
 * _config.h - Compiler and C library configuration definitions
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_CONFIG_H_INCLUDED
#define _RWSTD_RW_CONFIG_H_INCLUDED

// include generated configuration header
#include <config.h>

/*** library version numbers and ids **************************************/

#define _RWSTD_VER 0x05000000
//                    | | | |
//                   MMmmuupp
//                    | | | |
//                    | | | +--- pp = patch version number
//                    | | +----- uu = micro version number
//                    | +------- mm = minor version number
//                    +--------- MM = major version number

// library version string (patch number included only if non-zero)
#define _RWSTD_VER_STR   "5.0.0"

// library version numbers
#define _RWSTD_VER_MAJOR ((_RWSTD_VER >> 24) & 0xff)
#define _RWSTD_VER_MINOR ((_RWSTD_VER >> 16) & 0xff)
#define _RWSTD_VER_MICRO ((_RWSTD_VER >>  8) & 0xff)
#define _RWSTD_VER_PATCH (_RWSTD_VER & 0xff)

// a newer release of stdcxx is backward source and binary compatible
// with an older release if both releases share the same major version
// number; stdcxx releases whose major version numbers differ may or
// may not be source or binary compatible

// two releases of stdcxx are forward (as well as backward) source and
// binary compatible if both releases share the same major and minor
// version numbers

// backward compatibility means that a program linked with an older
// release of a library will not be adversely affected by upgrading
// to a more recent release of the same library

// forward compatibility means that a program linked with a newer
// release of a library will not be adversely affected by downgrading
// to an older release of the same library

/**************************************************************************
 * OVERRIDES FOR CONFIGURATION MACROS                                     *
 **************************************************************************/

#if defined (_RWSTD_NO_EXTENSIONS) && !defined (_RWSTD_STRICT_ANSI)
#  define _RWSTD_STRICT_ANSI
#endif   // _RWSTD_NO_EXTENSIONS && !_RWSTD_STRICT_ANSI


// -D _REENTRANT or any one of the other thread safety macros
// used in the conditional below turns on thread safety features
#if    defined (_REENTRANT)             \
    || defined (_THREAD_SAFE)           \
    || defined (_RWSTD_MULTI_THREAD)    \
    || defined (_RWSTD_DCE_THREADS)     \
    || defined (_RWSTD_SOLARIS_THREADS) \
    || defined (_RWSTD_POSIX_THREADS)
#  ifndef _RWSTD_REENTRANT
#    define _RWSTD_REENTRANT
#  endif   // _RWSTD_REENTRANT
#endif

/**************************************************************************
 * ARCHITECTURE-SPECIFIC OVERRIDES                                        *
 **************************************************************************/

/*** Alpha ****************************************************************/

#if defined (__alpha__) || defined (__alpha)

   // this applies to both Tru64 UNIX and Linux on Alpha
#  undef _RWSTD_IS_IEC559

#  ifdef _IEEE_FP
     // IEEE 754/IEC 559 conforming environment enabled
     // using the Compaq C++ -ieee compiler command line
     // option or with gcc -mieee
#    define _RWSTD_IS_IEC559   true
#  else
     // OSF1 (Tru64 UNIX) Compaq C++ without -ieee
#    define _RWSTD_IS_IEC559   false
#  endif

#endif   // Alpha

/*** Itanium (IA64) *******************************************************/

#if defined (__ia64__) || defined (__ia64)
#endif   // IA64

/*** MIPS *****************************************************************/

#if defined (__mips__) || defined (__mips)
#endif   // MIPS

/*** PA-RISC **************************************************************/

#if defined (__parisc__) || defined (__parsisc)
#endif   // PA RISC

/*** PowerPC **************************************************************/

#if defined (__powerpc__) || defined (__powerpc)
#endif   // PowerPC

/*** SPARC ****************************************************************/

#if defined (__sparc__) || defined (__sparc)
#endif   // SPARC

/*** AMD64/Intel EM64T ****************************************************/

#if    defined (__amd64__) || defined (__amd64) \
    || defined (__x86_64__) || defined (__x86_64)

#  if _RWSTD_VER_MAJOR < 5
#    ifdef _RWSTD_OS_LINUX
       // on Linux/AMD64, unless explicitly requested, disable the use
       // of atomic operations in string for binary compatibility with
       // stdcxx 4.1.x
#      ifndef _RWSTD_USE_STRING_ATOMIC_OPS
#        define _RWSTD_NO_STRING_ATOMIC_OPS
#      endif   // _RWSTD_USE_STRING_ATOMIC_OPS
#    endif   // _WIN32
#  endif   // stdcxx < 5.0

#endif   // AMD64/EM64T

/**************************************************************************
 * COMPILER-SPECIFIC OVERRIDES                                            * 
 **************************************************************************/

/*** Apogee C++ ***********************************************************/

// Apogee C++ uses an EDG front end but doesn't define __EDG__

#ifdef __APOGEE__
#endif   // __APOGEE__

/*** IBM VAC++ ************************************************************/

#if defined (__IBMCPP__)
#  include "_config-xlc.h"
#endif   // __IBMCPP___

/*** Compaq C++ ***********************************************************/

// Compaq C++ uses an EDG front end and #defines __EDG__

#ifdef __DECCXX
#  include "_config-deccxx.h"
#endif // __DECCXX

/*** EDG eccp (this is the vanilla EDG front end) *************************/

// NOTE: the __EDG__ macro is #defined by most EDG-based compilers

#if    defined (__EDG__)                \
    && !defined (__DECCXX)              \
    && !defined (__HP_aCC)              \
    && !defined (__INTEL_COMPILER)      \
    && !defined (_SGI_COMPILER_VERSION)
   // FIXME: make this more robust by detecting the EDG eccp demo
   // during library configuration (and avoid relying on compiler
   // specific macros)
#  include "_config-eccp.h"
#endif   // __EDG__

/*** GCC ******************************************************************/

#ifdef __GNUG__
#  include "_config-gcc.h"
#else   // if !defined (__GNUG__)
#  define _RWSTD_GNUC_ATTRIBUTE(ignore)
#endif   // __GNUG__

/*** HP aCC ***************************************************************/

#ifdef __HP_aCC
#  include "_config-acc.h"
#endif   // __HP_aCC

/*** Intel C++ ************************************************************/

#if defined (__INTEL_COMPILER)
#  include "_config-icc.h"
#endif   // __INTEL_COMPILER

/*** SGI MIPSpro **********************************************************/

// SGI MIPSpro uses an EDG front end; it may or may not #define __EDG__
// (starting with MIPSpro 7.4 the preprocessor does #define the macro
// along with _SGI_COMPILER_VERSION)

#ifdef _RWSTD_OS_IRIX64
   // if _SGI_COMPILER_VERSION is not #defined, assume MIPSpro
   // unless __GNUG__ is #defined
#  if defined (_SGI_COMPILER_VERSION) || !defined (__GNUG__)
#    include "_config-mipspro.h"
#  endif   // _SGI_COMPILER_VERSION || !__GNUG__
#endif   // _RWSTD_OS_IRIX64

/*** MSVC *****************************************************************/

#if defined (_MSC_VER) && !defined (__INTEL_COMPILER)
#  define _RWSTD_MSVC _MSC_VER
#  include "_config-msvc.h"
#endif   // _MSC_VER && !__INTEL_COMPILER

/*** Siemens CDS++ ********************************************************/

#ifdef SNI
#  define _RWSTD_NO_PTR_VALUE_TEMPLATE_OVERLOAD
#  if __STDC__ == 1
#    ifndef _RWSTD_STRICT_ANSI
#      define _RWSTD_STRICT_ANSI
#    endif
#  endif

#  if defined __SNI_THREAD_SUPPORT
#    define _RWSTD_NO_MBRTOWC
#    define _RWSTD_NO_WCRTOMB
#  endif
#endif   // SNI

/*** SunPro aka Sun C++ ***************************************************/

#ifdef __SUNPRO_CC
#  include "_config-sunpro.h"
#endif   // __SUNPRO_CC

/**************************************************************************
 * OPERATING SYSTEM-SPECIFIC OVERRIDES                                    * 
 **************************************************************************/

/*** OS2 ******************************************************************/

#ifdef __OS2__
#  define _RWSTD_NO_STATIC_MUTEX_INIT
#endif   // __OS2__

/*** Win{32,64} ***********************************************************/

#ifdef _WIN32
#  define _RWSTD_NO_STATIC_MUTEX_INIT
#  define _RWSTD_PATH_SEP '\\'
#  ifdef _RWSTD_LIB_SRC
     // Don't use implicit TLS in our library on Windows because of the TLS
     // might not be initialized when our library DLL or the DLL, that
     // linked statically against our library, is loaded explicitly using
     // LoadLibrary() function. (STDCXX-1023)
#    ifndef _RWSTD_NO_TLS
#      define _RWSTD_NO_TLS
#    endif
#    ifdef _RWSTD_THREAD
#      undef _RWSTD_THREAD
#    endif
#  endif   // _RWSTD_LIB_SRC
#endif   // _WIN32

#ifndef _RWSTD_PATH_SEP
#  define _RWSTD_PATH_SEP      '/'
#endif   // _RWSTD_PATH_SEP

/*** Non-10646 platforms **************************************************/

#if !defined (_RWSTD_OS_LINUX) && !defined (_WIN32)
   // Linux glibc and Windows use ISO 10646 (Unicode)
   // as the internal wchar_t encoding in all locales
#  ifndef _RWSTD_NO_ISO_10646_WCHAR_T
#    define _RWSTD_NO_ISO_10646_WCHAR_T
#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
#endif   // !Linux && !Windoze

/********************** Threads *******************************************/

#ifdef _RWSTD_REENTRANT
#  if    !defined (_RWSTD_DCE_THREADS)   \
      && !defined (_RWSTD_DEC_THREADS)   \
      && !defined (_RWSTD_POSIX_THREADS) \
      && !defined (_RWSTD_SOLARIS_THREADS)

   // default to POSIX threads except on Win32 or Win64

#    ifndef _WIN32
#      define _RWSTD_POSIX_THREADS
#    endif   // _WIN32
#  endif   // _RWSTD_*_THREADS
#endif   // _RWSTD_REENTRANT

#ifdef _RWSTD_DCE_THREADS
#  define _RWSTD_NO_STATIC_MUTEX_INIT
#endif   // _RWSTD_DCE_THREADS

/********************** Miscellaneous *************************************/

// g++ cannot inline functions that take a variable number of arguments
// or functions that contain static (local) variables
#if !defined (__GNUG__) || __GNUG__ > 2 || __GNUG_MINOR__ > 96
#  define _INLINE_VARARGS      inline
#  if !defined (__HP_aCC) || __HP_aCC > 012100
     // working around a known aCC 1.21 bug
#    define _INLINE_WITH_STATICS inline
#  endif   // !__HP_aCC || __HP_aCC > 012100
#endif   // !__GNUG__ || __GNUG__ > 2 || __GNUG_MINOR__ > 96

#if    defined (_RWSTD_NO_COLLAPSE_TEMPLATE_STATICS)   \
    || defined (_RWSTD_NO_STATIC_TEMPLATE_MEMBER_INIT)
   // static mutex initialization depends on the compiler's (and the
   // linker's in gcc's case) ability to correctly handle explicitly
   // initialized static members of class templates
#  ifndef _RWSTD_NO_STATIC_MUTEX_INIT
#    define _RWSTD_NO_STATIC_MUTEX_INIT
#  endif
#endif   // NO_COLLAPSE_TEMPLATE_STATICS || NO_STATIC_TEMPLATE_MEMBER_INIT

/********************** Environment ***************************************/

// pa-risc2 atomic ops related
#if defined(_PA_RISC2_0) && defined(__HP_aCC)
#  define _RWSTD_STRING_REF_OFFSET             _RWSTD_INT_MAX
#endif


#if defined (_RWSTD_NO_WCHAR_T) && !defined (_RWSTD_NO_NATIVE_WCHAR_T)
#  define _RWSTD_NO_NATIVE_WCHAR_T
#endif


#ifndef _RWSTD_NO_STL_SPECIALIZATION
// #ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
// disable partial specialization for void* of STL sequences
#  define _RWSTD_NO_STL_SPECIALIZATION
// #endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC
#endif   // _RWSTD_NO_STL_SPECIALIZATION

// 
// Macro for path to the ANSI 'C' headers 
// Must be set specifically for each platform when the
// C++ wrappers for 'C' headers are used.
//
#define _RWSTD_ANSIC(x) </usr/include/x>

// the default long double printf format prefix is "L" (unless #defined above)
#ifndef _RWSTD_LDBL_PRINTF_PREFIX
#  define _RWSTD_LDBL_PRINTF_PREFIX   "L"
#endif   // _RWSTD_LDBL_PRINTF_PREFIX

// the default long double scanf format prefix is "L" (unless #defined above)
#ifndef _RWSTD_LDBL_SCANF_PREFIX
#  define _RWSTD_LDBL_SCANF_PREFIX   "L"
#endif   // _RWSTD_LDBL_SCANF_PREFIX

// the default expansion of dllimport is empty (unless #defined above)
#ifndef _RWSTD_DLLIMPORT
#  define _RWSTD_DLLIMPORT   /* empty */
#endif   // _RWSTD_DLLIMPORT

// the default name of a libc symbol is itself (unless #defined above)
#ifndef _RWSTD_LIBC_SYM
#  define _RWSTD_LIBC_SYM(name)   name
#endif   // _RWSTD_LIBC_SYM

/********************** Library Option Dependencies ***********************/

/*
 * The following macro sets the default size of file stream internal buffers
 */

#ifndef _RWSTD_DEFAULT_BUFSIZE         // specified on command line?
#  define _RWSTD_DEFAULT_BUFSIZE   512
#endif

#ifndef _RWSTD_NO_LONG_LONG
   // Win32/64 #defines _RWSTD_LONG_LONG to __int64
#  ifndef _RWSTD_LONG_LONG
#    define _RWSTD_LONG_LONG long long
#  endif   // _RWSTD_LONG_LONG
#endif   // _RWSTD_NO_LONG_LONG


// disable all extensions in strict ANSI mode
#ifdef _RWSTD_STRICT_ANSI
   // long long is not in ANSI C++ yet (although it is in ANSI C99)
#  undef  _RWSTD_LONG_LONG
#  define _RWSTD_NO_EXT_FILEBUF
#  define _RWSTD_NO_EXT_VECTOR_BOOL_REF_OPS
#  define _RWSTD_NO_EXT_LOCALE
#  define _RWSTD_NO_EXT_DEEP_STRING_COPY
   // no support for exceptions derived from ios_base::failure
#  define _RWSTD_NO_EXT_FAILURE
   // no support for writing out integral values in base 2
#  define _RWSTD_NO_EXT_BIN_IO
   // disable safe conversion of iostream objects only to bool
   // (and enable conversion to void*)
#  define _RWSTD_NO_EXT_IOS_SAFE_CONVERSION
   // std::setbase manipulator accepts only required bases (i.e.,
   // 0, 8, 10, 16, and 2 unless _RWSTD_NO_EXT_BIN_IO is also #defined)
#  define _RWSTD_NO_EXT_SETBASE
   // disable the str(const char*) overload in stringbuf and stringstreams
#  define _RWSTD_NO_EXT_STRINGBUF_STR
   // no support for member overloads on all fundamental types
#  define _RWSTD_NO_EXT_NUM_GET
#  define _RWSTD_NO_EXT_NUM_PUT
   // no support for extended member overloads
#  define _RWSTD_NO_EXT_TIME_GET
   // no support for optional mt-locking in iostreams
#  define _RWSTD_NO_EXT_REENTRANT_IO
   // allocator<const T> not possible
#  define _RWSTD_NO_EXT_CONST_ALLOCATOR
   // missing operator new or delete not defined
#  define _RWSTD_NO_EXT_OPERATOR_NEW
   // void std::count<>() not defined (except when necessary)
#  define _RWSTD_NO_EXT_VOID_COUNT
   // void std::distance() not defined
#  define _RWSTD_NO_EXT_VOID_DISTANCE

   // calling width(0) only when insertion succeeds
#  define _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE

   // deque::assign() clears container before assignment
   // rather than assigning over existing elements
#  define _RWSTD_NO_EXT_DEQUE_ASSIGN_IN_PLACE
   // deque::insert() creates a copy of the container
   // to insert into and on success swaps the copy
#  define _RWSTD_NO_EXT_DEQUE_INSERT_IN_PLACE

   // primary templates not defined
#  define _RWSTD_NO_EXT_CHAR_TRAITS_PRIMARY
#  define _RWSTD_NO_EXT_CODECVT_PRIMARY
#  define _RWSTD_NO_EXT_COLLATE_PRIMARY
#  define _RWSTD_NO_EXT_CTYPE_PRIMARY
   // random number generator used by algorithms such as random_shuffle
   // utilizes the entire range of size_t and will not generate the same
   // sequence of values between ILP32 and LP64 (or LLP64)
#  define _RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE

   // disable the implementation of LWG issue 559 (numeric_limits
   // limits specialized on cv-qualified scalar types)
#  define _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS

// commented out macros not implemented yet
// #  define _RWSTD_NO_EXT_MONEYPUNCT_PRIMARY
// #  define _RWSTD_NO_EXT_NUMPUNCT_PRIMARY
// #  define _RWSTD_NO_EXT_TIME_GET_PRIMARY
// #  define _RWSTD_NO_EXT_TIME_PUT_PRIMARY
// #  define _RWSTD_NO_EXT_MESSAGES_PRIMARY

#endif   // _RWSTD_STRICT_ANSI

   // macros that are enabled in library source files
#ifdef _RWSTD_LIB_SRC
   // don't instantiate templates in library source code
   //  (but see _RWSTD_INSTANTIATE_TEMPLATES macro below) 
#  define _RWSTD_NO_TEMPLATE_DEFINITIONS
   // don't provide definitions of operator new in library 
#  define _RWSTD_NO_EXT_OPERATOR_NEW
#endif  // _RWSTD_LIB_SRC

#if _MSC_VER <= 1300
   // msvc60 expects a definition to be provided for all variants
   // of operator new/delete that are declared. This means that either
   // the operators must be defined - preventing redefinition in user code -
   // or that they must be undeclared - preventing them from being called
   // directly in user code. We have chosen the former option.
#  undef _RWSTD_NO_EXT_OPERATOR_NEW
#endif  // _MSC_VER

#ifdef _RWSTD_INSTANTIATE_TEMPLATES
   // instantiate templates (this macro is defined at the top of each
   // library source file (.cpp file) that explicitly instantiates class
   // or function templates using the _RWSTD_INSTANTIATE_N() macros
#  undef _RWSTD_NO_TEMPLATE_DEFINITIONS
#endif

#endif   // _RWSTD_RW_CONFIG_H_INCLUDED
