#*************************************************************************
#
#   $RCSfile: unitools.mk,v $
#
#   $Revision: 1.12 $
#
#   last change: $Author: hjs $ $Date: 2001-11-15 16:11:34 $
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
.IF "$(GUI)"!="UNX"
.IF "$(GUI)"!="MAC"
AWK*=awk
SORT*=sort
SED*=sed
TYPE*=type
RENAME*=ren
COPY*=copy
COPYUPDATE=/u
COPYRECURSE=/s
MKDIR*=call mkdir
MKDIRHIER=$(MKDIR) 
ECHON*=echos
.IF "$(GUI)"=="OS2"
GREP*=echo which grep ?
FIND*=$(SOLARROOT)$/util$/os2$/find.exe
LS*=dir /b
.ENDIF
.IF "$(GUI)"=="WNT"
GREP*=$(ENV_TOOLS)$/grep32.exe
FIND*=$(BUILD_TOOLS)$/find.exe
LS*=$(BUILD_TOOLS)$/ls.exe
GNUCOPY*=$(BUILD_TOOLS)$/cp.exe
GNUMAKE*=$(ENV_TOOLS)$/gnumake.exe
#wraper for solenv\bin\mkdir.pl to fix mkdir /p problem
MKDIR=+$(SOLARENV)$/bin$/mkdir.btm
TOUCH*=$(BUILD_TOOLS)$/touch.exe
PERL*:=call perl5.btm
SCP_CHECK_TOOL=checkscp.exe

#signing for win32 only
SIGNCODE=$(BUILD_TOOLS)$/PackSign$/signcode
CABARC=$(BUILD_TOOLS)$/PackSign$/cabarc
SIGNTOOL=$(BUILD_TOOLS)$/signtool$/signtool
.ENDIF
.IF "$(GUI)"=="WIN"
# using 32-bit tools 
GREP=$(SOLARROOT)$/util$/nt$/grep32.exe
FIND=$(SOLARROOT)$/util$/nt$/find.exe
LS=$(SOLARROOT)$/util$/nt$/ls.exe
MKDIRHIER=echo v | xcopy nul 
.ENDIF
.ELSE
SED*=sed
AWK=awk
SORT*=sort
PERL=perl
TYPE=cat
COPY=cp -f
COPYRECURSE=-r
GNUCOPY=cp
TOUCH=touch
RENAME=mv
MKDIR=mkdir
MKDIRHIER=mkdir -p
GREP=grep
FIND=find
LS=ls
ECHON=echo -n
.ENDIF
.ELSE
SED*=sed
AWK=awk
SORT*=sort
PERL=perl
TYPE=cat
COPY=cp -f
COPYRECURSE=-r
.IF "$(OS)"=="SOLARIS"
GNUCOPY=gnucp
GNUPATCH=gnupatch
.ELSE			# "$(OS)"=="SOLARIS"
GNUCOPY=cp
GNUPATCH=patch
.ENDIF			# "$(OS)"=="SOLARIS"
.IF "$(OS)"=="LINUX"
GNUMAKE=make
.ELSE			# "$(OS)"=="SOLARIS"
GNUMAKE=gmake
.ENDIF			# "$(OS)"=="SOLARIS"
TOUCH=touch
RENAME=mv
MKDIR=mkdir
MKDIRHIER=mkdir -p
GREP=grep
FIND=find
LS=ls
ECHON=echo -n
.ENDIF

RM+=$(RMFLAGS)

.IF "$(GUI)"=="UNX"
SCP_CHECK_TOOL=checkscp
NULLDEV=/dev/null
.ELIF "$(GUI)"=="MAC"
NULLDEV=/dev/null
.ELSE
NULLDEV=nul
.ENDIF

