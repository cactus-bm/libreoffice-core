#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

GUI := UNX
COM := GCC

gb_MKTEMP := mktemp -t gbuild.XXXXXX

gb_CC := cc
gb_CXX := g++
gb_GCCP := gcc
gb_AR := ar
gb_AWK := awk
gb_CLASSPATHSEP := :

# normalize setsolar and configure env.
ifeq ($(CPU),X)
CPUNAME := X86_64
endif

# use CC/CXX if they are nondefaults
ifneq ($(origin CC),default)
gb_CC := $(CC)
gb_GCCP := $(CC)
endif
ifneq ($(origin CXX),default)
gb_CXX := $(CXX)
endif

gb_OSDEFS := \
    -D$(OS) \
    -D_PTHREADS \
    -DUNIX \
    -DUNX \
    $(PTHREAD_CFLAGS) \

ifeq ($(GXX_INCLUDE_PATH),)
GXX_INCLUDE_PATH=$(COMPATH)/include/c++/$(shell gcc -dumpversion)
endif

gb_COMPILERDEFS := \
    -D$(COM) \
    -DHAVE_GCC_VISIBILITY_FEATURE \
    -DCPPU_ENV=gcc3 \
    -DGXX_INCLUDE_PATH=$(GXX_INCLUDE_PATH) \

ifeq ($(CPUNAME),X86_64)
gb_CPUDEFS := -D$(CPUNAME)
else
gb_CPUDEFS := -DX86
endif

gb_CFLAGS := \
    -Wall \
    -Wendif-labels \
    -Wextra \
    -Wshadow \
    -fPIC \
    -fmessage-length=0 \
    -fno-common \
    -fno-strict-aliasing \
    -fvisibility=hidden \
    -pipe \

gb_CXXFLAGS := \
    -Wall \
    -Wendif-labels \
    -Wextra \
    -Wno-ctor-dtor-privacy \
    -Wno-non-virtual-dtor \
    -Wshadow \
    -fPIC \
    -fmessage-length=0 \
    -fno-common \
    -fno-strict-aliasing \
    -fno-use-cxa-atexit \
    -fvisibility-inlines-hidden \
    -fvisibility=hidden \
    -pipe \

ifneq ($(EXTERNAL_WARNINGS_NOT_ERRORS),TRUE)
gb_CFLAGS_WERROR := -Werror
gb_CXXFLAGS_WERROR := -Werror
endif

ifneq ($(strip $(SYSBASE)),)
gb_CXXFLAGS += --sysroot=$(SYSBASE)
gb_CFLAGS += --sysroot=$(SYSBASE)
endif
gb_LinkTarget_EXCEPTIONFLAGS := \
    -DEXCEPTIONS_ON \
    -fexceptions \
    -fno-enforce-eh-specs \

gb_LinkTarget_NOEXCEPTIONFLAGS := \
    -DEXCEPTIONS_OFF \
    -fno-exceptions \

gb_LinkTarget_LDFLAGS := \
    -Wl,--sysroot=$(SYSBASE) \
    -Wl,-rpath-link,$(SYSBASE)/lib:$(SYSBASE)/usr/lib \
    -Wl,-z,combreloc \
    -Wl,-z,defs \
    $(subst -L../lib , ,$(SOLARLIB)) \

ifeq ($(HAVE_LD_HASH_STYLE),TRUE)
gb_LinkTarget_LDFLAGS += \
    -Wl,--hash-style=both \

endif

ifneq ($(HAVE_LD_BSYMBOLIC_FUNCTIONS),)
gb_LinkTarget_LDFLAGS += \
    -Wl,--dynamic-list-cpp-new \
    -Wl,--dynamic-list-cpp-typeinfo \
    -Wl,-Bsymbolic-functions \

endif

ifeq ($(gb_DEBUGLEVEL),2)
gb_COMPILEROPTFLAGS := -O0
else
gb_COMPILEROPTFLAGS := -Os
endif

gb_COMPILERNOOPTFLAGS := -O0

# Helper class

gb_Helper_abbreviate_dirs_native = $(gb_Helper_abbreviate_dirs)


# CObject class

define gb_CObject__command
$(call gb_Output_announce,$(2),$(true),C  ,3)
$(call gb_Helper_abbreviate_dirs,\
    mkdir -p $(dir $(1)) && \
    mkdir -p $(dir $(call gb_CObject_get_dep_target,$(2))) && \
    $(gb_CC) \
        $(DEFS) $(CFLAGS) \
        -c $(3) \
        -o $(1) \
        -MMD -MT $(call gb_CObject_get_target,$(2)) \
        -MF $(call gb_CObject_get_dep_target,$(2)) \
        -I$(dir $(3)) \
        $(INCLUDE))
endef


# CxxObject class

define gb_CxxObject__command
$(call gb_Output_announce,$(2),$(true),CXX,3)
$(call gb_Helper_abbreviate_dirs,\
    mkdir -p $(dir $(1)) && \
    mkdir -p $(dir $(call gb_CxxObject_get_dep_target,$(2))) && \
    $(gb_CXX) \
        $(DEFS) $(CXXFLAGS) \
        -c $(3) \
        -o $(1) \
        -MMD -MT $(call gb_CxxObject_get_target,$(2)) \
        -MF $(call gb_CxxObject_get_dep_target,$(2)) \
        -I$(dir $(3)) \
        $(INCLUDE_STL) $(INCLUDE))
endef


# LinkTarget class

define gb_LinkTarget__get_rpath_for_layer
$(patsubst $(1):%,%,$(filter $(1):%,$(gb_LinkTarget__RPATHS)))
endef

gb_LinkTarget__RPATHS := \
    URELIB:\dORIGIN \
    UREBIN:\dORIGIN/../lib:\dORIGIN \
    OOO:\dORIGIN:\dORIGIN/../ure-link/lib \
    BRAND:\dORIGIN:\dORIGIN/../basis-link/program:\dORIGIN/../basis-link/ure-link/lib \
    SDKBIN:\dORIGIN/../../ure-link/lib \
    NONEBIN:\dORIGIN/../lib:\dORIGIN \

gb_LinkTarget_CFLAGS := $(gb_CFLAGS) $(gb_CFLAGS_WERROR) $(gb_COMPILEROPTFLAGS)
gb_LinkTarget_CXXFLAGS := $(gb_CXXFLAGS) $(gb_CXXFLAGS_WERROR)

ifeq ($(gb_DEBUGLEVEL),2)
gb_LinkTarget_CXXFLAGS += -ggdb3 -finline-limit=0 -fno-inline -fno-default-inline
gb_LinkTarget_CFLAGS += -ggdb3 -finline-limit=0 -fno-inline -fno-default-inline

endif

gb_LinkTarget_INCLUDE := $(filter-out %/stl, $(subst -I. , ,$(SOLARINC)))
gb_LinkTarget_INCLUDE_STL := $(filter %/stl, $(subst -I. , ,$(SOLARINC)))

define gb_LinkTarget__command_dynamiclink
$(call gb_Helper_abbreviate_dirs,\
    mkdir -p $(dir $(1)) && \
    $(gb_CXX) \
        $(if $(filter Library CppunitTest,$(TARGETTYPE)),$(gb_Library_TARGETTYPEFLAGS)) \
        $(subst \d,$$,$(RPATH)) $(LDFLAGS) \
        $(patsubst lib%.so,-l%,$(foreach lib,$(LINKED_LIBS),$(call gb_Library_get_filename,$(lib)))) \
        $(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
        $(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
        $(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
        -Wl$(COMMA)--start-group $(foreach lib,$(LINKED_STATIC_LIBS),$(call gb_StaticLibrary_get_target,$(lib))) -Wl$(COMMA)--end-group \
        -o $(1))
endef

define gb_LinkTarget__command_staticlink
$(call gb_Helper_abbreviate_dirs,\
    mkdir -p $(dir $(1)) && \
    $(gb_AR) -rsu $(1) \
        $(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
        $(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
        $(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
        2> /dev/null)
endef

define gb_LinkTarget__command
$(call gb_Output_announce,$(2),$(true),LNK,4)
$(if $(filter Library CppunitTest Executable,$(TARGETTYPE)),$(call gb_LinkTarget__command_dynamiclink,$(1)))
$(if $(filter StaticLibrary,$(TARGETTYPE)),$(call gb_LinkTarget__command_staticlink,$(1)))
endef


# Library class

gb_Library_DEFS :=
gb_Library_TARGETTYPEFLAGS := -shared -Wl,-z,noexecstack
gb_Library_SYSPRE := lib
gb_Library_UNOVERPRE := $(gb_Library_SYSPRE)uno_
gb_Library_PLAINEXT := .so
gb_Library_RTEXT := gcc3$(gb_Library_PLAINEXT)
ifeq ($(gb_PRODUCT),$(true))
gb_Library_STLEXT := port_gcc$(gb_Library_PLAINEXT)
else
gb_Library_STLEXT := port_gcc_stldebug$(gb_Library_PLAINEXT)
endif

ifeq ($(CPUNAME),X86_64)
gb_Library_OOOEXT := lx$(gb_Library_PLAINEXT)
gb_Library_UNOEXT := .uno$(gb_Library_PLAINEXT)
else
gb_Library_OOOEXT := li$(gb_Library_PLAINEXT)
gb_Library_UNOEXT := .uno$(gb_Library_PLAINEXT)
endif

gb_Library_PLAINLIBS_NONE += \
    dl \
    jpeg \
    m \
    pthread \
    X11 \
    z

gb_Library_FILENAMES := \
    $(foreach lib,$(gb_Library_OOOLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_OOOEXT)) \
    $(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
    $(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
    $(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
    $(foreach lib,$(gb_Library_RTLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_RTEXT)) \
    $(foreach lib,$(gb_Library_RTVERLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_RTEXT)) \
    $(foreach lib,$(gb_Library_STLLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_STLEXT)) \
    $(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):$(lib)$(gb_Library_UNOEXT)) \
    $(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):$(lib)$(gb_Library_UNOEXT)) \
    $(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):$(gb_Library_UNOVERPRE)$(lib)$(gb_Library_PLAINEXT)) \


gb_Library_LAYER := \
    $(foreach lib,$(gb_Library_OOOLIBS),$(lib):OOO) \
    $(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):URELIB) \
    $(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):OOO) \
    $(foreach lib,$(gb_Library_RTLIBS),$(lib):OOO) \
    $(foreach lib,$(gb_Library_RTVERLIBS),$(lib):URELIB) \
    $(foreach lib,$(gb_Library_STLLIBS),$(lib):URELIB) \
    $(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):URELIB) \
    $(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):OOO) \
    $(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):URELIB) \

define gb_Library_get_rpath
'-Wl,-rpath,$(call gb_LinkTarget__get_rpath_for_layer,$(call gb_Library_get_layer,$(1)))' \
'-Wl,-rpath-link,$(gb_Library_OUTDIRLOCATION)'
endef

define gb_Library_Library_platform
$(call gb_LinkTarget_get_target,$(2)) : RPATH := $(call gb_Library_get_rpath,$(1))

endef


# StaticLibrary class

gb_StaticLibrary_DEFS :=
gb_StaticLibrary_SYSPRE := lib
gb_StaticLibrary_PLAINEXT := .a
gb_StaticLibrary_JPEGEXT := lib$(gb_StaticLibrary_PLAINEXT)

gb_StaticLibrary_FILENAMES := \
    $(foreach lib,$(gb_StaticLibrary_JPEGLIBS),$(lib):$(gb_StaticLibrary_SYSPRE)$(lib)$(gb_StaticLibrary_JPEGEXT)) \
    $(foreach lib,$(gb_StaticLibrary_PLAINLIBS),$(lib):$(gb_StaticLibrary_SYSPRE)$(lib)$(gb_StaticLibrary_PLAINEXT)) \

gb_StaticLibrary_StaticLibrary_platform =


# Executable class

gb_Executable_EXT :=

gb_Executable_LAYER := \
    $(foreach exe,$(gb_Executable_UREBIN),$(exe):UREBIN) \
    $(foreach exe,$(gb_Executable_SDK),$(exe):SDKBIN) \
    $(foreach exe,$(gb_Executable_OOO),$(exe):OOO) \
    $(foreach exe,$(gb_Executable_BRAND),$(exe):BRAND) \
    $(foreach exe,$(gb_Executable_NONE),$(exe):NONEBIN) \


define gb_Executable_get_rpath
'-Wl,-rpath,$(call gb_LinkTarget__get_rpath_for_layer,$(call gb_Executable_get_layer,$(1)))' \
-Wl,-rpath-link,$(gb_Library_OUTDIRLOCATION)
endef

define gb_Executable_Executable_platform
$(call gb_LinkTarget_get_target,$(2)) : RPATH := $(call gb_Executable_get_rpath,$(1))

endef


# CppunitTest class

gb_CppunitTest_CPPTESTPRECOMMAND := LD_LIBRARY_PATH=$(OUTDIR)/lib
gb_CppunitTest_SYSPRE := libtest_
gb_CppunitTest_EXT := .so
gb_CppunitTest_get_filename = $(gb_CppunitTest_SYSPRE)$(1)$(gb_CppunitTest_EXT)
gb_CppunitTest_get_libfilename = $(gb_CppunitTest_get_filename)

define gb_CppunitTest_CppunitTest_platform
$(call gb_LinkTarget_get_target,$(2)) : RPATH :=

endef

# JunitTest class

define gb_JunitTest_JunitTest_platform
$(call gb_JunitTest_get_target,$(1)) : DEFS := \
    -Dorg.openoffice.test.arg.soffice="$$$${OOO_TEST_SOFFICE:-path:$(OUTDIR)/installation/opt/openoffice.org3/program/soffice}" \
    -Dorg.openoffice.test.arg.env=LD_LIBRARY_PATH \
    -Dorg.openoffice.test.arg.user=file://$(call gb_JunitTest_get_userdir,$(1)) \

endef

# SdiTarget class

gb_SdiTarget_SVIDLPRECOMMAND := LD_LIBRARY_PATH=$(OUTDIR)/lib

# SrsPartMergeTarget

gb_SrsPartMergeTarget_TRANSEXPRECOMMAND := LD_LIBRARY_PATH=$(OUTDIR)/lib

# SrsPartTarget class

gb_SrsPartTarget_RSCTARGET := $(OUTDIR)/bin/rsc
gb_SrsPartTarget_RSCCOMMAND := LD_LIBRARY_PATH=$(OUTDIR)/lib SOLARBINDIR=$(OUTDIR)/bin $(gb_SrsPartTarget_RSCTARGET)

define gb_SrsPartTarget__command_dep
$(call gb_Helper_abbreviate_dirs,\
    $(gb_GCCP) \
        -MM -MT $(call gb_SrsPartTarget_get_target,$(1)) \
        $(INCLUDE) \
        $(DEFS) \
        -c -x c++-header $(2) \
        -o $(call gb_SrsPartTarget_get_dep_target,$(1)))
endef


# ComponentTarget

gb_XSLTPROCPRECOMMAND := LD_LIBRARY_PATH=$(OUTDIR)/lib
gb_Library_COMPONENTPREFIXES := \
    OOO:vnd.sun.star.expand:\dOOO_BASE_DIR/program/ \
    URELIB:vnd.sun.star.expand:\dURE_INTERNAL_LIB_DIR/ \


# vim: set noet sw=4 ts=4:
