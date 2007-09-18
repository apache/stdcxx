#!/bin/sh
#
# $Id$
#
# prints IBM VisualAge and XLC/C++ version
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
# reported versions and patches they correspond to:
#
# version    release/patch
# --------   -------------
# 6.0.0.0    VisualAge 6
# 6.0.0.14   VisualAge 6, Septemmber 2005 PTF
# 6.0.0.17   VisualAge 6, November 2006 PTF
# 7.0        VisualAge 7
# 7.0.0.1    VisualAge 7 November 2004 PTF
# 7.0.0.2    VisualAge 7 May 2005 PTF
# 7.0.0.3    VisualAge 7 Aug 2005 PTF
# 7.0.0.4    ???
# 7.0.0.5    VisualAge 7 January 2006 PTF
# 7.0.0.6    VisualAge 7 April 2006 PTF
# 7.0.0.7    VisualAge 7 August 2006 PTF
# 7.0.0.8    VisualAge 7 October 2006 PTF
# 7.0.0.9    VisualAge 7 Feb 2007 PTF
# 8.0.0.0    XLC/C++ 8
# 8.0.0.1    XLC/C++ 8 March 2006 PTF
# 8.0.0.10   XLC/C++ 8 June 2006 PTF
# 8.0.0.11   XLC/C++ 8 August 2006 PTF
# 8.0.0.12   XLC/C++ 8 December 2006 PTF
# 8.0.0.13   XLC/C++ 8 February 2007 PTF

if [ $# -gt 0 ]; then
    CXX=$1
else
    CXX=xlC
fi


xlcout=/tmp/$CXX_version.$$.out

$CXX -qversion >$xlcout 2>&1

if [ $? -eq 0 ]; then
    # VisualAge 7.0, 2005/8/31 or later
    version=`grep "Version:" $xlcout \
             | awk '{ if ($1 == "Version:") print $2; \
                      else if ($4 == "Version") print $5; \
                      else print $7; }'`
    rm $xlcout

    # collapse consecutive leading zeros
    echo $version | awk 'BEGIN { FS="." }{ print (0 + $1) "." (0 + $2) "." (0 + $3) "." (0 + $4) }'
    exit
fi

# check to see if this is the base 7.0
grep "V7" $xlcout > /dev/null 2>&1
v7_or_6=$?

if [ $v7_or_6 -ne 0 ]; then
    # if it's not 7.0 see if it's 6.0
    grep "version 6.0" $xlcout > /dev/null 2>&1
    v7_or_6=$?
fi

if [ $v7_or_6 -eq 0 ]; then

    # VisualAge 6.0 or 7.0 (unpatched)
    tmpsrc=/tmp/$CXX_version.$$.cpp
    tmpobj=/tmp/$CXX_version.$$.o

    echo "#pragma comment(compiler)" > $tmpsrc
    echo "int main() { }" >> $tmpsrc

    $CXX -c $tmpsrc -o $tmpobj
    status=$?

    if [ $status -eq 0 ]; then
        version=`strings $tmpobj \
                 | sed -n 's/.*Version \([^ ]*\) *.*/\1/p' \
                 | tail -1`
    fi

    rm -f $tmpsrc $tmpobj $xlcout 2>/dev/null

    if [ "$version" = "" ]; then
        version=`$CXX -qversion \
                 | awk '{ if ($4 == "Version") print $5; else print $7; }'`
    fi

    echo $version | sed "s/[^0-9]*\([0-9].*\)/\1/"
    exit
fi

# VisualAge 5.0 (and prior?)
echo >> $xlcout
sed "s/.*version \(.*\)/\1/" $xlcout
rm $xlcout
