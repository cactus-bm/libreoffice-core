#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1.1.1 $
#
#   last change: $Author: hr $ $Date: 2000-09-18 16:12:12 $
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

PRJNAME=basic
TARGET=sb
#basic.hid generieren
GEN_HID=TRUE

# --- Settings ---------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk

.IF "$(GUI)"=="WIN"
.IF "$(product)" != ""
LINKFLAGS=$(LINKFLAGS) /NOPACKC
.ENDIF
.ENDIF

.IF "$(depend)" == ""

# --- Allgemein ---------------------------------------------------

.IF "$(header)" == ""


# --- TESTTOOL MINIAPP ------------------------------------------------------

.IF "$(GUI)" != "MAC"
SRS3FILES= $(SRS)$/miniapp.srs
RES3TARGET=miniapp

APP3TARGET=miniapp
APP3STDLIBS= \
            $(TOOLSLIB) \
            $(SVTOOLLIB) \
            $(PLUGCTORLIB) \
            $(SVLIB) \
            $(SVMEMLIB) \
            $(SJLIB) \
            $(SO2LIB)
.IF "$(GUI)"=="UNX"
APP3STDLIBS+= \
            $(VOSLIB) $(OSLLIB)
.ENDIF


APP3LIBS= \
        $(LIBPRE) $(LB)$/miniapp.lib

APP3DEPN=\
        $(L)$/svtool.lib \
        $(L)$/itools.lib \
        $(SVLIBDEPEND) \
        $(LB)$/miniapp.lib

.IF "$(GUI)" != "UNX"
#               win16 braucht ein appobj
APP3OBJS=               $(OBJ)$/testapp.obj
.ENDIF

#APP3DEF=        $(MISC)$/$(PRJNAME).def
APP3RES=        $(RES)$/miniapp.res

.ENDIF

# --- Targets -----------------------------------------------------------

ALL: ALLTAR


#-------------------------------------------------------------------------
#                                                               Windows 3.x
#-------------------------------------------------------------------------


.IF "$(GUI)" == "WIN"

LINKFLAGS+=/NOCV /IG
LINK=$(DEVROOT)$/bin\optlinks\optlinks

$(MISC)$/$(PRJNAME).def: makefile
    echo NAME                BASIC                                                                                   >$@
    echo DESCRIPTION 'StarBASIC DevSystem (C)1994 STAR DIVISION GmbH'>>$@
    echo EXETYPE     WINDOWS                                                                                 >>$@
    echo PROTMODE                                                                                                    >>$@
    echo STUB                'winSTUB.EXE'                                   >>$@
    echo CODE                LOADONCALL MOVEABLE                                                     >>$@
    echo DATA                PRELOAD MULTIPLE MOVEABLE                                               >>$@
    echo HEAPSIZE    4096                                                                                    >>$@
    echo STACKSIZE   30000                                                                                   >>$@
.ENDIF # GUI == WIN

#-------------------------------------------------------------------------
#                                                               MAC
#-------------------------------------------------------------------------

.IF "$(GUI)" == "MAC"

$(MISC)$/$(PRJNAME).def: makefile
    echo Kein def-File fuer Applikationen auf Mac
.ENDIF # GUI == MAC

#-------------------------------------------------------------------------
#                                                                               OS/2
#-------------------------------------------------------------------------

.IF "$(GUI)" == "OS2"

$(MISC)$/$(PRJNAME).def: makefile
.IF "$(COM)"!="WTC"
    echo NAME                BASIC WINDOWAPI                                                                  >$@
    echo DESCRIPTION 'StarBASIC DevSystem (C)1993 STAR DIVISION GmbH' >>$@
    echo EXETYPE     OS2                                                                                      >>$@
    echo PROTMODE                                                                                                     >>$@
    echo STUB                'OS2STUB.EXE'                                    >>$@
    echo CODE                LOADONCALL                                                                       >>$@
    echo DATA                PRELOAD MULTIPLE                                                                 >>$@
    echo HEAPSIZE    4096                                                                                     >>$@
    echo STACKSIZE   30000                                                                                    >>$@
.ELSE
    @echo option DESCRIPTION 'StarBasic DLL'                           >$@
    @echo name $(BIN)$/$(SHL1TARGET).dll                             >>$@
#    @ldump -E1 -A -F$(MISC)$/$(SHL1TARGET).flt $(SLB)$/sb.lib    >>temp.def
    @ldump -E1 -A -F$(MISC)$/$(SHL1TARGET).flt $(LIB1TARGET)    >>temp.def
    @awk -f s:\util\exp.awk temp.def
    del temp.def
.ENDIF

.ENDIF # GUI == OS2

#-------------------------------------------------------------------------
#                                                               Windows NT
#-------------------------------------------------------------------------
#
#                                       default targets aus target.mk
#

# --- Basic-Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile
    @echo ------------------------------
    @echo Making: $@
    @echo WEP > $@
    @echo LIBMAIN >> $@
    @echo LibMain >> $@
    @echo Sbi >> $@
    @echo SvRTL >> $@
    @echo SbRtl_ >> $@

.ENDIF

# ------------------------------------------------------------------------
.ENDIF

.INCLUDE :  target.mk



