
# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk


.IF "$(J2EE_HOME)"==""
J2EE_HOME=$(SOLARROOT)$/j2sdkee1.2
.ENDIF

# --- ANT build environment  ---------------------------------------

ANT_HOME=$(SOLARBINDIR)

.IF "$(ANT_OPTIONAL)" ==""
ANT_OPTIONAL=$(SOLARBINDIR)$/jakarta-ant-1.4-optional.jar
.ENDIF

.IF "$(ANT_CLASSPATH)"==""
ANT_CLASSPATH:=$(SOLARBINDIR)$/xerces.jar$(PATH_SEPERATOR)$(CLASSDIR)$/xerces.jar$(PATH_SEPERATOR)$/$(SOLARBINDIR)$/jaxp.jar$(PATH_SEPERATOR)$/$(SOLARBINDIR)$/ant.jar$(PATH_SEPERATOR)$(ANT_OPTIONAL)$(PATH_SEPERATOR)$/$(SOLARBINDIR)$/junit.jar$(PATH_SEPERATOR)$(CLASSDIR)$/ant.jar
.IF "$(ANTPRJ_BOOTSTRAP)"==""
ANT_CLASSPATH!:=$(ANT_CLASSPATH)$(PATH_SEPERATOR)$(SOLARBINDIR)$/antprj.jar
.ENDIF
.ENDIF

ANT=java -Xmx128m org.apache.tools.ant.Main -Djava.home=$(JAVA_HOME) -Dant.home=$(ANT_HOME)

.IF "$(ANT_BUILDFILE)"==""
ANT_BUILDFILE=build.xml
.ENDIF

.IF "$(ANT_DEBUG)"==""
.IF "$(debug)"==""
ANT_DEBUG=off
.ELSE
ANT_DEBUG=on
.ENDIF
.ENDIF

.IF "$(ANT_OPT)"==""
.IF "$(optimize)"==""
ANT_OPT=off
.ELSE
ANT_OPT=on
.ENDIF
.ENDIF

.IF "$(wdkbuild)"==""
ANT_FLAGS!:=-Dprj=$(PRJ) -Dprjname=$(PRJNAME) -Ddebug=$(ANT_DEBUG) -Doptimize=$(ANT_OPT) -Dtarget=$(TARGET) -Dsolar.update=on -Dout=$(OUT) -Dinpath=$(INPATH) -Dproext=$(PROEXT) -Dsolar.bin=$(SOLARBINDIR) -Dsolar.jar=$(SOLARBINDIR) -Dsolar.doc=$(SOLARDOCDIR) -f $(ANT_BUILDFILE) $(ANT_FLAGS) -emacs
.ELSE
ANT_FLAGS!:=-f $(ANT_BUILDFILE) -Dwdk.build=$(wdkbuild) $(ANT_FLAGS) -emacs -v
.ENDIF

.INCLUDE : target.mk

CLASSPATH!:=$(CLASSPATH)$(PATH_SEPERATOR)$(ANT_CLASSPATH)$(PATH_SEPERATOR)$(JAVA_HOME)$/lib$/tools.jar
.EXPORT : CLASSPATH

# --- TARGETS -----------------------------------------------------

#$(CLASSDIR)$/solar.properties .PHONY:
$(CLASSDIR)$/solar.properties : $(SOLARENV)$/inc/minor.mk $(SOLARENV)$/inc/ant.properties
    @echo Making: $@
.IF "$(GUI)"=="UNX"
    @+echo solar.build="$(BUILD)" > $@
    @+echo solar.rscversion="$(RSCVERSION)" >> $@
    @+echo solar.rscrevision="$(RSCREVISION)" >> $@
    @+echo solar.minor="$(LAST_MINOR)" >> $@
    @+echo solar.sourceversion="$(SOURCEVERSION)" >> $@
    @+echo solar.udkstamp="$(UDKSTAMP)" >> $@
    @+echo solar.extstamp="$(EXTSTAMP)" >> $@
.ELSE
    @+echo solar.build=$(BUILD) > $@
    @+echo solar.rscversion=$(RSCVERSION) >> $@
    @+echo solar.rscrevision=$(RSCREVISION) >> $@
    @+echo solar.minor=$(LAST_MINOR) >> $@
    @+echo solar.sourceversion=$(SOURCEVERSION) >> $@
    @+echo solar.udkstamp=$(UDKSTAMP) >> $@
    @+echo solar.extstamp=$(EXTSTAMP) >> $@
.ENDIF
    @+cat $(DMAKEROOT)$/../ant.properties >> $@

ANTBUILD .PHONY:
    $(ANT) $(ANT_FLAGS)

clean  .PHONY:
    $(ANT) $(ANT_FLAGS) $@

prepare .PHONY:
    $(ANT) $(ANT_FLAGS) $@

main:
    $(ANT) $(ANT_FLAGS) $@

info:
    $(ANT) $(ANT_FLAGS) $@

jar .PHONY:
    $(ANT) $(ANT_FLAGS) $@

compile .PHONY:
    $(ANT) $(ANTFLAGS) $@

javadoc .PHONY:
    $(ANT) $(ANT_FLAGS) $@

config .PHONY:
    $(ANT) $(ANT_FLAGS) $@

test:
    $(ANT) $(ANT_FLAGS) $@


