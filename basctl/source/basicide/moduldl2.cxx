/*************************************************************************
 *
 *  $RCSfile: moduldl2.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: tbe $ $Date: 2001-11-07 10:18:27 $
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


#define GLOBALOVERFLOW


#include <ide_pch.hxx>

#include <svtools/filedlg.hxx>

#pragma hdrstop

#include <sot/storinfo.hxx>

#include <moduldlg.hrc>
#include <moduldlg.hxx>
#include <basidesh.hrc>
#include <basidesh.hxx>
#include <bastypes.hxx>
#include <basobj.hxx>
#include <baside2.hrc>
#include <iderdll.hxx>
#include <iderdll2.hxx>
#include <svx/passwd.hxx>
#include <sbxitem.hxx>
#include <basdoc.hxx>

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _SVSTOR_HXX //autogen
#include <so3/svstor.hxx>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_TEMPLATEDESCRIPTION_HPP_
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRYARYCONTAINER2_HPP_
#include <com/sun/star/script/XLibraryContainer2.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINERPASSWORD_HPP_
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#endif

#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>

using namespace ::comphelper;
using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::ui::dialogs;


//----------------------------------------------------------------------------
//  BasicLibUserData
//----------------------------------------------------------------------------

class BasicLibUserData
{
private:
    SfxObjectShell* m_pShell;

public:
                    BasicLibUserData( SfxObjectShell* pShell ) { m_pShell = pShell; }
                    virtual         ~BasicLibUserData() {};

    SfxObjectShell* GetShell() const { return m_pShell; }
};


//----------------------------------------------------------------------------
//  BasicLibLBoxString
//----------------------------------------------------------------------------

class BasicLibLBoxString : public SvLBoxString
{
public:
    BasicLibLBoxString( SvLBoxEntry* pEntry, USHORT nFlags, const String& rTxt ) :
        SvLBoxString( pEntry, nFlags, rTxt ) {}

    virtual void Paint( const Point& rPos, SvLBox& rDev, USHORT nFlags, SvLBoxEntry* pEntry );
};

//----------------------------------------------------------------------------

void BasicLibLBoxString::Paint( const Point& rPos, SvLBox& rDev, USHORT, SvLBoxEntry* pEntry )
{

    Font aOldFont( rDev.GetFont() );
    Font aFont( aOldFont );
    DBG_ASSERT( pEntry && pEntry->GetUserData(), "BasicLibLBoxString::Paint: Entry or UserData invalid!" );

    // change text color, if library is readonly
    SfxObjectShell* pShell = ((BasicLibUserData*)pEntry->GetUserData())->GetShell();
    ::rtl::OUString aOULibName( ((SvLBoxString*)pEntry->GetItem(1))->GetText() );
    Reference< script::XLibraryContainer2 > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
    Reference< script::XLibraryContainer2 > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pShell ), UNO_QUERY );
    if ( ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryReadOnly( aOULibName ) ) ||
         ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && xDlgLibContainer->isLibraryReadOnly( aOULibName ) ) )
    {
        aFont.SetColor( Application::GetSettings().GetStyleSettings().GetDeactiveTextColor() );
    }

    rDev.SetFont( aFont );
    rDev.DrawText( rPos, GetText() );
    rDev.SetFont( aOldFont );
}


//----------------------------------------------------------------------------
//  BasicCheckBox
//----------------------------------------------------------------------------

BasicCheckBox::BasicCheckBox( Window* pParent, const ResId& rResId ) :
        SvTabListBox( pParent, rResId )
{
    nMode = 0;
    pBasMgr = 0;
    long aTabs[] = { 1, 12 };   // Mindestens einen braucht die TabPos...
                                // 12 wegen der Checkbox
    SetTabs( aTabs );
    Init();
}

//----------------------------------------------------------------------------

__EXPORT BasicCheckBox::~BasicCheckBox()
{
    //delete pCheckButton;
}


//----------------------------------------------------------------------------

/*
void __EXPORT BasicCheckBox::CheckButtonHdl()
{
    SvLBoxEntry* pEntry = GetHdlEntry();
    DBG_ASSERT( pEntry, "HdlEntry?!" );
    ULONG nPos = GetModel()->GetAbsPos( pEntry );
    if ( !nPos && ( GetMode() == LIBMODE_MANAGER ) )
    {
        CheckEntryPos( 0, TRUE );
        ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_CANNOTUNLOADSTDLIB ) ) ).Execute();
    }
    else
    {
        // SelectHdl ausloesen, damit Buttons geprueft werden.
        SelectEntryPos( nPos, FALSE );
        SelectEntryPos( nPos, TRUE );
    }
}
*/

//----------------------------------------------------------------------------

void BasicCheckBox::Init()
{
    /*
    BasicCheckBoxBitmaps theBmps;

    pCheckButton = new SvLBoxButtonData;
    pCheckButton->aBmps[SV_BMP_UNCHECKED]   = theBmps.GetUncheckedBmp();
    pCheckButton->aBmps[SV_BMP_CHECKED]     = theBmps.GetCheckedBmp();
    pCheckButton->aBmps[SV_BMP_HICHECKED]   = theBmps.GetHiCheckedBmp();
    pCheckButton->aBmps[SV_BMP_HIUNCHECKED] = theBmps.GetHiUncheckedBmp();
    pCheckButton->aBmps[SV_BMP_TRISTATE]    = theBmps.GetTriStateBmp();
    pCheckButton->aBmps[SV_BMP_HITRISTATE]  = theBmps.GetHiTriStateBmp();
    EnableCheckButton( pCheckButton );
    */
    SetHighlightRange();
}

//----------------------------------------------------------------------------

SvLBoxEntry* BasicCheckBox::InsertEntry( const String& rStr, ULONG nPos )
{
//  return SvTreeListBox::InsertEntry( rStr, NULL, FALSE, nPos );
    return SvTabListBox::InsertEntry( rStr, nPos, 0 );
}

//----------------------------------------------------------------------------

void BasicCheckBox::RemoveEntry( ULONG nPos )
{
    if ( nPos < GetEntryCount() )
        SvTreeListBox::GetModel()->Remove( GetEntry( nPos ) );
}

//----------------------------------------------------------------------------

SvLBoxEntry* BasicCheckBox::FindEntry( const String& rName )
{
    ULONG nCount = GetEntryCount();
    for ( ULONG i = 0; i < nCount; i++ )
    {
        SvLBoxEntry* pEntry = GetEntry( i );
        DBG_ASSERT( pEntry, "pEntry?!" );
        if ( rName.CompareIgnoreCaseToAscii( GetEntryText( pEntry, 0 ) ) == COMPARE_EQUAL )
            return pEntry;
    }
    return 0;
}

//----------------------------------------------------------------------------

void BasicCheckBox::SelectEntryPos( ULONG nPos, BOOL bSelect )
{
    if ( nPos < GetEntryCount() )
        Select( GetEntry( nPos ), bSelect );
}

//----------------------------------------------------------------------------

ULONG BasicCheckBox::GetSelectEntryPos() const
{
    return GetModel()->GetAbsPos( FirstSelected() );
}

//----------------------------------------------------------------------------

/*
ULONG BasicCheckBox::GetCheckedEntryCount() const
{
    ULONG   nCheckCount = 0;
    ULONG   nCount      = GetEntryCount();

    for (ULONG i=0; i<nCount; i++ )
    {
        if ( IsChecked( i ) )
            nCheckCount++;
    }

    return nCheckCount;
}
*/

//----------------------------------------------------------------------------

/*
void BasicCheckBox::CheckEntryPos( ULONG nPos, BOOL bCheck )
{
    if ( nPos < GetEntryCount() )
    {
        SvLBoxEntry* pEntry = GetEntry( nPos );

        if ( bCheck != GetCheckButtonState( pEntry ) )
            SetCheckButtonState( pEntry,
                                 bCheck
                                    ? SvButtonState(SV_BUTTON_CHECKED)
                                    : SvButtonState(SV_BUTTON_UNCHECKED) );
    }
}
*/

//----------------------------------------------------------------------------

/*
BOOL BasicCheckBox::IsChecked( ULONG nPos ) const
{
    if ( nPos < GetEntryCount() )
        return (GetCheckButtonState( GetEntry( nPos ) ) == SV_BUTTON_CHECKED);
    return FALSE;
}
*/

//----------------------------------------------------------------------------

void BasicCheckBox::InitEntry( SvLBoxEntry* pEntry, const XubString& rTxt, const Image& rImg1, const Image& rImg2 )
{
    SvTabListBox::InitEntry( pEntry, rTxt, rImg1, rImg2 );
    USHORT nCount = pEntry->ItemCount();

    // initialize all columns with own string class (column 0 == bitmap)
    for ( USHORT nCol = 1; nCol < nCount; ++nCol )
    {
        SvLBoxString* pCol = (SvLBoxString*)pEntry->GetItem( nCol );
        BasicLibLBoxString* pStr = new BasicLibLBoxString( pEntry, 0, pCol->GetText() );
        pEntry->ReplaceItem( pStr, nCol );
    }
}

//----------------------------------------------------------------------------

BOOL __EXPORT BasicCheckBox::EditingEntry( SvLBoxEntry* pEntry, Selection& )
{
    if ( nMode != LIBMODE_MANAGER )
        return FALSE;

    DBG_ASSERT( pEntry, "Kein Eintrag?" );

    // check, if Standard library
    String aLibName = GetEntryText( pEntry, 0 );
    if ( aLibName.EqualsIgnoreCaseAscii( "Standard" ) )
    {
        ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_CANNOTCHANGENAMESTDLIB ) ) ).Execute();
        return FALSE;
    }

    // check, if library is readonly
    SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
    ::rtl::OUString aOULibName( aLibName );
    Reference< script::XLibraryContainer2 > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
    Reference< script::XLibraryContainer2 > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pShell ), UNO_QUERY );
    if ( ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryReadOnly( aOULibName ) && !xModLibContainer->isLibraryLink( aOULibName ) ) ||
         ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && xDlgLibContainer->isLibraryReadOnly( aOULibName ) && !xDlgLibContainer->isLibraryLink( aOULibName ) ) )
    {
        ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_LIBISREADONLY ) ) ).Execute();
        return FALSE;
    }

    // TODO: check if library is reference/link

    // Prueffen, ob Referenz...
    /*
    USHORT nLib = pBasMgr->GetLibId( GetEntryText( pEntry, 0 ) );
    DBG_ASSERT( nLib != LIB_NOTFOUND, "LibId ?!" );
    if ( pBasMgr->IsReference( nLib ) )
    {
        ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_CANNOTCHANGENAMEREFLIB ) ) ).Execute();
        return FALSE;
    }
    */
    return TRUE;
}

//----------------------------------------------------------------------------

BOOL __EXPORT BasicCheckBox::EditedEntry( SvLBoxEntry* pEntry, const String& rNewText )
{
    BOOL bValid = ( rNewText.Len() <= 30 ) && BasicIDE::IsValidSbxName( rNewText );
    String aCurText( GetEntryText( pEntry, 0 ) );
    if ( bValid && ( aCurText != rNewText ) )
    {
        DBG_ASSERT( pBasMgr, "BasMgr nicht gesetzt!" );
        SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );

        try
        {
            ::rtl::OUString aOUOldName( aCurText );
            ::rtl::OUString aOUNewName( rNewText );

            Reference< script::XLibraryContainer2 > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
            if ( xModLibContainer.is() )
            {
                xModLibContainer->renameLibrary( aOUOldName, aOUNewName );
            }

            Reference< script::XLibraryContainer2 > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pShell ), UNO_QUERY );
            if ( xDlgLibContainer.is() )
            {
                xDlgLibContainer->renameLibrary( aOUOldName, aOUNewName );
            }

            BasicIDE::MarkDocShellModified( pShell );
            BasicIDE::GetBindings().Invalidate( SID_BASICIDE_LIBSELECTOR );
            BasicIDE::GetBindings().Update( SID_BASICIDE_LIBSELECTOR );
        }
        catch ( container::ElementExistException& )
        {
            ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_SBXNAMEALLREADYUSED ) ) ).Execute();
            return FALSE;
        }
        catch ( container::NoSuchElementException& e )
        {
            ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
            DBG_ERROR( aBStr.GetBuffer() );
            return FALSE;
        }
    }

    if ( !bValid )
    {
        if ( rNewText.Len() > 30 )
            ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_LIBNAMETOLONG ) ) ).Execute();
        else
            ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_BADSBXNAME ) ) ).Execute();
    }

    return bValid;
}

//----------------------------------------------------------------------------

/*
BasicCheckBoxBitmaps::BasicCheckBoxBitmaps() :
    Resource        ( ResId( RID_RES_CHECKBITMAPS ) ),
    aUncheckedBmp   ( ResId( CHKBTN_UNCHECKED ) ),
    aCheckedBmp     ( ResId( CHKBTN_CHECKED ) ),
    aHiCheckedBmp   ( ResId( CHKBTN_HICHECKED ) ),
    aHiUncheckedBmp ( ResId( CHKBTN_HIUNCHECKED ) ),
    aTriStateBmp    ( ResId( CHKBTN_TRISTATE ) ),
    aHiTriStateBmp  ( ResId( CHKBTN_HITRISTATE ) )
{
    FreeResource();
}
*/


//----------------------------------------------------------------------------
// NewObjectDialog
//----------------------------------------------------------------------------

NewObjectDialog::NewObjectDialog( Window* pParent, USHORT nMode )
    : ModalDialog( pParent, IDEResId( RID_DLG_NEWLIB ) ),
        aText( this, IDEResId( RID_FT_NEWLIB ) ),
        aEdit( this, IDEResId( RID_ED_LIBNAME ) ),
        aOKButton( this, IDEResId( RID_PB_OK ) ),
        aCancelButton( this, IDEResId( RID_PB_CANCEL ) )
{
    FreeResource();
    aEdit.GrabFocus();

    if ( nMode == NEWOBJECTMODE_LIB )
    {
        SetText( String( IDEResId( RID_STR_NEWLIB ) ) );
    }
    else if ( nMode == NEWOBJECTMODE_MOD )
    {
        SetText( String( IDEResId( RID_STR_NEWMOD ) ) );
    }
    else
    {
        SetText( String( IDEResId( RID_STR_NEWDLG ) ) );
    }
}

//----------------------------------------------------------------------------

NewObjectDialog::~NewObjectDialog()
{
}


//----------------------------------------------------------------------------
//  LibPage
//----------------------------------------------------------------------------

LibPage::LibPage( Window * pParent ) :
        TabPage(        pParent,IDEResId( RID_TP_LIBS ) ),
        aBasicsText(    this,   IDEResId( RID_STR_BASICS ) ),
        aBasicsBox(     this,   IDEResId( RID_LB_BASICS ) ),
        aLibText(       this,   IDEResId( RID_STR_LIB ) ),
        aLibBox(        this,   IDEResId( RID_TRLBOX ) ),
        aEditButton(    this,   IDEResId( RID_PB_EDIT ) ),
        aCloseButton(   this,   IDEResId( RID_PB_CLOSE ) ),
        aPasswordButton(this,   IDEResId( RID_PB_PASSWORD ) ),
        aNewLibButton(  this,   IDEResId( RID_PB_NEWLIB ) ),
        aInsertLibButton( this, IDEResId( RID_PB_APPEND ) ),
        aDelButton(     this,   IDEResId( RID_PB_DELETE ) )
{
    FreeResource();
    pTabDlg = 0;

    aEditButton.SetClickHdl( LINK( this, LibPage, ButtonHdl ) );
    aNewLibButton.SetClickHdl( LINK( this, LibPage, ButtonHdl ) );
    aPasswordButton.SetClickHdl( LINK( this, LibPage, ButtonHdl ) );
    aInsertLibButton.SetClickHdl( LINK( this, LibPage, ButtonHdl ) );
    aDelButton.SetClickHdl( LINK( this, LibPage, ButtonHdl ) );
    aCloseButton.SetClickHdl( LINK( this, LibPage, ButtonHdl ) );
    aLibBox.SetSelectHdl( LINK( this, LibPage, TreeListHighlightHdl ) );

    aBasicsBox.SetSelectHdl( LINK( this, LibPage, BasicSelectHdl ) );

    aLibBox.SetMode( LIBMODE_MANAGER );
    aLibBox.EnableInplaceEditing( TRUE );
    //aLibBox.SetCheckButtonHdl( LINK( this, LibPage, CheckBoxHdl ) );
    aLibBox.SetWindowBits( WB_HSCROLL );
    aCloseButton.GrabFocus();

    long aTabs[] = { 2, 30, 120 };
    aLibBox.SetTabs( aTabs, MAP_PIXEL );

    FillListBox();
    aBasicsBox.SelectEntryPos( 0 );
    SetCurLib();

    CheckButtons();
}

//----------------------------------------------------------------------------

void LibPage::CheckButtons()
{
    /*
    SvLBoxEntry* pCur = aLibBox.GetCurEntry();
    BOOL bCurChecked = pCur ? aLibBox.IsChecked( aLibBox.GetModel()->GetAbsPos( pCur ) ) : FALSE;
    if ( bCurChecked )
    {
        aEditButton.Enable();
        if ( aLibBox.GetModel()->GetAbsPos( pCur ) != 0 )
            aPasswordButton.Enable();
        else
            aPasswordButton.Disable();
    }
    else
    {
        aEditButton.Disable();
        aPasswordButton.Disable();
    }

    if ( pCur && aLibBox.GetModel()->GetAbsPos( pCur ) )
        aDelButton.Enable();
    else
        aDelButton.Disable();
    */

    SvLBoxEntry* pCur = aLibBox.GetCurEntry();
    if ( pCur )
    {
        SfxObjectShell* pShell = BasicIDE::FindDocShell( aLibBox.GetBasicManager() );
        String aLibName = aLibBox.GetEntryText( pCur, 0 );
        ::rtl::OUString aOULibName( aLibName );
        Reference< script::XLibraryContainer2 > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
        Reference< script::XLibraryContainer2 > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pShell ), UNO_QUERY );

        if ( aLibName.EqualsIgnoreCaseAscii( "Standard" ) )
        {
            aPasswordButton.Disable();
            aDelButton.Disable();
        }
        else if ( ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryReadOnly( aOULibName ) ) ||
                  ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && xDlgLibContainer->isLibraryReadOnly( aOULibName ) ) )
        {
            aPasswordButton.Disable();

            if ( ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryReadOnly( aOULibName ) && !xModLibContainer->isLibraryLink( aOULibName ) ) ||
                 ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && xDlgLibContainer->isLibraryReadOnly( aOULibName ) && !xDlgLibContainer->isLibraryLink( aOULibName ) ) )
                aDelButton.Disable();
            else
                aDelButton.Enable();
        }
        else
        {
            aPasswordButton.Enable();
            aDelButton.Enable();
        }
    }
}

//----------------------------------------------------------------------------

/*
IMPL_LINK_INLINE_START( LibPage, CheckBoxHdl, SvTreeListBox *, EMPTYARG )
{
    CheckButtons();
    return 0;
}
IMPL_LINK_INLINE_END( LibPage, CheckBoxHdl, SvTreeListBox *, EMPTYARG )
*/

//----------------------------------------------------------------------------


void __EXPORT LibPage::ActivatePage()
{
    SetCurLib();
}

//----------------------------------------------------------------------------


void __EXPORT LibPage::DeactivatePage()
{
    //ActivateCurrentLibSettings();
}

//----------------------------------------------------------------------------


IMPL_LINK_INLINE_START( LibPage, TreeListHighlightHdl, SvTreeListBox *, pBox )
{
    if ( pBox->IsSelected( pBox->GetHdlEntry() ) )
        CheckButtons();
    return 0;
}
IMPL_LINK_INLINE_END( LibPage, TreeListHighlightHdl, SvTreeListBox *, pBox )

//----------------------------------------------------------------------------

IMPL_LINK_INLINE_START( LibPage, BasicSelectHdl, ListBox *, pBox )
{
    SetCurLib();
    CheckButtons();
    return 0;
}
IMPL_LINK_INLINE_END( LibPage, BasicSelectHdl, ListBox *, pBox )

//----------------------------------------------------------------------------

IMPL_LINK( LibPage, ButtonHdl, Button *, pButton )
{
    if ( pButton == &aEditButton )
    {
        //ActivateCurrentLibSettings();
        SfxViewFrame* pViewFrame = SfxViewFrame::Current();
        DBG_ASSERT( pViewFrame != NULL, "No current view frame!" );
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if ( pDispatcher )
        {
            pDispatcher->Execute( SID_BASICIDE_APPEAR, SFX_CALLMODE_SYNCHRON );
        }
        SvLBoxEntry* pCurEntry = aLibBox.GetCurEntry();
        DBG_ASSERT( pCurEntry, "Entry?!" );
        String aLib( CreateMgrAndLibStr( aCurBasMgr, aLibBox.GetEntryText( pCurEntry, 0 ) ) );
        SfxStringItem aLibName( SID_BASICIDE_ARG_LIBNAME, aLib );
        BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
        pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if ( pDispatcher )
        {
            pDispatcher->Execute( SID_BASICIDE_LIBSELECTED,
                                    SFX_CALLMODE_ASYNCHRON, &aLibName, 0L );
        }
        EndTabDialog( 1 );
    }
    else if ( pButton == &aNewLibButton )
        NewLib();
    else if ( pButton == &aInsertLibButton )
        InsertLib();
    else if ( pButton == &aDelButton )
        DeleteCurrent();
    else if ( pButton == &aCloseButton )
    {
        //ActivateCurrentLibSettings();
        EndTabDialog( 0 );
    }
    else if ( pButton == &aPasswordButton )
    {
        SfxObjectShell* pShell = BasicIDE::FindDocShell( aLibBox.GetBasicManager() );
        SvLBoxEntry* pCurEntry = aLibBox.GetCurEntry();
        String aLibName( aLibBox.GetEntryText( pCurEntry, 0 ) );
        ::rtl::OUString aOULibName( aLibName );

        // load module library (if not loaded)
        Reference< script::XLibraryContainer > xModLibContainer = BasicIDE::GetModuleLibraryContainer( pShell );
        if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && !xModLibContainer->isLibraryLoaded( aOULibName ) )
        {
            Application::EnterWait();
            xModLibContainer->loadLibrary( aOULibName );
            Application::LeaveWait();
        }

        // load dialog library (if not loaded)
        Reference< script::XLibraryContainer > xDlgLibContainer = BasicIDE::GetDialogLibraryContainer( pShell );
        if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && !xDlgLibContainer->isLibraryLoaded( aOULibName ) )
        {
            Application::EnterWait();
            xDlgLibContainer->loadLibrary( aOULibName );
            Application::LeaveWait();
        }

        // check, if library is password protected
        if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
        {
            Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
            if ( xPasswd.is() )
            {
                BOOL bProtected = xPasswd->isLibraryPasswordProtected( aOULibName );

                // change password dialog
                SvxPasswordDialog* pDlg = new SvxPasswordDialog( this, TRUE, !bProtected );
                pDlg->SetCheckPasswordHdl( LINK( this, LibPage, CheckPasswordHdl ) );

                if ( pDlg->Execute() == RET_OK )
                {
                    BOOL bNewProtected = xPasswd->isLibraryPasswordProtected( aOULibName );

                    if ( bNewProtected != bProtected )
                    {
                        ULONG nPos = (ULONG)aLibBox.GetModel()->GetAbsPos( pCurEntry );
                        aLibBox.GetModel()->Remove( pCurEntry );
                        ImpInsertLibEntry( aLibName, nPos );
                        aLibBox.SetCurEntry( aLibBox.GetEntry( nPos ) );
                    }
                }
                delete pDlg;
            }
        }
    }
    CheckButtons();
    return 0;
}

//----------------------------------------------------------------------------

IMPL_LINK_INLINE_START( LibPage, CheckPasswordHdl, SvxPasswordDialog *, pDlg )
{
    long nRet = 0;

    SfxObjectShell* pShell = BasicIDE::FindDocShell( aLibBox.GetBasicManager() );
    SvLBoxEntry* pCurEntry = aLibBox.GetCurEntry();
    ::rtl::OUString aOULibName( aLibBox.GetEntryText( pCurEntry, 0 ) );
    Reference< script::XLibraryContainerPassword > xPasswd( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );

    if ( xPasswd.is() )
    {
        try
        {
            ::rtl::OUString aOUOldPassword( pDlg->GetOldPassword() );
            ::rtl::OUString aOUNewPassword( pDlg->GetNewPassword() );
            xPasswd->changeLibraryPassword( aOULibName, aOUOldPassword, aOUNewPassword );
            nRet = 1;
        }
        catch (...)
        {
        }
    }

    return nRet;
}
IMPL_LINK_INLINE_END( LibPage, CheckPasswordHdl, SvxPasswordDialog *, pDlg )

//----------------------------------------------------------------------------

void LibPage::NewLib()
{
    BasicManager* pBasMgr = BasicIDE::FindBasicManager( aCurBasMgr );
    DBG_ASSERT( pBasMgr, "BasMgr?!" );
    SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );

    // create library name
    String aLibName;
    String aLibStdName( String( RTL_CONSTASCII_USTRINGPARAM( "Library" ) ) );
    //String aLibStdName( IDEResId( RID_STR_STDLIBNAME ) );
    BOOL bValid = FALSE;
    USHORT i = 1;
    while ( !bValid )
    {
        aLibName = aLibStdName;
        aLibName += String::CreateFromInt32( i );
        if ( !BasicIDE::HasModuleLibrary( pShell, aLibName ) && !BasicIDE::HasDialogLibrary( pShell, aLibName ) )
            bValid = TRUE;
        i++;
    }

    NewObjectDialog* pNewDlg = new NewObjectDialog( this, NEWOBJECTMODE_LIB );
    pNewDlg->SetObjectName( aLibName );

    if ( pNewDlg->Execute() )
    {
        if ( pNewDlg->GetObjectName().Len() )
            aLibName = pNewDlg->GetObjectName();

        if ( aLibName.Len() > 30 )
        {
            ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_LIBNAMETOLONG ) ) ).Execute();
        }
        else if ( !BasicIDE::IsValidSbxName( aLibName ) )
        {
            ErrorBox( this, WB_OK | WB_DEF_OK,
                        String( IDEResId( RID_STR_BADSBXNAME ) ) ).Execute();
        }
        else if ( BasicIDE::HasModuleLibrary( pShell, aLibName ) || BasicIDE::HasDialogLibrary( pShell, aLibName ) )
        {
            ErrorBox( this, WB_OK | WB_DEF_OK,
                        String( IDEResId( RID_STR_SBXNAMEALLREADYUSED2 ) ) ).Execute();
        }
        else
        {
            try
            {
                // create module and dialog library
                Reference< container::XNameContainer > xModLib = BasicIDE::CreateModuleLibrary( pShell, aLibName );
                Reference< container::XNameContainer > xDlgLib = BasicIDE::CreateDialogLibrary( pShell, aLibName );

                SvLBoxEntry* pEntry = aLibBox.InsertEntry( aLibName );
                pEntry->SetUserData( new BasicLibUserData( pShell ) );
                aLibBox.SetCurEntry( pEntry );
                //USHORT nPos = (USHORT)aLibBox.GetModel()->GetAbsPos( pEntry );
                //aLibBox.CheckEntryPos( nPos, bLoaded );

                // create a module
                String aModName = BasicIDE::CreateModuleName( pShell, aLibName );
                ::rtl::OUString aModule = BasicIDE::CreateModule( pShell, aLibName, aModName, TRUE );
                SbxItem aSbxItem( SID_BASICIDE_ARG_SBX, pShell, aLibName, aModName, BASICIDE_TYPE_MODULE );
                BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
                SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
                SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
                if( pDispatcher )
                {
                    pDispatcher->Execute( SID_BASICIDE_SBXINSERTED,
                                          SFX_CALLMODE_SYNCHRON, &aSbxItem, 0L );
                }
            }
            catch ( container::ElementExistException& e )
            {
                ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                DBG_ERROR( aBStr.GetBuffer() );
            }
            catch ( container::NoSuchElementException& e )
            {
                ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                DBG_ERROR( aBStr.GetBuffer() );
            }
        }
    }
    delete pNewDlg;
}

//----------------------------------------------------------------------------

void LibPage::InsertLib()
{
//  BasicManager* pBasMgr = BasicIDE::FindBasicManager( aCurBasMgr );
    BasicManager* pBasMgr = aLibBox.GetBasicManager();
    DBG_ASSERT( pBasMgr, "BasMgr?!" );

    Reference< lang::XMultiServiceFactory > xMSF( ::comphelper::getProcessServiceFactory() );
    Reference < XFilePicker > xFP;
    if( xMSF.is() )
    {
        Sequence <Any> aServiceType(1);
        aServiceType[0] <<= TemplateDescription::FILEOPEN_SIMPLE;
        xFP = Reference< XFilePicker >( xMSF->createInstanceWithArguments(
                    ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.FilePicker" ) ), aServiceType ), UNO_QUERY );
    }

    xFP->setTitle( String( IDEResId( RID_STR_APPENDLIBS ) ) );

    Reference< XFilterManager > xFltMgr(xFP, UNO_QUERY);
    xFltMgr->appendFilter( String( IDEResId( RID_STR_BASIC ) ), String( RTL_CONSTASCII_USTRINGPARAM( "*.sbl" ) ) );
    xFltMgr->appendFilter( String( IDEResId( RID_STR_DOC ) ), String( RTL_CONSTASCII_USTRINGPARAM( "*.sdw;*.sdc;*.sdd" ) ) );

    String aPath( IDE_DLL()->GetExtraData()->GetAddLibPath() );
    if ( aPath.Len() )
    {
        xFP->setDisplayDirectory( aPath );
        xFltMgr->setCurrentFilter( IDE_DLL()->GetExtraData()->GetAddLibFilter() );
    }
    else
    {
        // macro path from configuration management
        aPath = SvtPathOptions().GetWorkPath();
        INetURLObject aFileURL( aPath , INetURLObject::FSYS_DETECT );
        aFileURL.setFinalSlash();
        aPath = aFileURL.getFSysPath( INetURLObject::FSYS_DETECT );
        xFP->setDisplayDirectory( aPath );
        xFltMgr->setCurrentFilter( String( IDEResId( RID_STR_DOC ) ) );
    }

    if( xFP->execute() == RET_OK )
    {
        Sequence< ::rtl::OUString > aPaths = xFP->getFiles();
        aPath = aPaths[0];
        IDE_DLL()->GetExtraData()->SetAddLibPath( aPath );
        IDE_DLL()->GetExtraData()->SetAddLibFilter( xFltMgr->getCurrentFilter() );

        INetURLObject aFileURL( aPath );
        String aFullName( aFileURL.getFSysPath( INetURLObject::FSYS_DETECT ) );

        if ( SvStorage::IsStorageFile( aFullName ) )
        {
            SvStorageRef xStorage = new SvStorage( aFullName, STREAM_READ | STREAM_SHARE_DENYWRITE );
            if ( xStorage->GetError() )
                ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_ERROROPENSTORAGE ) ) ).Execute();
            else
            {
                // Die einzelnen Libs aus dem BasicStorage...
                SvStorageRef xBasicStorage = xStorage->OpenStorage( String( RTL_CONSTASCII_USTRINGPARAM( "StarBASIC" ) ), STREAM_READ | STREAM_SHARE_DENYWRITE, 0 );
                if ( xBasicStorage->GetError() )
                    InfoBox( this, String( IDEResId( RID_STR_NOLIBINSTORAGE ) ) ).Execute();
                else
                {
                    LibDialog* pLibDlg = 0;
                    SvStorageInfoList aInfoList( 4, 4 );
                    xBasicStorage->FillInfoList( &aInfoList );
                    for ( USHORT nStream = 0; nStream < aInfoList.Count(); nStream++ )
                    {
                        SvStorageInfo& rInf = aInfoList[nStream];
                        if ( rInf.IsStream() )
                        {
                            if ( !pLibDlg )
                            {
                                pLibDlg = new LibDialog( this );
                                pLibDlg->SetStorageName( aFileURL.getName() );
                                pLibDlg->GetLibBox().SetMode( LIBMODE_CHOOSER );
                                if ( pBasMgr == SFX_APP()->GetBasicManager() )
                                    pLibDlg->SetSeparateFileEnabled( TRUE );
                            }
                            SvLBoxEntry* pEntry = pLibDlg->GetLibBox().InsertEntry( rInf.GetName() );
                            USHORT nPos = (USHORT) pLibDlg->GetLibBox().GetModel()->GetAbsPos( pEntry );
                            //pLibDlg->GetLibBox().CheckEntryPos( nPos, TRUE);

                        }
                    }
                    xBasicStorage.Clear();
                    if ( !pLibDlg )
                        InfoBox( this, String( IDEResId( RID_STR_NOLIBINSTORAGE ) ) ).Execute();
                    else
                    {
                        BOOL bChanges = FALSE;
                        if ( pLibDlg->Execute() )
                        {
                            ULONG nNewPos = aLibBox.GetEntryCount();
//                          ULONG nCurPos = aLibBox.GetAbsPos( aLibBox.GetCurEntry() );
                            BOOL bReplace = pLibDlg->IsReplace();
                            BOOL bReference = pLibDlg->IsReference();
                            for ( USHORT nLib = 0; nLib < pLibDlg->GetLibBox().GetEntryCount(); nLib++ )
                            {
                                //if ( pLibDlg->GetLibBox().IsChecked( nLib ) )
                                //{
                                    SvLBoxEntry* pEntry = pLibDlg->GetLibBox().GetEntry( nLib );
                                    DBG_ASSERT( pEntry, "Entry?!" );
                                    String aName( pLibDlg->GetLibBox().GetEntryText( pEntry, 0 ) );
                                    // Optionen auswerten...
                                    if ( pBasMgr->HasLib( aName ) )
                                    {
                                        // Die Standard-Lib kann nicht ersetzt werden,
                                        // weil Basics verkettet.
                                        if ( bReplace && ( pBasMgr->GetLib( aName ) == pBasMgr->GetStdLib() ) )
                                        {
                                            ErrorBox( this, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_REPLACESTDLIB ) ) ).Execute();
                                            continue;
                                        }
                                        if ( bReplace )
                                        {
                                            SvLBoxEntry* pEntry = aLibBox.FindEntry( aName );
                                            if ( pEntry )
                                                aLibBox.SvTreeListBox::GetModel()->Remove( pEntry );
                                            pBasMgr->RemoveLib( pBasMgr->GetLibId( aName ) );
                                        }
                                        else if ( bReference )
                                        {
                                            // Referenz nicht moeglich, wenn Lib mit
                                            // Namen schon existiert, ausser bei Replace.
                                            String aErrStr( IDEResId( RID_STR_REFNOTPOSSIBLE ) );
                                            aErrStr.SearchAndReplace( String( RTL_CONSTASCII_USTRINGPARAM( "XX" ) ), aName );
                                            aErrStr += '\n';
                                            aErrStr += String( IDEResId( RID_STR_SBXNAMEALLREADYUSED ) );
                                            ErrorBox( this, WB_OK | WB_DEF_OK, aErrStr ).Execute();
                                            continue;
                                        }
                                    }
                                    StarBASIC* pNew = pBasMgr->AddLib( *xStorage, aName, bReference );
                                    if ( !pNew )
                                    {
                                        String aErrStr( IDEResId( RID_STR_ERROROPENLIB ) );
                                        ErrorBox( this, WB_OK | WB_DEF_OK, aErrStr ).Execute();
                                        continue;
                                    }
                                    USHORT nLib = pBasMgr->GetLibId( pNew->GetName() );
                                    if ( pLibDlg->IsSeparateFile() )
                                    {
                                        INetURLObject aFileURL( pBasMgr->GetStorageName() , INetURLObject::FSYS_DETECT );
                                        String aExt = aFileURL.getExtension();
                                        aFileURL.setName( pBasMgr->GetLibName( nLib ) );
                                        aFileURL.setExtension( aExt );
                                        pBasMgr->SetLibStorageName( nLib, aFileURL.getFSysPath( INetURLObject::FSYS_DETECT ) );
                                    }
                                    DBG_ASSERT( nLib != LIB_NOTFOUND, "Lib nicht eingefuegt?!" );
                                    ImpInsertLibEntry( pBasMgr->GetLibName( nLib ), nLib );
                                    bChanges = TRUE;
                                //}
                            }
                            SvLBoxEntry* pFirstNew = aLibBox.GetEntry( nNewPos );
                            if ( pFirstNew )
                                aLibBox.SetCurEntry( pFirstNew );
                        }
                        delete pLibDlg;
                        if ( bChanges )
                            BasicIDE::MarkDocShellModified( pBasMgr->GetStdLib() );
                    }
                }
            }
        }
        else
        {
            // Erstmal nur in Storages moeglich, wie soll ein Basic in eine
            // Datei kommen?
            InfoBox( this, String( IDEResId( RID_STR_NOLIBINSTORAGE ) ) ).Execute();
        }
    }
}

//----------------------------------------------------------------------------

void LibPage::DeleteCurrent()
{
    BasicManager* pBasMgr = BasicIDE::FindBasicManager( aCurBasMgr );
    DBG_ASSERT( pBasMgr, "BasMgr?!" );
    SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
    SvLBoxEntry* pCurEntry = aLibBox.GetCurEntry();
    DBG_ASSERT( pCurEntry && aLibBox.GetModel()->GetAbsPos( pCurEntry ), "Kann nicht loeschen!" );
    String aLibName( aLibBox.GetEntryText( pCurEntry, 0 ) );

    // check, if library is link
    BOOL bIsLibraryLink = FALSE;
    ::rtl::OUString aOULibName( aLibName );
    Reference< script::XLibraryContainer2 > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
    Reference< script::XLibraryContainer2 > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pShell ), UNO_QUERY );
    if ( ( xModLibContainer.is() && xModLibContainer->isLibraryLink( aOULibName ) ) ||
         ( xDlgLibContainer.is() && xDlgLibContainer->isLibraryLink( aOULibName ) ) )
    {
        bIsLibraryLink = TRUE;
    }

    if ( QueryDelLib( aLibName, bIsLibraryLink, this ) )
    {
        // inform BasicIDE
        String aLib( CreateMgrAndLibStr( aCurBasMgr, aLibName ) );
        SfxStringItem aLibItem( SID_BASICIDE_ARG_LIBNAME, aLib );
        BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if( pDispatcher )
        {
            pDispatcher->Execute( SID_BASICIDE_LIBREMOVED,
                                  SFX_CALLMODE_SYNCHRON, &aLibItem, 0L );
        }

        // remove library from module and dialog library containers
        if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
            xModLibContainer->removeLibrary( aOULibName );
        if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) )
            xDlgLibContainer->removeLibrary( aOULibName );

        ((SvLBox&)aLibBox).GetModel()->Remove( pCurEntry );
        BasicIDE::MarkDocShellModified( pShell );
    }
}

//----------------------------------------------------------------------------

void LibPage::EndTabDialog( USHORT nRet )
{
    DBG_ASSERT( pTabDlg, "TabDlg nicht gesetzt!" );
    if ( pTabDlg )
        pTabDlg->EndDialog( nRet );
}

//----------------------------------------------------------------------------

void LibPage::FillListBox()
{
    BasicManager* pBasicMgr = SFX_APP()->GetBasicManager();
    SfxObjectShell* pDocShell = 0;
    while ( pBasicMgr )
    {
        // Nur, wenn es ein dazugehoeriges Fenster gibt, damit nicht die
        // Gecachten Docs, die nicht sichtbar sind ( Remot-Dokumente )
        if ( !pDocShell || ( ( !pDocShell->ISA(BasicDocShell) ) && ( pBasicMgr != SFX_APP()->GetBasicManager() )
                                && ( SfxViewFrame::GetFirst( pDocShell ) ) ) )
        {
            String aBasMgr;
            if (pDocShell)
                aBasMgr = pDocShell->GetTitle( SFX_TITLE_FILENAME );
            else
                aBasMgr = Application::GetAppName();

            aBasicsBox.InsertEntry( aBasMgr, LISTBOX_APPEND );
        }

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }
}

//----------------------------------------------------------------------------

void LibPage::SetCurLib()
{
    String aSelected( aBasicsBox.GetSelectEntry() );
    if ( aSelected != aCurBasMgr )
    {
        //ActivateCurrentLibSettings();
        aCurBasMgr = aSelected;
        BasicManager* pBasMgr = BasicIDE::FindBasicManager( aCurBasMgr );
        DBG_ASSERT( pBasMgr, "BasMgr?!" );
        SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
        aLibBox.SetBasicManager( pBasMgr );
        aLibBox.Clear();
        //Image aLockedImg( IDEResId( RID_IMG_LOCKED ) );

        // get a sorted list of library names
        Sequence< ::rtl::OUString > aLibNames = BasicIDE::GetLibraryNames( pShell );
        sal_Int32 nLibCount = aLibNames.getLength();
        const ::rtl::OUString* pLibNames = aLibNames.getConstArray();

        for ( sal_Int32 i = 0 ; i < nLibCount ; i++ )
            ImpInsertLibEntry( pLibNames[ i ], i );

        SvLBoxEntry* pEntry = aLibBox.FindEntry( String::CreateFromAscii("Standard") );
        if ( !pEntry )
            pEntry = aLibBox.GetEntry( 0 );
        aLibBox.SetCurEntry( pEntry );
        //aLibBox.MakeVisible( pEntry );
    }
}

//----------------------------------------------------------------------------

/*
void LibPage::ActivateCurrentLibSettings()
{
    BasicManager* pBasMgr = BasicIDE::FindBasicManager( aCurBasMgr );
    if ( pBasMgr )  // Beim ersten mal 0
    {
        USHORT nLibs = (USHORT) aLibBox.GetEntryCount();
        // StandardLib kann nicht geaendert werden...
        for ( USHORT nLib = 1; nLib < nLibs; nLib++ )
        {
            BOOL bChecked = aLibBox.IsChecked( nLib );
            if ( bChecked != pBasMgr->IsLibLoaded( nLib ) )
            {
                String aLibName( pBasMgr->GetLibName( nLib ) );
                String aLib( CreateMgrAndLibStr( aCurBasMgr, aLibName ) );
                SfxStringItem aLibItem( SID_BASICIDE_ARG_LIBNAME, aLib );

                BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
                SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
                SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
                if ( bChecked )
                {
                    pBasMgr->LoadLib( nLib );
                    if( pDispatcher )
                    {
                        pDispatcher->Execute( SID_BASICIDE_LIBLOADED,
                            SFX_CALLMODE_ASYNCHRON, &aLibItem, 0L );
                    }
                }
                else
                {
                    // Vorher aufrufen, damit noch im BasMgr
                    if( pDispatcher )
                    {
                        pDispatcher->Execute( SID_BASICIDE_LIBREMOVED,
                            SFX_CALLMODE_SYNCHRON, &aLibItem, 0L );
                    }
                    pBasMgr->StoreLib( nLib );
                    pBasMgr->UnloadLib( nLib );
                }
            }
        }
    }
}
*/

//----------------------------------------------------------------------------

SvLBoxEntry* LibPage::ImpInsertLibEntry( const String& rLibName, ULONG nPos )
{
    // check, if library is password protected
    BOOL bProtected = FALSE;
    SfxObjectShell* pShell = BasicIDE::FindDocShell( aLibBox.GetBasicManager() );
    ::rtl::OUString aOULibName( rLibName );
    Reference< script::XLibraryContainer2 > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
    if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
    {
        Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
        if ( xPasswd.is() )
        {
            bProtected = xPasswd->isLibraryPasswordProtected( aOULibName );
        }
    }

    if ( bProtected )
    {
        Image aImg = Image( IDEResId( RID_IMG_LOCKED ) );
        Size aSz = aImg.GetSizePixel();
        aLibBox.SetDefaultExpandedEntryBmp( aImg );
        aLibBox.SetDefaultCollapsedEntryBmp( aImg );
    }

    SvLBoxEntry* pNewEntry = aLibBox.InsertEntry( rLibName, nPos );
    pNewEntry->SetUserData( new BasicLibUserData( pShell ) );

    // check, if library is link
    if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryLink( aOULibName ) )
    {
        String aLinkURL = xModLibContainer->getLibraryLinkURL( aOULibName );
        aLibBox.SetEntryText( aLinkURL, pNewEntry, 1 );
    }

    /* old code
    if ( pBasicManager->IsReference( nLib ) || pBasicManager->IsExtern( nLib ) )
    {
        String aLibStorage = pBasicManager->GetLibStorageName( nLib );
        if ( pBasicManager->GetStorageName().Len() )
        {
            INetURLObject aFileURL( pBasicManager->GetStorageName() , INetURLObject::FSYS_DETECT );
            aFileURL.removeSegment();
            aFileURL.removeFinalSlash();
            String aPath = aFileURL.getFSysPath( INetURLObject::FSYS_DETECT );

            if ( aLibStorage.CompareIgnoreCaseToAscii( aPath, aPath.Len() ) == COMPARE_EQUAL )
                aLibStorage.Erase( 0, aPath.Len()+1 );  // Dann ohne Pfadangabe...
            else if ( pBasicManager->GetRelLibStorageName( nLib ).Len() )
                aLibStorage = pBasicManager->GetRelLibStorageName( nLib ); // Kuerzer...
        }

        aLibBox.SetEntryText( aLibStorage, pNewEntry, 1 );
    }
    */

    //aLibBox.CheckEntryPos( nLib, pBasicManager->IsLibLoaded( nLib ) );

    if ( bProtected )
    {
        Image aImg; // Default zuruecksetzen
        aLibBox.SetDefaultExpandedEntryBmp( aImg );
        aLibBox.SetDefaultCollapsedEntryBmp( aImg );
    }

    return pNewEntry;
}

//----------------------------------------------------------------------------
