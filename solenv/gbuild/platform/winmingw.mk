# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2009 by Sun Microsystems, Inc.
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.	If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

GUI := WNT
COM := GCC

gb_TMPDIR:=$(if $(TMPDIR),$(TMPDIR),/tmp)
gb_MKTEMP := mktemp --tmpdir=$(gb_TMPDIR) gbuild.XXXXXX

gb_CC := $(CC)
gb_CXX := $(CXX)
gb_AR := $(shell $(CC) -print-prog-name=ar)
gb_AWK := awk
gb_CLASSPATHSEP := :
gb_YACC := bison
gb_RC := $(WINDRES)

gb_OSDEFS := \
	-DWINVER=0x0500 \
	-D_WIN32_IE=0x0500 \
	-DNT351 \
	-DWIN32 \
	-DWNT \

ifeq ($(GXX_INCLUDE_PATH),)
GXX_INCLUDE_PATH=$(COMPATH)/include/c++/$(shell gcc -dumpversion)
endif

gb_COMPILERDEFS := \
	-DGCC \
	-DGXX_INCLUDE_PATH=$(GXX_INCLUDE_PATH) \
	-DCPPU_ENV=gcc3 \
	-D_MT \
	-D_NATIVE_WCHAR_T_DEFINED \
	-D_MSC_EXTENSIONS \
	-D_FORCENAMELESSUNION \

gb_CPUDEFS := -D_M_IX86

gb_RCDEFS := \
	 -DWINVER=0x0400 \
	 -DWIN32 \

gb_RCFLAGS := \
	 -V

gb_CFLAGS := \
	-Wall \
	-Wendif-labels \
	-Wextra \
	-fmessage-length=0 \
	-fno-strict-aliasing \
	-pipe \

gb_CXXFLAGS := \
	-Wall \
	-Wendif-labels \
	-Wextra \
	-Wno-ctor-dtor-privacy \
	-Wno-non-virtual-dtor \
	-Wreturn-type \
	-Wshadow \
	-Wuninitialized \
	-fmessage-length=0 \
	-fno-strict-aliasing \
	-pipe \

ifneq ($(EXTERNAL_WARNINGS_NOT_ERRORS),TRUE)
gb_CFLAGS_WERROR := -Werror
gb_CXXFLAGS_WERROR := -Werror
endif

ifneq ($(SYSBASE),)
gb_CXXFLAGS += --sysroot=$(SYSBASE)
gb_CFLAGS += --sysroot=$(SYSBASE)
endif
gb_LinkTarget_EXCEPTIONFLAGS := \
	-DEXCEPTIONS_ON \
	-fexceptions \
	-fno-enforce-eh-specs \

gb_PrecompiledHeader_EXCEPTIONFLAGS := $(gb_LinkTarget_EXCEPTIONFLAGS)


gb_LinkTarget_NOEXCEPTIONFLAGS := \
	-DEXCEPTIONS_OFF \
	-fno-exceptions \

gb_NoexPrecompiledHeader_NOEXCEPTIONFLAGS := $(gb_LinkTarget_NOEXCEPTIONFLAGS)

gb_LinkTarget_LDFLAGS := \
	-Wl,--export-all-symbols \
	-Wl,--enable-stdcall-fixup \
	-Wl,--enable-runtime-pseudo-reloc-v2 \
	$(subst -L../lib , ,$(SOLARLIB)) \

ifeq ($(MINGW_GCCLIB_EH),YES)
gb_LinkTarget_LDFLAGS += -shared-libgcc
endif

gb_DEBUG_CFLAGS := -ggdb3 -finline-limit=0 -fno-inline -fno-default-inline
ifneq ($(gb_DEBUGLEVEL),0)

gb_COMPILEROPTFLAGS := -O0
else
gb_COMPILEROPTFLAGS := -Os
endif

gb_COMPILERNOOPTFLAGS := -O0

gb_STDLIBS := \
	mingwthrd \
	mingw32 \
	uwinapi \
	mingwex \


# Helper class

# For LibreOffice, MinGW is always cross-compilation, so the "native"
# platform for the BUILD *is* Unix. No Cygwin/Win32 stuff needed.

gb_Helper_SRCDIR_NATIVE := $(SRCDIR)
gb_Helper_WORKDIR_NATIVE := $(WORKDIR)
gb_Helper_OUTDIR_NATIVE := $(OUTDIR)
gb_Helper_REPODIR_NATIVE := $(REPODIR)

gb_Helper_abbreviate_dirs_native = $(gb_Helper_abbreviate_dirs)

# Set the proper enirotment variable so that our BUILD platform
# build-time shared libraries are found.
ifeq ($(OS_FOR_BUILD),MACOSX)
gb_Helper_set_ld_path := DYLD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib
else
gb_Helper_set_ld_path := LD_LIBRARY_PATH=$(OUTDIR_FOR_BUILD)/lib
endif

# Convert parameters filesystem root to native notation
# does some real work only on Windows, and this file is for
# cross-compilation.
define gb_Helper_convert_native
$(1)
endef

# YaccObject class

define gb_YaccObject__command
$(call gb_Output_announce,$(2),$(true),YAC,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(3)) && \
	$(gb_YACC) $(T_YACCFLAGS) --defines=$(4) -o $(3) $(1) )

endef

# CObject class

# We (LibreOffice) support MinGW only for cross-compilation. This is identical to
# gb_CObject__command in unxgcc.mk

# $(call gb_CObject__command,object,relative-source,source,dep-file)
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

# $(call gb_CxxObject__command,object,relative-source,source,dep-file)
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


# PrecompiledHeader class

gb_PrecompiledHeader_EXT := .gch

gb_PrecompiledHeader_get_enableflags = -I$(WORKDIR)/PrecompiledHeader/$(gb_PrecompiledHeader_DEBUGDIR) \
					-DPRECOMPILED_HEADERS \
					-Winvalid-pch \

ifeq ($(gb_FULLDEPS),$(true))
define gb_PrecompiledHeader__command_deponcompile
$(call gb_Helper_abbreviate_dirs,\
	$(OUTDIR_FOR_BUILD)/bin/makedepend \
		$(4) $(5) \
		-I$(dir $(3)) \
		$(filter-out -I$(COMPATH)% -I$(JAVA_HOME),$(6)) \
		$(3) \
		-f - \
	| $(gb_AWK) -f $(GBUILDDIR)/processdeps.awk \
		-v OBJECTFILE=$(1) \
		-v OUTDIR=$(OUTDIR)/ \
		-v WORKDIR=$(WORKDIR)/ \
		-v SRCDIR=$(SRCDIR)/ \
		-v REPODIR=$(REPODIR)/ \
	> $(call gb_PrecompiledHeader_get_dep_target,$(2)))
endef
else
gb_PrecompiledHeader__command_deponcompile =
endif

define gb_PrecompiledHeader__command
$(call gb_Output_announce,$(2),$(true),PCH,1)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(call gb_PrecompiledHeader_get_dep_target,$(2))) && \
	$(gb_CXX) \
		-x c++-header \
		$(4) $(5) \
		-I$(dir $(3)) \
		$(6) \
		-c $(3) \
		-o$(1))
$(call gb_PrecompiledHeader__command_deponcompile,$(1),$(2),$(3),$(4),$(5),$(6))

endef

# NoexPrecompiledHeader class

gb_NoexPrecompiledHeader_EXT := .gch

gb_NoexPrecompiledHeader_get_enableflags = -I$(WORKDIR)/NoexPrecompiledHeader/$(gb_NoexPrecompiledHeader_DEBUGDIR) \
					-Winvalid-pch \

ifeq ($(gb_FULLDEPS),$(true))
define gb_NoexPrecompiledHeader__command_deponcompile
$(call gb_Helper_abbreviate_dirs,\
	$(OUTDIR_FOR_BUILD)/bin/makedepend \
		$(4) $(5) \
		-I$(dir $(3)) \
		$(filter-out -I$(COMPATH)% -I$(JAVA_HOME),$(6)) \
		$(3) \
		-f - \
	| $(gb_AWK) -f $(GBUILDDIR)/processdeps.awk \
		-v OBJECTFILE=$(1) \
		-v OUTDIR=$(OUTDIR)/ \
		-v WORKDIR=$(WORKDIR)/ \
		-v SRCDIR=$(SRCDIR)/ \
		-v REPODIR=$(REPODIR)/ \
	> $(call gb_NoexPrecompiledHeader_get_dep_target,$(2)))
endef
else
gb_NoexPrecompiledHeader__command_deponcompile =
endif

define gb_NoexPrecompiledHeader__command
$(call gb_Output_announce,$(2),$(true),PCH,1)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(call gb_NoexPrecompiledHeader_get_dep_target,$(2))) && \
	$(gb_CXX) \
		-x c++-header \
		$(4) $(5) \
		-I$(dir $(3)) \
		$(6) \
		-c $(3) \
		-o$(1))
$(call gb_NoexPrecompiledHeader__command_deponcompile,$(1),$(2),$(3),$(4),$(5),$(6))

endef


# AsmObject class

gb_AsmObject_get_source = $(1)/$(2).s

# $(call gb_AsmObject__command,object,relative-source,source,dep-file)
define gb_AsmObject__command
$(call gb_Output_announce,$(2),$(true),ASM,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	$(gb_CC) \
		$(DEFS) \
		$(T_CFLAGS) \
		-c $(3) \
		-o $(1) \
		-I$(dir $(3)) \
		$(INCLUDE)) && \
	echo "$(1) : $(3)" > $(4)
endef


# LinkTarget class

gb_LinkTarget_CFLAGS := $(gb_CFLAGS) $(gb_CFLAGS_WERROR)
gb_LinkTarget_CXXFLAGS := $(gb_CXXFLAGS) $(gb_CXXFLAGS_WERROR)

ifeq ($(gb_SYMBOL),$(true))
gb_LinkTarget_CXXFLAGS += -ggdb2
gb_LinkTarget_CFLAGS += -ggdb2
endif

gb_LinkTarget_INCLUDE :=\
	$(filter-out %/stl, $(subst -I. , ,$(SOLARINC))) \
	$(foreach inc,$(subst ;, ,$(JDKINC)),-I$(inc)) \

gb_LinkTarget_INCLUDE_STL := $(filter %/stl, $(subst -I. , ,$(SOLARINC)))

define gb_LinkTarget__command_dynamiclinkexecutable
$(call gb_Output_announce,$(2),$(true),LNK,4)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(gb_CXX) $(strip \
		$(gb_Executable_TARGETTYPEFLAGS) \
		$(if $(filter YES,$(gb_Executable_TARGETGUI)), -mwindows, -mconsole) \
		$(T_LDFLAGS) \
		$(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
		$(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
		$(foreach object,$(GENCOBJECTS),$(call gb_GenCObject_get_target,$(object))) \
		$(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
		$(foreach object,$(ASMOBJECTS),$(call gb_AsmObject_get_target,$(object))) \
		$(foreach extraobjectlist,$(EXTRAOBJECTLISTS),@$(extraobjectlist)) \
		$(if $(LINKED_STATIC_LIBS),-Wl$(COMMA)--start-group $(foreach lib,$(LINKED_STATIC_LIBS),$(call gb_StaticLibrary_get_target,$(lib))) -Wl$(COMMA)--end-group) \
		$(patsubst lib%.a,-l%,$(patsubst lib%.dll.a,-l%,$(foreach lib,$(LINKED_LIBS),$(call gb_Library_get_implibname,$(lib))))) \
		$(LIBS) \
		-Wl$(COMMA)-Map$(COMMA)$(basename $(1)).map \
		-o $(1)))
endef

define gb_LinkTarget__command_dynamiclinklibrary
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(gb_CXX) $(strip \
		$(gb_Library_TARGETTYPEFLAGS) \
		$(T_LDFLAGS) \
		$(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
		$(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
		$(foreach object,$(GENCOBJECTS),$(call gb_GenCObject_get_target,$(object))) \
		$(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
		$(foreach object,$(ASMOBJECTS),$(call gb_AsmObject_get_target,$(object))) \
		$(foreach extraobjectlist,$(EXTRAOBJECTLISTS),@$(extraobjectlist)) \
		$(if $(LINKED_STATIC_LIBS),-Wl$(COMMA)--start-group $(foreach lib,$(LINKED_STATIC_LIBS),$(call gb_StaticLibrary_get_target,$(lib))) -Wl$(COMMA)--end-group) \
		$(patsubst lib%.a,-l%,$(patsubst lib%.dll.a,-l%,$(foreach lib,$(LINKED_LIBS),$(call gb_Library_get_implibname,$(lib))))) \
		$(LIBS) \
		-Wl$(COMMA)-Map$(COMMA)$(basename $(DLLTARGET)).map \
		-Wl$(COMMA)--out-implib$(COMMA)$(1) \
		-o $(dir $(1))/$(notdir $(DLLTARGET))))
endef

define gb_LinkTarget__command_staticlinklibrary
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(gb_AR) -rsu $(1) \
		$(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
		$(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
		$(foreach object,$(GENCOBJECTS),$(call gb_GenCObject_get_target,$(object))) \
		$(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
		$(foreach extraobjectlist,$(EXTRAOBJECTLISTS),@$(extraobjectlist)) \
		$(if $(findstring s,$(MAKEFLAGS)),2> /dev/null))
endef

define gb_LinkTarget__command
$(call gb_Output_announce,$(2),$(true),LNK,4)
$(if $(filter Executable,$(TARGETTYPE)),$(call gb_LinkTarget__command_dynamiclinkexecutable,$(1),$(2)))
$(if $(filter Library CppunitTest,$(TARGETTYPE)),$(call gb_LinkTarget__command_dynamiclinklibrary,$(1),$(2)))
$(if $(filter StaticLibrary,$(TARGETTYPE)),$(call gb_LinkTarget__command_staticlinklibrary,$(1)))
endef


# Library class

gb_Library_DEFS := -D_DLL
gb_Library_TARGETTYPEFLAGS := -shared
gb_Library_get_rpath :=

gb_Library_SYSPRE := lib
gb_Library_UNOVERPRE := $(gb_Library_SYSPRE)uno_
gb_Library_UDK_MAJORVER := 3

gb_Library_PLAINEXT := .dll.a

gb_Library_RTEXT := gcc3$(gb_Library_PLAINEXT)
gb_Library_RTVEREXT := $(gb_Library_RTEXT)
gb_Library_OOOEXT := $(gb_Library_DLLPOSTFIX)$(gb_Library_PLAINEXT)
gb_Library_UNOEXT := .uno$(gb_Library_PLAINEXT)
gb_Library_UNOVEREXT := $(gb_Library_UDK_MAJORVER)$(gb_Library_PLAINEXT)

gb_Library_DLLEXT := .dll

gb_Library_RTDLLEXT := gcc3$(gb_Library_DLLEXT)
gb_Library_RTVERDLLEXT := $(gb_Library_RTDLLEXT)
gb_Library_OOODLLEXT := $(gb_Library_DLLPOSTFIX)$(gb_Library_DLLEXT)
gb_Library_UNODLLEXT := .uno$(gb_Library_DLLEXT)
gb_Library_UNOVERDLLEXT := $(gb_Library_UDK_MAJORVER)$(gb_Library_DLLEXT)

gb_Library_win32_OLDNAMES := moldname

gb_Library_PLAINLIBS_NONE += \
	mingwthrd \
	mingw32 \
	mingwex \
	advapi32 \
	comdlg32 \
	crypt32 \
	d3d9 \
	d3dx \
	ddraw \
	gdi32 \
	gdiplus \
	gnu_getopt \
	imm32 \
	kernel32 \
	jawt \
	msi \
	msimg32 \
	msvcrt \
	msvcprt \
	mpr \
	$(gb_Library_win32_OLDNAMES) \
	ole32 \
	oleaut32 \
	shell32 \
	shlwapi \
	unicows \
	user32 \
	uuid \
	uwinapi \
	winmm \
	version \
	winspool \

gb_Library_LAYER := \
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):OOO) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):OOO) \

gb_Library_FILENAMES :=\
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_OOOEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_RTEXT)) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_RTEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):$(lib)$(gb_Library_UNOEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):$(lib)$(gb_Library_UNOEXT)) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):$(gb_Library_UNOVERPRE)$(lib)$(gb_Library_PLAINEXT)) \


gb_Library_DLLFILENAMES :=\
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):$(lib)$(gb_Library_OOODLLEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):$(lib)$(gb_Library_DLLEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):$(lib)$(gb_Library_DLLEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):$(lib)$(gb_Library_DLLEXT)) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):$(lib)$(gb_Library_RTDLLEXT)) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):$(lib)$(gb_Library_RTDLLEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):$(lib)$(gb_Library_UNODLLEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):$(lib)$(gb_Library_UNODLLEXT)) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):$(lib)$(gb_Library_UNOVERDLLEXT)) \

gb_Library_IARCSYSPRE := lib
gb_Library_IARCEXT := .a

gb_Library_ILIBEXT := .lib

define gb_Library_Library_platform
$(call gb_LinkTarget_set_dlltarget,$(2),$(OUTDIR)/bin/$(notdir $(3)))

$(call gb_LinkTarget_set_auxtargets,$(2),\
	$(patsubst %.dll,%.map,$(3)) \
)

$(call gb_Library_get_target,$(1)) \
$(call gb_Library_get_clean_target,$(1)) : AUXTARGETS := $(OUTDIR)/bin/$(notdir $(3))

$(call gb_Deliver_add_deliverable,$(OUTDIR)/bin/$(notdir $(3)),$(3),$(1))

endef

define gb_Library_add_default_nativeres
$(call gb_WinResTarget_WinResTarget_init,$(1)/$(2))
$(call gb_WinResTarget_add_file,$(1)/$(2),solenv/inc/shlinfo)
$(call gb_WinResTarget_set_defs,$(1)/$(2),\
		$$(DEFS) \
		-DADDITIONAL_VERINFO1 \
		-DADDITIONAL_VERINFO2 \
		-DADDITIONAL_VERINFO3 \
)
$(call gb_Library_add_nativeres,$(1),$(2))
$(call gb_Library_get_clean_target,$(1)) : $(call gb_WinResTarget_get_clean_target,$(1)/$(2))

endef

define gb_LinkTarget_add_nativeres
$(call gb_LinkTarget_get_target,$(1)) : $(call gb_WinResTarget_get_target,$(1)/$(2))
$(call gb_LinkTarget_get_target,$(1)) : NATIVERES += $(call gb_WinResTarget_get_target,$(1)/$(2))

endef

define gb_Library_get_dllname
$(patsubst $(1):%,%,$(filter $(1):%,$(gb_Library_DLLFILENAMES)))
endef

define gb_Library_get_implibname
$(patsubst $(1):%,%,$(filter $(1):%,$(gb_Library_FILENAMES)))
endef


# StaticLibrary class

gb_StaticLibrary_DEFS :=
gb_StaticLibrary_SYSPRE := lib
gb_StaticLibrary_PLAINEXT := .a
gb_StaticLibrary_JPEGEXT := lib$(gb_StaticLibrary_PLAINEXT)

gb_StaticLibrary_FILENAMES := \
	$(foreach lib,$(gb_StaticLibrary_JPEGLIBS),$(lib):$(gb_StaticLibrary_SYSPRE)$(lib)$(gb_StaticLibrary_JPEGEXT)) \
	$(foreach lib,$(gb_StaticLibrary_PLAINLIBS),$(lib):$(gb_StaticLibrary_SYSPRE)$(lib)$(gb_StaticLibrary_PLAINEXT)) \

gb_StaticLibrary_FILENAMES := $(patsubst salcpprt:salcpprt%,salcpprt:cpprtl%,$(gb_StaticLibrary_FILENAMES))

gb_StaticLibrary_StaticLibrary_platform =

# Executable class

gb_Executable_EXT := .exe
gb_Executable_TARGETTYPEFLAGS :=
gb_Executable_get_rpath :=
gb_Executable_Executable_platform =
gb_Executable_TARGETGUI :=

# CppunitTest class

ifeq ($(CROSS_COMPILING),YES)
gb_CppunitTest_CPPTESTPRECOMMAND := :
else
gb_CppunitTest_CPPTESTPRECOMMAND := $(gb_Helper_set_ld_path)
endif
gb_CppunitTest_SYSPRE := itest_
gb_CppunitTest_EXT := .lib
gb_CppunitTest_get_filename = $(gb_CppunitTest_SYSPRE)$(1)$(gb_CppunitTest_EXT)
gb_CppunitTest_get_libfilename = test_$(1).dll

define gb_CppunitTest_CppunitTest_platform
$(call gb_LinkTarget_set_dlltarget,$(2),$(3))

$(call gb_LinkTarget_set_auxtargets,$(2),\
	$(patsubst %.dll,%.map,$(3)) \
)

endef

# SdiTarget class

gb_SdiTarget_SVIDLPRECOMMAND := $(gb_Helper_set_ld_path)

# SrsPartMergeTarget

gb_SrsPartMergeTarget_TRANSEXPRECOMMAND := $(gb_Helper_set_ld_path)

# SrsPartTarget class

gb_SrsPartTarget_RSCTARGET := $(OUTDIR_FOR_BUILD)/bin/rsc
gb_SrsPartTarget_RSCCOMMAND := $(gb_Helper_set_ld_path) SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_SrsPartTarget_RSCTARGET)

ifeq ($(gb_FULLDEPS),$(true))
define gb_SrsPartTarget__command_dep
$(call gb_Helper_abbreviate_dirs,\
	$(OUTDIR_FOR_BUILD)/bin/makedepend \
		$(INCLUDE) \
		$(DEFS) \
		$(2) \
		-f - \
	| $(gb_AWK) -f $(GBUILDDIR)/processdeps.awk \
		-v OBJECTFILE=$(call gb_SrsPartTarget_get_target,$(1)) \
		-v OUTDIR=$(OUTDIR)/ \
		-v WORKDIR=$(WORKDIR)/ \
		-v SRCDIR=$(SRCDIR)/ \
		-v REPODIR=$(REPODIR)/ \
	> $(call gb_SrsPartTarget_get_dep_target,$(1)))
endef
else
gb_SrsPartTarget__command_dep =
endif

# WinResTarget class

gb_WinResTarget_POSTFIX :=.res

define gb_WinResTarget__command
$(call gb_Output_announce,$(2),$(true),RES,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(gb_RC) \
		$(DEFS) $(FLAGS) \
		-I$(dir $(3)) \
		$(INCLUDE) \
		-o $(1) \
		$(RCFILE) )
endef

$(eval $(call gb_Helper_make_dep_targets,\
	WinResTarget \
))

ifeq ($(gb_FULLDEPS),$(true))
define gb_WinResTarget__command_dep
$(call gb_Helper_abbreviate_dirs,\
	$(OUTDIR_FOR_BUILD)/bin/makedepend \
		$(INCLUDE) \
		$(DEFS) \
		$(2) \
		-f - \
	| $(gb_AWK) -f $(GBUILDDIR)/processdeps.awk \
		-v OBJECTFILE=$(call gb_WinResTarget_get_target,$(1)) \
		-v OUTDIR=$(OUTDIR)/ \
		-v WORKDIR=$(WORKDIR)/ \
		-v SRCDIR=$(SRCDIR)/ \
		-v REPODIR=$(REPODIR)/ \
	> $(call gb_WinResTarget_get_dep_target,$(1)))
endef
else
gb_WinResTarget__command_dep =
endif

# ComponentTarget

gb_XSLTPROCPRECOMMAND := $(gb_Helper_set_ld_path)

# UnoApiTarget

gb_UnoApiTarget_IDLCTARGET := $(OUTDIR_FOR_BUILD)/bin/idlc
gb_UnoApiTarget_IDLCCOMMAND := $(gb_Helper_set_ld_path) SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_IDLCTARGET)
gb_UnoApiTarget_REGMERGETARGET := $(OUTDIR_FOR_BUILD)/bin/regmerge
gb_UnoApiTarget_REGMERGECOMMAND := $(gb_Helper_set_ld_path) SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_REGMERGETARGET)
gb_UnoApiTarget_REGCOMPARETARGET := $(OUTDIR_FOR_BUILD)/bin/regcompare
gb_UnoApiTarget_REGCOMPARECOMMAND := $(gb_Helper_set_ld_path) SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_REGCOMPARETARGET)
gb_UnoApiTarget_CPPUMAKERTARGET := $(OUTDIR_FOR_BUILD)/bin/cppumaker
gb_UnoApiTarget_CPPUMAKERCOMMAND := $(gb_Helper_set_ld_path) SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_CPPUMAKERTARGET)
gb_UnoApiTarget_REGVIEWTARGET := $(OUTDIR_FOR_BUILD)/bin/regview
gb_UnoApiTarget_REGVIEWCOMMAND := $(gb_Helper_set_ld_path) SOLARBINDIR=$(OUTDIR_FOR_BUILD)/bin $(gb_UnoApiTarget_REGVIEWTARGET)

# Configuration
gb_CFGEXPRECOMMAND :=  PATH="$${PATH}:$(OUTDIR)/bin"

# Python
gb_PYTHON_PRECOMMAND :=  PATH="$${PATH}:$(OUTDIR)/bin" PYTHONHOME="$(OUTDIR)/lib/python" PYTHONPATH="$(OUTDIR)/lib/python;$(OUTDIR)/lib/python/lib-dynload"

# vim: set noet sw=4:
