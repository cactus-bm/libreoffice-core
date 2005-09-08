#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.15 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 20:23:09 $
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

PRJNAME=sc
TARGET=app

PROJECTPCH4DLL=TRUE
PROJECTPCH=ui_pch
PDBTARGET=ui_pch
PROJECTPCHSOURCE=..$/pch$/ui_pch

AUTOSEG=true

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

#OBJFILES = \
#	$(OBJ)$/sclib.obj  \

SLOFILES =  \
    $(SLO)$/scmod.obj  \
    $(SLO)$/scmod2.obj  \
    $(SLO)$/scdll.obj  \
    $(SLO)$/typemap.obj  \
    $(SLO)$/transobj.obj \
    $(SLO)$/drwtrans.obj \
    $(SLO)$/lnktrans.obj \
    $(SLO)$/seltrans.obj \
    $(SLO)$/inputhdl.obj \
    $(SLO)$/inputwin.obj \
    $(SLO)$/rfindlst.obj \
    $(SLO)$/uiitems.obj  \
    $(SLO)$/msgpool.obj \
    $(SLO)$/client.obj

EXCEPTIONSFILES= \
    $(SLO)$/drwtrans.obj \
    $(SLO)$/scmod2.obj \
    $(SLO)$/scmod.obj \
    $(SLO)$/client.obj

#LIB3TARGET=$(SLB)$/ysclib.lib
#LIB3OBJFILES=$(SLO)$/sclib.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

#$(SLO)$/sclib.obj :	$(INCCOM)$/scdll0.hxx
#$(OBJ)$/sclib.obj :	$(INCCOM)$/scdll0.hxx

$(INCCOM)$/scdll0.hxx: makefile.mk
.IF "$(GUI)"=="UNX"
    echo \#define DLL_NAME \"libsc$(UPD)$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE			# "$(GUI)"=="UNX"
.IF "$(USE_SHELL)"!="4nt"
    echo \#define DLL_NAME \"sc$(UPD)$(DLLPOSTFIX).DLL\" >$@
.ELSE			#  "$(USE_SHELL)"!="4nt"
    echo #define DLL_NAME "sc$(UPD)$(DLLPOSTFIX).DLL" >$@
.ENDIF			#  "$(USE_SHELL)"!="4nt"
.ENDIF			# "$(GUI)"=="UNX"

$(SRS)$/app.srs: $(SOLARINCDIR)$/svx$/globlmn.hrc

