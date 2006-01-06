#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: kz $ $Date: 2006-01-06 13:12:52 $
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

PRJ = ..$/..$/..

PRJNAME	= lingucomponent
TARGET	= spell
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

.IF "$(MYSPELLLIB)"==""
.ELSE
@echo "Build Hunspell instead of system Myspell."
.ENDIF

.IF "$(HUNSPELLLIB)"==""
.IF "$(GUI)"=="UNX"
HUNSPELLLIB=-lhunspell
.ENDIF # unx
.IF "$(GUI)"=="WNT"
HUNSPELLLIB=hunspell.lib
.ENDIF # wnt
.ENDIF

.IF "$(ULINGULIB)"==""
.IF "$(GUI)"=="UNX"
ULINGULIB=-lulingu
.ENDIF # unx
.IF "$(GUI)"=="WNT"
ULINGULIB=libulingu.lib
.ENDIF # wnt
.ENDIF




#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

UNOUCRDEP=	$(SOLARBINDIR)$/types.rdb
UNOUCRRDB=	$(SOLARBINDIR)$/types.rdb

UNOTYPES=\
    com.sun.star.linguistic2.DictionaryEvent\
    com.sun.star.linguistic2.DictionaryEventFlags\
    com.sun.star.linguistic2.DictionaryListEvent\
    com.sun.star.linguistic2.DictionaryListEventFlags\
    com.sun.star.linguistic2.DictionaryType\
    com.sun.star.linguistic2.LinguServiceEventFlags\
    com.sun.star.linguistic2.SpellFailure\
    com.sun.star.linguistic2.XDictionary\
    com.sun.star.linguistic2.XDictionary1\
    com.sun.star.linguistic2.XDictionaryEntry\
    com.sun.star.linguistic2.XDictionaryEventListener\
    com.sun.star.linguistic2.XDictionaryList\
    com.sun.star.linguistic2.XDictionaryListEventListener\
    com.sun.star.linguistic2.XHyphenatedWord\
    com.sun.star.linguistic2.XHyphenator\
    com.sun.star.linguistic2.XLinguServiceEventBroadcaster\
    com.sun.star.linguistic2.XLinguServiceEventListener\
    com.sun.star.linguistic2.XLinguServiceManager\
    com.sun.star.linguistic2.XMeaning\
    com.sun.star.linguistic2.XPossibleHyphens\
    com.sun.star.linguistic2.XSearchableDictionaryList\
    com.sun.star.linguistic2.XSpellAlternatives\
    com.sun.star.linguistic2.XSpellChecker\
    com.sun.star.linguistic2.XSupportedLocales\
    com.sun.star.linguistic2.XThesaurus


.IF "$(SYSTEM_HUNSPELL)" != "YES"
CXXFLAGS += -I..$/hunspell -I..$/..$/lingutil
CFLAGSCXX += -I..$/hunspell -I..$/..$/lingutil
CFLAGSCC += -I..$/hunspell  -I..$/..$/lingutil
.ELSE
CXXGLAGS += $(HUNSPELL_CFLAGS)
CFLAGSCXX += $(HUNSPELL_CFLAGS)
CFLAGSCC += $(HUNSPELL_CFLAGS)
.ENDIF

.IF "$(header)" == ""

EXCEPTIONSFILES=	\
        $(SLO)$/sprophelp.obj\
        $(SLO)$/sspellimp.obj

SLOFILES=	\
        $(SLO)$/sprophelp.obj\
        $(SLO)$/sreg.obj\
        $(SLO)$/sspellimp.obj


SHL1TARGET= $(TARGET)$(UPD)$(DLLPOSTFIX)

SHL1STDLIBS= \
        $(CPPULIB) 	 \
        $(CPPUHELPERLIB) 	 \
        $(UNOLIB)		\
        $(VOSLIB)		\
        $(TOOLSLIB)		\
        $(SVTOOLLIB)	\
        $(SVLLIB)		\
        $(VCLLIB)		\
        $(SFXLIB)		\
        $(SALLIB)		\
        $(UCBHELPERLIB)	\
        $(UNOTOOLSLIB)	\
        $(LNGLIB) \
                $(HUNSPELLLIB)

.IF "$(SYSTEM_HUNSPELL)" != "YES"
SHL1STDLIBS+=   $(ULINGULIB)
.ENDIF

# build DLL
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1IMPLIB=		i$(TARGET)
SHL1DEPN=		$(SHL1LIBS)
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def
.IF "$(OS)"!="MACOSX"
SHL1VERSIONMAP= $(TARGET).map
.ENDIF

# build DEF file
DEF1NAME	 =$(SHL1TARGET)
#DEF1DEPN    =$(MISC)$/$(SHL1TARGET).flt
#DEFLIB1NAME =$(TARGET)
#DEF1DES     =Linguistic2 main DLL
DEF1EXPORTFILE=	exports.dxp

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

