'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2009 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: ch2_lvl1.bas,v $
'*
'* $Revision: 1.0 $
'*
'* last change: $Author: hde $ $Date: 2009-04-14 15:12:01 $
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
'* owner : helge.delfs@sun.com
'*
'* short description : Chart2 functional tests
'*
'\************************************************************************

sub main
    use "chart2\tools\ch_tools_common.inc"
    use "chart2\optional\includes\ch2_lvl1a.inc"
    
    Call hStatusIn ( "Chart2", "ch2_lvl1.bas" )
    
	Call ch2_lvl1a
    
	Call hStatusOut
	
	
end sub
'
'-------------------------------------------------------------------------------
'
sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    use "global\tools\includes\optional\t_set_standard_controls.inc"
    Call GetUseFiles     
    gApplication = "Calc"
end sub

