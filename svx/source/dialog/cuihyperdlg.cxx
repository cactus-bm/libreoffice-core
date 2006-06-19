/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: cuihyperdlg.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 15:04:47 $
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

#ifdef SVX_DLLIMPLEMENTATION
#undef SVX_DLLIMPLEMENTATION
#endif

// include ---------------------------------------------------------------

#ifndef _SV_SETTINGS_HXX
#include <vcl/settings.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_VIEWOPTIONS_HXX
#include <svtools/viewoptions.hxx>
#endif

#include "cuihyperdlg.hxx"

#include "hlinettp.hxx"
#include "hlmailtp.hxx"
#include "hldoctp.hxx"
#include "hldocntp.hxx"

#include "hyperdlg.hrc"

//########################################################################
//#                                                                      #
//# Childwindow-Wrapper-Class                                            #
//#                                                                      #
//########################################################################

SvxHlinkCtrl::SvxHlinkCtrl( USHORT _nId, SfxBindings & rBindings, SvxHpLinkDlg* pDlg )
: SfxControllerItem ( _nId, rBindings )
  ,aOnlineForwarder  ( SID_INTERNET_ONLINE , *this )
  ,aRdOnlyForwarder  ( SID_READONLY_MODE, *this )
{
    pParent = pDlg;
}

void SvxHlinkCtrl::StateChanged( USHORT nSID, SfxItemState eState,
                                 const SfxPoolItem* pState )
{
    if ( eState == SFX_ITEM_AVAILABLE )
    {
        switch ( nSID )
        {
            case SID_INTERNET_ONLINE :
            {
                pParent->EnableInetBrowse( !( (SfxBoolItem*)pState)->GetValue() );
            }
            break;
            case SID_HYPERLINK_GETLINK :
            {
                pParent->SetPage ( (SvxHyperlinkItem*)pState);
            }
            break;
            case SID_READONLY_MODE :
            {
                pParent->SetReadOnlyMode( ( (SfxBoolItem*)pState)->GetValue() == TRUE );
            }
            break;
        }
    }
}



// -----------------------------------------------------------------------



//########################################################################
//#                                                                      #
//# Hyperlink - Dialog                                                   #
//#                                                                      #
//########################################################################

/*************************************************************************
|*
|* Contructor / Destructor
|*
|************************************************************************/

SvxHpLinkDlg::SvxHpLinkDlg (Window* pParent, SfxBindings* pBindings)
:   IconChoiceDialog( pParent, SVX_RES ( RID_SVXDLG_NEWHYPERLINK ) ),
    maCtrl          ( SID_HYPERLINK_GETLINK, *pBindings, this ),
    mpBindings      ( pBindings ),
    mbReadOnly      ( sal_False ),
    mbIsHTMLDoc     ( sal_False )
{
    mbGrabFocus = sal_True;
    // insert pages
    Image aImage;
    Image aImageHC;
    String aStrTitle;
    SvxIconChoiceCtrlEntry* pEntry = NULL;

    aStrTitle = SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLINETTP );
    aImage = Image( SVX_RES ( RID_SVXBMP_HLINETTP ) );
    aImageHC = Image( SVX_RES ( RID_SVXBMP_HLINETTP_H ) );
    pEntry = AddTabPage ( RID_SVXPAGE_HYPERLINK_INTERNET, aStrTitle, aImage, aImageHC, SvxHyperlinkInternetTp::Create );
    pEntry->SetQuickHelpText( SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLINETTP_HELP ) );
    aStrTitle = SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLMAILTP );
    aImage = Image( SVX_RES ( RID_SVXBMP_HLMAILTP ) );
    aImageHC = Image( SVX_RES ( RID_SVXBMP_HLMAILTP_H ) );
    pEntry = AddTabPage ( RID_SVXPAGE_HYPERLINK_MAIL, aStrTitle, aImage, aImageHC, SvxHyperlinkMailTp::Create );
    pEntry->SetQuickHelpText( SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLMAILTP_HELP ) );
    aStrTitle = SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLDOCTP );
    aImage = Image( SVX_RES ( RID_SVXBMP_HLDOCTP ) );
    aImageHC = Image( SVX_RES ( RID_SVXBMP_HLDOCTP_H ) );
    pEntry = AddTabPage ( RID_SVXPAGE_HYPERLINK_DOCUMENT, aStrTitle, aImage, aImageHC, SvxHyperlinkDocTp::Create );
    pEntry->SetQuickHelpText( SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLDOCTP_HELP ) );
    aStrTitle = SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLDOCNTP );
    aImage = Image( SVX_RES ( RID_SVXBMP_HLDOCNTP ) );
    aImageHC = Image( SVX_RES ( RID_SVXBMP_HLDOCNTP_H ) );
    pEntry = AddTabPage ( RID_SVXPAGE_HYPERLINK_NEWDOCUMENT, aStrTitle, aImage, aImageHC, SvxHyperlinkNewDocTp::Create );
    pEntry->SetQuickHelpText( SVX_RESSTR( RID_SVXSTR_HYPERDLG_HLDOCNTP_HELP ) );

    // all tab pages set -> create mnemonics
    //  CreateIconTextAutoMnemonics();  #99671# not useful, because this is not what user expects when using mnemonics on the pages

    // create itemset for tabpages
    mpItemSet = new SfxItemSet( SFX_APP()->GetPool(), SID_HYPERLINK_GETLINK,
                               SID_HYPERLINK_SETLINK );

    SvxHyperlinkItem aItem;
    mpItemSet->Put (aItem, SID_HYPERLINK_GETLINK);

    SetInputSet (mpItemSet);

    // Init Dialog
    Start (FALSE);

    pBindings->Update( SID_READONLY_MODE );

    // set OK/Cancel - button
    GetOKButton().SetText ( SVX_RESSTR(RID_SVXSTR_HYPDLG_APPLYBUT) );
    GetCancelButton().SetText ( SVX_RESSTR(RID_SVXSTR_HYPDLG_CLOSEBUT) );

    GetOKButton().SetClickHdl    ( LINK ( this, SvxHpLinkDlg, ClickApplyHdl_Impl ) );
    GetCancelButton().SetClickHdl( LINK ( this, SvxHpLinkDlg, ClickCloseHdl_Impl ) );
}

SvxHpLinkDlg::~SvxHpLinkDlg ()
{
    // delete config item, so the base class (IconChoiceDialog) can not load it on the next start
    SvtViewOptions aViewOpt( E_TABDIALOG, String::CreateFromInt32( SID_HYPERLINK_DIALOG ) );
    aViewOpt.Delete();

    delete mpItemSet;
}

/*************************************************************************
|*
|* Close Dialog-Window
|*
|************************************************************************/

BOOL SvxHpLinkDlg::Close()
{
    GetDispatcher()->Execute( SID_HYPERLINK_DIALOG,
                              SFX_CALLMODE_ASYNCHRON |
                              SFX_CALLMODE_RECORD);
    return TRUE;
}

/*************************************************************************
|*
|* When extrawindow is visible and its never moved by user, then move that
|* window, too.
|*
|************************************************************************/

void SvxHpLinkDlg::Move()
{
    SvxHyperlinkTabPageBase* pCurrentPage = ( SvxHyperlinkTabPageBase* )
                                            GetTabPage ( GetCurPageId() );

    if( pCurrentPage->IsMarkWndVisible () )
    {
        // Pos&Size of this dialog-window
        Point aDlgPos ( GetPosPixel () );
        Size aDlgSize ( GetSizePixel () );

        // Size of Office-Main-Window
        Size aWindowSize( SFX_APP()->GetTopWindow()->GetSizePixel() );

        // Size of Extrawindow
        Size aExtraWndSize( pCurrentPage->GetSizeExtraWnd() );

        BOOL bDoInvalid ;
        if( aDlgPos.X()+(1.02*aDlgSize.Width())+aExtraWndSize.Width() > aWindowSize.Width() )
        {
            if( aDlgPos.X() - ( 0.02*aDlgSize.Width() ) - aExtraWndSize.Width() < 0 )
            {
                // Pos Extrawindow anywhere
                bDoInvalid = pCurrentPage->MoveToExtraWnd( Point( 1, long(1.1*aDlgPos.Y()) ), TRUE );
            }
            else
            {
                // Pos Extrawindow on the left side of Dialog
                bDoInvalid = pCurrentPage->MoveToExtraWnd( aDlgPos -
                                                           Point( long(0.02*aDlgSize.Width()), 0 ) -
                                                           Point( aExtraWndSize.Width(), 0 ) );
            }
        }
        else
        {
            // Pos Extrawindow on the right side of Dialog
            bDoInvalid = pCurrentPage->MoveToExtraWnd ( aDlgPos + Point( long(1.02*aDlgSize.Width()), 0 ) );
        }

        if ( bDoInvalid )
            Invalidate(INVALIDATE_BACKGROUND);
    }

    Window::Move();
}

/*long SvxHpLinkDlg::PreNotify( NotifyEvent& rNEvt )
{
    long nRet = 0;

    if( rNEvt.GetType() == EVENT_KEYINPUT )
    {
        DBG_ASSERT( rNEvt.GetKeyEvent(), "-SvxHpLinkDlg::PreNotify(): no KeyEvent for key event?!" );

        const KeyEvent* pKEvt = rNEvt.GetKeyEvent();

        if( KEY_MOD2 == pKEvt->GetKeyCode().GetModifier() && pKEvt->GetCharCode() && HandleShortCutKey( *pKEvt ) )
            nRet = 1;
    }

    if( !nRet )
        nRet = IconChoiceDialog::PreNotify( rNEvt );

    return nRet;
}*/

/*************************************************************************
|*
|* Click on Apply-button
|*
|************************************************************************/

IMPL_LINK ( SvxHpLinkDlg, ClickApplyHdl_Impl, void *, EMPTYARG )
{
    SfxItemSet aItemSet( SFX_APP()->GetPool(), SID_HYPERLINK_GETLINK,
                         SID_HYPERLINK_SETLINK );

    SvxHyperlinkTabPageBase* pCurrentPage = (SvxHyperlinkTabPageBase*)
                                            GetTabPage ( GetCurPageId() );

    if ( pCurrentPage->AskApply() )
    {
        pCurrentPage->FillItemSet( aItemSet );

        SvxHyperlinkItem *aItem = (SvxHyperlinkItem *)
                                  aItemSet.GetItem (SID_HYPERLINK_SETLINK);

        String aStrEmpty;
        if ( aItem->GetURL() != aStrEmpty )
            GetDispatcher()->Execute( SID_HYPERLINK_SETLINK, SFX_CALLMODE_ASYNCHRON |
                                      SFX_CALLMODE_RECORD, aItem, 0L);

        ( (SvxHyperlinkTabPageBase*)GetTabPage ( GetCurPageId() ) )->DoApply();
    }

    return( 0L );
}

/*************************************************************************
|*
|* Click on Close-button
|*
|************************************************************************/

IMPL_LINK ( SvxHpLinkDlg, ClickCloseHdl_Impl, void *, EMPTYARG )
{
    Close();

    return( 0L );
}

/*************************************************************************
|*
|* Set Page
|*
|************************************************************************/

USHORT SvxHpLinkDlg::SetPage ( SvxHyperlinkItem* pItem )
{
    USHORT nPageId = RID_SVXPAGE_HYPERLINK_INTERNET;

    String aStrURL ( pItem->GetURL() );
    INetURLObject aURL ( aStrURL );
    INetProtocol eProtocolTyp = aURL.GetProtocol();

    switch ( eProtocolTyp )
    {
        case INET_PROT_HTTP :
        case INET_PROT_FTP :
        case INET_PROT_TELNET :
            nPageId = RID_SVXPAGE_HYPERLINK_INTERNET;
            break;
        case INET_PROT_FILE :
        case INET_PROT_POP3 :
        case INET_PROT_IMAP :
            nPageId = RID_SVXPAGE_HYPERLINK_DOCUMENT;
            break;
        case INET_PROT_MAILTO :
        case INET_PROT_NEWS :
            nPageId = RID_SVXPAGE_HYPERLINK_MAIL;
            break;
        default :
            sal_Char const sNewsSrvScheme[] = "news://";
                // TODO news:// is nonsense

            if ( aStrURL.SearchAscii( sNewsSrvScheme ) == 0 )
                nPageId = RID_SVXPAGE_HYPERLINK_DOCUMENT;
            else
            {
                sal_Char const sHash[] = "#";
                if( aStrURL.SearchAscii( sHash ) == 0 )
                    nPageId = RID_SVXPAGE_HYPERLINK_DOCUMENT;
                else
                {
                    eProtocolTyp = INET_PROT_NOT_VALID;
                    nPageId = GetCurPageId();
                }
            }
            break;
    }

    ShowPage (nPageId);

    SvxHyperlinkTabPageBase* pCurrentPage = (SvxHyperlinkTabPageBase*)GetTabPage( nPageId );

    mbIsHTMLDoc = BOOL( pItem->GetInsertMode() & HLINK_HTMLMODE );

    SfxItemSet& aPageSet =  (SfxItemSet&)GetTabPage (nPageId)->GetItemSet ();
    aPageSet.Put ( *pItem );

    pCurrentPage->Reset( aPageSet );
    if ( mbGrabFocus )
    {
        pCurrentPage->SetInitFocus();   // #92535# grab the focus only once at initialization
        mbGrabFocus = sal_False;
    }
    return nPageId;
}

/*************************************************************************
|*
|* Enable/Disable to browse targets in a html-doc
|*
|************************************************************************/

void SvxHpLinkDlg::EnableInetBrowse( sal_Bool bEnable )
{
    SvxHyperlinkTabPageBase* pCurrentPage = ( SvxHyperlinkTabPageBase* )
                                            GetTabPage ( GetCurPageId() );
    pCurrentPage->SetOnlineMode( bEnable );
}

/*************************************************************************
|*
|* Enable/Disable ReadOnly mode
|*
|************************************************************************/

void SvxHpLinkDlg::SetReadOnlyMode( sal_Bool bRdOnly )
{
    mbReadOnly = bRdOnly;
    if ( bRdOnly )
        GetOKButton().Disable();
    else
        GetOKButton().Enable();
}

