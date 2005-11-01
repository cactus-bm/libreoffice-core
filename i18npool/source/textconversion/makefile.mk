#*************************************************************************
#*
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: kz $ $Date: 2005-11-01 14:57:30 $
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
#************************************************************************/

PRJ=..$/..

PRJNAME=i18npool
TARGET=textconversion

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :	svpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	sv.mk

# --- Files --------------------------------------------------------

MY_MISC_CXXFILES = \
        $(MISC)$/hhc_char.cxx \
        $(MISC)$/stc_char.cxx \
        $(MISC)$/stc_word.cxx

SLOFILES=   \
        $(SLO)$/textconversionImpl.obj \
        $(SLO)$/textconversion.obj \
        $(SLO)$/textconversion_ko.obj \
        $(SLO)$/textconversion_zh.obj \
        $(SLO)$/hhc_char.obj \
        $(SLO)$/stc_char.obj \
        $(SLO)$/stc_word.obj

APP1TARGET = genconv_dict

APP1OBJS   = $(OBJ)$/genconv_dict.obj

APP1STDLIBS = $(SALLIB)

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

data$/%.dic : $(APP1TARGETN)
    +@echo dummy

$(MISC)$/%.cxx : data$/%.dic
    $(APP1TARGETN)$ $* $< $@



