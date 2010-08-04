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


$(eval $(call gb_Library_Library,qstart_gtk))

$(eval $(call gb_Library_set_include,qstart_gtk,\
    $$(INCLUDE) \
    -I$(SRCDIR)/sfx2/inc \
    -I$(SRCDIR)/sfx2/inc/sfx2 \
    -I$(SRCDIR)/sfx2/inc/pch \
    -I$(OUTDIR)/inc/offuh \
    -I$(OUTDIR)/inc \
    $(filter -I%,$(GTK_CFLAGS)) \
))

$(eval $(call gb_Library_set_defs,qstart_gtk,\
    $$(DEFS) \
    -DDLL_NAME=$(notdir $(call gb_Library_get_target,sfx2)) \
    -DENABLE_QUICKSTART_APPLET \
))

$(eval $(call gb_Library_set_cflags,qstart_gtk,\
    $$(CFLAGS) \
    $(filter-out -I%,$(GTK_CFLAGS)) \
))

$(eval $(call gb_Library_set_ldflags,qstart_gtk,\
    $$(LDFLAGS) \
    $(GTK_LIBS) \
))

$(eval $(call gb_Library_add_linked_libs,qstart_gtk,\
    comphelper \
    cppu \
    cppuhelper \
    eggtray \
    fwe \
    i18nisolang1 \
    sal \
    sax \
    sb \
    sot \
    stl \
    svl \
    svt \
    tk \
    tl \
    ucbhelper \
    utl \
    vcl \
    vos3 \
    xml2 \
    sfx \
))

$(eval $(call gb_Library_add_linked_system_libs,qstart_gtk,\
    icuuc \
    dl \
    m \
    pthread \
))

$(eval $(call gb_Library_add_exception_objects,qstart_gtk,\
    sfx2/source/appl/shutdowniconunx \
))

# vim: set noet sw=4 ts=4:
