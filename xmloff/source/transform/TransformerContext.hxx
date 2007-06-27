/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TransformerContext.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 16:26:05 $
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

#ifndef _XMLOFF_TRANSFORMERCONTEXT_HXX
#define _XMLOFF_TRANSFORMERCONTEXT_HXX

#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _SALHELPER_SIMPLEREFERENCEOBJECT_HXX_
#include <salhelper/simplereferenceobject.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif

class SvXMLNamespaceMap;
class XMLTransformerBase;

class XMLTransformerContext : public ::salhelper::SimpleReferenceObject
{
    friend class XMLTransformerBase;

    XMLTransformerBase& m_rTransformer;

    ::rtl::OUString m_aQName;

    SvXMLNamespaceMap   *m_pRewindMap;

    SvXMLNamespaceMap  *GetRewindMap() const { return m_pRewindMap; }
    void SetRewindMap( SvXMLNamespaceMap *p ) { m_pRewindMap = p; }

protected:

    XMLTransformerBase& GetTransformer() { return m_rTransformer; }
    const XMLTransformerBase& GetTransformer() const { return m_rTransformer; }

    void SetQName( const ::rtl::OUString& rQName ) { m_aQName = rQName; }

public:
    TYPEINFO();

    const ::rtl::OUString& GetQName() const { return m_aQName; }
    sal_Bool HasQName( sal_uInt16 nPrefix,
                       ::xmloff::token::XMLTokenEnum eToken ) const;
    sal_Bool HasNamespace( sal_uInt16 nPrefix ) const;

    // A contexts constructor does anything that is required if an element
    // starts. Namespace processing has been done already.
    // Note that virtual methods cannot be used inside constructors. Use
    // StartElement instead if this is required.
    XMLTransformerContext( XMLTransformerBase& rTransformer,
                        const ::rtl::OUString& rQName );

    // A contexts destructor does anything that is required if an element
    // ends. By default, nothing is done.
    // Note that virtual methods cannot be used inside destructors. Use
    // EndElement instead if this is required.
    virtual ~XMLTransformerContext();

    // Create a childs element context. By default, the import's
    // CreateContext method is called to create a new default context.
    virtual XMLTransformerContext *CreateChildContext( sal_uInt16 nPrefix,
                                   const ::rtl::OUString& rLocalName,
                                   const ::rtl::OUString& rQName,
                                   const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

    // StartElement is called after a context has been constructed and
    // before a elements context is parsed. It may be used for actions that
    // require virtual methods. The default is to do nothing.
    virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

    // EndElement is called before a context will be destructed, but
    // after a elements context has been parsed. It may be used for actions
    // that require virtual methods. The default is to do nothing.
    virtual void EndElement();

    // This method is called for all characters that are contained in the
    // current element. The default is to ignore them.
    virtual void Characters( const ::rtl::OUString& rChars );

    // Is the current context a persistent one (i.e. one that saves is content
    // rather than exporting it directly?
    virtual sal_Bool IsPersistent() const;

    // Export the whole element. By default, nothing is done here
    virtual void Export();

    // Export the element content. By default, nothing is done here
    virtual void ExportContent();
};


#endif  //  _XMLOFF_TRANSFORMERCONTEXT_HXX

