#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.26 $
#
#   last change: $Author: rt $ $Date: 2006-03-08 14:01:39 $
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

PRJ=..$/..

PRJINC=$(PRJ)$/source
PRJNAME=configmgr
TARGET=xml
ENABLE_EXCEPTIONS=TRUE

# --- Settings ---

.INCLUDE : settings.mk

.IF "$(OS)"=="SOLARIS" && "$(COM)"!="GCC"
CFLAGSCXX+=-instances=static
.ENDIF

# --- Files ---


SLOFILES=\
    $(SLO)$/matchlocale.obj \
    $(SLO)$/typeconverter.obj \
    $(SLO)$/simpletypehelper.obj \
    $(SLO)$/valueconverter.obj \
    $(SLO)$/elementparser.obj \
    $(SLO)$/elementformatter.obj \
    $(SLO)$/basicparser.obj \
    $(SLO)$/layerparser.obj \
    $(SLO)$/schemaparser.obj \
    $(SLO)$/parsersvc.obj \
    $(SLO)$/writersvc.obj \
    $(SLO)$/layerwriter.obj \
    $(SLO)$/valueformatter.obj \
    $(SLO)$/xmlstrings.obj \

# --- Targets ---

.INCLUDE : target.mk

