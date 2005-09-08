#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: unxscoi.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:59:55 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************


# mak file fuer wnticci
ASM=
AFLAGS=

CDEFS+=-D_PTHREADS -D_REENTRANT 
CDEFS+=-D_STD_NO_NAMESPACE -D_VOS_NO_NAMESPACE -D_UNO_NO_NAMESPACE -DX86 -DNEW_SOLAR
# kann c++ was c braucht??

# architecture dependent flags for the C and C++ compiler that can be changed by
# exporting the variable ARCH_FLAGS="..." in the shell, which is used to start build
ARCH_FLAGS*=-mpentium

CXX*=/nw386/dev/s/solenv/unxscoi/bin/g++
CC*=/nw386/dev/s/solenv/unxscoi/bin/gcc
CFLAGS=-c $(INCLUDE)
CFLAGSCC=$(ARCH_FLAGS)
CFLAGSEXCEPTIONS=-fexceptions
CFLAGS_NO_EXCEPTIONS=-fno-exceptions
CFLAGSCXX=-fguiding-decls -frtti $(ARCH_FLAGS)
CFLAGSCXX+= -Wno-ctor-dtor-privacy
PICSWITCH:=-fPIC
CFLAGSOBJGUIST=$(PICSWITCH)
CFLAGSOBJCUIST=$(PICSWITCH)
CFLAGSOBJGUIMT=$(PICSWITCH)
CFLAGSOBJCUIMT=$(PICSWITCH)
CFLAGSSLOGUIMT=$(PICSWITCH)
CFLAGSSLOCUIMT=$(PICSWITCH)
CFLAGSPROF=
CFLAGSDEBUG=-g
CFLAGSDBGUTIL=
CFLAGSOPT=-O2
CFLAGSNOOPT=-O
CFLAGSOUTOBJ=-o

STATIC		= -Wl,-Bstatic
DYNAMIC		= -Wl,-Bdynamic

THREADLIB=
LINK=/nw386/dev/s/solenv/unxscoi/bin/gcc
LINKFLAGS=
# SCO hat grosse Probleme mit fork/exec und einigen shared libraries
# rsc2 muss daher statisch gelinkt werden
.IF "$(PRJNAME)"=="rsc"
LINKFLAGSAPPGUI=-L/nw386/dev/s/solenv/unxscoi/lib $(STATIC) -lpthread_init $(DYNAMIC)
LINKFLAGSAPPCUI=-L/nw386/dev/s/solenv/unxscoi/lib $(STATIC) -lpthread_init $(DYNAMIC)
.ELSE
LINKFLAGSAPPGUI=-L/nw386/dev/s/solenv/unxscoi/lib -lpthread_init
LINKFLAGSAPPCUI=-L/nw386/dev/s/solenv/unxscoi/lib -lpthread_init
.ENDIF
LINKFLAGSSHLGUI=-G -W,l,-Bsymbolic
LINKFLAGSSHLCUI=-G -W,l,-Bsymbolic
LINKFLAGSTACK=
LINKFLAGSPROF=
LINKFLAGSDEBUG=-g
LINKFLAGSOPT=

# standard C++ Library
#
# das statische dazulinken der libstdc++ macht jede shared library um 50k
# (ungestrippt) oder so groesser, auch wenn sie ueberhaupt nicht gebraucht
# wird. Da muessen wir uns was besseres ueberlegen.
STDLIBCPP=-Wl,-Bstatic -lstdc++ -Wl,-Bdynamic

# reihenfolge der libs NICHT egal!
STDOBJGUI=
STDSLOGUI=
STDOBJCUI=
STDSLOCUI=
STDLIBGUIST=-lXext -lX11 -ldl -lsocket -lm
STDLIBCUIST=-ldl -lsocket -lm
.IF "$(PRJNAME)"=="rsc"
STDLIBGUIMT=-lXext -lX11 $(STATIC) -lpthread $(DYNAMIC) -ldl -lsocket -lm
STDLIBCUIMT=$(STATIC) -lpthread $(DYNAMIC) -ldl -lsocket -lm 
.ELSE
STDLIBGUIMT=-lXext -lX11 -lpthread -ldl -lsocket -lm
STDLIBCUIMT=-lpthread -ldl -lsocket -lm 
.ENDIF
#STDSHLGUIMT=-lXext -lX11 -lpthread -ldl -lsocket -lm
#STDSHLCUIMT=-lpthread -ldl -lsocket -lm 

STDLIBCPP= -lstdc++
SHLLINKARCONLY=yes

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

DLLPOSTFIX=ci
DLLPRE=lib
DLLPOST=.so

LDUMP=cppfilt /b /n /o /p

.IF "$(WORK_STAMP)"!="MIX364"
DLLPOSTFIX=ci
.ELSE
DLLPOSTFIX=
.ENDIF
