#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: hr $ $Date: 2004-08-03 11:36:52 $
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

PRJ=..$/..$/..

PRJNAME=sc
TARGET=dbgui
LIBTARGET=no
PROJECTPCH4DLL=TRUE
PROJECTPCH=ui_pch
PDBTARGET=ui_pch
PROJECTPCHSOURCE=..\pch\ui_pch

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES =	\
    $(SLO)$/sortdlg.obj		\
    $(SLO)$/tpsort.obj		\
    $(SLO)$/filtdlg.obj		\
    $(SLO)$/sfiltdlg.obj	\
    $(SLO)$/foptmgr.obj		\
    $(SLO)$/pfiltdlg.obj	\
    $(SLO)$/dbnamdlg.obj	\
    $(SLO)$/expftext.obj	\
    $(SLO)$/subtdlg.obj		\
    $(SLO)$/tpsubt.obj		\
    $(SLO)$/fieldwnd.obj	\
    $(SLO)$/pvlaydlg.obj	\
    $(SLO)$/pvfundlg.obj	\
    $(SLO)$/dpgroupdlg.obj	\
    $(SLO)$/dapitype.obj	\
    $(SLO)$/dapidata.obj	\
    $(SLO)$/consdlg.obj		\
    $(SLO)$/scendlg.obj		\
    $(SLO)$/imoptdlg.obj	\
    $(SLO)$/validate.obj	\
    $(SLO)$/csvsplits.obj	\
    $(SLO)$/csvcontrol.obj	\
    $(SLO)$/csvruler.obj	\
    $(SLO)$/csvgrid.obj		\
    $(SLO)$/csvtablebox.obj	\
    $(SLO)$/asciiopt.obj	\
    $(SLO)$/scuiasciiopt.obj	\
     $(SLO)$/scuiimoptdlg.obj

EXCEPTIONSFILES= \
    $(SLO)$/dapidata.obj

SRS1NAME=$(TARGET)
SRC1FILES =  \
        pivot.src		\
        pvfundlg.src	\
        dpgroupdlg.src	\
        dapitype.src	\
        consdlg.src		\
        scendlg.src     \
        imoptdlg.src    \
        validate.src    \
        asciiopt.src    \
        outline.src

LIB1TARGET = $(SLB)$/$(TARGET).lib

LIB1OBJFILES =  \
    $(SLO)$/filtdlg.obj		\
    $(SLO)$/sfiltdlg.obj	\
    $(SLO)$/foptmgr.obj		\
    $(SLO)$/dbnamdlg.obj	\
    $(SLO)$/expftext.obj	\
    $(SLO)$/fieldwnd.obj	\
    $(SLO)$/pvlaydlg.obj	\
    $(SLO)$/pvfundlg.obj	\
    $(SLO)$/dpgroupdlg.obj	\
    $(SLO)$/consdlg.obj		\
    $(SLO)$/imoptdlg.obj	\
    $(SLO)$/csvsplits.obj	\
    $(SLO)$/csvcontrol.obj	\
    $(SLO)$/csvruler.obj	\
    $(SLO)$/csvgrid.obj		\
    $(SLO)$/csvtablebox.obj	\
    $(SLO)$/asciiopt.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk


