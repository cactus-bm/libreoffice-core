#*************************************************************************
#*
#*    $Workfile:$
#*
#*    Creation date     KR 28.06.99
#*    last change       $Author: kz $ $Date: 2004-08-30 17:26:25 $
#*
#*    $Revision: 1.18 $
#*
#*    $Logfile:$
#*
#*    Copyright 2000 Sun Microsystems, Inc. All Rights Reserved.
#*
#*************************************************************************

PRJ		= ..$/..$/..$/..$/..
PRJNAME = xmlhelp
PACKAGE = com$/sun$/star$/help
TARGET  = com_sun_star_help

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

JARFILES 		= jaxp.jar parser.jar xt.jar unoil.jar ridl.jar jurt.jar jut.jar xmlsearch.jar db.jar
EXTRAJARFILES 	= 

CLASSGENDIR		= $(OUT)$/classgen
RDB		 		= $(SOLARBINDIR)$/types.rdb
JAVAFILES		= $(subst,$(CLASSDIR)$/$(PACKAGE)$/, $(subst,.class,.java $(JAVACLASSFILES))) 

UNIXTEXT= \
    $(MISC)$/helpserver.sh

# --- Files --------------------------------------------------------

JAVACLASSFILES = \
    $(CLASSDIR)$/$(PACKAGE)$/HelpCompiler.class			        \
    $(CLASSDIR)$/$(PACKAGE)$/HelpLinker.class  			        \
    $(CLASSDIR)$/$(PACKAGE)$/HelpContentIdentifier.class        \
    $(CLASSDIR)$/$(PACKAGE)$/HelpProvider.class                 \
    $(CLASSDIR)$/$(PACKAGE)$/HelpContent.class                  \
    $(CLASSDIR)$/$(PACKAGE)$/HelpOutputStream.class             \
    $(CLASSDIR)$/$(PACKAGE)$/HelpDatabases.class                \
    $(CLASSDIR)$/$(PACKAGE)$/HelpURLStreamHandlerFactory.class  \
    $(CLASSDIR)$/$(PACKAGE)$/HelpURLStreamHandler.class         \
    $(CLASSDIR)$/$(PACKAGE)$/FileURLStreamHandlerWithNotify.class  \
    $(CLASSDIR)$/$(PACKAGE)$/HelpURLStreamHandlerWithJars.class \
    $(CLASSDIR)$/$(PACKAGE)$/HelpURLConnection.class            \
    $(CLASSDIR)$/$(PACKAGE)$/HelpURLConnectionWithJars.class    \
    $(CLASSDIR)$/$(PACKAGE)$/HelpURLParameter.class             \
    $(CLASSDIR)$/$(PACKAGE)$/HelpResultSetFactory.class         \
    $(CLASSDIR)$/$(PACKAGE)$/HelpDynamicResultSet.class         \
    $(CLASSDIR)$/$(PACKAGE)$/HelpResultSetBase.class            \
    $(CLASSDIR)$/$(PACKAGE)$/HelpResultSet.class                \
    $(CLASSDIR)$/$(PACKAGE)$/HelpResultSetForRoot.class         \
    $(CLASSDIR)$/$(PACKAGE)$/HelpIndexer.class                  \
    $(CLASSDIR)$/$(PACKAGE)$/HelpKeyword.class                  \
    $(CLASSDIR)$/$(PACKAGE)$/HelpTransformer.class              \
    $(CLASSDIR)$/$(PACKAGE)$/HelpPackager.class                 \
    $(CLASSDIR)$/$(PACKAGE)$/CreateDb.class                     \
    $(CLASSDIR)$/$(PACKAGE)$/XSLData.class                      \
    $(CLASSDIR)$/$(PACKAGE)$/StringDbt.class 

JARCLASSDIRS	= com
JARTARGET		= $(PRJNAME).jar
JARCOMPRESS 	= TRUE
CUSTOMMANIFESTFILE = manifest


# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

