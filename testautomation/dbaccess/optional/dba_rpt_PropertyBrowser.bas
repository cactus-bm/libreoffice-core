'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: dba_rpt_PropertyBrowser.bas,v $
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
'* short description : Test for the property browser
'*
'\***********************************************************************
sub main
    
    printlog "---------------------------------------------------------------"
    printlog "-----         D B A C C E S S  -  Property Browser        -----"
    printlog "---------------------------------------------------------------"

    use "dbaccess/optional/includes/rpt_PropertyBrowser.inc"

    call hStatusIn ("dbaccess" , "dba_rpt_PropertyBrowser.bas")

    if ( tools_reporttools_InstallExtension() = 0 ) then        
        call rpt_PropertyBrowser
    else
        warnlog "report extension could not installed"    
    endif
    
    call hStatusOut
    
end sub


sub LoadIncludeFiles      
   use "dbaccess/tools/dbinit.inc"   
   Call sDBInit
   Call GetUseFiles
   gApplication   = "WRITER"
end sub
