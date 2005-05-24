/*************************************************************************
 *
 *  $RCSfile: pages.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-05-24 14:01:38 $
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

#include "pages.hxx"
#include "wizard.hrc"
#include "wizard.hxx"
#include "migration.hxx"
#include <vcl/msgbox.hxx>
#include <vcl/mnemonic.hxx>
#include <app.hxx>
#include <rtl/ustring.hxx>
#include <osl/file.hxx>
#include <unotools/bootstrap.hxx>
#include <svtools/regoptions.hxx>
#include <svtools/useroptions.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/system/XSystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/beans/XMaterialHolder.hpp>
#include <com/sun/star/beans/NamedValue.hpp>

#ifndef _COMPHELPER_CONFIGURATIONHELPER_HXX_
#include <comphelper/configurationhelper.hxx>
#endif

namespace desktop
{

using namespace rtl;
using namespace osl;
using namespace utl;
using namespace svt;
using namespace com::sun::star::system;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;

static void _setBold(FixedText& ft)
{
    Font f = ft.GetControlFont();
    f.SetWeight(WEIGHT_BOLD);
    ft.SetControlFont(f);
}

WelcomePage::WelcomePage( svt::OWizardMachine* parent, const ResId& resid)
    : OWizardPage(parent, resid)
    , m_ftHead(this, WizardResId(FT_WELCOME_HEADER))
    , m_ftBody(this, WizardResId(FT_WELCOME_BODY))
    , m_pParent(parent)
{
    FreeResource();

    _setBold(m_ftHead);

    // we need to choose the welcome text that is diplayed
    // choices are the default text, default text+migradtion,
    // OEM and extended OEM
    switch (checkOEM())
    {
    case OEM_NONE:
        // check for migration
        if (Migration::checkMigration())
        {
            String aText(WizardResId(STR_WELCOME_MIGRATION));
            // replace %OLDPRODUCT with found version name
            aText.SearchAndReplaceAll( UniString::CreateFromAscii("%OLD_VERSION"), Migration::getOldVersionName());
            m_ftBody.SetText( aText );
        }
        if (checkEval()) {
            String aText(WizardResId(STR_WELCOME_EVAL));
            aText.SearchAndReplaceAll( UniString::CreateFromAscii("%EVALDAYS"), UniString::CreateFromAscii("90"));
            m_ftBody.SetText( aText );
        }
        break;
    case OEM_NORMAL:
        m_ftBody.SetText(String(WizardResId(STR_WELCOME_OEM)));
        break;
    case OEM_EXTENDED:
        m_ftBody.SetText(String(WizardResId(STR_WELCOME_OEM_EXT)));
        break;
    }

}

WelcomePage::OEMType WelcomePage::checkOEM()
{
    return OEM_NONE;
}

bool WelcomePage::checkEval()
{
    bool bResult = false;
    Reference< XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
    Reference< XMaterialHolder > xHolder(xFactory->createInstance(
        OUString::createFromAscii("com.sun.star.tab.tabreg")), UNO_QUERY);
    if (xHolder.is()) {
        Any aData = xHolder->getMaterial();
        Sequence < NamedValue > aSeq;
        if (aData >>= aSeq) bResult = true;
    }
    return bResult;
}

void WelcomePage::ActivatePage()
{
    OWizardPage::ActivatePage();
    // this page has no controls, so forwarding to default
    // button (next) won't work if we grap focus
    // GrabFocus();
}

LicensePage::LicensePage( svt::OWizardMachine* parent, const ResId& resid)
    : OWizardPage(parent, resid)
    , m_pbDown(this, WizardResId(PB_LICENSE_DOWN))
    , m_ftHead(this, WizardResId(FT_LICENSE_HEADER))
    , m_ftBody1(this, WizardResId(FT_LICENSE_BODY_1))
    , m_ftBody1Txt(this, WizardResId(FT_LICENSE_BODY_1_TXT))
    , m_ftBody2(this, WizardResId(FT_LICENSE_BODY_2))
    , m_ftBody2Txt(this, WizardResId(FT_LICENSE_BODY_2_TXT))
    , m_mlLicense(this, WizardResId(ML_LICENSE))
    , m_bLicenseRead(sal_False)
    , m_pParent(parent)
{
    FreeResource();

    _setBold(m_ftHead);

    m_mlLicense.SetEndReachedHdl( LINK(this, LicensePage, EndReachedHdl) );
    m_mlLicense.SetScrolledHdl( LINK(this, LicensePage, ScrolledHdl) );
    m_pbDown.SetClickHdl( LINK(this, LicensePage, PageDownHdl) );

    // We want a automatic repeating page down button
    WinBits aStyle = m_pbDown.GetStyle();
    aStyle |= WB_REPEAT;
    m_pbDown.SetStyle( aStyle );

    // replace %PAGEDOWN in text2 with button text
    String aText = m_ftBody1Txt.GetText();
    aText.SearchAndReplaceAll( UniString::CreateFromAscii("%PAGEDOWN"),
        MnemonicGenerator::EraseAllMnemonicChars(m_pbDown.GetText()));

    m_ftBody1Txt.SetText( aText );

    OUString aLicensePath = FirstStartWizard::getLicensePath();
    // load license text
    File aLicenseFile(aLicensePath);
    if ( aLicenseFile.open(OpenFlag_Read) == FileBase::E_None)
    {
        DirectoryItem d;
        DirectoryItem::get(aLicensePath, d);
        FileStatus fs(FileStatusMask_FileSize);
        d.getFileStatus(fs);
        sal_uInt64 nBytesRead = 0;
        sal_uInt64 nPosition = 0;
        sal_uInt32 nBytes = (sal_uInt32)fs.getFileSize();
        sal_Char *pBuffer = new sal_Char[nBytes];
        // FileBase RC r = FileBase::E_None;
        while (aLicenseFile.read(pBuffer+nPosition, nBytes-nPosition, nBytesRead) == FileBase::E_None
            && nPosition + nBytesRead < nBytes)
        {
            nPosition += nBytesRead;
        }
        OUString aLicenseString(pBuffer, nBytes, RTL_TEXTENCODING_UTF8,
                OSTRING_TO_OUSTRING_CVTFLAGS | RTL_TEXTTOUNICODE_FLAGS_GLOBAL_SIGNATURE);
        delete[] pBuffer;
        m_mlLicense.SetText(aLicenseString);

    }
}

void LicensePage::ActivatePage()
{
    OWizardPage::ActivatePage();
    m_bLicenseRead = m_mlLicense.IsEndReached();
    m_pbDown.GrabFocus();
    implCheckNextButton();
}

sal_Bool LicensePage::determineNextButtonState()
{
    if (m_mlLicense.IsEndReached())
        m_pbDown.Disable();
    else
        m_pbDown.Enable();

    return m_bLicenseRead;
}

IMPL_LINK( LicensePage, PageDownHdl, PushButton *, EMPTYARG )
{
    m_mlLicense.ScrollDown( SCROLL_PAGEDOWN );
    return 0;
}

IMPL_LINK( LicensePage, EndReachedHdl, LicenseView *, EMPTYARG )
{
    m_bLicenseRead = TRUE;
    implCheckNextButton();
    return 0;
}

IMPL_LINK( LicensePage, ScrolledHdl, LicenseView *, EMPTYARG )
{
    implCheckNextButton();
    return 0;
}


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

void LicenseView::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
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



// -------------------------------------------------------------------

MigrationPage::MigrationPage( svt::OWizardMachine* parent, const ResId& resid)
    : OWizardPage(parent, resid)
    , m_ftHead(this, WizardResId(FT_MIGRATION_HEADER))
    , m_ftBody(this, WizardResId(FT_MIGRATION_BODY))
    , m_cbMigration(this, WizardResId(CB_MIGRATION))
    , m_bMigrationDone(sal_False)
{
    FreeResource();
    _setBold(m_ftHead);

    // replace %OLDPRODUCT with found version name
    String aText = m_ftBody.GetText();
    aText.SearchAndReplaceAll( UniString::CreateFromAscii("%OLDPRODUCT"), Migration::getOldVersionName());
    m_ftBody.SetText( aText );
}

sal_Bool MigrationPage::commitPage(COMMIT_REASON _eReason)
{
    if (_eReason == eTravelForward && m_cbMigration.IsChecked() && !m_bMigrationDone)
    {
        EnterWait();
        Migration::doMigration();
        LeaveWait();
        m_bMigrationDone = sal_True;
    }
    else
        Migration::cancelMigration();
    return sal_True;
}

void MigrationPage::ActivatePage()
{
    OWizardPage::ActivatePage();
    GrabFocus();
}

UserPage::UserPage( svt::OWizardMachine* parent, const ResId& resid)
    : OWizardPage(parent, resid)
    , m_ftHead(this, WizardResId(FT_USER_HEADER))
    , m_ftBody(this, WizardResId(FT_USER_BODY))
    , m_ftFirst(this, WizardResId(FT_USER_FIRST))
    , m_edFirst(this, WizardResId(ED_USER_FIRST))
    , m_ftLast(this, WizardResId(FT_USER_LAST))
    , m_edLast(this, WizardResId(ED_USER_LAST))
    , m_ftInitials(this, WizardResId(FT_USER_INITIALS))
    , m_edInitials(this, WizardResId(ED_USER_INITIALS))
    , m_ftFather(this, WizardResId(FT_USER_FATHER))
    , m_edFather(this, WizardResId(ED_USER_FATHER))
    , m_lang(Application::GetSettings().GetUILanguage())
{
    FreeResource();
    _setBold(m_ftHead);

    // check whether this is a russian version. otherwise
    // we'll hide the 'Fathers name' field
    SvtUserOptions aUserOpt;
    m_edFirst.SetText(aUserOpt.GetFirstName());
    m_edLast.SetText(aUserOpt.GetLastName());
    m_edInitials.SetText(aUserOpt.GetID());
    if (m_lang == LANGUAGE_RUSSIAN)
    {
        m_ftFather.Show();
        m_edFather.Show();
        m_edFather.SetText(aUserOpt.GetFathersName());
    }
}

sal_Bool UserPage::commitPage(COMMIT_REASON _eReason)
{
    SvtUserOptions aUserOpt;
    aUserOpt.SetFirstName(m_edFirst.GetText());
    aUserOpt.SetLastName(m_edLast.GetText());
    aUserOpt.SetID( m_edInitials.GetText());
    if (m_lang == LANGUAGE_RUSSIAN)
        aUserOpt.SetFathersName(m_edFather.GetText());

    return sal_True;
}

void UserPage::ActivatePage()
{
    OWizardPage::ActivatePage();
    GrabFocus();
}

RegistrationPage::RegistrationPage( svt::OWizardMachine* parent, const ResId& resid)
    : OWizardPage(parent, resid)
    , m_ftHeader(this, WizardResId(FT_REGISTRATION_HEADER))
    , m_fiImage(this, WizardResId(IMG_REGISTRATION))
    , m_ftBody(this, WizardResId(FT_REGISTRATION_BODY))
    , m_rbNow(this, WizardResId(RB_REGISTRATION_NOW))
    , m_rbLater(this, WizardResId(RB_REGISTRATION_LATER))
    , m_rbNever(this, WizardResId(RB_REGISTRATION_NEVER))
    , m_rbReg(this, WizardResId(RB_REGISTRATION_REG))
    , m_flSeparator(this, WizardResId(FL_REGISTRATION))
    , m_ftEnd(this, WizardResId(FT_REGISTRATION_END))
    , m_bNeverVisible( sal_True )
{
    FreeResource();
    _setBold(m_ftHeader);

    impl_retrieveConfigurationData();
    updateButtonStates();
}

sal_Bool RegistrationPage::determineNextButtonState()
{
    return sal_False;
}

void RegistrationPage::ActivatePage()
{
    OWizardPage::ActivatePage();
    GrabFocus();
}

void RegistrationPage::impl_retrieveConfigurationData()
{
    static ::rtl::OUString PACKAGE = ::rtl::OUString::createFromAscii("org.openoffice.FirstStartWizard");
    static ::rtl::OUString PATH    = ::rtl::OUString::createFromAscii("TabPages/Registration/RegistrationOptions/NeverButton");
    static ::rtl::OUString KEY     = ::rtl::OUString::createFromAscii("Visible");

    ::com::sun::star::uno::Any aValue;
    try
    {
        aValue = ::comphelper::ConfigurationHelper::readDirectKey(
                                ::comphelper::getProcessServiceFactory(),
                                PACKAGE,
                                PATH,
                                KEY,
                                ::comphelper::ConfigurationHelper::E_READONLY);
    }
    catch(const ::com::sun::star::uno::Exception&)
        { aValue.clear(); }

    aValue >>= m_bNeverVisible;
}

void RegistrationPage::updateButtonStates()
{
    if ( !m_bNeverVisible )
    {
        ::Point aNeverPos = m_rbNever.GetPosPixel();

        m_rbReg.SetPosPixel( aNeverPos );
        m_rbNever.Show( FALSE );
    }
}

sal_Bool RegistrationPage::commitPage(COMMIT_REASON _eReason)
{
    if ( _eReason == eFinish )
    {
        RegOptions aOptions;
        if ( m_rbNow.IsChecked())
        {
            sal_Bool bSuccess = sal_False;
            try
            {
                // create the Desktop component which can load components
                Reference < XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
                Reference< XSystemShellExecute > xSystemShell(
                    xFactory->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.system.SystemShellExecute" ) ),
                    UNO_QUERY_THROW
                );

                // access the configuration to retrieve the URL we shall use for registration
                ::rtl::OUString sRegistrationURL( aOptions.getRegistrationURL( ) );
                OSL_ENSURE( sRegistrationURL.getLength(), "OProductRegistration::doOnlineRegistration: invalid URL found!" );
                if ( xSystemShell.is() && sRegistrationURL.getLength() )
                {
                    xSystemShell->execute( sRegistrationURL, ::rtl::OUString(), SystemShellExecuteFlags::DEFAULTS );
                    bSuccess = sal_True;
                }
            }
            catch( const Exception& )
            {
            }
            if ( !bSuccess )
            {
                ErrorBox aRegistrationError( this, WizardResId( ERRBOX_REG_NOSYSBROWSER ) );
                aRegistrationError.Execute();
            }
        }
        else if (m_rbLater.IsChecked())
        {
            aOptions.activateReminder(7);
            // avtivate a reminder job...
        }
        // aOptions.markSessionDone();
    }
    return sal_True;
}

} // namespace desktop


