# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Package_Package,slideshow_opengl_shader,$(SRCDIR)/slideshow/opengl))

$(eval $(call gb_Package_add_files,slideshow_opengl_shader,$(LIBO_BIN_FOLDER)/opengl,\
		slideshow/opengl/basicVertexShader.glsl \
		slideshow/opengl/dissolveFragmentShader.glsl \
		slideshow/opengl/staticFragmentShader.glsl \
))

# vim: set noet sw=4 ts=4:
