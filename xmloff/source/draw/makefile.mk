#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.15 $
#
#   last change: $Author: thb $ $Date: 2001-07-24 17:06:07 $
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

PRJ=..$/..
PRJNAME=xmloff
TARGET=draw
AUTOSEG=true
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE : svpre.mk
.INCLUDE : settings.mk
.INCLUDE : sv.mk

# --- Files --------------------------------------------------------

SLOFILES =	\
        $(SLO)$/XMLNumberStyles.obj\
        $(SLO)$/XMLGraphicsDefaultStyle.obj\
        $(SLO)$/viewcontext.obj\
        $(SLO)$/eventimp.obj\
        $(SLO)$/layerexp.obj\
        $(SLO)$/layerimp.obj\
        $(SLO)$/ximpshow.obj\
        $(SLO)$/animimp.obj\
        $(SLO)$/animexp.obj\
        $(SLO)$/numithdl.obj\
        $(SLO)$/sdxmlimp.obj \
        $(SLO)$/sdxmlexp.obj \
        $(SLO)$/ximpstyl.obj \
        $(SLO)$/ximpbody.obj \
        $(SLO)$/ximpshap.obj \
        $(SLO)$/ximpgrp.obj \
        $(SLO)$/ximp3dscene.obj \
        $(SLO)$/ximp3dobject.obj \
        $(SLO)$/ximpnote.obj \
        $(SLO)$/sdpropls.obj \
        $(SLO)$/propimp0.obj \
        $(SLO)$/xexptran.obj \
        $(SLO)$/shapeexport.obj \
        $(SLO)$/shapeexport2.obj \
        $(SLO)$/shapeexport3.obj \
        $(SLO)$/shapeimport.obj \
        $(SLO)$/XMLImageMapExport.obj \
        $(SLO)$/XMLImageMapContext.obj \
        $(SLO)$/XMLShapePropertySetContext.obj \
        $(SLO)$/XMLShapeStyleContext.obj \
        $(SLO)$/ximppage.obj

SVXLIGHTOBJFILES =	\
        $(OBJ)$/XMLNumberStyles.obj\
        $(OBJ)$/XMLGraphicsDefaultStyle.obj\
        $(OBJ)$/viewcontext.obj\
        $(OBJ)$/eventimp.obj\
        $(OBJ)$/layerimp.obj\
        $(OBJ)$/ximpshow.obj\
        $(OBJ)$/animimp.obj\
        $(OBJ)$/numithdl.obj\
        $(OBJ)$/sdxmlimp.obj \
        $(OBJ)$/ximpstyl.obj \
        $(OBJ)$/ximpbody.obj \
        $(OBJ)$/ximpshap.obj \
        $(OBJ)$/ximpgrp.obj \
        $(OBJ)$/ximp3dscene.obj \
        $(OBJ)$/ximp3dobject.obj \
        $(OBJ)$/ximpnote.obj \
        $(OBJ)$/sdpropls.obj \
        $(OBJ)$/propimp0.obj \
        $(OBJ)$/xexptran.obj \
        $(OBJ)$/shapeimport.obj \
        $(OBJ)$/XMLImageMapContext.obj \
        $(OBJ)$/XMLShapePropertySetContext.obj \
        $(OBJ)$/XMLShapeStyleContext.obj \
        $(OBJ)$/ximppage.obj

# --- Targets --------------------------------------------------------------

.INCLUDE : target.mk

