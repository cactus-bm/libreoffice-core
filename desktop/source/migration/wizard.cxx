/*************************************************************************
 *
 *  $RCSfile: wizard.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-15 15:49:35 $
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

#include <migration.hxx>
#include "wizard.hxx"
#include "wizard.hrc"
#include "pages.hxx"
#include "../app/desktopresid.hxx"
#include "app.hxx"

#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <rtl/string.hxx>
#include <rtl/strbuf.hxx>

#include <comphelper/processfactory.hxx>
#include <tools/date.hxx>
#include <tools/time.hxx>
#include <tools/datetime.hxx>
#include <osl/file.hxx>
#include <osl/time.h>
#include <unotools/bootstrap.hxx>
#include <vcl/msgbox.hxx>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <com/sun/star/lang/XComponent.hpp>

using namespace svt;
using namespace rtl;
using namespace osl;
using namespace utl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;

namespace desktop
{

const FirstStartWizard::WizardState FirstStartWizard::STATE_WELCOME = 0;
const FirstStartWizard::WizardState FirstStartWizard::STATE_LICENSE = 1;
const FirstStartWizard::WizardState FirstStartWizard::STATE_MIGRATION = 2;
const FirstStartWizard::WizardState FirstStartWizard::STATE_USER = 3;
const FirstStartWizard::WizardState FirstStartWizard::STATE_REGISTRATION = 4;

// WZB_NEXT|WZB_PREVIOUS|WZB_FINISH|WZB_CANCEL|WZB_HELP

FirstStartWizard::FirstStartWizard(Window* pParent)
    :RoadmapWizard( pParent, DesktopResId(DLG_FIRSTSTART_WIZARD),
        WZB_NEXT|WZB_PREVIOUS|WZB_FINISH|WZB_CANCEL|WZB_HELP, DesktopResId(STR_FIRSTSTART_TITLE))
    ,m_aDefaultPath(0)
    ,m_aMigrationPath(0)
    ,m_bDone(sal_False)
    ,m_bAccepted(sal_False)
{
    // ---
    // FreeResource();


    enableState(STATE_USER, sal_False);
    enableState(STATE_REGISTRATION, sal_False);

    ShowButtonFixedLine(sal_True);
    Size aTPSize(TP_WIDTH, TP_HEIGHT);
    SetPageSizePixel(LogicToPixel(aTPSize, MAP_APPFONT));

    //set help id
    m_pPrevPage->SetHelpId(HID_FIRSTSTART_PREV);
    m_pNextPage->SetHelpId(HID_FIRSTSTART_NEXT);
    m_pCancel->SetHelpId(HID_FIRSTSTART_CANCEL);
    m_pFinish->SetHelpId(HID_FIRSTSTART_FINISH);
    m_pHelp->SetUniqueId(UID_FIRSTSTART_HELP);

    // save button lables
    m_sNext = m_pNextPage->GetText();
    m_sCancel = m_pCancel->GetText();

    // save cancel click handler
    m_lnkCancel = m_pCancel->GetClickHdl();

    // prepare initial state and
    // check whether migration should be offered
    // and disable the migration tabpage if no
    // migration is needed
    if(Migration::checkMigration())
    {
        declarePath(m_aDefaultPath,
            STATE_WELCOME,
            STATE_LICENSE,
            STATE_MIGRATION,
            STATE_REGISTRATION,
            WZS_INVALID_STATE
            );
            enableState(STATE_MIGRATION, sal_False);
    }
    else
    {
        declarePath(m_aDefaultPath,
            STATE_WELCOME,
            STATE_LICENSE,
            STATE_USER,
            STATE_REGISTRATION,
            WZS_INVALID_STATE
            );
    }
    enableState(STATE_USER, sal_False);
    enableState(STATE_REGISTRATION, sal_False);

    activatePath(m_aDefaultPath, sal_True);

    enterState(STATE_WELCOME);
    ActivatePage();

    // set text of finish putton:
    m_pFinish->SetText(String(DesktopResId(STR_FINISH)));
    // disable "finish button"
    enableButtons(WZB_FINISH, sal_False);

}

void FirstStartWizard::enterState(WizardState _nState)
{
    RoadmapWizard::enterState(_nState);
    // default state
    // all on
    enableButtons(0xff, sal_True);
    // finish off
    enableButtons(WZB_FINISH, sal_False);
    // default text
    m_pCancel->SetText(m_sCancel);
    m_pCancel->SetClickHdl(m_lnkCancel);
    m_pNextPage->SetText(m_sNext);

    // specialized state
    switch (_nState)
    {
    case STATE_WELCOME:
        enableButtons(WZB_PREVIOUS, sal_False);
        break;
    case STATE_LICENSE:
        m_pCancel->SetText(String(DesktopResId(STR_LICENSE_DECLINE)));
        m_pNextPage->SetText(String(DesktopResId(STR_LICENSE_ACCEPT)));
        enableButtons(WZB_NEXT, sal_False);
        // attach warning dialog to cancel/decline button
        m_pCancel->SetClickHdl( LINK(this, FirstStartWizard, DeclineHdl) );
        break;
    case STATE_REGISTRATION:
        enableButtons(WZB_NEXT, sal_False);
        enableButtons(WZB_FINISH, sal_True);
        break;
    }

}

IMPL_LINK( FirstStartWizard, DeclineHdl, PushButton *, arg )
{
    QueryBox aBox(this, DesktopResId(QB_ASK_DECLINE));
    sal_Int32 ret = aBox.Execute();
    if ( ret == BUTTON_OK || ret == BUTTON_YES)
    {
        Close();
        return sal_False;
    }
    else
        return sal_True;
}


TabPage* FirstStartWizard::createPage(WizardState _nState)
{
    TabPage *pTabPage = 0;
    switch (_nState)
    {
    case STATE_WELCOME:
        pTabPage = new WelcomePage(this, DesktopResId(TP_WELCOME));
        break;
    case STATE_LICENSE:
        pTabPage = new LicensePage(this, DesktopResId(TP_LICENSE));
        break;
    case STATE_MIGRATION:
        pTabPage = new MigrationPage(this, DesktopResId(TP_MIGRATION));
        break;
    case STATE_USER:
        pTabPage = new UserPage(this, DesktopResId(TP_USER));
        break;
    case STATE_REGISTRATION:
        pTabPage = new RegistrationPage(this, DesktopResId(TP_REGISTRATION));
        break;
    }
    pTabPage->Show();

    return pTabPage;
}

String FirstStartWizard::getStateDisplayName(WizardState _nState)
{
    String sName;
    switch(_nState)
    {
    case STATE_WELCOME:
        sName = String(DesktopResId(STR_STATE_WELCOME));
        break;
    case STATE_LICENSE:
        sName = String(DesktopResId(STR_STATE_LICENSE));
        break;
    case STATE_MIGRATION:
        sName = String(DesktopResId(STR_STATE_MIGRATION));
        break;
    case STATE_USER:
        sName = String(DesktopResId(STR_STATE_USER));
        break;
    case STATE_REGISTRATION:
        sName = String(DesktopResId(STR_STATE_REGISTRATION));
        break;
    }
    return sName;
}

sal_Bool FirstStartWizard::prepareLeaveCurrentState( CommitPageReason _eReason )
{
    if (_eReason == eTravelForward && getCurrentState() == STATE_LICENSE && !m_bAccepted)
    {
        // the license acceptance is handled here, because it needs to change the state
        // of the roadmap wizard which the page implementation does not know.
        if (Migration::checkMigration())
            enableState(FirstStartWizard::STATE_MIGRATION, sal_True);
        enableState(FirstStartWizard::STATE_USER, sal_True);
        enableState(FirstStartWizard::STATE_REGISTRATION, sal_True);
        m_bAccepted = sal_True;
        storeAcceptDate();
    }
    return svt::RoadmapWizard::prepareLeaveCurrentState(_eReason);
}

sal_Bool FirstStartWizard::leaveState(WizardState _nState )
{

    return sal_True;
}

sal_Bool FirstStartWizard::onFinish(sal_Int32 _nResult)
{
    // return sal_True;
    if (svt::RoadmapWizard::onFinish(_nResult))
    {
        enableQuickstart();
        disableWizard();
        return sal_True;
    }
    else
        return sal_False;
}

short FirstStartWizard::Execute()
{
    if (isFirstStart() || !isLicenseAccepted())
        return svt::RoadmapWizard::Execute();
    else
        return sal_True;
}


static DateTime _oslDateTimeToDateTime(const oslDateTime& aDateTime)
{
    return DateTime(
        Date(aDateTime.Day, aDateTime.Month, aDateTime.Year),
        Time(aDateTime.Hours, aDateTime.Minutes, aDateTime.Seconds));
}

static OUString _makeDateTimeString (const DateTime& aDateTime, sal_Bool bUTC = sal_False)
{
    OStringBuffer aDateTimeString;
    aDateTimeString.append((sal_Int32)aDateTime.GetYear());
    aDateTimeString.append("-");
    if (aDateTime.GetMonth()<10) aDateTimeString.append("0");
    aDateTimeString.append((sal_Int32)aDateTime.GetMonth());
    aDateTimeString.append("-");
    if (aDateTime.GetDay()<10) aDateTimeString.append("0");
    aDateTimeString.append((sal_Int32)aDateTime.GetDay());
    aDateTimeString.append("T");
    if (aDateTime.GetHour()<10) aDateTimeString.append("0");
    aDateTimeString.append((sal_Int32)aDateTime.GetHour());
    aDateTimeString.append(":");
    if (aDateTime.GetMin()<10) aDateTimeString.append("0");
    aDateTimeString.append((sal_Int32)aDateTime.GetMin());
    aDateTimeString.append(":");
    if (aDateTime.GetSec()<10) aDateTimeString.append("0");
    aDateTimeString.append((sal_Int32)aDateTime.GetSec());
    if (bUTC) aDateTimeString.append("Z");

    return OStringToOUString(aDateTimeString.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US);
}

static sal_Bool _parseDateTime(const OUString& aString, DateTime& aDateTime)
{
    // take apart a canonical literal xsd:dateTime string
    //CCYY-MM-DDThh:mm:ss(Z)

    OUString aDateTimeString = aString.trim();

    // check length
    if (aDateTimeString.getLength() < 19 || aDateTimeString.getLength() > 20)
        return sal_False;

    sal_Int32 nDateLength = 10;
    sal_Int32 nTimeLength = 8;

    OUString aDateTimeSep = OUString::createFromAscii("T");
    OUString aDateSep = OUString::createFromAscii("-");
    OUString aTimeSep = OUString::createFromAscii(":");
    OUString aUTCString = OUString::createFromAscii("Z");

    OUString aDateString = aDateTimeString.copy(0, nDateLength);
    OUString aTimeString = aDateTimeString.copy(nDateLength+1, nTimeLength);

    sal_Int32 nIndex = 0;
    sal_Int32 nYear = aDateString.getToken(0, '-', nIndex).toInt32();
    sal_Int32 nMonth = aDateString.getToken(0, '-', nIndex).toInt32();
    sal_Int32 nDay = aDateString.getToken(0, '-', nIndex).toInt32();
    nIndex = 0;
    sal_Int32 nHour = aTimeString.getToken(0, ':', nIndex).toInt32();
    sal_Int32 nMinute = aTimeString.getToken(0, ':', nIndex).toInt32();
    sal_Int32 nSecond = aTimeString.getToken(0, ':', nIndex).toInt32();

    Date tmpDate((USHORT)nDay, (USHORT)nMonth, (USHORT)nYear);
    Time tmpTime(nHour, nMinute, nSecond);
    DateTime tmpDateTime(tmpDate, tmpTime);
    if (aString.indexOf(aUTCString) < 0)
        tmpDateTime.ConvertToUTC();

    aDateTime = tmpDateTime;
    return sal_True;
}

static OUString _getCurrentDateString()
{
    OUString aString;
    return _makeDateTimeString(DateTime());
}


static const OUString sConfigSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider");
static const OUString sAccessSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationUpdateAccess");
static const OUString sReadSrvc   = OUString::createFromAscii("com.sun.star.configuration.ConfigurationAccess");

void FirstStartWizard::storeAcceptDate()
{

    try {
        Reference < XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
        // get configuration provider
        Reference< XMultiServiceFactory > theConfigProvider = Reference< XMultiServiceFactory >(
        xFactory->createInstance(sConfigSrvc), UNO_QUERY_THROW);
        Sequence< Any > theArgs(1);
        NamedValue v(OUString::createFromAscii("NodePath"),
            makeAny(OUString::createFromAscii("org.openoffice.Setup/Office")));
        theArgs[0] <<= v;
        Reference< XPropertySet > pset = Reference< XPropertySet >(
            theConfigProvider->createInstanceWithArguments(sAccessSrvc, theArgs), UNO_QUERY_THROW);
        Any result = pset->getPropertyValue(OUString::createFromAscii("LicenseAcceptDate"));

        OUString aAcceptDate = _getCurrentDateString();
        pset->setPropertyValue(OUString::createFromAscii("LicenseAcceptDate"), makeAny(aAcceptDate));
        Reference< XChangesBatch >(pset, UNO_QUERY_THROW)->commitChanges();
    } catch (RuntimeException)
    {
    }

}

void FirstStartWizard::disableWizard()
{

    try {
        Reference < XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
        // get configuration provider
        Reference< XMultiServiceFactory > theConfigProvider = Reference< XMultiServiceFactory >(
        xFactory->createInstance(sConfigSrvc), UNO_QUERY_THROW);
        Sequence< Any > theArgs(1);
        NamedValue v(OUString::createFromAscii("NodePath"),
            makeAny(OUString::createFromAscii("org.openoffice.Setup/Office")));
        theArgs[0] <<= v;
        Reference< XPropertySet > pset = Reference< XPropertySet >(
            theConfigProvider->createInstanceWithArguments(sAccessSrvc, theArgs), UNO_QUERY_THROW);
        pset->setPropertyValue(OUString::createFromAscii("FirstStartWizardCompleted"), makeAny(sal_True));
        Reference< XChangesBatch >(pset, UNO_QUERY_THROW)->commitChanges();
    } catch (RuntimeException)
    {
    }

}


void FirstStartWizard::enableQuickstart()
{
    sal_Bool bQuickstart( sal_True );
    sal_Bool bAutostart( sal_True );
    Sequence< Any > aSeq( 2 );
    aSeq[0] <<= bQuickstart;
    aSeq[1] <<= bAutostart;

    Reference < XInitialization > xQuickstart( ::comphelper::getProcessServiceFactory()->createInstance(
        OUString::createFromAscii( "com.sun.star.office.Quickstart" )),UNO_QUERY );
    if ( xQuickstart.is() )
        xQuickstart->initialize( aSeq );

}

sal_Bool FirstStartWizard::isFirstStart()
{
    try {
        Reference < XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
        // get configuration provider
        Reference< XMultiServiceFactory > theConfigProvider = Reference< XMultiServiceFactory >(
        xFactory->createInstance(sConfigSrvc), UNO_QUERY_THROW);
        Sequence< Any > theArgs(1);
        NamedValue v(OUString::createFromAscii("NodePath"),
            makeAny(OUString::createFromAscii("org.openoffice.Setup/Office")));
        theArgs[0] <<= v;
        Reference< XPropertySet > pset = Reference< XPropertySet >(
            theConfigProvider->createInstanceWithArguments(sAccessSrvc, theArgs), UNO_QUERY_THROW);
        Any result = pset->getPropertyValue(OUString::createFromAscii("FirstStartWizardCompleted"));
        sal_Bool bCompleted = sal_False;
        if ((result >>= bCompleted) && bCompleted)
            return sal_False;  // wizard was already completed
        else
            return sal_True;
    } catch (RuntimeException)
    {
        return sal_True;
    }
}

sal_Bool FirstStartWizard::isLicenseAccepted()
{
    try
    {
        Reference < XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
        // get configuration provider
        Reference< XMultiServiceFactory > theConfigProvider = Reference< XMultiServiceFactory >(
        xFactory->createInstance(sConfigSrvc), UNO_QUERY_THROW);
        Sequence< Any > theArgs(1);
        NamedValue v(OUString::createFromAscii("NodePath"),
            makeAny(OUString::createFromAscii("org.openoffice.Setup/Office")));
        theArgs[0] <<= v;
        Reference< XPropertySet > pset = Reference< XPropertySet >(
            theConfigProvider->createInstanceWithArguments(sAccessSrvc, theArgs), UNO_QUERY_THROW);
        Any result = pset->getPropertyValue(OUString::createFromAscii("LicenseAcceptDate"));

        OUString aAcceptDate;
        if (result >>= aAcceptDate)
        {
            // compare to date of license file
            OUString aLicenseURL = FirstStartWizard::getLicensePath();
            DirectoryItem aDirItem;
            if (DirectoryItem::get(aLicenseURL, aDirItem) != FileBase::E_None)
                return sal_False;
            FileStatus aStatus(FileStatusMask_All);
            if (aDirItem.getFileStatus(aStatus) != FileBase::E_None)
                return sal_False;
            TimeValue aTimeVal = aStatus.getModifyTime();
            oslDateTime aDateTimeVal;
            if (!osl_getDateTimeFromTimeValue(&aTimeVal, &aDateTimeVal))
                return sal_False;

            // compare dates
            DateTime aLicenseDateTime = _oslDateTimeToDateTime(aDateTimeVal);
            DateTime aAcceptDateTime;
            if (!_parseDateTime(aAcceptDate, aAcceptDateTime))
                return sal_False;

            if ( aAcceptDateTime > aLicenseDateTime )
                return sal_True;
        }
        return sal_False;
    } catch (RuntimeException)
    {
        return sal_False;
    }
}

OUString FirstStartWizard::getLicensePath()
{
    // license file name
    static const char *szLicensePath = "/share/readme";
    static const char *szUNXLicenseName = "/LICENSE";
    static const char *szUNXLicenseExt = "";
    static const char *szWNTLicenseName = "/license";
    static const char *szWNTLicenseExt = ".txt";
    static OUString aLicensePath;

    if (aLicensePath.getLength() > 0)
        return aLicensePath;

    OUString aBaseInstallPath;
    Bootstrap::PathStatus aBaseLocateResult =
        Bootstrap::locateBaseInstallation(aBaseInstallPath);
    if (aBaseLocateResult != Bootstrap::PATH_EXISTS)
    {
        // yuck! no license :/
    }

    // determine the filename of the license to show
    OUString  aLangString;
    ::com::sun::star::lang::Locale aLocale;
    OString aMgrName = OString("dkt") + OString::valueOf((sal_Int32)SUPD, 10);
    AllSettings aSettings(Application::GetSettings());
    aLocale = aSettings.GetUILocale();
    ResMgr* pResMgr = ResMgr::SearchCreateResMgr(aMgrName, aLocale);

    aLangString = aLocale.Language;
    if ( aLocale.Country.getLength() != 0 )
    {
        aLangString += OUString::createFromAscii("-");
        aLangString += aLocale.Country;
        if ( aLocale.Variant.getLength() != 0 )
        {
            aLangString += OUString::createFromAscii("-");
            aLangString += aLocale.Variant;
        }
    }
#ifdef WNT
    aLicensePath =
        aBaseInstallPath + OUString::createFromAscii(szLicensePath)
        + OUString::createFromAscii(szWNTLicenseName)
        + OUString::createFromAscii("_")
        + aLangString
        + OUString::createFromAscii(szWNTLicenseExt);
#else
    aLicensePath =
        aBaseInstallPath + OUString::createFromAscii(szLicensePath)
        + OUString::createFromAscii(szUNXLicenseName)
        + OUString::createFromAscii("_")
        + aLangString
        + OUString::createFromAscii(szUNXLicenseExt);
#endif
    return aLicensePath;
}
}
