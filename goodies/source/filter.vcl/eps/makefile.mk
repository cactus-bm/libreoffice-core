#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 02:45:33 $
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

PRJNAME=goodies
TARGET=eps
DEPTARGET=veps

#GEN_HID=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :	settings.mk

# --- Allgemein ----------------------------------------------------------

.IF "$(editdebug)"!="" || "$(EDITDEBUG)"!=""
CDEFS+= -DEDITDEBUG
.ENDIF

SRS1NAME=$(TARGET)
SRC1FILES =	dlgeps.src \
            epsstr.src

SLOFILES =	$(SLO)$/eps.obj \
            $(SLO)$/dlgeps.obj

# ==========================================================================

RESLIB1NAME=$(TARGET)
RESLIB1SRSFILES=$(SRS)$/$(TARGET).srs

SHL1TARGET= 	eps$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB= 	eps
SHL1STDLIBS=	$(TOOLSLIB) $(SVLIB) $(SVLIB) $(SVTOOLLIB) $(CPPULIB) $(SALLIB)

SHL1DEPN=		$(LB)$/eps.lib
SHL1LIBS=		$(SLB)$/eps.lib

.IF "$(GUI)" != "UNX"
SHL1OBJS=		$(SLO)$/eps.obj
.ENDIF

SHL1VERSIONMAP=exports.map
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

# ==========================================================================

.INCLUDE :	target.mk
