'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: dba_Main.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: jsi $ $Date: 2008-06-16 07:43:45 $
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
'* owner : marc.neumann@sun.com
'*
'* short description : DBAccess Main Test
'*
'\***********************************************************************
sub main
    
    printlog "---------------------------------------------------------------------"
    printlog "-----            D B A C C E S S  -  M A I N T E S T            -----"
    printlog "---------------------------------------------------------------------"
   
    use "dbaccess/required/includes/Query.inc"
    use "dbaccess/required/includes/MainApp.inc"
    use "dbaccess/required/includes/TableDesign.inc"
    use "dbaccess/required/includes/Table.inc"
    use "dbaccess/required/includes/Forms.inc"
    use "dbaccess/required/includes/DatabaseTypes.inc"
    use "dbaccess/required/includes/Wizards.inc"
    
    Dim startes
    startes = Now
    
    call hStatusIn ("dbaccess","dba_Main.bas" , "DBAccess Main Test")
   
    call MainApp
    call Query
    call TableDesign
    call Table
    call Forms
    
    call ExitRestartTheOffice
    call DatabaseTypes
    call ExitRestartTheOffice    
    call Wizards
    
    call hStatusOut
    
    printlog "---------------------------------------------------------------------"
    printlog "End: " & Date & ", " & Time & ". (" & wielange(Startes) & ")"
    printlog "---------------------------------------------------------------------"
end sub


sub LoadIncludeFiles
    use "global/tools/includes/optional/extension_manager_tools.inc"
    use "dbaccess/tools/dbinit.inc"
    Call sDBInit
    Call GetUseFiles
    gApplication   = "WRITER"
end sub
