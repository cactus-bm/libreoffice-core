/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: license_dialog.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-26 08:24:22 $
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
#include "precompiled_desktop.hxx"

#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "unotools/configmgr.hxx"
#include "comphelper/servicedecl.hxx"
#include "comphelper/unwrapargs.hxx"
#include "i18npool/mslangid.hxx"
#include "vcl/svapp.hxx"
#include "vcl/msgbox.hxx"
#include "toolkit/helper/vclunohelper.hxx"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/task/XJobExecutor.hpp"
#include "svtools/svmedit.hxx"
#include "svtools/lstner.hxx"
#include "svtools/xtextedt.hxx"
//#include <vcl/mnemonic.hxx>
//#include <vcl/tabpage.hxx>
//#include <vcl/fixed.hxx>
//#include <vcl/button.hxx>
//#include <vcl/dialog.hxx>
#include <vcl/scrbar.hxx>
//#include <svtools/wizardmachine.hxx>



//#include "boost/bind.hpp"
#include "dp_gui_shared.hxx"
#include "license_dialog.hxx"
#include "dp_gui.hrc"

using namespace ::dp_misc;
namespace cssu = ::com::sun::star::uno;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using ::rtl::OUString;

namespace dp_gui {

class LicenseView : public MultiLineEdit, public SfxListener
{
    BOOL            mbEndReached;
    Link            maEndReachedHdl;
    Link            maScrolledHdl;

public:
    LicenseView( Window* pParent, const ResId& rResId );
    ~LicenseView();

    void ScrollDown( ScrollType eScroll );

    BOOL IsEndReached() const;
    BOOL EndReached() const { return mbEndReached; }
    void SetEndReached( BOOL bEnd ) { mbEndReached = bEnd; }

    void SetEndReachedHdl( const Link& rHdl )  { maEndReachedHdl = rHdl; }
    const Link& GetAutocompleteHdl() const { return maEndReachedHdl; }

    void SetScrolledHdl( const Link& rHdl )  { maScrolledHdl = rHdl; }
    const Link& GetScrolledHdl() const { return maScrolledHdl; }

    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

protected:
    using MultiLineEdit::Notify;
};

struct LicenseDialogImpl : public ModalDialog
{
    cssu::Reference<cssu::XComponentContext> m_xComponentContext;
    ::rtl::OUString m_sTitle;
    //virtual BOOL Close();
    //virtual void Resize();
    //DECL_LINK( headbar_dragEnd, HeaderBar * );
    //
    //void clickClose( USHORT id );
    //void clickAdd( USHORT id );
    //void clickRemove( USHORT id );

    //void updateButtonStates(
    //    css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv =
    //    com::sun::star::uno::Reference<
    //    com::sun::star::ucb::XCommandEnvironment>() );
    //
    //css::uno::Reference<css::uno::XComponentContext> m_xComponentContext;

    // controls:
    //::std::auto_ptr<SelectionBoxControl> m_selectionBox;
    //::std::auto_ptr<HeaderBar> m_headerBar;
    //::std::auto_ptr<TreeListBoxImpl> m_treelb;

    //::std::auto_ptr<ThreadedPushButton> m_addButton;
    //::std::auto_ptr<ThreadedPushButton> m_removeButton;
    //::std::auto_ptr<ThreadedPushButton> m_enableButton;
    //::std::auto_ptr<ThreadedPushButton> m_disableButton;
    //::std::auto_ptr<ThreadedPushButton> m_exportButton;
    //::std::auto_ptr<FixedLine> m_bottomLine;

    FixedText m_ftHead;
    FixedText m_ftBody1;
    FixedText m_ftBody1Txt;
    FixedText m_ftBody2;
    FixedText m_ftBody2Txt;
    FixedImage m_fiArrow1;
    FixedImage m_fiArrow2;
    LicenseView m_mlLicense;
    PushButton m_pbDown;
    FixedLine m_flBottom;

    OKButton m_acceptButton;
    CancelButton m_declineButton;

    DECL_LINK(PageDownHdl, PushButton*);
    DECL_LINK(ScrolledHdl, LicenseView*);
    DECL_LINK(EndReachedHdl, LicenseView*);

    bool m_bLicenseRead;
//    DECL_STATIC_LINK( DialogImpl, destroyDialog, void * );
//    static ::rtl::Reference<LicenseDialogImpl> s_dialog;

    virtual ~LicenseDialogImpl();

    LicenseDialogImpl(
        Window * pParent,
        css::uno::Reference< css::uno::XComponentContext > const & xContext,
        const ::rtl::OUString & sLicenseText);

    virtual void Activate();

};

LicenseView::LicenseView( Window* pParent, const ResId& rResId )
    : MultiLineEdit( pParent, rResId )
{
    SetLeftMargin( 5 );
    mbEndReached = IsEndReached();
    StartListening( *GetTextEngine() );
}

LicenseView::~LicenseView()
{
    maEndReachedHdl = Link();
    maScrolledHdl   = Link();
    EndListeningAll();
}

void LicenseView::ScrollDown( ScrollType eScroll )
{
    ScrollBar*  pScroll = GetVScrollBar();
    if ( pScroll )
        pScroll->DoScrollAction( eScroll );
}

BOOL LicenseView::IsEndReached() const
{
    BOOL bEndReached;

    ExtTextView*    pView = GetTextView();
    ExtTextEngine*  pEdit = GetTextEngine();
    ULONG           nHeight = pEdit->GetTextHeight();
    Size            aOutSize = pView->GetWindow()->GetOutputSizePixel();
    Point           aBottom( 0, aOutSize.Height() );

    if ( (ULONG) pView->GetDocPos( aBottom ).Y() >= nHeight - 1 )
        bEndReached = TRUE;
    else
        bEndReached = FALSE;

    return bEndReached;
}

void LicenseView::Notify( SfxBroadcaster&, const SfxHint& rHint )
{
    if ( rHint.IsA( TYPE(TextHint) ) )
    {
        BOOL    bLastVal = EndReached();
        ULONG   nId = ((const TextHint&)rHint).GetId();

        if ( nId == TEXT_HINT_PARAINSERTED )
        {
            if ( bLastVal )
                mbEndReached = IsEndReached();
        }
        else if ( nId == TEXT_HINT_VIEWSCROLLED )
        {
            if ( ! mbEndReached )
                mbEndReached = IsEndReached();
            maScrolledHdl.Call( this );
        }

        if ( EndReached() && !bLastVal )
        {
            maEndReachedHdl.Call( this );
        }
    }
}

//==============================================================================================================

LicenseDialogImpl::LicenseDialogImpl(
    Window * pParent,
    cssu::Reference< cssu::XComponentContext > const & xContext,
    const ::rtl::OUString & sLicenseText):
        ModalDialog(pParent, DpGuiResId(RID_DLG_LICENSE))
        ,m_xComponentContext(xContext)
        ,m_sTitle(String(DpGuiResId(RID_STR_LICENSE_TITLE)))
        ,m_ftHead(this, DpGuiResId(FT_LICENSE_HEADER))
        ,m_ftBody1(this, DpGuiResId(FT_LICENSE_BODY_1))
        ,m_ftBody1Txt(this, DpGuiResId(FT_LICENSE_BODY_1_TXT))
        ,m_ftBody2(this, DpGuiResId(FT_LICENSE_BODY_2))
        ,m_ftBody2Txt(this, DpGuiResId(FT_LICENSE_BODY_2_TXT))
        ,m_fiArrow1(this, DpGuiResId(FI_LICENSE_ARROW1))
        ,m_fiArrow2(this, DpGuiResId(FI_LICENSE_ARROW2))
        ,m_mlLicense(this, DpGuiResId(ML_LICENSE))
        ,m_pbDown(this, DpGuiResId(PB_LICENSE_DOWN))
        ,m_flBottom(this, DpGuiResId(FL_LICENSE))
        ,m_acceptButton(this, DpGuiResId(BTN_LICENSE_ACCEPT))
        ,m_declineButton(this, DpGuiResId(BTN_LICENSE_DECLINE))
        ,m_bLicenseRead(false)

{

    if (GetBackground().GetColor().IsDark())
    {
        // high contrast mode needs other images
        m_fiArrow1.SetImage(Image(DpGuiResId(IMG_LICENCE_ARROW_HC)));
        m_fiArrow2.SetImage(Image(DpGuiResId(IMG_LICENCE_ARROW_HC)));
    }

    FreeResource();

    m_acceptButton.SetUniqueId(UID_BTN_LICENSE_ACCEPT);
    m_fiArrow1.Show(true);
    m_fiArrow2.Show(false);
    m_mlLicense.SetText(sLicenseText);

    m_mlLicense.SetEndReachedHdl( LINK(this, LicenseDialogImpl, EndReachedHdl) );
    m_mlLicense.SetScrolledHdl( LINK(this, LicenseDialogImpl, ScrolledHdl) );
    m_pbDown.SetClickHdl( LINK(this, LicenseDialogImpl, PageDownHdl) );

    // We want a automatic repeating page down button
    WinBits aStyle = m_pbDown.GetStyle();
    aStyle |= WB_REPEAT;
    m_pbDown.SetStyle( aStyle );
}

LicenseDialogImpl::~LicenseDialogImpl()
{
}

void LicenseDialogImpl::Activate()
{
    if (!m_bLicenseRead)
    {
        //Only enable the scroll down button if the license text does not fit into the window
        if (m_mlLicense.IsEndReached())
        {
            m_pbDown.Disable();
            m_acceptButton.Enable();
            m_acceptButton.GrabFocus();
        }
        else
        {
            m_pbDown.Enable();
            m_pbDown.GrabFocus();
            m_acceptButton.Disable();
        }
    }
}

IMPL_LINK( LicenseDialogImpl, ScrolledHdl, LicenseView *, EMPTYARG )
{

    if (m_mlLicense.IsEndReached())
        m_pbDown.Disable();
    else
        m_pbDown.Enable();

    return 0;
}

IMPL_LINK( LicenseDialogImpl, PageDownHdl, PushButton *, EMPTYARG )
{
    m_mlLicense.ScrollDown( SCROLL_PAGEDOWN );
    return 0;
}

IMPL_LINK( LicenseDialogImpl, EndReachedHdl, LicenseView *, EMPTYARG )
{
    m_acceptButton.Enable();
    m_acceptButton.GrabFocus();
    m_fiArrow1.Show(false);
    m_fiArrow2.Show(true);
    m_bLicenseRead = true;
    return 0;
}

//=================================================================================




LicenseDialog::LicenseDialog( Sequence<Any> const& args,
                          Reference<XComponentContext> const& xComponentContext)
    : m_xComponentContext(xComponentContext)
{
    comphelper::unwrapArgs( args, m_parent, m_sLicenseText );
}

// XExecutableDialog
//______________________________________________________________________________
void LicenseDialog::setTitle( OUString const & ) throw (RuntimeException)
{

}

//______________________________________________________________________________
sal_Int16 LicenseDialog::execute() throw (RuntimeException)
{
    //if (! dp_gui::DialogImpl::s_dialog.is())
    //{
    //
    //}

    const ::vos::OGuard guard( Application::GetSolarMutex() );
    std::auto_ptr<LicenseDialogImpl> dlg(new LicenseDialogImpl(
        VCLUnoHelper::GetWindow(m_parent), m_xComponentContext, m_sLicenseText));

    //if (m_initialTitle.getLength() > 0) {
//          dialog->SetText( m_initialTitle );
//          m_initialTitle = OUString();
//      }

    return dlg->Execute();
}

// XJobExecutor
//______________________________________________________________________________
//void LicenseDialog::trigger( OUString const & event ) throw (RuntimeException)
//{
//    execute();
//}

} // namespace dp_gui

