/*************************************************************************
 *
 *  $RCSfile: sfxhelp.cxx,v $
 *
 *  $Revision: 1.34 $
 *
 *  last change: $Author: mba $ $Date: 2001-06-19 07:20:37 $
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

#include "sfxhelp.hxx"

#pragma hdrstop

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XTASK_HPP_
#include <com/sun/star/frame/XTask.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XTASKSSUPPLIER_HPP_
#include <com/sun/star/frame/XTasksSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <toolkit/helper/vclunohelper.hxx>

#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_HELPOPT_HXX
#include <svtools/helpopt.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif

#include <berkeleydb/db_cxx.h>
#include <svtools/pathoptions.hxx>
#include <rtl/ustring.hxx>

#define _SVSTDARR_ULONGSSORT
#include <svtools/svstdarr.hxx>     // SvUShorts

#include "sfxsids.hrc"
#include "app.hxx"
#include "viewfrm.hxx"
#include "msgpool.hxx"
#include "newhelp.hxx"
#include "objsh.hxx"
#include "docfac.hxx"
#include "sfxresid.hxx"
#include "app.hrc"

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::rtl;

#define ERROR_TAG   String( DEFINE_CONST_UNICODE("Error: ") )
#define PATH_TAG    String( DEFINE_CONST_UNICODE("\nPath: ") )

// class SfxHelp_Impl --------------------------------------------------

#define STARTERLIST 0

class SfxHelpOptions_Impl : public utl::ConfigItem
{
    SvULongsSort*   pIds;
public:

                    SfxHelpOptions_Impl();
                    ~SfxHelpOptions_Impl();
    BOOL            HasId( ULONG nId )
                    { USHORT nDummy; return pIds->Seek_Entry( nId, &nDummy ); }
};

static Sequence< OUString > GetPropertyNames()
{
    static const char* aPropNames[] =
    {
        "HelpAgentStarterList",
    };

    const int nCount = sizeof( aPropNames ) / sizeof( const char* );
    Sequence< OUString > aNames( nCount );
    OUString* pNames = aNames.getArray();
    for ( int i = 0; i < nCount; i++ )
        pNames[i] = OUString::createFromAscii( aPropNames[i] );

    return aNames;
}

// -----------------------------------------------------------------------

SfxHelpOptions_Impl::SfxHelpOptions_Impl()
    : ConfigItem( OUString::createFromAscii("Office.SFX/Help") )
    , pIds( 0 )
{
    Sequence< OUString > aNames = GetPropertyNames();
    Sequence< Any > aValues = GetProperties( aNames );
    EnableNotification( aNames );
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT( aValues.getLength() == aNames.getLength(), "GetProperties failed" );
    if ( aValues.getLength() == aNames.getLength() )
    {
        for ( int nProp = 0; nProp < aNames.getLength(); nProp++ )
        {
            DBG_ASSERT( pValues[nProp].hasValue(), "property value missing" );
            if ( pValues[nProp].hasValue() )
            {
                switch ( nProp )
                {
                    case STARTERLIST :
                    {
                        ::rtl::OUString aCodedList;
                        if ( pValues[nProp] >>= aCodedList )
                        {
                            String aTmp( aCodedList );
                            USHORT nCount = aTmp.GetTokenCount( ',' );
                            pIds = new SvULongsSort();
                            for ( USHORT n=0; n<nCount; n++ )
                                pIds->Insert( (ULONG) aTmp.GetToken( n, ',' ).ToInt64() );
                        }

                        else
                            DBG_ERROR( "Wrong Type!" );
                        break;
                    }

                    default:
                        DBG_ERROR( "Wrong Type!" );
                        break;
                }
            }
        }
    }
}

SfxHelpOptions_Impl::~SfxHelpOptions_Impl()
{
    delete pIds;
}

class SfxHelp_Impl
{
private:
    String      m_aDBPath;
    String      m_aModule;
    Db*         m_pDB;
    sal_Bool    m_bIsDebug;
    sal_Bool    m_bIsOpen;
    SfxHelpOptions_Impl* m_pOpt;

public:
    SfxHelp_Impl( const String& rPath, sal_Bool bDebug );
    ~SfxHelp_Impl();

    String      GetHelpText( ULONG nHelpId, const String& rModule );
    SfxHelpOptions_Impl* GetOptions();
};

SfxHelp_Impl::SfxHelp_Impl( const String& rPath, sal_Bool bDebug ) :

    m_aDBPath   ( rPath ),
    m_pDB       ( NULL ),
    m_bIsDebug  ( bDebug ),
    m_bIsOpen   ( sal_False ),
    m_pOpt      ( 0 )

{
}

SfxHelp_Impl::~SfxHelp_Impl()
{
    if ( m_bIsOpen )
        m_pDB->close(0);
    delete m_pDB;
    delete m_pOpt;
}

SfxHelpOptions_Impl* SfxHelp_Impl::GetOptions()
{
    if ( !m_pOpt )
        m_pOpt = new SfxHelpOptions_Impl;
    return m_pOpt;
}

String SfxHelp_Impl::GetHelpText( ULONG nHelpId, const String& rModule )
{
    sal_Bool bOpenDB = sal_True;
    String aHelpText;
    ByteString aPathStr;

    if ( !m_pDB )
        m_pDB = new Db( NULL, 0 );

    if ( m_aModule != rModule )
    {
        m_aModule = rModule;
        INetURLObject aPath( m_aDBPath );
        aPath.insertName( rModule );
        aPath.setExtension( DEFINE_CONST_UNICODE("ht") );
        aPathStr = ByteString( aPath.getFSysPath( INetURLObject::FSYS_DETECT ), osl_getThreadTextEncoding() );
        try
        {
            int nError = m_pDB->open( aPathStr.GetBuffer(), NULL, DB_BTREE, DB_RDONLY, 0664 );

            if ( nError )
            {
                bOpenDB = sal_False;
                m_aModule.Erase();
                if ( m_bIsDebug )
                {
                    aHelpText = ERROR_TAG;
                    aHelpText += String::CreateFromInt32( nError );
                }
            }
            else if ( !m_bIsOpen )
                m_bIsOpen = sal_True;
        }
        catch( DbException& eDb )
        {
            bOpenDB = sal_False;
            m_aModule.Erase();
            if ( m_bIsDebug )
            {
                aHelpText = ERROR_TAG;
                aHelpText += String::CreateFromAscii( eDb.what() );
            }
        }
    }

    if ( bOpenDB )
    {
        ByteString aKeyStr = ByteString::CreateFromInt64( nHelpId );
        Dbt aKey( (void*)aKeyStr.GetBuffer(), aKeyStr.Len() );
        Dbt aData;
        aData.set_flags( DB_DBT_MALLOC );
        try
        {
            if ( !m_pDB->get( NULL, &aKey, &aData, 0 ) )
            {
                ByteString aHelpStr( (sal_Char*)aData.get_data(), aData.get_size() );
                aHelpText = String( aHelpStr, RTL_TEXTENCODING_UTF8 );
            }
        }
        catch( DbException& eDb )
        {
            if ( m_bIsDebug )
            {
                aHelpText = ERROR_TAG;
                aHelpText += String::CreateFromAscii( eDb.what() );
                aHelpText += PATH_TAG;
                aHelpText += String::CreateFromAscii( aPathStr.GetBuffer() );
            }
        }
    }
    else if ( m_bIsDebug )
    {
        aHelpText += PATH_TAG;
        aHelpText += String::CreateFromAscii( aPathStr.GetBuffer() );
    }

    return aHelpText;
}

void AppendConfigToken_Impl( String& rURL, sal_Bool bQuestionMark )
{
    Any aLocale = ::utl::ConfigManager::GetConfigManager()->GetDirectConfigProperty( ::utl::ConfigManager::LOCALE );
    ::rtl::OUString aLocaleStr;
    if ( !( aLocale >>= aLocaleStr ) )
        aLocaleStr = ::rtl::OUString( DEFINE_CONST_UNICODE("en") );

    SvtHelpOptions aHelpOpt;
    if ( bQuestionMark )
        rURL += '?';
    else
        rURL += '&';
    rURL += DEFINE_CONST_UNICODE("Language=");
    rURL += String( aLocaleStr );
    rURL += DEFINE_CONST_UNICODE("&System=");
    rURL += aHelpOpt.GetSystem();
}

SfxHelp::SfxHelp() :

    pImp     ( NULL ),
    bIsDebug( sal_False )

{
    char* pEnv = getenv( "help_debug" );
    if ( pEnv )
        bIsDebug = sal_True;

    Any aLocale = ::utl::ConfigManager::GetConfigManager()->GetDirectConfigProperty( ::utl::ConfigManager::LOCALE );
    ::rtl::OUString aLocaleStr;
    if ( !( aLocale >>= aLocaleStr ) )
        aLocaleStr = ::rtl::OUString( DEFINE_CONST_UNICODE("en") );
    sal_Int32 nSepPos = aLocaleStr.indexOf( '_' );
    if ( nSepPos != -1 )
    {
        aLanguageStr = aLocaleStr.copy( 0, nSepPos );
        aCountryStr = aLocaleStr.copy( nSepPos+1 );
    }
    else
    {
        nSepPos = aLocaleStr.indexOf( '-' );
        if ( nSepPos != -1 )
        {
            aLanguageStr = aLocaleStr.copy( 0, nSepPos );
            aCountryStr = aLocaleStr.copy( nSepPos+1 );
        }
        else
        {
            aLanguageStr = aLocaleStr;
        }
    }

    INetURLObject aPath( SvtPathOptions().GetHelpPath(), INET_PROT_FILE );
    aPath.insertName( aLanguageStr );
    pImp = new SfxHelp_Impl( aPath.GetMainURL(), bIsDebug );
}

SfxHelp::~SfxHelp()
{
    delete pImp;
}

String SfxHelp::GetHelpModuleName( ULONG nHelpId )
{
    String aModuleName;
    SfxViewFrame *pViewFrame = SfxViewFrame::Current();
    if ( pViewFrame )
    {
        // Wenn es ein Slot ist, kann es sein, da\s internes InPlace vorliegt
        // und eine Container-SlotId gefragt ist
        if (nHelpId >= (ULONG) SID_SFX_START && nHelpId <= (ULONG) SHRT_MAX)
        {
            if ( pViewFrame->GetParentViewFrame_Impl() )
            {
                // Ist es ein ContainerSlot ?
                const SfxSlot* pSlot = SFX_APP()->GetSlotPool(pViewFrame).GetSlot( (USHORT) nHelpId );
                if ( !pSlot || pSlot->IsMode( SFX_SLOT_CONTAINER ) )
                    pViewFrame = pViewFrame->GetParentViewFrame_Impl();
            }
        }

        if( pViewFrame->GetObjectShell() )
            aModuleName = String::CreateFromAscii( pViewFrame->GetObjectShell()->GetFactory().GetShortName() );
    }

    // cut sub factoryname, if necessary
    xub_StrLen nPos = aModuleName.Search( '/' );
    if ( nPos != STRING_NOTFOUND )
        aModuleName.Erase( nPos );

    return aModuleName;
}

BOOL SfxHelp::Start( const String& rURL, const Window* pWindow )
{
    Reference < XTasksSupplier > xDesktop( ::comphelper::getProcessServiceFactory()->createInstance(
                DEFINE_CONST_UNICODE("com.sun.star.frame.Desktop") ), UNO_QUERY );

    Reference < XTask > xActiveTask = xDesktop->getActiveTask();
    if ( !xActiveTask.is() )
        return FALSE;

    // try to find the help frame
    ::rtl::OUString aTarget = ::rtl::OUString( DEFINE_CONST_UNICODE("OFFICE_HELP") );
    Reference < XDispatchProvider > xFrame( xActiveTask->findFrame( aTarget, FrameSearchFlag::GLOBAL ), UNO_QUERY );
    Sequence < PropertyValue > aProps;
    sal_Int32 nFlag = FrameSearchFlag::GLOBAL;
    if ( aTicket.Len() )
    {
        xFrame = Reference < XDispatchProvider > ( xActiveTask, UNO_QUERY );
    }
    else
    {
        // otherwise the URL can be dispatched to the help frame
        if ( !xFrame.is() )
        {
            Reference < XFrame > xTask = xActiveTask->findFrame( DEFINE_CONST_UNICODE( "_blank" ), 0 );
            xFrame = Reference < XDispatchProvider >( xTask, UNO_QUERY );
            Window* pWin = VCLUnoHelper::GetWindow( xTask->getContainerWindow() );
            pWin->SetText( String( SfxResId( STR_HELP_WINDOW_TITLE ) ) );
            SfxHelpWindow_Impl* pHlpWin = new SfxHelpWindow_Impl( xTask, pWin, WB_DOCKBORDER );
            pHlpWin->Show();
            Reference< ::com::sun::star::awt::XWindow > xWindow = VCLUnoHelper::GetInterface( pHlpWin );
            xWindow->setPosSize( 50, 50, 300, 200, ::com::sun::star::awt::PosSize::SIZE );
            if ( !xTask->setComponent( xWindow, Reference < XController >() ) )
                return FALSE;
            else
            {
                pHlpWin->setContainerWindow( xTask->getContainerWindow() );
//                pHlpWin->SetFactory( aHelpModuleName, sal_True );
                xTask->getContainerWindow()->setVisible( sal_True );

                pHlpWin->FirstOpenMessage();
            }
        }
    }

    if ( xFrame.is() )
    {
        ::com::sun::star::util::URL aURL;
        aURL.Complete = rURL;
        Reference < XURLTransformer > xTrans( ::comphelper::getProcessServiceFactory()->createInstance(
                DEFINE_CONST_UNICODE("com.sun.star.util.URLTransformer" )), UNO_QUERY );
        xTrans->parseStrict( aURL );
        Reference < XDispatch > xDispatch = xFrame->queryDispatch( aURL,
                                DEFINE_CONST_UNICODE("OFFICE_HELP"), FrameSearchFlag::ALL );
        if ( xDispatch.is() )
        {
            xDispatch->dispatch( aURL, aProps );
        }

        return TRUE;
    }
    else
        return FALSE;
}

BOOL SfxHelp::Start( ULONG nHelpId, const Window* pWindow )
{
    String aHelpModuleName( GetHelpModuleName( nHelpId ) );
    String aHelpURL = CreateHelpURL( nHelpId, aHelpModuleName );
    return Start( aHelpURL, pWindow );
}

XubString SfxHelp::GetHelpText( ULONG nHelpId, const Window* pWindow )
{
    String aModuleName = GetHelpModuleName( nHelpId );
    XubString aHelpText = pImp->GetHelpText( nHelpId, aModuleName );;
    if ( bIsDebug )
    {
        aHelpText += DEFINE_CONST_UNICODE("\n\n");
        aHelpText += aModuleName;
        aHelpText += DEFINE_CONST_UNICODE(" - ");
        aHelpText += String::CreateFromInt64( nHelpId );
    }

    return aHelpText;
}

String SfxHelp::CreateHelpURL( ULONG nHelpId, const String& rModuleName )
{
    // build up the help URL
    String aHelpURL;
    if ( aTicket.Len() )
    {
        // if there is a ticket, we are inside a plugin, so a special Help URL must be sent
        aHelpURL = DEFINE_CONST_UNICODE("vnd.sun.star.cmd:help?");
        aHelpURL += DEFINE_CONST_UNICODE("HELP_Request_Mode=contextIndex&HELP_Session_Mode=context&HELP_CallMode=portal&HELP_Device=html");

        if ( !nHelpId )
        {
            aHelpURL += DEFINE_CONST_UNICODE("&startId=go");
        }
        else
        {
            aHelpURL += DEFINE_CONST_UNICODE("&HELP_ContextID=-");
            aHelpURL += String::CreateFromInt64( nHelpId );
        }

        aHelpURL += DEFINE_CONST_UNICODE("&HELP_ProgramID=");
        aHelpURL += rModuleName;
        aHelpURL += DEFINE_CONST_UNICODE("&HELP_User=");
        aHelpURL += aUser;
        aHelpURL += DEFINE_CONST_UNICODE("&HELP_Ticket=");
        aHelpURL += aTicket;
        aHelpURL += DEFINE_CONST_UNICODE("&HELP_Language=");
        aHelpURL += aLanguageStr;
        if ( aCountryStr.Len() )
        {
            aHelpURL += DEFINE_CONST_UNICODE("&HELP_Country=");
            aHelpURL += aCountryStr;
        }
    }
    else
    {
        aHelpURL = String::CreateFromAscii("vnd.sun.star.help://");
        aHelpURL += rModuleName;

        if ( !nHelpId )
        {
            aHelpURL += String( DEFINE_CONST_UNICODE("/start") );
        }
        else
        {
            aHelpURL += '/';
            aHelpURL += String::CreateFromInt64( nHelpId );
        }

        AppendConfigToken_Impl( aHelpURL, sal_True );
    }

    return aHelpURL;
}

void SfxHelp::OpenHelpAgent( SfxFrame *pFrame, ULONG nHelpId )
{
    SfxHelpOptions_Impl *pOpt = pImp->GetOptions();
    if ( !pOpt->HasId( nHelpId ) )
        return;

    try
    {
        URL aURL;
        aURL.Complete = CreateHelpURL( nHelpId, GetHelpModuleName( nHelpId ) );
        Reference < XURLTransformer > xTrans( ::comphelper::getProcessServiceFactory()->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer" )), UNO_QUERY );
        xTrans->parseStrict(aURL);

        Reference< XDispatchProvider > xDispProv( pFrame->GetTopFrame()->GetFrameInterface(), UNO_QUERY );
        Reference< XDispatch > xHelpDispatch;
        if (xDispProv.is())
            xHelpDispatch = xDispProv->queryDispatch(aURL, ::rtl::OUString::createFromAscii("_helpagent"), FrameSearchFlag::PARENT | FrameSearchFlag::SELF);

        DBG_ASSERT( xHelpDispatch.is(), "OpenHelpAgent: could not get a dispatcher!" );
        if ( xHelpDispatch.is() )
            xHelpDispatch->dispatch( aURL, Sequence< PropertyValue >() );
    }
    catch( const Exception& )
    {
        DBG_ASSERT( sal_False, "OpenHelpAgent: caught an exception while executing the dispatch!" );
    }
}

SfxHelp* SfxHelp::GetHelp()
{
    return (SfxHelp*) Application::GetHelp();
}
