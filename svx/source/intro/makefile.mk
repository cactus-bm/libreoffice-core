#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: rt $ $Date: 2005-12-14 15:16:15 $
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
PRJ=..$/..$

PRJNAME=svx
TARGET=intro

# --- Settings ------------------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk


RSCLOCINC!:=$(RSCLOCINC);$(PRJ)$/RES

# setting the compiled by $user$ string in the About box
OOO_VENDOR*="$(USER)$(USERNAME)"
RSCDEFS+=-DOOO_VENDOR=$(OOO_VENDOR)

# --- Allgemein -----------------------------------------------------------

LOCALIZE_ME=intro_tmpl.hrc

SRS1NAME= iso
SRC1FILES=	\
    $(SRS1NAME).src

RESLIB1NAME=$(SRS1NAME)
RESLIB1IMAGES=$(PRJ)$/res
RESLIB1SRSFILES= \
    $(SRS)$/$(SRS1NAME).srs

# Version "OpenOffice.org"
SRS4NAME= ooo
SRC4FILES=	\
    $(SRS4NAME).src

RESLIB4NAME=$(SRS4NAME)
RESLIB4IMAGES=$(PRJ)$/res
RESLIB4SRSFILES= \
    $(SRS)$/$(SRS4NAME).srs

.INCLUDE :  target.mk

$(SRS)$/iso.srs: $(INCCOM)$/intro.hrc
$(SRS)$/ooo.srs: $(INCCOM)$/intro.hrc
