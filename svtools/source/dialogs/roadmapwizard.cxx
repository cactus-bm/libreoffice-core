/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: roadmapwizard.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 15:14:46 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#ifndef SVTOOLS_INC_ROADMAPWIZARD_HXX
#include "roadmapwizard.hxx"
#endif

#ifndef _SVTOOLS_ROADMAP_HXX
#include "roadmap.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include <stdarg.h>

#include <vector>
#include <map>
#include <set>

//........................................................................
namespace svt
{
//........................................................................

    namespace
    {
        typedef ::std::vector< WizardTypes::WizardState >       Path;
        typedef ::std::map< RoadmapWizardTypes::PathId, Path >  Paths;
        typedef ::std::set< WizardTypes::WizardState >          StateSet;
    }

    struct RoadmapWizardImpl : public RoadmapWizardTypes
    {
        ORoadmap*   pRoadmap;

        Paths       aPaths;
        PathId      nActivePath;
        bool        bActivePathIsDefinite;
           FixedLine*   pFixedLine;


        StateSet    aDisabledStates;

        RoadmapWizardImpl()
            :pRoadmap( NULL )
            ,nActivePath( -1 )
            ,bActivePathIsDefinite( false )
            ,pFixedLine(NULL)
        {
        }

        ~RoadmapWizardImpl()
        {
            delete pRoadmap;
            delete pFixedLine;
        }

        /// returns the index of the current state in given path, or -1
        sal_Int32 getStateIndexInPath( WizardTypes::WizardState _nState, const Path& _rPath );
        /// returns the index of the current state in the path with the given id, or -1
        sal_Int32 getStateIndexInPath( WizardTypes::WizardState _nState, PathId _nPathId );
        /// returns the index of the first state in which the two given paths differ
        sal_Int32 getFirstDifferentIndex( const Path& _rLHS, const Path& _rRHS );
    };

    //--------------------------------------------------------------------
    sal_Int32 RoadmapWizardImpl::getStateIndexInPath( WizardTypes::WizardState _nState, const Path& _rPath )
    {
        sal_Int32 nStateIndexInPath = 0;
        Path::const_iterator aPathLoop = _rPath.begin();
        for ( ; aPathLoop != _rPath.end(); ++aPathLoop, ++nStateIndexInPath )
            if ( *aPathLoop == _nState )
                break;
        if ( aPathLoop == _rPath.end() )
            nStateIndexInPath = -1;
        return nStateIndexInPath;
    }

    //--------------------------------------------------------------------
    sal_Int32 RoadmapWizardImpl::getStateIndexInPath( WizardTypes::WizardState _nState, PathId _nPathId )
    {
        sal_Int32 nStateIndexInPath = -1;
        Paths::const_iterator aPathPos = aPaths.find( _nPathId );
        if ( aPathPos != aPaths.end( ) )
            nStateIndexInPath = getStateIndexInPath( _nState, aPathPos->second );
        return nStateIndexInPath;
    }

    //--------------------------------------------------------------------
    sal_Int32 RoadmapWizardImpl::getFirstDifferentIndex( const Path& _rLHS, const Path& _rRHS )
    {
        sal_Int32 nMinLength = ::std::min( _rLHS.size(), _rRHS.size() );
        for ( sal_Int32 nCheck = 0; nCheck < nMinLength; ++nCheck )
        {
            if ( _rLHS[ nCheck ] != _rRHS[ nCheck ] )
                return nCheck;
        }
        return nMinLength;
    }

    //====================================================================
    //= RoadmapWizard
    //====================================================================
    DBG_NAME( RoadmapWizard )
    //--------------------------------------------------------------------
#if OSL_DEBUG_LEVEL > 0
    const char* CheckInvariants( const void* pVoid )
    {
        return static_cast< const RoadmapWizard* >( pVoid )->checkInvariants();
    }

    //--------------------------------------------------------------------
    const sal_Char* RoadmapWizard::checkInvariants() const
    {
        // all paths have to start with the same state
        WizardState nSharedFirstState = WZS_INVALID_STATE;
        for ( Paths::const_iterator aPath = m_pImpl->aPaths.begin();
              aPath != m_pImpl->aPaths.end();
              ++aPath
            )
        {
            if ( aPath->second.empty() )
                return "RoadmapWizard::checkInvariants: paths should not be empty!";

            if ( nSharedFirstState == WZS_INVALID_STATE )
                // first path
                nSharedFirstState = aPath->second[ 0 ];
            else
                if ( nSharedFirstState != aPath->second[ 0 ] )
                    return "RoadmapWizard::checkInvariants: alls paths must start with the same state!";
        }

        if ( !m_pImpl->aPaths.empty() )
        {
            Paths::const_iterator aCurrentPathPos = m_pImpl->aPaths.find( m_pImpl->nActivePath );
            if ( aCurrentPathPos == m_pImpl->aPaths.end() )
                return "RoadmapWizard::checkInvariants: invalid active path!";

            if ( -1 == m_pImpl->getStateIndexInPath( getCurrentState(), m_pImpl->nActivePath ) )
                return "RoadmapWizard::checkInvariants: the current state is not part of the current path!";
        }

        return NULL;
    }
#endif

    //--------------------------------------------------------------------
    RoadmapWizard::RoadmapWizard( Window* _pParent, const ResId& _rRes, sal_uInt32 _nButtonFlags, const ResId& _rRoadmapTitleResource, sal_Bool _bCheckButtonStates )
        :OWizardMachine( _pParent, _rRes, _nButtonFlags, _bCheckButtonStates, sal_True, 1 )
        ,m_pImpl( new RoadmapWizardImpl )
    {
        DBG_CTOR( RoadmapWizard, CheckInvariants );

        m_pImpl->pRoadmap = new ORoadmap( this, WB_TABSTOP );
        m_pImpl->pRoadmap->SetText( String( _rRoadmapTitleResource ) );
        m_pImpl->pRoadmap->SetPosPixel( Point( 0, 0 ) );
        m_pImpl->pRoadmap->SetItemSelectHdl( LINK( this, RoadmapWizard, OnRoadmapItemSelected ) );

        Size aRoadmapSize =( LogicToPixel( Size( 85, 0 ), MAP_APPFONT ) );
        aRoadmapSize.Height() = GetSizePixel().Height();
        m_pImpl->pRoadmap->SetSizePixel( aRoadmapSize );

        m_pImpl->pFixedLine = new FixedLine( this, WB_VERT );
        m_pImpl->pFixedLine->Show();
        m_pImpl->pFixedLine->SetPosPixel( Point( aRoadmapSize.Width() + 1, 0 ) );
        m_pImpl->pFixedLine->SetSizePixel( Size( LogicToPixel( Size( 2, 0 ) ).Width(), aRoadmapSize.Height() ) );

        SetViewWindow( m_pImpl->pRoadmap );
        SetViewAlign( WINDOWALIGN_LEFT );
        m_pImpl->pRoadmap->Show();
    }

    //--------------------------------------------------------------------
    RoadmapWizard::~RoadmapWizard()
    {
        delete m_pImpl;
        DBG_DTOR( RoadmapWizard, CheckInvariants );
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::SetRoadmapBitmap( const BitmapEx& _rBitmap )
    {
        m_pImpl->pRoadmap->SetRoadmapBitmap( _rBitmap );
    }

    //--------------------------------------------------------------------
    const BitmapEx& RoadmapWizard::GetRoadmapBitmap( ) const
    {
        return m_pImpl->pRoadmap->GetRoadmapBitmap();
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::SetRoadmapInteractive( sal_Bool _bInteractive )
    {
        m_pImpl->pRoadmap->SetRoadmapInteractive( _bInteractive );
    }

    //--------------------------------------------------------------------
    sal_Bool RoadmapWizard::IsRoadmapInteractive()
    {
        return m_pImpl->pRoadmap->IsRoadmapInteractive();
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::declarePath( PathId _nPathId, WizardState _nFirstState, ... )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        DBG_ASSERT( _nFirstState != WZS_INVALID_STATE, "RoadmapWizard::declarePath: there should be at least one state in the path!" );
        if ( _nFirstState == WZS_INVALID_STATE )
            return;

        Path aNewPath;

        // collect the elements of the path
        va_list aStateList;
        va_start( aStateList, _nFirstState );

        WizardState nState = _nFirstState;
        while ( nState != WZS_INVALID_STATE )
        {
            aNewPath.push_back( nState );
            nState = sal::static_int_cast< WizardState >(
                va_arg( aStateList, int ));
        }
        va_end( aStateList );

        DBG_ASSERT( _nFirstState == 0, "RoadmapWizard::declarePath: first state must be NULL." );
            // The WizardDialog (our very base class) always starts with a mnCurLevel == 0

        m_pImpl->aPaths.insert( Paths::value_type( _nPathId, aNewPath ) );

        if ( m_pImpl->aPaths.size() == 1 )
            // the very first path -> activate it
            activatePath( _nPathId, false );
        else
            implUpdateRoadmap( );
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::activatePath( PathId _nPathId, bool _bDecideForIt )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        if ( ( _nPathId == m_pImpl->nActivePath ) && ( _bDecideForIt == m_pImpl->bActivePathIsDefinite ) )
            // nothing to do
            return;

        // does the given path exist?
        Paths::const_iterator aNewPathPos = m_pImpl->aPaths.find( _nPathId );
        DBG_ASSERT( aNewPathPos != m_pImpl->aPaths.end(), "RoadmapWizard::activate: there is no such path!" );
        if ( aNewPathPos == m_pImpl->aPaths.end() )
            return;

        // determine the index of the current state in the current path
        sal_Int32 nCurrentStatePathIndex = -1;
        if ( m_pImpl->nActivePath != -1 )
            nCurrentStatePathIndex = m_pImpl->getStateIndexInPath( getCurrentState(), m_pImpl->nActivePath );

        DBG_ASSERT( (sal_Int32)aNewPathPos->second.size() > nCurrentStatePathIndex,
            "RoadmapWizard::activate: you cannot activate a path which has less states than we've already advanced!" );
            // If this asserts, this for instance means that we are already in state number, say, 5
            // of our current path, and the caller tries to activate a path which has less than 5
            // states
        if ( (sal_Int32)aNewPathPos->second.size() <= nCurrentStatePathIndex )
            return;

#if OSL_DEBUG_LEVEL > 0
        // assert that the current and the new path are equal, up to nCurrentStatePathIndex
        Paths::const_iterator aActivePathPos = m_pImpl->aPaths.find( m_pImpl->nActivePath );
        if ( aActivePathPos != m_pImpl->aPaths.end() )
            DBG_ASSERT( m_pImpl->getFirstDifferentIndex( aActivePathPos->second, aNewPathPos->second ) > nCurrentStatePathIndex,
                "RoadmapWizard::activate: you cannot activate a path which conflicts with the current one *before* the current state!" );
#endif

        m_pImpl->nActivePath = _nPathId;
        m_pImpl->bActivePathIsDefinite = _bDecideForIt;

        implUpdateRoadmap( );
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::implInsertState( WizardState _nState, sal_Int32 _nIndex )
    {
        // is this state enabled?
        sal_Bool bEnabled = m_pImpl->aDisabledStates.find( _nState ) == m_pImpl->aDisabledStates.end();

        m_pImpl->pRoadmap->InsertRoadmapItem(
            _nIndex,
            getStateDisplayName( _nState ),
            _nState,
            bEnabled
        );
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::implUpdateRoadmap( )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        Paths::const_iterator aActivePathPos = m_pImpl->aPaths.find( m_pImpl->nActivePath );
        DBG_ASSERT( aActivePathPos != m_pImpl->aPaths.end(), "RoadmapWizard::implUpdateRoadmap: there is no such path!" );

        sal_Int32 nCurrentStatePathIndex = m_pImpl->getStateIndexInPath( getCurrentState(), aActivePathPos->second );

        // determine up to which index (in the new path) we have to display the items
        RoadmapTypes::ItemIndex nUpperStepBoundary = (RoadmapTypes::ItemIndex)aActivePathPos->second.size();
        sal_Bool bIncompletePath = sal_False;
        if ( !m_pImpl->bActivePathIsDefinite )
        {
            for ( Paths::const_iterator aPathPos = m_pImpl->aPaths.begin();
                  aPathPos != m_pImpl->aPaths.end();
                  ++aPathPos
                )
            {
                if ( aPathPos->first == m_pImpl->nActivePath )
                    // it's the path we are just activating -> no need to check anything
                    continue;
                // the index from which on both paths differ
                sal_Int32 nDivergenceIndex = m_pImpl->getFirstDifferentIndex( aActivePathPos->second, aPathPos->second );
                if ( nDivergenceIndex <= nCurrentStatePathIndex )
                    // they differ in an index which we have already left behind us
                    // -> this is no conflict anymore
                    continue;

                // the path conflicts with our new path -> don't activate the
                // *complete* new path, but only up to the step which is unambiguous
                nUpperStepBoundary = nDivergenceIndex;
                bIncompletePath = sal_True;
            }
        }

        // now, we have to remove all items after nCurrentStatePathIndex, and insert the items from the active
        // path, up to (excluding) nUpperStepBoundary
        RoadmapTypes::ItemIndex nLoopUntil = ::std::max( (RoadmapTypes::ItemIndex)nUpperStepBoundary, m_pImpl->pRoadmap->GetItemCount() );
        for ( RoadmapTypes::ItemIndex nItemIndex = nCurrentStatePathIndex; nItemIndex < nLoopUntil; ++nItemIndex )
        {
            bool bExistentItem = ( nItemIndex < m_pImpl->pRoadmap->GetItemCount() );
            bool bNeedItem = nItemIndex < nUpperStepBoundary;

            if ( bExistentItem )
            {
                if ( !bNeedItem )
                {
                    while ( nItemIndex < m_pImpl->pRoadmap->GetItemCount() )
                        m_pImpl->pRoadmap->DeleteRoadmapItem( nItemIndex );
                    break;
                }
                else
                {
                    // there is an item with this index in the roadmap - does it match what is requested by
                    // the respective state in the active path?
                    RoadmapTypes::ItemId nPresentItemId = m_pImpl->pRoadmap->GetItemID( nItemIndex );
                    WizardState nRequiredState = aActivePathPos->second[ nItemIndex ];
                    if ( nPresentItemId != nRequiredState )
                    {
                        m_pImpl->pRoadmap->ChangeRoadmapItemLabel( nPresentItemId, getStateDisplayName( nRequiredState ), nItemIndex  );
                        m_pImpl->pRoadmap->EnableRoadmapItem( nPresentItemId, m_pImpl->aDisabledStates.find( nRequiredState ) == m_pImpl->aDisabledStates.end(), nItemIndex  );
                        m_pImpl->pRoadmap->ChangeRoadmapItemID( nPresentItemId, (RoadmapTypes::ItemId)nRequiredState, nItemIndex );
                    }
                }
            }
            else
            {
                DBG_ASSERT( bNeedItem, "RoadmapWizard::implUpdateRoadmap: ehm - none needed, none present - why did the loop not terminate?" );
                if ( bNeedItem )
                    implInsertState( aActivePathPos->second[ nItemIndex ], nItemIndex );
            }
        }

/*
        // remove all old roadmap items
        RoadmapTypes::ItemIndex nDeleteIndex = nCurrentStatePathIndex;
        ++nDeleteIndex;
        while ( nDeleteIndex < m_pImpl->pRoadmap->GetItemCount() )
            m_pImpl->pRoadmap->DeleteRoadmapItem( nDeleteIndex );

        // insert new roadmap items
        for (   Path::const_iterator aStateLoop = aNewPathPos->second.begin() + nCurrentStatePathIndex + 1;
                aStateLoop != aNewPathPos->second.begin() + nUpperStepBoundary;
                ++aStateLoop
            )
        {
            implInsertState( *aStateLoop, aStateLoop - aNewPathPos->second.begin() );
        }
*/

        m_pImpl->pRoadmap->SetRoadmapComplete( !bIncompletePath );
    }

    //--------------------------------------------------------------------
    WizardTypes::WizardState RoadmapWizard::determineNextState( WizardState _nCurrentState )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        sal_Int32 nCurrentStatePathIndex = -1;

        Paths::const_iterator aActivePathPos = m_pImpl->aPaths.find( m_pImpl->nActivePath );
        if ( aActivePathPos != m_pImpl->aPaths.end() )
            nCurrentStatePathIndex = m_pImpl->getStateIndexInPath( _nCurrentState, aActivePathPos->second );

        DBG_ASSERT( nCurrentStatePathIndex != -1, "RoadmapWizard::determineNextState: ehm - how can we travel if there is no (valid) active path?" );
        if ( nCurrentStatePathIndex == -1 )
            return WZS_INVALID_STATE;

        sal_Int32 nNextStateIndex = nCurrentStatePathIndex + 1;

        while   (   ( nNextStateIndex < (sal_Int32)aActivePathPos->second.size() )
                &&  ( m_pImpl->aDisabledStates.find( aActivePathPos->second[ nNextStateIndex ] ) != m_pImpl->aDisabledStates.end() )
                )
        {
            ++nNextStateIndex;
        }

        if ( nNextStateIndex >= (sal_Int32)aActivePathPos->second.size() )
            // there is no next state in the current path (at least none which is enabled)
            return WZS_INVALID_STATE;

        return aActivePathPos->second[ nNextStateIndex ];
    }

    //--------------------------------------------------------------------
    IMPL_LINK( RoadmapWizard, OnRoadmapItemSelected, void*, EMPTYARG )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        RoadmapTypes::ItemId nCurItemId = m_pImpl->pRoadmap->GetCurrentRoadmapItemID();
        if ( nCurItemId == getCurrentState() )
            // nothing to do
            return 1L;

        if(IsInCallOfLink())
            return 0;
        SetInCallOfLink( true );

        sal_Int32 nCurrentIndex = m_pImpl->getStateIndexInPath( getCurrentState(), m_pImpl->nActivePath );
        sal_Int32 nNewIndex     = m_pImpl->getStateIndexInPath( nCurItemId, m_pImpl->nActivePath );

        DBG_ASSERT( ( nCurrentIndex != -1 ) && ( nNewIndex != -1 ),
            "RoadmapWizard::OnRoadmapItemSelected: something's wrong here!" );
        if ( ( nCurrentIndex == -1 ) || ( nNewIndex == -1 ) )
        {
            SetInCallOfLink( false );
            return 0L;
        }

        sal_Bool bResult = sal_True;
        if ( nNewIndex > nCurrentIndex )
        {
            bResult = skipUntil( (WizardState)nCurItemId );
            WizardState nTemp = (WizardState)nCurItemId;
            while( nTemp )
            {
                if( m_pImpl->aDisabledStates.find( --nTemp ) != m_pImpl->aDisabledStates.end() )
                    removePageFromHistory( nTemp );
            }
        }
        else
            bResult = skipBackwardUntil( (WizardState)nCurItemId );

        if ( !bResult )
            m_pImpl->pRoadmap->SelectRoadmapItemByID( getCurrentState() );

        SetInCallOfLink( false );
        return 1L;
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::enterState( WizardState _nState )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        OWizardMachine::enterState( _nState );

        // synchronize the roadmap
        implUpdateRoadmap( );
        m_pImpl->pRoadmap->SelectRoadmapItemByID( getCurrentState() );
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::enableState( WizardState _nState, bool _bEnable )
    {
        DBG_CHKTHIS( RoadmapWizard, CheckInvariants );

        // remember this (in case the state appears in the roadmap later on)
        if ( _bEnable )
            m_pImpl->aDisabledStates.erase( _nState );
        else
        {
            m_pImpl->aDisabledStates.insert( _nState );
            removePageFromHistory( _nState );
        }

        // if the state is currently in the roadmap, reflect it's new status
        m_pImpl->pRoadmap->EnableRoadmapItem( (RoadmapTypes::ItemId)_nState, _bEnable );
    }
    //--------------------------------------------------------------------
    bool    RoadmapWizard::isStateEnabled( WizardState _nState )
    {
        sal_Bool bEnabled = m_pImpl->aDisabledStates.find( _nState ) == m_pImpl->aDisabledStates.end();
        return bEnabled;
    }
    //--------------------------------------------------------------------
    void RoadmapWizard::Resize()
    {
        OWizardMachine::Resize();

        if ( IsReallyShown() && !IsInInitShow() )
            ResizeFixedLine();
    }


    //--------------------------------------------------------------------
    void RoadmapWizard::StateChanged( StateChangedType nType )
    {
        WizardDialog::StateChanged( nType );

        if ( nType == STATE_CHANGE_INITSHOW )
            ResizeFixedLine();
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::ResizeFixedLine()
    {
        Size aSize( m_pImpl->pRoadmap->GetSizePixel() );
        aSize.Width() = m_pImpl->pFixedLine->GetSizePixel().Width();
        m_pImpl->pFixedLine->SetSizePixel( aSize );
    }

    //--------------------------------------------------------------------
    void RoadmapWizard::updateRoadmapItemLabel( WizardState _nState )
    {
        Paths::const_iterator aActivePathPos = m_pImpl->aPaths.find( m_pImpl->nActivePath );
        RoadmapTypes::ItemIndex nUpperStepBoundary = (RoadmapTypes::ItemIndex)aActivePathPos->second.size();
        RoadmapTypes::ItemIndex nLoopUntil = ::std::max( (RoadmapTypes::ItemIndex)nUpperStepBoundary, m_pImpl->pRoadmap->GetItemCount() );
        sal_Int32 nCurrentStatePathIndex = -1;
        if ( m_pImpl->nActivePath != -1 )
            nCurrentStatePathIndex = m_pImpl->getStateIndexInPath( getCurrentState(), m_pImpl->nActivePath );
        for ( RoadmapTypes::ItemIndex nItemIndex = nCurrentStatePathIndex; nItemIndex < nLoopUntil; ++nItemIndex )
        {
            bool bExistentItem = ( nItemIndex < m_pImpl->pRoadmap->GetItemCount() );
            if ( bExistentItem )
            {
                // there is an item with this index in the roadmap - does it match what is requested by
                // the respective state in the active path?
                RoadmapTypes::ItemId nPresentItemId = m_pImpl->pRoadmap->GetItemID( nItemIndex );
                WizardState nRequiredState = aActivePathPos->second[ nItemIndex ];
                if ( _nState == nRequiredState )
                {
                    m_pImpl->pRoadmap->ChangeRoadmapItemLabel( nPresentItemId, getStateDisplayName( nRequiredState ) );
                    break;
                }
            }
        }
    }

//........................................................................
}   // namespace svt
//........................................................................
