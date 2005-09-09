#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.21 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 16:06:30 $
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

PRJNAME=xmloff
TARGET=xo
USE_DEFFILE=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE: $(PRJ)$/util$/makefile.pmk

# --- Allgemein ----------------------------------------------------
NO_BSYMBOLIC=TRUE
ENABLE_EXCEPTIONS=TRUE

LIB1TARGET= $(SLB)$/xo.lib
LIB1FILES=	\
    $(SLB)$/core.lib \
    $(SLB)$/meta.lib \
    $(SLB)$/script.lib \
    $(SLB)$/style.lib \
    $(SLB)$/text.lib \
    $(SLB)$/draw.lib \
    $(SLB)$/chart.lib \
    $(SLB)$/forms.lib \
    $(SLB)$/xforms.lib

# --- Shared-Library -----------------------------------------------

.IF "$(GUI)"!="UNX"
LIB4TARGET= $(LB)$/ixo.lib
LIB4FILES=	$(LB)$/_ixo.lib
LIB4OBJFILES=\
    $(OBJ)$/xmlkywd.obj
.ENDIF

SHL1TARGET= xo$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB= _ixo
SHL1USE_EXPORTS=ordinal

SHL1STDLIBS= \
        $(SVTOOLLIB)	\
        $(TOOLSLIB)		\
        $(VOSLIB)		\
        $(CPPULIB)		\
        $(CPPUHELPERLIB)\
        $(COMPHELPERLIB)\
        $(RTLLIB)		\
        $(SVLIB)		\
        $(TKLIB)		\
        $(SVLLIB)		\
        $(SALLIB)		\
        $(UNOTOOLSLIB)	\
        $(ONELIB)		\
        $(GOODIESLIB)	\
        $(SALHELPERLIB)

# SCO: the linker does know about weak symbols, but we can't ignore multiple defined symbols
.IF "(OS)"=="SCO" || "$(OS)$(COM)"=="OS2GCC"
SHL1STDLIBS+=-licg617mxp
.ENDIF

SHL1DEF=    $(MISC)$/$(SHL1TARGET).def
SHL1LIBS=   $(LIB1TARGET)


SHL5STDLIBS= \
        $(TOOLSLIB)         \
        $(RTLLIB)           \
        $(SALLIB)           \
        $(SALHELPERLIB)     \
        $(CPPULIB)          \
        $(CPPUHELPERLIB)    \
        $(ONELIB)           \
        $(COMPHELPERLIB)

.IF "$(GUI)"=="UNX" || "$(GUI)"=="MAC"
    SHL5STDLIBS += -lxo$(OFFICEUPD)$(DLLPOSTFIX)
.ELSE
    SHL5STDLIBS += $(LIBPRE) ixo.lib
.ENDIF

# --- Static-Lib ---------------------------------------------------------

.IF "$(SVXLIGHT)" != "" 
LIB2TARGET= $(LB)$/xol.lib
LIB2ARCHIV= $(LB)$/libxol.a
LIB2FILES=  \
            $(LB)$/sxl_core.lib\
            $(LB)$/sxl_meta.lib\
            $(LB)$/sxl_draw.lib\
            $(LB)$/sxl_forms.lib\
            $(LB)$/sxl_script.lib\
            $(LB)$/sxl_style.lib\
            $(LB)$/sxl_text.lib \
            $(LB)$/sxl_transform.lib
.ENDIF

# --- Def-File ---------------------------------------------------------

DEF1NAME    =$(SHL1TARGET)
DEFLIB1NAME =xo
DEF1DES     =XML Office Lib

# --- Targets ----------------------------------------------------------

.INCLUDE :  target.mk
