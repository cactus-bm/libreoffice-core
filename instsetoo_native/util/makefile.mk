#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: hr $ $Date: 2004-10-14 12:57:56 $
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
PRJ=..
PRJNAME=OpenOffice
TARGET=util
GEN_HID2=TRUE

.INCLUDE:  settings.mk
.EXPORT: LAST_MINOR

SHARED_COM_SDK_PATH*:=.

INSTALLDIR=$(OUT)

.INCLUDE: target.mk


.IF "$(UPDATER)"!=""
ALLTAR : updatepack
.ELSE			# "$(UPDATER)"!=""
ALLTAR : openoffice
.ENDIF			# "$(UPDATER)"!=""

updatepack:
    +$(PERL) -w $(SOLARENV)$/bin$/packager.pl

.IF "$(alllangiso)"!=""

.IF "$(BUILD_SPECIAL)"!=""  
.IF "$(OOO_RELEASE_BUILD)"==""
openoffice: openoffice_en-US
.ELSE
openoffice: $(foreach,i,$(alllangiso) openoffice_$i)
.ENDIF
.ELSE
openoffice: $(foreach,i,$(alllangiso) openoffice_$i)
.ENDIF

ooolanguagepack : $(foreach,i,$(alllangiso) ooolanguagepack_$i)

openoffice_%:
    +$(PERL) -w $(SOLARENV)$/bin$/make_installer.pl -f openoffice.lst -l $(@:s/openoffice_//) -p OpenOffice -packagelist ..$/inc_openoffice$/unix$/packagelist.txt -u $(OUT) -buildid $(BUILD) -msitemplate ..$/inc_openoffice$/windows$/msi_templates -msilanguage $(COMMONMISC)$/win_ulffiles

ooolanguagepack_%:
    +$(PERL) -w $(SOLARENV)$/bin$/make_installer.pl -f openoffice.lst -l $(@:s/ooolanguagepack_//) -p OpenOffice -packagelist ..$/inc_openoffice$/unix$/packagelist_language.txt -u $(OUT) -buildid $(BUILD) -msitemplate ..$/inc_openoffice$/windows$/msi_templates -msilanguage $(COMMONMISC)$/win_ulffiles -languagepack

.ELSE			# "$(alllangiso)"!=""
openoffice:
    @+echo cannot pack nothing...

.ENDIF			# "$(alllangiso)"!=""
