MKFILENAME:=tg_lib.mk

#######################################################
# lines needed for rekursion

.IF "$(MULTI_LIB_FLAG)" == ""
$(LIB1TARGETN) .NULL : LIB1

$(LIB2TARGETN) .NULL : LIB2

$(LIB3TARGETN) .NULL : LIB3

$(LIB4TARGETN) .NULL : LIB4

$(LIB5TARGETN) .NULL : LIB5

$(LIB6TARGETN) .NULL : LIB6

$(LIB7TARGETN) .NULL : LIB7

$(LIB8TARGETN) .NULL : LIB8

$(LIB9TARGETN) .NULL : LIB9
.ENDIF



.IF "$(MULTI_LIB_FLAG)"==""
LIB1 LIB2 LIB3 LIB4 LIB5 LIB6 LIB7 LIB8 LIB9:
    @dmake $(LIB$(TNR)TARGETN) MULTI_LIB_FLAG=true TNR:=$(TNR) $(MFLAGS) $(CALLMACROS)
.ELSE

#######################################################
# Anweisungen fuer das Linken
# unroll begin

.IF "$(LIB$(TNR)TARGET)" != ""
.IF "$(LIB$(TNR)ARCHIV)" != ""

$(LIB$(TNR)ARCHIV) :	$(LIB$(TNR)TARGET)
    @echo Making: $@
    @-+$(RM) $@ >& $(NULLDEV)
.IF "$(GUI)"=="MAC"
    +$(LIBMGR) $(LIBFLAGS) -o $(shell $(UNIX2MACPATH) $(LIB$(TNR)ARCHIV) `cat /dev/null $@ | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g`)
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(GUI)"=="UNX"
    @+-$(RM) $(MISC)$/$(LIB$(TNR)ARCHIV:b).cmd
.IF "$(OS)" =="HPUX_FRAG_HR"
    @+-$(RM) $(MISC)$/$(LIB$(TNR)ARCHIV:b)_closetempl.cmd
    @+echo $(LINK) +inst_close -c `cat $(LIB$(TNR)TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB$(TNR)ARCHIV:b)_closetempl.cmd
    @cat $(MISC)$/$(LIB$(TNR)ARCHIV:b)_closetempl.cmd
    @source $(MISC)$/$(LIB$(TNR)ARCHIV:b)_closetempl.cmd
.ENDIF
    @+echo $(LIBMGR) $(LIB$(TNR)FLAGS) $(LIBFLAGS) $(LIB$(TNR)ARCHIV) `cat $(LIB$(TNR)TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(MISC)$/$(LIB$(TNR)ARCHIV:b).cmd
.IF "$(OS)$(COM)"=="NETBSDGCC"
    @+echo  ranlib $(LIB$(TNR)ARCHIV) >> $(MISC)$/$(LIB$(TNR)ARCHIV:b).cmd
.ENDIF
    @cat $(MISC)$/$(LIB$(TNR)ARCHIV:b).cmd
    @source $(MISC)$/$(LIB$(TNR)ARCHIV:b).cmd
.ELSE			# "$(GUI)"=="UNX"
    @echo just a dummy > $@
.ENDIF			# "$(GUI)"=="UNX"
.ENDIF          # "$(GUI)"=="MAC"

.ENDIF			# "$(LIB$(TNR)ARCHIV)" != ""

$(LIB$(TNR)TARGET) :	$(LIB$(TNR)FILES) \
                        $(LIB$(TNR)OBJFILES) \
                        $(LIB$(TNR)DEPN)
    @echo using: $(LIB$(TNR)FILES)
    @echo using: $(LIB$(TNR)TARGET)
    @echo ------------------------------
    @echo Making: $@
.IF "$(GUI)"!="MAC"
.IF "$(GUI)"!="UNX"
    @-+IF EXIST $@ del $@
.ENDIF
.ENDIF
.IF "$(GUI)"=="UNX"
    @+$(RM) $@
    @+echo $(LIB$(TNR)OBJFILES:s/.obj/.o/) | sed "s#$(PRJ:s/./\./)$/$(ROUT)#$(ROUT)#g" | xargs -n 1 > $@
    @+cat /dev/null $(LIB$(TNR)FILES:s/.obj/.o/) | xargs -n 1 >> $@
    @+$(RM) $(@:d)$(@:b).dump
.IF "$(OS)"=="MACOSX"
    @-+nm `cat $(LIB$(TNR)TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.ELSE
    @+nm `cat $(LIB$(TNR)TARGET) | sed s\#'^'$(ROUT)\#$(PRJ)$/$(ROUT)\#g` > $(@:d)$(@:b).dump
.ENDIF
.ELSE			# "$(GUI)"=="UNX"
.IF "$(GUI)"=="MAC"
    @+$(RM) $@
    @+echo $(LIB$(TNR)OBJFILES) | sed s\#$(PRJ:s/./\./)$/$(ROUT)\#$(ROUT)\#g | xargs -n 1 > $@
    @+cat /dev/null $(LIB$(TNR)FILES) | xargs -n 1 >> $@
.ELSE                   # "$(GUI)"=="MAC"
.IF "$(GUI)"=="WNT"
    $(LIBMGR) $(LIBFLAGS) /OUT:$@ @$(mktmp $(LIB$(TNR)FILES) $(LIB$(TNR)OBJFILES))
    @+echo.
.ELSE			# "$(GUI)"=="WNT"
    @+-$(RM) $@
.IF "$(COM)"=="ICC"
    $(LIBMGR) $(LIBFLAGS) $@ @$(mktmp $(LIB$(TNR)FILES:+"&\n") \
    $(LIB$(TNR)OBJFILES:+"&\n");)
.ELSE
    +echo $(LIBMGR) r $@ $(LIB$(TNR)OBJFILES)
    $(LIBMGR) r $@ $(LIB$(TNR)OBJFILES) $(LIB$(TNR)FILES) bla.lib
.ENDIF
.ENDIF          # "$(GUI)"=="WNT"
.ENDIF          # "$(GUI)"=="MAC"
.ENDIF          # "$(GUI)"=="UNX"
.ENDIF          # "$(LIB$(TNR)TARGET)" != ""

# Anweisungen fuer das LIBTARGETs
# unroll end
#######################################################

.ENDIF			# "$(MULTI_LIB_FLAG)" == ""
