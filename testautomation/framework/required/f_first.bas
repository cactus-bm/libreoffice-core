'encoding UTF-8  Do not remove or change this line!
'*******************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: f_first.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: jsk $ $Date: 2008-06-20 07:23:33 $
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
'/******************************************************************************
'*
'* owner : thorsten.bosbach@sun.com
'*
'* short description : first test, to check global functionality
'*
'\******************************************************************************


sub main

    use "framework\required\includes\first.inc"
    use "framework\required\includes\graphics_import.inc"
    
    call hStatusIn ("framework", "f_first.bas")
    call tAllNew
    
    gApplication = "Draw"
    call tInFilterCountCompare
    call tQuickInsertGraphNotAsLink
    call tJava
    
    gApplication = "Writer"
    call tToolsOptionsTest
    call tDatabaseCheck
    call tHelpRegistration
    call hStatusOut   
    
end sub

'*******************************************************************************

sub LoadIncludeFiles

    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    
    use "global\required\includes\g_option.inc"
    
    use "global\tools\includes\optional\filetools.inc"
    use "global\tools\includes\optional\docfuncs.inc"
    use "global\tools\includes\optional\listfuncs.inc"

    gApplication = "WRITER"
    call GetUseFiles()
    
end sub

'-------------------------------------------------------------------------

