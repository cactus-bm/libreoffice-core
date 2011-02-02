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

$(eval $(call gb_Library_Library,vbaswobj))

$(eval $(call gb_Library_set_componentfile,vbaswobj,sw/util/vbaswobj))

$(eval $(call gb_Library_set_include,vbaswobj,\
    -I$(SRCDIR)/sw/source/core/inc \
    -I$(SRCDIR)/sw/source/filter/inc \
    -I$(SRCDIR)/sw/source/ui/inc \
    -I$(SRCDIR)/sw/inc/pch \
    -I$(SRCDIR)/sw/inc \
    -I$(WORKDIR)/Misc/sw/ \
    $$(INCLUDE) \
    -I$(OUTDIR)/inc/offuh \
    -I$(OUTDIR)/inc/ \
))

$(eval $(call gb_Library_add_linked_libs,vbaswobj,\
    comphelper \
    cppu \
    cppuhelper \
    editeng \
    i18nisolang1 \
    sal \
    sb \
    sfx \
    stl \
    svl \
    svt \
    svx \
    svxcore \
    sw \
    tk \
    tl \
    utl \
    vbahelper \
    vcl \
    $(gb_StdLibs) \
))

$(eval $(call gb_Library_add_exception_objects,vbaswobj,\
    sw/source/ui/vba/service \
    sw/source/ui/vba/vbadocument \
    sw/source/ui/vba/vbasections \
    sw/source/ui/vba/vbadialog \
    sw/source/ui/vba/vbawrapformat \
    sw/source/ui/vba/vbafont \
    sw/source/ui/vba/vbaheaderfooterhelper \
    sw/source/ui/vba/vbarangehelper \
    sw/source/ui/vba/vbaaddin \
    sw/source/ui/vba/vbaautotextentry \
    sw/source/ui/vba/vbarange \
    sw/source/ui/vba/vbadocumentproperties \
    sw/source/ui/vba/vbaeventshelper \
    sw/source/ui/vba/vbastyle \
    sw/source/ui/vba/vbapane \
    sw/source/ui/vba/vbaglobals \
    sw/source/ui/vba/vbatemplate \
    sw/source/ui/vba/vbaaddins \
    sw/source/ui/vba/vbaview \
    sw/source/ui/vba/vbaheaderfooter \
    sw/source/ui/vba/vbabookmarks \
    sw/source/ui/vba/vbaoptions \
    sw/source/ui/vba/vbadialogs \
    sw/source/ui/vba/vbapalette \
    sw/source/ui/vba/vbaparagraph \
    sw/source/ui/vba/vbafind \
    sw/source/ui/vba/vbasection \
    sw/source/ui/vba/vbabookmark \
    sw/source/ui/vba/vbaapplication \
    sw/source/ui/vba/vbawindow \
    sw/source/ui/vba/vbareplacement \
    sw/source/ui/vba/vbatable \
    sw/source/ui/vba/vbaselection \
    sw/source/ui/vba/vbasystem \
    sw/source/ui/vba/vbainformationhelper \
    sw/source/ui/vba/vbapagesetup \
    sw/source/ui/vba/vbafield \
    sw/source/ui/vba/vbatables \
    sw/source/ui/vba/vbavariable \
    sw/source/ui/vba/vbadocuments \
    sw/source/ui/vba/vbaparagraphformat \
    sw/source/ui/vba/vbaborders \
    sw/source/ui/vba/vbavariables \
    sw/source/ui/vba/vbastyles \
    sw/source/ui/vba/vbapanes \
    sw/source/ui/vba/wordvbahelper \
))

ifeq ($(OS),LINUX)
$(eval $(call gb_Library_set_ldflags,vbaswobj,\
    $$(LDFLAGS) \
    -Wl$(COMMA)-O1 \
    -Wl$(COMMA)-z$(COMMA)noexecstack \
))
endif

ifeq ($(OS),WNT)
ifneq ($(USE_MINGW),)
$(eval $(call gb_Library_add_linked_libs,vbaswobj,\
    mingwthrd \
    $(gb_MINGW_LIBSTDCPP) \
    mingw32 \
    $(gb_MINGW_LIBGCC) \
    uwinapi \
    moldname \
    mingwex \
    advapi32 \
    kernel32 \
    msvcrt \
    user32 \
))
endif
endif
# vim: set noet sw=4 ts=4:
