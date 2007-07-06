#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.12 $
#
#   last change: $Author: rt $ $Date: 2007-07-06 07:43:44 $
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

PROJECTPCH4DLL=TRUE
PROJECTPCH=svxpch
PROJECTPCHSOURCE=$(PRJ)$/util$/svxpch

PRJNAME=svx
TARGET=tbxctrls
AUTOSEG=true
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

CFLAGS+=-DDG_DLL

# --- Files --------------------------------------------------------

SLOFILES=	\
        $(SLO)$/formatpaintbrushctrl.obj\
        $(SLO)$/fontworkgallery.obj\
        $(SLO)$/extrusioncontrols.obj \
        $(SLO)$/colrctrl.obj	\
        $(SLO)$/fillctrl.obj	\
        $(SLO)$/grafctrl.obj	\
        $(SLO)$/itemwin.obj		\
        $(SLO)$/layctrl.obj		\
        $(SLO)$/lboxctrl.obj	\
        $(SLO)$/linectrl.obj	\
        $(SLO)$/tbcontrl.obj	\
        $(SLO)$/tbxalign.obj	\
        $(SLO)$/tbxdrctl.obj    \
        $(SLO)$/verttexttbxctrl.obj \
        $(SLO)$/toolbarmenu.obj \
        $(SLO)$/tbxcolor.obj \
        $(SLO)$/tbxcolorupdate.obj

SRS1NAME=$(TARGET)
SRC1FILES = \
        fontworkgallery.src		\
        extrusioncontrols.src		\
        colrctrl.src			\
        lboxctrl.src			\
        linectrl.src			\
        tbcontrl.src			\
        tbxdraw.src				\
        grafctrl.src

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

