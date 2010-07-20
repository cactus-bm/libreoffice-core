'encoding UTF-8  Do not remove or change this line!
'*******************************************************************************
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
'/******************************************************************************
'*
'*  owner : joerg.skottke@oracle.com
'*
'*  short description : Update-Test for some dialogs in Basic-IDE
'*
'\******************************************************************************

sub main

    use "framework\required\includes\basic_macroassignment.inc"
    use "framework\required\includes\basic_dialog_i18n.inc"
    use "framework\required\includes\basic_organizer.inc"
    use "framework\required\includes\basic_dialog_export.inc"
    use "framework\required\includes\basic_dialog_i18n_import.inc"
    use "framework\required\includes\scripting_basics.inc"
    use "framework\required\includes\script_organizers.inc"

    call hStatusIn( "framework" , "f_programmability_dialogs.bas" )
    call tUpdtMacroAssignment()
    call tUpdtBasicDialogI18n()
    call tUpdtBasicOrganizer()
    call tUpdtDialogExport()
    call tBasicDialogI18nImport()
    call tScripting()       ' script organizer nodes if no documents are open
    call tUpdtScriptCount() ' script organizer nodes with one open document
    call hStatusOut()

end sub

'*******************************************************************************

sub LoadIncludeFiles

    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"

    use "global\tools\includes\optional\t_basic_ide_tools.inc"
    use "global\tools\includes\optional\t_basic_organizer_tools.inc"
    use "global\tools\includes\optional\t_treelist_tools.inc"
    use "global\tools\includes\optional\t_docfuncs.inc"
    use "global\tools\includes\optional\t_filetools.inc"    

    use "framework\tools\includes\formcontrols.inc"
    use "framework\tools\includes\pbrowser_tools.inc"

    gApplication = "WRITER"
    call GetUseFiles()

end sub

'-------------------------------------------------------------------------
