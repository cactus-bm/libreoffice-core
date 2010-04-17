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

$(eval $(call gb_Library_Library,svl))

$(call gb_Library_get_headers_target,svl) : $(call gb_Package_get_target,svl_inc)

$(eval $(call gb_Library_set_include,svl,\
    $$(SOLARINC) \
    -I$(WORKDIR)/inc/svl \
    -I$(WORKDIR)/inc/ \
    -I$(SRCDIR)/svl/inc \
    -I$(SRCDIR)/svl/source/inc \
    -I$(SRCDIR)/svl/inc/pch \
    -I$(OUTDIR)/inc/offuh \
    -I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_set_defs,svl,\
    $$(DEFS) \
    -DSVL_DLLIMPLEMENTATION \
))

$(eval $(call gb_Library_add_linked_libs,svl,\
    comphelper \
    cppu \
    i18nisolang1 \
    i18nutil \
    sal \
    sot \
    tl \
    ucbhelper \
    utl \
    vos3 \
    basegfx \
    cppuhelper \
    jvmfwk \
    stl \
))

$(eval $(call gb_Library_add_linked_system_libs,svl,\
    icuuc \
    dl \
    m \
    pthread \
))


$(eval $(call gb_Library_add_exception_objects,svl,\
    svl/inc/pch/precompiled_svl \
    svl/source/config/cjkoptions \
    svl/source/config/ctloptions \
    svl/source/config/itemholder2 \
    svl/source/config/languageoptions \
    svl/source/filepicker/pickerhelper \
    svl/source/filepicker/pickerhistory \
    svl/source/filerec/filerec \
    svl/source/items/aeitem \
    svl/source/items/cenumitm \
    svl/source/items/cintitem \
    svl/source/items/cntwall \
    svl/source/items/ctypeitm \
    svl/source/items/custritm \
    svl/source/items/dateitem \
    svl/source/items/eitem \
    svl/source/items/flagitem \
    svl/source/items/globalnameitem \
    svl/source/items/ilstitem \
    svl/source/items/imageitm \
    svl/source/items/intitem \
    svl/source/items/itemiter \
    svl/source/items/itempool \
    svl/source/items/itemprop \
    svl/source/items/itemset \
    svl/source/items/lckbitem \
    svl/source/items/macitem \
    svl/source/items/poolcach \
    svl/source/items/poolio \
    svl/source/items/poolitem \
    svl/source/items/ptitem \
    svl/source/items/rectitem \
    svl/source/items/rngitem \
    svl/source/items/sfontitm \
    svl/source/items/sitem \
    svl/source/items/slstitm \
    svl/source/items/stritem \
    svl/source/items/style \
    svl/source/items/stylepool \
    svl/source/items/szitem \
    svl/source/items/visitem \
    svl/source/items/whiter \
    svl/source/memtools/svarray \
    svl/source/misc/PasswordHelper \
    svl/source/misc/adrparse \
    svl/source/misc/documentlockfile \
    svl/source/misc/filenotation \
    svl/source/misc/folderrestriction \
    svl/source/misc/fstathelper \
    svl/source/misc/inethist \
    svl/source/misc/inettype \
    svl/source/misc/lngmisc \
    svl/source/misc/lockfilecommon \
    svl/source/misc/ownlist \
    svl/source/misc/restrictedpaths \
    svl/source/misc/sharecontrolfile \
    svl/source/misc/strmadpt \
    svl/source/misc/svldata \
    svl/source/misc/urihelper \
    svl/source/notify/brdcst \
    svl/source/notify/broadcast \
    svl/source/notify/cancel \
    svl/source/notify/hint \
    svl/source/notify/isethint \
    svl/source/notify/listener \
    svl/source/notify/listenerbase \
    svl/source/notify/listeneriter \
    svl/source/notify/lstner \
    svl/source/notify/smplhint \
    svl/source/numbers/nbdll \
    svl/source/numbers/numfmuno \
    svl/source/numbers/numhead \
    svl/source/numbers/numuno \
    svl/source/numbers/supservs \
    svl/source/numbers/zforfind \
    svl/source/numbers/zforlist \
    svl/source/numbers/zformat \
    svl/source/numbers/zforscan \
    svl/source/svsql/converter \
    svl/source/undo/undo \
    svl/source/uno/pathservice \
    svl/source/uno/registerservices \
))

ifeq ($(OS),WNT)
$(eval $(call gb_Library_add_exception_objects,svl,\
    svl/source/svdde/ddecli \
    svl/source/svdde/ddedata \
    svl/source/svdde/ddeinf \
    svl/source/svdde/ddestrg \
    svl/source/svdde/ddesvr \
    svl/source/svdde/ddewrap \
))

$(eval $(call gb_Library_add_linked_libs,svl,\
    advapi32 \
    kernel32 \
    gdi32 \
    msvcrt \
    shell32 \
    user32 \
    uwinapi \
))
else
$(eval $(call gb_Library_add_exception_objects,svl,\
    svl/unx/source/svdde/ddedummy \
))
endif
# vim: set noet sw=4 ts=4:
