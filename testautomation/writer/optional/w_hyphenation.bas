'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: w_hyphenation.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: fredrikh $ $Date: 2008-06-18 09:11:25 $
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
'* owner :  fredrik.haegg@sun.com
'*
'* short description : Tools / Hyphenation test
'*
'\*******************************************************************

sub main

    use "writer\tools\includes\w_tools.inc"
    use "writer\tools\includes\w_tool4.inc"    
    use "writer\optional\includes\hyphenation\w_hyphenation.inc"

    printlog Chr(13) + "******* Writer - Hyphenation - Test *******"

    Call hStatusIn ( "writer" , "w_hyphenation.bas" )
    Call wEnableHyphenation(true)    
    Call tHyphenation_1            'Test Hyphenation  using ctrl and "-"
    Call tHyphenation_2            'Test Hyphenation  using Tools/Hyphenation ,test like fol-lowing
    Call tHyphenation_3            'Test Hyphenation  using Tools/Hyphenation ,test like follow-ing
    Call wEnableHyphenation(false)

    Call hStatusOut

end sub

sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    Call GetUseFiles
    gApplication = "Writer"
end sub
