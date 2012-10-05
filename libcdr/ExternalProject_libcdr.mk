# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalProject_ExternalProject,libcdr))

$(eval $(call gb_ExternalProject_use_unpacked,libcdr,cdr))

$(eval $(call gb_ExternalProject_register_targets,libcdr,\
	build \
))

$(eval $(call gb_ExternalProject_use_externals,cdrlib,\
	libwpd \
))

ifeq ($(OS)$(COM),WNTMSC)

$(call gb_ExternalProject_get_state_target,libcdr,build) :
	cd $(EXTERNAL_WORKDIR)/build/win32 \
	&& export LIBWPD_INCLUDE_DIR=$(OUTDIR)/inc/external \
	&& export LIBWPG_INCLUDE_DIR=$(OUTDIR)/inc/external \
	&& export LCMS2_INCLUDE_DIR=$(OUTDIR)/inc/lcms2 \
	&& export ZLIB_INCLUDE_DIR=$(OUTDIR)/inc/external/zlib \
	&& $(COMPATH)/vcpackages/vcbuild.exe libcdr.vcproj "Release|Win32" \
	&& touch $@

else

libcdr_cflags :=
libcdr_libs :=

ifeq ($(SYSTEM_LIBWPD),YES)

libcdr_cflags += $(WPD_CFLAGS)
libcdr_libs += $(WPD_LIBS)

else # !SYSTEM_LIBWPD

libcdr_cflags += -I$(OUTDIR)/inc/external
libcdr_libs += $(SOLARLIB)

$(eval $(call gb_ExternalProject_use_package,libwpd))

endif # SYSTEM_LIBWPD


ifeq ($(SYSTEM_LIBWPG),YES)

libcdr_cflags += $(WPG_CFLAGS)
libcdr_libs += $(WPG_LIBS)

else # !SYSTEM_LIBWPG

libcdr_cflags += -I$(OUTDIR)/inc/external
libcdr_libs += $(SOLARLIB)

$(eval $(call gb_ExternalProject_use_package,libwpg_inc))
$(eval $(call gb_ExternalProject_use_static_libraries,wpglib))

endif # SYSTEM_LIBWPG


ifeq ($(SYSTEM_LCMS2),YES)

else # !SYSTEM_LCMS2

libcdr_cflags += -I$(OUTDIR)/inc/lcms2
libcdr_libs += $(SOLARLIB)

endif # SYSTEM_LCMS2

zlib_cflags :=
zlib_libs :=

ifneq ($(SYSTEM_ZLIB),YES)

zlib_cflags += -I$(OUTDIR)/inc/external/zlib
zlib_libs += $(SOLARLIB)

endif # !SYSTEM_ZLIB

$(call gb_ExternalProject_get_state_target,libcdr,build) :
	cd $(EXTERNAL_WORKDIR) \
	&& PKG_CONFIG="" \
	LIBCDR_CFLAGS="$(libcdr_cflags)" \
	LIBCDR_LIBS="$(libcdr_libs)" \
	ZLIB_CFLAGS="$(zlib_cflags)" \
	ZLIB_LIBS="$(zlib_libs)" \
	./configure \
		--with-pic \
		--enable-static \
		--disable-shared \
		--without-docs \
		--disable-debug \
		--disable-werror \
		$(if $(filter YES,$(CROSS_COMPILING)),--build=$(BUILD_PLATFORM) --host=$(HOST_PLATFORM)) \
	&& (cd $(EXTERNAL_WORKDIR)/src/lib && $(GNUMAKE) -j$(EXTMAXPROCESS)) \
	&& touch $@

endif

# vim: set noet sw=4 ts=4:
