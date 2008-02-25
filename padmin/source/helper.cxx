/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: helper.cxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: obo $ $Date: 2008-02-25 15:46:57 $
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

#include <unistd.h>

#ifndef _PAD_HELPER_HXX_
#include <helper.hxx>
#endif
#ifndef _PAD_PADIALOG_HRC_
#include <padialog.hrc>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_EXECUTABLEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFOLDERPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFolderPicker.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XCONTROLACCESS_HPP_
#include <com/sun/star/ui/dialogs/XControlAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _UNOTOOLS_CONFIGNODE_HXX_
#include <unotools/confignode.hxx>
#endif
#ifndef _VCL_UNOHELP_HXX
#include <vcl/unohelp.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_MSLANGID_HXX
#include <i18npool/mslangid.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif


using namespace osl;
using namespace rtl;
using namespace padmin;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::ui::dialogs;

#define MAX_PATH 1024

/*
 *  PaResId
 */

ResId padmin::PaResId( sal_uInt32 nId )
{
    static ResMgr* pPaResMgr = NULL;
    if( ! pPaResMgr )
    {
        ::com::sun::star::lang::Locale aLocale;
//      LanguageType nLang = LANGUAGE_SYSTEM;

        utl::OConfigurationNode aNode =
            utl::OConfigurationTreeRoot::tryCreateWithServiceFactory(
                    vcl::unohelper::GetMultiServiceFactory(),
                    OUString::createFromAscii( "org.openoffice.Setup/L10N" ) );
        if ( aNode.isValid() )
        {
            rtl::OUString aLoc;
            Any aValue = aNode.getNodeValue( OUString::createFromAscii( "ooLocale" ) );
            if( aValue >>= aLoc )
            {
//                LanguageType nTmpLang = MsLangId::convertIsoStringToLanguage( aLoc );
//                if( nTmpLang != LANGUAGE_DONTKNOW )
//                    nLang = nTmpLang;
                sal_Int32 nIndex = 0;
                aLocale.Language = aLoc.getToken( 0, '-', nIndex );
                aLocale.Country = aLoc.getToken( 0, '-', nIndex );
                aLocale.Variant = aLoc.getToken( 0, '-', nIndex );
            }
        }
        pPaResMgr = ResMgr::SearchCreateResMgr( "spa", aLocale );
        AllSettings aSettings = Application::GetSettings();
//        aSettings.SetUILanguage( nLang );
        aSettings.SetUILocale( aLocale );
        Application::SetSettings( aSettings );
    }
    return ResId( nId, *pPaResMgr );
}

/*
 *  FindFiles
 */

void padmin::FindFiles( const String& rDirectory, ::std::list< String >& rResult, const String& rSuffixes, bool bRecursive )
{
    rResult.clear();

    OUString aDirPath;
    ::osl::FileBase::getFileURLFromSystemPath( rDirectory, aDirPath );
    Directory aDir( aDirPath );
    if( aDir.open() != FileBase::E_None )
        return;
    DirectoryItem aItem;
    while( aDir.getNextItem( aItem ) == FileBase::E_None )
    {
        FileStatus aStatus( FileStatusMask_FileName         |
                            FileStatusMask_Type
                            );
        if( aItem.getFileStatus( aStatus ) == FileBase::E_None &&
            ( aStatus.getFileType() == FileStatus::Regular ||
              aStatus.getFileType() == FileStatus::Link ) )
        {
            String aFileName = aStatus.getFileName();
            int nToken = rSuffixes.GetTokenCount( ';' );
            while( nToken-- )
            {
                String aSuffix = rSuffixes.GetToken( nToken, ';' );
                if( aFileName.Len() > aSuffix.Len()+1 )
                {
                    String aExtension = aFileName.Copy( aFileName.Len()-aSuffix.Len() );
                    if( aFileName.GetChar( aFileName.Len()-aSuffix.Len()-1 ) == '.' &&
                        aExtension.EqualsIgnoreCaseAscii( aSuffix ) )
                    {
                        rResult.push_back( aFileName );
                        break;
                    }
                }
            }
        }
        else if( bRecursive &&
                 ( aStatus.getFileType() == FileStatus::Directory ||
                   aStatus.getFileType() == FileStatus::Link ) )
        {
            OUStringBuffer aSubDir( rDirectory );
            aSubDir.appendAscii( "/", 1 );
            aSubDir.append( aStatus.getFileName() );
            std::list< String > subfiles;
            FindFiles( aSubDir.makeStringAndClear(), subfiles, rSuffixes, bRecursive );
            for( std::list< String >::const_iterator it = subfiles.begin(); it != subfiles.end(); ++it )
            {
                OUStringBuffer aSubFile( aStatus.getFileName() );
                aSubFile.appendAscii( "/", 1 );
                aSubFile.append( *it );
                rResult.push_back( aSubFile.makeStringAndClear() );
            }
        }
    }
    aDir.close();
}

/*
 *  DelMultiListBox
 */

long DelMultiListBox::Notify( NotifyEvent& rEvent )
{
    long nRet = 0;

    if( rEvent.GetType() == EVENT_KEYINPUT &&
        rEvent.GetKeyEvent()->GetKeyCode().GetCode() == KEY_DELETE )
    {
        m_aDelPressedLink.Call( this );
        nRet = 1;
    }
    else
        nRet = MultiListBox::Notify( rEvent );

    return nRet;
}

/*
 *  DelListBox
 */

long DelListBox::Notify( NotifyEvent& rEvent )
{
    long nRet = 0;

    if( rEvent.GetType() == EVENT_KEYINPUT &&
        rEvent.GetKeyEvent()->GetKeyCode().GetCode() == KEY_DELETE )
    {
        m_aDelPressedLink.Call( this );
        nRet = 1;
    }
    else
        nRet = ListBox::Notify( rEvent );

    return nRet;
}

/*
 *  QueryString
 */

QueryString::QueryString( Window* pParent, String& rQuery, String& rRet, const ::std::list< String >& rChoices ) :
        ModalDialog( pParent, PaResId( RID_STRINGQUERYDLG ) ),
        m_aOKButton( this, PaResId( RID_STRQRY_BTN_OK ) ),
        m_aCancelButton( this, PaResId( RID_STRQRY_BTN_CANCEL ) ),
        m_aFixedText( this, PaResId( RID_STRQRY_TXT_RENAME ) ),
        m_aEdit( this, PaResId( RID_STRQRY_EDT_NEWNAME ) ),
        m_aComboBox( this, PaResId( RID_STRQRY_BOX_NEWNAME ) ),
        m_rReturnValue( rRet )
{
    FreeResource();
    m_aOKButton.SetClickHdl( LINK( this, QueryString, ClickBtnHdl ) );
    m_aFixedText.SetText( rQuery );
    if( rChoices.begin() != rChoices.end() )
    {
        m_aComboBox.SetText( m_rReturnValue );
        m_aComboBox.InsertEntry( m_rReturnValue );
        for( ::std::list<String>::const_iterator it = rChoices.begin(); it != rChoices.end(); ++it )
            m_aComboBox.InsertEntry( *it );
        m_aEdit.Show( FALSE );
        m_bUseEdit = false;
    }
    else
    {
        m_aEdit.SetText( m_rReturnValue );
        m_aComboBox.Show( FALSE );
        m_bUseEdit = true;
    }
    SetText( Application::GetDisplayName() );
}

QueryString::~QueryString()
{
}

IMPL_LINK( QueryString, ClickBtnHdl, Button*, pButton )
{
    if( pButton == &m_aOKButton )
    {
        m_rReturnValue = m_bUseEdit ? m_aEdit.GetText() : m_aComboBox.GetText();
        EndDialog( 1 );
    }
    else
        EndDialog(0);
    return 0;
}

/*
 *  AreYouSure
 */

BOOL padmin::AreYouSure( Window* pParent, int nRid )
{
    if( nRid == -1 )
        nRid = RID_YOU_SURE;
    QueryBox aQueryBox( pParent, WB_YES_NO | WB_DEF_NO,
                        String( PaResId( nRid ) ) );
    return aQueryBox.Execute() == RET_NO ? FALSE : TRUE;
}

/*
 *  getPadminRC
 */

static Config* pRC = NULL;

Config& padmin::getPadminRC()
{
    if( ! pRC )
    {
        static const char* pEnv = getenv( "HOME" );
        String aFileName( pEnv ? pEnv : "", osl_getThreadTextEncoding() );
        aFileName.AppendAscii( "/.padminrc" );
        pRC = new Config( aFileName );
    }
    return *pRC;
}

void padmin::freePadminRC()
{
    if( pRC )
        delete pRC, pRC = NULL;
}

bool padmin::chooseDirectory( String& rInOutPath )
{
    bool bRet = false;
    Reference< XMultiServiceFactory > xFactory( ::comphelper::getProcessServiceFactory() );
    if( xFactory.is() )
    {
        Reference< XFolderPicker > xFolderPicker( xFactory->createInstance( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.FolderPicker" ) ) ), UNO_QUERY );
        if( xFolderPicker.is() )
        {
            Reference< XControlAccess > xCA( xFolderPicker, UNO_QUERY );
            if( xCA.is() )
            {
                try
                {
                    Any aState;
                    aState <<= sal_False;
                    xCA->setControlProperty( OUString( RTL_CONSTASCII_USTRINGPARAM( "HelpButton" ) ),
                                             OUString( RTL_CONSTASCII_USTRINGPARAM( "Visible" ) ),
                                             aState );

                }
                catch( ... )
                {
                }
            }
            INetURLObject aObj( rInOutPath, INET_PROT_FILE, INetURLObject::ENCODE_ALL );
            xFolderPicker->setDisplayDirectory( aObj.GetMainURL(INetURLObject::DECODE_TO_IURI) );
            if( xFolderPicker->execute() == ExecutableDialogResults::OK )
            {
                aObj = INetURLObject( xFolderPicker->getDirectory() );
                rInOutPath = aObj.PathToFileName();
                bRet = true;
            }
        }
#if OSL_DEBUG_LEVEL > 1
        else
            fprintf( stderr, "could not get FolderPicker service\n" );
#endif
    }
    return bRet;
}
