/*************************************************************************
 *
 *  $RCSfile: sdmod.cxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-04 08:53:24 $
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

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#include <svtools/languageoptions.hxx>
#endif
#ifndef _UNOTOOLS_UCBSTREAMHELPER_HXX
#include <unotools/ucbstreamhelper.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif
#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif
#ifndef _SV_STATUS_HXX //autogen
#include <vcl/status.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXMSG_HXX //autogen
#include <sfx2/msg.hxx>
#endif
#ifndef _SFXOBJFACE_HXX //autogen
#include <sfx2/objface.hxx>
#endif
#ifndef _SFX_PRINTER_HXX
#include <sfx2/printer.hxx>
#endif
#ifndef _SVX_PSZCTRL_HXX //autogen
#include <svx/pszctrl.hxx>
#endif
#ifndef _SVX_ZOOMCTRL_HXX //autogen
#include <svx/zoomctrl.hxx>
#endif
#ifndef _SVX_MODCTRL_HXX //autogen
#include <svx/modctrl.hxx>
#endif
#ifndef _ZFORLIST_HXX
#include <svtools/zforlist.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _EHDL_HXX
#include <svtools/ehdl.hxx>
#endif

#define ITEMID_SEARCH           SID_SEARCH_ITEM
#include <svx/svxids.hrc>
#include <svx/srchitem.hxx>
#include <svx/svxerr.hxx>

#pragma hdrstop

#define _SD_DLL                 // fuer SD_MOD()
#include "sderror.hxx"
#include "sdmod.hxx"
#include "sddll.hxx"
#include "sdresid.hxx"
#include "optsitem.hxx"
#ifndef SD_DRAW_DOC_SHELL_HXX
#include "DrawDocShell.hxx"
#endif
#include "drawdoc.hxx"
#include "app.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "cfgids.hxx"
#include "tools/SdGlobalResourceContainer.hxx"

TYPEINIT1( SdModule, SfxModule );

#define SdModule
#include "sdslots.hxx"


SFX_IMPL_INTERFACE(SdModule, SfxModule, SdResId(STR_APPLICATIONOBJECTBAR))
{
    SFX_STATUSBAR_REGISTRATION(RID_DRAW_STATUSBAR);
}

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

SdModule::SdModule(SfxObjectFactory* pDrawObjFact, SfxObjectFactory* pGraphicObjFact)
:   SfxModule( SfxApplication::CreateResManager("sd"), FALSE,
                  pDrawObjFact, pGraphicObjFact, NULL ),
    bWaterCan(FALSE),
    pTransferClip(NULL),
    pTransferDrag(NULL),
    pTransferSelection(NULL),
    pImpressOptions(NULL),
    pDrawOptions(NULL),
    pSearchItem(NULL),
    pNumberFormatter( NULL ),
    mpResourceContainer(new ::sd::SdGlobalResourceContainer())
{
    SetName( UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "StarDraw" ) ) );  // Nicht uebersetzen!
    pSearchItem = new SvxSearchItem(ITEMID_SEARCH);
    pSearchItem->SetAppFlag(SVX_SEARCHAPP_DRAW);
    StartListening( *SFX_APP() );
    SvxErrorHandler::Get();
    mpErrorHdl = new SfxErrorHandler( RID_SD_ERRHDL,
                                         ERRCODE_AREA_SD,
                                         ERRCODE_AREA_SD_END,
                                         GetResMgr() );

    // Create a new ref device and (by calling SetReferenceDevice())
    // set its resolution to 600 DPI.  This leads to a visually better
    // formatting of text in small sizes (6 point and below.)
    VirtualDevice* pDevice = new VirtualDevice;
    mpVirtualRefDevice = pDevice;
    pDevice->SetMapMode( MAP_100TH_MM );
    pDevice->SetReferenceDevice ( VirtualDevice::REFDEV_MODE06 );
}



/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

SdModule::~SdModule()
{
    delete pSearchItem;

    if( pNumberFormatter )
        delete pNumberFormatter;

    delete mpErrorHdl;
    delete static_cast< VirtualDevice* >( mpVirtualRefDevice );
}


/*************************************************************************
|*
|* Statusbar erzeugen
|*
\************************************************************************/

#define AUTOSIZE_WIDTH  180
#define TEXT_WIDTH(s)   rStatusBar.GetTextWidth((s))

void SdModule::FillStatusBar(StatusBar& rStatusBar)
{
    // Hinweis
    rStatusBar.InsertItem( SID_CONTEXT, TEXT_WIDTH( String().Fill( 30, 'x' ) ), // vorher 52
                            SIB_IN | SIB_LEFT | SIB_AUTOSIZE );

    // Groesse und Position
    rStatusBar.InsertItem( SID_ATTR_SIZE, SvxPosSizeStatusBarControl::GetDefItemWidth(rStatusBar), // vorher 42
                            SIB_IN | SIB_USERDRAW );
                            // SIB_AUTOSIZE | SIB_LEFT | SIB_OWNERDRAW );

    // Massstab
    rStatusBar.InsertItem( SID_ATTR_ZOOM, SvxZoomStatusBarControl::GetDefItemWidth(rStatusBar), SIB_IN | SIB_CENTER );
/*
    // Einfuege- / Uberschreibmodus
    rStatusBar.InsertItem( SID_ATTR_INSERT, TEXT_WIDTH( "EINFG" ),
                            SIB_IN | SIB_CENTER );

    // Selektionsmodus
    rStatusBar.InsertItem( SID_STATUS_SELMODE, TEXT_WIDTH( "ERG" ),
                            SIB_IN | SIB_CENTER );
*/
    // Dokument geaendert
    rStatusBar.InsertItem( SID_DOC_MODIFIED, SvxModifyControl::GetDefItemWidth(rStatusBar) );


    // Seite
    rStatusBar.InsertItem( SID_STATUS_PAGE, TEXT_WIDTH( String().Fill( 24, 'X' ) ),
                            SIB_IN | SIB_LEFT );

    // Praesentationslayout
    rStatusBar.InsertItem( SID_STATUS_LAYOUT, TEXT_WIDTH( String().Fill( 10, 'X' ) ),
                            SIB_IN | SIB_LEFT | SIB_AUTOSIZE );
}



/*************************************************************************
|*
|* get notifications
|*
\************************************************************************/

void SdModule::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    if( rHint.ISA( SfxSimpleHint ) &&
        ( (SfxSimpleHint&) rHint ).GetId() == SFX_HINT_DEINITIALIZING )
    {
        delete pImpressOptions, pImpressOptions = NULL;
        delete pDrawOptions, pDrawOptions = NULL;
    }
}

/*************************************************************************
|*
|* Optionen zurueckgeben
|*
\************************************************************************/

SdOptions* SdModule::GetSdOptions(DocumentType eDocType)
{
    SdOptions* pOptions = NULL;

    if (eDocType == DOCUMENT_TYPE_DRAW)
    {
        if (!pDrawOptions)
            pDrawOptions = new SdOptions( SDCFG_DRAW );

        pOptions = pDrawOptions;
    }
    else if (eDocType == DOCUMENT_TYPE_IMPRESS)
    {
        if (!pImpressOptions)
            pImpressOptions = new SdOptions( SDCFG_IMPRESS );

        pOptions = pImpressOptions;
    }
    if( pOptions )
    {
        UINT16 nMetric = pOptions->GetMetric();

        ::sd::DrawDocShell* pDocSh = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
        SdDrawDocument* pDoc = NULL;
        if (pDocSh)
            pDoc = pDocSh->GetDoc();

        if( nMetric != 0xffff && pDoc && eDocType == pDoc->GetDocumentType() )
            PutItem( SfxUInt16Item( SID_ATTR_METRIC, nMetric ) );
    }

    return(pOptions);
}

/*************************************************************************
|*
|* Optionen-Stream fuer interne Options oeffnen und zurueckgeben;
|* falls der Stream zum Lesen geoeffnet wird, aber noch nicht
|* angelegt wurde, wird ein 'leeres' RefObject zurueckgegeben
|*
\************************************************************************/

SvStorageStreamRef SdModule::GetOptionStream( const String& rOptionName,
                                              SdOptionStreamMode eMode )
{
    ::sd::DrawDocShell*     pDocSh = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
    SvStorageStreamRef  xStm;

    if( pDocSh )
    {
        DocumentType    eType = pDocSh->GetDoc()->GetDocumentType();
        String          aStmName;

        if( !xOptionStorage.Is() )
        {
            INetURLObject aURL( SvtPathOptions().GetUserConfigPath() );

            aURL.Append( UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "drawing.cfg" ) ) );

            SvStream* pStm = ::utl::UcbStreamHelper::CreateStream( aURL.GetMainURL( INetURLObject::NO_DECODE ), STREAM_READWRITE );

            if( pStm )
                xOptionStorage = new SvStorage( pStm, TRUE );
        }

        if( DOCUMENT_TYPE_DRAW == eType )
            aStmName.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "Draw_" ) );
        else
            aStmName.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "Impress_" ) );

        aStmName += rOptionName;

        if( SD_OPTION_STORE == eMode || xOptionStorage->IsContained( aStmName ) )
            xStm = xOptionStorage->OpenStream( aStmName );
    }

    return xStm;
}

/*************************************************************************
|*
\************************************************************************/

SvNumberFormatter* SdModule::GetNumberFormatter()
{
    if( !pNumberFormatter )
        pNumberFormatter = new SvNumberFormatter( ::comphelper::getProcessServiceFactory(), LANGUAGE_SYSTEM );

    return pNumberFormatter;
}

/*************************************************************************
|*
\************************************************************************/

OutputDevice* SdModule::GetVirtualRefDevice (void)
{
    return mpVirtualRefDevice;
}

/** This method is deprecated and only an alias to
    <member>GetVirtualRefDevice()</member>.  The given argument is ignored.
*/
OutputDevice* SdModule::GetRefDevice (::sd::DrawDocShell& rDocShell)
{
    return GetVirtualRefDevice();
}


/*************************************************************************
|*
\************************************************************************/

::com::sun::star::text::WritingMode SdModule::GetDefaultWritingMode() const
{
/*
    const SvtLanguageOptions aLanguageOptions;

    return( aLanguageOptions.IsCTLFontEnabled() ?
            ::com::sun::star::text::WritingMode_RL_TB :
            ::com::sun::star::text::WritingMode_LR_TB );
*/
    return ::com::sun::star::text::WritingMode_LR_TB;
}
