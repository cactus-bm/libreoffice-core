#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: ihi $ $Date: 2007-09-13 17:57:17 $
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
TARGET=macab
TARGET2=$(TARGET)drv

ENABLE_EXCEPTIONS=TRUE

# --- Settings ----------------------------------

.INCLUDE : settings.mk
.INCLUDE :  $(PRJ)$/version.mk

.IF "$(GUI)" == "UNX"
.IF "$(OS)" == "MACOSX"

# === MACAB base library ==========================

# --- Files -------------------------------------

SLOFILES= \
    $(SLO)$/MacabDriver.obj     \
    $(SLO)$/MacabServices.obj

DEPOBJFILES= \
    $(SLO2FILES)

# --- Library -----------------------------------

SHL1VERSIONMAP=$(TARGET).map

SHL1TARGET= $(TARGET)$(MACAB_MAJOR)
SHL1OBJS=$(SLOFILES)
SHL1STDLIBS=\
    $(CPPULIB)                  \
    $(CPPUHELPERLIB)            \
    $(SALLIB)

SHL1DEPN=
SHL1IMPLIB= i$(TARGET)

SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME=   $(SHL1TARGET)

# === MACAB impl library ==========================

# --- Files -------------------------------------

SLO2FILES=\
    $(SLO)$/MacabColumns.obj            \
    $(SLO)$/MacabTable.obj              \
    $(SLO)$/MacabTables.obj             \
    $(SLO)$/MacabCatalog.obj            \
    $(SLO)$/MacabResultSet.obj          \
    $(SLO)$/MacabStatement.obj          \
    $(SLO)$/MacabPreparedStatement.obj  \
    $(SLO)$/MacabDatabaseMetaData.obj   \
    $(SLO)$/MacabConnection.obj         \
    $(SLO)$/MacabResultSetMetaData.obj  \
    $(SLO)$/macabcondition.obj          \
    $(SLO)$/macaborder.obj              \
        $(SLO)$/MacabRecord.obj             \
        $(SLO)$/MacabRecords.obj            \
        $(SLO)$/MacabHeader.obj             \
        $(SLO)$/MacabGroup.obj              \
        $(SLO)$/MacabAddressBook.obj
 
MACAB_LIB=-framework Carbon -framework AddressBook

# --- Library -----------------------------------

SHL2VERSIONMAP=$(TARGET2).map

SHL2TARGET= $(TARGET2)$(MACAB_MAJOR)
SHL2OBJS=$(SLO2FILES)
SHL2STDLIBS=\
    $(CPPULIB)                  \
    $(CPPUHELPERLIB)            \
    $(VOSLIB)                   \
    $(SALLIB)                   \
    $(DBTOOLSLIB)               \
    $(COMPHELPERLIB)            \
    $(MACAB_LIB)

SHL2DEPN=
SHL2IMPLIB= i$(TARGET2)

SHL2DEF=    $(MISC)$/$(SHL2TARGET).def

DEF2NAME=   $(SHL2TARGET)

# --- Targets -----------------------------------
.ELSE		# "$(OS)" == "MACOSX"
dummy:
    @echo Not using Mac OS X - nothing to build
.ENDIF

.ELSE		# "$(GUI)" == "UNX"
dummy:
    @echo "Nothing to build for GUI $(GUI)"
.ENDIF

.INCLUDE : target.mk

