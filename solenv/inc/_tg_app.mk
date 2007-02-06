# unroll begin

.IF "$(APP1LINKTYPE)" != ""
#must be either STATIC or SHARED
APP1LINKTYPEFLAG=$(APPLINK$(APP1LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP1CODETYPE)"=="C"
APP1LINKER=$(LINKC)
APP1STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP1LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP1CODETYPE)"=="C"
APP1LINKER=$(LINK)
APP1STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP1LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP1CODETYPE)"=="C"

.IF "$(APP1STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP1STACKN=$(LINKFLAGSTACK)$(APP1STACK)
.ENDIF
.ELSE
APP1STACKN=
.ENDIF

.IF "$(APP1NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP1OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP1DEPN+:=$(APP1DEPNU)
USE_APP1DEF=
.ENDIF

.IF "$(APP1TARGETN)"!=""

.IF "$(APP1PRODUCTNAME)"!=""
APP1PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP1PRODUCTNAME)\"
.ENDIF			# "$(APP1PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP1TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP1LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP1TARGETN:b)_linkinc.ls
$(APP1TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP1LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP1TARGETN): $(APP1OBJS) $(APP1LIBS) \
    $(APP1RES) \
    $(APP1ICON) $(APP1DEPN) $(USE_APP1DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP1OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP1LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP1LINKER) $(APP1LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP1STDLIBS) $(APP1STDLIB) $(STDLIB1)` \
    $(APP1LINKTYPEFLAG) $(APP1STDLIBS) $(APP1STDLIB) $(STDLIB1) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_1.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @echo $(APP1LINKER) $(APP1LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP1OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @cat $(mktmp /dev/null $(APP1LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @echo $(APP1LINKTYPEFLAG) $(APP1LIBSALCPPRT) $(APP1STDLIBS) $(APP1STDLIB) $(STDLIB1) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_1.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_1.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP1LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP1LINKRES:b).rc
.IF "$(APP1ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP1ICON:s/\/\\/)" >> $(MISC)$/$(APP1LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP1ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP1LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP1ICON)" != ""
.IF "$(APP1VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP1LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP1VERINFO)$(EMQ)" >> $(MISC)$/$(APP1LINKRES:b).rc
.ENDIF		# "$(APP1VERINFO)" != ""
    $(RC) -DWIN32 $(APP1PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP1LINKRES:b).rc
.ENDIF			# "$(APP1LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP1LINKER) @$(mktmp \
        $(APP1LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP1BASEX) \
        $(APP1STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP1TARGET)).map} \
        $(STDOBJ) \
        $(APP1LINKRES) \
        $(APP1RES) \
        $(APP1OBJS) \
        $(APP1LIBS) \
        $(APP1STDLIBS) \
        $(APP1STDLIB) $(STDLIB1) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP1TARGET).lnk
        -$(RM) $(MISC)\$(APP1TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP1TARGET).lst
        $(APP1LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP1BASEX) \
        $(APP1STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP1LINKRES) \
        $(APP1RES) \
        $(APP1OBJS) \
        $(APP1LIBS) \
        $(APP1STDLIBS) \
        $(APP1STDLIB) $(STDLIB1))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP1TARGETN:b)_linkobj.lst >> $(MISC)\$(APP1TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP1TARGET).lst $(THEN) type $(MISC)$/$(APP1TARGET).lst  >> $(MISC)$/$(APP1TARGET).lnk $(FI)
        $(APP1LINKER) @$(MISC)\$(APP1TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP1TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP1TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP2LINKTYPE)" != ""
#must be either STATIC or SHARED
APP2LINKTYPEFLAG=$(APPLINK$(APP2LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP2CODETYPE)"=="C"
APP2LINKER=$(LINKC)
APP2STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP2LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP2CODETYPE)"=="C"
APP2LINKER=$(LINK)
APP2STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP2LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP2CODETYPE)"=="C"

.IF "$(APP2STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP2STACKN=$(LINKFLAGSTACK)$(APP2STACK)
.ENDIF
.ELSE
APP2STACKN=
.ENDIF

.IF "$(APP2NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP2OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP2DEPN+:=$(APP2DEPNU)
USE_APP2DEF=
.ENDIF

.IF "$(APP2TARGETN)"!=""

.IF "$(APP2PRODUCTNAME)"!=""
APP2PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP2PRODUCTNAME)\"
.ENDIF			# "$(APP2PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP2TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP2LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP2TARGETN:b)_linkinc.ls
$(APP2TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP2LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP2TARGETN): $(APP2OBJS) $(APP2LIBS) \
    $(APP2RES) \
    $(APP2ICON) $(APP2DEPN) $(USE_APP2DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP2OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP2LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP2LINKER) $(APP2LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP2STDLIBS) $(APP2STDLIB) $(STDLIB2)` \
    $(APP2LINKTYPEFLAG) $(APP2STDLIBS) $(APP2STDLIB) $(STDLIB2) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_2.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @echo $(APP2LINKER) $(APP2LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP2OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @cat $(mktmp /dev/null $(APP2LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @echo $(APP2LINKTYPEFLAG) $(APP2LIBSALCPPRT) $(APP2STDLIBS) $(APP2STDLIB) $(STDLIB2) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_2.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_2.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP2LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP2LINKRES:b).rc
.IF "$(APP2ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP2ICON:s/\/\\/)" >> $(MISC)$/$(APP2LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP2ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP2LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP2ICON)" != ""
.IF "$(APP2VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP2LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP2VERINFO)$(EMQ)" >> $(MISC)$/$(APP2LINKRES:b).rc
.ENDIF		# "$(APP2VERINFO)" != ""
    $(RC) -DWIN32 $(APP2PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP2LINKRES:b).rc
.ENDIF			# "$(APP2LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP2LINKER) @$(mktmp \
        $(APP2LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP2BASEX) \
        $(APP2STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP2TARGET)).map} \
        $(STDOBJ) \
        $(APP2LINKRES) \
        $(APP2RES) \
        $(APP2OBJS) \
        $(APP2LIBS) \
        $(APP2STDLIBS) \
        $(APP2STDLIB) $(STDLIB2) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP2TARGET).lnk
        -$(RM) $(MISC)\$(APP2TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP2TARGET).lst
        $(APP2LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP2BASEX) \
        $(APP2STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP2LINKRES) \
        $(APP2RES) \
        $(APP2OBJS) \
        $(APP2LIBS) \
        $(APP2STDLIBS) \
        $(APP2STDLIB) $(STDLIB2))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP2TARGETN:b)_linkobj.lst >> $(MISC)\$(APP2TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP2TARGET).lst $(THEN) type $(MISC)$/$(APP2TARGET).lst  >> $(MISC)$/$(APP2TARGET).lnk $(FI)
        $(APP2LINKER) @$(MISC)\$(APP2TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP2TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP2TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP3LINKTYPE)" != ""
#must be either STATIC or SHARED
APP3LINKTYPEFLAG=$(APPLINK$(APP3LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP3CODETYPE)"=="C"
APP3LINKER=$(LINKC)
APP3STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP3LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP3CODETYPE)"=="C"
APP3LINKER=$(LINK)
APP3STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP3LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP3CODETYPE)"=="C"

.IF "$(APP3STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP3STACKN=$(LINKFLAGSTACK)$(APP3STACK)
.ENDIF
.ELSE
APP3STACKN=
.ENDIF

.IF "$(APP3NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP3OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP3DEPN+:=$(APP3DEPNU)
USE_APP3DEF=
.ENDIF

.IF "$(APP3TARGETN)"!=""

.IF "$(APP3PRODUCTNAME)"!=""
APP3PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP3PRODUCTNAME)\"
.ENDIF			# "$(APP3PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP3TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP3LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP3TARGETN:b)_linkinc.ls
$(APP3TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP3LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP3TARGETN): $(APP3OBJS) $(APP3LIBS) \
    $(APP3RES) \
    $(APP3ICON) $(APP3DEPN) $(USE_APP3DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP3OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP3LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP3LINKER) $(APP3LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP3STDLIBS) $(APP3STDLIB) $(STDLIB3)` \
    $(APP3LINKTYPEFLAG) $(APP3STDLIBS) $(APP3STDLIB) $(STDLIB3) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_3.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @echo $(APP3LINKER) $(APP3LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP3OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @cat $(mktmp /dev/null $(APP3LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @echo $(APP3LINKTYPEFLAG) $(APP3LIBSALCPPRT) $(APP3STDLIBS) $(APP3STDLIB) $(STDLIB3) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_3.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_3.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP3LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP3LINKRES:b).rc
.IF "$(APP3ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP3ICON:s/\/\\/)" >> $(MISC)$/$(APP3LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP3ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP3LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP3ICON)" != ""
.IF "$(APP3VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP3LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP3VERINFO)$(EMQ)" >> $(MISC)$/$(APP3LINKRES:b).rc
.ENDIF		# "$(APP3VERINFO)" != ""
    $(RC) -DWIN32 $(APP3PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP3LINKRES:b).rc
.ENDIF			# "$(APP3LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP3LINKER) @$(mktmp \
        $(APP3LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP3BASEX) \
        $(APP3STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP3TARGET)).map} \
        $(STDOBJ) \
        $(APP3LINKRES) \
        $(APP3RES) \
        $(APP3OBJS) \
        $(APP3LIBS) \
        $(APP3STDLIBS) \
        $(APP3STDLIB) $(STDLIB3) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP3TARGET).lnk
        -$(RM) $(MISC)\$(APP3TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP3TARGET).lst
        $(APP3LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP3BASEX) \
        $(APP3STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP3LINKRES) \
        $(APP3RES) \
        $(APP3OBJS) \
        $(APP3LIBS) \
        $(APP3STDLIBS) \
        $(APP3STDLIB) $(STDLIB3))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP3TARGETN:b)_linkobj.lst >> $(MISC)\$(APP3TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP3TARGET).lst $(THEN) type $(MISC)$/$(APP3TARGET).lst  >> $(MISC)$/$(APP3TARGET).lnk $(FI)
        $(APP3LINKER) @$(MISC)\$(APP3TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP3TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP3TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP4LINKTYPE)" != ""
#must be either STATIC or SHARED
APP4LINKTYPEFLAG=$(APPLINK$(APP4LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP4CODETYPE)"=="C"
APP4LINKER=$(LINKC)
APP4STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP4LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP4CODETYPE)"=="C"
APP4LINKER=$(LINK)
APP4STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP4LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP4CODETYPE)"=="C"

.IF "$(APP4STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP4STACKN=$(LINKFLAGSTACK)$(APP4STACK)
.ENDIF
.ELSE
APP4STACKN=
.ENDIF

.IF "$(APP4NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP4OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP4DEPN+:=$(APP4DEPNU)
USE_APP4DEF=
.ENDIF

.IF "$(APP4TARGETN)"!=""

.IF "$(APP4PRODUCTNAME)"!=""
APP4PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP4PRODUCTNAME)\"
.ENDIF			# "$(APP4PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP4TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP4LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP4TARGETN:b)_linkinc.ls
$(APP4TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP4LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP4TARGETN): $(APP4OBJS) $(APP4LIBS) \
    $(APP4RES) \
    $(APP4ICON) $(APP4DEPN) $(USE_APP4DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP4OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP4LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP4LINKER) $(APP4LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP4STDLIBS) $(APP4STDLIB) $(STDLIB4)` \
    $(APP4LINKTYPEFLAG) $(APP4STDLIBS) $(APP4STDLIB) $(STDLIB4) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_4.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @echo $(APP4LINKER) $(APP4LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP4OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @cat $(mktmp /dev/null $(APP4LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @echo $(APP4LINKTYPEFLAG) $(APP4LIBSALCPPRT) $(APP4STDLIBS) $(APP4STDLIB) $(STDLIB4) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_4.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_4.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP4LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP4LINKRES:b).rc
.IF "$(APP4ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP4ICON:s/\/\\/)" >> $(MISC)$/$(APP4LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP4ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP4LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP4ICON)" != ""
.IF "$(APP4VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP4LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP4VERINFO)$(EMQ)" >> $(MISC)$/$(APP4LINKRES:b).rc
.ENDIF		# "$(APP4VERINFO)" != ""
    $(RC) -DWIN32 $(APP4PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP4LINKRES:b).rc
.ENDIF			# "$(APP4LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP4LINKER) @$(mktmp \
        $(APP4LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP4BASEX) \
        $(APP4STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP4TARGET)).map} \
        $(STDOBJ) \
        $(APP4LINKRES) \
        $(APP4RES) \
        $(APP4OBJS) \
        $(APP4LIBS) \
        $(APP4STDLIBS) \
        $(APP4STDLIB) $(STDLIB4) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP4TARGET).lnk
        -$(RM) $(MISC)\$(APP4TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP4TARGET).lst
        $(APP4LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP4BASEX) \
        $(APP4STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP4LINKRES) \
        $(APP4RES) \
        $(APP4OBJS) \
        $(APP4LIBS) \
        $(APP4STDLIBS) \
        $(APP4STDLIB) $(STDLIB4))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP4TARGETN:b)_linkobj.lst >> $(MISC)\$(APP4TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP4TARGET).lst $(THEN) type $(MISC)$/$(APP4TARGET).lst  >> $(MISC)$/$(APP4TARGET).lnk $(FI)
        $(APP4LINKER) @$(MISC)\$(APP4TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP4TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP4TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP5LINKTYPE)" != ""
#must be either STATIC or SHARED
APP5LINKTYPEFLAG=$(APPLINK$(APP5LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP5CODETYPE)"=="C"
APP5LINKER=$(LINKC)
APP5STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP5LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP5CODETYPE)"=="C"
APP5LINKER=$(LINK)
APP5STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP5LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP5CODETYPE)"=="C"

.IF "$(APP5STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP5STACKN=$(LINKFLAGSTACK)$(APP5STACK)
.ENDIF
.ELSE
APP5STACKN=
.ENDIF

.IF "$(APP5NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP5OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP5DEPN+:=$(APP5DEPNU)
USE_APP5DEF=
.ENDIF

.IF "$(APP5TARGETN)"!=""

.IF "$(APP5PRODUCTNAME)"!=""
APP5PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP5PRODUCTNAME)\"
.ENDIF			# "$(APP5PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP5TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP5LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP5TARGETN:b)_linkinc.ls
$(APP5TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP5LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP5TARGETN): $(APP5OBJS) $(APP5LIBS) \
    $(APP5RES) \
    $(APP5ICON) $(APP5DEPN) $(USE_APP5DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP5OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP5LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP5LINKER) $(APP5LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP5STDLIBS) $(APP5STDLIB) $(STDLIB5)` \
    $(APP5LINKTYPEFLAG) $(APP5STDLIBS) $(APP5STDLIB) $(STDLIB5) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_5.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @echo $(APP5LINKER) $(APP5LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP5OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @cat $(mktmp /dev/null $(APP5LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @echo $(APP5LINKTYPEFLAG) $(APP5LIBSALCPPRT) $(APP5STDLIBS) $(APP5STDLIB) $(STDLIB5) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_5.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_5.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP5LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP5LINKRES:b).rc
.IF "$(APP5ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP5ICON:s/\/\\/)" >> $(MISC)$/$(APP5LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP5ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP5LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP5ICON)" != ""
.IF "$(APP5VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP5LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP5VERINFO)$(EMQ)" >> $(MISC)$/$(APP5LINKRES:b).rc
.ENDIF		# "$(APP5VERINFO)" != ""
    $(RC) -DWIN32 $(APP5PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP5LINKRES:b).rc
.ENDIF			# "$(APP5LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP5LINKER) @$(mktmp \
        $(APP5LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP5BASEX) \
        $(APP5STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP5TARGET)).map} \
        $(STDOBJ) \
        $(APP5LINKRES) \
        $(APP5RES) \
        $(APP5OBJS) \
        $(APP5LIBS) \
        $(APP5STDLIBS) \
        $(APP5STDLIB) $(STDLIB5) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP5TARGET).lnk
        -$(RM) $(MISC)\$(APP5TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP5TARGET).lst
        $(APP5LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP5BASEX) \
        $(APP5STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP5LINKRES) \
        $(APP5RES) \
        $(APP5OBJS) \
        $(APP5LIBS) \
        $(APP5STDLIBS) \
        $(APP5STDLIB) $(STDLIB5))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP5TARGETN:b)_linkobj.lst >> $(MISC)\$(APP5TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP5TARGET).lst $(THEN) type $(MISC)$/$(APP5TARGET).lst  >> $(MISC)$/$(APP5TARGET).lnk $(FI)
        $(APP5LINKER) @$(MISC)\$(APP5TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP5TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP5TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP6LINKTYPE)" != ""
#must be either STATIC or SHARED
APP6LINKTYPEFLAG=$(APPLINK$(APP6LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP6CODETYPE)"=="C"
APP6LINKER=$(LINKC)
APP6STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP6LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP6CODETYPE)"=="C"
APP6LINKER=$(LINK)
APP6STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP6LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP6CODETYPE)"=="C"

.IF "$(APP6STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP6STACKN=$(LINKFLAGSTACK)$(APP6STACK)
.ENDIF
.ELSE
APP6STACKN=
.ENDIF

.IF "$(APP6NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP6OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP6DEPN+:=$(APP6DEPNU)
USE_APP6DEF=
.ENDIF

.IF "$(APP6TARGETN)"!=""

.IF "$(APP6PRODUCTNAME)"!=""
APP6PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP6PRODUCTNAME)\"
.ENDIF			# "$(APP6PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP6TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP6LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP6TARGETN:b)_linkinc.ls
$(APP6TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP6LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP6TARGETN): $(APP6OBJS) $(APP6LIBS) \
    $(APP6RES) \
    $(APP6ICON) $(APP6DEPN) $(USE_APP6DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP6OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP6LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP6LINKER) $(APP6LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP6STDLIBS) $(APP6STDLIB) $(STDLIB6)` \
    $(APP6LINKTYPEFLAG) $(APP6STDLIBS) $(APP6STDLIB) $(STDLIB6) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_6.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @echo $(APP6LINKER) $(APP6LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP6OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @cat $(mktmp /dev/null $(APP6LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @echo $(APP6LINKTYPEFLAG) $(APP6LIBSALCPPRT) $(APP6STDLIBS) $(APP6STDLIB) $(STDLIB6) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_6.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_6.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP6LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP6LINKRES:b).rc
.IF "$(APP6ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP6ICON:s/\/\\/)" >> $(MISC)$/$(APP6LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP6ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP6LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP6ICON)" != ""
.IF "$(APP6VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP6LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP6VERINFO)$(EMQ)" >> $(MISC)$/$(APP6LINKRES:b).rc
.ENDIF		# "$(APP6VERINFO)" != ""
    $(RC) -DWIN32 $(APP6PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP6LINKRES:b).rc
.ENDIF			# "$(APP6LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP6LINKER) @$(mktmp \
        $(APP6LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP6BASEX) \
        $(APP6STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP6TARGET)).map} \
        $(STDOBJ) \
        $(APP6LINKRES) \
        $(APP6RES) \
        $(APP6OBJS) \
        $(APP6LIBS) \
        $(APP6STDLIBS) \
        $(APP6STDLIB) $(STDLIB6) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP6TARGET).lnk
        -$(RM) $(MISC)\$(APP6TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP6TARGET).lst
        $(APP6LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP6BASEX) \
        $(APP6STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP6LINKRES) \
        $(APP6RES) \
        $(APP6OBJS) \
        $(APP6LIBS) \
        $(APP6STDLIBS) \
        $(APP6STDLIB) $(STDLIB6))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP6TARGETN:b)_linkobj.lst >> $(MISC)\$(APP6TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP6TARGET).lst $(THEN) type $(MISC)$/$(APP6TARGET).lst  >> $(MISC)$/$(APP6TARGET).lnk $(FI)
        $(APP6LINKER) @$(MISC)\$(APP6TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP6TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP6TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP7LINKTYPE)" != ""
#must be either STATIC or SHARED
APP7LINKTYPEFLAG=$(APPLINK$(APP7LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP7CODETYPE)"=="C"
APP7LINKER=$(LINKC)
APP7STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP7LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP7CODETYPE)"=="C"
APP7LINKER=$(LINK)
APP7STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP7LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP7CODETYPE)"=="C"

.IF "$(APP7STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP7STACKN=$(LINKFLAGSTACK)$(APP7STACK)
.ENDIF
.ELSE
APP7STACKN=
.ENDIF

.IF "$(APP7NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP7OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP7DEPN+:=$(APP7DEPNU)
USE_APP7DEF=
.ENDIF

.IF "$(APP7TARGETN)"!=""

.IF "$(APP7PRODUCTNAME)"!=""
APP7PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP7PRODUCTNAME)\"
.ENDIF			# "$(APP7PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP7TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP7LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP7TARGETN:b)_linkinc.ls
$(APP7TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP7LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP7TARGETN): $(APP7OBJS) $(APP7LIBS) \
    $(APP7RES) \
    $(APP7ICON) $(APP7DEPN) $(USE_APP7DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP7OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP7LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP7LINKER) $(APP7LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP7STDLIBS) $(APP7STDLIB) $(STDLIB7)` \
    $(APP7LINKTYPEFLAG) $(APP7STDLIBS) $(APP7STDLIB) $(STDLIB7) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_7.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @echo $(APP7LINKER) $(APP7LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP7OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @cat $(mktmp /dev/null $(APP7LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @echo $(APP7LINKTYPEFLAG) $(APP7LIBSALCPPRT) $(APP7STDLIBS) $(APP7STDLIB) $(STDLIB7) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_7.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_7.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP7LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP7LINKRES:b).rc
.IF "$(APP7ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP7ICON:s/\/\\/)" >> $(MISC)$/$(APP7LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP7ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP7LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP7ICON)" != ""
.IF "$(APP7VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP7LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP7VERINFO)$(EMQ)" >> $(MISC)$/$(APP7LINKRES:b).rc
.ENDIF		# "$(APP7VERINFO)" != ""
    $(RC) -DWIN32 $(APP7PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP7LINKRES:b).rc
.ENDIF			# "$(APP7LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP7LINKER) @$(mktmp \
        $(APP7LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP7BASEX) \
        $(APP7STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP7TARGET)).map} \
        $(STDOBJ) \
        $(APP7LINKRES) \
        $(APP7RES) \
        $(APP7OBJS) \
        $(APP7LIBS) \
        $(APP7STDLIBS) \
        $(APP7STDLIB) $(STDLIB7) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP7TARGET).lnk
        -$(RM) $(MISC)\$(APP7TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP7TARGET).lst
        $(APP7LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP7BASEX) \
        $(APP7STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP7LINKRES) \
        $(APP7RES) \
        $(APP7OBJS) \
        $(APP7LIBS) \
        $(APP7STDLIBS) \
        $(APP7STDLIB) $(STDLIB7))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP7TARGETN:b)_linkobj.lst >> $(MISC)\$(APP7TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP7TARGET).lst $(THEN) type $(MISC)$/$(APP7TARGET).lst  >> $(MISC)$/$(APP7TARGET).lnk $(FI)
        $(APP7LINKER) @$(MISC)\$(APP7TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP7TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP7TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP8LINKTYPE)" != ""
#must be either STATIC or SHARED
APP8LINKTYPEFLAG=$(APPLINK$(APP8LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP8CODETYPE)"=="C"
APP8LINKER=$(LINKC)
APP8STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP8LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP8CODETYPE)"=="C"
APP8LINKER=$(LINK)
APP8STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP8LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP8CODETYPE)"=="C"

.IF "$(APP8STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP8STACKN=$(LINKFLAGSTACK)$(APP8STACK)
.ENDIF
.ELSE
APP8STACKN=
.ENDIF

.IF "$(APP8NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP8OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP8DEPN+:=$(APP8DEPNU)
USE_APP8DEF=
.ENDIF

.IF "$(APP8TARGETN)"!=""

.IF "$(APP8PRODUCTNAME)"!=""
APP8PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP8PRODUCTNAME)\"
.ENDIF			# "$(APP8PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP8TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP8LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP8TARGETN:b)_linkinc.ls
$(APP8TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP8LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP8TARGETN): $(APP8OBJS) $(APP8LIBS) \
    $(APP8RES) \
    $(APP8ICON) $(APP8DEPN) $(USE_APP8DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP8OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP8LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP8LINKER) $(APP8LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP8STDLIBS) $(APP8STDLIB) $(STDLIB8)` \
    $(APP8LINKTYPEFLAG) $(APP8STDLIBS) $(APP8STDLIB) $(STDLIB8) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_8.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @echo $(APP8LINKER) $(APP8LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP8OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @cat $(mktmp /dev/null $(APP8LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @echo $(APP8LINKTYPEFLAG) $(APP8LIBSALCPPRT) $(APP8STDLIBS) $(APP8STDLIB) $(STDLIB8) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_8.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_8.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP8LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP8LINKRES:b).rc
.IF "$(APP8ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP8ICON:s/\/\\/)" >> $(MISC)$/$(APP8LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP8ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP8LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP8ICON)" != ""
.IF "$(APP8VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP8LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP8VERINFO)$(EMQ)" >> $(MISC)$/$(APP8LINKRES:b).rc
.ENDIF		# "$(APP8VERINFO)" != ""
    $(RC) -DWIN32 $(APP8PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP8LINKRES:b).rc
.ENDIF			# "$(APP8LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP8LINKER) @$(mktmp \
        $(APP8LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP8BASEX) \
        $(APP8STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP8TARGET)).map} \
        $(STDOBJ) \
        $(APP8LINKRES) \
        $(APP8RES) \
        $(APP8OBJS) \
        $(APP8LIBS) \
        $(APP8STDLIBS) \
        $(APP8STDLIB) $(STDLIB8) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP8TARGET).lnk
        -$(RM) $(MISC)\$(APP8TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP8TARGET).lst
        $(APP8LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP8BASEX) \
        $(APP8STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP8LINKRES) \
        $(APP8RES) \
        $(APP8OBJS) \
        $(APP8LIBS) \
        $(APP8STDLIBS) \
        $(APP8STDLIB) $(STDLIB8))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP8TARGETN:b)_linkobj.lst >> $(MISC)\$(APP8TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP8TARGET).lst $(THEN) type $(MISC)$/$(APP8TARGET).lst  >> $(MISC)$/$(APP8TARGET).lnk $(FI)
        $(APP8LINKER) @$(MISC)\$(APP8TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP8TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP8TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP9LINKTYPE)" != ""
#must be either STATIC or SHARED
APP9LINKTYPEFLAG=$(APPLINK$(APP9LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP9CODETYPE)"=="C"
APP9LINKER=$(LINKC)
APP9STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP9LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP9CODETYPE)"=="C"
APP9LINKER=$(LINK)
APP9STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP9LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP9CODETYPE)"=="C"

.IF "$(APP9STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP9STACKN=$(LINKFLAGSTACK)$(APP9STACK)
.ENDIF
.ELSE
APP9STACKN=
.ENDIF

.IF "$(APP9NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP9OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP9DEPN+:=$(APP9DEPNU)
USE_APP9DEF=
.ENDIF

.IF "$(APP9TARGETN)"!=""

.IF "$(APP9PRODUCTNAME)"!=""
APP9PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP9PRODUCTNAME)\"
.ENDIF			# "$(APP9PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP9TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP9LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP9TARGETN:b)_linkinc.ls
$(APP9TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP9LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP9TARGETN): $(APP9OBJS) $(APP9LIBS) \
    $(APP9RES) \
    $(APP9ICON) $(APP9DEPN) $(USE_APP9DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP9OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP9LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP9LINKER) $(APP9LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP9STDLIBS) $(APP9STDLIB) $(STDLIB9)` \
    $(APP9LINKTYPEFLAG) $(APP9STDLIBS) $(APP9STDLIB) $(STDLIB9) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_9.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @echo $(APP9LINKER) $(APP9LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP9OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @cat $(mktmp /dev/null $(APP9LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @echo $(APP9LINKTYPEFLAG) $(APP9LIBSALCPPRT) $(APP9STDLIBS) $(APP9STDLIB) $(STDLIB9) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_9.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_9.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP9LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP9LINKRES:b).rc
.IF "$(APP9ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP9ICON:s/\/\\/)" >> $(MISC)$/$(APP9LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP9ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP9LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP9ICON)" != ""
.IF "$(APP9VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP9LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP9VERINFO)$(EMQ)" >> $(MISC)$/$(APP9LINKRES:b).rc
.ENDIF		# "$(APP9VERINFO)" != ""
    $(RC) -DWIN32 $(APP9PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP9LINKRES:b).rc
.ENDIF			# "$(APP9LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP9LINKER) @$(mktmp \
        $(APP9LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP9BASEX) \
        $(APP9STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP9TARGET)).map} \
        $(STDOBJ) \
        $(APP9LINKRES) \
        $(APP9RES) \
        $(APP9OBJS) \
        $(APP9LIBS) \
        $(APP9STDLIBS) \
        $(APP9STDLIB) $(STDLIB9) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP9TARGET).lnk
        -$(RM) $(MISC)\$(APP9TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP9TARGET).lst
        $(APP9LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP9BASEX) \
        $(APP9STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP9LINKRES) \
        $(APP9RES) \
        $(APP9OBJS) \
        $(APP9LIBS) \
        $(APP9STDLIBS) \
        $(APP9STDLIB) $(STDLIB9))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP9TARGETN:b)_linkobj.lst >> $(MISC)\$(APP9TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP9TARGET).lst $(THEN) type $(MISC)$/$(APP9TARGET).lst  >> $(MISC)$/$(APP9TARGET).lnk $(FI)
        $(APP9LINKER) @$(MISC)\$(APP9TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP9TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP9TARGETN)"!=""


# Instruction for linking
# unroll begin

.IF "$(APP10LINKTYPE)" != ""
#must be either STATIC or SHARED
APP10LINKTYPEFLAG=$(APPLINK$(APP10LINKTYPE))
.ENDIF

# decide how to link
.IF "$(APP10CODETYPE)"=="C"
APP10LINKER=$(LINKC)
APP10STDLIB=$(subst,CPPRUNTIME, $(STDLIB))
APP10LINKFLAGS+=$(LINKCFLAGS)
.ELSE			# "$(APP10CODETYPE)"=="C"
APP10LINKER=$(LINK)
APP10STDLIB=$(subst,CPPRUNTIME,$(STDLIBCPP) $(STDLIB))
APP10LINKFLAGS+=$(LINKFLAGS)
.ENDIF			# "$(APP10CODETYPE)"=="C"

.IF "$(APP10STACK)" != ""
.IF "$(LINKFLAGSTACK)" != ""
APP10STACKN=$(LINKFLAGSTACK)$(APP10STACK)
.ENDIF
.ELSE
APP10STACKN=
.ENDIF

.IF "$(APP10NOSAL)"==""
.IF "$(TARGETTYPE)" == "GUI"
APP10OBJS+= $(STDOBJVCL)
.ENDIF
.ENDIF

.IF "$(GUI)" == "UNX"
APP10DEPN+:=$(APP10DEPNU)
USE_APP10DEF=
.ENDIF

.IF "$(APP10TARGETN)"!=""

.IF "$(APP10PRODUCTNAME)"!=""
APP10PRODUCTDEF:=-DPRODUCT_NAME=\"$(APP10PRODUCTNAME)\"
.ENDIF			# "$(APP10PRODUCTNAME)"!=""

.IF "$(linkinc)"!=""
.IF "$(GUI)"=="WNT"
$(MISC)$/$(APP10TARGET)_linkinc.ls .PHONY:
    @@-$(RM) $@
    sed -f $(COMMON_ENV_TOOLS)$/chrel.sed $(foreach,i,$(APP10LIBS) $(i:s/.lib/.lin/)) >> $@
.ENDIF

LINKINCTARGETS+=$(MISC)$/$(APP10TARGETN:b)_linkinc.ls
$(APP10TARGETN) : $(LINKINCTARGETS)
.ENDIF          # "$(linkinc)"!=""

# Allow for target specific LIBSALCPPRT override
APP10LIBSALCPPRT*=$(LIBSALCPPRT)

$(APP10TARGETN): $(APP10OBJS) $(APP10LIBS) \
    $(APP10RES) \
    $(APP10ICON) $(APP10DEPN) $(USE_APP10DEF)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"=="UNX"
.IF "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(@:b).list
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @-$(RM) $(MISC)$/$(@:b).strip
    @echo $(STDSLO) $(APP10OBJS:s/.obj/.o/) \
    `cat /dev/null $(APP10LIBS) | sed s\#$(ROUT)\#$(OUT)\#g` | tr -s " " "\n" > $(MISC)$/$(@:b).list
    @echo $(APP10LINKER) $(APP10LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) -o $@ \
    `macosx-dylib-link-list $(PRJNAME) $(SOLARVERSION)$/$(INPATH)$/lib $(PRJ)$/$(INPATH)$/lib $(APP10STDLIBS) $(APP10STDLIB) $(STDLIB10)` \
    $(APP10LINKTYPEFLAG) $(APP10STDLIBS) $(APP10STDLIB) $(STDLIB10) -filelist $(MISC)$/$(@:b).list > $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @cat $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_10.cmd
# Need to strip __objcInit symbol to avoid duplicate symbols when loading
# libraries at runtime
    @-nm $@ | grep -v ' U ' | $(AWK) '{ print $$NF }' | grep -F -x '__objcInit' > $(MISC)$/$(@:b).strip
    @strip -i -R $(MISC)$/$(@:b).strip -X $@
    @ls -l $@
.IF "$(TARGETTYPE)"=="GUI"
    @echo "Making: $@.app"
    @macosx-create-bundle $@
.ENDIF		# "$(TARGETTYPE)"=="GUI"
.ELSE		# "$(OS)"=="MACOSX"
    @echo unx
    @-$(RM) $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @echo $(APP10LINKER) $(APP10LINKFLAGS) $(LINKFLAGSAPP) -L$(PRJ)$/$(INPATH)$/lib $(SOLARLIB) $(STDSLO) \
    $(APP10OBJS:s/.obj/.o/) '\' >  $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @cat $(mktmp /dev/null $(APP10LIBS)) | xargs -n 1 cat | sed s\#$(ROUT)\#$(OUT)\#g | sed 's#$$# \\#'  >> $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @echo $(APP10LINKTYPEFLAG) $(APP10LIBSALCPPRT) $(APP10STDLIBS) $(APP10STDLIB) $(STDLIB10) -o $@ >> $(MISC)$/$(TARGET).$(@:b)_10.cmd
    cat $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @+source $(MISC)$/$(TARGET).$(@:b)_10.cmd
    @ls -l $@
.ENDIF		# "$(OS)"=="MACOSX"
.ENDIF
.IF "$(GUI)" == "WNT"
    @@-$(MKDIR) $(@:d:d)
.IF "$(APP10LINKRES)" != ""
    @@-$(RM) $(MISC)$/$(APP10LINKRES:b).rc
.IF "$(APP10ICON)" != ""
.IF "$(USE_SHELL)"=="4nt"
    @-echo 1 ICON "$(APP10ICON:s/\/\\/)" >> $(MISC)$/$(APP10LINKRES:b).rc
.ELSE			# "$(USE_SHELL)"=="4nt"
    @-$(WRAPCMD) echo 1 ICON $(EMQ)"$(APP10ICON)$(EMQ)" | $(SED) 'sX\\X\\\\Xg' >> $(MISC)$/$(APP10LINKRES:b).rc
.ENDIF			# "$(USE_SHELL)"=="4nt"
.ENDIF		# "$(APP10ICON)" != ""
.IF "$(APP10VERINFO)" != ""
    @-echo $(EMQ)#define VERVARIANT	$(BUILD) >> $(MISC)$/$(APP10LINKRES:b).rc
    @-echo $(EMQ)#include  $(EMQ)"$(APP10VERINFO)$(EMQ)" >> $(MISC)$/$(APP10LINKRES:b).rc
.ENDIF		# "$(APP10VERINFO)" != ""
    $(RC) -DWIN32 $(APP10PRODUCTDEF) -I$(SOLARRESDIR) $(INCLUDE) $(RCLINKFLAGS) $(MISC)$/$(APP10LINKRES:b).rc
.ENDIF			# "$(APP10LINKRES)" != ""
.IF "$(linkinc)" == ""
    $(APP10LINKER) @$(mktmp \
        $(APP10LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP10BASEX) \
        $(APP10STACKN) \
        -out:$@ \
        -map:$(MISC)$/{$(subst,$/,_ $(APP10TARGET)).map} \
        $(STDOBJ) \
        $(APP10LINKRES) \
        $(APP10RES) \
        $(APP10OBJS) \
        $(APP10LIBS) \
        $(APP10STDLIBS) \
        $(APP10STDLIB) $(STDLIB10) \
        )
    @-echo linking $@.manifest ...
    $(IFEXIST) $@.manifest $(THEN) mt.exe -manifest $@.manifest -outputresource:$@$(EMQ);1 $(FI)
    $(IFEXIST) $@.manifest $(THEN) $(RM:s/+//) $@.manifest $(FI)
.ELSE
        -$(RM) $(MISC)\$(APP10TARGET).lnk
        -$(RM) $(MISC)\$(APP10TARGET).lst
        -$(RM) $(MISC)\linkobj.lst
        for %_i in ($(MISC)\*.obj) do type %_i >> $(MISC)\linkobj.lst
    type $(mktmp,$(MISC)\$(APP10TARGET).lst
        $(APP10LINKFLAGS) \
        $(LINKFLAGSAPP) $(APP10BASEX) \
        $(APP10STACKN) \
        -out:$@ \
        $(STDOBJ) \
        $(APP10LINKRES) \
        $(APP10RES) \
        $(APP10OBJS) \
        $(APP10LIBS) \
        $(APP10STDLIBS) \
        $(APP10STDLIB) $(STDLIB10))
        $(SED) -e 's/\(\.\.\\\)\{2,4\}/..\\/g' $(MISC)\$(APP10TARGETN:b)_linkobj.lst >> $(MISC)\$(APP10TARGET).lst
        $(IFEXIST) $(MISC)$/$(APP10TARGET).lst $(THEN) type $(MISC)$/$(APP10TARGET).lst  >> $(MISC)$/$(APP10TARGET).lnk $(FI)
        $(APP10LINKER) @$(MISC)\$(APP10TARGET).lnk
.ENDIF		# "$(linkinc)" == ""
.IF "$(APP10TARGET)" == "loader"
    $(PERL) loader.pl $@
.IF "$(USE_SHELL)"=="4nt"
    $(COPY) /b $(@)+$(@:d)unloader.exe $(@:d)_new.exe
.ELSE			# "$(USE_SHELL)"=="4nt"
    $(TYPE) $(@) $(@:d)unloader.exe > $(@:d)_new.exe
.ENDIF			# "$(USE_SHELL)"=="4nt"
    $(RM) $@
    $(RENAME) $(@:d)_new.exe $(@:d)loader.exe
.ENDIF			# "$(TARGET)" == "setup"

.ENDIF			# "$(GUI)" == "WNT"

.ENDIF			# "$(APP10TARGETN)"!=""


# Instruction for linking
