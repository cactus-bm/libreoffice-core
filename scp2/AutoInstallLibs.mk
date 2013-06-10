# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_AutoInstallLibs_AutoInstallLibs))

$(eval $(call gb_AutoInstallLibs_add_module,base,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,gnome,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,graphicfilter,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,impress,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,kde,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,math,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,onlineupdate,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,ooo,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,tde,LIBO_LIB_FILE))
$(eval $(call gb_AutoInstallLibs_add_module,ure,URE_PRIVATE_LIB,URE_EXECUTABLE))
$(eval $(call gb_AutoInstallLibs_add_module,writer,LIBO_LIB_FILE))

# vim: set noet sw=4 ts=4:
