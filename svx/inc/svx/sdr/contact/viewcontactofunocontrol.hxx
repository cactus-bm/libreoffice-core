/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: viewcontactofunocontrol.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 16:35:01 $
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

#ifndef SVX_SDR_CONTACT_VIEWCONTACTOFUNOCONTROL_HXX
#define SVX_SDR_CONTACT_VIEWCONTACTOFUNOCONTROL_HXX

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
/** === end UNO includes === **/

#ifndef _SDR_CONTACT_VIEWCONTACTOFSDRMEDIAOBJ_HXX
#include <svx/sdr/contact/viewcontactofsdrmediaobj.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include <svx/svxdllapi.h>
#endif

#include <memory>

class OutputDevice;
class Window;
class SdrUnoObj;
namespace com { namespace sun { namespace star {
    namespace awt {
        class XControl;
        class XControlContainer;
    }
} } }

//........................................................................
namespace sdr { namespace contact {
//........................................................................

    //====================================================================
    //= ViewContactOfUnoControl
    //====================================================================
    class ViewContactOfUnoControl_Impl;
    class SVX_DLLPRIVATE ViewContactOfUnoControl : public ViewContactOfSdrObj
    {
    private:
        ::std::auto_ptr< ViewContactOfUnoControl_Impl >   m_pImpl;

    public:
        ViewContactOfUnoControl( SdrUnoObj& _rUnoObject );
        virtual ~ViewContactOfUnoControl();

        /** access control to selected members
        */
        struct SdrUnoObjAccessControl { friend class ::SdrUnoObj; private: SdrUnoObjAccessControl() { } };

        /** retrieves the XControl asscociated with the ViewContact and the given device
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >
            getUnoControlForDevice( const OutputDevice* _pDevice, const SdrUnoObjAccessControl& ) const;

        /** retrieves a temporary XControl instance, whose parent is the given window
            @seealso SdrUnoObj::GetTemporaryControlForWindow
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >
            getTemporaryControlForWindow( const Window& _rWindow, ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >& _inout_ControlContainer ) const;

        /** invalidates all ViewObjectContacts

            This method is necessary when an SdrUnoObj changes completely, e.g. when
            some foreign instance set a new ->XControlModel.
        */
        void    invalidateAllContacts( const SdrUnoObjAccessControl&  );

    protected:
        virtual ViewObjectContact& CreateObjectSpecificViewObjectContact( ObjectContact& _rObjectContact );

        // ViewContactOfSdrObj overridables
        virtual sal_Bool ShouldPaintObject( DisplayInfo& rDisplayInfo, const ViewObjectContact& rAssociatedVOC );

    private:
        ViewContactOfUnoControl();                                            // never implemented
        ViewContactOfUnoControl( const ViewContactOfUnoControl& );              // never implemented
        ViewContactOfUnoControl& operator=( const ViewContactOfUnoControl& );   // never implemented
    };

//........................................................................
} } // namespace sdr::contact
//........................................................................

#endif // SVX_SDR_CONTACT_VIEWCONTACTOFUNOCONTROL_HXX

