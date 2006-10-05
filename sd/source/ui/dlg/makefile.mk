#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.31 $
#
#   last change: $Author: kz $ $Date: 2006-10-05 10:53:38 $
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

PROJECTPCH=sd
PROJECTPCHSOURCE=$(PRJ)$/util$/sd
PRJNAME=sd
TARGET=dlg
LIBTARGET=NO

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =\
    masterlayoutdlg.src\
    ins_page.src\
    headerfooterdlg.src\
    ins_paste.src\
    inspagob.src\
    present.src\
    dlg_char.src\
    paragr.src\
    new_foil.src\
    tabtempl.src\
    dlgpage.src\
    tpaction.src\
    animobjs.src\
    prltempl.src\
    prntopts.src\
    dlgsnap.src\
    copydlg.src\
    tpoption.src\
    dlgass.src\
    navigatr.src\
    sdpreslt.src\
    morphdlg.src\
    printdlg.src\
    dlgfield.src\
    custsdlg.src\
    brkdlg.src\
    vectdlg.src\
    dlgolbul.src \
    LayerDialog.src			\
    PaneDockingWindow.src


SLOFILES =  \
        $(SLO)$/inspage.obj \
        $(SLO)$/inspagob.obj \
        $(SLO)$/ins_paste.obj \
        $(SLO)$/present.obj  \
        $(SLO)$/dlgctrls.obj \
        $(SLO)$/dlgchar.obj \
        $(SLO)$/paragr.obj \
        $(SLO)$/newfoil.obj \
        $(SLO)$/tabtempl.obj \
        $(SLO)$/dlgpage.obj  \
        $(SLO)$/diactrl.obj  \
        $(SLO)$/gluectrl.obj  \
        $(SLO)$/sdtreelb.obj \
        $(SLO)$/animobjs.obj \
        $(SLO)$/prltempl.obj \
        $(SLO)$/prntopts.obj \
        $(SLO)$/dlgsnap.obj  \
        $(SLO)$/copydlg.obj  \
        $(SLO)$/dlgolbul.obj \
        $(SLO)$/tpaction.obj \
        $(SLO)$/tpoption.obj \
        $(SLO)$/filedlg.obj   \
        $(SLO)$/unchss.obj  \
        $(SLO)$/dlgassim.obj	\
        $(SLO)$/dlgass.obj  \
        $(SLO)$/assclass.obj \
        $(SLO)$/graphpro.obj \
        $(SLO)$/navigatr.obj \
        $(SLO)$/sdpreslt.obj \
        $(SLO)$/morphdlg.obj	\
        $(SLO)$/printdlg.obj	\
        $(SLO)$/dlgfield.obj	\
        $(SLO)$/custsdlg.obj	\
        $(SLO)$/brkdlg.obj      \
        $(SLO)$/vectdlg.obj		\
        $(SLO)$/docprev.obj     \
        $(SLO)$/SpellDialogChildWindow.obj \
        $(SLO)$/AnimationChildWindow.obj	\
        $(SLO)$/LayerDialogChildWindow.obj	\
        $(SLO)$/LayerDialogContent.obj		\
        $(SLO)$/LayerTabBar.obj				\
        $(SLO)$/NavigatorChildWindow.obj	\
        $(SLO)$/TemplateScanner.obj \
                $(SLO)$/sduiexp.obj \
        $(SLO)$/sddlgfact.obj \
        $(SLO)$/sdabstdlg.obj \
        $(SLO)$/PaneChildWindows.obj			\
        $(SLO)$/PaneDockingWindow.obj

EXCEPTIONSFILES= \
        $(SLO)$/tpaction.obj \
        $(SLO)$/filedlg.obj   	    \
        $(SLO)$/dlgass.obj		    \
        $(SLO)$/TemplateScanner.obj \
        $(SLO)$/sdtreelb.obj        \
        $(SLO)$/tpoption.obj

LIB1TARGET= $(SLB)$/$(TARGET).lib

LIB1OBJFILES= \
        $(SLO)$/dlgctrls.obj \
        $(SLO)$/diactrl.obj  \
        $(SLO)$/gluectrl.obj  \
        $(SLO)$/sdtreelb.obj \
        $(SLO)$/animobjs.obj \
        $(SLO)$/filedlg.obj   \
        $(SLO)$/unchss.obj  \
        $(SLO)$/assclass.obj \
        $(SLO)$/graphpro.obj \
        $(SLO)$/navigatr.obj \
        $(SLO)$/docprev.obj     \
        $(SLO)$/ins_paste.obj \
        $(SLO)$/AnimationChildWindow.obj	\
        $(SLO)$/LayerDialogChildWindow.obj	\
        $(SLO)$/LayerDialogContent.obj		\
        $(SLO)$/LayerTabBar.obj				\
        $(SLO)$/NavigatorChildWindow.obj	\
        $(SLO)$/SpellDialogChildWindow.obj \
        $(SLO)$/TemplateScanner.obj \
        $(SLO)$/sdabstdlg.obj \
        $(SLO)$/PaneChildWindows.obj		\
        $(SLO)$/PaneDockingWindow.obj


LIB2TARGET= $(SLB)$/sdui.lib

LIB2OBJFILES= \
        $(SLO)$/masterlayoutdlg.obj\
        $(SLO)$/headerfooterdlg.obj\
        $(SLO)$/sduiexp.obj \
        $(SLO)$/sddlgfact.obj \
        $(SLO)$/brkdlg.obj \
        $(SLO)$/copydlg.obj \
        $(SLO)$/custsdlg.obj \
        $(SLO)$/dlgchar.obj \
        $(SLO)$/dlgass.obj \
        $(SLO)$/dlgassim.obj \
        $(SLO)$/dlgfield.obj \
        $(SLO)$/dlgpage.obj \
        $(SLO)$/dlgsnap.obj \
        $(SLO)$/inspage.obj \
        $(SLO)$/ins_paste.obj \
        $(SLO)$/inspagob.obj \
        $(SLO)$/morphdlg.obj \
        $(SLO)$/newfoil.obj \
        $(SLO)$/dlgolbul.obj \
        $(SLO)$/paragr.obj \
        $(SLO)$/present.obj \
        $(SLO)$/printdlg.obj \
        $(SLO)$/prltempl.obj \
        $(SLO)$/sdpreslt.obj \
        $(SLO)$/tabtempl.obj \
        $(SLO)$/tpaction.obj \
        $(SLO)$/tpoption.obj \
        $(SLO)$/vectdlg.obj \
        $(SLO)$/prntopts.obj 

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

$(INCCOM)$/sduilib.hxx: makefile.mk
.IF "$(GUI)"=="UNX"
    $(RM) $@
    echo \#define DLL_NAME \"libsdui$(UPD)$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE
    echo $(EMQ)#define DLL_NAME $(EMQ)"sdui$(UPD)$(DLLPOSTFIX)$(DLLPOST)$(EMQ)" >$@
.ENDIF

$(SLO)$/sdabstdlg.obj : $(INCCOM)$/sduilib.hxx

