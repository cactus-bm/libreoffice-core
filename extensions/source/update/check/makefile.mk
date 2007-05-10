#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: kz $ $Date: 2007-05-10 13:11:36 $
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

PRJNAME=extensions
TARGET=updchk
PACKAGE=org.openoffice.Office

LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE

ABSXCSROOT=$(SOLARXMLDIR)
XCSROOT=..
DTDDIR=$(ABSXCSROOT)
XSLDIR=$(ABSXCSROOT)$/processing
PROCESSOUT=$(MISC)$/$(TARGET)
PROCESSORDIR=$(SOLARBINDIR)

# no validation by inspector class
NO_INSPECTION=TRUE

# --- Settings ---

.INCLUDE : settings.mk

# no "lib" prefix
DLLPRE =

# --- Files ---


SLOFILES=\
    $(SLO)$/updatecheck.obj \
    $(SLO)$/updatecheckconfig.obj \
    $(SLO)$/updateprotocol.obj
        
SHL1NOCHECK=TRUE
SHL1TARGET=$(TARGET).uno   
SHL1OBJS=$(SLOFILES)
SHL1DEF=$(MISC)$/$(SHL1TARGET).def

SHL1IMPLIB=i$(SHL1TARGET)
SHL1STDLIBS=    \
        $(CPPUHELPERLIB) \
        $(CPPULIB) \
        $(SALLIB)
        
SHL1VERSIONMAP=..$/exports.map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

PACKAGEDIR=$(subst,.,$/ $(PACKAGE))
SPOOLDIR=$(MISC)$/registry$/spool

XCUFILES= \
    Addons.xcu \
    Jobs.xcu

MYXCUFILES= \
    $(SPOOLDIR)$/$(PACKAGEDIR)$/Addons$/Addons-onlineupdate.xcu \
    $(SPOOLDIR)$/$(PACKAGEDIR)$/Jobs$/Jobs-onlineupdate.xcu

LOCALIZEDFILES= \
    Addons.xcu \
    Jobs.xcu

.IF "$(test)" != ""
APP1TARGET=updateprotocoltest
APP1STDLIBS= $(SHL1STDLIBS)
APP1OBJS= \
    $(SLO)$/updateprotocol.obj \
    $(SLO)$/updateprotocoltest.obj


.ENDIF # "$(test)" != ""

# --- Targets ---

.INCLUDE : target.mk

ALLTAR : $(MYXCUFILES)

.IF "$(WITH_LANG)"!=""
XCU_SOURCEDIR:=$(PROCESSOUT)$/merge$/$(PACKAGEDIR)
.ELSE			# "$(WITH_LANG)"!=""
XCU_SOURCEDIR:=.
.ENDIF			# "$(WITH_LANG)"!=""

$(SPOOLDIR)$/$(PACKAGEDIR)$/Addons$/Addons-onlineupdate.xcu : $(XCU_SOURCEDIR)$/Addons.xcu
    @-$(MKDIRHIER) $(@:d)
    @$(COPY) $< $@

$(SPOOLDIR)$/$(PACKAGEDIR)$/Jobs$/Jobs-onlineupdate.xcu : $(XCU_SOURCEDIR)$/Jobs.xcu
    @-$(MKDIRHIER) $(@:d)
    @$(PERL) transform.pl < $< > $@
#	@$(COPY) $< $@

