#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: jbu $ $Date: 2002-10-01 09:26:34 $
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
TARGET=icgm
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE : svpre.mk
.INCLUDE : settings.mk
.INCLUDE : sv.mk

# --- Uno III Header -----------------------------------------------

# Every source directory generates headers in a own output directory to
# enable parallel building of the source directories within a project!
UNOUCROUT=$(OUT)$/inc$/$(PRJNAME)$/$(TARGET)
INCPRE+=$(UNOUCROUT)

UNOTYPES=\
        com.sun.star.uno.TypeClass \
        com.sun.star.uno.XInterface \
        com.sun.star.lang.XMultiServiceFactory\
        com.sun.star.drawing.XMasterPageTarget\
        com.sun.star.drawing.FillStyle\
        com.sun.star.drawing.XMasterPageTarget\
        com.sun.star.drawing.PointSequence\
        com.sun.star.drawing.CircleKind\
        com.sun.star.drawing.PointSequenceSequence\
        com.sun.star.drawing.LineStyle\
        com.sun.star.drawing.LineDash\
        com.sun.star.drawing.Hatch\
        com.sun.star.drawing.MeasureTextVertPos\
        com.sun.star.drawing.XMasterPagesSupplier\
        com.sun.star.drawing.XDrawPage\
        com.sun.star.drawing.XDrawPages\
        com.sun.star.drawing.XDrawPagesSupplier\
        com.sun.star.drawing.XShapeGrouper\
        com.sun.star.drawing.PolyPolygonBezierCoords\
        com.sun.star.drawing.FlagSequence\
        com.sun.star.drawing.TextAdjust\
        com.sun.star.frame.XModel\
        com.sun.star.task.XStatusIndicator\
        com.sun.star.beans.XPropertySet\
        com.sun.star.awt.Gradient\
        com.sun.star.awt.FontDescriptor\
        com.sun.star.awt.FontSlant\
        com.sun.star.awt.FontWeight\
        com.sun.star.awt.FontUnderline\
        com.sun.star.awt.XBitmap\
        com.sun.star.awt.XPointer\
        com.sun.star.text.XText\
        com.sun.star.style.HorizontalAlignment



# --- Files --------------------------------------------------------

CXXFILES=	cgm.cxx 			\
            chart.cxx			\
            class0.cxx			\
            class1.cxx			\
            class2.cxx			\
            class3.cxx			\
            class4.cxx			\
            class5.cxx			\
            class7.cxx			\
            classx.cxx			\
            outact.cxx			\
            actmeta.cxx 		\
            actimpr.cxx 		\
            bundles.cxx 		\
            bitmap.cxx			\
            elements.cxx
#			svdem.cxx

SLOFILES =	$(SLO)$/cgm.obj		\
            $(SLO)$/chart.obj	\
            $(SLO)$/class0.obj	\
            $(SLO)$/class1.obj	\
            $(SLO)$/class2.obj	\
            $(SLO)$/class3.obj	\
            $(SLO)$/class4.obj	\
            $(SLO)$/class5.obj	\
            $(SLO)$/class7.obj	\
            $(SLO)$/classx.obj	\
            $(SLO)$/outact.obj	\
            $(SLO)$/actmeta.obj	\
            $(SLO)$/actimpr.obj	\
            $(SLO)$/bundles.obj	\
            $(SLO)$/bitmap.obj	\
            $(SLO)$/elements.obj
#			$(SLO)$/svdem.obj

SHL1TARGET	=	icg$(UPD)$(DLLPOSTFIX)
SHL1IMPLIB	=	icgm
SHL1DEPN	=	$(LB)$/icgm.lib
SHL1DEF		=	$(MISC)$/$(SHL1TARGET).def
SHL1DEPN	=	$(LB)$/icgm.lib
SHL1LIBS	=	$(SLB)$/icgm.lib
SHL1BASE	=	0x1c000000

.IF "$(GUI)"=="OS2"
SHL1OBJS	=	$(SLO)$/class0.obj
.ENDIF

SHL1STDLIBS =	\
                $(SVTOOLLIB)		\
                $(TKLIB)			\
                $(SVLIB)			\
                $(SOTLIB)			\
                $(UNOTOOLSLIB)		\
                $(TOOLSLIB) 		\
                $(CPPULIB)			\
                $(VOSLIB)			\
                $(SALLIB)
#				ims_ifac.lib

# --- Targets --------------------------------------------------------------

.INCLUDE : target.mk

# -------------------------------------------------------------------------

$(MISC)$/$(SHL1TARGET).flt:
    @echo ------------------------------
    @echo Making: $@
    @echo WEP>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@

# -------------------------------------------------------------------------

# THB: exports list goodies checked for 6.0 Final 6.12.2001
.IF "$(GUI)"=="MAC"

$(MISC)$/$(SHL1TARGET).def:  $(MISC)$/$(SHL1TARGET).flt makefile.mk
    @echo	ImportCGM												> $@
.ENDIF

# ------------------

.IF "$(GUI)"=="WIN"

$(MISC)$/$(SHL1TARGET).def:  $(MISC)$/$(SHL1TARGET).flt makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo LIBRARY     $(SHL1TARGET)                                  >$@
    @echo DESCRIPTION 'Filter DLL'                                  >>$@
    @echo EXETYPE	  WINDOWS                                       >>$@
    @echo PROTMODE                                                  >>$@
    @echo CODE        LOADONCALL MOVEABLE DISCARDABLE               >>$@
    @echo DATA        PRELOAD MOVEABLE SINGLE                       >>$@
    @echo HEAPSIZE    0                                             >>$@
    @echo EXPORTS                                                   >>$@
    @echo	_ImportCGM												>>$@
.ENDIF

# ------------------

.IF "$(GUI)" == "OS2"

$(MISC)$/$(SHL1TARGET).def:\
    makefile.mk \
    $(MISC)$/$(SHL1TARGET).flt
    @echo -------------------------------------------
    @echo DEF-File erstellen
.IF "$(COM)"!="WTC"
    @echo LIBRARY     $(DLLNAME) INITINSTANCE TERMINSTANCE           >$@
    @echo DESCRIPTION 'FILTER DLL'									>>$@
.IF "$(COM)" == "ZTC"
    @echo STUB        'os2STUB.EXE'                                 >>$@
.ENDIF
    @echo PROTMODE                                                  >>$@
    @echo CODE        LOADONCALL                                    >>$@
    @echo DATA        PRELOAD MULTIPLE NONSHARED                    >>$@
    @echo EXPORTS                                                   >>$@
.IF "$(COM)"=="ICC"
    @echo   ImportCGM												>>$@
.ELSE
    @echo   _ImportCGM												>>$@
.ENDIF
.ELSE
    @echo option DESCRIPTION 'Filter DLL'							>$@
    @echo name $(BIN)$/$(SHL1TARGET)                          		>>$@
    @echo	ImportCGM_												>>$@
    @gawk -f s:\util\exp.awk temp.def								>>$@
    @del temp.def
.ENDIF
.ENDIF

# ------------------

.IF "$(GUI)"=="WNT"

$(MISC)$/$(SHL1TARGET).def: makefile.mk $(MISC)$/$(SHL1TARGET).flt
    @echo -------------------------------------------
    @echo DEF-File erstellen
    @echo LIBRARY     $(DLLNAME)									>$@
    @echo DESCRIPTION 'Filter DLL'									>>$@
    @echo DATA READ WRITE NONSHARED									>>$@
    @echo EXPORTS													>>$@
    @echo ImportCGM													>>$@
.ENDIF
