/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: registrationdlg.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 21:58:34 $
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

#ifndef SVTOOLS_REGISTRATIONDLG_HXX
#include "registrationdlg.hxx"
#endif

#if 0 /* @@@ */
#ifndef _SVTOOLS_SVTDATA_HXX
#include <svtools/svtdata.hxx>
#endif
#ifndef _SVTOOLS_HRC
#include <svtools/svtools.hrc>
#endif
#endif /* @@@ */

#ifndef SVTOOLS_REGISTRATIONDLG_HRC
#include "registrationdlg.hrc"
#endif

#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

//........................................................................
namespace svt
{
//........................................................................

    static void lcl_moveControls( Control** _ppControls, sal_Int32 _nAmount )
    {
        if ( _ppControls )
            while ( *_ppControls )
            {
                Point aPos = (*_ppControls)->GetPosPixel();
                aPos.Y() += _nAmount;
                (*_ppControls)->SetPosPixel( aPos );

                ++_ppControls;
            }
    }

    //====================================================================
    //= RegistrationDialog
    //====================================================================
    //--------------------------------------------------------------------
    RegistrationDialog::RegistrationDialog( Window* _pWindow, const ResId& _rResId, bool _bEvalVersion )
        :ModalDialog( _pWindow, _rResId )
        ,m_eResponse        ( urRegisterLater )
        ,m_aLogo            ( this, ResId( FI_LOGO, *_rResId.GetResMgr() ) )
        ,m_aIntro           ( this, ResId( FT_INTRO, *_rResId.GetResMgr() ) )
        ,m_aNow             ( this, ResId( RB_NOW, *_rResId.GetResMgr() ) )
        ,m_aLater           ( this, ResId( RB_LATER, *_rResId.GetResMgr() ) )
        ,m_aNever           ( this, ResId( RB_NEVER, *_rResId.GetResMgr() ) )
        ,m_aAlreadyDone     ( this, ResId( RB_DONE, *_rResId.GetResMgr() ) )
        ,m_aSeparator       ( this, ResId( FL_SEPARATOR, *_rResId.GetResMgr() ) )
        ,m_aOK              ( this, ResId( BTN_OK, *_rResId.GetResMgr() ) )
        ,m_aHelp            ( this, ResId( BTN_HELP, *_rResId.GetResMgr() ) )
    {
        if ( _bEvalVersion )
        {   // if we're an eval version, we need to hide two of the options
            m_aNever.Hide( );
            m_aAlreadyDone.Hide( );

            // make the explanatory text somewhat smaller
            Size aIntroSize = m_aIntro.GetSizePixel();
            aIntroSize.Height() = LogicToPixel( Size( 0, 18 ), MAP_APPFONT ).Height();
            sal_Int32 nHeightDifference = m_aIntro.GetSizePixel().Height() - aIntroSize.Height();
            m_aIntro.SetSizePixel( aIntroSize );

            // resize the dialog, and move the controls below the ones we just hided
            sal_Int32 nAlreadyDoneLower = m_aAlreadyDone.GetPosPixel().Y() + m_aAlreadyDone.GetSizePixel().Height();
            sal_Int32 nLaterLower = m_aLater.GetPosPixel().Y() + m_aLater.GetSizePixel().Height();
            sal_Int32 nDifference = nAlreadyDoneLower - nLaterLower;

            sal_Int32 nOverallDifference = nDifference + nHeightDifference;

            // move
            Control* pVisibleRadios[] = { &m_aNow, &m_aLater, NULL };
            lcl_moveControls( pVisibleRadios, -nHeightDifference );

            Control* pControlsToMove[] = { &m_aSeparator, &m_aOK, &m_aHelp, NULL };
            lcl_moveControls( pControlsToMove, -nOverallDifference );

            // resize the dialog
            Size aSize = GetSizePixel();
            aSize.Height() -= nOverallDifference;
            SetSizePixel( aSize );
        }
        else
        {
            // the explanatory text needs to be completed
            String sCompleteIntro = m_aIntro.GetText( );
            sCompleteIntro += String( ResId( STR_COMPLETE_INTRO, *_rResId.GetResMgr() ) );
            m_aIntro.SetText( sCompleteIntro );
        }

        FreeResource();

        m_aNow.Check( TRUE );
    }

    //--------------------------------------------------------------------
    short RegistrationDialog::Execute()
    {
        short nResult = ModalDialog::Execute();

        // as a default, assume that the user wants to be reminded
        m_eResponse = urRegisterLater;

        if ( RET_OK == nResult )
        {
            if ( m_aNow.IsChecked() )
                m_eResponse = urRegisterNow;
            else if ( m_aLater.IsChecked() )
                m_eResponse = urRegisterLater;
            else if ( m_aNever.IsChecked() )
                m_eResponse = urRegisterNever;
            else if ( m_aAlreadyDone.IsChecked() )
                m_eResponse = urAlreadyRegistered;
#ifdef DBG_UTIL
            else
            {
                DBG_ERROR( "RegistrationDialog::Execute: invalid dialog state!" );
            }
#endif
        }
        return nResult;
    }
    //--------------------------------------------------------------------
    long RegistrationDialog::PreNotify( NotifyEvent& rNEvt )
    {
        long nHandled;
        if( rNEvt.GetType() == EVENT_KEYINPUT &&
            rNEvt.GetKeyEvent()->GetCharCode() &&
            rNEvt.GetKeyEvent()->GetKeyCode().GetCode() == KEY_ESCAPE)
        {
            EndDialog(RET_CANCEL);
            nHandled = 1;
        }
        else
            nHandled = ModalDialog::PreNotify( rNEvt );
        return nHandled;
    }


//........................................................................
}   // namespace svt
//........................................................................


