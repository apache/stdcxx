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

#ifndef _RWSTD_CONFIG_H_INCLUDED
#define _RWSTD_CONFIG_H_INCLUDED

#ifdef _RWSTD_USE_CONFIG
#  include <config.h>
#else
#  include <rw/config/rwconfig_std.h>
#endif   // _RWSTD_USE_CONFIG

/*** library version numbers and ids **************************************/

#define _RWSTD_VER 0x04010200
//                    | | | |
//                   MMmmuupp
//                    | | | |
//                    | | | +--- pp = patch release number
//                    | | +----- uu = micro release number
//                    | +------- mm = minor release number
//                    +--------- MM = major release number

// library version string (patch number included only if non-zero)
#define _RWSTD_VER_STR   "4.1.2"

// library version numbers
#define _RWSTD_VER_MAJOR ((_RWSTD_VER >> 24) & 0xff)
#define _RWSTD_VER_MINOR ((_RWSTD_VER >> 16) & 0xff)
#define _RWSTD_VER_MICRO ((_RWSTD_VER >>  8) & 0xff)
#define _RWSTD_VER_PATCH (_RWSTD_VER & 0xff)

// Two libraries are Source Compatible if they have the same major
// release number. Libraries whose major release numbers differ may
// or may not be source compatible.
// Two libraries are Binary Compatible (symbol-wise and functionally)
// if have the same major and minor release numbers. Libraries whose
// major and minor release numbers differ may or may not be binary
// compatible.

/*********************************************************************
 * OVERRIDES FOR CONFIGURATION MACROS                                *
 *********************************************************************/

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

/*********************************************************************
 * ARCHITECTURE-SPECIFIC OVERRIDES                                   *
 *********************************************************************/

/*** Alpha ***********************************************************/

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

/*** Itanium (IA64) **************************************************/

#if defined (__ia64__) || defined (__ia64)
#endif   // IA64

/*** MIPS ************************************************************/

#if defined (__mips__) || defined (__mips)
#endif   // MIPS

/*** PA-RISC *********************************************************/

#if defined (__parisc__) || defined (__parsisc)
#endif   // PA RISC

/*** PowerPC *********************************************************/

#if defined (__powerpc__) || defined (__powerpc)
#endif   // PowerPC

/*** SPARC **********************************************************/

#if defined (__sparc__) || defined (__sparc)
#endif   // SPARC

/*********************************************************************
 * COMPILER-SPECIFIC OVERRIDES                                       *
 *********************************************************************/

/*** Apogee C++ ******************************************************/

// Apogee C++ uses an EDG front end but doesn't define __EDG__

#ifdef __APOGEE__
#endif   // __APOGEE__

/*** IBM VAC++ *******************************************************/

#if defined (__IBMCPP__)

   // _THREAD_SAFE defined by the xlC_r compiler driver

#  ifndef _RWSTD_NO_PURE_C_HEADERS
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  if defined _RWSTD_NO_NEW_HEADER
#    undef _RWSTD_NO_NEW_HEADER
#  endif   // _RWSTD_NO_NEW_HEADER

// our <cxxx> libc headers put the libc functions in namespace std
#  ifdef _RWSTD_NO_LIBC_IN_STD
#    undef _RWSTD_NO_LIBC_IN_STD
#  endif   // _RWSTD_NO_LIBC_IN_STD

#  ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#    define _RWSTD_NO_DEPRECATED_C_HEADERS
#  endif

#  ifdef _RWSTD_USE_CONFIG
#    undef _RWSTD_ANSI_C_ASSERT_H
#    undef _RWSTD_ANSI_C_CTYPE_H
#    undef _RWSTD_ANSI_C_ERRNO_H
#    undef _RWSTD_ANSI_C_FLOAT_H
#    undef _RWSTD_ANSI_C_ISO646_H
#    undef _RWSTD_ANSI_C_LIMITS_H
#    undef _RWSTD_ANSI_C_LOCALE_H
#    undef _RWSTD_ANSI_C_MATH_H
#    undef _RWSTD_ANSI_C_SETJMP_H
#    undef _RWSTD_ANSI_C_SIGNAL_H
#    undef _RWSTD_ANSI_C_STDARG_H
#    undef _RWSTD_ANSI_C_STDDEF_H
#    undef _RWSTD_ANSI_C_STDIO_H
#    undef _RWSTD_ANSI_C_STDLIB_H
#    undef _RWSTD_ANSI_C_STRING_H
#    undef _RWSTD_ANSI_C_TIME_H
#    undef _RWSTD_ANSI_C_WCHAR_H
#    undef _RWSTD_ANSI_C_WCTYPE_H
#  endif   // _RWSTD_USE_CONFIG

   // static data members of cass templates aren't properly collapsed
   // (compiler emits one copy per each translation unit that contains
   // the definition of the template member)
#  define _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
   // the above prevents correct static mutex initialization
   // (but the alternative approach is also unreliable)
#  define _RWSTD_NO_STATIC_MUTEX_INIT

   // VisualAge for C++ version >= 5.0.2.0, when the tempinc model
   // (which makes use of implicit inclusion) is in effect (i.e.,
   // when neither of -qtemplateregistry or -qnotempinc is specified
   // on the command line) the compiler defines the macro __TEMPINC__
#  if __IBMCPP__ < 502 || !defined (__TEMPINC__)
     // override the autodetected setting of the macro
#    undef  _RWSTD_NO_IMPLICIT_INCLUSION
#    define _RWSTD_NO_IMPLICIT_INCLUSION
#  endif

#  if __IBMCPP__ >=  502 && defined (__TEMPINC__)
     // override the autodetected setting of the macro
#    undef _RWSTD_NO_IMPLICIT_INCLUSION
#  endif   // VAC++ >= 5.0.2 && __TEMPINC__

#  if !defined (_RWSTD_WCTRANS_T)
#    undef _RWSTD_NO_WCTRANS_T
#    define _RWSTD_WCTRANS_T  win_t(*wctrans_t)(wint_t)
#  endif // _RWSTD_WCTRANS_T

#  ifdef _RWSTD_OS_AIX
     // operator new and delete is not reliably replaceable across
     // shared library boundaries, which includes the shared library
     // version of the language support library
#    define _RWSTD_NO_REPLACEABLE_NEW_DELETE

#    if (8 == _RWSTD_LONG_SIZE)
       // avoid using strtof() in LP64 due to bug #379 (PMR 02164)
#      ifndef _RWSTD_NO_STRTOF
#        define _RWSTD_NO_STRTOF
#      endif   // _RWSTD_NO_STRTOF
#      ifndef _RWSTD_NO_STRTOF_IN_LIBC
#        define _RWSTD_NO_STRTOF_IN_LIBC
#      endif   // _RWSTD_NO_STRTOF_IN_LIBC
#    endif   // REENTRANT && LP64
#  endif   // AIX

#  ifndef __TEMPINC__
     // avoid VAC++ 7.0 diagnostic 1540-2910 (I) The template uses
     // a file organization for tempinc, but tempinc is not being
     // used. See bug #450

     // prevent warnings about VAC++ being unable to find
     // source for (intentionally undefined) member functions
     // of class templates
#    define _RWSTD_NO_UNDEFINED_TEMPLATES
#  endif   // __TEMPINC__
#endif   // __IBMCPP___

/*** Compaq C++ ******************************************************/

// Compaq C++ uses an EDG front end and #defines __EDG__

#ifdef __DECCXX

   // _REENTRANT is defined by -pthread option

#  if defined (_RWSTD_REENTRANT)
#    if defined (_PTHREAD_USE_D4) && !defined (_RWSTD_DCE_THREADS)
#      define _RWSTD_DCE_THREADS
#    endif   // _PTHREAD_USE_D4 && !_RWSTD_DCE_THREADS

#    if !defined (_RWSTD_DCE_THREADS) && !defined (_RWSTD_POSIX_THREADS)
#      define _RWSTD_POSIX_THREADS
#    endif   // !(_RWSTD_DCE_THREADS) && !(_RWSTD_POSIX_THREADS)
#  endif   // (_RWSTD_REENTRANT)

#  if __DECCXX_VER < 60390008
     // version of Compaq C++ prior to C++ do not implement
     // C++ C library headers (the <cxxx> kind)
#    define _RWSTD_NO_NEW_HEADER
#  endif   // __DECCXX_VER < 60390008

#endif // __DECCXX

/*** EDG eccp ********************************************************/

// NOTE: the __EDG__ macro is #defined by most edg-based compilers

#if    defined (__EDG__)                \
    && !defined (__DECCXX)              \
    && !defined (__INTEL_COMPILER)      \
    && !defined (_SGI_COMPILER_VERSION)

#  if defined (_RWSTD_REENTRANT) && !defined (_RWSTD_POSIX_THREADS)
#    define _RWSTD_POSIX_THREADS
#  endif // (_RWSTD_REENTRANT) && !(_RWSTD_POSIX_THREADS)

#  undef _RWSTD_NO_NEW_HEADER
#  undef _RWSTD_NO_LIBC_IN_STD

#  ifndef _RWSTD_NO_EXPORT
#    undef _RWSTD_NO_IMPLICIT_INCLUSION
#  endif

   // the vanilla front-end doesn't actually #include the header,
   // but it treats it specially instead, which throws off the test
#  undef _RWSTD_ANSI_C_STDARG_H 
#  define _RWSTD_ANSI_C_STDARG_H <stdarg.h>

#  ifdef _RWSTD_REENTRANT
     // std::uncaught_exception() not thread-safe in the front-end demo
#    define _RWSTD_NO_UNCAUGHT_EXCEPTION
#  endif   // _RWSTD_REENTRANT

#  ifndef _RWSTD_NO_MBSTATE_T
#    define _RWSTD_NO_MBSTATE_T
#  endif   // _RWSTD_NO_MBSTATE_T

#endif   // __EDG__

/*** GCC *************************************************************/

#ifdef __GNUG__

   // _REENTRANT is defined by the -pthread compiler option

#  ifndef _RWSTD_NO_IMPLICIT_INCLUSION
#    define _RWSTD_NO_IMPLICIT_INCLUSION
#  endif

#  ifndef _RWSTD_NO_EXPORT
     // not implemented
#    define _RWSTD_NO_EXPORT
#  endif

#  if !defined (_RWSTD_USE_PURE_C_HEADERS)
     // disabled except when requested
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // !_RWSTD_USE_PURE_C_HEADERS

#  ifdef _RWSTD_NO_PURE_C_HEADERS
     // deprecated C headers (the <xxx.h> kind) are fully implemented
     // in terms of the pure C++ C library headers (the <cxxx> kind)
#    define _RWSTD_NO_DEPRECATED_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  if !defined (_RWSTD_STRICT_ANSI) && __GNUG__ <= 2 && __GNUC_MINOR__ < 97
     // prevent gcc 2.9x ICE
#    define _RWSTD_NO_PLACEMENT_DELETE
#  endif   // !_RWSTD_STRICT_ANSI

#  if __GNUG__ <= 2 && __GNUC_MINOR__ < 97 && defined (_RWSTD_NO_HONOR_STD)
     // standard iostream objects are declared as objects of their respective
     // types by defined as POD's to prevent their destruction during program
     // lifetime (done to work around a g++ 2.95.2 bug that prevents g++
     // from deducing the type of references to the objects in template code)
#    define _RWSTD_NO_IOSTREAM_OBJECT_REFS
#  endif   //__GNUG__ <= 2 && __GNUC_MINOR__ < 97 && _RWSTD_NO_HONOR_STD

#  define _RWSTD_GNUC_ATTRIBUTE(attr)   __attribute__ (attr)

#  ifdef _RWSTD_OS_LINUX

#    ifdef _RWSTD_NO_NEW_HEADER
#      undef _RWSTD_NO_NEW_HEADER
#    endif   // _RWSTD_NO_NEW_HEADER

     // our <cxxx> libc headers put the libc functions in namespace std
#    ifdef _RWSTD_NO_LIBC_IN_STD
#      undef _RWSTD_NO_LIBC_IN_STD
#    endif   // _RWSTD_NO_LIBC_IN_STD

#    ifdef _RWSTD_ANSI_C_LIMITS_H
       // prevent system header warnings
#      undef _RWSTD_ANSI_C_LIMITS_H
#      define _RWSTD_ANSI_C_LIMITS_H <../include/limits.h>
#    endif  // _RWSTD_ANSI_C_LIMITS_H
#  endif   // _RWSTD_OS_LINUX

#  ifdef _RWSTD_OS_AIX
     // functions called from member functions of explicitly instantiated
     // class templates aren't "implicitly" instantiated (compiler bug)
#    define _RWSTD_NO_IMPLICIT_INSTANTIATION
#  endif

   // force using /usr/include/math.h 
   // prevent recursion caused by pulling in gcc's own "fixed" header
#  undef _RWSTD_ANSI_C_MATH_H

   // use new C++ libc headers
#  undef _RWSTD_NO_NEW_HEADER

#  ifdef _RWSTD_USE_CONFIG
#    undef _RWSTD_NO_LIBC_IN_STD
#  endif

#  undef _RWSTD_NO_DEPRECATED_LIBC_IN_STD

#  ifdef __CYGWIN__
     // use our own C++ libc headers
#    undef _RWSTD_NO_NEW_HEADER
     // disable long long handling (no libc support yet)
#    define _RWSTD_NO_LONG_LONG
     // libc is wrapped in namespaces std
#    undef _RWSTD_NO_LIBC_IN_STD
     // deprecated C++ libc headers don't introduce names into namespace std
#    ifndef _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#      define _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#    endif
#  endif   // __CYGWIN__

#  ifdef _RWSTD_OS_OSF1
      // sizeof (long double) == sizeof (double), 'L' causes SIGSEGV
#     define _RWSTD_LDBL_PRINTF_PREFIX   ""
#     define _RWSTD_LDBL_SCANF_PREFIX    "l"

#  endif   // _RWSTD_OS_OSF1

#  ifdef _RWSTD_OS_SUNOS

     // _SOLARIS_THREADS #defined when the -threads option is used on SunOS
#    if defined (_SOLARIS_THREADS) && !defined (_RWSTD_SOLARIS_THREADS)
#      define _RWSTD_SOLARIS_THREADS
#    endif // _SOLARIS_THREADS && !_RWSTD_SOLARIS_THREADS

     // _PTHREADS #defined when the -pthreads option is used on SunOS
#    if defined (_PTHREADS) && !defined (_RWSTD_POSIX_THREADS)
#      define _RWSTD_POSIX_THREADS
#    endif // _PTHREADS && !_RWSTD_POSIX_THREADS

#    if __GNUG__ == 3 && __GNUC_MINOR__ < 1
       // Avoid gcc 3.0.1 header configuration confusion with wchar
#      ifndef _RWSTD_NO_NEW_HEADER
#        define _RWSTD_NO_NEW_HEADER
#      endif
#      ifndef _RWSTD_NO_LIBC_IN_STD
#        define _RWSTD_NO_LIBC_IN_STD
#      endif
#    endif  // __GNUG__ == 3 && __GNUC_MINOR__ < 1
#  endif  // _RWSTD_OS_SUNOS

#else   // if !defined (__GNUG__)
#  define _RWSTD_GNUC_ATTRIBUTE(ignore)
#endif   // __GNUG__

/*** HP aCC **********************************************************/

#ifdef __HP_aCC

   // _REENTRANT is defined by the -mt compiler option

   // accommodate aCC 1.xx (HP-UX 10.xx), aCC 3.xx (HP-UX 11/PA-RISC)
   // and aCC 5.xx (HP-UX 11/IA64)
#  define _RWSTD_HP_aCC_MAJOR   (__HP_aCC / 10000)
#  define _RWSTD_HP_aCC_MINOR   (__HP_aCC % 10000)

   // new C++ libc headers supported
#  undef _RWSTD_NO_NEW_HEADER

#  ifndef _RWSTD_NO_PURE_C_HEADERS
     // disable for now
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

   // deprecated C++ libc headers don't introduce names into namespace std
#  ifndef _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#    define _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#  endif

   // deprecated C++ libc headers aren't used
#  define _RWSTD_NO_DEPRECATED_C_HEADERS

   // prevents Error (future) 553: Expected type arguments to follow
   //                              class template name.
#  define _RWSTD_REDUNDANT_TMPL_PARAMS

#  if (_RWSTD_HP_aCC_MINOR < 3300) && !defined (_RWSTD_NO_MBSTATE_T)
   // mbstate_t definition is guarded in _mbstate_t.h and is available
   // only if _INCLUDE__STDC_A1_SOURCE is defined (i.e., with -AA)
#    define _RWSTD_NO_MBSTATE_T
#  endif

#  if defined (_HP_NAMESPACE_STD) || defined (_NAMESPACE_STD)
#    undef _RWSTD_NO_RUNTIME_IN_STD
#    undef _RWSTD_NO_STD_BAD_TYPEID
#    undef _RWSTD_NO_STD_TYPE_INFO
#  endif

#  ifndef _RWSTD_NO_BAD_ALLOC_WHAT
     // avoid the following ld errors:
     // Unsatisfied symbol "Class tables [Vtable] dependent on
     // key function: "bad_alloc::what() const"" in file [*.o]
     // Unsatisfied symbol "typeid<bad_alloc>" in file [*.o]
#    define _RWSTD_NO_BAD_ALLOC_WHAT
#  endif   // _RWSTD_NO_BAD_ALLOC_WHAT

#  if __HP_aCC < 60000
#    ifndef _RWSTD_NO_EXTERN_TEMPLATE
#      ifndef _RWSTD_NO_STRING_NPOS_TYPE
         // define the type of basic_string::npos to be size_t
         // instead of basic_string::size_type to work around
         // an HP aCC extern template bug #333 (only the HP
         // front end has this bug; HP aCC 6.0 which uses the
         // EDG front end works fine
#        define _RWSTD_NO_STRING_NPOS_TYPE
#      endif   // _RWSTD_NO_STRING_NPOS_TYPE
#    endif   // _RWSTD_NO_EXTERN_TEMPLATE
#  endif   // aCC < 6.0
#endif   // __HP_aCC

/*** Intel C++ *******************************************************/

#if defined (__INTEL_COMPILER)

#  if !defined (_WIN32) && !defined (_WIN64)

     // enable the <cxxx> form of libc headers
     // we can only use the rw version in include/ansi
#    ifndef _RWSTD_NO_PURE_C_HEADERS
#      define _RWSTD_NO_PURE_C_HEADERS
#    endif   // _RWSTD_NO_PURE_C_HEADERS

#    ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#      define _RWSTD_NO_DEPRECATED_C_HEADERS
#    endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

#    undef _RWSTD_NO_NEW_HEADER
#    undef _RWSTD_NO_LIBC_IN_STD

#  else   // if defined (_WIN{32,64}
#    if defined (_RWSHARED)
#      // only when using shared libstd and Intel C++/MSVC
       // working around an Intel C++ 7.1 bug (PR #29178)
#      define _RWSTD_NO_OUTLINED_USE_FACET_SPECIALIZATIONS
#    endif   // _MSC_VER > 1200 && _RWSHARED
#  endif

#endif   // __INTEL_COMPILER

/*** SGI MIPSpro *****************************************************/

// SGI MIPSpro uses an EDG front end; it may or may not #define __EDG__
// (starting with MIPSpro 7.4 the preprocessor does #define the macro
// along with _SGI_COMPILER_VERSION)

#ifdef _RWSTD_OS_IRIX64

   // if _SGI_COMPILER_VERSION is not #defined, assume MIPSpro
   // unless __GNUG__ is #defined
#  if defined (_SGI_COMPILER_VERSION) || !defined (__GNUG__)

#  ifndef _RWSTD_NO_PURE_C_HEADERS
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  if defined _RWSTD_NO_NEW_HEADER
#    undef _RWSTD_NO_NEW_HEADER
#  endif   // _RWSTD_NO_NEW_HEADER

// our <cxxx> libc headers put the libc functions in namespace std
#  ifdef _RWSTD_NO_LIBC_IN_STD
#    undef _RWSTD_NO_LIBC_IN_STD
#  endif   // _RWSTD_NO_LIBC_IN_STD

#  ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#    define _RWSTD_NO_DEPRECATED_C_HEADERS
#  endif

#    define _RWSTD_NO_PLACEMENT_DELETE
#    //  The comptest sets compile instantiate when it shouldn't.
#    //  override it here.
#    if defined (_RWSTD_NO_IMPLICIT_INCLUSION)
#      undef _RWSTD_NO_IMPLICIT_INCLUSION
#    endif
#  endif

#endif

/*** MSVC (and Intel C++/Win{32,64}) ********************************/

#ifdef _MSC_VER

#  if _MSC_VER <= 1310   // MSVC <= 7.1
#  endif   // MSVC <= 7.1

#  if _MSC_VER <= 1300   // MSVC <= 7.0
#    define _RWSTD_NO_TYPENAME
#    define _RWSTD_EXCEPTION_HANDLER_IN_STD
#    define _RWSTD_NO_STATIC_DEF3
#    define _RWSTD_NO_PTR_VALUE_TEMPLATE_OVERLOAD
#    define _RWSTD_NO_PRAGMA_INSTANTIATE
     // disable explicit instantiation extensions if building
     // optimized client code (all templates still explicitly
     // instantiated in the library); 
     // *** Not currently done for MSVC 7, since an implicit
     //     instantiation bug causes multiply defined symbols
     //     in downstream libraries
#    ifndef _RWSTD_NO_STRING_NPOS_TYPE
       // define basic_string::npos to be of type size_t
       // instead of basic_string::size_type to work around
       // an MSVC 7.0 bug (PR #26549)
#      define _RWSTD_NO_STRING_NPOS_TYPE
#    endif   // _RWSTD_NO_STRING_NPOS_TYPE
#  endif   // MSVC <= 7.0

#  if _MSC_VER <= 1200   // MSVC <= 6.0
#  endif   // MSVC <= 6.0

   // enable the <cxxx> form of libc headers
   // we can only use the rw version in include/ansi
#  ifndef _RWSTD_NO_PURE_C_HEADERS
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  if defined _RWSTD_NO_NEW_HEADER
#    undef _RWSTD_NO_NEW_HEADER
#  endif   // _RWSTD_NO_NEW_HEADER

   // our <cxxx> libc headers put the libc functions in namespace std
#  ifdef _RWSTD_NO_LIBC_IN_STD
#    undef _RWSTD_NO_LIBC_IN_STD
#  endif   // _RWSTD_NO_LIBC_IN_STD

#  ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#    define _RWSTD_NO_DEPRECATED_C_HEADERS
#  endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

   // static data members of cass templates aren't properly collapsed
   // (compiler emits one copy per each translation unit that contains
   // the definition of the template member, and the linker fails to
   // multiple copies that reside in shared libraries)
#  ifndef _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
#    define _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
#  endif   // _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS

   // operator new and delete is not reliably replaceable across
   // shared library boundaries, which includes the shared library
   // version of the language support library
#  define _RWSTD_NO_REPLACEABLE_NEW_DELETE

   // names of (most) extern "C" libc symbols begin with an underscore
#  define _RWSTD_LIBC_SYM(name)   _ ## name

#  ifdef _DLL
#    define _RWSTD_DLLIMPORT   __declspec (dllimport)
#  endif   // _DLL

#  ifndef _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
     // both MSVC 7.x and Intel C++/Windows allow "inline" initializers
     // for static const integral data members but out-of-line definitions
     // cause multiply defined symbol errors (see PR #26562 and #30260)
     // disable their definitions in source files (the members are still
     // declared)
#    define _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
#  endif   // _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION

   // enable iostream and locale support for long long integers
#  define _RWSTD_LONG_LONG __int64

   // not declared/defined in libc
#  if !defined (_RWSTD_NO_STRTOLL_IN_LIBC)
#    define _RWSTD_NO_STRTOLL_IN_LIBC
#  endif   // _RWSTD_NO_STRTOLL_IN_LIBC

#  if !defined (_RWSTD_NO_STRTOULL_IN_LIBC)
#    define _RWSTD_NO_STRTOULL_IN_LIBC
#  endif   // _RWSTD_NO_STRTOULL_IN_LIBC

#  if defined (_WIN64)
     // FIXME: handle by forward declaring fuctions in <rw/_mutex.h>
#    define _RWSTD_NO_FWD_DECLARATIONS
#    if !defined (_RWSTD_NO_SSIZE_TYPE)
#      define _RWSTD_NO_SSIZE_TYPE
#    endif   // _RWSTD_NO_SSIZE_TYPE
#  endif   // _WIN64

   // disable "Same type qualifier used more than once"
#  pragma warning (disable: 4114)

   // disable "return type for operator->' is not a UDT"
#  pragma warning (disable: 4284)

   // disable "decorated name length exceeded"
#  pragma warning (disable: 4503)

   // disable "identifier was truncated to 255 characters"
#  pragma warning (disable: 4786)

#  if defined (WIN32) && !defined(_WIN32)
#    define _WIN32
#  endif

#  ifdef _MT
#    ifndef _RWSTD_REENTRANT
#      define _RWSTD_REENTRANT 1
#    endif   // _RWSTD_REENTRANT

#    ifndef _RWSTD_NO_TLS
       // thread-local stoprage declaration specifier
#      ifndef _RWSTD_THREAD
#        define _RWSTD_THREAD   __declspec (thread)
#      endif   // _RWSTD_THREAD
#    endif   // _RWSTD_NO_TLS
#  else   // if !defined (_MT)
#    ifndef _RWSTD_THREAD
#      define _RWSTD_THREAD   /* empty */
#    endif   // _RWSTD_THREAD
#  endif   //_MT

   // clean C++ C headers (the <cxxx> kind) not provided
#  ifndef _RWSTD_NO_PURE_C_HEADERS
#    define _RWSTD_NO_PURE_C_HEADERS
#  endif   // _RWSTD_NO_PURE_C_HEADERS

#  ifndef _RWSTD_NO_EXPORT
     // not implemented
#    define _RWSTD_NO_EXPORT
#  endif   // _RWSTD_NO_EXPORT

#  define _RWSTD_MUNMAP_ARG1_T void*
#  define _RWSTD_NO_NL_LANGINFO

#  if defined (_CPPRTTI)
     // override in case library was configured with -GR-
     // (i.e., with RTTI disabled)
#    undef _RWSTD_NO_DYNAMIC_CAST
#  elif !defined (_RWSTD_NO_DYNAMIC_CAST)
     // override in case library was configured with -GR
     // (i.e., with RTTI enabled)
#    define _RWSTD_NO_DYNAMIC_CAST
#  endif   // _CPPRTTI

#  ifndef _NATIVE_WCHAR_T_DEFINED
   // define wchar_t if it is not a keyword recognized by the compiler
   // (use the /Zc:wchar_t compiler option to enable wchar_t as a keyword)
typedef unsigned short wchar_t;

#  endif   // _NATIVE_WCHAR_T_DEFINED

   // functions are defined in MSVC's msvcprt.lib and msvcprtd.lib
   // which include the compiler's implementation of the C++ Standard
   // library and with which we (obviously) cannot use
#  ifndef _RWSTD_NO_WCTOB
#    define _RWSTD_NO_WCTOB
#  endif   // _RWSTD_NO_WCTOB

#  ifndef _RWSTD_NO_WCTOB_IN_LIBC
#    define _RWSTD_NO_WCTOB_IN_LIBC
#  endif   // _RWSTD_NO_WCTOB_IN_LIBC

#  ifndef _RWSTD_NO_MBRTOWC
#    define _RWSTD_NO_MBRTOWC
#  endif   // _RWSTD_NO_MBRTOWC

#  ifndef _RWSTD_NO_MBRTOWC_IN_LIBC
#    define _RWSTD_NO_MBRTOWC_IN_LIBC
#  endif   // _RWSTD_NO_MBRTOWC_IN_LIBC

#  ifndef _RWSTD_NO_WCRTOMB
#    define _RWSTD_NO_WCRTOMB
#  endif   // _RWSTD_NO_WCRTOMB

#  ifndef _RWSTD_NO_WCRTOMB_IN_LIBC
#    define _RWSTD_NO_WCRTOMB_IN_LIBC
#  endif   // _RWSTD_NO_WCRTOMB_IN_LIBC

#  ifndef _RWSTD_NO_MBSTOWCS
#    define _RWSTD_NO_MBSTOWCS
#  endif   // _RWSTD_NO_MBSTOWCS

#  ifndef _RWSTD_NO_MBSTOWCS_IN_LIBC
#    define _RWSTD_NO_MBSTOWCS_IN_LIBC
#  endif   // _RWSTD_NO_MBSTOWCS_IN_LIBC

#  ifndef _RWSTD_NO_MBRLEN
#    define _RWSTD_NO_MBRLEN
#  endif   // _RWSTD_NO_MBRLEN

#  ifndef _RWSTD_NO_MBRLEN_IN_LIBC
#    define _RWSTD_NO_MBRLEN_IN_LIBC
#  endif   // _RWSTD_NO_MBRLEN_IN_LIBC

#  ifndef _RWSTD_NO_WCSRTOMBS
#    define _RWSTD_NO_WCSRTOMBS
#  endif   // _RWSTD_NO_WCSRTOMBS

#  ifndef _RWSTD_NO_WCSRTOMBS_IN_LIBC
#    define _RWSTD_NO_WCSRTOMBS_IN_LIBC
#  endif   // _RWSTD_NO_WCSRTOMBS_IN_LIBC

#  ifndef _RWSTD_NO_BTOWC
#    define _RWSTD_NO_BTOWC
#  endif   // _RWSTD_NO_BTOWC

#  ifndef _RWSTD_NO_BTOWC_IN_LIBC
#    define _RWSTD_NO_BTOWC_IN_LIBC
#  endif   // _RWSTD_NO_BTOWC_IN_LIBC

#  ifndef _RWSTD_NO_MBSRTOWCS
#    define _RWSTD_NO_MBSRTOWCS
#  endif   // _RWSTD_NO_MBSRTOWCS

#  ifndef _RWSTD_NO_MBSRTOWCS_IN_LIBC
#    define _RWSTD_NO_MBSRTOWCS_IN_LIBC
#  endif   // _RWSTD_NO_MBSRTOWCS_IN_LIBC
#endif   // _MSC_VER

/*** Siemens CDS++ ***************************************************/

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

/*** SunPro **********************************************************/

#ifdef __SUNPRO_CC

  // _REENTRANT defined by the -mt compiler option

#  if __SUNPRO_CC >= 0x530 && __SUNPRO_CC <= 0x540
     // test fails due to a SunPro 5.3 bug (PR RW #26641/Sun #4526136)
#    undef _RWSTD_NO_MEMBER_TEMPLATES
#  endif   // SunPro 5.3, 5.4

#  if __SUNPRO_CC <= 0x540
     // works around a partial specialization bug (PR #28119)
#    ifndef _RWSTD_NO_EXT_CONST_ALLOCATOR
#      define _RWSTD_NO_EXT_CONST_ALLOCATOR
#    endif
#  endif   // SunPro < 5.4

#  ifndef _RWSTD_NO_EXPORT
#    define _RWSTD_NO_EXPORT
#  endif   // _RWSTD_NO_EXPORT

   // prevent warnings about SunPro being unable to find
   // source for (intentionally undefined) member functions
   // of class templates
#  define _RWSTD_NO_UNDEFINED_TEMPLATES
#endif   // __SUNPRO_CC

/*** OS2 **************************************************************/

#ifdef __OS2__
#  define _RWSTD_NO_STATIC_MUTEX_INIT
#endif   // __OS2__

/*** Win{32,64} *******************************************************/

#if defined (_WIN32) || defined (_WIN64)
#  define _RWSTD_NO_STATIC_MUTEX_INIT
#  define _RWSTD_PATH_SEP '\\'
#endif   // _WIN32

#ifndef _RWSTD_PATH_SEP
#  define _RWSTD_PATH_SEP      '/'
#endif   // _RWSTD_PATH_SEP

/*** Non-10646 platforms **********************************************/

#if !defined (_RWSTD_OS_LINUX) && !defined (_WIN32)
   // Linux glibc and Windows use ISO 10646 (Unicode)
   // as the internal wchar_t encoding in all locales
#  ifndef _RWSTD_NO_ISO_10646_WCHAR_T
#    define _RWSTD_NO_ISO_10646_WCHAR_T
#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
#endif   // !Linux && !Windoze

/********************** Threads ***************************************/

#ifdef _RWSTD_REENTRANT
#  if    !defined (_RWSTD_DCE_THREADS)   \
      && !defined (_RWSTD_DEC_THREADS)   \
      && !defined (_RWSTD_POSIX_THREADS) \
      && !defined (_RWSTD_SOLARIS_THREADS)

   // default to POSIX threads except on Win32 or Win64

#    if !defined (_WIN32) && !defined (_WIN64)
#      define _RWSTD_POSIX_THREADS
#    endif   // _WIN{32,64}
#  endif   // _RWSTD_*_THREADS
#endif   // _RWSTD_REENTRANT

#ifdef _RWSTD_DCE_THREADS
#  define _RWSTD_NO_STATIC_MUTEX_INIT
#endif   // _RWSTD_DCE_THREADS

/********************** Miscellaneous *********************************/

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

/********************** Environment *********************************/

// Most (but not all) non-unix systems convert new line to carriage
// return + line feed on output:

#if defined (__OS2__) || defined (_WIN32) || defined (_WIN64)
#  define _RWSTD_CRLF_CONVENTION
#endif


// define wherever atomic instructions are provided
// affects the size of string reference (i.e., is binary incompatible)
#if   defined (__DECCXX)                                        \
  || (defined (__i386__)         &&  defined (__GNUG__))        \
  || (defined (_WIN32)           && !defined (_WIN64))          \
  || (defined (_AIX43)           &&  defined (__IBMCPP__))      \
  || (defined (_RWSTD_OS_IRIX64) &&  defined (__host_mips))     \
  || (defined (_PA_RISC2_0)      &&  defined (__HP_aCC))        \
  || (defined (__sparc) && (defined (__SUNPRO_CC) || defined (__GNUG__)))
#  define _RWSTD_NO_STRING_MUTEX
#endif   

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

/********************** Library Option Dependencies *************************/

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


#endif   // _RWSTD_CONFIG_H_INCLUDED
