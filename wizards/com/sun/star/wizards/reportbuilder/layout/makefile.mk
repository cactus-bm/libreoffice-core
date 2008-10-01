#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.2.26.1 $
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

PRJ		= ..$/..$/..$/..$/..$/..
PRJNAME = wizards
TARGET  = reportbuilderwizardlayouts
PACKAGE = com$/sun$/star$/wizards$/reportbuilder$/layout

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
#.INCLUDE :  $(PRJ)$/util$/makefile.pmk

JARFILES= unoil.jar jurt.jar ridl.jar juh.jar java_uno.jar commonwizards.jar report.jar

# CUSTOMMANIFESTFILE= MANIFEST.MF

JARCLASSDIRS	= \
    com$/sun$/star$/wizards$/reportbuilder$/layout \
    com$/sun$/star$/wizards$/report

# JARTARGET	= $(TARGET).jar
# JARCLASSPATH = commonwizards.jar report.jar

# --- Files --------------------------------------------------------

JAVAFILES=                     \
    ReportBuilderLayouter.java \
    Tabular.java               \
    ColumnarSingleColumn.java  \
    ColumnarTwoColumns.java    \
    ColumnarThreeColumns.java  \
    InBlocksLabelsAbove.java   \
    InBlocksLabelsLeft.java    \
    SectionObject.java         \
    SectionEmptyObject.java    \
    SectionLabel.java          \
    SectionTextField.java      \
    DesignTemplate.java

JAVACLASSFILES = $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

