#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.12 $
#
#   last change: $Author: oj $ $Date: 2002-09-27 11:21:40 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

PRJ=..$/..

PRJNAME=svx
TARGET=form

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

IMGLST_SRS=$(SRS)$/form.srs
BMP_IN=$(PRJ)$/win$/res

SRS1NAME=form
SRC1FILES= \
    fmexpl.src 		\
    filtnav.src		\
    taborder.src 	\
    fmstring.src 	\
    tbxform.src		\
    fmsearch.src


SLOFILES = \
    $(SLO)$/dbcharsethelper.obj			\
    $(SLO)$/ParseContext.obj			\
    $(SLO)$/typeconversionclient.obj    \
    $(SLO)$/confirmdelete.obj      \
    $(SLO)$/dbtoolsclient.obj      \
    $(SLO)$/sqlparserclient.obj      \
    $(SLO)$/dataaccessdescriptor.obj      \
    $(SLO)$/fmpage.obj      \
    $(SLO)$/fmPropBrw.obj			\
    $(SLO)$/fmundo.obj  	\
    $(SLO)$/fmprop.obj      \
    $(SLO)$/fmmodel.obj		\
    $(SLO)$/navigatortree.obj      \
    $(SLO)$/navigatortreemodel.obj      \
    $(SLO)$/fmexpl.obj      \
    $(SLO)$/fmctrler.obj    \
    $(SLO)$/fmpgeimp.obj	\
    $(SLO)$/fmvwimp.obj     \
    $(SLO)$/taborder.obj    \
    $(SLO)$/fmdpage.obj		\
    $(SLO)$/fmitems.obj  	\
    $(SLO)$/tbxform.obj     \
    $(SLO)$/fmobj.obj      	\
    $(SLO)$/fmdmod.obj		\
    $(SLO)$/fmobjfac.obj	\
    $(SLO)$/fmsrccfg.obj	\
    $(SLO)$/fmservs.obj     \
    $(SLO)$/typemap.obj		\
    $(SLO)$/fmexch.obj      \
    $(SLO)$/tabwin.obj      \
    $(SLO)$/multipro.obj	\
    $(SLO)$/fmurl.obj		\
    $(SLO)$/fmfilter.obj	\
    $(SLO)$/filtnav.obj		\
    $(SLO)$/fmimplids.obj   \
    $(SLO)$/fmtools.obj     \
    $(SLO)$/fmshimp.obj     \
    $(SLO)$/fmshell.obj     \
    $(SLO)$/fmsearch.obj	\
    $(SLO)$/fmsrcimp.obj	\
    $(SLO)$/fmview.obj		\
    $(SLO)$/siimport.obj

.IF "$(OS)$(CPU)"=="SOLARISI" 
NOOPTFILES=$(SLO)$/fmprop.obj
.ENDIF

SVXLIGHTOBJFILES= \
    $(OBJ)$/fmpage.obj      \
    $(OBJ)$/fmmodel.obj

.INCLUDE :  target.mk
