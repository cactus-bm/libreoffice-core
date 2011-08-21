# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.	If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

$(eval $(call gb_Library_Library,vclplug_gtk))

$(eval $(call gb_Library_set_include,vclplug_gtk,\
    $$(INCLUDE) \
    -I$(SRCDIR)/vcl/inc \
    -I$(SRCDIR)/vcl/inc/pch \
    -I$(SRCDIR)/solenv/inc \
    -I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_add_defs,vclplug_gtk,\
    -DVCLPLUG_GTK_IMPLEMENTATION \
    -DVERSION=\"$(UPD)$(LAST_MINOR)\" \
))

ifneq ($(ENABLE_DBUS),)
# FIXME: pkg-config calls should be done in configure, and this should be _use_external
$(eval $(call gb_Library_set_include,vclplug_gtk,\
	$$(INCLUDE) \
	$(filter -I%,$(shell pkg-config --cflags dbus-glib-1)) \
))
$(eval $(call gb_Library_add_defs,vclplug_gtk,\
    -DENABLE_DBUS \
))
$(eval $(call gb_Library_add_libs,vclplug_gtk,\
    $(shell pkg-config --libs dbus-glib-1)\
))
endif

$(eval $(call gb_Library_add_api,vclplug_gtk,\
    offapi \
    udkapi \
))

$(eval $(call gb_Library_add_linked_libs,vclplug_gtk,\
    vclplug_gen \
    vcl \
    tl \
    utl \
    sot \
    ucbhelper \
    basegfx \
    comphelper \
    cppuhelper \
    i18nisolang1 \
    i18npaper \
    i18nutil \
    jvmaccess \
    cppu \
    sal \
    X11 \
    Xext \
    SM \
    ICE \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_use_externals,vclplug_gtk,\
	gtk \
	gthread \
	icule \
	icuuc \
))

$(eval $(call gb_Library_add_exception_objects,vclplug_gtk,\
    vcl/unx/gtk/a11y/atkaction \
    vcl/unx/gtk/a11y/atkbridge \
    vcl/unx/gtk/a11y/atkcomponent \
    vcl/unx/gtk/a11y/atkeditabletext \
    vcl/unx/gtk/a11y/atkfactory \
    vcl/unx/gtk/a11y/atkhypertext \
    vcl/unx/gtk/a11y/atkimage \
    vcl/unx/gtk/a11y/atklistener \
    vcl/unx/gtk/a11y/atkregistry \
    vcl/unx/gtk/a11y/atkselection \
    vcl/unx/gtk/a11y/atktable \
    vcl/unx/gtk/a11y/atktextattributes \
    vcl/unx/gtk/a11y/atktext \
    vcl/unx/gtk/a11y/atkutil \
    vcl/unx/gtk/a11y/atkvalue \
    vcl/unx/gtk/a11y/atkwindow \
    vcl/unx/gtk/a11y/atkwrapper \
    vcl/unx/gtk/app/gtkdata \
    vcl/unx/gtk/app/gtkinst \
    vcl/unx/gtk/app/gtksys \
    vcl/unx/gtk/gdi/salnativewidgets-gtk \
    vcl/unx/gtk/window/gtkframe \
    vcl/unx/gtk/window/gtkobject \
))

ifeq ($(OS),LINUX)
$(eval $(call gb_Library_add_linked_libs,vclplug_gtk,\
    dl \
    m \
    pthread \
))
endif
# vim: set noet sw=4 ts=4:
