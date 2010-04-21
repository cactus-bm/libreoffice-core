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

$(eval $(call gb_Library_Library,tl))

$(call gb_Library_get_headers_target,tl) : $(call gb_Package_get_target,tools_inc)

$(eval $(call gb_Library_set_include,tl,\
    $$(INCLUDE) \
    -I$(OUTDIR)/inc \
    -I$(WORKDIR)/inc/tools \
    -I$(SRCDIR)/tools/inc \
    -I$(SRCDIR)/tools/inc/pch \
    -I$(SRCDIR)/solenv/inc \
    -I$(SRCDIR)/solenv/inc/Xp31 \
    -I$(OUTDIR)/inc/tools \
    -I$(OUTDIR)/inc/offuh \
    -I$(OUTDIR)/inc/stl \
))

$(eval $(call gb_Library_set_cxxflags,tl,\
    $$(CXXFLAGS) \
    -DSHARED_LIB \
    -DTOOLS_DLLIMPLEMENTATION \
    -DVCL \
))

$(eval $(call gb_Library_add_linked_libs,tl,\
    basegfx \
    comphelper \
    i18nisolang1 \
    stl \
    cppu \
    sal \
    vos3 \
))


$(eval $(call gb_Library_add_exception_objects,tl,\
    tools/source/communi/geninfo \
    tools/source/communi/parser \
    tools/source/datetime/datetime \
    tools/source/datetime/tdate \
    tools/source/datetime/ttime \
    tools/source/debug/debug \
    tools/source/debug/stcktree \
    tools/source/fsys/comdep \
    tools/source/fsys/dirent \
    tools/source/fsys/filecopy \
    tools/source/fsys/fstat \
    tools/source/fsys/tdir \
    tools/source/fsys/tempfile \
    tools/source/fsys/urlobj \
    tools/source/fsys/wldcrd \
    tools/source/generic/b3dtrans \
    tools/source/generic/bigint \
    tools/source/generic/color \
    tools/source/generic/config \
    tools/source/generic/fract \
    tools/source/generic/gen \
    tools/source/generic/line \
    tools/source/generic/link \
    tools/source/generic/poly \
    tools/source/generic/poly2 \
    tools/source/generic/svborder \
    tools/source/generic/toolsin \
    tools/source/inet/inetmime \
    tools/source/inet/inetmsg \
    tools/source/inet/inetstrm \
    tools/source/memtools/contnr \
    tools/source/memtools/mempool \
    tools/source/memtools/multisel \
    tools/source/memtools/table \
    tools/source/memtools/unqidx \
    tools/source/misc/appendunixshellword \
    tools/source/misc/extendapplicationenvironment \
    tools/source/misc/getprocessworkingdir \
    tools/source/misc/solarmutex \
    tools/source/rc/isofallback \
    tools/source/rc/rc \
    tools/source/rc/resary \
    tools/source/rc/resmgr \
    tools/source/ref/errinf \
    tools/source/ref/globname \
    tools/source/ref/pstm \
    tools/source/ref/ref \
    tools/source/stream/cachestr \
    tools/source/stream/stream \
    tools/source/stream/strmsys \
    tools/source/stream/vcompat \
    tools/source/string/debugprint \
    tools/source/string/tenccvt \
    tools/source/string/tstring \
    tools/source/string/tustring \
    tools/source/testtoolloader/testtoolloader \
    tools/source/zcodec/zcodec \
))

ifeq ($(GUI),UNX)
$(eval $(call gb_Library_add_exception_objects,tl,\
    tools/unx/source/dll/toolsdll \
))
endif

ifeq ($(OS),LINUX)
$(eval $(call gb_Library_add_linked_libs,tl,\
    dl \
    m \
    pthread \
))
ifeq ($(SYSTEM_ZLIB),YES)
$(eval $(call gb_Library_set_cxxflags,tl,\
    $$(CXXFLAGS) \
    -DSYSTEM_ZLIB \
))
$(eval $(call gb_Library_add_linked_libs,tl,\
    z \
))
else
$(eval $(call gb_Library_add_linked_static_libs,tl,\
    zlib \
))
endif
endif

ifeq ($(OS),WNT)
$(eval $(call gb_Library_set_include,tl,\
    $$(INCLUDE) \
    -I$(SRCDIR)/tools/win/inc \
))

$(eval $(call gb_Library_add_exception_objects,tl,\
    tools/source/win/source/dll/toolsdll \
))

$(eval $(call gb_Library_add_linked_libs,tl,\
    advapi32 \
    kernel32 \
    mpr \
    msvcrt \
    oldnames \
    ole32 \
    shell32 \
    user32 \
    uuid \
    uwinapi \
))
endif
# tools/source/string/debugprint -DDEBUG -DEXCEPTIONS_OFF -DOSL_DEBUG_LEVEL=2 -DSHAREDLIB -DTOOLS_DLLIMPLEMENTATION -D_DLL_ -O0 -fno-exceptions -fpic -fvisibility=hidden -g
# -DOPTIMIZE
# no -DTOOLS_DLLIMPLEMENTATION on toolsdll
# -DEXCEPTIONS_OFF -fno-exceptions on geninfo parser datetime tdate ttime bigint color config fract gen line link poly2 svborder toolsin inetmime inetmsg inetstrm contnr mempool multisel table unqidx cachestr stream strmsys vcompat tenccvt tstring tustring testtoolloader
# vim: set noet sw=4 ts=4:
