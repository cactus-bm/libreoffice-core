#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: nf $ $Date: 2001-05-08 11:09:27 $
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

PRJNAME=tools
TARGET=bootstrp
TARGETTYPE=CUI
NO_DEFAULT_STL=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
.INCLUDE :      static.mk

CDEFS+=-D_TOOLS_STRINGLIST

# --- Files --------------------------------------------------------

.IF "$(OS)"=="MACOS"
dummy:
    @echo No bootstrp for Mac OS
.ELSE	# "$(OS)"=="MACOS"

OBJFILES=                       \
    $(OBJ)$/sstring.obj     \
    $(OBJ)$/appdef.obj      \
    $(OBJ)$/cppdep.obj      \
    $(OBJ)$/command.obj     \
    $(OBJ)$/prj.obj         \
    $(OBJ)$/inimgr.obj      \
    $(OBJ)$/revision.obj    \
    $(OBJ)$/shellprp.obj 	\
    $(OBJ)$/fattr.obj 		\
    $(OBJ)$/minormk.obj 	

SLOFILES=                   \
    $(SLO)$/sstring.obj     \
    $(SLO)$/appdef.obj      \
    $(SLO)$/cppdep.obj      \
    $(SLO)$/command.obj     \
    $(SLO)$/prj.obj         \
    $(SLO)$/inimgr.obj      \
    $(SLO)$/revision.obj    \
    $(SLO)$/shellprp.obj 	\
    $(SLO)$/fattr.obj 		\
    $(SLO)$/minormk.obj 	

LIB1TARGET= $(LB)$/btstrp.lib
LIB1ARCHIV= $(LB)$/libbtstrp.a
LIB1FILES=  $(LB)$/bootstrp.lib

LIB2TARGET= $(SLB)$/btstrpsh.lib
LIB2ARCHIV= $(SLB)$/libbtstrpsh.a
LIB2FILES=  $(SLB)$/bootstrp.lib

LIB3TARGET=$(LB)$/bsvermap.lib
LIB3ARCHIV= $(LB)$/libbsvermap.a
LIB3OBJFILES=\
    $(OBJ)$/vermap.obj

APP1TARGET=     i_server
APP1STACK=      16000
APP1OBJS=   $(OBJ)$/iserver.obj
APP1STDLIBS=$(STATIC_LIBS)
APP1LIBS=       $(LB)$/bootstrp.lib
APP1DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib

APP2TARGET=     copyprj
APP2OBJS=   $(OBJ)$/cprjexe.obj $(OBJ)$/copyprj.obj
APP2STDLIBS=$(STATIC_LIBS)
APP2LIBS=       $(LB)$/bootstrp.lib
APP2DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib

APP3TARGET=mkcreate
APP3OBJS=   $(OBJ)$/mkcrexe.obj $(OBJ)$/mkcreate.obj
APP3STDLIBS=$(STATIC_LIBS)
APP3LIBS=       $(LB)$/bootstrp.lib
APP3DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib

APP4TARGET=     setsolar
APP4OBJS=   $(OBJ)$/setsolar.obj $(OBJ)$/envset.obj $(OBJ)$/vermap.obj
APP4STDLIBS=$(STATIC_LIBS)
APP4LIBS=       $(LB)$/bootstrp.lib
APP4DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib

APP5TARGET= zipdep
APP5OBJS=	$(OBJ)$/zipdep.obj
APP5LIBS=	$(LB)$/bootstrp.lib
APP5STDLIBS=$(STATIC_LIBS) 
APP5DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib $(OBJ)$/zipdep.obj

APP6TARGET= sspretty
APP6OBJS=	$(OBJ)$/sspretty.obj
APP6LIBS=	$(LB)$/bootstrp.lib
APP6STDLIBS=$(STATIC_LIBS) 
APP6DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib $(OBJ)$/sspretty.obj

APP7TARGET= rscdep
APP7OBJS=	$(OBJ)$/rscdep.obj
APP7LIBS=	$(LB)$/bootstrp.lib
APP7STDLIBS=$(STATIC_LIBS) 
APP7DEPN=   $(LB)$/atools.lib $(LB)$/bootstrp.lib $(OBJ)$/rscdep.obj

DEPOBJFILES		=	$(APP1OBJS) $(APP2OBJS) $(APP3OBJS) $(APP4OBJS) $(APP5OBJS) $(APP6OBJS) 

.ENDIF	# "$(OS)"=="MACOS"

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
