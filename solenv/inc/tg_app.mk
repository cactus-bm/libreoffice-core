#*************************************************************************
#
#   $RCSfile: tg_app.mk,v $
#
#   $Revision: 1.49 $
#
#   last change: $Author: obo $ $Date: 2005-04-29 08:49:48 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************


.IF "$(MULTI_APP_FLAG)" == ""
$(APP1TARGETN) .NULL : APP1

$(APP2TARGETN) .NULL : APP2

$(APP3TARGETN) .NULL : APP3

$(APP4TARGETN) .NULL : APP4

$(APP5TARGETN) .NULL : APP5

$(APP6TARGETN) .NULL : APP6

$(APP7TARGETN) .NULL : APP7

$(APP8TARGETN) .NULL : APP8

$(APP9TARGETN) .NULL : APP9

.ENDIF

.IF "$(MULTI_APP_FLAG)"==""
APP1 APP2 APP3 APP4 APP5 APP6 APP7 APP8 APP9 :
.IF "$(GUI)" == "UNX"
    @echo $(SHELL) AUSE $(COMSPEC) MARTIN $(SHELLFLAGS)
.ENDIF
    +@dmake $(APP$(TNR)TARGETN) MULTI_APP_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ELSE


#######################################################
# instructions for linking
# unroll begin

.IF "$(APP$(TNR)LINKTYPE)" != ""
#must be either STATIC or SHARED
APP$(TNR)LINKTYPEFLAG=$(APPLINK$(APP$(TNR)LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP$(TNR)CODETYPE)"=="C"
APP$(TNR)LINKER=$(LINKC)
APP$(TNR)STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP$(TNR)LINKFLAGS=$(LINKCFLAGS)
.ELSE			# "$(APP$(TNR)CODETYPE)"=="C"
APP$(TNR)LINKER=$(LINK)
APP$(TNR)STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP$(TNR)LINKFLAGS=$(LINKFLAGS)
.ENDIF			# "$(APP$(TNR)CODETYPE)"=="C"

.IF "$(APP$(TNR)STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP$(TNR)STACKN=$(LINKFLAGSTACK)$(APP$(TNR)STACK)
.ENDIF
.ELSE
APP$(TNR)STACKN=
.ENDIF

.IF "$(APP$(TNR)NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP$(TNR)OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP$(TNR)DEPN+:=$(APP$(TNR)DEPNU)
USE_APP$(TNR)DEF=
.ENDIF

.IF "$(APP$(TNR)TARGETN)"!=""

.IF "$(APP$(TNR)PRODUCTNAME)"!=""
APP$(TNR)PRODUCTDEF:=-DPRODUCT_NAME="$(APP$(TNR)PRODUCTNAME)"
.ENDIF			# "$(APP$(TNR)PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP$(TNR)TARGET)_linkinc.ls .PHONY:
    @+-$(RM) $@ >& $(NULLDEV)
    +sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP$(TNR)LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP$(TNR)TARGETN:b)_linkinc.ls
$(APP$(TNR)TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP$(TNR)LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP$(TNR)TARGETN): $(APP$(TNR)OBJS) $(APP$(TNR)LIBS) \
    $(APP$(TNR)RES) \
    $(APP$(TNR)ICON) $(APP$(TNR)DEPN) $(USE_APP$(TNR)DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @+echo unx
    @+-$(RM) $(MISC)$/$(@:b).list
    @+-$(RM) $(MISC)$/$(@:b).cmd
    @+-$(RM) $(MISC)$/$(@:b).strip
    @+echo $(STDSLO) $(APP$(TNR)OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP$(TNR)LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @+echo $(APP$(TNR)LINKER) $(APP$(TNR)LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP$(TNR)STDLIBS) $(APP$(TNR)STDLIB) $(STDLIB$(TNR))` \
    $(APP$(TNR)LINKTYPEFLAG) $(APP$(TNR)STDLIBS) $(APP$(TNR)STDLIB) $(STDLIB$(TNR)) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(@:b).cmd
    @cat $(MISC)$/$(@:b).cmd
    @source $(MISC)$/$(@:b).cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @+-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @+echo unx
    @+-$(RM) $(MISC)$/$(@:b).cmd
    @+echo $(APP$(TNR)LINKER) $(APP$(TNR)LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP$(TNR)OBJS:s/.obj/.o/) "\" >  $(MISC)$/$(@:b).cmd
    @cat $(mktmp /dev/null $(APP$(TNR)LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(@:b).cmd
    @+echo $(APP$(TNR)LINKTYPEFLAG) $(APP$(TNR)LIBSALCPPRT) $(APP$(TNR)STDLIBS) $(APP$(TNR)STDLIB) $(STDLIB$(TNR)) -o $@ >> $(MISC)$/$(@:b).cmd
    cat $(MISC)$/$(@:b).cmd
    @source $(MISC)$/$(@:b).cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @+-$(MKDIR) $(@:d:d) >& $(NULLDEV)
.IF "$(APP$(TNR)LINKRES)" != ""
    @+-$(RM) $(MISC)$/$(APP$(TNR)LINKRES:b).rc >& $(NULLDEV)
.IF "$(APP$(TNR)ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-+echo 1 ICON "$(APP$(TNR)ICON:s/\/\\/)" >> $(MISC)$/$(APP$(TNR)LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-+$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP$(TNR)ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP$(TNR)LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP$(TNR)ICON)" != ""
.IF "$(APP$(TNR)VERINFO)" != ""
    @-+echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP$(TNR)LINKRES:b).rc
    @-+echo $(EMQ)#include  $(EMQ)"$(APP$(TNR)VERINFO)$(EMQ)" >> $(MISC)$/$(APP$(TNR)LINKRES:b).rc
.ENDIF		# "$(APP$(TNR)VERINFO)" != ""
    $(RC) -DWIN32 $(APP$(TNR)PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP$(TNR)LINKRES:b).rc
.ENDIF			# "$(APP$(TNR)LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP$(TNR)LINKER) @$(mktmp \
        $(APP$(TNR)LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP$(TNR)BASEX) \
        $(APP$(TNR)STACKN) \
        -out:$@ \
        -map:$(MISC)$/$(@:b).map \
        $(STDOBJ) \
        $(APP$(TNR)LINKRES) \
        $(APP$(TNR)RES) \
        $(APP$(TNR)OBJS) \
        $(APP$(TNR)LIBS) \
        $(APP$(TNR)STDLIBS) \
        $(APP$(TNR)STDLIB) $(STDLIB$(TNR)) \
        )
.ELSE
        +-$(RM) $(MISC)\$(APP$(TNR)TARGET).lnk
        +-$(RM) $(MISC)\$(APP$(TNR)TARGET).lst
        +-$(RM) $(MISC)\linkobj.lst
        +for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP$(TNR)TARGET).lst
        $(APP$(TNR)LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP$(TNR)BASEX) \
        $(APP$(TNR)STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP$(TNR)LINKRES) \
        $(APP$(TNR)RES) \
        $(APP$(TNR)OBJS) \
        $(APP$(TNR)LIBS) \
        $(APP$(TNR)STDLIBS) \
        $(APP$(TNR)STDLIB) $(STDLIB$(TNR)))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP$(TNR)TARGETN:b)_linkobj.lst >> $(MISC)\$(APP$(TNR)TARGET).lst
        +$(IFEXIST) $(MISC)\$(APP$(TNR)TARGET).lst $(THEN) type $(MISC)\$(APP$(TNR)TARGET).lst  >> $(MISC)\$(APP$(TNR)TARGET).lnk
        $(APP$(TNR)LINKER) @$(MISC)\$(APP$(TNR)TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP$(TNR)TARGET)" == "loader"
    +$(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    +$(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    +$(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    +$(RM) $@
    +$(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP$(TNR)TARGETN)"!=""


# Instruction for linking
# unroll end
#######################################################

.ENDIF		# "$(MULTI_APP_FLAG)"==""
