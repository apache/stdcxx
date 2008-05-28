// checking for mmap() and munmap() in <sys/mman.h>

/***************************************************************************
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
 * Copyright 2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <sys/mman.h>
#include <sys/types.h>


void* map_file (int fd)
{
    return mmap (0, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
}


int main (int argc, char *argv[])
{
    // avoid executing the code except when one or more command
    // line arguments have been specified
    if (argc < 2)
        return 0;

    void *p = map_file (argc);
    munmap (p, 0);

    return 0;
}
