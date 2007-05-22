#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: vg $ $Date: 2007-05-22 19:28:05 $
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
TARGET=				chview

ENABLE_EXCEPTIONS=	TRUE

# --- Settings -----------------------------------------------------

.INCLUDE: settings.mk

# --- export library -------------------------------------------------

#object files to build and link together to lib $(SLB)$/$(TARGET).lib
SLOFILES =  \
            $(SLO)$/ChartItemPool.obj \
            $(SLO)$/DrawModelWrapper.obj \
            $(SLO)$/NumberFormatterWrapper.obj \
            $(SLO)$/PropertyMapper.obj \
            $(SLO)$/Stripe.obj \
            $(SLO)$/VLineProperties.obj \
            $(SLO)$/ShapeFactory.obj \
            $(SLO)$/VLegendSymbolFactory.obj \
            $(SLO)$/DataPointSymbolSupplier.obj \
            $(SLO)$/Linear3DTransformation.obj \
            $(SLO)$/VPolarTransformation.obj \
            $(SLO)$/Clipping.obj \
            $(SLO)$/PlottingPositionHelper.obj \
            $(SLO)$/LabelPositionHelper.obj \
            $(SLO)$/PolarLabelPositionHelper.obj \
            $(SLO)$/PlotterBase.obj \
            $(SLO)$/VDataSeries.obj \
            $(SLO)$/VLegend.obj \
            $(SLO)$/VTitle.obj \
            $(SLO)$/ChartView.obj \
            $(SLO)$/_serviceregistration_view.obj

# --- Targets -----------------------------------------------------------------

.INCLUDE: target.mk
