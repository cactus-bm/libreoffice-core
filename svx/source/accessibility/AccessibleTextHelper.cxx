/*************************************************************************
 *
 *  $RCSfile: AccessibleTextHelper.cxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: thb $ $Date: 2002-08-02 11:32:41 $
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

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
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
        AccessibleTextHelper_Impl();
        ~AccessibleTextHelper_Impl();

        // XAccessibleContext child handling methods
        sal_Int32 SAL_CALL getAccessibleChildCount() SAL_THROW((uno::RuntimeException));
        uno::Reference< XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) SAL_THROW((lang::IndexOutOfBoundsException, uno::RuntimeException));

        // XAccessibleEventBroadcaster child related methods
        void SAL_CALL addEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) SAL_THROW((uno::RuntimeException));
        void SAL_CALL removeEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) SAL_THROW((uno::RuntimeException));

        // XAccessibleComponent child related methods
        uno::Reference< XAccessible > SAL_CALL getAccessibleAt( const awt::Point& aPoint ) SAL_THROW((uno::RuntimeException));

        SvxEditSourceAdapter& GetEditSource() const SAL_THROW((uno::RuntimeException));
        void SetEditSource( ::std::auto_ptr< SvxEditSource > pEditSource ) SAL_THROW((uno::RuntimeException));

        void SetEventSource( const uno::Reference< XAccessible >& rInterface ) { mxFrontEnd = rInterface; }
        uno::Reference< XAccessible > GetEventSource() const { return mxFrontEnd; }

        void SetOffset( const Point& );
        Point GetOffset() const { ::osl::MutexGuard aGuard( maMutex ); Point aPoint( maOffset ); return aPoint; }

        void SetStartIndex( sal_Int32 nOffset );
        sal_Int32 GetStartIndex() const
        {
            // Strictly correct only with locked solar mutex, // but
            // here we rely on the fact that sal_Int32 access is
            // atomic
            return mnStartIndex;
        }

        sal_Bool IsSelected() const;

        void Dispose();

        // do NOT hold object mutex when calling this! Danger of deadlock
        void FireEvent( const sal_Int16 nEventId, const uno::Any& rNewValue = uno::Any(), const uno::Any& rOldValue = uno::Any() ) const;
        void FireEvent( const AccessibleEventObject& rEvent ) const;

        void SetFocus( sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException));
        sal_Bool HaveFocus() SAL_THROW((::com::sun::star::uno::RuntimeException));
        void SetChildFocus( sal_Int32 nChild, sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException));
        void SetShapeFocus( sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException));
        void ChangeChildFocus( sal_Int32 nNewChild ) SAL_THROW((::com::sun::star::uno::RuntimeException));

#ifdef DBG_UTIL
        void CheckInvariants() const;
#endif

        // checks all children for visibility, throws away invisible ones
        void UpdateVisibleChildren();

        // check all children for changes in posit�on and size
        void UpdateBoundRect();

        // calls SetSelection on the forwarder and updates maLastSelection
        // cache.
        void UpdateSelection();

    private:

        // syntactic sugar for FireEvent
        void GotPropertyEvent( const uno::Any& rNewValue, const sal_Int16 nEventId ) const { FireEvent( nEventId, rNewValue ); }
        void LostPropertyEvent( const uno::Any& rOldValue, const sal_Int16 nEventId ) const { FireEvent( nEventId, uno::Any(), rOldValue ); }

        // shutdown usage of current edit source on myself and the children.
        void ShutdownEditSource() SAL_THROW((uno::RuntimeException));

        void ParagraphsMoved( sal_Int32 nFirst, sal_Int32 nMiddle, sal_Int32 nLast );
        void ParagraphsInserted( sal_Int32 nFirst );
        void ParagraphsRemoved( sal_Int32 nFirst );

        virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

        // lock solar mutex before
        SvxTextForwarder& GetTextForwarder() const SAL_THROW((uno::RuntimeException));
        // lock solar mutex before
        SvxViewForwarder& GetViewForwarder() const SAL_THROW((uno::RuntimeException));
        // lock solar mutex before
        SvxEditViewForwarder& GetEditViewForwarder( sal_Bool bCreate = sal_False ) const SAL_THROW((uno::RuntimeException));

        // are we in edit mode?
        sal_Bool IsActive() const SAL_THROW((uno::RuntimeException));

        // our frontend class (the one implementing the actual
        // interface). That's not necessarily the one containing the impl
        // pointer!
        uno::Reference< XAccessible > mxFrontEnd;

        // a wrapper for the text forwarders (guarded by solar mutex)
        mutable SvxEditSourceAdapter maEditSource;

        // store last selection (to correctly report selection changes, guarded by solar mutex)
        ESelection maLastSelection;

        // cache range of visible children (guarded by solar mutex)
        sal_Int32 mnFirstVisibleChild;
        sal_Int32 mnLastVisibleChild;

        // offset to add to all our children (unguarded, relying on
        // the fact that sal_Int32 access is atomic)
        sal_Int32 mnStartIndex;

        // the object handling our children (guarded by solar mutex)
        accessibility::AccessibleParaManager maParaManager;

        // spin lock to prevent notify in notify (guarded by solar mutex)
        sal_Bool mbInNotify;

        // whether the object or it's children has the focus set (guarded by solar mutex)
        sal_Bool mbGroupHasFocus;

        // whether we (this object) has the focus set (guarded by solar mutex)
        sal_Bool mbThisHasFocus;

        // whether the children are active and editable (guarded by solar mutex)
        sal_Bool mbChildrenActive;

        // the internal paragraph number of the child with the focus (guarded by solar mutex)
        sal_Int32 mnFocusedChild;

        // must be before maStateListeners, has to live longer
        mutable ::osl::Mutex maMutex;

        /// our current offset to the containing shape/cell (guarded by maMutex)
        Point maOffset;

        // handles our event listeners (guarded by maMutex)
        ::cppu::OInterfaceContainerHelper maStateListeners;

    };

    //------------------------------------------------------------------------
    //
    // AccessibleTextHelper_Impl implementation
    //
    //------------------------------------------------------------------------

    AccessibleTextHelper_Impl::AccessibleTextHelper_Impl() :
        mxFrontEnd( NULL ),
        maLastSelection( EE_PARA_NOT_FOUND,EE_PARA_NOT_FOUND,EE_PARA_NOT_FOUND,EE_PARA_NOT_FOUND ),
        mnFirstVisibleChild( -1 ),
        mnLastVisibleChild( -2 ),
        mnStartIndex( 0 ),
        mbInNotify( sal_False ),
        mbGroupHasFocus( sal_False ),
        mbThisHasFocus( sal_False ),
        mbChildrenActive( sal_False ),
        mnFocusedChild( -1 ),
        maOffset(0,0),
        maStateListeners( maMutex )
    {
    }

    AccessibleTextHelper_Impl::~AccessibleTextHelper_Impl()
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        try
        {
            Dispose();
        }
        catch( const uno::Exception& ) {}
    }

    SvxTextForwarder& AccessibleTextHelper_Impl::GetTextForwarder() const SAL_THROW((uno::RuntimeException))
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

    SvxViewForwarder& AccessibleTextHelper_Impl::GetViewForwarder() const SAL_THROW((uno::RuntimeException))
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

    SvxEditViewForwarder& AccessibleTextHelper_Impl::GetEditViewForwarder( sal_Bool bCreate ) const SAL_THROW((uno::RuntimeException))
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

    SvxEditSourceAdapter& AccessibleTextHelper_Impl::GetEditSource() const SAL_THROW((uno::RuntimeException))
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
        catch( const uno::Exception& ) {}

        return bRet;
    }

    // functor for sending child events (no stand-alone function, they are maybe not inlined)
    class AccessibleTextHelper_OffsetChildIndex : public ::std::unary_function< accessibility::AccessibleEditableTextPara&, void >
    {
    public:
        AccessibleTextHelper_OffsetChildIndex( sal_Int32 nDifference ) : mnDifference(nDifference) {}
        void operator()( accessibility::AccessibleEditableTextPara& rPara )
        {
            rPara.SetIndexInParent( rPara.GetIndexInParent() + mnDifference );
        }

    private:
        const sal_Int32 mnDifference;
    };

    void AccessibleTextHelper_Impl::SetStartIndex( sal_Int32 nOffset )
    {
        sal_Int32 nOldOffset( mnStartIndex );

        mnStartIndex = nOffset;

        if( nOldOffset != nOffset )
        {
            // update children
            AccessibleTextHelper_OffsetChildIndex aFunctor( nOffset - nOldOffset );

            ::std::for_each( maParaManager.begin(), maParaManager.end(),
                             AccessibleParaManager::WeakChildAdapter< AccessibleTextHelper_OffsetChildIndex > (aFunctor) );
        }
    }

    void AccessibleTextHelper_Impl::SetChildFocus( sal_Int32 nChild, sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
        if( bHaveFocus )
        {
            if( mbThisHasFocus )
                SetShapeFocus( sal_False );

            maParaManager.SetFocus( nChild );

            // we just received the focus, also send caret event then
            UpdateSelection();
        }
        else
        {
            maParaManager.SetFocus( -1 );

            if( mbGroupHasFocus )
                SetShapeFocus( sal_True );
        }
    }

    void AccessibleTextHelper_Impl::ChangeChildFocus( sal_Int32 nNewChild ) SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
        if( mbThisHasFocus )
            SetShapeFocus( sal_False );

        mbGroupHasFocus = sal_True;
        maParaManager.SetFocus( nNewChild );
    }

    void AccessibleTextHelper_Impl::SetShapeFocus( sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
        sal_Bool bOldFocus( mbThisHasFocus );

        mbThisHasFocus = bHaveFocus;

        if( bOldFocus != bHaveFocus )
        {
            if( bHaveFocus )
                GotPropertyEvent( uno::makeAny(AccessibleStateType::FOCUSED), AccessibleEventId::ACCESSIBLE_STATE_EVENT );
            else
                LostPropertyEvent( uno::makeAny(AccessibleStateType::FOCUSED), AccessibleEventId::ACCESSIBLE_STATE_EVENT );
        }
    }

    void AccessibleTextHelper_Impl::SetFocus( sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
        sal_Bool bOldFocus( mbGroupHasFocus );

        mbGroupHasFocus = bHaveFocus;

        if( IsActive() )
        {
            try
            {
                // find the one with the cursor and get/set focus accordingly
                ESelection aSelection;
                if( GetEditViewForwarder().GetSelection( aSelection ) )
                    SetChildFocus( aSelection.nEndPara, bHaveFocus );
            }
            catch( const uno::Exception& ) {}
        }
        else if( bOldFocus != bHaveFocus )
        {
            SetShapeFocus( bHaveFocus );
        }

        DBG_TRACE2("AccessibleTextHelper_Impl::SetFocus: focus changed, Object %d, state: %s", this, bHaveFocus ? "focused" : "not focused");
    }

    sal_Bool AccessibleTextHelper_Impl::HaveFocus() SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
        // No locking of solar mutex here, since we rely on the fact
        // that sal_Bool access is atomic
        return mbThisHasFocus;
    }

    sal_Bool AccessibleTextHelper_Impl::IsActive() const SAL_THROW((uno::RuntimeException))
    {
        try
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
        catch( const uno::RuntimeException& )
        {
            return sal_False;
        }
    }

    void AccessibleTextHelper_Impl::UpdateSelection()
    {
        try
        {
            ESelection aSelection;
            if( GetEditViewForwarder().GetSelection( aSelection ) )
            {
                if( !maLastSelection.IsEqual( aSelection ) )
                {
                    DBG_ASSERT( !mbThisHasFocus, "AccessibleTextHelper_Impl::UpdateSelection: editing, but focus on parent" );

                    // notify all affected paragraphs (TODO: may be suboptimal,
                    // since some paragraphs might stay selected)
                    if( maLastSelection.nStartPara != EE_PARA_NOT_FOUND )
                    {
                        // Did the caret move from one paragraph to another?
                        // #100530# no caret events if not focused.
                        if( mbGroupHasFocus &&
                            maLastSelection.nEndPara != aSelection.nEndPara )
                        {
                            maParaManager.FireEvent( maLastSelection.nEndPara,
                                                     maLastSelection.nEndPara+1,
                                                     AccessibleEventId::ACCESSIBLE_CARET_EVENT,
                                                     uno::makeAny(static_cast<sal_Int32>(-1)),
                                                     uno::makeAny(static_cast<sal_Int32>(maLastSelection.nEndPos)) );

                            ChangeChildFocus( aSelection.nEndPara );

                            DBG_TRACE3("AccessibleTextHelper_Impl::UpdateSelection(): focus changed, Object: %d, Paragraph: %d, Last paragraph: %d",
                                       this, aSelection.nEndPara, maLastSelection.nEndPara);
                        }
                    }

                    uno::Any aOldCursor;

                    if( maLastSelection.nStartPara != EE_PARA_NOT_FOUND )
                        aOldCursor <<= static_cast<sal_Int32>(maLastSelection.nEndPos);
                    else
                        aOldCursor <<= static_cast<sal_Int32>(-1);

                    // #100530# no caret events if not focused.
                    if( mbGroupHasFocus )
                    {
                        maParaManager.FireEvent( aSelection.nEndPara,
                                                 aSelection.nEndPara+1,
                                                 AccessibleEventId::ACCESSIBLE_CARET_EVENT,
                                                 uno::makeAny(static_cast<sal_Int32>(aSelection.nEndPos)),
                                                 aOldCursor );
                    }

                    maLastSelection = aSelection;

                    DBG_TRACE5("AccessibleTextHelper_Impl::UpdateSelection(): caret changed, Object: %d, New pos: %d, Old pos: %d, New para: %d, Old para: %d",
                               this, aSelection.nEndPos, maLastSelection.nEndPos, aSelection.nEndPara, maLastSelection.nEndPara);
                }
            }
        }
        // no selection? no update actions
        catch( const uno::RuntimeException& ) {}
    }

    void AccessibleTextHelper_Impl::ShutdownEditSource() SAL_THROW((uno::RuntimeException))
    {
        // This should only be called with solar mutex locked, i.e. from the main office thread

        // This here is somewhat clumsy: As soon as our children have
        // a NULL EditSource (maParaManager.SetEditSource()), they
        // enter the disposed state and cannot be reanimated. Thus, it
        // is unavoidable and a hard requirement to let go and create
        // from scratch each and every child.

        // invalidate children
        maParaManager.Dispose();
        maParaManager.SetNum(0);

        // lost all children
        if( mxFrontEnd.is() )
            FireEvent(AccessibleEventId::ACCESSIBLE_ALL_CHILDREN_CHANGED_EVENT);

        // quit listen on stale edit source
        if( maEditSource.IsValid() )
            EndListening( maEditSource.GetBroadcaster() );

        maEditSource.SetEditSource( ::std::auto_ptr< SvxEditSource >(NULL) );
    }

    void AccessibleTextHelper_Impl::SetEditSource( ::std::auto_ptr< SvxEditSource > pEditSource ) SAL_THROW((uno::RuntimeException))
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
        // guard against non-atomic access to maOffset data structure
        {
            ::osl::MutexGuard aGuard( maMutex );
            maOffset = rPoint;
        }

        maParaManager.SetEEOffset( rPoint );

        // in all cases, check visibility afterwards.
        UpdateVisibleChildren();
        UpdateBoundRect();
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

                    // child not yet created?
                    accessibility::AccessibleParaManager::WeakChild aChild( maParaManager.GetChild(nCurrPara) );
                    if( aChild.second.Width == 0 &&
                        aChild.second.Height == 0 &&
                        mxFrontEnd.is() )
                    {
                        GotPropertyEvent( uno::makeAny( maParaManager.CreateChild( nCurrPara - mnFirstVisibleChild,
                                                                                   mxFrontEnd, GetEditSource(), nCurrPara ).first ),
                                          AccessibleEventId::ACCESSIBLE_CHILD_EVENT );
                    }
                }
                else
                {
                    // not or no longer visible
                    if( maParaManager.IsReferencable( nCurrPara ) )
                    {
                        LostPropertyEvent( uno::makeAny( maParaManager.GetChild( nCurrPara ).first.get().getRef() ),
                                           AccessibleEventId::ACCESSIBLE_CHILD_EVENT );

                        // clear reference
                        maParaManager.Release( nCurrPara );
                    }
                }
            }
        }
        catch( const uno::Exception& )
        {
            DBG_ERROR("AccessibleTextHelper_Impl::UpdateVisibleChildren error while determining visible children");

            // something failed - currently no children
            mnFirstVisibleChild = -1;
            mnLastVisibleChild = -2;
            maParaManager.SetNum(0);

            // lost all children
            FireEvent(AccessibleEventId::ACCESSIBLE_ALL_CHILDREN_CHANGED_EVENT);
        }
    }

    // functor for checking changes in paragraph bounding boxes (no stand-alone function, maybe not inlined)
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
                    // update bounds
                    return accessibility::AccessibleParaManager::WeakChild( rChild.first, aNewRect );
                }

                // visible data changed
                aHardRef->FireEvent( AccessibleEventId::ACCESSIBLE_BOUNDRECT_EVENT );
            }

            // identity transform
            return rChild;
        }

    private:
        AccessibleTextHelper_Impl&  mrImpl;
    };

    void AccessibleTextHelper_Impl::UpdateBoundRect()
    {
        // send ACCESSIBLE_BOUNDRECT_EVENT to affected children
        AccessibleTextHelper_UpdateChildBounds aFunctor( *this );
        ::std::transform( maParaManager.begin(), maParaManager.end(), maParaManager.begin(), aFunctor );
    }

#ifdef DBG_UTIL
    void AccessibleTextHelper_Impl::CheckInvariants() const
    {
        if( mnFirstVisibleChild >= 0 &&
            mnFirstVisibleChild > mnLastVisibleChild )
        {
            DBG_ERROR( "AccessibleTextHelper: range invalid" );
        }
    }
#endif

    // functor for sending child events (no stand-alone function, they are maybe not inlined)
    class AccessibleTextHelper_LostChildEvent : public ::std::unary_function< const accessibility::AccessibleParaManager::WeakChild&, void >
    {
    public:
        AccessibleTextHelper_LostChildEvent( AccessibleTextHelper_Impl& rImpl ) : mrImpl(rImpl) {}
        void operator()( const accessibility::AccessibleParaManager::WeakChild& rPara )
        {
            // retrieve hard reference from weak one
            accessibility::AccessibleParaManager::WeakPara::HardRefType aHardRef( rPara.first.get() );

            if( aHardRef.is() )
                mrImpl.FireEvent(AccessibleEventId::ACCESSIBLE_CHILD_EVENT, uno::Any(), uno::makeAny( aHardRef.getRef() ) );
        }

    private:
        AccessibleTextHelper_Impl&  mrImpl;
    };

    void AccessibleTextHelper_Impl::ParagraphsMoved( sal_Int32 nFirst, sal_Int32 nMiddle, sal_Int32 nLast )
    {
        const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();

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
        if( nMiddle < nFirst )
        {
            ::std::swap(nFirst, nMiddle);
        }
        else if( nMiddle < nLast )
        {
            nLast = nLast + nMiddle - nFirst;
        }
        else
        {
            ::std::swap(nMiddle, nLast);
            nLast = nLast + nMiddle - nFirst;
        }

        if( nFirst < nParas && nMiddle < nParas && nLast < nParas )
        {
            // since we have no "paragraph index
            // changed" event on UAA, remove
            // [first,last] and insert again later (in
            // UpdateVisibleChildren)

            // maParaManager.Rotate( nFirst, nMiddle, nLast );

            // send CHILD_EVENT to affected children
            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

            ::std::advance( begin, nFirst );
            ::std::advance( end, nLast+1 );

            // TODO: maybe optimize here in the following way.  If the
            // number of removed children exceeds a certain threshold,
            // use ACCESSIBLE_ALL_CHILDREN_CHANGED_EVENT
            AccessibleTextHelper_LostChildEvent aFunctor( *this );

            ::std::for_each( begin, end, aFunctor );

            maParaManager.Release(nFirst, nLast+1);
            // should be no need for UpdateBoundRect, since all affected children are cleared.
        }
    }

    void AccessibleTextHelper_Impl::ParagraphsInserted( sal_Int32 nFirst )
    {
        const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();
        const sal_Int32 nOldNumParas( maParaManager.GetNum() );
        const sal_Int32 nLast( nParas - 1 );

        // resize child vector to the current child count
        maParaManager.SetNum( nParas );

        if( nFirst < nOldNumParas && nFirst < nParas && nLast < nParas )
        {
            // since we have no "paragraph index
            // changed" event on UAA, remove
            // [first,last] and insert again later (in
            // UpdateVisibleChildren)

            // move successors of inserted paragraph one position further
            //maParaManager.MoveRightFrom( pTextHint->GetValue() );

            // release everything from the insertion position until the end
            maParaManager.Release(nFirst, nLast+1);

            // send CHILD_EVENT to affected children
            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
            ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

            ::std::advance( begin, nFirst );
            ::std::advance( end, nLast+1 );

            // TODO: maybe optimize here in the following way.  If the
            // number of removed children exceeds a certain threshold,
            // use ACCESSIBLE_ALL_CHILDREN_CHANGED_EVENT
            AccessibleTextHelper_LostChildEvent aFunctor( *this );
            ::std::for_each( begin, end, aFunctor );
        }
    }

    void AccessibleTextHelper_Impl::ParagraphsRemoved( sal_Int32 nFirst )
    {
        const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();
        const sal_Int32 nLast( maParaManager.GetNum() - 1 );

        // since we have no "paragraph index
        // changed" event on UAA, remove
        // [first,last] and insert again later (in
        // UpdateVisibleChildren)

        // move successors of removed paragraph one position closer
        // maParaManager.MoveLeftFrom( pTextHint->GetValue() );

        // release everything from the remove position until the end
        maParaManager.Release(nFirst, nLast+1);

        // send CHILD_EVENT to affected children
        ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator begin = maParaManager.begin();
        ::accessibility::AccessibleParaManager::VectorOfChildren::const_iterator end = begin;

        ::std::advance( begin, nFirst );
        ::std::advance( end, nLast+1 );

        // TODO: maybe optimize here in the following way.  If the
        // number of removed children exceeds a certain threshold,
        // use ACCESSIBLE_ALL_CHILDREN_CHANGED_EVENT
        AccessibleTextHelper_LostChildEvent aFunctor( *this );
        ::std::for_each( begin, end, aFunctor );

        // resize child vector to the current child count
        maParaManager.SetNum( nParas );
    }

    void AccessibleTextHelper_Impl::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
    {
        // precondition: solar mutex locked
        DBG_TESTSOLARMUTEX();

        // precondition: not in a recursion
        if( mbInNotify )
            return;

        mbInNotify = sal_True;

        // determine hint type
        const SdrHint* pSdrHint = PTR_CAST( SdrHint, &rHint );
        const SfxSimpleHint* pSimpleHint = PTR_CAST( SfxSimpleHint, &rHint );
        const TextHint* pTextHint = PTR_CAST( TextHint, &rHint );
        const SvxViewHint* pViewHint = PTR_CAST( SvxViewHint, &rHint );
        const SvxEditSourceHint* pEditSourceHint = PTR_CAST( SvxEditSourceHint, &rHint );

        try
        {
            const sal_Int32 nParas = GetTextForwarder().GetParagraphCount();

            // precondition: edit engine and para manager consistent

            /* The problem here is the fact that the EditEngine events
             * do not always arrive in a logical order. For example,
             * when inserting a paragraph by pressing return at the
             * end of a line, currently the TEXT_MODIFIED hint arrives
             * before the PARAGRAPH_INSERTED hint. Therefore, we have
             * to update our paragraph count proactively here.
             */
            sal_Int32 nFirstParaInsert( 0 ); // default range is pessimization, don't know where to add
            if( (!pEditSourceHint && pTextHint && pTextHint->GetId() == TEXT_HINT_PARAINSERTED) )
                nFirstParaInsert = pTextHint->GetValue() == EE_PARA_ALL ? 0 : pTextHint->GetValue();

            if( nFirstParaInsert != 0 ||
                maParaManager.GetNum() < static_cast<sal_uInt32>(nParas) )
            {
                ParagraphsInserted( nFirstParaInsert );
                UpdateVisibleChildren();
                UpdateBoundRect();
            }

            sal_Int32 nFirstParaRemove( 0 ); // default range is pessimization, don't know where to add
            if( (!pEditSourceHint && pTextHint && pTextHint->GetId() == TEXT_HINT_PARAREMOVED) )
                nFirstParaRemove = pTextHint->GetValue() == EE_PARA_ALL ? 0 : pTextHint->GetValue();

            if( nFirstParaRemove != 0 ||
                maParaManager.GetNum() > static_cast<sal_uInt32>(nParas) )
            {
                // remove excess paragraphs (range is pessimization, don't know where to remove)
                ParagraphsRemoved( nFirstParaRemove );
                UpdateVisibleChildren();
                UpdateBoundRect();
            }

            if( pEditSourceHint )
            {
                switch( pEditSourceHint->GetId() )
                {
                    case EDITSOURCE_HINT_PARASMOVED:
                    {
                        DBG_ASSERT( pEditSourceHint->GetStartValue() < GetTextForwarder().GetParagraphCount() &&
                                    pEditSourceHint->GetEndValue() < GetTextForwarder().GetParagraphCount(),
                                    "AccessibleTextHelper_Impl::NotifyHdl: Invalid notification");

                        ParagraphsMoved(pEditSourceHint->GetStartValue(),
                                        pEditSourceHint->GetValue(),
                                        pEditSourceHint->GetEndValue());

                        // in all cases, check visibility afterwards.
                        UpdateVisibleChildren();

                        break;
                    }

                    case EDITSOURCE_HINT_SELECTIONCHANGED:
                        // notify listeners
                        try
                        {
                            UpdateSelection();
                        }
                        // maybe we're not in edit mode (this is not an error)
                        catch( const uno::Exception& ) {}
                        break;
                }
            }
            else if( pTextHint )
            {
                switch( pTextHint->GetId() )
                {
                    case TEXT_HINT_MODIFIED:
                    {
                        // notify listeners
                        sal_Int32 nPara( pTextHint->GetValue() );
                        if( nPara == static_cast<sal_Int32>(EE_PARA_ALL) )
                            maParaManager.FireEvent( 0, GetTextForwarder().GetParagraphCount(), AccessibleEventId::ACCESSIBLE_TEXT_EVENT );
                        else
                            if( nPara < nParas )
                                maParaManager.FireEvent( nPara, AccessibleEventId::ACCESSIBLE_TEXT_EVENT );
                        break;
                    }

                    case TEXT_HINT_PARAINSERTED:
                        // already happened above
                        break;

                    case TEXT_HINT_PARAREMOVED:
                        // already happened above
                        break;

                    case TEXT_HINT_TEXTHEIGHTCHANGED:
                        break;

                    case TEXT_HINT_VIEWSCROLLED:
                        break;
                }

                // in all cases, check visibility afterwards.
                UpdateVisibleChildren();
                UpdateBoundRect();
            }
            else if( pViewHint )
            {
                switch( pViewHint->GetId() )
                {
                    case SVX_HINT_VIEWCHANGED:
                        // just check visibility
                        UpdateVisibleChildren();
                        UpdateBoundRect();
                        break;
                }
            }
            else if( pSdrHint )
            {
                switch( pSdrHint->GetKind() )
                {
                    case HINT_BEGEDIT:
                    {
                        // change children state
                        maParaManager.SetActive();

                        // per definition, edit mode text has the focus
                        SetFocus( sal_True );
                        break;
                    }

                    case HINT_ENDEDIT:
                        // focused child now looses focus
                        ESelection aSelection;
                        if( GetEditViewForwarder().GetSelection( aSelection ) )
                            SetChildFocus( aSelection.nEndPara, sal_False );

                        // change children state
                        maParaManager.SetActive( sal_False );

                        maLastSelection = ESelection( EE_PARA_NOT_FOUND, EE_PARA_NOT_FOUND,
                                                      EE_PARA_NOT_FOUND, EE_PARA_NOT_FOUND);
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
                        catch( const uno::Exception& ) {}

                        break;
                }
            }
        }
        catch( const uno::Exception& )
        {
#ifdef DBG_UTIL
            OSL_TRACE("AccessibleTextHelper_Impl::Notify: Unhandled exception.");
#endif
            mbInNotify = sal_False;
        }

        mbInNotify = sal_False;
    }

    void AccessibleTextHelper_Impl::Dispose()
    {
        // dispose children
        maParaManager.Dispose();

        // quit listen on stale edit source
        if( maEditSource.IsValid() )
            EndListening( maEditSource.GetBroadcaster() );

        // clear references
        maEditSource.SetEditSource( ::std::auto_ptr< SvxEditSource >(NULL) );
        mxFrontEnd = NULL;
    }

    void AccessibleTextHelper_Impl::FireEvent( const sal_Int16 nEventId, const uno::Any& rNewValue, const uno::Any& rOldValue ) const
    {
        // -- object locked --
        ::osl::ClearableMutexGuard aGuard( maMutex );

        AccessibleEventObject aEvent;

        DBG_ASSERT(mxFrontEnd.is(), "AccessibleTextHelper::FireEvent: no event source set" );

        if( mxFrontEnd.is() )
            aEvent = AccessibleEventObject(mxFrontEnd->getAccessibleContext(), nEventId, rNewValue, rOldValue);
        else
            aEvent = AccessibleEventObject(uno::Reference< uno::XInterface >(), nEventId, rNewValue, rOldValue);

        // no locking necessary, OInterfaceIteratorHelper copies listeners if someone removes/adds in between
        // Further locking, actually, might lead to deadlocks, since we're calling out of this object
        aGuard.clear();
        // -- until here --

        FireEvent(aEvent);
    }

    void AccessibleTextHelper_Impl::FireEvent( const AccessibleEventObject& rEvent ) const
    {
        // -- object locked --
        ::osl::ClearableMutexGuard aGuard( maMutex );

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
                    xListener->notifyEvent (rEvent);
                }
                catch( const lang::DisposedException& e )
                {
                    // DisposedExceptions from the listener might indicate a
                    // broken connection to a different environment.

                    OSL_ENSURE(e.Context.is(), "AccessibleTextHelper::FireEvent: caught dispose exception with empty Context field");
                    // If the exception stems from the listener then remove it
                    // from the list of listeners.  If the Context field of the
                    // exception is empty this is interpreted to indicate the
                    // listener as well.
                    if (e.Context == xListener
                        || !e.Context.is())
                        aIter.remove();
                }
                catch( const uno::Exception& e )
                {
                    DBG_WARNING1("AccessibleTextHelper::FireEvent: exception %s from listener",
                                 ::rtl::OUStringToOString( e.Message, RTL_TEXTENCODING_DONTKNOW ).getStr() );
                }
            }
        }
    }

    // XAccessibleContext
    sal_Int32 SAL_CALL AccessibleTextHelper_Impl::getAccessibleChildCount() SAL_THROW((uno::RuntimeException))
    {
        return mnLastVisibleChild - mnFirstVisibleChild + 1;
    }

    uno::Reference< XAccessible > SAL_CALL AccessibleTextHelper_Impl::getAccessibleChild( sal_Int32 i ) SAL_THROW((lang::IndexOutOfBoundsException, uno::RuntimeException))
    {
        i -= GetStartIndex();

        if( 0 > i || i >= getAccessibleChildCount() ||
            GetTextForwarder().GetParagraphCount() <= i )
        {
            throw lang::IndexOutOfBoundsException(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Invalid child index")), mxFrontEnd);
        }

        DBG_ASSERT(mxFrontEnd.is(), "AccessibleTextHelper_Impl::UpdateVisibleChildren: no frontend set");

        if( mxFrontEnd.is() )
            return maParaManager.CreateChild( i, mxFrontEnd, GetEditSource(), mnFirstVisibleChild + i ).first;
        else
            return NULL;
    }

    void SAL_CALL AccessibleTextHelper_Impl::addEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) SAL_THROW((uno::RuntimeException))
    {
        maStateListeners.addInterface( xListener );
    }

    void SAL_CALL AccessibleTextHelper_Impl::removeEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) SAL_THROW((uno::RuntimeException))
    {
        maStateListeners.removeInterface( xListener );
    }

    uno::Reference< XAccessible > SAL_CALL AccessibleTextHelper_Impl::getAccessibleAt( const awt::Point& _aPoint ) SAL_THROW((uno::RuntimeException))
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
        return NULL;
    }

    //------------------------------------------------------------------------
    //
    // AccessibleTextHelper implementation (simply forwards to impl)
    //
    //------------------------------------------------------------------------

    AccessibleTextHelper::AccessibleTextHelper( ::std::auto_ptr< SvxEditSource > pEditSource ) :
        mpImpl( new AccessibleTextHelper_Impl() )
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

        SetEditSource( pEditSource );
    }

    AccessibleTextHelper::~AccessibleTextHelper()
    {
    }

    const SvxEditSource& AccessibleTextHelper::GetEditSource() const SAL_THROW((uno::RuntimeException))
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        const SvxEditSource& aEditSource = mpImpl->GetEditSource();

        mpImpl->CheckInvariants();

        return aEditSource;
#else
        return mpImpl->GetEditSource();
#endif
    }

    void AccessibleTextHelper::SetEditSource( ::std::auto_ptr< SvxEditSource > pEditSource ) SAL_THROW((uno::RuntimeException))
    {
#ifdef DBG_UTIL
        // precondition: solar mutex locked
        DBG_TESTSOLARMUTEX();

        mpImpl->CheckInvariants();
#endif

        mpImpl->SetEditSource( pEditSource );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    void AccessibleTextHelper::SetEventSource( const uno::Reference< XAccessible >& rInterface )
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif

        mpImpl->SetEventSource( rInterface );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    uno::Reference< XAccessible > AccessibleTextHelper::GetEventSource() const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        uno::Reference< XAccessible > xRet( mpImpl->GetEventSource() );

        mpImpl->CheckInvariants();

        return xRet;
#else
        return mpImpl->GetEventSource();
#endif
    }

    void AccessibleTextHelper::SetFocus( sal_Bool bHaveFocus ) SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
#ifdef DBG_UTIL
        // precondition: solar mutex locked
        DBG_TESTSOLARMUTEX();

        mpImpl->CheckInvariants();
#endif

        mpImpl->SetFocus( bHaveFocus );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    sal_Bool AccessibleTextHelper::HaveFocus() SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        sal_Bool bRet( mpImpl->HaveFocus() );

        mpImpl->CheckInvariants();

        return bRet;
#else
        return mpImpl->HaveFocus();
#endif
    }

    void AccessibleTextHelper::FireEvent( const sal_Int16 nEventId, const uno::Any& rNewValue, const uno::Any& rOldValue ) const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif

        mpImpl->FireEvent( nEventId, rNewValue, rOldValue );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    void AccessibleTextHelper::FireEvent( const AccessibleEventObject& rEvent ) const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif

        mpImpl->FireEvent( rEvent );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    void AccessibleTextHelper::SetOffset( const Point& rPoint )
    {
#ifdef DBG_UTIL
        // precondition: solar mutex locked
        DBG_TESTSOLARMUTEX();

        mpImpl->CheckInvariants();
#endif

        mpImpl->SetOffset( rPoint );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    Point AccessibleTextHelper::GetOffset() const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        Point aPoint( mpImpl->GetOffset() );

        mpImpl->CheckInvariants();

        return aPoint;
#else
        return mpImpl->GetOffset();
#endif
    }

    void AccessibleTextHelper::SetStartIndex( sal_Int32 nOffset )
    {
#ifdef DBG_UTIL
        // precondition: solar mutex locked
        DBG_TESTSOLARMUTEX();

        mpImpl->CheckInvariants();
#endif

        mpImpl->SetStartIndex( nOffset );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    sal_Int32 AccessibleTextHelper::GetStartIndex() const
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        sal_Int32 nOffset = mpImpl->GetStartIndex();

        mpImpl->CheckInvariants();

        return nOffset;
#else
        return mpImpl->GetStartIndex();
#endif
    }

    void AccessibleTextHelper::UpdateChildren() SAL_THROW((::com::sun::star::uno::RuntimeException))
    {
#ifdef DBG_UTIL
        // precondition: solar mutex locked
        DBG_TESTSOLARMUTEX();

        mpImpl->CheckInvariants();
#endif

        mpImpl->UpdateVisibleChildren();
        mpImpl->UpdateBoundRect();

        mpImpl->UpdateSelection();

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    void AccessibleTextHelper::Dispose()
    {
        // As Dispose calls ShutdownEditSource, which in turn
        // deregisters as listener on the edit source, have to lock
        // here
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif

        mpImpl->Dispose();

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();
#endif
    }

    sal_Bool AccessibleTextHelper::IsSelected() const
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

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
    sal_Int32 AccessibleTextHelper::GetChildCount() SAL_THROW((uno::RuntimeException))
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        sal_Int32 nRet = mpImpl->getAccessibleChildCount();

        mpImpl->CheckInvariants();

        return nRet;
#else
        return mpImpl->getAccessibleChildCount();
#endif
    }

    uno::Reference< XAccessible > AccessibleTextHelper::GetChild( sal_Int32 i ) SAL_THROW((lang::IndexOutOfBoundsException, uno::RuntimeException))
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        uno::Reference< XAccessible > xRet = mpImpl->getAccessibleChild( i );

        mpImpl->CheckInvariants();

        return xRet;
#else
        return mpImpl->getAccessibleChild( i );
#endif
    }

    void AccessibleTextHelper::AddEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) SAL_THROW((uno::RuntimeException))
    {
#ifdef DBG_UTIL
        mpImpl->CheckInvariants();

        mpImpl->addEventListener( xListener );

        mpImpl->CheckInvariants();
#else
        mpImpl->addEventListener( xListener );
#endif
    }

    void AccessibleTextHelper::RemoveEventListener( const uno::Reference< XAccessibleEventListener >& xListener ) SAL_THROW((uno::RuntimeException))
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
    uno::Reference< XAccessible > AccessibleTextHelper::GetAt( const awt::Point& aPoint ) SAL_THROW((uno::RuntimeException))
    {
        ::vos::OGuard aGuard( Application::GetSolarMutex() );

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
