/*************************************************************************
 *
 *  $RCSfile: newhelp.cxx,v $
 *
 *  $Revision: 1.27 $
 *
 *  last change: $Author: pb $ $Date: 2001-07-10 05:48:21 $
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

#include "newhelp.hxx"
#include "sfxuno.hxx"
#include "sfxresid.hxx"
#include "helpinterceptor.hxx"
#include "helper.hxx"
#include "msgpool.hxx"
#include "app.hxx"

#include "app.hrc"
#include "newhelp.hrc"
#include "helpid.hrc"

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTION_HPP_
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTY_HPP_
#include <com/sun/star/beans/Property.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_COMMANDABORTEDEXCEPTION_HPP_
#include <com/sun/star/ucb/CommandAbortedException.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif
#ifndef INCLUDED_SVTOOLS_VIEWOPTIONS_HXX
#include <svtools/viewoptions.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _CACHESTR_HXX
#include <tools/cachestr.hxx>
#endif
#ifndef _UNOTOOLS_STREAMHELPER_HXX_
#include <unotools/streamhelper.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_DYNAMICMENUOPTIONS_HXX
#include <svtools/dynamicmenuoptions.hxx>
#endif
#ifndef _SVTOOLS_IMAGEMGR_HXX
#include <svtools/imagemgr.hxx>
#endif
#include <ucbhelper/content.hxx>
#include <vcl/msgbox.hxx>

using namespace ::ucb;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace com::sun::star::ucb;

extern void AppendConfigToken_Impl( String& rURL, sal_Bool bQuestionMark ); // sfxhelp.cxx

// defines ---------------------------------------------------------------

#define SPLITSET_ID         0
#define COLSET_ID           1
#define INDEXWIN_ID         2
#define TEXTWIN_ID          3

#define TBI_INDEX           1001
#define TBI_START           1002
#define TBI_BACKWARD        1003
#define TBI_FORWARD         1004
#define TBI_CONTEXT         1005
#define TBI_PRINT           1006
#define TBI_BOOKMARKS       1007
#define TBI_SOURCEVIEW      1008

#define CONFIGNAME_HELPWIN      String(DEFINE_CONST_UNICODE("OfficeHelp"))
#define CONFIGNAME_INDEXWIN     String(DEFINE_CONST_UNICODE("OfficeHelpIndex"))
#define CONFIGNAME_SEARCHPAGE   String(DEFINE_CONST_UNICODE("OfficeHelpSearch"))
#define IMAGE_URL               String(DEFINE_CONST_UNICODE("private:factory/"))
#define PROPERTY_KEYWORDLIST    ::rtl::OUString(DEFINE_CONST_UNICODE("KeywordList"))
#define PROPERTY_KEYWORDREF     ::rtl::OUString(DEFINE_CONST_UNICODE("KeywordRef"))
#define PROPERTY_TITLE          ::rtl::OUString(DEFINE_CONST_UNICODE("Title"))
#define HELP_URL                ::rtl::OUString(DEFINE_CONST_UNICODE("vnd.sun.star.help://"))
#define HELP_SEARCH_TAG         ::rtl::OUString(DEFINE_CONST_UNICODE("/?Query="))

#define PARSE_URL( aURL ) \
    Reference < XURLTransformer > xTrans( ::comphelper::getProcessServiceFactory()->createInstance( \
            DEFINE_CONST_UNICODE("com.sun.star.util.URLTransformer" )), UNO_QUERY ); \
    xTrans->parseStrict( aURL )

#define GET_SLOT_NAME( nId ) \
    SFX_SLOTPOOL().GetSlotName_Impl( nId )

// class OpenStatusListener_Impl -----------------------------------------

void SAL_CALL OpenStatusListener_Impl::statusChanged( const FeatureStateEvent& Event ) throw(RuntimeException)
{
    m_bSuccess = Event.IsEnabled;
    m_bFinished = sal_True;
    m_aOpenLink.Call( this );
    m_xDispatch->removeStatusListener( this, Event.FeatureURL );
    m_aURL.Erase();
}

// -----------------------------------------------------------------------

void SAL_CALL OpenStatusListener_Impl::disposing( const EventObject& Source ) throw(RuntimeException)
{
    URL aURL;
    aURL.Complete = m_aURL;
    m_xDispatch->removeStatusListener( this, aURL );
}

// -----------------------------------------------------------------------

void OpenStatusListener_Impl::AddListener( Reference< XDispatch >& xDispatch,
                                           const ::com::sun::star::util::URL& aURL )
{
    m_aURL = aURL.Complete;
    m_xDispatch = xDispatch;
    m_xDispatch->addStatusListener( this, aURL );
}

// class ContentTabPage_Impl ---------------------------------------------

ContentTabPage_Impl::ContentTabPage_Impl( Window* pParent ) :

    TabPage( pParent ),

    aContentWin( this, WB_BORDER )

{
    aContentWin.SetPosPixel( Point( 5, 5 ) );
    aContentWin.SetBackground( Wallpaper( Color( COL_WHITE ) ) );
    aContentWin.Show();
}

// -----------------------------------------------------------------------

void ContentTabPage_Impl::Resize()
{
    Size aSize = GetOutputSizePixel();
    Point aPnt = aContentWin.GetPosPixel();
    aSize.Width() -= ( aPnt.X() * 2 );
    aSize.Height() -= ( aPnt.Y() * 2 );
    aContentWin.SetSizePixel( aSize );
}

// class IndexTabPage_Impl -----------------------------------------------

IndexTabPage_Impl::IndexTabPage_Impl( Window* pParent ) :

    TabPage( pParent, SfxResId( TP_HELP_INDEX ) ),

    aExpressionFT   ( this, ResId( FT_EXPRESSION ) ),
    aIndexCB        ( this, ResId( CB_INDEX ) ),
    aOpenBtn        ( this, ResId( PB_OPEN_INDEX ) )

{
    FreeResource();

    aIndexCB.EnableAutocomplete( TRUE );

    aOpenBtn.SetClickHdl( LINK( this, IndexTabPage_Impl, OpenHdl ) );
    aFactoryTimer.SetTimeoutHdl( LINK( this, IndexTabPage_Impl, FactoryHdl ) );
    aFactoryTimer.SetTimeout( 200 );

    nMinWidth = GetSizePixel().Width();
}

// -----------------------------------------------------------------------

IndexTabPage_Impl::~IndexTabPage_Impl()
{
    ClearIndex();
}

// -----------------------------------------------------------------------

void IndexTabPage_Impl::InitializeIndex()
{
    EnterWait();

    try
    {
        ::rtl::OUString aURL = HELP_URL;
        ::rtl::OUString _aFactory( aFactory );
        aURL += _aFactory;
        String aTemp = aURL;
        AppendConfigToken_Impl( aTemp, sal_True );
        aURL = aTemp;
        Content aCnt( aURL, Reference< ::com::sun::star::ucb::XCommandEnvironment > () );
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > xInfo = aCnt.getProperties();
        if ( xInfo->hasPropertyByName( PROPERTY_KEYWORDLIST ) )
        {
            ::com::sun::star::uno::Any aAny1 = aCnt.getPropertyValue( PROPERTY_KEYWORDLIST );
            ::com::sun::star::uno::Sequence< ::rtl::OUString > aKeywordList;
            ::com::sun::star::uno::Any aAny2 = aCnt.getPropertyValue( PROPERTY_KEYWORDREF );
            ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::rtl::OUString > > aKeywordRefList;
            if ( ( aAny1 >>= aKeywordList ) && ( aAny2 >>= aKeywordRefList ) )
            {
                const ::rtl::OUString* pKeywords  = aKeywordList.getConstArray();
                const ::com::sun::star::uno::Sequence< ::rtl::OUString >* pRefs = aKeywordRefList.getConstArray();
                sal_Int32 i, nCount = aKeywordList.getLength();
                DBG_ASSERT( aKeywordRefList.getLength() == nCount, "keywordlist and reflist with different length" );
                USHORT nPos;
                String aIndex, aSubIndex;

                for ( i = 0; i < nCount; ++i )
                {
                    ::com::sun::star::uno::Sequence< ::rtl::OUString > aRefList = pRefs[i];
                    const ::rtl::OUString* pRef  = aRefList.getConstArray();
                    sal_Int32 j, nRefCount = aRefList.getLength();

                    String aKeywordPair( pKeywords[i] );
                    xub_StrLen nTokenCount = aKeywordPair.GetTokenCount();
                    if ( 1 == nTokenCount )
                    {
                        for ( j = 0; j < nRefCount; ++j )
                        {
                            nPos = aIndexCB.InsertEntry( aKeywordPair );
                            String* pData = new String( pRef[j] );
                            aIndexCB.SetEntryData( nPos, (void*)(ULONG)pData );
                        }
                    }
                    else if ( 2 == nTokenCount )
                    {
                        xub_StrLen nIdx = 0;
                        String aToken = aKeywordPair.GetToken( 0, ';', nIdx );
                        if ( aIndex != aToken )
                        {
                            aIndex = aToken;
                            aIndexCB.InsertEntry( aIndex );
                        }
                        String aSubIndex( DEFINE_CONST_UNICODE("  ") );
                        aSubIndex += aKeywordPair.GetToken( 0, ';', nIdx );
                        for ( j = 0; j < nRefCount; ++j )
                        {
                            nPos = aIndexCB.InsertEntry( aSubIndex );
                            String* pData = new String( pRef[j] );
                            aIndexCB.SetEntryData( nPos, (void*)(ULONG)pData );
                        }
                    }
                    else
                    {
                        DBG_ERRORFILE( "unexpected token count of a keyword" );
                    }
                }
            }
        }
    }
    catch( ::com::sun::star::ucb::ContentCreationException& )
    {
        DBG_ERRORFILE( "content creation exception" );
    }
    catch( ::com::sun::star::ucb::CommandAbortedException& )
    {
        DBG_ERRORFILE( "command aborted exception" );
    }
    catch( ::com::sun::star::uno::RuntimeException& )
    {
        DBG_ERRORFILE( "runtime exception" );
    }
    catch( ::com::sun::star::uno::Exception& )
    {
        DBG_ERRORFILE( "Any other exception" );
    }

    LeaveWait();
}

// -----------------------------------------------------------------------

void IndexTabPage_Impl::ClearIndex()
{
    USHORT nCount = aIndexCB.GetEntryCount();
    for ( USHORT i = 0; i < nCount; ++i )
        delete (String*)(ULONG)aIndexCB.GetEntryData(i);
    aIndexCB.Clear();
    aIndexCB.Update();
}

// -----------------------------------------------------------------------

IMPL_LINK( IndexTabPage_Impl, OpenHdl, PushButton*, EMPTYARG )
{
    aIndexCB.GetDoubleClickHdl().Call( &aIndexCB );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( IndexTabPage_Impl, FactoryHdl, Timer*, EMPTYARG )
{
    InitializeIndex();
    return 0;
}

// -----------------------------------------------------------------------

void IndexTabPage_Impl::Resize()
{
    Size aSize = GetSizePixel();
    long nWidth = aExpressionFT.GetPosPixel().X() + aExpressionFT.GetSizePixel().Width();
    if ( aSize.Width() > nMinWidth || nWidth > aSize.Width() )
    {
        Point aPnt = aExpressionFT.GetPosPixel();
        long nDelta = ( aPnt.X() / 2 );
        Size aNewSize = aExpressionFT.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aExpressionFT.SetSizePixel( aNewSize );

        Size a6Size = LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
        Size aBtnSize = aOpenBtn.GetSizePixel();

        aPnt = aIndexCB.GetPosPixel();
        aNewSize = aIndexCB.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aNewSize.Height() = aSize.Height() - aPnt.Y();
        aNewSize.Height() -= ( aBtnSize.Height() + ( a6Size.Height() * 3 / 2 ) );
        aIndexCB.SetSizePixel( aNewSize );

        aPnt.X() += ( aNewSize.Width() - aBtnSize.Width() );
        aPnt.Y() += aNewSize.Height() + ( a6Size.Height() / 2 );
        aOpenBtn.SetPosPixel( aPnt );
    }
}

// -----------------------------------------------------------------------

void IndexTabPage_Impl::SetDoubleClickHdl( const Link& rLink )
{
    aIndexCB.SetDoubleClickHdl( rLink );
}

// -----------------------------------------------------------------------

void IndexTabPage_Impl::SetFactory( const String& rFactory )
{
    if ( rFactory != aFactory )
    {
        aFactory = rFactory;
        ClearIndex();
        aFactoryTimer.Start();
    }
}

// -----------------------------------------------------------------------

String IndexTabPage_Impl::GetSelectEntry() const
{
    String aRet;
    String* pData = (String*)(ULONG)aIndexCB.GetEntryData( aIndexCB.GetEntryPos( aIndexCB.GetText() ) );
    if ( pData )
        aRet = String( *pData );
    return aRet;
}

// class SearchBox_Impl --------------------------------------------------

long SearchBox_Impl::PreNotify( NotifyEvent& rNEvt )
{
    sal_Bool bHandled = sal_False;
    if ( !IsInDropDown() &&
         rNEvt.GetWindow() == GetSubEdit() &&
         rNEvt.GetType() == EVENT_KEYINPUT &&
         KEY_RETURN == rNEvt.GetKeyEvent()->GetKeyCode().GetCode() )
    {
        aSearchLink.Call( NULL );
        bHandled = sal_True;
    }
    return bHandled ? 1 : ComboBox::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

void SearchBox_Impl::Select()
{
    if ( !IsTravelSelect() )
        aSearchLink.Call( NULL );
}

// class SearchTabPage_Impl ----------------------------------------------

SearchTabPage_Impl::SearchTabPage_Impl( Window* pParent ) :

    TabPage( pParent, SfxResId( TP_HELP_SEARCH ) ),

    aSearchFT   ( this, ResId( FT_SEARCH ) ),
    aSearchED   ( this, ResId( ED_SEARCH ) ),
    aSearchBtn  ( this, ResId( PB_SEARCH ) ),
    aResultsLB  ( this, ResId( LB_RESULT ) ),
    aOpenBtn    ( this, ResId( PB_OPEN_SEARCH ) ),
    aScopeCB    ( this, ResId( CB_SCOPE ) )

{
    FreeResource();

    Link aLink = LINK( this, SearchTabPage_Impl, SearchHdl );
    aSearchED.SetSearchLink( aLink );
    aSearchBtn.SetClickHdl( aLink );
    aOpenBtn.SetClickHdl( LINK( this, SearchTabPage_Impl, OpenHdl ) );

    aMinSize = GetSizePixel();

    SvtViewOptions aViewOpt( E_TABPAGE, CONFIGNAME_SEARCHPAGE );
    if ( aViewOpt.Exists() )
    {
        String aUserData = aViewOpt.GetUserData();
        BOOL bChecked = ( 1 == aUserData.GetToken(0).ToInt32() ) ? TRUE : FALSE;
        aScopeCB.Check( bChecked );

        for ( USHORT i = 1; i < aUserData.GetTokenCount(); ++i )
        {
            String aToken = aUserData.GetToken(i);
            aSearchED.InsertEntry( INetURLObject::decode(
                aToken, '%', INetURLObject::DECODE_WITH_CHARSET ) );
        }
    }

    if ( aSearchED.GetEntryCount() )
        aSearchED.SetText( aSearchED.GetEntry(0) );
}

// -----------------------------------------------------------------------

SearchTabPage_Impl::~SearchTabPage_Impl()
{
    SvtViewOptions aViewOpt( E_TABPAGE, CONFIGNAME_SEARCHPAGE );
    sal_Int32 nChecked = aScopeCB.IsChecked() ? 1 : 0;
    String aUserData = String::CreateFromInt32( nChecked );
    aUserData += ';';
    USHORT nCount = Min( aSearchED.GetEntryCount(), (USHORT)10 );  // save only 10 entries

    for ( USHORT i = 0; i < nCount; ++i )
    {
        String aText = aSearchED.GetEntry(i);
        aUserData += INetURLObject::encode(
            aText, INetURLObject::PART_UNO_PARAM_VALUE, '%', INetURLObject::ENCODE_ALL );
        aUserData += ';';
    }

    aUserData.EraseTrailingChars(';');
    aViewOpt.SetUserData( aUserData );
}

// -----------------------------------------------------------------------

void SearchTabPage_Impl::ClearSearchResults()
{
    USHORT nCount = aResultsLB.GetEntryCount();
    for ( USHORT i = 0; i < nCount; ++i )
        delete (String*)(ULONG)aResultsLB.GetEntryData(i);
    aResultsLB.Clear();
    aResultsLB.Update();
}

// -----------------------------------------------------------------------

void SearchTabPage_Impl::RememberSearchText( const String& rSearchText )
{
    for ( USHORT i = 0; i < aSearchED.GetEntryCount(); ++i )
    {
        if ( rSearchText == aSearchED.GetEntry(i) )
        {
            aSearchED.RemoveEntry(i);
            break;
        }
    }

    aSearchED.InsertEntry( rSearchText, 0 );
}

// -----------------------------------------------------------------------

IMPL_LINK( SearchTabPage_Impl, SearchHdl, PushButton*, EMPTYARG )
{
    EnterWait();
    ClearSearchResults();

    String aSearchText = aSearchED.GetText();
    RememberSearchText( aSearchText );
    String aSearchURL = HELP_URL;
    aSearchURL += aFactory;
    aSearchURL += String( HELP_SEARCH_TAG );
    aSearchURL += aSearchText;
    AppendConfigToken_Impl( aSearchURL, sal_False );
    if ( aScopeCB.IsChecked() )
        aSearchURL += DEFINE_CONST_UNICODE("&Scope=Heading");

    Sequence< ::rtl::OUString > aFactories = SfxContentHelper::GetResultSet( aSearchURL );
    const ::rtl::OUString* pFacs  = aFactories.getConstArray();
    UINT32 i, nCount = aFactories.getLength();
    for ( i = 0; i < nCount; ++i )
    {
        String aRow( pFacs[i] );
        String aTitle, aType;
        xub_StrLen nIdx = 0;
        aTitle = aRow.GetToken( 0, '\t', nIdx );
        aType = aRow.GetToken( 0, '\t', nIdx );
        String* pURL = new String( aRow.GetToken( 0, '\t', nIdx ) );
        USHORT nPos = aResultsLB.InsertEntry( aTitle );
        aResultsLB.SetEntryData( nPos, (void*)(ULONG)pURL );
    }

    LeaveWait();
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SearchTabPage_Impl, OpenHdl, PushButton*, EMPTYARG )
{
    aResultsLB.GetDoubleClickHdl().Call( &aResultsLB );
    return 0;
}

// -----------------------------------------------------------------------

void SearchTabPage_Impl::Resize()
{
    Size aSize = GetSizePixel();
    long nWidth = aSearchBtn.GetPosPixel().X() + aSearchBtn.GetSizePixel().Width();
    if ( aSize.Width() > aMinSize.Width() || nWidth > aSize.Width() )
    {
        Point aPnt = aSearchFT.GetPosPixel();
        Size aNewSize = aSearchFT.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aSearchFT.SetSizePixel( aNewSize );
        aNewSize.Height() = aResultsLB.GetSizePixel().Height();
        aResultsLB.SetSizePixel( aNewSize );
        aNewSize.Height() = aScopeCB.GetSizePixel().Height();
        aScopeCB.SetSizePixel( aNewSize );

        aNewSize = aSearchED.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 ) -
                           ( aSearchBtn.GetSizePixel().Width() + ( aPnt.X() / 2 ) );
        aSearchED.SetSizePixel( aNewSize );
        Point aNewPnt = aSearchBtn.GetPosPixel();
        aNewPnt.X() = aPnt.X() + aNewSize.Width() + ( aPnt.X() / 2 );
        aSearchBtn.SetPosPixel( aNewPnt );
    }

    if ( aSize.Height() > aMinSize.Height() )
    {
        Size a6Size = LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
        long n3Height = a6Size.Height() / 2;
        Size aBtnSize = aOpenBtn.GetSizePixel();
        long nExtraHeight = aBtnSize.Height() + n3Height;

        Point aPnt = aResultsLB.GetPosPixel();
        Size aNewSize = aResultsLB.GetSizePixel();
        aNewSize.Height() = aSize.Height() - aPnt.Y();
        aNewSize.Height() -= ( nExtraHeight + ( a6Size.Height() * 3 / 2 ) );
        aResultsLB.SetSizePixel( aNewSize );

        aPnt.X() += ( aNewSize.Width() - aBtnSize.Width() );
        aPnt.Y() += aNewSize.Height() + a6Size.Height();
        aOpenBtn.SetPosPixel( aPnt );
    }
}

// -----------------------------------------------------------------------

void SearchTabPage_Impl::SetDoubleClickHdl( const Link& rLink )
{
    aResultsLB.SetDoubleClickHdl( rLink );
}

// -----------------------------------------------------------------------

String SearchTabPage_Impl::GetSelectEntry() const
{
    String aRet;
    String* pData = (String*)(ULONG)aResultsLB.GetEntryData( aResultsLB.GetSelectEntryPos() );
    if ( pData )
        aRet = String( *pData );
    return aRet;
}

// class BookmarksTabPage_Impl -------------------------------------------

void GetMenuEntry_Impl
(
    Sequence< PropertyValue >& aDynamicMenuEntry,
    ::rtl::OUString& rTitle,
    ::rtl::OUString& rURL,
    ::rtl::OUString& rFrame,
    ::rtl::OUString& rImageId
)
{
    for ( int i = 0; i < aDynamicMenuEntry.getLength(); i++ )
    {
        if ( aDynamicMenuEntry[i].Name == DYNAMICMENU_PROPERTYNAME_URL )
            aDynamicMenuEntry[i].Value >>= rURL;
        else if ( aDynamicMenuEntry[i].Name == DYNAMICMENU_PROPERTYNAME_TITLE )
            aDynamicMenuEntry[i].Value >>= rTitle;
        else if ( aDynamicMenuEntry[i].Name == DYNAMICMENU_PROPERTYNAME_IMAGEIDENTIFIER )
            aDynamicMenuEntry[i].Value >>= rImageId;
        else if ( aDynamicMenuEntry[i].Name == DYNAMICMENU_PROPERTYNAME_TARGETNAME )
            aDynamicMenuEntry[i].Value >>= rFrame;
    }
}

BookmarksBox_Impl::BookmarksBox_Impl( Window* pParent, const ResId& rResId ) :

    ListBox( pParent, rResId )

{
}

BookmarksBox_Impl::~BookmarksBox_Impl()
{
    // save bookmarks to configuration
    SvtDynamicMenuOptions aMenuOpt;
    aMenuOpt.Clear( E_HELPBOOKMARKS );
    rtl::OUString aEmpty;
    USHORT nCount = GetEntryCount();
    for ( USHORT i = 0; i < nCount; ++i )
    {
        String aTitle = GetEntry(i);
        String* pURL = (String*)(ULONG)GetEntryData(i);
        aMenuOpt.AppendItem( E_HELPBOOKMARKS, rtl::OUString( *pURL ), rtl::OUString( aTitle ), aEmpty, aEmpty );

        delete pURL;
    }
}

void BookmarksBox_Impl::DoAction( USHORT nAction )
{
    switch ( nAction )
    {
        case MID_OPEN :
            GetDoubleClickHdl().Call( NULL );
            break;

        case MID_RENAME :
           {
            USHORT nPos = GetSelectEntryPos();
            if ( nPos != LISTBOX_ENTRY_NOTFOUND )
            {
                SfxAddHelpBookmarkDialog_Impl aDlg( this, sal_True );
                aDlg.SetTitle( GetEntry( nPos ) );
                if ( aDlg.Execute() == RET_OK )
                {
                    String* pURL = (String*)(ULONG)GetEntryData( nPos );
                    RemoveEntry( nPos );
                    String aImageURL = IMAGE_URL;
                    aImageURL += INetURLObject( *pURL ).GetHost();
                    InsertEntry( aDlg.GetTitle(), SvFileInformationManager::GetImage( aImageURL ), nPos );
                    SetEntryData( nPos, (void*)(ULONG)( new String( *pURL ) ) );
                    delete pURL;
                }
            }
            break;
        }

        case MID_DELETE :
        {
            USHORT nPos = GetSelectEntryPos();
            if ( nPos != LISTBOX_ENTRY_NOTFOUND )
            {
                RemoveEntry( nPos );
                USHORT nCount = GetEntryCount();
                if ( nCount )
                {
                    if ( nPos >= nCount )
                        nPos = nCount - 1;
                    SelectEntryPos( nPos );
                }
            }
            break;
        }
    }
}

long BookmarksBox_Impl::Notify( NotifyEvent& rNEvt )
{
    long nRet = 0;
    USHORT nType = rNEvt.GetType();
    if ( EVENT_KEYINPUT == nType )
    {
        const KeyCode& rKey = rNEvt.GetKeyEvent()->GetKeyCode();

        if ( rKey.GetCode() == KEY_DELETE && GetEntryCount() > 0 )
            DoAction( MID_DELETE );
    }
    else if ( EVENT_COMMAND == nType )
    {
        const CommandEvent* pCEvt = rNEvt.GetCommandEvent();
        if ( pCEvt->GetCommand() == COMMAND_CONTEXTMENU )
        {
            PopupMenu aMenu( SfxResId( MENU_HELP_BOOKMARKS ) );
            sal_uInt16 nId = aMenu.Execute( this, pCEvt->GetMousePosPixel() );
            if ( nId != MENU_ITEM_NOTFOUND )
                DoAction( nId );
        }
    }
    return nRet ? nRet : ListBox::Notify( rNEvt );
}

// class BookmarksTabPage_Impl -------------------------------------------

BookmarksTabPage_Impl::BookmarksTabPage_Impl( Window* pParent ) :

    TabPage( pParent, SfxResId( TP_HELP_BOOKMARKS ) ),

    aBookmarksFT    ( this, ResId( FT_BOOKMARKS ) ),
    aBookmarksBox   ( this, ResId( LB_BOOKMARKS ) ),
    aBookmarksPB    ( this, ResId( PB_BOOKMARKS ) )

{
    FreeResource();

    nMinWidth = GetSizePixel().Width();

    // load bookmarks from configuration
    Sequence< Sequence< PropertyValue > > aDynamicMenuEntries;
    aDynamicMenuEntries = SvtDynamicMenuOptions().GetMenu( E_HELPBOOKMARKS );

    ::rtl::OUString aTitle;
    ::rtl::OUString aURL;
    ::rtl::OUString aImageURL;
    ::rtl::OUString aTargetFrame;

    UINT32 i, nCount = aDynamicMenuEntries.getLength();
    for ( i = 0; i < nCount; ++i )
    {
        GetMenuEntry_Impl( aDynamicMenuEntries[i], aTitle, aURL, aTargetFrame, aImageURL );
        AddBookmarks( aTitle, aURL );
    }

}

void BookmarksTabPage_Impl::Resize()
{
    Size aSize = GetSizePixel();
    long nWidth = aBookmarksFT.GetPosPixel().X() + aBookmarksFT.GetSizePixel().Width();
    if ( aSize.Width() > nMinWidth || nWidth > aSize.Width() )
    {
        Point aPnt = aBookmarksFT.GetPosPixel();
        long nDelta = ( aPnt.X() / 2 );
        Size aNewSize = aBookmarksFT.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aBookmarksFT.SetSizePixel( aNewSize );

        Size a6Size = LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
        Size aBtnSize = aBookmarksPB.GetSizePixel();

        aPnt = aBookmarksBox.GetPosPixel();
        aNewSize = aBookmarksBox.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aNewSize.Height() = aSize.Height() - aPnt.Y();
        aNewSize.Height() -= ( aBtnSize.Height() + ( a6Size.Height() * 3 / 2 ) );
        aBookmarksBox.SetSizePixel( aNewSize );

        aPnt.X() += ( aNewSize.Width() - aBtnSize.Width() );
        aPnt.Y() += aNewSize.Height() + ( a6Size.Height() / 2 );
        aBookmarksPB.SetPosPixel( aPnt );
    }
}

// -----------------------------------------------------------------------

void BookmarksTabPage_Impl::SetDoubleClickHdl( const Link& rLink )
{
    aBookmarksBox.SetDoubleClickHdl( rLink );
}

// -----------------------------------------------------------------------

String BookmarksTabPage_Impl::GetSelectEntry() const
{
    String aRet;
    String* pData = (String*)(ULONG)aBookmarksBox.GetEntryData( aBookmarksBox.GetSelectEntryPos() );
    if ( pData )
        aRet = String( *pData );
    return aRet;
}

// -----------------------------------------------------------------------

void BookmarksTabPage_Impl::AddBookmarks( const String& rTitle, const String& rURL )
{
    String aImageURL = IMAGE_URL;
    aImageURL += INetURLObject( rURL ).GetHost();
    USHORT nPos = aBookmarksBox.InsertEntry( rTitle, SvFileInformationManager::GetImage( aImageURL ) );
    aBookmarksBox.SetEntryData( nPos, (void*)(ULONG)( new String( rURL ) ) );
}

// class SfxHelpIndexWindow_Impl -----------------------------------------

SfxHelpIndexWindow_Impl::SfxHelpIndexWindow_Impl( Window* pParent ) :

    Window( pParent, SfxResId( WIN_HELP_INDEX ) ),

    aActiveLB   ( this, ResId( LB_ACTIVE ) ),
    aActiveLine ( this, ResId( FL_ACTIVE ) ),
    aTabCtrl    ( this, ResId( TC_INDEX ) ),

    pCPage      ( NULL ),
    pIPage      ( NULL ),
    pSPage      ( NULL ),
    pFPage      ( NULL )

{
    FreeResource();

    aTabCtrl.SetActivatePageHdl( LINK( this, SfxHelpIndexWindow_Impl, ActivatePageHdl ) );
    aTabCtrl.Show();

    sal_Int32 nPageId = HELP_INDEX_PAGE_INDEX;
    SvtViewOptions aViewOpt( E_TABDIALOG, CONFIGNAME_INDEXWIN );
    if ( aViewOpt.Exists() )
        nPageId = aViewOpt.GetPageID();
    aTabCtrl.SetCurPageId( (USHORT)nPageId );
    ActivatePageHdl( &aTabCtrl );
    aActiveLB.SetSelectHdl( LINK( this, SfxHelpIndexWindow_Impl, SelectHdl ) );
    nMinWidth = aActiveLB.GetSizePixel().Width();

    aInitTimer.SetTimeoutHdl( LINK( this, SfxHelpIndexWindow_Impl, InitHdl ) );
    aInitTimer.SetTimeout( 200 );
    aInitTimer.Start();
}

// -----------------------------------------------------------------------

SfxHelpIndexWindow_Impl::~SfxHelpIndexWindow_Impl()
{
    delete pCPage;
    delete pIPage;
    delete pSPage;
    delete pFPage;

    for ( USHORT i = 0; i < aActiveLB.GetEntryCount(); ++i )
        delete (String*)(ULONG)aActiveLB.GetEntryData(i);

    SvtViewOptions aViewOpt( E_TABDIALOG, CONFIGNAME_INDEXWIN );
    aViewOpt.SetPageID( (sal_Int32)aTabCtrl.GetCurPageId() );
}

// -----------------------------------------------------------------------

void SfxHelpIndexWindow_Impl::Initialize()
{
    String aHelpURL = HELP_URL;
    AppendConfigToken_Impl( aHelpURL, sal_True );
    Sequence< ::rtl::OUString > aFactories = SfxContentHelper::GetResultSet( aHelpURL );
    const ::rtl::OUString* pFacs  = aFactories.getConstArray();
    UINT32 i, nCount = aFactories.getLength();
    for ( i = 0; i < nCount; ++i )
    {
        String aRow( pFacs[i] );
        String aTitle, aType, aURL;
        xub_StrLen nIdx = 0;
        aTitle = aRow.GetToken( 0, '\t', nIdx );
        aType = aRow.GetToken( 0, '\t', nIdx );
        aURL = aRow.GetToken( 0, '\t', nIdx );
        String* pFactory = new String( INetURLObject( aURL ).GetHost() );
        USHORT nPos = aActiveLB.InsertEntry( aTitle );
        aActiveLB.SetEntryData( nPos, (void*)(ULONG)pFactory );
    }

    aActiveLB.SetDropDownLineCount( (USHORT)nCount );
    if ( aActiveLB.GetSelectEntryPos() == LISTBOX_ENTRY_NOTFOUND )
        SetActiveFactory();
}

// -----------------------------------------------------------------------

void SfxHelpIndexWindow_Impl::SetActiveFactory()
{
    DBG_ASSERT( pIPage, "index page not initialized" );

    for ( USHORT i = 0; i < aActiveLB.GetEntryCount(); ++i )
    {
        String* pFactory = (String*)(ULONG)aActiveLB.GetEntryData(i);
        pFactory->ToLowerAscii();
        if ( *pFactory == pIPage->GetFactory() )
        {
            aActiveLB.SelectEntryPos(i);
            break;
        }
    }
}

// -----------------------------------------------------------------------

IMPL_LINK( SfxHelpIndexWindow_Impl, ActivatePageHdl, TabControl *, pTabCtrl )
{
    const USHORT nId = pTabCtrl->GetCurPageId();
    TabPage* pPage = NULL;

    switch ( nId )
    {
        case HELP_INDEX_PAGE_CONTENTS:
        {
            if ( !pCPage )
                pCPage = new ContentTabPage_Impl( &aTabCtrl );
            pPage = pCPage;
            break;
        }

        case HELP_INDEX_PAGE_INDEX:
        {
            if ( !pIPage )
                pIPage = new IndexTabPage_Impl( &aTabCtrl );
            pPage = pIPage;
            break;
        }

        case HELP_INDEX_PAGE_SEARCH:
        {
            if ( !pSPage )
                pSPage = new SearchTabPage_Impl( &aTabCtrl );
            pPage = pSPage;
            break;
        }

        case HELP_INDEX_PAGE_BOOKMARKS:
        {
            if ( !pFPage )
                pFPage = new BookmarksTabPage_Impl( &aTabCtrl );
            pPage = pFPage;
            break;
        }
    }

    pTabCtrl->SetTabPage( nId, pPage );

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SfxHelpIndexWindow_Impl, SelectHdl, ListBox *, EMPTYARG )
{
    String* pFactory = (String*)(ULONG)aActiveLB.GetEntryData( aActiveLB.GetSelectEntryPos() );
    if ( pFactory )
    {
        String aFactory( *pFactory );
        aFactory.ToLowerAscii();
        SetFactory( aFactory, sal_False );
    }

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SfxHelpIndexWindow_Impl, InitHdl, Timer *, EMPTYARG )
{
    Initialize();
    return 0;
}

// -----------------------------------------------------------------------

void SfxHelpIndexWindow_Impl::Resize()
{
    Size aSize = GetOutputSizePixel();
    Size aNewSize;
    Point aPnt;

    if ( aSize.Width() < nMinWidth )
        aSize.Width() = nMinWidth;
    else
    {
        aPnt = aActiveLB.GetPosPixel();
        aNewSize = aActiveLB.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aActiveLB.SetSizePixel( aNewSize );

        aPnt = aActiveLine.GetPosPixel();
        aNewSize = aActiveLine.GetSizePixel();
        aNewSize.Width() = aSize.Width() - ( aPnt.X() * 2 );
        aActiveLine.SetSizePixel( aNewSize );
    }

    aPnt = aTabCtrl.GetPosPixel();
    aNewSize = aSize;
    aSize.Height() -= aPnt.Y();
    aTabCtrl.SetSizePixel( aSize );
}

// -----------------------------------------------------------------------

void SfxHelpIndexWindow_Impl::SetDoubleClickHdl( const Link& rLink )
{
    if ( !pIPage )
        pIPage = new IndexTabPage_Impl( &aTabCtrl );
    pIPage->SetDoubleClickHdl( rLink );
    if ( !pSPage )
        pSPage = new SearchTabPage_Impl( &aTabCtrl );
    pSPage->SetDoubleClickHdl( rLink );
    if ( !pFPage )
        pFPage = new BookmarksTabPage_Impl( &aTabCtrl );
    pFPage->SetDoubleClickHdl( rLink );
}

// -----------------------------------------------------------------------

void SfxHelpIndexWindow_Impl::SetFactory( const String& rFactory, sal_Bool bActive )
{
    if ( !pIPage )
        pIPage = new IndexTabPage_Impl( &aTabCtrl );
    pIPage->SetFactory( rFactory );
    if ( !pSPage )
        pSPage = new SearchTabPage_Impl( &aTabCtrl );
    pSPage->SetFactory( rFactory );

    if ( bActive )
        SetActiveFactory();
}

// -----------------------------------------------------------------------

String SfxHelpIndexWindow_Impl::GetSelectEntry() const
{
    String aRet;

    switch ( aTabCtrl.GetCurPageId() )
    {
        case HELP_INDEX_PAGE_CONTENTS:
            break;

        case HELP_INDEX_PAGE_INDEX:
            aRet = pIPage->GetSelectEntry();
            break;

        case HELP_INDEX_PAGE_SEARCH:
            aRet = pSPage->GetSelectEntry();
            break;

        case HELP_INDEX_PAGE_BOOKMARKS:
            aRet = pFPage->GetSelectEntry();
            break;
    }

    return aRet;
}

// -----------------------------------------------------------------------

void SfxHelpIndexWindow_Impl::AddBookmarks( const String& rTitle, const String& rURL )
{
    if ( !pFPage )
        pFPage = new BookmarksTabPage_Impl( &aTabCtrl );
    pFPage->AddBookmarks( rTitle, rURL );
}

// class SfxHelpTextWindow_Impl ------------------------------------------

SfxHelpTextWindow_Impl::SfxHelpTextWindow_Impl( SfxHelpWindow_Impl* pParent ) :

    Window( pParent, WB_CLIPCHILDREN ),

    aToolBox( this, 0 ),
    pHelpWin( pParent ),
    pTextWin( new Window( this, 0 ) ),
    bIsDebug( sal_False )

{
    xFrame = Reference < XFrame > ( ::comphelper::getProcessServiceFactory()->createInstance(
            DEFINE_CONST_UNICODE("com.sun.star.frame.Frame") ), UNO_QUERY );
    xFrame->initialize( VCLUnoHelper::GetInterface ( pTextWin ) );
    xFrame->setName( DEFINE_CONST_UNICODE("OFFICE_HELP") );

    aToolBox.SetHelpId( HID_HELP_TOOLBOX );
    aToolBox.InsertItem( TBI_INDEX, Image( SfxResId( IMG_HELP_TOOLBOX_INDEX ) ) );
    aToolBox.SetQuickHelpText( TBI_INDEX, String( SfxResId( STR_HELP_BUTTON_INDEX ) ) );
    aToolBox.SetHelpId( TBI_INDEX, HID_HELP_TOOLBOXITEM_INDEX );
    aToolBox.InsertItem( TBI_START, Image( SfxResId( IMG_HELP_TOOLBOX_START ) ) );
    aToolBox.SetQuickHelpText( TBI_START, String( SfxResId( STR_HELP_BUTTON_START ) ) );
    aToolBox.SetHelpId( TBI_START, HID_HELP_TOOLBOXITEM_START );
    aToolBox.InsertItem( TBI_BACKWARD, Image( SfxResId( IMG_HELP_TOOLBOX_PREV ) ) );
    aToolBox.SetQuickHelpText( TBI_BACKWARD, String( SfxResId( STR_HELP_BUTTON_PREV ) ) );
    aToolBox.SetHelpId( TBI_BACKWARD, HID_HELP_TOOLBOXITEM_BACKWARD );
    aToolBox.InsertItem( TBI_FORWARD, Image( SfxResId( IMG_HELP_TOOLBOX_NEXT ) ) );
    aToolBox.SetQuickHelpText( TBI_FORWARD, String( SfxResId( STR_HELP_BUTTON_NEXT ) ) );
    aToolBox.SetHelpId( TBI_FORWARD, HID_HELP_TOOLBOXITEM_FORWARD );
    aToolBox.InsertItem( TBI_PRINT, Image( SfxResId( IMG_HELP_TOOLBOX_PRINT ) ) );
    aToolBox.SetQuickHelpText( TBI_PRINT, String( SfxResId( STR_HELP_BUTTON_PRINT ) ) );
    aToolBox.SetHelpId( TBI_PRINT, HID_HELP_TOOLBOXITEM_PRINT );
    aToolBox.InsertItem( TBI_BOOKMARKS, Image( SfxResId( IMG_HELP_TOOLBOX_BOOKMARKS ) ) );
    aToolBox.SetQuickHelpText( TBI_BOOKMARKS, String( SfxResId( STR_HELP_BUTTON_ADDBOOKMARK ) ) );
    aToolBox.SetHelpId( TBI_BOOKMARKS, HID_HELP_TOOLBOXITEM_BOOKMARKS );

    Size aSize = aToolBox.CalcWindowSizePixel();
    aToolBox.SetSizePixel( aSize );
    aToolBox.SetOutStyle( TOOLBOX_STYLE_FLAT );
    aToolBox.Show();

    char* pEnv = getenv( "help_debug" );
    if ( pEnv )
        bIsDebug = sal_True;
}

// -----------------------------------------------------------------------

SfxHelpTextWindow_Impl::~SfxHelpTextWindow_Impl()
{
    xFrame->dispose();
}

// -----------------------------------------------------------------------

void SfxHelpTextWindow_Impl::Resize()
{
    Size aWinSize = GetOutputSizePixel();
    Size aSize = aToolBox.GetSizePixel();
    aSize.Width() = aWinSize.Width();
    aToolBox.SetSizePixel( aSize );

    long nToolBoxHeight = aSize.Height();
    aSize = aWinSize;
    aSize.Height() -= nToolBoxHeight;
    pTextWin->SetPosSizePixel( Point( 0, nToolBoxHeight  ), aSize );
}

// -----------------------------------------------------------------------

long SfxHelpTextWindow_Impl::PreNotify( NotifyEvent& rNEvt )
{
    long nDone = 0;
    if ( rNEvt.GetType() == EVENT_COMMAND && rNEvt.GetCommandEvent() )
    {
        const CommandEvent* pCmdEvt = rNEvt.GetCommandEvent();

        if ( pCmdEvt->IsMouseEvent() && pCmdEvt->GetCommand() == COMMAND_CONTEXTMENU )
        {
            // link at current mouse position ?
            const Point&  rPos = pCmdEvt->GetMousePosPixel();
            PopupMenu aMenu;
            aMenu.InsertItem( TBI_INDEX, String( SfxResId( STR_HELP_BUTTON_INDEX ) ) );
            aMenu.InsertSeparator();
            aMenu.InsertItem( TBI_START, String( SfxResId( STR_HELP_BUTTON_START ) ) );
            aMenu.InsertSeparator();
            aMenu.InsertItem( TBI_BACKWARD, String( SfxResId( STR_HELP_BUTTON_PREV ) ) );
            aMenu.InsertItem( TBI_FORWARD, String( SfxResId( STR_HELP_BUTTON_NEXT ) ) );
            aMenu.InsertSeparator();
            aMenu.InsertItem( TBI_PRINT, String( SfxResId( STR_HELP_BUTTON_PRINT ) ) );
            aMenu.InsertSeparator();
            aMenu.InsertItem( TBI_BOOKMARKS, String( SfxResId( STR_HELP_BUTTON_ADDBOOKMARK ) ) );
            if ( bIsDebug )
            {
                aMenu.InsertSeparator();
                aMenu.InsertItem( TBI_SOURCEVIEW, String( SfxResId( STR_HELP_BUTTON_SOURCEVIEW ) ) );
            }

            USHORT nId = aMenu.Execute( this, rPos );
            pHelpWin->DoAction( nId );
            nDone = 1;
        }
    }

    return nDone ? nDone : Window::PreNotify( rNEvt );
}

// class SfxHelpWindow_Impl ----------------------------------------------

void SfxHelpWindow_Impl::Resize()
{
    SplitWindow::Resize();
    InitSizes();
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::Split()
{
    SplitWindow::Split();

    nIndexSize = GetItemSize( INDEXWIN_ID );
    nTextSize = GetItemSize( TEXTWIN_ID );
    InitSizes();
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::MakeLayout()
{
    if ( nHeight > 0 )
    {
        if ( !bIndex )
        {
            Size aSize = pTextWin->GetOutputSizePixel();
            if ( xWindow.is() )
            {
                ::com::sun::star::awt::Rectangle aRect = xWindow->getPosSize();
                xWindow->setPosSize( aRect.X, aRect.Y, nCollapseWidth, nHeight,
                                     ::com::sun::star::awt::PosSize::SIZE );
            }
        }
        else if ( xWindow.is() )
        {
            ::com::sun::star::awt::Rectangle aRect = xWindow->getPosSize();
            xWindow->setPosSize( aRect.X, aRect.Y, nExpandWidth, nHeight,
                                 ::com::sun::star::awt::PosSize::SIZE );
        }
    }

    Clear();

    if ( bIndex )
    {
        pIndexWin->Show();
        InsertItem( COLSET_ID, 100, SPLITWINDOW_APPEND, SPLITSET_ID, SWIB_PERCENTSIZE | SWIB_COLSET );
        InsertItem( INDEXWIN_ID, pIndexWin, nIndexSize, SPLITWINDOW_APPEND, COLSET_ID, SWIB_PERCENTSIZE );
        InsertItem( TEXTWIN_ID, pTextWin, nTextSize, SPLITWINDOW_APPEND, COLSET_ID, SWIB_PERCENTSIZE );
    }
    else
    {
        pIndexWin->Hide();
        InsertItem( COLSET_ID, 100, SPLITWINDOW_APPEND, SPLITSET_ID, SWIB_PERCENTSIZE | SWIB_COLSET );
        InsertItem( TEXTWIN_ID, pTextWin, 100, SPLITWINDOW_APPEND, 1, SWIB_PERCENTSIZE );
    }
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::InitSizes()
{
    if ( xWindow.is() )
    {
        ::com::sun::star::awt::Rectangle aRect = xWindow->getPosSize();
        nHeight = aRect.Height;

        if ( bIndex )
        {
            nExpandWidth = aRect.Width;
            nCollapseWidth = nExpandWidth * nTextSize / 100;
        }
        else
        {
            nCollapseWidth = aRect.Width;
            nExpandWidth = nCollapseWidth * 100 / nTextSize;
        }
    }
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::LoadConfig()
{
    sal_Int32 nWidth;
     SvtViewOptions aViewOpt( E_WINDOW, CONFIGNAME_HELPWIN );
    if ( aViewOpt.Exists() )
    {
        aViewOpt.GetSize( nWidth, nHeight );
        bIndex = aViewOpt.IsVisible();
        String aUserData = aViewOpt.GetUserData();
        DBG_ASSERT( aUserData.GetTokenCount() == 2, "invalid user data" );
        nIndexSize = aUserData.GetToken(0).ToInt32();
        nTextSize = aUserData.GetToken(1).ToInt32();

        if ( bIndex )
        {
            nExpandWidth = nWidth;
            nCollapseWidth = nExpandWidth * nTextSize / 100;
        }
        else
        {
            nCollapseWidth = nWidth;
            nExpandWidth = nCollapseWidth * 100 / nTextSize;
        }
    }
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::SaveConfig()
{
    SvtViewOptions aViewOpt( E_WINDOW, CONFIGNAME_HELPWIN );

    if ( xWindow.is() )
    {
        ::com::sun::star::awt::Rectangle aRect = xWindow->getPosSize();
        aViewOpt.SetSize( aRect.Width, aRect.Height );
    }

    aViewOpt.SetVisible( bIndex );
    String aUserData = String::CreateFromInt32( nIndexSize );
    aUserData += ';';
    aUserData += String::CreateFromInt32( nTextSize );
    aViewOpt.SetUserData( aUserData );
}

// -----------------------------------------------------------------------

IMPL_LINK( SfxHelpWindow_Impl, SelectHdl, ToolBox* , pToolBox )
{
    if ( pToolBox )
        DoAction( pToolBox->GetCurItemId() );

    return 1;
}

//-------------------------------------------------------------------------

IMPL_LINK( SfxHelpWindow_Impl, OpenHdl, SfxHelpIndexWindow_Impl* , EMPTYARG )
{
    String aEntry = pIndexWin->GetSelectEntry();
    if ( aEntry.Len() > 0 )
    {
        INetURLObject aObj( aEntry );
        if ( aObj.GetProtocol() != INET_PROT_VND_SUN_STAR_HELP )
        {
            String aTemp = aEntry;
            aEntry = HELP_URL;
            aEntry += pIndexWin->GetFactory();
            aEntry += '/';
            aEntry += aTemp;
            AppendConfigToken_Impl( aEntry, sal_True );
        }
        URL aURL;
        aURL.Complete = aEntry;
        PARSE_URL( aURL );
        Reference < XDispatch > xDisp = pHelpInterceptor->queryDispatch( aURL, String(), 0 );
        if ( xDisp.is() )
        {
            if ( !IsWait() )
                EnterWait();
            ( (OpenStatusListener_Impl*)xOpenListener.get() )->AddListener( xDisp, aURL );
            xDisp->dispatch( aURL, Sequence < PropertyValue >() );
        }
    }

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SfxHelpWindow_Impl, ChangeHdl, HelpListener_Impl*, pListener )
{
    SetFactory( pListener->GetFactory(), sal_False );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SfxHelpWindow_Impl, OpenDoneHdl, OpenStatusListener_Impl*, pListener )
{
    String aModuleName = pListener->GetURL().GetToken( 2, '/' );
    SetFactory( aModuleName, sal_False );
    if ( IsWait() )
        LeaveWait();
    return 0;
}

// -----------------------------------------------------------------------

SfxHelpWindow_Impl::SfxHelpWindow_Impl(
    const ::com::sun::star::uno::Reference < ::com::sun::star::frame::XFrame >& rFrame,
    Window* pParent, WinBits nBits ) :

    SplitWindow( pParent, nBits | WB_3DLOOK ),

    pIndexWin       ( NULL ),
    pTextWin        ( NULL ),
    pHelpInterceptor( new HelpInterceptor_Impl() ),
    pHelpListener   ( new HelpListener_Impl( pHelpInterceptor ) ),
    nExpandWidth    ( 0 ),
    nCollapseWidth  ( 0 ),
    nHeight         ( 0 ),
    nIndexSize      ( 40 ),
    nTextSize       ( 60 ),
    bIndex          ( sal_True )

{
    SetHelpId( HID_HELP_WINDOW );

    OpenStatusListener_Impl* pOpenListener = new OpenStatusListener_Impl;
    xOpenListener = Reference< XStatusListener >( static_cast< ::cppu::OWeakObject* >(pOpenListener), UNO_QUERY );

    pHelpInterceptor->InitWaiter( (OpenStatusListener_Impl*)xOpenListener.get(), this );
    pIndexWin = new SfxHelpIndexWindow_Impl( this );
    pIndexWin->SetDoubleClickHdl( LINK( this, SfxHelpWindow_Impl, OpenHdl ) );
    pIndexWin->Show();
    pTextWin = new SfxHelpTextWindow_Impl( this );
    Reference < XFramesSupplier > xSup( rFrame, UNO_QUERY );
    Reference < XFrames > xFrames = xSup->getFrames();
    xFrames->append( pTextWin->getFrame() );
    pTextWin->SetSelectHdl( LINK( this, SfxHelpWindow_Impl, SelectHdl ) );
    pTextWin->Show();
    pHelpInterceptor->setInterception( pTextWin->getFrame() );
    pHelpListener->SetChangeHdl( LINK( this, SfxHelpWindow_Impl, ChangeHdl ) );
    ( (OpenStatusListener_Impl*)xOpenListener.get() )->SetOpenHdl( LINK( this, SfxHelpWindow_Impl, OpenDoneHdl ) );
    LoadConfig();
}

// -----------------------------------------------------------------------

SfxHelpWindow_Impl::~SfxHelpWindow_Impl()
{
    SaveConfig();
    delete pIndexWin;
    delete pTextWin;
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::setContainerWindow( Reference < ::com::sun::star::awt::XWindow > xWin )
{
    xWindow = xWin;
    MakeLayout();
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::SetFactory( const String& rFactory, sal_Bool bStart )
{
    pIndexWin->SetFactory( rFactory, sal_True );
    if ( bStart )
        pHelpInterceptor->SetFactory( rFactory );
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::SetHelpURL( const String& rURL )
{
    INetURLObject aObj( rURL );
    SetFactory( aObj.GetHost(), sal_True );
}

// -----------------------------------------------------------------------

void SfxHelpWindow_Impl::DoAction( USHORT nActionId )
{
    switch ( nActionId )
    {
        case TBI_INDEX :
        {
            bIndex = !bIndex;
            MakeLayout();
            break;
        }

        case TBI_START :
        {
            String aStartURL;
            aStartURL = HELP_URL;
            aStartURL += pIndexWin->GetFactory();
            aStartURL += DEFINE_CONST_UNICODE("/start");
            AppendConfigToken_Impl( aStartURL, sal_True );

            URL aURL;
            aURL.Complete = aStartURL;
            PARSE_URL( aURL );

            String aTarget( DEFINE_CONST_UNICODE("_self") );
            Reference < XDispatchProvider > xProv( pTextWin->getFrame(), UNO_QUERY );
            Reference < XDispatch > xDisp = xProv.is() ?
                xProv->queryDispatch( aURL, aTarget, 0 ) : Reference < XDispatch >();

            if ( xDisp.is() )
            {
                Sequence < PropertyValue > aArgs( 1 );
                aArgs[0].Name = String( DEFINE_CONST_UNICODE("ReadOnly") );
                BOOL bReadOnly = TRUE;
                aArgs[0].Value <<= bReadOnly;
                if ( !IsWait() )
                    EnterWait();
                ( (OpenStatusListener_Impl*)xOpenListener.get() )->AddListener( xDisp, aURL );
                xDisp->dispatch( aURL, aArgs );
            }
            break;
        }

        case TBI_BACKWARD :
        case TBI_FORWARD :
        {
            URL aURL;
            aURL.Complete = DEFINE_CONST_UNICODE(".uno:Backward");
            if ( TBI_FORWARD == nActionId )
                aURL.Complete = DEFINE_CONST_UNICODE(".uno:Forward");
            PARSE_URL( aURL );
            pHelpInterceptor->dispatch( aURL, Sequence < PropertyValue >() );
            break;
        }

        case TBI_CONTEXT :
            break;

        case TBI_PRINT :
        case TBI_SOURCEVIEW :
        {
            Reference < XDispatchProvider > xProv( pTextWin->getFrame(), UNO_QUERY );
            if ( xProv.is() )
            {
                URL aURL;
                if ( TBI_PRINT == nActionId )
                    aURL.Complete = DEFINE_CONST_UNICODE(".uno:Print");
                else
                    aURL.Complete = DEFINE_CONST_UNICODE(".uno:SourceView");
                PARSE_URL( aURL );
                Reference < XDispatch > xDisp = xProv->queryDispatch( aURL, String(), 0 );
                if ( xDisp.is() )
                    xDisp->dispatch( aURL, Sequence < PropertyValue >() );
            }
            break;
        }

        case TBI_BOOKMARKS :
        {
            String aURL = pHelpInterceptor->GetCurrentURL();
            Content aCnt( aURL, Reference< ::com::sun::star::ucb::XCommandEnvironment > () );
            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > xInfo = aCnt.getProperties();
            if ( xInfo->hasPropertyByName( PROPERTY_TITLE ) )
            {
                ::com::sun::star::uno::Any aAny = aCnt.getPropertyValue( PROPERTY_TITLE );
                rtl::OUString aValue;
                if ( aAny >>= aValue )
                {
                    String aTitle( aValue );
                    SfxAddHelpBookmarkDialog_Impl aDlg( this, sal_False );
                    aDlg.SetTitle( aTitle );
                    if ( aDlg.Execute() == RET_OK )
                    {
                        aTitle = aDlg.GetTitle();
                        pIndexWin->AddBookmarks( aTitle, aURL );
                    }
                }
            }
            break;
        }
    }
}

// class SfxAddHelpBookmarkDialog_Impl -----------------------------------

SfxAddHelpBookmarkDialog_Impl::SfxAddHelpBookmarkDialog_Impl( Window* pParent, sal_Bool bRename ) :

    ModalDialog( pParent, SfxResId( DLG_HELP_ADDBOOKMARK ) ),

    aTitleFT    ( this, ResId( FT_BOOKMARK_TITLE ) ),
    aTitleED    ( this, ResId( ED_BOOKMARK_TITLE ) ),
    aOKBtn      ( this, ResId( PB_BOOKMARK_OK ) ),
    aEscBtn     ( this, ResId( PB_BOOKMARK_CANCEL ) ),
    aHelpBtn    ( this, ResId( PB_BOOKMARK_HELP ) )

{
    if ( bRename )
        SetText( String( ResId( STR_BOOKMARK_RENAME ) ) );

    FreeResource();
}

// -----------------------------------------------------------------------

SfxAddHelpBookmarkDialog_Impl::~SfxAddHelpBookmarkDialog_Impl()
{
}

// -----------------------------------------------------------------------

void SfxAddHelpBookmarkDialog_Impl::SetTitle( const String& rTitle )
{
    aTitleED.SetText( rTitle );
    aTitleED.SetSelection( Selection( 0, rTitle.Len() ) );
}

