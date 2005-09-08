#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: sc.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:38:45 $
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


.INCLUDE :  sv.mk

# MakeDepn nicht das segdefs.hxx und die PCHs auswerten lassen
# PCHs werden nur in den ..._1st Verzeichnissen aufgenommen
EXCDEPN=segdefs.hxx
EXCDEPNPCH=core_pch.hxx;filt_pch.hxx;ui_pch.hxx
# done by protecting define now
.IF "$(GUI)" != "MAC"
#MKDEPFLAGS+=-E:$(EXCDEPN);$(EXCDEPNPCH)
.ELSE
#MKDEPFLAGS+=-E:"$(EXCDEPN);$(EXCDEPNPCH)"
.ENDIF

.IF "$(GUI)" == "WIN"
LINKFLAGSAPPGUI=/A:128
LIBFLAGS=$(LIBFLAGS) /PAGE:128
.ENDIF


.IF "$(GUI)$(COM)" == "WINMSC"
.IF "$(optimize)" != ""
CFLAGS=$(CFLAGS) -OV9
.ENDIF

# kein -GEfs wegen dann fehlender inlinings
CFLAGSOBJGUIST=-Alfd -GA -GEd

# Wer hatte dieses denn aus welchem Grunde eingebaut ????
#CFLAGSOBJGUIST=-Alfd -GA
# !ENDIF
.ENDIF
