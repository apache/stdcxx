#!/bin/sh
#
# $Id$
#
# script to configure the C++ Standard library
#
##############################################################################
#
# Licensed to the Apache Software  Foundation (ASF) under one or more
# contributor  license agreements.  See  the NOTICE  file distributed
# with  this  work  for  additional information  regarding  copyright
# ownership.   The ASF  licenses this  file to  you under  the Apache
# License, Version  2.0 (the  "License"); you may  not use  this file
# except in  compliance with the License.   You may obtain  a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the  License is distributed on an  "AS IS" BASIS,
# WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
# implied.   See  the License  for  the  specific language  governing
# permissions and limitations under the License.
#
# Copyright 1999-2007 Rogue Wave Software, Inc.
#
##############################################################################

CONFIG_FILE="config.h"
TOPDIR=":"

# process command line arguments
for f in $* ; do
    if [ "$CONFIG_FILE" = "" ] ; then
        CONFIG_FILE="$f"
        continue
    elif [ "$TOPDIR" = "" ] ; then
        TOPDIR="$f"
        continue
    elif [ "$f" = "-f" ] ; then
        unset CONFIG_FILE
        continue
    elif [ "$f" = "-d" ] ; then
        unset TOPDIR
        continue
    fi
done

if [ ! -d "$TOPDIR" ] ; then
    echo "TOPDIR ('$TOPDIR') not found."
    exit 1
fi
