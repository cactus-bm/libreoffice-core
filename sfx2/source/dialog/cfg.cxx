/*************************************************************************
 *
 *  $RCSfile: cfg.cxx,v $
 *
 *  $Revision: 1.43 $
 *
 *  last change: $Author: rt $ $Date: 2004-10-22 14:40:07 $
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
#ifndef _SV_WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#include <tools/urlobj.hxx>
#include <svtools/pathoptions.hxx>
#include <sot/storage.hxx>

#ifndef GCC
#pragma hdrstop
#endif

#include "cfg.hxx"

#include "dialog.hrc"
#include "cfg.hrc"
#ifndef _SFX_HELPID_HRC
#include "helpid.hrc"
#endif

#include "app.hxx"
#include "appdata.hxx"
#include "msg.hxx"
#include "msgpool.hxx"
#include "stbmgr.hxx"
#include "sfxresid.hxx"
#include "macrconf.hxx"
#include "minfitem.hxx"
#include "cfgmgr.hxx"
#include "sfxresid.hxx"
#include "objsh.hxx"
#include "dispatch.hxx"
#include "sfxtypes.hxx"
#include "eventdlg.hxx"
#include "tbxopdlg.hxx"
#include "minfitem.hxx"
#include "viewfrm.hxx"
#include "workwin.hxx"
#include "filedlghelper.hxx"
#include "request.hxx"

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

#ifndef  _COM_SUN_STAR_STYLE_XSTYLEFAMILIESSUPPLIER_HPP_
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::script;
namespace css = ::com::sun::star;

#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#include <svtools/imagemgr.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustrbuf.hxx>

USHORT SfxStylesInfo_Impl::START_ID_STYLES = 33000;

SfxStylesInfo_Impl::SfxStylesInfo_Impl()
    : m_nIDPool( 0 )
{}

void SfxStylesInfo_Impl::setModel(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel)
{
    m_xDoc = xModel;
}

USHORT SfxStylesInfo_Impl::cacheStyle(const ::rtl::OUString& sCommand)
{
    // command already cached?
    TCommand2IDMap::const_iterator pStyle = m_lCommand2IDs.find(sCommand);
    if (pStyle != m_lCommand2IDs.end())
        // yes - return ID
        return pStyle->second;

    // otherwise cache it as new item
    USHORT nOffset = m_nIDPool;
    USHORT nID     = (SfxStylesInfo_Impl::START_ID_STYLES + nOffset);
    m_lCommand2IDs[sCommand] = nID;
    m_lID2Commands.push_back(sCommand);
    m_nIDPool++;
    return nID;
}

static ::rtl::OUString FAMILY_CHARACTERSTYLE = ::rtl::OUString::createFromAscii("CharacterStyles");
static ::rtl::OUString FAMILY_PARAGRAPHSTYLE = ::rtl::OUString::createFromAscii("ParagraphStyles");
static ::rtl::OUString FAMILY_FRAMESTYLE     = ::rtl::OUString::createFromAscii("FrameStyles"    );
static ::rtl::OUString FAMILY_PAGESTYLE      = ::rtl::OUString::createFromAscii("PageStyles"     );
static ::rtl::OUString FAMILY_NUMBERINGSTYLE = ::rtl::OUString::createFromAscii("NumberingStyles");

static ::rtl::OUString CMDURL_SPART  = ::rtl::OUString::createFromAscii(".uno:StyleApply?Style:string=");
static ::rtl::OUString CMDURL_FPART2 = ::rtl::OUString::createFromAscii("&FamilyName:string=");

sal_Bool SfxStylesInfo_Impl::parseStyleCommand(const ::rtl::OUString&   sCommand,
                                                     SfxStyleInfo_Impl& aStyle  )
{
    sal_Int32 i1  = sCommand.indexOf(CMDURL_SPART);
    sal_Int32 i2  = sCommand.indexOf(CMDURL_FPART2);
    sal_Int32 i3  = CMDURL_SPART.getLength();
    sal_Int32 i4  = CMDURL_FPART2.getLength();
    sal_Int32 i5  = i2+i4;

    // not a style command in general!
    if (i1!=0)
        return sal_False;

    if (i2>i3)
    {
        aStyle.sStyle  = sCommand.copy(i3,i2-i3);
        aStyle.sFamily = sCommand.copy(i5);
    }

    if (!aStyle.sFamily.getLength() || !aStyle.sStyle.getLength())
    {
        DBG_ERROR("SfxStylesInfo_Impl::parseCommand()\nWrong format of command detected.\n");
        return sal_False;
    }

    aStyle.nId = cacheStyle(sCommand);
    aStyle.sLabel = aStyle.sStyle; // writer does the same!
    return sal_True;
}

::rtl::OUString SfxStylesInfo_Impl::generateCommand(const ::rtl::OUString sFamily, ::rtl::OUString sStyle)
{
    ::rtl::OUStringBuffer sCommand(1024);
    sCommand.append(CMDURL_SPART );
    sCommand.append(sStyle       );
    sCommand.append(CMDURL_FPART2);
    sCommand.append(sFamily      );
    return sCommand.makeStringAndClear();
}

::rtl::OUString SfxStylesInfo_Impl::getStyle(USHORT nID)
{
    if (nID < SfxStylesInfo_Impl::START_ID_STYLES)
        return ::rtl::OUString();

    USHORT nOffset = (nID - SfxStylesInfo_Impl::START_ID_STYLES);
    USHORT nSize   = (USHORT)m_lID2Commands.size();

    if (nSize > nOffset)
        return m_lID2Commands[nOffset];

    DBG_ERROR("SfxStylesInfo_Impl::getStyle()\nID of style does not firt range of cache!");
    return ::rtl::OUString();
}

::std::vector< SfxStyleInfo_Impl > SfxStylesInfo_Impl::getStyleFamilies()
{
    static ::rtl::OUString PROP_UINAME = ::rtl::OUString::createFromAscii("DisplayName");

    // Its an optional interface!
    css::uno::Reference< css::style::XStyleFamiliesSupplier > xModel(m_xDoc, css::uno::UNO_QUERY);
    if (!xModel.is())
        return ::std::vector< SfxStyleInfo_Impl >();

    css::uno::Reference< css::container::XNameAccess > xCont = xModel->getStyleFamilies();
    css::uno::Sequence< ::rtl::OUString > lFamilyNames = xCont->getElementNames();
    ::std::vector< SfxStyleInfo_Impl > lFamilies;
    sal_Int32 c = lFamilyNames.getLength();
    sal_Int32 i = 0;
    for(i=0; i<c; ++i)
    {
        SfxStyleInfo_Impl aFamilyInfo;
        aFamilyInfo.sFamily = lFamilyNames[i];
        aFamilyInfo.nId     = (USHORT)i;

        try
        {
            css::uno::Reference< css::beans::XPropertySet > xFamilyInfo;
            xCont->getByName(aFamilyInfo.sFamily) >>= xFamilyInfo;
            if (!xFamilyInfo.is())
            {
                // TODO_AS currently there is no support for an UIName property .. use internal family name instead
                aFamilyInfo.sLabel = aFamilyInfo.sFamily;
            }
            else
                xFamilyInfo->getPropertyValue(PROP_UINAME) >>= aFamilyInfo.sLabel;
        }
        catch(const css::uno::RuntimeException& exRun)
            { throw exRun; }
        catch(const css::uno::Exception&)
            { return ::std::vector< SfxStyleInfo_Impl >(); }

        lFamilies.push_back(aFamilyInfo);
    }

    return lFamilies;
}

::std::vector< SfxStyleInfo_Impl > SfxStylesInfo_Impl::getStyles(const ::rtl::OUString& sFamily)
{
    static ::rtl::OUString PROP_UINAME = ::rtl::OUString::createFromAscii("DisplayName");

    css::uno::Sequence< ::rtl::OUString > lStyleNames;
    css::uno::Reference< css::style::XStyleFamiliesSupplier > xModel(m_xDoc, css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::container::XNameAccess > xFamilies = xModel->getStyleFamilies();
    css::uno::Reference< css::container::XNameAccess > xStyleSet;
    try
    {
        xFamilies->getByName(sFamily) >>= xStyleSet;
        lStyleNames = xStyleSet->getElementNames();
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        { return ::std::vector< SfxStyleInfo_Impl >(); }

    ::std::vector< SfxStyleInfo_Impl > lStyles;
    sal_Int32                          c      = lStyleNames.getLength();
    sal_Int32                          i      = 0;
    for (i=0; i<c; ++i)
    {
        SfxStyleInfo_Impl aStyleInfo;
        aStyleInfo.sFamily  = sFamily;
        aStyleInfo.sStyle   = lStyleNames[i];
        aStyleInfo.sCommand = SfxStylesInfo_Impl::generateCommand(aStyleInfo.sFamily, aStyleInfo.sStyle);
        aStyleInfo.nId      = cacheStyle(aStyleInfo.sCommand);

        try
        {
            css::uno::Reference< css::beans::XPropertySet > xStyle;
            xStyleSet->getByName(aStyleInfo.sStyle) >>= xStyle;
            if (!xStyle.is())
                continue;
            xStyle->getPropertyValue(PROP_UINAME) >>= aStyleInfo.sLabel;
        }
        catch(const css::uno::RuntimeException& exRun)
            { throw exRun; }
        catch(const css::uno::Exception&)
            { continue; }

        lStyles.push_back(aStyleInfo);
    }
    return lStyles;
}
/*
struct SfxStatBarInfo_Impl
{
    USHORT          nId;
    String          aHelpText;

    SfxStatBarInfo_Impl(USHORT nID, String aString) :
        nId(nID),
        aHelpText(aString)
    {}
};
*/
//DECL_PTRARRAY(SfxStbInfoArr_Impl, SfxStatBarInfo_Impl*, 20, 4);
SV_IMPL_PTRARR(SfxGroupInfoArr_Impl, SfxGroupInfoPtr);

SfxMenuConfigEntry::SfxMenuConfigEntry( USHORT nInitId, const String& rInitStr,
                                    const String& rHelpText, BOOL bPopup )
    : nId( nInitId )
    , bPopUp(bPopup)
    , aHelpText(rHelpText)
    , aStr(rInitStr)
    , bConfigure( TRUE )
    , bStrEdited( FALSE )
{
    if ( SfxMacroConfig::IsMacroSlot( nId ) )
    {
        SFX_APP()->GetMacroConfig()->RegisterSlotId( nId );
        SfxMacroInfo* pInfo = SFX_APP()->GetMacroConfig()->GetMacroInfo( nId );
        if ( pInfo )
            aCommand = pInfo->GetURL();
        bStrEdited = TRUE;
    }
}

void SfxMenuConfigEntry::SetId( USHORT nNew )
{
    if ( SfxMacroConfig::IsMacroSlot( nId ) )
        SFX_APP()->GetMacroConfig()->ReleaseSlotId( nId );
    nId = nNew;
    if ( SfxMacroConfig::IsMacroSlot( nId ) )
    {
        SFX_APP()->GetMacroConfig()->RegisterSlotId( nId );
        SfxMacroInfo* pInfo = SFX_APP()->GetMacroConfig()->GetMacroInfo( nId );
        if ( pInfo )
            aCommand = pInfo->GetURL();
        bStrEdited = TRUE;
    }
    else
    {
        // Remove command string to fix problems if macro menu entry is overwritten
        // by a normal function!
        aCommand = String();
    }
}

SfxMenuConfigEntry::~SfxMenuConfigEntry()
{
    if ( SfxMacroConfig::IsMacroSlot( nId ) )
        SFX_APP()->GetMacroConfig()->ReleaseSlotId( nId );
}

SfxConfigFunctionListBox_Impl::SfxConfigFunctionListBox_Impl( Window* pParent, const ResId& rResId)
/*  Beschreibung
    Diese Listbox arbeiteitet mit der Grouplistbox zusammen und zeigt die zur Konfigurierung verf"ugbaren
    Funktionen bzw. Macros an, die zur selektierten Gruppe bzw. zum selektierten Modul in der Grouplistbox
    geh"oren.
*/
    : SvTreeListBox( pParent, rResId )
    , pCurEntry( 0 )
    , pStylesInfo( 0 )
{
    SetWindowBits( GetStyle() | WB_CLIPCHILDREN | WB_HSCROLL | WB_SORT );
    GetModel()->SetSortMode( SortAscending );

    // Timer f"ur die BallonHelp
    aTimer.SetTimeout( 200 );
    aTimer.SetTimeoutHdl(
        LINK( this, SfxConfigFunctionListBox_Impl, TimerHdl ) );
}

SfxConfigFunctionListBox_Impl::~SfxConfigFunctionListBox_Impl()
{
    ClearAll();
}

void SfxConfigFunctionListBox_Impl::MouseMove( const MouseEvent& rMEvt )
/*  Beschreibung
    Virtuelle Methode, die gerufen wird, wenn der Mauszeiger "uber der TreeListBox bewegt wurde.
    Wenn die Position des Mauszeigers "uber dem aktuell selektierten Entry liegt, wird ein Timer
    aufgesetzt, um ggf. einen Hilfetext einzublenden.
*/
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


IMPL_LINK( SfxConfigFunctionListBox_Impl, TimerHdl, Timer*, pTimer)
/*  Beschreibung
    Timer-Handler f"ur die Einblendung eines Hilfetextes. Wenn nach Ablauf des Timers
    der Mauszeiger immer noch auf dem aktuell selektierten Eintrag steht, wird der
    Helptext des Entries als Balloon-Help eingeblendet.
*/
{
    aTimer.Stop();
    Point aMousePos = GetPointerPosPixel();
    SvLBoxEntry *pEntry = GetCurEntry();
    if ( pEntry && GetEntry( aMousePos ) == pEntry && pCurEntry == pEntry )
        Help::ShowBalloon( this, OutputToScreenPixel( aMousePos ), GetHelpText( pEntry ) );
    return 0L;
}

void SfxConfigFunctionListBox_Impl::ClearAll()
/*  Beschreibung
    L"oscht alle Eintr"age in der FunctionListBox, alle UserDaten und alle evtl.
    vorhandenen MacroInfos.
*/
{
    USHORT nCount = aArr.Count();
    for ( USHORT i=0; i<nCount; i++ )
    {
        SfxGroupInfo_Impl *pData = aArr[i];

        if ( pData->nKind == SFX_CFGFUNCTION_MACRO ||
                 pData->nKind == SFX_CFGFUNCTION_SCRIPT )
        {
            SfxMacroInfo *pInfo = (SfxMacroInfo*) pData->pObject;
            SFX_APP()->GetMacroConfig()->ReleaseSlotId( pInfo->GetSlotId() );
            delete pInfo;
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

SvLBoxEntry* SfxConfigFunctionListBox_Impl::GetEntry_Impl( const String& rName )
/*  Beschreibung
    Ermittelt den SvLBoxEntry zu einem "ubergebenen String. Das setzt voraus, da\s
    die Namen eindeutig sind.
*/
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

SvLBoxEntry* SfxConfigFunctionListBox_Impl::GetEntry_Impl( USHORT nId )
/*  Beschreibung
    Ermittelt den SvLBoxEntry zu einer "ubergebenen Id.
*/
{
    SvLBoxEntry *pEntry = First();
    while ( pEntry )
    {
        SfxGroupInfo_Impl *pData = (SfxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pData && pData->nOrd == nId )
            return pEntry;
        pEntry = Next( pEntry );
    }

    return NULL;
}

SfxMacroInfo* SfxConfigFunctionListBox_Impl::GetMacroInfo()
/*  Beschreibung
    Gibt die MacroInfo des selektierten Entry zur"uck ( sofern vorhanden ).
*/
{
    SvLBoxEntry *pEntry = FirstSelected();
    if ( pEntry )
    {
        SfxGroupInfo_Impl *pData = (SfxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pData && ( pData->nKind == SFX_CFGFUNCTION_MACRO ||
                                        pData->nKind == SFX_CFGFUNCTION_SCRIPT ) )
            return (SfxMacroInfo*) pData->pObject;
    }

    return 0;
}

USHORT SfxConfigFunctionListBox_Impl::GetId( SvLBoxEntry *pEntry )
/*  Beschreibung
    Gibt die Ordnungsnummer ( SlotId oder Macro-Nummer ) des Eintrags zur"uck.
*/
{
    SfxGroupInfo_Impl *pData = pEntry ?
        (SfxGroupInfo_Impl*) pEntry->GetUserData() : 0;
    if ( pData )
        return pData->nOrd;
    return 0;
}

String SfxConfigFunctionListBox_Impl::GetHelpText( SvLBoxEntry *pEntry )
/*  Beschreibung
    Gibt den Helptext des selektierten Entry zur"uck.
*/
{
    // Information zum selektierten Entry aus den Userdaten holen
    SfxGroupInfo_Impl *pInfo = pEntry ? (SfxGroupInfo_Impl*) pEntry->GetUserData(): 0;
    if ( pInfo )
    {
        switch ( pInfo->nKind )
        {
            case SFX_CFGGROUP_FUNCTION :
            case SFX_CFGFUNCTION_SLOT :
            {
                // Eintrag ist eine Funktion, Hilfe aus der Office-Hilfe
                USHORT nId = pInfo->nOrd;
                String aText = Application::GetHelp()->GetHelpText( nId, this );

                if ( !aText.Len() )
                    aText = SFX_SLOTPOOL().GetSlotHelpText_Impl( nId );
                return aText;
            }
            break;

            case SFX_CFGGROUP_SCRIPTCONTAINER :
            case SFX_CFGFUNCTION_SCRIPT :
            case SFX_CFGGROUP_BASICMGR :
            case SFX_CFGGROUP_DOCBASICMGR :
            case SFX_CFGGROUP_BASICLIB :
            case SFX_CFGGROUP_BASICMOD :
            case SFX_CFGFUNCTION_MACRO :
            {
                // Eintrag ist ein Macro, Hilfe aus der MacroInfo
                SfxMacroInfo *pMacInfo = (SfxMacroInfo*) pInfo->pObject;
                return pMacInfo->GetHelpText();
            }
            break;

            case SFX_CFGGROUP_STYLES :
            {
                return String();
            }
            break;
        }
    }

    return String();
}

void SfxConfigFunctionListBox_Impl::FunctionSelected()
/*  Beschreibung
    Setzt die Balloonhelp zur"uck, da diese immer den Helptext des selektierten
    Entry anzeigen soll.
*/
{
    Help::ShowBalloon( this, Point(), String() );
}

void SfxConfigFunctionListBox_Impl::SetStylesInfo(SfxStylesInfo_Impl* pStyles)
{
    pStylesInfo = pStyles;
}

SfxConfigGroupListBox_Impl::SfxConfigGroupListBox_Impl(
    Window* pParent, const ResId& rResId, ULONG nConfigMode )
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
/*  Beschreibung
    Diese Listbox zeigt alle Funktionsgruppen und Basics an, die zur Konfiguration
    zur Verf"ugung stehen. Basics werden noch in Bibliotheken und Module untergliedert.
*/
{
    aScriptType = String(SfxResId(STR_BASICNAME));
    SetWindowBits( GetStyle() | WB_CLIPCHILDREN | WB_HSCROLL | WB_HASBUTTONS | WB_HASLINES | WB_HASLINESATROOT | WB_HASBUTTONSATROOT );
    SetNodeBitmaps( Image( BMP_COLLAPSED ), Image( BMP_EXPANDED ) , BMP_COLOR_NORMAL );
    SetNodeBitmaps( Image( BMP_COLLAPSED_HC ), Image( BMP_EXPANDED_HC ), BMP_COLOR_HIGHCONTRAST );

    // Check configuration to see whether only Basic macros,
  // only Scripting Framework scripts, or both should be listed
    Any value;
    sal_Bool tmp;

    value = ::utl::ConfigManager::GetConfigManager()->GetLocalProperty(
        ::rtl::OUString::createFromAscii(
            "Office.Scripting/ScriptDisplaySettings/ShowBasic" ) );

    value >>= tmp;

    if (tmp == sal_True) {
        bShowBasic = TRUE;
    }
    else {
        bShowBasic = FALSE;
    }

    value = ::utl::ConfigManager::GetConfigManager()->GetLocalProperty(
        ::rtl::OUString::createFromAscii(
            "Office.Scripting/ScriptDisplaySettings/ShowSF" ) );

    value >>= tmp;

    if (tmp == sal_True) {
        bShowSF = TRUE;
    }
    else {
        bShowSF = FALSE;
    }
}


SfxConfigGroupListBox_Impl::~SfxConfigGroupListBox_Impl()
{
    ClearAll();
}

void SfxConfigGroupListBox_Impl::ClearAll()
{
    USHORT nCount = aArr.Count();
    for ( USHORT i=0; i<nCount; i++ )
    {
        SfxGroupInfo_Impl *pData = aArr[i];
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

void SfxConfigGroupListBox_Impl::SetScriptType( const String& rScriptType )
{
    aScriptType = rScriptType;
    ULONG nPos=0;
    SvLBoxEntry *pEntry = (SvLBoxEntry*) GetModel()->GetEntryAtAbsPos( nPos++ );
    while ( pEntry )
    {
        SfxGroupInfo_Impl *pInfo = (SfxGroupInfo_Impl*) pEntry->GetUserData();
        if ( pInfo->nKind == SFX_CFGGROUP_BASICLIB && ( IsExpanded( pEntry ) || pInfo->bWasOpened ) )
        {
            Collapse( pEntry );
            SvLBoxEntry *pChild = FirstChild( pEntry );
            while (pChild)
            {
                GetModel()->Remove( pChild );
                pChild = FirstChild( pEntry );
            }

            Expand( pEntry );
        }

        pEntry = (SvLBoxEntry*) GetModel()->GetEntryAtAbsPos( nPos++ );
    }
}

void SfxConfigGroupListBox_Impl::SetStylesInfo(SfxStylesInfo_Impl* pStyles)
{
    pStylesInfo = pStyles;
}

String SfxConfigGroupListBox_Impl::GetGroup()
/*  Beschreibung
    Gibt den Namen der selektierten Funktionsgruppe bzw. des selektierten
    Basics zur"uck.
*/
{
    SvLBoxEntry *pEntry = FirstSelected();
    while ( pEntry )
    {
        SfxGroupInfo_Impl *pInfo = (SfxGroupInfo_Impl*) pEntry->GetUserData();
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

void SfxConfigGroupListBox_Impl::Init( SvStringsDtor *pArr, SfxSlotPool* pPool )
/*  Beschreibung
    Die Groupbox wird mit allen Funktionsgruppen und allen Basics gef"ullt
*/
{
    SetUpdateMode(FALSE);

    ClearAll(); // Remove all old entries from treelist box
    SfxApplication *pSfxApp = SFX_APP();

    // Verwendet wird der aktuelle Slotpool
    if ( nMode )
    {
        pSlotPool = pPool ? pPool : &SFX_SLOTPOOL();
        for ( USHORT i=1; i<pSlotPool->GetGroupCount(); i++ )
        {
            // Gruppe anw"ahlen ( Gruppe 0 ist intern )
            String aName = pSlotPool->SeekGroup( i );
            const SfxSlot *pSfxSlot = pSlotPool->FirstSlot();
            if ( pSfxSlot )
            {
                // Check if all entries are not useable. Don't
                // insert a group without any useable function.
                sal_Bool bActiveEntries = sal_False;
                while ( pSfxSlot )
                {
                    USHORT nId = pSfxSlot->GetSlotId();
                    if ( pSfxSlot->GetMode() & nMode )
                    {
                        bActiveEntries = sal_True;
                        break;
                    }

                    pSfxSlot = pSlotPool->NextSlot();
                }

                if ( bActiveEntries )
                {
                    // Wenn Gruppe nicht leer
                    SvLBoxEntry *pEntry = InsertEntry( aName, NULL );
                    SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_FUNCTION, i );
                    aArr.Insert( pInfo, aArr.Count() );
                    pEntry->SetUserData( pInfo );
                }
            }
        }
    }

    if ( bShowBasic )
    {
        // Basics einsammeln
        pSfxApp->EnterBasicCall();
        String aMacroName(' ');
        aMacroName += String(SfxResId(STR_BASICMACROS));

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
                String aAppBasTitle( SfxResId( STR_HUMAN_APPNAME ) );
                aAppBasTitle += aMacroName;
                SvLBoxEntry *pEntry = InsertEntry( aAppBasTitle, 0 );
                SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_BASICMGR, 0, pAppBasicMgr );
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
                    SfxGroupInfo_Impl *pInfo =
                        new SfxGroupInfo_Impl( SFX_CFGGROUP_DOCBASICMGR, 0, pDoc );
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

    SfxObjectShell *tmp = SfxObjectShell::GetWorkingDocument();
    OSL_TRACE("** ** About to initialise SF Scripts");
    if ( bShowSF )
    {
        OSL_TRACE("** ** bShowSF");
        // Add Scripting Framework entries
        Reference< browse::XBrowseNode > rootNode;
        Reference< XComponentContext > xCtx;
        try
        {
            Reference < beans::XPropertySet > xProps(
                ::comphelper::getProcessServiceFactory(), UNO_QUERY_THROW );
            xCtx.set( xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))), UNO_QUERY_THROW );
            Reference< browse::XBrowseNodeFactory > xFac( xCtx->getValueByName(
                ::rtl::OUString::createFromAscii( "/singletons/com.sun.star.script.browse.theBrowseNodeFactory") ), UNO_QUERY_THROW );
            rootNode.set( xFac->createView( browse::BrowseNodeFactoryViewTypes::MACROSELECTOR ) );
            //rootNode.set( xFac->createView( browse::BrowseNodeFactoryViewTypes::MACROORGANIZER ) );
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

                SfxGroupInfo_Impl *pInfo =
                    new SfxGroupInfo_Impl( SFX_CFGGROUP_SCRIPTCONTAINER, 0,
                        static_cast<void *>(rootNode.get()));

                String aTitle( SfxResId( STR_MACROS ) );
                SvLBoxEntry *pNewEntry = InsertEntry( aTitle, NULL );
                pNewEntry->SetUserData( pInfo );
                pNewEntry->EnableChildsOnDemand( TRUE );
                aArr.Insert( pInfo, aArr.Count() );
            }
            else
            {
                /*
                 * We are only showing scripts not slot APIs so skip
                 * Root node and show location nodes
                 */
                try {
                    if ( rootNode->hasChildNodes() )
                    {
                        Sequence< Reference< browse::XBrowseNode > > children =
                            rootNode->getChildNodes();
                        BOOL bIsRootNode = FALSE;

                        ::rtl::OUString user = ::rtl::OUString::createFromAscii("user");
                        ::rtl::OUString share = ::rtl::OUString::createFromAscii("share");
                        if ( rootNode->getName().equals(::rtl::OUString::createFromAscii("Root") ))
                        {
                            bIsRootNode = TRUE;
                        }

                        for ( ULONG n = 0; n < children.getLength(); n++ )
                        {
                            Reference< browse::XBrowseNode >& theChild = children[n];
                            BOOL bDisplay = TRUE;
                            /* To mimic current starbasic behaviour we
                            need to make sure that only the current document
                            is displayed in the config tree. Tests below
                            set the bDisplay flag to FALSE if the current
                            node is a first level child of the Root and is NOT
                            either the current document, user or share */
                            ::rtl::OUString currentDocTitle;
                               if ( SfxObjectShell::GetWorkingDocument() )
                            {
                                currentDocTitle = SfxObjectShell::GetWorkingDocument()->GetTitle();
                            }
                            ::rtl::OUString uiName = theChild->getName();

                            if ( bIsRootNode )
                            {
                                if (  ! ((theChild->getName().equals( user )  ||                                    theChild->getName().equals( share ) ||
                                    theChild->getName().equals( currentDocTitle ) ) ) )
                                {
                                    bDisplay=FALSE;
                                }
                                else
                                {
                                    if ( uiName.equals( user ) )
                                    {
                                        uiName = m_sMyMacros;
                                    }
                                    else if ( uiName.equals( share ) )
                                    {
                                        uiName = m_sProdMacros;
                                    }
                                }
                            }
                            if (children[n]->getType() != browse::BrowseNodeTypes::SCRIPT  && bDisplay )
                            {

                                /*
                                    We call acquire on the XBrowseNode so that it does not
                                    get autodestructed and become invalid when accessed later.
                                */
                                theChild->acquire();

                                SfxGroupInfo_Impl* pInfo =
                                    new SfxGroupInfo_Impl(SFX_CFGGROUP_SCRIPTCONTAINER,
                                        0, static_cast<void *>( theChild.get()));

                                Image aImage = GetImage( theChild, xCtx, bIsRootNode,BMP_COLOR_NORMAL );
                                SvLBoxEntry* pNewEntry =
                                    InsertEntry( uiName, NULL);
                                SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                aImage = GetImage( theChild, xCtx, bIsRootNode,BMP_COLOR_HIGHCONTRAST );
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

    // add styles
    {
        String sStyle( SfxResId( STR_GROUP_STYLES ) );
        SvLBoxEntry *pEntry = InsertEntry( sStyle, 0 );
        SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_STYLES, 0, 0 ); // TODO last parameter should contain user data
        aArr.Insert( pInfo, aArr.Count() );
        pEntry->SetUserData( pInfo );
        pEntry->EnableChildsOnDemand( TRUE );
    }

    MakeVisible( GetEntry( 0,0 ) );
    SetUpdateMode( TRUE );
}
Image SfxConfigGroupListBox_Impl::GetImage( Reference< browse::XBrowseNode > node, Reference< XComponentContext > xCtx, bool bIsRootNode, bool bHighContrast )
{
    Image aImage;
    if ( bIsRootNode )
    {
        ::rtl::OUString user = ::rtl::OUString::createFromAscii("user");
        ::rtl::OUString share = ::rtl::OUString::createFromAscii("share");
        if (node->getName().equals( user ) || node->getName().equals(share ) )
        {
            if( bHighContrast == BMP_COLOR_NORMAL )
                aImage = m_hdImage;
            else
                aImage = m_hdImage_hc;
        }
        else
        {
            ::rtl::OUString factoryURL;
            ::rtl::OUString nodeName = node->getName();
            Reference<XInterface> xDocumentModel = getDocumentModel(xCtx, nodeName );
            if ( xDocumentModel.is() )
            {
                Reference< ::drafts::com::sun::star::frame::XModuleManager >
                    xModuleManager(
                        xCtx->getServiceManager()
                            ->createInstanceWithContext(
                                ::rtl::OUString::createFromAscii("drafts." // xxx todo
                                      "com.sun.star.frame.ModuleManager"),
                                xCtx ),
                            UNO_QUERY_THROW );
                Reference<container::XNameAccess> xModuleConfig(
                    xModuleManager, UNO_QUERY_THROW );
                // get the long name of the document:
                ::rtl::OUString appModule( xModuleManager->identify(
                                    xDocumentModel ) );
                Sequence<beans::PropertyValue> moduleDescr;
                Any aAny = xModuleConfig->getByName(appModule);
                if( sal_True != ( aAny >>= moduleDescr ) )
                {
                    throw RuntimeException(::rtl::OUString::createFromAscii("SFTreeListBox::Init: failed to get PropertyValue"), Reference< XInterface >());
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
SfxConfigGroupListBox_Impl::getDocumentModel( Reference< XComponentContext >& xCtx, ::rtl::OUString& docName )
{
    Reference< XInterface > xModel;
    Reference< lang::XMultiComponentFactory > mcf =
            xCtx->getServiceManager();
    Reference< frame::XDesktop > desktop (
        mcf->createInstanceWithContext(
            ::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop"),                 xCtx ),
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
            ::rtl::OUString sTdocUrl = xModelToDocTitle( model );
            if( sTdocUrl.equals( docName ) )
            {
                xModel = model;
                break;
            }
        }
    }
    return xModel;
}

::rtl::OUString SfxConfigGroupListBox_Impl::xModelToDocTitle( const Reference< frame::XModel >& xModel )
{
    // Set a default name, this should never be seen.
    ::rtl::OUString docNameOrURL =
        ::rtl::OUString::createFromAscii("Unknown");
    if ( xModel.is() )
    {
        ::rtl::OUString tempName;
        try
        {
            Reference< beans::XPropertySet > propSet( xModel->getCurrentController()->getFrame(), UNO_QUERY );
            if ( propSet.is() )
            {
                if ( sal_True == ( propSet->getPropertyValue(::rtl::OUString::createFromAscii( "Title" ) ) >>= tempName ) )
                {
                    docNameOrURL = tempName;
                    if ( xModel->getURL().getLength() == 0 )
                    {
                        // process "UntitledX - YYYYYYYY"
                        // to get UntitledX
                        sal_Int32 pos = 0;
                        docNameOrURL = tempName.getToken(0,' ',pos);
                        OSL_TRACE("xModelToDocTitle() Title for document is %s.",
                            ::rtl::OUStringToOString( docNameOrURL,
                                            RTL_TEXTENCODING_ASCII_US ).pData->buffer );
                    }
                    else
                    {
                        Reference< document::XDocumentInfoSupplier >  xDIS( xModel, UNO_QUERY_THROW );
                        Reference< beans::XPropertySet > xProp (xDIS->getDocumentInfo(),  UNO_QUERY_THROW );
                        Any aTitle = xProp->getPropertyValue(::rtl::OUString::createFromAscii( "Title" ) );

                        aTitle >>= docNameOrURL;
                        if ( docNameOrURL.getLength() == 0 )
                        {
                            docNameOrURL =  parseLocationName( xModel->getURL() );
                        }
                    }
                }
            }
        }
        catch ( Exception& e )
        {
            OSL_TRACE("MiscUtils::xModelToDocTitle() exception thrown: !!! %s",
                ::rtl::OUStringToOString( e.Message,
                    RTL_TEXTENCODING_ASCII_US ).pData->buffer );
        }

    }
    else
    {
        OSL_TRACE("MiscUtils::xModelToDocTitle() doc model is null" );
    }
    return docNameOrURL;
}

::rtl::OUString SfxConfigGroupListBox_Impl::parseLocationName( const ::rtl::OUString& location )
{
    // strip out the last leaf of location name
    // e.g. file://dir1/dir2/Blah.sxw - > Blah.sxw
    ::rtl::OUString temp = location;
    sal_Int32 lastSlashIndex = temp.lastIndexOf( ::rtl::OUString::createFromAscii( "/" ) );

    if ( ( lastSlashIndex + 1 ) <  temp.getLength()  )
    {
        temp = temp.copy( lastSlashIndex + 1 );
    }
    // maybe we should throw here!!!
    else
    {
        OSL_TRACE("Something wrong with name, perhaps we should throw an exception");
    }
    return temp;
}

void SfxConfigGroupListBox_Impl::GroupSelected()
/*  Beschreibung
    Eine Funktionsgruppe oder eine Basicmodul wurde selektiert. Alle Funktionen bzw.
    Macros werden in der Functionlistbox anzeigt.
*/
{
    SvLBoxEntry *pEntry = FirstSelected();
    SfxGroupInfo_Impl *pInfo = (SfxGroupInfo_Impl*) pEntry->GetUserData();
    pFunctionListBox->SetUpdateMode(FALSE);
    pFunctionListBox->ClearAll();
    if ( pInfo->nKind != SFX_CFGGROUP_FUNCTION &&
             pInfo->nKind != SFX_CFGGROUP_BASICMOD &&
             pInfo->nKind != SFX_CFGGROUP_SCRIPTCONTAINER &&
             pInfo->nKind != SFX_CFGGROUP_STYLES )
    {
        pFunctionListBox->SetUpdateMode(TRUE);
        return;
    }

    switch ( pInfo->nKind )
    {
        case SFX_CFGGROUP_FUNCTION :
        {
            USHORT nGroup = pInfo->nOrd;
            String aSelectedGroup = pSlotPool->SeekGroup( nGroup );
            if ( aSelectedGroup != String() )
            {
                const SfxSlot *pSfxSlot = pSlotPool->FirstSlot();
                while ( pSfxSlot )
                {
                    USHORT nId = pSfxSlot->GetSlotId();
                    if ( pSfxSlot->GetMode() & nMode )
                    {
                        String aName = pSlotPool->GetSlotName_Impl( *pSfxSlot );
                        if ( aName.Len() && !pFunctionListBox->GetEntry_Impl( nId ) )
                        {
#ifdef DBG_UTIL
                            if ( pFunctionListBox->GetEntry_Impl( aName ) )
                                DBG_WARNINGFILE( "function name already exits" );
#endif
                            // Wenn die Namen unterschiedlich sind, dann auch die Funktion, denn zu
                            // einer Id liefert der Slotpool immer den gleichen Namen!
                            SvLBoxEntry* pFuncEntry = pFunctionListBox->InsertEntry( aName, NULL );
                            SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGFUNCTION_SLOT, nId );
                            pFunctionListBox->aArr.Insert( pInfo, pFunctionListBox->aArr.Count() );
                            pFuncEntry->SetUserData( pInfo );
                        }
                    }

                    pSfxSlot = pSlotPool->NextSlot();
                }
            }

            break;
        }

        case SFX_CFGGROUP_BASICMOD :
        {
            SvLBoxEntry *pLibEntry = GetParent( pEntry );
            SfxGroupInfo_Impl *pLibInfo =
                (SfxGroupInfo_Impl*) pLibEntry->GetUserData();
            SvLBoxEntry *pBasEntry = GetParent( pLibEntry );
            SfxGroupInfo_Impl *pBasInfo =
                (SfxGroupInfo_Impl*) pBasEntry->GetUserData();

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
                SfxGroupInfo_Impl *pInfo =
                    new SfxGroupInfo_Impl( SFX_CFGFUNCTION_MACRO, nId, pInf );
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
                                ::rtl::OUString uri;

                                Reference < beans::XPropertySet >xPropSet( children[n], UNO_QUERY );
                                if (!xPropSet.is())
                                {
                                    continue;
                                }

                                Any value =
                                    xPropSet->getPropertyValue( String::CreateFromAscii( "URI" ) );
                                value >>= uri;

                                SfxMacroInfo* aInfo = new SfxMacroInfo( uri );
                                aInfo->SetHelpText( uri );
                                SFX_APP()->GetMacroConfig()->GetSlotId( aInfo );

                                SfxGroupInfo_Impl* pInfo =
                                    new SfxGroupInfo_Impl(SFX_CFGFUNCTION_SCRIPT,
                                        aInfo->GetSlotId(), aInfo);

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

        case SFX_CFGGROUP_STYLES :
        {
            SfxStyleInfo_Impl* pFamily = (SfxStyleInfo_Impl*)(pInfo->pObject);
            if (pFamily)
            {
                const ::std::vector< SfxStyleInfo_Impl > lStyles = pStylesInfo->getStyles(pFamily->sFamily);
                ::std::vector< SfxStyleInfo_Impl >::const_iterator pIt;
                for (  pIt  = lStyles.begin();
                       pIt != lStyles.end()  ;
                     ++pIt                   )
                {
                    SfxStyleInfo_Impl* pStyle = new SfxStyleInfo_Impl(*pIt);
                    SvLBoxEntry* pFuncEntry = pFunctionListBox->InsertEntry( pStyle->sLabel, NULL );
                    SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_STYLES, pStyle->nId, pStyle );
                    pFunctionListBox->aArr.Insert( pInfo, pFunctionListBox->aArr.Count() );
                    pFuncEntry->SetUserData( pInfo );
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

BOOL SfxConfigGroupListBox_Impl::Expand( SvLBoxEntry* pParent )
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

void SfxConfigGroupListBox_Impl::RequestingChilds( SvLBoxEntry *pEntry )
/*  Beschreibung
    Ein Basic oder eine Bibliothek werden ge"offnet
*/
{
    SfxGroupInfo_Impl *pInfo = (SfxGroupInfo_Impl*) pEntry->GetUserData();
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
                    SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_BASICLIB, nLib, pLib );
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
                    SfxGroupInfo_Impl *pInf =
                        (SfxGroupInfo_Impl*) pParent->GetUserData();
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

                    BOOL bIsStarScript = FALSE; //pMod->ISA( SbJScriptModule );
                    BOOL bWantsStarScript = aScriptType.EqualsAscii("StarScript");
                    if ( bIsStarScript != bWantsStarScript )
                        continue;
                    pModEntry = InsertEntry( pMod->GetName(), pEntry );
                    SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_BASICMOD, 0, pMod );
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

                        ::rtl::OUString user = ::rtl::OUString::createFromAscii("user");
                        ::rtl::OUString share = ::rtl::OUString::createFromAscii("share");
                        if ( rootNode->getName().equals(::rtl::OUString::createFromAscii("Root") ))
                        {
                            bIsRootNode = TRUE;
                        }

                        for ( ULONG n = 0; n < children.getLength(); n++ )
                        {
                            Reference< browse::XBrowseNode >& theChild = children[n];
                            BOOL bDisplay = TRUE;
                            /* To mimic current starbasic behaviour we
                            need to make sure that only the current document
                            is displayed in the config tree. Tests below
                            set the bDisplay flag to FALSE if the current
                            node is a first level child of the Root and is NOT
                            either the current document, user or share */
                            ::rtl::OUString currentDocTitle;
                               if ( SfxObjectShell::GetWorkingDocument() )
                            {
                                currentDocTitle = SfxObjectShell::GetWorkingDocument()->GetTitle();
                            }
                            if ( bIsRootNode )
                            {
                                if (  ! ((theChild->getName().equals( user )  ||                                    theChild->getName().equals( share ) ||
                                    theChild->getName().equals( currentDocTitle ) ) ) )
                                {
                                    bDisplay=FALSE;
                                }
                            }
                            if (children[n]->getType() != browse::BrowseNodeTypes::SCRIPT  && bDisplay )
                            {

                                /*
                                    We call acquire on the XBrowseNode so that it does not
                                    get autodestructed and become invalid when accessed later.
                                */
                                theChild->acquire();

                                SfxGroupInfo_Impl* pInfo =
                                    new SfxGroupInfo_Impl(SFX_CFGGROUP_SCRIPTCONTAINER,
                                        0, static_cast<void *>( theChild.get()));

                                Image aImage = GetImage( theChild, Reference< XComponentContext >(), sal_False, BMP_COLOR_NORMAL );
                                SvLBoxEntry* pNewEntry =
                                    InsertEntry( theChild->getName(), pEntry );
                                SetExpandedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                SetCollapsedEntryBmp(pNewEntry, aImage, BMP_COLOR_NORMAL);
                                aImage = GetImage( theChild, Reference< XComponentContext >(), sal_False, BMP_COLOR_HIGHCONTRAST );
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
            break;
        }

        case SFX_CFGGROUP_STYLES:
        {
            if ( !GetChildCount( pEntry ) )
            {
                const ::std::vector< SfxStyleInfo_Impl >                 lStyleFamilies = pStylesInfo->getStyleFamilies();
                      ::std::vector< SfxStyleInfo_Impl >::const_iterator pIt;
                for (  pIt  = lStyleFamilies.begin();
                       pIt != lStyleFamilies.end()  ;
                     ++pIt                          )
                {
                    SfxStyleInfo_Impl* pFamily = new SfxStyleInfo_Impl(*pIt);
                    SvLBoxEntry* pStyleEntry = InsertEntry( pFamily->sLabel, pEntry );
                    SfxGroupInfo_Impl *pInfo = new SfxGroupInfo_Impl( SFX_CFGGROUP_STYLES, pFamily->nId, pFamily );
                    aArr.Insert( pInfo, aArr.Count() );
                    pStyleEntry->SetUserData( pInfo );
                    pStyleEntry->EnableChildsOnDemand( FALSE );
                }
            }
            break;
        }

        default:
            DBG_ERROR( "Falscher Gruppentyp!" );
            break;
    }
}

void SfxConfigGroupListBox_Impl::SelectMacro( const SfxMacroInfoItem *pItem )
{
    SelectMacro( pItem->GetBasicManager()->GetName(),
                 pItem->GetQualifiedName() );
}

void SfxConfigGroupListBox_Impl::SelectMacro( const String& rBasic,
         const String& rMacro )
{
    String aBasicName( rBasic );
    aBasicName += ' ';
    aBasicName += String(SfxResId(STR_BASICMACROS));
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

void SfxConfigDialog::ActivateToolBoxConfig( USHORT nId )
{
    SetCurPageId( TP_CONFIG_OBJECTBAR );
    nObjectBar = nId;
}

void SfxConfigDialog::PageCreated( USHORT nId, SfxTabPage& rPage )
{
    switch ( nId )
    {
        case TP_CONFIG_OBJECTBAR :
            if ( nObjectBar )
                ((SfxObjectBarConfigPage&)rPage).SetObjectBarId( nObjectBar );
            break;
        case TP_CONFIG_ACCEL:
            if ( pMacroInfo )
                ((SfxAcceleratorConfigPage&)rPage).SelectMacro( pMacroInfo );
            break;
        case TP_CONFIG_MENU:
            if ( pMacroInfo )
                ((SfxMenuConfigPage&)rPage).SelectMacro( pMacroInfo );
            break;
        case TP_CONFIG_EVENT:
            if ( pMacroInfo )
                ((SfxEventConfigPage&)rPage).SelectMacro( pMacroInfo );
            break;
        default:
            break;
    }
}

void SfxConfigDialog::ActivateMacroConfig(const SfxMacroInfoItem *pInfo)
{
    pMacroInfo = pInfo;
}

void SfxConfigDialog::ActivateTabPage( USHORT nSlotId )
{
    switch ( nSlotId )
    {
        case SID_CONFIG:
            break;
        case SID_TOOLBOXOPTIONS:
            SetCurPageId( TP_CONFIG_OBJECTBAR );
            break;
//      case SID_CONFIGSTATUSBAR:
//            SetCurPageId( TP_CONFIG_STATBAR );
//            break;
        case SID_CONFIGMENU:
            SetCurPageId( TP_CONFIG_MENU );
            break;
        case SID_CONFIGACCEL:
            SetCurPageId( TP_CONFIG_ACCEL );
            break;
        case SID_CONFIGEVENT:
            SetCurPageId( TP_CONFIG_EVENT );
            break;
    }
}

SfxTabPage *CreateMenuConfigPage( Window *pParent, const SfxItemSet& rSet )
{
    return new SfxMenuConfigPage( pParent, rSet );
}

SfxTabPage *CreateAccelConfigPage( Window *pParent, const SfxItemSet& rSet )
{
    return new SfxAcceleratorConfigPage( pParent, rSet );
}
/*
SfxTabPage *CreateStatusBarConfigPage( Window *pParent, const SfxItemSet& rSet )
{
    return new SfxStatusBarConfigPage( pParent, rSet );
}
*/
SfxTabPage *CreateObjectBarConfigPage( Window *pParent, const SfxItemSet& rSet )
{
    return new SfxObjectBarConfigPage( pParent, rSet );
}

SfxTabPage *CreateEventConfigPage( Window *pParent, const SfxItemSet& rSet )
{
    return new SfxEventConfigPage( pParent, rSet );
}

SfxConfigDialog::SfxConfigDialog( Window * pParent, const SfxItemSet* pSet, SfxViewFrame* pFrame )
    : SfxTabDialog( pFrame, pParent, SfxResId( DLG_CONFIG ), pSet )
    , pMacroInfo( 0 )
    , pViewFrame( pFrame )
    , nObjectBar( 0 )
{
    FreeResource();

    AddTabPage( TP_CONFIG_MENU, CreateMenuConfigPage, NULL );
    AddTabPage( TP_CONFIG_ACCEL, CreateAccelConfigPage, NULL );
//  AddTabPage( TP_CONFIG_STATBAR, CreateStatusBarConfigPage, NULL );
    AddTabPage( TP_CONFIG_OBJECTBAR, CreateObjectBarConfigPage, NULL );
    AddTabPage( TP_CONFIG_EVENT, CreateEventConfigPage, NULL );
}

SfxConfigDialog::~SfxConfigDialog()
{
}

short SfxConfigDialog::Ok()
{
    USHORT nRet = SfxTabDialog::Ok();

    // configurations in documents are stored with the documents
    SFX_APP()->GetConfigManager_Impl()->StoreConfiguration();
    return nRet;
}

// SfxStatusBarConfigPage::Ctor() **********************************************
/*
SfxStatusBarConfigPage::SfxStatusBarConfigPage( Window *pParent, const SfxItemSet& rSet ) :

    SfxTabPage( pParent, SfxResId( TP_CONFIG_STATBAR ), rSet ),

    aEntriesBox         ( this, ResId( BOX_STATBAR_ENTRIES         ) ),
    aStatusBarGroup     ( this, ResId( GRP_STATBAR                 ) ),
    aLoadButton         ( this, ResId( BTN_LOAD ) ),
    aSaveButton         ( this, ResId( BTN_SAVE ) ),
    aResetButton        ( this, ResId( BTN_RESET   ) ),
    pMgr( 0 ),
    bMgrCreated( FALSE )
{
    FreeResource();

    aLoadButton  .SetClickHdl ( LINK( this, SfxStatusBarConfigPage, Load      ) );
    aSaveButton  .SetClickHdl ( LINK( this, SfxStatusBarConfigPage, Save      ) );
    aResetButton .SetClickHdl ( LINK( this, SfxStatusBarConfigPage, Default      ) );
    aEntriesBox  .SetSelectHdl( LINK(this,SfxStatusBarConfigPage, SelectHdl));
    USHORT nGroup = 0;
    String aGroupName = SFX_SLOTPOOL().SeekGroup(nGroup);

    while (aGroupName.Len() != 0)
    {
        for (const SfxSlot *pSlot = SFX_SLOTPOOL().FirstSlot(); pSlot;
                            pSlot = SFX_SLOTPOOL().NextSlot())
        {
            if ( !pSlot->GetType() ||
                 pSlot->GetType()->Type() == TYPE(SfxVoidItem) ||
                 !(pSlot->GetMode() & SFX_SLOT_STATUSBARCONFIG) )
                continue;

            USHORT nId = pSlot->GetSlotId();
            USHORT i;
            for (i=0; i<aListOfIds.Count(); i++)
                if (aListOfIds[i] >= nId) break;

            if (i<aListOfIds.Count() && aListOfIds[i] == nId)
                continue;
            else
                aListOfIds.Insert(nId, i);
        }

        aGroupName = SFX_SLOTPOOL().SeekGroup(++nGroup);
    }
}

void SfxStatusBarConfigPage::Init()
{
    SvLBoxEntry *pEntry;
    USHORT nUsed=0;
    SvUShorts aListOfPos;
    pArr = new SfxStbInfoArr_Impl;

    for (USHORT i=0; i<aListOfIds.Count(); i++)
    {
        USHORT nId = aListOfIds[i];
        String aName = SFX_SLOTPOOL().GetSlotName_Impl( nId );
        if ( !aName.Len() )
            continue;

        SfxStatBarInfo_Impl* pInfo = new SfxStatBarInfo_Impl (nId, String());
        pArr->Append(pInfo);
        USHORT nPos = pMgr->GetItemPos(nId);
        if (nPos != STATUSBAR_ITEM_NOTFOUND)
        {
            USHORT np;
            for (np=0; np<aListOfPos.Count(); np++)
                if (aListOfPos[np] > nPos) break;
            aListOfPos.Insert(nPos,np);
            pEntry = aEntriesBox.InsertEntry(aName, (SvLBoxEntry*)0L, FALSE, (ULONG) np);
            aEntriesBox.SetCheckButtonState(pEntry, SV_BUTTON_CHECKED);
            nUsed++;
        }
        else
        {
            pEntry = aEntriesBox.InsertEntry(aName, (SvLBoxEntry*)0L);
        }

        pEntry->SetUserData(pInfo);
    }
}

void SfxStatusBarConfigPage::ResetConfig()
{
    aEntriesBox.Clear();
    for (USHORT n=0; n<pArr->Count(); n++)
        delete (*pArr)[n];
    delete pArr;
}

SfxStatusBarConfigPage::~SfxStatusBarConfigPage()
{
    if ( bMgrCreated )
        delete pMgr;
    ResetConfig();
}

IMPL_LINK( SfxStatusBarConfigPage, SelectHdl, SvTreeListBox *, pBox )
{
    return 0;
}


void SfxStatusBarConfigPage::Apply( SfxStatusBarManager* pStbMgr, BOOL bIsDefault )
{
    if ( !pStbMgr )
        return;

    if ( bIsDefault )
    {
        pStbMgr->UseDefault();
        pStbMgr->SetDefault(TRUE);
        return;
    }

    pStbMgr->Clear();
    long nWidth = 100;

    for ( SvLBoxEntry *pEntry = aEntriesBox.First(); pEntry; pEntry = aEntriesBox.Next(pEntry) )
    {
        if (aEntriesBox.GetCheckButtonState(pEntry) == SV_BUTTON_CHECKED)
        {
            USHORT nId =
                ((SfxStatBarInfo_Impl*) pEntry->GetUserData())->nId;
            pStbMgr->AddItem(nId, nWidth);
        }
    }

    pStbMgr->SetDefault( FALSE );
}

IMPL_LINK( SfxStatusBarConfigPage, Default, PushButton *, pPushButton )
{
    // creating a ConfigItem without ConfigManager forces it to use its default
    SfxStatusBarManager* pStbMgr = new SfxStatusBarManager( this, *pMgr, NULL );
    SfxStatusBarManager* pOld = pMgr;
    pMgr = pStbMgr;

    aEntriesBox.bDefault = TRUE;
    aEntriesBox.bModified = !pOld->IsDefault();
    aEntriesBox.SetUpdateMode(FALSE);
    ResetConfig();
    Init();
    aEntriesBox.SetUpdateMode(TRUE);
    aEntriesBox.Invalidate();

    pMgr = pOld;
    StatusBar* pBar = pStbMgr->GetStatusBar();
    delete pStbMgr;
    delete pBar;
    return 0;
}

IMPL_LINK( SfxStatusBarConfigPage, Load, Button *, pButton )
{
    String aCfgName = SfxConfigDialog::FileDialog_Impl( this,
        WB_OPEN | WB_STDMODAL | WB_3DLOOK, String( SfxResId( STR_LOADSTATBARCONFIG) ) );
    if ( aCfgName.Len() )
    {
        GetTabDialog()->EnterWait();
        BOOL bCreated = FALSE;
        BOOL bLoadedDocument = FALSE;
        SfxObjectShellRef xDoc;

//REMOVE            SfxConfigManager* pCfgMgr = SFX_APP()->GetConfigManager_Impl();
//REMOVE            if ( pCfgMgr->GetURL() != aCfgName )
//REMOVE            {
//REMOVE                // it was not the global configuration manager
//REMOVE                // first check if URL points to a document already loaded
//REMOVE                xDoc = SFX_APP()->DocAlreadyLoaded( aCfgName, TRUE, TRUE );
//REMOVE                if ( xDoc.Is() )
//REMOVE                    bLoadedDocument = TRUE;
//REMOVE                else
//REMOVE                    // try to load a document from the URL
//REMOVE                    xDoc = MakeObjectShellForOrganizer_Impl( aCfgName, TRUE );
//REMOVE                if ( xDoc.Is() )
//REMOVE                {
//REMOVE                    // get config manager, force creation if there was none before
//REMOVE                    pCfgMgr = xDoc->GetConfigManager( TRUE );
//REMOVE                }
//REMOVE                else
//REMOVE                {
//REMOVE                    // URL doesn't point to a document, must be a single storage
//REMOVE                    bCreated = TRUE;
//REMOVE                    SvStorageRef xStor = new SvStorage( aCfgName, STREAM_STD_READ );
//REMOVE                    if ( xStor->GetError() == ERRCODE_NONE )
//REMOVE                        pCfgMgr = new SfxConfigManager( xStor );
//REMOVE                    else
//REMOVE                        pCfgMgr = NULL;
//REMOVE                }
//REMOVE            }
//REMOVE
//REMOVE            if ( pCfgMgr )
//REMOVE            {
//REMOVE                // create new StatusBarManager and read configuration
//REMOVE                // constructing with a SfxConfigManager reads in configuration
//REMOVE                SfxStatusBarManager* pStbMgr = new SfxStatusBarManager( this, *pMgr, pCfgMgr );
//REMOVE
//REMOVE                // put new configuration into TabPage
//REMOVE                SfxStatusBarManager* pOld = pMgr;
//REMOVE                pMgr = pStbMgr;
//REMOVE                aEntriesBox.SetUpdateMode( FALSE );
//REMOVE                ResetConfig();
//REMOVE                Init();
//REMOVE                aEntriesBox.SetUpdateMode( TRUE );
//REMOVE                aEntriesBox.Invalidate();
//REMOVE                aEntriesBox.bDefault = FALSE;
//REMOVE                aEntriesBox.bModified = TRUE;
//REMOVE                pMgr = pOld;
//REMOVE
//REMOVE                StatusBar* pBar = pStbMgr->GetStatusBar();
//REMOVE                delete pStbMgr;
//REMOVE                delete pBar;
//REMOVE
//REMOVE                if ( bCreated )
//REMOVE                    delete pCfgMgr;
//REMOVE                else
//REMOVE                    pCfgMgr->ReInitialize( pMgr->GetType() );
//REMOVE
//REMOVE                if ( bLoadedDocument && !xDoc->GetConfigManager()->HasConfigItem( pMgr->GetType() ) )
//REMOVE                    // config item has global configuration until now, must be changed
//REMOVE                    pMgr->GetConfigManager()->ReConnect( pMgr->GetType(), pCfgMgr );
//REMOVE            }

        GetTabDialog()->LeaveWait();
    }

    return TRUE;
}

IMPL_LINK( SfxStatusBarConfigPage, Save, Button *, pButton )
{
    String aCfgName = SfxConfigDialog::FileDialog_Impl( this,
        WB_SAVEAS | WB_STDMODAL | WB_3DLOOK, String( SfxResId( STR_SAVESTATBARCONFIG) ) );
    if ( aCfgName.Len() )
    {
        GetTabDialog()->EnterWait();
        BOOL bCreated = FALSE;
        SfxObjectShellRef xDoc;

        BOOL bLoadedDocument = FALSE;

//REMOVE            SfxConfigManager* pCfgMgr = SFX_APP()->GetConfigManager_Impl();
//REMOVE            if ( pCfgMgr->GetURL() != aCfgName )
//REMOVE            {
//REMOVE                // it was not the global configuration manager
//REMOVE                // first check if URL points to a document already loaded
//REMOVE                xDoc = SFX_APP()->DocAlreadyLoaded( aCfgName, TRUE, TRUE );
//REMOVE                if ( xDoc.Is() )
//REMOVE                    bLoadedDocument = TRUE;
//REMOVE                else
//REMOVE                    // try to load a document from the URL
//REMOVE                    xDoc = MakeObjectShellForOrganizer_Impl( aCfgName, TRUE );
//REMOVE                if ( xDoc.Is() )
//REMOVE                {
//REMOVE                    // get config manager, force creation if there was none before
//REMOVE                    pCfgMgr = xDoc->GetConfigManager( TRUE );
//REMOVE                }
//REMOVE                else
//REMOVE                {
//REMOVE                    // URL doesn't point to a document, must be a single storage
//REMOVE                    bCreated = TRUE;
//REMOVE                    SvStorageRef xStor = new SvStorage( aCfgName, STREAM_STD_WRITE, STORAGE_TRANSACTED );
//REMOVE                    if ( xStor->GetError() == ERRCODE_NONE )
//REMOVE                        pCfgMgr = new SfxConfigManager( xStor );
//REMOVE                    else
//REMOVE                        pCfgMgr = NULL;
//REMOVE                }
//REMOVE            }
//REMOVE
//REMOVE            if ( pCfgMgr )
//REMOVE            {
//REMOVE                // create new StatusBarManager and apply changes
//REMOVE                SfxStatusBarManager* pStbMgr = new SfxStatusBarManager( this, *pMgr, pCfgMgr );
//REMOVE                Apply( pStbMgr, FALSE );
//REMOVE                pCfgMgr->StoreConfigItem( *pStbMgr );
//REMOVE                if ( bLoadedDocument )
//REMOVE                {
//REMOVE                    SfxRequest aReq( SID_SAVEDOC, SFX_CALLMODE_SYNCHRON, xDoc->GetPool() );
//REMOVE                    xDoc->ExecuteSlot( aReq );
//REMOVE                }
//REMOVE                else
//REMOVE                    pCfgMgr->StoreConfiguration();
//REMOVE
//REMOVE                StatusBar* pBar = pStbMgr->GetStatusBar();
//REMOVE                delete pStbMgr;
//REMOVE                delete pBar;
//REMOVE
//REMOVE                if ( bCreated )
//REMOVE                    delete pCfgMgr;
//REMOVE            }

        GetTabDialog()->LeaveWait();
    }

    return TRUE;
}

SfxStatusBarConfigListBox::SfxStatusBarConfigListBox( Window* pParent, const ResId& rResId)
 : SvTreeListBox(pParent, rResId)
 , bModified(FALSE)
 , bDefault(TRUE)
 , pCurEntry(0)
{

    pButton = new SvLBoxButtonData( this );
    EnableCheckButton(pButton);

    DragDropMode aDDMode = SV_DRAGDROP_CTRL_MOVE;
    SetDragDropMode( aDDMode );
    SetHighlightRange(); // OV Selektion ueber gesamte Zeilenbreite

    // Timer f"ur die BallonHelp
    aTimer.SetTimeout( 200 );
    aTimer.SetTimeoutHdl(
        LINK( this, SfxStatusBarConfigListBox, TimerHdl ) );
}

SfxStatusBarConfigListBox::~SfxStatusBarConfigListBox()
{
    delete pButton;
}

void SfxStatusBarConfigListBox::KeyInput( const KeyEvent& rKEvt )
{
    USHORT aCode = rKEvt.GetKeyCode().GetCode();
    if( aCode == KEY_SPACE )
    {
        SvLBoxEntry* pEntry = (SvLBoxEntry*) FirstSelected();
        if( pEntry && (GetCheckButtonState( pEntry ) == SV_BUTTON_UNCHECKED) )
            SetCheckButtonState(pEntry, SV_BUTTON_CHECKED);
        else
            SetCheckButtonState(pEntry, SV_BUTTON_UNCHECKED);
        GetCheckButtonHdl().Call(this);
    }
    else
        SvTreeListBox::KeyInput( rKEvt );
}

BOOL SfxStatusBarConfigListBox::NotifyAcceptDrop( SvLBoxEntry* pEntry )
{
    return SvTreeListBox::NotifyAcceptDrop(pEntry);
}

BOOL SfxStatusBarConfigListBox::NotifyMoving(SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                                SvLBoxEntry*& rpNewParent, ULONG& rNewChildPos)
{
    BOOL bRet =
        SvTreeListBox::NotifyMoving(pTarget, pEntry, rpNewParent, rNewChildPos);
    if (bRet)
    {
        bModified = TRUE;
        bDefault = FALSE;
    }
    return bRet;
}

void SfxStatusBarConfigListBox::CheckButtonHdl()
{
    bDefault = FALSE;
    bModified = TRUE;
}

void SfxStatusBarConfigListBox::MouseMove( const MouseEvent& rMEvt )
//  Beschreibung
//  Virtuelle Methode, die gerufen wird, wenn der Mauszeiger "uber der TreeListBox bewegt wurde.
//  Wenn die Position des Mauszeigers "uber dem aktuell selektierten Entry liegt, wird ein Timer
//  aufgesetzt, um ggf. einen Hilfetext einzublenden.
{
    Point aMousePos = rMEvt.GetPosPixel();
    SvLBoxEntry *pEntry = GetCurEntry();
    pCurEntry = pEntry;

    if ( pEntry && GetEntry( aMousePos ) == pEntry )
        aTimer.Start();
    else
    {
        Help::ShowBalloon( this, aMousePos, String() );
        aTimer.Stop();
    }
}

IMPL_LINK( SfxStatusBarConfigListBox, TimerHdl, Timer*, pTimer)
//  Beschreibung
//  Timer-Handler f"ur die Einblendung eines Hilfetextes. Wenn nach Ablauf des Timers
//  der Mauszeiger immer noch auf dem aktuell selektierten Eintrag steht, wird der
//  Helptext des Entries als Balloon-Help eingeblendet.
{
    if ( pTimer )
        pTimer->Stop();

    Point aMousePos = GetPointerPosPixel();
    SvLBoxEntry *pEntry = GetCurEntry();
    if ( pEntry && GetEntry( aMousePos ) == pEntry && pEntry == pCurEntry )
    {
        SfxStatBarInfo_Impl* pInfo = (SfxStatBarInfo_Impl*) pEntry->GetUserData();
        if ( !pInfo->aHelpText.Len() )
            pInfo->aHelpText = Application::GetHelp()->GetHelpText( pInfo->nId, this );

        Help::ShowBalloon( this, OutputToScreenPixel( aMousePos ), pInfo->aHelpText );
    }

    return 0L;
}



BOOL SfxStatusBarConfigPage::FillItemSet( SfxItemSet& )
{
    if ( aEntriesBox.bModified )
    {
        Apply( pMgr, aEntriesBox.bDefault );
        aEntriesBox.bModified = FALSE;
        pMgr->StoreConfig();
        return TRUE;
    }

    return FALSE;
}

void SfxStatusBarConfigPage::Reset( const SfxItemSet& )
{
    if ( !pMgr )
    {
        SfxViewFrame* pCurrent = GetTabDialog()->GetViewFrame();
        while ( pCurrent->GetParentViewFrame_Impl() )
            pCurrent = pCurrent->GetParentViewFrame_Impl();

        SfxWorkWindow* pWorkWin = pCurrent->GetFrame()->GetWorkWindow_Impl();
        pMgr = pWorkWin->GetStatusBarManager_Impl();
        if ( !pMgr )
        {
            pMgr = pWorkWin->MakeStatusBarManager_Impl( TRUE );
            bMgrCreated = TRUE;
        }

        aEntriesBox.bDefault = pMgr->IsDefault();
        Init();
    }
}
*/
String SfxConfigDialog::FileDialog_Impl( Window *pParent, WinBits nBits, const String& rTitle )
{
    BOOL bSave = ( ( nBits & WB_SAVEAS ) == WB_SAVEAS );
    short nDialogType = bSave? ::sfx2::FILESAVE_SIMPLE : ::sfx2::FILEOPEN_SIMPLE;

    sfx2::FileDialogHelper aFileDlg( nDialogType, 0 );
    aFileDlg.SetTitle( rTitle );
//  aFileDlg.SetDialogHelpId( bSave? HID_CONFIG_SAVE : HID_CONFIG_LOAD );
    aFileDlg.AddFilter( String(SfxResId(STR_FILTERNAME_ALL) ), DEFINE_CONST_UNICODE(FILEDIALOG_FILTER_ALL) );
    aFileDlg.AddFilter( String(SfxResId(STR_FILTERNAME_CFG)),DEFINE_CONST_UNICODE("*.cfg") );
    if ( ERRCODE_NONE == aFileDlg.Execute() )
        return aFileDlg.GetPath();
    else
        return String();
}

