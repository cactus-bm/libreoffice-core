/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ChartPlotAreaOOoTContext.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 15:37:31 $
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
#include "ChartPlotAreaOOoTContext.hxx"

#ifndef _XMLOFF_TRANSFORMER_BASE_HXX
#include "TransformerBase.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif
#ifndef _XMLOFF_DEEPTCONTEXT_HXX
#include "DeepTContext.hxx"
#endif
#ifndef _XMLOFF_ACTIONMAPTYPESOOO_HXX
#include "ActionMapTypesOOo.hxx"
#endif
#ifndef _XMLOFF_MUTABLEATTRLIST_HXX
#include "MutableAttrList.hxx"
#endif

using namespace ::com::sun::star;
using namespace ::xmloff::token;

using ::com::sun::star::uno::Reference;
using ::rtl::OUString;

class XMLAxisOOoContext : public XMLPersElemContentTContext
{
public:
    TYPEINFO();

    XMLAxisOOoContext( XMLTransformerBase& rTransformer,
                       const ::rtl::OUString& rQName );
    ~XMLAxisOOoContext();

    virtual void StartElement( const Reference< xml::sax::XAttributeList >& rAttrList );

    bool IsCategoryAxis() const;

private:
    bool m_bIsCategoryAxis;
};

TYPEINIT1( XMLAxisOOoContext, XMLPersElemContentTContext );

XMLAxisOOoContext::XMLAxisOOoContext(
    XMLTransformerBase& rTransformer,
    const ::rtl::OUString& rQName ) :
        XMLPersElemContentTContext( rTransformer, rQName ),
        m_bIsCategoryAxis( false )
{}

XMLAxisOOoContext::~XMLAxisOOoContext()
{}

void XMLAxisOOoContext::StartElement(
    const Reference< xml::sax::XAttributeList >& rAttrList )
{
    OUString aLocation, aMacroName;
    Reference< xml::sax::XAttributeList > xAttrList( rAttrList );
    XMLMutableAttributeList *pMutableAttrList = 0;
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );
        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetTransformer().GetNamespaceMap().GetKeyByAttrName( rAttrName, &aLocalName );

        if( nPrefix == XML_NAMESPACE_CHART &&
            IsXMLToken( aLocalName, XML_CLASS ) )
        {
            if( !pMutableAttrList )
            {
                pMutableAttrList = new XMLMutableAttributeList( xAttrList );
                xAttrList = pMutableAttrList;
            }

            const OUString& rAttrValue = xAttrList->getValueByIndex( i );
            XMLTokenEnum eToken = XML_TOKEN_INVALID;
            if( IsXMLToken( rAttrValue, XML_DOMAIN ) ||
                IsXMLToken( rAttrValue, XML_CATEGORY ))
            {
                eToken = XML_X;
                if( IsXMLToken( rAttrValue, XML_CATEGORY ) )
                    m_bIsCategoryAxis = true;
            }
            else if( IsXMLToken( rAttrValue, XML_VALUE ))
            {
                eToken = XML_Y;
            }
            else if( IsXMLToken( rAttrValue, XML_SERIES ))
            {
                eToken = XML_Z;
            }
            else
            {
                OSL_ENSURE( false, "ChartAxis: Invalid attribute value" );
            }

            if( eToken != XML_TOKEN_INVALID )
            {
                OUString aNewAttrQName(
                    GetTransformer().GetNamespaceMap().GetQNameByKey(
                        XML_NAMESPACE_CHART, GetXMLToken( XML_DIMENSION )));
                pMutableAttrList->RenameAttributeByIndex( i, aNewAttrQName );
                pMutableAttrList->SetValueByIndex( i, GetXMLToken( eToken ));
            }
        }
    }

    XMLPersElemContentTContext::StartElement( xAttrList );
}

bool XMLAxisOOoContext::IsCategoryAxis() const
{
    return m_bIsCategoryAxis;
}


TYPEINIT1( XMLChartPlotAreaOOoTContext, XMLProcAttrTransformerContext )

XMLChartPlotAreaOOoTContext::XMLChartPlotAreaOOoTContext(
    XMLTransformerBase & rTransformer, const ::rtl::OUString & rQName ) :
        XMLProcAttrTransformerContext( rTransformer, rQName, OOO_SHAPE_ACTIONS )
{
}

XMLChartPlotAreaOOoTContext::~XMLChartPlotAreaOOoTContext()
{}

XMLTransformerContext * XMLChartPlotAreaOOoTContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const ::rtl::OUString& rLocalName,
    const ::rtl::OUString& rQName,
    const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
    XMLTransformerContext *pContext = 0;

    if( XML_NAMESPACE_CHART == nPrefix &&
        IsXMLToken( rLocalName, XML_AXIS ) )
    {
        XMLAxisOOoContext * pAxisContext( new XMLAxisOOoContext( GetTransformer(), rQName ));
        AddContent( pAxisContext );
        pContext = pAxisContext;
    }
    else if( XML_NAMESPACE_CHART == nPrefix &&
             IsXMLToken( rLocalName, XML_CATEGORIES ) )
    {
        pContext = new XMLPersAttrListTContext( GetTransformer(), rQName );

        // put categories at correct axis
        XMLAxisContextVector::iterator aIter = m_aChildContexts.begin();
        bool bFound =false;

        // iterate over axis elements
        for( ; ! bFound && aIter != m_aChildContexts.end(); ++aIter )
        {
            XMLAxisOOoContext * pAxisContext = (*aIter).get();
            if( pAxisContext != 0 )
            {
                // iterate over attributes to find category axis
                Reference< xml::sax::XAttributeList > xNewAttrList( pAxisContext->GetAttrList());
                sal_Int16 nAttrCount = xNewAttrList.is() ? xNewAttrList->getLength() : 0;

                for( sal_Int16 i=0; i < nAttrCount; i++ )
                {
                    const OUString & rAttrName = xNewAttrList->getNameByIndex( i );
                    OUString aLocalName;
                    sal_uInt16 nNewPrefix =
                        GetTransformer().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                                             &aLocalName );
                    if( nNewPrefix == XML_NAMESPACE_CHART &&
                        pAxisContext->IsCategoryAxis() &&
                        IsXMLToken( aLocalName, XML_DIMENSION ) )
                    {
                        // category axis found
                        pAxisContext->AddContent( pContext );
                        bFound = true;
                        break;
                    }
                }
            }
        }
        OSL_ENSURE( bFound, "No suitable axis for categories found." );
    }
    else
    {
        ExportContent();
        pContext =  XMLProcAttrTransformerContext::CreateChildContext(
            nPrefix, rLocalName, rQName, xAttrList );
    }

    return pContext;
}

void XMLChartPlotAreaOOoTContext::EndElement()
{
    ExportContent();
    XMLProcAttrTransformerContext::EndElement();
}

void XMLChartPlotAreaOOoTContext::AddContent( XMLAxisOOoContext *pContext )
{
    OSL_ENSURE( pContext && pContext->IsPersistent(),
                "non-persistent context" );
    XMLAxisContextVector::value_type aVal( pContext );
    m_aChildContexts.push_back( aVal );
}


void XMLChartPlotAreaOOoTContext::ExportContent()
{
    XMLAxisContextVector::iterator aIter = m_aChildContexts.begin();

    for( ; aIter != m_aChildContexts.end(); ++aIter )
    {
        (*aIter)->Export();
    }

    m_aChildContexts.clear();
}
