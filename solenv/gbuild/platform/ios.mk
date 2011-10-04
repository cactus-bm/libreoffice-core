# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
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

# Darwin mktemp -t expects a prefix, not a pattern
gb_MKTEMP := /usr/bin/mktemp -t gbuild.

ifeq ($(CC),)
$(error You must set CC in the environment. See README.cross for example.)
endif
ifeq ($(CXX),)
$(error You must set CXX in the environment. See README.cross for example.)
endif

gb_CC := $(CC)
gb_CXX := $(CXX)
gb_GCCP := $(CC)
gb_AR := ar
gb_AWK := awk
gb_CLASSPATHSEP := :
gb_YACC := bison

gb_OSDEFS := \
	-D$(OS) \
	-D_PTHREADS \
	-DUNIX \
	-DUNX \
	-D_REENTRANT \
	-DNO_PTHREAD_PRIORITY \
	$(EXTRA_CDEFS) \

gb_COMPILERDEFS := \
	-D$(COM) \
	-DCPPU_ENV=gcc3 \
	-DGXX_INCLUDE_PATH=$(GXX_INCLUDE_PATH) \

gb_CPUDEFS := -DARM32

gb_CFLAGS := \
	-Wall \
	-Wendif-labels \
	-Wextra \
	-Wshadow \
	-fmessage-length=0 \
	-fno-common \
	-fno-strict-aliasing \
	-pipe \

gb_CXXFLAGS := \
	-Wall \
	-Wendif-labels \
	-Wextra \
	-Wno-ctor-dtor-privacy \
	-Wno-non-virtual-dtor \
	-fmessage-length=0 \
	-fno-common \
	-fno-strict-aliasing \
	-fsigned-char \
	-malign-natural \
	-pipe \
	#-Wshadow \ break in compiler headers already
	#-fsigned-char \ might be removed?
	#-malign-natural \ might be removed?

# these are to get gcc to switch to Objective-C++ or Objective-C mode
gb_OBJC_OBJCXX_COMMON_FLAGS := -fobjc-abi-version=2 -fobjc-legacy-dispatch -D__IPHONE_OS_VERSION_MIN_REQUIRED=40300

gb_OBJCXXFLAGS := -x objective-c++ $(gb_OBJC_OBJCXX_COMMON_FLAGS)

gb_OBJCFLAGS := -x objective-c $(gb_OBJC_OBJCXX_COMMON_FLAGS)

ifneq ($(EXTERNAL_WARNINGS_NOT_ERRORS),TRUE)
gb_CFLAGS_WERROR := -Werror -DLIBO_WERROR
gb_CXXFLAGS_WERROR := -Werror -DLIBO_WERROR
endif

gb_LinkTarget_EXCEPTIONFLAGS := \
	-DEXCEPTIONS_ON \
	-fexceptions \
	-fno-enforce-eh-specs \

gb_LinkTarget_NOEXCEPTIONFLAGS := \
	-DEXCEPTIONS_OFF \
	-fno-exceptions \

gb_LinkTarget_LDFLAGS := \
	$(subst -L../lib , ,$(SOLARLIB)) \
#man ld says: obsolete	-Wl,-multiply_defined,suppress \

ifneq ($(gb_DEBUGLEVEL),0)
gb_COMPILEROPTFLAGS := -O0
else
gb_COMPILEROPTFLAGS := -O2
endif

gb_COMPILERNOOPTFLAGS := -O0

# Helper class

gb_Helper_abbreviate_dirs_native = $(gb_Helper_abbreviate_dirs)

gb_Helper_set_ld_path := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib

# convert parameters filesystem root to native notation
# does some real work only on windows, make sure not to
# break the dummy implementations on unx*
define gb_Helper_convert_native
$(1)
endef

gb_Helper_OUTDIRLIBDIR := $(OUTDIR)/lib

# YaccObject class

define gb_YaccObject__command
$(call gb_Output_announce,$(2),$(true),YAC,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(3)) && \
	$(gb_YACC) $(T_YACCFLAGS) --defines=$(4) -o $(3) $(1) )

endef


# CObject class

define gb_CObject__command
$(call gb_Output_announce,$(2),$(true),C  ,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	$(gb_CC) \
		$(DEFS) \
		$(T_CFLAGS) \
		-c $(3) \
		-o $(1) \
		-MMD -MT $(1) \
		-MP -MF $(4) \
		-I$(dir $(3)) \
		$(INCLUDE))
endef


# CxxObject class

# N.B: $(T_CXXFLAGS) may contain -x objective-c++, which must come before -c
define gb_CxxObject__command
$(call gb_Output_announce,$(2),$(true),CXX,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	$(gb_CXX) \
		$(DEFS) \
		$(T_CXXFLAGS) \
		-c $(3) \
		-o $(1) \
		-MMD -MT $(1) \
		-MP -MF $(4) \
		-I$(dir $(3)) \
		$(INCLUDE_STL) $(INCLUDE))
endef


# ObjCxxObject class

define gb_ObjCxxObject__command
$(call gb_Output_announce,$(2),$(true),OCX,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	$(gb_CXX) \
		$(DEFS) \
		$(T_OBJCXXFLAGS) \
		-c $(3) \
		-o $(1) \
		-MMD -MT $(1) \
		-MP -MF $(4) \
		-I$(dir $(3)) \
		$(INCLUDE_STL) $(INCLUDE))
endef

# ObjCObject class

define gb_ObjCObject__command
$(call gb_Output_announce,$(2),$(true),OCC,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	mkdir -p $(dir $(call gb_ObjCObject_get_dep_target,$(2))) && \
	$(gb_CC) \
		$(DEFS) $(OBJCFLAGS) \
		-c $(3) \
		-o $(1) \
		-MMD -MT $(call gb_ObjCObject_get_target,$(2)) \
		-MP -MF $(call gb_ObjCObject_get_dep_target,$(2)) \
		-I$(dir $(3)) \
		$(INCLUDE_STL) $(INCLUDE))
endef


# LinkTarget class

gb_LinkTarget_CFLAGS := $(gb_CFLAGS) $(gb_CFLAGS_WERROR) $(gb_COMPILEROPTFLAGS)
gb_LinkTarget_CXXFLAGS := $(gb_CXXFLAGS) $(gb_CXXFLAGS_WERROR)
gb_LinkTarget_OBJCXXFLAGS := $(gb_CXXFLAGS) $(gb_CXXFLAGS_WERROR) $(gb_OBJCXXFLAGS) $(gb_COMPILEROPTFLAGS)
gb_LinkTarget_OBJCFLAGS := $(gb_CFLAGS) $(gb_OBJCFLAGS) $(gb_COMPILEROPTFLAGS)

ifeq ($(gb_SYMBOL),$(true))
gb_LinkTarget_CFLAGS += -g
gb_LinkTarget_CXXFLAGS += -g
gb_LinkTarget_OBJCXXFLAGS += -g
gb_LinkTarget_OBJCFLAGS += -g
endif

gb_LinkTarget_INCLUDE := $(filter-out %/stl, $(subst -I. , ,$(SOLARINC)))
gb_LinkTarget_INCLUDE_STL := $(filter %/stl, $(subst -I. , ,$(SOLARINC)))

# FIXME framework handling very hackish
define gb_LinkTarget__get_liblinkflags
$(patsubst lib%.a,-l%,$(foreach lib,$(filter-out $(gb_Library__FRAMEWORKS) $(gb_Library_UNOLIBS_OOO),$(1)),$(call gb_Library_get_filename,$(lib)))) \
$(foreach lib,$(filter $(gb_Library_UNOLIBS_OOO),$(1)),$(SOLARVER)/$(INPATH)/lib/$(lib)$(gb_Library_UNOEXT)) \
$(addprefix -framework ,$(filter $(gb_Library__FRAMEWORKS),$(1)))
endef

define gb_LinkTarget__get_layer
$(if $(filter Executable,$(1)),\
	$$(call gb_Executable_get_layer,$(2)),\
	$$(call gb_Library_get_layer,$(2)))
endef


define gb_LinkTarget__command_dynamiclink
	$(if $(filter Library CppunitTest,$(TARGETTYPE)),@echo No dynamic libraries should be built for iOS && exit 1, \
	mkdir -p $(dir $(1)) && \
	$(gb_CXX) \
		$(gb_Executable_TARGETTYPEFLAGS) \
		$(subst \d,$$,$(RPATH)) \
		$(T_LDFLAGS) \
		$(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
		$(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
		$(foreach object,$(OBJCOBJECTS),$(call gb_ObjCObject_get_target,$(object))) \
		$(foreach object,$(OBJCXXOBJECTS),$(call gb_ObjCxxObject_get_target,$(object))) \
		$(foreach object,$(GENCOBJECTS),$(call gb_GenCObject_get_target,$(object))) \
		$(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
		$(foreach extraobjectlist,$(EXTRAOBJECTLISTS),`cat $(extraobjectlist)`) \
		$(foreach lib,$(LINKED_STATIC_LIBS),$(call gb_StaticLibrary_get_target,$(lib))) \
		$(call gb_LinkTarget__get_liblinkflags,$(LINKED_LIBS)) \
		$(LIBS) \
		-o $(1))
endef

# parameters: 1-linktarget 2-cobjects 3-cxxobjects
define gb_LinkTarget__command_staticlink
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(gb_AR) -rsu $(1) \
		$(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
		$(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
		$(foreach object,$(OBJCOBJECTS),$(call gb_ObjCObject_get_target,$(object))) \
		$(foreach object,$(OBJCXXOBJECTS),$(call gb_ObjCxxObject_get_target,$(object))) \
		$(foreach object,$(GENCOBJECTS),$(call gb_GenCObject_get_target,$(object))) \
		$(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
		$(foreach extraobjectlist,$(EXTRAOBJECTLISTS),@$(extraobjectlist)) \
		$(if $(findstring s,$(MAKEFLAGS)),2> /dev/null))
endef

define gb_LinkTarget__command
$(call gb_Output_announce,$(2),$(true),LNK,4)
$(if $(filter CppunitTest Executable,$(TARGETTYPE)),$(call gb_LinkTarget__command_dynamiclink,$(1),$(2)))
$(if $(filter Library StaticLibrary,$(TARGETTYPE)),$(call gb_LinkTarget__command_staticlink,$(1)))
endef


# Library class

gb_Library_DEFS :=
gb_Library_SYSPRE := lib
gb_Library_UNOVERPRE := $(gb_Library_SYSPRE)uno_
gb_Library_PLAINEXT := .a
gb_Library_DLLEXT := .a
gb_Library_RTEXT := gcc3$(gb_Library_PLAINEXT)

gb_Library_OOOEXT := $(gb_Library_DLLPOSTFIX)$(gb_Library_PLAINEXT)
gb_Library_UNOEXT := .uno$(gb_Library_PLAINEXT)

gb_Library__FRAMEWORKS := \
    Foundation \
    CoreFoundation \
    CoreGraphics \

gb_Library_PLAINLIBS_NONE += \
    objc \
	jpeg \
	m \
	pthread \
	z \

gb_Library_FILENAMES := \
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_OOOEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_RTEXT)) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_RTEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):$(lib)$(gb_Library_UNOEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):$(lib)$(gb_Library_UNOEXT)) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):$(gb_Library_UNOVERPRE)$(lib)$(gb_Library_PLAINEXT)) \


gb_Library_LAYER := \
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):OOO) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):OOO) \

define gb_Library_Library_platform
$(call gb_LinkTarget_get_target,$(2)) : LAYER := $(call gb_Library_get_layer,$(1))

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
gb_Executable_TARGETTYPEFLAGS := -dead_strip

gb_Executable_LAYER := \
	$(foreach exe,$(gb_Executable_UREBIN),$(exe):OOO) \
	$(foreach exe,$(gb_Executable_SDK),$(exe):OOO) \
	$(foreach exe,$(gb_Executable_OOO),$(exe):OOO) \
	$(foreach exe,$(gb_Executable_BRAND),$(exe):OOO) \
	$(foreach exe,$(gb_Executable_NONE),$(exe):OOO) \


define gb_Executable_Executable_platform
$(call gb_LinkTarget_get_target,$(2)) : LAYER := $(call gb_Executable_get_layer,$(1))

endef


# CppunitTest class

gb_CppunitTest_CPPTESTPRECOMMAND := :
gb_CppunitTest_SYSPRE := libtest_
gb_CppunitTest_EXT := .dylib
gb_CppunitTest_LIBDIR := $(gb_Helper_OUTDIRLIBDIR)
gb_CppunitTest_get_filename = $(gb_CppunitTest_SYSPRE)$(1)$(gb_CppunitTest_EXT)
gb_CppunitTest_get_libfilename = $(gb_CppunitTest_get_filename)

define gb_CppunitTest_CppunitTest_platform
$(call gb_LinkTarget_get_target,$(2)) : LAYER := NONE

endef

# JunitTest class

define gb_JunitTest_JunitTest_platform
$(call gb_JunitTest_get_target,$(1)) : DEFS := \
	-Dorg.openoffice.test.arg.soffice="$$$${OOO_TEST_SOFFICE:-path:$(OUTDIR)/installation/opt/LibreOffice.app/Contents/MacOS/soffice}" \
	-Dorg.openoffice.test.arg.env=DYLD_LIBRARY_PATH \
	-Dorg.openoffice.test.arg.user=file://$(call gb_JunitTest_get_userdir,$(1)) \

endef

# SdiTarget class

gb_SdiTarget_SVIDLPRECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib

# SrsPartMergeTarget

gb_SrsPartMergeTarget_TRANSEXPRECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib

# SrsPartTarget class

gb_SrsPartTarget_RSCTARGET := $(OUTDIR_FOR_BUILD)/bin/rsc
gb_SrsPartTarget_RSCCOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_SrsPartTarget_RSCTARGET)

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

gb_XSLTPROCPRECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib

# UnoApiTarget

gb_UnoApiTarget_IDLCTARGET := $(OUTDIR_FOR_BUILD)/bin/idlc
gb_UnoApiTarget_IDLCCOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_IDLCTARGET)
gb_UnoApiTarget_REGMERGETARGET := $(OUTDIR_FOR_BUILD)/bin/regmerge
gb_UnoApiTarget_REGMERGECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_REGMERGETARGET)
gb_UnoApiTarget_REGCOMPARETARGET := $(OUTDIR_FOR_BUILD)/bin/regcompare
gb_UnoApiTarget_REGCOMPARECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_REGCOMPARETARGET)
gb_UnoApiTarget_CPPUMAKERTARGET := $(OUTDIR_FOR_BUILD)/bin/cppumaker
gb_UnoApiTarget_CPPUMAKERCOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_CPPUMAKERTARGET)
gb_UnoApiTarget_REGVIEWTARGET := $(OUTDIR_FOR_BUILD)/bin/regview
gb_UnoApiTarget_REGVIEWCOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_REGVIEWTARGET)

# Configuration
gb_CFGEXPRECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib

# Python
gb_PYTHON_PRECOMMAND := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib

# vim: set noet sw=4:
