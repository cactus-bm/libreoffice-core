#*************************************************************************
#
#   $RCSfile: unxsogi.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: kz $ $Date: 2005-03-03 17:37:30 $
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

# mk file for unxsogi
ASM=/usr/ccs/bin/as
AFLAGS=-P

SOLAR_JAVA=TRUE
JAVAFLAGSDEBUG=-g

# _PTHREADS is needed for the stl
CDEFS+=-D_PTHREADS -D_REENTRANT -DSYSV -DSUN -DSUN4 -D_POSIX_PTHREAD_SEMANTICS -DSTLPORT_VERSION=400 -D_USE_NAMESPACE=1 


.IF "$(SOLAR_JAVA)"!=""
JAVADEF=-DSOLAR_JAVA
.IF "$(debug)"==""
JAVA_RUNTIME=-ljava
.ELSE
JAVA_RUNTIME=-ljava_g
.ENDIF
.ENDIF 

CXX*=g++
CC*=gcc
CFLAGS=-c $(INCLUDE)
CFLAGSCC= -pipe

CFLAGSEXCEPTIONS=-fexceptions
CFLAGS_NO_EXCEPTIONS=-fno-exceptions

CFLAGSCXX= -pipe
PICSWITCH:=-fPIC

CFLAGSOBJGUIST=
CFLAGSOBJCUIST=
CFLAGSOBJGUIMT=
CFLAGSOBJCUIMT=
CFLAGSSLOGUIMT=$(PICSWITCH)
CFLAGSSLOCUIMT=$(PICSWITCH)
CFLAGSPROF=
CFLAGSDEBUG=-g
CFLAGSDBGUTIL=
CFLAGSOPT=-O2
CFLAGSNOOPT=
CFLAGSOUTOBJ=-o

STATIC		= -Wl,-Bstatic
DYNAMIC		= -Wl,-Bdynamic

LINK=gcc
LINKFLAGS=
LINKFLAGSAPPGUI=-Wl,-export-dynamic
LINKFLAGSSHLGUI=-shared
LINKFLAGSAPPCUI=-Wl,-export-dynamic
LINKFLAGSSHLCUI=-shared
LINKFLAGSTACK=
LINKFLAGSPROF=
LINKFLAGSDEBUG=-g
LINKFLAGSOPT=

LINKVERSIONMAPFLAG=-Wl,--version-script

# enable visibility define in "sal/types.h"
.IF "$(HAVE_GCC_VISIBILITY_FEATURE)" == "TRUE"
CDEFS += -DHAVE_GCC_VISIBILITY_FEATURE
.ENDIF # "$(HAVE_GCC_VISIBILITY_FEATURE)" == "TRUE"

# Reihenfolge der libs NICHT egal!

STDLIBCPP=-lstdc++

STDOBJVCL=$(L)$/salmain.o
STDOBJGUI=
STDSLOGUI=
STDOBJCUI=
STDSLOCUI=

STDLIBGUIST=$(DYNAMIC) -lstdc++ -lm
STDLIBCUIST=$(DYNAMIC) -lstdc++ -lm
STDLIBGUIMT=$(DYNAMIC) -lpthread -lthread -lstdc++ -lm
STDLIBCUIMT=$(DYNAMIC) -lpthread -lthread -lstdc++ -lm
# libraries for linking shared libraries
STDSHLGUIST=$(DYNAMIC) -lstdc++ -lm
STDSHLCUIST=$(DYNAMIC) -lstdc++ -lm
STDSHLGUIMT=$(DYNAMIC) -lpthread -lthread -lstdc++ -lm
STDSHLCUIMT=$(DYNAMIC) -lpthread -lthread -lstdc++ -lm

STDLIBGUIST+=-lX11
STDLIBGUIMT+=-lX11

LIBSTLPORT=$(DYNAMIC) -lstlport_gcc
LIBSTLPORTST=$(STATIC) -lstlport_gcc $(DYNAMIC)

LIBMGR=ar
LIBFLAGS=-r
# LIBEXT=.so

IMPLIB=
IMPLIBFLAGS=

MAPSYM=
MAPSYMFLAGS=

RC=irc
RCFLAGS=-fo$@ $(RCFILES)
RCLINK=
RCLINKFLAGS=
RCSETVERSION=

DLLPOSTFIX=sogi
DLLPRE=lib
DLLPOST=.so
