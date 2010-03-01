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

PRJ=..$/..$/..

PRJNAME=testshl2
TARGET=c5t_testresult
LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE
# ENABLE_RTTI=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

# CXXFILES = \
# 		  querytemplate.cxx	\
# 		  stringhelper.cxx

# ENVCFLAGSCXX+=-DCPPUNIT_BUILD_DLL
SLOFILES = \
    $(SLO)$/SynchronizedObject.obj \
    $(SLO)$/TestResult.obj \
    $(SLO)$/TestResultCollector.obj \
    $(SLO)$/TestSucessListener.obj \
    $(SLO)$/emacsTestResult.obj \
    $(SLO)$/log.obj \
    $(SLO)$/optionhelper.obj \
    $(SLO)$/outputter.obj \
    $(SLO)$/signal.obj \
    $(SLO)$/testshlTestResult.obj \
    $(SLO)$/treswrapper.obj

# currently unused
# 	$(SLO)$/TextTestResult.obj

LIB1TARGET= $(LB)$/$(TARGET).lib
LIB1OBJFILES=  $(SLOFILES)

.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"
LIB1ARCHIV=$(LB)$/lib$(TARGET)$(DLLPOSTFIX).a
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
LIB1ARCHIV=$(LB)$/lib$(TARGET)$(DLLPOSTFIX).a
.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

