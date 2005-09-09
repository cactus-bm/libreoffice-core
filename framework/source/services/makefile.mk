#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.21 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 01:44:38 $
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

PRJNAME=			framework
TARGET=				fwk_services
USE_DEFFILE=		TRUE
NO_BSYMBOLIC=		TRUE
ENABLE_EXCEPTIONS=	TRUE
BOOTSTRAP_SERVICE=	FALSE

# --- Settings -----------------------------------------------------

.INCLUDE :  		settings.mk

# --- Generate -----------------------------------------------------

SLOFILES=			\
                    $(SLO)$/documentlist.obj                        \
                    $(SLO)$/desktop.obj								\
                    $(SLO)$/frame.obj								\
                    $(SLO)$/urltransformer.obj						\
                    $(SLO)$/mediatypedetectionhelper.obj			\
                    $(SLO)$/documentproperties.obj					\
                    $(SLO)$/substitutepathvars.obj					\
                    $(SLO)$/pathsettings.obj                        \
                    $(SLO)$/backingcomp.obj							\
                    $(SLO)$/dispatchhelper.obj                      \
                    $(SLO)$/license.obj                      		\
                    $(SLO)$/modulemanager.obj                       \
                    $(SLO)$/autorecovery.obj                        \
                    $(SLO)$/sessionlistener.obj

SRS1NAME=$(TARGET)
SRC1FILES=          \
                    documentlist.src

RESLIB1NAME=		lgd
RESLIB1SRSFILES= 	$(SRS)$/$(TARGET).srs
RESLIB1DEPN=        documentlist.src \
                    documentlist.hrc


# --- Targets ------------------------------------------------------

.INCLUDE :			target.mk

