/*************************************************************************
 *
 *  $RCSfile: layerexport.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: fs $ $Date: 2001-02-01 09:46:47 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
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

#ifndef _XMLOFF_FORMS_LAYEREXPORT_HXX_
#define _XMLOFF_FORMS_LAYEREXPORT_HXX_

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_SCRIPTEVENTDESCRIPTOR_HPP_
#include <com/sun/star/script/ScriptEventDescriptor.hpp>
#endif
#ifndef _XMLOFF_FORMS_CALLBACKS_HXX_
#include "callbacks.hxx"
#endif
#ifndef _XMLOFF_FORMS_IFACECOMPARE_HXX_
#include "ifacecompare.hxx"
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif

class SvXMLExport;
class XMLPropertyHandlerFactory;
class SvXMLExportPropertyMapper;
//.........................................................................
namespace xmloff
{
//.........................................................................

    //=====================================================================
    //= OFormLayerXMLExport_Impl
    //=====================================================================
    /** the implementation class for OFormLayerXMLExport
    */
    class OFormLayerXMLExport_Impl
                :public IFormsExportContext
    {
        friend class OFormLayerXMLExport;

    protected:
        SvXMLExport&        m_rContext;

        // style handling
        ::vos::ORef< XMLPropertyHandlerFactory >    m_xPropertyHandlerFactory;
        ::vos::ORef< SvXMLExportPropertyMapper >    m_xExportMapper;

        DECLARE_STL_MAP( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >, ::rtl::OUString, OPropertySetCompare, MapPropertySet2String);
            // maps objects (property sets) to strings, e.g. control ids.
        DECLARE_STL_MAP( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >, MapPropertySet2String, ODrawPageCompare, MapPropertySet2Map);
            // map pages to maps (of property sets to strings)

        MapPropertySet2Map  m_aControlIds;
            // the control ids of all control on all pages we ever examined

        MapPropertySet2Map  m_aReferringControls;
            // for a given page (iter->first), and a given control (iter->second->first), this is the comma-separated
            // lists of ids of the controls refering to the control given.

        MapPropertySet2MapIterator
                            m_aCurrentPageIds;
            // the iterator for the control id map for the page beeing handled
        MapPropertySet2MapIterator
                            m_aCurrentPageReferring;
            // the same for the map of referring controls

        // TODO: To avoid this construct above, and to have a cleaner implementation, an class encapsulating the
        // export of a single page should be introduced.

    public:
        OFormLayerXMLExport_Impl(SvXMLExport& _rContext);

    protected:
        /** exports one single grid column
        */
        void    exportGridColumn(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxColumn,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& _rEvents
            );

        /** exports one single control
        */
        void    exportControl(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControl,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& _rEvents
            );

        /** exports one single form
        */
        void    exportForm(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxProps,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& _rEvents
            );

        /** seek to the page given.

            <p>This must be called before you can retrieve any ids for controls on the page.</p>

            @see
                getControlId
        */
        sal_Bool    seekPage(
            const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage);

        /** get the id of the given control.

            <p>You must have sought to the page of the control before calling this.</p>
        */
        ::rtl::OUString
                getControlId(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControl);

        // IFormsExportContext
        virtual void exportCollectionElements(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& _rxCollection);
        virtual SvXMLExport& getGlobalContext();
        virtual ::vos::ORef< SvXMLExportPropertyMapper > getStylePropertyMapper();

        /** clear any structures which have been build in the recent <method>examine</method> calls.
        */
        void clear();

        /** examine a forms collection.

            <p>The method will collect control ids and add styles to the export context as necessary.</p>

            <p>Every control in the object hierarchy given will be assigned to a unique id, which is stored for later
            use.</p>

            <p>In addition, any references the controls may have between each other, are collected and stored for
            later use.</p>

            <p>Upon calling this method, the id map will be cleared before collecting the new ids, so any ids
            you collected previously will be lost</p>

            @param _rxDrawPage
                the draw page which's forms collection should be examined

            @see getControlId
            @see exportControl
            @see exportForms
        */
        void examineForms(
            const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage);

        /** export a forms collection of a draw page

            <p>The method will obtain the forms collection of the page and call
            <method>exportCollectionElements</method>.</p>
        */
        void exportForms(
            const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage);

    protected:
        sal_Bool implCheckPage(
            const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage,
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& _rxForms);

        /** moves the m_aCurrentPage* members to the positions specifying the given page.

            @return <TRUE/> if there already were structures for the given page
        */
        sal_Bool implMoveIterators(
            const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage,
            sal_Bool _bClear);
    };

//.........................................................................
}   // namespace xmloff
//.........................................................................

#endif // _XMLOFF_FORMS_LAYEREXPORT_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2001/01/02 15:58:22  fs
 *  event ex- & import
 *
 *  Revision 1.7  2000/12/19 12:13:57  fs
 *  some changes ... now the exported styles are XSL conform
 *
 *  Revision 1.6  2000/12/18 15:14:35  fs
 *  some changes ... now exporting/importing styles
 *
 *  Revision 1.5  2000/12/13 10:40:15  fs
 *  new import related implementations - at this version, we should be able to import everything we export (which is all except events and styles)
 *
 *  Revision 1.4  2000/12/06 17:28:05  fs
 *  changes for the formlayer import - still under construction
 *
 *  Revision 1.3  2000/12/03 10:57:06  fs
 *  some changes to support more than one page to be examined/exported
 *
 *  Revision 1.2  2000/11/19 15:41:32  fs
 *  extended the export capabilities - generic controls / grid columns / generic properties / some missing form properties
 *
 *  Revision 1.1  2000/11/17 19:02:39  fs
 *  initial checkin - export and/or import the applications form layer
 *
 *
 *  Revision 1.0 17.11.00 17:21:00  fs
 ************************************************************************/

