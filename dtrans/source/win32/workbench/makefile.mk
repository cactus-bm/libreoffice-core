#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 18:30:18 $
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

PRJNAME=		dtrans
TARGET=			testwincb
TARGET1=		testmshl
LIBTARGET=		NO
TARGETTYPE=		CUI
USE_BOUNDCHK=
TESTCB=TRUE

.IF "$(USE_BOUNDCHK)"=="TR"
bndchk=tr
stoponerror=tr
.ENDIF

# --- Settings -----------------------------------------------------
#.INCLUDE :		$(PRJ)$/util$/makefile.pmk

.INCLUDE :  settings.mk

.IF "$(TESTCB)"=="TRUE"

.INCLUDE : ..$/..$/cppumaker.mk

CFLAGS+=-D_WIN32_DCOM -EHsc -Ob0

# --- Files --------------------------------------------------------

OBJFILES=   	$(OBJ)$/test_wincb.obj
APP1TARGET=		$(TARGET)
APP1OBJS=		$(OBJ)$/test_wincb.obj
                
                
APP1STDLIBS=	$(SALLIB) \
                $(CPPULIB)			\
                $(CPPUHELPERLIB)	\
                $(SOLARLIBDIR)$/uwinapi.lib\
                user32.lib \
                ole32.lib\
                comdlg32.lib

APP1LIBS= $(SLB)$/dtutils.lib
          

APP1NOSAL=  TRUE

.ENDIF

.IF "$(TESTCB)"==""

CFLAGS+=/D_WIN32_DCOM /EHsc /Ob0

OBJFILES=   	$(OBJ)$/testmarshal.obj
APP1TARGET=		$(TARGET1)
APP1OBJS=		$(OBJ)$/testmarshal.obj
                
APP1STDLIBS=	$(SALLIB)\
                user32.lib\
                ole32.lib\
                comsupp.lib\
                oleaut32.lib

APP1LIBS= 

APP1NOSAL=  TRUE

.ENDIF

# --- Targets ------------------------------------------------------
.INCLUDE :		target.mk
#.INCLUDE :		$(PRJ)$/util$/target.pmk
