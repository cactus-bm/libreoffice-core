#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: obo $ $Date: 2004-11-16 11:02:58 $
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

PRJ=..
PRJNAME=forms
TARGET=frm
USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :	svpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	sv.mk

LDUMP=ldump2.exe

# --- Library -----------------------------------
# ��� frm ���������������������������������������
LIB1TARGET=$(SLB)$/forms.lib
LIB1FILES=\
        $(SLB)$/common.lib \
        $(SLB)$/resource.lib \
        $(SLB)$/component.lib \
        $(SLB)$/helper.lib \
        $(SLB)$/solarcomponent.lib  \
        $(SLB)$/solarcontrol.lib \
        $(SLB)$/richtext.lib \
        $(SLB)$/xforms.lib \
        $(SLB)$/xformssubmit.lib \
        $(SLB)$/xformsxpath.lib

SHL1TARGET=$(TARGET)$(UPD)$(DLLPOSTFIX)

SHL1STDLIBS= \
        $(SALLIB) \
        $(OSLLIB) \
        $(ONELIB) \
        $(CPPULIB) \
        $(CPPUHELPERLIB) \
        $(TOOLSLIB) \
        $(VCLLIB) \
        $(SVTOOLLIB) \
        $(SVLLIB)	\
        $(TKLIB) \
        $(SFX2LIB) \
        $(VOSLIB) \
        $(UNOTOOLSLIB) \
        $(COMPHELPERLIB) \
        $(DBTOOLSLIB) \
        $(TKLIB) \
        $(SVXLIB) \
        $(UCBHELPERLIB) \
        $(XML2LIB) \
        $(ICUUCLIB) \
        $(ICUINLIB)

SHL1LIBS=$(LIB1TARGET)
SHL1DEPN=$(LIB1TARGET)	\
        makefile.mk


SHL1VERSIONMAP=$(TARGET).map 
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

# === .res file ==========================================================

RES1FILELIST=\
    $(SRS)$/resource.srs \

RESLIB1NAME=$(TARGET)
RESLIB1SRSFILES=$(RES1FILELIST)

.IF "$(GUI)"=="UNX"

# [ed] 6/19/02 Only add in libraries for X11 OS X builds

.IF "$(OS)"=="MACOSX"
.IF "$(GUIBASE)"=="unx"
SHL1STDLIBS +=\
        -lX11 -lXt -lXmu
.ENDIF
.ELSE
SHL1STDLIBS +=\
    -lX11
.ENDIF # OS == MACOSX

.ENDIF

# --- Targets ----------------------------------

.INCLUDE : target.mk

# --- Filter-Datei ---

.IF "$(depend)"==""

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo CLEAR_THE_FILE			> $@
    @echo __CT				    >>$@
    @echo createRegistryInfo    >>$@
    @echo queryInterface        >>$@
    @echo queryAggregation      >>$@
    @echo NavigationToolBar     >>$@
    @echo ONavigationBar        >>$@
.ENDIF

