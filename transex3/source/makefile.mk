#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.33 $
#   last change: $Author: hr $ $Date: 2004-10-11 13:56:09 $
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

PRJ=..

PRJNAME=transex
TARGET=transex3
TARGETTYPE=CUI
LIBTARGET=no
# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  libs.mk
CDEFS+= -DYY_NEVER_INTERACTIVE=1

.IF "$(SYSTEM_EXPAT)" == "YES"
CFLAGS+=-DSYSTEM_EXPAT
.ENDIF

# --- Files --------------------------------------------------------

OBJFILES=   			\
    $(OBJ)$/export.obj	\
    $(OBJ)$/export2.obj	\
    $(OBJ)$/merge.obj   \
    $(OBJ)$/wrdtrans.obj	\
    $(OBJ)$/wtratree.obj	\
    $(OBJ)$/wtranode.obj    \
    $(OBJ)$/srciter.obj		\
    $(OBJ)$/utf8conv.obj	\
    $(OBJ)$/xmlparse.obj    \
    $(OBJ)$/helpmerge.obj   \
    $(OBJ)$/helpex.obj      \
    $(OBJ)$/hw2fw.obj

LIB1TARGET= $(LB)$/transex.lib
LIB1ARCHIV= $(LB)$/libtransex.a
#LIB1FILES=  $(LB)$/transex3.lib
LIB1OBJFILES=        $(OBJ)$/export.obj      \
        $(OBJ)$/export2.obj     \
        $(OBJ)$/merge.obj   \
        $(OBJ)$/wrdtrans.obj    \
        $(OBJ)$/wtratree.obj    \
        $(OBJ)$/wtranode.obj    \
        $(OBJ)$/srciter.obj             \
        $(OBJ)$/utf8conv.obj    \
        $(OBJ)$/hw2fw.obj

APP1VERSIONMAP=exports.map

# extractor and merger for *.src and *.hrc
APP1TARGET=     $(TARGET)
APP1OBJS=   $(OBJ)$/src_yy.obj
APP1STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)
APP1LIBS+=	$(LB)$/$(PRJNAME).lib
APP1DEPN=   $(OBJ)$/src_yy.obj $(LB)$/$(PRJNAME).lib

#APP2TARGET= termilo
#APP2STACK=  16000
#APP2OBJS=   $(OBJ)$/termino.obj
#APP2STDLIBS=$(TOOLSLIBST) $(L)$/bootstrp.lib

APP2TARGET= helpex
APP2OBJS= $(OBJ)$/helpmerge.obj  $(OBJ)$/xmlparse.obj $(OBJ)$/export2.obj $(OBJ)$/utf8conv.obj $(OBJ)$/merge.obj $(OBJ)$/helpex.obj $(OBJ)$/hw2fw.obj
APP2STDLIBS=$(BTSTRPLIB) $(SALLIB) $(EXPATASCII3RDLIB) $(TOOLSLIB) $(VOSLIB) 

# extractor and merger for *.lng and *.lng
APP3TARGET= ulfex
APP3OBJS=   $(OBJ)$/lngmerge.obj $(OBJ)$/hw2fw.obj $(OBJ)$/merge.obj $(OBJ)$/export2.obj $(OBJ)$/lngex.obj $(OBJ)$/utf8conv.obj
APP3STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

# encoding converter for *.gsi
APP4TARGET= gsiconv
APP4OBJS=   $(OBJ)$/utf8conv.obj $(OBJ)$/gsiconv.obj
APP4STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

# tag checker for *.gsi
APP5TARGET= gsicheck
APP5OBJS=   $(OBJ)$/gsicheck.obj $(OBJ)$/tagtest.obj
APP5STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

# extractor and merger for *.cfg
APP6TARGET= cfgex
APP6OBJS=   $(OBJ)$/cfgmerge.obj $(OBJ)$/cfg_yy.obj $(OBJ)$/hw2fw.obj $(OBJ)$/merge.obj $(OBJ)$/export2.obj $(OBJ)$/utf8conv.obj

#.IF "$(OS)"!="MACOSX"
#APP6STDLIBS+= $(BTSTRPLIB)
#.ENDIF

APP6STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

# extractor and merger for *.xrm
APP7TARGET= xrmex
APP7OBJS=   $(OBJ)$/xrmmerge.obj $(OBJ)$/xrm_yy.obj $(OBJ)$/hw2fw.obj $(OBJ)$/merge.obj $(OBJ)$/export2.obj $(OBJ)$/utf8conv.obj

#.IF "$(OS)"!="MACOSX"
#APP7STDLIBS+= $(BTSTRPLIB)
#.ENDIF

APP7STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

#APP8TARGET=  sdfupdate
#APP8OBJS= $(OBJ)$/sdfupdate.obj
#APP8STDLIBS+= \
#            $(TOOLSLIB) \
#            $(VOSLIB) \
#            $(SALLIB)

#APP8TARGET= xgfconv
#APP8STACK=  16000
#APP8OBJS=   $(OBJ)$/utf8conv.obj $(OBJ)$/xgfconv.obj $(OBJ)$/export2.obj
#APP8STDLIBS=$(BTSTRPLIB) $(TOOLSLIBST)

# encoding converter for text files
APP8TARGET= txtconv
APP8STACK=  16000
APP8OBJS=   $(OBJ)$/utf8conv.obj $(OBJ)$/txtconv.obj $(OBJ)$/hw2fw.obj
APP8STDLIBS=$(TOOLSLIB) $(SALLIB)

# localizer for l10n framework
APP9TARGET= localize_sl
EXCEPTIONSFILES=                            \
                    $(OBJ)$/localize.obj
APP9OBJS=   $(OBJ)$/localize.obj $(OBJ)$/utf8conv.obj $(OBJ)$/srciter.obj $(OBJ)$/export2.obj
APP9STDLIBS+= \
            $(BTSTRPLIB) \
            $(TOOLSLIB) \
            $(VOSLIB) \
            $(SALLIB)

DEPOBJFILES=$(APP1OBJS) $(APP2OBJS) $(APP3OBJS) $(APP4OBJS) $(APP5OBJS) $(APP6OBJS) $(APP7OBJS) $(APP8OBJS) $(APP9OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

$(MISC)$/%_yy.c : %lex.l
    +flex -l -8 -o$@ $<

