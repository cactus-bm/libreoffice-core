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
'/******************************************************************************
'*
'*  owner : joerg.skottke@sun.com
'*
'*  short description : Export BASIC libraries
'*
'\******************************************************************************

public const LIBRARY_NAME = "tBasicExport"

' this is where Untitled2 should be in the libraries tree-list
public const DOCUMENT_POSITION = 4 

    
sub main

    use "framework\optional\includes\basic_library_export.inc"
    use "framework\optional\includes\basic_library_import.inc"

    ' Note: Testcases depend on each other, do not change the sequence
    call hStatusIn( "framework" , "f_basic_library_export_import.bas" )
    call tBasicLibraryExport()  
    call tBasicLibraryImport()
    call hStatusOut()

end sub

'-------------------------------------------------------------------------

sub LoadIncludeFiles

    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    
    use "global\tools\includes\optional\t_basic_ide_tools.inc" 
    use "global\tools\includes\optional\t_security_tools.inc"
    use "global\tools\includes\optional\t_treelist_tools.inc"
    use "global\tools\includes\optional\t_basic_ide_tools.inc"
    use "global\tools\includes\optional\t_basic_organizer_tools.inc" 
    use "global\tools\includes\optional\t_docfuncs.inc"
    use "global\tools\includes\optional\t_filetools.inc"
    use "global\tools\includes\optional\t_accels.inc"
    use "global\tools\includes\optional\t_listfuncs.inc"    
    use "global\tools\includes\optional\t_stringtools.inc" 
    use "global\tools\includes\optional\t_macro_tools.inc"

    use "framework\tools\includes\init_tools.inc"
    
    gApplication = "WRITER"
    call GetUseFiles()

end sub

