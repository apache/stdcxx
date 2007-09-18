#!/bin/sh
#
# $Id$
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
#
# fixtree.sh - creates a source directory tree (using symlinks) suitable for
# the GNUmakefile infrastructure from a directory tree required by HAL/RCB
#
##############################################################################

if [ $# -ne 1 ] ; then
    echo "usage: $0 [<directory>]"
    exit
fi

ROOTDIR=$1

if [ ! -d $ROOTDIR ] ; then
    echo "$0: $ROOTDIR not found"
    exit 1
fi

mkdir stdlib
if [ $? -ne 0 ] ; then
    exit 2
fi

if [ -d $ROOTDIR/include ] ; then
    ln -s $ROOTDIR/include stdlib/include
else
    echo "$0: $ROOTDIR/include: directory doesn't exist" >&2
    exit 3
fi

for d in etc examples source tests ; do
    if [ -d $ROOTDIR/$d ] ; then
        ln -s $ROOTDIR/$d/stdlib stdlib/$d
    else
        echo "$0: $ROOTDIR/$d: directory doesn't exist" >&2
        exit 3
    fi
done

if [ -r $ROOTDIR/etc/stdlib/config/GNUmakefile ] ; then
    ln -s $ROOTDIR/etc/stdlib/config/GNUmakefile stdlib/GNUmakefile
else
    echo "$0: $ROOTDIR/etc/stdlib/config/GNUmakefile file doesn't exist" >&2
    exit 3
fi

mkdir rwtest
if [ $? -ne 0 ] ; then
    exit 2
fi

if [ -d $ROOTDIR/source/rwtest ] ; then
    ln -s $ROOTDIR/source/rwtest rwtest/source
else
    echo "$0: $ROOTDIR/source/rwtest: directory doesn't exist (continuing)" >&2
fi

if [ -d $ROOTDIR/rw ] ; then
    ln -s $ROOTDIR/rw rwtest/rw
else
    echo "$0: $ROOTDIR/rw: directory doesn't exist (continuing)" >&2
fi
