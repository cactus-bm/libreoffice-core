#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
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

# fixes for all the libraries that are named with too much creativity and do
# not follow any of the established nameschemes

ifeq ($(OS),LINUX)

gb_Library_FILENAMES := $(patsubst comphelper:libcomphelper%,comphelper:libcomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppuhelper:libcppuhelper%,cppuhelper:libuno_cppuhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppuhelper:libcppuhelper%,cppuhelper:libuno_cppuhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst jvmfwk:libuno_jvmfwk%,jvmfwk:libjvmfwk%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst jvmfwk:libuno_jvmfwk%,jvmfwk:libjvmfwk%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst salhelper:libsalhelper%,salhelper:libuno_salhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst salhelper:libsalhelper%,salhelper:libuno_salhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ucbhelper:libucbhelper%,ucbhelper:libucbhelper4%,$(gb_Library_FILENAMES))

ifeq ($(USE_SYSTEM_STL),YES)
gb_Library_FILENAMES := $(patsubst stl:%,stl:libstdc++.so,$(gb_Library_FILENAMES))
gb_Library_TARGETS := $(filter-out stl,$(gb_Library_TARGETS))
endif

endif # ifeq ($(OS),LINUX)

ifeq ($(OS),SOLARIS)

gb_Library_FILENAMES := $(patsubst comphelper:libcomphelper%,comphelper:libcomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppuhelper:libcppuhelper%,cppuhelper:libuno_cppuhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ucbhelper:libucbhelper%,ucbhelper:libucbhelper4%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst jvmfwk:libuno_jvmfwk%,jvmfwk:libjvmfwk%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst salhelper:libsalhelper%,salhelper:libuno_salhelper%,$(gb_Library_FILENAMES))
#$(info libnames: $(gb_Library_FILENAMES))

endif

ifeq ($(OS),MACOSX)

gb_Library_FILENAMES := $(patsubst comphelper:libcomphelper%,comphelper:libcomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppuhelper:libcppuhelper%,cppuhelper:libuno_cppuhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst jvmfwk:libuno_jvmfwk%,jvmfwk:libjvmfwk%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst salhelper:libsalhelper%,salhelper:libuno_salhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ucbhelper:libucbhelper%,ucbhelper:libucbhelper4%,$(gb_Library_FILENAMES))

ifeq ($(USE_SYSTEM_STL),YES)
gb_Library_FILENAMES := $(patsubst stl:%,stl:libstdc++.dylib,$(gb_Library_FILENAMES))
gb_Library_TARGETS := $(filter-out stl,$(gb_Library_TARGETS))
endif

endif # ifeq ($(OS),MACOSX)

ifeq ($(OS),WNT)

ifneq ($(USE_MINGW),)

gb_Library_FILENAMES := $(patsubst comphelper:icomphelper%,comphelper:icomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppunit:icppunit%,cppunit:libcppunit.dll$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cui:icui%,cui:icuin%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst i18nisolang1:ii18nisolang1%,i18nisolang1:ii18nisolang%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst i18nisolang1:iii18nisolang1%,i18nisolang1:iii18nisolang%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sb:isb%,sb:basic%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sfx:isfx%,sfx:sfx%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst svt:isvt%,svt:svtool%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst tl:itl%,tl:itools%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst vbahelper:ivbahelper%,vbahelper:vbahelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst vos3:ivos3%,vos3:ivos%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst crypto:icrypto%,crypto:crypto%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ssl:issl%,ssl:ssl%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xml2:ixml2%,xml2:libxml2$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xslt:ixslt%,xslt:libxslt$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst rdf:irdf%,rdf:librdf.dll$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst z:iz%,z:zlib%,$(gb_Library_FILENAMES))
gb_StaticLibrary_FILENAMES := $(patsubst graphite:graphite%,graphite:graphite_dll%,$(gb_StaticLibrary_FILENAMES))

ifeq ($(gb_PRODUCT),$(true))
gb_Library_FILENAMES := $(patsubst stl:istl%,stl:stlport_vc71%,$(gb_Library_FILENAMES))
else
gb_Library_FILENAMES := $(patsubst stl:istl%,stl:stlport_vc71_stldebug%,$(gb_Library_FILENAMES))
endif

# handle libraries in msvc format that don't use an "i" prefix for their import library
# these are libraries built by OOo, but only a few of them
# all other libraries built by OOo and all platform libraries (exceptions see below) are used without an import library
# we link against their dlls in gcc format directly
gb_Library_NOILIBFILENAMES:=\
	icuuc \
	uwinapi \

gb_Library_FILENAMES := $(filter-out $(foreach lib,$(gb_Library_NOILIBFILENAMES),$(lib):%),$(gb_Library_FILENAMES))
gb_Library_FILENAMES += $(foreach lib,$(gb_Library_NOILIBFILENAMES),$(lib):$(lib)$(gb_Library_PLAINEXT))

# some Windows platform libraries are missing in mingw library set
# we have to use them from the PSDK by linking against their ilibs
gb_Library_ILIBFILENAMES:=\
	unicows \
	uuid \
	winmm \

gb_Library_DLLFILENAMES := $(filter-out $(foreach lib,$(gb_Library_ILIBFILENAMES),$(lib):%),$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES += $(foreach lib,$(gb_Library_ILIBFILENAMES),$(lib):$(PSDK_HOME)/lib/$(lib)$(gb_Library_ILIBEXT))

gb_Library_DLLFILENAMES := $(patsubst comphelper:comphelper%,comphelper:comphelp%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst cppunit:cppunit%,cppunit:cygcppunit-1-12-1%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst icuuc:icuuc%,icuuc:icuuc40%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst ucbhelper:ucbhelper%,ucbhelper:ucbhelper4%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst z:z%,z:zlib%,$(gb_Library_DLLFILENAMES))

ifeq ($(USE_SYSTEM_STL),YES)
gb_Library_FILENAMES := $(patsubst stl:%,stl:$(gb_Library_IARCSYSPRE)stdc++_s$(gb_Library_IARCSYSPRE),$(gb_Library_FILENAMES))
gb_Library_TARGETS := $(filter-out stl,$(gb_Library_TARGETS))
endif

else #ifneq ($(USE_MINGW),)

gb_Library_FILENAMES := $(patsubst comphelper:icomphelper%,comphelper:icomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppunit:icppunit%,cppunit:icppunit_dll%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cui:icui%,cui:icuin%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst i18nisolang1:ii18nisolang1%,i18nisolang1:ii18nisolang%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst i18nisolang1:iii18nisolang1%,i18nisolang1:iii18nisolang%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sb:isb%,sb:basic%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sfx:isfx%,sfx:sfx%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst svt:isvt%,svt:svtool%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst tl:itl%,tl:itools%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst vbahelper:ivbahelper%,vbahelper:vbahelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst vos3:ivos3%,vos3:ivos%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst crypto:icrypto%,crypto:libeay32%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ssl:issl%,ssl:ssleay32%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xml2:ixml2%,xml2:libxml2%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xslt:ixslt%,xslt:libxslt%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst rdf:irdf%,rdf:librdf%,$(gb_Library_FILENAMES))
gb_StaticLibrary_FILENAMES := $(patsubst graphite:graphite%,graphite:graphite_dll%,$(gb_StaticLibrary_FILENAMES))

ifeq ($(gb_PRODUCT),$(true))
gb_Library_FILENAMES := $(patsubst stl:istl%,stl:stlport_vc71%,$(gb_Library_FILENAMES))
else
gb_Library_FILENAMES := $(patsubst stl:istl%,stl:stlport_vc71_stldebug%,$(gb_Library_FILENAMES))
endif

# change the names of all import libraries that don't have an "i" prefix as in our standard naming schema
gb_Library_NOILIBFILENAMES := $(gb_Library_PLAINLIBS_NONE)
gb_Library_NOILIBFILENAMES += icule icuuc

gb_Library_FILENAMES := $(filter-out $(foreach lib,$(gb_Library_NOILIBFILENAMES),$(lib):%),$(gb_Library_FILENAMES))
gb_Library_FILENAMES += $(foreach lib,$(gb_Library_NOILIBFILENAMES),$(lib):$(lib)$(gb_Library_PLAINEXT))
gb_Library_FILENAMES := $(patsubst z:z%,z:zlib%,$(gb_Library_FILENAMES))

ifneq ($(gb_PRODUCT),$(true))
gb_Library_FILENAMES := $(patsubst msvcrt:msvcrt%,msvcrt:msvcrtd%,$(gb_Library_FILENAMES))
endif

gb_Library_DLLFILENAMES := $(patsubst comphelper:comphelper%,comphelper:comphelp%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst icuuc:icuuc%,icuuc:icuuc40%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst ucbhelper:ucbhelper%,ucbhelper:ucbhelper4%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst z:z%,z:zlib%,$(gb_Library_DLLFILENAMES))

endif # ifneq ($(USE_MINGW),)

endif # ifeq ($(OS),WNT)

# we do not require a known rule for these, when using system libs

ifeq ($(USE_SYSTEM_STL),YES)
gb_Library_TARGETS := $(filter-out stl,$(gb_Library_TARGETS))
endif

# vim: set noet sw=4 ts=4:
