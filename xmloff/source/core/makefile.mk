#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.23 $
#
#   last change: $Author: rt $ $Date: 2004-11-26 19:30:00 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

PRJ=..$/..

PRJNAME=xmloff
TARGET=core
AUTOSEG=true

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
CDEFS+=-DCONV_STAR_FONTS

# --- to build xmlkywd.obj in obj, too -----------------------------

OBJFILES =  $(OBJ)$/xmlkywd.obj
LIB2TARGET =$(LB)$/xmlkywd.lib
LIB2OBJFILES  =$(OBJFILES)

# --- Files --------------------------------------------------------

SLOFILES =	\
        $(SLO)$/unointerfacetouniqueidentifiermapper.obj \
        $(SLO)$/facreg.obj		\
        $(SLO)$/xmlcnitm.obj	\
        $(SLO)$/attrlist.obj	\
        $(SLO)$/i18nmap.obj		\
        $(SLO)$/nmspmap.obj		\
        $(SLO)$/unoatrcn.obj	\
        $(SLO)$/xmlehelp.obj	\
        $(SLO)$/xmlerror.obj	\
        $(SLO)$/xmlexp.obj		\
        $(SLO)$/xmlictxt.obj	\
        $(SLO)$/xmlimp.obj		\
        $(SLO)$/xmlkywd.obj		\
        $(SLO)$/xmltkmap.obj	\
        $(SLO)$/xmltoken.obj	\
        $(SLO)$/xmluconv.obj	\
        $(SLO)$/DomBuilderContext.obj \
        $(SLO)$/DomExport.obj \
        $(SLO)$/ProgressBarHelper.obj	\
        $(SLO)$/PropertySetMerger.obj	\
        $(SLO)$/SettingsExportHelper.obj	\
        $(SLO)$/DocumentSettingsContext.obj \
        $(SLO)$/XMLEmbeddedObjectExportFilter.obj \
        $(SLO)$/XMLEmbeddedObjectImportContext.obj \
        $(SLO)$/XMLBase64Export.obj \
        $(SLO)$/XMLBase64ImportContext.obj \
        $(SLO)$/XMLBasicExportFilter.obj

SVXLIGHTOBJFILES =	\
        $(OBJ)$/facreg.obj		\
        $(OBJ)$/xmlcnitm.obj	\
        $(OBJ)$/attrlist.obj	\
        $(OBJ)$/i18nmap.obj		\
        $(OBJ)$/nmspmap.obj		\
        $(OBJ)$/unoatrcn.obj	\
        $(OBJ)$/xmlehelp.obj	\
        $(OBJ)$/xmlerror.obj	\
        $(OBJ)$/xmlictxt.obj	\
        $(OBJ)$/xmlimp.obj		\
        $(OBJ)$/xmlkywd.obj		\
        $(OBJ)$/xmltkmap.obj	\
        $(OBJ)$/xmltoken.obj	\
        $(OBJ)$/xmluconv.obj	\
        $(OBJ)$/ProgressBarHelper.obj	\
        $(OBJ)$/PropertySetMerger.obj	\
        $(OBJ)$/DocumentSettingsContext.obj \
        $(OBJ)$/XMLEmbeddedObjectImportContext.obj \
        $(OBJ)$/XMLBase64ImportContext.obj

CPPUMAKERFLAGS=

UNOTYPES=\
    com.sun.star.formula.SymbolDescriptor
# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk
