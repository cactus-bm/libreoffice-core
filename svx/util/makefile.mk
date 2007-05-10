#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.49 $
#
#   last change: $Author: kz $ $Date: 2007-05-10 15:04:09 $
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

PRJNAME=svx
TARGET=svx
GEN_HID=TRUE
GEN_HID_OTHER=TRUE

USE_DEFFILE=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

.IF "$(OS)"=="IRIX"
LINKFLAGS+=-Wl,-LD_LAYOUT:lgot_buffer=30
.ENDIF

RSCLOCINC+=-I$(PRJ)$/source$/svdraw

# --- Svx - DLL ----------

LIB1TARGET= $(SLB)$/$(TARGET).lib
LIB1FILES=\
    $(SLB)$/animation.lib \
    $(SLB)$/overlay.lib \
    $(SLB)$/svdraw.lib \
    $(SLB)$/form.lib \
    $(SLB)$/fmcomp.lib \
    $(SLB)$/engine3d.lib \
    $(SLB)$/msfilter.lib \
    $(SLB)$/xout.lib \
    $(SLB)$/xml.lib



.IF "$(GUI)" == "OS2" || "(GUIBASE)" == "WIN"
LIB1FILES+=$(SLB)$/ibrwimp.lib
.ENDIF # (OS2 || WIN)

LIB2TARGET= $(SLB)$/$(TARGET)_2.lib
LIB2FILES=\
    $(SLB)$/init.lib \
    $(SLB)$/items.lib     \
    $(SLB)$/svxlink.lib   \
    $(SLB)$/svxrtf.lib    \
    $(SLB)$/editeng.lib   \
    $(SLB)$/outliner.lib \
    $(SLB)$/dialogs.lib	\
    $(SLB)$/mnuctrls.lib  \
    $(SLB)$/options.lib   \
    $(SLB)$/stbctrls.lib  \
    $(SLB)$/tbxctrls.lib  \
    $(SLB)$/unoedit.lib
    
LIB3TARGET= $(SLB)$/$(TARGET)_3.lib
LIB3FILES=\
    $(SLB)$/unodraw.lib	\
    $(SLB)$/unogallery.lib\
    $(SLB)$/gal.lib		\
    $(SLB)$/accessibility.lib	\
    $(SLB)$/customshapes.lib\
    $(SLB)$/toolbars.lib \
    $(SLB)$/properties.lib \
    $(SLB)$/contact.lib \
    $(SLB)$/mixer.lib \
    $(SLB)$/event.lib

.IF "$(GUI)" == "OS2" || "(GUIBASE)" == "WIN"
LIB1FILES+=$(SLB)$/ibrwimp.lib
.ENDIF # (OS2 || WIN)

HELPIDFILES=    ..$/inc$/helpid.hrc

# svx
SHL1TARGET= svx$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB= i$(TARGET)
SHL1USE_EXPORTS=ordinal

SHL1LIBS= $(LIB1TARGET) $(LIB2TARGET) $(LIB3TARGET)
SHL1STDLIBS= \
            $(AVMEDIALIB) \
            $(SFX2LIB) \
            $(XMLOFFLIB) \
            $(GOODIESLIB) \
            $(BASEGFXLIB) \
            $(BASICLIB) \
            $(LNGLIB) \
            $(SVTOOLLIB) \
            $(TKLIB) \
            $(VCLLIB) \
            $(SVLLIB) \
            $(SOTLIB) \
            $(UNOTOOLSLIB) \
            $(TOOLSLIB) \
            $(I18NISOLANGLIB) \
            $(XMLSCRIPTLIB) \
            $(COMPHELPERLIB) \
            $(UCBHELPERLIB)	\
            $(CPPUHELPERLIB)	\
            $(CPPULIB) \
            $(VOSLIB) \
            $(SALLIB) \
            $(ICUUCLIB)

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+=$(SHELLLIB)
.ENDIF # WNT

SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
DEF1NAME=	$(SHL1TARGET)
DEFLIB1NAME=$(TARGET) $(TARGET)_2 $(TARGET)_3


# cui
SHL2TARGET= cui$(UPD)$(DLLPOSTFIX)
SHL2VERSIONMAP= cui.map
SHL2IMPLIB=icui
SHL2DEPN=$(SHL1TARGETN)

SHL2DEF=	$(MISC)$/$(SHL2TARGET).def
DEF2NAME=	$(SHL2TARGET)

SHL2LIBS=	$(SLB)$/cui.lib
SHL2OBJS= \
        $(SLO)$/cuiexp.obj     \
        $(SLO)$/dlgfact.obj

SHL2STDLIBS= \
            $(SVXLIB) \
            $(AVMEDIALIB) \
            $(SFX2LIB) \
            $(GOODIESLIB) \
            $(BASEGFXLIB) \
            $(SVTOOLLIB) \
            $(TKLIB) \
            $(VCLLIB) \
            $(SVLLIB) \
            $(SOTLIB) \
            $(UNOTOOLSLIB) \
            $(TOOLSLIB) \
            $(I18NISOLANGLIB) \
            $(COMPHELPERLIB) \
            $(UCBHELPERLIB)	\
            $(CPPUHELPERLIB)	\
            $(CPPULIB) \
            $(VOSLIB) \
            $(SALLIB) \
            $(JVMFWKLIB) \
            $(ICUUCLIB)

.IF "$(GUI)"=="WNT"	
SHL2STDLIBS+= \
             $(SHLWAPILIB) \
             $(ADVAPI32LIB)
.ENDIF # WNT

# ------------------------------------------------------------------------------

# Resource files
SRSFILELIST=\
                $(SRS)$/svdstr.srs      \
                $(SRS)$/editeng.srs     \
                $(SRS)$/outliner.srs \
                $(SRS)$/dialogs.srs     \
                $(SRS)$/drawdlgs.srs \
                $(SRS)$/mnuctrls.srs \
                $(SRS)$/stbctrls.srs \
                $(SRS)$/tbxctrls.srs \
                $(SRS)$/options.srs     \
                $(SRS)$/svxitems.srs \
                $(SRS)$/form.srs \
                $(SRS)$/fmcomp.srs \
                $(SRS)$/engine3d.srs \
                $(SRS)$/unodraw.srs \
                $(SRS)$/svxlink.srs \
                $(SRS)$/accessibility.srs \
                $(SRS)$/toolbars.srs \
                $(SOLARCOMMONRESDIR)$/sfx.srs

RESLIB1NAME=svx
RESLIB1IMAGES=$(PRJ)$/res $(PRJ)$/source/svdraw
RESLIB1SRSFILES= $(SRSFILELIST)

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

