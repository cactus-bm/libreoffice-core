'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: f_extras_templatefileopen.bas,v $
'*
'* $Revision: 1.4 $
'*
'* last change: $Author: rt $ $Date: 2008-09-04 09:15:47 $
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
'* owner : joerg.skottke@sun.com
'*
'* short description : open all templates via file-open dialog
'*
'\*****************************************************************************

sub main

   use "framework\optional\includes\extras_file_open.inc"

   hSetMacroSecurityAPI( GC_MACRO_SECURITY_LEVEL_LOW )
   
   call hStatusIn( "framework", "f_extras_templatefileopen.bas" )
   call tOpenObjectsWithFileDialog( "TEMPLATES" )
   call hStatusOut
   
   hSetMacroSecurityAPI( GC_MACRO_SECURITY_LEVEL_DEFAULT )

end sub

'-------------------------------------------------------------------------

sub LoadIncludeFiles

   use "global\system\includes\master.inc"
   use "global\system\includes\gvariabl.inc"

   use "global\tools\includes\optional\t_docfuncs.inc"
   use "global\tools\includes\optional\t_filetools.inc"
   use "global\tools\includes\optional\t_security_tools.inc"
   use "global\tools\includes\optional\t_listfuncs.inc"

   use "framework\tools\includes\apicalls.inc"
   use "framework\tools\includes\template_tools.inc"

   gApplication = "WRITER"
   GetUseFiles
   
end sub

