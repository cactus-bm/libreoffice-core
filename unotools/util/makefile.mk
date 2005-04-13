#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.15 $
#
#   last change: $Author: obo $ $Date: 2005-04-13 12:31:55 $
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
PRJNAME=unotools
TARGET=utl
TARGETTYPE=CUI
USE_LDUMP2=TRUE

USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :	settings.mk

LDUMP=ldump2.exe

# --- Library -----------------------------------

LIB1TARGET=$(SLB)$/untools.lib
LIB1FILES=\
        $(SLB)$/i18n.lib \
        $(SLB)$/misc.lib \
        $(SLB)$/streaming.lib \
        $(SLB)$/config.lib \
        $(SLB)$/ucbhelp.lib \
        $(SLB)$/procfact.lib \
        $(SLB)$/property.lib \
        $(SLB)$/accessibility.lib

SHL1TARGET=$(TARGET)$(UPD)$(DLLPOSTFIX)

SHL1IMPLIB=iutl
SHL1USE_EXPORTS=ordinal

SHL1STDLIBS= \
        $(SALHELPERLIB) \
        $(COMPHELPERLIB) \
        $(UCBHELPERLIB) \
        $(CPPUHELPERLIB) \
        $(CPPULIB) \
        $(TOOLSLIB) \
        $(VOSLIB) \
        $(SALLIB)

SHL1LIBS=$(LIB1TARGET)
SHL1DEF=$(MISC)$/$(SHL1TARGET).def

SHL1DEPN=$(LIB1TARGET)

DEF1NAME	=$(SHL1TARGET)
DEF1DEPN        =$(MISC)$/$(SHL1TARGET).flt
DEFLIB1NAME     =untools
DEF1DES         =unotools

# --- Targets ----------------------------------

.INCLUDE : target.mk

# --- Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo CLEAR_THE_FILE		> $@
    @echo _TI					>> $@
    @echo _real					>> $@
    @echo NodeValueAccessor			>> $@
    @echo SubNodeAccess				>> $@
    @echo UpdateFromConfig			>> $@
    @echo UpdateToConfig				>> $@
        @echo _Impl >> $@

