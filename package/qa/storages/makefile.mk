#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.11 $
#
#   last change: $Author: obo $ $Date: 2006-10-13 11:46:54 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ = ..$/..
TARGET  = StorageUnitTest
PRJNAME = $(TARGET)
PACKAGE = complex$/storages

# --- Settings -----------------------------------------------------
.INCLUDE: settings.mk


#----- compile .java files -----------------------------------------

JARFILES        = ridl.jar unoil.jar jurt.jar juh.jar jut.jar java_uno.jar OOoRunner.jar

JAVAFILES       =\
                StorageUnitTest.java\
                StorageTest.java\
                TestHelper.java\
                BorderedStream.java\
                Test01.java\
                Test02.java\
                Test03.java\
                Test04.java\
                Test05.java\
                Test06.java\
                Test07.java\
                Test08.java\
                Test09.java\
                Test10.java\
                Test11.java\
                Test12.java\
                Test13.java\
                Test14.java\
                Test15.java\
                RegressionTest_114358.java\
                RegressionTest_i29169.java\
                RegressionTest_i30400.java\
                RegressionTest_i29321.java\
                RegressionTest_i30677.java\
                RegressionTest_i27773.java\
                RegressionTest_i46848.java\
                RegressionTest_i55821.java\
                RegressionTest_i35095.java\
                RegressionTest_i49755.java\
                RegressionTest_i59886.java\
                RegressionTest_i61909.java\
                RegressionTest_125919.java

JAVACLASSFILES	= $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

#----- make a jar from compiled files ------------------------------

MAXLINELENGTH = 100000

JARCLASSDIRS    = $(PACKAGE)
JARTARGET       = $(TARGET).jar
JARCOMPRESS 	= TRUE

# --- Parameters for the test --------------------------------------

# start an office if the parameter is set for the makefile
.IF "$(OFFICE)" == ""
CT_APPEXECCOMMAND =
.ELSE
CT_APPEXECCOMMAND = -AppExecutionCommand "$(OFFICE)$/soffice -accept=socket,host=localhost,port=8100;urp;"
.ENDIF

# test base is java complex
CT_TESTBASE = -TestBase java_complex

# test looks something like the.full.package.TestName
CT_TEST     = -o $(PACKAGE:s\$/\.\).$(JAVAFILES:b)

# start the runner application
CT_APP      = org.openoffice.Runner

# --- Targets ------------------------------------------------------

.INCLUDE: target.mk

RUN: run

run:
    +java -cp $(CLASSPATH) $(CT_APP) $(CT_TESTBASE) $(CT_APPEXECCOMMAND) $(CT_TEST)


