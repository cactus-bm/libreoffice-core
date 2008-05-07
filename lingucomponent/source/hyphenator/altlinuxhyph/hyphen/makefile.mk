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
# $Revision: 1.21 $
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

PRJ = ..$/..$/..$/..

PRJNAME	= lingucomponent
TARGET	= hyphen_lib
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

.IF "$(GUI)"=="UNX"
HNJLIB=-lhyphen
.ELIF  "$(GUI)"=="OS2"
HNJLIB=$(SLB)\hyphen.lib
.ELSE
HNJLIB=hyphen.lib
.ENDIF

.IF "$(ULINGULIB)"==""
.IF "$(GUI)"=="UNX"
ULINGULIB=-lulingu
.ENDIF # unx
.IF "$(GUI)"=="OS2"
ULINGULIB=$(SLB)\libulingu.lib
.ENDIF # unx
.IF "$(GUI)"=="WNT"
ULINGULIB=libulingu.lib
.ENDIF # wnt
.ENDIF

#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------


CFLAGS += -I..$/..$/..$/lingutil

EXCEPTIONSFILES=	\
        $(SLO)$/hprophelp.obj\
        $(SLO)$/hyphenimp.obj

SLOFILES=	\
        $(SLO)$/hprophelp.obj\
        $(SLO)$/hreg.obj\
        $(SLO)$/hyphenimp.obj

REALNAME:=hyphen
SHL1TARGET= $(REALNAME)$(DLLPOSTFIX)

SHL1STDLIBS= \
        $(CPPULIB) 	 \
        $(CPPUHELPERLIB) 	 \
        $(VOSLIB)		\
        $(I18NISOLANGLIB)   \
        $(TOOLSLIB)		\
        $(SVTOOLLIB)	\
        $(SVLLIB)		\
        $(VCLLIB)		\
        $(SALLIB)		\
        $(UCBHELPERLIB)	\
        $(UNOTOOLSLIB)	\
        $(LNGLIB) \
        $(HNJLIB)

.IF "$(SYSTEM_HUNSPELL)" != "YES"
SHL1STDLIBS+=   $(ULINGULIB)
.ELSE
SHL1STDLIBS+=   $(HUNSPELL_LIBS)
.ENDIF

# build DLL
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1IMPLIB=		i$(REALNAME)
SHL1DEPN=		$(SHL1LIBS)
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def

SHL1VERSIONMAP= $(REALNAME).map

# build DEF file
DEF1NAME	 =$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

