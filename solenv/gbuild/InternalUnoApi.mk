# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
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
# Copyright (C) 2012 Red Hat, Inc., David Tardon <dtardon@redhat.com>
#  (initial developer)
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

.PHONY : $(call gb_InternalUnoApi_get_clean_target,%)
$(call gb_InternalUnoApi_get_clean_target,%) :
	$(call gb_Helper_abbreviate_dirs_native,\
		rm -f $(call gb_InternalUnoApi_get_target,$*))

define gb_InternalUnoApi_InternalUnoApi
$(call gb_UnoApiTarget_UnoApiTarget,$(1))
$(call gb_UnoApiHeadersTarget_UnoApiHeadersTarget,$(1))

$(call gb_InternalUnoApi_get_target,$(1)) : $(call gb_UnoApiTarget_get_target,$(1))
$(call gb_InternalUnoApi_get_clean_target,$(1)) : $(call gb_UnoApiTarget_get_clean_target,$(1))
$(call gb_InternalUnoApi_get_clean_target,$(1)) : $(call gb_UnoApiHeadersTarget_get_clean_target,$(1))

$(call gb_Deliver_add_deliverable,$(call gb_InternalUnoApi_get_target,$(1)),$(call gb_UnoApiTarget_get_target,$(1)),$(1))

$$(eval $$(call gb_Module_register_target,$(call gb_InternalUnoApi_get_target,$(1)),$(call gb_InternalUnoApi_get_clean_target,$(1))))

endef

define gb_InternalUnoApi_add_idlfile
$(call gb_UnoApiTarget_add_idlfile,$(1),$(2),$(3))

endef

define gb_InternalUnoApi_add_idlfiles
$(call gb_UnoApiTarget_add_idlfiles,$(1),$(2),$(3))

endef

define gb_InternalUnoApi__add_api
$(call gb_UnoApiHeadersTarget_add_rdbfile,$(1),$(2))
$(call gb_InternalUnoApi_get_target,$(1)) : $(call gb_UnoApiTarget_get_target,$(2))

endef

define gb_InternalUnoApi_add_api
$(foreach rdb,$(2),$(call gb_InternalUnoApi__add_api,$(1),$(rdb)))

endef

# Express that the rdb $(2) depends on rdb $(3).
#
# This information is already available in the UnoApiTarget definition
# for $(2), but this may not be loaded if we are building from a
# different module. Thus, this is a necessary hack to make generation of
# headers on demand work.
#
# I suppose it would be possible to store the list of required rdbs for
# a rdb to a file and then load it when headers' generation is requested,
# but it feels like overkill...
define gb_InternalUnoApi_add_api_dependency
$(call gb_UnoApiHeadersTarget_add_rdbfile,$(2),$(3))

endef

define gb_InternalUnoApi_add_api_dependencies
$(foreach dep,$(3),$(call gb_InternalUnoApi_add_api_dependency,$(1),$(2),$(dep)))

endef

define gb_InternalUnoApi_set_xmlfile
$(call gb_UnoApiTarget_set_xmlfile,$(1),$(2))

endef

define gb_InternalUnoApi_set_include
$(call gb_UnoApiTarget_set_include,$(1),$(2))

endef

# vim: set noet sw=4 ts=4:
