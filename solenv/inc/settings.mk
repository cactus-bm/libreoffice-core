#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: settings.mk,v $
#
#   $Revision: 1.177 $
#
#   last change: $Author: kz $ $Date: 2005-11-07 12:43:19 $
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
MKFILENAME:=SETTINGS.MK

# check for new dmake features

.IF 400<=200
dmake_test_version:
    @+echo dmake version too old!
    force_dmake_to_error
.ENDIF

.IF "$(UNSUPPORTED_4nt_VERSION)"=="TRUE"
unsupported_4nt_version:
    @+echo 4NT version too old!
    force_dmake_to_error
.ENDIF

.IF "$(USE_COMMENT)"!=""
.INCLUDE : comment.mak
.ENDIF

# --- common tool makros --------------------------------------

# only need for ancient workspaces
.IF "$(OS)"=="SOLARIS"
wrapper_override_cc_wrapper=AUTO
#wrapper_override_cc_wrapper=FALSE
wrapper_use_clean_sourcecheck=TRUE
.ENDIF          # "$(OS)"=="SOLARIS"

.EXPORT : wrapper_override_cc_wrapper wrapper_use_clean_sourcecheck

# moved temporary wrapper here as it is used in unitools.mk
.IF "$(USE_SHELL)"!=""
use_shell*=$(USE_SHELL)
.ENDIF

.INCLUDE : unitools.mk

#have SCRIPTEXT already defined in product.mk but available for all
.IF "$(GUI)"=="WNT"
SCRIPTEXT=.btm
.ELSE           # "$(GUI)"=="WNT"
SCRIPTEXT=
.ENDIF          # "$(GUI)"=="WNT"

.IF "$(PRODUCTNAME)"!=""
.INCLUDE : product.mk
.ENDIF			# "$(PRODUCTNAME)"!=""

# --- set SOLARVERSION for older workspaces

.IF "$(SOLARVERSION)"==""
SOLARVERSION=$(SOLARVER)$/$(UPD)
.ENDIF

.INCLUDE : minor.mk

.IF "$(UPDATER)"!=""
%minor.mk :
.IF "$(SOURCEVERSION)"=="$(WORK_STAMP)"
    @+-$(MKDIRHIER) $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT) >& $(NULLDEV)
.ELSE			# "$(SOURCEVERSION)"=="$(WORK_STAMP)"
    @+echo "#"
    @+echo "#"
    @+echo "# ERROR: minor.mk in solenv/inc does not match your version!"
    @+echo "#"
    @+echo "#"
    force_dmake_to_error
.ENDIF			# "$(SOURCEVERSION)"=="$(WORK_STAMP)"
.IF "$(GUI)"=="UNX"
    @+tr -d "\015" < $(SOLARENV)$/inc$/minor.mk > $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT)$/$(UPD)minor.mk
    @+$(TOUCH) $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT)$/minormkchanged.flg >& $(NULLDEV)
.ELSE			# "$(GUI)"=="UNX"
    @+$(COPY) $(SOLARENV)$/inc$/minor.mk $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT)$/$(UPD)minor.mk >& $(NULLDEV)
    @+$(TOUCH) $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT)$/minormkchanged.flg >& $(NULLDEV)
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(UPDATER)"!=""

.IF "$(BSCLIENT)"=="TRUE"
.IF "$(UPDATER)"!="YES"
incorrect_settings:
    @+echo "#"
    @+echo "#" ERROR: setsolar option -bsclient used but UPDATER=YES not set!
    @+echo "#"
    force_dmake_to_error
.ENDIF
.ENDIF

.INCLUDE : $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT)$/$(UPD)minor.mk

%udkversion.mk : $(PRJ)$/inc$/udkversion.mk
    @+$(COPY) $(PRJ)$/inc$/udkversion.mk $@

.INCLUDE .IGNORE : $(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT)$/udkversion.mk

# --- reset defined Environments --------------------
.SUFFIXES:

ASM=
AFLAGS=


#START JAVA

#interpreter location
.IF "$(JAVAINTERPRETER)" == ""
.IF "$(JDK)" == "J++"
JAVAINTERPRETER=jview.exe
.ELSE
.IF "$(JDK)" == "SCAFE"
.IF "$(JDK_VERSION)" != ""
JAVAINTERPRETER=$(DEVROOT)$/vcafe11$/java$/bin$/java.exe
.ELSE
JAVAINTERPRETER=$(DEVROOT)$/cafepro$/java$/bin$/java.exe
.ENDIF
.ELSE
.IF "$(JDK)" == "gcj"
JAVAINTERPRETER=gij
.ELSE
JAVAINTERPRETER=java
.ENDIF
.ENDIF
.ENDIF
.ENDIF

#compiler location
.IF "$(JAVACOMPILER)" == ""
.IF "$(JDK)" == "J++"
JAVACOMPILER=jvc.exe
.ELSE
.IF "$(JDK)" == "SCAFE"
.IF "$(JDK_VERSION)" != ""
JAVACOMPILER=$(DEVROOT)$/vcafe11$/bin$/sj.exe
.ELSE
JAVACOMPILER=$(DEVROOT)$/cafepro$/bin$/sj.exe
.ENDIF
.ELSE
.IF "$(JDK)" == "gcj"
JAVACOMPILER=gcj
.ELSE
JAVACOMPILER=javac
.ENDIF
.ENDIF
.ENDIF
.ENDIF

#if javadoc is not already set
.IF "$(JAVADOC)"==""
JAVADOC=javadoc -J-Xmx120m
.ENDIF

#required arguments
.IF "$(JDK)" == "gcj"
JAVAC=$(JAVACOMPILER) --encoding=ISO-8859-15 -O2 -fno-assert -Wno-deprecated -C
JAVAI=$(JAVAINTERPRETER) -Dgnu.gcj.precompiled.db.path=$(GCJ_DATABASE)
.ELSE
JAVAC=$(JAVACOMPILER)
JAVAI=$(JAVAINTERPRETER)
.ENDIF

#classpath and response
.IF "$(JDK)" == "J++"
JAVACPS=/cp
.ELSE
.IF "$(JDK)" == "SCAFE"
JAVACPS=-classpath
JAVARESPONSE=TRUE
.ELSE
.IF "$(JDK)" == "gcj"
JAVACPS=--classpath
.ELSE
JAVACPS=-classpath
.ENDIF
JAVARESPONSE=
.ENDIF
.ENDIF

#END JAVA

CDEFS=
CXXDEFS=
CDEFSCXX=
CDEFSOBJ=
CDEFSSLO=
CDEFSGUI=
CDEFSCUI=
CDEFSST=
CDEFSMT=
CDEFSPROF=
CDEFSDEBUG=
CDEFSDBGUTIL=
CDEFSOPT=
HDEFS=

CFLAGS=
CFLAGSCALL=
CFLAGSCXX=
CFLAGSCC=
CFLAGSOBJ=
CFLAGSSLO=
CFLAGSOBJGUIST=
CFLAGSOBJCUIST=
CFLAGSOBJGUIMT=
CFLAGSOBJCUIMT=
CFLAGSSLOGUIMT=
CFLAGSSLOCUIMT=
CFLAGSPROF=
CFLAGSDEBUG=
CFLAGSDBGUTIL=
CFLAGSOPT=
CFLAGSNOOPT=
CFLAGSOUTOBJ=

CFLAGSINCXX=

LINKFLAGS=
LINKFLAGSAPPGUI=
LINKFLAGSSHLGUI=
LINKFLAGSAPPCUI=
LINKFLAGSSHLCUI=
LINKFLAGSTACK=
LINKFLAGSPROF=
LINKFLAGSDEBUG=
LINKFLAGSOPT=
LINKFLAGSAPP=
LINKFLAGSSHL=
LINKEXTENDLINE=

STDOBJGUI=
STDSLOGUI=
STDOBJCUI=
STDSLOCUI=
STDLIBGUIST=
STDLIBCUIST=
STDLIBGUIMT=
STDLIBCUIMT=
STDSHLGUIMT=
STDSHLCUIMT=
STDOBJ=
STDSLO=
STDLIB=
STDSHL=

LIBMGR=
LIBFLAGS=

IMPLIBMGR=
IMPLIBFLAGS=

MAPSYM=
MAPSYMFLAGS=

RSC=
RSCFLAGS=
RSCDEFS=
RSCLANG=

RC=
RCFLAGS=
RCLINK=
RCLINKFLAGS=
RCSETVERSION=

DLLPOSTFIX=

HC=
HCFLAGS=

MKDEP=

AS=
BC=
COBOL=
CPP=
FOR=
PASCAL=

BFLAGS=
COBFLAGS=
CPPFLAGS=
CXXFLAGS=
FFLAGS=
PFLAGS=
RFLAGS=

LNT=
LNTFLAGS=
LNTFLAGSGUI=
LNTFLAGSCOM=

# doesn't make sense
#IDLSTATICOBJS=

E2P=
CAP=
cap=

# not to reset because possibly taken from environment
#CC=
#CXX=
#LINK=


# avoid confusion with CUE PROFILE variable...
PROFILE=

# can be set on the command line. we shouldn't delete them!
#profile=
#REMOTE=
#remote=

# reset as setting those manually is no lonjger supported
DBGUTIL=
dbgutil=

# ===========================================================================
# unter NT werden Variablennamen an untergeordnete makefiles UPPERCASE
# weitergereicht, aber case significant ausgewertet!
# ---------------------------------------------------------------------------

DMAKE_WORK_DIR*:=$(subst,/,$/ $(PWD))

.IF "$(TMP)"!=""
tmp*=$(TMP)
.ENDIF

.IF "$(tmp)"!=""
TMP*=$(tmp)
.ENDIF

.IF "$(TEMP)"!=""
temp*=$(TEMP)
.ENDIF

.IF "$(temp)"!=""
TEMP*=$(temp)
.ENDIF

.IF "$(DEPEND)"!=""
depend=$(DEPEND)
.ENDIF

.IF "$(OPTIMIZE)"!=""
optimize=$(OPTIMIZE)
.ENDIF

.IF "$(NOPT)"!=""
nopt*=$(NOPT)
.ENDIF

.IF "$(GROUP)"!=""
group*=$(GROUP)
.ENDIF

.IF "$(group)"!=""
GROUP*=$(group)
.ENDIF

# override dbglevel if set manually
.IF "$(dbglevel)"!=""
DBG_LEVEL=$(dbglevel)
.ENDIF

.IF "$(NODEBUG)"!=""
nodebug=$(NODEBUG)
.ENDIF

.IF "$(hbtoolkit)"!=""
HBTOOLKIT=$(hbtoolkit)
.ENDIF

.IF "$(REMOTE)"!=""
remote=$(REMOTE)
.ENDIF

.IF "$(PRODUCT)"!=""
product*=$(PRODUCT)
.ENDIF

.IF "$(product)"!=""
PRODUCT*=$(product)
.ENDIF

.IF "$(SOLARLANG)" != ""
solarlang*=$(SOLARLANG)
.ENDIF

.IF "$(solarlang)" != ""
SOLARLANG:=$(solarlang)
.ENDIF

.IF "$(wall)"!=""
COMPILER_WARN_ALL=TRUE
.ENDIF          # "$(wall)"!=""

.IF "$(RSC_ONCE)"!=""
rsc_once*=$(RSC_ONCE)
.ENDIF

.IF "$(COMMON_BUILD)"!=""
common_build*=$(COMMON_BUILD)
.ENDIF

.IF "$(USE_SHL_VERSIONS)"!=""
use_shl_versions*=$(USE_SHL_VERSIONS)
.ENDIF

# --- env flags not case sensitive --------------------------------

#.IF "$(envcflags)"!=""
ENVCFLAGS*=$(envcflags)
#.ENDIF

#.IF "$(envcflagscxx)"!=""
ENVCFLAGSCXX*=$(envcflagscxx)
#.ENDIF


.IF "$(envcdefs)"!=""
ENVCDEFS*=$(envcdefs)
.ENDIF

.IF "$(envlibflags)"!=""
ENVLIBFLAGS*=$(envlibflags)
.ENDIF

.IF "$(envcflagsincxx)"!=""
ENVCFLAGSINCXX*=$(envcflagsincxx)
.ENDIF

.IF "$(envlinkflags)"!=""
ENVLINKFLAGS*=$(envlinkflags)
.ENDIF

.IF "$(envrscflags)"!=""
ENVRSCFLAGS*=$(envrscflags)
.ENDIF

.IF "$(envrscdefs)"!=""
ENVRSCDEFS*=$(envrscdefs)
.ENDIF

.IF "$(envrsclinkflags)"!=""
# ENVRSCLINKFLAGS*=$(envrsclinkflags)
.ENDIF

.IF "$(envrcflags)"!=""
ENVRCFLAGS*=$(envrcflags)
.ENDIF

.IF "$(envrclinkflags)"!=""
ENVRCLINKFLAGS*=$(envrclinkflags)
.ENDIF

# --- set both L10N variables to keep things working ---------------

L10N-framework=$(L10N_framework)

# --- Parameter Einstellungen ueberpruefen und umsetzen ------------

# profile immer mit product
.IF "$(profile)"!=""
.IF "$(product)"==""
product=full
.ENDIF
.ENDIF

.IF "$(debug)"!=""
DBG_LEVEL*=2
.ENDIF

# Produkt auf einen Wert setzen (alles klein)
.IF "$(product)" != ""
optimize=true
.IF "$(product)" == "full" || "$(product)" == "Full" || "$(product)" == "FULL"
product!=full
.ENDIF
.IF "$(product)" == "demo" || "$(product)" == "Demo" || "$(product)" == "DEMO"
product!=demo
.ENDIF
.IF "$(product)" == "compact" || "$(product)" == "Compact" || "$(product)" == "COMPACT"
product!=compact
.ENDIF
DBG_LEVEL*=0
.ELSE
optimize!=true
dbgutil!=true
DBG_LEVEL*=1
.ENDIF

.IF "$(debug)"!=""
.IF "$(debug)"!="D_FORCE_OPT"
optimize=
OPTIMIZE=
.ENDIF
.ENDIF

.IF "$(nopt)"!=""
optimize=
OPTIMIZE=
.ENDIF

# Optimierung bei FinalCheck funktioniert nicht!
.IF "$(bndchk)" != ""
optimize=
OPTIMIZE=
.ENDIF

.IF "$(USE_NEWCHARSET)"!=""
.IF "$(USE_NEWCHARSET)"!="SRC510"
NEWCHARSET=TRUE
.ENDIF
.ENDIF

######################################################
#
# sprachabh. einstellungen
#
######################################################

.INCLUDE : lang.mk

.INCLUDE : postset.mk

# --- add L10N_framework codes -------------------------------------

alllangiso+=$(L10N_framework:s/,/ /)
completelangiso+=$(L10N_framework:s/,/ /)
RSC_LANG_ISO+:=$(completelangiso)
.EXPORT : RSC_LANG_ISO

######################################################

.IF "$(TARGETTYPE)"==""
TARGETTYPE=CUI
.ENDIF

.IF "$(TARGETTHREAD)"==""
.IF "$(GUI)" == "UNX"
TARGETTHREAD=MT
.ELSE
.IF "$(MULTITHREAD_OBJ)"!=""
TARGETTHREAD=MT
.ELSE
TARGETTHREAD=ST
.ENDIF
.ENDIF
.ENDIF

# Neues Enironment setzen
.SUFFIXES : .exe .lst .lin .dll .obj .dlo .asm .lib .c .hxx .cxx .res .rc .src .srs .hlp .y .yxx .odl .idl .java .class .hid .cpp

# --- Pfade setzen -------------------------------------------------

# Output-Pfad
# profile has to be first for not getting the .pro extension
.IF "$(profile)"!=""
OUT=$(PRJ)$/$(OUTPATH).cap
ROUT=$(OUTPATH).cap
.ELSE

.IF "$(product)"!=""
OUT=$(PRJ)$/$(OUTPATH).pro
ROUT=$(OUTPATH).pro

.ELSE
.IF "$(profile)"!=""
OUT=$(PRJ)$/$(OUTPATH).cap
ROUT=$(OUTPATH).cap
.ENDIF
.IF "$(dbcs)"!=""
OUT=$(PRJ)$/$(OUTPATH).w
ROUT=$(OUTPATH).w
.ENDIF
# could already be set by makefile.mk
.IF "$(OUT)" == ""
OUT*=$(PRJ)$/$(OUTPATH)
ROUT*=$(OUTPATH)
.ENDIF
.ENDIF
.ENDIF

.IF "$(bndchk)" != ""
OUT:=$(PRJ)$/$(OUTPATH).bnd
ROUT=$(OUTPATH).bnd
.ENDIF
.IF "$(truetime)" != ""
OUT=$(PRJ)$/$(OUTPATH).tt
ROUT=$(OUTPATH).tt
.ENDIF
.IF "$(hbtoolkit)"!=""
OUT=$(PRJ)$/$(OUTPATH).tlk
ROUT=$(OUTPATH).tlk
.ENDIF

# common output tree
.IF "$(common_build)"!=""
COMMON_OUTDIR*=common
.IF "$(no_common_build_reslib)"==""
common_build_reslib=true
.ENDIF			# "$(no_common_build_reslib)"==""
.IF "$(no_common_build_zip)"==""
common_build_zip=true
.ENDIF			# "$(no_common_build_zip)"==""
.IF "$(no_common_build_sign_jar)"==""
common_build_sign_jar=true
.ENDIF			# "$(no_common_build_sign_jar)"==""
.IF "$(no_common_build_srs)"==""
common_build_srs=true
.ENDIF			# "$(no_common_build_srs)"==""
.ELSE			# "$(common_build)"!=""
COMMON_OUTDIR=$(OUTPATH)
.ENDIF			# "$(common_build)"!=""

LOCAL_OUT:=$(OUT)
LOCAL_COMMON_OUT:=$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))
.EXPORT : LOCAL_OUT LOCAL_COMMON_OUT

# --- generate output tree -----------------------------------------

%world.mk :
    @+$(MKOUT) $(ROUT)
.IF "$(GUI)"=="UNX"
    @+echo \# > $(OUT)$/inc$/myworld.mk
.ELSE			# "$(GUI)"=="UNX"
    @+echo # > $(OUT)$/inc$/myworld.mk
.ENDIF			# "$(GUI)"=="UNX"
.IF "$(common_build)"!=""
    @+$(MKOUT) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(ROUT))
.IF "$(GUI)"=="UNX"
    @+echo \# > {$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))}$/inc$/myworld.mk
.ELSE			# "$(GUI)"=="UNX"
    @+echo # > {$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))}$/inc$/myworld.mk
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF			# "$(common_build)"!=""

.INCLUDE .IGNORE : $(OUT)$/inc$/myworld.mk
.IF "$(common_build)"!=""
.INCLUDE .IGNORE : {$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))}$/inc$/myworld.mk
.ENDIF			# "$(common_build)"!=""

# --- ausgabebaum remote erzeugen ----------------------------------
.IF "$(remote)"!=""

%worldremote.mk :
    @+$(MKOUT) -r $(ROUT)
    @+echo # > $(OUT)$/inc$/myworldremote.mk
.IF "$(common_build)"!=""
    @+$(MKOUT) -r $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(ROUT))
    @+echo # > {$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))}$/inc$/myworldremote.mk
.ENDIF			# "$(common_build)"!=""

.INCLUDE .IGNORE : $(OUT)$/inc$/myworldremote.mk
.IF "$(common_build)"!=""
.INCLUDE .IGNORE : {$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))}$/inc$/myworldremote.mk
.ENDIF			# "$(common_build)"!=""

.ENDIF			# "$(remote)"!=""

# Misc-Pfad
.IF "$(UNR)"!=""
MISCX=$(OUT)$/umisc
MISC=$(OUT)$/umisc
.ELSE
MISC=$(OUT)$/misc
# pointing to misc in common output tree if exists
COMMONMISC={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(MISC))}
.ENDIF

OUTCXX=$(OUT)$/cxx

.IF "$(PACKAGE)"!=""
IDLPACKAGE=$(PACKAGE)
.IF "$(GUI)"=="UNX"
IDLPACKAGENAME:=$(shell, +echo $(IDLPACKAGE) | $(SED) 's/\\/\//g' )
.ENDIF			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="WNT"
IDLPACKAGENAME:=$(shell, +echo $(IDLPACKAGE) | $(SED) s/\\/\//g )
.ENDIF			# "$(GUI)"=="WNT"
.ELSE
IDLPACKAGE=$(PRJNAME)
IDLPACKAGENAME=$(PRJNAME)
.ENDIF

# Objekt-Pfad
OBJ=$(OUT)$/obj
SLO=$(OUT)$/slo
ROBJ=$(ROUT)$/obj
RSLO=$(ROUT)$/slo

# Particle Path
PAR=$(OUT)$/par

# Library-Pfad
LB=$(OUT)$/lib
SLB=$(OUT)$/slb

# wir haben ein ucr verzeichnis
UCR=$(OUT)$/ucr

# $(L) nur noch pfad ins solver\upd\...\lib
#L:={$(LB);$(SLB);$(ILIB)}
L=$(SOLARLIBDIR)

#.EXPORT : SLB LB L LIB
ENVPRJ:=$(PRJ)
#todo: check PRJ is still expected to exist in environment
.EXPORT : PRJ ENVPRJ

# Class-Path for java-classes
# obwohl der generierte Bytecode plattformunabhg. sein sollte
# generieren wir ihn ersteinmal ins abhaengige Verzeichnis
CLASSDIR=$(OUT)$/class
CLASSPATH!:=.$(PATH_SEPERATOR)$(CLASSDIR)$(PATH_SEPERATOR)$(CLASSPATH)
STARJAR=java -classpath $(CLASSPATH)$(PATH_SEPERATOR)$(SOLARENV)$/bin$/jtools.jar com.sun.star.tool.starjar.StarJar

STARDEP=javadep
.IF "$(PRJNAME)"=="sj2"
CLASSPATH!:=$(CLASSPATH:s/sj2/no/)
.ENDIF
.IF "$(PRJNAME)"=="uno"
CLASSPATH!:=$(CLASSPATH:s/uno/no/)
.ENDIF
.IF "$(PRJNAME)"=="tkt"
CLASSPATH!:=$(CLASSPATH:s/tkt/no/)
.ENDIF

# Makros fuer die Librarynamen des Solar
.INCLUDE .IGNORE : office.mk
.IF "$(OFFICEUPD)"==""
OFFICEUPD*:=$(UPD)
.ENDIF			# "$(OFFICEUPD)"==""
.INCLUDE : libs.mk

.IF "$(GUI)"=="WNT"
VERSIONOBJ=$(SLO)$/_version.obj
.ENDIF

.IF "$(GUI)"=="UNX"
VERSIONOBJ=$(SLO)$/_version.o
.ENDIF

.IF "$(GUI)"=="WNT"
WINVERSIONNAMES=$(UNIXVERSIONNAMES)
.ENDIF			# "$(GUI)"=="WNT"

.IF "$(GUI)"=="WNT"
SHELLLIB=$(LIBPRE) gdi32.lib $(LIBPRE) shell32.lib $(LIBPRE) advapi32.lib $(LIBPRE) comdlg32.lib
.ENDIF

.IF "$(GUI)" != "MAC"
SOLARLIBDEPN=\
    $(L)$/basic.lib\
    $(L)$/dg.lib\
    $(L)$/sfxdebug.lib\
    $(L)$/so2.lib\
    $(SVLIBDEPEND)\
    $(L)$/sfx.lib\
    $(L)$/isfx.lib\
    $(L)$/svmem.lib\
    $(L)$/svtool.lib\
    $(L)$/svx.lib\
    $(L)$/tools.lib
.ELSE
SOLARLIBDEPN=\
    $(SOLARLIB)
.ENDIF


# BIN-Pfad
.IF "$(UNR)"!=""
BIN=$(OUT)$/ubin
.ELSE
BIN=$(OUT)$/bin
.ENDIF
# pointing to misc in common output tree if exists
COMMONBIN=$(LOCAL_COMMON_OUT)$/bin

# Include-Pfad
# still without -I here s.a. target.mk INCLUDE
INC=$(PRJ)$/inc
INCLOCAL=..$/inc
INCGUI=$(PRJ)$/$(GUIBASE)$/inc
INCCOM=$(OUT)$/inc
INCCOMX=$(OUT)$/inc
INCUNOIDL=$(INCCOM)$/$(PRJNAME)
I={$(INCLUDE)}
INCDEPN=. $(INCGUI) $(INCLOCAL) $(INC)
.IF "$(PRJINC)"!=""
INCLOCPRJ=$(foreach,i,$(PRJINC) $i$/inc)
INCDEPN+=$(INCLOCPRJ)
.ENDIF
.IF "$(INCPRE)"!=""
INCDEPN+=$(INCPRE)
.ENDIF

# Resource-Pfad fuer .SRS

.IF "$(common_build_srs)"!=""
SRS=$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))$/srs
SRSX=$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(OUT))$/srs
.ELSE          # "$(common_build_srs)"!=""
SRS=$(OUT)$/srs
SRSX=$(OUT)$/srs
.ENDIF          # "$(common_build_srs)"!=""

# Resource-Pfad fuer .RC und .RES
RES=$(OUT)$/res


# das normale MISC wird nicht an LDMISC angepasst, stattdessen MISCX

.IF "$(make_xl)"!=""
BIN=$(PRJ)$/$(OUTPATH).xl$/bin
.ENDIF

.IF "$(remote)"!=""
BIN=$(LDBIN)$/remote
SLO=$(OUT)$/slo$/remote
OBJ=$(OUT)$/obj$/remote
RES=$(LDRES)$/remote
RSLO=$(INPATH)$/slo$/remote
ROBJ=$(INPATH)$/obj$/remote
MISC=$(OUT)$/misc$/remote
#SRSX=$(SRSX)$/remote
#INCCOMX=$(INCCOMX)$/remote
LB=$(OUT)$/lib$/remote
SLB=$(OUT)$/slb$/remote
REMOTEDEF=REMOTE_APPSERVER
REMOTELIB=$(SOLARVERSION)$/$(INPATH)$/lib$/remote
.ENDIF

# damit gezielt Abhaengigkeiten auf s: angegeben werden koennen

.IF "$(common_build)"!=""
SOLARIDLDIR=$(SOLARVERSION)$/common$(PROEXT)$/idl$(EXT_UPDMINOR)
.ELSE
SOLARIDLDIR=$(SOLARVERSION)$/$(INPATH)$/idl$(EXT_UPDMINOR)
.ENDIF

.IF "$(UPDMINOR)" != ""
EXT_UPDMINOR=.$(UPDMINOR)
.ELSE
EXT_UPDMINOR=
.ENDIF
SOLARRESDIR=$(SOLARVERSION)$/$(INPATH)$/res$(EXT_UPDMINOR)
SOLARRESXDIR=$(SOLARVERSION)$/$(INPATH)$/res$(EXT_UPDMINOR)
SOLARLIBDIR=$(SOLARVERSION)$/$(INPATH)$/lib$(EXT_UPDMINOR)
SOLARJAVADIR=$(SOLARVERSION)$/$(INPATH)$/java$(EXT_UPDMINOR)
SOLARINCDIR=$(SOLARVERSION)$/$(INPATH)$/inc$(EXT_UPDMINOR)
SOLARINCXDIR=$(SOLARVERSION)$/$(INPATH)$/inc$(EXT_UPDMINOR)
.IF "$(SOLARLANG)"!="deut"
.IF "$(SOLARLANG)" != ""
SOLARINCXDIR=$(SOLARVERSION)$/$(INPATH)$/inc$(EXT_UPDMINOR)$/$(SOLARLANG)
SOLARRESXDIR=$(SOLARVERSION)$/$(INPATH)$/res$(EXT_UPDMINOR)$/$(SOLARLANG)
.ENDIF
.ENDIF
SOLARBINDIR=$(SOLARVERSION)$/$(INPATH)$/bin$(EXT_UPDMINOR)
SOLARUCRDIR=$(SOLARVERSION)$/$(INPATH)$/ucr$(EXT_UPDMINOR)
SOLARPARDIR=$(SOLARVERSION)$/$(INPATH)$/par$(EXT_UPDMINOR)
SOLARXMLDIR=$(SOLARVERSION)$/$(INPATH)$/xml$(EXT_UPDMINOR)
SOLARDOCDIR=$(SOLARVERSION)$/$(INPATH)$/doc$(EXT_UPDMINOR)
SOLARPCKDIR=$(SOLARVERSION)$/$(INPATH)$/pck$(EXT_UPDMINOR)
SOLARCOMMONBINDIR=$(SOLARVERSION)$/common$(PROEXT)$/bin$(EXT_UPDMINOR)
SOLARCOMMONRESDIR=$(SOLARVERSION)$/common$(PROEXT)$/res$(EXT_UPDMINOR)
SOLARCOMMONPCKDIR=$(SOLARVERSION)$/common$(PROEXT)$/pck$(EXT_UPDMINOR)
.IF "$(common_build)"==""
SOLARCOMMONBINDIR=$(SOLARBINDIR)
SOLARCOMMONRESDIR=$(SOLARRESDIR)
SOLARCOMMONPCKDIR=$(SOLARPCKDIR)
.ENDIF


.IF "$(PRE)"==""
#JARDIR=$(CLASSDIR)
JARDIR=$(OUT)$/class
.ELSE
JARDIR=$(PRE)$/class
.ENDIF

#now mess around with SOLARINC, SOLARINCLUDES, SOLARLIB and LIB
.IF "$(UDKVERSION)"!=""
SOLARINC!:=-I$(SOLARROOT)$/udk$/$(UDKVERSION)$/$(OUTPATH)$/inc $(SOLARINC)
SOLARINCLUDES!:=-I$(SOLARROOT)$/udk$/$(UDKVERSION)$/$(OUTPATH)$/inc $(SOLARINCLUDES)
SOLARLIB!:=-L$(SOLARROOT)$/udk$/$(UDKVERSION)$/$(OUTPATH)$/lib $(SOLARLIB)
ILIB!:=$(SOLARROOT)$/udk$/$(UDKVERSION)$/$(OUTPATH)$/lib;$(ILIB)
.ENDIF			# "$(UDKVERSION)"!=""

# needs to be expanded!!!

# extend library path for MS Compiler
.IF "$(COM)"=="MSC"
LIB:=$(LB);$(SLB);$(ILIB)
.ENDIF

# --- Environment-Allgemein ----------------------------------------

.IF "$(CPUNAME)" == ""
CPUNAME=CPUNAME_HAS_TO_BE_SET_IN_ENVIRONMENT
.ENDIF

.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
DATESTRING:=$(shell date +%d%m%Y)
.ELSE			# "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
#can't override 4nt internal date to use date.exe :-(
DATESTRING:=$(shell +echo %@IF[%@LEN[%_DAY%]==1,0%_DAY%,%_DAY%]%@IF[%@LEN[%_MONTH%]==1,0%_MONTH%,%_MONTH%]%_YEAR%)
.ENDIF			# "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
SCPDEFS+=-DSCP_DATESTRING=$(DATESTRING)


.IF "$(USE_STLP_DEBUG)" != ""
SCPCDEFS+=-D_STLP_DEBUG
.ENDIF

.IF "$(UDK_MAJOR)"!=""
SCPDEFS+=-DUDK_MAJOR=$(UDK_MAJOR)
.ENDIF			# "$(UDK_MAJOR)"!=""

.IF "$(OFFICEUPD)"!=""
SCPDEFS+=-DOFFICEUPD=$(OFFICEUPD)
.ENDIF			# "$(OFFICEUPD)"!=""

SCPDEFS+=-U$(COMID) -DCOMID=$(COMID) -DCOMNAME=$(COMNAME) -D_$(COMID)
SCPDEFS+=-DCCNUMVER=$(CCNUMVER)
.IF "$(COM)"=="GCC"
SCPDEFS+=-DSHORTSTDCPP3=$(SHORTSTDCPP3)
.ENDIF			# "$(SHORTSTDCPP3)"!=""

UNOIDLDEFS+=-DSUPD=$(UPD) -DUPD=$(UPD)

UNOIDLDEPFLAGS=-Mdepend=$(SOLARVER)

.IF "$(PRE)"!=""
UNOIDLINC!:=-I$(PRE)$/idl $(UNOIDLINC)
.ENDIF
UNOIDLINC+=-I. -I.. -I$(PRJ) -I$(PRJ)$/inc -I$(PRJ)$/$(INPATH)$/idl -I$(OUT)$/inc -I$(SOLARIDLDIR) -I$(SOLARINCDIR)

.IF "$(remote)" != ""
CDEFS= -D$(OS) -D$(GUI) -D$(GVER) -D$(COM) -D$(CVER) -D$(CPUNAME) -D$(REMOTEDEF)
.ELSE
CDEFS= -D$(OS) -D$(GUI) -D$(GVER) -D$(COM) -D$(CVER) -D$(CPUNAME)
.ENDIF

.IF "$(USE_STLP_DEBUG)" != ""
CDEFS+=-D_STLP_DEBUG
.ENDIF

.IF "$(CDEFS_PRESET)" != ""
CDEFS+=$(CDEFS_PRESET)
.ENDIF

.IF "$(COM)"=="GCC"
GXX_INCLUDE_PATH*=$(COMPATH)$/include$/c++$/$(CCVER)
CDEFS+= -DGXX_INCLUDE_PATH=$(GXX_INCLUDE_PATH)
.ENDIF

.IF "$(TIMELOG)" != ""
CDEFS+=-DTIMELOG
.ENDIF

.IF "$(GUI)"=="UNX"
CDEFS+=-DCVER=$(CVER)
.ENDIF

.IF "$(USE_NAMESPACE)"==""
CDEFS+=-D_USE_NO_NAMESPACE
.ELSE
CDEFS+=-D_USE_NAMESPACE
.ENDIF

CDEFSCXX=
CDEFSOBJ=
CDEFSSLO=-DSHAREDLIB -D_DLL_
CDEFSGUI=-DGUI
CDEFSCUI=-DCUI
CDEFSST=-DSINGLETHREAD
CDEFSMT=-DMULTITHREAD
CDEFSPROF=-DPROFILE
.IF "$(dbg_build_only)" == ""
CDEFSDEBUG=-DDEBUG
.ENDIF
CDEFSDBGUTIL=-DDBG_UTIL
CDEFSOPT=-DOPTIMIZE
HDEFS=-D:$(GUI) -D:$(COM)

MKDEPFLAGS=-D_SOLAR__PRIVATE -I$(INCDEPN:s/ / -I/:s/-I-I/-I/)
MKDEPALLINC=$(SOLARINC:s/-I/ -I/)
MKDEPPREINC=-I$(PREPATH)$/$(INPATH)$/inc$(UPDMINOREXT)
MKDEPSOLENV=-I$(SOLARENV)$/inc -I$(SOLARENV)$/$(GUI)$(CVER)$(COMEX)$/inc
MKDEPSOLVER=-I$(SOLARVERSION)$/$(INPATH)$/inc$(UPDMINOREXT) -I$(SOLARVERSION)$/$(INPATH)$/inc
MKDEPLOCAL=-I$(INCCOM)

.IF "$(MKDEPENDALL)"!=""
MKDEPFLAGS+=$(MKDEPALLINC)
.ENDIF

.IF "$(MKDEPENDSOLENV)"!=""
MKDEPFLAGS+=$(MKDEPSOLENV)
.ENDIF

.IF "$(MKDEPENDPREPATH)"!=""
MKDEPFLAGS+=$(MKDEPPREINC)
.ENDIF

.IF "$(MKDEPENDSOLVER)"!=""
MKDEPFLAGS+=$(MKDEPSOLVER)
.ENDIF
# unused - nodep should do the same now
#.IF "$(MKDEPENDLOCAL)"!="NO"
MKDEPFLAGS+=$(MKDEPLOCAL)
#.ENDIF

BISON=bison
YACCFLAGS*=-d 

SVIDL=$(WRAPCMD) svidl

LDUMP=$(WRAPCMD) ldump4
LDUMP2=$(WRAPCMD) ldump4

ZIPUPDATE=-u -j
ZIPFLAGS=$(ZIPUPDATE)
#hack for dynamic language subdirs
LANGDIR=LANGDIR

#scplinker flags
SCPLINKFLAGS=-i $(PAR),$(SOLARPARDIR)
.IF "$(SCPLINKVERSION)"!=""
SCPLINKFLAGS+=-v $(SCPLINKVERSION)
.ENDIF			# "$(SCPLINKVERSION)"!=""

.IF "$(make_srs_deps)"!=""
RSC=$(WRAPCMD) rscdep
.ELSE # "$(make_srs_deps)"!=""
RSC=$(WRAPCMD) rsc
.ENDIF # "$(make_srs_deps)"!=""

#new
RSCUPDVER=$(RSCREVISION)
RSCUPDVERDEF=-DUPDVER="$(RSCUPDVER)"

RSCFLAGS=-s
.IF "$(remote)" != ""
RSCDEFS=-D$(GUI) -D$(GVER) -D$(COM) -D$(CVER) -DSUPD=$(UPD) -D$(REMOTEDEF) $(JAVADEF)
.ELSE
RSCDEFS=-D$(GUI) -D$(GVER) -D$(COM) -D$(CVER) -DSUPD=$(UPD) $(JAVADEF)
.ENDIF

.IF "$(BUILD_SPECIAL)"!=""
RSCDEFS+=-DBUILD_SPECIAL=$(BUILD_SPECIAL)
.ENDIF

RSCDEFIMG*=default_images

RSCEXTINC=.

.IF "$(DEBUG)" != ""
RSCDEFS+= -DDEBUG
.ENDIF

UNOIDL=unoidl

# settings for mozilla idl compiler
XPIDL=xpidl
XPIDL_LINK=xpt_link

# alle bisher verwendeten Linker benutzen + um LIBs auf der naechsten Zeile
# weiter zu schreiben, wenn es da eine Ausnahme geben sollte, muss
# LINKEXTENDLINE compilerabhaengig definiert werden
LINKEXTENDLINE=+

LNT=$(DEVROOT)$/lint$/lint
#LNTFLAGS=+v -i$(DEVROOT)$/lint\ -mL options.lnt -u
LNTFLAGS=+v -i$(DEVROOT)$/lint$/ -mL options.lnt -u
LNTFLAGSOUTOBJ=-os

# --- StandardLibraries --------------------------------------------

SVLIBS=tools $(SVLIB) svtool
SFXLIBS=sfx so2 basic
SVXLIBS=svxitems dialogs editeng svrtf svdraw outliner xout si basicide \
        tbxctrls

.IF "$(GUI)"=="WNT"
.INCLUDE : wnt.mk
.ENDIF

.IF "$(GUI)" == "UNX"
.INCLUDE : unx.mk
.ENDIF

# for multiprocess building in external modules
# allow seperate handling
EXTMAXPROCESS*=$(MAXPROCESS)

IDLC*=$(WRAPCMD) idlc
REGMERGE*=$(WRAPCMD) regmerge
REGCOMPARE*=$(WRAPCMD) regcompare
REGCOMP*=$(WRAPCMD) regcomp
CPPUMAKER*=$(WRAPCMD) cppumaker
JAVAMAKER*=$(WRAPCMD) javamaker
RDBMAKER*=$(WRAPCMD) rdbmaker
.IF "$(JAVAINTERPRETER)" == ""
JAVA*=java
.ELSE
JAVA*=$(JAVAINTERPRETER)
.ENDIF
SCPLINK=$(WRAPCMD) $(PERL) $(SOLARENV)$/bin/par2script.pl
LZIP*=lzip
CPPLCC*=$(WRAPCMD) cpplcc

.IF "$(DISABLE_ENHANCED_COMID)"==""
.INCLUDE : tg_compv.mk
.ELSE          # "$(DISABLE_ENHANCED_COMID)"==""
COMID=$(COM)
.ENDIF          # "$(DISABLE_ENHANCED_COMID)"==""
.IF "$(SOLAR_JAVA)"=="TRUE"
.IF "$(USE_JAVAVER)"!=""
.INCLUDE : tg_javav.mk
.ENDIF "$(USE_JAVAVER)"!=""
.ENDIF			# "$(SOLAR_JAVA)"=="TRUE"

# --- extend new environment ----------------------------------
CDEFS+= -DSUPD=$(UPD)

# flags to enable build with symbols; required for crashdump feature
.IF "$(ENABLE_CRASHDUMP)"!="" || "$(ENABLE_SYMBOLS)"!=""
CFLAGSENABLESYMBOLS_CC_ONLY*=$(CFLAGSENABLESYMBOLS)
CFLAGSCXX+=$(CFLAGSENABLESYMBOLS)
CFLAGSCC+=$(CFLAGSENABLESYMBOLS_CC_ONLY)
.ENDIF          # "$(ENABLE_CRASHDUMP)"!="" || "$(ENABLE_SYMBOLS)"!=""

.IF "$(profile)"!=""
CDEFS+= $(CDEFSPROF)
CFLAGS+= $(CFLAGSPROF)
.IF "$(WST)"!=""
LINKFLAGS=$(LINKFLAGSWST)
LINKCFLAGS=$(LINKFLAGSWST)
.ELSE
LINKFLAGSADD+=$(LINKFLAGSPROF)
.ENDIF
.ENDIF

.IF "$(debug)"!=""
JAVAFLAGS+=$(JAVAFLAGSDEBUG)
CDEFS+= $(CDEFSDEBUG)
CFLAGS+= $(CFLAGSDEBUG)
LINKFLAGSADD+= $(LINKFLAGSDEBUG)
.ENDIF

.IF "$(dbgutil)"!=""
CDEFS+=$(CDEFSDBGUTIL)
CFLAGS+=$(CFLAGSDBGUTIL)
RSCDEFS+=-DDBG_UTIL
.ENDIF

.IF "$(product)"!=""
CDEFS+= -DPRODUCT -DNDEBUG
HDEFS+= -D:PRODUCT
RSCDEFS+= -DPRODUCT 
CDEFS+=-DPRODUCT_FULL
HDEFS+=-D:PRODUCT_FULL
RSCDEFS+= -DPRODUCT_FULL -DNDEBUG
.ENDIF

.IF "$(DBG_LEVEL)"!=""
CDEFS+=-DOSL_DEBUG_LEVEL=$(DBG_LEVEL)
RSCDEFS+=-DOSL_DEBUG_LEVEL=$(DBG_LEVEL)
.ENDIF

.IF "$(svx_light)"!=""
CDEFS+=-DSVX_LIGHT
.ENDIF

.IF "$(optimize)"!=""
CDEFS+=$(CDEFSOPT)
CFLAGS+=$(CFLAGSOPT)
LINKFLAGSADD+=$(LINKFLAGSOPT)
.ELSE
CFLAGS+=$(CFLAGSNOOPT)
.ENDIF

.IF "$(ENABLE_EXCEPTIONS)"!=""
CFLAGSCXX+=$(CFLAGSEXCEPTIONS)
CDEFS+=-DEXCEPTIONS_ON
.ELSE
.IF "$(OLD_EXCEPTIONS)"==""
CDEFS+=-DEXCEPTIONS_OFF
CFLAGSCXX+=$(CFLAGS_NO_EXCEPTIONS)
.ELSE
CDEFS+=-DEXCEPTIONS_ON
.ENDIF
.ENDIF

# compose flags and defines for GUI
.IF "$(TARGETTYPE)"=="GUI"
CDEFS+= $(CDEFSGUI)
CFLAGSSLO+=$(CFLAGSSLOGUIMT)
CDEFSSLO+=$(CDEFSSLOMT)
LINKFLAGSAPP=$(LINKFLAGSAPPGUI)
LINKFLAGSSHL=$(LINKFLAGSSHLGUI)
STDOBJ=$(STDOBJGUI)
STDSLO=$(STDSLOGUI)

.IF "$(TARGETTHREAD)"=="MT"
CDEFSOBJ+= $(CDEFSMT) $(CDEFSOBJMT)
CFLAGSOBJ+=$(CFLAGSOBJGUIMT)
STDLIB=$(STDLIBGUIMT)
STDSHL=$(STDSHLGUIMT)
.ELSE
CDEFSOBJ+= $(CDEFSST) $(CDEFSOBJST)
CFLAGSOBJ+=$(CFLAGSOBJGUIST)
STDLIB=$(STDLIBGUIST)
STDSHL=$(STDSHLGUIST)
.ENDIF

.ENDIF

# compose flags and defines for CUI
.IF "$(TARGETTYPE)" == "CUI" || "$(TARGETTYPE)"=="cui"
CDEFS+= $(CDEFSCUI)
CFLAGSSLO+=$(CFLAGSSLOCUIMT)
CDEFSSLO+=$(CDEFSSLOMT)
LINKFLAGSAPP=$(LINKFLAGSAPPCUI)
LINKFLAGSSHL=$(LINKFLAGSSHLCUI)
STDOBJ=$(STDOBJCUI)
STDSLO=$(STDSLOCUI)

.IF "$(TARGETTHREAD)" == "MT" || "$(TARGETTHREAD)"=="mt"
CDEFSOBJ+=$(CDEFSMT) $(CDEFSOBJMT)
CFLAGSOBJ+=$(CFLAGSOBJCUIMT)
STDLIB=$(STDLIBCUIMT)
STDSHL=$(STDSHLCUIMT)
.ELSE
CDEFSOBJ+=$(CDEFSST) $(CDEFSOBJST)
CFLAGSOBJ+=$(CFLAGSOBJCUIST)
STDLIB=$(STDLIBCUIST)
STDSHL=$(STDSHLCUIST)
.ENDIF

.ENDIF

.EXPORT : PICSWITCH

.IF "$(USE_SYSTEM_STL)"=="YES"
LIBSTLPORT=""
.ENDIF

.IF "$(NO_DEFAULT_STL)"==""
STDLIB+=$(LIBSTLPORT)
STDSHL+=$(LIBSTLPORT)
.ENDIF			# "$(NO_DEFAULT_STL)"==""

# fill up unresolved symbols not found else where
.IF "$(FILLUPARC)"!=""
STDLIB+=$(FILLUPARC)
STDSHL+=$(FILLUPARC)
.ENDIF			# "$(FILUPARC)"!=""

.IF "$(DISABLE_JAVA)"==""
.IF "$(SOLAR_JAVA)"!=""
CDEFS+=$(JAVADEF)
.ENDIF          # "$(SOLAR_JAVA)"!=""
.ELSE           # "$(DISABLE_JAVA)"==""
SOLAR_JAVA!:=
.EXPORT : SOLAR_JAVA
.IF "$(JDKPATH)"!=""
environment_confusion:
    @+echo ----------------------------------------------------------
    @+echo -
    @+echo - Error!
    @+echo -
    @+echo - $$JDKPATH and $$DISABLE_JAVA are set. this will lead
    @+echo - to impropper results.
    @+echo -
    @+echo ----------------------------------------------------------
    force_dmake_to_error
.ENDIF          # "$(JDKPATH)"!=""
.ENDIF          # "$(DISABLE_JAVA)"==""

.IF "$(WORK_STAMP)"!=""
CDEFS+=-D$(WORK_STAMP)
.ENDIF

.INCLUDE .IGNORE: $(UPD)$(LAST_MINOR).mk

.IF "$(ENVWARNFLAGS)"==""
.IF "$(COMPILER_WARN_ALL)"!=""
CFLAGSAPPEND+=$(CFLAGSWALL)
.ELSE           # "$(WARN_ALL)"!=""
CFLAGSAPPEND+=$(CFLAGSDFLTWARN)
.ENDIF          # "$(WARN_ALL)"!=""
.ELSE			# "$(ENVWARNFLAGS)"==""
CFLAGSAPPEND+=$(ENVWARNFLAGS)
.ENDIF			# "$(ENVWARNFLAGS)"==""

CDEFS+= $(ADDCDEFS)
CDEFS+= $(ENVCDEFS)
CFLAGS+= $(CFLAGSCALL) $(ENVCFLAGS)
CFLAGSCXX+= $(ENVCFLAGSCXX)
# andersrum wg. -TPxxxx
CFLAGSINCXX+:=$(ENVCFLAGSINCXX)
LIBFLAGS+= $(ENVLIBFLAGS)
LINKFLAGSADD+= $(ENVLINKFLAGS)
RSCFLAGS+= $(ENVRSCFLAGS)
RSCDEFS+= $(ENVRSCDEFS)
# RSCLINKFLAGS+= $(ENVRSCLINKFLAGS)
RCFLAGS+= $(ENVRCFLAGS)
RCLINKFLAGS+= $(ENVRCLINKFLAGS)

# make sure both linker variables are set
LINKC*=$(LINK)
LINKCFLAGS*=$(LINKFLAGS)
LINKFLAGS+=$(LINKFLAGSADD)
LINKCFLAGS+=$(LINKFLAGSADD)

#to be removed soon!!!
.IF "$(TF_CNTEX)"=="$(WORK_STAMP)"
CDEFS+= -DTF_CNTEX
.ENDIF

#to be removed soon!!!
.IF "$(TF_CAPELLA)"!=""
CDEFS+= -DTF_CAPELLA
.ENDIF

#.IF "$(UPDATER)"=="YES"
.IF "$(PRJNAME)"=="chaos" || "$(PRJNAME)"=="svtools"
CDEFS+=-DUPD=\"$(UPD)\" -DMINOR=\"$(LAST_MINOR)\" -DBUILD_ID=\"$(BUILD)\"
.ENDIF
#.ENDIF

#to be removed soon!!!
.IF "$(TF_NEWEX)"!=""
CDEFS+= -DTF_NEWEX
.ENDIF

#.IF "$(UPDATER)"=="YES"
#SVXLIGHT=TRUE
#.ENDIF

#defaults for UCR HEADER
UNOUCRBASE*=UCR
UNOUCROUT*=$(OUT)$/inc
UNOUCRRDB*=$(SOLARBINDIR)$/types.rdb

# --- Compiler -----------------------------------------------------
.INCLUDE : rules.mk

# --- include typelists --------------------------------------------

.IF "$(COMP1TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP1TYPELIST).mk
$(COMP1TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP1TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP2TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP2TYPELIST).mk
$(COMP2TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP2TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP3TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP3TYPELIST).mk
$(COMP3TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP3TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP4TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP4TYPELIST).mk
$(COMP4TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP4TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP5TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP5TYPELIST).mk
$(COMP5TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP5TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP6TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP6TYPELIST).mk
$(COMP6TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP6TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP7TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP7TYPELIST).mk
$(COMP7TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP7TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP8TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP8TYPELIST).mk
$(COMP8TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP8TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

.IF "$(COMP9TYPELIST)"!=""
.INCLUDE .IGNORE : $(MISC)$/$(COMP9TYPELIST).mk
$(COMP9TYPELIST)_XML2CMPTYPES:=$(shell xml2cmp -types stdout $(MISC)$/$(COMP9TYPELIST)$($(WINVERSIONNAMES)_MAJOR).xml)
.ENDIF

# workaround for strange dmake bug:
# if the previous block was a rule or a target, "\#" isn't recognized
# as an escaped "#". if it was an assignment, escaping works...
some_unique_variable_name:=1
