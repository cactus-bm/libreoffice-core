#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 04:17:38 $
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
PRJINC=$(PRJ)$/source

PRJNAME=configmgr
TARGET=cm
ENABLE_EXCEPTIONS=TRUE


# --- Settings ---
.IF "$(DBGUTIL_OJ)"!=""
ENVCFLAGS+=/FR$(SLO)$/
.ENDIF

.INCLUDE : settings.mk

# --- Files ---

SLOFILES=\
    $(SLO)$/builddata.obj		\
    $(SLO)$/node.obj			\
    $(SLO)$/treefragment.obj	\
    $(SLO)$/treesegment.obj		\
    $(SLO)$/nodevisitor.obj		\
    $(SLO)$/nodeaccess.obj		\
    $(SLO)$/treeaccessor.obj	\
    $(SLO)$/valuenodeaccess.obj	\
    $(SLO)$/groupnodeaccess.obj	\
    $(SLO)$/setnodeaccess.obj	\
    $(SLO)$/changes.obj			\
    $(SLO)$/treenodefactory.obj			\
    $(SLO)$/treechangefactory.obj		\
    $(SLO)$/localizedtreeactions.obj	\
    $(SLO)$/treeactions.obj		\
    $(SLO)$/cmtreemodel.obj		\
    $(SLO)$/cmtree.obj			\
    $(SLO)$/nodeconverter.obj	\
    $(SLO)$/updatehelper.obj	\
    $(SLO)$/mergehelper.obj		\

# --- Targets ---

.INCLUDE : target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk

