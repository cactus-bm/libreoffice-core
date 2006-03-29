#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.14 $
#
#   last change: $Author: obo $ $Date: 2006-03-29 12:16:19 $
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
PRJINC=..$/..
PRJNAME=connectivity
TARGET=file

USE_LDUMP2=TRUE
USE_DEFFILE=TRUE
LDUMP=ldump2.exe

# --- Settings ----------------------------------
.IF "$(DBGUTIL_OJ)"!=""
ENVCFLAGS+=/FR$(SLO)$/
.ENDIF

.INCLUDE : settings.mk
.INCLUDE :  $(PRJ)$/version.mk

# --- Files -------------------------------------
EXCEPTIONSFILES=\
    $(SLO)$/FNoException.obj		\
    $(SLO)$/FConnection.obj			\
    $(SLO)$/FResultSetMetaData.obj	\
    $(SLO)$/FColumns.obj			\
    $(SLO)$/FTables.obj				\
    $(SLO)$/FTable.obj				\
    $(SLO)$/FCatalog.obj			\
    $(SLO)$/FResultSet.obj			\
    $(SLO)$/FStatement.obj			\
    $(SLO)$/FPreparedStatement.obj	\
    $(SLO)$/FDatabaseMetaData.obj	\
    $(SLO)$/FDriver.obj				\
    $(SLO)$/fanalyzer.obj			\
    $(SLO)$/fcomp.obj

SLOFILES=\
    $(EXCEPTIONSFILES)				\
    $(SLO)$/fcode.obj				\
    $(SLO)$/FStringFunctions.obj	\
    $(SLO)$/FDateFunctions.obj	\
    $(SLO)$/FNumericFunctions.obj



# --- Library -----------------------------------
SHL1TARGET=$(TARGET)$(UPD)$(DLLPOSTFIX)
SHL1OBJS=$(SLOFILES)
SHL1STDLIBS=\
    $(CPPULIB)					\
    $(CPPUHELPERLIB)			\
    $(VOSLIB)					\
    $(OSLLIB)					\
    $(SVLLIB)					\
    $(TOOLSLIB)					\
    $(SVTOOLLIB)				\
    $(UCBHELPERLIB)				\
    $(SALLIB)					\
    $(DBTOOLSLIB)				\
    $(UNOTOOLSLIB)				\
    $(COMPHELPERLIB)

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET)$(UPD)

SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)
DEF1DEPN=	$(MISC)$/$(SHL1TARGET).flt \
            $(SLB)$/$(TARGET).lib
DEFLIB1NAME=$(TARGET)

# --- Targets ----------------------------------

.INCLUDE : target.mk


# --- filter file ------------------------------

.IF "$(depend)"==""

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo CLEAR_THE_FILE	> $@
    @echo _TI				>>$@
    @echo _real				>>$@
.ENDIF

