#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 00:12:39 $
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

PRJ=				..$/..$/..
PRJINC=				$(PRJ)$/source
PRJNAME=			chart2
TARGET=				chcdialogs

ENABLE_EXCEPTIONS=	TRUE
#GEN_HID=           TRUE

# --- Settings -----------------------------------------------------

.INCLUDE: settings.mk

# i26518 the gcc-3.0.4 requires to enhance the template-depth
# this seems to be a compiler issue, so we recommend not to use 3.0.x anymore
.IF "$(COM)"=="GCC" 
    CFLAGS+=-ftemplate-depth-128
.ENDIF

# --- export library -------------------------------------------------

#object files to build and link together to lib $(SLB)$/$(TARGET).lib
SLOFILES=   	$(SLO)$/tp_AxisLabel.obj \
                $(SLO)$/tp_DataLabel.obj \
                $(SLO)$/tp_LegendPosition.obj \
                $(SLO)$/tp_PointGeometry.obj \
                $(SLO)$/tp_Scale.obj \
                $(SLO)$/tp_SeriesStatistic.obj \
                $(SLO)$/tp_SeriesToAxis.obj \
                $(SLO)$/tp_TitleRotation.obj \
                $(SLO)$/dlg_ObjectProperties.obj \
                $(SLO)$/dlg_ChartType.obj \
                $(SLO)$/dlg_RotateDiagram.obj \
                $(SLO)$/dlg_InsertAxis_Grid.obj \
                $(SLO)$/dlg_InsertDataLabel.obj \
                $(SLO)$/dlg_InsertLegend.obj \
                $(SLO)$/dlg_InsertStatistic.obj \
                $(SLO)$/dlg_InsertTitle.obj

# --- Resources ---------------------------------------------------------------

#               Bitmaps.src
#               Bitmaps_HC.src


SRC1FILES=		\
                tp_AxisLabel.src \
                tp_DataLabel.src \
                tp_LegendPosition.src \
                tp_PointGeometry.src \
                tp_Scale.src \
                tp_SeriesStatistic.src \
                tp_SeriesToAxis.src \
                tp_TitleRotation.src \
                dlg_ObjectProperties.src \
                Strings_Statistic.src \
                Strings_Scale.src \
                Strings_ChartTypes.src \
                Strings.src \
                dlg_ChartType.src \
                Bitmaps.src \
                Bitmaps_HC.src \
                dlg_RotateDiagram.src \
                dlg_InsertAxis_Grid.src \
                dlg_InsertDataLabel.src \
                dlg_InsertLegend.src \
                dlg_InsertStatistic.src \
                dlg_InsertTitle.src

SRS1NAME=$(TARGET)

# --- Targets -----------------------------------------------------------------
LOCALIZE_ME=res_DataLabel_tmpl.hrc res_LegendPosition_tmpl.hrc res_Statistic_tmpl.hrc 
.INCLUDE: target.mk
$(SRS)$/chcdialogs.srs: $(INCCOM)$/res_DataLabel.hrc
$(SRS)$/chcdialogs.srs: $(INCCOM)$/res_LegendPosition.hrc
$(SRS)$/chcdialogs.srs: $(INCCOM)$/res_Statistic.hrc

