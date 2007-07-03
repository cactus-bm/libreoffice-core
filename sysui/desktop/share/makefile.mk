#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.20 $
#
#   last change: $Author: rt $ $Date: 2007-07-03 12:58:02 $
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
TARGET=desktopshare

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Product Version Information ----------------------------------

.INCLUDE :  ../productversion.mk
.INCLUDE :  packtools.mk

# --- Files --------------------------------------------------------

.IF "$(WITH_LANG)"!=""
ULFDIR:=$(COMMONMISC)$/$(TARGET)
.ELSE			# "$(WITH_LANG)"!=""
ULFDIR:=.
.ENDIF			# "$(WITH_LANG)"!=""

ULFFILES= \
    documents.ulf \
    launcher_comment.ulf \
    launcher_genericname.ulf \
    launcher_name.ulf

LAUNCHERLIST = writer calc draw impress math base printeradmin qstart extension
LAUNCHERDEPN = ../menus/{$(LAUNCHERLIST)}.desktop

LAUNCHERFLAGFILE = $(COMMONMISC)/$(TARGET)/xdg.flag

MIMELIST = \
    text \
    text-template \
    spreadsheet \
    spreadsheet-template \
    drawing \
    drawing-template \
    presentation \
    presentation-template \
    formula \
    master-document \
    oasis-text \
    oasis-text-template \
    oasis-spreadsheet \
    oasis-spreadsheet-template \
    oasis-drawing \
    oasis-drawing-template \
    oasis-presentation \
    oasis-presentation-template \
    oasis-formula \
    oasis-master-document \
    oasis-database \
    oasis-web-template \
    extension

MIMEICONLIST = \
    oasis-text \
    oasis-text-template \
    oasis-spreadsheet \
    oasis-spreadsheet-template \
    oasis-drawing \
    oasis-drawing-template \
    oasis-presentation \
    oasis-presentation-template \
    oasis-formula \
    oasis-master-document \
    oasis-database \
    oasis-web-template \
    text \
    text-template \
    spreadsheet \
    spreadsheet-template \
    drawing \
    drawing-template \
    presentation \
    presentation-template \
    formula \
    master-document \
    database \
    extension

ICONDEPN = \
    ../icons/hicolor/{16x16 32x32 48x48}/apps/{$(LAUNCHERLIST:s/qstart//)}.png \
    ../icons/hicolor/{16x16 32x32 48x48}/mimetypes/{$(MIMEICONLIST)}.png \
    ../icons/locolor/{16x16 32x32}/apps/{$(LAUNCHERLIST:s/qstart//)}.png \
    ../icons/locolor/{16x16 32x32}/mimetypes/{$(MIMEICONLIST)}.png

.IF "$(PKGFORMAT)"!="$(PKGFORMAT:s/rpm//)"
SPECFILES = $(MISC)$/{redhat suse freedesktop mandriva}-menus.spec
.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(GUI)"=="UNX"
ALLTAR : $(LAUNCHERFLAGFILE) $(SPECFILES) $(COMMONMISC)$/{$(PRODUCTLIST)}$/build.flag

.ENDIF          # "$(GUI)"=="UNIX"

#
# Copy/patch the .desktop files to the output tree and 
# merge-in the translations. 
#
$(LAUNCHERFLAGFILE) : ../productversion.mk brand.pl translate.pl $(ULFDIR)$/launcher_name.ulf $(ULFDIR)$/launcher_comment.ulf $(ULFDIR)/launcher_genericname.ulf
$(LAUNCHERFLAGFILE) : $(LAUNCHERDEPN) 
    @$(MKDIRHIER) $(@:db).$(INPATH)
    @echo Creating desktop entries for $(@:f) ..
    @echo ---------------------------------
    @$(PERL) brand.pl -p '$${{PRODUCTNAME}} $${{PRODUCTVERSION}}' -u $(UNIXWRAPPERNAME) --iconprefix '$${{WITHOUTDOTUNIXPRODUCTNAME}}${ICONVERSION}-' $< $(@:db).$(INPATH)
    @$(PERL) translate.pl -p '$${{PRODUCTNAME}} $${{PRODUCTVERSION}}' -d $(@:db).$(INPATH) --ext "desktop" --key "Name" $(ULFDIR)$/launcher_name.ulf
    @$(PERL) translate.pl -p '$${{PRODUCTNAME}} $${{PRODUCTVERSION}}' -d $(@:db).$(INPATH) --ext "desktop" --key "Comment" $(ULFDIR)$/launcher_comment.ulf
    @$(PERL) translate.pl -p '$${{PRODUCTNAME}} $${{PRODUCTVERSION}}' -d $(@:db).$(INPATH) --ext "desktop" --key "GenericName" $(ULFDIR)$/launcher_comment.ulf

.IF "$(WITH_LIBSN)"=="YES"
    @noop x$(foreach,i,$(LAUNCHERLIST) $(shell echo "StartupNotify=true" >> $(@:db).$(INPATH)/$i.desktop))x
.ENDIF
    @$(MV) -f $(@:db).$(INPATH)/* $(@:d)
    @rmdir $(@:db).$(INPATH)
    @touch $@

#
# Create shared mime info xml file
#
$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.org.xml : ../mimetypes/{$(MIMELIST)}.desktop create_mime_xml.pl
$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.org.xml : $(ULFDIR)$/documents.ulf
    @echo Create shared mime info xml file ..
    @echo ---------------------------------
    @$(PERL) create_mime_xml.pl $< > $@.$(INPATH)
    @mv -f $@.$(INPATH) $@

.IF "$(PKGFORMAT)"!="$(PKGFORMAT:s/rpm//)"
$(SPECFILES) : add_specfile_triggers.sed symlink_triggers
$(SPECFILES) : ../$$(@:b:s/-menus//)/$$(@:f)
    @sed -f ../share/add_specfile_triggers.sed $< | tr -d "\015" >$@
.ENDIF

#
# Create GNOME mime-info files
#

$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.keys : ../mimetypes/openoffice.mime brand.pl translate.pl ../productversion.mk $(ULFDIR)$/documents.ulf
$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.keys : ../mimetypes/{$(MIMELIST)}.keys  
    @$(MKDIRHIER) $(@:d)
    @echo Creating GNOME .keys file for $(@:d:d:f) ..
    @echo ---------------------------------
    @$(PERL) brand.pl -p $(PRODUCTNAME.$(@:d:d:f)) -u $(UNIXFILENAME.$(@:d:d:f)) --iconprefix "$(ICONPREFIX.$(@:d:d:f))-" $< $(MISC)/$(@:d:d:f)
    @$(PERL) translate.pl -p $(PRODUCTNAME.$(@:d:d:f)) -d $(MISC)/$(@:d:d:f) --ext "keys" --key "description" $(ULFDIR)$/documents.ulf
    @cat $(MISC)/$(@:d:d:f)/{$(MIMELIST)}.keys > $@.$(INPATH)
    @mv -f $@.$(INPATH) $@

$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.mime : ../mimetypes/$$(@:f)
    @$(MKDIRHIER) $(@:d)
    @echo Creating GNOME .mime file for $(@:d:d:f) ..
    @echo ---------------------------------
    @cat $< | tr -d "\015" > $@.$(INPATH)
    @mv -f $@.$(INPATH) $@

$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.applications : ../productversion.mk 
$(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.applications : ../mimetypes/$$(@:f)
    @$(MKDIRHIER) $(@:d)
    @echo Creating GNOME .applications file for $(@:d:d:f) ..
    @echo ---------------------------------
    @cat $< | tr -d "\015" | sed -e "s/OFFICENAME/$(UNIXFILENAME.$(@:d:d:f))/" -e "s/%PRODUCTNAME/$(PRODUCTNAME.$(@:d:d:f)) $(PRODUCTVERSION.$(@:d:d:f))/" > $@.$(INPATH)
    @mv -f $@.$(INPATH) $@

#
# Create KDE mimelnk files
#
$(COMMONMISC)$/{$(PRODUCTLIST)}$/mimelnklist : brand.pl translate.pl ../productversion.mk $(ULFDIR)$/documents.ulf
$(COMMONMISC)$/{$(PRODUCTLIST)}$/mimelnklist : ../mimetypes/{$(MIMELIST)}.desktop
    @$(MKDIRHIER) $(@:db).$(INPATH)
    @echo Creating KDE mimelnk entries for $(@:d:d:f) ..
    @echo ---------------------------------
    @$(PERL) brand.pl -p "$(PRODUCTNAME.$(@:d:d:f))" -u $(UNIXFILENAME.$(@:d:d:f)) --iconprefix "$(ICONPREFIX.$(@:d:d:f))-" $< $(@:db).$(INPATH)
    @$(PERL) ../share/translate.pl -p "$(PRODUCTNAME.$(@:d:d:f))" -d $(@:db).$(INPATH) --ext "desktop" --key "Comment" $(ULFDIR)$/documents.ulf
    @mv -f $(@:db).$(INPATH)/* $(@:d)
    @rmdir $(@:db).$(INPATH)
    @echo  "{$(MIMELIST)}.desktop" > $@   

#
# Generate customized install scripts
#
$(COMMONMISC)$/{$(PRODUCTLIST)}$/create_tree.sh : makefile.mk
$(COMMONMISC)$/{$(PRODUCTLIST)}$/create_tree.sh : $$(@:f)
    @$(MKDIRHIER) $(@:d)
    @echo "#\!/bin/bash" > $@.$(INPATH)
    @echo "PREFIX=$(UNIXFILENAME.$(@:d:d:f))" >> $@.$(INPATH)
    @echo "ICON_PREFIX=$(ICONPREFIX.$(@:d:d:f))" >> $@.$(INPATH)
    @echo "ICON_SOURCE_DIR=../../../desktop/icons" >> $@.$(INPATH)
    @cat $< >> $@.$(INPATH)
    @chmod 774 $@.$(INPATH)
    @mv -f $@.$(INPATH) $@

#
# Create skript files
#

$(COMMONMISC)$/{$(PRODUCTLIST)}$/{openoffice printeradmin}.sh : ../productversion.mk
$(COMMONMISC)$/{$(PRODUCTLIST)}$/{openoffice printeradmin}.sh : $$(@:f)
    @$(MKDIRHIER) $(@:d)
    @cat $< | tr -d "\015" | sed -e "s/%PREFIX/$(UNIXFILENAME.$(@:d:d:f))/g" > $@.$(INPATH)
    @mv -f $@.$(INPATH) $@

#
# Launcher list
#

$(COMMONMISC)$/{$(PRODUCTLIST)}$/launcherlist : $(LAUNCHERDEPN)
    @$(MKDIRHIER) $(@:d)
    @echo "{$(LAUNCHERLIST:s/qstart//)}.desktop" >$@.$(INPATH)
    @mv -f $@.$(INPATH) $@

#
# Flagfile
#
$(COMMONMISC)$/{$(PRODUCTLIST)}$/build.flag : $(ICONDEPN) \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/launcherlist \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/mimelnklist \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/create_tree.sh \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.keys \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.mime \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.org.xml	\
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/openoffice.applications \
    $(COMMONMISC)$/{$(PRODUCTLIST)}$/{openoffice printeradmin}.sh
    @touch $@

