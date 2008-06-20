'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: f_options_loadsave.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: jsk $ $Date: 2008-06-20 07:07:37 $
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
'*  owner : thorsten.bosbach@sun.com
'*
'*  short description : Function: Tools->Options: Load/Save
'*
'\******************************************************************************

sub main

    use "framework\optional\includes\options_loadsave_general.inc"
    use "framework\optional\includes\options_loadsave_vba.inc"
    use "framework\optional\includes\options_loadsave_msoffice.inc"
    use "framework\optional\includes\options_loadsave_html.inc"

    call hStatusIn( "framework" , "f_options_loadsave.bas")


    call tLoadSaveGeneral
    call tLoadSaveVBA
    call tLoadSaveMSOffice
    call tLoadSaveHTML

    call hStatusOut
    
end sub

'*******************************************************************************

sub LoadIncludeFiles
   use "global\system\includes\master.inc"
   use "global\system\includes\gvariabl.inc"
   
   use "framework\tools\includes\options_tools.inc"
   
   gApplication = "WRITER"
   call GetUseFiles
end sub
