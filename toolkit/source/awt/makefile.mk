#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.23 $
#
#   last change: $Author: rt $ $Date: 2003-06-12 07:47:30 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

PRJ=..$/..

PRJNAME=toolkit
TARGET=awt

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :	svpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	sv.mk

# --- Files --------------------------------------------------------

SLOFILES=   \
            $(SLO)$/accessiblemenubasecomponent.obj		\
            $(SLO)$/accessiblemenucomponent.obj			\
            $(SLO)$/accessiblemenuitemcomponent.obj		\
            $(SLO)$/vclxaccessiblebox.obj				\
            $(SLO)$/vclxaccessiblebutton.obj			\
            $(SLO)$/vclxaccessiblecheckbox.obj			\
            $(SLO)$/vclxaccessiblecombobox.obj			\
            $(SLO)$/vclxaccessiblecomponent.obj 		\
            $(SLO)$/vclxaccessibledropdowncombobox.obj	\
            $(SLO)$/vclxaccessibledropdownlistbox.obj	\
            $(SLO)$/vclxaccessibleedit.obj				\
            $(SLO)$/vclxaccessiblefixedtext.obj			\
            $(SLO)$/vclxaccessiblelist.obj				\
            $(SLO)$/vclxaccessiblelistbox.obj			\
            $(SLO)$/vclxaccessiblelistitem.obj			\
            $(SLO)$/vclxaccessiblemenu.obj				\
            $(SLO)$/vclxaccessiblemenubar.obj			\
            $(SLO)$/vclxaccessiblemenuitem.obj			\
            $(SLO)$/vclxaccessiblemenuseparator.obj		\
            $(SLO)$/vclxaccessiblepopupmenu.obj			\
            $(SLO)$/vclxaccessibleradiobutton.obj		\
            $(SLO)$/vclxaccessiblescrollbar.obj			\
            $(SLO)$/vclxaccessiblestatusbar.obj			\
            $(SLO)$/vclxaccessiblestatusbaritem.obj		\
            $(SLO)$/vclxaccessibletabcontrol.obj		\
            $(SLO)$/vclxaccessibletabpage.obj			\
            $(SLO)$/vclxaccessibletabpagewindow.obj		\
            $(SLO)$/vclxaccessibletextcomponent.obj		\
            $(SLO)$/vclxaccessibletextfield.obj			\
            $(SLO)$/vclxaccessibletoolbox.obj			\
            $(SLO)$/vclxaccessibletoolboxitem.obj		\
            $(SLO)$/vclxbitmap.obj 						\
            $(SLO)$/vclxcontainer.obj 					\
            $(SLO)$/vclxdevice.obj 						\
            $(SLO)$/vclxfont.obj 						\
            $(SLO)$/vclxgraphics.obj 					\
            $(SLO)$/vclxmenu.obj 						\
            $(SLO)$/vclxpointer.obj 					\
            $(SLO)$/vclxprinter.obj 					\
            $(SLO)$/vclxregion.obj 						\
            $(SLO)$/vclxsystemdependentwindow.obj		\
            $(SLO)$/vclxtoolkit.obj 					\
            $(SLO)$/vclxtopwindow.obj 					\
            $(SLO)$/vclxwindow.obj 						\
            $(SLO)$/vclxwindows.obj

SRCFILES=\
            accessiblestrings.src

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

