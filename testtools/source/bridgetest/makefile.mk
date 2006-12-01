#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.27 $
#
#   last change: $Author: rt $ $Date: 2006-12-01 14:45:27 $
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

PRJ=..$/..

PRJNAME=testtools
TARGET=bridgetest
USE_DEFFILE=TRUE
ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO

.INCLUDE: settings.mk

DLLPRE = # no leading "lib" on .so files

.IF "$(GUI)"=="WNT"
BATCH_SUFFIX=.bat
GIVE_EXEC_RIGHTS=@echo
MY_URE_INTERNAL_JAVA_DIR=$(strip $(subst,\,/ file:///$(shell +$(WRAPCMD) echo $(SOLARBINDIR))))
MY_LOCAL_CLASSDIR=$(subst,\,/ file:///$(PWD)$/$(CLASSDIR))
.ELSE
BATCH_INPROCESS=bridgetest_inprocess
GIVE_EXEC_RIGHTS=chmod +x 
MY_URE_INTERNAL_JAVA_DIR=file://$(SOLARBINDIR)
MY_LOCAL_CLASSDIR=file://$(PWD)$/$(CLASSDIR)
.ENDIF

SLOFILES = \
    $(SLO)$/bridgetest.obj \
    $(SLO)$/cppobj.obj \
    $(SLO)$/currentcontextchecker.obj \
    $(SLO)$/multi.obj

# ---- test ----

LIB1TARGET=$(SLB)$/cppobj.lib
LIB1OBJFILES= \
        $(SLO)$/cppobj.obj $(SLO)$/currentcontextchecker.obj $(SLO)$/multi.obj

SHL1TARGET = cppobj.uno
SHL1STDLIBS= \
        $(CPPULIB)		\
        $(CPPUHELPERLIB)	\
        $(SALLIB)

SHL1LIBS=	$(LIB1TARGET)
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
DEF1NAME=	$(SHL1TARGET)
.IF "$(OS)$(CPU)$(COMNAME)" == "LINUXIgcc3"
SHL1VERSIONMAP = component.LINUXIgcc3.map
.ELSE
SHL1VERSIONMAP = component.map
.ENDIF

# ---- test object ----

LIB2TARGET=$(SLB)$/bridgetest.lib
LIB2OBJFILES= \
        $(SLO)$/bridgetest.obj \
        $(SLO)$/currentcontextchecker.obj \
        $(SLO)$/multi.obj

SHL2TARGET = bridgetest.uno
SHL2STDLIBS= \
        $(CPPULIB)		\
        $(CPPUHELPERLIB)	\
        $(SALLIB)

SHL2LIBS=	$(LIB2TARGET)
SHL2DEF=	$(MISC)$/$(SHL2TARGET).def
DEF2NAME=	$(SHL2TARGET)
.IF "$(OS)$(CPU)$(COMNAME)" == "LINUXIgcc3"
SHL2VERSIONMAP = component.LINUXIgcc3.map
.ELSE
SHL2VERSIONMAP = component.map
.ENDIF

SHL3TARGET = constructors.uno
SHL3OBJS = $(SLO)$/constructors.obj
SHL3STDLIBS = $(CPPULIB) $(CPPUHELPERLIB) $(SALLIB)
SHL3VERSIONMAP = component.map
SHL3IMPLIB = i$(SHL3TARGET)
DEF3NAME = $(SHL3TARGET)

.IF "$(SOLAR_JAVA)" != ""
JARFILES = java_uno.jar jurt.jar ridl.jar
JAVATARGETS=\
    $(DLLDEST)$/bridgetest_javaserver$(BATCH_SUFFIX) \
    $(DLLDEST)$/bridgetest_inprocess_java$(BATCH_SUFFIX)
.ENDIF

# --- Targets ------------------------------------------------------
ALL : \
        ALLTAR \
        $(DLLDEST)$/uno_types.rdb \
        $(DLLDEST)$/uno_services.rdb \
        $(DLLDEST)$/bridgetest_inprocess$(BATCH_SUFFIX) \
        $(DLLDEST)$/bridgetest_server$(BATCH_SUFFIX) \
        $(DLLDEST)$/bridgetest_client$(BATCH_SUFFIX) \
        $(JAVATARGETS)

.INCLUDE :	target.mk

#################################################################

$(DLLDEST)$/uno_types.rdb : $(SOLARBINDIR)$/udkapi.rdb
    echo $(DLLDEST)
    $(GNUCOPY) -p $? $@
    $(REGMERGE) $@ / $(BIN)$/bridgetest.rdb

$(DLLDEST)$/bridgetest_inprocess$(BATCH_SUFFIX) : bridgetest_inprocess
    $(GNUCOPY) -p $? $@
    $(GIVE_EXEC_RIGHTS) $@

$(DLLDEST)$/bridgetest_client$(BATCH_SUFFIX) : bridgetest_client
    $(GNUCOPY) -p $? $@
    $(GIVE_EXEC_RIGHTS) $@

$(DLLDEST)$/bridgetest_server$(BATCH_SUFFIX) : bridgetest_server
    $(GNUCOPY) -p $? $@
    $(GIVE_EXEC_RIGHTS) $@


.IF "$(SOLAR_JAVA)" != ""
NULLSTR=
# jar-files, which regcomp needs so that it can use java
MY_JARS=java_uno.jar ridl.jar jurt.jar juh.jar

# CLASSPATH, which regcomp needs to be run
MY_CLASSPATH_TMP=$(foreach,i,$(MY_JARS) $(SOLARBINDIR)$/$i)$(PATH_SEPERATOR)$(XCLASSPATH)
MY_CLASSPATH=$(strip $(subst,!,$(PATH_SEPERATOR) $(MY_CLASSPATH_TMP:s/ /!/)))$(PATH_SEPERATOR)..$/class

# Use "127.0.0.1" instead of "localhost", see #i32281#:
$(DLLDEST)$/bridgetest_javaserver$(BATCH_SUFFIX) : makefile.mk
    -rm -f $@
    +$(WRAPCMD) echo java -classpath $(MY_CLASSPATH)$(PATH_SEPERATOR)..$/class$/testComponent.jar \
        com.sun.star.comp.bridge.TestComponentMain \
        \""uno:socket,host=127.0.0.1,port=2002;urp;test"\" \
        > $@
    $(GIVE_EXEC_RIGHTS) $@

$(DLLDEST)$/bridgetest_inprocess_java$(BATCH_SUFFIX) : makefile.mk
    -rm -f $@
    echo uno -ro uno_services.rdb -ro uno_types.rdb \
        -s com.sun.star.test.bridge.BridgeTest \
        -env:URE_INTERNAL_JAVA_DIR=$(MY_URE_INTERNAL_JAVA_DIR) \
        -- com.sun.star.test.bridge.JavaTestObject noCurrentContext > $@
    $(GIVE_EXEC_RIGHTS) $@
.ENDIF

$(DLLDEST)$/uno_services.rdb .ERRREMOVE: $(DLLDEST)$/uno_types.rdb \
        $(DLLDEST)$/bridgetest.uno$(DLLPOST) $(DLLDEST)$/cppobj.uno$(DLLPOST) \
        $(MISC)$/$(TARGET)$/bootstrap.rdb $(SHL3TARGETN)
    - $(MKDIR) $(@:d)
    +cd $(DLLDEST) && $(REGCOMP) -register -br uno_types.rdb -r uno_services.rdb\
        -c acceptor.uno$(DLLPOST) \
        -c bridgefac.uno$(DLLPOST) \
        -c connector.uno$(DLLPOST) \
        -c remotebridge.uno$(DLLPOST) \
        -c uuresolver.uno$(DLLPOST) \
        -c bridgetest.uno$(DLLPOST) \
        -c cppobj.uno$(DLLPOST) \
        -c uriproc.uno$(DLLPOST) \
        -c $(SHL3TARGETN:f)
.IF "$(SOLAR_JAVA)" != ""
    $(REGCOMP) -register -br $(DLLDEST)$/uno_types.rdb -r $@ \
        -c javaloader.uno$(DLLPOST) -c javavm.uno$(DLLPOST)
    $(REGCOMP) -register  -br $(MISC)$/$(TARGET)$/bootstrap.rdb -r $@ -c \
        $(MY_LOCAL_CLASSDIR)/testComponent.jar \
        -env:URE_INTERNAL_JAVA_DIR=$(MY_URE_INTERNAL_JAVA_DIR)
.ENDIF

$(MISC)$/$(TARGET)$/bootstrap.rdb .ERRREMOVE:
    - $(MKDIR) $(@:d)
    + $(COPY) $(SOLARBINDIR)$/types.rdb $@
.IF "$(SOLAR_JAVA)" != ""
    $(REGCOMP) -register -r $@ -c javaloader.uno$(DLLPOST) \
        -c javavm.uno$(DLLPOST) -c uriproc.uno$(DLLPOST)
.ENDIF
