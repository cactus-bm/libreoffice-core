/*************************************************************************
 *
 *  $RCSfile: menudocumenthandler.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: cd $ $Date: 2001-05-03 08:57:01 $
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

#ifndef __FRAMEWORK_SERVICES_MENUDOCUMENTHANDLER_HXX_
#define __FRAMEWORK_SERVICES_MENUDOCUMENTHANDLER_HXX_

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef __COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_HELPER_OMUTEXMEMBER_HXX_
#include <helper/omutexmember.hxx>
#endif

#ifndef _RTL_USTRING_
#include <rtl/ustring>
#endif

#ifndef _SV_MENU_HXX
#include <vcl/menu.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

class ReadMenuDocumentHandlerBase : public ::com::sun::star::xml::sax::XDocumentHandler,
                                    public OMutexMember,    // Struct for right initalization of mutex member! Must be first of baseclasses.
                                    public ::cppu::OWeakObject
{
    public:
        ReadMenuDocumentHandlerBase();
        virtual ~ReadMenuDocumentHandlerBase();

        // XInterface
        virtual void SAL_CALL acquire() throw( ::com::sun::star::uno::RuntimeException )
            { OWeakObject::acquire(); }
        virtual void SAL_CALL release() throw( ::com::sun::star::uno::RuntimeException )
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
        OReadMenuDocumentHandler( MenuBar* pMenuBar );
        virtual ~OReadMenuDocumentHandler();

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
        USHORT      m_nItemId;
        int         m_nElementDepth;
        sal_Bool    m_bMenuBarMode;
        MenuBar*    m_pMenuBar;
};  // OReadMenuDocumentHandler


class OReadMenuBarHandler : public ReadMenuDocumentHandlerBase
{
    public:
        OReadMenuBarHandler( MenuBar* pMenuBar, USHORT* pItemId );
        virtual ~OReadMenuBarHandler();

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
        USHORT*     m_pItemId;
        int         m_nElementDepth;
        sal_Bool    m_bMenuMode;
        MenuBar*    m_pMenuBar;
};  // OReadMenuBarHandler


class OReadMenuHandler : public ReadMenuDocumentHandlerBase
{
    public:
        OReadMenuHandler( Menu* pMenu, USHORT* pItemId );
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
        USHORT*             m_pItemId;
        int                 m_nElementDepth;
        sal_Bool            m_bMenuPopupMode;
        Menu*               m_pMenu;
}; // OReadMenuHandler


class OReadMenuPopupHandler : public ReadMenuDocumentHandlerBase
{
    public:
        OReadMenuPopupHandler( Menu* pMenu, USHORT* pItemId );
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
        USHORT*             m_pItemId;
        sal_Bool            m_bMenuMode;
        Menu*               m_pMenu;
        NextElementClose    m_nNextElementExpected;
}; // OReadMenuPopupHandler


class OWriteMenuDocumentHandler
{
    public:
        OWriteMenuDocumentHandler(
            MenuBar* pMenu,
            ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > );
        virtual ~OWriteMenuDocumentHandler();

        void WriteMenuDocument() throw
            ( ::com::sun::star::xml::sax::SAXException,
              ::com::sun::star::uno::RuntimeException );
    protected:
        virtual void WriteMenu( Menu* pMenu ) throw
            ( ::com::sun::star::xml::sax::SAXException,
              ::com::sun::star::uno::RuntimeException );

        virtual void WriteMenuItem( Menu* pMenu, USHORT nItemId );
        virtual void WriteMenuSeparator();

        MenuBar* m_pMenuBar;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > m_xWriteDocumentHandler;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > m_xEmptyList;
        ::rtl::OUString m_aAttributeType;
};

} // namespace framework

#endif  // #ifndef __FRAMEWORK_SERVICES_MENUDOCUMENTHANDLER_HXX_
