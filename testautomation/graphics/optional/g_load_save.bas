'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
' DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
' 
' Copyright 2000, 2010 Oracle and/or its affiliates.
'
' OpenOffice.org - a multi-platform office productivity suite
'
' This file is part of OpenOffice.org.
'
' OpenOffice.org is free software: you can redistribute it and/or modify
' it under the terms of the GNU Lesser General Public License version 3
' only, as published by the Free Software Foundation.
'
' OpenOffice.org is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU Lesser General Public License version 3 for more details
' (a copy is included in the LICENSE file that accompanied this code).
'
' You should have received a copy of the GNU Lesser General Public License
' version 3 along with OpenOffice.org.  If not, see
' <http://www.openoffice.org/license.html>
' for a copy of the LGPLv3 License.
'
'/************************************************************************
'*
'* Owner : wolfram.garten@sun.com
'*
'* short description : Graphics Function: Load and Save
'*
'\******************************************************************

public glLocale (15*20) as string
public q as integer
public ExtensionString as String

sub main
    PrintLog "------------------------- g_load_save test -------------------------"
    Call hStatusIn ( "Graphics","g_load_save.bas")

    use "graphics\tools\id_tools.inc"
    use "graphics\tools\id_tools_2.inc"
    use "graphics\optional\includes\global\g_load_save.inc"

    hSetLocaleStrings ( gTesttoolPath + "graphics\tools\locale_1.txt" , glLocale () )

    PrintLog "-------------------------" + gApplication + "-------------------"
    Call tFileOpenSaveEtc
    Call tFileProperties
    Call tFilePassword52
    Call tFileOpenImport
    Call tTestDXF255CharBug
    Call tDeletedTemplate
    call tSaveLoadLayoutEmpty
    call tSaveLoadLayoutOLE
    call tSaveLoadLayoutText
    call tSaveLoadLayoutOutline
    call tSaveLoadLayoutPicture
    call tSaveLoadLayoutChart
    call tSaveLoadLayoutSpreadsheet

    gApplication = "IMPRESS"
    PrintLog "-------------------------" + gApplication + "-------------------"
    Call tFileOpenSaveEtc
    Call tFileProperties
    Call tFilePassword52
    Call tFileOpenImport
    Call tTestDXF255CharBug
    Call tDeletedTemplate
    call tSaveLoadLayoutEmpty
    call tSaveLoadLayoutOLE
    call tSaveLoadLayoutText
    call tSaveLoadLayoutOutline
    call tSaveLoadLayoutPicture
    call tSaveLoadLayoutChart
    call tSaveLoadLayoutSpreadsheet

    Call hStatusOut
end sub

sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    Call GetUseFiles
    gApplication = "DRAW"
end sub


