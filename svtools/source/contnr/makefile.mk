#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: fs $ $Date: 2002-07-19 13:27:41 $
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

PRJNAME=SVTOOLS
TARGET=svcontnr

PROJECTPCH4DLL=TRUE
PROJECTPCH=cont_pch
PROJECTPCHSOURCE=cont_pch

# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk

# --- Files --------------------------------------------------------

.IF "$(COM)"=="WTC"
CFLAGS=$(CFLAGS) -od -3r
.ENDIF

.IF "$(header)" == ""

CXXFILES=   svtabbx.cxx		\
            svicnvw.cxx		\
            svimpicn.cxx	\
            treelist.cxx	\
            svlbox.cxx		\
            svimpbox.cxx	\
            svlbitm.cxx		\
            svtreebx.cxx	\
            cont_pch.cxx	\
            ctrdll.cxx		\
            imivctl1.cxx	\
            imivctl2.cxx	\
            ivctrl.cxx		\
            fileview.cxx

SLOFILES=   $(SLO)$/svtabbx.obj		\
            $(SLO)$/svicnvw.obj		\
            $(SLO)$/svimpicn.obj	\
            $(SLO)$/treelist.obj	\
            $(SLO)$/svlbox.obj		\
            $(SLO)$/svimpbox.obj	\
            $(SLO)$/svlbitm.obj		\
            $(SLO)$/svtreebx.obj	\
            $(SLO)$/imivctl1.obj	\
            $(SLO)$/imivctl2.obj	\
            $(SLO)$/ivctrl.obj		\
            $(SLO)$/fileview.obj	\
            $(SLO)$/templwin.obj

EXCEPTIONSFILES=\
            $(SLO)$/fileview.obj	\
            $(SLO)$/templwin.obj

# Fuer das Setup bauen wir noch mal statisch
.IF "$(VCSID)" != "OV"

OBJFILES=	$(OBJ)$/svtabbx.obj	\
            $(OBJ)$/svicnvw.obj	\
            $(OBJ)$/svimpicn.obj	\
            $(OBJ)$/treelist.obj	\
            $(OBJ)$/svlbox.obj	\
            $(OBJ)$/svimpbox.obj	\
            $(OBJ)$/svlbitm.obj	\
            $(OBJ)$/svtreebx.obj

.ENDIF

.ENDIF

SRCFILES =\
            fileview.src	\
            templwin.src	\
            svcontnr.src

HXX1TARGET=	svcontnr
HXX1EXT=	hxx
HXX1FILES=	$(PRJ)$/inc$/svlbox.hxx \
        $(PRJ)$/inc$/svlbitm.hxx \
        $(PRJ)$/inc$/svtreebx.hxx \
        $(PRJ)$/inc$/svicnvw.hxx \
        $(PRJ)$/inc$/svtabbx.hxx \
        $(PRJ)$/inc$/treelist.hxx
HXX1EXCL=	-E:*include*

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
