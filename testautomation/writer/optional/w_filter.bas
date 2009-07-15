'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: w_filter.bas,v $
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
'* short description : Writer Filter Availability Test (Import/Export/Group names)
'*
'\*******************************************************************

global gSeperator as String
global gMeasurementUnit as String

sub main
 Dim ImportFilterDatei as String

  use "writer\tools\includes\w_tool6.inc"
  use "writer\optional\includes\filter\w_filter.inc"

   Call hStatusIn ( "writer", "w_filter.bas" )

   Call tSammleImportFilter
   Call tCollectSaveAsFilter
   Call tCollectExportFilters
   Call tGroupNameCheck

   Call hStatusOut

end sub

sub LoadIncludeFiles
   use "global\system\includes\master.inc"
   use "global\system\includes\gvariabl.inc"
   use "global\tools\includes\optional\t_xml1.inc"
   gApplication   = "WRITER"
   Call GetUseFiles()
end sub
