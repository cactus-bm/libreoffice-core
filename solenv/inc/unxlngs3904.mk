#*************************************************************************
#
#   $RCSfile: unxlngs3904.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: hr $ $Date: 2005-02-11 15:30:28 $
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

# mk file for linux s390 using gcc 3.X
ASM=
AFLAGS=

SOLAR_JAVA*=TRUE
JAVAFLAGSDEBUG=-g

# filter for supressing verbose messages from linker
#not needed at the moment
#LINKOUTPUT_FILTER=" |& $(SOLARENV)$/bin$/msg_filter"

# _PTHREADS is needed for the stl
CDEFS+=-DGLIBC=2 -D_PTHREADS -D_REENTRANT -DNEW_SOLAR -D_USE_NAMESPACE=1 -DSTLPORT_VERSION=400

# this is a platform with JAVA support
.IF "$(SOLAR_JAVA)"!=""
JAVADEF=-DSOLAR_JAVA
.IF "$(debug)"==""
JAVA_RUNTIME=-ljava
.ELSE
JAVA_RUNTIME=-ljava_g
.ENDIF
.ENDIF 

# name of C++ Compiler
CXX*=g++-3.2
# name of C Compiler
CC*=gcc-3.2

CFLAGS+=-fsigned-char -fmessage-length=0 -c $(INCLUDE)

# flags for the C++ Compiler
CFLAGSCC= -fsigned-char -pipe 

# Flags for enabling exception handling
CFLAGSEXCEPTIONS=-fexceptions -fno-enforce-eh-specs

# Flags for disabling exception handling
CFLAGS_NO_EXCEPTIONS=-fno-exceptions

CFLAGSCXX= -fsigned-char -pipe -fno-rtti 
#CFLAGSCXX= -fsigned-char -pipe
CFLAGSCXX+= -Wno-ctor-dtor-privacy
PICSWITCH:=-fPIC

# Compiler flags for compiling static object in single threaded environment with graphical user interface
CFLAGSOBJGUIST=$(PICSWITCH)
# Compiler flags for compiling static object in single threaded environment with character user interface
CFLAGSOBJCUIST=$(PICSWITCH)
# Compiler flags for compiling static object in multi threaded environment with graphical user interface
CFLAGSOBJGUIMT=$(PICSWITCH)
# Compiler flags for compiling static object in multi threaded environment with character user interface
CFLAGSOBJCUIMT=$(PICSWITCH)
# Compiler flags for compiling shared object in multi threaded environment with graphical user interface
CFLAGSSLOGUIMT=$(PICSWITCH)
# Compiler flags for compiling shared object in multi threaded environment with character user interface
CFLAGSSLOCUIMT=$(PICSWITCH)

# Compiler flags for profiling
CFLAGSPROF=

# Compiler flags for debugging
CFLAGSDEBUG=-g
CFLAGSDBGUTIL=

# Compiler flags for enabling optimazations
#CFLAGSOPT=-O2 -fno-schedule-insns -fno-strict-aliasing -fno-schedule-insns2
CFLAGSOPT=-O2 -fno-strict-aliasing

# Compiler flags for disabling optimazations
# don't change - require dto work around optimization bugs
CFLAGSNOOPT=-O0

# Compiler flags for discibing the output path
CFLAGSOUTOBJ=-o

# Enable all warnings
CFLAGSWALL=-Wall

# Set default warn level
CFLAGSDFLTWARN=

# switches for dynamic and static linking
STATIC		= -Wl,-Bstatic
DYNAMIC		= -Wl,-Bdynamic

# name of linker
LINK*=g++-3.2
# default linker flags
LINKFLAGS=

# linker flags for linking applications
LINKFLAGSAPPGUI= -Wl,-export-dynamic -Wl,--noinhibit-exec
LINKFLAGSAPPCUI= -Wl,-export-dynamic -Wl,--noinhibit-exec

# linker flags for linking shared libraries
LINKFLAGSSHLGUI= -shared
LINKFLAGSSHLCUI= -shared

LINKFLAGSTACK=
LINKFLAGSPROF=
LINKFLAGSDEBUG=-g
LINKFLAGSOPT=

.IF "$(NO_BSYMBOLIC)"==""
.IF "$(PRJNAME)" != "envtest"
LINKFLAGSSHLGUI+=-Wl,-Bsymbolic
LINKFLAGSSHLCUI+=-Wl,-Bsymbolic
.ENDIF
.ENDIF				# "$(NO_BSYMBOLIC)"==""

LINKVERSIONMAPFLAG=-Wl,--version-script

SONAME_SWITCH=-Wl,-h

# Sequence of libs does matter !

STDLIBCPP=-lstdc++

# default objectfilenames to link
STDOBJGUI=
STDSLOGUI=
STDOBJCUI=
STDSLOCUI=

# libraries for linking applications
STDLIBCUIST=-ldl -lm
STDLIBGUIMT=-lX11 -ldl -lpthread -lm
STDLIBCUIMT=-ldl -lpthread -lm
STDLIBGUIST=-lX11 -ldl -lm
# libraries for linking shared libraries
STDSHLGUIMT=-lX11 -lXext -ldl -lpthread -lm
STDSHLCUIMT=-ldl -lpthread -lm

LIBSTLPORT=$(DYNAMIC) -lstlport_gcc -lstdc++
LIBSTLPORTST=$(STATIC) -lstlport_gcc $(DYNAMIC)

#FILLUPARC=$(STATIC) -lsupc++ $(DYNAMIC)

# name of library manager
LIBMGR=ar
LIBFLAGS=-r

# tool for generating import libraries
IMPLIB=
IMPLIBFLAGS=

MAPSYM=
MAPSYMFLAGS=

RC=irc
RCFLAGS=-fo$@ $(RCFILES)
RCLINK=
RCLINKFLAGS=
RCSETVERSION=

# platform specific identifier for shared libs
DLLPOSTFIX=l3
DLLPRE=lib
DLLPOST=.so
