/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: formcontrolcontainer.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:17:23 $
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

#ifndef EXTENSIONS_BIB_FORMCONTROLCONTAINER_HXX
#define EXTENSIONS_BIB_FORMCONTROLCONTAINER_HXX

#ifndef _COMPHELPER_BROADCASTHELPER_HXX_
#include <comphelper/broadcasthelper.hxx>
#endif
#ifndef EXTENSIONS_BIB_LOADLISTENERADAPTER_HXX
#include "loadlisteneradapter.hxx"
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLCONTAINER_HPP_
#include <com/sun/star/awt/XControlContainer.hpp>
#endif

class BibDataManager;

//.........................................................................
namespace bib
{
//.........................................................................

    //=====================================================================
    //= FormControlContainer
    //=====================================================================
    class FormControlContainer
            :public ::comphelper::OBaseMutex
            ,public ::bib::OLoadListener
    {
    private:
        OLoadListenerAdapter*   m_pFormAdapter;
        ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadable >
                                m_xForm;
    private:
        void    implSetDesignMode( sal_Bool _bDesign );

    protected:
        FormControlContainer( );
        ~FormControlContainer( );

        sal_Bool    isFormConnected() const { return NULL != m_pFormAdapter; }
        void        connectForm( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadable >& _rxForm );
        void        disconnectForm();

        void        ensureDesignMode();

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >
                    getControlContainer() = 0;

    protected:
    // XLoadListener equivalents
        virtual void _loaded( const ::com::sun::star::lang::EventObject& _rEvent );
        virtual void _unloading( const ::com::sun::star::lang::EventObject& _rEvent );
        virtual void _unloaded( const ::com::sun::star::lang::EventObject& _rEvent );
        virtual void _reloading( const ::com::sun::star::lang::EventObject& _rEvent );
        virtual void _reloaded( const ::com::sun::star::lang::EventObject& _rEvent );

    };

//.........................................................................
}   // namespace bib
//.........................................................................

#endif // EXTENSIONS_BIB_FORMCONTROLCONTAINER_HXX

