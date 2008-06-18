'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: w_layout_regression.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: fredrikh $ $Date: 2008-06-18 09:11:26 $
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
'* owner : fredrik.haegg@sun.com
'*
'* short description : Load all files which crashed or looped because of layout problems
'*
'\***********************************************************************

global SourceDocument(200 ) as String

sub main

    use "writer\tools\includes\w_tools.inc"
    use "writer\loadsave\includes\w_layout_regression.inc"

    Dim StartZeit, SourcePath as string
    StartZeit = Now()

    SourcePath = ConvertPath(gtesttoolpath + "sun_writer\regression\input\layout")   
    printlog SourcePath
    if hDirectoryExists(SourcePath) = false then        
        Warnlog "Unable to find document directory! Please adapt 'SourcePath'"
        goto EarlyExit
    endif    
    
    Call hStatusIn ( "writer", "w_layout_regression.bas" )
    Call wSetMacroSecurityLevel(4)
    Call tLoadAllDocuments ( SourcePath )
    Call wSetMacroSecurityLevel(3)
    Call hStatusOut

    EarlyExit:
    Printlog Chr(13) + "End of Import - Test :"
    Printlog "Duration: "+ WieLange ( StartZeit )
    Printlog "Date: " +  Date + "    Time: " + Time

end sub

' **********************************

sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    use "global\tools\includes\t_dir.inc"
    use "global\tools\includes\t_lists.inc"
    Call GetUseFiles
    gApplication = "Writer"
end sub




