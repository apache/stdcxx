/***************************************************************************
 *
 * 20.tuple.h - framework for tuple tests
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
 * Copyright 2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#ifndef RW_20_TUPLE_H_INCLUDED
#define RW_20_TUPLE_H_INCLUDED

#include <tuple>

#include <rw_value.h>                       // for UserClass


// various tuple types for test purposes

typedef std::tuple <>                       EmptyTuple;

typedef std::tuple <int>                    IntTuple;
typedef std::tuple <const int>              ConstIntTuple;
typedef std::tuple <int&>                   IntRefTuple;

typedef std::tuple <IntTuple>               NestedTuple;

typedef std::tuple <long, const char*>      PairTuple;

typedef std::tuple <UserClass>              UserTuple;

#define BIG_TYPES   bool, char, int, double, void*, UserClass
#define BIG_SIZE    6

typedef std::tuple <BIG_TYPES>              BigTuple;


#endif   // define RW_20_TUPLE_H_INCLUDED
