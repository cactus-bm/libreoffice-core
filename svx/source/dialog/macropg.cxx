/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: macropg.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: kz $ $Date: 2007-10-09 15:17:51 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"

#ifndef _BASMGR_HXX //autogen
#include <basic/basmgr.hxx>
#endif


#include "macropg.hxx"

#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>
#ifndef _SV_MEDIT_HXX
#include <svtools/svmedit.hxx>
#endif
#ifndef _EITEM_HXX
#include <svtools/eitem.hxx>
#endif

#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/macrconf.hxx>
#include <sfx2/sfxdefs.hxx>

#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>

#include <svx/dialmgr.hxx>
#include "selector.hxx"
#include "cfg.hxx"
#include "macropg.hrc"
#include "helpid.hrc"
#include <svx/dialogs.hrc>

#ifndef _HEADERTABLISTBOX_HXX
#include "headertablistbox.hxx"
#endif
#ifndef _MACROPG_IMPL_HXX
#include "macropg_impl.hxx"
#endif

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

static ::rtl::OUString aVndSunStarUNO =
    ::rtl::OUString::createFromAscii( "vnd.sun.star.UNO:" );
static ::rtl::OUString aVndSunStarScript =
    ::rtl::OUString::createFromAscii( "vnd.sun.star.script:" );

_SvxMacroTabPage_Impl::_SvxMacroTabPage_Impl( const SfxItemSet& rAttrSet ) :
    pAssignFT( NULL ),
    pAssignPB( NULL ),
    pAssignComponentPB( NULL ),
    pDeletePB( NULL ),
    pMacroImg( NULL ),
    pComponentImg( NULL ),
    pMacroImg_h( NULL ),
    pComponentImg_h( NULL ),
    pStrEvent( NULL ),
    pAssignedMacro( NULL ),
    pEventLB( NULL ),
    bReadOnly( FALSE ),
    bIDEDialogMode( FALSE )
{
    const SfxPoolItem* pItem;
    if ( SFX_ITEM_SET == rAttrSet.GetItemState( SID_ATTR_MACROITEM, FALSE, &pItem ) )
        bIDEDialogMode = ((const SfxBoolItem*)pItem)->GetValue();
}

_SvxMacroTabPage_Impl::~_SvxMacroTabPage_Impl()
{
    delete pAssignFT;
    delete pAssignPB;
    delete pAssignComponentPB;
    delete pDeletePB;
    delete pMacroImg;
    delete pComponentImg;
    delete pMacroImg_h;
    delete pComponentImg_h;
    delete pStrEvent;
    delete pAssignedMacro;
    delete pEventLB;
}

// Achtung im Code wird dieses Array direkt (0, 1, ...) indiziert
static long nTabs[] =
    {
        2, // Number of Tabs
        0, 90
    };

#define TAB_WIDTH_MIN        10

// IDs for items in HeaderBar of EventLB
#define    ITEMID_EVENT        1
#define    ITMEID_ASSMACRO        2


#define LB_EVENTS_ITEMPOS    1
#define LB_MACROS_ITEMPOS    2


IMPL_LINK( _HeaderTabListBox, HeaderEndDrag_Impl, HeaderBar*, pBar )
{
    DBG_ASSERT( pBar == &maHeaderBar, "*_HeaderTabListBox::HeaderEndDrag_Impl: something is wrong here..." );
    (void)pBar;

    if( !maHeaderBar.GetCurItemId() )
        return 0;

    if( !maHeaderBar.IsItemMode() )
    {
        Size    aSz;
        USHORT    _nTabs = maHeaderBar.GetItemCount();
        long    nTmpSz = 0;
        long    nWidth = maHeaderBar.GetItemSize( ITEMID_EVENT );
        long    nBarWidth = maHeaderBar.GetSizePixel().Width();

        if( nWidth < TAB_WIDTH_MIN )
            maHeaderBar.SetItemSize( ITEMID_EVENT, TAB_WIDTH_MIN );
        else if( ( nBarWidth - nWidth ) < TAB_WIDTH_MIN )
            maHeaderBar.SetItemSize( ITEMID_EVENT, nBarWidth - TAB_WIDTH_MIN );

        {
            long    _nWidth;
            for( USHORT i = 1 ; i < _nTabs ; ++i )
            {
                _nWidth = maHeaderBar.GetItemSize( i );
                aSz.Width() =  _nWidth + nTmpSz;
                nTmpSz += _nWidth;
                maListBox.SetTab( i, PixelToLogic( aSz, MapMode( MAP_APPFONT ) ).Width(), MAP_APPFONT );
            }
        }
    }
    return 1;
}

long _HeaderTabListBox::Notify( NotifyEvent& rNEvt )
{
    long    nRet = Control::Notify( rNEvt );

    if( rNEvt.GetType() == EVENT_GETFOCUS )
    {
        if ( rNEvt.GetWindow() != &maListBox )
            maListBox.GrabFocus();
    }

    return nRet;
}

_HeaderTabListBox::_HeaderTabListBox( Window* pParent, const ResId& rId ) :
    Control( pParent, rId ),
    maListBox( this, WB_HSCROLL | WB_CLIPCHILDREN | WB_TABSTOP ),
    maHeaderBar( this, WB_BUTTONSTYLE | WB_BOTTOMBORDER )
{
//  maListBox.SetWindowBits( WB_HSCROLL | WB_CLIPCHILDREN | WB_TABSTOP );
}

_HeaderTabListBox::~_HeaderTabListBox()
{
}

void _HeaderTabListBox::ConnectElements( void )
{
    // calc pos and size of header bar
    Point    aPnt( 0, 0 );
    Size    aSize( maHeaderBar.CalcWindowSizePixel() );
    Size    aCtrlSize( GetOutputSizePixel() );
    aSize.Width() = aCtrlSize.Width();
    maHeaderBar.SetPosSizePixel( aPnt, aSize );

    // calc pos and size of ListBox
    aPnt.Y() += aSize.Height();
    aSize.Height() = aCtrlSize.Height() - aSize.Height();
    maListBox.SetPosSizePixel( aPnt, aSize );

    // set handler
    maHeaderBar.SetEndDragHdl( LINK( this, _HeaderTabListBox, HeaderEndDrag_Impl ) );

    maListBox.InitHeaderBar( &maHeaderBar );
}

void _HeaderTabListBox::Show( BOOL bVisible, USHORT nFlags )
{
    maListBox.Show( bVisible, nFlags );
    maHeaderBar.Show( bVisible, nFlags );
}

void _HeaderTabListBox::Enable( bool bEnable, bool bChild )
{
    maListBox.Enable( bEnable, bChild );
    maHeaderBar.Enable( bEnable, bChild );
}

// assign button ("Add Command") is enabled only if it is not read only
// delete button ("Remove Command") is enabled if a current binding exists
//     and it is not read only
void _SvxMacroTabPage::EnableButtons( const String& /*rLangName*/ )
{
    const SvLBoxEntry* pE = mpImpl->pEventLB->GetListBox().FirstSelected();
    if ( pE )
    {
        SvLBoxString* pEventMacro = (SvLBoxString*)pE->GetItem( LB_MACROS_ITEMPOS );
        mpImpl->pDeletePB->Enable( 0 != pEventMacro && !mpImpl->bReadOnly );

        mpImpl->pAssignPB->Enable( !mpImpl->bReadOnly );
        if( mpImpl->pAssignComponentPB )
            mpImpl->pAssignComponentPB->Enable( !mpImpl->bReadOnly );
    }
}

_SvxMacroTabPage::_SvxMacroTabPage( Window* pParent, const ResId& rResId, const SfxItemSet& rAttrSet )
    : SfxTabPage( pParent, rResId, rAttrSet ),
    m_xAppEvents(0),
    m_xDocEvents(0),
    bReadOnly(false),
    bDocModified(false),
    bAppEvents(false),
    bInitialized(false)
{
    mpImpl = new _SvxMacroTabPage_Impl( rAttrSet );
}

_SvxMacroTabPage::~_SvxMacroTabPage()
{
    // need to delete the user data
    SvHeaderTabListBox& rListBox = mpImpl->pEventLB->GetListBox();
    SvLBoxEntry* pE = rListBox.GetEntry( 0 );
    while( pE )
    {
        ::rtl::OUString* pEventName = (::rtl::OUString*)pE->GetUserData();
        delete pEventName;
        pE->SetUserData((void*)0);
        pE = rListBox.NextSibling( pE );
    }
    aUIStrings.clear();
    DELETEZ( mpImpl );
}
// -----------------------------------------------------------------------------
void _SvxMacroTabPage::InitResources()
{
    // the event name to UI string mappings for App Events
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnStartApp")),String( SVX_RES( RID_SVXSTR_EVENT_STARTAPP ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnCloseApp")),String( SVX_RES( RID_SVXSTR_EVENT_CLOSEAPP ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnNew")),String( SVX_RES( RID_SVXSTR_EVENT_CREATEDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnUnload")),String( SVX_RES( RID_SVXSTR_EVENT_CLOSEDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnPrepareUnload")),String( SVX_RES( RID_SVXSTR_EVENT_PREPARECLOSEDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnLoad")),String( SVX_RES( RID_SVXSTR_EVENT_OPENDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnSave")),String( SVX_RES( RID_SVXSTR_EVENT_SAVEDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnSaveAs")),String( SVX_RES( RID_SVXSTR_EVENT_SAVEASDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnSaveDone")),String( SVX_RES( RID_SVXSTR_EVENT_SAVEDOCDONE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnSaveAsDone")),String( SVX_RES( RID_SVXSTR_EVENT_SAVEASDOCDONE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnFocus")),String( SVX_RES( RID_SVXSTR_EVENT_ACTIVATEDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnUnfocus")),String( SVX_RES( RID_SVXSTR_EVENT_DEACTIVATEDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnPrint")),String( SVX_RES( RID_SVXSTR_EVENT_PRINTDOC ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnModifyChanged")),String( SVX_RES( RID_SVXSTR_EVENT_MODIFYCHANGED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnMailMerge")),String( SVX_RES( RID_SVXSTR_EVENT_MAILMERGE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OnPageCountChange")),String( SVX_RES( RID_SVXSTR_EVENT_PAGECOUNTCHANGE ))));

    // the event name to UI string mappings for forms & dialogs
    //
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveAction")),String( SVX_RES( RID_SVXSTR_EVENT_APPROVEACTIONPERFORMED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("actionPerformed")),String( SVX_RES( RID_SVXSTR_EVENT_ACTIONPERFORMED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("changed")),String( SVX_RES( RID_SVXSTR_EVENT_CHANGED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("textChanged")),String( SVX_RES( RID_SVXSTR_EVENT_TEXTCHANGED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("itemStateChanged")),String( SVX_RES( RID_SVXSTR_EVENT_ITEMSTATECHANGED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("focusGained")),String( SVX_RES( RID_SVXSTR_EVENT_FOCUSGAINED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("focusLost")),String( SVX_RES( RID_SVXSTR_EVENT_FOCUSLOST ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("keyPressed")),String( SVX_RES( RID_SVXSTR_EVENT_KEYTYPED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("keyReleased")),String( SVX_RES( RID_SVXSTR_EVENT_KEYUP ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mouseEntered")),String( SVX_RES( RID_SVXSTR_EVENT_MOUSEENTERED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mouseDragged")),String( SVX_RES( RID_SVXSTR_EVENT_MOUSEDRAGGED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mouseMoved")),String( SVX_RES( RID_SVXSTR_EVENT_MOUSEMOVED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mousePressed")),String( SVX_RES( RID_SVXSTR_EVENT_MOUSEPRESSED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mouseReleased")),String( SVX_RES( RID_SVXSTR_EVENT_MOUSERELEASED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("mouseExited")),String( SVX_RES( RID_SVXSTR_EVENT_MOUSEEXITED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveReset")),String( SVX_RES( RID_SVXSTR_EVENT_APPROVERESETTED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("resetted")),String( SVX_RES( RID_SVXSTR_EVENT_RESETTED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveSubmit")),String( SVX_RES( RID_SVXSTR_EVENT_SUBMITTED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveUpdate")),String( SVX_RES( RID_SVXSTR_EVENT_BEFOREUPDATE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("updated")),String( SVX_RES( RID_SVXSTR_EVENT_AFTERUPDATE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("loaded")),String( SVX_RES( RID_SVXSTR_EVENT_LOADED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("reloading")),String( SVX_RES( RID_SVXSTR_EVENT_RELOADING ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("reloaded")),String( SVX_RES( RID_SVXSTR_EVENT_RELOADED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("unloading")),String( SVX_RES( RID_SVXSTR_EVENT_UNLOADING ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("unloaded")),String( SVX_RES( RID_SVXSTR_EVENT_UNLOADED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("confirmDelete")),String( SVX_RES( RID_SVXSTR_EVENT_CONFIRMDELETE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveRowChange")),String( SVX_RES( RID_SVXSTR_EVENT_APPROVEROWCHANGE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("rowChanged")),String( SVX_RES( RID_SVXSTR_EVENT_ROWCHANGE ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveCursorMove")),String( SVX_RES( RID_SVXSTR_EVENT_POSITIONING ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("cursorMoved")),String( SVX_RES( RID_SVXSTR_EVENT_POSITIONED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("approveParameter")),String( SVX_RES( RID_SVXSTR_EVENT_APPROVEPARAMETER ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("errorOccured")),String( SVX_RES( RID_SVXSTR_EVENT_ERROROCCURED ))));
    aUIStrings.insert(UIEventsStringHash::value_type(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("adjustmentValueChanged")),String( SVX_RES( RID_SVXSTR_EVENT_ADJUSTMENTVALUECHANGED ))));
}

// the following method is called when the user clicks OK
// We use the contents of the hashes to replace the settings
BOOL _SvxMacroTabPage::FillItemSet( SfxItemSet& /*rSet*/ )
{
    try
    {
        ::rtl::OUString eventName;
        if( m_xAppEvents.is() )
        {
            EventsHash::iterator h_itEnd =  m_appEventsHash.end();
            EventsHash::iterator h_it = m_appEventsHash.begin();
            for ( ; h_it !=  h_itEnd; ++h_it )
            {
                eventName = h_it->first;
                try
                {
                    m_xAppEvents->replaceByName( eventName, GetPropsByName( eventName, m_appEventsHash ) );
                }
                catch(Exception e)
                {}
            }
        }
        if( m_xDocEvents.is() && bDocModified )
        {
            EventsHash::iterator h_itEnd =  m_docEventsHash.end();
            EventsHash::iterator h_it = m_docEventsHash.begin();
            for ( ; h_it !=  h_itEnd; ++h_it )
            {
                eventName = h_it->first;
                try
                {
                    m_xDocEvents->replaceByName( eventName, GetPropsByName( eventName, m_docEventsHash ) );
                }
                catch(Exception e)
                {}
            }
            // if we have a valid XModifiable (in the case of doc events)
            // call setModified(true)
            // in principle this should not be necessary (see issue ??)
            if(m_xModifiable.is())
            {
                m_xModifiable->setModified( sal_True );
            }
        }
    }
    catch(Exception&)
    {
    }
    // what is the return value about??
    return FALSE;
}

// the following method clears the bindings in the hashes for both doc & app
void _SvxMacroTabPage::Reset()
{
    // called once in creation - don't reset the data this time
    if(!bInitialized)
    {
        bInitialized = true;
        return;
    }

    try
    {
            Sequence< beans::PropertyValue > emptyProps(2);
            ::rtl::OUString sEmpty;
            emptyProps[0].Name = ::rtl::OUString::createFromAscii("EventType");
            emptyProps[0].Value <<= ::rtl::OUString::createFromAscii("Script");
            emptyProps[1].Name = ::rtl::OUString::createFromAscii("Script");
            emptyProps[1].Value <<= sEmpty;
            Any aEmptyProps;
            aEmptyProps <<= emptyProps;
            if( m_xAppEvents.is() )
            {
                EventsHash::iterator h_itEnd =  m_appEventsHash.end();
                EventsHash::iterator h_it = m_appEventsHash.begin();
                for ( ; h_it !=  h_itEnd; ++h_it )
                {
                    h_it->second.second = sEmpty;
                }
            }
            if( m_xDocEvents.is() && bDocModified )
            {
                EventsHash::iterator h_itEnd =  m_docEventsHash.end();
                EventsHash::iterator h_it = m_docEventsHash.begin();
                for ( ; h_it !=  h_itEnd; ++h_it )
                {
                    h_it->second.second = sEmpty;
                }
                // if we have a valid XModifiable (in the case of doc events)
                // call setModified(true)
                if(m_xModifiable.is())
                {
                    m_xModifiable->setModified( sal_True );
                }
            }
    }
    catch(Exception&)
    {
    }
    DisplayAppEvents(bAppEvents);
}

void _SvxMacroTabPage::SetReadOnly( BOOL bSet )
{
    mpImpl->bReadOnly = bSet;
}

BOOL _SvxMacroTabPage::IsReadOnly() const
{
    return mpImpl->bReadOnly;
}


class IconLBoxString : public SvLBoxString
{
    Image* m_pMacroImg;
    Image* m_pComponentImg;
    Image* m_pMacroImg_h;
    Image* m_pComponentImg_h;
    int m_nxImageOffset;

    public:
        IconLBoxString( SvLBoxEntry* pEntry, USHORT nFlags, const String& sText,
            Image* pMacroImg, Image* pComponentImg,
            Image* pMacroImg_h, Image* pComponentImg_h );
        virtual void Paint(const Point& aPos, SvLBox& aDevice, USHORT nFlags, SvLBoxEntry* pEntry );
};


IconLBoxString::IconLBoxString( SvLBoxEntry* pEntry, USHORT nFlags, const String& sText,
    Image* pMacroImg, Image* pComponentImg, Image* pMacroImg_h, Image* pComponentImg_h )
        : SvLBoxString( pEntry, nFlags, sText )
        , m_pMacroImg( pMacroImg )
        , m_pComponentImg( pComponentImg )
        , m_pMacroImg_h( pMacroImg_h )
        , m_pComponentImg_h( pComponentImg_h )
{
    m_nxImageOffset = 20;
}

//===============================================
void IconLBoxString::Paint( const Point& aPos, SvLBox& aDevice,
                               USHORT /*nFlags*/, SvLBoxEntry* /*pEntry*/ )
{
    String aTxt( GetText() );
    if( aTxt.Len() )
    {
        ::rtl::OUString aURL( aTxt );
        sal_Int32 nIndex = aURL.indexOf( aVndSunStarUNO );
        bool bUNO = nIndex == 0;

        Wallpaper aBackground = aDevice.GetBackground();
        Color     aColor      = aBackground.GetColor();
        BOOL      bHC         = aColor.IsDark();
        const Image* pImg;
        if( bHC )
            pImg = bUNO ? m_pComponentImg_h : m_pMacroImg_h;
        else
            pImg = bUNO ? m_pComponentImg : m_pMacroImg;
        aDevice.DrawImage( aPos, *pImg );

        ::rtl::OUString aPureMethod;
        if( bUNO )
        {
            sal_Int32 nBegin = aVndSunStarUNO.getLength();
            aPureMethod = aURL.copy( nBegin );
        }
        else
        {
            sal_Int32 nBegin = aVndSunStarScript.getLength();
            aPureMethod = aURL.copy( nBegin );
            aPureMethod = aPureMethod.copy( 0, aPureMethod.indexOf( '?' ) );
        }

        Point aPnt(aPos);
        aPnt.X() += m_nxImageOffset;
        aDevice.DrawText( aPnt, aPureMethod );
    }
}


// displays the app events if appEvents=true, otherwise displays the doc events
void _SvxMacroTabPage::DisplayAppEvents( bool appEvents)
{
    bAppEvents = appEvents;

    SvHeaderTabListBox&        rListBox = mpImpl->pEventLB->GetListBox();
    mpImpl->pEventLB->SetUpdateMode( FALSE );
    rListBox.Clear();
    SvLBoxEntry*    pE = rListBox.GetEntry( 0 );
    EventsHash* eventsHash;
    Reference< container::XNameReplace> nameReplace;
    if(bAppEvents)
    {
        eventsHash = &m_appEventsHash;
        nameReplace = m_xAppEvents;
    }
    else
    {
        eventsHash = &m_docEventsHash;
        nameReplace = m_xDocEvents;
    }
    // have to use the original XNameReplace since the hash iterators do
    // not guarantee the order in which the elements are returned
    if(!nameReplace.is())
    {
        return;
    }
    Sequence< ::rtl::OUString > eventNames = nameReplace->getElementNames();
    sal_Int32 nEventCount = eventNames.getLength();
    for(sal_Int32 nEvent = 0; nEvent < nEventCount; ++nEvent )
    {
        EventsHash::iterator h_it = eventsHash->find( eventNames[nEvent] );
        if( h_it == eventsHash->end() )
            continue;
        ::rtl::OUString eventName = h_it->first;
        ::rtl::OUString eventURL = h_it->second.second;
        //DF better to add the L10N version
        // might also have a hash between event name & L10N display name
        //for some reason the hash seems now to be empty?? Why??
        UIEventsStringHash::iterator ui_it = aUIStrings.find(eventName);
        String sTmp;
        if( ui_it != aUIStrings.end() )
        {
            // we have the L10N string
            sTmp = ui_it->second;
        }
        else
        {
            //no UI name => do not add
            continue;
        }
        sTmp += '\t';
        SvLBoxEntry*    _pE = rListBox.InsertEntry( sTmp );
        ::rtl::OUString* pEventName = new ::rtl::OUString( eventName );
        _pE->SetUserData( (void*)pEventName );
        String sNew( eventURL );
        _pE->ReplaceItem( new IconLBoxString( _pE, 0, sNew,
            mpImpl->pMacroImg, mpImpl->pComponentImg,
            mpImpl->pMacroImg_h, mpImpl->pComponentImg_h ), LB_MACROS_ITEMPOS );
        rListBox.GetModel()->InvalidateEntry( _pE );
        rListBox.Select( _pE );
        rListBox.MakeVisible( _pE );
    }
    pE = rListBox.GetEntry(0);
    if( pE )
        rListBox.Select( pE );
    rListBox.SetUpdateMode( TRUE );
    EnableButtons( String() );
}

// select event handler on the listbox
IMPL_STATIC_LINK( _SvxMacroTabPage, SelectEvent_Impl, SvTabListBox*, EMPTYARG )
{
    _SvxMacroTabPage_Impl*    pImpl = pThis->mpImpl;
    SvHeaderTabListBox&        rListBox = pImpl->pEventLB->GetListBox();
    SvLBoxEntry*            pE = rListBox.FirstSelected();
    ULONG                    nPos;

    if( !pE || LISTBOX_ENTRY_NOTFOUND ==
        ( nPos = rListBox.GetModel()->GetAbsPos( pE ) ) )
    {
        DBG_ASSERT( pE, "wo kommt der leere Eintrag her?" );
        return 0;
    }

    pThis->EnableButtons( String() );
    return 0;
}

IMPL_STATIC_LINK( _SvxMacroTabPage, AssignDeleteHdl_Impl, PushButton*, pBtn )
{
    return GenericHandler_Impl( pThis, pBtn );
}

IMPL_STATIC_LINK( _SvxMacroTabPage, DoubleClickHdl_Impl, SvTabListBox *, EMPTYARG )
{
    return GenericHandler_Impl( pThis, NULL );
}

// handler for double click on the listbox, and for the assign/delete buttons
long _SvxMacroTabPage::GenericHandler_Impl( _SvxMacroTabPage* pThis, PushButton* pBtn )
{
    _SvxMacroTabPage_Impl*    pImpl = pThis->mpImpl;
    SvHeaderTabListBox& rListBox = pImpl->pEventLB->GetListBox();
    SvLBoxEntry* pE = rListBox.FirstSelected();
    ULONG nPos;
    if( !pE || LISTBOX_ENTRY_NOTFOUND ==
        ( nPos = rListBox.GetModel()->GetAbsPos( pE ) ) )
    {
        DBG_ASSERT( pE, "wo kommt der leere Eintrag her?" );
        return 0;
    }

    const BOOL bAssEnabled = pBtn != pImpl->pDeletePB && pImpl->pAssignPB->IsEnabled();

    ::rtl::OUString* pEventName = (::rtl::OUString*)pE->GetUserData();

    ::rtl::OUString sEventURL;
    ::rtl::OUString sEventType;
    if(pThis->bAppEvents)
    {
        EventsHash::iterator h_it = pThis->m_appEventsHash.find( *pEventName );
        if(h_it != pThis->m_appEventsHash.end() )
        {
            sEventType = h_it->second.first;
            sEventURL = h_it->second.second;
        }
    }
    else
    {
        EventsHash::iterator h_it = pThis->m_docEventsHash.find( *pEventName );
        if(h_it != pThis->m_docEventsHash.end() )
        {
            sEventType = h_it->second.first;
            sEventURL = h_it->second.second;
        }
    }

    bool bDoubleClick = (pBtn == NULL);
    bool bUNOAssigned = (sEventURL.indexOf( aVndSunStarUNO ) == 0);
    if( pBtn == pImpl->pDeletePB )
    {
        // delete pressed
        sEventType = ::rtl::OUString::createFromAscii("Script");
        sEventURL = ::rtl::OUString();
        if(!pThis->bAppEvents)
            pThis->bDocModified = true;
    }
    else if( pBtn == pImpl->pAssignComponentPB || (bDoubleClick && bUNOAssigned) )
    {
        AssignComponentDialog* pAssignDlg = new AssignComponentDialog( pThis, sEventURL );

        short ret = pAssignDlg->Execute();
        if( ret )
        {
            sEventType = ::rtl::OUString::createFromAscii("UNO");
            sEventURL = pAssignDlg->getURL();
            if(!pThis->bAppEvents)
                pThis->bDocModified = true;
        }
        delete pAssignDlg;
    }
    else if( bAssEnabled )
    {
        // assign pressed
        SvxScriptSelectorDialog* pDlg = new SvxScriptSelectorDialog( pThis, FALSE, pThis->GetFrame() );
        if( pDlg )
        {
            short ret = pDlg->Execute();
            if ( ret )
            {
                sEventType = ::rtl::OUString::createFromAscii("Script");
                sEventURL = pDlg->GetScriptURL();
                if(!pThis->bAppEvents)
                    pThis->bDocModified = true;
            }
        }
    }

    // update the hashes
    if(pThis->bAppEvents)
    {
        EventsHash::iterator h_it = pThis->m_appEventsHash.find( *pEventName );
        h_it->second.first = sEventType;
        h_it->second.second = sEventURL;
    }
    else
    {
        EventsHash::iterator h_it = pThis->m_docEventsHash.find( *pEventName );
        h_it->second.first = sEventType;
        h_it->second.second = sEventURL;
    }

    // update the listbox entry
    pImpl->pEventLB->SetUpdateMode( FALSE );
    // pE->ReplaceItem( new SvLBoxString( pE, 0, sEventURL ), LB_MACROS_ITEMPOS );
    pE->ReplaceItem( new IconLBoxString( pE, 0, sEventURL,
            pImpl->pMacroImg, pImpl->pComponentImg,
            pImpl->pMacroImg_h, pImpl->pComponentImg_h ), LB_MACROS_ITEMPOS );

    rListBox.GetModel()->InvalidateEntry( pE );
    rListBox.Select( pE );
    rListBox.MakeVisible( pE );
    rListBox.SetUpdateMode( TRUE );

    pThis->EnableButtons( String() );
    return 0;
}

// pass in the XNameReplace.
// can remove the 3rd arg once issue ?? is fixed
void _SvxMacroTabPage::InitAndSetHandler( Reference< container::XNameReplace> xAppEvents, Reference< container::XNameReplace> xDocEvents, Reference< util::XModifiable > xModifiable )
{
    m_xAppEvents = xAppEvents;
    m_xDocEvents = xDocEvents;
    m_xModifiable = xModifiable;
    SvHeaderTabListBox&    rListBox = mpImpl->pEventLB->GetListBox();
    HeaderBar&            rHeaderBar = mpImpl->pEventLB->GetHeaderBar();
    Link                aLnk(STATIC_LINK(this, _SvxMacroTabPage, AssignDeleteHdl_Impl ));
    mpImpl->pDeletePB->SetClickHdl(    aLnk );
    mpImpl->pAssignPB->SetClickHdl(    aLnk );
    if( mpImpl->pAssignComponentPB )
        mpImpl->pAssignComponentPB->SetClickHdl( aLnk );
    rListBox.SetDoubleClickHdl( STATIC_LINK(this, _SvxMacroTabPage, DoubleClickHdl_Impl ) );

    rListBox.SetSelectHdl( STATIC_LINK( this, _SvxMacroTabPage, SelectEvent_Impl ));

    rListBox.SetSelectionMode( SINGLE_SELECTION );
    rListBox.SetTabs( &nTabs[0], MAP_APPFONT );
    Size aSize( nTabs[ 2 ], 0 );
    rHeaderBar.InsertItem( ITEMID_EVENT, *mpImpl->pStrEvent, LogicToPixel( aSize, MapMode( MAP_APPFONT ) ).Width() );
    aSize.Width() = 1764;        // don't know what, so 42^2 is best to use...
    rHeaderBar.InsertItem( ITMEID_ASSMACRO, *mpImpl->pAssignedMacro, LogicToPixel( aSize, MapMode( MAP_APPFONT ) ).Width() );
    rListBox.SetSpaceBetweenEntries( 0 );

    mpImpl->pEventLB->Show();
    mpImpl->pEventLB->ConnectElements();

    long nMinLineHeight = mpImpl->pMacroImg->GetSizePixel().Height() + 2;
    if( nMinLineHeight > mpImpl->pEventLB->GetListBox().GetEntryHeight() )
        mpImpl->pEventLB->GetListBox().SetEntryHeight(
            sal::static_int_cast< short >(nMinLineHeight) );

    mpImpl->pEventLB->Enable( TRUE );

    if(!m_xAppEvents.is())
    {
        return;
    }
    Sequence< ::rtl::OUString > eventNames = m_xAppEvents->getElementNames();
    sal_Int32 nEventCount = eventNames.getLength();
    for(sal_Int32 nEvent = 0; nEvent < nEventCount; ++nEvent )
    {
        //need exception handling here
        try
        {
            m_appEventsHash[ eventNames[nEvent] ] = GetPairFromAny( m_xAppEvents->getByName( eventNames[nEvent] ) );
        }
        catch (Exception e)
        {}
    }
    if(m_xDocEvents.is())
    {
        eventNames = m_xDocEvents->getElementNames();
        nEventCount = eventNames.getLength();
        for(sal_Int32 nEvent = 0; nEvent < nEventCount; ++nEvent )
        {
            try
            {
                m_docEventsHash[ eventNames[nEvent] ] = GetPairFromAny( m_xDocEvents->getByName( eventNames[nEvent] ) );
            }
            catch (Exception e)
            {}
        }
    }
}

// returns the two props EventType & Script for a given event name
Any _SvxMacroTabPage::GetPropsByName( const ::rtl::OUString& eventName, EventsHash& eventsHash )
{
    const ::std::pair< ::rtl::OUString, ::rtl::OUString >& rAssignedEvent( eventsHash[ eventName ] );

    Sequence< beans::PropertyValue > props(2);
    props[0].Name = ::rtl::OUString::createFromAscii("EventType");
    props[0].Value <<= rAssignedEvent.first;
    props[1].Name = ::rtl::OUString::createFromAscii("Script");
    props[1].Value <<= rAssignedEvent.second;

    return makeAny( props );
}

// converts the Any returned by GetByName into a pair which can be stored in
// the EventHash
::std::pair< ::rtl::OUString, ::rtl::OUString  > _SvxMacroTabPage::GetPairFromAny( Any aAny )
{
    Sequence< beans::PropertyValue > props;
    ::rtl::OUString type, url;
    if( sal_True == ( aAny >>= props ) )
    {
        sal_Int32 nCount = props.getLength();
        for( sal_Int32 index = 0; index < nCount ; ++index )
        {
            if ( props[ index ].Name.compareToAscii( "EventType" ) == 0 )
                props[ index ].Value >>= type;
            else if ( props[ index ].Name.compareToAscii( "Script" ) == 0 )
                props[ index ].Value >>= url;
        }
    }
    return ::std::make_pair( type, url );
}

SvxMacroTabPage::SvxMacroTabPage( Window* pParent, const ResId& rResId, const SfxItemSet& rSet, Reference< container::XNameReplace > xNameReplace, sal_uInt16 nSelectedIndex )
    : _SvxMacroTabPage( pParent, rResId, rSet )
{
    mpImpl->pStrEvent           = new String(                       SVX_RES( STR_EVENT ) );
    mpImpl->pAssignedMacro      = new String(                       SVX_RES( STR_ASSMACRO ) );
    mpImpl->pEventLB            = new _HeaderTabListBox( this,      SVX_RES( LB_EVENT ) );
    mpImpl->pAssignFT           = new FixedText( this,              SVX_RES( FT_ASSIGN ) );
    mpImpl->pAssignPB           = new PushButton( this,             SVX_RES( PB_ASSIGN ) );
    mpImpl->pDeletePB           = new PushButton( this,             SVX_RES( PB_DELETE ) );
    mpImpl->pAssignComponentPB  = new PushButton( this,         SVX_RES( PB_ASSIGN_COMPONENT ) );
    mpImpl->pMacroImg           = new Image(                        SVX_RES(IMG_MACRO) );
    mpImpl->pComponentImg       = new Image(                        SVX_RES(IMG_COMPONENT) );
    mpImpl->pMacroImg_h         = new Image(                        SVX_RES(IMG_MACRO_H) );
    mpImpl->pComponentImg_h     = new Image(                        SVX_RES(IMG_COMPONENT_H) );

    FreeResource();

    if( !mpImpl->bIDEDialogMode )
    {
        // Size aSizeAssign;
        // Point aPosAssign;
        // mpImpl->pAssignPB->GetPosSizePixel( aPosAssign, aSizeAssign );
        Point aPosAssign = mpImpl->pAssignPB->GetPosPixel();
        Point aPosComp = mpImpl->pAssignComponentPB->GetPosPixel();

        Point aPosDelete = mpImpl->pDeletePB->GetPosPixel();
        long nYDiff = aPosComp.Y() - aPosAssign.Y();
        aPosDelete.Y() -= nYDiff;
        mpImpl->pDeletePB->SetPosPixel( aPosDelete );

        mpImpl->pAssignComponentPB->Hide();
        mpImpl->pAssignComponentPB->Disable();
    }

    // must be done after FreeResource is called
    InitResources();

    mpImpl->pEventLB->GetListBox().SetHelpId( HID_SVX_MACRO_LB_EVENT );

    InitAndSetHandler( xNameReplace, Reference< container::XNameReplace>(0), Reference< util::XModifiable >(0));
    DisplayAppEvents(true);
    SvHeaderTabListBox& rListBox = mpImpl->pEventLB->GetListBox();
    SvLBoxEntry* pE = rListBox.GetEntry( (ULONG)nSelectedIndex );
    if( pE )
        rListBox.Select(pE);
}

SvxMacroTabPage::~SvxMacroTabPage()
{
}

SfxTabPage* SvxMacroTabPage::Create( Window* pParent, const SfxItemSet& rAttrSet, Reference< container::XNameReplace > xNameReplace, sal_uInt16 nSelectedIndex )
{
    return new SvxMacroTabPage( pParent, SVX_RES( RID_SVXPAGE_MACROASSIGN ), rAttrSet, xNameReplace, nSelectedIndex );
}

SvxMacroAssignDlg::SvxMacroAssignDlg( Window* pParent, SfxItemSet& rSet,
    Reference< container::XNameReplace > xNameReplace, sal_uInt16 nSelectedIndex  )
        : SvxMacroAssignSingleTabDialog( pParent, rSet, 0 )
{
    SetTabPage( SvxMacroTabPage::Create( this, rSet, xNameReplace, nSelectedIndex ) );
}

SvxMacroAssignDlg::~SvxMacroAssignDlg()
{
}


//===============================================

IMPL_LINK(AssignComponentDialog, ButtonHandler, Button *, EMPTYARG)
{
    ::rtl::OUString aMethodName = maMethodEdit.GetText();
    maURL = ::rtl::OUString();
    if( aMethodName.getLength() )
    {
        maURL = aVndSunStarUNO;
        maURL += aMethodName;
    }
    EndDialog(1);
    return 0;
}

AssignComponentDialog::AssignComponentDialog( Window * pParent, const ::rtl::OUString& rURL )
    : ModalDialog( pParent, SVX_RES( RID_SVXDLG_ASSIGNCOMPONENT ) )
    , maMethodLabel( this, SVX_RES( FT_METHOD ) )
    , maMethodEdit( this, SVX_RES( EDIT_METHOD ) )
    , maOKButton( this, SVX_RES( RID_PB_OK ) )
    , maCancelButton( this, SVX_RES( RID_PB_CANCEL ) )
    , maHelpButton( this, SVX_RES( RID_PB_HELP ) )
    , maURL( rURL )
{
    FreeResource();
    maOKButton.SetClickHdl(LINK(this, AssignComponentDialog, ButtonHandler));

    ::rtl::OUString aMethodName;
    if( maURL.getLength() )
    {
        sal_Int32 nIndex = maURL.indexOf( aVndSunStarUNO );
        if( nIndex == 0 )
        {
            sal_Int32 nBegin = aVndSunStarUNO.getLength();
            aMethodName = maURL.copy( nBegin );
        }
    }
    maMethodEdit.SetText( aMethodName, Selection( 0, SELECTION_MAX ) );
}

AssignComponentDialog::~AssignComponentDialog()
{
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxMacroAssignSingleTabDialog, OKHdl_Impl, Button *, pButton )
{
    (void)pButton; //unused
    pPage->FillItemSet( *pOutSet );
    EndDialog( RET_CANCEL );
    return 0;
}

// -----------------------------------------------------------------------

SvxMacroAssignSingleTabDialog::SvxMacroAssignSingleTabDialog
    ( Window *pParent, const SfxItemSet& rSet, sal_uInt16 nUniqueId ) :
        SfxModalDialog( pParent, nUniqueId, WinBits( WB_STDMODAL | WB_3DLOOK ) ),
        pFixedLine      ( 0 ),
        pOKBtn          ( 0 ),
        pCancelBtn      ( 0 ),
        pHelpBtn        ( 0 ),
        pPage           ( 0 ),
        pOptions        ( &rSet ),
        pOutSet         ( 0 )
{}


// -----------------------------------------------------------------------

SvxMacroAssignSingleTabDialog::~SvxMacroAssignSingleTabDialog()
{
    delete pFixedLine;
    delete pOKBtn;
    delete pCancelBtn;
    delete pHelpBtn;
    delete pPage;
}

// -----------------------------------------------------------------------

// According to SfxSingleTabDialog
void SvxMacroAssignSingleTabDialog::SetTabPage( SfxTabPage* pTabPage )
{
    pFixedLine = new FixedLine( this );

    pOKBtn = new OKButton( this, WB_DEFBUTTON );
    pOKBtn->SetClickHdl( LINK( this, SvxMacroAssignSingleTabDialog, OKHdl_Impl ) );

    pCancelBtn = new CancelButton( this );
    pHelpBtn = new HelpButton( this );

    pPage = pTabPage;

    if ( pPage )
    {
        String sUserData;
        pPage->SetUserData( sUserData );
        pPage->Reset( *pOptions );
        pPage->Show();

        // Set dialog's and buttons' size and position according to tabpage size
        long nSpaceX = LogicToPixel( Size( 6, 0 ), MAP_APPFONT ).Width();
        long nSpaceY = LogicToPixel( Size( 0, 6 ), MAP_APPFONT ).Height();
        long nHalfSpaceX = LogicToPixel( Size( 3, 0 ), MAP_APPFONT ).Width();
        long nHalfSpaceY = LogicToPixel( Size( 0, 3 ), MAP_APPFONT ).Height();

        pPage->SetPosPixel( Point() );
        Size aTabpageSize( pPage->GetSizePixel() );
        Size aDialogSize( aTabpageSize );
        Size aButtonSize = LogicToPixel( Size( 50, 14 ), MAP_APPFONT );
        long nButtonWidth  = aButtonSize.Width();
        long nButtonHeight = aButtonSize.Height();

        Size aFixedLineSize( aTabpageSize );
        long nFixedLineHeight = LogicToPixel( Size( 0, 8 ), MAP_APPFONT ).Height();
        aFixedLineSize.Height() = nFixedLineHeight;

        aDialogSize.Height() += nFixedLineHeight + nButtonHeight + nSpaceY + nHalfSpaceY;
        SetOutputSizePixel( aDialogSize );

        long nButtonPosY = aTabpageSize.Height() + nFixedLineHeight + nHalfSpaceY;
        long nHelpButtonPosX = nSpaceX;
        pHelpBtn->SetPosSizePixel( Point( nHelpButtonPosX, nButtonPosY), aButtonSize );
        pHelpBtn->Show();

        long nCancelButtonPosX = aDialogSize.Width() - nButtonWidth - nSpaceX + 1;
        pCancelBtn->SetPosSizePixel( Point( nCancelButtonPosX, nButtonPosY), aButtonSize );
        pCancelBtn->Show();

        long nOkButtonPosX = nCancelButtonPosX - nButtonWidth - nHalfSpaceX;
        pOKBtn->SetPosSizePixel( Point( nOkButtonPosX, nButtonPosY), aButtonSize );
        pOKBtn->Show();

        long nFixedLinePosY = aTabpageSize.Height();
        pFixedLine->SetPosSizePixel( Point( 0, nFixedLinePosY), aFixedLineSize );
        pFixedLine->Show();

        // Get text from TabPage
        SetText( pPage->GetText() );

        // Get IDs from TabPage
        SetHelpId( pPage->GetHelpId() );
        SetUniqueId( pPage->GetUniqueId() );
    }
}
