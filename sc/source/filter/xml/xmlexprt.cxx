/*************************************************************************
 *
 *  $RCSfile: xmlexprt.cxx,v $
 *
 *  $Revision: 1.88 $
 *
 *  last change: $Author: fs $ $Date: 2001-03-20 15:09:26 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifdef PCH
#include "filt_pch.hxx"
#endif

#pragma hdrstop

// INCLUDE ---------------------------------------------------------------
#ifndef SC_XMLEXPRT_HXX
#include "xmlexprt.hxx"
#endif

#ifndef SC_XMLMAPCH_HXX_
#include "xmlmapch.hxx"
#endif
#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif
#ifndef _SC_XMLSTYLE_HXX
#include "xmlstyle.hxx"
#endif
#ifndef SC_UNONAMES_HXX
#include "unonames.hxx"
#endif
#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_OUTLINETAB_HXX
#include "olinetab.hxx"
#endif
#ifndef SC_CELLSUNO_HXX
#include "cellsuno.hxx"
#endif
#ifndef SC_CELL_HXX
#include "cell.hxx"
#endif
#ifndef SC_RANGENAM_HXX
#include "rangenam.hxx"
#endif
#ifndef _SC_XMLTABLEMASTERPAGEEXPORT_HXX
#include "XMLTableMasterPageExport.hxx"
#endif
#ifndef SC_DRWLAYER_HXX
#include "drwlayer.hxx"
#endif
#ifndef _SC_XMLEXPORTDATAPILOT_HXX
#include "XMLExportDataPilot.hxx"
#endif
#ifndef SC_XMLEXPORTDATABASERANGES_HXX
#include "XMLExportDatabaseRanges.hxx"
#endif
#ifndef _SC_XMLEXPORTDDELINKS_HXX
#include "XMLExportDDELinks.hxx"
#endif
#ifndef _SC_XMLEXPORTITERATOR_HXX
#include "XMLExportIterator.hxx"
#endif
#ifndef _SC_XMLCOLUMNROWGROUPEXPORT_HXX
#include "XMLColumnRowGroupExport.hxx"
#endif
#ifndef _SC_XMLSTYLESEXPORTHELPER_HXX
#include "XMLStylesExportHelper.hxx"
#endif
#ifndef _SC_XMLCHANGETRACKINGEXPORTHELPER_HXX
#include "XMLChangeTrackingExportHelper.hxx"
#endif
#ifndef SC_DOCOPTIO_HXX
#include "docoptio.hxx"
#endif
#ifndef SC_XMLEXPORTSHAREDDATA_HXX
#include "XMLExportSharedData.hxx"
#endif

#ifndef _XMLOFF_XMLKYWD_HXX
#include <xmloff/xmlkywd.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include <xmloff/families.hxx>
#endif
#ifndef XMLOFF_NUMEHELP_HXX
#include <xmloff/numehelp.hxx>
#endif
#ifndef _XMLOFF_PROGRESSBARHELPER_HXX
#include <xmloff/ProgressBarHelper.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif
#ifndef _XMLOFF_TXTPARAE_HXX
#include <xmloff/txtparae.hxx>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif
#ifndef _SVX_UNOSHAPE_HXX
#include <svx/unoshape.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _SCH_MEMCHRT_HXX
#include <sch/memchrt.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_SHEET_XUSEDAREACURSOR_HPP_
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XCELLRANGEADDRESSABLE_HPP_
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XAREALINKS_HPP_
#include <com/sun/star/sheet/XAreaLinks.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XAREALINK_HPP_
#include <com/sun/star/sheet/XAreaLink.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_XCOLUMNROWRANGE_HPP_
#include <com/sun/star/table/XColumnRowRange.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XPRINTAREAS_HPP_
#include <com/sun/star/sheet/XPrintAreas.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XPROTECTABLE_HPP_
#include <com/sun/star/util/XProtectable.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_XSTYLEFAMILIESSUPPLIER_HPP_
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XCELLFORMATRANGESSUPPLIER_HPP_
#include <com/sun/star/sheet/XCellFormatRangesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XCELLRANGESQUERY_HPP_
#include <com/sun/star/sheet/XCellRangesQuery.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_CELLFLAGS_HPP_
#include <com/sun/star/sheet/CellFlags.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMERGEABLE_HPP_
#include <com/sun/star/util/XMergeable.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XARRAYFORMULARANGE_HPP_
#include <com/sun/star/sheet/XArrayFormulaRange.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_
#include <com/sun/star/text/XText.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XLABELRANGES_HPP_
#include <com/sun/star/sheet/XLabelRanges.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XLABELRANGE_HPP_
#include <com/sun/star/sheet/XLabelRange.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XNAMEDRANGES_HPP_
#include <com/sun/star/sheet/XNamedRanges.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XNAMEDRANGE_HPP_
#include <com/sun/star/sheet/XNamedRange.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XCELLRANGEREFERRER_HPP_
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_NAMEDRANGEFLAG_HPP_
#include <com/sun/star/sheet/NamedRangeFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XSHEETLINKABLE_HPP_
#include <com/sun/star/sheet/XSheetLinkable.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMSUPPLIER_HPP_
#include <com/sun/star/form/XFormsSupplier.hpp>
#endif

//! not found in unonames.hxx
#define SC_STANDARDFORMAT "StandardFormat"
#define SC_LAYERID "LayerID"

#define SC_DEFAULT_TABLE_COUNT 3

using namespace rtl;
using namespace com::sun::star;

//----------------------------------------------------------------------------

uno::Sequence< rtl::OUString > SAL_CALL ScXMLExport_getSupportedServiceNames() throw()
{
    const rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Calc.XMLExporter" ) );
    const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL ScXMLExport_getImplementationName() throw()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScXMLExport" ) );
}

uno::Reference< uno::XInterface > SAL_CALL ScXMLExport_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    return (cppu::OWeakObject*)new ScXMLExport(EXPORT_ALL);
}

uno::Sequence< rtl::OUString > SAL_CALL ScXMLExport_Meta_getSupportedServiceNames() throw()
{
    const rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Calc.XMLMetaExporter" ) );
    const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL ScXMLExport_Meta_getImplementationName() throw()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScXMLExport_Meta" ) );
}

uno::Reference< uno::XInterface > SAL_CALL ScXMLExport_Meta_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    return (cppu::OWeakObject*)new ScXMLExport(EXPORT_META);
}

uno::Sequence< rtl::OUString > SAL_CALL ScXMLExport_Styles_getSupportedServiceNames() throw()
{
    const rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Calc.XMLStylesExporter" ) );
    const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL ScXMLExport_Styles_getImplementationName() throw()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScXMLExport_Styles" ) );
}

uno::Reference< uno::XInterface > SAL_CALL ScXMLExport_Styles_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    return (cppu::OWeakObject*)new ScXMLExport(EXPORT_STYLES|EXPORT_MASTERSTYLES|EXPORT_AUTOSTYLES|EXPORT_FONTDECLS);
}

uno::Sequence< rtl::OUString > SAL_CALL ScXMLExport_Content_getSupportedServiceNames() throw()
{
    const rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Calc.XMLContentExporter" ) );
    const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL ScXMLExport_Content_getImplementationName() throw()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ScXMLExport_Content" ) );
}

uno::Reference< uno::XInterface > SAL_CALL ScXMLExport_Content_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    return (cppu::OWeakObject*)new ScXMLExport(EXPORT_SETTINGS|EXPORT_AUTOSTYLES|EXPORT_CONTENT|EXPORT_SCRIPTS|EXPORT_FONTDECLS);
}

//----------------------------------------------------------------------------

sal_Int16 ScXMLExport::GetFieldUnit()
{
    com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet> xProperties(
                comphelper::getProcessServiceFactory()->createInstance(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sheet.GlobalSheetSettings" )) ),
                com::sun::star::uno::UNO_QUERY);
    if (xProperties.is())
    {
        com::sun::star::uno::Any aAny = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Metric")));
        sal_Int16 nFieldUnit;
        if (aAny >>= nFieldUnit)
            return nFieldUnit;
    }
    return 0;
}


ScXMLExport::ScXMLExport(const sal_uInt16 nExportFlag) :
    SvXMLExport( SvXMLUnitConverter::GetMapUnit(GetFieldUnit()), sXML_spreadsheet, nExportFlag ),
    pDoc(NULL),
    mbShowProgress( sal_False ),
    pSharedData(NULL),
    pColumnStyles(NULL),
    pRowStyles(NULL),
    pCellStyles(NULL),
    pMergedRangesContainer(NULL),
    pValidationsContainer(NULL),
    xChartExportMapper(new ScExportMapper()),
    nOpenRow(-1),
    pRowFormatRanges(NULL),
    nCurrentTable(0),
    aTableStyles(),
    pCellsItr(NULL),
    bHasRowHeader(sal_False),
    bRowHeaderOpen(sal_False),
    pDetectiveObjContainer(NULL),
    pChangeTrackingExportHelper(NULL),
    aXShapesVec()
{
    pGroupColumns = new ScMyOpenCloseColumnRowGroup(*this, sXML_table_column_group);
    pGroupRows = new ScMyOpenCloseColumnRowGroup(*this, sXML_table_row_group);
    pColumnStyles = new ScColumnRowStyles();
    pRowStyles = new ScColumnRowStyles();
    pCellStyles = new ScFormatRangeStyles();
    pRowFormatRanges = new ScRowFormatRanges();
    pMergedRangesContainer = new ScMyMergedRangesContainer();
    pValidationsContainer = new ScMyValidationsContainer();
    pDetectiveObjContainer = new ScMyDetectiveObjContainer();
    pCellsItr = new ScMyNotEmptyCellsIterator(*this);

    // document is not set here - create ScChangeTrackingExportHelper later

    xScPropHdlFactory = new XMLScPropHdlFactory;
    xCellStylesPropertySetMapper = new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLScCellStylesProperties, xScPropHdlFactory);
    xColumnStylesPropertySetMapper = new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLScColumnStylesProperties, xScPropHdlFactory);
    xRowStylesPropertySetMapper = new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLScRowStylesProperties, xScPropHdlFactory);
    xTableStylesPropertySetMapper = new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLScTableStylesProperties, xScPropHdlFactory);
    xCellStylesExportPropertySetMapper = new ScXMLCellExportPropertyMapper(xCellStylesPropertySetMapper);
    xCellStylesExportPropertySetMapper->ChainExportMapper(XMLTextParagraphExport::CreateCharExtPropMapper(*this));
    xColumnStylesExportPropertySetMapper = new SvXMLExportPropertyMapper(xColumnStylesPropertySetMapper);
    xRowStylesExportPropertySetMapper = new ScXMLRowExportPropertyMapper(xRowStylesPropertySetMapper);
    xTableStylesExportPropertySetMapper = new SvXMLExportPropertyMapper(xTableStylesPropertySetMapper);

    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_CELL, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_CELL_STYLES_NAME)),
        xCellStylesExportPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_CELL_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_COLUMN, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_COLUMN_STYLES_NAME)),
        xColumnStylesExportPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_COLUMN_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_ROW, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_ROW_STYLES_NAME)),
        xRowStylesExportPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_ROW_STYLES_PREFIX)));
    GetAutoStylePool()->AddFamily(XML_STYLE_FAMILY_TABLE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_TABLE_STYLES_NAME)),
        xTableStylesExportPropertySetMapper, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_TABLE_STYLES_PREFIX)));
}


ScXMLExport::~ScXMLExport()
{
    if (pGroupColumns)
        delete pGroupColumns;
    if (pGroupRows)
        delete pGroupRows;
    if (pColumnStyles)
        delete pColumnStyles;
    if (pRowStyles)
        delete pRowStyles;
    if (pCellStyles)
        delete pCellStyles;
    if (pRowFormatRanges)
        delete pRowFormatRanges;
    if (pMergedRangesContainer)
        delete pMergedRangesContainer;
    if (pValidationsContainer)
        delete pValidationsContainer;
    if (pDetectiveObjContainer)
        delete pDetectiveObjContainer;
    if (pChangeTrackingExportHelper)
        delete pChangeTrackingExportHelper;
}

void SAL_CALL ScXMLExport::setSourceDocument( const uno::Reference<lang::XComponent>& xComponent )
                            throw(lang::IllegalArgumentException, uno::RuntimeException)
{
    SvXMLExport::setSourceDocument( xComponent );

    xModel = uno::Reference<frame::XModel>(xComponent, uno::UNO_QUERY);
    pDoc = ScXMLConverter::GetScDocument( xModel );
    DBG_ASSERT( pDoc, "ScXMLExport::setSourceDocument - no ScDocument!" );
    if (!pDoc)
        throw lang::IllegalArgumentException();

    // create ScChangeTrackingExportHelper after document is known
    pChangeTrackingExportHelper = new ScChangeTrackingExportHelper(*this);
}

void ScXMLExport::CollectSharedData(sal_Int32& nTableCount, sal_Int32& nShapesCount, const sal_Int32 nCellCount)
{
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
            nTableCount = xIndex->getCount();
            pCellStyles->AddNewTable(nTableCount - 1);
            for (sal_Int32 nTable = 0; nTable < nTableCount; nTable++)
            {
                uno::Any aTable = xIndex->getByIndex(nTable);
                uno::Reference<sheet::XSpreadsheet> xTable;
                if (aTable>>=xTable)
                {
                    uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xTable, uno::UNO_QUERY);
                    if (xDrawPageSupplier.is())
                    {
                        uno::Reference<drawing::XDrawPage> xDrawPage = xDrawPageSupplier->getDrawPage();
                        uno::Reference<container::XIndexAccess> xShapesIndex (xDrawPage, uno::UNO_QUERY);
                        if (xShapesIndex.is())
                        {
                            sal_Int32 nShapes = xShapesIndex->getCount();
                            for (sal_Int32 nShape = 0; nShape < nShapes; nShape++)
                            {
                                uno::Any aShape = xShapesIndex->getByIndex(nShape);
                                uno::Reference<drawing::XShape> xShape;
                                if (aShape >>= xShape)
                                {
                                    uno::Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
                                    if( xShapeProp.is() )
                                    {
                                        uno::Any aPropAny = xShapeProp->getPropertyValue(
                                            OUString( RTL_CONSTASCII_USTRINGPARAM( SC_LAYERID ) ) );
                                        sal_Int16 nLayerID;
                                        if( aPropAny >>= nLayerID )
                                        {
                                            if( nLayerID != SC_LAYER_INTERN )
                                                nShapesCount++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (!pSharedData)
        CreateSharedData(nTableCount);
    pSharedData->nProgressObjects = nCellCount + nShapesCount + nTableCount;
    pSharedData->nProgressReference = 2 * pSharedData->nProgressObjects;
    pSharedData->nProgressValue = pSharedData->nProgressReference / 10;
    pSharedData->nProgressReference += pSharedData->nProgressValue;
}

void ScXMLExport::CollectShapesAutoStyles(uno::Reference<sheet::XSpreadsheet>& xTable, const sal_Int32 nTable)
{
    uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xTable, uno::UNO_QUERY);
    if (xDrawPageSupplier.is())
    {
        uno::Reference<drawing::XDrawPage> xDrawPage = xDrawPageSupplier->getDrawPage();
        uno::Reference<container::XIndexAccess> xShapesIndex (xDrawPage, uno::UNO_QUERY);
        uno::Reference<drawing::XShapes> xShapes (xDrawPage, uno::UNO_QUERY);
        if (xShapesIndex.is() && xShapes.is())
        {
            GetShapeExport()->seekShapes(xShapes);
            aXShapesVec[nTable] = xShapes;
            uno::Reference< form::XFormsSupplier > xFormsSupplier( xDrawPage, uno::UNO_QUERY );
            if( xFormsSupplier.is() )
            {
                uno::Reference< container::XNameContainer > xForms( xFormsSupplier->getForms() );
                if( xForms.is() && xForms->hasElements() )
                {
                    GetFormExport()->examineForms(xDrawPage);
                    ScMyDrawPage aDrawPage;
                    aDrawPage.bHasForms = sal_True;
                    aDrawPage.xDrawPage = xDrawPage;
                    pSharedData->AddDrawPage(aDrawPage, nTable);
                }
            }
            sal_Int32 nShapes = xShapesIndex->getCount();
            for (sal_Int32 nShape = 0; nShape < nShapes; nShape++)
            {
                uno::Any aShape = xShapesIndex->getByIndex(nShape);
                uno::Reference<drawing::XShape> xShape;
                if (aShape >>= xShape)
                {
                    uno::Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
                    if( xShapeProp.is() )
                    {
                        uno::Any aPropAny = xShapeProp->getPropertyValue(
                            OUString( RTL_CONSTASCII_USTRINGPARAM( SC_LAYERID ) ) );
                        sal_Int16 nLayerID;
                        if( aPropAny >>= nLayerID )
                        {
                            if( nLayerID == SC_LAYER_INTERN )
                                CollectInternalShape( xShape );
                            else
                            {
                                SvxShape* pShapeImp = SvxShape::getImplementation(xShape);
                                if (pShapeImp)
                                {
                                    SdrObject *pSdrObj = pShapeImp->GetSdrObject();
                                    if (pSdrObj)
                                    {
                                        GetShapeExport()->collectShapeAutoStyles(xShape);
                                        if (ScDrawLayer::GetAnchor(pSdrObj) == SCA_CELL)
                                        {
                                            if (pDoc)
                                            {
                                                awt::Point aPoint = xShape->getPosition();
                                                awt::Size aSize = xShape->getSize();
                                                Rectangle aRectangle(aPoint.X, aPoint.Y, aPoint.X + aSize.Width, aPoint.Y + aSize.Height);
                                                ScRange aRange = pDoc->GetRange(static_cast<USHORT>(nTable), aRectangle);
                                                ScMyShape aMyShape;
                                                aMyShape.aAddress = aRange.aStart;
                                                aMyShape.aEndAddress = aRange.aEnd;
                                                aMyShape.nIndex = nShape;
                                                pSharedData->AddNewShape(aMyShape);
                                                pSharedData->SetLastColumn(nTable, aRange.aStart.Col());
                                                pSharedData->SetLastRow(nTable, aRange.aStart.Row());
                                            }
                                        }
                                        else
                                            pSharedData->AddTableShape(nTable, nShape);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ScXMLExport::_ExportMeta()
{
    SvXMLExport::_ExportMeta();
    sal_Int32 nCellCount(pDoc->GetCellCount());
    sal_Int32 nTableCount(0);
    sal_Int32 nShapesCount(0);
    GetAutoStylePool()->ClearEntries();
    CollectSharedData(nTableCount, nShapesCount, nCellCount);
    GetProgressBarHelper()->SetReference(pSharedData->nProgressReference);
    GetProgressBarHelper()->SetValue(pSharedData->nProgressValue);
    rtl::OUStringBuffer sBuffer;
    if (nTableCount)
    {
        GetMM100UnitConverter().convertNumber(sBuffer, nTableCount);
        AddAttribute(XML_NAMESPACE_META, sXML_table_count, sBuffer.makeStringAndClear());
    }
    if (nCellCount)
    {
        GetMM100UnitConverter().convertNumber(sBuffer, nCellCount);
        AddAttribute(XML_NAMESPACE_META, sXML_cell_count, sBuffer.makeStringAndClear());
    }
    if (nShapesCount)
    {
        GetMM100UnitConverter().convertNumber(sBuffer, nShapesCount);
        AddAttribute(XML_NAMESPACE_META, sXML_object_count, sBuffer.makeStringAndClear());
    }
    SvXMLElementExport aElemStat(*this, XML_NAMESPACE_META, sXML_document_statistic, sal_True, sal_True);
}

void ScXMLExport::_ExportFontDecls()
{
    GetFontAutoStylePool(); // make sure the pool is created
    SvXMLExport::_ExportFontDecls();
}

table::CellRangeAddress ScXMLExport::GetEndAddress(uno::Reference<sheet::XSpreadsheet>& xTable,const sal_Int16 nTable)
{
    table::CellRangeAddress aCellAddress;
    uno::Reference<sheet::XSheetCellCursor> xCursor = xTable->createCursor();
    uno::Reference<sheet::XUsedAreaCursor> xUsedArea (xCursor, uno::UNO_QUERY);
    uno::Reference<sheet::XCellRangeAddressable> xCellAddress (xCursor, uno::UNO_QUERY);
    if (xUsedArea.is() && xCellAddress.is())
    {
        xUsedArea->gotoEndOfUsedArea(sal_True);
        aCellAddress = xCellAddress->getRangeAddress();
    }
    return aCellAddress;
}

void ScXMLExport::GetAreaLinks( uno::Reference< sheet::XSpreadsheetDocument>& xSpreadDoc,
                                ScMyAreaLinksContainer& rAreaLinks )
{
    uno::Reference< beans::XPropertySet > xPropSet( xSpreadDoc, uno::UNO_QUERY );
    if( !xPropSet.is() ) return;

    uno::Reference< sheet::XAreaLinks > xAreaLinks;
    uno::Any aAny( xPropSet->getPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM( SC_UNO_AREALINKS ) ) ) );
    if( aAny >>= xAreaLinks )
    {
        uno::Reference< container::XIndexAccess > xLinksIAccess( xAreaLinks, uno::UNO_QUERY );
        if( xLinksIAccess.is() )
        {
            const OUString sFilter( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_FILTER ) );
            const OUString sFilterOpt( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_FILTOPT ) );
            const OUString sURL( RTL_CONSTASCII_USTRINGPARAM( SC_UNONAME_LINKURL ) );

            sal_Int32 nCount = xLinksIAccess->getCount();
            for( sal_Int32 nIndex = 0; nIndex < nCount; nIndex++ )
            {
                uno::Reference< sheet::XAreaLink > xAreaLink;
                uno::Any aLinkAny( xLinksIAccess->getByIndex( nIndex ) );
                if( aLinkAny >>= xAreaLink )
                {
                    ScMyAreaLink aAreaLink;
                    aAreaLink.aDestRange = xAreaLink->getDestArea();
                    aAreaLink.sSourceStr = xAreaLink->getSourceArea();
                    uno::Reference< beans::XPropertySet > xLinkProp( xAreaLink, uno::UNO_QUERY );
                    if( xLinkProp.is() )
                    {
                        aLinkAny = xLinkProp->getPropertyValue( sFilter );
                        aLinkAny >>= aAreaLink.sFilter;
                        aLinkAny = xLinkProp->getPropertyValue( sFilterOpt );
                        aLinkAny >>= aAreaLink.sFilterOptions;
                        aLinkAny = xLinkProp->getPropertyValue( sURL );
                        aLinkAny >>= aAreaLink.sURL;
                    }
                    rAreaLinks.AddNewAreaLink( aAreaLink );
                }
            }
        }
    }
    rAreaLinks.Sort();
}

// core implementation
void ScXMLExport::GetDetectiveOpList( ScMyDetectiveOpContainer& rDetOp )
{
    ScDetOpList* pOpList = pDoc->GetDetOpList();
    if( pOpList )
    {
        sal_uInt32 nCount = pOpList->Count();
        for( sal_uInt32 nIndex = 0; nIndex < nCount; nIndex++ )
        {
            ScDetOpData* pDetData = pOpList->GetObject( static_cast<USHORT>(nIndex) );
            if( pDetData )
                rDetOp.AddOperation( pDetData->GetOperation(), pDetData->GetPos(), nIndex );
        }
        rDetOp.Sort();
    }
}

sal_Bool ScXMLExport::GetxCurrentShapes(uno::Reference<container::XIndexAccess>& xShapes)
{
    uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xCurrentTable, uno::UNO_QUERY);
    if (xDrawPageSupplier.is())
    {
        uno::Reference<drawing::XDrawPage> xDrawPage = xDrawPageSupplier->getDrawPage();
        if (xDrawPage.is())
        {
            uno::Reference<container::XIndexAccess> xShapesIndex (xDrawPage, uno::UNO_QUERY);
            if (xShapesIndex.is())
            {
                xShapes = xShapesIndex;
                return sal_True;
            }
        }
    }
    return sal_False;
}

void ScXMLExport::WriteColumn(const sal_Int32 nRepeatColumns, const sal_Int32 nStyleIndex, const sal_Bool bIsVisible)
{
    CheckAttrList();
    AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *pColumnStyles->GetStyleNameByIndex(nStyleIndex));
    if (!bIsVisible)
        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_collapse);
    if (nRepeatColumns > 1)
    {
        OUString sOUEndCol = OUString::valueOf(static_cast <sal_Int32> (nRepeatColumns));
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, sOUEndCol);
    }
    SvXMLElementExport aElemR(*this, XML_NAMESPACE_TABLE, sXML_table_column, sal_True, sal_True);
}

void ScXMLExport::OpenHeaderColumn()
{
    rtl::OUString sName (GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_header_columns))));
    GetDocHandler()->ignorableWhitespace(sWS);
    GetDocHandler()->startElement( sName, GetXAttrList());
    ClearAttrList();
}

void ScXMLExport::CloseHeaderColumn()
{
    rtl::OUString sName (GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_header_columns))));
    GetDocHandler()->ignorableWhitespace(sWS);
    GetDocHandler()->endElement(sName);
}

void ScXMLExport::ExportColumns(const sal_Int16 nTable, const table::CellRangeAddress& aColumnHeaderRange, const sal_Bool bHasColumnHeader)
{
    sal_Int32 nColsRepeated (1);
    rtl::OUString sParent;
    sal_Int32 nIndex;
    sal_Bool bPrevIsVisible (sal_True);
    sal_Bool bWasHeader (sal_False);
    sal_Bool bIsHeader (sal_False);
    sal_Bool bIsClosed (sal_True);
    sal_Bool bIsFirst (sal_False);
    sal_Int32 nPrevIndex (-1);
    uno::Reference<table::XColumnRowRange> xColumnRowRange (xCurrentTable, uno::UNO_QUERY);
    if (xColumnRowRange.is())
    {
        uno::Reference<table::XTableColumns> xTableColumns = xColumnRowRange->getColumns();
        if (xTableColumns.is())
        {
            for (sal_Int32 nColumn = 0; nColumn <= pSharedData->GetLastColumn(nTable); nColumn++)
            {
                CheckAttrList();
                uno::Any aColumn = xTableColumns->getByIndex(nColumn);
                uno::Reference<table::XCellRange> xTableColumn;
                if (aColumn >>= xTableColumn)
                {
                    uno::Reference <beans::XPropertySet> xColumnProperties(xTableColumn, uno::UNO_QUERY);
                    if (xColumnProperties.is())
                    {
                        nIndex = pColumnStyles->GetStyleNameIndex(nTable, nColumn);

                        uno::Any aAny = xColumnProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_CELLVIS)));
                        sal_Bool bIsVisible(sal_True);
                        aAny >>= bIsVisible;
                        bIsHeader = bHasColumnHeader && (aColumnHeaderRange.StartColumn <= nColumn) && (nColumn <= aColumnHeaderRange.EndColumn);
                        if (bIsHeader != bWasHeader)
                        {
                            if (bIsHeader)
                            {
                                bIsFirst = sal_False;
                                if (nColumn > 0)
                                {
                                    WriteColumn(nColsRepeated, nPrevIndex, bPrevIsVisible);
                                    if (pGroupColumns->IsGroupEnd(nColumn - 1))
                                        pGroupColumns->CloseGroups(nColumn - 1);
                                }
                                bPrevIsVisible = bIsVisible;
                                nPrevIndex = nIndex;
                                nColsRepeated = 1;
                                bIsFirst = sal_True;
                                if(pGroupColumns->IsGroupStart(nColumn))
                                    pGroupColumns->OpenGroups(nColumn);
                                OpenHeaderColumn();
                                bWasHeader = sal_True;
                                bIsClosed = sal_False;
                            }
                            else
                            {
                                WriteColumn(nColsRepeated, nPrevIndex, bPrevIsVisible);
                                CloseHeaderColumn();
                                if (pGroupColumns->IsGroupEnd(nColumn - 1))
                                    pGroupColumns->CloseGroups(nColumn - 1);
                                bPrevIsVisible = bIsVisible;
                                nPrevIndex = nIndex;
                                nColsRepeated = 1;
                                bWasHeader = sal_False;
                                bIsClosed = sal_True;
                            }
                        }
                        else if (nColumn == 0)
                        {
                            if (pGroupColumns->IsGroupStart(nColumn))
                                pGroupColumns->OpenGroups(nColumn);
                            bPrevIsVisible = bIsVisible;
                            nPrevIndex = nIndex;
                            bIsFirst = sal_True;
                        }
                        else if ((bIsVisible == bPrevIsVisible) && (nIndex == nPrevIndex) &&
                            !pGroupColumns->IsGroupStart(nColumn) && !pGroupColumns->IsGroupEnd(nColumn - 1))
                            nColsRepeated++;
                        else
                        {
                            bIsFirst = sal_False;
                            WriteColumn(nColsRepeated, nPrevIndex, bPrevIsVisible);
                            if (pGroupColumns->IsGroupEnd(nColumn - 1))
                                pGroupColumns->CloseGroups(nColumn - 1);
                            if (pGroupColumns->IsGroupStart(nColumn))
                            {
                                if (bIsHeader)
                                    CloseHeaderColumn();
                                pGroupColumns->OpenGroups(nColumn);
                                if (bIsHeader)
                                    OpenHeaderColumn();
                            }
                            bPrevIsVisible = bIsVisible;
                            nPrevIndex = nIndex;
                            nColsRepeated = 1;
                        }
                    }
                }
            }
            //if (nColsRepeated > 1 || bIsFirst)
                WriteColumn(nColsRepeated, nPrevIndex, bPrevIsVisible);
            if (!bIsClosed)
                CloseHeaderColumn();
            if (pGroupColumns->IsGroupEnd(nColumn - 1))
                pGroupColumns->CloseGroups(nColumn - 1);
        }
    }
}

void ScXMLExport::WriteRowContent()
{
    ScMyRowFormatRange aRange;
    sal_Int32 nIndex = -1;
    sal_Int32 nCols = 0;
    sal_Int32 nPrevValidationIndex = -1;
    sal_Bool bIsAutoStyle(sal_True);
    sal_Bool bIsFirst(sal_True);
    while (pRowFormatRanges->GetNext(aRange))
    {
        if (bIsFirst)
        {
            nIndex = aRange.nIndex;
            nPrevValidationIndex = aRange.nValidationIndex;
            bIsAutoStyle = aRange.bIsAutoStyle;
            nCols = aRange.nRepeatColumns;
            bIsFirst = sal_False;
        }
        else
        {
            if (aRange.nIndex == nIndex && aRange.bIsAutoStyle == bIsAutoStyle &&
                nPrevValidationIndex == aRange.nValidationIndex)
                nCols += aRange.nRepeatColumns;
            else
            {
                AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *pCellStyles->GetStyleNameByIndex(nIndex, bIsAutoStyle));
                if (nPrevValidationIndex > -1)
                    AddAttribute(XML_NAMESPACE_TABLE, sXML_content_validation_name, pValidationsContainer->GetValidationName(nPrevValidationIndex));
                if (nCols > 1)
                {
                    rtl::OUStringBuffer aBuf;
                    GetMM100UnitConverter().convertNumber(aBuf, nCols);
                    AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, aBuf.makeStringAndClear());
                }
                SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_table_cell, sal_True, sal_True);
                nIndex = aRange.nIndex;
                bIsAutoStyle = aRange.bIsAutoStyle;
                nCols = aRange.nRepeatColumns;
                nPrevValidationIndex = aRange.nValidationIndex;
            }
        }
    }
    if (!bIsFirst)
    {
        table::CellAddress aCellAddress;
        AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *pCellStyles->GetStyleNameByIndex(nIndex, bIsAutoStyle));
        if (nPrevValidationIndex > -1)
            AddAttribute(XML_NAMESPACE_TABLE, sXML_content_validation_name, pValidationsContainer->GetValidationName(nPrevValidationIndex));
        if (nCols > 1)
        {
            rtl::OUStringBuffer aBuf;
            GetMM100UnitConverter().convertNumber(aBuf, nCols);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, aBuf.makeStringAndClear());
        }
        SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sXML_table_cell, sal_True, sal_True);
    }
}

void ScXMLExport::WriteRowStartTag(const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nEqualRows)
{
    AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *pRowStyles->GetStyleNameByIndex(nIndex));
    if (nFlag)
        if (nFlag & CR_HIDDEN)
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_collapse);
        else
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_visibility, sXML_filter);
    if (nEqualRows > 1)
    {
        rtl::OUStringBuffer aBuf;
        GetMM100UnitConverter().convertNumber(aBuf, nEqualRows);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_rows_repeated, aBuf.makeStringAndClear());
    }
    rtl::OUString aName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_row)));
    GetDocHandler()->ignorableWhitespace(sWS);
    GetDocHandler()->startElement( aName, GetXAttrList());
    ClearAttrList();
}

void ScXMLExport::OpenHeaderRows()
{
    rtl::OUString aName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_header_rows)));
    GetDocHandler()->ignorableWhitespace(sWS);
    GetDocHandler()->startElement( aName, GetXAttrList());
    ClearAttrList();
    bRowHeaderOpen = sal_True;
}

void ScXMLExport::CloseHeaderRows()
{
    rtl::OUString sName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_header_rows)));
    GetDocHandler()->ignorableWhitespace(sWS);
    GetDocHandler()->endElement(sName);
}

void ScXMLExport::OpenNewRow(const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nStartRow, const sal_Int32 nEqualRows)
{
    nOpenRow = nStartRow;
    if (pGroupRows->IsGroupStart(nStartRow))
    {
        if (bHasRowHeader && bRowHeaderOpen)
            CloseHeaderRows();
        pGroupRows->OpenGroups(nStartRow);
        if (bHasRowHeader && bRowHeaderOpen)
            OpenHeaderRows();
    }
    if (bHasRowHeader && !bRowHeaderOpen && nStartRow >= aRowHeaderRange.StartRow && nStartRow <= aRowHeaderRange.EndRow)
    {
        if (nStartRow == aRowHeaderRange.StartRow)
            OpenHeaderRows();
        sal_Int32 nEquals;
        if (aRowHeaderRange.EndRow < nStartRow + nEqualRows - 1)
            nEquals = aRowHeaderRange.EndRow - nStartRow + 1;
        else
            nEquals = nEqualRows;
        WriteRowStartTag(nIndex, nFlag, nEquals);
        nOpenRow = nStartRow + nEquals - 1;
        if (nEquals < nEqualRows)
        {
            CloseRow(nStartRow + nEquals - 1);
            WriteRowStartTag(nIndex, nFlag, nEqualRows - nEquals);
            nOpenRow = nStartRow + nEqualRows - 1;
        }
    }
    else
        WriteRowStartTag(nIndex, nFlag, nEqualRows);
}

void ScXMLExport::OpenAndCloseRow(const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nStartRow, const sal_Int32 nEqualRows)
{
    OpenNewRow(nIndex, nFlag, nStartRow, nEqualRows);
    WriteRowContent();
    CloseRow(nStartRow + nEqualRows - 1);
    pRowFormatRanges->Clear();
}

void ScXMLExport::OpenRow(const sal_Int16 nTable, const sal_Int32 nStartRow, const sal_Int32 nRepeatRow)
{
    if (nRepeatRow > 1)
    {
        sal_Int32 nPrevIndex, nIndex;
        sal_Int8 nPrevFlag, nFlag;
        sal_Int32 nEqualRows = 1;
        for (sal_Int32 nRow = nStartRow; nRow < nStartRow + nRepeatRow; nRow++)
        {
            if (nRow == nStartRow)
            {
                nPrevIndex = pRowStyles->GetStyleNameIndex(nTable, nRow);
                nPrevFlag = (pDoc->GetRowFlags(static_cast<USHORT>(nRow), nTable)) & (CR_HIDDEN | CR_FILTERED);
            }
            else
            {
                nIndex = pRowStyles->GetStyleNameIndex(nTable, nRow);
                nFlag = (pDoc->GetRowFlags(static_cast<USHORT>(nRow), nTable)) & (CR_HIDDEN | CR_FILTERED);
                if (nIndex == nPrevIndex && nFlag == nPrevFlag &&
                    !(bHasRowHeader && nRow == aRowHeaderRange.StartRow) &&
                    !(pGroupRows->IsGroupStart(nRow)) &&
                    !(pGroupRows->IsGroupEnd(nRow - 1)))
                    nEqualRows++;
                else
                {
                    OpenAndCloseRow(nPrevIndex, nPrevFlag, nRow - nEqualRows, nEqualRows);
                    nEqualRows = 1;
                    nPrevIndex = nIndex;
                    nPrevFlag = nFlag;
                }
            }
        }
        OpenNewRow(nPrevIndex, nPrevFlag, nRow - nEqualRows, nEqualRows);
    }
    else
    {
        sal_Int32 nIndex = pRowStyles->GetStyleNameIndex(nTable, nStartRow);
        sal_Int8 nFlag = (pDoc->GetRowFlags(static_cast<USHORT>(nStartRow), nTable)) & (CR_HIDDEN | CR_FILTERED);
        OpenNewRow(nIndex, nFlag, nStartRow, 1);
    }
    nOpenRow = nStartRow + nRepeatRow - 1;
}

void ScXMLExport::CloseRow(const sal_Int32 nRow)
{
    if (nOpenRow > -1)
    {
        rtl::OUString sName = GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_row)));
        GetDocHandler()->ignorableWhitespace(sWS);
        GetDocHandler()->endElement(sName);
        if (bHasRowHeader && nRow == aRowHeaderRange.EndRow)
        {
            CloseHeaderRows();
            bRowHeaderOpen = sal_False;
        }
        if (pGroupRows->IsGroupEnd(nRow))
        {
            if (bHasRowHeader && bRowHeaderOpen)
                CloseHeaderRows();
            pGroupRows->CloseGroups(nRow);
            if (bHasRowHeader && bRowHeaderOpen)
                OpenHeaderRows();
        }
    }
    nOpenRow = -1;
}

void ScXMLExport::ExportFormatRanges(const sal_Int32 nStartCol, const sal_Int32 nStartRow,
    const sal_Int32 nEndCol, const sal_Int32 nEndRow, const sal_Int16 nSheet)
{
    pRowFormatRanges->Clear();
    if (nStartRow == nEndRow)
    {
        pCellStyles->GetFormatRanges(nStartCol, nEndCol, nStartRow, nSheet, pRowFormatRanges);
        if (nOpenRow == - 1)
            OpenRow(nSheet, nStartRow, 1);
        WriteRowContent();
        pRowFormatRanges->Clear();
    }
    else
    {
        if (nOpenRow > -1)
        {
            pCellStyles->GetFormatRanges(nStartCol, pSharedData->GetLastColumn(nSheet), nStartRow, nSheet, pRowFormatRanges);
            WriteRowContent();
            CloseRow(nStartRow);
            sal_Int32 nRows = 1;
            sal_Int32 nTotalRows = nEndRow - nStartRow + 1 - 1;
            while (nRows < nTotalRows)
            {
                pRowFormatRanges->Clear();
                pCellStyles->GetFormatRanges(0, pSharedData->GetLastColumn(nSheet), nStartRow + nRows, nSheet, pRowFormatRanges);
                sal_Int32 nMaxRows = pRowFormatRanges->GetMaxRows();
                if (nMaxRows >= nTotalRows - nRows)
                {
                    OpenRow(nSheet, nStartRow + nRows, nTotalRows - nRows);
                    nRows += nTotalRows - nRows;
                }
                else
                {
                    OpenRow(nSheet, nStartRow + nRows, nMaxRows);
                    nRows += nMaxRows;
                }
                if (!pRowFormatRanges->GetSize())
                    pCellStyles->GetFormatRanges(0, pSharedData->GetLastColumn(nSheet), nStartRow + nRows, nSheet, pRowFormatRanges);
                WriteRowContent();
                CloseRow(nStartRow + nRows - 1);
            }
            if (nTotalRows == 1)
                CloseRow(nStartRow);
            OpenRow(nSheet, nEndRow, 1);
            pRowFormatRanges->Clear();
            pCellStyles->GetFormatRanges(0, nEndCol, nEndRow, nSheet, pRowFormatRanges);
            WriteRowContent();
        }
        else
        {
            sal_Int32 nRows = 0;
            sal_Int32 nTotalRows = nEndRow - nStartRow + 1 - 1;
            while (nRows < nTotalRows)
            {
                pCellStyles->GetFormatRanges(0, pSharedData->GetLastColumn(nSheet), nStartRow + nRows, nSheet, pRowFormatRanges);
                sal_Int32 nMaxRows = pRowFormatRanges->GetMaxRows();
                if (nMaxRows >= nTotalRows - nRows)
                {
                    OpenRow(nSheet, nStartRow + nRows, nTotalRows - nRows);
                    nRows += nTotalRows - nRows;
                }
                else
                {
                    OpenRow(nSheet, nStartRow + nRows, nMaxRows);
                    nRows += nMaxRows;
                }
                if (!pRowFormatRanges->GetSize())
                    pCellStyles->GetFormatRanges(0, pSharedData->GetLastColumn(nSheet), nStartRow + nRows, nSheet, pRowFormatRanges);
                WriteRowContent();
                CloseRow(nStartRow + nRows - 1);
            }
            OpenRow(nSheet, nEndRow, 1);
            pRowFormatRanges->Clear();
            pCellStyles->GetFormatRanges(0, nEndCol, nEndRow, nSheet, pRowFormatRanges);
            WriteRowContent();
        }
    }
}

sal_Bool ScXMLExport::GetColumnHeader(com::sun::star::table::CellRangeAddress& aColumnHeaderRange) const
{
    sal_Bool bResult(sal_False);
    uno::Reference <sheet::XPrintAreas> xPrintAreas (xCurrentTable, uno::UNO_QUERY);
    if (xPrintAreas.is())
    {
        bResult = xPrintAreas->getPrintTitleColumns();
        aColumnHeaderRange = xPrintAreas->getTitleColumns();
    }
    return bResult;
}

sal_Bool ScXMLExport::GetRowHeader(com::sun::star::table::CellRangeAddress& aRowHeaderRange) const
{
    sal_Bool bResult(sal_False);
    uno::Reference <sheet::XPrintAreas> xPrintAreas (xCurrentTable, uno::UNO_QUERY);
    if (xPrintAreas.is())
    {
        bResult = xPrintAreas->getPrintTitleRows();
        aRowHeaderRange = xPrintAreas->getTitleRows();
    }
    return bResult;
}

void ScXMLExport::FillFieldGroup(ScOutlineArray* pFields, ScMyOpenCloseColumnRowGroup* pGroups)
{
    sal_Int32 nDepth = pFields->GetDepth();
    for(sal_Int32 i = 0; i < nDepth; i++)
    {
        sal_Int32 nFields = pFields->GetCount(static_cast<USHORT>(i));
        for (sal_Int32 j = 0; j < nFields; j++)
        {
            ScMyColumnRowGroup aGroup;
            ScOutlineEntry* pEntry = pFields->GetEntry(static_cast<USHORT>(i), static_cast<USHORT>(j));
            aGroup.nField = pEntry->GetStart();
            aGroup.nLevel = static_cast<sal_Int16>(i);
            aGroup.bDisplay = !(pEntry->IsHidden());
            pGroups->AddGroup(aGroup, pEntry->GetEnd());
        }
    }
    if (nDepth)
        pGroups->Sort();
}

void ScXMLExport::FillColumnRowGroups()
{
    ScOutlineTable* pOutlineTable = pDoc->GetOutlineTable( nCurrentTable, sal_False );
    if(pOutlineTable)
    {
        ScOutlineArray* pCols = pOutlineTable->GetColArray();
        ScOutlineArray* pRows = pOutlineTable->GetRowArray();
        if (pCols)
            FillFieldGroup(pCols, pGroupColumns);
        if (pRows)
            FillFieldGroup(pRows, pGroupRows);
        pSharedData->SetLastColumn(nCurrentTable, pGroupColumns->GetLast());
        pSharedData->SetLastRow(nCurrentTable, pGroupRows->GetLast());
    }
}

void ScXMLExport::SetBodyAttributes()
{
    if (pDoc->IsDocProtected())
    {
        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_structure_protected, sXML_true);
        rtl::OUStringBuffer aBuffer;
        SvXMLUnitConverter::encodeBase64(aBuffer, pDoc->GetDocPassword());
        if (aBuffer.getLength())
            AddAttribute(XML_NAMESPACE_TABLE, sXML_protection_key, aBuffer.makeStringAndClear());
    }
}

void ScXMLExport::_ExportContent()
{
    if (!pSharedData)
    {
        sal_Int32 nTableCount(0);
        sal_Int32 nShapesCount(0);
        sal_Int32 nCellCount(pDoc->GetCellCount());
        CollectSharedData(nTableCount, nShapesCount, nCellCount);
        DBG_ERROR("no shared data setted");
    }
    pSharedData->nOldProgressValue = pSharedData->nProgressValue;
    ScXMLExportDatabaseRanges aExportDatabaseRanges(*this);
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
            pChangeTrackingExportHelper->CollectAndWriteChanges();
            WriteCalculationSettings(xSpreadDoc);
            sal_Int32 nTableCount = xIndex->getCount();
            ScMyAreaLinksContainer aAreaLinks;
            GetAreaLinks( xSpreadDoc, aAreaLinks );
            ScMyEmptyDatabaseRangesContainer aEmptyRanges = aExportDatabaseRanges.GetEmptyDatabaseRanges();
            ScMyDetectiveOpContainer aDetectiveOpContainer;
            GetDetectiveOpList( aDetectiveOpContainer );

            pCellStyles->Sort();
            pSharedData->SortShapesContainer();
            pMergedRangesContainer->Sort();
            pDetectiveObjContainer->Sort();

            ScMyNotEmptyCellsIterator aCellsItr(*this);
            pCellsItr = &aCellsItr;
            aCellsItr.SetShapes( pSharedData->GetShapesContainer() );
            aCellsItr.SetMergedRanges( pMergedRangesContainer );
            aCellsItr.SetAreaLinks( &aAreaLinks );
            aCellsItr.SetEmptyDatabaseRanges( &aEmptyRanges );
            aCellsItr.SetDetectiveObj( pDetectiveObjContainer );
            aCellsItr.SetDetectiveOp( &aDetectiveOpContainer );

            if (nTableCount > 0)
                pValidationsContainer->WriteValidations(*this);
            WriteTheLabelRanges( xSpreadDoc );
            for (sal_uInt16 nTable = 0; nTable < nTableCount; nTable++)
            {
                uno::Any aTable = xIndex->getByIndex(nTable);
                uno::Reference<sheet::XSpreadsheet> xTable;
                if (aTable>>=xTable)
                {
                    xCurrentTable = xTable;
                    uno::Reference<container::XNamed> xName (xTable, uno::UNO_QUERY );
                    if ( xName.is() )
                    {
                        nCurrentTable = nTable;
                        rtl::OUString sOUTableName = xName->getName();
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_name, sOUTableName);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, aTableStyles[nTable]);
                        uno::Reference<util::XProtectable> xProtectable (xTable, uno::UNO_QUERY);
                        if (xProtectable.is())
                            if (xProtectable->isProtected())
                            {
                                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_protected, sXML_true);
                                rtl::OUStringBuffer aBuffer;
                                SvXMLUnitConverter::encodeBase64(aBuffer, pDoc->GetTabPassword(nTable));
                                if (aBuffer.getLength())
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_protection_key, aBuffer.makeStringAndClear());
                            }
                        rtl::OUString sPrintRanges( GetPrintRanges() );
                        if( sPrintRanges.getLength() )
                            AddAttribute( XML_NAMESPACE_TABLE, sXML_print_ranges, sPrintRanges );
                        SvXMLElementExport aElemT(*this, XML_NAMESPACE_TABLE, sXML_table, sal_True, sal_True);
                        CheckAttrList();
                        WriteTableSource();
                        WriteScenario();
                        uno::Reference<drawing::XDrawPage> xDrawPage;
                        if (pSharedData->HasForm(nTable, xDrawPage) && xDrawPage.is())
                        {
                            ::xmloff::OOfficeFormsExport aForms(*this);
                            GetFormExport()->exportForms( xDrawPage );
                            sal_Bool bRet = GetFormExport()->seekPage( xDrawPage );
                            DBG_ASSERT( bRet, "OFormLayerXMLExport::seekPage failed!" );
                        }
                        GetxCurrentShapes(xCurrentShapes);
                        GetShapeExport()->seekShapes(aXShapesVec[nTable]);
                        WriteTableShapes();
                        table::CellRangeAddress aRange = GetEndAddress(xTable, nTable);
                        pSharedData->SetLastColumn(nTable, aRange.EndColumn);
                        pSharedData->SetLastRow(nTable, aRange.EndRow);
                        aCellsItr.SetCurrentTable(nTable);
                        pGroupColumns->NewTable();
                        pGroupRows->NewTable();
                        FillColumnRowGroups();
                        table::CellRangeAddress aColumnHeaderRange;
                        sal_Bool bHasColumnHeader(GetColumnHeader(aColumnHeaderRange));
                        if (bHasColumnHeader)
                            pSharedData->SetLastColumn(nTable, aColumnHeaderRange.EndColumn);
                        ExportColumns(nTable, aColumnHeaderRange, bHasColumnHeader);
                        bHasRowHeader = GetRowHeader(aRowHeaderRange);
                        bRowHeaderOpen = sal_False;
                        if (bHasRowHeader)
                            pSharedData->SetLastRow(nTable, aRowHeaderRange.EndRow);
                        sal_Bool bIsFirst(sal_True);
                        sal_Int32 nEqualCells(0);
                        ScMyCell aCell;
                        ScMyCell aPrevCell;
                        while(aCellsItr.GetNext(aCell))
                        {
                            if (bIsFirst)
                            {
                                ExportFormatRanges(0, 0, aCell.aCellAddress.Column - 1, aCell.aCellAddress.Row, nTable);
                                aPrevCell = aCell;
                                bIsFirst = sal_False;
                            }
                            else
                            {
                                if ((aPrevCell.aCellAddress.Row == aCell.aCellAddress.Row) &&
                                    (aPrevCell.aCellAddress.Column + nEqualCells + 1 == aCell.aCellAddress.Column))
                                {
                                    if(IsCellEqual(aPrevCell, aCell))
                                        nEqualCells++;
                                    else
                                    {
                                        SetRepeatAttribute(nEqualCells);
                                        WriteCell(aPrevCell);
                                        nEqualCells = 0;
                                        aPrevCell = aCell;
                                    }
                                }
                                else
                                {
                                    SetRepeatAttribute(nEqualCells);
                                    WriteCell(aPrevCell);
                                    ExportFormatRanges(aPrevCell.aCellAddress.Column + nEqualCells + 1, aPrevCell.aCellAddress.Row,
                                        aCell.aCellAddress.Column - 1, aCell.aCellAddress.Row, nTable);
                                    nEqualCells = 0;
                                    aPrevCell = aCell;
                                }
                            }
                        }
                        if (!bIsFirst)
                        {
                            SetRepeatAttribute(nEqualCells);
                            WriteCell(aPrevCell);
                            ExportFormatRanges(aPrevCell.aCellAddress.Column + nEqualCells + 1, aPrevCell.aCellAddress.Row,
                                pSharedData->GetLastColumn(nTable), pSharedData->GetLastRow(nTable), nTable);
                        }
                        else
                            ExportFormatRanges(0, 0, pSharedData->GetLastColumn(nTable), pSharedData->GetLastRow(nTable), nTable);
                        CloseRow(pSharedData->GetLastRow(nTable));
                        nEqualCells = 0;
                    }
                }
                if (pSharedData->nProgressValue < pSharedData->nOldProgressValue + pSharedData->nProgressObjects)
                    GetProgressBarHelper()->SetValue(++pSharedData->nProgressValue);
            }
        }
        WriteNamedExpressions(xSpreadDoc);
        aExportDatabaseRanges.WriteDatabaseRanges(xSpreadDoc);
        ScXMLExportDataPilot aExportDataPilot(*this);
        aExportDataPilot.WriteDataPilots(xSpreadDoc);
        WriteConsolidation();
        ScXMLExportDDELinks aExportDDELinks(*this);
        aExportDDELinks.WriteDDELinks(xSpreadDoc);
        GetProgressBarHelper()->SetValue(pSharedData->nProgressReference);
    }
}

void ScXMLExport::_ExportStyles( sal_Bool bUsed )
{
    SvXMLExport::_ExportStyles(bUsed);
    ScXMLStyleExport aStylesExp(*this, rtl::OUString(), GetAutoStylePool().get());
    uno::Reference <lang::XMultiServiceFactory> xMultiServiceFactory(GetModel(), uno::UNO_QUERY);
    if (xMultiServiceFactory.is())
    {
        uno::Reference <uno::XInterface> xInterface = xMultiServiceFactory->createInstance(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sheet.Defaults")));
        uno::Reference <beans::XPropertySet> xProperties(xInterface, uno::UNO_QUERY);
        if (xProperties.is())
            aStylesExp.exportDefaultStyle(xProperties, XML_STYLE_FAMILY_TABLE_CELL_STYLES_NAME, xCellStylesExportPropertySetMapper);
    }
    uno::Reference <style::XStyleFamiliesSupplier> xStyleFamiliesSupplier (xModel, uno::UNO_QUERY);
    if (xStyleFamiliesSupplier.is())
    {
        uno::Reference <container::XNameAccess> aStylesFamilies = xStyleFamiliesSupplier->getStyleFamilies();
        if (aStylesFamilies.is())
        {
            uno::Any aStyleFamily = aStylesFamilies->getByName(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CellStyles")));
            uno::Reference <container::XIndexAccess> aCellStyles;
            if (aStyleFamily >>= aCellStyles)
            {
                sal_Int32 nCount = aCellStyles->getCount();
                for (sal_Int32 i = 0; i < nCount; i++)
                {
                    uno::Any aCellStyle = aCellStyles->getByIndex(i);
                    uno::Reference <beans::XPropertySet> xCellProperties;
                    if (aCellStyle >>= xCellProperties)
                    {
                        uno::Any aNumberFormat = xCellProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_NUMFMT)));
                        sal_Int32 nNumberFormat;
                        if (aNumberFormat >>= nNumberFormat)
                        {
                            addDataStyle(nNumberFormat);
                        }
                    }
                }
            }
        }
    }
    exportDataStyles();

    aStylesExp.exportStyleFamily(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CellStyles")),
        XML_STYLE_FAMILY_TABLE_CELL_STYLES_NAME, xCellStylesExportPropertySetMapper, FALSE, XML_STYLE_FAMILY_TABLE_CELL);
}

void ScXMLExport::_ExportAutoStyles()
{
    uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
    if ( xSpreadDoc.is() )
    {
        uno::Reference<sheet::XSpreadsheets> xSheets = xSpreadDoc->getSheets();
        uno::Reference<container::XIndexAccess> xIndex( xSheets, uno::UNO_QUERY );
        if ( xIndex.is() )
        {
            if (getExportFlags() & EXPORT_CONTENT)
            {
                if (!pSharedData)
                {
                    sal_Int32 nTableCount(0);
                    sal_Int32 nShapesCount(0);
                    sal_Int32 nCellCount(pDoc->GetCellCount());
                    CollectSharedData(nTableCount, nShapesCount, nCellCount);
                    DBG_ERROR("no shared data setted");
                }
                pSharedData->nOldProgressValue = pSharedData->nProgressValue;
                rtl::OUString SC_SCOLUMNPREFIX(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_COLUMN_STYLES_PREFIX));
                rtl::OUString SC_SROWPREFIX(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_ROW_STYLES_PREFIX));
                rtl::OUString SC_SCELLPREFIX(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_TABLE_CELL_STYLES_PREFIX));
                GetChartExport()->setTableAddressMapper(xChartExportMapper);
                sal_Int32 nTableCount = xIndex->getCount();
                pCellStyles->AddNewTable(nTableCount - 1);
                uno::Reference<drawing::XShapes> xShapes;
                aXShapesVec.resize(nTableCount, xShapes);
                for (sal_uInt16 nTable = 0; nTable < nTableCount; nTable++)
                {
                    uno::Any aTable = xIndex->getByIndex(nTable);
                    uno::Reference<sheet::XSpreadsheet> xTable;
                    if (aTable>>=xTable)
                    {
                        CollectShapesAutoStyles(xTable, nTable);
                        uno::Reference<beans::XPropertySet> xTableProperties(xTable, uno::UNO_QUERY);
                        if (xTableProperties.is())
                        {
                            std::vector<XMLPropertyState> xPropStates = xTableStylesExportPropertySetMapper->Filter(xTableProperties);
                            if(xPropStates.size())
                            {
                                rtl::OUString sParent;
                                rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_TABLE, sParent, xPropStates);
                                if (!sName.getLength())
                                {
                                    sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_TABLE, sParent, xPropStates);
                                }
                                aTableStyles.push_back(sName);
                            }
                        }
                        uno::Reference<sheet::XCellFormatRangesSupplier> xCellFormatRanges ( xTable, uno::UNO_QUERY );
                        if ( xCellFormatRanges.is() )
                        {
                            uno::Reference<container::XIndexAccess> xFormatRangesIndex = xCellFormatRanges->getCellFormatRanges();
                            if (xFormatRangesIndex.is())
                            {
                                sal_Int32 nFormatRangesCount = xFormatRangesIndex->getCount();
                                for (sal_Int32 nFormatRange = 0; nFormatRange < nFormatRangesCount; nFormatRange++)
                                {
                                    uno::Any aFormatRange = xFormatRangesIndex->getByIndex(nFormatRange);
                                    uno::Reference<table::XCellRange> xCellRange;
                                    if (aFormatRange >>= xCellRange)
                                    {
                                        uno::Reference <beans::XPropertySet> xProperties (xCellRange, uno::UNO_QUERY);
                                        if (xProperties.is())
                                        {
                                            uno::Reference <sheet::XCellRangeAddressable> xCellRangeAddressable(xCellRange, uno::UNO_QUERY);
                                            if (xCellRangeAddressable.is())
                                            {
                                                table::CellRangeAddress aRangeAddress = xCellRangeAddressable->getRangeAddress();
                                                uno::Any aValidation = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_VALIXML)));
                                                sal_Int32 nValidationIndex(-1);
                                                if (pValidationsContainer->AddValidation(aValidation, aRangeAddress, nValidationIndex))
                                                {
                                                    pSharedData->SetLastColumn(nTable, aRangeAddress.EndColumn);
                                                    pSharedData->SetLastRow(nTable, aRangeAddress.EndRow);
                                                }
                                                uno::Any aNumberFormat = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_NUMFMT)));
                                                sal_Int32 nNumberFormat;
                                                if (aNumberFormat >>= nNumberFormat)
                                                {
                                                    addDataStyle(nNumberFormat);
                                                }
                                                uno::Any aStyle = xProperties->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_CELLSTYL)));
                                                rtl::OUString sStyleName;
                                                if (aStyle >>= sStyleName)
                                                {
                                                    std::vector< XMLPropertyState > xPropStates = xCellStylesExportPropertySetMapper->Filter( xProperties );
                                                    if (xPropStates.size())
                                                    {
                                                        sal_Int32 nIndex;
                                                        rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_CELL, sStyleName, xPropStates);
                                                        sal_Bool bIsAutoStyle = sal_True;
                                                        if (!sName.getLength())
                                                        {
                                                            sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_CELL, sStyleName, xPropStates);
                                                            rtl::OUString* pTemp = new rtl::OUString(sName);
                                                            nIndex = pCellStyles->AddStyleName(pTemp);
                                                        }
                                                        else
                                                            nIndex = pCellStyles->GetIndexOfStyleName(sName, SC_SCELLPREFIX, bIsAutoStyle);
                                                        pSharedData->SetLastColumn(nTable, aRangeAddress.EndColumn);
                                                        pSharedData->SetLastRow(nTable, aRangeAddress.EndRow);
                                                        pCellStyles->AddRangeStyleName(aRangeAddress, nIndex, bIsAutoStyle, nValidationIndex);
                                                        GetMerged(xCellRange, xTable);
                                                    }
                                                    else
                                                    {
                                                        GetMerged(xCellRange, xTable);
                                                        rtl::OUString* pTemp = new rtl::OUString(sStyleName);
                                                        sal_Int32 nIndex = pCellStyles->AddStyleName(pTemp, sal_False);
                                                        pCellStyles->AddRangeStyleName(aRangeAddress, nIndex, sal_False, nValidationIndex);
                                                        if (sStyleName.compareToAscii("Default") != 0)
                                                        {
                                                            pSharedData->SetLastColumn(nTable, aRangeAddress.EndColumn);
                                                            pSharedData->SetLastRow(nTable, aRangeAddress.EndRow);
                                                        }
                                                    }
                                                }
                                                if (pSharedData->nProgressValue < pSharedData->nOldProgressValue + pSharedData->nProgressObjects)
                                                    GetProgressBarHelper()->SetValue(++pSharedData->nProgressValue);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        uno::Reference<sheet::XCellRangesQuery> xCellRangesQuery (xTable, uno::UNO_QUERY);
                        uno::Reference<table::XColumnRowRange> xColumnRowRange (xTable, uno::UNO_QUERY);
                        if (xColumnRowRange.is())
                        {
                            if (pDoc)
                            {
                                uno::Reference<table::XTableColumns> xTableColumns = xColumnRowRange->getColumns();
                                if (xTableColumns.is())
                                {
                                    sal_Int32 nColumns = pDoc->GetLastFlaggedCol(nTable);
                                    pSharedData->SetLastColumn(nTable, nColumns);
                                    table::CellRangeAddress aCellAddress = GetEndAddress(xTable, nTable);
                                    if (aCellAddress.EndColumn > nColumns)
                                    {
                                        nColumns++;
                                        pColumnStyles->AddNewTable(nTable, aCellAddress.EndColumn);
                                    }
    //                              else if (nColumns < MAXCOL)
    //                                  pColumnStyles->AddNewTable(nTable, ++nColumns);
                                    else
                                        pColumnStyles->AddNewTable(nTable, nColumns);
                                    sal_Int32 nColumn = 0;
                                    while (/*nColumn <= nColumns && */nColumn <= MAXCOL)
                                    {
                                        sal_Int32 nIndex;
                                        uno::Any aColumn = xTableColumns->getByIndex(nColumn);
                                        uno::Reference<table::XCellRange> xTableColumn;
                                        if (aColumn >>= xTableColumn)
                                        {
                                            uno::Reference <beans::XPropertySet> xColumnProperties(xTableColumn, uno::UNO_QUERY);
                                            if (xColumnProperties.is())
                                            {
                                                std::vector<XMLPropertyState> xPropStates = xColumnStylesExportPropertySetMapper->Filter(xColumnProperties);
                                                if(xPropStates.size())
                                                {
                                                    rtl::OUString sParent;
                                                    rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_COLUMN, sParent, xPropStates);
                                                    if (!sName.getLength())
                                                    {
                                                        sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_COLUMN, sParent, xPropStates);
                                                        rtl::OUString* pTemp = new rtl::OUString(sName);
                                                        nIndex = pColumnStyles->AddStyleName(pTemp);
                                                    }
                                                    else
                                                        nIndex = pColumnStyles->GetIndexOfStyleName(sName, SC_SCOLUMNPREFIX);
                                                    pColumnStyles->AddFieldStyleName(nTable, nColumn, nIndex);
                                                }
                                            }
                                        }
                                        sal_Int32 nOld = nColumn;
                                        nColumn = pDoc->GetNextDifferentFlaggedCol(nTable, static_cast<USHORT>(nColumn));
                                        if (nColumn == MAXCOL)
                                            nColumn++;
                                        for (sal_Int32 i = nOld + 1; i < nColumn; i++)
                                            pColumnStyles->AddFieldStyleName(nTable, i, nIndex);
                                    }
                                    if (aCellAddress.EndColumn > nColumns)
                                    {
                                        sal_Int32 nIndex = pColumnStyles->GetStyleNameIndex(nTable, nColumns);
                                        for (sal_Int32 i = nColumns + 1; i <= aCellAddress.EndColumn; i++)
                                            pColumnStyles->AddFieldStyleName(nTable, i, nIndex);
                                    }
                                }
                                uno::Reference<table::XTableRows> xTableRows = xColumnRowRange->getRows();
                                if (xTableRows.is())
                                {
                                    sal_Int32 nRows = pDoc->GetLastFlaggedRow(nTable);
                                    pSharedData->SetLastRow(nTable, nRows);
                                    table::CellRangeAddress aCellAddress = GetEndAddress(xTable, nTable);
                                    if (aCellAddress.EndRow > nRows)
                                    {
                                        nRows++;
                                        pRowStyles->AddNewTable(nTable, aCellAddress.EndRow);
                                    }
    //                              else if (nRows < MAXROW)
    //                                  pRowStyles->AddNewTable(nTable, ++nRows);
                                    else
                                        pRowStyles->AddNewTable(nTable, nRows);
                                    sal_Int32 nRow = 0;
                                    while ( /*nRow <= nRows && */nRow <= MAXROW)
                                    {
                                        sal_Int32 nIndex;
                                        uno::Any aRow = xTableRows->getByIndex(nRow);
                                        uno::Reference<table::XCellRange> xTableRow;
                                        if (aRow >>= xTableRow)
                                        {
                                            uno::Reference <beans::XPropertySet> xRowProperties(xTableRow, uno::UNO_QUERY);
                                            if(xRowProperties.is())
                                            {
                                                std::vector<XMLPropertyState> xPropStates = xRowStylesExportPropertySetMapper->Filter(xRowProperties);
                                                if(xPropStates.size())
                                                {
                                                    rtl::OUString sParent;
                                                    rtl::OUString sName = GetAutoStylePool()->Find(XML_STYLE_FAMILY_TABLE_ROW, sParent, xPropStates);
                                                    if (!sName.getLength())
                                                    {
                                                        sName = GetAutoStylePool()->Add(XML_STYLE_FAMILY_TABLE_ROW, sParent, xPropStates);
                                                        rtl::OUString* pTemp = new rtl::OUString(sName);
                                                        nIndex = pRowStyles->AddStyleName(pTemp);
                                                    }
                                                    else
                                                        nIndex = pRowStyles->GetIndexOfStyleName(sName, SC_SROWPREFIX);
                                                    pRowStyles->AddFieldStyleName(nTable, nRow, nIndex);
                                                }
                                            }
                                        }
                                        sal_Int32 nOld = nRow;
                                        nRow = pDoc->GetNextDifferentFlaggedRow(nTable, static_cast<USHORT>(nRow));
                                        if (nRow == MAXROW)
                                            nRow++;
                                        for (sal_Int32 i = nOld + 1; i < nRow; i++)
                                            pRowStyles->AddFieldStyleName(nTable, i, nIndex);
                                    }
                                    if (aCellAddress.EndRow > nRows)
                                    {
                                        sal_Int32 nIndex = pRowStyles->GetStyleNameIndex(nTable, nRows);
                                        for (sal_Int32 i = nRows + 1; i <= aCellAddress.EndRow; i++)
                                            pRowStyles->AddFieldStyleName(nTable, i, nIndex);
                                    }
                                }
                            }
                        }
                        if (xCellRangesQuery.is())
                        {
                            uno::Reference<sheet::XSheetCellRanges> xSheetCellRanges = xCellRangesQuery->queryContentCells(sheet::CellFlags::STRING);
                            if (xSheetCellRanges.is())
                            {
                                uno::Reference<container::XEnumerationAccess> xCellsAccess = xSheetCellRanges->getCells();
                                if (xCellsAccess.is())
                                {
                                    uno::Reference<container::XEnumeration> xCells = xCellsAccess->createEnumeration();
                                    if (xCells.is())
                                    {
                                        while (xCells->hasMoreElements())
                                        {
                                            uno::Any aCell = xCells->nextElement();
                                            uno::Reference<table::XCell> xCell;
                                            if (aCell >>= xCell)
                                            {
                                                if (IsEditCell(xCell))
                                                {
                                                    uno::Reference<text::XText> xText(xCell, uno::UNO_QUERY);
                                                    if (xText.is())
                                                    {
                                                        GetTextParagraphExport()->collectTextAutoStyles(xText, sal_False, sal_False);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (pSharedData->nProgressValue < pSharedData->nOldProgressValue + pSharedData->nProgressObjects)
                        GetProgressBarHelper()->SetValue(++pSharedData->nProgressValue);
                }
                pChangeTrackingExportHelper->CollectAutoStyles();

                GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_COLUMN,
                    GetDocHandler(), GetMM100UnitConverter(), GetNamespaceMap());
                GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_ROW,
                    GetDocHandler(), GetMM100UnitConverter(), GetNamespaceMap());
                GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_TABLE,
                    GetDocHandler(), GetMM100UnitConverter(), GetNamespaceMap());
                exportAutoDataStyles();
                GetAutoStylePool()->exportXML(XML_STYLE_FAMILY_TABLE_CELL,
                    GetDocHandler(), GetMM100UnitConverter(), GetNamespaceMap());
                GetTextParagraphExport()->exportTextAutoStyles();

                GetShapeExport()->exportAutoStyles();

                //GetFormExport()->exportAutoStyles();

                pSharedData->nProgressValue = pSharedData->nOldProgressValue + pSharedData->nProgressObjects;
                GetProgressBarHelper()->SetValue(pSharedData->nProgressValue);
            }
            if (getExportFlags() & EXPORT_ALL)
                GetChartExport()->exportAutoStyles();
            if (getExportFlags() & EXPORT_MASTERSTYLES)
            {
                GetPageExport()->collectAutoStyles(sal_True);
                  GetPageExport()->exportAutoStyles();
            }
        }
    }
}

void ScXMLExport::_ExportMasterStyles()
{
    GetPageExport()->exportMasterStyles( sal_True );
}

void ScXMLExport::CollectInternalShape( uno::Reference< drawing::XShape > xShape )
{
    // detective objects
    SvxShape* pShapeImp = SvxShape::getImplementation( xShape );
    if( pShapeImp )
    {
        SdrObject *pObject = pShapeImp->GetSdrObject();
        if( pObject )
        {
            ScDetectiveFunc aDetFunc( pDoc, nCurrentTable );
            ScAddress       aPosition;
            ScRange         aSourceRange;
            sal_Bool        bRedLine;
            ScDetectiveObjType eObjType = aDetFunc.GetDetectiveObjectType(
                pObject, aPosition, aSourceRange, bRedLine );
            pDetectiveObjContainer->AddObject( eObjType, aPosition, aSourceRange, bRedLine );
        }
    }
}

sal_Bool ScXMLExport::GetMerge (const uno::Reference <sheet::XSpreadsheet>& xTable,
                            const sal_Int32 nCol, const sal_Int32 nRow,
                            table::CellRangeAddress& aCellAddress)
{
    uno::Reference<table::XCellRange> xCellRange = xTable->getCellRangeByPosition(nCol, nRow, nCol, nRow);
    if (xCellRange.is())
    {
        uno::Reference<sheet::XSheetCellRange> xSheetCellRange(xCellRange, uno::UNO_QUERY);
        if (xSheetCellRange.is())
        {
            uno::Reference<sheet::XSheetCellCursor> xCursor = xTable->createCursorByRange(xSheetCellRange);
            if(xCursor.is())
            {
                uno::Reference<sheet::XCellRangeAddressable> xCellAddress (xCursor, uno::UNO_QUERY);
                xCursor->collapseToMergedArea();
                aCellAddress = xCellAddress->getRangeAddress();
                return sal_True;
            }
        }
    }
    return sal_False;
}

void ScXMLExport::GetMerged (const uno::Reference <table::XCellRange>& xCellRange,
                            const uno::Reference <sheet::XSpreadsheet>& xTable)
{
    uno::Reference<sheet::XSheetCellRange> xSheetCellRange(xCellRange, uno::UNO_QUERY);
    if (xSheetCellRange.is())
    {
        uno::Reference<sheet::XSheetCellCursor> xCursor = xTable->createCursorByRange(xSheetCellRange);
        if(xCursor.is())
        {
            uno::Reference<sheet::XCellRangeAddressable> xCellAddress (xCursor, uno::UNO_QUERY);
            table::CellRangeAddress aCellAddress = xCellAddress->getRangeAddress();
            xCursor->collapseToMergedArea();
            table::CellRangeAddress aCellAddress2 = xCellAddress->getRangeAddress();
            if (aCellAddress2.StartRow != aCellAddress.StartRow ||
                aCellAddress2.EndRow != aCellAddress.EndRow ||
                aCellAddress2.StartColumn != aCellAddress.StartColumn ||
                aCellAddress2.EndColumn != aCellAddress.EndColumn)
            {
                table::CellRangeAddress aCellAddress3;
                sal_Int32 nNextRow (MAXROW);
                for (sal_Int32 j = aCellAddress2.StartRow; j <= aCellAddress2.EndRow; j++)
                {
                    for (sal_Int32 i = aCellAddress2.StartColumn; i <= aCellAddress2.EndColumn; i++)
                    {
                        nNextRow = MAXROW;
                        if (GetMerge(xTable, i, j, aCellAddress3) &&
                            (aCellAddress3.EndColumn > i || aCellAddress3.EndRow > j))
                        {
                            pMergedRangesContainer->AddRange(aCellAddress3);
                            i = aCellAddress3.EndColumn;
                            if (aCellAddress3.EndRow < nNextRow)
                                nNextRow = aCellAddress3.EndRow;
                        }
                    }
                    j = nNextRow;
                }
                pSharedData->SetLastColumn(aCellAddress2.Sheet, aCellAddress2.EndColumn);
                pSharedData->SetLastRow(aCellAddress2.Sheet, aCellAddress2.EndRow);
            }
        }
    }
}

sal_Bool ScXMLExport::IsMatrix (const uno::Reference <table::XCellRange>& xCellRange,
                            const uno::Reference <sheet::XSpreadsheet>& xTable,
                            const sal_Int32 nCol, const sal_Int32 nRow,
                            table::CellRangeAddress& aCellAddress, sal_Bool& bIsFirst) const
{
    bIsFirst = sal_False;
    uno::Reference <table::XCellRange> xMatrixCellRange = xCellRange->getCellRangeByPosition(nCol,nRow,nCol,nRow);
    uno::Reference <sheet::XArrayFormulaRange> xArrayFormulaRange (xMatrixCellRange, uno::UNO_QUERY);
    if (xMatrixCellRange.is() && xArrayFormulaRange.is())
    {
        rtl::OUString sArrayFormula = xArrayFormulaRange->getArrayFormula();
        if (sArrayFormula.getLength())
        {
            uno::Reference<sheet::XSheetCellRange> xMatrixSheetCellRange (xMatrixCellRange, uno::UNO_QUERY);
            if (xMatrixSheetCellRange.is())
            {
                uno::Reference<sheet::XSheetCellCursor> xMatrixSheetCursor = xTable->createCursorByRange(xMatrixSheetCellRange);
                if (xMatrixSheetCursor.is())
                {
                    xMatrixSheetCursor->collapseToCurrentArray();
                    uno::Reference<sheet::XCellRangeAddressable> xMatrixCellAddress (xMatrixSheetCursor, uno::UNO_QUERY);
                    if (xMatrixCellAddress.is())
                    {
                        aCellAddress = xMatrixCellAddress->getRangeAddress();
                        if ((aCellAddress.StartColumn == nCol && aCellAddress.StartRow == nRow) &&
                            (aCellAddress.EndColumn > nCol || aCellAddress.EndRow > nRow))
                        {
                            bIsFirst = sal_True;
                            return sal_True;
                        }
                        else if (aCellAddress.StartColumn != nCol || aCellAddress.StartRow != nRow ||
                            aCellAddress.EndColumn != nCol || aCellAddress.EndRow != nRow)
                            return sal_True;
                        else
                        {
                            bIsFirst = sal_True;
                            return sal_True;
                        }
                    }
                }
            }
        }
    }
    return sal_False;
}

sal_Bool ScXMLExport::GetCellText (const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell,
        rtl::OUString& sOUTemp) const
{
    uno::Reference <text::XText> xText (xCell, uno::UNO_QUERY);
    if (xText.is())
    {
        sOUTemp = xText->getString();
        return sal_True;
    }
    return sal_False;
}

sal_Int16 ScXMLExport::GetCellType(const sal_Int32 nNumberFormat, sal_Bool& bIsStandard)
{
    uno::Reference <util::XNumberFormatsSupplier> xNumberFormatsSupplier = GetNumberFormatsSupplier();
    if (xNumberFormatsSupplier.is())
    {
        uno::Reference <util::XNumberFormats> xNumberFormats = xNumberFormatsSupplier->getNumberFormats();
        if (xNumberFormats.is())
        {
            try
            {
                uno::Reference <beans::XPropertySet> xNumberPropertySet = xNumberFormats->getByKey(nNumberFormat);
                uno::Any aIsStandardFormat = xNumberPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_STANDARDFORMAT)));
                aIsStandardFormat >>= bIsStandard;
                uno::Any aNumberFormat = xNumberPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_TYPE)));
                sal_Int16 nNumberFormat;
                if ( aNumberFormat >>= nNumberFormat )
                {
                    return nNumberFormat;
                }
            }
            catch ( uno::Exception& )
            {
                DBG_ERROR("Numberformat not found");
            }
        }
    }
    return 0;
}

sal_Int32 ScXMLExport::GetCellNumberFormat(const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell) const
{
    uno::Reference <beans::XPropertySet> xPropertySet (xCell, uno::UNO_QUERY);
    if (xPropertySet.is())
    {
        uno::Any aNumberFormatPropertyKey = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_NUMFMT)));
        sal_Int32 nNumberFormatPropertyKey;
        if ( aNumberFormatPropertyKey>>=nNumberFormatPropertyKey )
        {
            return nNumberFormatPropertyKey;
        }
    }
    return 0;
}

sal_Bool ScXMLExport::GetCellStyleNameIndex(const ScMyCell& aCell, sal_Int32& nStyleNameIndex, sal_Bool& bIsAutoStyle, sal_Int32& nValidationIndex)
{
    sal_Int32 nIndex = pCellStyles->GetStyleNameIndex(aCell.aCellAddress.Sheet, aCell.aCellAddress.Column, aCell.aCellAddress.Row, bIsAutoStyle, nValidationIndex);
    if (nIndex > -1)
    {
        nStyleNameIndex = nIndex;
        return sal_True;
    }
    return sal_False;
}

OUString ScXMLExport::GetPrintRanges()
{
    rtl::OUString sPrintRanges;
    uno::Reference< sheet::XPrintAreas > xPrintAreas( xCurrentTable, uno::UNO_QUERY );
    if( xPrintAreas.is() )
    {
        uno::Sequence< table::CellRangeAddress > aRangeList( xPrintAreas->getPrintAreas() );
        ScXMLConverter::GetStringFromRangeList( sPrintRanges, aRangeList, pDoc );
    }
    return sPrintRanges;
}

void ScXMLExport::WriteCell (const ScMyCell& aCell)
{
    sal_Int32 nIndex;
    sal_Bool bIsAutoStyle;
    sal_Int32 nValidationIndex;
    if (GetCellStyleNameIndex(aCell, nIndex, bIsAutoStyle, nValidationIndex))
        AddAttribute(XML_NAMESPACE_TABLE, sXML_style_name, *pCellStyles->GetStyleNameByIndex(nIndex, bIsAutoStyle));
    if (nValidationIndex > -1)
        AddAttribute(XML_NAMESPACE_TABLE, sXML_content_validation_name, pValidationsContainer->GetValidationName(nValidationIndex));
    sal_Bool bIsMatrix(aCell.bIsMatrixBase || aCell.bIsMatrixCovered);
    sal_Bool bIsFirstMatrixCell(aCell.bIsMatrixBase);
    if (bIsFirstMatrixCell)
    {
        sal_Int32 nColumns = aCell.aMatrixRange.EndColumn - aCell.aMatrixRange.StartColumn + 1;
        sal_Int32 nRows = aCell.aMatrixRange.EndRow - aCell.aMatrixRange.StartRow + 1;
        rtl::OUStringBuffer sColumns;
        rtl::OUStringBuffer sRows;
        SvXMLUnitConverter::convertNumber(sColumns, nColumns);
        SvXMLUnitConverter::convertNumber(sRows, nRows);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_matrix_columns_spanned, sColumns.makeStringAndClear());
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_matrix_rows_spanned, sRows.makeStringAndClear());
    }
    table::CellContentType xCellType = aCell.xCell->getType();
    sal_Bool bIsEmpty = sal_False;
    switch (xCellType)
    {
    case table::CellContentType_EMPTY :
        {
            bIsEmpty = sal_True;
        }
        break;
    case table::CellContentType_VALUE :
        {
            XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(
                *this, GetCellNumberFormat(aCell.xCell), aCell.xCell->getValue(), XML_NAMESPACE_TABLE);
        }
        break;
    case table::CellContentType_TEXT :
        {
            rtl::OUString sValue;
            if (GetCellText(aCell.xCell, sValue))
                XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(
                    *this, aCell.xCell->getFormula(), sValue, XML_NAMESPACE_TABLE);
        }
        break;
    case table::CellContentType_FORMULA :
        {
            String sFormula;
            ScCellObj* pCellObj = (ScCellObj*) ScCellRangesBase::getImplementation( aCell.xCell );
            if ( pCellObj )
            {
                ScBaseCell* pBaseCell = pCellObj->GetDocument()->GetCell(pCellObj->GetPosition());
                ScFormulaCell* pFormulaCell = (ScFormulaCell*) pBaseCell;
                if (pBaseCell && pBaseCell->GetCellType() == CELLTYPE_FORMULA)
                {
                    if (!bIsMatrix || (bIsMatrix && bIsFirstMatrixCell))
                    {
                        pFormulaCell->GetEnglishFormula(sFormula, sal_True);
                        rtl::OUString sOUFormula(sFormula);
                        if (!bIsMatrix)
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_formula, sOUFormula);
                        else
                        {
                            rtl::OUString sMatrixFormula = sOUFormula.copy(1, sOUFormula.getLength() - 2);
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_formula, sMatrixFormula);
                        }
                    }
                    if (pFormulaCell->IsValue())
                    {
                        sal_Bool bIsStandard = sal_True;
                        GetCellType(GetCellNumberFormat(aCell.xCell), bIsStandard);
                        if (bIsStandard)
                        {
                            uno::Reference <sheet::XSpreadsheetDocument> xSpreadDoc( xModel, uno::UNO_QUERY );
                            if ( xSpreadDoc.is() )
                            {
                                if (pDoc)
                                {
                                    pFormulaCell->GetStandardFormat(*pDoc->GetFormatTable(), 0);
                                    XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(
                                        *this, pFormulaCell->GetStandardFormat(*pDoc->GetFormatTable(), 0),
                                        aCell.xCell->getValue(), XML_NAMESPACE_TABLE);
                                }
                            }
                        }
                        else
                            XMLNumberFormatAttributesExportHelper::SetNumberFormatAttributes(*this,
                                GetCellNumberFormat(aCell.xCell), aCell.xCell->getValue(), XML_NAMESPACE_TABLE);
                    }
                    else
                    {
                        AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_value_type, sXML_string);
                        rtl::OUString sValue;
                        if (GetCellText(aCell.xCell, sValue))
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_string_value, sValue);
                    }
                }
            }
        }
        break;
    }
    rtl::OUString sCellType;
    if (aCell.bIsCovered)
        sCellType = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_covered_table_cell));
    else
    {
        sCellType = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_table_cell));
        if (aCell.bIsMergedBase)
        {
            sal_Int32 nColumns = aCell.aMergeRange.EndColumn - aCell.aMergeRange.StartColumn + 1;
            sal_Int32 nRows = aCell.aMergeRange.EndRow - aCell.aMergeRange.StartRow + 1;
            rtl::OUStringBuffer sColumns;
            rtl::OUStringBuffer sRows;
            SvXMLUnitConverter::convertNumber(sColumns, nColumns);
            SvXMLUnitConverter::convertNumber(sRows, nRows);
            AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_spanned, sColumns.makeStringAndClear());
            AddAttribute(XML_NAMESPACE_TABLE, sXML_number_rows_spanned, sRows.makeStringAndClear());
        }
    }
    SvXMLElementExport aElemC(*this, XML_NAMESPACE_TABLE, sCellType, sal_True, sal_True);
    CheckAttrList();
    WriteAreaLink(aCell);
    WriteAnnotation(aCell);
    WriteDetective(aCell);
    if (!bIsEmpty)
    {
        if (IsEditCell(aCell.xCell))
        {
            uno::Reference<text::XText> xText(aCell.xCell, uno::UNO_QUERY);
            if ( xText.is())
                GetTextParagraphExport()->exportText(xText, sal_False, sal_False);
        }
        else
        {
            SvXMLElementExport aElemC(*this, XML_NAMESPACE_TEXT, sXML_p, sal_True, sal_False);
            rtl::OUString sOUText;
            sal_Bool bPrevCharWasSpace(sal_True);
              if (GetCellText(aCell.xCell, sOUText))
                GetTextParagraphExport()->exportText(sOUText, bPrevCharWasSpace);
        }
    }
    WriteShapes(aCell);
    if (!bIsEmpty)
        if (pSharedData->nProgressValue < pSharedData->nOldProgressValue + pSharedData->nProgressObjects)
            GetProgressBarHelper()->SetValue(++pSharedData->nProgressValue);
}

void ScXMLExport::ExportShape(const uno::Reference < drawing::XShape >& xShape, awt::Point* pPoint)
{
    uno::Reference < beans::XPropertySet > xShapeProps ( xShape, uno::UNO_QUERY );
    sal_Bool bMemChart(sal_False);
    rtl::OUString sPropCLSID (RTL_CONSTASCII_USTRINGPARAM("CLSID"));
    if (xShapeProps.is())
    {
        uno::Any aAny = xShapeProps->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ZOrder")));
        sal_Int32 nZOrder;
        if (aAny >>= nZOrder)
        {
            rtl::OUStringBuffer sBuffer;
            GetMM100UnitConverter().convertNumber(sBuffer, nZOrder);
            AddAttribute(XML_NAMESPACE_DRAW, sXML_zindex, sBuffer.makeStringAndClear());
        }
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo = xShapeProps->getPropertySetInfo();
        if( xPropSetInfo->hasPropertyByName( sPropCLSID ) )
        {
            uno::Any aAny = xShapeProps->getPropertyValue( sPropCLSID );
            rtl::OUString sCLSID;
            if (aAny >>= sCLSID)
            {
                if ( sCLSID.equalsIgnoreCase(GetChartExport()->getChartCLSID()) )
                {
                    uno::Reference < container::XNamed > xNamed (xShape, uno::UNO_QUERY );
                    rtl::OUString sOUName ( xNamed->getName() );
                    String sName(sOUName);
                    SchMemChart* pMemChart = pDoc->FindChartData(sName);
                    if (pMemChart && pMemChart->GetSeriesAddresses().getLength())
                    {
                        bMemChart = sal_True;
                        GetShapeExport()->exportShape(xShape, SEF_EXPORT_NO_CHART_DATA | SEF_DEFAULT, pPoint);
                    }
                }
            }
        }
    }
    if (!bMemChart)
        GetShapeExport()->exportShape(xShape, SEF_DEFAULT, pPoint);
}

void ScXMLExport::WriteShapes(const ScMyCell& rMyCell)
{
    if( rMyCell.bHasShape && xCurrentShapes.is() && rMyCell.aShapeVec.size() && pDoc )
    {
        awt::Point aPoint;
        Rectangle aRec = pDoc->GetMMRect(static_cast<USHORT>(rMyCell.aCellAddress.Column), static_cast<USHORT>(rMyCell.aCellAddress.Row),
            static_cast<USHORT>(rMyCell.aCellAddress.Column), static_cast<USHORT>(rMyCell.aCellAddress.Row), rMyCell.aCellAddress.Sheet);
        aPoint.X = aRec.Left();
        aPoint.Y = aRec.Top();
        awt::Point* pPoint = &aPoint;
        ScMyShapeVec::const_iterator aItr = rMyCell.aShapeVec.begin();
        while (aItr != rMyCell.aShapeVec.end())
        {
            uno::Any aAny = xCurrentShapes->getByIndex(aItr->nIndex);
            uno::Reference<drawing::XShape> xShape;
            if (aAny >>= xShape)
            {
                awt::Point aEndPoint;
                Rectangle aEndRec = pDoc->GetMMRect(aItr->aEndAddress.Col(), aItr->aEndAddress.Row(),
                    aItr->aEndAddress.Col(), aItr->aEndAddress.Row(), aItr->aEndAddress.Tab());
                rtl::OUString sEndAddress;
                ScXMLConverter::GetStringFromAddress(sEndAddress, aItr->aEndAddress, pDoc);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_end_cell_address, sEndAddress);
                aEndPoint.X = aEndRec.Left();
                aEndPoint.Y = aEndRec.Top();
                awt::Point aStartPoint = xShape->getPosition();
                awt::Size aSize = xShape->getSize();
                sal_Int32 nEndX = aStartPoint.X + aSize.Width - aEndPoint.X;
                sal_Int32 nEndY = aStartPoint.Y + aSize.Height - aEndPoint.Y;
                rtl::OUStringBuffer sBuffer;
                GetMM100UnitConverter().convertMeasure(sBuffer, nEndX);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_end_x, sBuffer.makeStringAndClear());
                GetMM100UnitConverter().convertMeasure(sBuffer, nEndY);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_end_y, sBuffer.makeStringAndClear());
                uno::Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
                if( xShapeProp.is() )
                {
                    uno::Any aPropAny = xShapeProp->getPropertyValue(
                        OUString( RTL_CONSTASCII_USTRINGPARAM( SC_LAYERID ) ) );
                    sal_Int16 nLayerID;
                    if( aPropAny >>= nLayerID )
                    {
                        if( nLayerID == SC_LAYER_BACK )
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_table_background, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
                    }
                }
                ExportShape(xShape, pPoint);
            }
            aItr++;
        }
    }
}

void ScXMLExport::WriteTableShapes()
{
    ScMyTableShapes* pTableShapes = pSharedData->GetTableShapes();
    if (pTableShapes && (*pTableShapes)[nCurrentTable].size())
    {
        DBG_ASSERT(pTableShapes->size() > static_cast<sal_uInt32>(nCurrentTable), "wrong Table");
        SvXMLElementExport aShapesElem(*this, XML_NAMESPACE_TABLE, sXML_shapes, sal_True, sal_False);
        ScMyTableShapeIndexes::iterator aItr = (*pTableShapes)[nCurrentTable].begin();
        while (aItr != (*pTableShapes)[nCurrentTable].end())
        {
            uno::Any aAny = xCurrentShapes->getByIndex(*aItr);
            uno::Reference<drawing::XShape> xShape;
            if (aAny >>= xShape)
            {
                uno::Reference< beans::XPropertySet > xShapeProp( xShape, uno::UNO_QUERY );
                if( xShapeProp.is() )
                {
                    uno::Any aPropAny = xShapeProp->getPropertyValue(
                        OUString( RTL_CONSTASCII_USTRINGPARAM( SC_LAYERID ) ) );
                    sal_Int16 nLayerID;
                    if( aPropAny >>= nLayerID )
                    {
                        if( nLayerID == SC_LAYER_BACK )
                            AddAttribute(XML_NAMESPACE_TABLE, sXML_table_background, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
                    }
                }
                ExportShape(xShape, NULL);
            }
            aItr = (*pTableShapes)[nCurrentTable].erase(aItr);
        }
    }
}

void ScXMLExport::WriteAreaLink( const ScMyCell& rMyCell )
{
    if( rMyCell.bHasAreaLink )
    {
        const ScMyAreaLink& rAreaLink = rMyCell.aAreaLink;
        AddAttribute( XML_NAMESPACE_TABLE, sXML_name, rAreaLink.sSourceStr );
        AddAttribute( XML_NAMESPACE_XLINK, sXML_href, rAreaLink.sURL );
        AddAttribute( XML_NAMESPACE_TABLE, sXML_filter_name, rAreaLink.sFilter );
        if( rAreaLink.sFilterOptions.getLength() )
            AddAttribute( XML_NAMESPACE_TABLE, sXML_filter_options, rAreaLink.sFilterOptions );
        OUStringBuffer sValue;
        SvXMLUnitConverter::convertNumber( sValue, rAreaLink.GetColCount() );
        AddAttribute( XML_NAMESPACE_TABLE, sXML_last_column_spanned, sValue.makeStringAndClear() );
        SvXMLUnitConverter::convertNumber( sValue, rAreaLink.GetRowCount() );
        AddAttribute( XML_NAMESPACE_TABLE, sXML_last_row_spanned, sValue.makeStringAndClear() );
        SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE, sXML_cell_range_source, sal_True, sal_True );
    }
}

void ScXMLExport::WriteAnnotation(const ScMyCell& rMyCell)
{
    if( rMyCell.bHasAnnotation )
    {
        uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor(rMyCell.xCell, uno::UNO_QUERY);
        if (xSheetAnnotationAnchor.is())
        {
            uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation = xSheetAnnotationAnchor->getAnnotation();
            uno::Reference<text::XSimpleText> xSimpleText(xSheetAnnotation, uno::UNO_QUERY);
            if (xSheetAnnotation.is() && xSimpleText.is())
            {
                rtl::OUString sOUText = xSimpleText->getString();
                if (sOUText.getLength())
                {
                    rtl::OUString sAuthor(xSheetAnnotation->getAuthor());
                    if (sAuthor.getLength())
                        AddAttribute(XML_NAMESPACE_OFFICE, sXML_author, sAuthor);
                    String aDate(xSheetAnnotation->getDate());
                    if (pDoc)
                    {
                        SvNumberFormatter* pNumForm = pDoc->GetFormatTable();
                        double fDate;
                        sal_uInt32 nfIndex = pNumForm->GetFormatIndex(NF_DATE_SYS_DDMMYYYY, LANGUAGE_SYSTEM);
                        if (pNumForm->IsNumberFormat(aDate, nfIndex, fDate))
                        {
                            rtl::OUStringBuffer sBuf;
                            GetMM100UnitConverter().convertDateTime(sBuf, fDate);
                            AddAttribute(XML_NAMESPACE_OFFICE, sXML_create_date, sBuf.makeStringAndClear());
                        }
                        else
                            AddAttribute(XML_NAMESPACE_OFFICE, sXML_create_date_string, rtl::OUString(aDate));
                    }
                    else
                        AddAttribute(XML_NAMESPACE_OFFICE, sXML_create_date_string, rtl::OUString(aDate));
                    if (xSheetAnnotation->getIsVisible())
                        AddAttributeASCII(XML_NAMESPACE_OFFICE, sXML_display, sXML_true);
                    SvXMLElementExport aElemA(*this, XML_NAMESPACE_OFFICE, sXML_annotation, sal_True, sal_True);
                    sal_Int32 i = 0;
                    rtl::OUStringBuffer sTemp;
                    sal_Bool bPrevCharWasSpace(sal_True);
                    String sText(sOUText);
                    rtl::OUString sOUText2 (sText.ConvertLineEnd(LINEEND_LF));
                    while(i < sOUText2.getLength())
                    {
                        if (sOUText2[i] == '\n')
                        {
                            SvXMLElementExport aElemP(*this, XML_NAMESPACE_TEXT, sXML_p, sal_True, sal_False);
                            GetTextParagraphExport()->exportText(sTemp.makeStringAndClear(), bPrevCharWasSpace);
                        }
                        else
                            sTemp.append(sOUText2[i]);
                        i++;
                    }
                    if (sTemp.getLength())
                    {
                        SvXMLElementExport aElemP(*this, XML_NAMESPACE_TEXT, sXML_p, sal_True, sal_False);
                        GetTextParagraphExport()->exportText(sTemp.makeStringAndClear(), bPrevCharWasSpace);
                    }
                }
            }
            CheckAttrList();
        }
    }
}

void ScXMLExport::WriteDetective( const ScMyCell& rMyCell )
{
    if( rMyCell.bHasDetectiveObj || rMyCell.bHasDetectiveOp )
    {
        const ScMyDetectiveObjVec& rObjVec = rMyCell.aDetectiveObjVec;
        const ScMyDetectiveOpVec& rOpVec = rMyCell.aDetectiveOpVec;
        sal_Int32 nObjCount = rObjVec.size();
        sal_Int32 nOpCount = rOpVec.size();
        if( nObjCount || nOpCount )
        {
            SvXMLElementExport aDetElem( *this, XML_NAMESPACE_TABLE, sXML_detective, sal_True, sal_True );
            OUString sString;
            for( ScMyDetectiveObjVec::const_iterator aObjItr = rObjVec.begin(); aObjItr != rObjVec.end(); aObjItr++ )
            {
                if( (aObjItr->eObjType == SC_DETOBJ_ARROW) || (aObjItr->eObjType == SC_DETOBJ_TOOTHERTAB))
                {
                    ScXMLConverter::GetStringFromRange( sString, aObjItr->aSourceRange, pDoc );
                    AddAttribute( XML_NAMESPACE_TABLE, sXML_cell_range_address, sString );
                }
                ScXMLConverter::GetStringFromDetObjType( sString, aObjItr->eObjType );
                AddAttribute( XML_NAMESPACE_TABLE, sXML_direction, sString );
                if( aObjItr->bHasError )
                    AddAttributeASCII( XML_NAMESPACE_TABLE, sXML_contains_error, sXML_true );
                SvXMLElementExport aRangeElem( *this, XML_NAMESPACE_TABLE, sXML_highlighted_range, sal_True, sal_True );
            }
            OUStringBuffer aBuffer;
            for( ScMyDetectiveOpVec::const_iterator aOpItr = rOpVec.begin(); aOpItr != rOpVec.end(); aOpItr++ )
            {
                OUString sString;
                ScXMLConverter::GetStringFromDetOpType( sString, aOpItr->eOpType );
                AddAttribute( XML_NAMESPACE_TABLE, sXML_name, sString );
                SvXMLUnitConverter::convertNumber( aBuffer, aOpItr->nIndex );
                AddAttribute( XML_NAMESPACE_TABLE, sXML_index, aBuffer.makeStringAndClear() );
                SvXMLElementExport aRangeElem( *this, XML_NAMESPACE_TABLE, sXML_operation, sal_True, sal_True );
            }
        }
    }
}

void ScXMLExport::SetRepeatAttribute (const sal_Int32 nEqualCellCount)
{
    if (nEqualCellCount > 0)
    {
        sal_Int32 nTemp = nEqualCellCount + 1;
        OUString sOUEqualCellCount = OUString::valueOf(nTemp);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_number_columns_repeated, sOUEqualCellCount);
    }
}

sal_Bool ScXMLExport::IsCellTypeEqual (const ScMyCell& aCell1, const ScMyCell& aCell2) const
{
    if (!aCell1.bHasEmptyDatabase && !aCell2.bHasEmptyDatabase)
        return (aCell1.xCell->getType() == aCell2.xCell->getType());
    else
        if (aCell1.bHasEmptyDatabase == aCell2.bHasEmptyDatabase && aCell1.bHasEmptyDatabase)
            return sal_True;
        else
            if (aCell1.bHasEmptyDatabase)
                return (aCell2.xCell->getType() == table::CellContentType_EMPTY);
            else
                return (aCell1.xCell->getType() == table::CellContentType_EMPTY);
}

sal_Bool ScXMLExport::IsEditCell(const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell) const
{
    ScCellObj* pCellObj = (ScCellObj*) ScCellRangesBase::getImplementation( xCell );
    if ( pCellObj )
    {
        ScBaseCell* pBaseCell = pCellObj->GetDocument()->GetCell(pCellObj->GetPosition());
        if (pBaseCell)
        {
            if (pBaseCell->GetCellType() == CELLTYPE_EDIT)
                return sal_True;
            else
                return sal_False;
        }
    }
    return sal_True;
}

sal_Bool ScXMLExport::IsAnnotationEqual(const uno::Reference<table::XCell>& xCell1,
                                        const uno::Reference<table::XCell>& xCell2)
{
    uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor1(xCell1, uno::UNO_QUERY);
    uno::Reference<sheet::XSheetAnnotationAnchor> xSheetAnnotationAnchor2(xCell2, uno::UNO_QUERY);
    if (xSheetAnnotationAnchor1.is() && xSheetAnnotationAnchor2.is())
    {
        uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation1 = xSheetAnnotationAnchor1->getAnnotation();
        uno::Reference <sheet::XSheetAnnotation> xSheetAnnotation2 = xSheetAnnotationAnchor2->getAnnotation();
        uno::Reference<text::XSimpleText> xSimpleText1(xSheetAnnotation1, uno::UNO_QUERY);
        uno::Reference<text::XSimpleText> xSimpleText2(xSheetAnnotation2, uno::UNO_QUERY);
        if (xSheetAnnotation1.is() && xSimpleText1.is() &&
            xSheetAnnotation2.is() && xSimpleText2.is())
        {
            rtl::OUString sText1 = xSimpleText1->getString();
            rtl::OUString sText2 = xSimpleText2->getString();
            sal_Int32 nLength1 = sText1.getLength();
            sal_Int32 nLength2 = sText2.getLength();
            if (nLength1 && nLength2)
                if (sText1 == sText2 &&
                    xSheetAnnotation1->getAuthor() == xSheetAnnotation2->getAuthor() &&
                    xSheetAnnotation1->getDate() == xSheetAnnotation2->getDate() &&
                    xSheetAnnotation1->getIsVisible() == xSheetAnnotation2->getIsVisible())
                    return sal_True;
                else
                    return sal_False;
            else
                if (nLength1 || nLength2)
                    return sal_False;
                else
                    return sal_True;
        }
    }
    return sal_False;
}

sal_Bool ScXMLExport::IsCellEqual (const ScMyCell& aCell1, const ScMyCell& aCell2)
{
    sal_Bool bIsEqual = sal_False;
    if( !aCell1.bIsMergedBase && !aCell2.bIsMergedBase &&
        aCell1.bIsCovered == aCell2.bIsCovered &&
        !aCell1.bIsMatrixBase && !aCell2.bIsMatrixBase &&
        aCell1.bIsMatrixCovered == aCell2.bIsMatrixCovered &&
        aCell1.bHasAnnotation == aCell2.bHasAnnotation &&
        !aCell1.bHasShape && !aCell2.bHasShape &&
        aCell1.bHasAreaLink == aCell2.bHasAreaLink &&
        !aCell1.bHasDetectiveObj && !aCell2.bHasDetectiveObj)
    {
        if( aCell1.bHasAreaLink &&
            (aCell1.aAreaLink.GetColCount() == 1) &&
            (aCell2.aAreaLink.GetColCount() == 1) &&
            aCell1.aAreaLink.Compare( aCell2.aAreaLink ) )
        {
            if (!aCell1.bHasAnnotation || (aCell1.bHasAnnotation && IsAnnotationEqual(aCell1.xCell, aCell2.xCell)))
            {
                sal_Int32 nIndex1, nIndex2;
                sal_Bool bIsAutoStyle1, bIsAutoStyle2;
                sal_Int32 nValidationIndex1, nValidationIndex2;
                if (GetCellStyleNameIndex(aCell1, nIndex1, bIsAutoStyle1, nValidationIndex1) &&
                    GetCellStyleNameIndex(aCell2, nIndex2, bIsAutoStyle2, nValidationIndex2))
                {
                    if ((nIndex1 == nIndex2) && (bIsAutoStyle1 == bIsAutoStyle2) &&
                        (nValidationIndex1 == nValidationIndex2) &&
                        IsCellTypeEqual(aCell1, aCell2))
                    {
                        table::CellContentType eCellType = aCell1.xCell->getType();
                        switch ( eCellType )
                        {
                        case table::CellContentType_EMPTY :
                            {
                                bIsEqual = sal_True;
                            }
                            break;
                        case table::CellContentType_VALUE :
                            {
                                double fCell1 = aCell1.xCell->getValue();
                                double fCell2 = aCell2.xCell->getValue();
                                bIsEqual = (fCell1 == fCell2);
                            }
                            break;
                        case table::CellContentType_TEXT :
                            {
                                if (IsEditCell(aCell1.xCell) || IsEditCell(aCell2.xCell))
                                    bIsEqual = sal_False;
                                else
                                {
                                    OUString sOUCell1, sOUCell2;
                                    if (GetCellText(aCell1.xCell, sOUCell1) && GetCellText(aCell2.xCell, sOUCell2))
                                    {
                                        bIsEqual = (sOUCell1 == sOUCell2);
                                    }
                                    else
                                        bIsEqual = sal_False;
                                }
                            }
                            break;
                        case table::CellContentType_FORMULA :
                            {
                                bIsEqual = sal_False;
                            }
                            break;
                        default :
                            {
                                bIsEqual = sal_False;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return bIsEqual;
}

void ScXMLExport::WriteCalculationSettings(const uno::Reference <sheet::XSpreadsheetDocument>& xSpreadDoc)
{
    uno::Reference<beans::XPropertySet> xPropertySet(xSpreadDoc, uno::UNO_QUERY);
    if (xPropertySet.is())
    {
        sal_Bool bCalcAsShown = ::cppu::any2bool( xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_CALCASSHOWN))) );
        sal_Bool bIgnoreCase = ::cppu::any2bool( xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_IGNORECASE))) );
        sal_Bool bLookUpLabels = ::cppu::any2bool( xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_LOOKUPLABELS))) );
        sal_Bool bMatchWholeCell = ::cppu::any2bool( xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_MATCHWHOLE))) );
        sal_Bool bIsIterationEnabled = ::cppu::any2bool( xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_ITERENABLED))) );
        sal_uInt16 nYear2000 = pDoc->GetDocOptions().GetYear2000();
        uno::Any aAny = xPropertySet->getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_ITERCOUNT)));
        sal_Int32 nIterationCount(100);
        aAny >>= nIterationCount;
        aAny = xPropertySet->getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_ITEREPSILON)));
        double fIterationEpsilon;
        aAny >>= fIterationEpsilon;
        aAny = xPropertySet->getPropertyValue( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_NULLDATE)));
        util::Date aNullDate;
        aAny >>= aNullDate;
        if (bCalcAsShown || bIgnoreCase || !bLookUpLabels || !bMatchWholeCell || bIsIterationEnabled || nIterationCount != 100 || fIterationEpsilon != 0.001 ||
            aNullDate.Day != 30 || aNullDate.Month != 12 || aNullDate.Year != 1899 || nYear2000 != 1930)
        {
            if (bIgnoreCase)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_case_sensitive, sXML_false);
            if (bCalcAsShown)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_precision_as_shown, sXML_true);
            if (!bMatchWholeCell)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_search_criteria_must_apply_to_whole_cell, sXML_false);
            if (!bLookUpLabels)
                AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_automatic_find_labels, sXML_false);
            if (nYear2000 != 1930)
            {
                rtl::OUStringBuffer sBuffer;
                GetMM100UnitConverter().convertNumber(sBuffer, nYear2000);
                AddAttribute(XML_NAMESPACE_TABLE, sXML_null_year, sBuffer.makeStringAndClear());
            }
            SvXMLElementExport aCalcSettings(*this, XML_NAMESPACE_TABLE, sXML_calculation_settings, sal_True, sal_True);
            {
                if (aNullDate.Day != 30 || aNullDate.Month != 12 || aNullDate.Year != 1899)
                {
                    rtl::OUStringBuffer sDate;
                    GetMM100UnitConverter().convertDateTime(sDate, 0.0, aNullDate);
                    AddAttribute(XML_NAMESPACE_TABLE, sXML_date_value, sDate.makeStringAndClear());
                    SvXMLElementExport aElemNullDate(*this, XML_NAMESPACE_TABLE, sXML_null_date, sal_True, sal_True);
                }
                if (bIsIterationEnabled || nIterationCount != 100 || fIterationEpsilon != 0.001)
                {
                    rtl::OUStringBuffer sBuffer;
                    if (bIsIterationEnabled)
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_status, rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("enable")));
                    if (nIterationCount != 100)
                    {
                        GetMM100UnitConverter().convertNumber(sBuffer, nIterationCount);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_steps, sBuffer.makeStringAndClear());
                    }
                    if (fIterationEpsilon != 0.001)
                    {
                        GetMM100UnitConverter().convertDouble(sBuffer, fIterationEpsilon);
                        AddAttribute(XML_NAMESPACE_TABLE, sXML_maximum_difference, sBuffer.makeStringAndClear());
                    }
                    SvXMLElementExport aElemIteration(*this, XML_NAMESPACE_TABLE, sXML_iteration, sal_True, sal_True);
                }
            }
        }
    }
}

void ScXMLExport::WriteTableSource()
{
    uno::Reference <sheet::XSheetLinkable> xLinkable (xCurrentTable, uno::UNO_QUERY);
    if (xLinkable.is())
    {
        sheet::SheetLinkMode nMode (xLinkable->getLinkMode());
        if (nMode != sheet::SheetLinkMode_NONE)
        {
            rtl::OUString sLink (xLinkable->getLinkUrl());
            uno::Reference <beans::XPropertySet> xProps (GetModel(), uno::UNO_QUERY);
            if (xProps.is())
            {
                uno::Any aAny = xProps->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_SHEETLINKS)));
                uno::Reference <container::XIndexAccess> xIndex;
                if (aAny >>= xIndex)
                {
                    sal_Int32 nCount = xIndex->getCount();
                    if (nCount)
                    {
                        sal_Bool bFound(sal_False);
                        uno::Reference <beans::XPropertySet> xLinkProps;
                        for (sal_Int32 i = 0; (i < nCount) && !bFound; i++)
                        {
                            uno::Any aSheetLink = xIndex->getByIndex(i);
                            if (aSheetLink >>= xLinkProps)
                            {
                                aAny = xLinkProps->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_LINKURL)));
                                rtl::OUString sNewLink;
                                if (aAny >>= sNewLink)
                                    bFound = sLink.equals(sNewLink);
                            }
                        }
                        if (bFound && xLinkProps.is())
                        {
                            rtl::OUString sFilter;
                            rtl::OUString sFilterOptions;
                            rtl::OUString sTableName (xLinkable->getLinkSheetName());
                            aAny = xLinkProps->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_FILTER)));
                            aAny >>= sFilter;
                            aAny = xLinkProps->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_FILTOPT)));
                            aAny >>= sFilterOptions;
                            if (sLink.getLength())
                            {
                                AddAttribute(XML_NAMESPACE_XLINK, sXML_href, sLink);
                                if (sTableName.getLength())
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_table_name, sTableName);
                                if (sFilter.getLength())
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_filter_name, sFilter);
                                if (sFilterOptions.getLength())
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_filter_options, sFilterOptions);
                                if (nMode != sheet::SheetLinkMode_NORMAL)
                                    AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_mode, sXML_copy_results_only);
                                SvXMLElementExport aSourceElem(*this, XML_NAMESPACE_TABLE, sXML_table_source, sal_True, sal_True);
                            }
                        }
                    }
                }
            }
        }
    }
}

// core implementation
void ScXMLExport::WriteScenario()
{
    if (pDoc->IsScenario(nCurrentTable))
    {
        String      sComment;
        Color       aColor;
        sal_uInt16  nFlags;
        pDoc->GetScenarioData(nCurrentTable, sComment, aColor, nFlags);
        if (!(nFlags & SC_SCENARIO_SHOWFRAME))
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_display_border, sXML_false);
        rtl::OUStringBuffer aBuffer;
        SvXMLUnitConverter::convertColor(aBuffer, aColor);
        AddAttribute(XML_NAMESPACE_TABLE, sXML_border_color, aBuffer.makeStringAndClear());
        if (!(nFlags & SC_SCENARIO_TWOWAY))
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_copy_back, sXML_false);
        if (!(nFlags & SC_SCENARIO_ATTRIB))
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_copy_styles, sXML_false);
        if (nFlags & SC_SCENARIO_VALUE)
            AddAttributeASCII(XML_NAMESPACE_TABLE, sXML_copy_formulas, sXML_false);
        SvXMLUnitConverter::convertBool(aBuffer, pDoc->IsActiveScenario(nCurrentTable));
        AddAttribute(XML_NAMESPACE_TABLE, sXML_is_active, aBuffer.makeStringAndClear());
        const ScRangeList* pRangeList = pDoc->GetScenarioRanges(nCurrentTable);
        rtl::OUString sRangeListStr;
        ScXMLConverter::GetStringFromRangeList( sRangeListStr, pRangeList, pDoc );
        AddAttribute(XML_NAMESPACE_TABLE, sXML_scenario_ranges, sRangeListStr);
        if (sComment.Len())
            AddAttribute(XML_NAMESPACE_TABLE, sXML_comment, rtl::OUString(sComment));
        SvXMLElementExport aElem(*this, XML_NAMESPACE_TABLE, sXML_scenario, sal_True, sal_True);
    }
}

void ScXMLExport::WriteTheLabelRanges( const uno::Reference< sheet::XSpreadsheetDocument >& xSpreadDoc )
{
    uno::Reference< beans::XPropertySet > xDocProp( xSpreadDoc, uno::UNO_QUERY );
    if( !xDocProp.is() ) return;

    sal_Int32 nCount = 0;
    uno::Any aAny = xDocProp->getPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM( SC_UNO_COLLABELRNG ) ) );
    uno::Reference< sheet::XLabelRanges > xLabelRanges;
    uno::Reference< container::XIndexAccess > xColRangesIAccess;
    if( aAny >>= xLabelRanges )
        xColRangesIAccess = uno::Reference< container::XIndexAccess >( xLabelRanges, uno::UNO_QUERY );
    if( xColRangesIAccess.is() )
        nCount += xColRangesIAccess->getCount();

    aAny = xDocProp->getPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM( SC_UNO_ROWLABELRNG ) ) );
    uno::Reference< container::XIndexAccess > xRowRangesIAccess;
    if( aAny >>= xLabelRanges )
        xRowRangesIAccess = uno::Reference< container::XIndexAccess >( xLabelRanges, uno::UNO_QUERY );
    if( xRowRangesIAccess.is() )
        nCount += xRowRangesIAccess->getCount();

    if( nCount )
    {
        SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE, sXML_label_ranges, sal_True, sal_True );
        WriteLabelRanges( xColRangesIAccess, sal_True );
        WriteLabelRanges( xRowRangesIAccess, sal_False );
    }
}

void ScXMLExport::WriteLabelRanges( const uno::Reference< container::XIndexAccess >& xRangesIAccess, sal_Bool bColumn )
{
    if( !xRangesIAccess.is() ) return;

    sal_Int32 nCount = xRangesIAccess->getCount();
    for( sal_Int32 nIndex = 0; nIndex < nCount; nIndex++ )
    {
        uno::Any aRangeAny = xRangesIAccess->getByIndex( nIndex );
        uno::Reference< sheet::XLabelRange > xRange;
        if( aRangeAny >>= xRange )
        {
            OUString sRangeStr;
            table::CellRangeAddress aCellRange( xRange->getLabelArea() );
            ScXMLConverter::GetStringFromRange( sRangeStr, aCellRange, pDoc );
            AddAttribute( XML_NAMESPACE_TABLE, sXML_label_cell_range_address, sRangeStr );
            aCellRange = xRange->getDataArea();
            ScXMLConverter::GetStringFromRange( sRangeStr, aCellRange, pDoc );
            AddAttribute( XML_NAMESPACE_TABLE, sXML_data_cell_range_address, sRangeStr );
            AddAttribute( XML_NAMESPACE_TABLE, sXML_orientation, OUString::createFromAscii( bColumn ? sXML_column : sXML_row ) );
            SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE, sXML_label_range, sal_True, sal_True );
        }
    }
}

void ScXMLExport::WriteNamedExpressions(const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc)
{
    uno::Reference <beans::XPropertySet> xPropertySet (xSpreadDoc, uno::UNO_QUERY);
    if (xPropertySet.is())
    {
        uno::Any aNamedRanges = xPropertySet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_NAMEDRANGES)));
        uno::Reference <sheet::XNamedRanges> xNamedRanges;
        CheckAttrList();
        if (aNamedRanges >>= xNamedRanges)
        {
            uno::Sequence <rtl::OUString> aRangesNames = xNamedRanges->getElementNames();
            sal_Int32 nNamedRangesCount = aRangesNames.getLength();
            if (nNamedRangesCount > 0)
            {
                if (pDoc)
                {
                    ScRangeName* pNamedRanges = pDoc->GetRangeName();
                    SvXMLElementExport aElemNEs(*this, XML_NAMESPACE_TABLE, sXML_named_expressions, sal_True, sal_True);
                    for (sal_Int32 i = 0; i < nNamedRangesCount; i++)
                    {
                        CheckAttrList();
                        rtl::OUString sNamedRange = aRangesNames[i];
                        uno::Any aNamedRange = xNamedRanges->getByName(sNamedRange);
                        uno::Reference <sheet::XNamedRange> xNamedRange;
                        if (aNamedRange >>= xNamedRange)
                        {
                            uno::Reference <container::XNamed> xNamed (xNamedRange, uno::UNO_QUERY);
                            uno::Reference <sheet::XCellRangeReferrer> xCellRangeReferrer (xNamedRange, uno::UNO_QUERY);
                            if (xNamed.is() && xCellRangeReferrer.is())
                            {
                                rtl::OUString sOUName = xNamed->getName();
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_name, sOUName);

                                OUString sOUBaseCellAddress;
                                ScXMLConverter::GetStringFromAddress( sOUBaseCellAddress,
                                    xNamedRange->getReferencePosition(), pDoc, sal_False, SCA_ABS_3D );
                                AddAttribute(XML_NAMESPACE_TABLE, sXML_base_cell_address, sOUBaseCellAddress);

                                sal_uInt16 nRangeIndex;
                                String sName(sOUName);
                                pNamedRanges->SearchName(sName, nRangeIndex);
                                ScRangeData* pNamedRange = (*pNamedRanges)[nRangeIndex]; //should get directly and not with ScDocument
                                String sContent(xNamedRange->getContent());
                                pNamedRange->GetEnglishSymbol(sContent, sal_True);
                                rtl::OUString sOUTempContent(sContent);
                                uno::Reference <table::XCellRange> xCellRange = xCellRangeReferrer->getReferredCells();
                                if(xCellRange.is())
                                {
                                    rtl::OUString sOUContent = sOUTempContent.copy(1, sOUTempContent.getLength() - 2);
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_cell_range_address, sOUContent);
                                    sal_Int32 nRangeType = xNamedRange->getType();
                                    rtl::OUStringBuffer sBufferRangeType;
                                    if ((nRangeType & sheet::NamedRangeFlag::COLUMN_HEADER) == sheet::NamedRangeFlag::COLUMN_HEADER)
                                        sBufferRangeType.appendAscii(sXML_repeat_column);
                                    if ((nRangeType & sheet::NamedRangeFlag::ROW_HEADER) == sheet::NamedRangeFlag::ROW_HEADER)
                                    {
                                        if (sBufferRangeType.getLength() > 0)
                                            sBufferRangeType.appendAscii(" ");
                                        sBufferRangeType.appendAscii(sXML_repeat_row);
                                    }
                                    if ((nRangeType & sheet::NamedRangeFlag::FILTER_CRITERIA) == sheet::NamedRangeFlag::FILTER_CRITERIA)
                                    {
                                        if (sBufferRangeType.getLength() > 0)
                                            sBufferRangeType.appendAscii(" ");
                                        sBufferRangeType.appendAscii(sXML_filter);
                                    }
                                    if ((nRangeType & sheet::NamedRangeFlag::PRINT_AREA) == sheet::NamedRangeFlag::PRINT_AREA)
                                    {
                                        if (sBufferRangeType.getLength() > 0)
                                            sBufferRangeType.appendAscii(" ");
                                        sBufferRangeType.appendAscii(sXML_print_range);
                                    }
                                    rtl::OUString sRangeType = sBufferRangeType.makeStringAndClear();
                                    if (sRangeType.getLength())
                                        AddAttribute(XML_NAMESPACE_TABLE, sXML_range_usable_as, sRangeType);
                                    SvXMLElementExport aElemNR(*this, XML_NAMESPACE_TABLE, sXML_named_range, sal_True, sal_True);
                                }
                                else
                                {
                                    AddAttribute(XML_NAMESPACE_TABLE, sXML_expression, sOUTempContent);
                                    SvXMLElementExport aElemNE(*this, XML_NAMESPACE_TABLE, sXML_named_expression, sal_True, sal_True);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// core implementation
void ScXMLExport::WriteConsolidation()
{
    const ScConsolidateParam* pCons = pDoc->GetConsolidateDlgData();
    if( pCons )
    {
        OUString sStrData;

        ScXMLConverter::GetStringFromFunction( sStrData, pCons->eFunction );
        AddAttribute( XML_NAMESPACE_TABLE, sXML_function, sStrData );

        sStrData = OUString();
        for( sal_Int32 nIndex = 0; nIndex < pCons->nDataAreaCount; nIndex++ )
            ScXMLConverter::GetStringFromArea( sStrData, *pCons->ppDataAreas[ nIndex ], pDoc, sal_True );
        AddAttribute( XML_NAMESPACE_TABLE, sXML_source_cell_range_addresses, sStrData );

        ScXMLConverter::GetStringFromAddress( sStrData, ScAddress( pCons->nCol, pCons->nRow, pCons->nTab ), pDoc );
        AddAttribute( XML_NAMESPACE_TABLE, sXML_target_cell_address, sStrData );

        if( pCons->bByCol && !pCons->bByRow )
            AddAttributeASCII( XML_NAMESPACE_TABLE, sXML_use_label, sXML_column );
        else if( !pCons->bByCol && pCons->bByRow )
            AddAttributeASCII( XML_NAMESPACE_TABLE, sXML_use_label, sXML_row );
        else if( pCons->bByCol && pCons->bByRow )
            AddAttributeASCII( XML_NAMESPACE_TABLE, sXML_use_label, sXML_both );

        if( pCons->bReferenceData )
            AddAttributeASCII( XML_NAMESPACE_TABLE, sXML_link_to_source_data, sXML_true );

        SvXMLElementExport aElem( *this, XML_NAMESPACE_TABLE, sXML_consolidation, sal_True, sal_True );
    }
}

SvXMLAutoStylePoolP* ScXMLExport::CreateAutoStylePool()
{
    return new ScXMLAutoStylePoolP(*this);
}

XMLPageExport* ScXMLExport::CreatePageExport()
{
    return new XMLTableMasterPageExport( *this );
}

XMLShapeExport* ScXMLExport::CreateShapeExport()
{
/*  UniReference < XMLPropertySetMapper > xShapeStylesPropertySetMapper = new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLScShapeStylesProperties, xScPropHdlFactory);
    SvXMLExportPropertyMapper *pShapeStylesExportPropertySetMapper = new SvXMLExportPropertyMapper(xShapeStylesPropertySetMapper);*/
    return new XMLShapeExport(*this/*, pShapeStylesExportPropertySetMapper*/);
}

void ScXMLExport::CreateSharedData(const sal_Int32 nTableCount)
{
    pSharedData = new ScMySharedData(nTableCount);
}

