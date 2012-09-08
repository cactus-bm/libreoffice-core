# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Package_Package,libwps_inc,$(call gb_UnpackedTarball_get_dir,wps/src/lib)))

$(eval $(call gb_Package_use_unpacked,libwps_inc,wps))

$(eval $(call gb_Package_add_files,libwps_inc,inc/external/libwps,\
	libwps.h \
	WPSDocument.h \
))

# vim: set noet sw=4 ts=4:
