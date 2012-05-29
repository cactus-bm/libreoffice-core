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
# Copyright (C) 2012 David Ostrovsky <d.ostrovsky@gmx.de> (initial developer)
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

$(eval $(call gb_CustomTarget_CustomTarget,testtools/uno_services))

TTUS := $(call gb_CustomTarget_get_workdir,testtools/uno_services)

$(call gb_CustomTarget_get_target,testtools/uno_services) : $(TTUS)/uno_services.rdb

#$(call gb_CustomTarget_get_target,i18npool/localedata) : \
#	$(patsubst %.xml,$(IPLD)/localedata_%.cxx, \
#		$(notdir $(wildcard $(SRCDIR)/i18npool/source/localedata/data/*.xml)))

#$(IPLD)/localedata_%.cxx : $(SRCDIR)/i18npool/source/localedata/data/%.xml \
#		$(IPLD)/saxparser.rdb $(call gb_Executable_get_target_for_build,saxparser)
#	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),SAX,1)
#	$(call gb_Helper_abbreviate_dirs_native, \
#		$(call gb_Helper_execute,saxparser) $* $< $@.tmp \
#			$(call gb_Helper_make_url,$(IPLD)/saxparser.rdb) \
#			$(call gb_Helper_make_url,$(OUTDIR_FOR_BUILD)/bin/types.rdb) \
#			-env:LO_LIB_DIR=$(call gb_Helper_make_url,$(gb_Helper_OUTDIR_FOR_BUILDLIBDIR)) \
#			$(if $(findstring s,$(MAKEFLAGS)),> /dev/null 2>&1) && \
#		sed 's/\(^.*get[^;]*$$\)/SAL_DLLPUBLIC_EXPORT \1/' $@.tmp > $@ && \
#		rm $@.tmp)

$(TTUS)/uno_services.rdb : $(TTUS)/uno_services.input \
		$(gb_XSLTPROCTARGET) $(SOLARENV)/bin/packcomponents.xslt
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),RDB,1)
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_XSLTPROC) --nonet --stringparam prefix $(OUTDIR_FOR_BUILD)/xml/ \
			-o $@ $(SOLARENV)/bin/packcomponents.xslt $<)

$(TTUS)/uno_services.input :| $(TTUS)/.dir
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),ECH,1)
	echo '<list>$(my_components:^"<filename>":+".component</filename>")</list>' > $@

# vim: set noet sw=4 ts=4:
