# -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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
# The Initial Developer of the Original Code is
# 	Peter Foley <pefoley2@verizon.net>
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
#

$(eval $(call gb_Executable_Executable,pluginapp.bin))

$(eval $(call gb_Executable_set_include,pluginapp.bin,\
	$$(INCLUDE) \
	-I$(SRCDIR)/extensions/source/plugin/inc \
))

$(eval $(call gb_Executable_add_api,pluginapp.bin,\
	offapi \
	udkapi \
))

$(eval $(call gb_Executable_add_exception_objects,pluginapp.bin,\
	extensions/source/plugin/unx/npwrap \
	extensions/source/plugin/unx/npnapi \
	extensions/source/plugin/unx/mediator \
	extensions/source/plugin/unx/plugcon \
))

$(eval $(call gb_Executable_add_linked_libs,pluginapp.bin,\
	sal \
))

ifeq ($(OS),SOLARIS)
$(eval $(call gb_Executable_add_libs,pluginapp.bin,\
	-lXm \
	-lXt \
	-lXext \
	-lX11 \
	-ldl \
))
else ifeq ($(filter-out FREEBSD NETBSD OPENBSD DRAGONFLY,$(OS)),)
$(eval $(call gb_Executable_add_libs,pluginapp.bin,\
	-lXt \
	-lXext \
	-lX11 \
))
else ifeq ($(OS),AIX)
$(eval $(call gb_Executable_add_libs,pluginapp.bin,\
	-lXpm \
	-lXmu \
	-lXt \
	-lXext \
	-lX11 \
	-ldl \
))
else
$(eval $(call gb_Executable_add_libs,pluginapp.bin,\
	-lXt \
	-lXext \
	-lX11 \
	-ldl \
))
endif


ifeq ($(ENABLE_GTK),TRUE)
$(eval $(call gb_Executable_use_external,pluginapp.bin,gtk))

# the orignal dmakefile said: don't ask, it's ugly
ifeq ($(OS),SOLARIS)
$(eval $(call gb_Executable_set_ldflags,pluginapp.bin,\
	-z nodefs \
))
endif

endif

# vim:set shiftwidth=4 softtabstop=4 expandtab:
