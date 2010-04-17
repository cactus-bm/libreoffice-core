#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2009 by Sun Microsystems, Inc.
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.	If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

$(eval $(call gb_Library_Library,xo))

$(call gb_Library_get_headers_target,xo) : $(call gb_Package_get_target,xo_inc)

$(eval $(call gb_Library_set_include,xo,\
    $$(SOLARINC) \
    -I. \
    -I$(WORKDIR)/inc/ \
    -I$(SRCDIR)/xmloff/inc \
    -I$(SRCDIR)/xmloff/source/inc \
    -I$(SRCDIR)/xmloff/inc/pch \
    -I$(OUTDIR)/inc/offuh \
    -I$(OUTDIR)/inc \
))

$(eval $(call gb_Library_set_defs,xo,\
    $$(DEFS) \
    -DXMLOFF_DLLIMPLEMENTATION \
))

$(eval $(call gb_Library_add_linked_libs,xo,\
    basegfx \
    comphelper \
    cppu \
    cppuhelper \
    i18nisolang1 \
    sal \
    stl \
    svl \
    tl \
    utl \
    vos3 \
))

$(eval $(call gb_Library_add_exception_objects,xo,\
    xmloff/source/draw/ximpshap \
    xmloff/source/draw/ximppage \
    xmloff/source/draw/ximpstyl \
    xmloff/source/draw/XMLShapeStyleContext \
    xmloff/source/draw/sdxmlimp \
    xmloff/source/draw/shapeimport \
    xmloff/source/draw/ximplink \
    xmloff/source/draw/propimp0 \
    xmloff/source/draw/ximpbody \
    xmloff/source/draw/eventimp \
    xmloff/source/draw/layerimp \
    xmloff/source/draw/shapeexport2 \
    xmloff/source/draw/animimp \
    xmloff/source/draw/ximp3dscene \
    xmloff/source/draw/ximpgrp \
    xmloff/source/draw/XMLImageMapExport \
    xmloff/source/draw/numithdl \
    xmloff/source/draw/XMLReplacementImageContext \
    xmloff/source/draw/ximpshow \
    xmloff/source/draw/descriptionimp \
    xmloff/source/draw/xexptran \
    xmloff/source/draw/animationexport \
    xmloff/source/draw/viewcontext \
    xmloff/source/draw/sdpropls \
    xmloff/source/draw/shapeexport3 \
    xmloff/source/draw/ximp3dobject \
    xmloff/source/draw/EnhancedCustomShapeToken \
    xmloff/source/draw/ximpcustomshape \
    xmloff/source/draw/shapeexport4 \
    xmloff/source/draw/animationimport \
    xmloff/source/draw/XMLGraphicsDefaultStyle \
    xmloff/source/draw/ximpnote \
    xmloff/source/draw/shapeexport \
    xmloff/source/draw/XMLImageMapContext \
    xmloff/source/draw/sdxmlexp \
    xmloff/source/draw/animexp \
    xmloff/source/draw/layerexp \
    xmloff/source/draw/XMLNumberStyles \
    xmloff/source/draw/XMLShapePropertySetContext \
    xmloff/source/chart/SchXMLImport \
    xmloff/source/chart/XMLErrorIndicatorPropertyHdl \
    xmloff/source/chart/PropertyMaps \
    xmloff/source/chart/ColorPropertySet \
    xmloff/source/chart/transporttypes \
    xmloff/source/chart/XMLSymbolImageContext \
    xmloff/source/chart/SchXMLTextListContext \
    xmloff/source/chart/SchXMLTools \
    xmloff/source/chart/XMLLabelSeparatorContext \
    xmloff/source/chart/XMLChartPropertyContext \
    xmloff/source/chart/SchXMLSeriesHelper \
    xmloff/source/chart/SchXMLSeries2Context \
    xmloff/source/chart/SchXMLPlotAreaContext \
    xmloff/source/chart/contexts \
    xmloff/source/chart/XMLErrorBarStylePropertyHdl \
    xmloff/source/chart/XMLAxisPositionPropertyHdl \
    xmloff/source/chart/SchXMLParagraphContext \
    xmloff/source/chart/XMLTextOrientationHdl \
    xmloff/source/chart/XMLSymbolTypePropertyHdl \
    xmloff/source/chart/SchXMLChartContext \
    xmloff/source/chart/XMLChartStyleContext \
    xmloff/source/chart/SchXMLAutoStylePoolP \
    xmloff/source/chart/SchXMLTableContext \
    xmloff/source/chart/SchXMLCalculationSettingsContext \
    xmloff/source/chart/SchXMLExport \
    xmloff/source/text/XMLChangedRegionImportContext \
    xmloff/source/text/XMLIndexUserSourceContext \
    xmloff/source/text/txtfldi \
    xmloff/source/text/XMLTextShapeImportHelper \
    xmloff/source/text/XMLIndexTOCStylesContext \
    xmloff/source/text/XMLIndexSimpleEntryContext \
    xmloff/source/text/txtftne \
    xmloff/source/text/XMLTextColumnsExport \
    xmloff/source/text/XMLChangeImportContext \
    xmloff/source/text/XMLSectionSourceImportContext \
    xmloff/source/text/XMLIndexAlphabeticalSourceContext \
    xmloff/source/text/txtparae \
    xmloff/source/text/XMLIndexTabStopEntryContext \
    xmloff/source/text/XMLTextMasterPageContext \
    xmloff/source/text/XMLTextHeaderFooterContext \
    xmloff/source/text/XMLIndexBibliographyEntryContext \
    xmloff/source/text/XMLIndexTOCContext \
    xmloff/source/text/XMLSectionImportContext \
    xmloff/source/text/XMLStringBufferImportContext \
    xmloff/source/text/XMLTextShapeStyleContext \
    xmloff/source/text/XMLSectionFootnoteConfigImport \
    xmloff/source/text/txtparai \
    xmloff/source/text/XMLIndexIllustrationSourceContext \
    xmloff/source/text/txtprmap \
    xmloff/source/text/XMLRedlineExport \
    xmloff/source/text/XMLTextMasterPageExport \
    xmloff/source/text/XMLIndexTableSourceContext \
    xmloff/source/text/XMLLineNumberingExport \
    xmloff/source/text/XMLSectionExport \
    xmloff/source/text/XMLTextMasterStylesContext \
    xmloff/source/text/XMLTextFrameContext \
    xmloff/source/text/XMLFootnoteImportContext \
    xmloff/source/text/txtvfldi \
    xmloff/source/text/txtdropi \
    xmloff/source/text/XMLTextListAutoStylePool \
    xmloff/source/text/XMLIndexTitleTemplateContext \
    xmloff/source/text/XMLChangeElementImportContext \
    xmloff/source/text/XMLIndexSourceBaseContext \
    xmloff/source/text/XMLTextNumRuleInfo \
    xmloff/source/text/XMLSectionFootnoteConfigExport \
    xmloff/source/text/XMLFootnoteConfigurationImportContext \
    xmloff/source/text/XMLIndexTOCSourceContext \
    xmloff/source/text/txtlists \
    xmloff/source/text/XMLLineNumberingSeparatorImportContext \
    xmloff/source/text/XMLLineNumberingImportContext \
    xmloff/source/text/XMLIndexMarkExport \
    xmloff/source/text/XMLSectionSourceDDEImportContext \
    xmloff/source/text/XMLFootnoteBodyImportContext \
    xmloff/source/text/XMLTextColumnsContext \
    xmloff/source/text/XMLAutoTextContainerEventImport \
    xmloff/source/text/XMLAutoMarkFileContext \
    xmloff/source/text/txtflde \
    xmloff/source/text/txtstyli \
    xmloff/source/text/txtsecte \
    xmloff/source/text/XMLPropertyBackpatcher \
    xmloff/source/text/XMLChangeInfoContext \
    xmloff/source/text/XMLCalculationSettingsContext \
    xmloff/source/text/XMLIndexBodyContext \
    xmloff/source/text/XMLTextFrameHyperlinkContext \
    xmloff/source/text/txtexppr \
    xmloff/source/text/XMLTextCharStyleNamesElementExport \
    xmloff/source/text/XMLAutoTextEventExport \
    xmloff/source/text/txtdrope \
    xmloff/source/text/XMLIndexSpanEntryContext \
    xmloff/source/text/XMLIndexChapterInfoEntryContext \
    xmloff/source/text/XMLTextListItemContext \
    xmloff/source/text/XMLIndexTemplateContext \
    xmloff/source/text/XMLAutoTextEventImport \
    xmloff/source/text/txtstyle \
    xmloff/source/text/XMLTrackedChangesImportContext \
    xmloff/source/text/XMLTextTableContext \
    xmloff/source/text/XMLIndexBibliographyConfigurationContext \
    xmloff/source/text/txtprhdl \
    xmloff/source/text/txtimppr \
    xmloff/source/text/XMLTextMarkImportContext \
    xmloff/source/text/XMLIndexBibliographySourceContext \
    xmloff/source/text/txtimp \
    xmloff/source/text/XMLIndexObjectSourceContext \
    xmloff/source/text/XMLTextPropertySetContext \
    xmloff/source/text/XMLTextListBlockContext \
    xmloff/source/script/XMLEventImportHelper \
    xmloff/source/script/xmlscripti \
    xmloff/source/script/XMLStarBasicExportHandler \
    xmloff/source/script/XMLScriptExportHandler \
    xmloff/source/script/XMLEventsImportContext \
    xmloff/source/script/xmlbasici \
    xmloff/source/script/XMLEventExport \
    xmloff/source/script/XMLScriptContextFactory \
    xmloff/source/script/XMLStarBasicContextFactory \
    xmloff/source/table/XMLTableImport \
    xmloff/source/table/XMLTableExport \
    xmloff/source/xforms/XFormsSubmissionContext \
    xmloff/source/xforms/SchemaRestrictionContext \
    xmloff/source/xforms/XFormsInstanceContext \
    xmloff/source/xforms/SchemaContext \
    xmloff/source/xforms/XFormsBindContext \
    xmloff/source/xforms/xformsexport \
    xmloff/source/xforms/xformsimport \
    xmloff/source/xforms/SchemaSimpleTypeContext \
    xmloff/source/xforms/TokenContext \
    xmloff/source/xforms/XFormsModelContext \
    xmloff/source/xforms/xformsapi \
    xmloff/source/forms/controlelement \
    xmloff/source/forms/formcellbinding \
    xmloff/source/forms/elementexport \
    xmloff/source/forms/eventimport \
    xmloff/source/forms/layerimport \
    xmloff/source/forms/elementimport \
    xmloff/source/forms/formattributes \
    xmloff/source/forms/formlayerimport \
    xmloff/source/forms/officeforms \
    xmloff/source/forms/layerexport \
    xmloff/source/forms/formlayerexport \
    xmloff/source/forms/eventexport \
    xmloff/source/forms/formenums \
    xmloff/source/forms/valueproperties \
    xmloff/source/forms/formstyles \
    xmloff/source/forms/controlpropertyhdl \
    xmloff/source/forms/propertyexport \
    xmloff/source/forms/gridcolumnproptranslator \
    xmloff/source/forms/propertyimport \
    xmloff/source/forms/controlpropertymap \
    xmloff/source/forms/formevents \
    xmloff/source/forms/strings \
    xmloff/source/forms/attriblistmerge \
    xmloff/source/forms/logging \
    xmloff/source/forms/formsimp \
    xmloff/source/style/opaquhdl \
    xmloff/source/style/impastp4 \
    xmloff/source/style/PageMasterImportContext \
    xmloff/source/style/DashStyle \
    xmloff/source/style/MultiPropertySetHelper \
    xmloff/source/style/PageMasterStyleMap \
    xmloff/source/style/WordWrapPropertyHdl \
    xmloff/source/style/XMLConstantsPropertyHandler \
    xmloff/source/style/xmltabi \
    xmloff/source/style/MarkerStyle \
    xmloff/source/style/XMLElementPropertyContext \
    xmloff/source/style/VisAreaExport \
    xmloff/source/style/undlihdl \
    xmloff/source/style/XMLIsPercentagePropertyHandler \
    xmloff/source/style/EnumPropertyHdl \
    xmloff/source/style/durationhdl \
    xmloff/source/style/XMLFontAutoStylePool \
    xmloff/source/style/shadwhdl \
    xmloff/source/style/xmlaustp \
    xmloff/source/style/numehelp \
    xmloff/source/style/adjushdl \
    xmloff/source/style/PageMasterExportPropMapper \
    xmloff/source/style/postuhdl \
    xmloff/source/style/TransGradientStyle \
    xmloff/source/style/XMLRectangleMembersHandler \
    xmloff/source/style/xmlbahdl \
    xmloff/source/style/xmlprcon \
    xmloff/source/style/ImageStyle \
    xmloff/source/style/xmlnumi \
    xmloff/source/style/kernihdl \
    xmloff/source/style/XMLBitmapLogicalSizePropertyHandler \
    xmloff/source/style/impastp1 \
    xmloff/source/style/xmlnumfi \
    xmloff/source/style/VisAreaContext \
    xmloff/source/style/bordrhdl \
    xmloff/source/style/prhdlfac \
    xmloff/source/style/xmlnumfe \
    xmloff/source/style/xmlimppr \
    xmloff/source/style/cdouthdl \
    xmloff/source/style/styleexp \
    xmloff/source/style/AttributeContainerHandler \
    xmloff/source/style/PageMasterPropHdl \
    xmloff/source/style/XMLBackgroundImageExport \
    xmloff/source/style/PageMasterPropMapper \
    xmloff/source/style/PagePropertySetContext \
    xmloff/source/style/XMLPercentOrMeasurePropertyHandler \
    xmloff/source/style/XMLBackgroundImageContext \
    xmloff/source/style/prstylei \
    xmloff/source/style/csmaphdl \
    xmloff/source/style/PageHeaderFooterContext \
    xmloff/source/style/XMLFontStylesContext \
    xmloff/source/style/PageMasterImportPropMapper \
    xmloff/source/style/lspachdl \
    xmloff/source/style/StyleMap \
    xmloff/source/style/XMLClipPropertyHandler \
    xmloff/source/style/XMLPageExport \
    xmloff/source/style/PageMasterPropHdlFactory \
    xmloff/source/style/xmlnume \
    xmloff/source/style/NamedBoolPropertyHdl \
    xmloff/source/style/xmlexppr \
    xmloff/source/style/xmltabe \
    xmloff/source/style/chrhghdl \
    xmloff/source/style/FillStyleContext \
    xmloff/source/style/breakhdl \
    xmloff/source/style/fonthdl \
    xmloff/source/style/escphdl \
    xmloff/source/style/xmlstyle \
    xmloff/source/style/HatchStyle \
    xmloff/source/style/chrlohdl \
    xmloff/source/style/xmlprhdl \
    xmloff/source/style/GradientStyle \
    xmloff/source/style/uniref \
    xmloff/source/style/impastp2 \
    xmloff/source/style/impastp3 \
    xmloff/source/style/XMLFootnoteSeparatorExport \
    xmloff/source/style/XMLBitmapRepeatOffsetPropertyHandler \
    xmloff/source/style/DrawAspectHdl \
    xmloff/source/style/tabsthdl \
    xmloff/source/style/xmlprmap \
    xmloff/source/style/XMLFillBitmapSizePropertyHandler \
    xmloff/source/style/XMLFootnoteSeparatorImport \
    xmloff/source/style/backhdl \
    xmloff/source/style/weighhdl \
    xmloff/source/style/SinglePropertySetInfoCache \
    xmloff/source/style/shdwdhdl \
    xmloff/source/core/i18nmap \
    xmloff/source/core/xmlimp \
    xmloff/source/core/unointerfacetouniqueidentifiermapper \
    xmloff/source/core/xmlexp \
    xmloff/source/core/XMLEmbeddedObjectExportFilter \
    xmloff/source/core/PropertySetMerger \
    xmloff/source/core/xmltoken \
    xmloff/source/core/xmlerror \
    xmloff/source/core/nmspmap \
    xmloff/source/core/xmlkywd \
    xmloff/source/core/xmluconv \
    xmloff/source/core/XMLBase64Export \
    xmloff/source/core/DomExport \
    xmloff/source/core/RDFaExportHelper \
    xmloff/source/core/xmltkmap \
    xmloff/source/core/DocumentSettingsContext \
    xmloff/source/core/xmlictxt \
    xmloff/source/core/XMLBase64ImportContext \
    xmloff/source/core/xmlehelp \
    xmloff/source/core/XMLEmbeddedObjectImportContext \
    xmloff/source/core/xmlcnitm \
    xmloff/source/core/ProgressBarHelper \
    xmloff/source/core/XMLBasicExportFilter \
    xmloff/source/core/facreg \
    xmloff/source/core/unoatrcn \
    xmloff/source/core/SettingsExportHelper \
    xmloff/source/core/DomBuilderContext \
    xmloff/source/core/attrlist \
    xmloff/source/core/RDFaImportHelper \
    xmloff/source/meta/xmlmetae \
    xmloff/source/meta/xmlversion \
    xmloff/source/meta/MetaImportComponent \
    xmloff/source/meta/xmlmetai \
    xmloff/source/meta/MetaExportComponent \
))

ifeq ($(OS),LINUX)
$(eval $(call gb_Library_add_linked_libs,xo,\
    icuuc \
    dl \
    m \
    pthread \
))
endif

ifeq ($(OS),WNT)
$(eval $(call gb_Library_add_linked_libs,xo,\
    kernel32 \
    msvcrt \
    oldnames \
    user32 \
    uwinapi \
))
endif
# vim: set noet ts=4 sw=4:
