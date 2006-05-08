#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: hr $ $Date: 2006-05-08 15:19:21 $
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
TARGET=				fwk_uielement
USE_DEFFILE=		TRUE
NO_BSYMBOLIC=		TRUE
ENABLE_EXCEPTIONS=	TRUE
BOOTSTRAP_SERVICE=	FALSE

# --- Settings -----------------------------------------------------

.INCLUDE :  		svpre.mk
.INCLUDE :  		settings.mk
.INCLUDE :  		sv.mk

# --- Generate -----------------------------------------------------

SLOFILES=			\
                    $(SLO)$/menubarwrapper.obj              \
                    $(SLO)$/menubarmanager.obj              \
                    $(SLO)$/fontmenucontroller.obj          \
                    $(SLO)$/fontsizemenucontroller.obj      \
                    $(SLO)$/objectmenucontroller.obj        \
                    $(SLO)$/headermenucontroller.obj        \
                    $(SLO)$/footermenucontroller.obj        \
                    $(SLO)$/controlmenucontroller.obj       \
                    $(SLO)$/macrosmenucontroller.obj   		\
                    $(SLO)$/uicommanddescription.obj        \
                    $(SLO)$/itemcontainer.obj               \
                    $(SLO)$/constitemcontainer.obj          \
                    $(SLO)$/rootitemcontainer.obj           \
                    $(SLO)$/toolbarwrapper.obj              \
                    $(SLO)$/toolbar.obj                     \
                    $(SLO)$/toolbarmanager.obj              \
                    $(SLO)$/addonstoolbarwrapper.obj        \
                    $(SLO)$/addonstoolbarmanager.obj        \
                    $(SLO)$/generictoolbarcontroller.obj    \
                    $(SLO)$/toolbarsmenucontroller.obj      \
                    $(SLO)$/statusbarwrapper.obj            \
                    $(SLO)$/statusbar.obj                   \
                    $(SLO)$/statusbarmanager.obj			\
                    $(SLO)$/recentfilesmenucontroller.obj   \
                    $(SLO)$/progressbarwrapper.obj          \
                    $(SLO)$/statusindicatorinterfacewrapper.obj \
                    $(SLO)$/logoimagestatusbarcontroller.obj \
                    $(SLO)$/logotextstatusbarcontroller.obj \
                    $(SLO)$/newmenucontroller.obj           \
                    $(SLO)$/simpletextstatusbarcontroller.obj\
                    $(SLO)$/complextoolbarcontroller.obj     \
                    $(SLO)$/comboboxtoolbarcontroller.obj    \
                    $(SLO)$/imagebuttontoolbarcontroller.obj \
                    $(SLO)$/togglebuttontoolbarcontroller.obj \
                    $(SLO)$/buttontoolbarcontroller.obj

# --- Targets ------------------------------------------------------

.INCLUDE :			target.mk
