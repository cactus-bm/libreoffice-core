#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: obo $ $Date: 2007-01-25 12:37:50 $
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


PRJ=..$/..$/..$/..

PRJNAME=shell
TARGET=shlxthdl

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

INCPRE+=$(MFC_INCLUDE)

# --- Files --------------------------------------------------------


RCFILES=$(RES)$/$(TARGET).rc

ULFFILES=$(TARGET).ulf

.IF "$(WITH_LANG)"!=""
ULFDIR:=$(COMMONMISC)$/$(TARGET)
.ELSE			# "$(WITH_LANG)"!=""
ULFDIR:=.
.ENDIF			# "$(WITH_LANG)"!=""
# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk


# Generate the native Windows resource file
# using lngconvex.exe 

$(RCFILES) : $(ULFDIR)$/shlxthdl.ulf makefile.mk rcfooter.txt rcheader.txt rctmpl.txt
    $(WRAPCMD) $(BIN)$/lngconvex.exe -ulf $(ULFDIR)$/shlxthdl.ulf -rc $(RES)$/shlxthdl.rc -rct rctmpl.txt -rch rcheader.txt -rcf rcfooter.txt
    
