#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.91 $
#
#   last change: $Author: vg $ $Date: 2007-05-25 14:36:51 $
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

PRJNAME=vcl
TARGET=vcl
TARGETTYPE=GUI
VERSION=$(UPD)
USE_DEFFILE=TRUE

.IF "$(OS)" == "SOLARIS"
LINKFLAGSRUNPATH=-R/usr/sfw/lib -R\''$$ORIGIN'\'
.ENDIF

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  makefile.pmk
.INCLUDE :  makefile2.pmk


# --- Allgemein ----------------------------------------------------------

HXXDEPNLST= $(INC)$/vcl$/accel.hxx       \
            $(INC)$/vcl$/animate.hxx     \
            $(INC)$/vcl$/apptypes.hxx    \
            $(INC)$/vcl$/bitmap.hxx      \
            $(INC)$/vcl$/bitmapex.hxx    \
            $(INC)$/vcl$/bmpacc.hxx      \
            $(INC)$/vcl$/btndlg.hxx      \
            $(INC)$/vcl$/button.hxx      \
            $(INC)$/vcl$/ctrl.hxx        \
            $(INC)$/vcl$/cursor.hxx      \
            $(INC)$/vcl$/cmdevt.hxx      \
            $(INC)$/vcl$/decoview.hxx    \
            $(INC)$/vcl$/dialog.hxx      \
            $(INC)$/vcl$/dllapi.h        \
            $(INC)$/vcl$/dockwin.hxx     \
            $(INC)$/vcl$/edit.hxx        \
            $(INC)$/vcl$/event.hxx       \
            $(INC)$/vcl$/field.hxx       \
            $(INC)$/vcl$/fixed.hxx       \
            $(INC)$/vcl$/floatwin.hxx    \
            $(INC)$/vcl$/font.hxx        \
            $(INC)$/vcl$/fontcvt.hxx     \
            $(INC)$/vcl$/floatwin.hxx    \
            $(INC)$/vcl$/graph.hxx       \
            $(INC)$/vcl$/group.hxx       \
            $(INC)$/vcl$/help.hxx        \
            $(INC)$/vcl$/jobset.hxx      \
            $(INC)$/vcl$/keycodes.hxx    \
            $(INC)$/vcl$/keycod.hxx      \
            $(INC)$/vcl$/image.hxx       \
            $(INC)$/vcl$/lstbox.h             \
            $(INC)$/vcl$/lstbox.hxx      \
            $(INC)$/vcl$/mapmod.hxx      \
            $(INC)$/vcl$/metaact.hxx     \
            $(INC)$/vcl$/menu.hxx        \
            $(INC)$/vcl$/menubtn.hxx     \
            $(INC)$/vcl$/metric.hxx      \
            $(INC)$/vcl$/morebtn.hxx     \
            $(INC)$/vcl$/msgbox.hxx      \
            $(INC)$/vcl$/octree.hxx      \
            $(INC)$/vcl$/outdev.hxx      \
            $(INC)$/vcl$/outdev3d.hxx    \
            $(INC)$/vcl$/pointr.hxx      \
            $(INC)$/vcl$/ptrstyle.hxx    \
            $(INC)$/vcl$/prntypes.hxx    \
            $(INC)$/vcl$/print.hxx       \
            $(INC)$/vcl$/prndlg.hxx      \
            $(INC)$/vcl$/region.hxx      \
            $(INC)$/vcl$/salbtype.hxx    \
            $(INC)$/vcl$/scrbar.hxx      \
            $(INC)$/vcl$/slider.hxx      \
            $(INC)$/vcl$/seleng.hxx      \
            $(INC)$/vcl$/settings.hxx    \
            $(INC)$/vcl$/sound.hxx       \
            $(INC)$/vcl$/sndstyle.hxx    \
            $(INC)$/vcl$/split.hxx       \
            $(INC)$/vcl$/splitwin.hxx    \
            $(INC)$/vcl$/spin.hxx        \
            $(INC)$/vcl$/spinfld.hxx     \
            $(INC)$/vcl$/status.hxx      \
            $(INC)$/vcl$/stdtext.hxx     \
            $(INC)$/vcl$/sv.h            \
            $(INC)$/vcl$/svapp.hxx       \
            $(INC)$/vcl$/syschild.hxx    \
            $(INC)$/sysdata.hxx     \
            $(INC)$/vcl$/syswin.hxx      \
            $(INC)$/vcl$/tabctrl.hxx     \
            $(INC)$/vcl$/tabdlg.hxx      \
            $(INC)$/vcl$/tabpage.hxx     \
            $(INC)$/vcl$/toolbox.hxx     \
            $(INC)$/vcl$/timer.hxx       \
            $(INC)$/vcl$/virdev.hxx      \
            $(INC)$/vcl$/wall.hxx        \
            $(INC)$/vcl$/waitobj.hxx     \
            $(INC)$/vcl$/wintypes.hxx    \
            $(INC)$/vcl$/window.hxx      \
            $(INC)$/vcl$/wrkwin.hxx

.IF "$(linkinc)" != ""
SHL11FILE=  $(MISC)$/app.slo
SHL12FILE=  $(MISC)$/gdi.slo
SHL13FILE=  $(MISC)$/win.slo
SHL14FILE=  $(MISC)$/ctrl.slo
#SHL15FILE=  $(MISC)$/ex.slo
SHL16FILE=  $(MISC)$/salapp.slo
SHL17FILE=  $(MISC)$/salwin.slo
SHL18FILE=  $(MISC)$/salgdi.slo
.ENDIF

LIB1TARGET= $(SLB)$/$(TARGET).lib
LIB1FILES=  $(SLB)$/app.lib     \
            $(SLB)$/gdi.lib     \
            $(SLB)$/win.lib     \
            $(SLB)$/ctrl.lib    \
            $(SLB)$/helper.lib	\
            $(SLB)$/components.lib


.IF "$(GUI)" == "UNX"
LIB1FILES+=$(SLB)$/salplug.lib
SHL1STDLIBS+=\
            -lpsp$(VERSION)$(DLLPOSTFIX)
.ELSE
LIB1FILES+= \
            $(SLB)$/salwin.lib  \
            $(SLB)$/salgdi.lib  \
            $(SLB)$/salapp.lib
.ENDIF

SHL1TARGET= vcl$(VERSION)$(DLLPOSTFIX)
SHL1IMPLIB= ivcl
SHL1STDLIBS+=\
            $(SOTLIB)           \
            $(UNOTOOLSLIB)      \
            $(TOOLSLIB)         \
            $(I18NISOLANGLIB)   \
            $(COMPHELPERLIB)	\
            $(UCBHELPERLIB)     \
            $(CPPUHELPERLIB)    \
            $(CPPULIB)          \
            $(VOSLIB)           \
            $(SALLIB)			\
            $(BASEGFXLIB)		\
            $(ICUUCLIB)			\
            $(ICULELIB)			\
            $(JVMACCESSLIB)
SHL1USE_EXPORTS=ordinal

.IF "$(USE_BUILTIN_RASTERIZER)"!=""
    LIB1FILES +=    $(SLB)$/glyphs.lib
    SHL1STDLIBS+=   $(FREETYPELIB)
.ENDIF # USE_BUILTIN_RASTERIZER

SHL1LIBS=   $(LIB1TARGET)
.IF "$(GUI)"!="UNX"
.IF "$(COM)"!="GCC"
SHL1OBJS=   $(SLO)$/salshl.obj
.ENDIF
.ENDIF

.IF "$(GUI)" != "MAC"
.IF "$(GUI)" != "UNX"
SHL1RES=    $(RES)$/salsrc.res
.ENDIF
.ENDIF
SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
DEF1DEPN    =   $(HXXDEPNLST) \
                $(LIB1TARGET)
DEF1DES     =VCL
DEFLIB1NAME =vcl

# --- W32 ----------------------------------------------------------------

.IF "$(GUI)" == "WNT"

.IF "$(COM)" == "GCC"
SHL1STDLIBS += $(PSPLIB)
.ENDIF

SHL1STDLIBS += $(UWINAPILIB)      \
               $(GDI32LIB)        \
               $(MSIMG32LIB)        \
               $(WINSPOOLLIB)     \
               $(OLE32LIB)        \
               $(SHELL32LIB)      \
               $(ADVAPI32LIB)

.IF "$(COM)" == "GCC"
SHL1STDLIBS += $(IMM32LIB)
.ELSE
SHL1STDLIBS += $(PSPLIB)         \
               $(IMM32LIB)
.ENDIF

.IF "$(GUI)$(COM)$(CPU)" == "WNTMSCI"
LINKFLAGSSHL += /ENTRY:LibMain@12
.ENDIF
.ENDIF


# --- UNX ----------------------------------------------------------------

# UNX sal plugins
.IF "$(GUI)" == "UNX"

# basic pure X11 plugin
LIB2TARGET=$(SLB)$/ipure_x
LIB2FILES= \
            $(SLB)$/salwin.lib  \
            $(SLB)$/salgdi.lib  \
            $(SLB)$/salapp.lib
SHL2TARGET=vclplug_gen$(UPD)$(DLLPOSTFIX)
SHL2IMPLIB=ipure_x
SHL2LIBS=$(LIB2TARGET)
SHL2DEPN=$(SHL1IMPLIBN) $(SHL1TARGETN)

# libs for generic plugin
SHL2STDLIBS=\
            $(VCLLIB)\
            -lpsp$(VERSION)$(DLLPOSTFIX)\
            $(SOTLIB)           \
            $(UNOTOOLSLIB)      \
            $(TOOLSLIB)         \
            $(COMPHELPERLIB)	\
            $(UCBHELPERLIB)     \
            $(CPPUHELPERLIB)    \
            $(CPPULIB)          \
            $(VOSLIB)           \
            $(SALLIB)

# prepare linking of Xinerama
.IF "$(USE_XINERAMA)" != "NO"

.IF "$(OS)"=="MACOSX"
XINERAMALIBS=-lXinerama
.ELSE
.IF "$(OS)" != "SOLARIS" || "$(USE_XINERAMA_VERSION)" == "Xorg"
.IF "$(XINERAMA_LINK)" == "dynamic"
XINERAMALIBS= -lXinerama
.ELSE
XINERAMALIBS= -Wl,-Bstatic -lXinerama -Wl,-Bdynamic 
.ENDIF # XINERAMA_LINK == dynamic
.ENDIF # OS == SOLARIS
.ENDIF # OS == MACOSX

SHL2STDLIBS += $(XINERAMALIBS)
.ENDIF # USE_XINERAMA != NO

.IF "$(XRENDER_LINK)" == "YES"
SHL2STDLIBS+=`pkg-config --libs xrender`
.ENDIF


.IF "$(ENABLE_PASF)" != ""
.IF "$(OS)"=="MACOSX"
SHL2STDLIBS += -framework CoreAudio -framework AudioToolbox
.ENDIF
SHL2STDLIBS += -lsndfile -lportaudio
.ENDIF # ENABLE_PASF

.IF "$(ENABLE_NAS)" != ""
SHL2STDLIBS += -laudio
.IF "$(XAU_LIBS)" != ""
SHL2STDLIBS += $(XAU_LIBS)
.ENDIF
.IF "$(OS)"=="SOLARIS"
# needed by libaudio.a
SHL2STDLIBS += -ldl -lnsl -lsocket
.ENDIF # SOLARIS
.ENDIF

.IF "$(GUIBASE)"=="unx"

.IF "$(WITH_LIBSN)"=="YES"
SHL2STDLIBS+=$(LIBSN_LIBS)
.ENDIF

SHL2STDLIBS += -lXext -lSM -lICE -lX11
.IF "$(OS)"!="MACOSX" && "$(OS)"!="FREEBSD"
# needed by salprnpsp.cxx
SHL2STDLIBS+= -ldl
.ENDIF

.ENDIF          # "$(GUIBASE)"=="unx"

# dummy plugin
LIB3TARGET=$(SLB)$/idummy_plug_
LIB3FILES= \
            $(SLB)$/dapp.lib
SHL3TARGET=vclplug_dummy$(UPD)$(DLLPOSTFIX)
SHL3IMPLIB=idummy_plug_
SHL3LIBS=$(LIB3TARGET)
SHL3DEPN=$(SHL1IMPLIBN) $(SHL1TARGETN)

# libs for dummy plugin
SHL3STDLIBS=\
            $(VCLLIB)\
            -lpsp$(VERSION)$(DLLPOSTFIX)\
            $(SOTLIB)           \
            $(UNOTOOLSLIB)      \
            $(TOOLSLIB)         \
            $(COMPHELPERLIB)	\
            $(UCBHELPERLIB)     \
            $(CPPUHELPERLIB)    \
            $(CPPULIB)          \
            $(VOSLIB)           \
            $(SALLIB)

# gtk plugin
.IF "$(ENABLE_GTK)" != ""
PKGCONFIG_MODULES=gtk+-2.0 gthread-2.0
.INCLUDE: pkg_config.mk

LIB4TARGET=$(SLB)$/igtk_plug_
LIB4FILES=\
            $(SLB)$/gtkapp.lib\
            $(SLB)$/gtka11y.lib \
            $(SLB)$/gtkgdi.lib\
            $(SLB)$/gtkwin.lib

SHL4TARGET=vclplug_gtk$(UPD)$(DLLPOSTFIX)
SHL4IMPLIB=igtk_plug_
SHL4LIBS=$(LIB4TARGET)
SHL4DEPN=$(SHL1IMPLIBN) $(SHL1TARGETN) $(SHL2IMPLIBN) $(SHL2TARGETN)
# libs for gtk plugin
SHL4STDLIBS+=$(PKGCONFIG_LIBS:s/ -lpangoxft-1.0//)
# hack for faked SO environment
.IF "$(PKGCONFIG_ROOT)"!=""
SHL4SONAME+=-z nodefs
SHL4NOCHECK=TRUE
.ENDIF          # "$(PKGCONFIG_ROOT)"!=""


SHL4STDLIBS+=-l$(SHL2TARGET)
SHL4STDLIBS+=$(SHL3STDLIBS)
.ENDIF # "$(ENABLE_GTK)" != ""

# KDE plugin
.IF "$(ENABLE_KDE)" != ""
LIB5TARGET=$(SLB)$/ikde_plug_
LIB5FILES=$(SLB)$/kdeplug.lib
SHL5TARGET=vclplug_kde$(UPD)$(DLLPOSTFIX)
SHL5IMPLIB=ikde_plug_
SHL5LIBS=$(LIB5TARGET)
SHL5DEPN=$(SHL2TARGETN)
# libs for KDE plugin
SHL5STDLIBS=$(KDE_LIBS)
SHL5STDLIBS+=-l$(SHL2TARGET)
SHL5STDLIBS+=$(SHL3STDLIBS)
.ENDIF # "$(ENABLE_KDE)" != ""

# headless plugin
.IF "$(ENABLE_HEADLESS)" != ""
LIB6TARGET=$(SLB)$/iheadless_plug_
LIB6FILES= \
            $(SLB)$/headless.lib
SHL6TARGET=vclplug_svp$(UPD)$(DLLPOSTFIX)
SHL6IMPLIB=iheadless_plug_
SHL6LIBS=$(LIB6TARGET)
SHL6DEPN=$(SHL1IMPLIBN) $(SHL1TARGETN) $(SHL2IMPLIBN) $(SHL2TARGETN)

# libs for dummy plugin
SHL6STDLIBS=\
        $(VCLLIB)\
        -lpsp$(VERSION)$(DLLPOSTFIX)\
            $(SOTLIB)           \
            $(UNOTOOLSLIB)      \
            $(TOOLSLIB)         \
            $(COMPHELPERLIB)	\
            $(UCBHELPERLIB)     \
            $(CPPUHELPERLIB)    \
            $(CPPULIB)          \
            $(VOSLIB)           \
            $(SALLIB)

SHL6STDLIBS+=-l$(SHL2TARGET)
.IF "$(PKGCONFIG)" == ""
PKGCONFIG_MODULES=glib-2.0
.INCLUDE: pkg_config.mk
.ENDIF
PKG6CONFIG_LIBS:=$(shell $(PKGCONFIG) $(PKGCONFIG_PREFIX) --libs glib-2.0)
SHL6STDLIBS+=$(PKG6CONFIG_LIBS)

.IF "$(PKGCONFIG_ROOT)"!=""
SHL6SONAME+=-z nodefs
SHL6NOCHECK=TRUE
.ENDIF          # "$(PKGCONFIG_ROOT)"!=""

.ENDIF
.ENDIF # UNX

# --- Allgemein ----------------------------------------------------------

.INCLUDE :  target.mk

