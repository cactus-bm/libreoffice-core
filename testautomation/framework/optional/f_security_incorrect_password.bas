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
'*  owner : joerg.skottke@sun.com
'*
'*  short description : Verify that documents really get password protected
'*
'\******************************************************************************

sub main

    use "framework\optional\includes\security_incorrect_password.inc"
    
    dim iApplication as integer
    
    call hStatusIn( "framework" , "f_security_incorrect_password.bas" )
    
    for iApplication =  1 to 5 step 2

        printlog( "" )
        printlog( hNumericDocType( iApplication + 1 ) & " - Format: " & 680 )
        call tIncorrectPassword( "current" )
        
    next iApplication

    call hStatusOut()

end sub

'-------------------------------------------------------------------------

sub LoadIncludeFiles

    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    
    use "global\tools\includes\optional\t_security_tools.inc"
    use "global\tools\includes\optional\t_filetools.inc"
    use "global\tools\includes\optional\t_docfuncs.inc"

    use "framework\tools\includes\init_tools.inc"
    
    gApplication = "WRITER"
    call GetUseFiles()

end sub

'-------------------------------------------------------------------------
