#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: obo $ $Date: 2006-07-13 13:21:39 $
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
PRJ=..$/..$/..

PRJNAME=shell
TARGET=kdebe

LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE

COMP1TYPELIST=$(TARGET)
COMPRDB=$(SOLARBINDIR)$/types.rdb
UNOUCROUT=$(OUT)$/inc$/$(TARGET)
INCPRE=$(UNOUCROUT)

# --- Settings ---

.INCLUDE : settings.mk

UNIXTEXT=$(MISC)/$(TARGET)1-ucd.txt

# no "lib" prefix
DLLPRE =

.IF "$(ENABLE_KDE)" == "TRUE"

CFLAGS+=$(KDE_CFLAGS)

# --- Files ---

SLOFILES=\
    $(SLO)$/kdebackend.obj \
    $(SLO)$/kdecommonlayer.obj \
    $(SLO)$/kdeinetlayer.obj \
    $(SLO)$/kdevcllayer.obj \
    $(SLO)$/kdepathslayer.obj \
    $(SLO)$/kdebecdef.obj

SHL1NOCHECK=TRUE
SHL1TARGET=$(TARGET)1.uno   
SHL1OBJS=$(SLOFILES)
SHL1DEF=$(MISC)$/$(SHL1TARGET).def

SHL1IMPLIB=i$(SHL1TARGET)
SHL1STDLIBS=    \
        $(CPPUHELPERLIB) \
        $(CPPULIB) \
        $(SALLIB) \
        $(KDE_LIBS) -lkio
        
SHL1VERSIONMAP=exports.map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

.ENDIF          # "$(ENABLE_KDE)" == "TRUE"

# --- Targets ---

.INCLUDE : target.mk
