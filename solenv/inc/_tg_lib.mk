# unroll begin

.IF "$(LIB1TARGET)" != ""

$(LIB1TARGET) :	$(LIB1FILES) \
                        $(LIB1OBJFILES) \
                        $(LIB1DEPN)
    @echo using: $(LIB1FILES)
    @echo using: $(LIB1TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB1OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB1FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB1TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB1ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB1ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB1ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB1TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB1ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB1ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB1ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB1FLAGS) $(LIBFLAGS) $(LIB1ARCHIV) `cat $(LIB1TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB1ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB1ARCHIV) >> $(MISC)$/$(LIB1ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB1ARCHIV:b).cmd
    @source $(MISC)$/$(LIB1ARCHIV:b).cmd
.ENDIF			# "$(LIB1ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB1OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB1FILES) | xargs -n 1 >> $@
.IF "$(LIB1ARCHIV)" != ""
    @+$(RM) $(LIB1ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB1ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB1ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB1FILES:+"\n+":^"&") -+$(LIB1OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB1FILES) $(LIB1OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB1FILES:+"&\n") \
    $(LIB1OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB1OBJFILES)
    $(LIBMGR) r $@ $(LIB1OBJFILES) $(LIB1FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB1TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB2TARGET)" != ""

$(LIB2TARGET) :	$(LIB2FILES) \
                        $(LIB2OBJFILES) \
                        $(LIB2DEPN)
    @echo using: $(LIB2FILES)
    @echo using: $(LIB2TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB2OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB2FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB2TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB2ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB2ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB2ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB2TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB2ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB2ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB2ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB2FLAGS) $(LIBFLAGS) $(LIB2ARCHIV) `cat $(LIB2TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB2ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB2ARCHIV) >> $(MISC)$/$(LIB2ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB2ARCHIV:b).cmd
    @source $(MISC)$/$(LIB2ARCHIV:b).cmd
.ENDIF			# "$(LIB2ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB2OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB2FILES) | xargs -n 1 >> $@
.IF "$(LIB2ARCHIV)" != ""
    @+$(RM) $(LIB2ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB2ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB2ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB2FILES:+"\n+":^"&") -+$(LIB2OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB2FILES) $(LIB2OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB2FILES:+"&\n") \
    $(LIB2OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB2OBJFILES)
    $(LIBMGR) r $@ $(LIB2OBJFILES) $(LIB2FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB2TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB3TARGET)" != ""

$(LIB3TARGET) :	$(LIB3FILES) \
                        $(LIB3OBJFILES) \
                        $(LIB3DEPN)
    @echo using: $(LIB3FILES)
    @echo using: $(LIB3TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB3OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB3FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB3TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB3ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB3ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB3ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB3TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB3ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB3ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB3ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB3FLAGS) $(LIBFLAGS) $(LIB3ARCHIV) `cat $(LIB3TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB3ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB3ARCHIV) >> $(MISC)$/$(LIB3ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB3ARCHIV:b).cmd
    @source $(MISC)$/$(LIB3ARCHIV:b).cmd
.ENDIF			# "$(LIB3ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB3OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB3FILES) | xargs -n 1 >> $@
.IF "$(LIB3ARCHIV)" != ""
    @+$(RM) $(LIB3ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB3ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB3ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB3FILES:+"\n+":^"&") -+$(LIB3OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB3FILES) $(LIB3OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB3FILES:+"&\n") \
    $(LIB3OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB3OBJFILES)
    $(LIBMGR) r $@ $(LIB3OBJFILES) $(LIB3FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB3TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB4TARGET)" != ""

$(LIB4TARGET) :	$(LIB4FILES) \
                        $(LIB4OBJFILES) \
                        $(LIB4DEPN)
    @echo using: $(LIB4FILES)
    @echo using: $(LIB4TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB4OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB4FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB4TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB4ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB4ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB4ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB4TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB4ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB4ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB4ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB4FLAGS) $(LIBFLAGS) $(LIB4ARCHIV) `cat $(LIB4TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB4ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB4ARCHIV) >> $(MISC)$/$(LIB4ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB4ARCHIV:b).cmd
    @source $(MISC)$/$(LIB4ARCHIV:b).cmd
.ENDIF			# "$(LIB4ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB4OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB4FILES) | xargs -n 1 >> $@
.IF "$(LIB4ARCHIV)" != ""
    @+$(RM) $(LIB4ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB4ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB4ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB4FILES:+"\n+":^"&") -+$(LIB4OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB4FILES) $(LIB4OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB4FILES:+"&\n") \
    $(LIB4OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB4OBJFILES)
    $(LIBMGR) r $@ $(LIB4OBJFILES) $(LIB4FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB4TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB5TARGET)" != ""

$(LIB5TARGET) :	$(LIB5FILES) \
                        $(LIB5OBJFILES) \
                        $(LIB5DEPN)
    @echo using: $(LIB5FILES)
    @echo using: $(LIB5TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB5OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB5FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB5TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB5ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB5ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB5ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB5TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB5ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB5ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB5ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB5FLAGS) $(LIBFLAGS) $(LIB5ARCHIV) `cat $(LIB5TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB5ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB5ARCHIV) >> $(MISC)$/$(LIB5ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB5ARCHIV:b).cmd
    @source $(MISC)$/$(LIB5ARCHIV:b).cmd
.ENDIF			# "$(LIB5ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB5OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB5FILES) | xargs -n 1 >> $@
.IF "$(LIB5ARCHIV)" != ""
    @+$(RM) $(LIB5ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB5ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB5ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB5FILES:+"\n+":^"&") -+$(LIB5OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB5FILES) $(LIB5OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB5FILES:+"&\n") \
    $(LIB5OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB5OBJFILES)
    $(LIBMGR) r $@ $(LIB5OBJFILES) $(LIB5FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB5TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB6TARGET)" != ""

$(LIB6TARGET) :	$(LIB6FILES) \
                        $(LIB6OBJFILES) \
                        $(LIB6DEPN)
    @echo using: $(LIB6FILES)
    @echo using: $(LIB6TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB6OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB6FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB6TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB6ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB6ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB6ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB6TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB6ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB6ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB6ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB6FLAGS) $(LIBFLAGS) $(LIB6ARCHIV) `cat $(LIB6TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB6ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB6ARCHIV) >> $(MISC)$/$(LIB6ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB6ARCHIV:b).cmd
    @source $(MISC)$/$(LIB6ARCHIV:b).cmd
.ENDIF			# "$(LIB6ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB6OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB6FILES) | xargs -n 1 >> $@
.IF "$(LIB6ARCHIV)" != ""
    @+$(RM) $(LIB6ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB6ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB6ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB6FILES:+"\n+":^"&") -+$(LIB6OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB6FILES) $(LIB6OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB6FILES:+"&\n") \
    $(LIB6OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB6OBJFILES)
    $(LIBMGR) r $@ $(LIB6OBJFILES) $(LIB6FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB6TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB7TARGET)" != ""

$(LIB7TARGET) :	$(LIB7FILES) \
                        $(LIB7OBJFILES) \
                        $(LIB7DEPN)
    @echo using: $(LIB7FILES)
    @echo using: $(LIB7TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB7OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB7FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB7TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB7ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB7ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB7ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB7TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB7ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB7ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB7ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB7FLAGS) $(LIBFLAGS) $(LIB7ARCHIV) `cat $(LIB7TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB7ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB7ARCHIV) >> $(MISC)$/$(LIB7ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB7ARCHIV:b).cmd
    @source $(MISC)$/$(LIB7ARCHIV:b).cmd
.ENDIF			# "$(LIB7ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB7OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB7FILES) | xargs -n 1 >> $@
.IF "$(LIB7ARCHIV)" != ""
    @+$(RM) $(LIB7ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB7ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB7ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB7FILES:+"\n+":^"&") -+$(LIB7OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB7FILES) $(LIB7OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB7FILES:+"&\n") \
    $(LIB7OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB7OBJFILES)
    $(LIBMGR) r $@ $(LIB7OBJFILES) $(LIB7FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB7TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB8TARGET)" != ""

$(LIB8TARGET) :	$(LIB8FILES) \
                        $(LIB8OBJFILES) \
                        $(LIB8DEPN)
    @echo using: $(LIB8FILES)
    @echo using: $(LIB8TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB8OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB8FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB8TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB8ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB8ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB8ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB8TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB8ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB8ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB8ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB8FLAGS) $(LIBFLAGS) $(LIB8ARCHIV) `cat $(LIB8TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB8ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB8ARCHIV) >> $(MISC)$/$(LIB8ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB8ARCHIV:b).cmd
    @source $(MISC)$/$(LIB8ARCHIV:b).cmd
.ENDIF			# "$(LIB8ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB8OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB8FILES) | xargs -n 1 >> $@
.IF "$(LIB8ARCHIV)" != ""
    @+$(RM) $(LIB8ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB8ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB8ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB8FILES:+"\n+":^"&") -+$(LIB8OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB8FILES) $(LIB8OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB8FILES:+"&\n") \
    $(LIB8OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB8OBJFILES)
    $(LIBMGR) r $@ $(LIB8OBJFILES) $(LIB8FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB8TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB9TARGET)" != ""

$(LIB9TARGET) :	$(LIB9FILES) \
                        $(LIB9OBJFILES) \
                        $(LIB9DEPN)
    @echo using: $(LIB9FILES)
    @echo using: $(LIB9TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB9OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB9FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB9TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB9ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB9ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB9ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB9TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB9ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB9ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB9ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB9FLAGS) $(LIBFLAGS) $(LIB9ARCHIV) `cat $(LIB9TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB9ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB9ARCHIV) >> $(MISC)$/$(LIB9ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB9ARCHIV:b).cmd
    @source $(MISC)$/$(LIB9ARCHIV:b).cmd
.ENDIF			# "$(LIB9ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB9OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB9FILES) | xargs -n 1 >> $@
.IF "$(LIB9ARCHIV)" != ""
    @+$(RM) $(LIB9ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB9ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB9ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB9FILES:+"\n+":^"&") -+$(LIB9OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB9FILES) $(LIB9OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB9FILES:+"&\n") \
    $(LIB9OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB9OBJFILES)
    $(LIBMGR) r $@ $(LIB9OBJFILES) $(LIB9FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB9TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
# unroll begin

.IF "$(LIB10TARGET)" != ""

$(LIB10TARGET) :	$(LIB10FILES) \
                        $(LIB10OBJFILES) \
                        $(LIB10DEPN)
    @echo using: $(LIB10FILES)
    @echo using: $(LIB10TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB10OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB10FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
    @+nm `cat $(LIB10TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.IF "$(LIB10ARCHIV)" != ""
    @+-$(RM) $(MISC)$/$(LIB10ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB10ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB10TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB10ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB10ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB10ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB10FLAGS) $(LIBFLAGS) $(LIB10ARCHIV) `cat $(LIB10TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB10ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB10ARCHIV) >> $(MISC)$/$(LIB10ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB10ARCHIV:b).cmd
    @source $(MISC)$/$(LIB10ARCHIV:b).cmd
.ENDIF			# "$(LIB10ARCHIV)" != ""
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB10OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB10FILES) | xargs -n 1 >> $@
.IF "$(LIB10ARCHIV)" != ""
    @+$(RM) $(LIB10ARCHIV)
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB10ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ENDIF			# "$(LIB10ARCHIV)" != ""
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(COM)" == "BLC"
    @+-$(RM) $@ >& $(NULLDEV)
    $(LIBMGR) $@ $(LIBFLAGS) +$(LIB10FILES:+"\n+":^"&") -+$(LIB10OBJFILES:+"\n+":^"&")
.ELSE			# "$(COM)" == "BLC"
.IF "$(GUI)"=="WNT"
# wnt ist hier
# $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(?:+"\n")) - warum ist das schrott?
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB10FILES) $(LIB10OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
# os2icci3 hier
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB10FILES:+"&\n") \
    $(LIB10OBJFILES:+"&\n");)
.ELSE
# os2gcci hier
    +echo $(LIBMGR) r $@ $(LIB10OBJFILES)
    $(LIBMGR) r $@ $(LIB10OBJFILES) $(LIB10FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF			# "$(COM)" == "BLC"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB10TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
#next Target
