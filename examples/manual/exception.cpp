/**************************************************************************
 *
 * except.cpp - Example program for exceptions.
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

#include <iostream>
#include <stdexcept>

#include <examples.h>

#ifndef _RWSTD_NO_EXCEPTIONS

int main ()
{
   try {
       // Enable exceptions in cin.
       std::cin.exceptions (std::ios::eofbit);

       // Clear all bits and set eofbit.
       std::cin.clear (std::ios::eofbit);
   }
   catch (const std::ios::failure &e) {
       std::cout << "Caught an exception: " << e.what () << std::endl;
   }
   catch (const std::exception &e) {
       std::cout << "Caught an exception: " << e.what () << std::endl;

       return 1;   // Indicate failure.
   }
   catch (...) {
       std::cout << "Caught an unknown exception" << std::endl;

       return 1;   // Indicate failure.
   }

   return 0;
}

#else

int main ()
{
    std::cout << "Exceptions not supported." << std::endl;

    return 0;
}


#endif   // _RWSTD_NO_EXCEPTIONS
