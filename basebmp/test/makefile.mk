#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: thb $ $Date: 2006-05-31 10:12:13 $
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

PRJ=..

PRJNAME=basebmp
TARGET=tests
TARGETTYPE=GUI

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

.IF "$(debug)"!="" || "$(DEBUG)"!=""

.IF "$(COM)"=="MSC"
# disable inlining for MSVC
CFLAGS += -Ob0
.ENDIF

.IF "$(COM)"=="GCC"
# disable inlining for gcc
CFLAGS += -fno-inline
.ENDIF

.ENDIF

# --- Common ----------------------------------------------------------

# BEGIN ----------------------------------------------------------------
# auto generated Target:tests by codegen.pl 
SHL1OBJS=  \
    $(SLO)$/basictest.obj		\
    $(SLO)$/filltest.obj		\
    $(SLO)$/linetest.obj

SHL1TARGET= tests
SHL1STDLIBS= 	$(SALLIB)		 \
                $(CPPUNITLIB)	 \
                $(BASEGFXLIB)	 \
                $(BASEBMPLIB)	

SHL1IMPLIB= i$(SHL1TARGET)

DEF1NAME    =$(SHL1TARGET)
SHL1VERSIONMAP = export.map 

# END ------------------------------------------------------------------

APP2TARGET= bmpdemo

APP2OBJS=	\
    $(OBJ)$/bmpdemo.obj

APP2STDLIBS=$(TOOLSLIB) 		\
            $(COMPHELPERLIB)	\
            $(BASEGFXLIB)	    \
            $(BASEBMPLIB)	    \
            $(CPPULIB)			\
            $(CPPUHELPERLIB)	\
            $(UCBHELPERLIB)		\
            $(SALLIB)			\
            $(VCLLIB)	

.IF "$(GUI)"!="UNX"
APP2DEF=	$(MISC)$/$(TARGET).def
.ENDIF

#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
SLOFILES=$(SHL1OBJS) 

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk
.INCLUDE : _cppunit.mk 
