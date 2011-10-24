# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
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
# The Initial Developer of the Original Code is
#       Caolán McNamara, Red Hat, Inc. <caolanm@redhat.com>
# Portions created by the Initial Developer are Copyright (C) 2011 the
# Initial Developer. All Rights Reserved.
#
# Major Contributor(s):
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.
#*************************************************************************

$(eval $(call gb_CppunitTest_CppunitTest,sc_macros_test))

$(eval $(call gb_CppunitTest_add_exception_objects,sc_macros_test, \
    sc/qa/unit/macros-test \
))

$(eval $(call gb_CppunitTest_add_linked_libs,sc_macros_test, \
    avmedia \
    basegfx \
    comphelper \
    cppu \
    cppuhelper \
    drawinglayer \
    editeng \
    fileacc \
    for \
    forui \
    i18nisolang1 \
    msfilter \
    oox \
    sal \
    salhelper \
    sax \
    sb \
    sc \
    sfx \
    sot \
    svl \
    svt \
    svx \
    svxcore \
	test \
    tl \
    tk \
    ucbhelper \
	unotest \
    utl \
    vbahelper \
    vcl \
    xo \
	$(gb_STDLIBS) \
))

$(eval $(call gb_CppunitTest_set_include,sc_macros_test,\
    -I$(realpath $(SRCDIR)/sc/inc/pch) \
    -I$(realpath $(SRCDIR)/sc/source/ui/inc) \
    -I$(realpath $(SRCDIR)/sc/inc) \
    $$(INCLUDE) \
    -I$(OUTDIR)/inc \
))

$(eval $(call gb_CppunitTest_add_api,sc_macros_test,\
    offapi \
    oovbaapi \
    udkapi \
))

$(eval $(call gb_CppunitTest_uses_ure,sc_macros_test))

$(eval $(call gb_CppunitTest_add_type_rdbs,sc_macros_test,\
    oovbaapi \
    types \
))

$(eval $(call gb_CppunitTest_add_service_rdbs,sc_macros_test,\
    sc_macros_test \
))

$(eval $(call gb_CppunitTest_set_args,sc_macros_test,\
    --headless \
    --protector unoexceptionprotector$(gb_Library_DLLEXT) unoexceptionprotector \
    -env:OOO_CONFIG_REGISTRY_DIR=$(call gb_CppunitTarget__make_url,$(OUTDIR)/xml/registry) \
))

# we need to
# a) explicitly depend on library msword because it is not implied by a link
#    relation
# b) explicitly depend on the sc resource files needed at unit-test runtime
$(call gb_CppunitTest_get_target,sc_macros_test) : $(call gb_Library_get_target,scfilt) $(WORKDIR)/AllLangRes/sc

# vim: set noet sw=4 ts=4:
