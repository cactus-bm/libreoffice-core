#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: jl $ $Date: 2001-02-08 15:09:52 $
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

PRJ=..$/..$/..$/

PRJNAME=dtrans
TARGET=dndTest
TARGETTYPE=CUI
LIBTARGET=NO

#USE_DEFFILE=	TRUE
NO_BSYMBOLIC=	TRUE
ENABLE_EXCEPTIONS=TRUE
BOOTSTRAP_SERVICE=FALSE

# --- Settings ---

.INCLUDE : svpre.mk
.INCLUDE : settings.mk
.INCLUDE : sv.mk

# --- Files ---

# CFLAGS+=/GR -DUNICODE -D_UNICODE
CFLAGS+= -D_WIN32_DCOM
UNOUCRDEP=	$(SOLARBINDIR)$/applicat.rdb
UNOUCRRDB=	$(SOLARBINDIR)$/applicat.rdb

.IF "$(BOOTSTRAP_SERVICE)" == "TRUE"
UNOUCROUT=	$(OUT)$/inc$/comprehensive
INCPRE+=	$(OUT)$/inc$/comprehensive
CPPUMAKERFLAGS += -C
.ELSE
UNOUCROUT=	$(OUT)$/inc
INCPRE+=	$(OUT)$/inc -I$(SOLARINCDIR)$/external$/atl
.ENDIF

UNOTYPES= com.sun.star.lang.XInitialization \
          com.sun.star.lang.XComponent	\
          com.sun.star.datatransfer.dnd.XDropTarget \
          com.sun.star.datatransfer.dnd.XDropTargetListener \
          com.sun.star.datatransfer.dnd.XDropTargetFactory \
          com.sun.star.datatransfer.dnd.DNDConstants		\
          com.sun.star.datatransfer.dnd.XDragSource
          


.IF "$(depend)" != ""

.ENDIF # depend

APP1NOSAL=TRUE

APP1TARGET=	$(TARGET)
APP1OBJS=	$(OBJ)$/dndTest.obj	\
            $(OBJ)$/atlwindow.obj \
            $(OBJ)$/targetlistener.obj \
            $(OBJ)$/implhelper.obj		\
            $(OBJ)$/transferable.obj

LIBCIMT=msvcrtd.lib
            

APP1STDLIBS= \
    $(SALLIB) \
    $(CPPUHELPERLIB) \
    $(CPPULIB)		\
    user32.lib		\
    ole32.lib		\
    comsupp.lib		\
    oleaut32.lib	\
    gdi32.lib			\
    $(LB)$/dtutils.lib

APP1LIBS=	\
            $(SOLARLIBDIR)$/imtaolecb.lib\
            $(SOLARLIBDIR)$/user9x.lib\
            $(SOLARLIBDIR)$/tools32.lib\



.IF "$(GUI)"=="WNT"
APP1STDLIBS += $(LIBCIMT)
APP2STDLIBS += $(LIBCIMT)
.ENDIF

APP1DEF=	$(MISC)\$(APP1TARGET).def

# --- Targets ---

.INCLUDE : target.mk

