#!/bin/sh
#
# $Id: //stdlib/dev/etc/stdlib/config/fixtree.sh#1 $
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
