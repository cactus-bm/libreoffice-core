#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 03:20:06 $
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
PRJNAME=sd
TARGET=eppt
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

.IF "$(COM)"=="GCC"
NOOPTFILES= $(SLO)$/epptso.obj
.ENDIF

SLOFILES =	$(SLO)$/eppt.obj			\
            $(SLO)$/epptso.obj			\
            $(SLO)$/escherex.obj		\
            $(SLO)$/dinfos2.obj			\
            $(SLO)$/pptexanimations.obj

SHL1TARGET	=	emp$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB	=	eppt
SHL1DEPN	=	$(LB)$/eppt.lib
SHL1VERSIONMAP=exports.map
SHL1DEF 	=	$(MISC)$/$(SHL1TARGET).def
SHL1LIBS	=	$(SLB)$/eppt.lib

DEF1NAME=$(SHL1TARGET)

SHL1STDLIBS = 	\
                $(SVXLIB)			\
                $(SOTLIB)           \
                $(GOODIESLIB)		\
                $(VCLLIB)			\
                $(SOTLIB)			\
                $(UNOTOOLSLIB)		\
                $(TOOLSLIB) 		\
                $(UCBHELPERLIB)		\
                $(CPPULIB)			\
                $(SALLIB)

# --- Targets --------------------------------------------------------------

.INCLUDE : target.mk
