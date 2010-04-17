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

$(eval $(call gb_Library_Library,xof))

$(eval $(call gb_Library_set_include,xof,\
    $$(SOLARINC) \
    -I$(WORKDIR)/inc/ \
    -I$(SRCDIR)/xmloff/inc \
    -I$(SRCDIR)/xmloff/source/inc \
    -I$(SRCDIR)/xmloff/inc/pch \
    -I$(OUTDIR)/inc/offuh \
    -I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_add_linked_libs,xof,\
    comphelper \
    cppu \
    cppuhelper \
    sal \
    salhelper \
    stl \
    xo \
))

$(eval $(call gb_Library_add_exception_objects,xof,\
    xmloff/source/transform/OOo2Oasis \
    xmloff/source/transform/StyleOOoTContext \
    xmloff/source/transform/EventOOoTContext \
    xmloff/source/transform/DocumentTContext \
    xmloff/source/transform/NotesTContext \
    xmloff/source/transform/Oasis2OOo \
    xmloff/source/transform/TransformerContext \
    xmloff/source/transform/ChartOASISTContext \
    xmloff/source/transform/PropertyActionsOASIS \
    xmloff/source/transform/DeepTContext \
    xmloff/source/transform/DlgOASISTContext \
    xmloff/source/transform/FormPropOOoTContext \
    xmloff/source/transform/TransformerBase \
    xmloff/source/transform/FormPropOASISTContext \
    xmloff/source/transform/RenameElemTContext \
    xmloff/source/transform/ControlOOoTContext \
    xmloff/source/transform/FrameOASISTContext \
    xmloff/source/transform/EventOASISTContext \
    xmloff/source/transform/TransformerTokenMap \
    xmloff/source/transform/PersMixedContentTContext \
    xmloff/source/transform/ProcAttrTContext \
    xmloff/source/transform/MetaTContext \
    xmloff/source/transform/TransformerActions \
    xmloff/source/transform/ControlOASISTContext \
    xmloff/source/transform/ChartPlotAreaOOoTContext \
    xmloff/source/transform/MutableAttrList \
    xmloff/source/transform/PropertyActionsOOo \
    xmloff/source/transform/ChartOOoTContext \
    xmloff/source/transform/EventMap \
    xmloff/source/transform/FlatTContext \
    xmloff/source/transform/CreateElemTContext \
    xmloff/source/transform/ProcAddAttrTContext \
    xmloff/source/transform/XMLFilterRegistration \
    xmloff/source/transform/MergeElemTContext \
    xmloff/source/transform/PersAttrListTContext \
    xmloff/source/transform/FrameOOoTContext \
    xmloff/source/transform/ChartPlotAreaOASISTContext \
    xmloff/source/transform/StyleOASISTContext \
    xmloff/source/transform/IgnoreTContext \
))

ifeq ($(OS),LINUX)
$(eval $(call gb_Library_add_linked_libs,xof,\
    icuuc \
    dl \
    m \
    pthread \
))
endif

ifeq ($(OS),WNT)
$(eval $(call gb_Library_add_linked_libs,xof,\
    kernel32 \
    msvcrt \
    oldnames \
    user32 \
    uwinapi \
))
endif
# vim: set noet ts=4 sw=4:
