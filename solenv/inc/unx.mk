#*************************************************************************
#
#   $RCSfile: unx.mk,v $
#
#   $Revision: 1.23 $
#
#   last change: $Author: hr $ $Date: 2004-02-04 12:37:45 $
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

# --- Unix-Environment ---------------------------------------

.IF "$(GUI)" == "UNX"

# --- Ole2 Behandlung -------------

OLE2=
OLE2DEF=


# Dieses Define gilt fuer alle  WNT- Plattformen
# fuer Compiler, wo kein Java unterstuetzt wird, muss dies ge'undeft werden
#JAVADEF=-DSOLAR_JAVA

# --- Compiler ---

.IF "$(COM)$(OS)$(CPU)" == "C50SOLARISS"
.INCLUDE : unxsols2.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "C50SOLARISI"
.INCLUDE : unxsoli2.mk
.ENDIF

.IF "$(OS)$(CPU)$(COMEX)" == "SOLARISS3"
.INCLUDE : unxsols3.mk
.ENDIF

.IF "$(OS)$(CPU)$(COMEX)" == "SOLARISI3"
.INCLUDE : unxsoli3.mk
.ENDIF

.IF "$(OS)$(CPU)$(COMEX)" == "SOLARISS4"
.INCLUDE : unxsols4.mk
.ENDIF

.IF "$(OS)$(CPU)$(COMEX)" == "SOLARISI4"
.INCLUDE : unxsoli4.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCSOLARISS"
.INCLUDE : unxsogs.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCSCOI"
.INCLUDE : unxscoi.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)$(GLIBC)" == "GCCLINUXI"
.INCLUDE : unxlnxi.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)$(GLIBC)" == "GCCLINUXP2"
.INCLUDE : unxlngp.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCLINUXS"
.INCLUDE : unxlngs.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC295LINUXI2REDHAT60"
.INCLUDE : unxlngi3.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC300LINUXI2REDHAT60"
.INCLUDE : unxlngi4.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC322LINUXI2REDHAT60"
.INCLUDE : unxlngi5.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC300LINUXX2REDHAT60"
.INCLUDE : unxlngx4.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC295LINUXP2REDHAT60"
.INCLUDE : unxlngppc.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC300LINUXP2REDHAT60"
.INCLUDE : unxlngppc4.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)$(GLIBC)" == "GCCC300LINUX32REDHAT60"
.INCLUDE : unxlngs3904.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)$(GLIBC)" == "GCCLINUXR2REDHAT60"
.INCLUDE : unxlngr.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "ACCHPUXR"
.INCLUDE : unxhpxr.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCHPUXR"
.INCLUDE : unxhpgr.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCNETBSDA"
.INCLUDE : unxbsda.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "ICCAIXP"
.INCLUDE : unxaixp.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)" == "GCCC295NETBSDI"
.INCLUDE : unxbsdi.mk
.ENDIF

.IF "$(COM)$(CVER)$(OS)$(CPU)" == "GCCC300NETBSDI"
.INCLUDE : unxbsdi2.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCNETBSDS"
.INCLUDE : unxbsds.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCFREEBSDI"
.INCLUDE : unxfbsdi.mk
.ENDIF


.IF "$(COM)$(OS)$(CPU)" == "C730IRIXM"
.INCLUDE : unxirxm3.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCIRIXM"
.INCLUDE : unxirgm.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "C710IRIXM"
.INCLUDE : unxirxm.mk
.ENDIF

.IF "$(COM)$(OS)$(CPU)" == "GCCMACOSXP"
.INCLUDE : unxmacxp.mk
.ENDIF

# --- *ix  Allgemein ---
HC=hc
HCFLAGS=
PATH_SEPERATOR*=:
CDEFS+=-D__DMAKE

CDEFS+=-DUNIX

# fuer linux: bison -y -d
YACC*=yacc
YACCFLAGS*=-d -t

EXECPOST=
SCPPOST=.ins
DLLDEST=$(LB)
SONAME_SWITCH*=-h

.IF "$(UNIXVERSIONNAMES)"!=""
DLLPOST!:=$(DLLPOST).$($(UNIXVERSIONNAMES)_MAJOR).$($(UNIXVERSIONNAMES)_MINOR).$($(UNIXVERSIONNAMES)_MICRO)
.ENDIF			# "$(UNIXVERSIONNAMES)"!=""
.ENDIF			# "$(GUI)" == "UNX"

