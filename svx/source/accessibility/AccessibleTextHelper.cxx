/*************************************************************************
 *
 *  $RCSfile: AccessibleTextHelper.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: thb $ $Date: 2002-05-17 19:09:14 $
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

#pragma hdrstop

//------------------------------------------------------------------------
//
// Global header
//
//------------------------------------------------------------------------

#include <limits.h>
#include <memory>
#include <algorithm>

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif

#ifndef _CPPUHELPER_WEAKREF_HXX_
#include <cppuhelper/weakref.hxx>
#endif

#ifndef _COM_SUN_STAR_AWT_POINT_HPP_
#include <com/sun/star/awt/Point.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_RECTANGLE_HPP_
#include <com/sun/star/awt/Rectangle.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEEVENTID_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleEventId.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <drafts/com/sun/star/accessibility/XAccessible.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECONTEXT_HPP_
#include <drafts/com/sun/star/accessibility/XAccessibleContext.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLECOMPONENT_HPP_
#include <drafts/com/sun/star/accessibility/XAccessibleComponent.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEROLE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleRole.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLETEXTTYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleTextType.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLETEXT_HPP_
#include <drafts/com/sun/star/accessibility/XAccessibleText.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLEEDITABLETEXT_HPP_
#include <drafts/com/sun/star/accessibility/XAccessibleEditableText.hpp>
#endif

#ifndef _DRAFTS_COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLESTATETYPE_HPP_
#include <drafts/com/sun/star/accessibility/AccessibleStateType.hpp>
#endif

#ifndef _UTL_ACCESSIBLESTATESETHELPER_HXX_
#include <unotools/accessiblestatesethelper.hxx>
#endif


//------------------------------------------------------------------------
//
// Project-local header
//
//------------------------------------------------------------------------

#ifndef _SVX_ACCESSILE_TEXT_HELPER_HXX_
#include "AccessibleTextHelper.hxx"
#endif

#ifndef _SVX_UNOSHAPE_HXX
#include "unoshape.hxx"
#endif

#ifndef _UNO_LINGU_HXX
#include "unolingu.hxx"
#endif

#ifndef _SVX_UNOTEXT_HXX
#include "unotext.hxx"
#endif

#include "unoedhlp.hxx"
#include "unopracc.hxx"
#include "AccessibleParaManager.hxx"
#include "AccessibleEditableTextPara.hxx"
#include "svdmodel.hxx"
#include "svdpntv.hxx"
#include "editdata.hxx"
#include "editeng.hxx"
#include "editview.hxx"

using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star::accessibility;

namespace accessibility
{

//------------------------------------------------------------------------
//
// AccessibleTextHelper_Impl declaration
//
//------------------------------------------------------------------------

    class AccessibleTextHelper_Impl : public SfxListener
    {

    public:

        // receive pointer to our frontend class and view window
        AccessibleTextHelper_Impl( const uno::Reference< XAccessible >& rInterface );
        ~AccessibleTextHelper_Impl();

        // XAccessibleContext child handling methods
        sal_Int32 SAL_CALL getAccessibleChildCount() throw (uno::RuntimeException);
        uno::Reference< XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException);

        // XAccessibleEventBroadcaster child related methods
        void SAL_CALL addEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) throw (uno::RuntimeException);
        void SAL_CALL removeEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) throw (uno::RuntimeException);

        // XAccessibleComponent child related methods
        uno::Reference< XAccessible > SAL_CALL getAccessibleAt( const awt::Point& aPoint ) throw (uno::RuntimeException);

        SvxEditSourceAdapter& GetEditSource() const throw (uno::RuntimeException);
        void SetEditSource( ::std::auto_ptr< SvxEditSource > pEditSource ) throw (uno::RuntimeException);

        void SetOffset( const Point& );
        const Point& GetOffset() const { return maOffset; } // Strictly correct only with locked solar mutex!

        sal_Bool IsSelected() const;

        // do NOT hold object mutex when calling this! Danger of deadlock
        void FireEvent( const sal_Int16 nEventId, const uno::Any& rNewValue = uno::Any(), const uno::Any& rOldValue = uno::Any() ) const;

#ifdef DBG_UTIL
        void CheckInvariants() const;
#endif

        // checks all children for visibility, throws away invisible ones
        void UpdateVisibleChildren();

    private:

        // syntactic sugar for FireEvent
        void GotPropertyEvent( const uno::Any& rNewValue, const sal_Int16 nEventId ) const { FireEvent( nEventId, rNewValue ); }
        void LostPropertyEvent( const uno::Any& rOldValue, const sal_Int16 nEventId ) const { FireEvent( nEventId, uno::Any(), rOldValue ); }

        // shutdown usage of current edit source on myself and the children.
        void ShutdownEditSource() throw (uno::RuntimeException);

        virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

        // lock solar mutex before
        SvxTextForwarder& GetTextForwarder() const throw (uno::RuntimeException);
        // lock solar mutex before
        SvxViewForwarder& GetViewForwarder() const throw (uno::RuntimeException);
        // lock solar mutex before
        SvxEditViewForwarder& GetEditViewForwarder( sal_Bool bCreate = sal_False ) const throw (uno::RuntimeException);

        // are we in edit mode?
        sal_Bool IsActive() const throw (uno::RuntimeException);

        void UpdateVisibleData();

        // calls SetSelection on the forwarder and updates maLastSelection
        // cache. Caution: calls StateChangeEvent and sets object into
        // edit mode!
        void UpdateSelection( const ESelection& );

        /// our current offset to the containing shape/cell (guarded by solar mutex)
        Point maOffset;

        // our frontend class (the one implementing the actual
        // interface). That's not necessarily the one containing the impl
        // pointer!
        const uno::Reference< XAccessible > mxFrontEnd;

        // a wrapper for the text forwarders (guarded by solar mutex)
        mutable SvxEditSourceAdapter maEditSource;

        // store last selection (to correctly report selection changes, guarded by solar mutex)
        ESelection maLastSelection;

        // cache range of visible children (guarded by solar mutex)
        sal_Int32 mnFirstVisibleChild;
        sal_Int32 mnLastVisibleChild;

        // the object handling our children (guarded by solar mutex)
        accessibility::AccessibleParaManager maParaManager;

        // spin lock to prevent notify in notify
        sal_Bool mbInNotify;

        // must be before maStateListeners, has to live longer
        mutable ::osl::Mutex maMutex;

        // handles our event listeners (guarded by maMutex)
        ::cppu::OInterfaceContainerHelper maStateListeners;

    };

    //------------------------------------------------------------------------
    //
    // AccessibleTextHelper_Impl implementation
    //
    //------------------------------------------------------------------------

    AccessibleTextHelper_Impl::AccessibleTextHelper_Impl( const uno::Reference< XAccessible >& rInterface ) :
        maOffset(0,0),
        mxFrontEnd( rInterface ),
        maLastSelection( 0,0,0,0 ),
        mnFirstVisibleChild( -1 ),
        mnLastVisibleChild( -2 ),
        mbInNotify( sal_False ),
        maStateListeners( maMutex )
    {
    }

    AccessibleTextHelper_Impl::~AccessibleTextHelper_Impl()
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        try
        {
            // shutdown and release edit source
            SetEditSource( ::std::auto_ptr< SvxEditSource >() );
        }
        catch( const uno::RuntimeException& ) {}

        // owner is responsible for dispose and clear on listeners
    }

    SvxTextForwarder& AccessibleTextHelper_Impl::GetTextForwarder() const throw (uno::RuntimeException)
    {
        if( !maEditSource.IsValid() )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown edit source")), mxFrontEnd);

        SvxTextForwarder* pTextForwarder = maEditSource.GetTextForwarder();

        if( !pTextForwarder )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to fetch text forwarder, model might be dead")), mxFrontEnd);

        if( pTextForwarder->IsValid() )
            return *pTextForwarder;
        else
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Text forwarder is invalid, model might be dead")), mxFrontEnd);
    }

    SvxViewForwarder& AccessibleTextHelper_Impl::GetViewForwarder() const throw (uno::RuntimeException)
    {
        if( !maEditSource.IsValid() )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown edit source")), mxFrontEnd);

        SvxViewForwarder* pViewForwarder = maEditSource.GetViewForwarder();

        if( !pViewForwarder )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to fetch view forwarder, model might be dead")), mxFrontEnd);

        if( pViewForwarder->IsValid() )
            return *pViewForwarder;
        else
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("View forwarder is invalid, model might be dead")), mxFrontEnd);
    }

    SvxEditViewForwarder& AccessibleTextHelper_Impl::GetEditViewForwarder( sal_Bool bCreate ) const throw (uno::RuntimeException)
    {
        if( !maEditSource.IsValid() )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unknown edit source")), mxFrontEnd);

        SvxEditViewForwarder* pViewForwarder = maEditSource.GetEditViewForwarder( bCreate );

        if( !pViewForwarder )
        {
            if( bCreate )
                throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Unable to fetch edit view forwarder, model might be dead")), mxFrontEnd);
            else
                throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("No edit view forwarder, object not in edit mode")), mxFrontEnd);
        }

        if( pViewForwarder->IsValid() )
            return *pViewForwarder;
        else
        {
            if( bCreate )
                throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("View forwarder is invalid, model might be dead")), mxFrontEnd);
            else
                throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("View forwarder is invalid, object not in edit mode")), mxFrontEnd);
        }
    }

    SvxEditSourceAdapter& AccessibleTextHelper_Impl::GetEditSource() const throw (uno::RuntimeException)
    {
        if( maEditSource.IsValid() )
            return maEditSource;
        else
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AccessibleTextHelper_Impl::GetEditSource: no edit source")), mxFrontEnd );
    }

    sal_Bool AccessibleTextHelper_Impl::IsSelected() const
    {
        sal_Bool bRet = sal_False;

        try
        {
            ESelection aSelection;
            bRet = GetEditViewForwarder().GetSelection( aSelection );
        }
        catch( const uno::RuntimeException& ) {}

        return bRet;
    }

    sal_Bool AccessibleTextHelper_Impl::IsActive() const throw (uno::RuntimeException)
    {
        SvxEditSource& rEditSource = GetEditSource();
        SvxEditViewForwarder* pViewForwarder = rEditSource.GetEditViewForwarder();

        if( !pViewForwarder )
            return sal_False;

        if( pViewForwarder->IsValid() )
            return sal_True;
        else
            return sal_False;
    }

    void AccessibleTextHelper_Impl::UpdateSelection( const ESelection& rSelection )
    {
        if( !maLastSelection.IsEqual( rSelection ) )
        {
            // notify all affected paragraphs (TODO: may be suboptimal,
            // since some paragraphs might stay selected)

            maParaManager.FireEvent( maLastSelection.nStartPara,
                                     maLastSelection.nEndPara,
                                     AccessibleEventId::ACCESSIBLE_SELECTION_EVENT );

            maParaManager.FireEvent( maLastSelection.nStartPara,
                                     maLastSelection.nStartPara,
                                     AccessibleEventId::ACCESSIBLE_CARET_EVENT );

            maParaManager.FireEvent( rSelection.nStartPara,
                                     rSelection.nEndPara,
                                     AccessibleEventId::ACCESSIBLE_SELECTION_EVENT );

            maParaManager.FireEvent( rSelection.nStartPara,
                                     rSelection.nStartPara,
                                     AccessibleEventId::ACCESSIBLE_CARET_EVENT );

            maLastSelection = rSelection;
        }
    }

    // functor for sending child events
    class AccessibleTextHelper_SendAccessibleChildEvent : public ::std::unary_function< accessibility::AccessibleEditableTextPara&, void >
    {
    public:
        AccessibleTextHelper_SendAccessibleChildEvent( AccessibleTextHelper_Impl& rImpl ) : mrImpl(rImpl) {}
        void operator()( accessibility::AccessibleEditableTextPara& rPara )
        {
            uno::Reference< XAccessible > xChild( static_cast< ::cppu::OWeakObject* > ( &rPara ), uno::UNO_QUERY );
            mrImpl.FireEvent(AccessibleEventId::ACCESSIBLE_CHILD_EVENT, uno::Any(), uno::makeAny( xChild ) );
        }

    private:
        AccessibleTextHelper_Impl&  mrImpl;
    };

    void AccessibleTextHelper_Impl::ShutdownEditSource() throw (uno::RuntimeException)
    {
        // This should only be called with solar mutex locked, i.e. from the main office thread

        // invalidate children
        maParaManager.SetEditSource( NULL );

        // loosing all children
        maParaManager.ForEach( AccessibleTextHelper_SendAccessibleChildEvent( *this ) );
        maParaManager.SetNum(0);

        // quit listen on stale edit source
        if( maEditSource.IsValid() )
            EndListening( maEditSource.GetBroadcaster() );

        maEditSource.SetEditSource( ::std::auto_ptr< SvxEditSource >(NULL) );
    }

    void AccessibleTextHelper_Impl::SetEditSource( ::std::auto_ptr< SvxEditSource > pEditSource ) throw (uno::RuntimeException)
    {
        // This should only be called with solar mutex locked, i.e. from the main office thread

        // shutdown old edit source
        ShutdownEditSource();

        // set new edit source
        maEditSource.SetEditSource( pEditSource );

        // init child vector to the current child count
        if( maEditSource.IsValid() )
        {
            maParaManager.SetNum( GetTextForwarder().GetParagraphCount() );

            // listen on new edit source
            StartListening( maEditSource.GetBroadcaster() );

            UpdateVisibleChildren();
        }
    }

    void AccessibleTextHelper_Impl::SetOffset( const Point& rPoint )
    {
        maOffset = rPoint;
        maParaManager.SetEEOffset( rPoint );

        // in all cases, check visibility afterwards.
        UpdateVisibleChildren();
    }

    void AccessibleTextHelper_Impl::UpdateVisibleChildren()
    {
        try
        {
            SvxTextForwarder& rCacheTF = GetTextForwarder();
            SvxViewForwarder& rCacheVF = GetViewForwarder();

            Rectangle aViewArea = rCacheVF.GetVisArea();

            if( IsActive() )
            {
                // maybe the edit view scrolls, adapt aViewArea
                Rectangle aEditViewArea = GetEditViewForwarder().GetVisArea();
                aViewArea += aEditViewArea.TopLeft();

                // now determine intersection
                aViewArea.Intersection( aEditViewArea );
            }

            Rectangle aTmpBB, aParaBB;
            sal_Bool bFirstChild = sal_True;
            sal_Int32 nCurrPara;
            sal_Int32 nParas=rCacheTF.GetParagraphCount();

            mnFirstVisibleChild = -1;
            mnLastVisibleChild = -2;

            for( nCurrPara=0; nCurrPara<nParas; ++nCurrPara )
            {
                DBG_ASSERT(nCurrPara >= 0 && nCurrPara <= USHRT_MAX,
                           "AccessibleTextHelper_Impl::UpdateVisibleChildren: index value overflow");

                aTmpBB = rCacheTF.GetParaBounds( static_cast< USHORT >( nCurrPara ) );

                // convert to screen coordinates
                aParaBB = accessibility::AccessibleEditableTextPara::LogicToPixel( aTmpBB, rCacheTF.GetMapMode(), rCacheVF );

                if( aParaBB.IsOver( aViewArea ) )
                {
                    // at least partially visible
                    if( bFirstChild )
                    {
                        bFirstChild = sal_False;
                        mnFirstVisibleChild = nCurrPara;
                    }

                    mnLastVisibleChild = nCurrPara;

                    GotPropertyEvent( uno::makeAny( getAccessibleChild(nCurrPara) ), AccessibleEventId::ACCESSIBLE_CHILD_EVENT );
                }
                else
                {
                    // not or no longer visible
                    if( maParaManager.IsReferencable( nCurrPara ) )
                    {
                        LostPropertyEvent( uno::makeAny( getAccessibleChild(nCurrPara) ), AccessibleEventId::ACCESSIBLE_CHILD_EVENT );

                        // clear reference
                        maParaManager.Release( nCurrPara );
                    }
                }
            }
        }
        catch( const uno::RuntimeException& )
        {
            DBG_ERROR("AccessibleTextHelper_Impl::UpdateVisibleChildren error while determining visible children");

            // something failed - currently no children
            mnFirstVisibleChild = -1;
            mnLastVisibleChild = -2;

            maParaManager.ForEach( AccessibleTextHelper_SendAccessibleChildEvent( *this ) );
            maParaManager.SetNum(0);
        }
    }

    // functor for checking changes in paragraph bounding boxes
    class AccessibleTextHelper_UpdateChildBounds : public ::std::unary_function< const accessibility::AccessibleParaManager::WeakChild&,
                                                      accessibility::AccessibleParaManager::WeakChild >
    {
    public:
        AccessibleTextHelper_UpdateChildBounds( AccessibleTextHelper_Impl& rImpl ) : mrImpl(rImpl) {}
        accessibility::AccessibleParaManager::WeakChild operator()( const accessibility::AccessibleParaManager::WeakChild& rChild )
        {
            // retrieve hard reference from weak one
            accessibility::AccessibleParaManager::WeakPara::HardRefType aHardRef( rChild.first.get() );

            if( aHardRef.is() )
            {
                awt::Rectangle          aNewRect = aHardRef->getBounds();
                const awt::Rectangle&   aOldRect = rChild.second;

                if( aNewRect.X != aOldRect.X ||
                    aNewRect.Y != aOldRect.Y ||
                    aNewRect.Width != aOldRect.Width ||
                    aNewRect.Height != aOldRect.Height )
                {
                    // visible data changed
                    aHardRef->FireEvent( AccessibleEventId::ACCESSIBLE_VISIBLE_DATA_EVENT );

                    // update bounds
                    return accessibility::AccessibleParaManager::WeakChild( rChild.first, aNewRect );
                }
            }

            // identity transform
            return rChild;
        }

    private:
        AccessibleTextHelper_Impl&  mrImpl;
    };

    void AccessibleTextHelper_Impl::UpdateVisibleData()
    {
        // send CHILD_EVENT to affected children
        ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
        ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

        ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( begin, mnFirstVisibleChild );
        ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( end, mnLastVisibleChild );

        ::std::for_each( begin, end, AccessibleTextHelper_UpdateChildBounds( *this ) );
    }

#ifdef DBG_UTIL
    void AccessibleTextHelper_Impl::CheckInvariants() const
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        if( !mxFrontEnd.is() )
            DBG_ERROR( "AccessibleTextHelper: no frontend" );

        if( mnFirstVisibleChild >= 0 &&
            mnFirstVisibleChild > mnLastVisibleChild )
        {
            DBG_ERROR( "AccessibleTextHelper: range invalid" );
        }
    }
#endif

    void AccessibleTextHelper_Impl::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
    {
        // do not recurse
        if( mbInNotify )
            return;

        mbInNotify = sal_True;

        const SfxSimpleHint* pSimpleHint = PTR_CAST( SfxSimpleHint, &rHint );
        const TextHint* pTextHint = PTR_CAST( TextHint, &rHint );
        const SvxViewHint* pViewHint = PTR_CAST( SvxViewHint, &rHint );
        const SvxEditSourceHint* pEditSourceHint = PTR_CAST( SvxEditSourceHint, &rHint );

        try
        {
            // precondition: solar mutex locked
            if( pEditSourceHint )
            {
                if( pEditSourceHint->GetValue() < GetTextForwarder().GetParagraphCount() )
                {
                    switch( pEditSourceHint->GetId() )
                    {
                        case EDITSOURCE_HINT_PARASMOVED:
                        {
                            DBG_ASSERT( pEditSourceHint->GetStartValue() < GetTextForwarder().GetParagraphCount() &&
                                        pEditSourceHint->GetEndValue() < GetTextForwarder().GetParagraphCount(),
                                        "AccessibleTextHelper_Impl::NotifyHdl: Invalid notification");

                            const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();
                            sal_Int32 nFirst, nMiddle, nLast;

                            /* rotate paragraphs
                             * =================
                             *
                             * Three cases:
                             *
                             * 1.
                             *   ... nParagraph ... nParam1 ... nParam2 ...
                             *       |______________[xxxxxxxxxxx]
                             *              becomes
                             *       [xxxxxxxxxxx]|______________
                             *
                             * tail is 0
                             *
                             * 2.
                             *   ... nParam1 ... nParagraph ... nParam2 ...
                             *       [xxxxxxxxxxx|xxxxxxxxxxxxxx]____________
                             *              becomes
                             *       ____________[xxxxxxxxxxx|xxxxxxxxxxxxxx]
                             *
                             * tail is nParagraph - nParam1
                             *
                             * 3.
                             *   ... nParam1 ... nParam2 ... nParagraph ...
                             *       [xxxxxxxxxxx]___________|____________
                             *              becomes
                             *       ___________|____________[xxxxxxxxxxx]
                             *
                             * tail is nParam2 - nParam1
                             */

                            // sort nParagraph, nParam1 and nParam2 in ascending order, calc range
                            if( pEditSourceHint->GetValue() < pEditSourceHint->GetStartValue() )
                            {
                                nFirst = pEditSourceHint->GetValue();
                                nMiddle = pEditSourceHint->GetStartValue();
                                nLast = pEditSourceHint->GetEndValue();
                            }
                            else if( pEditSourceHint->GetValue() < pEditSourceHint->GetEndValue() )
                            {
                                nFirst = pEditSourceHint->GetStartValue();
                                nMiddle = pEditSourceHint->GetValue();
                                nLast = pEditSourceHint->GetEndValue() + nMiddle - nFirst;
                            }
                            else
                            {
                                nFirst = pEditSourceHint->GetStartValue();
                                nMiddle = pEditSourceHint->GetEndValue();
                                nLast = pEditSourceHint->GetValue() + nMiddle - nFirst;
                            }

                            if( nFirst < nParas && nMiddle < nParas && nLast < nParas )
                            {
                                // since we have no "paragraph index
                                // changed" event on UAA, remove
                                // [first,last] and insert again later (in
                                // UpdateVisibleChildren)

                                // maParaManager.Rotate( nFirst, nMiddle, nLast );

                                maParaManager.Release(nFirst, nLast);

                                // send CHILD_EVENT to affected children
                                ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
                                ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

                                ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( begin, nFirst );
                                ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( end, nLast );

                                ::std::for_each( begin, end,
                                                 ::accessibility::AccessibleParaManager::WeakChildAdapter< AccessibleTextHelper_SendAccessibleChildEvent >
                                                 (AccessibleTextHelper_SendAccessibleChildEvent( *this )) );
                            }
#ifdef DBG_UTIL
                            else
                            {
                                DBG_ERROR("AccessibleTextHelper_Impl::NotifyHdl: Invalid move ranges");
                            }
#endif
                            break;
                        }

                        case EDITSOURCE_HINT_SELECTIONCHANGED:
                            // notify listeners
                            ESelection aSelection;
                            try
                            {
                                if( GetEditViewForwarder().GetSelection( aSelection ) )
                                    UpdateSelection( aSelection );
                                else
                                    UpdateSelection( ESelection() );
                            }
                            // maybe we're not in edit mode (this is not an error)
                            catch( const uno::RuntimeException& ) {}
                            break;
                    }

                    // in all cases, check visibility afterwards.
                    UpdateVisibleChildren();
                }
#ifdef DBG_UTIL
                else
                {
                    DBG_ERROR("AccessibleTextHelper_Impl::NotifyHdl: Invalid notification");
                }
#endif
            }
            else if( pTextHint )
            {
                if( pTextHint->GetValue() < GetTextForwarder().GetParagraphCount() )
                {
                    sal_Bool bUpdateVisibleData = sal_False;

                    switch( pTextHint->GetId() )
                    {
                        case TEXT_HINT_MODIFIED:
                            // notify listeners
                            maParaManager.FireEvent( pTextHint->GetValue(), AccessibleEventId::ACCESSIBLE_TEXT_EVENT );
                            break;

                        case TEXT_HINT_PARAINSERTED:
                        {
                            // resize child vector to the current child count
                            const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();

                            maParaManager.SetNum( nParas );

                            // since we have no "paragraph index
                            // changed" event on UAA, remove
                            // [first,last] and insert again later (in
                            // UpdateVisibleChildren)

                            // move successors of inserted paragraph one position further
                            //maParaManager.MoveRightFrom( pTextHint->GetValue() );

                            // release everything from the insertion position until the end
                            maParaManager.Release(pTextHint->GetValue(), nParas);

                            // send CHILD_EVENT to affected children
                            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
                            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

                            ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( begin, pTextHint->GetValue() );
                            ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( end, nParas );

                            ::std::for_each( begin, end,
                                             ::accessibility::AccessibleParaManager::WeakChildAdapter< AccessibleTextHelper_SendAccessibleChildEvent >
                                             (AccessibleTextHelper_SendAccessibleChildEvent( *this )) );
                            break;
                        }

                        case TEXT_HINT_PARAREMOVED:
                        {
                            const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();

                            // since we have no "paragraph index
                            // changed" event on UAA, remove
                            // [first,last] and insert again later (in
                            // UpdateVisibleChildren)

                            // move successors of removed paragraph one position closer
                            // maParaManager.MoveLeftFrom( pTextHint->GetValue() );

                            // release everything from the remove position until the end
                            maParaManager.Release(pTextHint->GetValue(), nParas);

                            // send CHILD_EVENT to affected children
                            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
                            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

                            ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( begin, pTextHint->GetValue() );
                            ::std::advance< ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator, sal_Int32 >( end, nParas );

                            ::std::for_each( begin, end,
                                             ::accessibility::AccessibleParaManager::WeakChildAdapter< AccessibleTextHelper_SendAccessibleChildEvent >
                                             (AccessibleTextHelper_SendAccessibleChildEvent( *this )) );

                            // resize child vector to the current child count
                            maParaManager.SetNum( nParas );
                            break;
                        }

                        case TEXT_HINT_TEXTHEIGHTCHANGED:
                            bUpdateVisibleData = sal_True;
                            break;

                        case TEXT_HINT_VIEWSCROLLED:
                            bUpdateVisibleData = sal_True;
                            break;
                    }

                    // in all cases, check visibility afterwards.
                    UpdateVisibleChildren();

                    if( bUpdateVisibleData )
                        UpdateVisibleData();
                }
#ifdef DBG_UTIL
                else
                {
                    DBG_ERROR("AccessibleTextHelper_Impl::NotifyHdl: Invalid notification");

                }
#endif
            }
            else if( pViewHint )
            {
                switch( pViewHint->GetId() )
                {
                    case SVX_HINT_VIEWCHANGED:
                        // just check visibility
                        UpdateVisibleData();
                        UpdateVisibleChildren();
                        break;
                }
            }
            // it's VITAL to keep the SfxSimpleHint last! It's the base of some classes above!
            else if( pSimpleHint )
            {
                switch( pSimpleHint->GetId() )
                {
                    case SFX_HINT_DYING:
                        // edit source is dying under us, become defunc then
                        try
                        {
                            // make edit source inaccessible
                            // Note: cannot destroy it here, since we're called from there!
                            ShutdownEditSource();
                        }
                        catch( const uno::RuntimeException& ) {}

                        break;
                }
            }
        }
        catch( const uno::RuntimeException& )
        {
#ifdef DBG_UTIL
            DBG_ERROR("AccessibleTextHelper_Impl::Notify: Unhandled exception.");
#endif
        }

        mbInNotify = sal_False;
    }

    void AccessibleTextHelper_Impl::FireEvent( const sal_Int16 nEventId, const uno::Any& rNewValue, const uno::Any& rOldValue ) const
    {
        // -- object locked --
        ::osl::ClearableMutexGuard aGuard( maMutex );

        AccessibleEventObject aEvent(mxFrontEnd, nEventId, rNewValue, rOldValue);

        ::cppu::OInterfaceIteratorHelper aIter( const_cast< AccessibleTextHelper_Impl* >(this)->maStateListeners );

        // no locking necessary, OInterfaceIteratorHelper copies listeners if someone removes/adds in between
        // Further locking, actually, might lead to deadlocks, since we're calling out of this object
        aGuard.clear();
        // -- until here --

        while( aIter.hasMoreElements() )
        {
            uno::Reference < XAccessibleEventListener > xListener( aIter.next(), uno::UNO_QUERY );

            if( xListener.is() )
            {
                try
                {
                    xListener->notifyEvent( aEvent );
                }
                catch( const uno::RuntimeException& )
                {
#ifdef DBG_UTIL
                    DBG_ERROR("AccessibleTextHelper_Impl::StateChangeEvent: Caught runtime exception from listener (bridge/listener dead?).");
#endif
                }
            }
        }
    }

    // XAccessibleContext
    sal_Int32 SAL_CALL AccessibleTextHelper_Impl::getAccessibleChildCount() throw (uno::RuntimeException)
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        return mnLastVisibleChild - mnFirstVisibleChild + 1;
    }

    uno::Reference< XAccessible > SAL_CALL AccessibleTextHelper_Impl::getAccessibleChild( sal_Int32 i ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        if( 0 > i || i > getAccessibleChildCount() ||
            GetTextForwarder().GetParagraphCount() <= i )
        {
            throw lang::IndexOutOfBoundsException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Invalid child index")), mxFrontEnd);
        }

        return maParaManager.GetChild( i, mxFrontEnd, GetEditSource(), mnFirstVisibleChild + i ).first;
    }

    void SAL_CALL AccessibleTextHelper_Impl::addEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) throw (uno::RuntimeException)
    {
        maStateListeners.addInterface( xListener );
    }

    void SAL_CALL AccessibleTextHelper_Impl::removeEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) throw (uno::RuntimeException)
    {
        maStateListeners.removeInterface( xListener );
    }

    uno::Reference< XAccessible > SAL_CALL AccessibleTextHelper_Impl::getAccessibleAt( const awt::Point& _aPoint ) throw (uno::RuntimeException)
    {
        // make given position relative
        if( !mxFrontEnd.is() )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AccessibleTextHelper_Impl::getAccessibleAt: frontend invalid")), mxFrontEnd );

        uno::Reference< XAccessibleContext > xFrontEndContext = mxFrontEnd->getAccessibleContext();

        if( !xFrontEndContext.is() )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AccessibleTextHelper_Impl::getAccessibleAt: frontend invalid")), mxFrontEnd );

        uno::Reference< XAccessibleComponent > xFrontEndComponent( xFrontEndContext, uno::UNO_QUERY );

        if( !xFrontEndComponent.is() )
            throw uno::RuntimeException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AccessibleTextHelper_Impl::getAccessibleAt: frontend is no XAccessibleComponent")),
                                        mxFrontEnd );

        Point aPoint( _aPoint.X, _aPoint.Y );
        awt::Point aRefPoint = xFrontEndComponent->getLocationOnScreen();
        aPoint -= Point( aRefPoint.X, aRefPoint.Y );

        // respect EditEngine offset to surrounding shape/cell
        aPoint -= GetOffset();

        // convert to EditEngine coordinate system
        SvxTextForwarder& rCacheTF = GetTextForwarder();
        Point aLogPoint( GetViewForwarder().PixelToLogic( aPoint, rCacheTF.GetMapMode() ) );

        // iterate over all visible children (including those not yet created)
        sal_Int32 nChild;
        for( nChild=mnFirstVisibleChild; nChild <= mnLastVisibleChild; ++nChild )
        {
            DBG_ASSERT(nChild >= 0 && nChild <= USHRT_MAX,
                       "AccessibleTextHelper_Impl::getAccessibleAt: index value overflow");

            Rectangle aParaBounds( rCacheTF.GetParaBounds( static_cast< USHORT > (nChild) ) );

            if( aParaBounds.IsInside( aLogPoint ) )
            {
                DBG_ASSERT(nChild - mnFirstVisibleChild >= 0 && nChild - mnFirstVisibleChild <= USHRT_MAX,
                           "AccessibleTextHelper_Impl::getAccessibleAt: index value overflow");

                return getAccessibleChild( static_cast< USHORT > (nChild - mnFirstVisibleChild) );
            }
        }

        // found none
        return uno::Reference< XAccessible >();
    }

    //------------------------------------------------------------------------
    //
    // AccessibleTextHelper implementation (simply forwards to impl)
    //
    //------------------------------------------------------------------------

    AccessibleTextHelper::AccessibleTextHelper( const uno::Reference< XAccessible >&    rInterface,
                                                      ::std::auto_ptr< SvxEditSource >      pEditSource ) :
        mpImpl( new AccessibleTextHelper_Impl( rInterface ) )
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        SetEditSource( pEditSource );
    }

    AccessibleTextHelper::~AccessibleTextHelper()
    {
    }

    SvxEditSource& AccessibleTextHelper::GetEditSource() const throw (uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        SvxEditSource& aEditSource = mpImpl->GetEditSource();

        mpImpl->CheckInvariants();

        return aEditSource;
#else
        return mpImpl->GetEditSource();
#endif
    }

    void AccessibleTextHelper::SetEditSource( ::std::auto_ptr< SvxEditSource > pEditSource ) throw (uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->SetEditSource( pEditSource );

        mpImpl->CheckInvariants();
#else
        mpImpl->SetEditSource( pEditSource );
#endif
    }

    void AccessibleTextHelper::SetFocus( sal_Bool bHaveFocus ) throw (::com::sun::star::uno::RuntimeException)
    {
        // TODO
    }

    sal_Bool AccessibleTextHelper::HaveFocus() throw (::com::sun::star::uno::RuntimeException)
    {
        // TODO
        return sal_False;
    }

    void AccessibleTextHelper::FireEvent( const sal_Int16 nEventId, const uno::Any& rNewValue, const uno::Any& rOldValue ) const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->FireEvent( nEventId, rNewValue, rOldValue );

        mpImpl->CheckInvariants();
#else
        mpImpl->StateChangeEvent( nEventId, rNewValue, rOldValue );
#endif
    }

    void AccessibleTextHelper::SetOffset( const Point& rPoint )
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->SetOffset( rPoint );

        mpImpl->CheckInvariants();
#else
        mpImpl->SetOffset( rPoint );
#endif
    }

    const Point& AccessibleTextHelper::GetOffset() const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        const Point& rRef = mpImpl->GetOffset();

        mpImpl->CheckInvariants();

        return rRef;
#else
        return mpImpl->GetOffset();
#endif
    }

    void AccessibleTextHelper::SetChildrenOffset( sal_Int32 nOffset )
    {
        // TODO
    }

    sal_Int32 AccessibleTextHelper::GetChildrenOffset() const
    {
        // TODO
        return 0;
    }

    void AccessibleTextHelper::UpdateChildren() throw (::com::sun::star::uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->UpdateVisibleChildren();

        mpImpl->CheckInvariants();
#else
        mpImpl->UpdateVisibleChildren();
#endif
    }

    sal_Bool AccessibleTextHelper::IsSelected() const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        sal_Bool aRet = mpImpl->IsSelected();

        mpImpl->CheckInvariants();

        return aRet;
#else
        return mpImpl->IsSelected();
#endif
    }

    // XAccessibleContext
    sal_Int32 AccessibleTextHelper::GetChildCount() throw (uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        sal_Int32 nRet = mpImpl->getAccessibleChildCount();

        mpImpl->CheckInvariants();

        return nRet;
#else
        return mpImpl->getAccessibleChildCount();
#endif
    }

    uno::Reference< XAccessible > AccessibleTextHelper::GetChild( sal_Int32 i ) throw (lang::IndexOutOfBoundsException, uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        uno::Reference< XAccessible > xRet = mpImpl->getAccessibleChild( i );

        mpImpl->CheckInvariants();

        return xRet;
#else
        return mpImpl->getAccessibleChild( i );
#endif
    }

    void AccessibleTextHelper::AddEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) throw (uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->addEventListener( xListener );

        mpImpl->CheckInvariants();
#else
        mpImpl->addEventListener( xListener );
#endif
    }

    void AccessibleTextHelper::RemoveEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) throw (uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->removeEventListener( xListener );

        mpImpl->CheckInvariants();
#else
        mpImpl->removeEventListener( xListener );
#endif
    }

    // XAccessibleComponent
    uno::Reference< XAccessible > AccessibleTextHelper::GetAt( const awt::Point& aPoint ) throw (uno::RuntimeException)
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        uno::Reference< XAccessible > xChild = mpImpl->getAccessibleAt( aPoint );

        mpImpl->CheckInvariants();

        return xChild;
#else
        return mpImpl->getAccessibleAt( aPoint );
#endif
    }

} // end of namespace accessibility

//------------------------------------------------------------------------
