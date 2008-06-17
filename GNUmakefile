# -*- Makefile -*-
# 
# $Id$
#
# main makefile to build and test the C++ Standard library
#
########################################################################
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
########################################################################
#
# Usage:
#
#   Invoke from $TOPDIR to create the build directory tree, $BUILDDIR
#   configure and build the library, example programs, utility programs
#   testsuite driver, and the testsuite.
#
#   $ make [ BUILDTYPE=<build-type> ] \
#          [ BUILDDIR=<build-dir> ] \
#          [ CONFIG=<config-file> ] \
#          [ PHDIR=<plumhall-testsuite-source-dir> ]
#          [ <targets> ]
#
#   From $BUILDDIR/include to [re]configure or just run one or more tests:
#
#   $ make [ config | config.h | <target> ... ]
#
#   From $TOPDIR or $BUILDDIR to configure if necessary and build the lib,
#   tests, plumhall testsuite, and examples:
#
#   $ make [ lib ] [ bin ] [ rwtest ] [ tests ] [ phtst ] [ examples ]
#
#   From $TOPDIR or $BUILDDIR to build the lib, tests, and examples, run the
#   built executables, and (by default, unless DRYRUN is non-empty)
#   post results in Perforce:
#
#   $ make run | runall | run_all [ RUN=<executables> | ALL ]
#
#   For every invocation of make the variables CPPOPTS, CXXOPTS, LDOPTS,
#   and RUNOPTS can be set on the command line to pass additional options to
#   the preprocessor, compiler, linker, and the runall.sh script, respectively.
#
#   The variable MAKE can be set to contain any additional make arguments
#   to be passed to recursive invokations of make (e.g., make -j).
#
#   The variable SHELL can be defined to override the default shell, /bin/sh.
#
########################################################################
#
# Targets:
#
#   builddir  - creates the build directory (BUILDDIR) and makefile.in
#
#   config    - performs configuration tests and creates the configuration
#               header, $(BUILDDIR)/include/config.h
#
#   libstd    - builds the library
#
#   rwtest    - builds the testsuite driver
#
#   tests     - builds the testsuite
#
#   examples  - builds the example programs
#
#   util      - builds the utility programs
# 
##############################################################################
#
# Nonstandard variables:
#
#   BUILDDIR  - (optional) the root of the build directory tree,
#               the default value is $(TOPDIR)/build
#
#   BUILDTAG  - (optional) a string appended to a report filename
#               the default value is empty
#
#   BUILDTYPE - (optional) one of
#                   8s, 8d, 11s, 11d, 12s, 12d, 15s, 15d,
#                   8S, 8D, 11S, 11D, 12S, 12D, 15S, 15D
#               where the capitalized forms are wide forms of the others
#               the default value is 11s
#
#   BUILDMODE - (optional) a comma separated list of at most one of each of
#                  debug, optimized, shared
#                  threads, pthreads, dcethreads, wide
#               where
#               *  debug turns on debugging and disables optimization
#               *  optimized enables optimization and disables debugging
#               *  shared creates and links with a shared library
#               *  threads uses Solaris threads for thread safety
#               *  pthreads uses POSIX threads for thread safety
#               *  dcethreads uses DCE threads for thread safety
#               *  wide uses wide (typically 64-bit) memory model
#               the default value is debug (same as 11s)
#
#   CATFILE   - name of library catalog file to be generated - see gencat(1)
#
#   CCVER     - compiler version (must contain no spaces)
#
#   DEPENDFLAGS - preprocessor flags used to generate dependencies
#
#   LIBBASE   - name of the library w/o the suffix suitable
#               for use by the linker after the -l option
#
#   DEPENDDIR - the subdirectory where dependency files will reside
#
#   LIBNAME   - full name of the library being built
#
#   LIBVER    - library version number in the <major>.<minor>.<micro> format
#
#   LIBSUFFIX - archive/shared library suffix (defaults to a)
#
#   LDSOFLAGS - linker flags used when building a shared lib
#
#   LDLIBS    - set of system libraries to link executables with
#
#   PHDIR     - directory containing the PlumHall testsuite distribution
#                 (e.g., ".../lvs99a")
#
#   PHWARNFLAGS - extra flags to suppress warnings coming from PlumHall tests
#
#   PICFLAGS  - flags to generate Position Independent Code
#               for shared libraries
#
#   PREFIX    - the root of the installation directory where the bin/
#               include/ and lib/ subdirectories will be rooted
#
#   PRELINKFLAGS - flags passed to the prelinker
#
#   RUNFLAGS  - default options passed to runall.sh when running executables
#   RUNOPTS   - additional options to pass to runall.sh (empty by default)
#
#   SHARED    - set to `true' if building a shared lib, `false' otherwise
#
#   WARNFLAGS - any compiler warning options
#
#   WITH_PURIFY - set to `true' to build with purify. additional flags
#                 can be specified in PURIFYFLAGS.
#
#   WITH_CADVISE - set to `true' to build with cadvise. additional flags
#                  can be specified in CADVISEFLAGS.
#
########################################################################

SHELL = /bin/sh

# define the value of OSNAME first so that it can be used in *.config files
OSNAME := $(shell uname)

# provide a value if it isn't already set by (an older version of) make
ifeq ($(CURDIR),)
  ifneq ($(PWD),)
    # use the shell PWD variable to avoid expanding NFS paths
    # as mount points instead of the user-visible directories
    CURDIR := $(PWD)
  else
    CURDIR := $(shell pwd)
  endif
endif


-include ./makefile.in


# buildpath must not be defined
ifneq ($(buildpath),)
  $(error "buildpath not empty ('$(buildpath)')")
endif


# check to see if BUILDDIR is defined
ifeq ($(BUILDDIR),)
  # the default value is $(TOPDIR)/build
  buildpath = $(CURDIR)/build
else
  # see if BUILDDIR is an absolute pathname
  ifeq ($(filter /%,$(BUILDDIR)),)
    # ...if not, prepend the name of CURDIR
    buildpath = $(CURDIR)/$(BUILDDIR)
  else
    # ...otherwise, make sure it doesn't refer to TOPDIR
    ifeq ($(BUILDDIR),$(TOPDIR))
      $(error "BUILDDIR ('$(BUILDDIR)') is same as TOPDIR ('$(TOPDIR)')")
    endif
    buildpath = $(BUILDDIR)
  endif   # ifneq ($(filter /%,$(BUILDDIR)),)
endif   # ifneq ($(BUILDDIR),)


ifeq ($(filter /%,$(buildpath)),)
  # buildpath must be an absolute pathname
  $(error "buildpath=$(buildpath): not an absolute pathname")
endif

ifeq ($(buildpath),$(TOPDIR))
  # buildpath must be distinct from TOPDIR
  $(error "buildpath=$(buildpath): not distinct from TOPDIR=$(TOPDIR)")
endif


MAKEFILE_IN = $(buildpath)/makefile.in
-include $(MAKEFILE_IN)


INCDIR      = $(buildpath)/include
LIBDIR      = $(buildpath)/lib
BINDIR      = $(buildpath)/bin
EXMDIR      = $(buildpath)/examples
TSTDIR      = $(buildpath)/tests
PHTSTDIR    = $(buildpath)/plumhall
MAKEDIRS    = $(buildpath)          \
              $(LIBDIR)             \
              $(buildpath)/rwtest   \
              $(BINDIR)             \
              $(buildpath)/nls      \
              $(TSTDIR)             \
              $(PHTSTDIR)           \
              $(EXMDIR)             \
              $(INCDIR)

# file to write log of the build to
LOGFILE = /dev/null

# convert a relative pathname to an absolute one
ifneq ($(shell echo $(LOGFILE) | sed -n "s/^ *\/.*/\//p"),/)
  LOGFILE := $(buildpath)/$(LOGFILE)
endif


ifeq ($(TOPDIR),)
  # this is the first invocation of make in TOPDIR
  in_topdir = 1
else
  ifeq ($(TOPDIR),$(CURDIR))
    # this is a recursive invocation of make in TOPDIR
    in_topdir = 1
  endif
endif


ifeq ($(in_topdir),1)

  ######################################################################
  # THIS BLOCK IS EVALUATED ONLY WHEN MAKE IS INVOKED IN TOPDIR
  ######################################################################

  # try to determine configuration (unless specified on the command line)
  # invoke $(SHELL) from within the $(shell) function to silence shell
  # any error messages when the compiler isn't found
  ifeq ($(CONFIG),)
    ifeq ($(shell $(SHELL) -c "g++ -v" >/dev/null 2>&1 \
                  && echo $$?),0)
      # use gcc on every OS by default
      CONFIG = gcc.config
    else
      ifeq ($(OSNAME),AIX)
        # check for VisualAge on AIX
        # avoid comparing the exit status to 0 since VAC++ return 249...
        ifneq ($(shell $(SHELL) -c "xlC -qversion" >/dev/null 2>&1; \
                       echo $$?),127)
          CONFIG = vacpp.config
        endif
      else
        ifeq ($(OSNAME),HP-UX)
          # check for aCC on HP-UX
          ifeq ($(shell $(SHELL) -c "aCC -V" >/dev/null 2>&1 && echo $$?),0)
            CONFIG = acc.config
          endif
        else
          ifeq ($(OSNAME),IRIX64)
            # check for MIPSpro on IRIX
            ifeq ($(shell $(SHELL) -c "CC -v" >/dev/null 2>&1 && echo $$?),0)
              CONFIG = mipspro.config
            endif
          else
            ifeq ($(OSNAME),OSF1)
              # check for Compaq C++ on Tru64 UNIX
              ifeq ($(shell $(SHELL) -c "cxx -V" >/dev/null 2>&1; echo $$?),0)
                CONFIG = osf_cxx.config
              endif
            else
              ifeq ($(OSNAME),SunOS)
                # check for SunPro on Solaris
                ifeq ($(shell $(SHELL) -c "CC -V" >/dev/null 2>&1 \
                              && echo $$?),0)
                  CONFIG = sunpro.config
                endif
              endif   # SunOS
            endif   # OSF1
          endif   # IRIX64
        endif   # HP-UX
      endif   # AIX
    endif   # gcc

    ifeq ($(CONFIG),)
      $(error "could not find a config file for this platform: $(OSNAME)")
    else
      $(warning "CONFIG not specified, using $(CONFIG)")
    endif

  endif   # ifeq ($(CONFIG),)

  ifeq ($(CONFIG),)
    $(error "CONFIG not defined")
  endif

  # decode the BUILDTYPE value and set BUILDMODE correspondingly
  ifeq ($(BUILDTYPE),8s)
    bmode = optimized,narrow
  endif   # ifeq ($(BUILDTYPE),8s)

  ifeq ($(BUILDTYPE),8S)
    bmode = optimized,wide
  endif   # ifeq ($(BUILDTYPE),8s)

  ifeq ($(BUILDTYPE),8d)
    bmode = shared,optimized,narrow
  endif   # ifeq ($(BUILDTYPE),8d)

  ifeq ($(BUILDTYPE),8D)
    bmode = shared,optimized,wide
  endif   # ifeq ($(BUILDTYPE),8d)

  ifeq ($(BUILDTYPE),11s)
    bmode = debug,narrow
  endif   # ifeq ($(BUILDTYPE),11s)

  ifeq ($(BUILDTYPE),11S)
    bmode = debug,wide
  endif   # ifeq ($(BUILDTYPE),11s)

  # 11d - single-thread, debug, shared
  ifeq ($(BUILDTYPE),11d)
    bmode = debug,shared,narrow
  endif   # ifeq ($(BUILDTYPE),11d)

  ifeq ($(BUILDTYPE),11D)
    bmode = debug,shared,wide
  endif   # ifeq ($(BUILDTYPE),11d)

  # 12s - multi-thread, optimized, static
  ifeq ($(BUILDTYPE),12s)
    bmode = pthreads,optimized,narrow
  endif   # ifeq ($(BUILDTYPE),12s)

  ifeq ($(BUILDTYPE),12S)
    bmode = pthreads,optimized,wide
  endif   # ifeq ($(BUILDTYPE),12s)

  # 12d - multi-thread, optimized, shared
  ifeq ($(BUILDTYPE),12d)
    bmode = pthreads,shared,optimized,narrow
  endif   # ifeq ($(BUILDTYPE),12d)

  ifeq ($(BUILDTYPE),12D)
    bmode = pthreads,shared,optimized,wide
  endif   # ifeq ($(BUILDTYPE),12d)

  # 15s - multi-thread, debug, static
  ifeq ($(BUILDTYPE),15s)
    bmode = debug,pthreads,narrow
  endif   # ifeq ($(BUILDTYPE),15s)

  ifeq ($(BUILDTYPE),15S)
    bmode = debug,pthreads,wide
  endif   # ifeq ($(BUILDTYPE),15s)

  # 15d - multi-thread, debug, shared
  ifeq ($(BUILDTYPE),15d)
    bmode = debug,pthreads,shared,narrow
  endif   # ifeq ($(BUILDTYPE),15d)

  ifeq ($(BUILDTYPE),15D)
    bmode = debug,pthreads,shared,wide
  endif   # ifeq ($(BUILDTYPE),15d)

  ifneq ($(BUILDTYPE),)
    ifeq ($(bmode),)
      $(error "unknown BUILDTYPE value: '$(BUILDTYPE)'")
    endif
  endif

  ifeq ($(TOPDIR),)
    # during the first (non-recursive) invocation only,
    # check to make sure at most one of BUILDTYPE and BUILDMODE
    # is defined
    ifneq ($(BUILDTYPE),)
      ifneq ($(BUILDMODE),)
        $(error "at most one of BUILDMODE and BUILDTYPE may be defined")
      else
        BUILDMODE=$(bmode)
      endif
    endif
    TOPDIR = $(CURDIR)
  endif

  ifeq ($(filter /%,$(CONFIG)),)
    ifeq ($(filter %/%,$(CONFIG)),)
      configpath = $(TOPDIR)/etc/config/$(CONFIG)
    else
      configpath = $(TOPDIR)/$(CONFIG)
    endif
  else
    configpath = $(CONFIG)
  endif

  include $(configpath)

  ETCDIR = $(TOPDIR)/etc/config

  ifeq ($(findstring debug,$(BUILDMODE)),debug)
    ifeq ($(findstring optimized,$(BUILDMODE)),optimized)
      $(error "cannot have both debug and optimized options")
    endif
  endif

  # unless $(LD) is set, use the same command to link as to compile
  ifeq ($(LD),ld)
    LD = $(CXX)
  endif

  ifeq ($(AS_EXT),)
    # assume the .S extension for assembly files by default,
    # unless specified otherwise in the config file
    AS_EXT = ".S"
  endif

  # debug/optimized
  ifeq ($(findstring debug,$(BUILDMODE)),debug)
    CXXFLAGS += $(DEBUG_CXXFLAGS)
    CPPFLAGS += -D_RWSTDDEBUG $(DEBUG_CPPFLAGS)
  else
    # TODO - check the number, check if the compiler supports it
    ifeq ($(findstring optimized,$(BUILDMODE)),optimized)
      CXXFLAGS  += $(OPTMZ_CXXFLAGS)
      CPPFLAGS  += $(OPTMZ_CPPFLAGS)
    endif
  endif

  ifeq ($(SHARED_SUFFIX),)
    # shared library suffix default (may be set in config file)
    SHARED_SUFFIX = .so
  endif

  ifeq ($(STATIC_SUFFIX),)
    # archive library suffix default (may be set in config file)
    STATIC_SUFFIX = .a
  endif

  # shared/archive
  ifeq ($(findstring shared,$(BUILDMODE)),shared)
    CXXFLAGS  += $(SHARED_CXXFLAGS)
    CPPFLAGS  += $(SHARED_CPPFLAGS)
    LDFLAGS   += $(SHARED_LDFLAGS)
    LIBSUFFIX  = $(SHARED_SUFFIX)
  else
    CXXFLAGS  += $(STATIC_CXXFLAGS)
    CPPFLAGS  += $(STATIC_CPPFLAGS)
    LDFLAGS   += $(STATIC_LDFLAGS)
    LIBSUFFIX  = $(STATIC_SUFFIX)

    # not applicable to non-shared builds
    PICFLAGS   =
    LDSOFLAGS  =
    RPATH      =
  endif

  # POSIX, Solaris, DCE threads
  ifeq ($(findstring pthreads,$(BUILDMODE)),pthreads)
    CPPFLAGS += $(MULTI_CPPFLAGS_POSIX)
    LDFLAGS  += $(MULTI_LDFLAGS_POSIX)
  else
    ifeq ($(findstring dcethreads,$(BUILDMODE)),dcethreads)
      ifneq ($(OSNAME),OSF1)
        $(error "DCE threads not suported on this platform:" $(OSNAME))
      endif

      CPPFLAGS += $(MULTI_CPPFLAGS_DCE)
      LDFLAGS  += $(MULTI_LDFLAGS_DCE)
    else
      ifeq ($(findstring threads,$(BUILDMODE)),threads)
        ifneq ($(OSNAME),SunOS)
          $(error "Solaris threads not suported on this platform: " $(OSNAME))
        endif

        CPPFLAGS +=  $(MULTI_CPPFLAGS_SOLARIS)
        LDFLAGS  +=  $(MULTI_LDFLAGS_SOLARIS)
      else
        CPPFLAGS += $(SINGL_CPPFLAGS)
        LDFLAGS   += $(SINGL_LDFLAGS)
      endif
    endif
  endif

  # wide (typically 64-bit) mode
  ifeq ($(findstring wide,$(BUILDMODE)),wide)
    CXXFLAGS  += $(CXXFLAGS.wide)
    LDFLAGS   += $(LDFLAGS.wide)
    LDSOFLAGS += $(LDSOFLAGS.wide)
    ARFLAGS   += $(ARFLAGS.wide)
  endif
  ifeq ($(findstring narrow,$(BUILDMODE)),narrow)
    CXXFLAGS  += $(CXXFLAGS.narrow)
    LDFLAGS   += $(LDFLAGS.narrow)
    LDSOFLAGS += $(LDSOFLAGS.narrow)
    ARFLAGS   += $(ARFLAGS.narrow)
  endif

  # platform is determined as {OS-name}-{OS-version}-{hardware}
  PLATFORM := $(shell uname -srm | sed "s/[ \/]/-/g")

  ifeq ($(OSNAME),SunOS)
    # Sun recommends to use uname -p rather than the POSIX uname -m
    PLATFORM := $(shell uname -srp | sed "s/[ \/]/-/g")
  else
    ifeq ($(OSNAME),AIX)
      PLATFORM := $(shell uname -srv | awk '{ print $$1 "-" $$3 "." $$2 }')
    else
      ifeq ($(findstring CYGWIN,$(OSNAME)),CYGWIN)
        PLATFORM := $(shell uname -sm | sed "s/[ \/]/-/g")
      endif
    endif
  endif

  # harmonize all the different Intel IA32 chips
  PLATFORM := $(subst i486,i86,$(PLATFORM))
  PLATFORM := $(subst i586,i86,$(PLATFORM))
  PLATFORM := $(subst i686,i86,$(PLATFORM))
  PLATFORM := $(shell echo $(PLATFORM) | tr "[:upper:]" "[:lower:]")

  CONFIG_H := $(CXX)-$(CCVER)-$(BUILDTYPE)

  # name of the library w/o the prefix and suffix
  # suitable for use by the linker after the -l option
  LIBBASE = std$(BUILDTYPE)

  # full library filename (including suffix)
  LIBNAME = lib$(LIBBASE)$(LIBSUFFIX)

  ifeq ($(DEPENDDIR),)
    DEPENDDIR=.depend
  endif

  # obtain library version number from the macro _RWSTD_VER
  # #defined in the rw/_config.h library header
  LIBVER := $(shell awk '/^.define _RWSTD_VER / { major = substr ($$3, 3, 2); minor = substr ($$3, 5, 2); micro = substr ($$3, 7, 2); print (major + 0) "." (minor + 0) "." (micro + 0) }' $(TOPDIR)/include/rw/_config.h)

########################################################################
#  TARGETS
########################################################################

all: libstd

# make-builddir function: creates the build directory tree
# defined as a single shell command to avoid executing each command
# in a separate subshell
define make-builddir
    [ "$(buildpath)" = "" -o -x $(buildpath) ] && exit 0;                    \
    echo "creating BUILDDIR=$(buildpath)";                                   \
    mkdir -p $(MAKEDIRS);                                                    \
    [ $$? -ne 0 ] && {                                                       \
        echo "unable to create build directory";                             \
        exit 1;                                                              \
    };                                                                       \
    ln -sf $(TOPDIR)/GNUmakefile         $(buildpath);                       \
    ln -sf $(ETCDIR)/GNUmakefile.cfg     $(buildpath)/include/GNUmakefile;   \
    ln -sf $(ETCDIR)/GNUmakefile.lib     $(LIBDIR)/GNUmakefile;              \
    ln -sf $(ETCDIR)/GNUmakefile.rwt     $(buildpath)/rwtest/GNUmakefile;    \
    ln -sf $(ETCDIR)/GNUmakefile.exm     $(EXMDIR)/GNUmakefile;              \
    ln -sf $(ETCDIR)/GNUmakefile.tst     $(TSTDIR)/GNUmakefile;              \
    ln -sf $(ETCDIR)/GNUmakefile.ph      $(PHTSTDIR)/GNUmakefile;            \
    ln -sf $(ETCDIR)/GNUmakefile.bin     $(BINDIR)/GNUmakefile;              \
    ln -sf $(ETCDIR)/makefile.common     $(buildpath);                       \
    ln -sf $(ETCDIR)/makefile.rules      $(buildpath);                       \
    ln -sf $(ETCDIR)/configure.sh        $(buildpath)/include/configure;     \
    ln -sf $(BINDIR)/exec                $(buildpath)/run;                   \
    ln -sf $(BINDIR)/exec                $(BINDIR)/run;                      \
    ln -sf $(ETCDIR)/run_locale_utils.sh $(BINDIR)/run_utils;                \
    ln -sf $(BINDIR)/exec                $(TSTDIR)/run;                      \
    ln -sf $(BINDIR)/exec                $(PHTSTDIR)/run;                    \
    ln -sf $(BINDIR)/exec                $(EXMDIR)/run
endef   # make-builddir


# create $(MAKEFILE_IN)
$(MAKEFILE_IN): $(configpath)
	@(   $(make-builddir);                                              \
             echo "generating $(MAKEFILE_IN) from $(configpath)"            \
          && echo "TOPDIR     = $(TOPDIR)"               >> $(MAKEFILE_IN)  \
          && echo "BUILDDIR   = $(buildpath)"            >> $(MAKEFILE_IN)  \
          && echo "CONFIG     = $$""(TOPDIR)/etc/config/$(CONFIG)"          \
                                                         >> $(MAKEFILE_IN)  \
          && echo "CONFIG_H   = $(CONFIG_H)"             >> $(MAKEFILE_IN)  \
          && echo "BUILDTYPE  = $(BUILDTYPE)"            >> $(MAKEFILE_IN)  \
          && echo "BUILDMODE  = $(BUILDMODE)"            >> $(MAKEFILE_IN)  \
          && echo "CXX        = $(CXX)"                  >> $(MAKEFILE_IN)  \
          && echo "CXXFLAGS   = $(CXXFLAGS)"             >> $(MAKEFILE_IN)  \
          && echo "PRELINKFLAGS = $(PRELINKFLAGS)"       >> $(MAKEFILE_IN)  \
          && echo "PICFLAGS   = $(PICFLAGS)"             >> $(MAKEFILE_IN)  \
          && echo "CPPFLAGS   = $(CPPFLAGS)"             >> $(MAKEFILE_IN)  \
          && echo "WARNFLAGS  = $(WARNFLAGS)"            >> $(MAKEFILE_IN)  \
          && echo "DEPENDFLAGS     = $(DEPENDFLAGS)"     >> $(MAKEFILE_IN)  \
          && echo "DEPENDFLAGS.cpp = $(DEPENDFLAGS.cpp)" >> $(MAKEFILE_IN)  \
          && echo "DEPENDFLAGS$(AS_EXT) = $(value DEPENDFLAGS$(AS_EXT))"    \
                                                         >> $(MAKEFILE_IN)  \
          && echo "AS_EXT     = $(AS_EXT)"               >> $(MAKEFILE_IN)  \
          && echo "LD         = $(LD)"                   >> $(MAKEFILE_IN)  \
          && echo "LDFLAGS    = $(LDFLAGS)"              >> $(MAKEFILE_IN)  \
          && echo "LDLIBS     = $(LDLIBS)"               >> $(MAKEFILE_IN)  \
          && echo "LDSOFLAGS  = $(LDSOFLAGS)"            >> $(MAKEFILE_IN)  \
          && echo "MAPFLAGS   = $(MAPFLAGS)"             >> $(MAKEFILE_IN)  \
          && echo "RPATH      = $(RPATH)"                >> $(MAKEFILE_IN)  \
          && [ "$(MAPFILE)" = "" ]                                          \
          || echo "MAPFILE    = $$""(TOPDIR)/$(MAPFILE)" >> $(MAKEFILE_IN)  \
          && echo "RUNFLAGS   = -t 300"                  >> $(MAKEFILE_IN)  \
          && echo "DEPENDDIR  = $(DEPENDDIR)"            >> $(MAKEFILE_IN)  \
          && echo "PHDIR      = $(PHDIR)"                >> $(MAKEFILE_IN)  \
          && echo "PHWARNFLAGS = $(PHWARNFLAGS)"         >> $(MAKEFILE_IN)  \
          && echo "LIBSUFFIX  = $(LIBSUFFIX)"            >> $(MAKEFILE_IN)  \
          && echo "LIBBASE    = std$$""(BUILDTYPE)"      >> $(MAKEFILE_IN)  \
          && echo "LIBVER     = $(LIBVER)"               >> $(MAKEFILE_IN)  \
          && echo "LIBNAME    = lib$$""(LIBBASE)$$""(LIBSUFFIX)"            \
                                                         >> $(MAKEFILE_IN)  \
          && echo "AR         = $(AR)"                   >> $(MAKEFILE_IN)  \
          && echo "ARFLAGS    = $(ARFLAGS)"              >> $(MAKEFILE_IN)  \
          && echo "CCVER      = $(CCVER)"                >> $(MAKEFILE_IN)  \
          && echo "SHARED     = $(SHARED)"               >> $(MAKEFILE_IN)  \
          && echo "CATFILE    = $(CATFILE)"              >> $(MAKEFILE_IN)  \
          && echo "OMIT_EXM_SRCS = $(OMIT_EXM_SRCS)"     >> $(MAKEFILE_IN)  \
          && echo "OMIT_TST_SRCS = $(OMIT_TST_SRCS)"     >> $(MAKEFILE_IN)  \
          && echo "BUILDTAG   = $(BUILDTAG)"             >> $(MAKEFILE_IN)  \
          && echo "PLATFORM   = $(PLATFORM)"             >> $(MAKEFILE_IN)  \
          && echo "DEFAULT_SHROBJ = $(DEFAULT_SHROBJ)"   >> $(MAKEFILE_IN)  \
          && echo "WITH_CADVISE = $(WITH_CADVISE)"       >> $(MAKEFILE_IN)  \
          && echo "CADVISEFLAGS = $(CADVISEFLAGS)"       >> $(MAKEFILE_IN)  \
          && echo "WITH_PURIFY = $(WITH_PURIFY)"         >> $(MAKEFILE_IN)  \
          && echo "PURIFYFLAGS = $(PURIFYFLAGS)"         >> $(MAKEFILE_IN)  \
          && echo "CXX_REPOSITORY = $(CXX_REPOSITORY)"	 >> $(MAKEFILE_IN));

# creates the build directory tree and generates makefile.in
builddir: $(MAKEFILE_IN)
	@echo "build directory created: BUILDDIR=$(BUILDDIR)"


# prevent trying to make makefile.in when $(BUILDDIR) is
# not defined and -include $(BUILDDIR)/makefile.in fails
makefile.in:


.DEFAULT:
	@$(MAKE) -C$(buildpath) $(MAKEFLAGS) $(MAKECMDGOALS) \
                 BUILDDIR=$(buildpath)


else   # ifneq ($(in_topdir),1) ##############################################

  ######################################################################
  # THIS BLOCK IS EVALUATED ONLY WHEN MAKE IS INVOKED IN BUILDDIR
  ######################################################################

all: config lib examples util rwtest tests $(PHDTSTDIR)

# configure (create config.h)
config:
	@$(MAKE) -C$(BUILDDIR)/include

# build utilities
util:
	@$(MAKE) -C$(BINDIR)

# build library
lib:
	@$(MAKE) -C$(LIBDIR)

# build the test library
rwtest:
	@$(MAKE) -C$(BUILDDIR)/rwtest

# build tests, ignore failures
tests:
	-$(MAKE) -C$(TSTDIR)

# build the Plum Hall test suite, ignore failures
phtst:
	-@$(MAKE) -C$(PHTSTDIR)

# make examples, ignore failures
examples:
	-@$(MAKE) -C$(EXMDIR)

# install library, headers, and utilities
install:
	mkdir -p $(PREFIX)
	$(MAKE) -Clib $@
	$(MAKE) -Cbin $@
	mkdir -p $(PREFIX)/include
	cp include/config.h $(PREFIX)/include
	cp -Rf $(TOPDIR)/include $(PREFIX)

# try each submakefile
.DEFAULT:
	-@$(MAKE) -C$(BUILDDIR)/include $@
	-@$(MAKE) -C$(LIBDIR) $@
	-@$(MAKE) -C$(BUILDDIR)/rwtest $@
	-@$(MAKE) -C$(BUILDDIR)/bin $@
	-@$(MAKE) -C$(TSTDIR) $@
	-@$(MAKE) -C$(PHTSTDIR) $@
	-@$(MAKE) -C$(EXMDIR) $@

endif   # ($(CURDIR),$(TOPDIR))


.PHONY: all bin builddir examples install lib phtst post rwtest tests util
