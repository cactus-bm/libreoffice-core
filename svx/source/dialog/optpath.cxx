/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: optpath.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-13 12:01:11 $
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

#include "svxdlg.hxx" //CHINA001

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif

#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef _PICKERHELPER_HXX
#include <svtools/pickerhelper.hxx>
#endif
#ifndef _AEITEM_HXX //autogen
#include <svtools/aeitem.hxx>
#endif
#ifndef _SVTABBX_HXX //autogen
#include <svtools/svtabbx.hxx>
#endif
#ifndef _SVT_FILEDLG_HXX //autogen
#include <svtools/filedlg.hxx>
#endif
#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif
#include <tools/urlobj.hxx>
#include <vcl/svapp.hxx>
#include <svtools/defaultoptions.hxx>
#include <unotools/localfilehelper.hxx>

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif

#define _SVX_OPTPATH_CXX

#include "optpath.hxx"
#include "dialmgr.hxx"
//CHINA001 #include "multipat.hxx"
#include "optpath.hrc"
#include "dialogs.hrc"
#include "helpid.hrc"

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_EXCEPTION_HPP_
#include <com/sun/star/uno/Exception.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef  _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFOLDERPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFolderPicker.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_EXECUTABLEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif
#ifndef SVX_OPTHEADERTABLISTBOX_HXX
#include "optHeaderTabListbox.hxx"
#endif
#ifndef _SVX_READONLYIMAGE_HXX
#include <readonlyimage.hxx>
#endif
#ifndef _SV_HELP_HXX
#include <vcl/help.hxx>
#endif

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ui::dialogs;
using namespace ::com::sun::star::uno;
using namespace svx;

// define ----------------------------------------------------------------

#define TAB_WIDTH1      80
#define TAB_WIDTH_MIN   10
#define TAB_WIDTH2      1000
#define ITEMID_TYPE     1
#define ITEMID_PATH     2

#define POSTFIX_INTERNAL    String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "_internal" ) )
#define POSTFIX_USER        String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "_user" ) )
#define POSTFIX_WRITABLE    String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "_writable" ) )
#define VAR_ONE             String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "%1" ) )

// struct OptPath_Impl ---------------------------------------------------

struct OptPath_Impl
{
    SvtDefaultOptions           m_aDefOpt;
    Image                       m_aLockImage;
    Image                       m_aLockImageHC;
    String                      m_sMultiPathDlg;
    Reference< XPropertySet >   m_xPathSettings;

    OptPath_Impl(const ResId& rLockRes, const ResId& rLockResHC) :
        m_aLockImage(rLockRes),
        m_aLockImageHC(rLockResHC){}
};

// struct PathUserData_Impl ----------------------------------------------

struct PathUserData_Impl
{
    USHORT          nRealId;
    SfxItemState    eState;
    String          sUserPath;
    String          sWritablePath;

    PathUserData_Impl( USHORT nId ) :
        nRealId( nId ), eState( SFX_ITEM_UNKNOWN ) {}
};

struct Handle2CfgNameMapping_Impl
{
    USHORT      m_nHandle;
    const char* m_pCfgName;
};

static Handle2CfgNameMapping_Impl __READONLY_DATA Hdl2CfgMap_Impl[] =
{
    { SvtPathOptions::PATH_AUTOCORRECT, "AutoCorrect" },
    { SvtPathOptions::PATH_AUTOTEXT,    "AutoText" },
    { SvtPathOptions::PATH_BACKUP,      "Backup" },
    { SvtPathOptions::PATH_GALLERY,     "Gallery" },
    { SvtPathOptions::PATH_GRAPHIC,     "Graphic" },
    { SvtPathOptions::PATH_TEMP,        "Temp" },
    { SvtPathOptions::PATH_TEMPLATE,    "Template" },
    { SvtPathOptions::PATH_WORK,        "Work" },
    { USHRT_MAX, NULL }
};

static String getCfgName_Impl( USHORT _nHandle )
{
    String sCfgName;
    USHORT nIndex = 0;
    while ( Hdl2CfgMap_Impl[ nIndex ].m_nHandle != USHRT_MAX )
    {
        if ( Hdl2CfgMap_Impl[ nIndex ].m_nHandle == _nHandle )
        {
            // config name found
            sCfgName = String::CreateFromAscii( Hdl2CfgMap_Impl[ nIndex ].m_pCfgName );
            break;
        }
        ++nIndex;
    }

    return sCfgName;
}

#define MULTIPATH_DELIMITER     ';'

String Convert_Impl( const String& rValue )
{
    char cDelim = MULTIPATH_DELIMITER;
    USHORT nCount = rValue.GetTokenCount( cDelim );
    String aReturn;
    for ( USHORT i=0; i<nCount ; ++i )
    {
        String aValue = rValue.GetToken( i, cDelim );
        INetURLObject aObj( aValue );
        if ( aObj.GetProtocol() == INET_PROT_FILE )
            aReturn += String(aObj.PathToFileName());
        else if ( ::utl::LocalFileHelper::IsFileContent( aValue ) )
            aReturn += String(aObj.GetURLPath( INetURLObject::DECODE_WITH_CHARSET ));
        if ( i+1 < nCount)
            aReturn += MULTIPATH_DELIMITER;
    }

    return aReturn;
}

// class SvxControlFocusHelper ---------------------------------------------

long SvxControlFocusHelper::Notify( NotifyEvent& rNEvt )
{
    long nRet = Control::Notify( rNEvt );

    if ( m_pFocusCtrl && rNEvt.GetWindow() != m_pFocusCtrl && rNEvt.GetType() == EVENT_GETFOCUS )
        m_pFocusCtrl->GrabFocus();
    return nRet;
}

// functions -------------------------------------------------------------

BOOL IsMultiPath_Impl( const USHORT nIndex )
{
    return ( SvtPathOptions::PATH_AUTOCORRECT == nIndex ||
             SvtPathOptions::PATH_AUTOTEXT == nIndex ||
             SvtPathOptions::PATH_BASIC == nIndex ||
             SvtPathOptions::PATH_GALLERY == nIndex ||
             SvtPathOptions::PATH_TEMPLATE == nIndex );
}

// class SvxPathTabPage --------------------------------------------------

SvxPathTabPage::SvxPathTabPage( Window* pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, SVX_RES( RID_SFXPAGE_PATH ), rSet ),

    aTypeText       ( this, ResId( FT_TYPE ) ),
    aPathText       ( this, ResId( FT_PATH ) ),
    aPathCtrl       ( this, ResId( LB_PATH ) ),
    aStandardBtn    ( this, ResId( BTN_STANDARD ) ),
    aPathBtn        ( this, ResId( BTN_PATH ) ),
    aStdBox         ( this, ResId( GB_STD ) ),

    pHeaderBar      ( NULL ),
    pPathBox        ( NULL ),
    pImpl           ( new OptPath_Impl( ResId(IMG_LOCK), ResId(IMG_LOCK_HC) ) )

{
    pImpl->m_sMultiPathDlg = String( ResId( STR_MULTIPATHDLG ) );
    aStandardBtn.SetClickHdl( LINK( this, SvxPathTabPage, StandardHdl_Impl ) );
    Link aLink = LINK( this, SvxPathTabPage, PathHdl_Impl );
    aPathBtn.SetClickHdl( aLink );
    Size aBoxSize = aPathCtrl.GetOutputSizePixel();
    pHeaderBar = new HeaderBar( &aPathCtrl, WB_BUTTONSTYLE | WB_BOTTOMBORDER );
    pHeaderBar->SetPosSizePixel( Point( 0, 0 ), Size( aBoxSize.Width(), 16 ) );
    pHeaderBar->SetSelectHdl( LINK( this, SvxPathTabPage, HeaderSelect_Impl ) );
    pHeaderBar->SetEndDragHdl( LINK( this, SvxPathTabPage, HeaderEndDrag_Impl ) );
    Size aSz;
    aSz.Width() = TAB_WIDTH1;
    pHeaderBar->InsertItem( ITEMID_TYPE, aTypeText.GetText(),
                            LogicToPixel( aSz, MapMode( MAP_APPFONT ) ).Width(),
                            HIB_LEFT | HIB_VCENTER | HIB_CLICKABLE | HIB_UPARROW );
    aSz.Width() = TAB_WIDTH2;
    pHeaderBar->InsertItem( ITEMID_PATH, aPathText.GetText(),
                            LogicToPixel( aSz, MapMode( MAP_APPFONT ) ).Width(),
                            HIB_LEFT | HIB_VCENTER );

    static long nTabs[] = {3, 0, TAB_WIDTH1, TAB_WIDTH1 + TAB_WIDTH2 };
    Size aHeadSize = pHeaderBar->GetSizePixel();

    WinBits nBits = WB_SORT | WB_HSCROLL | WB_CLIPCHILDREN | WB_TABSTOP;
    pPathBox = new OptHeaderTabListBox( &aPathCtrl, nBits );
    aPathCtrl.SetFocusControl( pPathBox );
    pPathBox->SetWindowBits( nBits );
    pPathBox->SetDoubleClickHdl( aLink );
    pPathBox->SetSelectHdl( LINK( this, SvxPathTabPage, PathSelect_Impl ) );
    pPathBox->SetSelectionMode( MULTIPLE_SELECTION );
    pPathBox->SetPosSizePixel( Point( 0, aHeadSize.Height() ),
                               Size( aBoxSize.Width(), aBoxSize.Height() - aHeadSize.Height() ) );
    pPathBox->SetTabs( &nTabs[0], MAP_APPFONT );
    pPathBox->InitHeaderBar( pHeaderBar );
    pPathBox->SetHighlightRange();
    pPathBox->SetHelpId( HID_OPTPATH_CTL_PATH );
    pHeaderBar->SetHelpId( HID_OPTPATH_HEADERBAR );
    pPathBox->Show();
    pHeaderBar->Show();

    FreeResource();
}

// -----------------------------------------------------------------------

SvxPathTabPage::~SvxPathTabPage()
{
    // #110603# do not grab focus to a destroyed window !!!
    aPathCtrl.SetFocusControl( NULL );

    pHeaderBar->Hide();
    for ( USHORT i = 0; i < pPathBox->GetEntryCount(); ++i )
        delete (PathUserData_Impl*)pPathBox->GetEntry(i)->GetUserData();
    delete pPathBox;
    delete pHeaderBar;
    delete pImpl;
}

// -----------------------------------------------------------------------

SfxTabPage* SvxPathTabPage::Create( Window* pParent,
                                    const SfxItemSet& rAttrSet )
{
    return ( new SvxPathTabPage( pParent, rAttrSet ) );
}

// -----------------------------------------------------------------------

BOOL SvxPathTabPage::FillItemSet( SfxItemSet& )
{
    SvtPathOptions aPathOpt;
    for ( USHORT i = 0; i < pPathBox->GetEntryCount(); ++i )
    {
        PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pPathBox->GetEntry(i)->GetUserData();
        USHORT nRealId = pPathImpl->nRealId;
        if ( pPathImpl->eState == SFX_ITEM_SET )
            SetPathList( nRealId, pPathImpl->sUserPath, pPathImpl->sWritablePath );
    }
    return TRUE;
}

// -----------------------------------------------------------------------

void SvxPathTabPage::Reset( const SfxItemSet& )
{
    pPathBox->Clear();
    SvtPathOptions aPathOpt; //! deprecated

    for( USHORT i = 0; i <= (USHORT)SvtPathOptions::PATH_WORK; ++i )
    {
        switch (i)
        {
            case SvtPathOptions::PATH_AUTOCORRECT:
            case SvtPathOptions::PATH_AUTOTEXT:
            case SvtPathOptions::PATH_BACKUP:
            case SvtPathOptions::PATH_GALLERY:
            case SvtPathOptions::PATH_GRAPHIC:
            case SvtPathOptions::PATH_TEMP:
            case SvtPathOptions::PATH_TEMPLATE:
            case SvtPathOptions::PATH_WORK:
            {
                String aStr( SVX_RES( RID_SVXSTR_PATH_NAME_START + i ) );
                String sInternal, sUser, sWritable;
                GetPathList( i, sInternal, sUser, sWritable );
                String sTmpPath = sUser;
                if ( sTmpPath.Len() > 0 && sWritable.Len() > 0 )
                    sTmpPath += MULTIPATH_DELIMITER;
                sTmpPath += sWritable;
                String aValue( sTmpPath );
                aStr += '\t';
                aStr += Convert_Impl( aValue );
                SvLBoxEntry* pEntry = pPathBox->InsertEntry( aStr );
                if ( aPathOpt.IsPathReadonly( (SvtPathOptions::Pathes)i ) )
                {
                    pPathBox->SetCollapsedEntryBmp( pEntry, pImpl->m_aLockImage, BMP_COLOR_NORMAL );
                    pPathBox->SetCollapsedEntryBmp( pEntry, pImpl->m_aLockImageHC, BMP_COLOR_HIGHCONTRAST );
                }
                PathUserData_Impl* pPathImpl = new PathUserData_Impl(i);
                pPathImpl->sUserPath = sUser;
                pPathImpl->sWritablePath = sWritable;
                pEntry->SetUserData( pPathImpl );
            }
        }
    }

    String aUserData = GetUserData();
    if ( aUserData.Len() )
    {
        // Spaltenbreite restaurieren
        pHeaderBar->SetItemSize( ITEMID_TYPE, aUserData.GetToken(0).ToInt32() );
        HeaderEndDrag_Impl( NULL );
        // Sortierrichtung restaurieren
        BOOL bUp = (BOOL)(USHORT)aUserData.GetToken(1).ToInt32();
        HeaderBarItemBits nBits = pHeaderBar->GetItemBits(ITEMID_TYPE);

        if ( bUp )
        {
            nBits &= ~HIB_UPARROW;
            nBits |= HIB_DOWNARROW;
        }
        else
        {
            nBits &= ~HIB_DOWNARROW;
            nBits |= HIB_UPARROW;
        }
        pHeaderBar->SetItemBits( ITEMID_TYPE, nBits );
        HeaderSelect_Impl( NULL );
    }
    PathSelect_Impl( NULL );
}

// -----------------------------------------------------------------------

void SvxPathTabPage::FillUserData()
{
    String aUserData = String::CreateFromInt32( pHeaderBar->GetItemSize( ITEMID_TYPE ) );
    aUserData += ';';
    HeaderBarItemBits nBits = pHeaderBar->GetItemBits( ITEMID_TYPE );
    BOOL bUp = ( ( nBits & HIB_UPARROW ) == HIB_UPARROW );
    aUserData += bUp ? '1' : '0';
    SetUserData( aUserData );
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, PathSelect_Impl, OptHeaderTabListBox *, EMPTYARG )

/*  [Beschreibung]

*/

{
    USHORT nSelCount = 0;
    SvLBoxEntry* pEntry = pPathBox->FirstSelected();

    //the entry image indicates whether the path is write protected
    Image aEntryImage;
    if(pEntry)
        aEntryImage = pPathBox->GetCollapsedEntryBmp( pEntry );
    BOOL bEnable = !aEntryImage;
    while ( pEntry && ( nSelCount < 2 ) )
    {
        nSelCount++;
        pEntry = pPathBox->NextSelected( pEntry );
    }

    aPathBtn.Enable( 1 == nSelCount && bEnable);
    aStandardBtn.Enable( nSelCount > 0 && bEnable);
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, StandardHdl_Impl, PushButton *, EMPTYARG )
{
    SvLBoxEntry* pEntry = pPathBox->FirstSelected();
    while ( pEntry )
    {
        PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
        String aOldPath = pImpl->m_aDefOpt.GetDefaultPath( pPathImpl->nRealId );

        if ( aOldPath.Len() )
        {
            String sInternal, sUser, sWritable, sTemp;
            GetPathList( pPathImpl->nRealId, sInternal, sUser, sWritable );

            USHORT i;
            USHORT nOldCount = aOldPath.GetTokenCount( MULTIPATH_DELIMITER );
            USHORT nIntCount = sInternal.GetTokenCount( MULTIPATH_DELIMITER );
            for ( i = 0; i < nOldCount; ++i )
            {
                bool bFound = false;
                String sOnePath = aOldPath.GetToken( i, MULTIPATH_DELIMITER );
                for ( USHORT j = 0; !bFound && j < nIntCount; ++j )
                {
                    if ( sInternal.GetToken( i, MULTIPATH_DELIMITER ) == sOnePath )
                        bFound = true;
                }
                if ( !bFound )
                {
                    if ( sTemp.Len() > 0 )
                        sTemp += MULTIPATH_DELIMITER;
                    sTemp += sOnePath;
                }
            }

            String sUserPath, sWritablePath;
            nOldCount = sTemp.GetTokenCount( MULTIPATH_DELIMITER );
            for ( USHORT i = 0; nOldCount > 0 && i < nOldCount - 1; ++i )
            {
                if ( sUserPath.Len() > 0 )
                    sUserPath += MULTIPATH_DELIMITER;
                sUserPath += sTemp.GetToken( i, MULTIPATH_DELIMITER );
            }
            sWritablePath = sTemp.GetToken( nOldCount - 1, MULTIPATH_DELIMITER );

            pPathBox->SetEntryText( Convert_Impl( sTemp ), pEntry, 1 );
            pPathImpl->eState = SFX_ITEM_SET;
            pPathImpl->sUserPath = sUserPath;
            pPathImpl->sWritablePath = sWritablePath;
        }
        pEntry = pPathBox->NextSelected( pEntry );
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, PathHdl_Impl, PushButton *, EMPTYARG )
{
    SvLBoxEntry* pEntry = pPathBox->GetCurEntry();
    USHORT nPos = ( pEntry != NULL ) ? ( (PathUserData_Impl*)pEntry->GetUserData() )->nRealId : 0;
    String sInternal, sUser, sWritable;
    if ( pEntry )
    {
        PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
        GetPathList( pPathImpl->nRealId, sInternal, sUser, sWritable );
        sUser = pPathImpl->sUserPath;
        sWritable = pPathImpl->sWritablePath;
    }

    if(pEntry && !(!((OptHeaderTabListBox*)pPathBox)->GetCollapsedEntryBmp(pEntry)))
        return 0;

    if ( IsMultiPath_Impl( nPos ) )
    {
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if ( pFact )
        {
            AbstractSvxMultiPathDialog* pMultiDlg =
                pFact->CreateSvxMultiPathDialog( this, ResId( RID_SVXDLG_MULTIPATH ) );
            DBG_ASSERT( pMultiDlg, "Dialogdiet fail!" );
            pMultiDlg->EnableRadioButtonMode();

            String sPath( sUser );
            if ( sPath.Len() > 0 )
                sPath += MULTIPATH_DELIMITER;
            sPath += sWritable;
            pMultiDlg->SetPath( sPath );

            String sPathName = pPathBox->GetEntryText( pEntry, 0 );
            String sNewTitle( pImpl->m_sMultiPathDlg );
            sNewTitle.SearchAndReplace( VAR_ONE, sPathName );
            pMultiDlg->SetTitle( sNewTitle );

            if ( pMultiDlg->Execute() == RET_OK && pEntry )
            {
                sUser.Erase();
                sWritable.Erase();
                String sFullPath;
                String sNewPath = pMultiDlg->GetPath();
                char cDelim = MULTIPATH_DELIMITER;
                USHORT nCount = sNewPath.GetTokenCount( cDelim );
                if ( nCount > 0 )
                {
                    USHORT i = 0;
                    for ( ; i < nCount - 1; ++i )
                    {
                        if ( sUser.Len() > 0 )
                            sUser += cDelim;
                        sUser += sNewPath.GetToken( i, cDelim );
                    }
                    if ( sFullPath.Len() > 0 )
                        sFullPath += cDelim;
                    sFullPath += sUser;
                    sWritable += sNewPath.GetToken( i, cDelim );
                    sFullPath += cDelim;
                    sFullPath += sWritable;
                }

                pPathBox->SetEntryText( Convert_Impl( sFullPath ), pEntry, 1 );
                // save modified flag
                PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pEntry->GetUserData();
                pPathImpl->eState = SFX_ITEM_SET;
                pPathImpl->sUserPath = sUser;
                pPathImpl->sWritablePath = sWritable;
            }
            delete pMultiDlg;
        }
    }
    else if ( pEntry )
    {
        try
        {
            rtl::OUString aService( RTL_CONSTASCII_USTRINGPARAM( FOLDER_PICKER_SERVICE_NAME ) );
            Reference < XMultiServiceFactory > xFactory( ::comphelper::getProcessServiceFactory() );
            Reference < XFolderPicker > xFolderPicker( xFactory->createInstance( aService ), UNO_QUERY );

//          svt::SetDialogHelpId( xFolderPicker, HID_OPTIONS_PATHS_SELECTFOLDER );

            INetURLObject aURL( sWritable, INET_PROT_FILE );
            xFolderPicker->setDisplayDirectory( aURL.GetMainURL( INetURLObject::NO_DECODE ) );
            short nRet = xFolderPicker->execute();

            if ( ExecutableDialogResults::OK != nRet )
                return 0;

            // old path is an URL?
            INetURLObject aObj( sWritable );
            FASTBOOL bURL = ( aObj.GetProtocol() != INET_PROT_NOT_VALID );
            rtl::OUString aPathStr = xFolderPicker->getDirectory();
            INetURLObject aNewObj( aPathStr );
            aNewObj.removeFinalSlash();

            // then the new path also an URL else system path
            String aNewPathStr = bURL ? aPathStr : aNewObj.getFSysPath( INetURLObject::FSYS_DETECT );

            FASTBOOL bChanged =
#ifdef UNX
// Unix is case sensitive
                                ( aNewPathStr != sWritable );
#else
                                ( aNewPathStr.CompareIgnoreCaseToAscii( sWritable ) != COMPARE_EQUAL );
#endif
            if ( bChanged )
            {
                pPathBox->SetEntryText( Convert_Impl( aNewPathStr ), pEntry, 1 );
                nPos = (USHORT)pPathBox->GetModel()->GetAbsPos( pEntry );
                PathUserData_Impl* pPathImpl = (PathUserData_Impl*)pPathBox->GetEntry(nPos)->GetUserData();
                pPathImpl->eState = SFX_ITEM_SET;
                pPathImpl->sWritablePath = aNewPathStr;
            }
        }
        catch( Exception& )
        {
            DBG_ERRORFILE( "SvxPathTabPage::PathHdl_Impl: exception from folder picker" )
        }
    }
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, HeaderSelect_Impl, HeaderBar*, pBar )
{
    if ( pBar && pBar->GetCurItemId() != ITEMID_TYPE )
        return 0;

    HeaderBarItemBits nBits = pHeaderBar->GetItemBits(ITEMID_TYPE);
    BOOL bUp = ( ( nBits & HIB_UPARROW ) == HIB_UPARROW );
    SvSortMode eMode = SortAscending;

    if ( bUp )
    {
        nBits &= ~HIB_UPARROW;
        nBits |= HIB_DOWNARROW;
        eMode = SortDescending;
    }
    else
    {
        nBits &= ~HIB_DOWNARROW;
        nBits |= HIB_UPARROW;
    }
    pHeaderBar->SetItemBits( ITEMID_TYPE, nBits );
    SvTreeList* pModel = pPathBox->GetModel();
    pModel->SetSortMode( eMode );
    pModel->Resort();
    return 1;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPathTabPage, HeaderEndDrag_Impl, HeaderBar*, pBar )
{
    if ( pBar && !pBar->GetCurItemId() )
        return 0;

    if ( !pHeaderBar->IsItemMode() )
    {
        Size aSz;
        USHORT nTabs = pHeaderBar->GetItemCount();
        long nTmpSz = 0;
        long nWidth = pHeaderBar->GetItemSize(ITEMID_TYPE);
        long nBarWidth = pHeaderBar->GetSizePixel().Width();

        if(nWidth < TAB_WIDTH_MIN)
            pHeaderBar->SetItemSize( ITEMID_TYPE, TAB_WIDTH_MIN);
        else if ( ( nBarWidth - nWidth ) < TAB_WIDTH_MIN )
            pHeaderBar->SetItemSize( ITEMID_TYPE, nBarWidth - TAB_WIDTH_MIN );

        for ( USHORT i = 1; i <= nTabs; ++i )
        {
            long _nWidth = pHeaderBar->GetItemSize(i);
            aSz.Width() =  _nWidth + nTmpSz;
            nTmpSz += _nWidth;
            pPathBox->SetTab( i, PixelToLogic( aSz, MapMode(MAP_APPFONT) ).Width(), MAP_APPFONT );
        }
    }
    return 1;
}

// -----------------------------------------------------------------------

void SvxPathTabPage::GetPathList(
    USHORT _nPathHandle, String& _rInternalPath, String& _rUserPath, String& _rWritablePath )
{
    String sCfgName = getCfgName_Impl( _nPathHandle );

    // load PathSettings service if necessary
    if ( !pImpl->m_xPathSettings.is() )
    {
        Reference< XMultiServiceFactory > xSMgr = comphelper::getProcessServiceFactory();
        pImpl->m_xPathSettings = Reference< XPropertySet >( xSMgr->createInstance(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.util.PathSettings") ) ), UNO_QUERY );
    }

    try
    {
        if ( pImpl->m_xPathSettings.is() )
        {
            // load internal paths
            String sProp( sCfgName );
            sProp = sCfgName;
            sProp += POSTFIX_INTERNAL;
            Any aAny = pImpl->m_xPathSettings->getPropertyValue( sProp );
            Sequence< ::rtl::OUString > aPathSeq;
            if ( aAny >>= aPathSeq )
            {
                long i, nCount = aPathSeq.getLength();
                const ::rtl::OUString* pPaths = aPathSeq.getConstArray();

                for ( i = 0; i < nCount; ++i )
                {
                    if ( _rInternalPath.Len() > 0 )
                        _rInternalPath += ';';
                    _rInternalPath += String( pPaths[i] );
                }
            }
            // load user paths
            sProp = sCfgName;
            sProp += POSTFIX_USER;
            aAny = pImpl->m_xPathSettings->getPropertyValue( sProp );
            if ( aAny >>= aPathSeq )
            {
                long i, nCount = aPathSeq.getLength();
                const ::rtl::OUString* pPaths = aPathSeq.getConstArray();

                for ( i = 0; i < nCount; ++i )
                {
                    if ( _rUserPath.Len() > 0 )
                        _rUserPath += ';';
                    _rUserPath += String( pPaths[i] );
                }
            }
            // then the writable path
            sProp = sCfgName;
            sProp += POSTFIX_WRITABLE;
            aAny = pImpl->m_xPathSettings->getPropertyValue( sProp );
            ::rtl::OUString sWritablePath;
            if ( aAny >>= sWritablePath )
                _rWritablePath = String( sWritablePath );
        }
    }
    catch( const Exception& )
    {
        OSL_ENSURE( sal_False, "SvxPathTabPage::GetPathList(): caught an exception!" );
    }
}

// -----------------------------------------------------------------------

void SvxPathTabPage::SetPathList(
    USHORT _nPathHandle, const String& _rUserPath, const String& _rWritablePath )
{
    String sCfgName = getCfgName_Impl( _nPathHandle );

    // load PathSettings service if necessary
    if ( !pImpl->m_xPathSettings.is() )
    {
        Reference< XMultiServiceFactory > xSMgr = comphelper::getProcessServiceFactory();
        pImpl->m_xPathSettings = Reference< XPropertySet >( xSMgr->createInstance(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.util.PathSettings") ) ), UNO_QUERY );
    }

    try
    {
        if ( pImpl->m_xPathSettings.is() )
        {
            // save user paths
            char cDelim = MULTIPATH_DELIMITER;
            USHORT nCount = _rUserPath.GetTokenCount( cDelim );
            Sequence< ::rtl::OUString > aPathSeq( nCount );
            ::rtl::OUString* pArray = aPathSeq.getArray();
            for ( USHORT i = 0; i < nCount; ++i )
                pArray[i] = ::rtl::OUString( _rUserPath.GetToken( i, cDelim ) );
            String sProp( sCfgName );
            sProp += POSTFIX_USER;
            Any aValue = makeAny( aPathSeq );
            pImpl->m_xPathSettings->setPropertyValue( sProp, aValue );

            // then the writable path
            aValue = makeAny( ::rtl::OUString( _rWritablePath ) );
            sProp = sCfgName;
            sProp += POSTFIX_WRITABLE;
            pImpl->m_xPathSettings->setPropertyValue( sProp, aValue );
        }
    }
    catch( const Exception& )
    {
        OSL_ENSURE( sal_False, "SvxPathTabPage::SetPathList(): caught an exception!" );
    }
}

