#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1.1.1 $
#
#   last change: $Author: hr $ $Date: 2000-09-18 16:30:15 $
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

PRJ=..$/..$/..

PRJNAME=goodies
TARGET=idxf
DEPTARGET=vidxf

PROJECTPCH4DLL=TRUE
PROJECTPCH=eeng_pch
PROJECTPCHSOURCE=eeng_pch


# --- Settings -----------------------------------------------------------

.IF "$(VCL)" != ""

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk

# --- Allgemein ----------------------------------------------------------

.IF "$(editdebug)"!="" || "$(EDITDEBUG)"!=""
CDEFS+= -DEDITDEBUG
.ENDIF

SLOFILES =  $(SLO)$/dxfgrprd.obj \
            $(SLO)$/dxfvec.obj   \
            $(SLO)$/dxfentrd.obj \
            $(SLO)$/dxfblkrd.obj \
            $(SLO)$/dxftblrd.obj \
            $(SLO)$/dxfreprd.obj \
            $(SLO)$/dxf2mtf.obj  \
            $(SLO)$/idxf.obj

# ==========================================================================

SHL1TARGET=     idx$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB=     idxf
SHL1STDLIBS=    $(TOOLSLIB) $(SVLIB) $(CPPULIB)

SHL1DEPN=       $(LB)$/idxf.lib
SHL1LIBS=       $(SLB)$/idxf.lib # $(LB)$/rtftoken.lib

.IF "$(GUI)" != "UNX"
SHL1OBJS=       $(SLO)$/idxf.obj
.ENDIF

SHL1DEF=        $(MISC)$/$(SHL1TARGET).def
SHL1BASE=0x1c000000

.IF "$(depend)" == ""

all:    ALLTAR

.IF "$(GUI)" == "OS2"

$(MISC)$/$(SHL1TARGET).def:\
    makefile.mk \
    $(MISC)$/$(SHL1TARGET).flt
    @echo -------------------------------------------
    @echo DEF-File erstellen
.IF "$(COM)"!="WTC"
    @echo LIBRARY     $(DLLNAME) INITINSTANCE TERMINSTANCE           >$@
    @echo DESCRIPTION 'StarView Filter DLL'                         >>$@
.IF "$(COM)" == "ZTC"
    @echo STUB        'os2STUB.EXE'                                 >>$@
.ENDIF
    @echo PROTMODE                                                  >>$@
    @echo CODE        LOADONCALL                                    >>$@
    @echo DATA        PRELOAD MULTIPLE NONSHARED                    >>$@
    @echo EXPORTS                                                   >>$@
.IF "$(COM)"=="ICC"
    @echo    GraphicImport                                         >>$@
.ELSE
    @echo    _GraphicImport                                         >>$@
.ENDIF
.ELSE
    @echo option DESCRIPTION 'StarView Filter DLL'                   >$@
    @echo name $(BIN)$/$(SHL1TARGET)                          >>$@
    @echo    GraphicImport_  >>temp.def
    @gawk -f s:\util\exp.awk temp.def                               >>$@
    @del temp.def
.ENDIF
.ENDIF

.IF "$(GUI)"=="WNT"

$(MISC)$/$(SHL1TARGET).def: makefile.mk $(MISC)$/$(SHL1TARGET).flt
    @echo -------------------------------------------
    @echo DEF-File erstellen
        @echo LIBRARY     $(DLLNAME)                    >$@
        @echo DESCRIPTION 'Svx RTF DLL'                 >>$@
        @echo DATA                READ WRITE NONSHARED  >>$@
        @echo EXPORTS                                   >>$@
        @echo     GraphicImport                         >>$@

.ENDIF

$(MISC)$/$(SHL1TARGET).flt:
    @echo ------------------------------
    @echo Making: $@
    @echo WEP>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@


.ENDIF

.INCLUDE :  target.mk

.ELSE
dummy:
    @+echo No VCL - do nothing!
.ENDIF #VCL
