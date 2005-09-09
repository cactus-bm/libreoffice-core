/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XMLAutoTextEventExport.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 14:57:32 $
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

#ifndef _XMLOFF_XMLAUTOTEXTEVENTEXPORT_HXX
#define _XMLOFF_XMLAUTOTEXTEVENTEXPORT_HXX

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif

#include <set>


namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
    namespace container { class XNameAccess; }
    namespace frame { class XModel; }
    namespace lang { class XMultiServiceFactory; }
    namespace uno { template<class X> class Reference; }
    namespace uno { template<class X> class Sequence; }
    namespace uno { class XInterface; }
    namespace uno { class Exception; }
    namespace xml { namespace sax { class XDocumentHandler; } }
} } }


/**
 * Component for the export of events attached to autotext blocks.
 * Via the XInitialization interface it expects up to two strings, the
 * first giving the file name (URL) of the autotext group, and the second
 * identifying the autotext. If one of the strings is not given, it
 * will export the whole group / all groups.
 */
class XMLAutoTextEventExport : public SvXMLExport
{
    ::com::sun::star::uno::Reference<
        ::com::sun::star::container::XNameAccess> xEvents;

    const ::rtl::OUString sEventType;
    const ::rtl::OUString sNone;


public:

    // #110680#
    //XMLAutoTextEventExport();
    XMLAutoTextEventExport(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory, sal_uInt16 nFlags
        );

    // #110680#
    //XMLAutoTextEventExport(
    //  const ::rtl::OUString& rFileName,
    //  const ::com::sun::star::uno::Reference<
    //      ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
    //  const ::com::sun::star::uno::Reference<
    //      ::com::sun::star::frame::XModel > & rModel,
    //  const ::com::sun::star::uno::Reference<
    //      ::com::sun::star::container::XNameAccess > & rEvents);
    XMLAutoTextEventExport(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
        const ::rtl::OUString& rFileName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > & rModel,
        const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & rEvents, sal_uInt16 nFlags);

    ~XMLAutoTextEventExport();

    // XInitialization
    virtual void SAL_CALL initialize(
        const ::com::sun::star::uno::Sequence<
            ::com::sun::star::uno::Any> & rArguments )
        throw(
            ::com::sun::star::uno::Exception,
            ::com::sun::star::uno::RuntimeException);

protected:

    /// export the events off all autotexts
    virtual sal_uInt32 exportDoc(
        enum ::xmloff::token::XMLTokenEnum eClass = xmloff::token::XML_TOKEN_INVALID );

    /// does the document have any events ?
    sal_Bool hasEvents();

    /// export the events element
    void exportEvents();


    /// add the namespaces used by events
    /// (to be called for the document element)
    void addNamespaces();


    // methods without content:
    virtual void _ExportMeta();
    virtual void _ExportScripts();
    virtual void _ExportFontDecls();
    virtual void _ExportStyles( sal_Bool bUsed ) ;
    virtual void _ExportAutoStyles();
    virtual void _ExportMasterStyles();
    virtual void _ExportChangeTracking();
    virtual void _ExportContent();
};



// global functions to support the component

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
    XMLAutoTextEventExport_getSupportedServiceNames()
    throw();

::rtl::OUString SAL_CALL XMLAutoTextEventExport_getImplementationName()
    throw();

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
    XMLAutoTextEventExportOOO_createInstance(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::lang::XMultiServiceFactory > & )
    throw( ::com::sun::star::uno::Exception );

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
    XMLAutoTextEventExportOOO_getSupportedServiceNames()
    throw();

::rtl::OUString SAL_CALL XMLAutoTextEventExportOOO_getImplementationName()
    throw();

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
    XMLAutoTextEventExportOOO_createInstance(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::lang::XMultiServiceFactory > & )
    throw( ::com::sun::star::uno::Exception );

#endif

