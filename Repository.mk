#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2009 by Sun Microsystems, Inc.
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

$(eval $(call gb_Helper_register_repository,SRCDIR))


$(eval $(call gb_Helper_register_executables,NONE, \
    bmp \
    bmpsum \
    g2g \
    mkunroll \
    rscdep \
    so_checksum \
    sspretty \
))

$(eval $(call gb_Helper_register_libraries,OOOLIBS, \
    avmedia \
    basegfx \
    cui \
    drawinglayer \
    editeng \
    eggtray \
    fwe \
    fwi \
    fwk \
    fwl \
    fwm \
    lng \
    msfilter \
    msword \
    qstart_gtk \
    sax \
    sb \
    sfx \
    sot \
    svl \
    svt \
    svx \
    svxcore \
    sw \
    swd \
    swui \
    textconversiondlgs \
    tk \
    tl \
    utl \
    vbahelper \
    vcl \
    xcr \
    xo \
    xof \
))

$(eval $(call gb_Helper_register_libraries,PLAINLIBS_URE, \
    xml2 \
))

$(eval $(call gb_Helper_register_libraries,PLAINLIBS_OOO, \
    icuuc \
    cppunit \
))


$(eval $(call gb_Helper_register_libraries,RTLIBS, \
    comphelper \
    i18nisolang1 \
    i18nutil \
    ucbhelper \
    vos3 \
))

$(eval $(call gb_Helper_register_libraries,RTVERLIBS, \
    cppuhelper \
    salhelper \
))

$(eval $(call gb_Helper_register_libraries,STLLIBS, \
    stl \
))

$(eval $(call gb_Helper_register_libraries,UNOLIBS_OOO, \
    fsstorage \
    hatchwindowfactory \
    passwordcontainer \
    productregistration \
    vbaswobj \
))


$(eval $(call gb_Helper_register_libraries,UNOVERLIBS, \
    cppu \
    jvmfwk \
    sal \
))

$(eval $(call gb_Helper_register_static_libraries,PLAINLIBS, \
    jpeglib \
    ooopathutils \
    salcpprt \
    zlib \
))

# vim: set noet sw=4 ts=4:
