#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: obo $ $Date: 2008-01-04 16:22:24 $
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

PRJNAME=desktop
TARGET=guiloader
LIBTARGET=NO
TARGETTYPE=GUI

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

APP1TARGET=guiloader
APP1NOSAL=TRUE
APP1ICON=$(SOLARRESDIR)$/icons/ooo-main-app.ico
.IF "$(COM)"=="GCC"
APP1OBJS=$(OBJ)$/genericloader.obj
.ELSE
APP1OBJS=$(OBJ)$/genericloader.obj $(OBJ)$/extendloaderenvironment.obj
.ENDIF
STDLIB1=$(SHLWAPILIB)

APP2TARGET=so$/guiloader
APP2NOSAL=TRUE
APP2ICON=$(SOLARRESDIR)$/icons/so8-main-app.ico
.IF "$(COM)"=="GCC"
APP2OBJS=$(OBJ)$/genericloader.obj
.ELSE
APP2OBJS=$(OBJ)$/genericloader.obj $(OBJ)$/extendloaderenvironment.obj
.ENDIF
STDLIB2=$(SHLWAPILIB)

# Until every DLL is linked against $(DELAYLOADOBJ) just as on wntmsci10:
.IF "$(OS)$(COM)$(CPU)$(COMEX)" == "WNTMSCI11"
APP1OBJS+=$(L)$/delayload.obj
APP2OBJS+=$(L)$/delayload.obj
.ENDIF

# --- Targets ------------------------------------------------------


.INCLUDE :  target.mk

