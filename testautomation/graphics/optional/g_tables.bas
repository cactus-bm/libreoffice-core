'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'*
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: g_tables.bas,v $
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
'* owner : wolfram.garten@sun.com
'*
'* short description : Testing of impress tables
'*
'\***********************************************************************

sub main
    Printlog "------------------- Impress Tables Test -------------------------"
    call hStatusIn ("Graphics","i_tables.bas")

    use "graphics\optional\includes\global\g_tables.inc"

    PrintLog "-------------------------" + gApplication + "-------------------"

    call tiInsertTableUsingMenu
    call tiInsertTableUsingButton
    call tiInsertTableUsingToolbarbutton
    call tiTableObjectBar
    gApplication = "DRAW"

    PrintLog "-------------------------" + gApplication + "-------------------"

    call tiInsertTableUsingMenu
    call tiInsertTableUsingToolbarbutton
    call tiTableObjectBar

    call hStatusOut

end sub

'----------------------------------------------
sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    gApplication = "IMPRESS"
    Call getUseFiles
end sub

