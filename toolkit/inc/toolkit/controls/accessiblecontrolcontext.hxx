/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: accessiblecontrolcontext.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:47:47 $
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

#ifndef TOOLKIT_ACCESSIBLE_CONTROL_CONTEXT_HXX
#define TOOLKIT_ACCESSIBLE_CONTROL_CONTEXT_HXX

#ifndef COMPHELPER_ACCESSIBLE_COMPONENT_HELPER_HXX
#include <comphelper/accessiblecomponenthelper.hxx>
#endif
#ifndef COMPHELPER_ACCIMPLACCESS_HXX
#include <comphelper/accimplaccess.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XEVENTLISTENER_HPP_
#include <com/sun/star/lang/XEventListener.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif

class Window;
//........................................................................
namespace toolkit
{
//........................................................................

    //====================================================================
    //= OAccessibleControlContext
    //====================================================================

    typedef ::comphelper::OAccessibleComponentHelper    OAccessibleControlContext_Base;
    typedef ::cppu::ImplHelper1 <   ::com::sun::star::lang::XEventListener
                                >   OAccessibleControlContext_IBase;

    /** class implementing the AccessibleContext for an UNO control - to be used in design mode of the control.
        <p><b>life time control<b/><br/>
        This control should be held weak by the creator (an UNO control), it itself holds a hard reference to the
        control model, and a weak reference to the control. The reference to the model is freed when the model
        is beeing disposed.</p>
    */
    class OAccessibleControlContext
            :public ::comphelper::OAccessibleImplementationAccess
            ,public OAccessibleControlContext_Base
            ,public OAccessibleControlContext_IBase
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                    m_xControlModel;        // the model of the control which's context we implement
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
                    m_xModelPropsInfo;      // the cached property set info of the model

    protected:
        /// ctor. @see Init
        OAccessibleControlContext();
        ~OAccessibleControlContext();

        /** late ctor
        */
        void Init(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxCreator
        )   SAL_THROW( ( ::com::sun::star::uno::Exception ) );

        // OCommonAccessibleComponent overridables
        virtual ::com::sun::star::awt::Rectangle SAL_CALL implGetBounds(  ) throw (::com::sun::star::uno::RuntimeException);

    public:
        /** creates an accessible context for an uno control
        @param _rxCreator
            the uno control's XAccessible interface. This must be an XControl, from which an XControlModel
            can be retrieved.
        */
        static OAccessibleControlContext* create(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxCreator
        ) SAL_THROW( ( ) );

    protected:
        // XInterface
        DECLARE_XINTERFACE( )
        DECLARE_XTYPEPROVIDER( )

        // XAccessibleContext
        virtual sal_Int32 SAL_CALL getAccessibleChildCount(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Int16 SAL_CALL getAccessibleRole(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getAccessibleName(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  ) throw (::com::sun::star::uno::RuntimeException);

        // XAccessibleComponent
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL grabFocus(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getForeground(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getBackground(  ) throw (::com::sun::star::uno::RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

    private:
        // retrieves the value of a string property from the model, if the property is present
        ::rtl::OUString getModelStringProperty( const sal_Char* _pPropertyName ) SAL_THROW( ( ) );

        // starts listening at the control model (currently for disposal only)
        void startModelListening( ) SAL_THROW( ( ::com::sun::star::uno::Exception ) );
        // stops listening at the control model
        void stopModelListening( ) SAL_THROW( ( ::com::sun::star::uno::Exception ) );

        Window* implGetWindow( ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >* _pxUNOWindow = NULL ) const;
    };

//........................................................................
}   // namespace toolkit
//........................................................................

#endif // TOOLKIT_ACCESSIBLE_CONTROL_CONTEXT_HXX

