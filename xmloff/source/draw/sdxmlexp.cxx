/*************************************************************************
 *
 *  $RCSfile: sdxmlexp.cxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: mib $ $Date: 2000-11-29 14:31:35 $
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

#pragma hdrstop

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

#ifndef _XMLOFF_XMLKYWD_HXX
#include "xmlkywd.hxx"
#endif

#ifndef _LANG_HXX
#include <tools/lang.hxx>
#endif

#ifndef _XMLOFF_XMLMETAE_HXX
#include "xmlmetae.hxx"
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFOSUPPLIER_HPP_
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORSUPPLIER_HPP_
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _SDXMLEXP_HXX
#include "sdxmlexp.hxx"
#endif

#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_VIEW_PAPERORIENTATION_HPP_
#include <com/sun/star/view/PaperOrientation.hpp>
#endif

#ifndef _COM_SUN_STAR_STYLE_XSTYLEFAMILIESSUPPLIER_HPP_
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_STYLE_XSTYLE_HPP_
#include <com/sun/star/style/XStyle.hpp>
#endif

#ifndef _COM_SUN_STAR_PRESENTATION_XPRESENTATIONPAGE_HPP_
#include <com/sun/star/presentation/XPresentationPage.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGETARGET_HPP_
#include <com/sun/star/drawing/XMasterPageTarget.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_CONNECTORTYPE_HPP_
#include <com/sun/star/drawing/ConnectorType.hpp>
#endif

#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_
#include <com/sun/star/text/XText.hpp>
#endif

#ifndef _COM_SUN_STAR_CHART_XCHARTDOCUMENT_HPP_
#include <com/sun/star/chart/XChartDocument.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLASTPLP_HXX
#include "xmlaustp.hxx"
#endif

#ifndef _XMLOFF_FAMILIES_HXX_
#include "families.hxx"
#endif

#ifndef _XMLOFF_STYLEEXP_HXX_
#include "styleexp.hxx"
#endif

#ifndef _SDPROPLS_HXX
#include "sdpropls.hxx"
#endif

#ifndef _XMLOFF_XMLEXPPR_HXX
#include "xmlexppr.hxx"
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif

#ifndef _XEXPTRANSFORM_HXX
#include "xexptran.hxx"
#endif

#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONBEZIERCOORDS_HPP_
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#endif

#ifndef _ISOLANG_HXX
#include <tools/isolang.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _CPPUHELPER_EXTRACT_HXX_
#include <cppuhelper/extract.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _B3D_HMATRIX_HXX
#include <goodies/hmatrix.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_HOMOGENMATRIX_HPP_
#include <com/sun/star/drawing/HomogenMatrix.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONSHAPE3D_HPP_
#include <com/sun/star/drawing/PolyPolygonShape3D.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_DOUBLESEQUENCE_HPP_
#include <com/sun/star/drawing/DoubleSequence.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_PROJECTIONMODE_HPP_
#include <com/sun/star/drawing/ProjectionMode.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_SHADEMODE_HPP_
#include <com/sun/star/drawing/ShadeMode.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_DIRECTION3D_HPP_
#include <com/sun/star/drawing/Direction3D.hpp>
#endif

#ifndef _XMLOFF_PROPERTYSETMERGER_HXX_
#include "PropertySetMerger.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star;

//////////////////////////////////////////////////////////////////////////////

inline sal_Int32 FRound( double fVal )
{
    return( fVal > 0.0 ? (sal_Int32) ( fVal + 0.5 ) : -(sal_Int32) ( -fVal + 0.5 ) );
}

//////////////////////////////////////////////////////////////////////////////
// wrapper to have control for virtual function calls to handle all kinds
// of possible item specialities

class ImpPresPageDrawStylePropMapper : public SvXMLExportPropertyMapper
{
    /** this method is called for every item that has the MID_FLAG_NO_ITEM_EXPORT flag set */
    virtual void handleNoItem(
        SvXMLAttributeList& rAttrList, const XMLPropertyState& rProperty,
        const SvXMLUnitConverter& rUnitConverter, const SvXMLNamespaceMap& rNamespaceMap,
        const ::std::vector< XMLPropertyState > *pProperties = 0,
        sal_uInt32 nIdx = 0 ) const;

    /** this method is called for every item that has the MID_FLAG_ELEMENT_EXPORT flag set */
    virtual void handleElementItem(
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XDocumentHandler > & rHandler,
        const XMLPropertyState& rProperty, const SvXMLUnitConverter& rUnitConverter,
        const SvXMLNamespaceMap& rNamespaceMap, sal_uInt16 nFlags,
        const ::std::vector< XMLPropertyState > *pProperties = 0,
        sal_uInt32 nIdx = 0 ) const;

    /** this method is called for every item that has the MID_FLAG_SPECIAL_ITEM_EXPORT flag set */
    virtual void handleSpecialItem(
        SvXMLAttributeList& rAttrList, const XMLPropertyState& rProperty,
        const SvXMLUnitConverter& rUnitConverter, const SvXMLNamespaceMap& rNamespaceMap,
        const ::std::vector< XMLPropertyState > *pProperties = 0,
        sal_uInt32 nIdx = 0 ) const;

public:
    ImpPresPageDrawStylePropMapper( const UniReference< XMLPropertySetMapper >& rMapper );
    virtual ~ImpPresPageDrawStylePropMapper();
};

ImpPresPageDrawStylePropMapper::ImpPresPageDrawStylePropMapper(
    const UniReference< XMLPropertySetMapper >& rMapper )
:   SvXMLExportPropertyMapper( rMapper )
{
}

ImpPresPageDrawStylePropMapper::~ImpPresPageDrawStylePropMapper()
{
}

void ImpPresPageDrawStylePropMapper::handleNoItem(
    SvXMLAttributeList& rAttrList, const XMLPropertyState& rProperty,
    const SvXMLUnitConverter& rUnitConverter, const SvXMLNamespaceMap& rNamespaceMap,
    const ::std::vector< XMLPropertyState > *pProperties,
    sal_uInt32 nIdx ) const
{
    // call parent
    SvXMLExportPropertyMapper::handleNoItem(rAttrList, rProperty, rUnitConverter, rNamespaceMap, pProperties, nIdx );
}

void ImpPresPageDrawStylePropMapper::handleElementItem(
    const com::sun::star::uno::Reference< com::sun::star::xml::sax::XDocumentHandler > & rHandler,
    const XMLPropertyState& rProperty, const SvXMLUnitConverter& rUnitConverter,
    const SvXMLNamespaceMap& rNamespaceMap, sal_uInt16 nFlags,
    const ::std::vector< XMLPropertyState > *pProperties,
    sal_uInt32 nIdx) const
{
    // call parent
    SvXMLExportPropertyMapper::handleElementItem(rHandler, rProperty, rUnitConverter, rNamespaceMap, nFlags, pProperties, nIdx );
}

void ImpPresPageDrawStylePropMapper::handleSpecialItem(
    SvXMLAttributeList& rAttrList, const XMLPropertyState& rProperty,
    const SvXMLUnitConverter& rUnitConverter, const SvXMLNamespaceMap& rNamespaceMap,
    const ::std::vector< XMLPropertyState > *pProperties,
    sal_uInt32 nIdx ) const
{
    // call parent
    SvXMLExportPropertyMapper::handleSpecialItem(rAttrList, rProperty, rUnitConverter, rNamespaceMap, pProperties, nIdx );
}

//////////////////////////////////////////////////////////////////////////////

class ImpXMLShapeStyleInfo
{
    OUString                            msStyleName;
    sal_Int32                           mnFamily;

public:
    ImpXMLShapeStyleInfo(
        const OUString& rStyStr,
        sal_Int32 nFam);

    const OUString& GetStyleName() const { return msStyleName; }
    sal_Int32 GetFamily() const { return mnFamily; }
};

ImpXMLShapeStyleInfo::ImpXMLShapeStyleInfo(
    const OUString& rStyStr,
    sal_Int32 nFam)
:   msStyleName(rStyStr),
    mnFamily(nFam)
{
}

DECLARE_LIST(ImpXMLShapeStyleInfoList, ImpXMLShapeStyleInfo*);

//////////////////////////////////////////////////////////////////////////////

class ImpXMLDrawPageInfo
{
    OUString                        msStyleName;
    OUString                        msPageLayoutName;

public:
    ImpXMLDrawPageInfo(const OUString& rStyStr);

    void SetPageLayoutName(const OUString& rStr);

    const OUString& GetStyleName() const { return msStyleName; }
    const OUString& GetPageLayoutName() const { return msPageLayoutName; }
};

ImpXMLDrawPageInfo::ImpXMLDrawPageInfo(const OUString& rStyStr)
:   msStyleName(rStyStr)
{
}

void ImpXMLDrawPageInfo::SetPageLayoutName(const OUString& rStr)
{
    msPageLayoutName = rStr;
}

DECLARE_LIST(ImpXMLDrawPageInfoList, ImpXMLDrawPageInfo*);

//////////////////////////////////////////////////////////////////////////////

class ImpXMLEXPPageMasterInfo
{
    sal_Int32                   mnBorderBottom;
    sal_Int32                   mnBorderLeft;
    sal_Int32                   mnBorderRight;
    sal_Int32                   mnBorderTop;
    sal_Int32                   mnWidth;
    sal_Int32                   mnHeight;
    view::PaperOrientation      meOrientation;
    OUString                    msName;
    OUString                    msMasterPageName;

public:
    ImpXMLEXPPageMasterInfo(const SdXMLExport& rExp, const uno::Reference<drawing::XDrawPage>& xPage);
    BOOL operator==(const ImpXMLEXPPageMasterInfo& rInfo) const;
    void SetName(const OUString& rStr);

    const OUString& GetName() const { return msName; }
    const OUString& GetMasterPageName() const { return msMasterPageName; }

    sal_Int32 GetBorderBottom() const { return mnBorderBottom; }
    sal_Int32 GetBorderLeft() const { return mnBorderLeft; }
    sal_Int32 GetBorderRight() const { return mnBorderRight; }
    sal_Int32 GetBorderTop() const { return mnBorderTop; }
    sal_Int32 GetWidth() const { return mnWidth; }
    sal_Int32 GetHeight() const { return mnHeight; }
    view::PaperOrientation GetOrientation() const { return meOrientation; }
};

ImpXMLEXPPageMasterInfo::ImpXMLEXPPageMasterInfo(
    const SdXMLExport& rExp,
    const uno::Reference<drawing::XDrawPage>& xPage)
:   mnBorderBottom(0),
    mnBorderLeft(0),
    mnBorderRight(0),
    mnBorderTop(0),
    mnWidth(0),
    mnHeight(0),
    meOrientation(rExp.IsDraw() ? view::PaperOrientation_PORTRAIT : view::PaperOrientation_LANDSCAPE)
{
    uno::Reference <beans::XPropertySet> xPropSet(xPage, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        uno::Any aAny;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("BorderBottom")));
        aAny >>= mnBorderBottom;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("BorderLeft")));
        aAny >>= mnBorderLeft;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("BorderRight")));
        aAny >>= mnBorderRight;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("BorderTop")));
        aAny >>= mnBorderTop;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("Width")));
        aAny >>= mnWidth;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("Height")));
        aAny >>= mnHeight;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("Orientation")));
        aAny >>= meOrientation;
    }

    uno::Reference <container::XNamed> xMasterNamed(xPage, uno::UNO_QUERY);
    if(xMasterNamed.is())
    {
        msMasterPageName = xMasterNamed->getName();
    }
}

BOOL ImpXMLEXPPageMasterInfo::operator==(const ImpXMLEXPPageMasterInfo& rInfo) const
{
    return ((mnBorderBottom == rInfo.mnBorderBottom)
        && (mnBorderLeft == rInfo.mnBorderLeft)
        && (mnBorderRight == rInfo.mnBorderRight)
        && (mnBorderTop == rInfo.mnBorderTop)
        && (mnWidth == rInfo.mnWidth)
        && (mnHeight == rInfo.mnHeight)
        && (meOrientation == rInfo.meOrientation));
}

void ImpXMLEXPPageMasterInfo::SetName(const OUString& rStr)
{
    msName = rStr;
}

DECLARE_LIST(ImpXMLEXPPageMasterList, ImpXMLEXPPageMasterInfo*);

//////////////////////////////////////////////////////////////////////////////

#define IMP_AUTOLAYOUT_INFO_MAX         (27L)

class ImpXMLAutoLayoutInfo
{
    sal_uInt16                  mnType;
    ImpXMLEXPPageMasterInfo*    mpPageMasterInfo;
    OUString                    msLayoutName;
    Rectangle                   maTitleRect;
    Rectangle                   maPresRect;
    sal_Int32                   mnGapX;
    sal_Int32                   mnGapY;

public:
    ImpXMLAutoLayoutInfo(sal_uInt16 nTyp, ImpXMLEXPPageMasterInfo* pInf);

    BOOL operator==(const ImpXMLAutoLayoutInfo& rInfo) const;

    sal_uInt16 GetLayoutType() const { return mnType; }
    sal_Int32 GetGapX() const { return mnGapX; }
    sal_Int32 GetGapY() const { return mnGapY; }

    const OUString& GetLayoutName() const { return msLayoutName; }
    void SetLayoutName(const OUString& rNew) { msLayoutName = rNew; }

    const Rectangle& GetTitleRectangle() const { return maTitleRect; }
    const Rectangle& GetPresRectangle() const { return maPresRect; }

    static BOOL IsCreateNecessary(sal_uInt16 nTyp);
};

BOOL ImpXMLAutoLayoutInfo::IsCreateNecessary(sal_uInt16 nTyp)
{
    if(nTyp == 5 /* AUTOLAYOUT_ORG */
        || nTyp == 20 /* AUTOLAYOUT_NONE */
        || nTyp >= IMP_AUTOLAYOUT_INFO_MAX)
        return FALSE;
    return TRUE;
}

BOOL ImpXMLAutoLayoutInfo::operator==(const ImpXMLAutoLayoutInfo& rInfo) const
{
    return ((mnType == rInfo.mnType
        && mpPageMasterInfo == rInfo.mpPageMasterInfo));
}

ImpXMLAutoLayoutInfo::ImpXMLAutoLayoutInfo(sal_uInt16 nTyp, ImpXMLEXPPageMasterInfo* pInf)
:   mnType(nTyp),
    mpPageMasterInfo(pInf)
{
    // create full info (initialze with typical values)
    Point aPagePos(0,0);
    Size aPageSize(28000, 21000);
    Size aPageInnerSize(28000, 21000);

    if(mpPageMasterInfo)
    {
        aPagePos = Point(mpPageMasterInfo->GetBorderLeft(), mpPageMasterInfo->GetBorderTop());
        aPageSize = Size(mpPageMasterInfo->GetWidth(), mpPageMasterInfo->GetHeight());
        aPageInnerSize = aPageSize;
        aPageInnerSize.Width() -= mpPageMasterInfo->GetBorderLeft() + mpPageMasterInfo->GetBorderRight();
        aPageInnerSize.Height() -= mpPageMasterInfo->GetBorderTop() + mpPageMasterInfo->GetBorderBottom();
    }

    // title rectangle aligning
    Point aTitlePos(aPagePos);
    Size aTitleSize(aPageInnerSize);

    if(mnType == 21 /* AUTOLAYOUT_NOTES */)
    {
        aTitleSize.Height() = (long) (aTitleSize.Height() / 2.5);
        Point aPos = aTitlePos;
        aPos.Y() += long( aTitleSize.Height() * 0.083 );
        Size aPartArea = aTitleSize;
        Size aSize;

        // tatsaechliche Seitengroesse in das Handout-Rechteck skalieren
        double fH = (double) aPartArea.Width()  / aPageSize.Width();
        double fV = (double) aPartArea.Height() / aPageSize.Height();

        if ( fH > fV )
            fH = fV;
        aSize.Width()  = (long) (fH * aPageSize.Width());
        aSize.Height() = (long) (fH * aPageSize.Height());

        aPos.X() += (aPartArea.Width() - aSize.Width()) / 2;
        aPos.Y() += (aPartArea.Height()- aSize.Height())/ 2;

        aTitlePos = aPos;
        aTitleSize = aSize;
    }
    else
    {
        aTitlePos.X() += long( aTitleSize.Width() * 0.0735 );
        aTitlePos.Y() += long( aTitleSize.Height() * 0.083 );
        aTitleSize.Width() = long( aTitleSize.Width() * 0.854 );
        aTitleSize.Height() = long( aTitleSize.Height() * 0.167 );
    }

    maTitleRect.SetPos(aTitlePos);
    maTitleRect.SetSize(aTitleSize);

    // layout rectangle aligning
    Point aLayoutPos(aPagePos);
    Size aLayoutSize(aPageInnerSize);

    if(mnType == 21 /* AUTOLAYOUT_NOTES */)
    {
        aLayoutPos.X() += long( aLayoutSize.Width() * 0.0735 );
        aLayoutPos.Y() += long( aLayoutSize.Height() * 0.472 );
        aLayoutSize.Width() = long( aLayoutSize.Width() * 0.854 );
        aLayoutSize.Height() = long( aLayoutSize.Height() * 0.444 );
    }
    else if(mnType >= 22 && mnType <= 26) // AUTOLAYOUT_HANDOUT
    {
        // keep info for inner area in maPresRect, put info for gap size
        // to maTitleRect position
        mnGapX = (aPageSize.Width() - aPageInnerSize.Width()) / 2;
        mnGapY = (aPageSize.Height() - aPageInnerSize.Height()) / 2;

        if(!mnGapX)
            mnGapX = aPageSize.Width() / 10;

        if(!mnGapY)
            mnGapY = aPageSize.Height() / 10;

        if(mnGapX < aPageInnerSize.Width() / 10)
            mnGapX = aPageInnerSize.Width() / 10;

        if(mnGapY < aPageInnerSize.Height() / 10)
            mnGapY = aPageInnerSize.Height() / 10;
    }
    else
    {
        aLayoutPos.X() += long( aLayoutSize.Width() * 0.0735 );
        aLayoutPos.Y() += long( aLayoutSize.Height() * 0.278 );
        aLayoutSize.Width() = long( aLayoutSize.Width() * 0.854 );
        aLayoutSize.Height() = long( aLayoutSize.Height() * 0.630 );
    }

    maPresRect.SetPos(aLayoutPos);
    maPresRect.SetSize(aLayoutSize);
}

DECLARE_LIST(ImpXMLAutoLayoutInfoList, ImpXMLAutoLayoutInfo*);

//////////////////////////////////////////////////////////////////////////////

SdXMLExport::SdXMLExport(
    const uno::Reference<frame::XModel>& rMod,
    const OUString& rFileName,
    const uno::Reference<xml::sax::XDocumentHandler>& rHandler,
    const uno::Reference< ::com::sun::star::container::XIndexContainer > & rEGO,
    BOOL bShowProgr,
    BOOL bIsDraw)
:   SvXMLExport( rFileName, rHandler, rMod, rEGO, MAP_CM ),
    mpPageMasterInfoList(new ImpXMLEXPPageMasterList(1, 4, 4)),
    mpPageMaterUsageList(new ImpXMLEXPPageMasterList(1, 4, 4)),
    mpDrawPageInfoList(new ImpXMLDrawPageInfoList(4, 8, 8)),
    mpShapeStyleInfoList(new ImpXMLShapeStyleInfoList(16, 64, 64)),
    mpAutoLayoutInfoList(new ImpXMLAutoLayoutInfoList(1, 4, 4)),
    mpSdPropHdlFactory(0L),
    mpPropertySetMapper(0L),
    mpPresPagePropsMapper(0L),
    mnDocMasterPageCount(0L),
    mnDocDrawPageCount(0L),
    mnShapeStyleInfoIndex(0L),
    mbIsDraw(bIsDraw),
    mbFamilyGraphicUsed(FALSE),
    mbFamilyPresentationUsed(FALSE),
    msZIndex( RTL_CONSTASCII_USTRINGPARAM(sXML_zindex) ),
    msEmptyPres( RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject") ),
    msModel( RTL_CONSTASCII_USTRINGPARAM("Model") ),
    msStartShape( RTL_CONSTASCII_USTRINGPARAM("StartShape") ),
    msEndShape( RTL_CONSTASCII_USTRINGPARAM("EndShape") )
{
    // prepare factory parts
    mpSdPropHdlFactory = new XMLSdPropHdlFactory;
    if(mpSdPropHdlFactory)
    {
        // set lock to avoid deletion
        mpSdPropHdlFactory->acquire();

        // build one ref
        const UniReference< XMLPropertyHandlerFactory > aFactoryRef = mpSdPropHdlFactory;

        // construct PropertySetMapper
        UniReference < XMLPropertySetMapper > xMapper =
            new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLSDProperties, aFactoryRef);
        mpPropertySetMapper = new ImpPresPageDrawStylePropMapper( xMapper );
        if(mpPropertySetMapper)
        {
            // set lock to avoid deletion
            mpPropertySetMapper->acquire();
        }

        // construct PresPagePropsMapper
        xMapper =
            new XMLPropertySetMapper((XMLPropertyMapEntry*)aXMLSDPresPageProps, aFactoryRef);

        mpPresPagePropsMapper = new ImpPresPageDrawStylePropMapper( xMapper );
        if(mpPresPagePropsMapper)
        {
            // set lock to avoid deletion
            mpPresPagePropsMapper->acquire();
        }
    }

    // add family name
      GetAutoStylePool()->AddFamily(
        XML_STYLE_FAMILY_SD_GRAPHICS_ID,
        OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_GRAPHICS_NAME)),
          GetPropertySetMapper(),
          OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_GRAPHICS_PREFIX)));
    GetAutoStylePool()->AddFamily(
        XML_STYLE_FAMILY_SD_PRESENTATION_ID,
        OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_PRESENTATION_NAME)),
          GetPropertySetMapper(),
          OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_PRESENTATION_PREFIX)));

    // prepare access to styles
    uno::Reference< style::XStyleFamiliesSupplier > xFamSup( GetModel(), uno::UNO_QUERY );
    if(xFamSup.is())
    {
        mxDocStyleFamilies = xFamSup->getStyleFamilies();
    }

    // prepare access to master pages
    uno::Reference < drawing::XMasterPagesSupplier > xMasterPagesSupplier(GetModel(), uno::UNO_QUERY);
    if(xMasterPagesSupplier.is())
    {
        mxDocMasterPages = mxDocMasterPages.query( xMasterPagesSupplier->getMasterPages() );
        if(mxDocMasterPages.is())
        {
            mnDocMasterPageCount = mxDocMasterPages->getCount();
        }
    }

    // prepare access to draw pages
    uno::Reference <drawing::XDrawPagesSupplier> xDrawPagesSupplier(GetModel(), uno::UNO_QUERY);
    if(xDrawPagesSupplier.is())
    {
        mxDocDrawPages = mxDocDrawPages.query( xDrawPagesSupplier->getDrawPages() );
        if(mxDocDrawPages.is())
        {
            mnDocDrawPageCount = mxDocDrawPages->getCount();
        }
    }

    // add namespaces
    _GetNamespaceMap().AddAtIndex(
        XML_NAMESPACE_PRESENTATION, sXML_np_presentation, sXML_n_presentation, XML_NAMESPACE_PRESENTATION);

    // get status indicator (if intended)
    if(bShowProgr)
    {
        uno::Reference<frame::XController> xController(rMod->getCurrentController());
        if(xController.is())
        {
            uno::Reference<frame::XFrame> xFrame(xController->getFrame());
            if(xFrame.is())
            {
                uno::Reference<task::XStatusIndicatorSupplier> xFactory(xFrame, uno::UNO_QUERY);
                if(xFactory.is())
                {
                    mxStatusIndicator = xFactory->getStatusIndicator();
                }
            }
        }
    }

    // add progress view
    if(mxStatusIndicator.is())
    {
        const OUString aText(RTL_CONSTASCII_USTRINGPARAM("XML Export"));
        mxStatusIndicator->start(aText, 100);
    }
}

//////////////////////////////////////////////////////////////////////////////

__EXPORT SdXMLExport::~SdXMLExport()
{
    // cleanup factory, decrease refcount. Should lead to destruction.
    if(mpSdPropHdlFactory)
    {
        mpSdPropHdlFactory->release();
        mpSdPropHdlFactory = 0L;
    }

    // cleanup mapper, decrease refcount. Should lead to destruction.
    if(mpPropertySetMapper)
    {
        mpPropertySetMapper->release();
        mpPropertySetMapper = 0L;
    }

    // cleanup presPage mapper, decrease refcount. Should lead to destruction.
    if(mpPresPagePropsMapper)
    {
        mpPresPagePropsMapper->release();
        mpPresPagePropsMapper = 0L;
    }

    // clear evtl. temporary page master infos
    if(mpPageMasterInfoList)
    {
        while(mpPageMasterInfoList->Count())
            delete mpPageMasterInfoList->Remove(mpPageMasterInfoList->Count() - 1L);
        delete mpPageMasterInfoList;
        mpPageMasterInfoList = 0L;
    }
    if(mpPageMaterUsageList)
    {
        delete mpPageMaterUsageList;
        mpPageMaterUsageList = 0L;
    }

    // clear draw style infos
    if(mpDrawPageInfoList)
    {
        while(mpDrawPageInfoList->Count())
            delete mpDrawPageInfoList->Remove(mpDrawPageInfoList->Count() - 1L);
        delete mpDrawPageInfoList;
        mpDrawPageInfoList = 0L;
    }

    // clear shape style infos
    if(mpShapeStyleInfoList)
    {
        while(mpShapeStyleInfoList->Count())
            delete mpShapeStyleInfoList->Remove(mpShapeStyleInfoList->Count() - 1L);
        delete mpShapeStyleInfoList;
        mpShapeStyleInfoList = 0L;
    }

    // clear auto-layout infos
    if(mpAutoLayoutInfoList)
    {
        while(mpAutoLayoutInfoList->Count())
            delete mpAutoLayoutInfoList->Remove(mpAutoLayoutInfoList->Count() - 1L);
        delete mpAutoLayoutInfoList;
        mpAutoLayoutInfoList = 0L;
    }

    // stop progress view
    if(mxStatusIndicator.is())
    {
        mxStatusIndicator->end();
        mxStatusIndicator->reset();
    }
}

//////////////////////////////////////////////////////////////////////////////
// to get default values in XPropertySet use this wrapper class

class ImpDefaultMapper : public ::cppu::WeakAggImplHelper1< beans::XPropertySet >
{
    uno::Reference< beans::XPropertyState >     mxState;
    uno::Reference< beans::XPropertySet >       mxSet;

public:
    ImpDefaultMapper( uno::Reference< beans::XPropertyState >& rxState );

    // Methods
    virtual uno::Reference< beans::XPropertySetInfo > SAL_CALL getPropertySetInfo() throw(uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue ) throw(beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException);
    virtual uno::Any SAL_CALL getPropertyValue( const OUString& PropertyName ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException);

    // empty implementations
    virtual void SAL_CALL addPropertyChangeListener( const OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& xListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& aListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException);
};

ImpDefaultMapper::ImpDefaultMapper( uno::Reference< beans::XPropertyState >& rxState )
:   mxState( rxState ),
    mxSet( rxState, uno::UNO_QUERY )
{
}

uno::Reference< beans::XPropertySetInfo > SAL_CALL ImpDefaultMapper::getPropertySetInfo() throw(uno::RuntimeException)
{
    return mxSet->getPropertySetInfo();
}

void SAL_CALL ImpDefaultMapper::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue ) throw(beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    mxState->setPropertyToDefault( aPropertyName /*, aValue */ );
}

uno::Any SAL_CALL ImpDefaultMapper::getPropertyValue( const OUString& PropertyName ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    return mxState->getPropertyDefault(  PropertyName  );
}

// empty implementations
void SAL_CALL ImpDefaultMapper::addPropertyChangeListener( const OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& xListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL ImpDefaultMapper::removePropertyChangeListener( const OUString& aPropertyName, const uno::Reference< beans::XPropertyChangeListener >& aListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL ImpDefaultMapper::addVetoableChangeListener( const OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL ImpDefaultMapper::removeVetoableChangeListener( const OUString& PropertyName, const uno::Reference< beans::XVetoableChangeListener >& aListener ) throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteDefaultStyleInfos()
{
    // create annd write pool defaults
    AddAttribute(XML_NAMESPACE_STYLE, sXML_name, OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_drawpool)));
    AddAttribute(XML_NAMESPACE_STYLE, sXML_family, OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_POOL_NAME)));

    // write drawpool style
    SvXMLElementExport aPSY(*this, XML_NAMESPACE_STYLE, sXML_style, sal_True, sal_True);

    // write graphics style properites
    uno::Any aAny(mxDocStyleFamilies->getByName(OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_GRAPHICS_NAME))));
    uno::Reference< container::XIndexAccess > xGraphicStyles;

    if(aAny >>= xGraphicStyles)
    {
        BOOL bDone(FALSE);
        const sal_Int32 nNum = xGraphicStyles->getCount();

        for(sal_Int32 a(0L); !bDone && a < nNum; a++)
        {
            aAny = xGraphicStyles->getByIndex(a);
            uno::Reference< style::XStyle > xSingleStyle;

            if(aAny >>= xSingleStyle)
            {
                OUString aParentStyle = xSingleStyle->getParentStyle();
                if(!aParentStyle.getLength())
                {
                    // style without parent found
                    uno::Reference< beans::XPropertyState > xPropState(xSingleStyle, uno::UNO_QUERY);
                    if(xPropState.is())
                    {
                        uno::Reference< beans::XPropertySet > xImpDefaultMapper( new ImpDefaultMapper( xPropState ) );
                        const UniReference< SvXMLExportPropertyMapper > aMapperRef = GetPropertySetMapper();
                        std::vector< XMLPropertyState > xPropStates = aMapperRef->Filter( xImpDefaultMapper );

                        if(xPropStates.size())
                        {
                            aMapperRef->exportXML(GetDocHandler(), xPropStates,
                                GetMM100UnitConverter(), GetNamespaceMap());
                            bDone = TRUE;
                        }
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteObjGraphicStyleInfos()
{
    XMLStyleExport aStEx(*this,
        OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_drawpool)), GetAutoStylePool().get());
    const UniReference< SvXMLExportPropertyMapper > aMapperRef = GetPropertySetMapper();

    aStEx.exportStyleFamily(XML_STYLE_FAMILY_SD_GRAPHICS_NAME, XML_STYLE_FAMILY_SD_GRAPHICS_NAME,
        aMapperRef, FALSE, XML_STYLE_FAMILY_SD_GRAPHICS_ID);
}

//////////////////////////////////////////////////////////////////////////////

BOOL SdXMLExport::ImpPrepAutoLayoutInfo(const uno::Reference<drawing::XDrawPage>& xPage, OUString& rName)
{
    rName = OUString();
    BOOL bRetval(FALSE);

    uno::Reference <beans::XPropertySet> xPropSet(xPage, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        sal_uInt16 nType;
        uno::Any aAny;

        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("Layout")));
        if(aAny >>= nType)
        {
            if(ImpXMLAutoLayoutInfo::IsCreateNecessary(nType))
            {
                ImpXMLEXPPageMasterInfo* pInfo = 0L;

                // get master-page info
                uno::Reference < drawing::XMasterPageTarget > xMasterPageInt(xPage, uno::UNO_QUERY);
                if(xMasterPageInt.is())
                {
                    uno::Reference<drawing::XDrawPage> xUsedMasterPage(xMasterPageInt->getMasterPage());
                    if(xUsedMasterPage.is())
                    {
                        uno::Reference < container::XNamed > xMasterNamed(xUsedMasterPage, uno::UNO_QUERY);
                        if(xMasterNamed.is())
                        {
                            OUString sMasterPageName = xMasterNamed->getName();
                            pInfo = ImpGetPageMasterInfoByName(sMasterPageName);
                        }
                    }
                }

                // create entry and look for existance
                ImpXMLAutoLayoutInfo* pNew = new ImpXMLAutoLayoutInfo(nType, pInfo);
                BOOL bDidExist(FALSE);

                for(sal_uInt32 nCnt = 0L; !bDidExist && nCnt < mpAutoLayoutInfoList->Count(); nCnt++)
                {
                    if(*mpAutoLayoutInfoList->GetObject(nCnt) == *pNew)
                    {
                        delete pNew;
                        pNew = mpAutoLayoutInfoList->GetObject(nCnt);
                        bDidExist = TRUE;
                    }
                }

                if(!bDidExist)
                {
                    mpAutoLayoutInfoList->Insert(pNew, LIST_APPEND);
                    OUString sNewName = OUString(RTL_CONSTASCII_USTRINGPARAM("AL"));
                    sNewName += OUString::valueOf(sal_Int32(mpAutoLayoutInfoList->Count() - 1));
                    sNewName += OUString(RTL_CONSTASCII_USTRINGPARAM("T"));
                    sNewName += OUString::valueOf(sal_Int32(nType));
                    pNew->SetLayoutName(sNewName);
                }

                rName = pNew->GetLayoutName();
                bRetval = TRUE;
            }
        }
    }

    return bRetval;
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteAutoLayoutInfos()
{
    if(mpAutoLayoutInfoList->Count())
    {
        for(sal_uInt32 nCnt = 0L; nCnt < mpAutoLayoutInfoList->Count(); nCnt++)
        {
            ImpXMLAutoLayoutInfo* pInfo = mpAutoLayoutInfoList->GetObject(nCnt);
            if(pInfo)
            {
                // prepare presentation-page layout attributes, style-name
                AddAttribute(XML_NAMESPACE_STYLE, sXML_name, pInfo->GetLayoutName());

                // write draw-style attributes
                SvXMLElementExport aDSE(*this, XML_NAMESPACE_STYLE, sXML_presentation_page_layout, sal_True, sal_True);

                // write presentation placeholders
                switch(pInfo->GetLayoutType())
                {
                    case 0 : // AUTOLAYOUT_TITLE
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderSubtitle, pInfo->GetPresRectangle());
                        break;
                    }
                    case 1 : // AUTOLAYOUT_ENUM
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, pInfo->GetPresRectangle());
                        break;
                    }
                    case 2 : // AUTOLAYOUT_CHART
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderChart, pInfo->GetPresRectangle());
                        break;
                    }
                    case 3 : // AUTOLAYOUT_2TEXT
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aRight);
                        break;
                    }
                    case 4 : // AUTOLAYOUT_TEXTCHART
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderChart, aRight);
                        break;
                    }
                    case 6 : // AUTOLAYOUT_TEXTCLIP
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderGraphic, aRight);
                        break;
                    }
                    case 7 : // AUTOLAYOUT_CHARTTEXT
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderChart, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aRight);
                        break;
                    }
                    case 8 : // AUTOLAYOUT_TAB
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTable, pInfo->GetPresRectangle());
                        break;
                    }
                    case 9 : // AUTOLAYOUT_CLIPTEXT
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderGraphic, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aRight);
                        break;
                    }
                    case 10 : // AUTOLAYOUT_TEXTOBJ
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aRight);
                        break;
                    }
                    case 11 : // AUTOLAYOUT_OBJ
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, pInfo->GetPresRectangle());
                        break;
                    }
                    case 12 : // AUTOLAYOUT_TEXT2OBJ
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRightTop(aLeft);
                        aRightTop.Left() = long(aRightTop.Left() + aRightTop.GetWidth() * 1.05);
                        aRightTop.setHeight(long(aRightTop.GetHeight() * 0.477));
                        Rectangle aRightBottom(aRightTop);
                        aRightBottom.Top() = long(aRightBottom.Top() + aRightBottom.GetHeight() * 1.095);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aRightTop);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aRightBottom);
                        break;
                    }
                    case 13 : // AUTOLAYOUT_OBJTEXT
                    {
                        Rectangle aLeft(pInfo->GetPresRectangle());
                        aLeft.setWidth(long(aLeft.GetWidth() * 0.488));
                        Rectangle aRight(aLeft);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aRight);
                        break;
                    }
                    case 14 : // AUTOLAYOUT_OBJOVERTEXT
                    {
                        Rectangle aTop(pInfo->GetPresRectangle());
                        aTop.setHeight(long(aTop.GetHeight() * 0.477));
                        Rectangle aBottom(aTop);
                        aBottom.Top() = long(aBottom.Top() + aBottom.GetHeight() * 1.095);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aTop);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aBottom);
                        break;
                    }
                    case 15 : // AUTOLAYOUT_2OBJTEXT
                    {
                        Rectangle aLeftTop(pInfo->GetPresRectangle());
                        aLeftTop.setWidth(long(aLeftTop.GetWidth() * 0.488));
                        Rectangle aRight(aLeftTop);
                        aRight.Left() = long(aRight.Left() + aRight.GetWidth() * 1.05);
                        aLeftTop.setHeight(long(aLeftTop.GetHeight() * 0.477));
                        Rectangle aLeftBottom(aLeftTop);
                        aLeftBottom.Top() = long(aLeftBottom.Top() + aLeftBottom.GetHeight() * 1.095);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aLeftTop);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aLeftBottom);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aRight);
                        break;
                    }
                    case 16 : // AUTOLAYOUT_2OBJOVERTEXT
                    {
                        Rectangle aTopLeft(pInfo->GetPresRectangle());
                        aTopLeft.setHeight(long(aTopLeft.GetHeight() * 0.477));
                        Rectangle aBottom(aTopLeft);
                        aBottom.Top() = long(aBottom.Top() + aBottom.GetHeight() * 1.095);
                        aTopLeft.setWidth(long(aTopLeft.GetWidth() * 0.488));
                        Rectangle aTopRight(aTopLeft);
                        aTopRight.Left() = long(aTopRight.Left() + aTopRight.GetWidth() * 1.05);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aTopLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aTopRight);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aBottom);
                        break;
                    }
                    case 17 : // AUTOLAYOUT_TEXTOVEROBJ
                    {
                        Rectangle aTop(pInfo->GetPresRectangle());
                        aTop.setHeight(long(aTop.GetHeight() * 0.477));
                        Rectangle aBottom(aTop);
                        aBottom.Top() = long(aBottom.Top() + aBottom.GetHeight() * 1.095);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderOutline, aTop);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aBottom);
                        break;
                    }
                    case 18 : // AUTOLAYOUT_4OBJ
                    {
                        Rectangle aTopLeft(pInfo->GetPresRectangle());
                        aTopLeft.setHeight(long(aTopLeft.GetHeight() * 0.477));
                        aTopLeft.setWidth(long(aTopLeft.GetWidth() * 0.488));
                        Rectangle aBottomLeft(aTopLeft);
                        aBottomLeft.Top() = long(aBottomLeft.Top() + aBottomLeft.GetHeight() * 1.095);
                        Rectangle aTopRight(aTopLeft);
                        aTopRight.Left() = long(aTopRight.Left() + aTopRight.GetWidth() * 1.05);
                        Rectangle aBottomRight(aTopRight);
                        aBottomRight.Top() = long(aBottomRight.Top() + aBottomRight.GetHeight() * 1.095);

                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aTopLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aTopRight);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aBottomLeft);
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderObject, aBottomRight);
                        break;
                    }
                    case 19 : // AUTOLAYOUT_ONLY_TITLE
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderTitle, pInfo->GetTitleRectangle());
                        break;
                    }
                    case 21 : // AUTOLAYOUT_NOTES
                    {
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderPage, pInfo->GetTitleRectangle());
                        ImpWriteAutoLayoutPlaceholder(XmlPlaceholderNotes, pInfo->GetPresRectangle());
                        break;
                    }
                    case 22 : // AUTOLAYOUT_HANDOUT1
                    case 23 : // AUTOLAYOUT_HANDOUT2
                    case 24 : // AUTOLAYOUT_HANDOUT3
                    case 25 : // AUTOLAYOUT_HANDOUT4
                    case 26 : // AUTOLAYOUT_HANDOUT6
                    {
                        sal_Int32 nColCnt, nRowCnt;
                        sal_Int32 nGapX = pInfo->GetGapX();
                        sal_Int32 nGapY = pInfo->GetGapY();

                        switch(pInfo->GetLayoutType())
                        {
                            case 22 : nColCnt = 1; nRowCnt = 1; break;
                            case 23 : nColCnt = 1; nRowCnt = 2; break;
                            case 24 : nColCnt = 1; nRowCnt = 3; break;
                            case 25 : nColCnt = 2; nRowCnt = 2; break;
                            case 26 : nColCnt = 3; nRowCnt = 3; break;
                        }

                        Size aPartSize(pInfo->GetTitleRectangle().GetSize());
                        Point aPartPos(pInfo->GetTitleRectangle().TopLeft());

                        if(aPartSize.Width() > aPartSize.Height())
                        {
                            sal_Int32 nZwi(nColCnt);
                            nColCnt = nRowCnt;
                            nRowCnt = nZwi;
                        }

                        aPartSize.Width() = (aPartSize.Width() - ((nColCnt - 1) * nGapX)) / nColCnt;
                        aPartSize.Height() = (aPartSize.Height() - ((nRowCnt - 1) * nGapY)) / nRowCnt;

                        Point aTmpPos(aPartPos);

                        for(sal_Int32 a = 0L; a < nRowCnt; a++)
                        {
                            aTmpPos.X() = aPartPos.X();

                            for(sal_Int32 b = 0L; b < nColCnt; b++)
                            {
                                Rectangle aTmpRect(aTmpPos, aPartSize);

                                ImpWriteAutoLayoutPlaceholder(XmlPlaceholderHandout, aTmpRect);
                                aTmpPos.X() += aPartSize.Width() + nGapX;
                            }

                            aTmpPos.Y() += aPartSize.Height() + nGapY;
                        }
                        break;
                    }
                    default:
                    {
                        DBG_ERROR("XMLEXP: unknown autolayout export");
                        break;
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteAutoLayoutPlaceholder(XmlPlaceholder ePl, const Rectangle& rRect)
{
    OUString aStr;
    OUStringBuffer sStringBuffer;

    // prepare presentation-placeholder attributes, presentation:object
    switch(ePl)
    {
        case XmlPlaceholderTitle: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("title")); break;
        case XmlPlaceholderOutline: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("outline")); break;
        case XmlPlaceholderSubtitle: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("subtitle")); break;
        case XmlPlaceholderText: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("text")); break;
        case XmlPlaceholderGraphic: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("graphic")); break;
        case XmlPlaceholderObject: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("object")); break;
        case XmlPlaceholderChart: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("chart")); break;
        case XmlPlaceholderOrgchart: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("orgchart")); break;
        case XmlPlaceholderTable: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("table")); break;
        case XmlPlaceholderPage: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("page")); break;
        case XmlPlaceholderNotes: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("notes")); break;
        case XmlPlaceholderHandout: aStr = OUString(RTL_CONSTASCII_USTRINGPARAM("handout")); break;
    }

    AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_object, aStr);

    // svg:x,y,width,height
    GetMM100UnitConverter().convertMeasure(sStringBuffer, rRect.Left());
    aStr = sStringBuffer.makeStringAndClear();
    AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);

    GetMM100UnitConverter().convertMeasure(sStringBuffer, rRect.Top());
    aStr = sStringBuffer.makeStringAndClear();
    AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);

    GetMM100UnitConverter().convertMeasure(sStringBuffer, rRect.GetWidth());
    aStr = sStringBuffer.makeStringAndClear();
    AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);

    GetMM100UnitConverter().convertMeasure(sStringBuffer, rRect.GetHeight());
    aStr = sStringBuffer.makeStringAndClear();
    AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);

    // write presentation-placeholder
    SvXMLElementExport aPPL(*this, XML_NAMESPACE_PRESENTATION, sXML_placeholder, sal_True, sal_True);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpPrepPageMasterInfos()
{
    if(mnDocMasterPageCount)
    {
        // look for needed page-masters, create these
        for(sal_Int32 nMPageId = 0L; nMPageId < mnDocMasterPageCount; nMPageId++)
        {
            uno::Any aAny(mxDocMasterPages->getByIndex(nMPageId));
            uno::Reference< drawing::XDrawPage > xMasterPage;
            ImpXMLEXPPageMasterInfo* pNewInfo = 0L;
            BOOL bDoesExist(FALSE);

            if(aAny >>= xMasterPage)
            {
                pNewInfo = new ImpXMLEXPPageMasterInfo(*this, xMasterPage);

                // compare with prev page-master infos
                for(sal_uInt32 a = 0; !bDoesExist && a < mpPageMasterInfoList->Count(); a++)
                {
                    if(mpPageMasterInfoList->GetObject(a)
                        && *mpPageMasterInfoList->GetObject(a) == *pNewInfo)
                    {
                        delete pNewInfo;
                        pNewInfo = mpPageMasterInfoList->GetObject(a);
                        bDoesExist = TRUE;
                    }
                }
            }

            // add entry when not found same page-master infos
            if(!bDoesExist)
                mpPageMasterInfoList->Insert(pNewInfo, LIST_APPEND);
            mpPageMaterUsageList->Insert(pNewInfo, LIST_APPEND);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWritePageMasterInfos()
{
    // write created page-masters, create names for these
    for(sal_uInt32 nCnt = 0L; nCnt < mpPageMasterInfoList->Count(); nCnt++)
    {
        ImpXMLEXPPageMasterInfo* pInfo = mpPageMasterInfoList->GetObject(nCnt);
        if(pInfo)
        {
            // create name
            OUString sNewName = OUString(RTL_CONSTASCII_USTRINGPARAM("PM"));

            sNewName += OUString::valueOf((sal_Int32)nCnt);
            pInfo->SetName(sNewName);

            // prepare page-master attributes
            OUString sString;
            OUStringBuffer sStringBuffer;

            sString = sNewName;
            AddAttribute(XML_NAMESPACE_STYLE, sXML_name, sString);

            // write page-master
            SvXMLElementExport aPME(*this, XML_NAMESPACE_STYLE, sXML_page_master, sal_True, sal_True);

            // prepare style:properties inside page-master
            GetMM100UnitConverter().convertMeasure(sStringBuffer, pInfo->GetBorderTop());
            sString = sStringBuffer.makeStringAndClear();
            AddAttribute(XML_NAMESPACE_FO, sXML_margin_top, sString);

            GetMM100UnitConverter().convertMeasure(sStringBuffer, pInfo->GetBorderBottom());
            sString = sStringBuffer.makeStringAndClear();
            AddAttribute(XML_NAMESPACE_FO, sXML_margin_bottom, sString);

            GetMM100UnitConverter().convertMeasure(sStringBuffer, pInfo->GetBorderLeft());
            sString = sStringBuffer.makeStringAndClear();
            AddAttribute(XML_NAMESPACE_FO, sXML_margin_left, sString);

            GetMM100UnitConverter().convertMeasure(sStringBuffer, pInfo->GetBorderRight());
            sString = sStringBuffer.makeStringAndClear();
            AddAttribute(XML_NAMESPACE_FO, sXML_margin_right, sString);

            GetMM100UnitConverter().convertMeasure(sStringBuffer, pInfo->GetWidth());
            sString = sStringBuffer.makeStringAndClear();
            AddAttribute(XML_NAMESPACE_FO, sXML_page_width, sString);

            GetMM100UnitConverter().convertMeasure(sStringBuffer, pInfo->GetHeight());
            sString = sStringBuffer.makeStringAndClear();
            AddAttribute(XML_NAMESPACE_FO, sXML_page_height, sString);

            if(pInfo->GetOrientation() == view::PaperOrientation_PORTRAIT)
                AddAttributeASCII(XML_NAMESPACE_FO, sXML_page_orientation, sXML_orientation_portrait);
            else
                AddAttributeASCII(XML_NAMESPACE_FO, sXML_page_orientation, sXML_orientation_landscape);

            // write style:properties
            SvXMLElementExport aPMF(*this, XML_NAMESPACE_STYLE, sXML_properties, sal_True, sal_True);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

ImpXMLEXPPageMasterInfo* SdXMLExport::ImpGetPageMasterInfoByName(const OUString& rName)
{
    if(rName.getLength() && mpPageMasterInfoList->Count())
    {
        for(sal_uInt32 nCnt = 0L; nCnt < mpPageMasterInfoList->Count(); nCnt++)
        {
            ImpXMLEXPPageMasterInfo* pInfo = mpPageMasterInfoList->GetObject(nCnt);
            if(pInfo)
            {
                if(pInfo->GetMasterPageName().getLength() && rName.equals(pInfo->GetMasterPageName()))
                {
                    return pInfo;
                }
            }
        }
    }
    return 0L;
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpPrepDrawPageInfos()
{
    // create draw:style-name entries for page export
    // containing presentation page attributes AND background attributes
    // fixed family for page-styles is "drawing-page" (XML_STYLE_FAMILY_SD_DRAWINGPAGE_NAME)
    if(mnDocDrawPageCount)
    {
        // prepare name creation
        for(sal_Int32 nCnt = 0L; nCnt < mnDocDrawPageCount; nCnt++)
        {
            uno::Any aAny(mxDocDrawPages->getByIndex(nCnt));
            uno::Reference<drawing::XDrawPage> xDrawPage;

            if(aAny >>= xDrawPage)
            {
                // create name
                OUString sNewName = OUString(RTL_CONSTASCII_USTRINGPARAM("P"));
                sNewName += OUString::valueOf((sal_Int32)nCnt);

                // create Info object
                ImpXMLDrawPageInfo* pInfo = new ImpXMLDrawPageInfo(sNewName);
                mpDrawPageInfoList->Insert(pInfo, LIST_APPEND);

                if(IsImpress())
                {
                    // create presentation-page-layout
                    OUString aStr;

                    if(ImpPrepAutoLayoutInfo(xDrawPage, aStr))
                    {
                        pInfo->SetPageLayoutName(aStr);
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteDrawPageInfos()
{
    if(mnDocDrawPageCount && mpDrawPageInfoList->Count())
    {
        // prepare write
        for(sal_Int32 nCnt = 0L; nCnt < mnDocDrawPageCount; nCnt++)
        {
            uno::Any aAny(mxDocDrawPages->getByIndex(nCnt));
            uno::Reference<drawing::XDrawPage> xDrawPage;

            if(aAny >>= xDrawPage)
            {
                ImpXMLDrawPageInfo* pInfo = mpDrawPageInfoList->GetObject(nCnt);
                if(pInfo)
                {
                    // prepare draw-style attributes, style-name
                    AddAttribute(XML_NAMESPACE_STYLE, sXML_name, pInfo->GetStyleName());

                    // style-family
                    AddAttribute(XML_NAMESPACE_STYLE, sXML_family,
                        OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_DRAWINGPAGE_NAME)));

                    // write draw-style attributes
                    SvXMLElementExport aDSE(*this, XML_NAMESPACE_STYLE, sXML_style, sal_True, sal_True);

                    // write draw-style properites
                    uno::Reference< beans::XPropertySet > xPropSet1(xDrawPage, uno::UNO_QUERY);
                    if(xPropSet1.is())
                    {
                        // since the background items are in a different propertyset
                        // which itself is a property of the pages property set
                        // we now merge these two propertysets if possible to simulate
                        // a single propertyset with all draw page properties
                        const OUString aBackground(RTL_CONSTASCII_USTRINGPARAM("Background"));
                        uno::Reference< beans::XPropertySet > xPropSet2;
                        uno::Reference< beans::XPropertySetInfo > xInfo( xPropSet1->getPropertySetInfo() );
                        if( xInfo.is() && xInfo->hasPropertyByName( aBackground ) )
                        {
                            uno::Any aAny( xPropSet1->getPropertyValue( aBackground ) );
                            aAny >>= xPropSet2;
                        }

                        uno::Reference< beans::XPropertySet > xPropSet;
                        if( xPropSet2.is() )
                            xPropSet = PropertySetMerger_CreateInstance( xPropSet1, xPropSet2 );
                        else
                            xPropSet = xPropSet1;

                        const UniReference< SvXMLExportPropertyMapper > aMapperRef( GetPresPagePropsMapper() );
                        std::vector< XMLPropertyState > xPropStates( aMapperRef->Filter( xPropSet ) );

                        if(xPropStates.size())
                        {
                            aMapperRef->exportXML(GetDocHandler(), xPropStates,
                                GetMM100UnitConverter(), GetNamespaceMap());
                        }
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
void SdXMLExport::ImpWritePresentationStyles()
{
    if(IsImpress())
    {
        for(sal_Int32 nCnt = 0L; nCnt < mnDocMasterPageCount; nCnt++)
        {
            uno::Any aAny(mxDocMasterPages->getByIndex(nCnt));
            uno::Reference<container::XNamed> xNamed;

            if(aAny >>= xNamed)
            {
                // write presentation styles (ONLY if presentation)
                if(IsImpress() && mxDocStyleFamilies.is() && xNamed.is())
                {
                    XMLStyleExport aStEx(*this,
                        OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_drawpool)), GetAutoStylePool().get());
                    const UniReference< SvXMLExportPropertyMapper > aMapperRef( GetPropertySetMapper() );

                    OUString aPrefix = xNamed->getName();
                    aPrefix += OUString(RTL_CONSTASCII_USTRINGPARAM("-"));
                    aStEx.exportStyleFamily(xNamed->getName(),
                        XML_STYLE_FAMILY_SD_PRESENTATION_NAME, aMapperRef, FALSE,
                        XML_STYLE_FAMILY_SD_PRESENTATION_ID, &aPrefix);
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::SetProgress(sal_Int32 nProg)
{
    // set progress view
    if(mxStatusIndicator.is())
        mxStatusIndicator->setValue(nProg);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::_ExportContent()
{
    // page export
    for(sal_Int32 nPageInd(0); nPageInd < mnDocDrawPageCount; nPageInd++)
    {
        uno::Any aAny(mxDocDrawPages->getByIndex(nPageInd));
        uno::Reference<drawing::XDrawPage> xDrawPage;

        SetProgress(((nPageInd + 1) * 100) / mnDocDrawPageCount);

        if(aAny >>= xDrawPage)
        {
            // prepare page attributes, name of page
            uno::Reference < container::XNamed > xNamed(xDrawPage, uno::UNO_QUERY);
            if(xNamed.is())
                AddAttribute(XML_NAMESPACE_DRAW, sXML_name, xNamed->getName());

            // draw:style-name (presentation page attributes AND background attributes)
            ImpXMLDrawPageInfo* pInfo = mpDrawPageInfoList->GetObject(nPageInd);
            if(pInfo)
            {
                OUString sString = pInfo->GetStyleName();
                AddAttribute(XML_NAMESPACE_DRAW, sXML_style_name, sString);
            }

            // draw:master-page-name
            uno::Reference < drawing::XMasterPageTarget > xMasterPageInt(xDrawPage, uno::UNO_QUERY);
            if(xMasterPageInt.is())
            {
                uno::Reference<drawing::XDrawPage> xUsedMasterPage(xMasterPageInt->getMasterPage());
                if(xUsedMasterPage.is())
                {
                    uno::Reference < container::XNamed > xMasterNamed(xUsedMasterPage, uno::UNO_QUERY);
                    if(xMasterNamed.is())
                    {
                        AddAttribute(XML_NAMESPACE_DRAW, sXML_master_page_name, xMasterNamed->getName());
                    }
                }
            }

            // presentation:page-layout-name
            if(pInfo && IsImpress() && pInfo->GetPageLayoutName().getLength())
            {
                OUString sString = pInfo->GetPageLayoutName();
                AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_presentation_page_layout_name, sString);
            }

            // write page
            SvXMLElementExport aDPG(*this, XML_NAMESPACE_DRAW, sXML_page, sal_True, sal_True);

            // write graphic objects on this page (if any)
            uno::Reference< container::XIndexAccess > xShapes(xDrawPage, uno::UNO_QUERY);
            if(xShapes.is() && xShapes->getCount())
            {
                // write shapes per se
                ImpWriteSingleShapeStyleInfos(xShapes);
            }

            // write presentation notes (ONLY if presentation)
            if(IsImpress())
            {
                uno::Reference< presentation::XPresentationPage > xPresPage(xDrawPage, uno::UNO_QUERY);
                if(xPresPage.is())
                {
                    uno::Reference< drawing::XDrawPage > xNotesPage(xPresPage->getNotesPage());
                    if(xNotesPage.is())
                    {
                        uno::Reference< container::XIndexAccess > xShapes(xNotesPage, uno::UNO_QUERY);
                        if(xShapes.is() && xShapes->getCount())
                        {
                            // write presentation notes
                            SvXMLElementExport aPSY(*this, XML_NAMESPACE_PRESENTATION, sXML_notes, sal_True, sal_True);

                            // write shapes per se
                            ImpWriteSingleShapeStyleInfos(xShapes);
                        }
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportTextBoxShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */ )
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

        SdXMLImExTransform2D aTransform;
        OUString aStr;
        OUStringBuffer sStringBuffer;

        // presentation attribute (if presentation)
        sal_Bool bIsPresShape(FALSE);
        sal_Bool bIsEmptyPresObj(FALSE);
        sal_Bool bIsPlaceholderDependant(TRUE);

        switch(eShapeType)
        {
            case XmlShapeTypePresSubtitleShape:
            {
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_subtitle));
                bIsPresShape = TRUE;
                break;
            }
            case XmlShapeTypePresTitleTextShape:
            {
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_title));
                bIsPresShape = TRUE;
                break;
            }
            case XmlShapeTypePresOutlinerShape:
            {
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_outline));
                bIsPresShape = TRUE;
                break;
            }
            case XmlShapeTypePresNotesShape:
            {
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_notes));
                bIsPresShape = TRUE;
                break;
            }
        }

        if(bIsPresShape)
        {
            // is empty pes shape?
            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject"))))
            {
                uno::Any aAny( xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject"))) );
                aAny >>= bIsEmptyPresObj;
            }

            // is user-transformed?
            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(OUString(RTL_CONSTASCII_USTRINGPARAM("IsPlaceholderDependent"))))
            {
                uno::Any aAny( xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("IsPlaceholderDependent"))) );
                aAny >>= bIsPlaceholderDependant;
            }

            // write presentation class entry
            rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_class, aStr);

            // write presentation placeholder entry
            if(bIsEmptyPresObj)
                rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_placeholder,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));

            // write user-transformed entry
            if(!bIsPlaceholderDependant)
                rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_user_transformed,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        }

        // text shape, prepare parameters
        awt::Point aPoint( xShape->getPosition() );
        if( pRefPoint )
        {
            aPoint.X -= pRefPoint->X;
            aPoint.Y -= pRefPoint->Y;
        }

        awt::Size aSize( xShape->getSize() );

        if( nFeatures & SEF_EXPORT_X )
        {
            // svg: x
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);
        }

        if( nFeatures & SEF_EXPORT_Y )
        {
            // svg: y
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);
        }

        if( nFeatures & SEF_EXPORT_WIDTH )
        {
            // svg: width
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Width);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);
        }

        if( nFeatures & SEF_EXPORT_HEIGHT )
        {
            // svg: height
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Height);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);
        }

        // evtl. rotation (100'th degree, part of transformation)?
        sal_Int32 nRotAngle(0L);
        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("RotateAngle")));
        aAny >>= nRotAngle;
        if(nRotAngle)
            aTransform.AddRotate(nRotAngle / 100.0);

        // does transformation need to be exported?
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        // write text-box
        SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_text_box, sal_True, sal_True);

        // export text
        if(!bIsEmptyPresObj)
        {
            uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
            if( xText.is() )
                rExp.GetTextParagraphExport()->exportText( xText );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportRectangleShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /*= SEF_DEFAULT */,    com::sun::star::awt::Point* pRefPoint /* = NULL */ )
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        SdXMLImExTransform2D aTransform;
        OUString aStr;
        OUStringBuffer sStringBuffer;

        // rectangle, prepare parameters
        awt::Point aPoint = xShape->getPosition();
        if( pRefPoint )
        {
            aPoint.X -= pRefPoint->X;
            aPoint.Y -= pRefPoint->Y;
        }

        awt::Size aSize = xShape->getSize();

        if( nFeatures & SEF_EXPORT_X )
        {
            // svg: x
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);
        }

        if( nFeatures & SEF_EXPORT_Y )
        {
            // svg: y
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);
        }

        if( nFeatures & SEF_EXPORT_WIDTH )
        {
            // svg: width
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Width);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);
        }

        if( nFeatures & SEF_EXPORT_HEIGHT )
        {
            // svg: height
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Height);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);
        }

        // evtl. rotation (100'th degree, part of transformation)?
        sal_Int32 nRotAngle(0L);
        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("RotateAngle")));
        aAny >>= nRotAngle;
        if(nRotAngle)
            aTransform.AddRotate(nRotAngle / 100.0);

        // does transformation need to be exported?
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        // evtl. corner radius?
        sal_Int32 nCornerRadius(0L);
        aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("CornerRadius")));
        aAny >>= nCornerRadius;
        if(nCornerRadius)
        {
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nCornerRadius);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_corner_radius, aStr);
        }

        // write rectangle
        SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_rect, sal_True, sal_True);

        // export text
        uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
        if( xText.is() )
            rExp.GetTextParagraphExport()->exportText( xText );
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportLineShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */ )
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        SdXMLImExTransform2D aTransform;
        OUString aStr;
        OUStringBuffer sStringBuffer;
        awt::Point aStart(0,0);
        awt::Point aEnd(1,1);

        drawing::PointSequenceSequence* pSourcePolyPolygon = 0L;
        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("PolyPolygon")));
        pSourcePolyPolygon = (drawing::PointSequenceSequence*)aAny.getValue();

        if(pSourcePolyPolygon)
        {
            drawing::PointSequence* pOuterSequence = pSourcePolyPolygon->getArray();
            if(pOuterSequence)
            {
                drawing::PointSequence* pInnerSequence = pOuterSequence++;
                if(pInnerSequence)
                {
                    awt::Point* pArray = pInnerSequence->getArray();
                    if(pArray)
                    {
                        if(pInnerSequence->getLength() > 0)
                        {
                            aStart = awt::Point(pArray->X, pArray->Y);
                            pArray++;
                        }

                        if(pInnerSequence->getLength() > 1)
                        {
                            aEnd = awt::Point(pArray->X, pArray->Y);
                        }
                    }
                }
            }
        }

        if( pRefPoint )
        {
            aStart.X -= pRefPoint->X;
            aStart.Y -= pRefPoint->Y;
            aEnd.X -= pRefPoint->X;
            aEnd.Y -= pRefPoint->Y;
        }

        // svg: x1
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aStart.X);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x1, aStr);

        // svg: y1
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aStart.Y);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y1, aStr);

        // svg: x2
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aEnd.X);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x2, aStr);

        // svg: y2
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aEnd.Y);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y2, aStr);

        // evtl. rotation (100'th degree, part of transformation)?
        sal_Int32 nRotAngle(0L);
        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("RotateAngle")));
        aAny >>= nRotAngle;
        if(nRotAngle)
            aTransform.AddRotate(nRotAngle / 100.0);

        // does transformation need to be exported?
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        // write line
        SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_line, sal_True, sal_True);

        // export text
        uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
        if( xText.is() )
            rExp.GetTextParagraphExport()->exportText( xText );
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportEllipseShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        SdXMLImExTransform2D aTransform;
        // get size to decide between Circle and Ellipse
        awt::Point aPoint = xShape->getPosition();
        if( pRefPoint )
        {
            aPoint.X -= pRefPoint->X;
            aPoint.Y -= pRefPoint->Y;
        }

        awt::Size aSize = xShape->getSize();
        sal_Int32 nRx((aSize.Width + 1) / 2);
        sal_Int32 nRy((aSize.Height + 1) / 2);
        BOOL bCircle(nRx == nRy);
        OUString aStr;
        OUStringBuffer sStringBuffer;

        if( nFeatures & SEF_EXPORT_X )
        {
            // svg: cx
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X + nRx);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_cx, aStr);
        }

        if( nFeatures & SEF_EXPORT_Y )
        {
            // svg: cy
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y + nRy);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_cy, aStr);
        }

        // evtl. rotation (100'th degree, part of transformation)?
        sal_Int32 nRotAngle(0L);
        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("RotateAngle")));
        aAny >>= nRotAngle;
        if(nRotAngle)
            aTransform.AddRotate(nRotAngle / 100.0);

        // does transformation need to be exported?
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        if(bCircle)
        {
            // svg: r
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nRx);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_r, aStr);

            // write circle
            SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_circle, sal_True, sal_True);

            // export text
            uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
            if( xText.is() )
                rExp.GetTextParagraphExport()->exportText( xText );
        }
        else
        {
            // svg: rx
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nRx);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_rx, aStr);

            // svg: ry
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nRy);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_ry, aStr);

            // write ellipse
            SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_ellipse, sal_True, sal_True);

            // export text
            uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
            if( xText.is() )
                rExp.GetTextParagraphExport()->exportText( xText );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportPolygonShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        BOOL bClosed(eShapeType == XmlShapeTypeDrawPolyPolygonShape
            || eShapeType == XmlShapeTypeDrawClosedBezierShape);
        BOOL bBezier(eShapeType == XmlShapeTypeDrawClosedBezierShape
            || eShapeType == XmlShapeTypeDrawOpenBezierShape);
        SdXMLImExTransform2D aTransform;
        OUString aStr;
        OUStringBuffer sStringBuffer;

        // prepare posistion and size parameters
        awt::Point aPoint = xShape->getPosition();
        if( pRefPoint )
        {
            aPoint.X -= pRefPoint->X;
            aPoint.Y -= pRefPoint->Y;
        }

        awt::Size aSize = xShape->getSize();

        if( nFeatures & SEF_EXPORT_X )
        {
            // svg: x
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);
        }

        if( nFeatures & SEF_EXPORT_Y )
        {
            // svg: y
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);
        }

        // svg: width
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Width);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);

        // svg: height
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Height);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);

        // evtl. rotation (100'th degree, part of transformation)?
        sal_Int32 nRotAngle(0L);
        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("RotateAngle")));
        aAny >>= nRotAngle;
        if(nRotAngle)
            aTransform.AddRotate(nRotAngle / 100.0);

        // does transformation need to be exported?
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        // create and export ViewBox
        SdXMLImExViewBox aViewBox(0, 0, aSize.Width, aSize.Height);
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_viewBox, aViewBox.GetExportString(rExp.GetMM100UnitConverter()));


        if(bBezier)
        {
            // get PolygonBezier
            aAny = xPropSet->getPropertyValue(
                OUString(RTL_CONSTASCII_USTRINGPARAM("PolyPolygonBezier")));
            drawing::PolyPolygonBezierCoords* pSourcePolyPolygon =
                (drawing::PolyPolygonBezierCoords*)aAny.getValue();

            if(pSourcePolyPolygon && pSourcePolyPolygon->Coordinates.getLength())
            {
                sal_Int32 nOuterCnt(pSourcePolyPolygon->Coordinates.getLength());
                drawing::PointSequence* pOuterSequence = pSourcePolyPolygon->Coordinates.getArray();
                drawing::FlagSequence*  pOuterFlags = pSourcePolyPolygon->Flags.getArray();

                if(pOuterSequence && pOuterFlags)
                {
                    // prepare svx:d element export
                    SdXMLImExSvgDElement aSvgDElement(aViewBox);

                    for(sal_Int32 a(0L); a < nOuterCnt; a++)
                    {
                        drawing::PointSequence* pSequence = pOuterSequence++;
                        drawing::FlagSequence* pFlags = pOuterFlags++;

                        if(pSequence && pFlags)
                        {
                            aSvgDElement.AddPolygon(pSequence, pFlags,
                                aPoint, aSize, rExp.GetMM100UnitConverter(), bClosed);
                        }
                    }

                    // write point array
                    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_d, aSvgDElement.GetExportString());
                }

                // write object now
                SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_path, sal_True, sal_True);

                // export text
                uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
                if( xText.is() )
                    rExp.GetTextParagraphExport()->exportText( xText );
            }
        }
        else
        {
            // get non-bezier polygon
            aAny = xPropSet->getPropertyValue(
                OUString(RTL_CONSTASCII_USTRINGPARAM("PolyPolygon")));
            drawing::PointSequenceSequence* pSourcePolyPolygon =
                (drawing::PointSequenceSequence*)aAny.getValue();

            if(pSourcePolyPolygon && pSourcePolyPolygon->getLength())
            {
                sal_Int32 nOuterCnt(pSourcePolyPolygon->getLength());

                if(1L == nOuterCnt && !bBezier)
                {
                    // simple polygon shape, can be written as svg:points sequence
                    drawing::PointSequence* pSequence = pSourcePolyPolygon->getArray();
                    if(pSequence)
                    {
                        SdXMLImExPointsElement aPoints(pSequence, aViewBox, aPoint, aSize, rExp.GetMM100UnitConverter());

                        // write point array
                        rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_points, aPoints.GetExportString());
                    }

                    // write object now
                    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW,
                        bClosed ? sXML_polygon : sXML_polyline , sal_True, sal_True);

                    // export text
                    uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
                    if( xText.is() )
                        rExp.GetTextParagraphExport()->exportText( xText );
                }
                else
                {
                    // polypolygon or bezier, needs to be written as a svg:path sequence
                    drawing::PointSequence* pOuterSequence = pSourcePolyPolygon->getArray();
                    if(pOuterSequence)
                    {
                        // prepare svx:d element export
                        SdXMLImExSvgDElement aSvgDElement(aViewBox);

                        for(sal_Int32 a(0L); a < nOuterCnt; a++)
                        {
                            drawing::PointSequence* pSequence = pOuterSequence++;
                            if(pSequence)
                            {
                                aSvgDElement.AddPolygon(pSequence, 0L, aPoint,
                                    aSize, rExp.GetMM100UnitConverter(), bClosed);
                            }
                        }

                        // write point array
                        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_d, aSvgDElement.GetExportString());
                    }

                    // write object now
                    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_path, sal_True, sal_True);

                    // export text
                    uno::Reference< text::XText > xText( xShape, uno::UNO_QUERY );
                    if( xText.is() )
                        rExp.GetTextParagraphExport()->exportText( xText );
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportGraphicObjectShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        SdXMLImExTransform2D aTransform;
        OUString aStr;
        OUStringBuffer sStringBuffer;

        sal_Bool bIsEmptyPresObj = sal_False;
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

        // prepare posistion and size parameters
        awt::Point aPoint = xShape->getPosition();
        if( pRefPoint )
        {
            aPoint.X -= pRefPoint->X;
            aPoint.Y -= pRefPoint->Y;
        }

        awt::Size aSize = xShape->getSize();

        if( nFeatures & SEF_EXPORT_X )
        {
            // svg: x
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);
        }

        if( nFeatures & SEF_EXPORT_Y )
        {
            // svg: y
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);
        }

        // svg: width
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Width);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);

        // svg: height
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Height);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);

        // evtl. rotation (100'th degree, part of transformation)?
        sal_Int32 nRotAngle(0L);
        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("RotateAngle")));
        aAny >>= nRotAngle;
        if(nRotAngle)
            aTransform.AddRotate(nRotAngle / 100.0);

        // does transformation need to be exported?
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        if(eShapeType == XmlShapeTypePresGraphicObjectShape)
        {
            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject"))))
            {
                uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject")));
                aAny >>= bIsEmptyPresObj;
            }

            // write presentation class entry
            rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_class,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_graphic)));

            // write presentation placeholder entry
            if(bIsEmptyPresObj)
                rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_placeholder,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        }

        if( !bIsEmptyPresObj )
        {
            aAny = xPropSet->getPropertyValue( OUString(RTL_CONSTASCII_USTRINGPARAM("GraphicURL")));
            aAny >>= aStr;
            rExp.AddAttribute(XML_NAMESPACE_XLINK, sXML_href, aStr );

            aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_simple));
            rExp.AddAttribute(XML_NAMESPACE_XLINK, sXML_type, aStr );

            aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_embed));
            rExp.AddAttribute(XML_NAMESPACE_XLINK, sXML_show, aStr );

            aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_onLoad));
            rExp.AddAttribute(XML_NAMESPACE_XLINK, sXML_actuate, aStr );
        }
        // write graphic object
        SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_image, sal_True, sal_True);
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportChartShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

        sal_Bool bIsEmptyPresObj = sal_False;
        if(eShapeType == XmlShapeTypePresChartShape)
        {
            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject"))))
            {
                uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject")));
                aAny >>= bIsEmptyPresObj;
            }

            // write presentation class entry
            rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_class,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_chart)));

            // write presentation placeholder entry
            if(bIsEmptyPresObj)
                rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_placeholder,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        }

        OUString aStr;
        OUStringBuffer sStringBuffer;

        // rectangle, prepare parameters
        awt::Point aPoint( xShape->getPosition());
        if( pRefPoint )
        {
            aPoint.X -= pRefPoint->X;
            aPoint.Y -= pRefPoint->Y;
        }

        awt::Size aSize( xShape->getSize());

        if( nFeatures & SEF_EXPORT_X )
        {
            // svg: x
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);
        }

        if( nFeatures & SEF_EXPORT_Y )
        {
            // svg: y
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);
        }

        // svg: width
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Width);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);

        // svg: height
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Height);
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);

        uno::Reference< chart::XChartDocument > xChartDoc;
        if( !bIsEmptyPresObj )
        {
            uno::Any aAny( xPropSet->getPropertyValue( OUString(RTL_CONSTASCII_USTRINGPARAM("Model") ) ) );
            aAny >>= xChartDoc;
        }

        if( xChartDoc.is() )
        {
            rExp.GetChartExport()->exportChart( xChartDoc, sal_False );
        }
        else
        {
            // write chart object (fake for now, replace later)
            SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_CHART, sXML_chart, sal_True, sal_True);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportSpreadsheetShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

        if(eShapeType == XmlShapeTypePresTableShape)
        {
            sal_Bool bIsEmptyPresObj = sal_False;

            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject"))))
            {
                uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject")));
                aAny >>= bIsEmptyPresObj;
            }

            // write presentation class entry
            rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_class,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_table)));

            // write presentation placeholder entry
            if(bIsEmptyPresObj)
                rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_placeholder,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        }

        // write spreadsheet object (fake for now, replace later)
        SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML__unknown_, sal_True, sal_True);
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportControlShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    // this is a control shape, in this place the database team
    // would have to export the control abilities. Add Export later
    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_control, sal_True, sal_True);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportConnectorShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    uno::Reference< beans::XPropertySet > xProps( xShape, uno::UNO_QUERY );

    OUString aStr;
    OUStringBuffer sStringBuffer;

    // export connection kind
    drawing::ConnectorType eType = drawing::ConnectorType_STANDARD;
    uno::Any aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EdgeKind")));
    aAny >>= eType;

    if( eType != drawing::ConnectorType_STANDARD )
    {
        SvXMLUnitConverter::convertEnum( sStringBuffer, (sal_uInt16)eType, aXML_ConnectionKind_EnumMap );
        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_type, aStr);
    }

    // export line skew
    sal_Int32 nDelta1 = 0, nDelta2 = 0, nDelta3 = 0;

    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EdgeLine1Delta")));
    aAny >>= nDelta1;
    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EdgeLine2Delta")));
    aAny >>= nDelta2;
    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EdgeLine3Delta")));
    aAny >>= nDelta3;

    if( nDelta1 != 0 || nDelta2 != 0 || nDelta3 != 0 )
    {
        rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nDelta1);
        if( nDelta2 != 0 || nDelta3 != 0 )
        {
            const char aSpace = ' ';
            sStringBuffer.appendAscii( &aSpace, 1 );
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nDelta2);
            if( nDelta3 != 0 )
            {
                sStringBuffer.appendAscii( &aSpace, 1 );
                rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nDelta3);
            }
        }

        aStr = sStringBuffer.makeStringAndClear();
        rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_line_skew, aStr);
    }

    // export start and end point
    awt::Point aStart(0,0);
    awt::Point aEnd(1,1);

    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("StartPosition")));
    aAny >>= aStart;

    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EndPosition")));
    aAny >>= aEnd;

    if( pRefPoint )
    {
        aStart.X -= pRefPoint->X;
        aStart.Y -= pRefPoint->Y;
        aEnd.X -= pRefPoint->X;
        aEnd.Y -= pRefPoint->Y;
    }

    // svg: x1
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aStart.X);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x1, aStr);

    // svg: y1
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aStart.Y);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y1, aStr);

    // svg: x2
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aEnd.X);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x2, aStr);

    // svg: y2
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aEnd.Y);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y2, aStr);

    uno::Reference< drawing::XShape > xTempShape;

    // export start connection
    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("StartShape") ) );
    if( aAny >>= xTempShape )
    {
        sal_Int32 nShapeId = rExp.GetShapeExport()->getShapeId( xTempShape );
        rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_start_shape, OUString::valueOf( nShapeId ));

        aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("StartGluePointIndex")) );
        sal_Int32 nGluePointId;
        if( aAny >>= nGluePointId )
        {
            if( nGluePointId != -1 )
            {
                rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_start_glue_point, OUString::valueOf( nGluePointId ));
            }
        }
    }

    // export end connection
    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EndShape")) );
    if( aAny >>= xTempShape )
    {
        sal_Int32 nShapeId = rExp.GetShapeExport()->getShapeId( xTempShape );
        rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_end_shape, OUString::valueOf( nShapeId ));

        aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EndGluePointIndex")) );
        sal_Int32 nGluePointId;
        if( aAny >>= nGluePointId )
        {
            if( nGluePointId != -1 )
            {
                rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_end_glue_point, OUString::valueOf( nGluePointId ));
            }
        }
    }

    // write connector shape. Add Export later.
    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_connector, sal_True, sal_True);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportMeasureShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    uno::Reference< beans::XPropertySet > xProps( xShape, uno::UNO_QUERY );

    OUString aStr;
    OUStringBuffer sStringBuffer;

    // export start and end point
    awt::Point aStart(0,0);
    awt::Point aEnd(1,1);

    uno::Any aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("StartPosition")));
    aAny >>= aStart;

    aAny = xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("EndPosition")));
    aAny >>= aEnd;

    if( pRefPoint )
    {
        aStart.X -= pRefPoint->X;
        aStart.Y -= pRefPoint->Y;
        aEnd.X -= pRefPoint->X;
        aEnd.Y -= pRefPoint->Y;
    }

    // svg: x1
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aStart.X);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x1, aStr);

    // svg: y1
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aStart.Y);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y1, aStr);

    // svg: x2
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aEnd.X);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x2, aStr);

    // svg: y2
    rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aEnd.Y);
    aStr = sStringBuffer.makeStringAndClear();
    rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y2, aStr);

    // write measure shape. Add Export later.
    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_measure, sal_True, sal_True);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportOLE2Shape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

        if(eShapeType == XmlShapeTypePresOLE2Shape)
        {
            sal_Bool bIsEmptyPresObj = sal_False;

            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject"))))
            {
                uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("IsEmptyPresentationObject")));
                aAny >>= bIsEmptyPresObj;
            }

            // write presentation class entry
            rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_class,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_object)));

            // write presentation placeholder entry
            if(bIsEmptyPresObj)
                rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_placeholder,
                OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_true)));
        }

        // write spreadsheet object (fake for now, replace later)
        SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML__unknown_, sal_True, sal_True);
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportPageShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    OUString aStr;

    // a presentation page shape, normally used on notes pages only. If
    // it is used not as presentation shape, it may have been created with
    // copy-paste exchange between draw and impress (this IS possible...)
    if(eShapeType == XmlShapeTypePresPageShape)
    {
        rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_class,
            OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_presentation_page)));
    }

    // write Page shape
    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_page_thumbnail, sal_True, sal_True);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExportCaptionShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    // write Caption shape. Add export later.
    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DRAW, sXML_caption, sal_True, sal_True);
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExport3DShape(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */)
{
    const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        // transformation (UNO_NAME_3D_TRANSFORM_MATRIX == "D3DTransformMatrix")
        uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DTransformMatrix")));
        drawing::HomogenMatrix xHomMat;
        aAny >>= xHomMat;
        SdXMLImExTransform3D aTransform;
        aTransform.AddHomogenMatrix(xHomMat);
        if(aTransform.NeedsAction())
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

        switch(eShapeType)
        {
            case XmlShapeTypeDraw3DCubeObject:
            {
                // write 3DCube shape
                SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DR3D, sXML_3DCube, sal_True, sal_True);
                break;
            }
            case XmlShapeTypeDraw3DSphereObject:
            {
                // write 3DSphere shape
                SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DR3D, sXML_3DSphere, sal_True, sal_True);
                break;
            }
            case XmlShapeTypeDraw3DLatheObject:
            case XmlShapeTypeDraw3DExtrudeObject:
            {
                // write special 3DLathe/3DExtrude attributes
                uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DPolyPolygon3D")));
                drawing::PolyPolygonShape3D xPolyPolygon3D;
                aAny >>= xPolyPolygon3D;

                // look for maximal values
                double fXMin, fXMax, fYMin, fYMax;
                BOOL bInit(FALSE);
                sal_Int32 nOuterSequenceCount(xPolyPolygon3D.SequenceX.getLength());
                drawing::DoubleSequence* pInnerSequenceX = xPolyPolygon3D.SequenceX.getArray();
                drawing::DoubleSequence* pInnerSequenceY = xPolyPolygon3D.SequenceY.getArray();

                for(sal_Int32 a(0L); a < nOuterSequenceCount; a++)
                {
                    sal_Int32 nInnerSequenceCount(pInnerSequenceX->getLength());
                    double* pArrayX = pInnerSequenceX->getArray();
                    double* pArrayY = pInnerSequenceY->getArray();

                    for(sal_Int32 b(0L); b < nInnerSequenceCount; b++)
                    {
                        double fX = *pArrayX++;
                        double fY = *pArrayY++;

                        if(bInit)
                        {
                            if(fX > fXMax)
                                fXMax = fX;

                            if(fX < fXMin)
                                fXMin = fX;

                            if(fY > fYMax)
                                fYMax = fY;

                            if(fY < fYMin)
                                fYMin = fY;
                        }
                        else
                        {
                            fXMin = fXMax = fX;
                            fYMin = fYMax = fY;
                            bInit = TRUE;
                        }
                    }

                    pInnerSequenceX++;
                    pInnerSequenceY++;
                }

                // export ViewBox
                awt::Point aMinPoint(FRound(fXMin), FRound(fYMin));
                awt::Size aMaxSize(FRound(fXMax) - aMinPoint.X, FRound(fYMax) - aMinPoint.Y);
                SdXMLImExViewBox aViewBox(
                    aMinPoint.X, aMinPoint.Y, aMaxSize.Width, aMaxSize.Height);
                rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_viewBox,
                    aViewBox.GetExportString(rExp.GetMM100UnitConverter()));

                // prepare svx:d element export
                SdXMLImExSvgDElement aSvgDElement(aViewBox);
                pInnerSequenceX = xPolyPolygon3D.SequenceX.getArray();
                pInnerSequenceY = xPolyPolygon3D.SequenceY.getArray();

                for(a = 0L; a < nOuterSequenceCount; a++)
                {
                    sal_Int32 nInnerSequenceCount(pInnerSequenceX->getLength());
                    double* pArrayX = pInnerSequenceX->getArray();
                    double* pArrayY = pInnerSequenceY->getArray();
                    drawing::PointSequence aPoly(nInnerSequenceCount);
                    awt::Point* pInnerSequence = aPoly.getArray();

                    for(sal_Int32 b(0L); b < nInnerSequenceCount; b++)
                    {
                        double fX = *pArrayX++;
                        double fY = *pArrayY++;

                        *pInnerSequence = awt::Point(FRound(fX), FRound(fY));
                        pInnerSequence++;
                    }

                    // calculate closed flag
                    awt::Point* pFirst = aPoly.getArray();
                    awt::Point* pLast = pFirst + (nInnerSequenceCount - 1);
                    BOOL bClosed = (pFirst->X == pLast->X && pFirst->Y == pLast->Y);

                    aSvgDElement.AddPolygon(&aPoly, 0L, aMinPoint,
                        aMaxSize, rExp.GetMM100UnitConverter(), bClosed);
                }

                // write point array
                rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_d, aSvgDElement.GetExportString());

                if(eShapeType == XmlShapeTypeDraw3DLatheObject)
                {
                    // write 3DLathe shape
                    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DR3D, sXML_3DLathe, sal_True, sal_True);
                }
                else
                {
                    // write 3DExtrude shape
                    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DR3D, sXML_3DExtrude, sal_True, sal_True);
                }
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpExport3DScene(SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType eShapeType, sal_Int32 nFeatures, awt::Point* pRefPoint)
{
    uno::Reference< container::XIndexAccess > xShapes(xShape, uno::UNO_QUERY);
    if(xShapes.is() && xShapes->getCount())
    {
        // write 3DScene attributes
        const uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
        if(xPropSet.is())
        {
            OUString aStr;
            OUStringBuffer sStringBuffer;

            // rectangle, prepare parameters
            awt::Point aPoint = xShape->getPosition();
            awt::Size aSize = xShape->getSize();
            if( pRefPoint )
            {
                aPoint.X -= pRefPoint->X;
                aPoint.Y -= pRefPoint->Y;
            }

            // svg: x
            if( nFeatures & SEF_EXPORT_X )
            {
                rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.X);
                aStr = sStringBuffer.makeStringAndClear();
                rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_x, aStr);
            }

            // svg: y
            if( nFeatures & SEF_EXPORT_Y )
            {
                rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aPoint.Y);
                aStr = sStringBuffer.makeStringAndClear();
                rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_y, aStr);
            }

            // svg: width
            if( nFeatures & SEF_EXPORT_WIDTH )
            {
                rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Width);
                aStr = sStringBuffer.makeStringAndClear();
                rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_width, aStr);
            }

            // svg: height
            if( nFeatures & SEF_EXPORT_HEIGHT )
            {
                rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, aSize.Height);
                aStr = sStringBuffer.makeStringAndClear();
                rExp.AddAttribute(XML_NAMESPACE_SVG, sXML_height, aStr);
            }

            // world transformation (UNO_NAME_3D_TRANSFORM_MATRIX == "D3DTransformMatrix")
            uno::Any aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DTransformMatrix")));
            drawing::HomogenMatrix xHomMat;
            aAny >>= xHomMat;
            SdXMLImExTransform3D aTransform;
            aTransform.AddHomogenMatrix(xHomMat);
            if(aTransform.NeedsAction())
                rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_transform, aTransform.GetExportString(rExp.GetMM100UnitConverter()));

            // VRP

            // VPN

            // VUP

            // projection "D3DScenePerspective" drawing::ProjectionMode
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DScenePerspective")));
            drawing::ProjectionMode xPrjMode;
            aAny >>= xPrjMode;
            if(xPrjMode == drawing::ProjectionMode_PARALLEL)
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_parallel));
            else
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_perspective));
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_projection, aStr);

            // distance
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneDistance")));
            sal_Int32 nDistance;
            aAny >>= nDistance;
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nDistance);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_distance, aStr);

            // focalLength
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneFocalLength")));
            sal_Int32 nFocalLength;
            aAny >>= nFocalLength;
            rExp.GetMM100UnitConverter().convertMeasure(sStringBuffer, nFocalLength);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_focal_length, aStr);

            // shadowSlant
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneShadowSlant")));
            sal_Int16 nShadowSlant;
            aAny >>= nShadowSlant;
            rExp.GetMM100UnitConverter().convertNumber(sStringBuffer, (sal_Int32)nShadowSlant);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_shadow_slant, aStr);

            // shadeMode
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneShadeMode")));
            drawing::ShadeMode xShadeMode;
            aAny >>= xShadeMode;
            if(xShadeMode == drawing::ShadeMode_FLAT)
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_flat));
            else if(xShadeMode == drawing::ShadeMode_PHONG)
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_phong));
            else if(xShadeMode == drawing::ShadeMode_SMOOTH)
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_gouraud));
            else
                aStr = OUString(RTL_CONSTASCII_USTRINGPARAM(sXML_draft));
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_shade_mode, aStr);

            // ambientColor
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneAmbientColor")));
            sal_Int32 aColTemp;
            Color aAmbientColor;
            aAny >>= aColTemp; aAmbientColor.SetColor(aColTemp);
            rExp.GetMM100UnitConverter().convertColor(sStringBuffer, aAmbientColor);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_ambient_color, aStr);

            // lightingMode
            aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneTwoSidedLighting")));
            sal_Bool bTwoSidedLighting;
            aAny >>= bTwoSidedLighting;
            rExp.GetMM100UnitConverter().convertBool(sStringBuffer, bTwoSidedLighting);
            aStr = sStringBuffer.makeStringAndClear();
            rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_lighting_mode, aStr);

            // write 3DScene shape
            SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DR3D, sXML_3DScene, sal_True, sal_True);

            // write lamps 1..8 as content
            {
                // lightcolor
                Color aLightColor[8];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor1")));
                aAny >>= aColTemp; aLightColor[0].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor2")));
                aAny >>= aColTemp; aLightColor[1].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor3")));
                aAny >>= aColTemp; aLightColor[2].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor4")));
                aAny >>= aColTemp; aLightColor[3].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor5")));
                aAny >>= aColTemp; aLightColor[4].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor6")));
                aAny >>= aColTemp; aLightColor[5].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor7")));
                aAny >>= aColTemp; aLightColor[6].SetColor(aColTemp);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightColor8")));
                aAny >>= aColTemp; aLightColor[7].SetColor(aColTemp);

                // lightdirection
                Vector3D aLightDirection[8];
                drawing::Direction3D xLightDir;
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection1")));
                aAny >>= xLightDir; aLightDirection[0] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection2")));
                aAny >>= xLightDir; aLightDirection[1] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection3")));
                aAny >>= xLightDir; aLightDirection[2] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection4")));
                aAny >>= xLightDir; aLightDirection[3] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection5")));
                aAny >>= xLightDir; aLightDirection[4] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection6")));
                aAny >>= xLightDir; aLightDirection[5] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection7")));
                aAny >>= xLightDir; aLightDirection[6] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightDirection8")));
                aAny >>= xLightDir; aLightDirection[7] = Vector3D(xLightDir.DirectionX, xLightDir.DirectionY, xLightDir.DirectionZ);

                // lighton
                sal_Bool bLightOnOff[8];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn1")));
                aAny >>= bLightOnOff[0];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn2")));
                aAny >>= bLightOnOff[1];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn3")));
                aAny >>= bLightOnOff[2];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn4")));
                aAny >>= bLightOnOff[3];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn5")));
                aAny >>= bLightOnOff[4];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn6")));
                aAny >>= bLightOnOff[5];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn7")));
                aAny >>= bLightOnOff[6];
                aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("D3DSceneLightOn8")));
                aAny >>= bLightOnOff[7];

                for(sal_uInt32 a(0L); a < 8; a++)
                {
                    // lightcolor
                    rExp.GetMM100UnitConverter().convertColor(sStringBuffer, aLightColor[a]);
                    aStr = sStringBuffer.makeStringAndClear();
                    rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_diffuse_color, aStr);

                    // lightdirection
                    rExp.GetMM100UnitConverter().convertVector3D(sStringBuffer, aLightDirection[a]);
                    aStr = sStringBuffer.makeStringAndClear();
                    rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_direction, aStr);

                    // lighton
                    rExp.GetMM100UnitConverter().convertBool(sStringBuffer, bLightOnOff[a]);
                    aStr = sStringBuffer.makeStringAndClear();
                    rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_enabled, aStr);

                    // specular
                    rExp.GetMM100UnitConverter().convertBool(sStringBuffer, (BOOL)(a == 0L));
                    aStr = sStringBuffer.makeStringAndClear();
                    rExp.AddAttribute(XML_NAMESPACE_DR3D, sXML_specular, aStr);

                    // write light entry
                    SvXMLElementExport aOBJ(rExp, XML_NAMESPACE_DR3D, sXML_light, sal_True, sal_True);
                }
            }

            // write members
//          rExp.ImpWriteSingleShapeStyleInfos(xShapes, nFeatures, pRefPoint );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteSingleShapeStyleInfo(const uno::Reference< drawing::XShape >& xShape, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */ )
{
    // get correct shape-style info
    ImpXMLShapeStyleInfo* pInfo =
        (mnShapeStyleInfoIndex < mpShapeStyleInfoList->Count()) ?
        mpShapeStyleInfoList->GetObject(mnShapeStyleInfoIndex) : 0L;
    mnShapeStyleInfoIndex++;

    if(pInfo && pInfo->GetStyleName().getLength())
    {
        XmlShapeType eShapeType(XmlShapeTypeNotYetSet);

        ImpCalcShapeType(xShape, eShapeType);
        ImpWriteSingleShapeStyleInfo(*this, xShape,
            pInfo->GetFamily(), pInfo->GetStyleName(), eShapeType, nFeatures, pRefPoint );
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpCalcShapeType(const uno::Reference< drawing::XShape >& xShape,
    XmlShapeType& eShapeType)
{
    // set in every case, so init here
    eShapeType = XmlShapeTypeUnknown;

    uno::Reference< drawing::XShapeDescriptor > xShapeDescriptor(xShape, uno::UNO_QUERY);
    if(xShapeDescriptor.is())
    {
        String aType((OUString)xShapeDescriptor->getShapeType());

        if(aType.EqualsAscii((const sal_Char*)"com.sun.star.", 0, 13))
        {
            if(aType.EqualsAscii("drawing.", 13, 8))
            {
                // drawing shapes
                if     (aType.EqualsAscii("Rectangle", 21, 9)) { eShapeType = XmlShapeTypeDrawRectangleShape; }
                else if(aType.EqualsAscii("Ellipse", 21, 7)) { eShapeType = XmlShapeTypeDrawEllipseShape; }
                else if(aType.EqualsAscii("Control", 21, 7)) { eShapeType = XmlShapeTypeDrawControlShape; }
                else if(aType.EqualsAscii("Connector", 21, 9)) { eShapeType = XmlShapeTypeDrawConnectorShape; }
                else if(aType.EqualsAscii("Measure", 21, 7)) { eShapeType = XmlShapeTypeDrawMeasureShape; }
                else if(aType.EqualsAscii("Line", 21, 4)) { eShapeType = XmlShapeTypeDrawLineShape; }
                else if(aType.EqualsAscii("PolyPolygon", 21, 11)) { eShapeType = XmlShapeTypeDrawPolyPolygonShape; }
                else if(aType.EqualsAscii("PolyLine", 21, 8)) { eShapeType = XmlShapeTypeDrawPolyLineShape; }
                else if(aType.EqualsAscii("OpenBezier", 21, 10)) { eShapeType = XmlShapeTypeDrawOpenBezierShape; }
                else if(aType.EqualsAscii("ClosedBezier", 21, 12)) { eShapeType = XmlShapeTypeDrawClosedBezierShape; }
                else if(aType.EqualsAscii("GraphicObject", 21, 13)) { eShapeType = XmlShapeTypeDrawGraphicObjectShape; }
                else if(aType.EqualsAscii("Group", 21, 5)) { eShapeType = XmlShapeTypeDrawGroupShape; }
                else if(aType.EqualsAscii("Text", 21, 4)) { eShapeType = XmlShapeTypeDrawTextShape; }
                else if(aType.EqualsAscii("OLE2", 21, 4))
                {
                    eShapeType = XmlShapeTypeDrawOLE2Shape;

                    // get info about presentation shape
                    uno::Reference <beans::XPropertySet> xPropSet(xShape, uno::UNO_QUERY);

                    if(xPropSet.is())
                    {
                        uno::Any aAny;
                        aAny = xPropSet->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("Model")));
                        uno::Reference <lang::XServiceInfo> xObjectInfo;

                        if(aAny >>= xObjectInfo)
                        {
                            if(xObjectInfo->supportsService(OUString(RTL_CONSTASCII_USTRINGPARAM
                                ("com.sun.star.chart.ChartDocument"))))
                            {
                                eShapeType = XmlShapeTypeDrawChartShape;
                            }
                            else if(xObjectInfo->supportsService(OUString(RTL_CONSTASCII_USTRINGPARAM
                                ("com.sun.star.sheet.SpreadsheetDocument"))))
                            {
                                eShapeType = XmlShapeTypeDrawTableShape;
                            }
                            else
                            {
                                // general OLE2 Object
                            }
                        }
                    }
                }
                else if(aType.EqualsAscii("Page", 21, 4)) { eShapeType = XmlShapeTypeDrawPageShape; }
                else if(aType.EqualsAscii("Frame", 21, 5)) { eShapeType = XmlShapeTypeDrawFrameShape; }
                else if(aType.EqualsAscii("Caption", 21, 6)) { eShapeType = XmlShapeTypeDrawCaptionShape; }

                // 3D shapes
                else if(aType.EqualsAscii("Scene", 21 + 7, 5)) { eShapeType = XmlShapeTypeDraw3DSceneObject; }
                else if(aType.EqualsAscii("Cube", 21 + 7, 4)) { eShapeType = XmlShapeTypeDraw3DCubeObject; }
                else if(aType.EqualsAscii("Sphere", 21 + 7, 6)) { eShapeType = XmlShapeTypeDraw3DSphereObject; }
                else if(aType.EqualsAscii("Lathe", 21 + 7, 5)) { eShapeType = XmlShapeTypeDraw3DLatheObject; }
                else if(aType.EqualsAscii("Extrude", 21 + 7, 7)) { eShapeType = XmlShapeTypeDraw3DExtrudeObject; }
            }
            else if(aType.EqualsAscii("presentation.", 13, 13))
            {
                // presentation shapes
                if     (aType.EqualsAscii("TitleText", 26, 9)) { eShapeType = XmlShapeTypePresTitleTextShape; }
                else if(aType.EqualsAscii("Outliner", 26, 8)) { eShapeType = XmlShapeTypePresOutlinerShape;  }
                else if(aType.EqualsAscii("Subtitle", 26, 8)) { eShapeType = XmlShapeTypePresSubtitleShape;  }
                else if(aType.EqualsAscii("GraphicObject", 26, 13)) { eShapeType = XmlShapeTypePresGraphicObjectShape;  }
                else if(aType.EqualsAscii("Page", 26, 4)) { eShapeType = XmlShapeTypePresPageShape;  }
                else if(aType.EqualsAscii("OLE2", 26, 4)) { eShapeType = XmlShapeTypePresOLE2Shape; }
                else if(aType.EqualsAscii("Chart", 26, 5)) { eShapeType = XmlShapeTypePresChartShape;  }
                else if(aType.EqualsAscii("Table", 26, 5)) { eShapeType = XmlShapeTypePresTableShape;  }
                else if(aType.EqualsAscii("OrgChart", 26, 8)) { eShapeType = XmlShapeTypePresOrgChartShape;  }
                else if(aType.EqualsAscii("Notes", 26, 5)) { eShapeType = XmlShapeTypePresNotesShape;  }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpWriteSingleShapeStyleInfo(
    SvXMLExport& rExp,
    const uno::Reference< drawing::XShape >& xShape,
    sal_uInt16 nFamily, const OUString& rStyleName, XmlShapeType eShapeType,
    sal_Int32 nFeatures /* = SEF_DEFAULT */,
    com::sun::star::awt::Point* pRefPoint /* = NULL */ )
{
    if( rStyleName.getLength() )
    {
        // add style-name attribute (REQUIRED) Style-name, evtl. auto-style
        if(XML_STYLE_FAMILY_SD_GRAPHICS_ID == nFamily)
            rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_style_name, rStyleName);
        else
            rExp.AddAttribute(XML_NAMESPACE_PRESENTATION, sXML_style_name, rStyleName);
    }

    // export shape name if he has one
    uno::Reference< container::XNamed > xNamed( xShape, uno::UNO_QUERY );
    if( xNamed.is() )
    {
        const OUString aName( xNamed->getName() );
        if( aName.getLength() )
            rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_name, aName );
    }

    // export shape id if needed
    sal_Int32 nShapeId = rExp.GetShapeExport()->getShapeId( xShape );
    if( nShapeId != -1 )
    {
        const OUString sId( OUString::valueOf( nShapeId ) );
        rExp.AddAttribute(XML_NAMESPACE_DRAW, sXML_id, sId );
    }

    switch(eShapeType)
    {
        case XmlShapeTypeDrawRectangleShape:
        {
            ImpExportRectangleShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }
        case XmlShapeTypeDrawEllipseShape:
        {
            ImpExportEllipseShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }
        case XmlShapeTypeDrawLineShape:
        {
            ImpExportLineShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }
        case XmlShapeTypeDrawPolyPolygonShape:  // closed PolyPolygon
        case XmlShapeTypeDrawPolyLineShape:     // open PolyPolygon
        case XmlShapeTypeDrawClosedBezierShape: // closed PolyPolygon containing curves
        case XmlShapeTypeDrawOpenBezierShape:   // open PolyPolygon containing curves
        {
            ImpExportPolygonShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawTextShape:
        case XmlShapeTypePresTitleTextShape:
        case XmlShapeTypePresOutlinerShape:
        case XmlShapeTypePresSubtitleShape:
        case XmlShapeTypePresNotesShape:
        {
            ImpExportTextBoxShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawGraphicObjectShape:
        case XmlShapeTypePresGraphicObjectShape:
        {
            ImpExportGraphicObjectShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawChartShape:
        case XmlShapeTypePresChartShape:
        {
            ImpExportChartShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawTableShape:
        case XmlShapeTypePresTableShape:
        {
            ImpExportSpreadsheetShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawControlShape:
        {
            ImpExportControlShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawConnectorShape:
        {
            ImpExportConnectorShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawMeasureShape:
        {
            ImpExportMeasureShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawOLE2Shape:
        case XmlShapeTypePresOLE2Shape:
        {
            ImpExportOLE2Shape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawPageShape:
        case XmlShapeTypePresPageShape:
        {
            ImpExportPageShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDrawCaptionShape:
        {
            ImpExportCaptionShape(rExp, xShape, eShapeType, nFeatures, pRefPoint );
            break;
        }

        case XmlShapeTypeDraw3DCubeObject:
        case XmlShapeTypeDraw3DSphereObject:
        case XmlShapeTypeDraw3DLatheObject:
        case XmlShapeTypeDraw3DExtrudeObject:
        {
            ImpExport3DShape((SdXMLExport&)rExp, xShape, eShapeType);
            break;
        }

        case XmlShapeTypeDraw3DSceneObject:
        {
            // empty 3dscene
            DBG_ASSERT(FALSE, "XMLEXP: WriteShape: empty 3DScene in export, not exported.");
            break;
        }

        case XmlShapeTypeDrawGroupShape:
        {
            // empty group
            DBG_ASSERT(FALSE, "XMLEXP: WriteShape: empty GroupShape in export, not exported.");
            break;
        }

        case XmlShapeTypePresOrgChartShape:
        case XmlShapeTypeDrawFrameShape:
        case XmlShapeTypeUnknown:
        case XmlShapeTypeNotYetSet:
        default:
        {
            // this should never happen and is an error
            DBG_ERROR("XMLEXP: WriteShape: unknown or unexpected type of shape in export!");
            break;
        }
    }
}

void SdXMLExport::ImpWriteSingleShapeStyleInfos(uno::Reference< container::XIndexAccess >& xShapes, sal_Int32 nFeatures /* = SEF_DEFAULT */, awt::Point* pRefPoint /* = NULL */ )
{
    const sal_Int32 nShapeCount(xShapes->getCount());
    sal_Int32 nShapeId;

    // loop over shapes
    for(nShapeId = 0L; nShapeId < nShapeCount; nShapeId++)
    {
        uno::Any aAny(xShapes->getByIndex(nShapeId));
        uno::Reference< drawing::XShape > xShape;

        if(aAny >>= xShape)
        {
            uno::Reference< container::XIndexAccess > xShapes(xShape, uno::UNO_QUERY);
            if(xShapes.is() && xShapes->getCount())
            {
                // group shape or 3Dscene?
                sal_Bool bIsScene(FALSE);
                uno::Reference< drawing::XShapeDescriptor > xShapeDescriptor(xShape, uno::UNO_QUERY);
                if(xShapeDescriptor.is())
                {
                    String aType((OUString)xShapeDescriptor->getShapeType());
                    if(aType.EqualsAscii((const sal_Char*)"com.sun.star.drawing.Shape3DSceneObject"))
                    {
                        bIsScene = TRUE;
                    }
                }

                if(bIsScene)
                {
                    // write 3DScene
                    ImpExport3DScene(*this, xShape, XmlShapeTypeDraw3DSceneObject, nFeatures, pRefPoint);
                }
                else
                {
                    // write group shape
                    SvXMLElementExport aPGR(*this, XML_NAMESPACE_DRAW, sXML_g, sal_True, sal_True);

                    // write members
                    ImpWriteSingleShapeStyleInfos(xShapes, nFeatures, pRefPoint);
                }
            }
            else
            {
                // single shape
                ImpWriteSingleShapeStyleInfo(xShape, nFeatures, pRefPoint );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::ImpPrepSingleShapeStyleInfo(uno::Reference< drawing::XShape >& xShape,
    const OUString& rPrefix)
{
    uno::Reference< beans::XPropertySet > xPropSet(xShape, uno::UNO_QUERY);
    if(xPropSet.is())
    {
        OUString aParentName;
        OUString aNewName;
        uno::Reference< style::XStyle > xStyle;
        sal_Int32 nFamily(XML_STYLE_FAMILY_SD_GRAPHICS_ID);

        uno::Any aAny = xPropSet->getPropertyValue(
            OUString(RTL_CONSTASCII_USTRINGPARAM("Style")));
        if(aAny >>= xStyle)
        {
            // get family ID
            uno::Reference< beans::XPropertySet > xStylePropSet(xStyle, uno::UNO_QUERY);
            if(xStylePropSet.is())
            {
                OUString aFamilyName;
                aAny = xStylePropSet->getPropertyValue(
                    OUString(RTL_CONSTASCII_USTRINGPARAM("Family")));
                if(aAny >>= aFamilyName)
                {
                    if(aFamilyName.getLength() && aFamilyName.equals(
                        OUString(RTL_CONSTASCII_USTRINGPARAM("presentation"))))
                    {
                        nFamily = XML_STYLE_FAMILY_SD_PRESENTATION_ID;
                    }
                }
            }

            // get parent-style name
            if(XML_STYLE_FAMILY_SD_PRESENTATION_ID == nFamily && rPrefix.getLength())
            {
                aParentName = rPrefix;
                aParentName += xStyle->getName();
            }
            else
                aParentName = xStyle->getName();
        }

        // filter propset
        std::vector< XMLPropertyState > xPropStates = GetPropertySetMapper()->Filter( xPropSet );

        if(!xPropStates.size())
        {
            // no hard attributes, use parent style name for export
            aNewName = aParentName;
        }
        else
        {
            // there are filtered properties -> hard attributes
            // try to find this style in AutoStylePool
            aNewName = GetAutoStylePool()->Find(nFamily, aParentName, xPropStates);

            if(!aNewName.getLength())
            {
                // Style did not exist, add it to AutoStalePool
                aNewName = GetAutoStylePool()->Add(nFamily, aParentName, xPropStates);
            }
        }

        // in aNewName is the StyleInfo to be used for exporting this object.
        // Remember: String maybe still empty due to objects without style,
        // like PageObjects(!)
        // Now remember this association in helper class for later export
        ImpXMLShapeStyleInfo* pInfo = new ImpXMLShapeStyleInfo(aNewName, nFamily);
        mpShapeStyleInfoList->Insert(pInfo, LIST_APPEND);

        // prep text styles
        uno::Reference< text::XText > xText(xShape, uno::UNO_QUERY);
        if(xText.is())
        {
            uno::Reference< beans::XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );

            sal_Bool bIsEmptyPresObj = sal_False;

            if( xPropSetInfo.is() && xPropSetInfo->hasPropertyByName(msEmptyPres))
            {
                uno::Any aAny = xPropSet->getPropertyValue(msEmptyPres);
                aAny >>= bIsEmptyPresObj;
            }

            if(!bIsEmptyPresObj)
            {
                GetTextParagraphExport()->collectTextAutoStyles( xText );
            }
        }

        // check for calc ole
        const OUString aShapeType( xShape->getShapeType() );
        if( aShapeType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "com.sun.star.drawing.OLE2Shape" )) ||
            aShapeType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "com.sun.star.presentation.CalcShape" )) )
        {
            uno::Reference< chart::XChartDocument > xChartDoc;
            uno::Any aAny( xPropSet->getPropertyValue( msModel ) );
            aAny >>= xChartDoc;
            if( xChartDoc.is() )
            {
                GetChartExport()->collectAutoStyles( xChartDoc );
            }
        }

        // check for connector
        if( aShapeType.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "com.sun.star.drawing.ConnectorShape" ) ) )
        {
            uno::Reference< drawing::XShape > xConnection;

            // create shape ids for export later
            if( xPropSet->getPropertyValue( msStartShape ) >>= xConnection )
            {
                GetShapeExport()->createShapeId( xConnection );
            }
            if( xPropSet->getPropertyValue( msEndShape ) >>= xConnection )
            {
                GetShapeExport()->createShapeId( xConnection );
            }
        }
    }
}

void SdXMLExport::ImpPrepSingleShapeStyleInfos(uno::Reference< container::XIndexAccess >& xShapes,
    const OUString& rPrefix)
{
    const sal_Int32 nShapeCount(xShapes->getCount());
    sal_Int32 nShapeId;

    // loop over shapes
    for(nShapeId = 0L; nShapeId < nShapeCount; nShapeId++)
    {
        uno::Any aAny(xShapes->getByIndex(nShapeId));
        uno::Reference< drawing::XShape > xShape;

        if(aAny >>= xShape)
        {
            uno::Reference< container::XIndexAccess > xShapes(xShape, uno::UNO_QUERY);
            if(xShapes.is() && xShapes->getCount())
            {
                // group shape
                ImpPrepSingleShapeStyleInfos(xShapes, rPrefix);
            }
            else
            {
                // single shape
                ImpPrepSingleShapeStyleInfo(xShape, rPrefix);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::_ExportStyles(BOOL bUsed)
{
    // export fill styles
    SvXMLExport::_ExportStyles( bUsed );

    // prepare and write default-draw-style-pool
    ImpWriteDefaultStyleInfos();

    // write draw:style-name for object graphic-styles
    ImpWriteObjGraphicStyleInfos();

    // prepare page-master infos
    ImpPrepPageMasterInfos();

    // write page-master infos
    ImpWritePageMasterInfos();

    // write presentation styles
    ImpWritePresentationStyles();

    // prepare draw:style-name for page export
    ImpPrepDrawPageInfos();

    // write draw:style-name for page export
    ImpWriteDrawPageInfos();

    // write draw:auto-layout-name for page export
    ImpWriteAutoLayoutInfos();
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::_ExportAutoStyles()
{
    // create auto style infos for objects on master pages
    for(sal_Int32 nMPageId(0L); nMPageId < mnDocMasterPageCount; nMPageId++)
    {
        uno::Any aAny(mxDocMasterPages->getByIndex(nMPageId));
        uno::Reference< drawing::XDrawPage > xMasterPage;

        if(aAny >>= xMasterPage)
        {
            // get MasterPage Name
            OUString aMasterPageNamePrefix;
            uno::Reference < container::XNamed > xNamed(xMasterPage, uno::UNO_QUERY);
            if(xNamed.is())
            {
                aMasterPageNamePrefix = xNamed->getName();
            }
            if(aMasterPageNamePrefix.getLength())
            {
                aMasterPageNamePrefix += OUString(RTL_CONSTASCII_USTRINGPARAM("-"));
            }

            uno::Reference< container::XIndexAccess > xShapes(xMasterPage, uno::UNO_QUERY);
            if(xShapes.is() && xShapes->getCount())
                ImpPrepSingleShapeStyleInfos(xShapes, aMasterPageNamePrefix);

            if(IsImpress())
            {
                uno::Reference< presentation::XPresentationPage > xPresPage(xMasterPage, uno::UNO_QUERY);
                if(xPresPage.is())
                {
                    uno::Reference< drawing::XDrawPage > xNotesPage(xPresPage->getNotesPage());
                    if(xNotesPage.is())
                    {
                        uno::Reference< container::XIndexAccess > xShapes(xNotesPage, uno::UNO_QUERY);
                        if(xShapes.is() && xShapes->getCount())
                            ImpPrepSingleShapeStyleInfos(xShapes, aMasterPageNamePrefix);
                    }
                }
            }
        }
    }

    // create auto style infos for objects on pages
    for(sal_Int32 nPageInd(0); nPageInd < mnDocDrawPageCount; nPageInd++)
    {
        uno::Any aAny(mxDocDrawPages->getByIndex(nPageInd));
        uno::Reference<drawing::XDrawPage> xDrawPage;

        if(aAny >>= xDrawPage)
        {
            // get MasterPage Name
            OUString aMasterPageNamePrefix;
            uno::Reference < drawing::XMasterPageTarget > xMasterPageInt(xDrawPage, uno::UNO_QUERY);
            if(xMasterPageInt.is())
            {
                uno::Reference<drawing::XDrawPage> xUsedMasterPage(xMasterPageInt->getMasterPage());
                if(xUsedMasterPage.is())
                {
                    uno::Reference < container::XNamed > xMasterNamed(xUsedMasterPage, uno::UNO_QUERY);
                    if(xMasterNamed.is())
                    {
                        aMasterPageNamePrefix = xMasterNamed->getName();
                    }
                }
            }
            if(aMasterPageNamePrefix.getLength())
            {
                aMasterPageNamePrefix += OUString(RTL_CONSTASCII_USTRINGPARAM("-"));
            }

            // prepare object infos
            uno::Reference< container::XIndexAccess > xShapes(xDrawPage, uno::UNO_QUERY);
            if(xShapes.is() && xShapes->getCount())
                ImpPrepSingleShapeStyleInfos(xShapes, aMasterPageNamePrefix);

            // prepare presentation notes page object infos (ONLY if presentation)
            if(IsImpress())
            {
                uno::Reference< presentation::XPresentationPage > xPresPage(xDrawPage, uno::UNO_QUERY);
                if(xPresPage.is())
                {
                    uno::Reference< drawing::XDrawPage > xNotesPage(xPresPage->getNotesPage());
                    if(xNotesPage.is())
                    {
                        uno::Reference< container::XIndexAccess > xShapes(xNotesPage, uno::UNO_QUERY);
                        if(xShapes.is() && xShapes->getCount())
                            ImpPrepSingleShapeStyleInfos(xShapes, aMasterPageNamePrefix);
                    }
                }
            }
        }
    }

    GetShapeExport()->exportAutoStyles();

    // ...for text
    GetTextParagraphExport()->exportTextAutoStyles();

    // ...for chart
    GetChartExport()->exportAutoStyles();
}

//////////////////////////////////////////////////////////////////////////////

void SdXMLExport::_ExportMasterStyles()
{
    // export MasterPages in master-styles section
    for(sal_Int32 nMPageId = 0L; nMPageId < mnDocMasterPageCount; nMPageId++)
    {
        uno::Any aAny(mxDocMasterPages->getByIndex(nMPageId));
        uno::Reference< drawing::XDrawPage > xMasterPage;

        if(aAny >>= xMasterPage)
        {
            // prepare masterpage attributes
            OUString sMasterPageName;
            uno::Reference < container::XNamed > xNamed(xMasterPage, uno::UNO_QUERY);
            if(xNamed.is())
            {
                sMasterPageName = xNamed->getName();
                AddAttribute(XML_NAMESPACE_STYLE, sXML_name, sMasterPageName);
            }

            ImpXMLEXPPageMasterInfo* pInfo = mpPageMaterUsageList->GetObject(nMPageId);
            if(pInfo)
            {
                OUString sString = pInfo->GetName();
                AddAttribute(XML_NAMESPACE_STYLE, sXML_page_master_name, sString);
            }

            // write masterpage
            SvXMLElementExport aMPG(*this, XML_NAMESPACE_STYLE, sXML_master_page, sal_True, sal_True);

            // write graphic objects on this master page (if any)
            uno::Reference< container::XIndexAccess > xShapes(xMasterPage, uno::UNO_QUERY);
            if(xShapes.is() && xShapes->getCount())
                ImpWriteSingleShapeStyleInfos(xShapes);

            // write presentation notes (ONLY if presentation)
            if(IsImpress())
            {
                uno::Reference< presentation::XPresentationPage > xPresPage(xMasterPage, uno::UNO_QUERY);
                if(xPresPage.is())
                {
                    uno::Reference< drawing::XDrawPage > xNotesPage(xPresPage->getNotesPage());
                    if(xNotesPage.is())
                    {
                        uno::Reference< container::XIndexAccess > xShapes(xNotesPage, uno::UNO_QUERY);
                        if(xShapes.is() && xShapes->getCount())
                        {
                            // write presentation notes
                            SvXMLElementExport aPSY(*this, XML_NAMESPACE_PRESENTATION, sXML_notes, sal_True, sal_True);

                            // write shapes per se
                            ImpWriteSingleShapeStyleInfos(xShapes);
                        }
                    }
                }
            }
        }
    }
}


