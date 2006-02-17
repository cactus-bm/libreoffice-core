#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.17 $
#
#   last change: $Author: hr $ $Date: 2006-02-17 16:03:55 $
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
TARGET=npsoplugin

ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :  	settings.mk

INCPRE=$(SOLARINCDIR)$/npsdk

.IF "$(GUI)"=="UNX"

# not sure about -DMOZ_X11 but otheriwse some struct member don't exist...
CFLAGS+=-DMOZ_X11

.IF "$(ENABLE_GTK)"==""

dummy:
    @echo GTK disabled - nothing to build

.ELSE           # "$(ENABLE_GTK)"==""
.IF "$(OS)"=="LINUX" || "$(OS)"=="FREEBSD"
INC+= -DNP_LINUX
.ENDIF
PKGCONFIG_MODULES=gtk+-2.0
.INCLUDE: pkg_config.mk
.ENDIF          # "$(ENABLE_GTK)"==""
.ENDIF          # "$(GUI)"=="UNX"
.IF "$(GUI)"=="WNT" 
INC+= -DENGLISH
.ENDIF


# --- Files -------------------------------------

SLOFILES=       $(SLO)$/npshell.obj \
            $(SLO)$/so_env.obj

SHL1OBJS=	$(SLO)$/npshell.obj \
            $(SLO)$/so_env.obj

.IF "$(GUI)"=="UNX"
STDLIBS+=$(SOLARLIBDIR)$/npunix.o
SHL1STDLIBS+=$(SOLARLIBDIR)$/npunix.o
#.IF "$(OS)"!="FREEBSD"
#SHL1STDLIBS+= -ldl -lnsl
#.ENDIF #FREEBSD
.ENDIF #UNX
.IF "$(GUI)"=="WNT"
STDLIBS+=$(SOLARLIBDIR)$/npwin.obj
SHL1STDLIBS+= shell32.lib $(SOLARLIBDIR)$/npwin.obj

.ENDIF

OBJFILES=\
        $(OBJ)$/so_closelistener.obj\
        $(OBJ)$/so_instance.obj\
        $(OBJ)$/so_env.obj\
        $(OBJ)$/so_main.obj

# --- Plugin executable -------------------------
.IF "$(OS)"=="SOLARIS"
LINKFLAGSAPP!:=$(LINKFLAGSAPP:s/-z defs/-z nodefs/)
.ENDIF          # "$(OS)"=="SOLARIS"
APP1TARGET=nsplugin$(EXEPOSTFIX)
APP1OBJS=$(OBJFILES)

APP1STDLIBS=\
            $(CPPULIB)			\
            $(CPPUHELPERLIB)	\
            $(UNOTOOLSLIB)		\
            $(COMPHELPERLIB)	\
            $(VOSLIB)			\
            $(SALLIB)			\
            $(VCLLIB)			\
            $(SVLLIB)			\
            $(TOOLSLIB)

.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="LINUX" || "$(OS)"=="FREEBSD" || "$(OS)"=="MACOSX"
.ELSE
APP1STDLIBS+= -ldl -lnsl -lnls -lsocket
.ENDIF
.ENDIF

.IF "$(GUI)"=="WNT"
DEPOBJFILES=$(OBJ)$/nsp_windows.obj
APP1OBJS+=$(OBJ)$/nsp_windows.obj
APP1STDLIBS+= ws2_32.lib shell32.lib ole32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib oleaut32.lib uuid.lib
.ENDIF

# --- Library -----------------------------------

SHL1TARGET=$(TARGET)_so
.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= ws2_32.lib shell32.lib ole32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib
.ENDIF

SHL1DEPN=
SHL1IMPLIB=	i$(SHL1TARGET)
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

# ----- get some additional keys in versioninfo ------------
SHL1ADD_VERINFO=nsplugin.rc
SHL1DEPN+=$(SHL1ADD_VERINFO)

DEF1NAME=$(SHL1TARGET)
.IF "$(GUI)"=="UNX"
DEF1EXPORTFILE=exports.dxp
.ENDIF
.IF "$(GUI)"=="WNT"
DEF1EXPORTFILE=exports_wnt.dxp
.ENDIF

SHL2OBJS=$(SHL1OBJS)
SHL2STDLIBS=$(SHL1STDLIBS)
SHL2TARGET=$(TARGET)
.IF "$(GUI)"=="WNT"
SHL2STDLIBS+= ws2_32.lib shell32.lib ole32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib
.ENDIF

SHL2DEPN=
SHL2IMPLIB=	i$(SHL2TARGET)
SHL2DEF=	$(MISC)$/$(SHL2TARGET).def

# ----- get some additional keys in versioninfo ------------
SHL2ADD_VERINFO=nsplugin_oo.rc
SHL2DEPN+=$(SHL2ADD_VERINFO)

DEF2NAME=$(SHL2TARGET)
.IF "$(GUI)"=="UNX"
DEF2EXPORTFILE=exports.dxp
.ENDIF
.IF "$(GUI)"=="WNT"
DEF2EXPORTFILE=exports_wnt.dxp
.ENDIF

# --- Targets ----------------------------------

.INCLUDE : target.mk

