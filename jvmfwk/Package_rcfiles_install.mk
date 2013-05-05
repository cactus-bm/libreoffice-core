# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Package_Package,jvmfwk_rcfiles_install,$(SRCDIR)/jvmfwk))

$(eval $(call gb_Package_set_outdir,jvmfwk_rcfiles_install,$(INSTDIR)))

ifneq ($(OS),WNT)
ifeq ($(OS),FREEBSD)
$(eval $(call gb_Package_add_file,jvmfwk_rcfiles_install,ure/share/misc/javavendors.xml,distributions/OpenOfficeorg/javavendors_freebsd.xml))
else ifeq ($(OS),MACOSX)
$(eval $(call gb_Package_add_file,jvmfwk_rcfiles_install,ure/share/misc/javavendors.xml,distributions/OpenOfficeorg/javavendors_macosx.xml))
else ifeq ($(OS),LINUX)
$(eval $(call gb_Package_add_file,jvmfwk_rcfiles_install,ure/share/misc/javavendors.xml,distributions/OpenOfficeorg/javavendors_linux.xml))
else ifeq ($(OS),AIX)
$(eval $(call gb_Package_add_file,jvmfwk_rcfiles_install,ure/share/misc/javavendors.xml,distributions/OpenOfficeorg/javavendors_linux.xml))
else
$(eval $(call gb_Package_add_file,jvmfwk_rcfiles_install,ure/share/misc/javavendors.xml,distributions/OpenOfficeorg/javavendors_unx.xml))
endif
else ifeq ($(OS),WNT)
$(eval $(call gb_Package_add_file,jvmfwk_rcfiles_install,ure/share/misc/javavendors.xml,distributions/OpenOfficeorg/javavendors_wnt.xml))
else
$(call gb_Output_error,Unsupported platform)
endif

# vim:set noet sw=4 ts=4:
