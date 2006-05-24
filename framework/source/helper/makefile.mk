#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.29 $
#
#   last change: $Author: vg $ $Date: 2006-05-24 13:39:09 $
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
TARGET=				fwk_helper
USE_DEFFILE=		TRUE
ENABLE_EXCEPTIONS=	TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  		settings.mk

# --- defines ------------------------------------------------------

CDEFS+=-DCOMPMOD_NAMESPACE=framework

# --- Generate -----------------------------------------------------

SLOFILES=			$(SLO)$/timerhelper.obj	 						\
                    $(SLO)$/ocomponentaccess.obj					\
                    $(SLO)$/ocomponentenumeration.obj				\
                    $(SLO)$/oframes.obj								\
                    $(SLO)$/statusindicatorfactory.obj				\
                    $(SLO)$/statusindicator.obj						\
                    $(SLO)$/imageproducer.obj						\
                    $(SLO)$/xmldocproperties.obj					\
                    $(SLO)$/propertysetcontainer.obj				\
                    $(SLO)$/actiontriggerhelper.obj					\
                    $(SLO)$/persistentwindowstate.obj				\
                    $(SLO)$/networkdomain.obj						\
                    $(SLO)$/acceleratorinfo.obj                     \
                    $(SLO)$/uielementwrapperbase.obj                \
                    $(SLO)$/popupmenucontrollerbase.obj             \
                    $(SLO)$/dockingareadefaultacceptor.obj          \
                    $(SLO)$/uiconfigelementwrapperbase.obj          \
                    $(SLO)$/moduleidentifier.obj                    \
                    $(SLO)$/shareablemutex.obj                      \
                    $(SLO)$/vclstatusindicator.obj                  \
                    $(SLO)$/wakeupthread.obj                        \
                    $(SLO)$/configimporter.obj

# --- Targets ------------------------------------------------------

.INCLUDE :			target.mk
