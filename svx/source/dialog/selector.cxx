/*************************************************************************
 *
 *  $RCSfile: selector.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 14:43:46 $
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

#ifndef _SV_HELP_HXX //autogen
#include <vcl/help.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_METRIC_HXX //autogen
#include <vcl/metric.hxx>
#endif
#ifndef _SBXCLASS_HXX //autogen
#include <svtools/sbx.hxx>
#endif
#ifndef _SB_SBSTAR_HXX //autogen
#include <basic/sbstar.hxx>
#endif
#ifndef __SBX_SBXMETHOD_HXX //autogen
#include <svtools/sbxmeth.hxx>
#endif
#ifndef _SB_SBMOD_HXX //autogen
#include <basic/sbmod.hxx>
#endif
#ifndef _BASMGR_HXX //autogen
#include <basic/basmgr.hxx>
#endif

#pragma hdrstop

#include "selector.hxx"
#include "dialmgr.hxx"

#include "selector.hrc"
#include "fmresids.hrc"
#include "dialogs.hrc"

#include <sfx2/app.hxx>
#include <sfx2/msg.hxx>
#include <sfx2/msgpool.hxx>
#include <sfx2/macrconf.hxx>
#include <sfx2/minfitem.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/dispatch.hxx>

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif

#ifndef _UNOTOOLS_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif

#ifndef  _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef  _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif

#ifndef  _COM_SUN_STAR_SCRIPT_PROVIDER_XSCRIPTPROVIDERSUPPLIER_HPP_
#include <com/sun/star/script/provider/XScriptProviderSupplier.hpp>
#endif

#ifndef  _COM_SUN_STAR_SCRIPT_PROVIDER_XSCRIPTPROVIDER_HPP_
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#endif

#ifndef  _COM_SUN_STAR_SCRIPT_BROWSE_XBROWSENODE_HPP_
#include <com/sun/star/script/browse/XBrowseNode.hpp>
#endif

#ifndef  _COM_SUN_STAR_SCRIPT_BROWSE_BROWSENODETYPES_HPP_
#include <com/sun/star/script/browse/BrowseNodeTypes.hpp>
#endif

#ifndef  _COM_SUN_STAR_SCRIPT_BROWSE_BROWSENODEFACTORY_HPP_
#include <com/sun/star/script/browse/XBrowseNodeFactory.hpp>
#endif
#ifndef  _COM_SUN_STAR_SCRIPT_BROWSE_BROWSENODEFACTORYVIEWTYPE_HPP_
#include <com/sun/star/script/browse/BrowseNodeFactoryViewTypes.hpp>
#endif
#include <drafts/com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHINFORMATIONPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchInformationProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_DISPATCHINFORMATION_HPP_
#include <com/sun/star/frame/DispatchInformation.hpp>
#endif

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::script;

#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#include <svtools/imagemgr.hxx>
#include <tools/urlobj.hxx>

#define PRTSTR(x) rtl::OUStringToOString(x, RTL_TEXTENCODING_ASCII_US).pData->buffer

SV_IMPL_PTRARR(SvxGroupInfoArr_Impl, SvxGroupInfoPtr);

class TestOUString : public OUString
{
public:

    TestOUString() : OUString()
    { OSL_TRACE("Creating empty TestOUString"); }

    TestOUString( OUString value ) : OUString( value )
    { OSL_TRACE("Creating nontempty OUString: %s", PRTSTR(value)); }

    ~TestOUString()
    { OSL_TRACE("Destroying TestOUString: %s", PRTSTR(*this)); }
};

class ScriptInfo
{
private:

    OUString m_sURI;
    OUString m_sDescription;

public:

    ScriptInfo( const OUString& uri )
        : m_sURI( uri ) {}
    ~ScriptInfo() {}

    void SetHelpText( const OUString& description )
    {
        if ( description.getLength() != 0 )
        {
            m_sDescription = description.copy(0);
        }
    };
    const OUString& GetHelpText() const { return m_sDescription; }
    const OUString& GetURI() const { return m_sURI; }
};

/*
 * The implementations of SvxConfigFunctionListBox_Impl and
 * SvxConfigGroupListBox_Impl are copied from sfx2/source/dialog/cfg.cxx
 */
SvxConfigFunctionListBox_Impl::SvxConfigFunctionListBox_Impl( Window* pParent, const ResId& rResId)
    : SvTreeListBox( pParent, rResId )
    , pCurEntry( 0 )
    , m_pDraggingEntry( 0 )
{
    SetWindowBits( GetStyle() | WB_CLIPCHILDREN | WB_HSCROLL | WB_SORT );
    GetModel()->SetSortMode( SortAscending );

    // Timer f"ur die BallonHelp
    aTimer.SetTimeout( 200 );
    aTimer.SetTimeoutHdl(
        LINK( this, SvxConfigFunctionListBox_Impl, TimerHdl ) );
}

SvxConfigFunctionListBox_Impl::~SvxConfigFunctionListBox_Impl()
{
    ClearAll();
}

SvLBoxEntry* SvxConfigFunctionListBox_Impl::GetLastSelectedEntry()
{
    if ( m_pDraggingEntry != NULL )
    {
        return m_pDraggingEntry;
    }
    else
    {
        return FirstSelected();
    }
}

void SvxConfigFunctionListBox_Impl::MouseMove( const MouseEvent& rMEvt )
{
    Point aMousePos = rMEvt.GetPosPixel();
    pCurEntry = GetCurEntry();

    if ( pCurEntry && GetEntry( aMousePos ) == pCurEntry )
        aTimer.Start();
    else
    {
        Help::ShowBalloon( this, aMousePos, String() );
        aTimer.Stop();
    }
}


IMPL_LINK( SvxConfigFunctionListBox_Impl, TimerHdl, Timer*, pTimer)
{
    aTimer.Stop();
    Point aMousePos = GetPointerPosPixel();
    SvLBoxEntry *pEntry = GetCurEntry();
    if ( pEntry && GetEntry( aMousePos ) == pEntry && pCurEntry == pEntry )
        Help::ShowBalloon( this, OutputToScreenPixel( aMousePos ), GetHelpText( pEntry ) );
    return 0L;
}

void SvxConfigFunctionListBox_Impl::ClearAll()
{
    USHORT nCount = aArr.Count();
    for ( USHORT i=0; i<nCount; i++ )
    {
        SvxGroupInfo_Impl *pData = aArr[i];

        if ( pData->nKind == SFX_CFGFUNCTION_SLOT && pData->pObject != NULL )
        {
            delete (OUString*)pData->pObject;
        }

        if ( pData->nKind == SFX_CFGFUNCTION_MACRO )
        {
            SfxMacroInfo *pInfo = (SfxMacroInfo*) pData->pObject;
            SFX_APP()->GetMacroConfig()->ReleaseSlotId( pInfo->GetSlotId() );
            delete pInfo;
        }

        if ( pData->nKind == SFX_CFGFUNCTION_SCRIPT )
        {
            ScriptInfo *pInfo = (ScriptInfo*) pData->pObject;
            if ( pInfo != NULL )
            {
                delete pInfo;
            }
        }

        if ( pData->nKind == SFX_CFGGROUP_SCRIPTCONTAINER )
        {
            XInterface* xi = static_cast<XInterface *>(pData->pObject);
            if (xi != NULL)
            {
                xi->release();
            }
        }

        delete pData;
    }

    aArr.Remove( 0, nCount );
    Clear();
}

SvLBoxEntry* SvxConfigFunctionListBox_Impl::GetEntry_Impl( const String& rName )
{
    SvLBoxEntry *pEntry = First();
    while ( pEntry )
    {
        if ( GetEntryText( pEntry ) == rName )
            return pEntry;
        pEntry = Next( pEntry );
    }

    return NULL;
}

SvLBoxEntry* SvxConfigFunctionListBox_Impl::GetEntry_Impl( USHORT nId )
{
    SvLBoxEntry *pEntry = First();
    while ( pEntry )
    {
        SvxGroupInfo_Impl *pData = (SvxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pData && pData->nOrd == nId )
            return pEntry;
        pEntry = Next( pEntry );
    }

    return NULL;
}

SfxMacroInfo* SvxConfigFunctionListBox_Impl::GetMacroInfo()
{
    SvLBoxEntry *pEntry = GetLastSelectedEntry();
    if ( pEntry )
    {
        SvxGroupInfo_Impl *pData = (SvxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pData && ( pData->nKind == SFX_CFGFUNCTION_MACRO ||
                                        pData->nKind == SFX_CFGFUNCTION_SCRIPT ) )
            return (SfxMacroInfo*) pData->pObject;
    }

    return 0;
}

USHORT SvxConfigFunctionListBox_Impl::GetId( SvLBoxEntry *pEntry )
{
    SvxGroupInfo_Impl *pData = pEntry ?
        (SvxGroupInfo_Impl*) pEntry->GetUserData() : 0;
    if ( pData )
        return pData->nOrd;
    return 0;
}

String SvxConfigFunctionListBox_Impl::GetHelpText( SvLBoxEntry *pEntry )
{
    // Information zum selektierten Entry aus den Userdaten holen
    SvxGroupInfo_Impl *pInfo =
        pEntry ? (SvxGroupInfo_Impl*) pEntry->GetUserData(): 0;

    if ( pInfo )
    {
        if ( pInfo->nKind == SFX_CFGFUNCTION_SLOT )
        {
            OUString* aCmdURL = (OUString*) pInfo->pObject;

            OUString aHelpText =
                Application::GetHelp()->GetHelpText( *aCmdURL, this );

            return aHelpText;
        }
        else if ( pInfo->nKind == SFX_CFGFUNCTION_SCRIPT )
        {
            ScriptInfo* pScriptInfo = (ScriptInfo*) pInfo->pObject;
            return pScriptInfo->GetHelpText();
        }
        else if ( pInfo->nKind == SFX_CFGFUNCTION_MACRO )
        {
            // Eintrag ist ein Macro, Hilfe aus der MacroInfo
            SfxMacroInfo *pMacInfo = (SfxMacroInfo*) pInfo->pObject;
            return pMacInfo->GetHelpText();
        }
    }

    return String();
}

void SvxConfigFunctionListBox_Impl::FunctionSelected()
{
    Help::ShowBalloon( this, Point(), String() );
}

// drag and drop support
DragDropMode SvxConfigFunctionListBox_Impl::NotifyStartDrag(
    TransferDataContainer& aTransferDataContainer, SvLBoxEntry* pEntry )
{
    m_pDraggingEntry = pEntry;
    return GetDragDropMode();
}

void SvxConfigFunctionListBox_Impl::DragFinished( sal_Int8 nDropAction )
{
    m_pDraggingEntry = NULL;
}

sal_Int8
SvxConfigFunctionListBox_Impl::AcceptDrop( const AcceptDropEvent& rEvt )
{
    return DND_ACTION_NONE;
}

SvxConfigGroupListBox_Impl::SvxConfigGroupListBox_Impl(
    Window* pParent, const ResId& rResId,
    ULONG nConfigMode, const Reference< frame::XFrame >& xFrame )
        : SvTreeListBox( pParent, rResId )
        , nMode( nConfigMode ), bShowSF( FALSE ), bShowBasic( TRUE ),
    m_sMyMacros(ResId(STR_MYMACROS)),
    m_sProdMacros(ResId(STR_PRODMACROS)),
    m_hdImage(ResId(IMG_HARDDISK)),
    m_hdImage_hc(ResId(IMG_HARDDISK_HC)),
    m_libImage(ResId(IMG_LIB)),
    m_libImage_hc(ResId(IMG_LIB_HC)),
    m_macImage(ResId(IMG_MACRO)),
    m_macImage_hc(ResId(IMG_MACRO_HC)),
    m_docImage(ResId(IMG_DOC)),
    m_docImage_hc(ResId(IMG_DOC_HC))
{
    FreeResource();

    if ( xFrame != NULL )
    {
        m_xFrame.set( xFrame );
    }

    SetWindowBits( GetStyle() | WB_CLIPCHILDREN | WB_HSCROLL | WB_HASBUTTONS | WB_HASLINES | WB_HASLINESATROOT | WB_HASBUTTONSATROOT );

    ImageList aNavigatorImages( SVX_RES( RID_SVXIMGLIST_FMEXPL ) );

    SetNodeBitmaps(
        aNavigatorImages.GetImage( RID_SVXIMG_COLLAPSEDNODE ),
        aNavigatorImages.GetImage( RID_SVXIMG_EXPANDEDNODE ),
        BMP_COLOR_NORMAL );

    SetNodeBitmaps(
        aNavigatorImages.GetImage( RID_SVXIMG_COLLAPSEDNODE ),
        aNavigatorImages.GetImage( RID_SVXIMG_EXPANDEDNODE ),
        BMP_COLOR_HIGHCONTRAST );

    // Check configuration to see whether only Basic macros,
    // only Scripting Framework scripts, or both should be listed
    Any value;
    sal_Bool tmp;

    value = ::utl::ConfigManager::GetConfigManager()->GetLocalProperty(
        OUString::createFromAscii(
            "Office.Scripting/ScriptDisplaySettings/ShowBasic" ) );

    value >>= tmp;

    if (tmp == sal_True) {
        bShowBasic = TRUE;
    }
    else {
        bShowBasic = FALSE;
    }

    value = ::utl::ConfigManager::GetConfigManager()->GetLocalProperty(
        OUString::createFromAscii(
            "Office.Scripting/ScriptDisplaySettings/ShowSF" ) );

    value >>= tmp;

    if (tmp == sal_True) {
        bShowSF = TRUE;
    }
    else {
        bShowSF = FALSE;
    }
}


SvxConfigGroupListBox_Impl::~SvxConfigGroupListBox_Impl()
{
    ClearAll();
}

void SvxConfigGroupListBox_Impl::ClearAll()
{
    USHORT nCount = aArr.Count();
    for ( USHORT i=0; i<nCount; i++ )
    {
        SvxGroupInfo_Impl *pData = aArr[i];
        if ( pData->nKind == SFX_CFGGROUP_SCRIPTCONTAINER )
        {
            XInterface* xi = static_cast<XInterface *>(pData->pObject);
            if (xi != NULL)
            {
                xi->release();
            }
        }
        delete pData;
    }

    aArr.Remove( 0, nCount );
    Clear();
}

String SvxConfigGroupListBox_Impl::GetGroup()
{
    SvLBoxEntry *pEntry = FirstSelected();
    while ( pEntry )
    {
        SvxGroupInfo_Impl *pInfo = (SvxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pInfo->nKind == SFX_CFGGROUP_FUNCTION )
        {
            return GetEntryText( pEntry );
            break;
        }

        if ( pInfo->nKind == SFX_CFGGROUP_BASICMGR )
        {
            BasicManager *pMgr = (BasicManager*) pInfo->pObject;
            return pMgr->GetName();
            break;
        }

        if ( pInfo->nKind == SFX_CFGGROUP_DOCBASICMGR )
        {
            SfxObjectShell *pDoc = (SfxObjectShell*) pInfo->pObject;
            return pDoc->GetTitle();
            break;
        }

        pEntry = GetParent( pEntry );
    }

    return String();
}

void SvxConfigGroupListBox_Impl::Init( SvStringsDtor *pArr )
{
    SetUpdateMode(FALSE);
    ClearAll();

    Reference< XComponentContext > xContext;
    Reference < beans::XPropertySet > xProps(
        ::comphelper::getProcessServiceFactory(), UNO_QUERY_THROW );

    xContext.set( xProps->getPropertyValue(
        rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))),
        UNO_QUERY );

    // are we showing builtin commands?
    if ( nMode && xContext.is() && m_xFrame.is() )
    {
        Reference< lang::XMultiComponentFactory > xMCF =
            xContext->getServiceManager();

        Reference< frame::XDispatchInformationProvider > xDIP(
            m_xFrame, UNO_QUERY );

        Reference< drafts::com::sun::star::frame::XModuleManager >
            xModuleManager( xMCF->createInstanceWithContext(
                OUString::createFromAscii(
                    "drafts.com.sun.star.frame.ModuleManager" ),
                xContext ),
            UNO_QUERY );

        OUString aModuleId = xModuleManager->identify( m_xFrame );

        Reference< container::XNameAccess > xNameAccess(
            xMCF->createInstanceWithContext(
                OUString::createFromAscii(
                    "drafts.com.sun.star.frame.UICommandDescription" ),
                xContext ),
            UNO_QUERY );

        if ( xNameAccess.is() )
        {
            xNameAccess->getByName( aModuleId ) >>= m_xModuleCommands;
        }

        Reference< container::XNameAccess > xAllCategories(
            xMCF->createInstanceWithContext(
                OUString::createFromAscii(
                    "com.sun.star.ui.UICategoryDescription" ),
                xContext ),
            UNO_QUERY );

        Reference< container::XNameAccess > xModuleCategories;
        if ( xAllCategories.is() )
        {
            if ( aModuleId.getLength() != 0 )
            {
                try
                {
                    xModuleCategories = Reference< container::XNameAccess >(
                           xAllCategories->getByName( aModuleId ), UNO_QUERY );
                }
                catch ( container::NoSuchElementException& )
                {
                }
            }

            if ( !xModuleCategories.is() )
            {
                xModuleCategories = xAllCategories;
            }
        }

        if ( xModuleCategories.is() )
        {
            Sequence< sal_Int16 > gids =
                xDIP->getSupportedCommandGroups();

            for ( sal_Int32 i = 0; i < gids.getLength(); i++ )
            {
                Sequence< frame::DispatchInformation > commands;
                try
                {
                    commands =
                        xDIP->getConfigurableDispatchInformation( gids[i] );
                }
                catch ( container::NoSuchElementException& )
                {
                    continue;
                }

                if ( commands.getLength() == 0 )
                {
                    continue;
                }

                sal_Int32 gid = gids[i];
                OUString idx = OUString::valueOf( gid );
                OUString group = idx;
                try
                {
                    xModuleCategories->getByName( idx ) >>= group;
                }
                catch ( container::NoSuchElementException& )
                {
                }

                SvLBoxEntry *pEntry = InsertEntry( group, NULL );

                SvxGroupInfo_Impl *pInfo =
                    new SvxGroupInfo_Impl( SFX_CFGGROUP_FUNCTION, gids[i] );
                aArr.Insert( pInfo, aArr.Count() );

                pEntry->SetUserData( pInfo );
            }
        }
    }

    if ( bShowBasic )
    {
        // Basics einsammeln
        SfxApplication* pSfxApp = SFX_APP();

        pSfxApp->EnterBasicCall();
        String aMacroName(' ');
        aMacroName += String( ResId( STR_BASICMACROS, DIALOG_MGR() ) );


        // Zuerst AppBasic
        BasicManager *pAppBasicMgr = pSfxApp->GetBasicManager();
        BOOL bInsert = TRUE;
        if ( pArr )
        {
            bInsert = FALSE;
            for ( USHORT n=0; n<pArr->Count(); n++ )
            {
                if ( *(*pArr)[n] == pSfxApp->GetName() )
                {
                    bInsert = TRUE;
                    break;
                }
            }
        }

        if ( bInsert )
        {
            pAppBasicMgr->SetName( pSfxApp->GetName() );
            if ( pAppBasicMgr->GetLibCount() )
            {
                // Nur einf"ugen, wenn Bibliotheken vorhanden
                OUString aAppBasTitle;
                utl::ConfigManager::GetDirectConfigProperty(
                    utl::ConfigManager::PRODUCTNAME) >>= aAppBasTitle;
                aAppBasTitle += aMacroName;

                SvLBoxEntry *pEntry = InsertEntry( aAppBasTitle, 0 );
                SvxGroupInfo_Impl *pInfo = new SvxGroupInfo_Impl( SFX_CFGGROUP_BASICMGR, 0, pAppBasicMgr );
                aArr.Insert( pInfo, aArr.Count() );
                pEntry->SetUserData( pInfo );
                pEntry->EnableChildsOnDemand( TRUE );
    //          Expand( pEntry );
            }
        }

        //SfxObjectShell *pDoc = SfxObjectShell::GetFirst();
        SfxObjectShell *pDoc = SfxObjectShell::GetWorkingDocument();
        //while ( pDoc )
        if ( pDoc )
        {
            BOOL bInsert = TRUE;
            if ( pArr )
            {
                bInsert = FALSE;
                for ( USHORT n=0; n<pArr->Count(); n++ )
                {
                    if ( *(*pArr)[n] == pDoc->GetTitle() )
                    {
                        bInsert = TRUE;
                        break;
                    }
                }
            }

            if ( bInsert )
            {
                BasicManager *pBasicMgr = pDoc->GetBasicManager();
                if ( pBasicMgr != pAppBasicMgr && pBasicMgr->GetLibCount() )
                {
                    pBasicMgr->SetName( pDoc->GetTitle() );

                    // Nur einf"ugen, wenn eigenes Basic mit Bibliotheken
                    SvLBoxEntry *pEntry = InsertEntry( pDoc->GetTitle().Append(aMacroName), NULL );
                    SvxGroupInfo_Impl *pInfo =
                        new SvxGroupInfo_Impl( SFX_CFGGROUP_DOCBASICMGR, 0, pDoc );
                    aArr.Insert( pInfo, aArr.Count() );
                    pEntry->SetUserData( pInfo );
                    pEntry->EnableChildsOnDemand( TRUE );
    //              Expand( pEntry );
                }
            }

            //pDoc = SfxObjectShell::GetNext(*pDoc);
        }

        pSfxApp->LeaveBasicCall();
    }

    if ( bShowSF && xContext.is() )
    {
        // Add Scripting Framework entries
        Reference< browse::XBrowseNode > rootNode;
        Reference< XComponentContext> xCtx;

        try
        {
            Reference < beans::XPropertySet > xProps(
                ::comphelper::getProcessServiceFactory(), UNO_QUERY_THROW );
            xCtx.set( xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))), UNO_QUERY_THROW );
            Reference< browse::XBrowseNodeFactory > xFac( xCtx->getValueByName(
                OUString::createFromAscii( "/singletons/com.sun.star.script.browse.theBrowseNodeFactory") ), UNO_QUERY_THROW );
            rootNode.set( xFac->createView( browse::BrowseNodeFactoryViewTypes::MACROSELECTOR ) );
        }
        catch( Exception& e )
        {
            OSL_TRACE(" Caught some exception whilst retrieving browse nodes from factory... Exception: %s",
                ::rtl::OUStringToOString( e.Message , RTL_TEXTENCODING_ASCII_US ).pData->buffer );
            // TODO exception handling
        }

        if ( rootNode.is() )
        {
            if ( nMode )
            {
            /*
                We call acquire on the XBrowseNode so that it does not
                get autodestructed and become invalid when accessed later.
            */
            rootNode->acquire();

            SvxGroupInfo_Impl *pInfo =
                new SvxGroupInfo_Impl( SFX_CFGGROUP_SCRIPTCONTAINER, 0,
                    static_cast<void *>(rootNode.get()));

            String aTitle =
                String( ResId( STR_SELECTOR_MACROS, DIALOG_MGR() ) );

            SvLBoxEntry *pNewEntry = InsertEntry( aTitle, NULL );
            pNewEntry->SetUserData( pInfo );
            pNewEntry->EnableChildsOnDemand( TRUE );
            aArr.Insert( pInfo, aArr.Count() );
            }
            else
            {
                try {
                    if ( rootNode->hasChildNodes() )
                    {
                        Sequence< Reference< browse::XBrowseNode > > children =
                            rootNode->getChildNodes();
                        BOOL bIsRootNode = FALSE;

                        OUString user = OUString::createFromAscii("user");
                        OUString share = OUString::createFromAscii("share");
                        if ( rootNode->getName().equals(OUString::createFromAscii("Root") ))
                        {
                            bIsRootNode = TRUE;
                        }

                        OUString sDisplayTitle;
                        OUString sModelTitle;
                        SfxObjectShell* pCurrentDoc = SfxObjectShell::GetWorkingDocument();
                        if ( pCurrentDoc )
                        {
                            sDisplayTitle = pCurrentDoc->GetTitle();
                            sModelTitle = xModelToDocTitle( pCurrentDoc->GetModel() );
                        }

                        if ( sDisplayTitle.getLength() == 0 && sModelTitle.getLength() != 0 )
                        {
                            sDisplayTitle = sModelTitle;
                        }

                        for ( ULONG n = 0; n < children.getLength(); n++ )
                        {
                            /* To mimic current starbasic behaviour we
                            need to make sure that only the current document
                            is displayed in the config tree. Tests below
                            set the bDisplay flag to FALSE if the current
                            node is a first level child of the Root and is NOT
                            either the current document, user or share */
                            Reference< browse::XBrowseNode >& theChild = children[n];
                            ::rtl::OUString uiName = theChild->getName();
                            BOOL bDisplay = TRUE;

                            if ( bIsRootNode )
                            {
                                if ( uiName.equals( sModelTitle ) )
                                {
                                    uiName = sDisplayTitle;
                                }
                                else if ( uiName.equals( user ) )
                                {
                                    uiName = m_sMyMacros;
                                }
                                else if ( uiName.equals( share ) )
                                {
                                    uiName = m_sProdMacros;
                                }
                                else
                                {
                                    bDisplay = FALSE;
                                }
                            }
                            if (children[n]->getType() != browse::BrowseNodeTypes::SCRIPT  && bDisplay )
                            {

                                /*
                                    We call acquire on the XBrowseNode so that it does not
                                    get autodestructed and become invalid when accessed later.
                                */
                                theChild->acquire();

                                SvxGroupInfo_Impl* pInfo =
                                    new SvxGroupInfo_Impl(SFX_CFGGROUP_SCRIPTCONTAINER,
                                        0, static_cast<void *>( theChild.get()));

                                OSL_TRACE("adding child node %s",::rtl::OUStringToOString( uiName, RTL_TEXTENCODING_ASCII_US ).pData->buffer );
                                Image aImage = GetImage( theChild, xContext, bIsRootNode,BMP_COLOR_NORMAL );
                                SvLBoxEntry* pNewEntry =
                                    InsertEntry( uiName, NULL);
                                SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                aImage = GetImage( theChild, xContext, bIsRootNode,BMP_COLOR_HIGHCONTRAST );
                                SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_HIGHCONTRAST);
                                SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_HIGHCONTRAST);

                                pNewEntry->SetUserData( pInfo );
                                aArr.Insert( pInfo, aArr.Count() );

                                if ( children[n]->hasChildNodes() )
                                {
                                    Sequence< Reference< browse::XBrowseNode > > grandchildren =
                                        children[n]->getChildNodes();

                                    for ( ULONG m = 0; m < grandchildren.getLength(); m++ )
                                    {
                                        if ( grandchildren[m]->getType() == browse::BrowseNodeTypes::CONTAINER )
                                        {
                                            pNewEntry->EnableChildsOnDemand( TRUE );
                                            m = grandchildren.getLength();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                catch (RuntimeException &e) {
                    // do nothing, the entry will not be displayed in the UI
                }
            }
        }
    }
    MakeVisible( GetEntry( 0,0 ) );
    SetUpdateMode( TRUE );
}

Image SvxConfigGroupListBox_Impl::GetImage( Reference< browse::XBrowseNode > node, Reference< XComponentContext > xCtx, bool bIsRootNode, bool bHighContrast )
{
    Image aImage;
    if ( bIsRootNode )
    {
        OUString user = OUString::createFromAscii("user");
        OUString share = OUString::createFromAscii("share");
        if (node->getName().equals( user ) || node->getName().equals(share ) )
        {
            if( bHighContrast == BMP_COLOR_NORMAL )
                aImage = m_hdImage;
            else
                aImage = m_hdImage_hc;
        }
        else
        {
            OUString factoryURL;
            OUString nodeName = node->getName();
            Reference<XInterface> xDocumentModel = getDocumentModel(xCtx, nodeName );
            if ( xDocumentModel.is() )
            {
                Reference< ::drafts::com::sun::star::frame::XModuleManager >
                    xModuleManager(
                        xCtx->getServiceManager()
                            ->createInstanceWithContext(
                                OUString::createFromAscii("drafts." // xxx todo
                                      "com.sun.star.frame.ModuleManager"),
                                xCtx ),
                            UNO_QUERY_THROW );
                Reference<container::XNameAccess> xModuleConfig(
                    xModuleManager, UNO_QUERY_THROW );
                // get the long name of the document:
                OUString appModule( xModuleManager->identify(
                                    xDocumentModel ) );
                Sequence<beans::PropertyValue> moduleDescr;
                Any aAny = xModuleConfig->getByName(appModule);
                if( sal_True != ( aAny >>= moduleDescr ) )
                {
                    throw RuntimeException(OUString::createFromAscii("SFTreeListBox::Init: failed to get PropertyValue"), Reference< XInterface >());
                }
                beans::PropertyValue const * pmoduleDescr =
                    moduleDescr.getConstArray();
                for ( sal_Int32 pos = moduleDescr.getLength(); pos--; )
                {
                    if (pmoduleDescr[ pos ].Name.equalsAsciiL(
                            RTL_CONSTASCII_STRINGPARAM(
                                "ooSetupFactoryEmptyDocumentURL") ))
                    {
                        pmoduleDescr[ pos ].Value >>= factoryURL;
                        OSL_TRACE("factory url for doc images is %s",
                        ::rtl::OUStringToOString( factoryURL , RTL_TEXTENCODING_ASCII_US ).pData->buffer );
                        break;
                    }
                }
            }
            if( factoryURL.getLength() > 0 )
            {
                if( bHighContrast == BMP_COLOR_NORMAL )
                    aImage = SvFileInformationManager::GetFileImage(
                        INetURLObject(factoryURL), false,
                        BMP_COLOR_NORMAL );
                else
                    aImage = SvFileInformationManager::GetFileImage(
                        INetURLObject(factoryURL), false,
                        BMP_COLOR_HIGHCONTRAST );
            }
            else
            {
                if( bHighContrast == BMP_COLOR_NORMAL )
                    aImage = m_docImage;
                else
                    aImage = m_docImage_hc;
            }
        }
    }
    else
    {
        if( node->getType() == browse::BrowseNodeTypes::SCRIPT )
        {
            if( bHighContrast == BMP_COLOR_NORMAL )
                aImage = m_macImage;
            else
                aImage = m_macImage_hc;
        }
        else
        {
            if( bHighContrast == BMP_COLOR_NORMAL )
                aImage = m_libImage;
            else
                aImage = m_libImage_hc;
        }
    }
    return aImage;
}

Reference< XInterface  >
SvxConfigGroupListBox_Impl::getDocumentModel(
    Reference< XComponentContext >& xCtx, OUString& docName )
{
    Reference< XInterface > xModel;
    Reference< lang::XMultiComponentFactory > mcf =
            xCtx->getServiceManager();
    Reference< frame::XDesktop > desktop (
        mcf->createInstanceWithContext(
            OUString::createFromAscii("com.sun.star.frame.Desktop"),                 xCtx ),
            UNO_QUERY );

    Reference< container::XEnumerationAccess > componentsAccess =
        desktop->getComponents();
    Reference< container::XEnumeration > components =
        componentsAccess->createEnumeration();
    sal_Int32 docIndex = 0;
    while (components->hasMoreElements())
    {
        Reference< frame::XModel > model(
            components->nextElement(), UNO_QUERY );
        if ( model.is() )
        {
            OUString sTdocUrl;
            SvxScriptSelectorDialog::GetDocTitle( model, sTdocUrl );

            if( sTdocUrl.equals( docName ) )
            {
                xModel = model;
                break;
            }
        }
    }
    return xModel;
}

void SvxConfigGroupListBox_Impl::GroupSelected()
{
    SvLBoxEntry *pEntry = FirstSelected();
    SvxGroupInfo_Impl *pInfo = (SvxGroupInfo_Impl*) pEntry->GetUserData();
    pFunctionListBox->SetUpdateMode(FALSE);
    pFunctionListBox->ClearAll();
    if ( pInfo->nKind != SFX_CFGGROUP_FUNCTION &&
             pInfo->nKind != SFX_CFGGROUP_BASICMOD &&
             pInfo->nKind != SFX_CFGGROUP_SCRIPTCONTAINER )
    {
        pFunctionListBox->SetUpdateMode(TRUE);
        return;
    }

    switch ( pInfo->nKind )
    {
        case SFX_CFGGROUP_FUNCTION :
        {
            SvLBoxEntry *pEntry = FirstSelected();
            if ( pEntry != NULL )
            {
                SvxGroupInfo_Impl *pInfo =
                    (SvxGroupInfo_Impl*) pEntry->GetUserData();

                Reference< frame::XDispatchInformationProvider > xDIP(
                    m_xFrame, UNO_QUERY );

                Sequence< frame::DispatchInformation > commands;
                try
                {
                    commands = xDIP->getConfigurableDispatchInformation(
                        pInfo->nOrd );
                }
                catch ( container::NoSuchElementException& )
                {
                }

                for ( sal_Int32 i = 0; i < commands.getLength(); i++ )
                {
                    if ( commands[i].Command.getLength() == 0 )
                    {
                        continue;
                    }

                    Image aImage;

                    OUString* aCmdURL = new OUString( commands[i].Command );
                        // RTL_CONSTASCII_USTRINGPARAM( ".uno:" ) );

                    if ( m_pImageProvider )
                    {
                        aImage = m_pImageProvider->GetImage( *aCmdURL );
                    }

                    OUString aLabel;
                    try
                    {
                        Any a = m_xModuleCommands->getByName( *aCmdURL );
                        Sequence< beans::PropertyValue > aPropSeq;

                        if ( a >>= aPropSeq )
                        {
                            for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
                            {
                                if ( aPropSeq[i].Name.equalsAscii( "Name" ) )
                                {
                                    aPropSeq[i].Value >>= aLabel;
                                    break;
                                }
                            }
                        }
                    }
                    catch ( container::NoSuchElementException& )
                    {
                    }

                    if ( aLabel.getLength() == 0 )
                    {
                        aLabel = commands[i].Command;
                    }

                    SvLBoxEntry* pFuncEntry = NULL;
                    if ( !!aImage )
                    {
                        pFuncEntry = pFunctionListBox->InsertEntry(
                            aLabel, aImage, aImage );
                    }
                    else
                    {
                        pFuncEntry = pFunctionListBox->InsertEntry(
                            aLabel, NULL );
                    }

                    SvxGroupInfo_Impl *pInfo = new SvxGroupInfo_Impl(
                        SFX_CFGFUNCTION_SLOT, 123, aCmdURL );

                    pFunctionListBox->aArr.Insert(
                        pInfo, pFunctionListBox->aArr.Count() );

                    pFuncEntry->SetUserData( pInfo );
                }
            }
            break;
        }

        case SFX_CFGGROUP_BASICMOD :
        {
            SvLBoxEntry *pLibEntry = GetParent( pEntry );
            SvxGroupInfo_Impl *pLibInfo =
                (SvxGroupInfo_Impl*) pLibEntry->GetUserData();
            SvLBoxEntry *pBasEntry = GetParent( pLibEntry );
            SvxGroupInfo_Impl *pBasInfo =
                (SvxGroupInfo_Impl*) pBasEntry->GetUserData();

            StarBASIC *pLib = (StarBASIC*) pLibInfo->pObject;
            SfxObjectShell *pDoc = NULL;
            if ( pBasInfo->nKind == SFX_CFGGROUP_DOCBASICMGR )
                pDoc = (SfxObjectShell*) pBasInfo->pObject;

            SbModule *pMod = (SbModule*) pInfo->pObject;
            for ( USHORT nMeth=0; nMeth < pMod->GetMethods()->Count(); nMeth++ )
            {
                SbxMethod *pMeth = (SbxMethod*)pMod->GetMethods()->Get(nMeth);
                SfxMacroInfoPtr pInf = new SfxMacroInfo( pDoc,
                                                         pLib->GetName(),
                                                         pMod->GetName(),
                                                         pMeth->GetName());
                if ( pMeth->GetInfo() )
                    pInf->SetHelpText( pMeth->GetInfo()->GetComment() );
                USHORT nId = SFX_APP()->GetMacroConfig()->GetSlotId( pInf );
                if ( !nId )
                    break;      // Kein Slot mehr frei

                SvLBoxEntry* pFuncEntry =
                    pFunctionListBox->InsertEntry( pMeth->GetName(), NULL );
                SvxGroupInfo_Impl *pInfo =
                    new SvxGroupInfo_Impl( SFX_CFGFUNCTION_MACRO, nId, pInf );
                pFunctionListBox->aArr.Insert( pInfo, pFunctionListBox->aArr.Count() );
                pFuncEntry->SetUserData( pInfo );
            }

            break;
        }

        case SFX_CFGGROUP_SCRIPTCONTAINER:
        {
            if ( !GetChildCount( pEntry ) )
            {
                Reference< browse::XBrowseNode > rootNode(
                    reinterpret_cast< browse::XBrowseNode* >( pInfo->pObject ) ) ;

                try {
                    if ( rootNode->hasChildNodes() )
                    {
                        Sequence< Reference< browse::XBrowseNode > > children =
                            rootNode->getChildNodes();

                        for ( ULONG n = 0; n < children.getLength(); n++ )
                        {
                            if (children[n]->getType() == browse::BrowseNodeTypes::SCRIPT)
                            {
                                OUString uri;
                                OUString description;

                                Reference < beans::XPropertySet >xPropSet( children[n], UNO_QUERY );
                                if (!xPropSet.is())
                                {
                                    continue;
                                }

                                Any value =
                                    xPropSet->getPropertyValue( String::CreateFromAscii( "URI" ) );
                                value >>= uri;

                                try
                                {
                                    value = xPropSet->getPropertyValue(
                                        String::CreateFromAscii( "Description" ) );
                                    value >>= description;
                                }
                                catch (Exception &e) {
                                    // do nothing, the description will be empty
                                }

                                ScriptInfo* aInfo = new ScriptInfo( uri );
                                aInfo->SetHelpText( description );

                                SvxGroupInfo_Impl* pInfo =
                                    new SvxGroupInfo_Impl(
                                        SFX_CFGFUNCTION_SCRIPT, 123, aInfo );

                                Image aImage = GetImage( children[n], Reference< XComponentContext >(), sal_False, BMP_COLOR_NORMAL );
                                SvLBoxEntry* pNewEntry =
                                    pFunctionListBox->InsertEntry( children[n]->getName(), NULL );
                                pFunctionListBox->SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                pFunctionListBox->SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                aImage = GetImage( children[n], Reference< XComponentContext >(), sal_False, BMP_COLOR_HIGHCONTRAST );
                                pFunctionListBox->SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_HIGHCONTRAST);
                                pFunctionListBox->SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_HIGHCONTRAST);

                                pNewEntry->SetUserData( pInfo );

                                pFunctionListBox->aArr.Insert(
                                    pInfo, pFunctionListBox->aArr.Count() );

                            }
                        }
                    }
                }
                catch (RuntimeException &e) {
                    // do nothing, the entry will not be displayed in the UI
                }
            }
            break;
        }

        default:
        {
            return;
            break;
        }
    }

    if ( pFunctionListBox->GetEntryCount() )
        pFunctionListBox->Select( pFunctionListBox->GetEntry( 0, 0 ) );

    pFunctionListBox->SetUpdateMode(TRUE);
}

BOOL SvxConfigGroupListBox_Impl::Expand( SvLBoxEntry* pParent )
{
    BOOL bRet = SvTreeListBox::Expand( pParent );
    if ( bRet )
    {
        // Wieviele Entries k"onnen angezeigt werden ?
        ULONG nEntries = GetOutputSizePixel().Height() / GetEntryHeight();

        // Wieviele Kinder sollen angezeigt werden ?
        ULONG nChildCount = GetVisibleChildCount( pParent );

        // Passen alle Kinder und der parent gleichzeitig in die View ?
        if ( nChildCount+1 > nEntries )
        {
            // Wenn nicht, wenigstens parent ganz nach oben schieben
            MakeVisible( pParent, TRUE );
        }
        else
        {
            // An welcher relativen ViewPosition steht der aufzuklappende parent
            SvLBoxEntry *pEntry = GetFirstEntryInView();
            ULONG nParentPos = 0;
            while ( pEntry && pEntry != pParent )
            {
                nParentPos++;
                pEntry = GetNextEntryInView( pEntry );
            }

            // Ist unter dem parent noch genug Platz f"ur alle Kinder ?
            if ( nParentPos + nChildCount + 1 > nEntries )
                ScrollOutputArea( (short)( nEntries - ( nParentPos + nChildCount + 1 ) ) );
        }
    }

    return bRet;
}

void SvxConfigGroupListBox_Impl::RequestingChilds( SvLBoxEntry *pEntry )
{
    SvxGroupInfo_Impl *pInfo = (SvxGroupInfo_Impl*) pEntry->GetUserData();
    pInfo->bWasOpened = TRUE;
    switch ( pInfo->nKind )
    {
        case SFX_CFGGROUP_BASICMGR :
        case SFX_CFGGROUP_DOCBASICMGR :
        {
            if ( !GetChildCount( pEntry ) )
            {
                // Erstmaliges "Offnen
                BasicManager *pMgr;
                if ( pInfo->nKind == SFX_CFGGROUP_DOCBASICMGR )
                    pMgr = ((SfxObjectShell*)pInfo->pObject)->GetBasicManager();
                else
                    pMgr = (BasicManager*) pInfo->pObject;

                SvLBoxEntry *pLibEntry = 0;
                for ( USHORT nLib=0; nLib<pMgr->GetLibCount(); nLib++)
                {
                    StarBASIC* pLib = pMgr->GetLib( nLib );
                    pLibEntry = InsertEntry( pMgr->GetLibName( nLib ), pEntry );
                    SvxGroupInfo_Impl *pInfo = new SvxGroupInfo_Impl( SFX_CFGGROUP_BASICLIB, nLib, pLib );
                    aArr.Insert( pInfo, aArr.Count() );
                    pLibEntry->SetUserData( pInfo );
                    pLibEntry->EnableChildsOnDemand( TRUE );
                }
            }

            break;
        }

        case SFX_CFGGROUP_BASICLIB :
        {
            if ( !GetChildCount( pEntry ) )
            {
                // Erstmaliges "Offnen
                StarBASIC *pLib = (StarBASIC*) pInfo->pObject;
                if ( !pLib )
                {
                    // Lib mu\s nachgeladen werden
                    SvLBoxEntry *pParent = GetParent( pEntry );
                    SvxGroupInfo_Impl *pInf =
                        (SvxGroupInfo_Impl*) pParent->GetUserData();
                    BasicManager *pMgr;
                    if ( pInf->nKind == SFX_CFGGROUP_DOCBASICMGR )
                        pMgr = ((SfxObjectShell*)pInf->pObject)->GetBasicManager();
                    else
                        pMgr = (BasicManager*) pInf->pObject;

                    if ( pMgr->LoadLib( pInfo->nOrd ) )
                        pInfo->pObject = pLib = pMgr->GetLib( pInfo->nOrd );
                    else
                        break;
                }

                SvLBoxEntry *pModEntry = 0;
                for ( USHORT nMod=0; nMod<pLib->GetModules()->Count(); nMod++ )
                {
                    SbModule* pMod = (SbModule*)pLib->GetModules()->Get( nMod );

                    pModEntry = InsertEntry( pMod->GetName(), pEntry );
                    SvxGroupInfo_Impl *pInfo =
                        new SvxGroupInfo_Impl( SFX_CFGGROUP_BASICMOD, 0, pMod );
                    aArr.Insert( pInfo, aArr.Count() );
                    pModEntry->SetUserData( pInfo );
                }
            }

            break;
        }

        case SFX_CFGGROUP_SCRIPTCONTAINER:
        {
            if ( !GetChildCount( pEntry ) )
            {
                Reference< browse::XBrowseNode > rootNode(
                    reinterpret_cast< browse::XBrowseNode* >( pInfo->pObject ) ) ;
                try {
                    if ( rootNode->hasChildNodes() )
                    {
                        Sequence< Reference< browse::XBrowseNode > > children =
                            rootNode->getChildNodes();
                        BOOL bIsRootNode = FALSE;

                        OUString user = OUString::createFromAscii("user");
                        OUString share = OUString::createFromAscii("share");
                        if ( rootNode->getName().equals(OUString::createFromAscii("Root") ))
                        {
                            bIsRootNode = TRUE;
                        }

                        OUString sDisplayTitle;
                        OUString sModelTitle;
                        SfxObjectShell* pCurrentDoc = SfxObjectShell::GetWorkingDocument();
                        if ( pCurrentDoc )
                        {
                            sDisplayTitle = pCurrentDoc->GetTitle();
                            sModelTitle = xModelToDocTitle( pCurrentDoc->GetModel() );
                        }

                        if ( sDisplayTitle.getLength() == 0 && sModelTitle.getLength() != 0 )
                        {
                            sDisplayTitle = sModelTitle;
                        }

                        for ( ULONG n = 0; n < children.getLength(); n++ )
                        {
                            /* To mimic current starbasic behaviour we
                            need to make sure that only the current document
                            is displayed in the config tree. Tests below
                            set the bDisplay flag to FALSE if the current
                            node is a first level child of the Root and is NOT
                            either the current document, user or share */
                            Reference< browse::XBrowseNode >& theChild = children[n];
                            ::rtl::OUString uiName = theChild->getName();
                            BOOL bDisplay = TRUE;

                            if ( bIsRootNode )
                            {
                                if ( uiName.equals( sModelTitle ) )
                                {
                                    uiName = sDisplayTitle;
                                }
                                else if ( uiName.equals( user ) )
                                {
                                    uiName = m_sMyMacros;
                                }
                                else if ( uiName.equals( share ) )
                                {
                                    uiName = m_sProdMacros;
                                }
                                else
                                {
                                    bDisplay = FALSE;
                                }
                            }
                            if (children[n]->getType() != browse::BrowseNodeTypes::SCRIPT  && bDisplay )
                            {

                                /*
                                    We call acquire on the XBrowseNode so that it does not
                                    get autodestructed and become invalid when accessed later.
                                */
                                theChild->acquire();

                                SvxGroupInfo_Impl* pInfo =
                                    new SvxGroupInfo_Impl(SFX_CFGGROUP_SCRIPTCONTAINER,
                                        0, static_cast<void *>( theChild.get()));

                                OUString uiName = theChild->getName();
                                sal_Bool isRootNode = sal_False;
                                //if we show slots (as in the customize dialog)
                                //then the user & share are added at depth=1
                                if(nMode && GetModel()->GetDepth(pEntry) == 0)
                                {
                                    if ( uiName.equals( user ) )
                                    {
                                        uiName = m_sMyMacros;
                                        isRootNode = sal_True;
                                    }
                                    else if ( uiName.equals( share ) )
                                    {
                                        uiName = m_sProdMacros;
                                        isRootNode = sal_True;
                                    }
                                }
                                Image aImage = GetImage( theChild, Reference< XComponentContext >(), isRootNode, BMP_COLOR_NORMAL );
                                SvLBoxEntry* pNewEntry =
                                    InsertEntry( uiName, pEntry );
                                SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                aImage = GetImage( theChild, Reference< XComponentContext >(), isRootNode, BMP_COLOR_HIGHCONTRAST );
                                SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_HIGHCONTRAST);
                                SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_HIGHCONTRAST);

                                pNewEntry->SetUserData( pInfo );
                                aArr.Insert( pInfo, aArr.Count() );

                                /* i30923 - Would be nice if there was a better
                                 * way to determine if a basic lib had children
                                 * without having to ask for them (which forces
                                 * the library to be loaded */
                                pNewEntry->EnableChildsOnDemand( TRUE );
                            }
                        }
                    }
                }
                catch (RuntimeException &e) {
                    // do nothing, the entry will not be displayed in the UI
                }
            }
            break;
        }

        default:
            DBG_ERROR( "Falscher Gruppentyp!" );
            break;
    }
}

void SvxConfigGroupListBox_Impl::SelectMacro( const SfxMacroInfoItem *pItem )
{
    SelectMacro( pItem->GetBasicManager()->GetName(),
                 pItem->GetQualifiedName() );
}

void SvxConfigGroupListBox_Impl::SelectMacro( const String& rBasic,
         const String& rMacro )
{
    String aBasicName( rBasic );
    aBasicName += ' ';
    aBasicName += String( ResId( STR_BASICMACROS, DIALOG_MGR() ) );

    String aLib, aModule, aMethod;
    USHORT nCount = rMacro.GetTokenCount('.');
    aMethod = rMacro.GetToken( nCount-1, '.' );
    if ( nCount > 2 )
    {
        aLib = rMacro.GetToken( 0, '.' );
        aModule = rMacro.GetToken( nCount-2, '.' );
    }

    SvLBoxEntry *pEntry = FirstChild(0);
    while ( pEntry )
    {
        String aEntryBas = GetEntryText( pEntry );
        if ( aEntryBas == aBasicName )
        {
            Expand( pEntry );
            SvLBoxEntry *pLib = FirstChild( pEntry );
            while ( pLib )
            {
                String aEntryLib = GetEntryText( pLib );
                if ( aEntryLib == aLib )
                {
                    Expand( pLib );
                    SvLBoxEntry *pMod = FirstChild( pLib );
                    while ( pMod )
                    {
                        String aEntryMod = GetEntryText( pMod );
                        if ( aEntryMod == aModule )
                        {
                            Expand( pMod );
                            MakeVisible( pMod );
                            Select( pMod );
                            SvLBoxEntry *pMethod = pFunctionListBox->First();
                            while ( pMethod )
                            {
                                String aEntryMethod = GetEntryText( pMethod );
                                if ( aEntryMethod == aMethod )
                                {
                                    pFunctionListBox->Select( pMethod );
                                    pFunctionListBox->MakeVisible( pMethod );
                                    return;
                                }
                                pMethod = pFunctionListBox->Next( pMethod );
                            }
                        }
                        pMod = NextSibling( pMod );
                    }
                }
                pLib = NextSibling( pLib );
            }
        }
        pEntry = NextSibling( pEntry );
    }
}

void
SvxScriptSelectorDialog::GetDocTitle(
    const Reference< frame::XModel >& xModel,
    OUString& rTitle )
{
    OUString aResult = OUString::createFromAscii("Unknown");

    if ( !xModel.is() )
    {
        rTitle = aResult;
        return;
    }

    try
    {
        Reference< beans::XPropertySet > xProps(
            xModel->getCurrentController()->getFrame(), UNO_QUERY_THROW );

        OUString prop = OUString::createFromAscii( "Title" );
        OUString tmp;

        if ( sal_True == ( xProps->getPropertyValue( prop ) >>= tmp ) )
        {
            if ( xModel->getURL().getLength() == 0 )
            {
                // process "UntitledX - YYYYYYYY" // to get UntitledX
                sal_Int32 pos = 0;
                aResult = tmp.getToken(0, ' ', pos);
            }
            else
            {
                Reference< document::XDocumentInfoSupplier > xDIS(
                    xModel, UNO_QUERY );

                if ( xDIS.is() )
                {
                    Reference< beans::XPropertySet > xProp (
                        xDIS->getDocumentInfo(), UNO_QUERY );
                    xProp->getPropertyValue( prop ) >>= tmp;
                }

                if ( tmp.getLength() == 0 )
                {
                    // strip out the last leaf of location name
                    // e.g. file://dir1/dir2/Blah.sxw - > Blah.sxw
                    tmp = xModel->getURL();
                    sal_Int32 idx = tmp.lastIndexOf( '/' );
                    if ( ( idx + 1 ) <  tmp.getLength()  )
                    {
                        tmp = tmp.copy( idx + 1 );
                    }
                }
                aResult = tmp;
            }
        }
    }
    catch ( Exception& e )
    {
        OSL_TRACE("GetDocTitle() exception: %s", PRTSTR(e.Message));
    }
    rTitle = aResult;
}

/*
 * Implementation of SvxScriptSelectorDialog
 *
 * This dialog is used for selecting Slot API commands
 * and Scripting Framework Scripts.
 */

SvxScriptSelectorDialog::SvxScriptSelectorDialog(
  Window* pParent, BOOL bShowSlots, const Reference< frame::XFrame >& xFrame )
    :
    ModelessDialog( pParent, SVX_RES( RID_DLG_SCRIPTSELECTOR ) ),
    aDialogDescription( this, ResId( TXT_SELECTOR_DIALOG_DESCRIPTION ) ),
    aGroupText( this, ResId( TXT_SELECTOR_CATEGORIES ) ),
    aCategories( this, ResId( BOX_SELECTOR_CATEGORIES ),
        bShowSlots ? SFX_SLOT_MENUCONFIG : 0, xFrame ),
    aFunctionText( this, ResId( TXT_SELECTOR_COMMANDS ) ),
    aCommands( this, ResId( BOX_SELECTOR_COMMANDS ) ),
    aOKButton( this, ResId( BTN_SELECTOR_OK ) ),
    aCancelButton( this, ResId( BTN_SELECTOR_CANCEL ) ),
    aHelpButton( this, ResId( BTN_SELECTOR_HELP ) ),
    aDescription( this, ResId( GRP_SELECTOR_DESCRIPTION ) ),
    aDescriptionText( this, ResId( TXT_SELECTOR_DESCRIPTION ) ),
    m_bShowSlots( bShowSlots )
{

    ResMgr* pMgr = DIALOG_MGR();

    // If we are showing Slot API commands update labels in the UI, and
    // enable drag'n'drop
    if ( m_bShowSlots )
    {
        aGroupText.SetText( String( ResId( STR_SELECTOR_CATEGORIES, pMgr ) ) );
        aOKButton.SetText( String( ResId( STR_SELECTOR_ADD, pMgr ) ) );
        aCancelButton.SetText( String( ResId( STR_SELECTOR_CLOSE, pMgr ) ) );
        aFunctionText.SetText( String( ResId( STR_SELECTOR_COMMANDS, pMgr ) ) );
        aDialogDescription.SetText(
                    String( ResId( STR_SELECTOR_ADD_COMMANDS_DESCRIPTION, pMgr ) ) );
        SetText( String( ResId( STR_SELECTOR_ADD_COMMANDS, pMgr ) ) );

        aCommands.SetDragDropMode( SV_DRAGDROP_APP_COPY );
    }

    ResizeControls();

    aCategories.SetFunctionListBox( &aCommands );
    aCategories.Init();
    // aCategories.Select( aCategories.GetEntry( 0, 0 ) );

    aCategories.SetSelectHdl(
            LINK( this, SvxScriptSelectorDialog, SelectHdl ) );
    aCommands.SetSelectHdl( LINK( this, SvxScriptSelectorDialog, SelectHdl ) );

    aOKButton.SetClickHdl( LINK( this, SvxScriptSelectorDialog, ClickHdl ) );
    aCancelButton.SetClickHdl( LINK( this, SvxScriptSelectorDialog, ClickHdl ) );

    UpdateUI();
    FreeResource();
}

void SvxScriptSelectorDialog::ResizeControls()
{
    Point p, newp;
    Size s, news;
    long gap;

    USHORT style = TEXT_DRAW_MULTILINE | TEXT_DRAW_TOP |
                   TEXT_DRAW_LEFT | TEXT_DRAW_WORDBREAK;

    // get dimensions of dialog instructions control
    p = aDialogDescription.GetPosPixel();
    s = aDialogDescription.GetSizePixel();

    // get dimensions occupied by text in the control
    Rectangle rect =
        GetTextRect( Rectangle( p, s ), aDialogDescription.GetText(), style );
    news = rect.GetSize();

    // the gap is the difference between the control height and its text height
    gap = s.Height() - news.Height();

    // resize the dialog instructions control
    news = Size( s.Width(), s.Height() - gap );
    aDialogDescription.SetSizePixel( news );

    // resize other controls to fill the gap
    p = aGroupText.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aGroupText.SetPosPixel( newp );

    p = aCategories.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aCategories.SetPosPixel( newp );
    s = aCategories.GetSizePixel();
    news = Size( s.Width(), s.Height() + gap );
    aCategories.SetSizePixel( news );

    p = aFunctionText.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aFunctionText.SetPosPixel( newp );

    p = aCommands.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aCommands.SetPosPixel( newp );
    s = aCommands.GetSizePixel();
    news = Size( s.Width(), s.Height() + gap );
    aCommands.SetSizePixel( news );

    p = aOKButton.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aOKButton.SetPosPixel( newp );

    p = aCancelButton.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aCancelButton.SetPosPixel( newp );

    p = aHelpButton.GetPosPixel();
    newp = Point( p.X(), p.Y() - gap );
    aHelpButton.SetPosPixel( newp );
}

SvxScriptSelectorDialog::~SvxScriptSelectorDialog()
{
}

IMPL_LINK( SvxScriptSelectorDialog, SelectHdl, Control*, pCtrl )
{
    if ( pCtrl == &aCategories )
    {
        aCategories.GroupSelected();
    }
    else if ( pCtrl == &aCommands )
    {
        aCommands.FunctionSelected();
    }
    UpdateUI();
    return 0;
}

// Check if command is selected and enable the OK button accordingly
// Grab the help text for this id if available and update the description field
void
SvxScriptSelectorDialog::UpdateUI()
{
    OUString url = GetScriptURL();
    if ( url != NULL && url.getLength() != 0 )
    {
        String rMessage =
            aCommands.GetHelpText( aCommands.FirstSelected() );
        aDescriptionText.SetText( rMessage );

        aOKButton.Enable( TRUE );
    }
    else
    {
        aDescriptionText.SetText( String() );
        aOKButton.Enable( FALSE );
    }
}

IMPL_LINK( SvxScriptSelectorDialog, ClickHdl, Button *, pButton )
{
    if ( pButton == &aCancelButton )
    {
        // If we are displaying Slot API commands then the dialog is being
        // run from Tools/Configure and we should not close it, just hide it
        if ( m_bShowSlots == FALSE )
        {
            EndDialog( RET_CANCEL );
        }
        else
        {
            Hide();
        }
    }
    else if ( pButton == &aOKButton )
    {
        GetAddHdl().Call( this );

        // If we are displaying Slot API commands then this the dialog is being
        // run from Tools/Configure and we should not close it
        if ( m_bShowSlots == FALSE )
        {
            EndDialog( RET_OK );
        }
        else
        {
            // Select the next entry in the list if possible
            SvLBoxEntry* current = aCommands.FirstSelected();
            SvLBoxEntry* next = aCommands.NextSibling( current );

            if ( next != NULL )
            {
                aCommands.Select( next );
            }
        }
    }

    return 0;
}

void
SvxScriptSelectorDialog::SetRunLabel()
{
    aOKButton.SetText( String( ResId( STR_SELECTOR_RUN, DIALOG_MGR() ) ) );
}

USHORT
SvxScriptSelectorDialog::GetSelectedId()
{
    return aCommands.GetId( aCommands.GetLastSelectedEntry() );
}

String
SvxScriptSelectorDialog::GetScriptURL()
{
    OUString result;

    SvLBoxEntry *pEntry = aCommands.GetLastSelectedEntry();
    if ( pEntry )
    {
        SvxGroupInfo_Impl *pData = (SvxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pData->nKind == SFX_CFGFUNCTION_SLOT )
        {
            if ( pData->pObject != NULL )
            {
                OUString* tmp = (OUString*) pData->pObject;
                result = OUString( *tmp );
            }
        }
        else if ( pData->nKind == SFX_CFGFUNCTION_SCRIPT )
        {
            ScriptInfo* pInfo = (ScriptInfo*) pData->pObject;
            if ( pInfo != NULL )
            {
                result = pInfo->GetURI();
            }
        }
        else
        {
            SfxMacroInfo* info = aCommands.GetMacroInfo();

            if ( info != NULL )
            {
                result = info->GetURL();
            }
        }
    }

    return result;
}

String
SvxScriptSelectorDialog::GetSelectedDisplayName()
{
    return aCommands.GetEntryText( aCommands.GetLastSelectedEntry() );
}

String
SvxScriptSelectorDialog::GetSelectedHelpText()
{
    return aCommands.GetHelpText( aCommands.GetLastSelectedEntry() );
}
