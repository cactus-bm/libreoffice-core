/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: accessiblekeybindinghelper.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 22:46:55 $
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

// includes --------------------------------------------------------------

#ifndef COMPHELPER_ACCESSIBLE_KEYBINDING_HELPER_HXX
#include <comphelper/accessiblekeybindinghelper.hxx>
#endif


//..............................................................................
namespace comphelper
{
//..............................................................................

    using namespace ::com::sun::star; // MT 04/2003: was ::drafts::com::sun::star - otherwise to many changes
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::accessibility;

    //==============================================================================
    // OAccessibleKeyBindingHelper
    //==============================================================================

    OAccessibleKeyBindingHelper::OAccessibleKeyBindingHelper()
    {
    }

    // -----------------------------------------------------------------------------

    OAccessibleKeyBindingHelper::OAccessibleKeyBindingHelper( const OAccessibleKeyBindingHelper& rHelper )
        : cppu::WeakImplHelper1<XAccessibleKeyBinding>( rHelper )
        , m_aKeyBindings( rHelper.m_aKeyBindings )
    {
    }

    // -----------------------------------------------------------------------------

    OAccessibleKeyBindingHelper::~OAccessibleKeyBindingHelper()
    {
    }

    // -----------------------------------------------------------------------------

    void OAccessibleKeyBindingHelper::AddKeyBinding( const Sequence< awt::KeyStroke >& rKeyBinding ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        m_aKeyBindings.push_back( rKeyBinding );
    }

    // -----------------------------------------------------------------------------

    void OAccessibleKeyBindingHelper::AddKeyBinding( const awt::KeyStroke& rKeyStroke ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        Sequence< awt::KeyStroke > aSeq(1);
        aSeq[0] = rKeyStroke;
        m_aKeyBindings.push_back( aSeq );
    }

    // -----------------------------------------------------------------------------
    // XAccessibleKeyBinding
    // -----------------------------------------------------------------------------

    sal_Int32 OAccessibleKeyBindingHelper::getAccessibleKeyBindingCount() throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return m_aKeyBindings.size();
    }

    // -----------------------------------------------------------------------------

    Sequence< awt::KeyStroke > OAccessibleKeyBindingHelper::getAccessibleKeyBinding( sal_Int32 nIndex ) throw (IndexOutOfBoundsException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( nIndex < 0 || nIndex >= (sal_Int32)m_aKeyBindings.size() )
            throw IndexOutOfBoundsException();

        return m_aKeyBindings[nIndex];
    }

    // -----------------------------------------------------------------------------

//..............................................................................
}   // namespace comphelper
//..............................................................................
