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

$(eval $(call gb_Library_Library,textconversiondlgs))

$(eval $(call gb_Library_set_componentfile,textconversiondlgs,svx/util/textconversiondlgs))

$(eval $(call gb_Library_set_include,textconversiondlgs,\
    -I$(SRCDIR)/svx/inc \
    -I$(SRCDIR)/svx/inc/pch \
    $$(INCLUDE) \
    -I$(OUTDIR)/inc/offuh \
))

$(eval $(call gb_Library_set_defs,textconversiondlgs,\
    $$(DEFS) \
))

$(eval $(call gb_Library_add_linked_libs,textconversiondlgs,\
    comphelper \
    cppuhelper \
    cppu \
    sal \
    svl \
    svt \
    stl \
    tk \
    tl \
    utl \
    vcl \
))

$(eval $(call gb_Library_add_exception_objects,textconversiondlgs,\
    svx/source/unodialogs/textconversiondlgs/services \
    svx/source/unodialogs/textconversiondlgs/chinese_dictionarydialog \
    svx/source/unodialogs/textconversiondlgs/chinese_translationdialog \
    svx/source/unodialogs/textconversiondlgs/chinese_translation_unodialog \
    svx/source/unodialogs/textconversiondlgs/resid \
))

ifeq ($(OS),LINUX)
$(eval $(call gb_Library_add_linked_libs,textconversiondlgs,\
    dl \
    m \
    pthread \
    stl \
))
endif

ifeq ($(OS),WNT)
$(eval $(call gb_Library_add_linked_libs,textconversiondlgs,\
    advapi32 \
    kernel32 \
    msvcrt \
    oldnames \
    user32 \
    uwinapi \
))
endif

# vim: set noet sw=4 ts=4:
