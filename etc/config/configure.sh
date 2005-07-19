#!/bin/sh
#
# $Id: //stdlib/dev/etc/stdlib/config/configure.sh#3 $
#
# script to configure the C++ Standard library
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
