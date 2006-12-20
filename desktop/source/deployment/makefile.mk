#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: ihi $ $Date: 2006-12-20 14:21:24 $
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

PRJ = ..$/..

PRJNAME = desktop
TARGET = deployment
ENABLE_EXCEPTIONS = TRUE
#USE_DEFFILE = TRUE
NO_BSYMBOLIC = TRUE

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/source$/deployment$/inc$/dp_misc.mk

INCPRE += inc

DLLPRE =

SHL1TARGET = $(TARGET)$(UPD)$(DLLPOSTFIX).uno
SHL1VERSIONMAP = $(TARGET).map

SHL1LIBS = \
    $(SLB)$/deployment_manager.lib \
    $(SLB)$/deployment_migration.lib \
    $(SLB)$/deployment_registry.lib \
    $(SLB)$/deployment_registry_component.lib \
    $(SLB)$/deployment_registry_configuration.lib \
    $(SLB)$/deployment_registry_package.lib \
    $(SLB)$/deployment_registry_script.lib \
    $(SLB)$/deployment_registry_sfwk.lib
SHL1OBJS = \
    $(SLO)$/dp_log.obj \
    $(SLO)$/dp_persmap.obj \
    $(SLO)$/dp_services.obj \
    $(SLO)$/dp_xml.obj

SHL1STDLIBS = \
        $(SALLIB) \
        $(CPPULIB) \
        $(CPPUHELPERLIB) \
        $(UCBHELPERLIB) \
        $(COMPHELPERLIB) \
        $(TOOLSLIB) \
        $(XMLSCRIPTLIB) \
        $(SVLLIB) \
        $(DEPLOYMENTMISCLIB)

SHL1DEPN =
SHL1IMPLIB = i$(TARGET)
SHL1DEF = $(MISC)$/$(SHL1TARGET).def

DEF1NAME = $(SHL1TARGET)

SLOFILES = $(LIB1OBJFILES)

RESLIB1NAME = $(TARGET)

RESLIB1SRSFILES = \
        $(SRS)$/deployment_registry_configuration.srs \
        $(SRS)$/deployment_registry_component.srs \
        $(SRS)$/deployment_registry_script.srs \
        $(SRS)$/deployment_registry_sfwk.srs \
        $(SRS)$/deployment_registry_package.srs \
        $(SRS)$/deployment_registry.srs \
        $(SRS)$/deployment_manager.srs \
        $(SRS)$/deployment_misc.srs \
    $(SRS)$/deployment_unopkg.srs

.INCLUDE : target.mk

