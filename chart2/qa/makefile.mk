#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: vg $ $Date: 2007-05-22 17:13:45 $
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

PRJNAME	= chart2
PRJ		= ..
TARGET	= chart_qa

PACKAGE = qa

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

MAXLINELENGTH = 100000

# - rdb ----

RDB 		= $(SOLARBINDIR)$/types.rdb
JAVADIR 	= $(OUT)$/misc$/java
JARFILES	= ridl.jar

# --- Files --------------------------------------------------------

JAVACLASSFILES = \
    $(CLASSDIR)$/$(PACKAGE)$/TestCaseOldAPI.class

JARFILES  = ridl.jar jurt.jar unoil.jar juh.jar OOoRunner.jar
JAVAFILES = $(subst,$(CLASSDIR)$/$(PACKAGE)$/, $(subst,.class,.java $(JAVACLASSFILES)))

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

# --- Dependencies -------------------------------------------------

# Note: define dependencies after including target.mk (otherwise no targets are
# defined)

ALLTAR: runtest

# --- Rules --------------------------------------------------------

runtest: $(subst,.class,.run $(JAVACLASSFILES))

%.run: %.class
    java -classpath $(CLASSPATH) org.openoffice.Runner -TestBase java_complex -o $(subst,$(CLASSDIR)$/$(PACKAGE)$/,$(PACKAGE). $(subst,.class, $<))
