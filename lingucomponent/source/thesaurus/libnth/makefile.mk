#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: obo $ $Date: 2007-03-09 08:50:17 $
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

PRJ = ..$/..$/..

PRJNAME	= lingucomponent
TARGET	= lnth
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE


.IF "$(MYTHESLIB)"==""
.IF "$(GUI)"=="UNX"
MYTHESLIB=-lmythes
.ENDIF # unx
.IF "$(GUI)"=="WNT"
MYTHESLIB=libmythes.lib
.ENDIF # wnt
.ENDIF

.IF "$(ULINGULIB)"==""
.IF "$(GUI)"=="UNX"
ULINGULIB=-lulingu
.ENDIF # unx
.IF "$(GUI)"=="WNT"
ULINGULIB=libulingu.lib
.ENDIF # wnt
.ENDIF


#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

.IF "$(SYSTEM_MYSPELL)" == "YES" && "$(SYSTEM_MYTHES)" == "YES"
CXXFLAGS += $(MYSPELL_CFLAGS)
CFLAGSCXX += $(MYSPELL_CFLAGS)
CFLAGSCC += $(MYSPELL_CFLAGS)
.ENDIF
.IF "$(SYSTEM_MYSPELL)" == "YES" && "$(SYSTEM_MYTHES)" != "YES"
CXXFLAGS += -I..$/mythes $(MYSPELL_CFLAGS)
CFLAGSCXX += -I..$/mythes $(MYSPELL_CFLAGS)
CFLAGSCC += -I..$/mythes $(MYSPELL_CFLAGS)
.ENDIF
.IF "$(SYSTEM_MYPSPELL)" != "YES" && "$(SYSTEM_MYTHES)" == "YES"
CXXFLAGS += -I..$/..$/lingutil
CFLAGSCXX += -I..$/..$/lingutil
CFLAGSCC += -I..$/..$/lingutil
.ENDIF
.IF "$(SYSTEM_MYSPELL)" != "YES" && "$(SYSTEM_MYTHES)" != "YES"
CXXFLAGS += -I..$/mythes -I..$/..$/lingutil
CFLAGSCXX += -I..$/mythes -I..$/..$/lingutil
CFLAGSCC += -I..$/mythes -I..$/..$/lingutil
.ENDIF

EXCEPTIONSFILES=	\
        $(SLO)$/ntprophelp.obj\
        $(SLO)$/nthesimp.obj

SLOFILES=	\
        $(SLO)$/nthesdta.obj\
        $(SLO)$/ntprophelp.obj\
        $(SLO)$/ntreg.obj\
        $(SLO)$/nthesimp.obj


SHL1TARGET= $(TARGET)$(UPD)$(DLLPOSTFIX)

SHL1STDLIBS= \
        $(CPPULIB) 	 \
        $(CPPUHELPERLIB) 	 \
        $(VOSLIB)		\
        $(TOOLSLIB)		\
        $(SVTOOLLIB)	\
        $(SVLLIB)		\
        $(VCLLIB)		\
        $(SALLIB)		\
        $(UCBHELPERLIB)	\
        $(UNOTOOLSLIB)	\
        $(LNGLIB) \
                $(MYTHESLIB)

.IF "$(SYSTEM_MYSPELL)" != "YES"
SHL1STDLIBS+=   $(ULINGULIB)
.ELSE
SHL1STDLIBS+=   $(MYSPELL_LIBS)
.ENDIF
# build DLL
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1IMPLIB=		i$(TARGET)
SHL1DEPN=		$(SHL1LIBS)
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def

SHL1VERSIONMAP= $(TARGET).map

# build DEF file
DEF1NAME	 =$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

