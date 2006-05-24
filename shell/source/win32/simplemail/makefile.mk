#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: vg $ $Date: 2006-05-24 14:05:21 $
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
PRJNAME=shell
TARGET=smplmail
LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE
COMP1TYPELIST=$(TARGET)

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

SLOFILES=$(SLO)$/smplmailentry.obj\
         $(SLO)$/smplmailmsg.obj\
         $(SLO)$/smplmailclient.obj\
         $(SLO)$/smplmailsuppl.obj\
         $(SLO)$/simplemapi.obj
                        
SHL1TARGET=$(TARGET).uno

SHL1STDLIBS=$(CPPULIB)\
            $(CPPUHELPERLIB)\
            $(SALLIB)\
            advapi32.lib

SHL1IMPLIB=i$(SHL1TARGET)

SHL1OBJS=$(SLOFILES) 
SHL1DEF=$(MISC)$/$(SHL1TARGET).def

DEF1NAME=$(SHL1TARGET)
DEF1EXPORTFILE=	exports.dxp

# --- mapimailer --------------------------------------------------------

TARGETTYPE=CUI

OBJFILES=   $(OBJ)$/senddoc.obj\
            $(OBJ)$/simplemapi.obj

APP1TARGET=senddoc
APP1OBJS=$(OBJFILES)
APP1STDLIBS=kernel32.lib\
            $(SALLIB)
            
APP1DEF=$(MISC)$/$(APP1TARGET).def

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

