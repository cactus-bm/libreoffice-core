#*************************************************************************
#*
#*  $RCSfile: makefile.mk,v $
#*
#*  $Revision: 1.29 $
#*
#*  last change: $Author: mh $ $Date: 2001-12-06 14:47:37 $
#*
#*  The Contents of this file are made available subject to the terms of
#*  either of the following licenses
#*
#*         - GNU Lesser General Public License Version 2.1
#*         - Sun Industry Standards Source License Version 1.1
#*
#*  Sun Microsystems Inc., October, 2000
#*
#*  GNU Lesser General Public License Version 2.1
#*  =============================================
#*  Copyright 2000 by Sun Microsystems, Inc.
#*  901 San Antonio Road, Palo Alto, CA 94303, USA
#*
#*  This library is free software; you can redistribute it and/or
#*  modify it under the terms of the GNU Lesser General Public
#*  License version 2.1, as published by the Free Software Foundation.
#*
#*  This library is distributed in the hope that it will be useful,
#*  but WITHOUT ANY WARRANTY; without even the implied warranty of
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#*  Lesser General Public License for more details.
#*
#*  You should have received a copy of the GNU Lesser General Public
#*  License along with this library; if not, write to the Free Software
#*  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#*  MA  02111-1307  USA
#*
#*
#*  Sun Industry Standards Source License Version 1.1
#*  =================================================
#*  The contents of this file are subject to the Sun Industry Standards
#*  Source License Version 1.1 (the "License"); You may not use this file
#*  except in compliance with the License. You may obtain a copy of the
#*  License at http://www.openoffice.org/license.html.
#*
#*  Software provided under this License is provided on an "AS IS" basis,
#*  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#*  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#*  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#*  See the License for the specific provisions governing your rights and
#*  obligations concerning the Software.
#*
#*  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#*
#*  Copyright: 2000 by Sun Microsystems, Inc.
#*
#*  All Rights Reserved.
#*
#*  Contributor(s): _______________________________________
#*
#*
#*************************************************************************

PRJ=..

PRJNAME=svtools
TARGET=svtool
TARGETTYPE=GUI
RESTARGET=svt
RESTARGETSIMPLE=svs
VERSION=$(UPD)
GEN_HID=TRUE
ENABLE_EXCEPTIONS=TRUE
# --- Settings -----------------------------------------------------

.INCLUDE :	svpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	sv.mk

USE_LDUMP2=TRUE

# --- general section ----------------------------------------------------

LIB1TARGET= $(LB)$/svmem.lib
LIB1ARCHIV=  $(LB)$/libsvarray.a
LIB1FILES=	$(LB)$/svarray.lib

LIB2TARGET=   $(LB)$/filearch.lib
LIB2OBJFILES= $(OBJ)$/filearch.obj

.IF "$(GUI)"!="UNX"
LIB3TARGET= $(LB)$/svtool.lib
LIB3FILES=	$(LB)$/_svt.lib
LIB3OBJFILES=\
    $(OBJ)$/rtfkey2.obj
.ENDIF

.IF "$(GUI)"!="UNX"
LIB4TARGET= $(LB)$/isvl.lib
LIB4FILES=	$(LB)$/_isvl.lib
LIB4OBJFILES=\
    $(OBJ)$/htmlkey2.obj
.ENDIF

LIB7TARGET= $(SLB)$/svt.lib
LIB7FILES=	\
        $(SLB)$/misc2.lib		\
        $(SLB)$/items2.lib		\
        $(SLB)$/browse.lib		\
        $(SLB)$/ctrl.lib		\
        $(SLB)$/dialogs.lib 	\
        $(SLB)$/edit.lib		\
        $(SLB)$/unoiface.lib	\
        $(SLB)$/filter.lib		\
        $(SLB)$/igif.lib		\
        $(SLB)$/ipng.lib		\
        $(SLB)$/jpeg.lib		\
        $(SLB)$/ixpm.lib		\
        $(SLB)$/ixbm.lib		\
        $(SLB)$/sbx.lib 		\
        $(SLB)$/numbers.lib 	\
        $(SLB)$/wmf.lib 		\
        $(SLB)$/undo.lib		\
        $(SLB)$/urlobj.lib		\
        $(SLB)$/plugapp.lib 	\
        $(SLB)$/svcontnr.lib	\
        $(SLB)$/syslocale.lib   \
        $(SLB)$/svhtml2.lib     \
        $(SLB)$/filepicker.lib


.IF "$(GUI)" == "OS2"
LIB7FILES+= 	$(SLB)$/eaimp.lib
.ENDIF

.IF "$(GUI)" == "UNX"
LIB7FILES+= 	$(SLB)$/eaimp.lib
.ENDIF

LIB7FILES+= \
            $(SLB)$/svdde.lib
.IF "$(GUI)"=="WNT"
LIB7FILES+=$(SOLARLIBDIR)$/user9x.lib $(SOLARLIBDIR)$/tools32.lib
.ENDIF

LIB8TARGET= $(SLB)$/svl.lib
LIB8OBJFILES= \
            $(SLO)$/svtdata.obj \
            $(SLO)$/registerlight.obj

LIB8FILES=	\
        $(SLB)$/config.lib	\
        $(SLB)$/filerec.lib \
        $(SLB)$/items1.lib	\
        $(SLB)$/misc1.lib	\
        $(SLB)$/notify.lib	\
        $(SLB)$/passwordcontainer.lib	\
        $(SLB)$/svarray.lib \
        $(SLB)$/svhtml1.lib \
        $(SLB)$/svrtf.lib	\
        $(SLB)$/svsql.lib	\
        $(SLB)$/cmdparse.lib

# generation of resourcen-lib ----------------------------------------

RESLIB1NAME=	$(RESTARGET)
RESLIB1SRSFILES=$(SRS)$/filter.srs  \
        $(SRS)$/misc.srs        \
        $(SRS)$/sbx.srs         \
        $(SRS)$/ctrl.srs        \
        $(SRS)$/items.srs       \
        $(SRS)$/dialogs.srs     \
        $(SRS)$/plugapp.srs     \
        $(SRS)$/svcontnr.srs    \
        $(SRS)$/filepicker.srs  \
        $(SRS)$/uno.srs         \
        $(SRS)$/browse.srs

RESLIB2NAME=	$(RESTARGETSIMPLE)
RESLIB2SRSFILES=\
    $(SRS)$/mediatyp.srs

# build the shared library  --------------------------------------------------

SHL1TARGET= svt$(VERSION)$(DLLPOSTFIX)
SHL1IMPLIB= _svt
SHL1STDLIBS=$(TOOLSLIB)			\
            $(VOSLIB)			\
            $(CPPULIB)			\
            $(CPPUHELPERLIB)	\
            $(RTLLIB)			\
            $(SVLIB)			\
            $(TKLIB)			\
            $(SVLLIB)			\
            $(SALLIB)			\
            $(UNOTOOLSLIB)		\
            $(COMPHELPERLIB)		\
            $(UCBHELPERLIB)

.IF "$(GUI)"=="WNT"
SHL1STDLIBS+= \
        $(LIBPRE) advapi32.lib	\
        $(LIBPRE) gdi32.lib
.ENDIF

SHL1STDLIBS+= \
            $(SOTLIB) \
            $(JPEG3RDLIB)

SHL1LIBS= \
        $(SLB)$/svt.lib 	\
        $(LB)$/svmem.lib

SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)
DEF1DEPN=	$(MISC)$/$(SHL1TARGET).flt
DEFLIB1NAME =svt
DEF1DES 	=SvTools
DEF1EXPORTFILE=	svt.dxp


# --- svtools lite --------------------------------------------------

SHL2TARGET= svl$(VERSION)$(DLLPOSTFIX)
SHL2IMPLIB= _isvl
SHL1OBJS=$(SLO)$/svtdata.obj
SHL2STDLIBS=$(TOOLSLIB) 	\
        $(VOSLIB)			\
        $(CPPULIB)			\
        $(COMPHELPERLIB)		\
        $(CPPUHELPERLIB)	\
        $(RTLLIB)			\
        $(SALLIB)			\
        $(SALHELPERLIB) \
        $(UNOTOOLSLIB)		\
        $(UCBHELPERLIB)

.IF "$(GUI)"=="WNT"
SHL2STDLIBS+= \
            $(LIBPRE) advapi32.lib	\
            $(LIBPRE) gdi32.lib
.ENDIF

SHL2LIBS=	$(SLB)$/svl.lib

SHL2DEF=	$(MISC)$/$(SHL2TARGET).def

DEF2NAME=	$(SHL2TARGET)
DEF2DEPN=	$(MISC)$/$(SHL2TARGET).flt $(SLB)$/svl.lib
DEFLIB2NAME=svl
DEF2DES =SvTools lite

# --- bmpgui application --------------------------------------------------

APP1TARGET	=	bmpgui
APP1BASE	=	0x10000000
APP1DEPN	=   $(L)$/itools.lib  $(SVLIBDEPEND)
APP1OBJS	=   $(OBJ)$/bmpgui.obj	\
                $(OBJ)$/bmpcore.obj

APP1STDLIBS	=	$(SVLIB)		\
                $(TOOLSLIB)		\
                $(VOSLIB) 		\
                $(SALLIB)

.IF "$(GUI)"!="UNX"
APP1STDLIBS+= svtool.lib
.ELSE
APP1STDLIBS+= -lsvt$(UPD)$(DLLSUFFIX)
APP1STDLIBS+= -lsvl$(UPD)$(DLLSUFFIX)
.ENDIF

# --- g2g application --------------------------------------------------

APP2TARGET	=	g2g
APP2BASE	=	0x10000000
APP2DEPN	=   $(L)$/itools.lib  $(SVLIBDEPEND)
APP2OBJS	=   $(OBJ)$/g2g.obj	

APP2STDLIBS	=	$(SVLIB)		\
                $(TOOLSLIB)		\
                $(VOSLIB) 		\
                $(SALLIB)

.IF "$(GUI)"!="UNX"
APP2STDLIBS+= svtool.lib
.ELSE
APP2STDLIBS+= -lsvt$(UPD)$(DLLSUFFIX)
APP2STDLIBS+= -lsvl$(UPD)$(DLLSUFFIX)
.ENDIF

# --- Targets ------------------------------------------------------

.IF "$(GUI)"=="UNX"
SVTTARGETS= $(LB)$/lib$(SHL2TARGET)$(DLLPOST) $(LB)$/lib$(SHL1TARGET)$(DLLPOST)
.ELSE
SVTTARGETS= $(LB)$/isvl.lib \
            $(BIN)$/$(SHL2TARGET)$(DLLPOST) $(BIN)$/$(SHL1TARGET)$(DLLPOST)
.ENDIF

# just a quick fix - has to be cleaned up some day...
.IF "$(L10N-framework)"==""
ALL: $(SLB)$/svl.lib \
    $(SLB)$/svt.lib \
    $(MISC)$/$(SHL2TARGET).flt \
    $(MISC)$/$(SHL1TARGET).flt \
    $(MISC)$/$(SHL2TARGET).def \
    $(MISC)$/$(SHL1TARGET).def \
    $(SVTTARGETS) \
    $(SRS)$/hidother.hid \
    ALLTAR
.ENDIF          # "$(L10N-framework)"==""

.INCLUDE :	target.mk

$(SRS)$/hidother.hid: hidother.src
.IF "$(GUI)$(CPU)"=="WNTI"
.IF "$(BUILD_SOSL)"==""
    @+echo no hids
    @+-mhids hidother.src ..\$(INPATH)$/srs sfx2 hidother
.ENDIF
.ELSE
    @echo nix
.ENDIF

# --- Svtools-Control-Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo AUTO_CODE > $@
    @echo BrowserDataWin >> $@
    @echo BrowserColumn >> $@
    @echo ButtonFrame >> $@
    @echo CreateLoader >> $@
    @echo DdeString >> $@
    @echo DlgSource >> $@
    @echo DlgExport >> $@
    @echo EditWindow >> $@
    @echo FileEntry >> $@
    @echo GIFLZWDecompressor >> $@
    @echo GIFReader >> $@
    @echo PNGReader >> $@
    @echo ImpDetect >> $@
    @echo ImpDraw >> $@
    @echo ImpGet >> $@
    @echo ImpPut >>$@
    @echo ImpSv >> $@
    @echo JPEGReader >> $@
    @echo WMFReader >> $@
    @echo EnhWMFReader >> $@
    @echo WMFWriter >> $@
    @echo LinkStub >> $@
    @echo MultiTextLineInfo >> $@
    @echo Regexpr >> $@
    @echo RemoteControlServer >> $@
    @echo RemoteControlService >> $@
    @echo RetStream >> $@
    @echo SCmdStream >> $@
    @echo SbxArrayRef >> $@
    @echo SbxBasicFormater >> $@
    @echo SbxRes >> $@
    @echo SbxVariableRef >> $@
    @echo SfxListUndoAction >> $@
    @echo SfxPointerEntry >> $@
    @echo SfxUINT32s >> $@
    @echo SfxULongRangeItem >> $@
    @echo SfxULongRanges >> $@
    @echo SfxULongRangesItem >> $@
    @echo SgfFontLst >> $@
    @echo SgfFontOne >> $@
    @echo StatementCommand >> $@
    @echo StatementControl >> $@
    @echo StatementFlow >> $@
    @echo StatementList >> $@
    @echo StatementSlot >> $@
    @echo SVDDE_MISC_CODE >> $@
    @echo SvInterfaceClient >> $@
    @echo SvImp >> $@
    @echo SvStringLockBytes >> $@
    @echo SVTOOLS_FILTER >> $@
    @echo SVTOOLS_CODE >> $@
    @echo SVTOOLS_FILTER4 >> $@
    @echo SVTOOLS_FILTER_CODE >> $@
    @echo ValueSetItem >> $@
    @echo WMFRecord >> $@
    @echo WriteRecord >> $@
    @echo XPMReader >> $@
    @echo XBMReader >> $@
    @echo _grow >> $@
    @echo _ImplINetHistoryLoadTimer >> $@
    @echo _ImplINetHistoryStoreTimer >> $@
    @echo sRTF_>>$@
    @echo sHTML_>>$@
    @echo unnamed>>$@
.IF "$(GUI)"=="WNT" || "$(GUI)"=="WIN"
    @echo WEP>>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@
.ENDIF
.IF "$(COM)"=="MSC"
    @echo ??_7>>$@
    @echo ??_8>>$@
    @echo ??_C>>$@
    @echo ??_E>>$@
    @echo ??_F>>$@
    @echo ??_G>>$@
    @echo ??_H>>$@
    @echo ??_I>>$@
    @echo 0Imp>>$@
    @echo Impl@@>>$@
    @echo Imp@@>>$@
    @echo __CT>>$@
    @echo _STL>>$@
.ENDIF

$(MISC)$/$(SHL2TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo AUTO_CODE > $@
    @echo BrowserDataWin >> $@
    @echo BrowserColumn >> $@
    @echo ButtonFrame >> $@
    @echo CreateLoader >> $@
    @echo DdeString >> $@
    @echo DlgSource >> $@
    @echo DlgExport >> $@
    @echo EditWindow >> $@
    @echo FileEntry >> $@
    @echo GIFLZWDecompressor >> $@
    @echo GIFReader >> $@
    @echo PNGReader >> $@
    @echo ImpDetect >> $@
    @echo ImpDraw >> $@
    @echo ImpGet >> $@
    @echo ImpPut >>$@
    @echo ImpSvNum >>$@
    @echo JPEGReader >> $@
    @echo WMFReader >> $@
    @echo EnhWMFReader >> $@
    @echo WMFWriter >> $@
    @echo LinkStub >> $@
    @echo MultiTextLineInfo >> $@
    @echo Regexpr >> $@
    @echo RemoteControlServer >> $@
    @echo RemoteControlService >> $@
    @echo RetStream >> $@
    @echo SCmdStream >> $@
    @echo SbxArrayRef >> $@
    @echo SbxBasicFormater >> $@
    @echo SbxRes >> $@
    @echo SbxVariableRef >> $@
    @echo SfxListUndoAction >> $@
    @echo SfxPointerEntry >> $@
    @echo SfxUINT32s >> $@
    @echo SfxULongRangeItem >> $@
    @echo SfxULongRanges >> $@
    @echo SfxULongRangesItem >> $@
    @echo SgfFontLst >> $@
    @echo SgfFontOne >> $@
    @echo StatementCommand >> $@
    @echo StatementControl >> $@
    @echo StatementFlow >> $@
    @echo StatementList >> $@
    @echo StatementSlot >> $@
    @echo SVDDE_MISC_CODE >> $@
    @echo SvInterfaceClient >> $@
    @echo SvImp >> $@
    @echo SvStringLockBytes >> $@
    @echo svtools >> $@
    @echo SVTOOLS_FILTER >> $@
    @echo SVTOOLS_CODE >> $@
    @echo SVTOOLS_FILTER4 >> $@
    @echo SVTOOLS_FILTER_CODE >> $@
    @echo ValueSetItem >> $@
    @echo WMFRecord >> $@
    @echo WriteRecord >> $@
    @echo XPMReader >> $@
    @echo XBMReader >> $@
    @echo _grow >> $@
    @echo _ImplINetHistoryLoadTimer >> $@
    @echo _ImplINetHistoryStoreTimer >> $@
    @echo sRTF_>>$@
    @echo sHTML_>>$@
    @echo SdbSqlScanner>>$@
    @echo Sdbyy_scan>>$@
    @echo SqlInternalNode>>$@
    @echo unnamed>>$@
.IF "$(GUI)"=="WNT" || "$(GUI)"=="WIN"
    @echo WEP>>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@
.ENDIF
.IF "$(COM)"=="MSC"
    @echo ??_7>>$@
    @echo ??_8>>$@
    @echo ??_C>>$@
    @echo ??_E>>$@
    @echo ??_F>>$@
    @echo ??_G>>$@
    @echo ??_H>>$@
    @echo ??_I>>$@
    @echo 0Imp>>$@
    @echo Impl@@>>$@
    @echo Imp@@>>$@
    @echo __CT>>$@
    @echo _STL>>$@
.ENDIF

