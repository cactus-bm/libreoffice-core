#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
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
PRJNAME=instsetoo_native
TARGET=util
GEN_HID2=TRUE

.INCLUDE:  settings.mk
.EXPORT: LAST_MINOR
.EXPORT: PRJ
.EXPORT: PRJNAME

.INCLUDE .IGNORE: $(SRC_ROOT)$/cwsname.mk

SHARED_COM_SDK_PATH*:=.

INSTALLDIR=$(OUT)

.INCLUDE: target.mk

.IF "$(OS)" == "LINUX"
FORMAT*=-format rpm
.ENDIF

.IF "$(OS)" == "SOLARIS"
FORMAT*=-format pkg
.ENDIF

# epm supports the following formats:
# aix - AIX software distribution
# bsd - FreeBSD, NetBSD, or OpenBSD software distribution
# depot or swinstall - HP-UX software distribution
# dpkg - Debian software distribution
# inst or tardist - IRIX software distribution
# osx - MacOS X software distribution
# pkg - Solaris software distribution
# rpm - RedHat software distribution
# setld - Tru64 (setld) software distribution
# native - "Native" software distribution for the platform
# portable - Portable software distribution

.IF "$(UPDATER)"=="" || "$(USE_PACKAGER)"==""
ALLTAR : openoffice
.ELSE			# "$(UPDATER)"=="" || "$(USE_PACKAGER)"==""
ALLTAR : updatepack
.ENDIF			# "$(UPDATER)"=="" || "$(USE_PACKAGER)"==""

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
    +$(PERL) -w $(SOLARENV)$/bin$/make_installer.pl -f $(PRJ)$/util$/openoffice.lst -l $(@:s/openoffice_//) -p OpenOffice -packagelist $(PRJ)$/inc_openoffice$/unix$/packagelist.txt -u $(OUT) -buildid $(BUILD) -msitemplate $(PRJ)$/inc_openoffice$/windows$/msi_templates -msilanguage $(COMMONMISC)$/win_ulffiles $(FORMAT)

ooolanguagepack_%:
    +$(PERL) -w $(SOLARENV)$/bin$/make_installer.pl -f $(PRJ)$/util$/openoffice.lst -l $(@:s/ooolanguagepack_//) -p OpenOffice -packagelist $(PRJ)$/inc_openoffice$/unix$/packagelist_language.txt -u $(OUT) -buildid $(BUILD) -msitemplate $(PRJ)$/inc_ooolangpack$/windows$/msi_templates -msilanguage $(COMMONMISC)$/win_ulffiles -languagepack $(FORMAT)

.ELSE			# "$(alllangiso)"!=""
openoffice:
    @+echo cannot pack nothing...

.ENDIF			# "$(alllangiso)"!=""
