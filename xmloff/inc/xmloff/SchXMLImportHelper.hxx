/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SchXMLImportHelper.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 16:04:04 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#ifndef _XMLOFF_SCH_XMLIMPORTHELPER_HXX_
#define _XMLOFF_SCH_XMLIMPORTHELPER_HXX_

#ifndef _UNIVERSALL_REFERENCE_HXX
#include <xmloff/uniref.hxx>
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include <xmloff/families.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_XSTRINGMAPPING_HPP_
#include <com/sun/star/util/XStringMapping.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XCHARTDOCUMENT_HPP_
#include <com/sun/star/chart/XChartDocument.hpp>
#endif

namespace com { namespace sun { namespace star {
    namespace frame {
        class XModel;
    }
    namespace task {
        class XStatusIndicator;
    }
    namespace xml {
        namespace sax {
            class XAttributeList;
        }
    }
    namespace chart2 {
        namespace data {
            class XDataProvider;
            class XLabeledDataSequence;
        }
        class XChartDocument;
        class XDataSeries;
    }
}}}

class SvXMLUnitConverter;
class SvXMLStylesContext;
class XMLChartImportPropertyMapper;
class SvXMLTokenMap;
class SvXMLImportContext;
class SvXMLImport;

// ========================================

/** With this class you can import a <chart:chart> element containing
    its data as <table:table> element or without internal table. In
    the latter case you have to provide a table address mapper that
    converts table addresses in XML format to the appropriate application
    format.
 */
class SchXMLImportHelper : public UniRefBase
{
private:
    com::sun::star::uno::Reference< com::sun::star::chart::XChartDocument > mxChartDoc;
    SvXMLStylesContext* mpAutoStyles;

    SvXMLTokenMap* mpChartDocElemTokenMap;
    SvXMLTokenMap* mpTableElemTokenMap;
    SvXMLTokenMap* mpChartElemTokenMap;
    SvXMLTokenMap* mpPlotAreaElemTokenMap;
    SvXMLTokenMap* mpSeriesElemTokenMap;
    SvXMLTokenMap* mpAxisElemTokenMap;

    SvXMLTokenMap* mpChartAttrTokenMap;
    SvXMLTokenMap* mpPlotAreaAttrTokenMap;
    SvXMLTokenMap* mpAxisAttrTokenMap;
    SvXMLTokenMap* mpLegendAttrTokenMap;
    SvXMLTokenMap* mpAutoStyleAttrTokenMap;
    SvXMLTokenMap* mpCellAttrTokenMap;
    SvXMLTokenMap* mpSeriesAttrTokenMap;

public:

    SchXMLImportHelper();
    ~SchXMLImportHelper();

    /** get the context for reading the <chart:chart> element with subelements.
        The result is stored in the XModel given if it also implements
        XChartDocument
     */
    SvXMLImportContext* CreateChartContext(
        SvXMLImport& rImport,
        sal_uInt16 nPrefix, const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference<
            com::sun::star::frame::XModel > xChartModel,
        const com::sun::star::uno::Reference<
            com::sun::star::xml::sax::XAttributeList >& xAttrList );

    /** set the auto-style context that will be used to retrieve auto-styles
        used inside the following <chart:chart> element to parse
     */
    void SetAutoStylesContext( SvXMLStylesContext* pAutoStyles ) { mpAutoStyles = pAutoStyles; }
    SvXMLStylesContext* GetAutoStylesContext() const { return mpAutoStyles; }

    const com::sun::star::uno::Reference<
        com::sun::star::chart::XChartDocument >& GetChartDocument()
        { return mxChartDoc; }

    const SvXMLTokenMap& GetDocElemTokenMap();
    const SvXMLTokenMap& GetTableElemTokenMap();
    const SvXMLTokenMap& GetChartElemTokenMap();
    const SvXMLTokenMap& GetPlotAreaElemTokenMap();
    const SvXMLTokenMap& GetSeriesElemTokenMap();
    const SvXMLTokenMap& GetAxisElemTokenMap();

    const SvXMLTokenMap& GetChartAttrTokenMap();
    const SvXMLTokenMap& GetPlotAreaAttrTokenMap();
    const SvXMLTokenMap& GetAxisAttrTokenMap();
    const SvXMLTokenMap& GetLegendAttrTokenMap();
    const SvXMLTokenMap& GetAutoStyleAttrTokenMap();
    const SvXMLTokenMap& GetCellAttrTokenMap();
    const SvXMLTokenMap& GetSeriesAttrTokenMap();

    static sal_uInt16 GetChartFamilyID() { return XML_STYLE_FAMILY_SCH_CHART_ID; }

    sal_Int32   GetNumberOfSeries();
    sal_Int32   GetLengthOfSeries();
    void        ResizeChartData( sal_Int32 nSeries, sal_Int32 nDataPoints = -1 );

    // if no data provider exists by, now the model (as XChild) is asked for its
    // parent which creates the data provider that is finally set at the chart
    // document
    static ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataProvider >
        GetDataProvider( const ::com::sun::star::uno::Reference<
                             ::com::sun::star::chart2::XChartDocument > & xChartDoc );

    /** @param bPushLastChartType If </FALSE>, in case a new chart type has to
               be added (because it does not exist yet), it is appended at the
               end of the chart-type container.  When </TRUE>, a new chart type
               is added at one position before the last one, i.e. the formerly
               last chart type is pushed back, so that it remains the last one.

               This is needed when the global chart type is set to type A, but
               the first series has type B. Then B should appear before A (done
               by passing true).  Once a series of type A has been read,
               following new chart types are again be added at the end (by
               passing false).
     */
    static ::com::sun::star::uno::Reference<
                ::com::sun::star::chart2::XDataSeries > GetNewDataSeries(
                    const ::com::sun::star::uno::Reference<
                        ::com::sun::star::chart2::XChartDocument > & xDoc,
                    sal_Int32 nCoordinateSystemIndex,
                    const ::rtl::OUString & rChartTypeName,
                    bool bPushLastChartType = false );

    static void DeleteDataSeries(
                    const ::com::sun::star::uno::Reference<
                        ::com::sun::star::chart2::XDataSeries >& xSeries,
                    const ::com::sun::star::uno::Reference<
                        ::com::sun::star::chart2::XChartDocument > & xDoc );

    static ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::data::XLabeledDataSequence > GetNewLabeledDataSequence();
};

#endif  // _XMLOFF_SCH_XMLIMPORTHELPER_HXX_
