'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: c_updt1.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: jsi $ $Date: 2008-06-16 08:06:04 $
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
'/***********************************************************************
'*
'* owner : oliver.craemer@sun.com
'*
'* short description : Resource-Test Spreadsheet Application Part I
'*
'\***********************************************************************

sub main
    use "spreadsheet\required\includes\c_upd_filemenu.inc"
    use "spreadsheet\required\includes\c_upd_editmenu.inc"
    use "spreadsheet\required\includes\c_upd_editmenu2.inc"
    use "spreadsheet\required\includes\c_upd_viewmenu.inc"
    use "spreadsheet\required\includes\c_upd_insertmenu.inc"
    use "spreadsheet\tools\includes\c_cell_tools.inc"
    use "spreadsheet\tools\includes\c_select_tools.inc"

    Printlog "----------------------------------------------------"
    Printlog "--- Resource-Test Spreadsheet Application Part I ---"
    Printlog "----------------------------------------------------"   

    call hStatusIn("Spreadsheet", "c_updt1.bas", "Resource Test for Spreadsheet Part I")
    
    call c_upd_filemenu      'FILE   Menu
    call c_upd_editmenu1     'EDIT   Menu Part I
    call c_upd_editmenu2     'EDIT   Menu Part II
    call c_upd_viewmenu      'VIEW   Menu
    call c_upd_insertmenu    'INSERT Menu
      
    call hStatusOut

end sub

' ********************************************
' ** - global start routines
' ********************************************
sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    use "global\tools\includes\optional\t_locale_tools.inc"
    gApplication   = "Calc"
    GetUseFiles ()
end sub
