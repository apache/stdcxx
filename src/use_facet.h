/***************************************************************************
 *
 * use_facet.h
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
 **************************************************************************/
 
#ifndef _RWSTD_USE_FACET_H_INCLUDED
#define _RWSTD_USE_FACET_H_INCLUDED


#include <new>
#include <rw/_defs.h>

#include "access.h"


// helper macro _RWSTD_DEFINE_FACET_FACTORY() defines a facet factory
// function called to construct, on demand, objects of specializations
// of facet class templates
// _byname facets are constructed on the heap while ordinary facets
// are constructed in static memory for efficiency and to avoid leaks,
// and are not destroyed during program lifetime

#ifndef _RWSTD_NO_BYNAME_FACET

     // definition for facets that come in both forms, plain and _byname
#  define _RWSTD_DEFINE_FACET_FACTORY(Linkage, Facet, TArgs, fid)          \
       typedef _STD:: Facet TArgs __rw_ ## fid ## _facet;                  \
       _RWSTD_NAMESPACE (__rw)                                             \
       {                                                                   \
           Linkage __rw_facet*                                             \
           __rw_ct_ ## fid (_RWSTD_SIZE_T ref, const char *name)           \
           {                                                               \
               __rw_facet* pfacet;                                         \
               if (name) {                                                 \
                   /* construct a _byname facet on the heap */             \
                   pfacet = new _STD:: Facet ## _byname TArgs (name, ref); \
               }                                                           \
               else {                                                      \
                   /* construct an ordinary facet in static memory */      \
                   static union {                                          \
                       void *align_;                                       \
                       char  data_ [sizeof (__rw_ ## fid ## _facet)];      \
                   } f;                                                    \
                   static __rw_facet* const pf =                           \
                       new (&f) __rw_ ## fid ## _facet (ref);              \
                   pfacet = pf;                                            \
               }                                                           \
               /* set the pointer to the facet id */                       \
               __rw_access::_C_get_pid (*pfacet) =                         \
                   &(__rw_access::_C_get_id (__rw_ ## fid ## _facet::id)); \
               return pfacet;                                              \
           }                                                               \
       } /* namespace __rw */                                              \
       typedef void __rw_unused_type

#else   // if defined (_RWSTD_NO_BYNAME_FACET)

     // definition for facets such as num_get that do not have a _byname form
#  define _RWSTD_DEFINE_FACET_FACTORY(Linkage, Facet, TArgs, fid)          \
       typedef _STD:: Facet TArgs __rw_ ## fid ## _facet;                  \
       _RWSTD_NAMESPACE (__rw)                                             \
       {                                                                   \
           Linkage __rw_facet*                                             \
           __rw_ct_ ## fid (_RWSTD_SIZE_T ref, const char*)                \
           {                                                               \
               /* construct an ordinary facet in static memory */          \
               static union {                                              \
                   void *align_;                                           \
                   char  data_ [sizeof (__rw_ ## fid ## _facet)];          \
               } f;                                                        \
               static __rw_facet* const pf =                               \
                   new (&f) __rw_ ## fid ## _facet (ref);                  \
               /* set the pointer to the facet id */                       \
               __rw_access::_C_get_pid (*pf) =                             \
                   &(__rw_access::_C_get_id (__rw_ ## fid ## _facet::id)); \
               return pf;                                                  \
           }                                                               \
       } /* namespace __rw */                                              \
       typedef void __rw_unused_type

#endif   // _RWSTD_NO_BYNAME_FACET


#ifndef _RWSTD_NO_SPECIALIZED_FACET_ID
   // provide definitions of explicit specializations of static
   // template data members to guarantee a single instance of
   // each in a program when the compiler or linker don't
   // (e.g., VisualAge C++ on AIX)
#  define _RWSTD_SPECIALIZE_FACET_ID(Facet, TArgs)        \
      _RWSTD_NAMESPACE (std) {                            \
          _RWSTD_SPECIALIZED_CLASS                        \
          _RW::__rw_facet_id Facet TArgs::                \
          id (_RW::__rw_facet_id::_C_Init);               \
      }                                                   \
      typedef void __rw_unused_type
#else   // if defined (_RWSTD_NO_SPECIALIZED_FACET_ID)
   // facet id's will not be explicitly specialized 
#  define _RWSTD_SPECIALIZE_FACET_ID(ignore1, ignore2)    \
      typedef void __rw_unused_type
#endif   // _RWSTD_NO_SPECIALIZED_FACET_ID


#define _RWSTD_SPECIALIZE_USE_FACET(fid)                              \
    _RWSTD_NAMESPACE (__rw) {                                         \
        _RWSTD_EXPORT                                                 \
        const __rw_facet*                                             \
        __rw_get_facet_by_id                                          \
            (const _STD::locale &__loc,                               \
             const __rw_facet_id&,                                    \
             const __rw_ ## fid ## _facet*) {                         \
            return __rw_get_std_facet (__loc, __rw_facet::_C_ ## fid, \
                                       __rw_ct_ ## fid);              \
       }                                                              \
    }                                                                 \
    typedef void __rw_unused_type

#endif   // _RWSTD_USE_FACET_H_INCLUDED
