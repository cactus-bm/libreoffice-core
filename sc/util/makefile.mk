#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.37 $
#
#   last change: $Author: ihi $ $Date: 2006-11-14 16:01:11 $
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

PRJ=..

PRJNAME=sc
TARGET=scalc3
GEN_HID=TRUE
GEN_HID_OTHER=TRUE
USE_DEFFILE=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk

.IF "$(OS)"=="IRIX"
LINKFLAGS+=-Wl,-LD_LAYOUT:lgot_buffer=30
.ENDIF

# --- Resourcen ----------------------------------------------------

RESLIB1LIST=\
    $(SRS)$/miscdlgs.srs	\
    $(SRS)$/docshell.srs	\
    $(SRS)$/ui.srs		\
    $(SRS)$/dbgui.srs	\
    $(SRS)$/drawfunc.srs \
    $(SRS)$/core.srs 	\
    $(SRS)$/styleui.srs	\
    $(SRS)$/formdlgs.srs \
    $(SRS)$/pagedlg.srs	\
    $(SRS)$/navipi.srs	\
    $(SOLARCOMMONRESDIR)$/sfx.srs

RESLIB1NAME=sc
RESLIB1IMAGES=\
    $(PRJ)$/res					\
    $(PRJ)$/res/imglst/apptbx	\
    $(PRJ)$/res/imglst/dbgui	\
    $(PRJ)$/res/imglst/navipi

RESLIB1SRSFILES=\
    $(RESLIB1LIST)

# --- StarClac DLL

SHL1TARGET= sc$(UPD)$(DLLPOSTFIX)
SHL1USE_EXPORTS=ordinal
SHL1IMPLIB= sci

.IF "$(OS)"!="MACOSX"
# static libraries
SHL1STDLIBS+= $(SCHLIB)
.ENDIF

# dynamic libraries
SHL1STDLIBS+=       \
    $(BASICLIB)	\
    $(SFXLIB)		\
    $(SVTOOLLIB)	\
    $(SVLLIB)		\
    $(SVXLIB)		\
    $(GOODIESLIB)	\
    $(BASEGFXLIB) \
    $(VCLLIB)		\
    $(CPPULIB)		\
    $(CPPUHELPERLIB)	\
    $(COMPHELPERLIB)	\
    $(UCBHELPERLIB)	\
    $(TKLIB)		\
    $(VOSLIB)		\
    $(SALLIB)		\
    $(TOOLSLIB)	\
    $(I18NISOLANGLIB) \
    $(UNOTOOLSLIB) \
    $(SOTLIB)		\
    $(XMLOFFLIB)	\
    $(DBTOOLSLIB)	\
    $(AVMEDIALIB)

.IF "$(OS)"=="MACOSX"
# static libraries at end for OS X
SHL1STDLIBS+= $(SCHLIB)
.ENDIF

SHL1LIBS= $(LIB1TARGET)
LIB1TARGET=$(SLB)$/scall.lib
LIB1FILES = $(LIB3TARGET) $(LIB4TARGET)

.IF "$(GUI)"!="UNX"
SHL1OBJS=   $(SLO)$/scdll.obj
.ENDIF

SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)
DEFLIB1NAME= scall

.IF "$(GUI)" == "WNT"
SHL1RES=    $(RCTARGET)
.ENDIF

# --- Linken der Applikation ---------------------------------------

LIB3TARGET=$(SLB)$/scalc3.lib
LIB3FILES=	\
    $(SLB)$/app.lib \
    $(SLB)$/docshell.lib \
    $(SLB)$/view.lib \
    $(SLB)$/undo.lib \
    $(SLB)$/attrdlg.lib \
    $(SLB)$/namedlg.lib \
    $(SLB)$/miscdlgs.lib \
    $(SLB)$/formdlgs.lib \
    $(SLB)$/cctrl.lib \
    $(SLB)$/dbgui.lib \
    $(SLB)$/pagedlg.lib \
    $(SLB)$/drawfunc.lib \
    $(SLB)$/navipi.lib

LIB3FILES+= \
            $(SLB)$/unoobj.lib


LIB4TARGET=$(SLB)$/scalc3c.lib
LIB4FILES=	\
    $(SLB)$/data.lib \
    $(SLB)$/tool.lib \
    $(SLB)$/dif.lib \
    $(SLB)$/excel.lib \
    $(SLB)$/xcl97.lib \
    $(SLB)$/html.lib \
    $(SLB)$/lotus.lib \
    $(SLB)$/qpro.lib\
    $(SLB)$/rtf.lib \
    $(SLB)$/xml.lib \
    $(SLB)$/accessibility.lib \
    $(SLB)$/ftools.lib \
    $(SLB)$/scflt.lib

SHL2TARGET= scd$(UPD)$(DLLPOSTFIX)
SHL2IMPLIB= scdimp
SHL2VERSIONMAP= scd.map
SHL2DEF=$(MISC)$/$(SHL2TARGET).def
DEF2NAME=		$(SHL2TARGET)

SHL2STDLIBS= \
            $(SFX2LIB) \
            $(SVTOOLLIB) \
            $(SVLLIB) \
            $(VCLLIB) \
            $(TOOLSLIB) \
            $(UCBHELPERLIB)	\
            $(CPPUHELPERLIB) \
            $(CPPULIB) \
            $(SOTLIB) \
            $(SALLIB)

SHL2OBJS=   $(SLO)$/scdetect.obj \
        $(SLO)$/detreg.obj
SHL2DEPN+=	makefile.mk


# add for scui
SHL8TARGET= scui$(UPD)$(DLLPOSTFIX)
SHL8IMPLIB= scuiimp
SHL8VERSIONMAP= scui.map
SHL8DEF=$(MISC)$/$(SHL8TARGET).def
SHL8DEPN=$(SHL1TARGETN)
DEF8NAME=$(SHL8TARGET)

SHL8STDLIBS= \
            $(ISCLIB) \
            $(SVXLIB) \
            $(SFX2LIB) \
            $(SVTOOLLIB) \
            $(VCLLIB) \
            $(SVLLIB) \
            $(SOTLIB) \
            $(UNOTOOLSLIB) \
            $(TOOLSLIB) \
            $(I18NISOLANGLIB) \
            $(COMPHELPERLIB) \
            $(CPPULIB) \
            $(SALLIB)

SHL8LIBS=   $(SLB)$/scui.lib
LIB8TARGET = $(SLB)$/scui.lib

LIB8FILES=$(SLB)$/styleui.lib	\
        $(SLB)$/optdlg.lib

LIB8OBJFILES = \
        $(SLO)$/scuiexp.obj     \
        $(SLO)$/scdlgfact.obj \
        $(SLO)$/tpsubt.obj		\
        $(SLO)$/tptable.obj	\
        $(SLO)$/tpstat.obj	\
        $(SLO)$/tabpages.obj	\
        $(SLO)$/tpsort.obj		\
        $(SLO)$/sortdlg.obj		\
        $(SLO)$/validate.obj	\
        $(SLO)$/textdlgs.obj		\
        $(SLO)$/subtdlg.obj		\
        $(SLO)$/tphf.obj		\
        $(SLO)$/scuitphfedit.obj	\
        $(SLO)$/hfedtdlg.obj	\
        $(SLO)$/attrdlg.obj	\
        $(SLO)$/scuiimoptdlg.obj	\
        $(SLO)$/strindlg.obj		\
        $(SLO)$/shtabdlg.obj		\
        $(SLO)$/scendlg.obj		\
        $(SLO)$/pvfundlg.obj	\
        $(SLO)$/pfiltdlg.obj	\
        $(SLO)$/namepast.obj		\
        $(SLO)$/namecrea.obj		\
        $(SLO)$/mvtabdlg.obj		\
        $(SLO)$/mtrindlg.obj		\
        $(SLO)$/linkarea.obj		\
        $(SLO)$/lbseldlg.obj		\
        $(SLO)$/instbdlg.obj		\
        $(SLO)$/inscodlg.obj		\
        $(SLO)$/inscldlg.obj		\
        $(SLO)$/groupdlg.obj		\
        $(SLO)$/filldlg.obj			\
        $(SLO)$/delcodlg.obj		\
        $(SLO)$/delcldlg.obj		\
        $(SLO)$/dapitype.obj	\
        $(SLO)$/dapidata.obj	\
        $(SLO)$/crdlg.obj			\
        $(SLO)$/scuiasciiopt.obj	\
        $(SLO)$/scuiautofmt.obj	\
        $(SLO)$/dpgroupdlg.obj	\
        $(SLO)$/editfield.obj

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk

ALLTAR:	$(MISC)$/linkinc.ls



