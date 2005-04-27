#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.13 $
#
#   last change: $Author: obo $ $Date: 2005-04-27 09:45:56 $
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

PRJNAME=svtools
TARGET=dialogs

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :  $(PRJ)$/util$/svt.pmk

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES=		filedlg2.src	\
                                so3res.src    \
                                formats.src    \
                prnsetup.src	\
                printdlg.src	\
                colrdlg.src		\
                logindlg.src	\
                addresstemplate.src	\
                wizardmachine.src


EXCEPTIONSFILES=	$(SLO)$/addresstemplate.obj	\
                                $(SLO)$/insdlg.obj       \
                    $(SLO)$/printdlg.obj \
                    $(SLO)$/wizardmachine.obj


SLOFILES=	\
                $(SLO)$/insdlg.obj       \
                $(SLO)$/roadmapwizard.obj 	\
                $(SLO)$/wizardmachine.obj 	\
                $(SLO)$/addresstemplate.obj 	\
                $(SLO)$/filedlg.obj 	\
                $(SLO)$/filedlg2.obj	\
                $(SLO)$/prnsetup.obj	\
                $(SLO)$/printdlg.obj	\
                $(SLO)$/colctrl.obj 	\
                $(SLO)$/colrdlg.obj 	\
                $(SLO)$/property.obj	\
                $(SLO)$/wizdlg.obj		\
                $(SLO)$/mcvmath.obj		\
                $(SLO)$/logindlg.obj

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk
