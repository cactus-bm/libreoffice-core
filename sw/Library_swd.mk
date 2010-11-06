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

$(eval $(call gb_Library_Library,swd))

$(eval $(call gb_Library_set_componentfile,swd,sw/util/swd))

$(eval $(call gb_Library_set_include,swd,\
    $$(INCLUDE) \
    -I$(WORKDIR)/inc/sw/sdi \
    -I$(WORKDIR)/inc/sw \
    -I$(SRCDIR)/sw/inc \
    -I$(SRCDIR)/sw/inc/pch \
    -I$(SRCDIR)/sw/source/core/inc \
    -I$(SRCDIR)/sw/source/filter/inc \
    -I$(SRCDIR)/sw/source/ui/inc \
    -I$(OUTDIR)/inc/sw \
    -I$(OUTDIR)/inc/offuh \
))

$(eval $(call gb_Library_set_defs,swd,\
    $$(DEFS) \
    -DACCESSIBLE_LAYOUT \
))

$(eval $(call gb_Library_add_linked_libs,swd,\
    comphelper \
    sfx \
    sot \
    svl \
    svt \
    tl \
    ucbhelper \
    cppu \
    cppuhelper \
    sal \
    utl \
    vcl \
))

$(eval $(call gb_Library_add_exception_objects,swd,\
    sw/source/core/except/errhdl \
    sw/source/filter/basflt/iodetect \
    sw/source/ui/uno/detreg \
    sw/source/ui/uno/swdet2 \
    sw/source/ui/uno/swdetect \
))
ifeq ($(OS),WNT)
$(eval $(call gb_Library_add_linked_libs,swd,\
    kernel32 \
    msvcrt \
    uwinapi \
))
endif
# vim: set noet sw=4 ts=4:
