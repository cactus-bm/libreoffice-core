#*************************************************************************
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2000, 2010 Oracle and/or its affiliates.
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#***********************************************************************/

.IF "$(OS)" == "WNT"
my_file = file:///
.ELSE
my_file = file://
.END

# The following conditional is an approximation of: UPDATER set to YES and
# SHIPDRIVE set and CWS_WORK_STAMP not set and either SOL_TMP not set or
# SOLARENV set to a pathname of which SOL_TMP is not a prefix:
.IF "$(UPDATER)" == "YES" && "$(SHIPDRIVE)" != "" && \
    "$(CWS_WORK_STAMP)" == "" && "$(SOLARENV:s/$(SOL_TMP)//" == "$(SOLARENV)"
my_instsets = $(shell ls -dt \
    $(SHIPDRIVE)/$(INPATH)/OpenOffice/archive/$(WORK_STAMP)_$(LAST_MINOR)_native_packed-*_$(defaultlangiso).$(BUILD))
installationtest_instset = $(my_instsets:1)
.ELSE
installationtest_instset = \
    $(SOLARSRC)/instsetoo_native/$(INPATH)/OpenOffice/archive/install/$(defaultlangiso)
.END

.IF "$(OS)" == "WNT"
installationtest_instpath = `cat $(MISC)/$(TARGET)/installation.flag`
.ELSE
installationtest_instpath = $(SOLARVERSION)/$(INPATH)/installation$(UPDMINOREXT)
.END

.IF "$(OS)" == "MACOSX"
my_sofficepath = \
    $(installationtest_instpath)/opt/OpenOffice.org.app/Contents/MacOS/soffice
.ELIF "$(OS)" == "WNT"
my_sofficepath = \
    $(installationtest_instpath)'/opt/OpenOffice.org 3/program/soffice.exe'
.ELSE
my_sofficepath = \
    $(installationtest_instpath)/opt/openoffice.org3/program/soffice
.END

.IF "$(OOO_TEST_SOFFICE)" == ""
my_soffice = path:$(my_sofficepath)
.ELSE
my_soffice = '$(OOO_TEST_SOFFICE:s/'/'\''/)'
.END

.IF "$(OOO_LIBRARY_PATH_VAR)" != ""
my_cppenv = \
    -env:arg-env=$(OOO_LIBRARY_PATH_VAR)"$${{$(OOO_LIBRARY_PATH_VAR)+=$$$(OOO_LIBRARY_PATH_VAR)}}"
my_javaenv = \
    -Dorg.openoffice.test.arg.env=$(OOO_LIBRARY_PATH_VAR)"$${{$(OOO_LIBRARY_PATH_VAR)+=$$$(OOO_LIBRARY_PATH_VAR)}}"
.END

# Work around Windows problems with long pathnames (see issue 50885) by
# installing into the temp directory instead of the module output tree (in which
# case $(TARGET).installation.flag contains the path to the temp installation,
# which is removed after smoketest); can be removed once issue 50885 is fixed;
# on other platforms, a single installation to solver is created in
# smoketestoo_native:
.IF "$(OS)" == "WNT" && "$(OOO_TEST_SOFFICE)" == ""
$(MISC)/$(TARGET)/installation.flag : $(shell \
        ls $(installationtest_instset)/OOo_*_install-arc_$(defaultlangiso).zip)
    $(MKDIRHIER) $(@:d)
    my_tmp=$$(cygpath -m $$(mktemp -dt ooosmoke.XXXXXX)) && \
    unzip $(installationtest_instset)/OOo_*_install-arc_$(defaultlangiso).zip \
        -d "$$my_tmp" && \
    mv "$$my_tmp"/OOo_*_install-arc_$(defaultlangiso) "$$my_tmp"/opt && \
    echo "$$my_tmp" > $@
.END

cpptest .PHONY :
    $(RM) -r $(MISC)/$(TARGET)/user
    $(MKDIRHIER) $(MISC)/$(TARGET)/user
    $(CPPUNITTESTER) \
        -env:UNO_SERVICES=$(my_file)$(SOLARXMLDIR)/ure/services.rdb \
        -env:UNO_TYPES=$(my_file)$(SOLARBINDIR)/types.rdb \
        -env:arg-soffice=$(my_soffice) -env:arg-user=$(MISC)/$(TARGET)/user \
        $(my_cppenv) $(TEST_ARGUMENTS:^"-env:arg-testarg.") $(CPPTEST_LIBRARY)
    $(RM) -r $(MISC)/$(TARGET)/user
.IF "$(OS)" == "WNT" && "$(OOO_TEST_SOFFICE)" == ""
    $(RM) -r $(installationtest_instpath) $(MISC)/$(TARGET)/installation.flag
cpptest : $(MISC)/$(TARGET)/installation.flag
.END

.IF "$(SOLAR_JAVA)" == "TRUE" && "$(OOO_JUNIT_JAR)" != ""
javatest .PHONY : $(JAVATARGET)
    $(RM) -r $(MISC)/$(TARGET)/user
    $(MKDIRHIER) $(MISC)/$(TARGET)/user
    $(JAVAI) $(JAVAIFLAGS) $(JAVACPS) \
        '$(OOO_JUNIT_JAR)$(PATH_SEPERATOR)$(CLASSPATH)' \
        -Dorg.openoffice.test.arg.soffice=$(my_soffice) \
        -Dorg.openoffice.test.arg.user=$(my_file)$(PWD)/$(MISC)/$(TARGET)/user \
        $(my_javaenv) $(TEST_ARGUMENTS:^"-Dorg.openoffice.test.arg.testarg.") \
        org.junit.runner.JUnitCore \
        $(foreach,i,$(JAVATESTFILES) $(subst,/,. $(PACKAGE)).$(i:s/.java//))
    $(RM) -r $(MISC)/$(TARGET)/user
.IF "$(OS)" == "WNT" && "$(OOO_TEST_SOFFICE)" == ""
    $(RM) -r $(installationtest_instpath) $(MISC)/$(TARGET)/installation.flag
javatest : $(MISC)/$(TARGET)/installation.flag
.END
.ELSE
javatest .PHONY :
    echo 'javatest needs SOLAR_JAVA=TRUE and OOO_JUNIT_JAR'
.END
