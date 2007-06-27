/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PersAttrListTContext.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 16:22:44 $
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

#ifndef _XMLOFF_PERSATTRLISTTCONTEXT_HXX
#define _XMLOFF_PERSATTRLISTTCONTEXT_HXX

#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_TRANSFORMERCONTEXT_HXX
#include "TransformerContext.hxx"
#endif


class XMLPersAttrListTContext : public XMLTransformerContext
{

    ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XAttributeList > m_xAttrList;
    ::rtl::OUString m_aElemQName;
    sal_uInt16 m_nActionMap;

protected:

    void SetExportQName( const ::rtl::OUString& r ) { m_aElemQName = r; }

public:
    TYPEINFO();

    // A contexts constructor does anything that is required if an element
    // starts. Namespace processing has been done already.
    // Note that virtual methods cannot be used inside constructors. Use
    // StartElement instead if this is required.
    XMLPersAttrListTContext( XMLTransformerBase& rTransformer,
                           const ::rtl::OUString& rQName );

    // attr list persistence + attribute processing
    XMLPersAttrListTContext( XMLTransformerBase& rTransformer,
                           const ::rtl::OUString& rQName,
                              sal_uInt16 nActionMap );

    // attr list persistence + renaming
    XMLPersAttrListTContext( XMLTransformerBase& rTransformer,
                           const ::rtl::OUString& rQName,
                              sal_uInt16 nPrefix,
                              ::xmloff::token::XMLTokenEnum eToken );

    // attr list persistence + renaming + attribute processing
    XMLPersAttrListTContext( XMLTransformerBase& rTransformer,
                           const ::rtl::OUString& rQName,
                              sal_uInt16 nPrefix,
                              ::xmloff::token::XMLTokenEnum eToken,
                              sal_uInt16 nActionMap );

    // A contexts destructor does anything that is required if an element
    // ends. By default, nothing is done.
    // Note that virtual methods cannot be used inside destructors. Use
    // EndElement instead if this is required.
    virtual ~XMLPersAttrListTContext();

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
    // current element.
    virtual void Characters( const ::rtl::OUString& rChars );

    virtual sal_Bool IsPersistent() const;
    virtual void Export();
    virtual void ExportContent();

    const ::rtl::OUString& GetExportQName() const { return m_aElemQName; }

    void AddAttribute( sal_uInt16 nAPrefix,
        ::xmloff::token::XMLTokenEnum eAToken,
           ::xmloff::token::XMLTokenEnum eVToken );

    void AddAttribute( sal_uInt16 nAPrefix,
        ::xmloff::token::XMLTokenEnum eAToken,
        const ::rtl::OUString & rValue );

    ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XAttributeList >
        GetAttrList() const;
};

#endif  //  _XMLOFF_PERSATTRLISTTCONTEXT_HXX

