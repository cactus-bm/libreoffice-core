#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: pjunck $ $Date: 2004-11-03 08:03:42 $
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

PRJNAME=	ucbhelper
TARGET=		ucbhelper
NO_BSYMBOLIC=	TRUE
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

.INCLUDE :	..$/version.mk

# --- Files --------------------------------------------------------

LIB1TARGET=	$(SLB)$/$(TARGET).lib
LIB1FILES=	$(SLB)$/client.lib \
        $(SLB)$/provider.lib

SHL1TARGET=	$(TARGET)$(UCBHELPER_MAJOR)$(COMID)
SHL1STDLIBS = \
    $(CPPUHELPERLIB) \
    $(CPPULIB) \
    $(SALHELPERLIB) \
    $(SALLIB) \
    $(VOSLIB)

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= wsock32.lib
.ENDIF # WNT

.IF "$(OS)"=="SOLARIS"
SHL1STDLIBS+= -lnsl -lsocket
.ENDIF # SOLARIS

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET)
SHL1LIBS=	$(LIB1TARGET)
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)
DEF1DEPN=	$(MISC)$/$(SHL1TARGET).flt
DEFLIB1NAME=	$(TARGET)
DEF1DES=	Universal Content Broker - Helpers
#DEF1EXPORT1=	createComponentFactory
#DEF1EXPORT2=	writeComponentInfo

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

# --- Filter -----------------------------------------------------------

$(MISC)$/$(SHL1TARGET).flt : ucbhelper.flt
    @echo ------------------------------
    @echo Making: $@
    @+$(TYPE) ucbhelper.flt > $@

