/*************************************************************************
 *
 *  $RCSfile: valueacc.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: ka $ $Date: 2002-02-25 16:39:54 $
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

#define _SV_VALUESET_CXX
#define private public

#include "valueset.hxx"
#include "valueimp.hxx"

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEROLE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star;

// ----------------
// - ValueSetItem -
// ----------------

ValueSetItem::ValueSetItem( ValueSet& rParent ) :
    mrParent( rParent ),
    mnId( 0 ),
    mnBits( 0 ),
    mpData( NULL ),
    mpxAcc( NULL )
{
}

// -----------------------------------------------------------------------

ValueSetItem::~ValueSetItem()
{
    if( mpxAcc )
    {
        static_cast< ValueItemAcc* >( mpxAcc->get() )->ParentDestroyed();
        delete mpxAcc;
    }
}

// -----------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > ValueSetItem::GetAccessible()
{
    if( !mpxAcc )
        mpxAcc = new uno::Reference< accessibility::XAccessible >( new ValueItemAcc( this ) );

    return *mpxAcc;
}

// -----------------------------------------------------------------------

void ValueSetItem::ClearAccessible()
{
    if( mpxAcc )
        delete mpxAcc, mpxAcc = NULL;
}


// ---------------
// - ValueSetAcc -
// ---------------

ValueSetAcc::ValueSetAcc( ValueSet* pParent ) :
    mpParent( pParent )
{
}

// -----------------------------------------------------------------------------

ValueSetAcc::~ValueSetAcc()
{
}

// -----------------------------------------------------------------------

void ValueSetAcc::FireAccessibleEvent( short nEventId, const uno::Any& rOldValue, const uno::Any& rNewValue )
{
    if( nEventId )
    {
        maMutex.acquire();
        ::std::vector< uno::Reference< accessibility::XAccessibleEventListener > > aTmpListeners( mxEventListeners );
        maMutex.release();

        ::std::vector< uno::Reference< accessibility::XAccessibleEventListener > >::const_iterator  aIter( aTmpListeners.begin() );
        accessibility::AccessibleEventObject                                                        aEvtObject;

        aEvtObject.EventId = nEventId;
        aEvtObject.NewValue = rNewValue;
        aEvtObject.OldValue = rOldValue;

        while( aIter != aTmpListeners.end() )
            (*aIter++)->notifyEvent( aEvtObject );
    }
}

// -----------------------------------------------------------------------------

const uno::Sequence< sal_Int8 >& ValueSetAcc::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq;

    if( !aSeq.getLength() )
    {
        static osl::Mutex           aCreateMutex;
        osl::Guard< osl::Mutex >    aGuard( aCreateMutex );

        aSeq.realloc( 16 );
        rtl_createUuid( reinterpret_cast< sal_uInt8* >( aSeq.getArray() ), 0, sal_True );
    }

    return aSeq;
}

// -----------------------------------------------------------------------------

ValueSetAcc* ValueSetAcc::getImplementation( const uno::Reference< uno::XInterface >& rxData )
    throw()
{
    try
    {
        uno::Reference< lang::XUnoTunnel > xUnoTunnel( rxData, uno::UNO_QUERY );
        return( xUnoTunnel.is() ? ( (ValueSetAcc*)(void*) xUnoTunnel->getSomething( ValueSetAcc::getUnoTunnelId() ) ) : NULL );
    }
    catch( const ::com::sun::star::uno::Exception& )
    {
        return NULL;
    }
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessibleContext > SAL_CALL ValueSetAcc::getAccessibleContext()
    throw (uno::RuntimeException)
{
    return this;
}

// -----------------------------------------------------------------------------

sal_Int32 SAL_CALL ValueSetAcc::getAccessibleChildCount()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    return( mpParent->ImplGetVisibleItemCount() );
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueSetAcc::getAccessibleChild( sal_Int32 i )
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    uno::Reference< accessibility::XAccessible >    xRet;
    ValueSetItem*                                   pItem = mpParent->ImplGetVisibleItem( static_cast< USHORT >( i ) );

    if( pItem )
        xRet = pItem->GetAccessible();
    else
        throw lang::IndexOutOfBoundsException();

    return xRet;
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueSetAcc::getAccessibleParent()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    Window*                                         pParent = mpParent->GetParent();
    uno::Reference< accessibility::XAccessible >    xRet;

    if( pParent )
        xRet = pParent->GetAccessible();

    return xRet;
}

// -----------------------------------------------------------------------------

sal_Int32 SAL_CALL ValueSetAcc::getAccessibleIndexInParent()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard     aGuard( maMutex );
    Window*                 pParent = mpParent->GetParent();
    sal_Int32               nRet = 0;

    if( pParent )
    {
        sal_Bool bFound = sal_False;

        for( USHORT i = 0, nCount = pParent->GetChildCount(); ( i < nCount ) && !bFound; i++ )
        {
            if( pParent->GetChild( i ) == mpParent )
            {
                nRet = i;
                bFound = sal_True;
            }
        }
    }

    return nRet;
}

// -----------------------------------------------------------------------------

sal_Int16 SAL_CALL ValueSetAcc::getAccessibleRole()
    throw (uno::RuntimeException)
{
    return accessibility::AccessibleRole::UNKNOWN;
}

// -----------------------------------------------------------------------------

::rtl::OUString SAL_CALL ValueSetAcc::getAccessibleDescription()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    return mpParent->GetText();
}

// -----------------------------------------------------------------------------

::rtl::OUString SAL_CALL ValueSetAcc::getAccessibleName()
    throw (uno::RuntimeException)
{
    return ::rtl::OUString::createFromAscii( "ValueSet" );
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessibleRelationSet > SAL_CALL ValueSetAcc::getAccessibleRelationSet()
    throw (uno::RuntimeException)
{
    // !!!
    return uno::Reference< accessibility::XAccessibleRelationSet >();
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessibleStateSet > SAL_CALL ValueSetAcc::getAccessibleStateSet()
    throw (uno::RuntimeException)
{
    // !!!
    return uno::Reference< accessibility::XAccessibleStateSet >();
}

// -----------------------------------------------------------------------------

lang::Locale SAL_CALL ValueSetAcc::getLocale()
    throw (accessibility::IllegalAccessibleComponentStateException, uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    const ::rtl::OUString                           aEmptyStr;
    uno::Reference< accessibility::XAccessible >    xParent( getAccessibleParent() );
    lang::Locale                                    aRet( aEmptyStr, aEmptyStr, aEmptyStr );

    if( xParent.is() )
    {
        uno::Reference< accessibility::XAccessibleContext > xParentContext( xParent->getAccessibleContext() );

        if( xParentContext.is() )
            aRet = xParentContext->getLocale ();
    }

    return aRet;
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::addEventListener( const uno::Reference< accessibility::XAccessibleEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );

    if( rxListener.is() )
    {
           ::std::vector< uno::Reference< accessibility::XAccessibleEventListener > >::const_iterator aIter = mxEventListeners.begin();
        sal_Bool bFound = sal_False;

        while( !bFound && ( aIter != mxEventListeners.end() ) )
        {
            if( *aIter++ == rxListener )
                bFound = sal_True;
        }

        if (!bFound)
            mxEventListeners.push_back( rxListener );
    }
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::removeEventListener( const uno::Reference< accessibility::XAccessibleEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );

    if( rxListener.is() )
    {
           ::std::vector< uno::Reference< accessibility::XAccessibleEventListener > >::iterator aIter = mxEventListeners.begin();
        sal_Bool bFound = sal_False;

        while( !bFound && ( aIter != mxEventListeners.end() ) )
        {
            if( *aIter == rxListener )
            {
                mxEventListeners.erase( aIter );
                bFound = sal_True;
            }
            else
                aIter++;
        }
    }
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueSetAcc::contains( const awt::Point& aPoint )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    const Rectangle     aOutRect( Point(), mpParent->GetOutputSizePixel() );

    return aOutRect.IsInside( Point( aPoint.X, aPoint.Y ) );
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueSetAcc::getAccessibleAt( const awt::Point& aPoint )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    const USHORT                                    nItemId = mpParent->GetItemId( Point( aPoint.X, aPoint.Y ) );
    uno::Reference< accessibility::XAccessible >    xRet;

    if( VALUESET_ITEM_NOTFOUND != nItemId )
    {
        const USHORT nItemPos = mpParent->GetItemPos( nItemId );

        if( VALUESET_ITEM_NONEITEM != nItemPos )
        {
            ValueSetItem* pItem = mpParent->mpItemList->GetObject( nItemPos );

            if( ( pItem->meType != VALUESETITEM_SPACE ) && !pItem->maRect.IsEmpty() )
                xRet = pItem->GetAccessible();
        }
    }

    return xRet;
}

// -----------------------------------------------------------------------------

awt::Rectangle SAL_CALL ValueSetAcc::getBounds()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    const Point         aOutPos( mpParent->GetPosPixel() );
    const Size          aOutSize( mpParent->GetOutputSizePixel() );
    awt::Rectangle      aRet;

    aRet.X = aOutPos.X();
    aRet.Y = aOutPos.Y();
    aRet.Width = aOutSize.Width();
    aRet.Height = aOutSize.Height();

    return aRet;
}

// -----------------------------------------------------------------------------

awt::Point SAL_CALL ValueSetAcc::getLocation()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    const Point         aOutPos( mpParent->GetPosPixel() );
    awt::Point          aRet;

    aRet.X = aOutPos.X();
    aRet.Y = aOutPos.Y();

    return aRet;
}

// -----------------------------------------------------------------------------

awt::Point SAL_CALL ValueSetAcc::getLocationOnScreen()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    const Point         aScreenPos( mpParent->OutputToAbsoluteScreenPixel( Point() ) );
    awt::Point          aRet;

    aRet.X = aScreenPos.X();
    aRet.Y = aScreenPos.Y();

    return aRet;
}

// -----------------------------------------------------------------------------

awt::Size SAL_CALL ValueSetAcc::getSize()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    const Size          aOutSize( mpParent->GetOutputSizePixel() );
    awt::Size           aRet;

    aRet.Width = aOutSize.Width();
    aRet.Height = aOutSize.Height();

    return aRet;
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueSetAcc::isShowing()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    return mpParent->IsVisible();
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueSetAcc::isVisible()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    return mpParent->IsVisible();
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueSetAcc::isFocusTraversable()
    throw (uno::RuntimeException)
{
    return sal_True;
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::addFocusListener( const uno::Reference< awt::XFocusListener >& rxListener )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );

    if( rxListener.is() )
    {
           ::std::vector< uno::Reference< awt::XFocusListener > >::const_iterator aIter = mxFocusListeners.begin();
        sal_Bool bFound = sal_False;

        while( !bFound && ( aIter != mxFocusListeners.end() ) )
        {
            if( *aIter++ == rxListener )
                bFound = sal_True;
        }

        if (!bFound)
            mxFocusListeners.push_back( rxListener );
    }
}

// -----------------------------------------------------------------------------
void SAL_CALL ValueSetAcc::removeFocusListener( const uno::Reference< awt::XFocusListener >& rxListener )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );

    if( rxListener.is() )
    {
           ::std::vector< uno::Reference< awt::XFocusListener > >::iterator aIter = mxFocusListeners.begin();
        sal_Bool bFound = sal_False;

        while( !bFound && ( aIter != mxFocusListeners.end() ) )
        {
            if( *aIter == rxListener )
            {
                mxFocusListeners.erase( aIter );
                bFound = sal_True;
            }
            else
                aIter++;
        }
    }
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::grabFocus()
    throw (uno::RuntimeException)
{
    mpParent->GrabFocus();
}

// -----------------------------------------------------------------------------

uno::Any SAL_CALL ValueSetAcc::getAccessibleKeyBinding()
    throw (uno::RuntimeException)
{
    return uno::Any();
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::selectAccessibleChild( sal_Int32 nChildIndex )
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    ValueSetItem*       pItem = mpParent->ImplGetVisibleItem( static_cast< USHORT >( nChildIndex ) );

    if( pItem )
        mpParent->SelectItem( pItem->mnId );
    else
        throw lang::IndexOutOfBoundsException();
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueSetAcc::isAccessibleChildSelected( sal_Int32 nChildIndex )
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    ValueSetItem*       pItem = mpParent->ImplGetVisibleItem( static_cast< USHORT >( nChildIndex ) );
    sal_Bool            bRet = sal_False;

    if( pItem )
        bRet = mpParent->IsItemSelected( pItem->mnId );
    else
        throw lang::IndexOutOfBoundsException();

    return bRet;
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::clearAccessibleSelection()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    mpParent->SetNoSelection();
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::selectAllAccessible()
    throw (uno::RuntimeException)
{
    // unsupported due to single selection only
}

// -----------------------------------------------------------------------------

sal_Int32 SAL_CALL ValueSetAcc::getSelectedAccessibleChildCount()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    sal_Int32           nRet = 0;

    for( USHORT i = 0, nCount = mpParent->ImplGetVisibleItemCount(); i < nCount; i++ )
    {
        ValueSetItem* pItem = mpParent->ImplGetVisibleItem( i );

        if( pItem && mpParent->IsItemSelected( pItem->mnId ) )
            ++nRet;
    }

    return nRet;
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueSetAcc::getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex )
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    uno::Reference< accessibility::XAccessible >    xRet;

    for( USHORT i = 0, nCount = mpParent->ImplGetVisibleItemCount(), nSel = 0; ( i < nCount ) && !xRet.is(); i++ )
    {
        ValueSetItem* pItem = mpParent->ImplGetVisibleItem( i );

        if( pItem && mpParent->IsItemSelected( pItem->mnId ) && ( nSelectedChildIndex == static_cast< sal_Int32 >( nSel++ ) ) )
            xRet = pItem->GetAccessible();
    }

    return xRet;
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueSetAcc::deselectSelectedAccessibleChild( sal_Int32 nSelectedChildIndex )
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    sal_Bool            bDone = sal_False;

    for( USHORT i = 0, nCount = mpParent->ImplGetVisibleItemCount(), nSel = 0; ( i < nCount ) && !bDone; i++ )
    {
        ValueSetItem* pItem = mpParent->ImplGetVisibleItem( i );

        if( pItem && mpParent->IsItemSelected( pItem->mnId ) && ( nSelectedChildIndex == static_cast< sal_Int32 >( nSel++ ) ) )
        {
            mpParent->SetNoSelection();
            bDone = sal_True;
        }
    }
}

// -----------------------------------------------------------------------------

sal_Int64 SAL_CALL ValueSetAcc::getSomething( const uno::Sequence< sal_Int8 >& rId ) throw( uno::RuntimeException )
{
    sal_Int64 nRet;

    if( ( rId.getLength() == 16 ) && ( 0 == rtl_compareMemory( ValueSetAcc::getUnoTunnelId().getConstArray(), rId.getConstArray(), 16 ) ) )
        nRet = reinterpret_cast< sal_Int64 >( this );
    else
        nRet = 0;

    return nRet;
}

// ----------------
// - ValueItemAcc -
// ----------------

ValueItemAcc::ValueItemAcc( ValueSetItem* pParent ) :
    mpParent( pParent )
{
}

// -----------------------------------------------------------------------------

ValueItemAcc::~ValueItemAcc()
{
}

// -----------------------------------------------------------------------------

void ValueItemAcc::ParentDestroyed()
{
    const ::vos::OGuard aGuard( maMutex );
    mpParent = NULL;
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessibleContext > SAL_CALL ValueItemAcc::getAccessibleContext()
    throw (uno::RuntimeException)
{
    return this;
}

// -----------------------------------------------------------------------------

sal_Int32 SAL_CALL ValueItemAcc::getAccessibleChildCount()
    throw (uno::RuntimeException)
{
    return 0;
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueItemAcc::getAccessibleChild( sal_Int32 i )
    throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
{
    throw lang::IndexOutOfBoundsException();
    return uno::Reference< accessibility::XAccessible >();
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueItemAcc::getAccessibleParent()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    uno::Reference< accessibility::XAccessible >    xRet;

    if( mpParent )
        xRet = mpParent->mrParent.GetAccessible();

    return xRet;
}

// -----------------------------------------------------------------------------

sal_Int32 SAL_CALL ValueItemAcc::getAccessibleIndexInParent()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    sal_Int32           nRet = 0;

    if( mpParent )
    {
        sal_Bool bDone = sal_False;

        for( USHORT i = 0, nCount = mpParent->mrParent.ImplGetVisibleItemCount(); ( i < nCount ) && !bDone; i++ )
        {
            ValueSetItem* pItem = mpParent->mrParent.ImplGetVisibleItem( i );

            if( pItem && ( pItem->GetAccessible().get() == this ) )
            {
                nRet = i;
                bDone = sal_True;
            }
        }
    }

    return nRet;
}

// -----------------------------------------------------------------------------

sal_Int16 SAL_CALL ValueItemAcc::getAccessibleRole()
    throw (uno::RuntimeException)
{
    return accessibility::AccessibleRole::UNKNOWN;
}

// -----------------------------------------------------------------------------

::rtl::OUString SAL_CALL ValueItemAcc::getAccessibleDescription()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    ::rtl::OUString     aRet;

    if( mpParent )
        aRet = mpParent->maText;

    return aRet;
}

// -----------------------------------------------------------------------------

::rtl::OUString SAL_CALL ValueItemAcc::getAccessibleName()
    throw (uno::RuntimeException)
{
    return ::rtl::OUString::createFromAscii( "ValueSetItem" );
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessibleRelationSet > SAL_CALL ValueItemAcc::getAccessibleRelationSet()
    throw (uno::RuntimeException)
{
    // !!!
    return uno::Reference< accessibility::XAccessibleRelationSet >();
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessibleStateSet > SAL_CALL ValueItemAcc::getAccessibleStateSet()
    throw (uno::RuntimeException)
{
    // !!!
    return uno::Reference< accessibility::XAccessibleStateSet >();
}

// -----------------------------------------------------------------------------

lang::Locale SAL_CALL ValueItemAcc::getLocale()
    throw (accessibility::IllegalAccessibleComponentStateException, uno::RuntimeException)
{
    const ::vos::OGuard                             aGuard( maMutex );
    const ::rtl::OUString                           aEmptyStr;
    uno::Reference< accessibility::XAccessible >    xParent( getAccessibleParent() );
    lang::Locale                                    aRet( aEmptyStr, aEmptyStr, aEmptyStr );

    if( xParent.is() )
    {
        uno::Reference< accessibility::XAccessibleContext > xParentContext( xParent->getAccessibleContext() );

        if( xParentContext.is() )
            aRet = xParentContext->getLocale();
    }

    return aRet;
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueItemAcc::addEventListener( const uno::Reference< accessibility::XAccessibleEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );

    if( rxListener.is() )
    {
           ::std::vector< uno::Reference< accessibility::XAccessibleEventListener > >::const_iterator aIter = mxEventListeners.begin();
        sal_Bool bFound = sal_False;

        while( !bFound && ( aIter != mxEventListeners.end() ) )
        {
            if( *aIter++ == rxListener )
                bFound = sal_True;
        }

        if (!bFound)
            mxEventListeners.push_back( rxListener );
    }
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueItemAcc::removeEventListener( const uno::Reference< accessibility::XAccessibleEventListener >& rxListener )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );

    if( rxListener.is() )
    {
           ::std::vector< uno::Reference< accessibility::XAccessibleEventListener > >::iterator aIter = mxEventListeners.begin();
        sal_Bool bFound = sal_False;

        while( !bFound && ( aIter != mxEventListeners.end() ) )
        {
            if( *aIter == rxListener )
            {
                mxEventListeners.erase( aIter );
                bFound = sal_True;
            }
            else
                aIter++;
        }
    }
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueItemAcc::contains( const awt::Point& aPoint )
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    sal_Bool            bRet = sal_False;

    if( mpParent )
        bRet = Rectangle( Point(), mpParent->maRect.GetSize() ).IsInside( Point( aPoint.X, aPoint.Y ) );

    return bRet;
}

// -----------------------------------------------------------------------------

uno::Reference< accessibility::XAccessible > SAL_CALL ValueItemAcc::getAccessibleAt( const awt::Point& aPoint )
    throw (uno::RuntimeException)
{
    return( uno::Reference< accessibility::XAccessible >() );
}

// -----------------------------------------------------------------------------

awt::Rectangle SAL_CALL ValueItemAcc::getBounds()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    awt::Rectangle      aRet;

    if( mpParent )
    {
        aRet.X = mpParent->maRect.Left();
        aRet.Y = mpParent->maRect.Top();
        aRet.Width = mpParent->maRect.GetWidth();
        aRet.Height = mpParent->maRect.GetHeight();
    }

    return aRet;
}

// -----------------------------------------------------------------------------

awt::Point SAL_CALL ValueItemAcc::getLocation()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    awt::Point          aRet;

    if( mpParent )
    {
        aRet.X = mpParent->maRect.Left();
        aRet.Y = mpParent->maRect.Top();
    }

    return aRet;
}

// -----------------------------------------------------------------------------

awt::Point SAL_CALL ValueItemAcc::getLocationOnScreen()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    awt::Point          aRet;

    if( mpParent )
    {
        const Point aScreenPos( mpParent->mrParent.OutputToAbsoluteScreenPixel( mpParent->maRect.TopLeft() ) );

        aRet.X = aScreenPos.X();
        aRet.Y = aScreenPos.Y();
    }

    return aRet;
}

// -----------------------------------------------------------------------------

awt::Size SAL_CALL ValueItemAcc::getSize()
    throw (uno::RuntimeException)
{
    const ::vos::OGuard aGuard( maMutex );
    awt::Size           aRet;

    if( mpParent )
    {
        aRet.Width = mpParent->maRect.GetWidth();
        aRet.Height = mpParent->maRect.GetHeight();
    }

    return aRet;
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueItemAcc::isShowing()
    throw (uno::RuntimeException)
{
    return sal_True;
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueItemAcc::isVisible()
    throw (uno::RuntimeException)
{
    return sal_True;
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL ValueItemAcc::isFocusTraversable()
    throw (uno::RuntimeException)
{
    return sal_False;
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueItemAcc::addFocusListener( const uno::Reference< awt::XFocusListener >& rxListener )
    throw (uno::RuntimeException)
{
    // nothing to do
}

// -----------------------------------------------------------------------------
void SAL_CALL ValueItemAcc::removeFocusListener( const uno::Reference< awt::XFocusListener >& rxListener )
    throw (uno::RuntimeException)
{
    // nothing to do
}

// -----------------------------------------------------------------------------

void SAL_CALL ValueItemAcc::grabFocus()
    throw (uno::RuntimeException)
{
    // nothing to do
}

// -----------------------------------------------------------------------------

uno::Any SAL_CALL ValueItemAcc::getAccessibleKeyBinding()
    throw (uno::RuntimeException)
{
    return uno::Any();
}
