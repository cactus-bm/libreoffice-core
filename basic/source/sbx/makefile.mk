#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: hr $ $Date: 2006-06-19 17:48:33 $
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

PRJNAME=basic
TARGET=sbx

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk


# --- Allgemein -----------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES=	format.src

SLOFILES=	\
    $(SLO)$/sbxbase.obj	\
    $(SLO)$/sbxres.obj  	\
    $(SLO)$/sbxvalue.obj	\
    $(SLO)$/sbxvals.obj	\
    $(SLO)$/sbxvar.obj  	\
    $(SLO)$/sbxarray.obj	\
    $(SLO)$/sbxobj.obj	\
    $(SLO)$/sbxcoll.obj	\
    $(SLO)$/sbxexec.obj	\
    $(SLO)$/sbxint.obj 	\
    $(SLO)$/sbxlng.obj 	\
    $(SLO)$/sbxsng.obj 	\
    $(SLO)$/sbxmstrm.obj	\
    $(SLO)$/sbxdbl.obj 	\
    $(SLO)$/sbxcurr.obj 	\
    $(SLO)$/sbxdate.obj	\
    $(SLO)$/sbxstr.obj 	\
    $(SLO)$/sbxbool.obj	\
    $(SLO)$/sbxchar.obj	\
    $(SLO)$/sbxbyte.obj	\
    $(SLO)$/sbxuint.obj	\
    $(SLO)$/sbxulng.obj	\
    $(SLO)$/sbxform.obj	\
    $(SLO)$/sbxscan.obj 	\
    $(SLO)$/sbxdec.obj


EXCEPTIONSFILES=$(SLO)$/sbxarray.obj

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk


