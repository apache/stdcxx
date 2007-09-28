/**************************************************************************
 *
 * path.h
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef RWSTD_PATH_H_INCLUDED
#define RWSTD_PATH_H_INCLUDED

#include <string>


// creates a directory in the current working directory
// returns 0 on success, non-zero value on failure
int
makedir (const char*);


void
create_symlink (const std::string &dir, 
                const std::string &xname, 
                const std::string &sname);

// returns the pathname of a file `fname' which may be a file name
// or an absolute or relative pathname; relative pathnames are searched
// for in the following order
//   1.   if `other' is an absolute pathname, use dirname(other)
//   1.1. if `other' is a relative pathname, then use cwd() + dirname(other)
//        + fname
//   2.   relative to the ${RWSTD_SRC_ROOT} environment variable, if
//        it is set and non-empty
//   3.   relative to the current working directory
std::string
get_pathname (const std::string &fname,
              const std::string &other = std::string ());


#endif   // RWSTD_PATH_H_INCLUDED
