#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.2 $
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
#
#*************************************************************************

PRJ = ..
TARGET  = FormIntegrationTest
PRJNAME = forms

# --- Settings -----------------------------------------------------
.INCLUDE: settings.mk


#----- compile .java files -----------------------------------------

JARFILES        = ridl.jar unoil.jar jurt.jar juh.jar jut.jar java_uno.jar OOoRunner.jar ConnectivityTools.jar
JAVAFILES       :=  $(shell @$(FIND) org -name "*.java") \
                    $(shell @$(FIND) integration -name "*.java")
JAVACLASSFILES	:= $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(i:d)$/$(i:b).class)

#----- make a jar from compiled files ------------------------------

MAXLINELENGTH = 100000

#JARCLASSDIRS    =
JARTARGET       = $(TARGET).jar
JARCOMPRESS 	= TRUE

# --- Runner Settings ----------------------------------------------

# create connection string for OOoRunner
.IF "$(RUNNER_CONNECTION_STRING)" == ""
    .IF "$(OOO_RUNNER_PORT)" == ""
        OOO_RUNNER_PORT=8100
    .ENDIF
    .IF "$(OOO_RUNNER_HOST)" == ""
        OOO_RUNNER_HOST=localhost
    .ENDIF
    RUNNER_CONNECTION_STRING=socket,host=$(OOO_RUNNER_HOST),port=$(OOO_RUNNER_PORT)
.ENDIF

# classpath and argument list
RUNNER_CLASSPATH = -cp $(CLASSPATH)$(PATH_SEPERATOR)$(SOLARBINDIR)$/OOoRunner.jar$(PATH_SEPERATOR)$(CLASSPATH)$(PATH_SEPERATOR)$(SOLARBINDIR)$/ConnectivityTools.jar
RUNNER_ARGS = org.openoffice.Runner -TestBase java_complex -cs $(RUNNER_CONNECTION_STRING)

# --- Targets ------------------------------------------------------

.IF "$(depend)" == ""
ALL :   ALLTAR
    @echo -----------------------------------------------------
    @echo - do a 'dmake show_targets' to show available targets
    @echo -----------------------------------------------------
.ELSE
ALL: 	ALLDEP
.ENDIF

.INCLUDE :  target.mk

show_targets:
    +@java $(RUNNER_CLASSPATH) integration.forms.ShowTargets $(foreach,i,$(JAVAFILES) $(i:s/.\$///:s/.java//))

run:
    +$(COPY) integration$/forms$/*.props $(CLASSDIR)$/$(PACKAGE) && java $(RUNNER_CLASSPATH) $(RUNNER_ARGS) -sce forms_all.sce

run_%:
    +$(COPY) integration$/forms$/*.props $(CLASSDIR)$/$(PACKAGE) && java $(RUNNER_CLASSPATH) $(RUNNER_ARGS) -o integration.$(PRJNAME).$(@:s/run_//)

