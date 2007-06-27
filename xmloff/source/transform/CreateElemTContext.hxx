/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CreateElemTContext.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 16:19:58 $
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

#ifndef _XMLOFF_CREATEELEMTCONTEXT_HXX
#define _XMLOFF_CREATEELEMTCONTEXT_HXX

#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_TRANSFORMERCONTEXT_HXX
#include "TransformerContext.hxx"
#endif

class XMLMutableAttributeList;
class XMLTransformerActions;
class XMLCreateElemTransformerContext : public XMLTransformerContext
{
    sal_uInt16 m_nActionMap;

protected:

    sal_uInt16 GetActionMap() const { return m_nActionMap; }

public:
    TYPEINFO();

    XMLCreateElemTransformerContext( XMLTransformerBase& rTransformer,
                           const ::rtl::OUString& rQName,
                              sal_uInt16 nActionMap );

    virtual ~XMLCreateElemTransformerContext();

    virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

#endif  //  _XMLOFF_CREATEELEMCONTEXT_HXX

