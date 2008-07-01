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
# Copyright 2001-2008 Rogue Wave Software, Inc.
#
##############################################################################
#
# Test to determine which ANSI C library functions are declared in the
# respective header or, if not, whether they are defined in the library
# binary (libc or libm)
#
# To test, perform the following steps after making sure that vars.sh
# exists in the cwd, or after defining the appropriate shell variables:
#
#   $ rm -f header.h logfile.log
#   $ touch header.h logfile.log
#   $ libc_decl header.h logfile.log [function [headers]]
#
##############################################################################

output=/dev/tty
logfile=/dev/tty

OSNAME=`uname -s`

if [ $OSNAME = "SunOS" -a -x /usr/xpg4/bin/basename ]; then
    # use the POSIX basename rather than the one in /usr/bin
    # to avoid interpreting the suffix in a special way
    basename=/usr/xpg4/bin/basename
else
    basename=basename
fi

[ ! -d "$TMPDIR" ] && TMPDIR=/tmp

# set the output file if specified, otherwise use the console
if [ -f "$1" ]; then
    output="$1"
fi

# set the log file if specified, otherwise use the console
if [ -f "$2" ]; then
    logfile="$2"
fi

if [ ! -z "$3" ]; then
    function="$3"
fi

[ -r vars.sh ] && . ./vars.sh

# include headers.inc file
. $TOPDIR/etc/config/src/headers.inc

if [ ! -z "$4" ]; then
    hdrs="$4"
fi



CPPFLAGS="`echo $CPPFLAGS | sed 's:-I *[^ ]*::g'`"

if [ "$CXX" = "aCC" ] ; then

    cxx_major="`echo $CXX_VER | sed 's/.*\.\([0-9][0-9]*\)\..*/\1/'`"
    echo $CXXFLAGS | grep '[-]Aa' >/dev/null 2>&1
    has_Aa=$?

    if [ "$cxx_major" -le "05" -a $has_Aa -eq 0 ] ; then
        # prepend -I/usr/include to CXXOPTS for HP aCC when the -Aa
        # command line option is specified (aCC 3 and 5 but not aCC
        # 6 on IPF)
        CXXFLAGS="$CXXFLAGS -I/usr/include"
    fi

    unset cxx_major
fi

LDFLAGS="$LDFLAGS $LDLIBS"

capitalize='sed y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'

##############################################################################
# determine whether each header exists and its full pathname

no_new_headers=0

for h in $hdrs ; do

    hdr_base=`${basename} $h \.h`
    hdr=$h

    if [ "$hdr_base" = "$h" ] ; then
        # check for C++ C library headers first
        if [ $h != new -a $h != typeinfo ] ; then
            hdr="c$h"
        fi;

        printf "%-50.50s " "checking for <$hdr>"

        tmpfile=$TMPDIR/$h-$$
        echo "#include <$hdr>" > $tmpfile.cpp
        sym="`echo $hdr | $capitalize`"

        echo "$CXX -E $CPPFLAGS $CXXFLAGS $tmpfile.cpp >$tmpfile.i" >>$logfile
        $CXX -E $CPPFLAGS $CXXFLAGS $tmpfile.cpp >$tmpfile.i 2>>$logfile
        if [ $? -eq 0 ] ; then
            echo "ok"
            echo "// #define _RWSTD_NO_${sym}" >>$output
        else
            echo "no (_RWSTD_NO_${sym})"
            echo "#define _RWSTD_NO_${sym}" >> $output
            # <ciso646> is a bogus header, ignore if missing
            [ $sym != ciso646 ] && no_new_headers=1
        fi
    fi

    # check for (deprecated C++) C library headers
    # or for any headers specified with the .h suffix
    printf "%-50.50s " "checking for <$hdr_base.h>"

    tmpfile=$TMPDIR/$hdr_base-$$
    echo "#include <$hdr_base.h>" > $tmpfile.cpp
    sym="`echo $hdr_base | $capitalize`"

    echo "$CXX -E $CPPFLAGS $CXXFLAGS $tmpfile.cpp >$tmpfile.i" >>$logfile
    $CXX -E $CPPFLAGS $CXXFLAGS $tmpfile.cpp >$tmpfile.i 2>>$logfile
    if [ $? -eq 0 ] ; then
        path=`  cat $tmpfile.i \
              | sed -n "s|[^\"]* \(\".*/$hdr_base\.h\"\).*|\1|p" \
              | head -n 1`

        # handle headers implemented internally by some compilers
        # (such as <stdarg.h> with the vanilla EDG eccp)
        [ "$path" = "" ] && path="<$hdr_base.h>"

        echo "ok ($path)"
        if [ "$hdr_base" = "$h" ]; then
            echo "#define _RWSTD_ANSI_C_${sym}_H $path" >>$output
        else
            echo "// #define _RWSTD_NO_${sym}_H   /* $path */" >>$output
        fi
    else
        echo "no (_RWSTD_NO_${sym}_H)"
        echo "#define _RWSTD_NO_${sym}_H" >>$output
    fi

    rm -f $tmpfile.cpp $tmpfile.i

done


##############################################################################
# determine the support for namespaces

printf "%-50.50s " "checking for namespaces"

no_namespace=0

echo "$CXX -c $CPPFLAGS $CXXFLAGS $TOPDIR/etc/config/src/NAMESPACE.cpp" \
     >>$logfile
$CXX -c $CPPFLAGS $CXXFLAGS $TOPDIR/etc/config/src/NAMESPACE.cpp \
     >>$logfile 2>&1
if [ $? -eq 0 ] ; then
    echo "ok"
    echo "// #define _RWSTD_NO_NAMESPACE" >>$output
else
    echo "no (_RWSTD_NO_NAMESPACE)"
    echo "#define _RWSTD_NO_NAMESPACE" >>$output
    no_namespace=1
fi


##############################################################################
# determine wheteher each function is declared/defined

# create a test source file template into which each iteration below
# substitutes the header name, function name, and function arguments
tmpsrc=$TMPDIR/libc_decl_tmpsrc-$$.cpp

cat << EOF > $tmpsrc

extern "C"
{
    typedef void (*funptr_t)();
}

// take the address of the function in a way
// that foils even clever optimizers

union funptr_type {
    funptr_t      pf;
    unsigned long ul;
} funptri;

#ifdef CHECK_DECL

#  if defined (__linux__) && defined (__GNUG__) && __GNUG__ < 3
     // prevent empty exception specifications from triggering
     // gcc 2.9x -fhonor-std link errors looking for std::terminate()
#    include <features.h>
#    undef __THROW
#    define __THROW
#  endif   // gcc < 3 on Linux

#  if    defined (__EDG__)                \
      && !defined (__DECCXX)              \
      && !defined (__HP_aCC)              \
      && !defined (__INTEL_COMPILER)      \
      && !defined (_SGI_COMPILER_VERSION)
     // disable error #450-D: the type "long long" is nonstandard
     // when using the vanilla EDG eccp in strict mode (i.e., w/o
     // long long support)
#    pragma diag_suppress 450
#  endif   // EDG eccp on Linux

#  include HDRNAME

#  if !$no_namespace

namespace std { }
using namespace std;

#  endif   // no_namespace

int main (int argc, char**)
{
    // with gcc, prevent intrinsics from causing false positives

#  if TAKE_ADDR

    funptri.pf = (funptr_t)&FUNNAME;

    return (argc < 1 ? !funptri.ul : !!funptri.ul) + 0;
    
#  else   // if !TAKE_ADDR

    // disable warnings about an unused variable
    (void)&argc;

    // call the function using the supplied arguments
    (void)FUN;

    return 0;

#  endif   // TAKE_ADDR

}

#else   // if !defined (CHECK_DECL)

extern "C" void FUNNAME ();

int main (int argc, char**)
{
    funptri.pf = (funptr_t)&FUNNAME;

    return (argc < 1 ? !funptri.ul : !!funptri.ul) + 0;
}

#endif   // CHECK_DECL

EOF

for h in $hdrs ; do

    hdr_base=`${basename} $h \.h`
    eval funs=\$$hdr_base

    if [ -z "$funs" ] ; then
        continue
    fi

    if [ "$hdr_base" = "math" ] ; then
        lib=m
    else
        lib=c
    fi

    # float and long double versions of <math.h> functions
    # are not expected to be declared in <cmath> or in std
    use_libc_header=0

    for f in $funs; do

        # determine whether the tested function is followed by
        # a function argument list; if so, call it, otherwise
        # take its address
        funname=`echo $f | sed "s/\([a-zA-Z_][a-zA-Z_0-9]*\).*/\1/"`

        if [ "$function" != "" -a "$function" != "$funname" ]; then
            continue
        fi

        if [ "$funname" = "$f" ] ; then
            # take the address of the function
            # (the function must not be overloaded)
            take_addr=1
        else
            # function may be overloaded, call it instead
            take_addr=0
        fi

        # starting with acosf(), look in <math.h> rather than <cmath>
        [ "$f" = acosf ] && use_libc_header=1

        std=""

        # determine which form of the header to #include
        if [ $use_libc_header -eq 1 ] ; then
            hdrname="${h}.h"
        elif [ $no_new_headers -eq 0 ] ; then
            hdrname="c${h}"
            if [ $no_namespace -eq 0 ] ; then
                std="std::"
            fi
        else
            hdrname="${h}.h"
        fi

        printf "%-50.50s " "checking for $std$funname() in <$hdrname>"

        # temporary object file, and executable file names
        tmpobj=$TMPDIR/$funname-$$.o
        tmpexe=$TMPDIR/$funname-$$

        sym="_RWSTD_NO_`echo $funname | $capitalize`"

        echo "$CXX -c -DCHECK_DECL $CXXFLAGS $WARNFLAGS "   \
             "-DHDRNAME=\"<$hdrname>\" -DFUNNAME=$funname " \
             "-DFUN=$f -DTAKE_ADDR=$take_addr "             \
             "$tmpsrc -o $tmpobj"                           \
             "&& $LD $tmpobj $LDFLAGS -l$lib" >>$logfile 2>&1

        # spell out all arguments just like above, being careful
        # about quoting HDRNAME
        $CXX -c -DCHECK_DECL $CXXFLAGS $WARNFLAGS          \
                -DHDRNAME="<$hdrname>" -DFUNNAME=$funname  \
                -DFUN=$f -DTAKE_ADDR=$take_addr            \
             $tmpsrc -o $tmpobj >>$logfile 2>&1            \
        && $LD $tmpobj $LDFLAGS -l$lib >>$logfile 2>&1

        if [ $? -eq 0 ] ; then
            echo "ok"
            sym="// #define $sym"
            echo $sym >>$output
        else
            # if a symbol isn't declared in the header,
            # see if it maybe exists in the library
            echo "no ($sym)"
            echo "#define $sym" >>$output

            sym="`echo ${sym}_IN_LIB$lib | $capitalize`"
            
            printf "%-50.50s " "checking for extern \"C\" $funname() in lib$lib"

            # define cxxflags for convenience
            cxxflags="$CXXFLAGS $WARNFLAGS -DFUNNAME=$funname"

            echo "$CXX -c $cxxflags $tmpsrc -o $tmpobj \
                 && $LD $tmpobj $LDFLAGS -l$lib" >>$logfile

            $CXX -c $cxxflags $tmpsrc -o $tmpobj >>$logfile 2>&1 \
            && $LD $tmpobj $LDFLAGS -l$lib >>$logfile 2>&1

            if [ $? -eq 0 ] ; then
                echo "ok"
                sym="// #define $sym"
            else
                echo "no ($sym)"
                sym="#define $sym"
            fi
            echo $sym >>$output
        fi
        rm -f $tmpobj
        
    done
done

rm -f $tmpsrc

exit


##############################################################################
##############################################################################
#
# helper script to generate the contents of <cmath> (needs editing)
#

c90_funs="acosf asinf atanf atan2f ceilf cosf coshf expf fabsf floorf fmodf frexpf ldexpf logf log10f modff powf sinf sinhf sqrtf tanf tanhf acos asin atan atan2 ceil cos cosh exp fabs floor fmod frexp ldexp log log10 modf pow sin sinh sqrt tan tanh acosl asinl atanl atan2l ceill cosl coshl expl fabsl floorl fmodl frexpl ldexpl logl log10l modfl powl sinl sinhl sqrtl tanl tanhl"

c99_funs="cbrtf copysignf erfcf erff expm1f exp2f fdimf fmaf fmaxf fminf hypotf ilogbf lgammaf logbf log1pf log2f llrintf lrintf lroundf llroundf nanf nearbyintf nextafterf nexttowardf remainderf remquof rintf roundf scalbnf scalblnf tgammaf cbrt copysign erf erfc expm1 exp2 fdim fma fmax fmin hypot ilogb lgamma logb log1p log2 llrint lrint lround llround nan nearbyint nextafter nexttoward remainder remquo rint round scalbn scalbln tgamma cbrtl copysignl erfcl erfl expm1l exp2l fdiml fmal fmaxl fminl hypotl ilogbl lgammal logbl log1pl log2l llrintl lrintl lroundl llroundl nanl nearbyintl nextafterl nexttowardl remainderl remquol rintl roundl scalbnl scalblnl tgammal"

cap='sed y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'

for f in $c90_funs ; do
    sym=`echo $f | $cap`

    type=double

    echo $f | grep "[^f]f$" >/dev/null
    if [ $? -eq 0 ] ; then
        type=float
    else
        echo $f | grep "[^l]l$" >/dev/null
        if [ $? -eq 0 ] ; then
            type="long double"
        fi
    fi

    # assume a non-conforming header with all libc names in global namespace
    echo "#ifndef _RWSTD_NO_$sym"
    echo "#  define _$sym(x)    ::$f (x)"
    echo "#elif !defined (_RWSTD_NO_${sym}_IN_LIBM)"
    echo "extern \"C\" $type $f ($type);"
    echo "#  define _$sym(x)    ::$f (x)"
    echo "#else"
    echo "#  define _$sym(x)    ::$f (double (x))"
    echo "#endif   // _RWSTD_NO_$sym"
    echo
done
