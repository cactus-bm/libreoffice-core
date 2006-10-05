#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.43 $
#
#   last change: $Author: kz $ $Date: 2006-10-05 10:43:23 $
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

INCPRE=$(MISC)

PRJNAME=transex3
TARGET=transex
TARGETTYPE=CUI
LIBTARGET=no
# --- Settings -----------------------------------------------------
ENABLE_EXCEPTIONS=TRUE

.INCLUDE :  settings.mk
CDEFS+= -DYY_NEVER_INTERACTIVE=1
#CDEFS+= -pg

.IF "$(SYSTEM_EXPAT)" == "YES"
CFLAGS+=-DSYSTEM_EXPAT
.ENDIF

    
# --- Files --------------------------------------------------------

OBJFILES=   			\
    $(OBJ)$/export.obj	\
    $(OBJ)$/export2.obj	\
    $(OBJ)$/merge.obj   \
    $(OBJ)$/srciter.obj		\
    $(OBJ)$/utf8conv.obj	\
    $(OBJ)$/xmlparse.obj    \
    $(OBJ)$/helpmerge.obj   \
    $(OBJ)$/helpex.obj      \
    $(OBJ)$/file.obj        \
    $(OBJ)$/directory.obj   \
    $(OBJ)$/hw2fw.obj

LIB1TARGET= $(LB)$/$(TARGET).lib
LIB1ARCHIV= $(LB)$/libtransex.a
#LIB1FILES=  $(LB)$/transex3.lib
LIB1OBJFILES=        $(OBJ)$/export.obj      \
        $(OBJ)$/export2.obj     \
        $(OBJ)$/merge.obj   \
        $(OBJ)$/srciter.obj             \
        $(OBJ)$/file.obj \
        $(OBJ)$/directory.obj     \
        $(OBJ)$/utf8conv.obj    \
        $(OBJ)$/hw2fw.obj

APP1VERSIONMAP=exports.map

# extractor and merger for *.src and *.hrc
APP1TARGET=  transex3
#APP1OBJS=   $(OBJ)$/src_yy.obj
APP1OBJS=   $(OBJ)$/src_yy_wrapper.obj

APP1STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

.IF "$(OS)"=="MACOSX"
# static libs at end for OS X
.ENDIF

APP1LIBS+=	$(LB)$/$(TARGET).lib
APP1DEPN=   $(OBJ)$/src_yy_wrapper.obj $(LB)$/$(TARGET).lib

APP2TARGET= helpex
APP2OBJS= $(OBJ)$/helpmerge.obj  $(OBJ)$/xmlparse.obj $(OBJ)$/export2.obj $(OBJ)$/utf8conv.obj $(OBJ)$/merge.obj $(OBJ)$/helpex.obj $(OBJ)$/hw2fw.obj

.IF "$(OS)"!="MACOSX"
.ENDIF

APP2STDLIBS+=$(SALLIB) $(EXPATASCII3RDLIB) $(TOOLSLIB) $(VOSLIB)

.IF "$(OS)"=="MACOSX"
# static libs at end for OS X
.ENDIF

# extractor and merger for *.lng and *.lng
APP3TARGET= ulfex
APP3OBJS=   $(OBJ)$/lngmerge.obj $(OBJ)$/hw2fw.obj $(OBJ)$/merge.obj $(OBJ)$/export2.obj $(OBJ)$/lngex.obj $(OBJ)$/utf8conv.obj

.IF "$(OS)"!="MACOSX"
#APP3STDLIBS+= $(BTSTRPLIB)
.ENDIF
APP3STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)
.IF "$(OS)"=="MACOSX"
# static libs at end for OS X
.ENDIF

# encoding converter for *.gsi
APP4TARGET= gsiconv
APP4OBJS=   $(OBJ)$/utf8conv.obj $(OBJ)$/gsiconv.obj
APP4STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

# tag checker for *.gsi
APP5TARGET= gsicheck
APP5OBJS=   $(OBJ)$/gsicheck.obj $(OBJ)$/tagtest.obj
APP5STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

# extractor and merger for *.cfg
APP6TARGET= cfgex
APP6OBJS=   $(OBJ)$/cfgmerge.obj $(OBJ)$/cfg_yy_wrapper.obj $(OBJ)$/hw2fw.obj $(OBJ)$/merge.obj $(OBJ)$/export2.obj $(OBJ)$/utf8conv.obj

.IF "$(OS)"!="MACOSX"
#APP6STDLIBS+= $(BTSTRPLIB)
.ENDIF

APP6STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

.IF "$(OS)"=="MACOSX"
# static libs at end for OS X
.ENDIF

# extractor and merger for *.xrm
APP7TARGET= xrmex
APP7OBJS=   $(OBJ)$/xrmmerge.obj $(OBJ)$/xrm_yy_wrapper.obj $(OBJ)$/hw2fw.obj $(OBJ)$/merge.obj $(OBJ)$/export2.obj $(OBJ)$/utf8conv.obj

.IF "$(OS)"!="MACOSX"
.ENDIF

APP7STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

.IF "$(OS)"=="MACOSX"
# static libs at end for OS X
.ENDIF

# encoding converter for text files
APP8TARGET= txtconv
APP8STACK=  16000
APP8OBJS=   $(OBJ)$/utf8conv.obj $(OBJ)$/txtconv.obj $(OBJ)$/hw2fw.obj
APP8STDLIBS=$(TOOLSLIB) $(SALLIB)

# localizer for l10n framework
APP9TARGET= localize_sl
EXCEPTIONSFILES=                            \
                    $(OBJ)$/localize.obj
APP9OBJS=   $(OBJ)$/localize.obj $(OBJ)$/utf8conv.obj $(OBJ)$/srciter.obj $(OBJ)$/export2.obj $(OBJ)$/file.obj $(OBJ)$/directory.obj

APP9STDLIBS+= \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

DEPOBJFILES=$(APP1OBJS) $(APP2OBJS) $(APP3OBJS) $(APP4OBJS) $(APP5OBJS) $(APP6OBJS) $(APP7OBJS) $(APP8OBJS) $(APP9OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

$(MISC)$/%_yy.c : %lex.l
    +flex -l -w -8 -o$@ $<

# Helper to suppress warnings in lex generated c code, see #i57362#

$(OBJ)$/src_yy_wrapper.obj: $(MISC)$/src_yy.c
$(OBJ)$/cfg_yy_wrapper.obj: $(MISC)$/cfg_yy.c
$(OBJ)$/xrm_yy_wrapper.obj: $(MISC)$/xrm_yy.c

