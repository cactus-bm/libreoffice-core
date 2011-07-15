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

# Make has no support for 'or' clauses in conditionals,
# we use a filter expression instead.
ifneq (,$(filter LINUX-GCC MACOSX-GCC WNT-GCC DRAGONFLY-GCC OPENBSD-GCC FREEBSD-GCC NETBSD-GCC IOS-GCC ANDROID-GCC, $(OS)-$(COM)))
# Could we in fact just check for $(COM) == "GCC" here?
gb_Library_FILENAMES := $(patsubst comphelper:libcomphelper%,comphelper:libcomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppuhelper:libcppuhelper%,cppuhelper:libuno_cppuhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst jvmfwk:libuno_jvmfwk%,jvmfwk:libjvmfwk%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst salhelper:libsalhelper%,salhelper:libuno_salhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ucbhelper:libucbhelper%,ucbhelper:libucbhelper4%,$(gb_Library_FILENAMES))


endif


ifeq ($(OS),WNT)

ifeq ($(COM),GCC)
gb_Library_FILENAMES := $(patsubst sb:isb%,sb:basic%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sfx:isfx%,sfx:sfx%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst svt:isvt%,svt:svtool%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst tl:itl%,tl:itools%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst vbahelper:ivbahelper%,vbahelper:vbahelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xml2:ixml2%,xml2:libxml2$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xslt:ixslt%,xslt:libxslt$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst rdf:irdf%,rdf:librdf.dll$(gb_Library_IARCEXT),$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst z:iz%,z:zlib%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst graphite2_off:%.dll.a,graphite2_off:%.a,$(gb_Library_FILENAMES))

# Libraries not provided by mingw(-w64), available only in the Windows
# SDK. So if these actually are liked with somewhere, we can't
# cross-compile that module then using MinGW. That needs to be fixed
# then, and we need to use these libraries through run-time lookup of
# APIs. Or something.

gb_Library_SDKLIBFILENAMES:=\
    unicows \

gb_Library_DLLFILENAMES := $(filter-out $(foreach lib,$(gb_Library_SDKLIBFILENAMES),$(lib):%),$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES += $(foreach lib,$(gb_Library_SDKLIBFILENAMES),$(lib):$(WINDOWS_SDK_HOME)/lib/$(lib).lib)

gb_Library_DLLFILENAMES := $(patsubst comphelper:comphelper%,comphelper:comphelp%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst crypto:crypto%,crypto:libeay32%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst icuuc:icuuc%,icuuc:icuuc40%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst ssl:ssl%,ssl:ssleay32%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst ucbhelper:ucbhelper%,ucbhelper:ucbhelper4%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst z:z%,z:zlib%,$(gb_Library_DLLFILENAMES))


else
gb_Library_FILENAMES := $(patsubst cairo:icairo%,cairo:cairo%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst comphelper:icomphelper%,comphelper:icomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppunit:icppunit%,cppunit:icppunit_dll%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst crypto:libcrypto%,crypto:libcrypto_static%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cui:icui%,cui:icuin%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst i18nisolang1:ii18nisolang1%,i18nisolang1:ii18nisolang%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sb:isb%,sb:basic%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst sfx:isfx%,sfx:sfx%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ssl:libssl%,ssl:libssl_static%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst svt:isvt%,svt:svtool%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst tl:itl%,tl:itools%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst vbahelper:ivbahelper%,vbahelper:vbahelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xml2:ixml2%,xml2:libxml2%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xslt:ixslt%,xslt:libxslt%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst rdf:irdf%,rdf:librdf%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst z:iz%,z:zlib%,$(gb_Library_FILENAMES))
gb_Library_NOILIBFILENAMES:=\
    advapi32 \
    gdi32 \
    gdiplus \
    graphite2_off \
    gnu_getopt \
    icuuc \
    icule \
    imm32\
    kernel32 \
    msimg32 \
    msvcrt \
    msvcprt \
    mpr \
    $(gb_Library_win32_OLDNAMES) \
    ole32 \
    oleaut32 \
    shell32 \
    shlwapi \
    sot \
    unicows \
    user32 \
    uuid \
    uwinapi \
    version \
    winspool \

gb_Library_FILENAMES := $(filter-out $(foreach lib,$(gb_Library_NOILIBFILENAMES),$(lib):%),$(gb_Library_FILENAMES))
gb_Library_FILENAMES += $(foreach lib,$(gb_Library_NOILIBFILENAMES),$(lib):$(lib)$(gb_Library_PLAINEXT))
ifneq ($(gb_PRODUCT),$(true))
gb_Library_FILENAMES := $(patsubst msvcrt:msvcrt%,msvcrt:msvcrtd%,$(gb_Library_FILENAMES))
endif

gb_Library_DLLFILENAMES := $(patsubst comphelper:comphelper%,comphelper:comphelp%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst icuuc:icuuc%,icuuc:icuuc40%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst ucbhelper:ucbhelper%,ucbhelper:ucbhelper4%,$(gb_Library_DLLFILENAMES))
gb_Library_DLLFILENAMES := $(patsubst z:z%,z:zlib%,$(gb_Library_DLLFILENAMES))

endif

endif

ifeq ($(OS),SOLARIS)
gb_Library_FILENAMES := $(patsubst comphelper:libcomphelper%,comphelper:libcomphelp%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst cppuhelper:libcppuhelper%,cppuhelper:libuno_cppuhelper%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst ucbhelper:libucbhelper%,ucbhelper:libucbhelper4%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst jvmfwk:libuno_jvmfwk%,jvmfwk:libjvmfwk%,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst salhelper:libsalhelper%,salhelper:libuno_salhelper%,$(gb_Library_FILENAMES))
#$(info libnames: $(gb_Library_FILENAMES))

endif

# Some external libraries get built as static libraries for Android
# because we haven't bothered fixing their configury to recognize that
# shared libraries can be built for the platform.
ifeq ($(OS),ANDROID)
gb_Library_FILENAMES := $(patsubst cppunit:%.so,cppunit:%.a,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xml2:%.so,xml2:%.a,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst rdf:%.so,rdf:%.a,$(gb_Library_FILENAMES))
gb_Library_FILENAMES := $(patsubst xslt:%.so,xslt:%.a,$(gb_Library_FILENAMES))

endif

gb_Library_FILENAMES := $(patsubst writerfilter_uno:writerfilter_uno%,writerfilter_uno:writerfilter%,$(gb_Library_FILENAMES))

# we do not require a known rule for these, when using system libs

ifeq ($(SYSTEM_CAIRO),YES)
gb_Library_TARGETS := $(filter-out cairo,$(gb_Library_TARGETS))
endif

ifeq ($(SYSTEM_LIBXML),YES)
gb_Library_TARGETS := $(filter-out xml2,$(gb_Library_TARGETS))
endif

ifeq ($(SYSTEM_LIBXSLT),YES)
gb_Library_TARGETS := $(filter-out xslt,$(gb_Library_TARGETS))
endif

ifeq ($(SYSTEM_OPENSSL),YES)
gb_Library_TARGETS := $(filter-out crypto,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out ssl,$(gb_Library_TARGETS))
endif

ifeq ($(SYSTEM_REDLAND),YES)
gb_Library_TARGETS := $(filter-out rdf,$(gb_Library_TARGETS))
endif

ifeq ($(SYSTEM_ICU),YES)
gb_Library_TARGETS := $(filter-out icudata,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out icui18n,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out icuio,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out icule,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out iculx,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out icutu,$(gb_Library_TARGETS))
gb_Library_TARGETS := $(filter-out icuuc,$(gb_Library_TARGETS))
endif

ifeq ($(SYSTEM_CPPUNIT),YES)
gb_Library_TARGETS := $(filter-out cppunit,$(gb_Library_TARGETS))
endif

# vim: set noet sw=4 ts=4:
