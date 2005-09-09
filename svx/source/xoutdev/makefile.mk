#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 01:18:26 $
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

PRJNAME=svx
PROJECTPCH=xout
PROJECTPCHSOURCE=xoutpch
TARGET=xout
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES= \
        $(SLO)$/xcpoly.obj 		\
        $(SLO)$/xout.obj   		\
        $(SLO)$/xout1.obj  		\
        $(SLO)$/xattr.obj  		\
        $(SLO)$/xattr2.obj  	\
        $(SLO)$/xattrbmp.obj	\
        $(SLO)$/xline.obj   	\
        $(SLO)$/xpool.obj  		\
        $(SLO)$/xtable.obj 		\
        $(SLO)$/xtabcolr.obj	\
        $(SLO)$/xtablend.obj	\
        $(SLO)$/xtabdash.obj	\
        $(SLO)$/xtabhtch.obj	\
        $(SLO)$/xtabgrdt.obj	\
        $(SLO)$/xtabbtmp.obj	\
        $(SLO)$/xexch.obj		\
        $(SLO)$/xiocomp.obj		\
        $(SLO)$/_xpoly.obj  	\
        $(SLO)$/_ximp.obj   	\
        $(SLO)$/_xoutbmp.obj	\
        $(SLO)$/_xfont.obj

.INCLUDE :  target.mk
