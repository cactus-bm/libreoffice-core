#************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: tg_config.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: ihi $ $Date: 2006-08-04 10:00:42 $
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

PACKAGEDIR*:=$(subst,.,$/ $(PACKAGE))

#
# --- XCS ---
#

.IF "$(XCSFILES)"!=""
# remove unnecessary info from the component schemas
XCS_TRIM=$(PROCESSOUT)$/registry$/schema$/$(PACKAGEDIR)$/{$(XCSFILES)}

# transform xcs format to properties format
# XCS_RESOURCES=$(foreach,i,$(XCSFILES) $(MISC)$/registry$/res$/{$(alllangiso)}$/$(PACKAGEDIR)$/$(i:b).properties)

ALLTAR: \
    $(XCS_TRIM) 
.ENDIF			# "$(XCSFILES)"!=""

.IF "$(XCS_TRIM)" != ""
$(XCS_TRIM) :   $(DTDDIR)$/registry$/component-schema.dtd \
                $(XSLDIR)$/schema_val.xsl \
                $(XSLDIR)$/sanity.xsl \
                $(XSLDIR)$/schema_trim.xsl 
.ENDIF

XML_APIS_JAR*=$(SOLARBINDIR)$/xml-apis.jar
XERCES_JAR*=$(SOLARBINDIR)$/xercesImpl.jar

$(PROCESSOUT)$/registry$/schema$/$(PACKAGEDIR)$/%.xcs : %.xcs
    @+echo -------------+ validating and stripping schema files
    -$(MKDIRHIER) $(@:d)
.IF "$(XSLTPROC)"=="NO_XSLTPROC"
.IF "$(NO_INSPECTION)"==""
    $(JAVAI) $(JAVACPS) $(XML_APIS_JAR)$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/cfgimport.jar -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl org.openoffice.configuration.Inspector $<
.ENDIF			# "$(NO_INSPECTION)"==""
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR) -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/schema_val.xsl $(@:d)$*.val componentName=$(PACKAGE).$*
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR) -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/sanity.xsl $(@:d)$*.san 
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR) -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/schema_trim.xsl $(@:d)$*.tmp
    $(JAVAI) $(JAVACPS) $(XML_APIS_JAR)$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/schema.jar -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl org.openoffice.helper.PrettyPrinter $(@:d)$*.tmp $@
.ELSE
# xsltproc already validates against the dtd.  For additional validation,
# org.openoffice.configuration.Inspector should be replaced and the
# replacement should be invoked here.
    $(XSLTPROC) -o $(@:d)$*.val \
                --stringparam componentName $(PACKAGE).$* \
                $(XSLDIR)$/schema_val.xsl $<
    $(XSLTPROC) -o $(@:d)$*.san \
                $(XSLDIR)$/sanity.xsl $<
    $(XSLTPROC) -o $(@:d)$*.tmp \
                $(XSLDIR)$/schema_trim.xsl $<
# xsltproc already seems to pretty-print the xml, so
# org.openoffice.helper.PrettyPrinter seems to be unnecessary.
    cp $(@:d)$*.tmp $@
.ENDIF
    +$(RM) $(@:d)$*.tmp > $(NULLDEV)
    +$(RM) $(@:d)$*.val > $(NULLDEV)
    +$(RM) $(@:d)$*.san > $(NULLDEV)
    
$(PROCESSOUT)$/merge$/$(PACKAGEDIR)$/%.xcs : %.xcs
# just a copy for now - insert "cfgex" commandline when required
    +$(MKDIRHIER) $(@:d)
    +$(COPY) $< $@

# Create properties files for localized <info> contents (Currently not built)
.IF "$(XCS_RESOURCES)" != ""
$(XCS_RESOURCES) :   $(XSLDIR)$/resource.xsl 
.ENDIF

$(PROCESSOUT)$/registry$/res$/{$(alllangiso)}$/$(PACKAGEDIR)$/%.properties :| $(PROCESSOUT)$/merge$/$(PACKAGEDIR)$/%.xcs
    @+echo -------------+ creating locale dependent resource bundles
    -$(MKDIRHIER) $(@:d)
.IF "$(XSLTPROC)"=="NO_XSLTPROC"
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR) -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/resource.xsl $@ locale={$(subst,$/$(PACKAGEDIR)$/$(@:f), $(subst,$(PROCESSOUT)$/registry$/res$/, $@))}
.ELSE
    $(XSLTPROC) -o $@ \
                --stringparam locale {$(subst,$/$(PACKAGEDIR)$/$(@:f), $(subst,$(PROCESSOUT)$/registry$/res$/, $@))} \
                $(XSLDIR)$/resource.xsl $<
.ENDIF

# 
# --- XCU ---
#

.IF "$(XCUFILES)"!=""
XCU_DEFAULT =$(PROCESSOUT)$/registry$/data$/$(PACKAGEDIR)$/{$(XCUFILES)}
XCU_MODULES =$(foreach,i,$(MODULEFILES) $(PROCESSOUT)$/registry$/spool$/$(PACKAGEDIR)$/$(i))
XCU_LANG    =$(foreach,i,$(LOCALIZEDFILES) $(PROCESSOUT)$/registry$/res$/{$(alllangiso)}$/$(PACKAGEDIR)$/$(i))
.ENDIF			# "$(XCUFILES)"!=""

.IF "$(XCU_DEFAULT)" != ""
$(XCU_DEFAULT) : $(DTDDIR)$/registry$/component-update.dtd \
                 $(XSLDIR)$/data_val.xsl \
                 $(XSLDIR)$/alllang.xsl 
.ENDIF

$(PROCESSOUT)$/registry$/data$/$(PACKAGEDIR)$/%.xcu : %.xcu
    @+echo -------------+ validating and creating a locale independent file
    -$(MKDIRHIER) $(@:d) 
.IF "$(XSLTPROC)"=="NO_XSLTPROC"
.IF "$(NO_INSPECTION)"==""
    $(JAVAI) $(JAVACPS) $(XML_APIS_JAR)$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/cfgimport.jar -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl org.openoffice.configuration.Inspector $<
.ENDIF			# "$(NO_INSPECTION)"==""
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/cfgimport.jar -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/data_val.xsl $(@:d)$*.val xcs=$(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$*.xcs schemaRoot=$(XCSROOT)$/registry$/schema
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/cfgimport.jar -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/alllang.xsl $(@:d)$*.tmp xcs=$(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$*.xcs schemaRoot=$(XCSROOT)$/registry$/schema
    $(JAVAI) $(JAVACPS) $(XML_APIS_JAR)$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/schema.jar -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl org.openoffice.helper.PrettyPrinter $(@:d)$*.tmp $@
.ELSE
# xsltproc already validates against the dtd.  For additional validation,
# org.openoffice.configuration.Inspector should be replaced and the
# replacement should be invoked here.
    $(XSLTPROC) -o $(@:d)$*.val \
                --stringparam xcs $(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$*.xcs \
                --stringparam schemaRoot $(XCSROOT)$/registry$/schema \
                $(XSLDIR)$/data_val.xsl $<
    $(XSLTPROC) -o $(@:d)$*.tmp \
                --stringparam xcs $(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$*.xcs \
                --stringparam schemaRoot $(XCSROOT)$/registry$/schema \
                $(XSLDIR)$/alllang.xsl $<
# xsltproc already seems to pretty-print the xml, so
# org.openoffice.helper.PrettyPrinter seems to be unnecessary.
    cp $(@:d)$*.tmp $@
.ENDIF
    +$(RM) $(@:d)$*.tmp > $(NULLDEV)
    +$(RM) $(@:d)$*.val > $(NULLDEV)

# --- localizations ---
.IF "$(XCU_LANG)"!=""
$(XCU_LANG) : localize.sdf
.ENDIF			# "$(XCU_LANG)"!=""

$(PROCESSOUT)$/merge$/$(PACKAGEDIR)$/%.xcu : %.xcu
    +$(MKDIRHIER) $(@:d)
    $(WRAPCMD) $(CFGEX) -p $(PRJNAME) -i $(@:f) -o $@ -m localize.sdf -l all

.IF "$(XCU_LANG)" != ""
$(XCU_LANG) : $(XSLDIR)$/alllang.xsl 
.ENDIF

$(PROCESSOUT)$/registry$/res$/{$(alllangiso)}$/$(PACKAGEDIR)$/%.xcu :| $(PROCESSOUT)$/merge$/$(PACKAGEDIR)$/%.xcu
    @+echo -------------+ creating locale dependent entries
    -$(MKDIRHIER) $(@:d)
.IF "$(XSLTPROC)"=="NO_XSLTPROC"
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/cfgimport.jar -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/alllang.xsl $(@:d)$*.tmp xcs=$(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$*.xcs schemaRoot=$(XCSROOT)$/registry$/schema locale={$(subst,$/$(PACKAGEDIR)$/$(@:f), $(subst,$(PROCESSOUT)$/registry$/res$/, $@))}	
    $(JAVAI) $(JAVACPS) $(XML_APIS_JAR)$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/schema.jar -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl org.openoffice.helper.PrettyPrinter $(@:d)$*.tmp $@
.ELSE
    $(XSLTPROC) -o $(@:d)$*.tmp \
                --stringparam xcs $(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$*.xcs \
                --stringparam schemaRoot $(XCSROOT)$/registry$/schema \
                --stringparam locale {$(subst,$/$(PACKAGEDIR)$/$(@:f), $(subst,$(PROCESSOUT)$/registry$/res$/, $@))} \
                $(XSLDIR)$/alllang.xsl $<
# xsltproc already seems to pretty-print the xml, so
# org.openoffice.helper.PrettyPrinter seems to be unnecessary.
    cp $(@:d)$*.tmp $@
.ENDIF
    +$(RM) $(@:d)$*.tmp > $(NULLDEV)

# --- languagepack tag modules ---
.IF "$(LANGUAGEPACKS)" != ""
$(LANGUAGEPACKS) : $(XSLDIR)$/delcomment.sed 
.ENDIF


$(PROCESSOUT)$/registry$/spool$/Langpack-%.xcu : Langpack.xcu.tmpl
    @+echo -------------+ creating a Langpack module for locale $*
    -$(MKDIRHIER) $(@:d) 
    $(SED) -e "s/__LANGUAGE__/$*/" -f $(XSLDIR)$/delcomment.sed $< > $@
    

# --- modules ---
.IF "$(XCU_MODULES)" != ""
$(XCU_MODULES) : $(XSLDIR)$/alllang.xsl 
.ENDIF


$(PROCESSOUT)$/registry$/spool$/$(PACKAGEDIR)$/%.xcu :| $$(@:b:s/-/./:b).xcu
    @+echo -------------+ creating a module file
    -$(MKDIRHIER) $(@:d) 
.IF "$(XSLTPROC)"=="NO_XSLTPROC"
    $(JAVAI) $(JAVACPS) $(SOLARBINDIR)$/xt.jar$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/cfgimport.jar -Dcom.jclark.xsl.sax.parser=org.apache.xerces.parsers.SAXParser com.jclark.xsl.sax.Driver $< $(XSLDIR)$/alllang.xsl $(@:d)$(@:f:s/.xcu/.tmp/) xcs=$(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$(<:b).xcs schemaRoot=$(XCSROOT)$/registry$/schema module={$(subst,$(<:b)-, $(*))}
    $(JAVAI) $(JAVACPS) $(XML_APIS_JAR)$(PATH_SEPERATOR)$(XERCES_JAR)$(PATH_SEPERATOR)$(PROCESSORDIR)$/schema.jar -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl org.openoffice.helper.PrettyPrinter $(@:d)$(@:f:s/.xcu/.tmp/) $@
.ELSE
    $(XSLTPROC) -o $(@:d)$(@:f:s/.xcu/.tmp/) \
                --stringparam xcs $(XCSROOT)$/registry$/schema$/$(PACKAGEDIR)$/$(<:b).xcs \
                --stringparam schemaRoot $(XCSROOT)$/registry$/schema \
                --stringparam module $(subst,$(<:b)-, $(*)) \
                $(XSLDIR)$/alllang.xsl $<
# xsltproc already seems to pretty-print the xml, so
# org.openoffice.helper.PrettyPrinter seems to be unnecessary.
    cp $(@:d)$(@:f:s/.xcu/.tmp/) $@
.ENDIF
    +$(RM) $(@:d)$(@:f:s/.xcu/.tmp/) > $(NULLDEV)

.IF "$(XCUFILES)"!=""
ALLTAR: \
    $(XCU_DEFAULT) \
    $(XCU_MODULES) \
    $(XCU_LANG)

$(XCU_DEFAULT) : $$(@:d:s!$(PROCESSOUT)$/registry$/data$/!$(XCSROOT)$/registry$/schema$/!)$$(@:f:s/.xcu/.xcs/)

.ENDIF			# "$(XCUFILES)"!=""

