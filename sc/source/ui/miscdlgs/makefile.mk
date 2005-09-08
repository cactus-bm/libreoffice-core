#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 22:11:51 $
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

PRJNAME=sc
TARGET=miscdlgs
LIBTARGET=no

PROJECTPCH4DLL=TRUE
PROJECTPCH=ui_pch
PDBTARGET=ui_pch
PROJECTPCHSOURCE=..$/pch$/ui_pch

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES =  \
    $(SLO)$/delcldlg.obj		\
    $(SLO)$/inscldlg.obj		\
    $(SLO)$/delcodlg.obj		\
    $(SLO)$/inscodlg.obj		\
    $(SLO)$/strindlg.obj		\
    $(SLO)$/mtrindlg.obj		\
    $(SLO)$/lbseldlg.obj		\
    $(SLO)$/filldlg.obj			\
    $(SLO)$/autofmt.obj			\
    $(SLO)$/solvrdlg.obj		\
    $(SLO)$/mvtabdlg.obj		\
    $(SLO)$/groupdlg.obj		\
    $(SLO)$/tabopdlg.obj		\
    $(SLO)$/crdlg.obj			\
    $(SLO)$/namecrea.obj		\
    $(SLO)$/namepast.obj		\
    $(SLO)$/teamdlg.obj			\
    $(SLO)$/textdlgs.obj		\
    $(SLO)$/chartdlg.obj		\
    $(SLO)$/anyrefdg.obj		\
    $(SLO)$/crnrdlg.obj			\
    $(SLO)$/shtabdlg.obj		\
    $(SLO)$/instbdlg.obj		\
    $(SLO)$/acredlin.obj		\
    $(SLO)$/highred.obj			\
    $(SLO)$/simpref.obj			\
    $(SLO)$/redcom.obj			\
    $(SLO)$/linkarea.obj		\
    $(SLO)$/warnbox.obj	\
    $(SLO)$/scuiautofmt.obj

SRS1NAME=$(TARGET)
SRC1FILES = \
    instbdlg.src				\
    acredlin.src				\
    highred.src					\
    linkarea.src

NOOPTFILES=\
    $(SLO)$/chartdlg.obj
    
LIB1TARGET = $(SLB)$/$(TARGET).lib

LIB1OBJFILES =  \
    $(SLO)$/autofmt.obj			\
    $(SLO)$/solvrdlg.obj		\
    $(SLO)$/tabopdlg.obj		\
    $(SLO)$/teamdlg.obj			\
    $(SLO)$/chartdlg.obj		\
    $(SLO)$/anyrefdg.obj		\
    $(SLO)$/crnrdlg.obj			\
    $(SLO)$/acredlin.obj		\
    $(SLO)$/highred.obj			\
    $(SLO)$/simpref.obj			\
    $(SLO)$/redcom.obj			\
    $(SLO)$/warnbox.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

