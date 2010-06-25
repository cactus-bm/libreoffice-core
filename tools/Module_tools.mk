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

$(eval $(call gb_Module_Module,tools))

$(eval $(call gb_Module_add_targets,tools,\
    Executable_mkunroll \
    Executable_rscdep \
    Executable_so_checksum \
    Executable_sspretty \
    Library_tl \
    Package_inc \
))

# TODO:
#COPY tools/unxlngx6.pro/lib/atools.lib unxlngx6.pro/lib/atools.lib
#COPY tools/unxlngx6.pro/lib/bootstrp2.lib unxlngx6.pro/lib/bootstrp2.lib
#COPY tools/unxlngx6.pro/lib/btstrp.lib unxlngx6.pro/lib/btstrp.lib
#COPY tools/unxlngx6.pro/lib/libatools.a unxlngx6.pro/lib/libatools.a
#COPY tools/unxlngx6.pro/lib/libbootstrp2.a unxlngx6.pro/lib/libbootstrp2.a
#COPY tools/unxlngx6.pro/lib/libbtstrp.a unxlngx6.pro/lib/libbtstrp.a
#COPY tools/unxlngx6.pro/lib/libstdstrm.a unxlngx6.pro/lib/libstdstrm.a
#COPY tools/unxlngx6.pro/lib/stdstrm.lib unxlngx6.pro/lib/stdstrm.lib
#COPY tools/unxlngx6.pro/obj/pathutils.obj unxlngx6.pro/lib/pathutils-obj.obj
#COPY tools/unxlngx6.pro/slo/pathutils.obj unxlngx6.pro/lib/pathutils-slo.obj

#todo: link tools dynamically everywhere
#todo: ALWAYSDBGFLAG etc.

# vim: set noet sw=4 ts=4:
