#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: kz $ $Date: 2008-03-07 12:55:05 $
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
PRJ=..
PRJNAME=packimages
TARGET=packimages

.INCLUDE:  settings.mk
# point to an existing directory as default that poses no threat
# to a "find" that looks for "*.png"
RSCCUSTOMIMG*=$(PRJ)
.INCLUDE: target.mk

IMAGES := $(COMMONBIN)$/images.zip
SORTED_LIST=$(RES)$/img$/sorted.lst
BRANDINGIMG*=$(SOLARSRC)$/default_images
# Custom sets, at 24x24 & 16x16 fall-back to industrial preferentially
CUSTOM_PREFERRED_FALLBACK_1*=$(SOLARSRC)$/ooo_custom_images$/tango
CUSTOM_PREFERRED_FALLBACK_2*=$(SOLARSRC)$/ooo_custom_images$/industrial
CUSTOM_IMAGE_SETS=hicontrast industrial crystal tango
CUSTOM_IMAGES+=$(foreach,i,$(CUSTOM_IMAGE_SETS) images_$i)

CRYSTAL_TARBALL=$(SOLARSRC)$/external_images$/ooo_crystal_images-1.tar.gz

ALLTAR : $(IMAGES) $(CUSTOM_IMAGES)

$(IMAGES) $(CUSTOM_IMAGES) : $(SORTED_LIST)

$(SORTED_LIST) : image-sort.lst
    +-$(MKDIR) $(RES)$/img
    +$(PERL) $(SOLARENV)$/bin$/image-sort.pl image-sort.lst $(SOLARXMLDIR) $@

$(RES)$/img$/commandimagelist.ilst .PHONY :
    @@-$(MKDIR) $(RES)$/img
#+-$(RM) $@ $@.$(INPATH)
.IF "$(use_shell)"!="4nt"
    $(FIND) $(SOLARSRC)$/$(RSCDEFIMG)/res/commandimagelist -name "*.png" | sed "s#$(SOLARSRC)$/$(RSCDEFIMG)/res#%GLOBALRES%#" | $(PERL) $(SOLARENV)$/bin$/sort.pl > $@.$(INPATH)
.ELSE			# "$(use_shell)"!="4nt"
    $(FIND) $(SOLARSRC)$/$(RSCDEFIMG)/res/commandimagelist -name "*.png" | sed "s/$(SOLARSRC:s/\/./).$(RSCDEFIMG)\/res/%%GLOBALRES%%/" | $(PERL) $(SOLARENV)$/bin$/sort.pl > $@.$(INPATH)
.ENDIF			# "$(use_shell)"!="4nt"
    $(PERL) $(SOLARENV)$/bin$/diffmv.pl $@.$(INPATH) $@

$(COMMONBIN)$/images.zip .PHONY: $(RES)$/img$/commandimagelist.ilst
    $(PERL) $(SOLARENV)$/bin$/packimages.pl -g $(SOLARSRC)$/$(RSCDEFIMG) -m $(SOLARSRC)$/$(RSCDEFIMG) -c $(RSCCUSTOMIMG) -l $(SOLARCOMMONRESDIR)$/img -s $(SORTED_LIST) -l $(RES)$/img -o $@

images_% : $(RES)$/img$/commandimagelist.ilst $(SORTED_LIST)
    $(PERL) $(SOLARENV)$/bin$/packimages.pl -g $(SOLARSRC)$/$(RSCDEFIMG) -m $(SOLARSRC)$/$(RSCDEFIMG) -c $(RSCCUSTOMIMG) -c $(SOLARSRC)$/ooo_custom_images$/$(@:s/images_//) -c $(MISC)$/$(@:s/images_//) -c $(CUSTOM_PREFERRED_FALLBACK_1) -c $(CUSTOM_PREFERRED_FALLBACK_2) -l $(SOLARCOMMONRESDIR)$/img -l $(RES)$/img -s $(SORTED_LIST) -o $(COMMONBIN)$/$@.zip

$(COMMONBIN)$/%$/images.zip  .PHONY : $(RES)$/img$/commandimagelist.ilst
    @@-$(MKDIR) $(COMMONBIN)$/$(@:d:d:f)
    $(PERL) $(SOLARENV)$/bin$/packimages.pl -g $(SOLARSRC)$/$(RSCDEFIMG) -m $(SOLARSRC)$/$(RSCDEFIMG) -c $(BRANDINGIMG)$/$(@:d:d:f) -c $(RSCCUSTOMIMG)$/$(@:d:d:f) -c $(RSCCUSTOMIMG) -c $(SOLARSRC)$/ooo_custom_images$/$(@:d:d:f) -c $(MISC)$/$(@:d:d:f) -c $(CUSTOM_PREFERRED_FALLBACK_1) -c $(CUSTOM_PREFERRED_FALLBACK_2) -l $(SOLARCOMMONRESDIR)$/img -l $(RES)$/img -s $(SORTED_LIST) -o $@

# generate the HiContrast icon set
$(MISC)$/hicontrast.flag .PHONY :
    $(PERL) $(SOLARENV)$/bin$/hicontrast-to-theme.pl $(SOLARSRC)$/default_images $(MISC)$/hicontrast && $(TOUCH) $@

# unpack the Crystal icon set
$(MISC)$/crystal.flag : $(CRYSTAL_TARBALL)
    cd $(MISC) && gzip -d -c $(CRYSTAL_TARBALL) | ( tar -xf - ) && $(TOUCH) $(@:f)
.IF "$(GUI)"=="UNX"
    chmod -R g+w $(MISC)$/crystal
.ENDIF
    @$(TYPE) $@ || echo "ERROR: unpacking $(CRYSTAL_TARBALL) failed"

# dependencies
images_hicontrast : $(MISC)$/hicontrast.flag $(RES)$/img$/commandimagelist.ilst

images_crystal : $(MISC)$/crystal.flag $(RES)$/img$/commandimagelist.ilst

