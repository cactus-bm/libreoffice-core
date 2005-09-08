#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.27 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 23:49:59 $
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
PRJ=..$/..

PROJECTPCH4DLL=TRUE
PROJECTPCH=svxpch
PROJECTPCHSOURCE=$(PRJ)$/util\svxpch

PRJNAME=svx
TARGET=options

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

CFLAGS+=-DDG_DLL

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES=\
    optasian.src		\
    optcolor.src		\
    optjsearch.src		\
    optgenrl.src		\
    optdict.src		\
    optaccessibility.src	\
    optsave.src		\
    optpath.src		\
    optlingu.src		\
    optgrid.src		\
    optinet2.src		\
    multipat.src		\
    optctl.src

#EXCEPTIONSFILES=\
#	$(SLO)$/optasian.obj		\
#	$(SLO)$/optlingu.obj		\
#	$(SLO)$/optaccessibility.obj	\
#	$(SLO)$/optsave.obj		\
#	$(SLO)$/optpath.obj

#SLOFILES=\
#	$(EXCEPTIONSFILES)	\
#	$(SLO)$/asiancfg.obj	\
#	$(SLO)$/htmlcfg.obj	\
#	$(SLO)$/optcolor.obj    \
#	$(SLO)$/optjsearch.obj  \
#	$(SLO)$/optitems.obj	\
#	$(SLO)$/optgenrl.obj	\
#	$(SLO)$/adritem.obj	\
#	$(SLO)$/optgrid.obj	\
#	$(SLO)$/optinet2.obj	\
#	$(SLO)$/optextbr.obj    \
#	$(SLO)$/srchcfg.obj	\
#	$(SLO)$/optctl.obj

EXCEPTIONSFILES=\
    $(SLO)$/optlingu.obj

SLOFILES=\
    $(EXCEPTIONSFILES)	\
    $(SLO)$/asiancfg.obj    \
    $(SLO)$/htmlcfg.obj \
    $(SLO)$/optitems.obj    \
    $(SLO)$/optgrid.obj \
    $(SLO)$/srchcfg.obj
# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

