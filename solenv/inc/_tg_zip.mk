# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP1TARGET)"!=""
ZIP1DIR*=$(ZIPDIR)
ZIP1FLAGS*=$(ZIPFLAGS)
.IF "$(zip1generatedlangs)"!=""
zip1langdirs*=$(alllangiso)
.ELSE           # "$(zip1generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip1langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP1DIR) . $(ZIP1DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP1DIR) . $(ZIP1DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip1langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP1DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip1generatedlangs)"!=""
.IF "$(ZIP1FORCEALLLANG)"!=""
zip1alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip1langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP1ALLLANG)" != ""
zip1alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip1langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP1ALLLANG)" != ""
.ENDIF			# "$(ZIP1TARGET)"!=""

.IF "$(ZIP1TARGETN)"!=""

ZIP1TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP1TARGET).dpzz : $(ZIP1TARGETN)

.IF "$(common_build_zip)"!=""
ZIP1HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP1TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP1HELPVAR=$(BIN)$/$(ZIP1TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP1DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP1DIR))) $(command_seperator) $(ZIPDEP) $(ZIP1FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP1DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP1LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP1TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP1DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP1FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP1LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP1TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP1DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP1DIR))) $(command_seperator) $(ZIPDEP) $(ZIP1FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP1DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP1LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP1TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP1DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP1FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP1LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP1TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP1DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP1TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP1HELPVAR)_, $(@:db))} $(ZIP1DIR)) $(command_seperator) zip $(ZIP1FLAGS) $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP1LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP1FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,{$(subst,$(ZIP1HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP1DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP1HELPVAR)_, $(@:db))} $(ZIP1DIR)) $(command_seperator) zip $(ZIP1FLAGS) $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP1LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP1TMP).$(ZIP1TARGET){$(subst,$(ZIP1HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP1DIR)" != ""
    +zip $(ZIP1FLAGS) $@ $(foreach,j,$(ZIP1LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP1TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP1DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP2TARGET)"!=""
ZIP2DIR*=$(ZIPDIR)
ZIP2FLAGS*=$(ZIPFLAGS)
.IF "$(zip2generatedlangs)"!=""
zip2langdirs*=$(alllangiso)
.ELSE           # "$(zip2generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip2langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP2DIR) . $(ZIP2DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP2DIR) . $(ZIP2DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip2langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP2DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip2generatedlangs)"!=""
.IF "$(ZIP2FORCEALLLANG)"!=""
zip2alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip2langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP2ALLLANG)" != ""
zip2alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip2langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP2ALLLANG)" != ""
.ENDIF			# "$(ZIP2TARGET)"!=""

.IF "$(ZIP2TARGETN)"!=""

ZIP2TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP2TARGET).dpzz : $(ZIP2TARGETN)

.IF "$(common_build_zip)"!=""
ZIP2HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP2TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP2HELPVAR=$(BIN)$/$(ZIP2TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP2DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP2DIR))) $(command_seperator) $(ZIPDEP) $(ZIP2FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP2DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP2LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP2TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP2DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP2FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP2LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP2TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP2DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP2DIR))) $(command_seperator) $(ZIPDEP) $(ZIP2FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP2DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP2LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP2TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP2DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP2FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP2LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP2TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP2DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP2TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP2HELPVAR)_, $(@:db))} $(ZIP2DIR)) $(command_seperator) zip $(ZIP2FLAGS) $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP2LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP2FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,{$(subst,$(ZIP2HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP2DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP2HELPVAR)_, $(@:db))} $(ZIP2DIR)) $(command_seperator) zip $(ZIP2FLAGS) $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP2LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP2TMP).$(ZIP2TARGET){$(subst,$(ZIP2HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP2DIR)" != ""
    +zip $(ZIP2FLAGS) $@ $(foreach,j,$(ZIP2LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP2TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP2DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP3TARGET)"!=""
ZIP3DIR*=$(ZIPDIR)
ZIP3FLAGS*=$(ZIPFLAGS)
.IF "$(zip3generatedlangs)"!=""
zip3langdirs*=$(alllangiso)
.ELSE           # "$(zip3generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip3langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP3DIR) . $(ZIP3DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP3DIR) . $(ZIP3DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip3langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP3DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip3generatedlangs)"!=""
.IF "$(ZIP3FORCEALLLANG)"!=""
zip3alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip3langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP3ALLLANG)" != ""
zip3alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip3langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP3ALLLANG)" != ""
.ENDIF			# "$(ZIP3TARGET)"!=""

.IF "$(ZIP3TARGETN)"!=""

ZIP3TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP3TARGET).dpzz : $(ZIP3TARGETN)

.IF "$(common_build_zip)"!=""
ZIP3HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP3TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP3HELPVAR=$(BIN)$/$(ZIP3TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP3DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP3DIR))) $(command_seperator) $(ZIPDEP) $(ZIP3FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP3DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP3LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP3TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP3DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP3FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP3LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP3TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP3DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP3DIR))) $(command_seperator) $(ZIPDEP) $(ZIP3FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP3DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP3LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP3TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP3DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP3FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP3LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP3TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP3DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP3TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP3HELPVAR)_, $(@:db))} $(ZIP3DIR)) $(command_seperator) zip $(ZIP3FLAGS) $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP3LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP3FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,{$(subst,$(ZIP3HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP3DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP3HELPVAR)_, $(@:db))} $(ZIP3DIR)) $(command_seperator) zip $(ZIP3FLAGS) $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP3LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP3TMP).$(ZIP3TARGET){$(subst,$(ZIP3HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP3DIR)" != ""
    +zip $(ZIP3FLAGS) $@ $(foreach,j,$(ZIP3LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP3TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP3DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP4TARGET)"!=""
ZIP4DIR*=$(ZIPDIR)
ZIP4FLAGS*=$(ZIPFLAGS)
.IF "$(zip4generatedlangs)"!=""
zip4langdirs*=$(alllangiso)
.ELSE           # "$(zip4generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip4langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP4DIR) . $(ZIP4DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP4DIR) . $(ZIP4DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip4langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP4DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip4generatedlangs)"!=""
.IF "$(ZIP4FORCEALLLANG)"!=""
zip4alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip4langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP4ALLLANG)" != ""
zip4alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip4langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP4ALLLANG)" != ""
.ENDIF			# "$(ZIP4TARGET)"!=""

.IF "$(ZIP4TARGETN)"!=""

ZIP4TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP4TARGET).dpzz : $(ZIP4TARGETN)

.IF "$(common_build_zip)"!=""
ZIP4HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP4TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP4HELPVAR=$(BIN)$/$(ZIP4TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP4DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP4DIR))) $(command_seperator) $(ZIPDEP) $(ZIP4FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP4DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP4LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP4TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP4DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP4FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP4LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP4TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP4DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP4DIR))) $(command_seperator) $(ZIPDEP) $(ZIP4FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP4DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP4LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP4TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP4DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP4FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP4LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP4TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP4DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP4TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP4HELPVAR)_, $(@:db))} $(ZIP4DIR)) $(command_seperator) zip $(ZIP4FLAGS) $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP4LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP4FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,{$(subst,$(ZIP4HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP4DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP4HELPVAR)_, $(@:db))} $(ZIP4DIR)) $(command_seperator) zip $(ZIP4FLAGS) $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP4LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP4TMP).$(ZIP4TARGET){$(subst,$(ZIP4HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP4DIR)" != ""
    +zip $(ZIP4FLAGS) $@ $(foreach,j,$(ZIP4LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP4TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP4DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP5TARGET)"!=""
ZIP5DIR*=$(ZIPDIR)
ZIP5FLAGS*=$(ZIPFLAGS)
.IF "$(zip5generatedlangs)"!=""
zip5langdirs*=$(alllangiso)
.ELSE           # "$(zip5generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip5langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP5DIR) . $(ZIP5DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP5DIR) . $(ZIP5DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip5langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP5DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip5generatedlangs)"!=""
.IF "$(ZIP5FORCEALLLANG)"!=""
zip5alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip5langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP5ALLLANG)" != ""
zip5alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip5langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP5ALLLANG)" != ""
.ENDIF			# "$(ZIP5TARGET)"!=""

.IF "$(ZIP5TARGETN)"!=""

ZIP5TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP5TARGET).dpzz : $(ZIP5TARGETN)

.IF "$(common_build_zip)"!=""
ZIP5HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP5TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP5HELPVAR=$(BIN)$/$(ZIP5TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP5DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP5DIR))) $(command_seperator) $(ZIPDEP) $(ZIP5FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP5DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP5LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP5TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP5DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP5FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP5LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP5TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP5DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP5DIR))) $(command_seperator) $(ZIPDEP) $(ZIP5FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP5DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP5LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP5TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP5DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP5FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP5LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP5TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP5DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP5TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP5HELPVAR)_, $(@:db))} $(ZIP5DIR)) $(command_seperator) zip $(ZIP5FLAGS) $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP5LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP5FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,{$(subst,$(ZIP5HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP5DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP5HELPVAR)_, $(@:db))} $(ZIP5DIR)) $(command_seperator) zip $(ZIP5FLAGS) $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP5LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP5TMP).$(ZIP5TARGET){$(subst,$(ZIP5HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP5DIR)" != ""
    +zip $(ZIP5FLAGS) $@ $(foreach,j,$(ZIP5LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP5TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP5DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP6TARGET)"!=""
ZIP6DIR*=$(ZIPDIR)
ZIP6FLAGS*=$(ZIPFLAGS)
.IF "$(zip6generatedlangs)"!=""
zip6langdirs*=$(alllangiso)
.ELSE           # "$(zip6generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip6langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP6DIR) . $(ZIP6DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP6DIR) . $(ZIP6DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip6langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP6DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip6generatedlangs)"!=""
.IF "$(ZIP6FORCEALLLANG)"!=""
zip6alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip6langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP6ALLLANG)" != ""
zip6alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip6langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP6ALLLANG)" != ""
.ENDIF			# "$(ZIP6TARGET)"!=""

.IF "$(ZIP6TARGETN)"!=""

ZIP6TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP6TARGET).dpzz : $(ZIP6TARGETN)

.IF "$(common_build_zip)"!=""
ZIP6HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP6TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP6HELPVAR=$(BIN)$/$(ZIP6TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP6DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP6DIR))) $(command_seperator) $(ZIPDEP) $(ZIP6FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP6DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP6LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP6TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP6DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP6FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP6LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP6TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP6DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP6DIR))) $(command_seperator) $(ZIPDEP) $(ZIP6FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP6DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP6LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP6TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP6DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP6FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP6LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP6TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP6DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP6TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP6HELPVAR)_, $(@:db))} $(ZIP6DIR)) $(command_seperator) zip $(ZIP6FLAGS) $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP6LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP6FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,{$(subst,$(ZIP6HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP6DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP6HELPVAR)_, $(@:db))} $(ZIP6DIR)) $(command_seperator) zip $(ZIP6FLAGS) $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP6LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP6TMP).$(ZIP6TARGET){$(subst,$(ZIP6HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP6DIR)" != ""
    +zip $(ZIP6FLAGS) $@ $(foreach,j,$(ZIP6LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP6TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP6DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP7TARGET)"!=""
ZIP7DIR*=$(ZIPDIR)
ZIP7FLAGS*=$(ZIPFLAGS)
.IF "$(zip7generatedlangs)"!=""
zip7langdirs*=$(alllangiso)
.ELSE           # "$(zip7generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip7langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP7DIR) . $(ZIP7DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP7DIR) . $(ZIP7DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip7langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP7DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip7generatedlangs)"!=""
.IF "$(ZIP7FORCEALLLANG)"!=""
zip7alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip7langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP7ALLLANG)" != ""
zip7alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip7langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP7ALLLANG)" != ""
.ENDIF			# "$(ZIP7TARGET)"!=""

.IF "$(ZIP7TARGETN)"!=""

ZIP7TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP7TARGET).dpzz : $(ZIP7TARGETN)

.IF "$(common_build_zip)"!=""
ZIP7HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP7TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP7HELPVAR=$(BIN)$/$(ZIP7TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP7DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP7DIR))) $(command_seperator) $(ZIPDEP) $(ZIP7FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP7DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP7LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP7TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP7DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP7FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP7LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP7TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP7DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP7DIR))) $(command_seperator) $(ZIPDEP) $(ZIP7FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP7DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP7LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP7TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP7DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP7FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP7LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP7TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP7DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP7TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP7HELPVAR)_, $(@:db))} $(ZIP7DIR)) $(command_seperator) zip $(ZIP7FLAGS) $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP7LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP7FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,{$(subst,$(ZIP7HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP7DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP7HELPVAR)_, $(@:db))} $(ZIP7DIR)) $(command_seperator) zip $(ZIP7FLAGS) $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP7LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP7TMP).$(ZIP7TARGET){$(subst,$(ZIP7HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP7DIR)" != ""
    +zip $(ZIP7FLAGS) $@ $(foreach,j,$(ZIP7LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP7TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP7DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP8TARGET)"!=""
ZIP8DIR*=$(ZIPDIR)
ZIP8FLAGS*=$(ZIPFLAGS)
.IF "$(zip8generatedlangs)"!=""
zip8langdirs*=$(alllangiso)
.ELSE           # "$(zip8generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip8langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP8DIR) . $(ZIP8DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP8DIR) . $(ZIP8DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip8langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP8DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip8generatedlangs)"!=""
.IF "$(ZIP8FORCEALLLANG)"!=""
zip8alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip8langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP8ALLLANG)" != ""
zip8alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip8langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP8ALLLANG)" != ""
.ENDIF			# "$(ZIP8TARGET)"!=""

.IF "$(ZIP8TARGETN)"!=""

ZIP8TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP8TARGET).dpzz : $(ZIP8TARGETN)

.IF "$(common_build_zip)"!=""
ZIP8HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP8TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP8HELPVAR=$(BIN)$/$(ZIP8TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP8DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP8DIR))) $(command_seperator) $(ZIPDEP) $(ZIP8FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP8DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP8LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP8TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP8DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP8FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP8LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP8TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP8DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP8DIR))) $(command_seperator) $(ZIPDEP) $(ZIP8FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP8DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP8LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP8TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP8DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP8FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP8LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP8TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP8DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP8TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP8HELPVAR)_, $(@:db))} $(ZIP8DIR)) $(command_seperator) zip $(ZIP8FLAGS) $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP8LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP8FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,{$(subst,$(ZIP8HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP8DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP8HELPVAR)_, $(@:db))} $(ZIP8DIR)) $(command_seperator) zip $(ZIP8FLAGS) $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP8LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP8TMP).$(ZIP8TARGET){$(subst,$(ZIP8HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP8DIR)" != ""
    +zip $(ZIP8FLAGS) $@ $(foreach,j,$(ZIP8LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP8TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP8DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP9TARGET)"!=""
ZIP9DIR*=$(ZIPDIR)
ZIP9FLAGS*=$(ZIPFLAGS)
.IF "$(zip9generatedlangs)"!=""
zip9langdirs*=$(alllangiso)
.ELSE           # "$(zip9generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip9langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP9DIR) . $(ZIP9DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP9DIR) . $(ZIP9DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip9langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP9DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip9generatedlangs)"!=""
.IF "$(ZIP9FORCEALLLANG)"!=""
zip9alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip9langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP9ALLLANG)" != ""
zip9alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip9langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP9ALLLANG)" != ""
.ENDIF			# "$(ZIP9TARGET)"!=""

.IF "$(ZIP9TARGETN)"!=""

ZIP9TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP9TARGET).dpzz : $(ZIP9TARGETN)

.IF "$(common_build_zip)"!=""
ZIP9HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP9TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP9HELPVAR=$(BIN)$/$(ZIP9TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP9DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP9DIR))) $(command_seperator) $(ZIPDEP) $(ZIP9FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP9DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP9LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP9TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP9DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP9FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP9LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP9TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP9DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP9DIR))) $(command_seperator) $(ZIPDEP) $(ZIP9FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP9DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP9LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP9TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP9DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP9FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP9LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP9TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP9DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP9TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP9HELPVAR)_, $(@:db))} $(ZIP9DIR)) $(command_seperator) zip $(ZIP9FLAGS) $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP9LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP9FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,{$(subst,$(ZIP9HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP9DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP9HELPVAR)_, $(@:db))} $(ZIP9DIR)) $(command_seperator) zip $(ZIP9FLAGS) $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP9LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP9TMP).$(ZIP9TARGET){$(subst,$(ZIP9HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP9DIR)" != ""
    +zip $(ZIP9FLAGS) $@ $(foreach,j,$(ZIP9LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP9TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP9DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

# unroll begin

.IF "$(GUI)"=="WNT"
command_seperator=&&
#command_seperator=^
avoid_cvs_dir=-x "*CVS*"
.ELSE
command_seperator=;
avoid_cvs_dir=-x "*CVS*"
.ENDIF

.IF "$(ZIP10TARGET)"!=""
ZIP10DIR*=$(ZIPDIR)
ZIP10FLAGS*=$(ZIPFLAGS)
.IF "$(zip10generatedlangs)"!=""
zip10langdirs*=$(alllangiso)
.ELSE           # "$(zip10generatedlangs)"!=""
.IF "$(GUI)"=="UNX" || "$(USE_SHELL)"!="4nt"
zip10langdirs:=$(shell +-test -d {$(subst,$/$(LANGDIR), $(null,$(ZIP10DIR) . $(ZIP10DIR)))}/ && find {$(subst,$/$(LANGDIR), $(null,$(ZIP10DIR) . $(ZIP10DIR)))}/ -type d ! -name CVS ! -name "." | sed "s/\.\/\///" | sed "s/\.\///" )
.ELSE			# "$(GUI)"=="UNX"
zip10langdirs:=$(subst,CVS, $(shell +-dir {$(subst,$/$(LANGDIR), $(ZIP10DIR))} /ba:d ))
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(zip10generatedlangs)"!=""
.IF "$(ZIP10FORCEALLLANG)"!=""
zip10alllangiso:=$(foreach,i,$(completelangiso) $(foreach,j,$(zip10langdirs) $(eq,$i,$j  $i $(NULL))))
.ELSE          # "$(ZIP10ALLLANG)" != ""
zip10alllangiso*:=$(foreach,i,$(alllangiso) $(foreach,j,$(zip10langdirs) $(eq,$i,$j  $i $(NULL))))
.ENDIF          # "$(ZIP10ALLLANG)" != ""
.ENDIF			# "$(ZIP10TARGET)"!=""

.IF "$(ZIP10TARGETN)"!=""

ZIP10TMP:=$(mktmp iii)

$(MISC)$/$(TARGET).$(PWD:f).$(ZIP10TARGET).dpzz : $(ZIP10TARGETN)

.IF "$(common_build_zip)"!=""
ZIP10HELPVAR={$(subst,$(OUTPATH),$(COMMON_OUTDIR) $(BIN))$/$(ZIP10TARGET)}
.ELSE			# "$(common_build_zip)"!=""
ZIP10HELPVAR=$(BIN)$/$(ZIP10TARGET)
.ENDIF			# "$(common_build_zip)"!=""

$(ZIP10DEPFILE) :
    +echo # > $(MISC)$/$(@:f)
.IF "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    @+echo type 1
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP10DIR))) $(command_seperator) $(ZIPDEP) $(ZIP10FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP10DIR)))$/ $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP10LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP10TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP10DIR)" != ""
    @+echo type 2
    +-$(ZIPDEP) $(ZIP10FLAGS) $(subst,$(OUTPATH),$(COMMON_OUTDIR) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/))) $(foreach,j,$(ZIP10LIST) "{$(subst,LANGDIR,{$(subst,$(ZIP10TARGET)_, $(@:f:b))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP10DIR)" != ""
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    @+echo type 3
    +-$(CDD) $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP10DIR))) $(command_seperator) $(ZIPDEP) $(ZIP10FLAGS) -prefix $(subst,LANGDIR,. $(subst,$/LANGDIR, $(ZIP10DIR)))$/ $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP10LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP10TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(PWD)$/$(PRJ)$/$(ROUT)$/misc$/$(@:f)
.ELSE			# "$(ZIP10DIR)" != ""
    @+echo type 4
    +-$(ZIPDEP) $(ZIP10FLAGS) $(subst,$/misc$/,$/bin$/ $(@:s/.dpzz/.zip/)) $(foreach,j,$(ZIP10LIST) "{$(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP10TARGET)_, $(subst,$/misc$/,$/bin$/ $(@:db)))} $j )}") $(avoid_cvs_dir) >> $(MISC)$/$(@:f)
.ENDIF			# "$(ZIP10DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
    @+echo $@ : makefile.mk >> $(MISC)$/$(@:f)


$(ZIP10TARGETN) : delzip
    @+echo ------------------------------
    @+echo Making: $@
    @$(!eq,$?,$(?:s/delzip/zzz/) +-$(RM) +echo) $(uniq $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)) >&$(NULLDEV)
    @+echo rebuilding zipfiles $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
    @+echo ------------------------------ $(eq,$?,$(?:s/delzip/zzz/) >&$(NULLDEV) )
.IF "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP10HELPVAR)_, $(@:db))} $(ZIP10DIR)) $(command_seperator) zip $(ZIP10FLAGS) $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP10LIST:s/LANGDIR/LANGDIR_away/)) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)    
    +$(RM) $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) >& $(NULLDEV)
    +zip $(ZIP10FLAGS) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,{$(subst,$(ZIP10HELPVAR)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@) $@.$(INPATH) >& $(NULLDEV)
    @+-$(RM) $(subst,$(COMMON_OUTDIR),$(OUTPATH) $@)
    @+-$(RM) $@ >& $(NULLDEV)
    @+$(IFEXIST) $@.$(INPATH) $(THEN) $(RENAME) $@.$(INPATH) $@
#	@+$(IFEXIST) $@ $(THEN) $(TOUCH) $@
.ELSE			# "$(common_build_zip)"!=""
.IF "$(ZIP10DIR)" != ""
    @+-$(GNUCOPY) -p $@ $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip >& $(NULLDEV)
    +-$(CDD) $(subst,LANGDIR,{$(subst,$(ZIP10HELPVAR)_, $(@:db))} $(ZIP10DIR)) $(command_seperator) zip $(ZIP10FLAGS) $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip $(subst,LANGDIR_away$/, $(ZIP10LIST:s/LANGDIR/LANGDIR_away/)) -x delzip  $(avoid_cvs_dir) $(CHECKZIPRESULT)
    +$(COPY) $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip  $@ 
    +$(RM) $(ZIP10TMP).$(ZIP10TARGET){$(subst,$(ZIP10HELPVAR),_ $(@:db))}.zip
.ELSE			# "$(ZIP10DIR)" != ""
    +zip $(ZIP10FLAGS) $@ $(foreach,j,$(ZIP10LIST) $(subst,LANGDIR,{$(subst,$(BIN)$/$(ZIP10TARGET)_, $(@:db))} $j )) -x delzip $(avoid_cvs_dir) $(CHECKZIPRESULT)
.ENDIF			# "$(ZIP10DIR)" != ""
.ENDIF			# "$(common_build_zip)"!=""
.ENDIF

