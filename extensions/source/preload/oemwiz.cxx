/*************************************************************************
 *
 *  $RCSfile: oemwiz.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: os $ $Date: 2001-12-17 08:33:04 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _EXTENSIONS_PRELOAD_OEMWIZ_HXX_
#include "oemwiz.hxx"
#endif
#ifndef _EXTENSIONS_COMPONENT_MODULE_HXX_
#include "componentmodule.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef SVTOOLS_URIHELPER_HXX
#include <svtools/urihelper.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif
#ifndef _SVX_OPTGENRL_HXX
#include <svx/optgenrl.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_USEROPTIONS_HXX
#include <svtools/useroptions.hxx>
#endif
#ifndef _EXTENSIONS_PRELOAD_PRELOAD_HRC_
#include "preload.hrc"
#endif
#ifndef _SVX_ADRITEM_HXX
#include <svx/adritem.hxx>
#endif
#ifndef _SFXSIDS_HRC
#include <sfx2/sfxsids.hrc>
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
//.........................................................................
namespace preload
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
//.........................................................................

    struct OEMPreloadDialog_Impl
    {
        SfxItemSet*     pSet;
        SvtUserOptions  aOptions;
        TabPage* pWelcomePage;
        TabPage* pLicensePage;
        TabPage* pUserDataPage;

        OEMPreloadDialog_Impl(Window* pDialog);
        ~OEMPreloadDialog_Impl()
        {
            delete pWelcomePage;
            delete pLicensePage;
            delete pUserDataPage;
            delete pSet;
        }
        void WriteUserData();
    };
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    OEMPreloadDialog_Impl::OEMPreloadDialog_Impl(Window* pDialog)
        {
            SfxItemPool& rPool = SFX_APP()->GetPool();
            pSet = new SfxItemSet(rPool, SID_ATTR_ADDRESS, SID_ATTR_ADDRESS);
            String sTmp;
            SvxAddressItem aAddressItem(sTmp,
                aOptions.GetID(), aOptions.GetFirstName(), aOptions.GetLastName(), SID_ATTR_ADDRESS);

            aAddressItem.SetToken(POS_COMPANY, aOptions.GetCompany() );
            aAddressItem.SetToken(POS_STREET, aOptions.GetStreet()) ;
            aAddressItem.SetToken(POS_CITY, aOptions.GetCity() );
            aAddressItem.SetToken(POS_STATE, aOptions.GetState() );
            aAddressItem.SetToken(POS_PLZ, aOptions.GetZip() );
            aAddressItem.SetToken(POS_COUNTRY, aOptions.GetCountry() );
            aAddressItem.SetToken(POS_POSITION, aOptions.GetPosition() );
            aAddressItem.SetToken(POS_TITLE, aOptions.GetTitle() );
            aAddressItem.SetToken(POS_TEL_PRIVATE, aOptions.GetTelephoneHome() );
            aAddressItem.SetToken(POS_TEL_COMPANY, aOptions.GetTelephoneWork() );
            aAddressItem.SetToken(POS_FAX, aOptions.GetFax() );
            aAddressItem.SetToken(POS_EMAIL, aOptions.GetEmail() );
            pSet->Put(aAddressItem);
            pWelcomePage = new OEMWelcomeTabPage(pDialog);
            pLicensePage = new OEMLicenseTabPage(pDialog);
            pUserDataPage = new SvxGeneralTabPage(pDialog, *pSet);
            ((SvxGeneralTabPage*)pUserDataPage)->Reset(*pSet);
        }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
        void OEMPreloadDialog_Impl::WriteUserData()
        {
            ((SvxGeneralTabPage*)pUserDataPage)->FillItemSet(*pSet);
            const SvxAddressItem& rAddressItem = (const SvxAddressItem&)pSet->Get(SID_ATTR_ADDRESS);
            aOptions.SetID(rAddressItem.GetShortName());
            aOptions.SetFirstName(rAddressItem.GetFirstName());
            aOptions.SetLastName(rAddressItem.GetName());

            aOptions.SetCompany(rAddressItem.GetToken(POS_COMPANY) );
            aOptions.SetStreet(rAddressItem.GetToken(POS_STREET)) ;
            aOptions.SetCity(rAddressItem.GetToken(POS_CITY ) );
            aOptions.SetState(rAddressItem.GetToken(POS_STATE) );
            aOptions.SetZip(rAddressItem.GetToken(POS_PLZ ) );
            aOptions.SetCountry(rAddressItem.GetToken(POS_COUNTRY) );
            aOptions.SetPosition(rAddressItem.GetToken(POS_POSITION ));
            aOptions.SetTitle(rAddressItem.GetToken(POS_TITLE ) );
            aOptions.SetTelephoneHome(rAddressItem.GetToken(POS_TEL_PRIVATE ) );
            aOptions.SetTelephoneWork(rAddressItem.GetToken(POS_TEL_COMPANY ) );
            aOptions.SetFax(rAddressItem.GetToken(POS_FAX ) );
            aOptions.SetEmail(rAddressItem.GetToken(POS_EMAIL ) );
        }

/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    //=====================================================================
    //= OEMPreloadDialog
    //=====================================================================
    //---------------------------------------------------------------------
    OEMPreloadDialog::OEMPreloadDialog( Window* _pParent,
            const Reference< XPropertySet >& _rxObjectModel, const Reference< XMultiServiceFactory >& _rxORB )
        :WizardDialog(_pParent, ModuleRes(RID_DLG_OEMWIZARD)/*, _rxObjectModel, _rxORB*/)
        ,aPrevPB(this,      ResId(PB_PREV   ))
        ,aNextPB(this,      ResId(PB_NEXT   ))
        ,aCancelPB(this,    ResId(PB_CANCEL ))
        ,aAcceptST(ResId(ST_ACCEPT))
        ,aFinishST(ResId(ST_FINISH))
        ,aLicense(ResId(ST_LICENSE_AGREEMENT))
        ,aUserData(ResId(ST_INSERT_USER_DATA))
        ,pImpl(new OEMPreloadDialog_Impl(this))
    {
          FreeResource();
          aDlgTitle = GetText();
          aPrevPB.Enable(FALSE);
          aNextST = aNextPB.GetText();
          aPrevPB.SetClickHdl(LINK(this, OEMPreloadDialog, NextPrevPageHdl));
          aNextPB.SetClickHdl(LINK(this, OEMPreloadDialog, NextPrevPageHdl));
          AddButton( &aPrevPB, WIZARDDIALOG_BUTTON_STDOFFSET_X );
          AddButton( &aNextPB, WIZARDDIALOG_BUTTON_STDOFFSET_X );
          AddButton( &aCancelPB, WIZARDDIALOG_BUTTON_STDOFFSET_X );

          SetPrevButton(&aPrevPB);
          SetNextButton(&aNextPB);

          AddPage( pImpl->pWelcomePage );
          AddPage( pImpl->pLicensePage );
          AddPage( pImpl->pUserDataPage );
          SetPage( OEM_WELCOME, pImpl->pWelcomePage );
          SetPage( OEM_LICENSE, pImpl->pLicensePage );
          SetPage( OEM_USERDATA, pImpl->pUserDataPage );
          ShowPage( OEM_WELCOME );
    }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    OEMPreloadDialog::~OEMPreloadDialog( )
    {
        delete pImpl;
    }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    IMPL_LINK(OEMPreloadDialog, NextPrevPageHdl, PushButton*, pButton)
    {
        if(pButton == &aPrevPB)
        {
            if(GetCurLevel())
                ShowPage(GetCurLevel() - 1);
        }
        else if(OEM_USERDATA > GetCurLevel())
            ShowPage(GetCurLevel() + 1);
        else
        {
            pImpl->WriteUserData();
            Finnish(RET_OK);
        }

        String sTitle(aDlgTitle);

        switch(GetCurLevel())
        {
            case OEM_WELCOME:
                aNextPB.SetText(aNextST);
            break;
            case OEM_LICENSE:
                sTitle += aLicense;
                aNextPB.SetText(aAcceptST);
                aCancelPB.GrabFocus();
            break;
            case OEM_USERDATA:
                sTitle += aUserData;
                aNextPB.SetText(aFinishST);
            break;
        }
        SetText(sTitle);
        aPrevPB.Enable(GetCurLevel() != OEM_WELCOME);
        return 0;
    }
/* -----------------------------14.11.2001 11:33------------------------------

 ---------------------------------------------------------------------------*/
    sal_Bool OEMPreloadDialog::LoadFromLocalFile(const String& rFileName, String& rContent)
    {
        SvtPathOptions aPathOpt;
        String sFileName = aPathOpt.GetModulePath();
        sFileName = URIHelper::SmartRelToAbs(sFileName);
        INetURLObject aURLObject(sFileName);
        aURLObject.removeSegment();
        sFileName = aURLObject.GetMainURL();
        sFileName += rFileName;

        SfxMedium aMedium( sFileName,STREAM_READ, TRUE );
        SvStream* pInStream = aMedium.GetInStream();
        if( !pInStream )
            return sal_False;

        pInStream->ReadCString( rContent, RTL_TEXTENCODING_UTF8 );

        xub_StrLen nPos;
        while ( ( nPos = rContent.Search( 12 )) != STRING_NOTFOUND )
            rContent.Erase( nPos, 1 );
        return sal_True;
    }

/* -----------------------------13.11.2001 12:29------------------------------

 ---------------------------------------------------------------------------*/
    OEMWelcomeTabPage::OEMWelcomeTabPage(Window* pParent) :
        TabPage(pParent, ModuleRes(RID_TP_WELCOME)),
        aInfoFT(this, ResId(FT_INFO))
    {
        FreeResource();
    }
/* -----------------------------13.11.2001 12:30------------------------------

 ---------------------------------------------------------------------------*/
    OEMWelcomeTabPage::~OEMWelcomeTabPage()
    {
    }
/* -----------------------------13.11.2001 12:29------------------------------

 ---------------------------------------------------------------------------*/
    OEMLicenseTabPage::OEMLicenseTabPage(Window* pParent) :
        TabPage(pParent, ModuleRes(RID_TP_LICENSE)),
        aInfo1FT(this, ResId(FT_INFO1)),
        aLicenseML(this, ResId(ML_LICENSE)),
        aInfo2FT(this, ResId(FT_INFO2))
    {
        FreeResource();
        String sTmp(aInfo2FT.GetText());
        sTmp.SearchAndReplaceAscii("%1", ((OEMPreloadDialog*)pParent)->GetAcceptString());
        sTmp.SearchAndReplaceAscii("%2", ((OEMPreloadDialog*)pParent)->GetCancelString());

        aInfo2FT.SetText(sTmp);
    }
/* -----------------------------13.11.2001 12:30------------------------------

 ---------------------------------------------------------------------------*/
    OEMLicenseTabPage::~OEMLicenseTabPage()
    {
    }
/* ---------------------------14.11.2001 11:24----------------------------

    -----------------------------------------------------------------------*/
    void OEMLicenseTabPage::ActivatePage()
    {
        if(aLicenseML.GetText().Len())
            return;
        aLicenseML.SetLeftMargin( 5 );
        String sLicense;
        OEMPreloadDialog::LoadFromLocalFile(String::CreateFromAscii("license.txt"), sLicense);
        aLicenseML.SetText( sLicense );
    }

//.........................................................................
}   // namespace preload
//.........................................................................


