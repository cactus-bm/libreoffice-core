#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.69 $
#
#   last change: $Author: obo $ $Date: 2007-03-09 09:17:08 $
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

PRJNAME=desktop
TARGET=soffice
TARGETTYPE=GUI
GEN_HID=TRUE
GEN_HID_OTHER=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk

VERINFONAME=verinfo

TARGETOBJS=	\
            $(OBJ)$/main.obj				\
            $(OBJ)$/app.obj					\
            $(OBJ)$/lockfile.obj			\
            $(OBJ)$/lockfile2.obj			\
            $(OBJ)$/intro.obj				\
            $(OBJ)$/officeipcthread.obj		\
            $(OBJ)$/appinit.obj				\
            $(OBJ)$/cmdlineargs.obj			\
            $(OBJ)$/oinstanceprovider.obj	\
            $(OBJ)$/opluginframefactory.obj	\
            $(OBJ)$/appsys.obj				\
            $(OBJ)$/desktopresid.obj		\
            $(OBJ)$/dispatchwatcher.obj		\
            $(OBJ)$/configinit.obj				\
            $(OBJ)$/checkinstall.obj		\
            $(OBJ)$/cmdlinehelp.obj         \
            $(OBJ)$/langselect.obj          \
            $(OBJ)$/userinstall.obj         \
            $(OBJ)$/desktopcontext.obj



# --- Resourcen ----------------------------------------------------

.IF "$(GUI)" == "WNT"
RCFILES=verinfo.rc
.ENDIF

# --- Linking of static libs ---------------------------------------

.IF "$(GUI)" == "WNT"

LIB1TARGET=$(SLB)$/$(TARGET).lib
LIB1FILES=$(TARGETOBJS)
LIB1FILES += $(OBJ)$/copyright_ascii_sun.obj

#LIB2TARGET=$(SLB)$/officeloader.lib
#LIB2FILES=$(OBJ)$/officeloader.obj


.ENDIF

# --- Linken der Applikation ---------------------------------------

.IF "$(OS)" == "MACOSX"
LINKFLAGSAPPGUI!:=	$(LINKFLAGSAPPGUI:s/-bind_at_load//)
.ENDIF # MACOSX

#.IF "$(OS)" == "LINUX" || "$(OS)" == "FREEBSD" || "$(OS)" == "NETBSD"
## #74158# linux needs sal/vos/tools at end of link list, solaris needs it first,
## winXX is handled like solaris for now
#APP1_STDPRE=
#APP1_STDPOST=$(CPPULIB) $(CPPUHELPERLIB) $(UNOLIB) $(TOOLSLIB) \
#	$(VOSLIB) $(SALLIB)
#.ELSE
#APP1_STDPRE=$(SALLIB) $(VOSLIB) $(TOOLSLIB) $(UNOLIB) $(CPPULIB) \
#	$(CPPUHELPERLIB)
#APP1_STDPOST=
#.ENDIF

RESLIB1NAME=		dkt
RESLIB1IMAGES=		$(PRJ)$/res
RESLIB1SRSFILES=	$(SRS)$/desktop.srs \
                    $(SRS)$/wizard.srs

APP1TARGET=so$/$(TARGET)
APP1NOSAL=TRUE
APP1STDLIBS=			\
    $(VCLLIB)			\
    $(SVLLIB)           \
    $(SVTOOLLIB)            \
    $(UNOTOOLSLIB)		\
    $(TOOLSLIB)			\
    $(I18NISOLANGLIB)   \
    $(COMPHELPERLIB)	\
    $(UCBHELPERLIB)		\
    $(VOSLIB)			\
    $(CPPUHELPERLIB)	\
    $(CPPULIB)			\
    $(TKLIB)            \
        $(SALLIB)


#	$(APP1_STDPRE)		\
#	$(SVLLIB)			\
#	$(VCLLIB)			\
#   $(APP1_STDPOST)		\
#	$(UNOTOOLSLIB)		\
#	$(UCBHELPERLIB)		\
#	$(COMPHELPERLIB)	\
#	$(SALHELPERLIB)		\

APP1OBJS=$(TARGETOBJS)
APP1OBJS += $(OBJ)$/copyright_ascii_sun.obj

.IF "$(GUI)" == "UNX"
.IF "$(OS)" == "LINUX" || "$(OS)" == "FREEBSD"
APP1STDLIBS+= -lXext -lSM -lICE
.ENDIF
.ENDIF

APP1DEPN= $(APP1RES) verinfo.rc

.IF "$(GUI)" == "WNT"
APP1RES=    $(RES)$/desktop.res
APP1ICON=$(SOLARRESDIR)$/icons/so8-main-app.ico
APP1VERINFO=verinfo.rc
APP1LINKRES=$(MISC)$/$(TARGET)1.res
APP1STACK=10000000

# create a manifest file with the same name as the
#office executable file soffice.exe.manifest
#$(BIN)$/$(TARGET).exe.manifest: template.manifest
#$(COPY) $< $@

.ENDIF # WNT

APP5TARGET=soffice
APP5NOSAL=TRUE
APP5STDLIBS=			\
    $(VCLLIB)			\
    $(SVLLIB)			\
    $(SVTOOLLIB)        \
    $(UNOTOOLSLIB)      \
    $(TOOLSLIB)			\
    $(I18NISOLANGLIB)   \
    $(COMPHELPERLIB)	\
    $(UCBHELPERLIB)		\
    $(VOSLIB)			\
    $(CPPUHELPERLIB)	\
    $(CPPULIB)			\
    $(SALLIB)			\
    $(TKLIB)

APP5OBJS=$(TARGETOBJS)
APP5OBJS += $(OBJ)$/copyright_ascii_ooo.obj

.IF "$(OS)" == "LINUX"
APP5STDLIBS+= -lXext -lSM -lICE
.ENDIF # LINUX

APP5DEPN= $(APP1TARGETN) $(APP5RES) ooverinfo.rc
APP5DEF=    $(MISCX)$/$(TARGET).def

.IF "$(GUI)" == "WNT"
APP5RES=    $(RES)$/oodesktop.res
APP5ICON=$(SOLARRESDIR)$/icons/ooo-main-app.ico
APP5VERINFO=ooverinfo.rc
APP5LINKRES=$(MISC)$/ooffice5.res
APP5STACK=10000000
.ENDIF # WNT

.IF "$(GUI)" == "WNT"
APP6TARGET=so$/officeloader
APP6RES=$(RES)$/soloader.res
APP6NOSAL=TRUE
APP6DEPN= $(APP1TARGETN) $(APP6RES) verinfo.rc
APP6VERINFO=verinfo.rc
APP6LINKRES=$(MISC)$/soffice6.res
APP6ICON=$(SOLARRESDIR)$/icons/so8-main-app.ico
APP6OBJS = \
        $(OBJ)$/officeloader.obj
STDLIB6=advapi32.lib

APP7TARGET=officeloader
APP7RES=$(RES)$/ooloader.res
APP7NOSAL=TRUE
APP7DEPN= $(APP1TARGETN) $(APP7RES) ooverinfo.rc
APP7VERINFO=ooverinfo.rc
APP7LINKRES=$(MISC)$/ooffice7.res
APP7ICON=$(SOLARRESDIR)$/icons/ooo-main-app.ico
APP7OBJS = \
        $(OBJ)$/officeloader.obj
STDLIB7=advapi32.lib
.ENDIF # WNT

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(APP1TARGETN)"!=""
$(APP1TARGETN) : $(BIN)$/so
.ENDIF			# "$(APP1TARGETN)"!=""

.IF "$(GUI)" == "WNT"
ALLTAR: $(MISC)$/$(TARGET).exe.manifest
ALLTAR: $(MISC)$/$(TARGET).bin.manifest
ALLTAR: $(BIN)$/$(TARGET).bin
ALLTAR: $(BIN)$/so$/$(TARGET).bin
.ENDIF # WNT

$(BIN)$/soffice_oo$(EXECPOST) : $(APP5TARGETN)
    $(COPY) $< $@

$(BIN)$/so$/soffice_so$(EXECPOST) : $(APP1TARGETN)
    $(COPY) $< $@

ALLTAR : $(BIN)$/so$/soffice_so$(EXECPOST) $(BIN)$/soffice_oo$(EXECPOST)


.IF "$(GUI)" == "WNT"

# create a manifest file with the same name as the
# office executable file soffice.exe.manifest
.IF "$(CCNUMVER)" <= "001399999999"
$(MISC)$/$(TARGET).exe.manifest: template.manifest
   $(COPY) $< $@
.ELSE
$(MISC)$/$(TARGET).exe.template.manifest: template.manifest
   $(COPY) $< $@

$(MISC)$/$(TARGET).exe.linker.manifest: $(BIN)$/$(TARGET)$(EXECPOST)
   mt.exe -inputresource:$(BIN)$/$(TARGET)$(EXECPOST) -out:$@

$(MISC)$/$(TARGET).exe.manifest: $(MISC)$/$(TARGET).exe.template.manifest $(MISC)$/$(TARGET).exe.linker.manifest
   mt.exe -manifest $(MISC)$/$(TARGET).exe.linker.manifest $(MISC)$/$(TARGET).exe.template.manifest -out:$@
.ENDIF

# create a manifest file with the same name as the
# office executable file soffice.bin.manifest
.IF "$(CCNUMVER)" <= "001399999999"
$(MISC)$/$(TARGET).bin.manifest: template.manifest
   $(COPY) $< $@
.ELSE
$(MISC)$/$(TARGET).bin.manifest: $(MISC)$/$(TARGET).exe.manifest
   $(COPY) $(MISC)$/$(TARGET).exe.manifest $@
.ENDIF

$(BIN)$/$(TARGET).bin: $(BIN)$/$(TARGET)$(EXECPOST)
   $(COPY) $< $@

$(BIN)$/so$/$(TARGET).bin: $(BIN)$/so$/$(TARGET)$(EXECPOST)
   $(COPY) $< $@

.ENDIF # WNT

$(BIN)$/so: makefile.mk
    @echo APP5 : $(APP5TARGET)
    @@-$(MKDIR) $(BIN)$/so

