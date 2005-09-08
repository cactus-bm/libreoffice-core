/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: accessiblecomponenthelper.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 02:46:22 $
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

#ifndef COMPHELPER_ACCESSIBLE_COMPONENT_HELPER_HXX
#include <comphelper/accessiblecomponenthelper.hxx>
#endif

//.........................................................................
namespace comphelper
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::accessibility;

    //=====================================================================
    //= OCommonAccessibleComponent
    //=====================================================================
    //---------------------------------------------------------------------
    OCommonAccessibleComponent::OCommonAccessibleComponent( )
    {
    }

    //---------------------------------------------------------------------
    OCommonAccessibleComponent::OCommonAccessibleComponent( IMutex* _pExternalLock )
        :OAccessibleContextHelper( _pExternalLock )
    {
    }

    //---------------------------------------------------------------------
    OCommonAccessibleComponent::~OCommonAccessibleComponent( )
    {
        forgetExternalLock();
            // this ensures that the lock, which may be already destroyed as part of the derivee,
            // is not used anymore
    }

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL OCommonAccessibleComponent::containsPoint( const Point& _rPoint ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );
        return  ( _rPoint.X >= 0 )
            &&  ( _rPoint.Y >= 0 )
            &&  ( _rPoint.X < aBounds.Width )
            &&  ( _rPoint.Y < aBounds.Height );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OCommonAccessibleComponent::getLocation(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );
        return Point( aBounds.X, aBounds.Y );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OCommonAccessibleComponent::getLocationOnScreen(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );

        Point aScreenLoc( 0, 0 );

        Reference< XAccessibleComponent > xParentComponent( implGetParentContext(), UNO_QUERY );
        OSL_ENSURE( xParentComponent.is(), "OCommonAccessibleComponent::getLocationOnScreen: no parent component!" );
        if ( xParentComponent.is() )
        {
            Point aParentScreenLoc( xParentComponent->getLocationOnScreen() );
            Point aOwnRelativeLoc( getLocation() );
            aScreenLoc.X = aParentScreenLoc.X + aOwnRelativeLoc.X;
            aScreenLoc.Y = aParentScreenLoc.Y + aOwnRelativeLoc.Y;
        }

        return aScreenLoc;
    }

    //--------------------------------------------------------------------
    Size SAL_CALL OCommonAccessibleComponent::getSize(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );
        return Size( aBounds.Width, aBounds.Height );
    }

    //--------------------------------------------------------------------
    Rectangle SAL_CALL OCommonAccessibleComponent::getBounds(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        return implGetBounds();
    }

    //=====================================================================
    //= OAccessibleComponentHelper
    //=====================================================================
    //---------------------------------------------------------------------
    OAccessibleComponentHelper::OAccessibleComponentHelper( )
    {
    }

    //---------------------------------------------------------------------
    OAccessibleComponentHelper::OAccessibleComponentHelper( IMutex* _pExternalLock )
        :OCommonAccessibleComponent( _pExternalLock )
    {
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE2( OAccessibleComponentHelper, OCommonAccessibleComponent, OAccessibleComponentHelper_Base )
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( OAccessibleComponentHelper, OCommonAccessibleComponent, OAccessibleComponentHelper_Base )
        // (order matters: the first is the class name, the second is the class doing the ref counting)

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL OAccessibleComponentHelper::containsPoint( const Point& _rPoint ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::containsPoint( _rPoint );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleComponentHelper::getLocation(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocation( );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleComponentHelper::getLocationOnScreen(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocationOnScreen( );
    }

    //--------------------------------------------------------------------
    Size SAL_CALL OAccessibleComponentHelper::getSize(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getSize( );
    }

    //--------------------------------------------------------------------
    Rectangle SAL_CALL OAccessibleComponentHelper::getBounds(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getBounds( );
    }

    //=====================================================================
    //= OAccessibleExtendedComponentHelper
    //=====================================================================
    //---------------------------------------------------------------------
    OAccessibleExtendedComponentHelper::OAccessibleExtendedComponentHelper( )
    {
    }

    //---------------------------------------------------------------------
    OAccessibleExtendedComponentHelper::OAccessibleExtendedComponentHelper( IMutex* _pExternalLock )
        :OCommonAccessibleComponent( _pExternalLock )
    {
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE2( OAccessibleExtendedComponentHelper, OCommonAccessibleComponent, OAccessibleExtendedComponentHelper_Base )
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( OAccessibleExtendedComponentHelper, OCommonAccessibleComponent, OAccessibleExtendedComponentHelper_Base )
        // (order matters: the first is the class name, the second is the class doing the ref counting)

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL OAccessibleExtendedComponentHelper::containsPoint( const Point& _rPoint ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::containsPoint( _rPoint );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleExtendedComponentHelper::getLocation(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocation( );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleExtendedComponentHelper::getLocationOnScreen(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocationOnScreen( );
    }

    //--------------------------------------------------------------------
    Size SAL_CALL OAccessibleExtendedComponentHelper::getSize(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getSize( );
    }

    //--------------------------------------------------------------------
    Rectangle SAL_CALL OAccessibleExtendedComponentHelper::getBounds(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getBounds( );
    }

//.........................................................................
}   // namespace comphelper
//.........................................................................


