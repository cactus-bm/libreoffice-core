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
# Copyright (C) 2011 David Tardon, Red Hat Inc. <dtardon@redhat.com> (initial developer)
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

$(eval $(call gb_Executable_Executable,loader2))

$(eval $(call gb_Executable_set_targettype_gui,loader2,YES))

$(eval $(call gb_Executable_set_include,loader2,\
    $$(INCLUDE) \
))

$(eval $(call gb_Executable_add_linked_libs,loader2,\
    advapi32 \
    gdi32 \
    msi \
    shell32 \
))

ifeq ($(COM),MSC)
ifeq ($(gb_PRODUCT),)
$(eval $(call gb_Executable_add_linked_libs,loader2,\
    libcmtd \
))
else
$(eval $(call gb_Executable_add_linked_libs,loader2,\
    libcmt \
))
endif
endif

$(foreach file, \
    desktop/win32/source/setup/setup_main \
    desktop/win32/source/setup/setup_w, \
$(eval $(call gb_Executable_add_exception_objects,loader2,$(file)))\
$(eval $(call gb_CxxObject_get_target,$(file)) : DEFS := $(filter-out -D_DLL,$(gb_LinkTarget_DEFAULTDEFS) $(CPPFLAGS)))\
$(eval $(call gb_CxxObject_get_target,$(file)) : T_CXXFLAGS := -MT $(filter-out -MD,$(gb_LinkTarget_EXCEPTIONFLAGS) $(gb_LinkTarget_CXXFLAGS)))\
)

# vim: set ts=4 sw=4 et:
