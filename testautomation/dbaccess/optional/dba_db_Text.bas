'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: dba_db_Text.bas,v $
'*
'* $Revision: 1.1 $
'*
'* last change: $Author: msc $ $Date: 2008-06-20 12:58:25 $
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
'* owner : marc.neumann@sun.com
'*
'* short description : Create Text DS & further standard db tests
'*
'\***********************************************************************
sub main
    printlog "---------------------------------------------------"
    printlog "---       D B A C C E S S    Text Table         ---"
    printlog "---------------------------------------------------"

    use "dbaccess/optional/includes/db_Query.inc"
    use "dbaccess/optional/includes/db_Text.inc"
    
    call hStatusIn ("dbaccess" , "dba_db_Text.bas")
    
    Dim sFileName as string
	sFileName = gOfficePath + "user/work/TT_Text.odb"
    
    Dim sDBURL as string
	sDBURL = "user/work"
   
        'needed for: tQuery testcase - query file is copied in the work directory
        app.FileCopy gTesttoolPath + ConvertPath("dbaccess/optional/input/text_datasource/TT_Query1.txt"),gOfficePath + ConvertPath("user/work/TT_Query1.txt")
   
   		dim dbok as boolean
   		dbok = false
        dbok = fCreateTextDatasource(sFileName, gOfficePath + ConvertPath(sDBURL))
        if dbok = true then
        
            
            call db_Query(sFileName, "text")
            
            'use "dbaccess/optional/includes/b_lvl1_Forms.inc"
            'call Forms_Test (sFileName)
            
            call db_Text(sFileName)
                        

		else
			warnlog "Data Source could not be created - beyond testcases stopped"
		endif
       
    call hStatusOut
    
end sub


sub LoadIncludeFiles   
   use "dbaccess/tools/dbinit.inc"   
   Call sDBInit
   Call GetUseFiles
   gApplication   = "WRITER"
end sub
