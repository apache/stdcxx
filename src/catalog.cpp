/***************************************************************************
 *
 * rwcats.cpp - Implementation of XPG4 messages
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#if defined (_RWSTD_NO_NL_TYPES_H) || defined (_RWSTD_NO_CATOPEN_CATGETS)

#include <vector>
#include <stdio.h>
#include "catalog.h"


_RWSTD_NAMESPACE (__rw) { 

_USING (namespace std);


typedef _STD::vector<__rw_cat*> CatVector;

static CatVector __rw_catlist (2);
  

#ifdef _RWSTD_USE_RC

  const char* __rw_cat::catgets(int, int msg_num, const char* s)
  {
    __cat_type::iterator it = __msgs.find(msg_num);
    if (it != __msgs.end())
      return (*it).second.c_str ();
    else {
      char buf[NL_TEXTMAX];
      if (LoadString(__cat,(UINT)msg_num,buf,NL_TEXTMAX))
      {
          pair<__cat_type::iterator,bool> ret =
              __msgs.insert (__msg_type (msg_num, string(buf)));
        if (ret.second)
          return (*ret.first).second.c_str ();
      }
    }
    return s;
  }

#endif //_RWSTD_USE_RC

  CatVector::size_type __catfind(nl_catd id)
  {
    for (CatVector::size_type i = 0;
         i < __rw_catlist.size() && __rw_catlist[i]; ++i) {

      if (__rw_catlist[i]->id() == id)
          return i;
    }
    
    return __rw_catlist.size();
  }  


}   // namespace __rw


_USING (namespace __rw);


nl_catd catopen(const char* name, int oflag)
{
    __rw_cat* cat = 0;
    
    _TRY {
        cat = new __rw_cat(name,oflag);

        if (cat && cat->good()) {
            CatVector::size_type i = 0;
            for (; i < __rw_catlist.size() && __rw_catlist[i]; ++i) ;
            if (__rw_catlist.size() == i)
                __rw_catlist.resize(__rw_catlist.size() * 2,0);
            __rw_catlist[i] = cat;
            return cat->id();
        }
    }
    _CATCH (...) {
    }

    delete cat;
    return (nl_catd)-1; 
}


const char * catgets(nl_catd catd, int set_num, int msg_num, const char* s)
{
    _TRY {

        CatVector::size_type i = __catfind(catd);
        if (i < __rw_catlist.size())
            return __rw_catlist[i]->catgets(set_num,msg_num,s);

        return s;
    }
    _CATCH (...) {
        return s;
    }
}


int catclose(nl_catd catd)
{
  CatVector::size_type i = __catfind(catd);
  if (i < __rw_catlist.size())
  {
    delete __rw_catlist[i];
    __rw_catlist[i] = 0;
    CatVector::size_type j;
    for (j = i+1; j < __rw_catlist.size() && __rw_catlist[j]; j++)
      __rw_catlist[j-1] = __rw_catlist[j];
    if (j < __rw_catlist.size())
      __rw_catlist[j] = 0;
    else
      __rw_catlist[__rw_catlist.size () - 1] = 0;
    return 0;
  }
  return -1;
}


#endif   // _RWSTD_NO_NL_TYPES_H || _RWSTD_NO_CATOPEN_CATGETS
