/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: orienthelper.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 15:24:40 $
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

#ifndef SVX_ORIENTHELPER_HXX
#include "orienthelper.hxx"
#endif

#include <vector>
#include <utility>

#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef SVX_DIALCONTROL_HXX
#include "dialcontrol.hxx"
#endif

namespace svx {

// ============================================================================

struct OrientationHelper_Impl
{
    typedef std::pair< Window*, TriState >  WindowPair;
    typedef std::vector< WindowPair >       WindowVec;

    OrientationHelper&  mrParent;
    WindowVec           maWinVec;
    svx::DialControl&   mrCtrlDial;
    CheckBox&           mrCbStacked;

    explicit            OrientationHelper_Impl(
                            OrientationHelper& rParent,
                            svx::DialControl& rCtrlDial,
                            CheckBox& rCbStacked );

    void                AddDependentWindow( Window& rWindow, TriState eDisableIfStacked );

    void                EnableDependentWindows();
    void                EnableWindow( Window& rWindow, TriState eDisableIfStacked );

    void                ShowDependentWindows();

    DECL_LINK( ClickHdl, void* );
};

// ----------------------------------------------------------------------------

OrientationHelper_Impl::OrientationHelper_Impl(
        OrientationHelper& rParent, svx::DialControl& rCtrlDial, CheckBox& rCbStacked ) :
    mrParent( rParent ),
    mrCtrlDial( rCtrlDial ),
    mrCbStacked( rCbStacked )
{
    maWinVec.push_back( WindowPair( &mrCtrlDial, STATE_CHECK ) );
    maWinVec.push_back( WindowPair( &mrCbStacked, STATE_DONTKNOW ) );
    mrCbStacked.SetClickHdl( LINK( this, OrientationHelper_Impl, ClickHdl ) );
}

void OrientationHelper_Impl::AddDependentWindow( Window& rWindow, TriState eDisableIfStacked )
{
    maWinVec.push_back( std::make_pair( &rWindow, eDisableIfStacked ) );
    EnableWindow( rWindow, eDisableIfStacked );
}

void OrientationHelper_Impl::EnableDependentWindows()
{
    for( WindowVec::iterator aIt = maWinVec.begin(), aEnd = maWinVec.end(); aIt != aEnd; ++aIt )
        EnableWindow( *aIt->first, aIt->second );
}

void OrientationHelper_Impl::EnableWindow( Window& rWindow, TriState eDisableIfStacked )
{
    bool bEnabled = mrParent.IsEnabled();
    bool bDisableOnStacked = false;
    switch( eDisableIfStacked )
    {
        // STATE_CHECK: Disable window, if stacked text is turned on or "don't know".
        case STATE_CHECK:   bDisableOnStacked = (mrCbStacked.GetState() != STATE_NOCHECK);  break;
        // STATE_NOCHECK: Disable window, if stacked text is turned off or "don't know".
        case STATE_NOCHECK: bDisableOnStacked = (mrCbStacked.GetState() != STATE_CHECK);    break;
        default: ;//prevent warning
    }
    rWindow.Enable( bEnabled && !bDisableOnStacked );
}

void OrientationHelper_Impl::ShowDependentWindows()
{
    bool bVisible = mrParent.IsVisible();
    for( WindowVec::iterator aIt = maWinVec.begin(), aEnd = maWinVec.end(); aIt != aEnd; ++aIt )
        aIt->first->Show( bVisible );
}

IMPL_LINK( OrientationHelper_Impl, ClickHdl, void*, EMPTYARG )
{
    EnableDependentWindows();
    return 0L;
}

// ============================================================================

OrientationHelper::OrientationHelper(
        Window* pParent, svx::DialControl& rCtrlDial, CheckBox& rCbStacked ) :
    Window( pParent ),
    mpImpl( new OrientationHelper_Impl( *this, rCtrlDial, rCbStacked ) )
{
    mpImpl->EnableDependentWindows();
}

OrientationHelper::OrientationHelper(
        Window* pParent,
        svx::DialControl& rCtrlDial, NumericField& rNfRotation, CheckBox& rCbStacked ) :
    Window( pParent ),
    mpImpl( new OrientationHelper_Impl( *this, rCtrlDial, rCbStacked ) )
{
    mpImpl->mrCtrlDial.SetLinkedField( &rNfRotation );
    mpImpl->EnableDependentWindows();
}

OrientationHelper::~OrientationHelper()
{
}

void OrientationHelper::StateChanged( StateChangedType nStateChange )
{
    if( nStateChange == STATE_CHANGE_ENABLE )
        mpImpl->EnableDependentWindows();
    if( nStateChange == STATE_CHANGE_VISIBLE )
        mpImpl->ShowDependentWindows();
    Window::StateChanged( nStateChange );
}

void OrientationHelper::AddDependentWindow( Window& rWindow, TriState eDisableIfStacked )
{
    mpImpl->AddDependentWindow( rWindow, eDisableIfStacked );
}

void OrientationHelper::SetStackedState( TriState eState )
{
    if( eState != GetStackedState() )
    {
        mpImpl->mrCbStacked.SetState( eState );
        mpImpl->EnableDependentWindows();
    }
}

TriState OrientationHelper::GetStackedState() const
{
    return mpImpl->mrCbStacked.GetState();
}

void OrientationHelper::EnableStackedTriState( bool bEnable )
{
    mpImpl->mrCbStacked.EnableTriState( bEnable );
}

// ============================================================================

OrientStackedWrapper::OrientStackedWrapper( OrientationHelper& rOrientHlp ) :
    SingleControlWrapperType( rOrientHlp )
{
}

bool OrientStackedWrapper::IsControlDontKnow() const
{
    return GetControl().GetStackedState() == STATE_DONTKNOW;
}

void OrientStackedWrapper::SetControlDontKnow( bool bSet )
{
    GetControl().EnableStackedTriState( bSet );
    GetControl().SetStackedState( bSet ? STATE_DONTKNOW : STATE_NOCHECK );
}

bool OrientStackedWrapper::GetControlValue() const
{
    return GetControl().GetStackedState() == STATE_CHECK;
}

void OrientStackedWrapper::SetControlValue( bool bValue )
{
    GetControl().SetStackedState( bValue ? STATE_CHECK : STATE_NOCHECK );
}

// ============================================================================

} // namespace svx

