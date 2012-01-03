# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2011 Matúš Kukan <matus.kukan@gmail.com>
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

# Extension class

gb_Extension_ZIPCOMMAND := zip $(if $(findstring s,$(MAKEFLAGS)),-q)
ifeq ($(GUI),WNT)
gb_Extension_LICENSEFILE := license.txt
else
gb_Extension_LICENSEFILE := LICENSE
endif
gb_Extension_XRMEXTARGET := $(call gb_Executable_get_target,xrmex)
gb_Extension_XRMEXCOMMAND := \
	$(gb_Helper_set_ld_path) $(gb_Extension_XRMEXTARGET)
gb_Extension_PROPMERGETARGET := $(call gb_Executable_get_target,propmerge)
gb_Extension_PROPMERGECOMMAND := \
	$(PERL) $(gb_Extension_PROPMERGETARGET)
gb_Extension_SDFLOCATION := $(SRCDIR)/translations/$(INPATH)/misc/sdf/
# does not contain en-US because it is special cased in gb_Extension_Extension
gb_Extension_LANGS := $(filter-out en-US,$(gb_WITH_LANG))

# remove extension directory in workdir and oxt file in workdir and outdir
$(call gb_Extension_get_clean_target,%) :
	$(call gb_Output_announce,$*,$(false),OXT,3)
	$(call gb_Helper_abbreviate_dirs,\
		rm -f -r $(call gb_Extension_get_workdir,$*) && \
		rm -f $(call gb_Extension_get_target,$*) && \
		rm -f $(call gb_Extension_get_outdir_target,$*))

ifeq ($(strip $(gb_WITH_LANG)),)
$(call gb_Extension_get_workdir,%)/description.xml :
	$(call gb_Output_announce,$*/description.xml,$(true),CPY,3)
	$(call gb_Helper_abbreviate_dirs,\
		mkdir -p $(call gb_Extension_get_workdir,$*) && \
		cp -f $(LOCATION)/description.xml $@)
else
$(call gb_Extension_get_workdir,%)/description.xml : | \
		$(gb_Extension_XRMEXTARGET)
	$(call gb_Output_announce,$*/description.xml,$(true),XRM,3)
	$(call gb_Helper_abbreviate_dirs_native,\
		mkdir -p $(call gb_Extension_get_workdir,$*) && \
		$(gb_Extension_XRMEXCOMMAND) \
			-p $(PRJNAME) \
			-i $(filter %.xml,$^) \
			-o $@ \
			-m $(SDF) \
			-l all)
endif

# rule to create oxt package in workdir
# --filesync makes sure that all files in the oxt package will be removed that no longer are in $(FILES)
$(call gb_Extension_get_target,%) : \
		$(call gb_Extension_get_workdir,%)/description.xml
	$(call gb_Output_announce,$*,$(true),OXT,3)
	$(call gb_Helper_abbreviate_dirs_native,\
		mkdir -p $(call gb_Extension_get_workdir,$*)/META-INF \
			$(call gb_Extension_get_workdir,$*)/registration && \
		cp -f $(LOCATION)/manifest.xml $(call gb_Extension_get_workdir,$*)/META-INF && \
		cp -f $(OUTDIR)/bin/osl/$(gb_Extension_LICENSEFILE) $(call gb_Extension_get_workdir,$*)/registration && \
		cd $(call gb_Extension_get_workdir,$*) && \
		$(gb_Extension_ZIPCOMMAND) -rX --filesync \
			$(call gb_Extension_get_target,$*) \
			$(FILES) \
			$(foreach lang,$(gb_Extension_LANGS),description-$(lang).txt))

# TODO: needs dependency on $(OUTDIR)/bin/osl/$(gb_Extension_LICENSEFILE) once readlicense_oo will be gbuildized
# or just another simpler solution

# set file list and location of manifest and description files
# register target and clean target
# add deliverable
# add dependency for outdir target to workdir target (pattern rule for delivery is in Package.mk)
define gb_Extension_Extension
$(call gb_Extension_get_target,$(1)) : FILES := META-INF description.xml registration
$(call gb_Extension_get_target,$(1)) : LOCATION := $(SRCDIR)/$(2)
$(call gb_Extension_get_target,$(1)) : PRJNAME := $(firstword $(subst /, ,$(2)))
$(call gb_Extension_get_target,$(1)) : \
	SDF := $(gb_Extension_SDFLOCATION)$(2)/localize.sdf
$(call gb_Extension_get_workdir,$(1))/description.xml : \
	$(SRCDIR)/$(2)/description.xml \
	$(if $(gb_WITH_LANG),$(gb_Extension_SDFLOCATION)$(2)/localize.sdf)
$(call gb_Extension_add_file,$(1),description-en-US.txt,$(SRCDIR)/$(2)/description-en-US.txt)
$(eval $(call gb_Module_register_target,$(call gb_Extension_get_outdir_target,$(1)),$(call gb_Extension_get_clean_target,$(1))))
$(call gb_Deliver_add_deliverable,$(call gb_Extension_get_outdir_target,$(1)),$(call gb_Extension_get_target,$(1)),$(1))
$(call gb_Extension_get_outdir_target,$(1)) : $(call gb_Extension_get_target,$(1))

endef

# adding a file creates a dependency to it
# file is copied to $(WORKDIR)
define gb_Extension_add_file
$(call gb_Extension_get_target,$(1)) : FILES += $(2)
$(call gb_Extension_get_target,$(1)) : $(call gb_Extension_get_workdir,$(1))/$(2)
$(call gb_Extension_get_workdir,$(1))/$(2) : $(3)
	mkdir -p $$(dir $$@)
	cp -f $$< $$@

endef

# localize .properties file
# source file is copied to $(WORKDIR)
define gb_Extension_localize_properties
$(call gb_Extension_get_target,$(1)) : FILES += $(2) $(foreach lang,$(subst -,_,$(gb_Extension_LANGS)),$(subst en_US,$(lang),$(2)))
$(call gb_Extension_get_target,$(1)) : SDF2 := $(gb_Extension_SDFLOCATION)$(subst $(SRCDIR),,$(dir $(3)))localize.sdf
$(call gb_Extension_get_target,$(1)) : $(call gb_Extension_get_workdir,$(1))/$(2)
$(call gb_Extension_get_workdir,$(1))/$(2) : $(3)
	$(call gb_Output_announce,$(2),$(true),PRP,3)
	mkdir -p $$(dir $$@)
	cp -f $$< $$@
	$(gb_Extension_PROPMERGECOMMAND) -i $$@ -m $$(SDF2)

endef
#	$(call gb_Output_announce,$@,$(true),PRP,3)

# vim: set noet sw=4 ts=4:
