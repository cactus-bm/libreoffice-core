#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.17 $
#
#   last change: $Author: kz $ $Date: 2007-05-10 15:01:23 $
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

PRJ=..$/..

PRJNAME=sysui
TARGET=solaris

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Product Version Information ----------------------------------

.INCLUDE :  ../productversion.mk

# --- Files --------------------------------------------------------
 
.IF "$(OS)"=="SOLARIS"

PKGFILES = $(BIN)$/pkg$/{$(PRODUCTLIST)}-desktop-integration.tar.gz
   
PKGDATESTRING = $(shell date -u +%Y.%m.%d)
PKGARCH=sparc,i386

FASPAC=`test -f $(SOLARBINDIR)/faspac-so.sh && echo "/sbin/sh" || echo "echo"`

.ENDIF
    
# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(OS)"=="SOLARIS"

ALLTAR : $(PKGFILES) 

# --- pkginfo ----------------------------------------------------

# Copy the pkginfo file to $(MISC) 
$(MISC)/{$(PRODUCTLIST)}/{pkginfo depend} : $$(@:f) ../productversion.mk makefile.mk
    @$(MKDIRHIER) $(@:d)
    @cat $(@:f) | tr -d "\015" | sed -e "s/%PRODUCTNAME/$(PRODUCTNAME.$(@:d:d:f)) $(PRODUCTVERSION.$(@:d:d:f))/g" -e "s/%pkgprefix/$(@:d:d:f:s/.//)/" > $@

# --- space, postinstall & mailcap ---------------------------------

# Copy the prototype file to $(MISC)
$(MISC)/{$(PRODUCTLIST)}/{space postinstall mailcap} : $$(@:f) ../productversion.mk
    @$(MKDIRHIER) $(@:d)
    @cat $(@:f) | tr -d "\015" | sed -e "s/%PREFIX/$(UNIXFILENAME.$(@:d:d:f))/g" -e "s_%SOURCE_$(MISC)/$(@:d:d:f)_g" > $@

# --- checkinstall & copyright--------------------------------

# Copy the checkinstall and copyright file to $(MISC) 
$(MISC)/{$(PRODUCTLIST)}/{checkinstall copyright} : $$(@:f)
    @$(MKDIRHIER) $(@:d)
    @cat $(@:f) | tr -d "\015" > $@

# --- prototype ---------------------------------------------------

# Copy the prototype file to $(MISC)
$(MISC)/{$(PRODUCTLIST)}$/prototype : $$(@:f) ../productversion.mk $(COMMONMISC)$/$$(@:d:d:f)$/cdelauncherlist makefile.mk
    @$(MKDIRHIER) $(@:d)
    cat $(@:f) | tr -d "\015" | sed -e "s/%PREFIX/$(UNIXFILENAME.$(@:d:d:f))/g" -e "s_%SOURCE_$(COMMONMISC)/$(@:d:d:f)_g" -e "s/%ICONPREFIX/$(ICONPREFIX.$(@:d:d:f))/g" > $@
    pkgproto $(COMMONMISC)$/$(@:d:d:f)/types=usr/dt/appconfig/types | awk '{ printf "%s %s %s 0%d%d root bin\n",$$1, $$2, $$3, $$4/100, $$4%10*11 }' >> $@

# --- packaging ---------------------------------------------------

$(PKGFILES) : $(MISC)/{$(PRODUCTLIST)}/{checkinstall copyright space pkginfo depend postinstall mailcap} makefile.mk
$(PKGFILES) : $(MISC)$/{$(PRODUCTLIST)}$/prototype
    @-$(RM) $(BIN)$/$(@:f)
    @$(MKDIRHIER) $(@:d)
    pkgmk -r . -f $(MISC)$/$(@:b:b:s/-/ /:1)$/prototype -o -d $(PKGDIR) ARCH=$(PKGARCH) VERSION=$(PKGVERSION.$(@:b:s/-/ /:1)),REV=$(PKGREV).$(PKGDATESTRING)
    $(FASPAC) $(SOLARBINDIR)/faspac-so.sh -a -d $(PKGDIR) $(@:b:b:s/-/ /:1:s/.//)-desktop-integratn
    @tar -cf - -C $(PKGDIR) $(@:b:b:s/-/ /:1:s/.//)-desktop-integratn | gzip > $@
    @rm -rf $(PKGDIR)/$(@:b:b:s/-/ /:1:s/.//)-desktop-integratn

.ENDIF
