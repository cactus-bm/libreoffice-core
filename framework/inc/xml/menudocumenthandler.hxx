/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: menudocumenthandler.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 11:09:05 $
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

#ifndef __FRAMEWORK_XML_MENUDOCUMENTHANDLER_HXX_
#define __FRAMEWORK_XML_MENUDOCUMENTHANDLER_HXX_

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef __COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSINGLECOMPONENTFACTORY_HPP_
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef _RTL_USTRING_
#include <rtl/ustring.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

class ReadMenuDocumentHandlerBase : public ::com::sun::star::xml::sax::XDocumentHandler,
                                    public ThreadHelpBase,  // Struct for right initalization of mutex member! Must be first of baseclasses.
                                    public ::cppu::OWeakObject
{
    public:
        ReadMenuDocumentHandlerBase();
        virtual ~ReadMenuDocumentHandlerBase();

        // XInterface
        virtual void SAL_CALL acquire() throw()
            { OWeakObject::acquire(); }
        virtual void SAL_CALL release() throw()
            { OWeakObject::release(); }
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
            const ::com::sun::star::uno::Type & rType ) throw( ::com::sun::star::uno::RuntimeException );

        // XDocumentHandler
        virtual void SAL_CALL startDocument(void)
        throw ( ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException ) = 0;

        virtual void SAL_CALL endDocument(void)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException ) = 0;

        virtual void SAL_CALL startElement(
            const rtl::OUString& aName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException ) = 0;

        virtual void SAL_CALL endElement(const rtl::OUString& aName)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException ) = 0;

        virtual void SAL_CALL characters(const rtl::OUString& aChars)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException ) = 0;

        virtual void SAL_CALL ignorableWhitespace(const rtl::OUString& aWhitespaces)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL processingInstruction(const rtl::OUString& aTarget,
                                                    const rtl::OUString& aData)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL setDocumentLocator(
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > &xLocator)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

    protected:
        ::rtl::OUString getErrorLineString();

        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > m_xLocator;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler> m_xReader;
};


class OReadMenuDocumentHandler : public ReadMenuDocumentHandlerBase
{
    public:
        // #110897#
        OReadMenuDocumentHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
            const com::sun::star::uno::Reference< com::sun::star::container::XIndexContainer >& rItemContainer );
        virtual ~OReadMenuDocumentHandler();

        // #110897#
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& getServiceFactory();

        // XDocumentHandler
        virtual void SAL_CALL startDocument(void)
        throw ( ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endDocument(void)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL startElement(
            const rtl::OUString& aName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endElement(const rtl::OUString& aName)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL characters(const rtl::OUString& aChars)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

    private:
          int       m_nElementDepth;
          sal_Bool  m_bMenuBarMode;
          com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuBarContainer;
          com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;
          const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& mxServiceFactory;
};  // OReadMenuDocumentHandler


class OReadMenuBarHandler : public ReadMenuDocumentHandlerBase
{
    public:
        // #110897#
        OReadMenuBarHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
            const com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rMenuBarContainer,
            const com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >& rContainerFactory );
        virtual ~OReadMenuBarHandler();

        // #110897#
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& getServiceFactory();

        // XDocumentHandler
        virtual void SAL_CALL startDocument(void)
        throw ( ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endDocument(void)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL startElement(
            const rtl::OUString& aName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endElement(const rtl::OUString& aName)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL characters(const rtl::OUString& aChars)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

    private:
        int         m_nElementDepth;
        sal_Bool    m_bMenuMode;
        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuBarContainer;
        com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;

        // #110897#
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& mxServiceFactory;
};  // OReadMenuBarHandler


class OReadMenuHandler : public ReadMenuDocumentHandlerBase
{
    public:
        OReadMenuHandler( const com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rMenuContainer,
                          const com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >& rContainerFactory );
        virtual ~OReadMenuHandler();

        // XDocumentHandler
        virtual void SAL_CALL startDocument(void)
        throw ( ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endDocument(void)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL startElement(
            const rtl::OUString& aName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endElement(const rtl::OUString& aName)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL characters(const rtl::OUString& aChars)
        throw(  ::com::sun::star::xml::sax::SAXException,
                ::com::sun::star::uno::RuntimeException );

    private:
        int                 m_nElementDepth;
        sal_Bool            m_bMenuPopupMode;
        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuContainer;
        com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;
}; // OReadMenuHandler


class OReadMenuPopupHandler : public ReadMenuDocumentHandlerBase
{
    public:
        OReadMenuPopupHandler( const com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rMenuContainer,
                               const com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >& rContainerFactory );
        virtual ~OReadMenuPopupHandler();

        // XDocumentHandler
        virtual void SAL_CALL startDocument(void)
            throw ( ::com::sun::star::xml::sax::SAXException,
                    ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endDocument(void)
            throw ( ::com::sun::star::xml::sax::SAXException,
                    ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL startElement(
            const rtl::OUString& aName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
            throw ( ::com::sun::star::xml::sax::SAXException,
                    ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL endElement(const rtl::OUString& aName)
            throw ( ::com::sun::star::xml::sax::SAXException,
                    ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL characters(const rtl::OUString& aChars)
            throw ( ::com::sun::star::xml::sax::SAXException,
                    ::com::sun::star::uno::RuntimeException );

    private:
        enum NextElementClose { ELEM_CLOSE_NONE, ELEM_CLOSE_MENUITEM, ELEM_CLOSE_MENUSEPARATOR };

        int                 m_nElementDepth;
        sal_Bool            m_bMenuMode;
        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuContainer;
        com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;
        NextElementClose    m_nNextElementExpected;
}; // OReadMenuPopupHandler


class OWriteMenuDocumentHandler
{
    public:
        OWriteMenuDocumentHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rMenuBarContainer,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >& rDocumentHandler );
        virtual ~OWriteMenuDocumentHandler();

        void WriteMenuDocument() throw
            ( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    protected:
        virtual void WriteMenu( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rSubMenuContainer ) throw
            ( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );

        virtual void WriteMenuItem( const rtl::OUString& aCommandURL, const rtl::OUString& aLabel, const rtl::OUString& aHelpURL );
        virtual void WriteMenuSeparator();

        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > m_xMenuBarContainer;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > m_xWriteDocumentHandler;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > m_xEmptyList;
        ::rtl::OUString m_aAttributeType;
};

} // namespace framework

#endif  // #ifndef __FRAMEWORK_XML_MENUDOCUMENTHANDLER_HXX_
