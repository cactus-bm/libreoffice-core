#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: productversion.mk,v $
#
#   $Revision: 1.22 $
#
#   last change: $Author: kz $ $Date: 2007-05-10 15:18:33 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************
# NOTE: remove $(UPD) for final releases
PRODUCTNAME     = OpenOffice.org
PRODUCTVERSION  = 2.3
LONGPRODUCTNAME = OpenOffice.org 2.3
UNIXFILENAME    = openoffice.org2.3
PKGVERSION      = 2.3

.IF "$(OS)"=="LINUX"
PKGREV          = $(BUILD)
.ELSE
PKGREV          = $(LAST_MINOR:s/m//:s/s/./)
.ENDIF

ABSLOCALOUT:=$(shell cd $(PRJ) && pwd)$/$(ROUT)

# create desktop-integration subfolder on linux
.IF "$(OS)"=="LINUX"
# rpm seems to require absolute paths here :(
PKGDIR:=$(ABSLOCALOUT)$/bin$/desktop-integration
RPMMACROS= \
    --define "_rpmdir $(PKGDIR)" \
    --define "_rpmfilename %%{{NAME}}-%%{{VERSION}}-%%{{RELEASE}}.%%{{ARCH}}.rpm"
.ELSE
PKGDIR=$(BIN)
.ENDIF

PRODUCTLIST = openoffice.org broffice.org

# default values to minimize maintainance effort 
PRODUCTVERSION = 2.2
PKGVERSION = $(PRODUCTVERSION)
# gnome-vfs treats everything behind the last '.' as an icon extension, 
# even though the "icon_filename" in '.keys' is specified as filename 
# without extension. Since it also does not know how to handle "2-writer"
# type icons :-), we are stripping all '.' for now.
# ToDo: maybe we use a product major later ??
ICONVERSION = $(PRODUCTVERSION:s/.//g)

PRODUCTNAME.openoffice.org = OpenOffice.org
PRODUCTVERSION.openoffice.org = $(PRODUCTVERSION)
PKGVERSION.openoffice.org = $(PKGVERSION)
UNIXFILENAME.openoffice.org = $(PRODUCTNAME.openoffice.org:l)$(PRODUCTVERSION.openoffice.org)
ICONPREFIX.openoffice.org = $(UNIXFILENAME.openoffice.org:s/.//g)

PRODUCTNAME.broffice.org = BrOffice.org
PRODUCTVERSION.broffice.org = $(PRODUCTVERSION)
PKGVERSION.broffice.org = $(PKGVERSION)
UNIXFILENAME.broffice.org = $(PRODUCTNAME.broffice.org:l)$(PRODUCTVERSION.broffice.org)
ICONPREFIX.broffice.org = $(UNIXFILENAME.broffice.org:s/.//g)


