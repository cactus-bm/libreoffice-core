'encoding UTF-8  Do not remove or change this line!
'**************************************************************************
'* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
'* 
'* Copyright 2008 by Sun Microsystems, Inc.
'*
'* OpenOffice.org - a multi-platform office productivity suite
'*
'* $RCSfile: f_basic_templatedocuments.bas,v $
'*
'* $Revision: 1.2 $
'*
'* last change: $Author: jsk $ $Date: 2008-06-20 08:03:52 $
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
'/******************************************************************************
'*
'*  owner : joerg.skottke@sun.com
'*
'*  short description : My Macros/standard execution with user defined template
'*
'\******************************************************************************

sub main

    use "framework\optional\includes\basic_usertemplate.inc"
    use "framework\optional\includes\basic_spectemplate.inc"

    call hStatusIn( "framework" , "f_basic_templatedocuments.bas" )

    call tMacroUserTemplate()
    call tBasicSpecTemplate()

    call hStatusOut()

end sub

'-------------------------------------------------------------------------

sub LoadIncludeFiles

    use "global\system\includes\master.inc"
    use "global\system\includes\gvariabl.inc"

    use "global\tools\includes\optional\f_docfuncs.inc"
    use "global\tools\includes\optional\t_filetools.inc"
    use "global\tools\includes\optional\t_basic_ide_tools.inc"
    use "global\tools\includes\optional\t_basic_organizer_tools.inc"
    use "global\tools\includes\optional\t_treelist_tools.inc"
    use "global\tools\includes\optional\accels.inc"
    use "global\tools\includes\optional\t_control_objects.inc"
    use "global\tools\includes\optional\t_security_tools.inc"

    use "framework\tools\includes\private_environment.inc"
    use "framework\tools\includes\webtools.inc"
    use "framework\tools\includes\filter_tools.inc"
    use "framework\tools\includes\template_tools.inc"
    use "framework\tools\includes\init_tools.inc"
    use "framework\tools\includes\scriptorganizer_tools.inc"

    gApplication = "WRITER"
    call GetUseFiles()

end sub

