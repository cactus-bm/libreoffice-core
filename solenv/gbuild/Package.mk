#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2009 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.	If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

# PackagePart class

$(foreach destination,$(call gb_PackagePart_get_destinations), $(destination)/%) :
	$(call gb_Deliver_deliver,$<,$@)

define gb_PackagePart_PackagePart
$(OUTDIR)/$(1) : $(2)
$(call gb_Deliver_add_deliverable,$(OUTDIR)/$(1),$(2))

endef


# Package class

.PHONY : $(call gb_Package_get_clean_target,%)
$(call gb_Package_get_clean_target,%) :
	$(call gb_Output_announce,$*,$(false),PKG,2)
	-$(call gb_Helper_abbreviate_dirs,\
		rm -f $(FILES))

$(call gb_Package_get_target,%) :
	$(call gb_Output_announce,$*,$(true),PKG,2)
	mkdir -p $(dir $@) && touch $@

define gb_Package_Package
gb_TARGET_PACKAGE_$(1)_SOURCEDIR := $(2)
$(call gb_Package_get_clean_target,$(1)) : FILES := $(call gb_Package_get_target,$(1))
$$(eval $$(call gb_Module_register_target,$(call gb_Package_get_target,$(1)),$(call gb_Package_get_clean_target,$(1))))

endef

define gb_Package_add_file
$(call gb_Package_get_target,$(1)) : $(OUTDIR)/$(2)
$(call gb_Package_get_clean_target,$(1)) : FILES += $(OUTDIR)/$(2)
$(call gb_PackagePart_PackagePart,$(2),$$(gb_TARGET_PACKAGE_$(1)_SOURCEDIR)/$(3))
$(OUTDIR)/$(2) : $$(gb_TARGET_PACKAGE_$(1)_SOURCEDIR)/$(3)
endef

# vim: set noet sw=4 ts=4:
