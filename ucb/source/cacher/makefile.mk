#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.12 $
#
#   last change: $Author: ihi $ $Date: 2007-06-05 17:51:22 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ=..$/..
PRJNAME=ucb
TARGET=cached
ENABLE_EXCEPTIONS=TRUE
NO_BSYMBOLIC=TRUE

# Version
UCB_MAJOR=1

.INCLUDE: settings.mk

SLOFILES=\
    $(SLO)$/contentresultsetwrapper.obj \
    $(SLO)$/cachedcontentresultsetstub.obj \
    $(SLO)$/cachedcontentresultset.obj \
    $(SLO)$/dynamicresultsetwrapper.obj \
    $(SLO)$/cacheddynamicresultsetstub.obj \
    $(SLO)$/cacheddynamicresultset.obj \
    $(SLO)$/cacheserv.obj

LIB1TARGET=$(SLB)$/_$(TARGET).lib
LIB1OBJFILES=$(SLOFILES)

SHL1TARGET=$(TARGET)$(UCB_MAJOR)
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
SHL1STDLIBS=\
    $(CPPUHELPERLIB) \
    $(CPPULIB) \
    $(SALLIB)

SHL1LIBS=$(LIB1TARGET)
SHL1IMPLIB=i$(TARGET)

SHL1VERSIONMAP=exports.map

DEF1NAME=$(SHL1TARGET)

.INCLUDE: target.mk

