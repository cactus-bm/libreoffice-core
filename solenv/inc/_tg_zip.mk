# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP1TARGETN)"!=""

ZIP1DIR*=$(ZIPDIR)
ZIP1FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP1TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP1TARGET).dpzz : $(ZIP1TARGETN)

.IF "$(common_build_zip)"!=""
ZIP1HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP1TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP1HELPVAR=$(BIN)$/$(ZIP1TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP1TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP1TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP1TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP1HELPVAR), $(@:db))}) $(ZIP1DIR)) $(command_seperator) zip $(ZIP1FLAGS) $(ZIP1TMP).zip $(ZIP1LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP1TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP1TMP).zip
.ELSE			# "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP1FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP1HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP1TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP1HELPVAR), $(@:db))}) $(ZIP1DIR)) $(command_seperator) zip $(ZIP1FLAGS) $(ZIP1TMP).zip $(ZIP1LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP1TMP).zip  $@ 
    +$(RM) $(ZIP1TMP).zip
.ELSE			# "$(ZIP1DIR)" != ""
    +-zip $(ZIP1FLAGS) $@ $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP1TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP1DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    +-zipdep $(ZIP1FLAGS) $@ $(foreach,j,$(ZIP1LIST) $(ZIP1DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP1TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP1DIR)" != ""
    @+-zipdep $(ZIP1FLAGS) $@ $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP1HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP1DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    +-zipdep $(ZIP1FLAGS) $@ $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP1TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP1DIR)" != ""
    +-zipdep $(ZIP1FLAGS) $@ $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP1TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP1DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP2TARGETN)"!=""

ZIP2DIR*=$(ZIPDIR)
ZIP2FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP2TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP2TARGET).dpzz : $(ZIP2TARGETN)

.IF "$(common_build_zip)"!=""
ZIP2HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP2TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP2HELPVAR=$(BIN)$/$(ZIP2TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP2TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP2TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP2TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP2HELPVAR), $(@:db))}) $(ZIP2DIR)) $(command_seperator) zip $(ZIP2FLAGS) $(ZIP2TMP).zip $(ZIP2LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP2TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP2TMP).zip
.ELSE			# "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP2FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP2HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP2TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP2HELPVAR), $(@:db))}) $(ZIP2DIR)) $(command_seperator) zip $(ZIP2FLAGS) $(ZIP2TMP).zip $(ZIP2LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP2TMP).zip  $@ 
    +$(RM) $(ZIP2TMP).zip
.ELSE			# "$(ZIP2DIR)" != ""
    +-zip $(ZIP2FLAGS) $@ $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP2TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP2DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    +-zipdep $(ZIP2FLAGS) $@ $(foreach,j,$(ZIP2LIST) $(ZIP2DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP2TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP2DIR)" != ""
    @+-zipdep $(ZIP2FLAGS) $@ $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP2HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP2DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    +-zipdep $(ZIP2FLAGS) $@ $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP2TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP2DIR)" != ""
    +-zipdep $(ZIP2FLAGS) $@ $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP2TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP2DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP3TARGETN)"!=""

ZIP3DIR*=$(ZIPDIR)
ZIP3FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP3TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP3TARGET).dpzz : $(ZIP3TARGETN)

.IF "$(common_build_zip)"!=""
ZIP3HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP3TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP3HELPVAR=$(BIN)$/$(ZIP3TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP3TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP3TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP3TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP3HELPVAR), $(@:db))}) $(ZIP3DIR)) $(command_seperator) zip $(ZIP3FLAGS) $(ZIP3TMP).zip $(ZIP3LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP3TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP3TMP).zip
.ELSE			# "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP3FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP3HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP3TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP3HELPVAR), $(@:db))}) $(ZIP3DIR)) $(command_seperator) zip $(ZIP3FLAGS) $(ZIP3TMP).zip $(ZIP3LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP3TMP).zip  $@ 
    +$(RM) $(ZIP3TMP).zip
.ELSE			# "$(ZIP3DIR)" != ""
    +-zip $(ZIP3FLAGS) $@ $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP3TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP3DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    +-zipdep $(ZIP3FLAGS) $@ $(foreach,j,$(ZIP3LIST) $(ZIP3DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP3TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP3DIR)" != ""
    @+-zipdep $(ZIP3FLAGS) $@ $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP3HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP3DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    +-zipdep $(ZIP3FLAGS) $@ $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP3TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP3DIR)" != ""
    +-zipdep $(ZIP3FLAGS) $@ $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP3TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP3DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP4TARGETN)"!=""

ZIP4DIR*=$(ZIPDIR)
ZIP4FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP4TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP4TARGET).dpzz : $(ZIP4TARGETN)

.IF "$(common_build_zip)"!=""
ZIP4HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP4TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP4HELPVAR=$(BIN)$/$(ZIP4TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP4TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP4TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP4TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP4HELPVAR), $(@:db))}) $(ZIP4DIR)) $(command_seperator) zip $(ZIP4FLAGS) $(ZIP4TMP).zip $(ZIP4LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP4TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP4TMP).zip
.ELSE			# "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP4FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP4HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP4TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP4HELPVAR), $(@:db))}) $(ZIP4DIR)) $(command_seperator) zip $(ZIP4FLAGS) $(ZIP4TMP).zip $(ZIP4LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP4TMP).zip  $@ 
    +$(RM) $(ZIP4TMP).zip
.ELSE			# "$(ZIP4DIR)" != ""
    +-zip $(ZIP4FLAGS) $@ $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP4TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP4DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    +-zipdep $(ZIP4FLAGS) $@ $(foreach,j,$(ZIP4LIST) $(ZIP4DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP4TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP4DIR)" != ""
    @+-zipdep $(ZIP4FLAGS) $@ $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP4HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP4DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    +-zipdep $(ZIP4FLAGS) $@ $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP4TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP4DIR)" != ""
    +-zipdep $(ZIP4FLAGS) $@ $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP4TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP4DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP5TARGETN)"!=""

ZIP5DIR*=$(ZIPDIR)
ZIP5FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP5TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP5TARGET).dpzz : $(ZIP5TARGETN)

.IF "$(common_build_zip)"!=""
ZIP5HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP5TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP5HELPVAR=$(BIN)$/$(ZIP5TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP5TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP5TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP5TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP5HELPVAR), $(@:db))}) $(ZIP5DIR)) $(command_seperator) zip $(ZIP5FLAGS) $(ZIP5TMP).zip $(ZIP5LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP5TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP5TMP).zip
.ELSE			# "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP5FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP5HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP5TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP5HELPVAR), $(@:db))}) $(ZIP5DIR)) $(command_seperator) zip $(ZIP5FLAGS) $(ZIP5TMP).zip $(ZIP5LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP5TMP).zip  $@ 
    +$(RM) $(ZIP5TMP).zip
.ELSE			# "$(ZIP5DIR)" != ""
    +-zip $(ZIP5FLAGS) $@ $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP5TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP5DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    +-zipdep $(ZIP5FLAGS) $@ $(foreach,j,$(ZIP5LIST) $(ZIP5DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP5TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP5DIR)" != ""
    @+-zipdep $(ZIP5FLAGS) $@ $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP5HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP5DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    +-zipdep $(ZIP5FLAGS) $@ $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP5TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP5DIR)" != ""
    +-zipdep $(ZIP5FLAGS) $@ $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP5TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP5DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP6TARGETN)"!=""

ZIP6DIR*=$(ZIPDIR)
ZIP6FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP6TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP6TARGET).dpzz : $(ZIP6TARGETN)

.IF "$(common_build_zip)"!=""
ZIP6HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP6TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP6HELPVAR=$(BIN)$/$(ZIP6TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP6TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP6TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP6TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP6HELPVAR), $(@:db))}) $(ZIP6DIR)) $(command_seperator) zip $(ZIP6FLAGS) $(ZIP6TMP).zip $(ZIP6LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP6TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP6TMP).zip
.ELSE			# "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP6FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP6HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP6TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP6HELPVAR), $(@:db))}) $(ZIP6DIR)) $(command_seperator) zip $(ZIP6FLAGS) $(ZIP6TMP).zip $(ZIP6LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP6TMP).zip  $@ 
    +$(RM) $(ZIP6TMP).zip
.ELSE			# "$(ZIP6DIR)" != ""
    +-zip $(ZIP6FLAGS) $@ $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP6TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP6DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    +-zipdep $(ZIP6FLAGS) $@ $(foreach,j,$(ZIP6LIST) $(ZIP6DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP6TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP6DIR)" != ""
    @+-zipdep $(ZIP6FLAGS) $@ $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP6HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP6DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    +-zipdep $(ZIP6FLAGS) $@ $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP6TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP6DIR)" != ""
    +-zipdep $(ZIP6FLAGS) $@ $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP6TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP6DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP7TARGETN)"!=""

ZIP7DIR*=$(ZIPDIR)
ZIP7FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP7TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP7TARGET).dpzz : $(ZIP7TARGETN)

.IF "$(common_build_zip)"!=""
ZIP7HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP7TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP7HELPVAR=$(BIN)$/$(ZIP7TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP7TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP7TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP7TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP7HELPVAR), $(@:db))}) $(ZIP7DIR)) $(command_seperator) zip $(ZIP7FLAGS) $(ZIP7TMP).zip $(ZIP7LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP7TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP7TMP).zip
.ELSE			# "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP7FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP7HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP7TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP7HELPVAR), $(@:db))}) $(ZIP7DIR)) $(command_seperator) zip $(ZIP7FLAGS) $(ZIP7TMP).zip $(ZIP7LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP7TMP).zip  $@ 
    +$(RM) $(ZIP7TMP).zip
.ELSE			# "$(ZIP7DIR)" != ""
    +-zip $(ZIP7FLAGS) $@ $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP7TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP7DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    +-zipdep $(ZIP7FLAGS) $@ $(foreach,j,$(ZIP7LIST) $(ZIP7DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP7TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP7DIR)" != ""
    @+-zipdep $(ZIP7FLAGS) $@ $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP7HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP7DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    +-zipdep $(ZIP7FLAGS) $@ $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP7TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP7DIR)" != ""
    +-zipdep $(ZIP7FLAGS) $@ $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP7TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP7DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP8TARGETN)"!=""

ZIP8DIR*=$(ZIPDIR)
ZIP8FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP8TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP8TARGET).dpzz : $(ZIP8TARGETN)

.IF "$(common_build_zip)"!=""
ZIP8HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP8TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP8HELPVAR=$(BIN)$/$(ZIP8TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP8TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP8TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP8TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP8HELPVAR), $(@:db))}) $(ZIP8DIR)) $(command_seperator) zip $(ZIP8FLAGS) $(ZIP8TMP).zip $(ZIP8LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP8TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP8TMP).zip
.ELSE			# "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP8FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP8HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP8TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP8HELPVAR), $(@:db))}) $(ZIP8DIR)) $(command_seperator) zip $(ZIP8FLAGS) $(ZIP8TMP).zip $(ZIP8LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP8TMP).zip  $@ 
    +$(RM) $(ZIP8TMP).zip
.ELSE			# "$(ZIP8DIR)" != ""
    +-zip $(ZIP8FLAGS) $@ $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP8TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP8DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    +-zipdep $(ZIP8FLAGS) $@ $(foreach,j,$(ZIP8LIST) $(ZIP8DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP8TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP8DIR)" != ""
    @+-zipdep $(ZIP8FLAGS) $@ $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP8HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP8DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    +-zipdep $(ZIP8FLAGS) $@ $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP8TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP8DIR)" != ""
    +-zipdep $(ZIP8FLAGS) $@ $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP8TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP8DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP9TARGETN)"!=""

ZIP9DIR*=$(ZIPDIR)
ZIP9FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP9TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP9TARGET).dpzz : $(ZIP9TARGETN)

.IF "$(common_build_zip)"!=""
ZIP9HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP9TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP9HELPVAR=$(BIN)$/$(ZIP9TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP9TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP9TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP9TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP9HELPVAR), $(@:db))}) $(ZIP9DIR)) $(command_seperator) zip $(ZIP9FLAGS) $(ZIP9TMP).zip $(ZIP9LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP9TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP9TMP).zip
.ELSE			# "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP9FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP9HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP9TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP9HELPVAR), $(@:db))}) $(ZIP9DIR)) $(command_seperator) zip $(ZIP9FLAGS) $(ZIP9TMP).zip $(ZIP9LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP9TMP).zip  $@ 
    +$(RM) $(ZIP9TMP).zip
.ELSE			# "$(ZIP9DIR)" != ""
    +-zip $(ZIP9FLAGS) $@ $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP9TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP9DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    +-zipdep $(ZIP9FLAGS) $@ $(foreach,j,$(ZIP9LIST) $(ZIP9DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP9TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP9DIR)" != ""
    @+-zipdep $(ZIP9FLAGS) $@ $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP9HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP9DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    +-zipdep $(ZIP9FLAGS) $@ $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP9TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP9DIR)" != ""
    +-zipdep $(ZIP9FLAGS) $@ $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP9TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP9DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP10TARGETN)"!=""

ZIP10DIR*=$(ZIPDIR)
ZIP10FLAGS*=$(ZIPFLAGS)
# gives a uniq filename as long as this dmake is running
ZIP10TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP10TARGET).dpzz : $(ZIP10TARGETN)

.IF "$(common_build_zip)"!=""
ZIP10HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP10TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP10HELPVAR=$(BIN)$/$(ZIP10TARGET)
.ENDIF			# "$(common_build_zip)"!=""

.IF "$(make_zip_deps)" == ""
$(ZIP10TARGETN) : delzip
.ELSE			# "$(make_zip_deps)" == ""
$(ZIP10TARGETN) :
.ENDIF			# "$(make_zip_deps)" == ""
.IF "$(make_zip_deps)" == ""
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP10TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP10HELPVAR), $(@:db))}) $(ZIP10DIR)) $(command_seperator) zip $(ZIP10FLAGS) $(ZIP10TMP).zip $(ZIP10LIST:s/LANGDIR/./) -x delzip $(avoid_cvs_dir) 
    +$(COPY) $(ZIP10TMP).zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP10TMP).zip
.ELSE			# "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +-zip $(ZIP10FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP10HELPVAR), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.new >& $(NULLDEV)
    @+-$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="UNX"
    @+if ( -r $@.new ) $(RENAME) $@.new $@
#	@+if ( -r $@ ) $(TOUCH) $@
.ELSE			# "$(GUI)"=="UNX"
    @+if exist $@.new $(RENAME) $@.new $@
#	@+if exist $@ $(TOUCH) $@
.ENDIF			# "$(GUI)"=="UNX"
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP10TMP).zip >& $(NULLDEV)
    +-cd $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP10HELPVAR), $(@:db))}) $(ZIP10DIR)) $(command_seperator) zip $(ZIP10FLAGS) $(ZIP10TMP).zip $(ZIP10LIST:s/LANGDIR/./) -x delzip  $(avoid_cvs_dir)
    +$(COPY) $(ZIP10TMP).zip  $@ 
    +$(RM) $(ZIP10TMP).zip
.ELSE			# "$(ZIP10DIR)" != ""
    +-zip $(ZIP10FLAGS) $@ $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP10TARGET), $(@:db))}) $j )) -x delzip $(avoid_cvs_dir)
.ENDIF			# "$(ZIP10DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ELSE			# "$(make_zip_deps)" != ""
.IF "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    +-zipdep $(ZIP10FLAGS) $@ $(foreach,j,$(ZIP10LIST) $(ZIP10DIR)$/{$(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP10TARGET), $(@:db))}) $j )}) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP10DIR)" != ""
    @+-zipdep $(ZIP10FLAGS) $@ $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(ZIP10HELPVAR), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP10DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    +-zipdep $(ZIP10FLAGS) $@ $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP10TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ELSE			# "$(ZIP10DIR)" != ""
    +-zipdep $(ZIP10FLAGS) $@ $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,$(longlang_{$(subst,$(BIN)$/$(ZIP10TARGET), $(@:db))}) $j )) >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(ZIP10DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(TARGET).$(PWD:f).$(@:b).dpzz
.ENDIF			# "$(make_zip_deps)" != ""
.ENDIF

# Anweisungen fuer das Linken
