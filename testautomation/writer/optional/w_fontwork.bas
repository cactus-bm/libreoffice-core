'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: w_fontwork.bas,v $
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
'* short description : Fontwork test
'*
'\*******************************************************************

global gSeperator, gMeasurementUnit as String
sub main
    Dim StartZeit
    StartZeit = Now()

    use "writer\tools\includes\w_tools.inc"    
    use "writer\optional\includes\fontwork\w_fontwork1.inc"

    printlog Chr(13) + "Loading of Include - Files takes: " + Wielange ( StartZeit )
    printlog Chr(13) + "******* Writer - Fontwork - Test *******"

    gSeperator = GetDecimalSeperator()
    'Setting the measurement unit to centimeters.
    gMeasurementUnit = fSetMeasurementToCM()

    Call hStatusIn ( "writer" , "w_fontwork.bas" )
    Call tFontWork_1    'Test pre-defined shapes (upper/lower Semicircle,...) to the selected text object
    Call tFontWork_2    'Test Orientation
    Call tFontWork_3    'Test Align , include Left , Center, Right and Autosize
    Call tFontWork_4    'Test distance and indent
    Call tFontWork_5    'Test Contour - include contour and text contour
    Call tFontWork_6    'Test shadow - include vertical and slanted
    Call hStatusOut

    Printlog Chr(13) + "End of Fontwork - Test :"
    Printlog "Duration: "+ WieLange ( StartZeit )
    Printlog "Date: " +  Date + "    Time: " + Time

end sub

sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    Call GetUseFiles
    gApplication = "Writer"
end sub
