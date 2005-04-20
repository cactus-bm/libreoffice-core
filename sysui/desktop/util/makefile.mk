#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
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

PRJNAME=sysui
TARGET=launcher

TARGETTYPE=CUI
LIBTARGET=NO
NO_DEFAULT_STL=TRUE
LIBSALCPPRT=$(0)

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

.IF "$(ENABLE_GNOMEVFS)"!=""
COMPILER_WARN_ALL=TRUE
PKGCONFIG_MODULES=gnome-vfs-2.0
.INCLUDE: pkg_config.mk

.IF "$(OS)" == "SOLARIS"
LINKFLAGS+=-z nodefs
.ENDIF # "$(OS)" == "SOLARIS"

# Disable lazy binding of symbols
.IF "$(OS)" == "LINUX"
LINKFLAGS+=-Wl,-z,now
.ENDIF
.IF "$(OS)" == "SOLARIS"
LINKFLAGS+=-z now
.ENDIF  

.IF "$(OS)" == "LINUX"
# hack for faked SO environment
CFLAGS+=-gdwarf-2
PKGCONFIG_LIBS!:=-Wl,--export-dynamic $(PKGCONFIG_LIBS)
.ENDIF

.ENDIF          # "$(ENABLE_GNOMEVFS)"!=""

# --- Files --------------------------------------------------------

.IF "$(ENABLE_GNOMEVFS)"!=""
OBJFILES += $(OBJ)$/gnome-set-default-application.obj

APP1TARGET=gnome-set-default-application
APP1OBJS=$(OBJ)$/gnome-set-default-application.obj
APP1LIBS=
APP1STDLIBS+=$(PKGCONFIG_LIBS)
.ENDIF          # "$(ENABLE_GNOMEVFS)"!=""

SCRIPTS= \
    $(BIN)$/viewdoc.sh

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

ALLTAR: $(SCRIPTS)

$(SCRIPTS) : $$(@:f)
    +@rm -f $@ 2>/dev/null
    +@cat $(@:f) | tr -d "\015" > $@
    +@chmod 555 $@
