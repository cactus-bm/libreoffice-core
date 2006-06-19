#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: unxlngppc.mk,v $
#
#   $Revision: 1.24 $
#
#   last change: $Author: hr $ $Date: 2006-06-19 17:15:27 $
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

# Makefile for linux-ppc 
# Christer Gustavsson <cg@nocrew.org> 
ASM= 
AFLAGS= 

SOLAR_JAVA*= 
JAVAFLAGSDEBUG=-g 

# filter for supressing verbose messages from linker
#not needed at the moment
#LINKOUTPUT_FILTER=" |& $(SOLARENV)$/bin$/msg_filter"

# _PTHREADS is needed for the stl 
CDEFS+=$(PTHREAD_CFLAGS) -DGLIBC=2 -D_PTHREADS -D_REENTRANT -DNEW_SOLAR -D_USE_NAMESPACE=1 -DSTLPORT_VERSION=400 -DPOWERPC -DPPC 

# enable visibility define in "sal/types.h"
.IF "$(HAVE_GCC_VISIBILITY_FEATURE)" == "TRUE"
CDEFS += -DHAVE_GCC_VISIBILITY_FEATURE
.ENDIF # "$(HAVE_GCC_VISIBILITY_FEATURE)" == "TRUE"

# this is a platform with JAVA support
.IF "$(SOLAR_JAVA)"!="" 
JAVADEF=-DSOLAR_JAVA 
.IF "$(debug)"=="" 
JAVA_RUNTIME=-ljava 
.ELSE 
JAVA_RUNTIME=-ljava_g 
.ENDIF 
.ENDIF 

# architecture dependent flags for the C and C++ compiler that can be changed by
# exporting the variable ARCH_FLAGS="..." in the shell, which is used to start build
ARCH_FLAGS*=

# name of C++ Compiler
CXX*=g++ 
# name of C Compiler
CC*=gcc

# source code is still not signed versus unsigned char clean 
CFLAGS=-fsigned-char -nostdinc -c $(INCLUDE) -I$(SOLARENV)/unxlngppc/usr/include 
CFLAGSCC=-fsigned-char $(ARCH_FLAGS)

# Flags for enabling exception handling
CFLAGSEXCEPTIONS=-fexceptions 
# Flags for disabling exception handling
CFLAGS_NO_EXCEPTIONS=-fno-exceptions 

CFLAGSCXX= -pipe -frtti $(ARCH_FLAGS)
PICSWITCH:=-fPIC

#Note: the build is not consistent in that it links static librtaries
# libraries into dynamic libraries in places, so use -fPIC throughout
# until fixed.

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

# Compiler flags for enabling optimizations
CFLAGSOPT=-O2  -fno-schedule-insns -fno-strict-aliasing -fno-schedule-insns2

# Compiler flags for disabling optimizations
CFLAGSNOOPT=-O0

# Compiler flags for describing the output path
CFLAGSOUTOBJ=-o 

CFLAGSWARNCC=
CFLAGSWARNCXX=$(CFLAGSWARNCC) -Wno-ctor-dtor-privacy
# -Wshadow does not work for C with nested uses of pthread_cleanup_push:
CFLAGSWALLCC=-Wall -Wextra -Wendif-labels
CFLAGSWALLCXX=$(CFLAGSWALLCC) -Wshadow -Wno-ctor-dtor-privacy
CFLAGSWERRCC=-Werror

# switches for dynamic and static linking
STATIC = -Wl,-Bstatic 
DYNAMIC = -Wl,-Bdynamic 

# name of linker
LINK*=$(CXX)
LINKC*=$(CC)
# default linker flags
LINKFLAGS=-Wl,-rpath,\''$$ORIGIN'\'


# linker flags for linking applications
LINKFLAGSAPPGUI= -Wl,-export-dynamic 
LINKFLAGSAPPCUI= -Wl,-export-dynamic 
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
.ENDIF # "$(NO_BSYMBOLIC)"=="" 

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

LIBSTLPORT=$(DYNAMIC) -lstlport_gcc
LIBSTLPORTST=$(STATIC) -lstlport_gcc $(DYNAMIC)

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
DLLPOSTFIX=lp
DLLPRE=lib 
DLLPOST=.so 
