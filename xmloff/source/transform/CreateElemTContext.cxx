/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CreateElemTContext.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 11:22:01 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmloff.hxx"

#ifndef _XMLOFF_CREATEELEMTCONTEXT_HXX
#include "CreateElemTContext.hxx"
#endif
#ifndef _XMLOFF_MUTABLEATTRLIST_HXX
#include "MutableAttrList.hxx"
#endif
#ifndef _XMLOFF_TRANSFORMERBASE_HXX
#include "TransformerBase.hxx"
#endif
#ifndef _XMLOFF_TRANSFORMERACTIONS_HXX
#include "TransformerActions.hxx"
#endif
#ifndef _XMLOFF_TCONTEXTVECTOR_HXX
#include "TContextVector.hxx"
#endif
#ifndef _XMLOFF_FLATTCONTEXT_HXX
#include "FlatTContext.hxx"
#endif
#ifndef _XMLOFF_ATTRTRANSFORMERACTION_HXX
#include "AttrTransformerAction.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif

using ::rtl::OUString;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::xmloff::token;

TYPEINIT1( XMLCreateElemTransformerContext, XMLTransformerContext );

XMLCreateElemTransformerContext::XMLCreateElemTransformerContext(
        XMLTransformerBase& rImp,
        const OUString& rQName,
       sal_uInt16 nActionMap ) :
    XMLTransformerContext( rImp, rQName ),
    m_nActionMap( nActionMap )
{
}

XMLCreateElemTransformerContext::~XMLCreateElemTransformerContext()
{
}

void XMLCreateElemTransformerContext::StartElement(
        const Reference< XAttributeList >& rAttrList )
{
    Reference< XAttributeList > xAttrList( rAttrList );

    XMLTransformerContextVector aChildContexts;

    XMLMutableAttributeList *pMutableAttrList = 0;
    XMLTransformerActions *pActions =
        GetTransformer().GetUserDefinedActions( m_nActionMap );
    OSL_ENSURE( pActions, "go no actions" );
    if( pActions )
    {
        sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
        for( sal_Int16 i=0; i < nAttrCount; ++i )
        {
            const OUString& rAttrName = xAttrList->getNameByIndex( i );
            const OUString& rAttrValue = xAttrList->getValueByIndex( i );
            OUString aLocalName;
            sal_uInt16 nPrefix =
                GetTransformer().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                           &aLocalName );

            XMLTransformerActions::key_type aKey( nPrefix, aLocalName );
            XMLTransformerActions::const_iterator aIter =
                    pActions->find( aKey );
            if( !(aIter == pActions->end() ) )
            {
                if( !pMutableAttrList )
                {
                    pMutableAttrList = new XMLMutableAttributeList( xAttrList );
                    xAttrList = pMutableAttrList;
                }
                sal_uInt32 nAction = (*aIter).second.m_nActionType;
                switch( nAction )
                {
                case XML_ATACTION_MOVE_TO_ELEM:
                    {
                        OUString aElemQName(
                            GetTransformer().GetNamespaceMap().GetQNameByKey(
                                (*aIter).second.GetQNamePrefixFromParam1(),
                                ::xmloff::token::GetXMLToken(
                                (*aIter).second.GetQNameTokenFromParam1()) ) );
                        XMLTransformerContext *pContext =
                            new XMLPersTextContentTContext( GetTransformer(),
                                                       aElemQName );
                        pContext->Characters( rAttrValue );
                        XMLTransformerContextVector::value_type aVal(
                                pContext );
                        aChildContexts.push_back( aVal );
                        pMutableAttrList->RemoveAttributeByIndex( i );
                        --i;
                        --nAttrCount;
                    }
                    break;
                default:
                    OSL_ENSURE( !this, "unknown action" );
                    break;
                }
            }
        }
    }
    XMLTransformerContext::StartElement( xAttrList );

    XMLTransformerContextVector::iterator aIter = aChildContexts.begin();

    for( ; aIter != aChildContexts.end(); ++aIter )
    {
        (*aIter)->Export();
    }
}
