# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID1PARTICLE)"!=""
$(HIDSID1PARTICLE): $(SDI1TARGET) $(MISC)$/$(SDI1NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI1NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI1NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID1PARTICLE)"!=""

.IF "$(SDI1TARGET)"!=""
$(SDI1TARGET): $(SVSDI1DEPEND) $(SDI1NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI1NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI1NAME).ilb	\
    -fm$(MISC)$/$(SDI1NAME).don	\
    -fl$(MISC)$/$(SDI1NAME).lst         \
    -fx$(SDI1EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI1NAME).sid	\
    $(SDI1NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI1TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI1TARGET)"!=""
$(SDI1TARGET): $(SVSDI1DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI1TARGET)"!=""

.IF "$(HIDSID1PARTICLE)"!=""
$(HIDSID1PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID1PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID2PARTICLE)"!=""
$(HIDSID2PARTICLE): $(SDI2TARGET) $(MISC)$/$(SDI2NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI2NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI2NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID2PARTICLE)"!=""

.IF "$(SDI2TARGET)"!=""
$(SDI2TARGET): $(SVSDI2DEPEND) $(SDI2NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI2NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI2NAME).ilb	\
    -fm$(MISC)$/$(SDI2NAME).don	\
    -fl$(MISC)$/$(SDI2NAME).lst         \
    -fx$(SDI2EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI2NAME).sid	\
    $(SDI2NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI2TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI2TARGET)"!=""
$(SDI2TARGET): $(SVSDI2DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI2TARGET)"!=""

.IF "$(HIDSID2PARTICLE)"!=""
$(HIDSID2PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID2PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID3PARTICLE)"!=""
$(HIDSID3PARTICLE): $(SDI3TARGET) $(MISC)$/$(SDI3NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI3NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI3NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID3PARTICLE)"!=""

.IF "$(SDI3TARGET)"!=""
$(SDI3TARGET): $(SVSDI3DEPEND) $(SDI3NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI3NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI3NAME).ilb	\
    -fm$(MISC)$/$(SDI3NAME).don	\
    -fl$(MISC)$/$(SDI3NAME).lst         \
    -fx$(SDI3EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI3NAME).sid	\
    $(SDI3NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI3TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI3TARGET)"!=""
$(SDI3TARGET): $(SVSDI3DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI3TARGET)"!=""

.IF "$(HIDSID3PARTICLE)"!=""
$(HIDSID3PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID3PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID4PARTICLE)"!=""
$(HIDSID4PARTICLE): $(SDI4TARGET) $(MISC)$/$(SDI4NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI4NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI4NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID4PARTICLE)"!=""

.IF "$(SDI4TARGET)"!=""
$(SDI4TARGET): $(SVSDI4DEPEND) $(SDI4NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI4NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI4NAME).ilb	\
    -fm$(MISC)$/$(SDI4NAME).don	\
    -fl$(MISC)$/$(SDI4NAME).lst         \
    -fx$(SDI4EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI4NAME).sid	\
    $(SDI4NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI4TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI4TARGET)"!=""
$(SDI4TARGET): $(SVSDI4DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI4TARGET)"!=""

.IF "$(HIDSID4PARTICLE)"!=""
$(HIDSID4PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID4PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID5PARTICLE)"!=""
$(HIDSID5PARTICLE): $(SDI5TARGET) $(MISC)$/$(SDI5NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI5NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI5NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID5PARTICLE)"!=""

.IF "$(SDI5TARGET)"!=""
$(SDI5TARGET): $(SVSDI5DEPEND) $(SDI5NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI5NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI5NAME).ilb	\
    -fm$(MISC)$/$(SDI5NAME).don	\
    -fl$(MISC)$/$(SDI5NAME).lst         \
    -fx$(SDI5EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI5NAME).sid	\
    $(SDI5NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI5TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI5TARGET)"!=""
$(SDI5TARGET): $(SVSDI5DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI5TARGET)"!=""

.IF "$(HIDSID5PARTICLE)"!=""
$(HIDSID5PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID5PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID6PARTICLE)"!=""
$(HIDSID6PARTICLE): $(SDI6TARGET) $(MISC)$/$(SDI6NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI6NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI6NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID6PARTICLE)"!=""

.IF "$(SDI6TARGET)"!=""
$(SDI6TARGET): $(SVSDI6DEPEND) $(SDI6NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI6NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI6NAME).ilb	\
    -fm$(MISC)$/$(SDI6NAME).don	\
    -fl$(MISC)$/$(SDI6NAME).lst         \
    -fx$(SDI6EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI6NAME).sid	\
    $(SDI6NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI6TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI6TARGET)"!=""
$(SDI6TARGET): $(SVSDI6DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI6TARGET)"!=""

.IF "$(HIDSID6PARTICLE)"!=""
$(HIDSID6PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID6PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID7PARTICLE)"!=""
$(HIDSID7PARTICLE): $(SDI7TARGET) $(MISC)$/$(SDI7NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI7NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI7NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID7PARTICLE)"!=""

.IF "$(SDI7TARGET)"!=""
$(SDI7TARGET): $(SVSDI7DEPEND) $(SDI7NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI7NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI7NAME).ilb	\
    -fm$(MISC)$/$(SDI7NAME).don	\
    -fl$(MISC)$/$(SDI7NAME).lst         \
    -fx$(SDI7EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI7NAME).sid	\
    $(SDI7NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI7TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI7TARGET)"!=""
$(SDI7TARGET): $(SVSDI7DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI7TARGET)"!=""

.IF "$(HIDSID7PARTICLE)"!=""
$(HIDSID7PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID7PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID8PARTICLE)"!=""
$(HIDSID8PARTICLE): $(SDI8TARGET) $(MISC)$/$(SDI8NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI8NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI8NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID8PARTICLE)"!=""

.IF "$(SDI8TARGET)"!=""
$(SDI8TARGET): $(SVSDI8DEPEND) $(SDI8NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI8NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI8NAME).ilb	\
    -fm$(MISC)$/$(SDI8NAME).don	\
    -fl$(MISC)$/$(SDI8NAME).lst         \
    -fx$(SDI8EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI8NAME).sid	\
    $(SDI8NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI8TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI8TARGET)"!=""
$(SDI8TARGET): $(SVSDI8DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI8TARGET)"!=""

.IF "$(HIDSID8PARTICLE)"!=""
$(HIDSID8PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID8PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID9PARTICLE)"!=""
$(HIDSID9PARTICLE): $(SDI9TARGET) $(MISC)$/$(SDI9NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI9NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI9NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID9PARTICLE)"!=""

.IF "$(SDI9TARGET)"!=""
$(SDI9TARGET): $(SVSDI9DEPEND) $(SDI9NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI9NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI9NAME).ilb	\
    -fm$(MISC)$/$(SDI9NAME).don	\
    -fl$(MISC)$/$(SDI9NAME).lst         \
    -fx$(SDI9EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI9NAME).sid	\
    $(SDI9NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI9TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI9TARGET)"!=""
$(SDI9TARGET): $(SVSDI9DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI9TARGET)"!=""

.IF "$(HIDSID9PARTICLE)"!=""
$(HIDSID9PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID9PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
# unroll begin

.IF "$(make_srs_deps)"==""

.IF "$(HIDSID10PARTICLE)"!=""
$(HIDSID10PARTICLE): $(SDI10TARGET) $(MISC)$/$(SDI10NAME).sid
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@.$(ROUT).tmp $@
.IF "$(USE_SHELL)"=="4nt"
    @$(TYPE) $(MISC)$/$(SDI10NAME).sid | $(AWK) "$$1==\"#define\" { print $$2, $$3 }" > $@.$(ROUT).tmp
.ELSE
    @$(TYPE) $(MISC)$/$(SDI10NAME).sid | $(AWK) '$$1=="#define" { print $$2, $$3 }' > $@.$(ROUT).tmp
.ENDIF
    @+-$(RM) $@
    @+$(RENAME) $@.$(ROUT).tmp $@
.ENDIF # "$(HIDSID10PARTICLE)"!=""

.IF "$(SDI10TARGET)"!=""
$(SDI10TARGET): $(SVSDI10DEPEND) $(SDI10NAME).sdi
    @echo ------------------------------
    @echo Making: $@
    @+-$(RM) $@
    $(SVIDL) @$(mktmp \
    -fs$(INCCOMX)$/$(SDI10NAME).hxx	\
    -fd$(INCCOMX)$/$(SDI10NAME).ilb	\
    -fm$(MISC)$/$(SDI10NAME).don	\
    -fl$(MISC)$/$(SDI10NAME).lst         \
    -fx$(SDI10EXPORT).sdi		\
    -fy$(MISC)$/xx$(PRJNAME).csv		\
    -fz$(MISC)$/$(SDI10NAME).sid	\
    $(SDI10NAME).sdi -I$(MISC) -I$(SVSDIINC) -I$(INC) -I$(INCLUDE) -I$(SOLARVER)$/$(UPD)$/$(INPATH)$/inc )
.ENDIF # "$(SDI10TARGET)"!=""

.ELSE			# "$(make_srs_deps)"==""
.IF "$(SDI10TARGET)"!=""
$(SDI10TARGET): $(SVSDI10DEPEND)
    @+echo jetzt nicht...
.ENDIF # "$(SDI10TARGET)"!=""

.IF "$(HIDSID10PARTICLE)"!=""
$(HIDSID10PARTICLE):
    @+echo jetzt nicht...
.ENDIF # "$(HIDSID10PARTICLE)"!=""
.ENDIF			# "$(make_srs_deps)"==""


# Instruction for ???
