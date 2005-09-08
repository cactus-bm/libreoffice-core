#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.12 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 18:26:58 $
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

PRJ=..$/..$/..

PRJNAME=dtrans
TARGET=dtobjfact
ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# ------------------------------------------------------------------

.INCLUDE : ..$/..$/cppumaker.mk
#-DUNICODE -D_UNICODE
CFLAGS+=-GR -Ob0
UNOTYPES= \
    com.sun.star.datatransfer.XSystemTransferable
    

SLOFILES=$(SLO)$/DtObjFactory.obj\
         $(SLO)$/APNDataObject.obj\
         $(SLO)$/DOTransferable.obj\
         $(SLO)$/DTransHelper.obj\
         $(SLO)$/XTDataObject.obj\
         $(SLO)$/TxtCnvtHlp.obj\
         $(SLO)$/DataFmtTransl.obj\
         $(SLO)$/FmtFilter.obj\
         $(SLO)$/FetcList.obj\
         $(SLO)$/Fetc.obj\
         $(SLO)$/XNotifyingDataObject.obj

LIB1TARGET=$(SLB)$/$(TARGET).lib
LIB1OBJFILES=$(SLOFILES)

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk
