#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: hr $ $Date: 2005-06-09 14:56:45 $
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

PRJNAME=extensions
TARGET=pl
TARGETTYPE=GUI

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------
LIB1TARGET = $(SLB)$/plall.lib
LIB1FILES  = \
    $(SLB)$/plbase.lib	\
    $(SHL1LINKLIB)

.IF "$(GUI)" == "UNX"
SHL1LINKLIB = $(SLB)$/plunx.lib
.IF "$(OS)" == "SOLARIS"
SHL1OWNLIBS = -lsocket
.ENDIF # SOLARIS
.ENDIF # UNX

.IF "$(GUI)" == "WNT"
SHL1LINKLIB = $(SLB)$/plwin.lib
SHL1OWNLIBS = \
    version.lib	\
    ole32.lib	\
    advapi32.lib
.ENDIF # WNT

SHL1TARGET= $(TARGET)$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB= i$(TARGET)

SHL1VERSIONMAP=exports.map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

SHL1LIBS=$(LIB1TARGET)

.IF "$(OS)"=="MACOSX"
SHL1STDLIBS= \
    $(LIBSTLPORT)		\
    $(TKLIB)
.ELSE
SHL1STDLIBS= \
    $(TKLIB)
.ENDIF

SHL1STDLIBS+= \
    $(VCLLIB)			\
    $(SVLLIB)			\
    $(TOOLSLIB)			\
    $(VOSLIB)			\
    $(UCBHELPERLIB)		\
    $(CPPUHELPERLIB)	\
    $(CPPULIB)			\
    $(SALLIB)

.IF "$(OS)"=="MACOSX"
SHL1STDLIBS+= -lX11
.ENDIF

SHL1STDLIBS+=$(SHL1OWNLIBS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk



