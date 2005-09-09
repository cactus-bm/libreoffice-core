#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 09:59:40 $
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

PRJ		= ..$/..$/..$/..$/..
PRJNAME = wizards
TARGET  = web
PACKAGE = com$/sun$/star$/wizards$/web

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
#.INCLUDE :  $(PRJ)$/util$/makefile.pmk
JARFILES= unoil.jar jurt.jar ridl.jar juh.jar jut.jar java_uno.jar java_uno_accessbridge commonwizards.jar xalan.jar xercesImp.jar xml-apis.jar
CUSTOMMANIFESTFILE= MANIFEST.MF

JARCLASSDIRS	= com$/sun$/star$/wizards$/web
JARTARGET		= web.jar

# --- Files --------------------------------------------------------

JAVAFILES=			\
    AbstractErrorHandler.java \
    BackgroundsDialog.java \
    CallWizard.java \
    ErrorHandler.java \
    ExtensionVerifier.java \
    FTPDialog.java \
    FTPDialogResources.java \
    IconsDialog.java \
    ImageListDialog.java \
    LogTaskListener.java \
    Process.java \
    ProcessErrorHandler.java \
    ProcessErrors.java \
    ProcessStatusRenderer.java \
    StatusDialog.java \
    StylePreview.java \
    TOCPreview.java \
    WebWizard.java \
    WebWizardConst.java \
    WebWizardDialog.java \
    WebWizardDialogResources.java \
    WWD_Events.java \
    WWD_General.java \
    WWD_Startup.java \
    WWHID.java

JAVACLASSFILES = $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
