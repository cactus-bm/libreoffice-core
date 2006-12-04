#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.26 $
#
#   last change: $Author: rt $ $Date: 2006-12-04 16:38:14 $
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

PRJNAME=vcl
TARGET=salapp
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
.INCLUDE :  $(PRJ)$/util$/makefile2.pmk

# --- Files --------------------------------------------------------

.IF "$(GUIBASE)"!="unx"

dummy:
    @echo "Nothing to build for GUIBASE $(GUIBASE)"

.ELSE		# "$(GUIBASE)"!="unx"

SLOFILES=\
            $(SLO)$/i18n_cb.obj			\
            $(SLO)$/i18n_ic.obj			\
            $(SLO)$/i18n_im.obj			\
            $(SLO)$/i18n_xkb.obj		\
            $(SLO)$/i18n_wrp.obj		\
            $(SLO)$/i18n_status.obj		\
            $(SLO)$/i18n_keysym.obj		\
            $(SLO)$/saldata.obj			\
            $(SLO)$/saltimer.obj		\
            $(SLO)$/saldisp.obj			\
            $(SLO)$/salinst.obj			\
            $(SLO)$/salsys.obj			\
            $(SLO)$/soicon.obj			\
            $(SLO)$/sm.obj				\
            $(SLO)$/keysymnames.obj		\
            $(SLO)$/wmadaptor.obj		\
            $(SLO)$/salsound2.obj		\
            $(SLO)$/osssound.obj		\
            $(SLO)$/devaudiosound.obj		\
            $(SLO)$/audioconvert.obj		\
            $(SLO)$/rptpsound.obj		\
            $(SLO)$/nassound.obj		

EXCEPTIONSFILES=\
            $(SLO)$/wmadaptor.obj		\
            $(SLO)$/saldata.obj			\
            $(SLO)$/saldisp.obj			\
            $(SLO)$/i18n_status.obj		\
            $(SLO)$/i18n_cb.obj				\
            $(SLO)$/i18n_ic.obj				\
            $(SLO)$/salsys.obj


.IF "$(ENABLE_NAS)" != ""
CDEFS+=-DUSE_NAS
.ENDIF

.IF "$(ENABLE_PASF)" != ""
CDEFS+=-DUSE_PASF
SLOFILES+=$(SLO)$/pasfsound.obj
.IF "$(SYSTEM_SNDFILE)" == "YES"
CDEFS+=-DSYSTEM_SNDFILE
.ENDIF
.IF "$(SYSTEM_PORTAUDIO)" == "YES"
CDEFS+=-DSYSTEM_PORTAUDIO
.ENDIF
.ENDIF

.IF "$(WITH_LIBSN)"=="YES"
CDEFS+=-DHAVE_LIBSN
CFLAGS+=$(LIBSN_CFLAGS)
.ENDIF

.IF "$(USE_XINERAMA)" != "NO"
CDEFS+=-DUSE_XINERAMA
.IF "$(USE_XINERAMA_VERSION)" == "Xorg"
CDEFS+=-DUSE_XINERAMA_XORG
.ELIF "$(USE_XINERAMA_VERSION)" == "Xsun"
CDEFS+=-DUSE_XINERAMA_XSUN
.ELSE
# provide sensible default
.IF "$(OS)" != "SOLARIS"
CDEFS+=-DUSE_XINERAMA_XORG
.ELSE
CDEFS+=-DUSE_XINERAMA_XSUN
.ENDIF
.ENDIF
.ENDIF

.IF "$(OS)$(CPU)" == "SOLARISS"
.IF "$(COM)"!="GCC"
SLOFILES+=$(SLO)$/getfpsols.obj
.ENDIF			# "$(COM)"!="GCC"
.ELIF "$(OS)$(CPU)" == "SOLARISI"
SLOFILES+=$(SLO)$/getfpsoli.obj
.ENDIF

.ENDIF		# "$(GUIBASE)"!="unx"

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

.INCLUDE :  $(PRJ)$/util$/target.pmk

# local rule to create the additional object file for SOLARIS
$(SLO)$/%.obj: %.s
    $(CXX) -c -o $(@:s/.obj/.o/) $< && touch $@

