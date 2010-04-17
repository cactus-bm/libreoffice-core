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

$(eval $(call gb_Library_Library,swui))

$(eval $(call gb_Library_set_include,swui,\
    $$(INCLUDE) \
    -I$(WORKDIR)/inc/sw/sdi \
    -I$(WORKDIR)/inc/sw/ \
    -I$(SRCDIR)/sw/inc \
    -I$(SRCDIR)/sw/inc/pch \
    -I$(SRCDIR)/sw/source/core/inc \
    -I$(SRCDIR)/sw/source/filter/inc \
    -I$(SRCDIR)/sw/source/ui/inc \
    -I$(OUTDIR)/inc/sw \
    -I$(OUTDIR)/inc/offuh \
))

$(eval $(call gb_Library_set_defs,swui,\
    $$(DEFS) \
    -DACCESSIBLE_LAYOUT \
))

$(eval $(call gb_Library_add_linked_libs,swui,\
    comphelper \
    cppu \
    cppuhelper \
    editeng \
    i18nisolang1 \
    msfilter \
    sal \
    sfx \
    sot \
    stl \
    svl \
    svt \
    svx \
    svx \
    svxcore \
    sw \
    tk \
    tl \
    ucbhelper \
    utl \
    vcl \
))

$(eval $(call gb_Library_add_exception_objects,swui,\
    sw/source/core/undo/SwRewriter \
    sw/source/ui/chrdlg/break \
    sw/source/ui/chrdlg/chardlg \
    sw/source/ui/chrdlg/drpcps \
    sw/source/ui/chrdlg/numpara \
    sw/source/ui/chrdlg/pardlg \
    sw/source/ui/chrdlg/swuiccoll \
    sw/source/ui/chrdlg/tblnumfm \
    sw/source/ui/config/mailconfigpage \
    sw/source/ui/config/optcomp \
    sw/source/ui/config/optload \
    sw/source/ui/config/optpage \
    sw/source/ui/dbui/addresslistdialog \
    sw/source/ui/dbui/createaddresslistdialog \
    sw/source/ui/dbui/customizeaddresslistdialog \
    sw/source/ui/dbui/dbinsdlg \
    sw/source/ui/dbui/dbtablepreviewdialog \
    sw/source/ui/dbui/mailmergewizard \
    sw/source/ui/dbui/mmaddressblockpage \
    sw/source/ui/dbui/mmdocselectpage \
    sw/source/ui/dbui/mmgreetingspage \
    sw/source/ui/dbui/mmlayoutpage \
    sw/source/ui/dbui/mmmergepage \
    sw/source/ui/dbui/mmoutputpage \
    sw/source/ui/dbui/mmoutputtypepage \
    sw/source/ui/dbui/mmpreparemergepage \
    sw/source/ui/dbui/selectdbtabledialog \
    sw/source/ui/dialog/abstract \
    sw/source/ui/dialog/addrdlg \
    sw/source/ui/dialog/ascfldlg \
    sw/source/ui/dialog/docstdlg \
    sw/source/ui/dialog/macassgn \
    sw/source/ui/dialog/swdialmgr \
    sw/source/ui/dialog/swdlgfact \
    sw/source/ui/dialog/swuiexp \
    sw/source/ui/dialog/uiregionsw \
    sw/source/ui/dialog/wordcountdialog \
    sw/source/ui/dochdl/selglos \
    sw/source/ui/envelp/envfmt \
    sw/source/ui/envelp/envlop1 \
    sw/source/ui/envelp/envprt \
    sw/source/ui/envelp/label1 \
    sw/source/ui/envelp/labelexp \
    sw/source/ui/envelp/labfmt \
    sw/source/ui/envelp/labprt \
    sw/source/ui/envelp/mailmrge \
    sw/source/ui/fldui/DropDownFieldDialog \
    sw/source/ui/fldui/FldRefTreeListBox \
    sw/source/ui/fldui/changedb \
    sw/source/ui/fldui/flddb \
    sw/source/ui/fldui/flddinf \
    sw/source/ui/fldui/flddok \
    sw/source/ui/fldui/fldedt \
    sw/source/ui/fldui/fldfunc \
    sw/source/ui/fldui/fldpage \
    sw/source/ui/fldui/fldref \
    sw/source/ui/fldui/fldtdlg \
    sw/source/ui/fldui/fldvar \
    sw/source/ui/fldui/inpdlg \
    sw/source/ui/fldui/javaedit \
    sw/source/ui/fmtui/tmpdlg \
    sw/source/ui/frmdlg/column \
    sw/source/ui/frmdlg/cption \
    sw/source/ui/frmdlg/frmdlg \
    sw/source/ui/frmdlg/frmpage \
    sw/source/ui/frmdlg/pattern \
    sw/source/ui/frmdlg/uiborder \
    sw/source/ui/frmdlg/wrap \
    sw/source/ui/index/cntex \
    sw/source/ui/index/cnttab \
    sw/source/ui/index/multmrk \
    sw/source/ui/index/swuiidxmrk \
    sw/source/ui/misc/bookmark \
    sw/source/ui/misc/docfnote \
    sw/source/ui/misc/glosbib \
    sw/source/ui/misc/glossary \
    sw/source/ui/misc/insfnote \
    sw/source/ui/misc/insrule \
    sw/source/ui/misc/linenum \
    sw/source/ui/misc/num \
    sw/source/ui/misc/outline \
    sw/source/ui/misc/pgfnote \
    sw/source/ui/misc/pggrid \
    sw/source/ui/misc/srtdlg \
    sw/source/ui/misc/swmodalredlineacceptdlg \
    sw/source/ui/table/colwd \
    sw/source/ui/table/convert \
    sw/source/ui/table/instable \
    sw/source/ui/table/mergetbl \
    sw/source/ui/table/rowht \
    sw/source/ui/table/splittbl \
    sw/source/ui/table/tabledlg \
    sw/source/ui/table/tautofmt \
    sw/source/ui/utlui/swrenamexnameddlg \
))

$(WORKDIR)/inc/sw/swuilib.hxx :
    $(call gb_Helper_announce,Creating swuilib.hxx ...)
    mkdir -p $$(dirname $@) && echo "#define DLL_NAME \"$(notdir $(call gb_Library_get_target,swui))\"" > $@

.PHONY : $(WORKDIR)/Clean/inc/sw/swuilib.hxx
$(WORKDIR)/Clean/inc/sw/swuilib.hxx :
    $(call gb_Helper_announce,Cleaning up swuilib.hxx ...)
    -$(call gb_Helper_abbreviate_dirs,\
        rm -f $(WORKDIR)/inc/sw/swuilib.hxx)


$(call gb_CxxObject_get_dep_target,sw/source/ui/dialog/swabstdlg) : $(WORKDIR)/inc/sw/swuilib.hxx
$(call gb_CxxObject_get_target,sw/source/ui/dialog/swabstdlg) : $(WORKDIR)/inc/sw/swuilib.hxx

$(call gb_Library_get_clean_target,swui) : $(WORKDIR)/Clean/inc/sw/swuilib.hxx

ifeq ($(OS),WNT)
$(eval $(call gb_Library_add_linked_libs,swui,\
    advapi32 \
    kernel32 \
    msvcrt \
    oldnames \
    user32 \
    uwinapi \
))
endif
# vim: set noet sw=4 ts=4:
