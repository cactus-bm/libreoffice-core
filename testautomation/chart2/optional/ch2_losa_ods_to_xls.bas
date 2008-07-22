'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: ch2_losa_ods_to_xls.bas,v $
'*
'* $Revision: 1.2 $
'*
'* last change: $Author: obo $ $Date: 2008-07-22 10:35:03 $
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
'* owner : joerg.sievers@Sun.COM
'*
'* short description : Load/Save test for chart files - Input: ODS, Output: XLS
'*
'\************************************************************************

Global gsFileFormat as String
Global gsDirectoryInSourcePath as String
Global gsOutputFileCustomExtension as String

sub main      
    gsFileFormat = "MS Excel 97"
        
    'Important for determination of input file source path
    '-> gtesttoolpath & "chart2\optional\input\" & gsDirectoryInSourcePath &"\"
    gsDirectoryInSourcePath = "ods"
    
    'Custom output file extension. Can be different to result from hGetFilternameExtension.
    'e.g. "_excel95.xls"
    gsOutputFileCustomExtension = ".xls"        

    Printlog "--------------------------------------------------"
    Printlog "---       Load charts in ODS - SaveAS XLS      ---"
    Printlog "--------------------------------------------------"

    use "chart2/optional/includes/loadsave/ch2_losa.inc"
    use "chart2/tools/ch_tools_common.inc"
    
    Call hStatusIn("Chart2", "ch2_losa_ods_to_xls.bas")       
        Call subLoSaFromTo   
    Call hStatusOut                                 
end sub
'
'-------------------------------------------------------------------------
'
sub LoadIncludeFiles
    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"
    gApplication   = "Calc"
    Call GetUseFiles
end sub
