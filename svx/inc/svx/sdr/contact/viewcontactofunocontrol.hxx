/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: viewcontactofunocontrol.hxx,v $
 * $Revision: 1.7 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef SVX_SDR_CONTACT_VIEWCONTACTOFUNOCONTROL_HXX
#define SVX_SDR_CONTACT_VIEWCONTACTOFUNOCONTROL_HXX

/** === begin UNO includes === **/
#include <com/sun/star/uno/Reference.hxx>
/** === end UNO includes === **/
#include <svx/sdr/contact/viewcontactofsdrmediaobj.hxx>
#include <svx/svxdllapi.h>

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

