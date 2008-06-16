'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: g_stylist.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: jsi $ $Date: 2008-06-16 10:42:37 $
'*
'* This file is part of OpenOffice.org.
'*
'* OpenOffice.org is free software: you can redistribute it and/or modify
'* it under the terms of the GNU Lesser General Public License version 3
'* only, as published by the Free Software Foundation.
'*
'* OpenOffice.org is distributed in the hope that it will be useful,
'* but WITHOUT ANY WARRANTY; without even the implied warranty of
'* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
'* GNU Lesser General Public License version 3 for more details
'* (a copy is included in the LICENSE file that accompanied this code).
'*
'* You should have received a copy of the GNU Lesser General Public License
'* version 3 along with OpenOffice.org.  If not, see
'* <http://www.openoffice.org/license.html>
'* for a copy of the LGPLv3 License.
'*
'/************************************************************************
'*
'* Owner : wolfram.garten@sun.com
'*
'* short description : Graphics Function: Stylist
'*
'\*******************************************************************

public glLocale (15*20) as string

sub main
    PrintLog "------------------------- Stylist Test -------------------"
    Call hStatusIn ( "Graphics","g_stylist.bas" )
    GetOLEDefaultNames

    use "graphics\tools\id_tools.inc"
    use "graphics\tools\id_tools_2.inc"
    use "graphics\optional\includes\global\g_stylist.inc"
    use "graphics\optional\includes\impress\i_stylist.inc"

    if hSetLocaleStrings ( gTesttoolPath + "graphics\tools\locale_1.txt" , glLocale () ) = FALSE then
        warnlog "Locales file doesn't exist graphics\tools\locale_1.txt"
    endif

    gApplication = "Impress"

    PrintLog "-------------------------" + gApplication + "-------------------"
    Call tFormatStylistFlyer
    Call tFormatStylistDocuments
    Call tFormatStylistTemplatesUse
    Call tFormatStylistTemplateCreate
if iSprache <> "82" then
    Call tFormatStylistTemplateSelect
    Call tFormatStylistTemplateNew
else
   qaerrorlog "#i65490# - Deactivated tFormatStylistTemplateSelect, tFormatStylistTemplateNew due to bug."
endif
    call tiFormatStylist
    call tFormatStylist

if iSprache <> "81" then
   if iSprache <> "82" then
      Call tFormatStylistBackground
   else
      qaerrorlog "#i65534# - Deactivated tFormatStylistBackground due to bug under Korean."
   endif
else
   qaerrorlog "Deactivated tFormatStylistBackground due to string-name-difficulties."
endif

    gApplication = "Draw"
    PrintLog "-------------------------" + gApplication + "-------------------"

    Call tFormatStylistFlyer
    Call tFormatStylistDocuments
    Call tFormatStylistTemplatesUse
'    Call tFormatStylistTemplateCreate
if iSprache <> "82" then
    Call tFormatStylistTemplateSelect
    Call tFormatStylistTemplateNew
else
   qaerrorlog "#i65490# - Deactivated tFormatStylistTemplateSelect, tFormatStylistTemplateNew due to bug."
endif
    call tiFormatStylist
    call tFormatStylist

    Call hStatusOut
end sub

sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    gApplication = "Impress"
    Call GetUseFiles
end sub
