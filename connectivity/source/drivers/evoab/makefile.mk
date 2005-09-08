#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 05:48:40 $
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
TARGET=evoab

# --- Settings ----------------------------------
.IF "$(DBGUTIL_OJ)"!=""
ENVCFLAGS+=/FR$(SLO)$/
.ENDIF

.INCLUDE : settings.mk
.INCLUDE :  $(PRJ)$/version.mk

.IF "$(GUI)"=="UNX"
# --- Files -------------------------------------

EXCEPTIONSFILES=\
    $(SLO)$/LResultSet.obj			\
    $(SLO)$/LStatement.obj			\
    $(SLO)$/LPreparedStatement.obj	\
    $(SLO)$/LFolderList.obj			\
    $(SLO)$/LConfigAccess.obj		\
    $(SLO)$/LColumnAlias.obj		\
    $(SLO)$/LTable.obj				\
    $(SLO)$/LDatabaseMetaData.obj	\
    $(SLO)$/LCatalog.obj			\
    $(SLO)$/LColumns.obj			\
    $(SLO)$/LTables.obj				\
    $(SLO)$/LConnection.obj			\
    $(SLO)$/LServices.obj			\
    $(SLO)$/LDriver.obj

SLOFILES=\
    $(EXCEPTIONSFILES)				\
    $(SLO)$/LNoException.obj		\
    $(SLO)$/LDebug.obj


SHL1VERSIONMAP=$(TARGET).map


# --- Library -----------------------------------
#SHL1TARGET=$(TARGET)$(UPD)$(DLLPOSTFIX)
SHL1TARGET=	$(EVOAB_TARGET)$(EVOAB_MAJOR)
SHL1OBJS=$(SLOFILES)
SHL1STDLIBS=\
    $(CPPULIB)					\
    $(CPPUHELPERLIB)			\
    $(VOSLIB)					\
    $(OSLLIB)					\
    $(SVLLIB)					\
    $(SVLIB)					\
    $(TOOLSLIB)					\
    $(SVTOOLLIB)				\
    $(SVTLIB)					\
    $(UNOTOOLSLIB)				\
    $(UCBHELPERLIB)				\
    $(SALLIB)					\
    $(DBTOOLSLIB)				\
    $(DBFILELIB)				\
    $(COMPHELPERLIB)

.IF "$(DBFILELIB)" == ""
SHL1STDLIBS+= ifile$(UPD).lib
.ENDIF

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET)

SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)

.ENDIF
# --- Targets ----------------------------------

.INCLUDE : target.mk


