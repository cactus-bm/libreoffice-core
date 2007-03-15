/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: baside3.cxx,v $
 *
 *  $Revision: 1.36 $
 *
 *  last change: $Author: obo $ $Date: 2007-03-15 15:53:05 $
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
#include "precompiled_basctl.hxx"

//svdraw.hxx
//#define _SVDRAW_HXX ***
#define _SDR_NOITEMS
#define _SDR_NOTOUCH
#define _SDR_NOTRANSFORM
#define _SDR_NOOBJECTS
//#define _SDR_NOVIEWS ***
#define _SDR_NOVIEWMARKER
#define _SDR_NODRAGMETHODS
//#define _SDR_NOUNDO ***
#define _SDR_NOXOUTDEV

#include <ide_pch.hxx>


#include <basidesh.hrc>
#include <baside3.hxx>

#ifndef _BASCTL_ACCESSIBLEDIALOGWINDOW_HXX_
#include <accessibledialogwindow.hxx>
#endif
#ifndef _BASCTL_DLGED_HXX
#include <dlged.hxx>
#endif
#ifndef _BASCTL_DLGEDMOD_HXX
#include <dlgedmod.hxx>
#endif
#ifndef _BASCTL_DLGEDVIEW_HXX
#include <dlgedview.hxx>
#endif
#ifndef _BASCTL_DLGEDDEF_HXX
#include <dlgeddef.hxx>
#endif
#ifndef _BASCTL_PROPBRW_HXX
#include <propbrw.hxx>
#endif

#include <basobj.hxx>
#include <iderdll.hxx>
#include <basidesh.hxx>
#include <idetemp.hxx>
#include <helpid.hrc>
#include <bastype2.hxx>

#ifndef _SVDVIEW_HXX
#include <svx/svdview.hxx>
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _XMLSCRIPT_XMLDLG_IMEXP_HXX_
#include <xmlscript/xmldlg_imexp.hxx>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRYARYCONTAINER2_HPP_
#include <com/sun/star/script/XLibraryContainer2.hpp>
#endif


using namespace comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::io;


TYPEINIT1( DialogWindow, IDEBaseWindow );

DialogWindow::DialogWindow( Window* pParent, const ScriptDocument& rDocument, String aLibName, String aName,
    const com::sun::star::uno::Reference< com::sun::star::container::XNameContainer >& xDialogModel )
        :IDEBaseWindow( pParent, rDocument, aLibName, aName )
        ,pUndoMgr(NULL)
{
    InitSettings( TRUE, TRUE, TRUE );

    pEditor = new DlgEditor();
    pEditor->SetWindow( this );
    pEditor->SetDialog( xDialogModel );

    // Undo einrichten
    pUndoMgr = new SfxUndoManager;

    Link aDummyLink;
    aOldNotifyUndoActionHdl = pEditor->GetModel()->GetNotifyUndoActionHdl();
    pEditor->GetModel()->SetNotifyUndoActionHdl(
        LINK(this, DialogWindow, NotifyUndoActionHdl));

    SetHelpId( HID_BASICIDE_DIALOGWINDOW );

    // set readonly mode for readonly libraries
    ::rtl::OUString aOULibName( aLibName );
    Reference< script::XLibraryContainer2 > xDlgLibContainer( GetDocument().getLibraryContainer( E_DIALOGS ), UNO_QUERY );
    if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) && xDlgLibContainer->isLibraryReadOnly( aOULibName ) )
        SetReadOnly( TRUE );

    if ( rDocument.isDocument() && rDocument.isReadOnly() )
        SetReadOnly( TRUE );
}

DialogWindow::~DialogWindow()
{
    delete pEditor;
    delete pUndoMgr;
}

void DialogWindow::LoseFocus()
{
    if ( IsModified() )
        StoreData();

    Window::LoseFocus();
}



void DialogWindow::Paint( const Rectangle& rRect )
{
    pEditor->Paint( rRect );
}



void DialogWindow::Resize()
{
    if ( GetHScrollBar() && GetVScrollBar() ) {
        pEditor->SetScrollBars( GetHScrollBar(), GetVScrollBar() );
    }
}



void DialogWindow::MouseButtonDown( const MouseEvent& rMEvt )
{
    pEditor->MouseButtonDown( rMEvt );

    SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
    if ( pBindings )
        pBindings->Invalidate( SID_SHOW_PROPERTYBROWSER );
}



void DialogWindow::MouseButtonUp( const MouseEvent& rMEvt )
{
    pEditor->MouseButtonUp( rMEvt );
    SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
    if( (pEditor->GetMode() == DLGED_INSERT) && !pEditor->IsCreateOK() )
    {
        pEditor->SetMode( DLGED_SELECT );
        if ( pBindings )
            pBindings->Invalidate( SID_CHOOSE_CONTROLS );
    }
    if ( pBindings )
    {
        pBindings->Invalidate( SID_SHOW_PROPERTYBROWSER );
        pBindings->Invalidate( SID_DOC_MODIFIED );
        pBindings->Invalidate( SID_SAVEDOC );
    }
}



void DialogWindow::MouseMove( const MouseEvent& rMEvt )
{
    pEditor->MouseMove( rMEvt );
}



void DialogWindow::KeyInput( const KeyEvent& rKEvt )
{
    if( rKEvt.GetKeyCode() == KEY_BACKSPACE )
    {
        BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if( pDispatcher )
        {
            pDispatcher->Execute( SID_BACKSPACE );
        }
    }
    else
    {
        if( !pEditor->KeyInput( rKEvt ) )
        {
            if( !SfxViewShell::Current()->KeyInput( rKEvt ) )
                Window::KeyInput( rKEvt );
        }
    }
}

void DialogWindow::Command( const CommandEvent& rCEvt )
{
    if ( ( rCEvt.GetCommand() == COMMAND_WHEEL ) ||
            ( rCEvt.GetCommand() == COMMAND_STARTAUTOSCROLL ) ||
            ( rCEvt.GetCommand() == COMMAND_AUTOSCROLL ) )
    {
        HandleScrollCommand( rCEvt, GetHScrollBar(), GetVScrollBar() );
    }
    else if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU )
    {
        BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if ( pDispatcher )
        {
            SdrView* pView = GetView();
            if( !rCEvt.IsMouseEvent() && pView->AreObjectsMarked() )
            {
                Rectangle aMarkedRect( pView->GetMarkedRect() );
                Point MarkedCenter( aMarkedRect.Center() );
                Point PosPixel( LogicToPixel( MarkedCenter ) );
                pDispatcher->ExecutePopup( IDEResId(RID_POPUP_DLGED), this, &PosPixel );
            }
            else
            {
                pDispatcher->ExecutePopup( IDEResId(RID_POPUP_DLGED) );
            }

        }
    }
    else
        IDEBaseWindow::Command( rCEvt );
}




IMPL_LINK( DialogWindow, NotifyUndoActionHdl, SfxUndoAction *, pUndoAction )
{
    (void)pUndoAction;

    // not working yet for unocontrols
    /*
    if (pUndoAction)
    {
        pUndoMgr->AddUndoAction( pUndoAction );
        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
            pBindings->Invalidate( SID_UNDO );
    }
    */

    return 0;
}



void __EXPORT DialogWindow::DoInit()
{
    GetHScrollBar()->Show();
    GetVScrollBar()->Show();
    pEditor->SetScrollBars( GetHScrollBar(), GetVScrollBar() );
}



void __EXPORT DialogWindow::DoScroll( ScrollBar* pCurScrollBar )
{
    pEditor->DoScroll( pCurScrollBar );
}

void __EXPORT DialogWindow::GetState( SfxItemSet& rSet )
{
    SfxWhichIter aIter(rSet);
    for ( USHORT nWh = aIter.FirstWhich(); 0 != nWh; nWh = aIter.NextWhich() )
    {
        switch ( nWh )
        {
            case SID_PASTE:
            {
                if ( !IsPasteAllowed() )
                    rSet.DisableItem( nWh );

                if ( IsReadOnly() )
                    rSet.DisableItem( nWh );
            }
            break;
            case SID_COPY:
            {
                // any object selected?
                if ( !pEditor->GetView()->AreObjectsMarked() )
                    rSet.DisableItem( nWh );
            }
            break;
            case SID_CUT:
            case SID_DELETE:
            case SID_BACKSPACE:
            {
                // any object selected?
                if ( !pEditor->GetView()->AreObjectsMarked() )
                    rSet.DisableItem( nWh );

                if ( IsReadOnly() )
                    rSet.DisableItem( nWh );
            }
            break;
            case SID_REDO:
            {
                if ( !pUndoMgr->GetUndoActionCount() )
                    rSet.DisableItem( nWh );
            }
            break;

            // Nur Dialogfenster:
            case SID_DIALOG_TESTMODE:
            {
                // ist die IDE noch aktiv?
                if( IDE_DLL()->GetShell()->GetFrame() )
                {
                    rSet.Put( SfxBoolItem( SID_DIALOG_TESTMODE,
                              (pEditor->GetMode() == DLGED_TEST) ? TRUE : FALSE) );
                }
                else
                    rSet.Put( SfxBoolItem( SID_DIALOG_TESTMODE,FALSE ));
            }
            break;

            case SID_CHOOSE_CONTROLS:
            {
                if ( IsReadOnly() )
                {
                    rSet.DisableItem( nWh );
                }
                else
                {
                    SfxAllEnumItem aItem( SID_CHOOSE_CONTROLS );
                    if ( GetEditor()->GetMode() == DLGED_SELECT )
                        aItem.SetValue( SVX_SNAP_SELECT );
                    else
                    {
                        USHORT nObj;
                        switch( pEditor->GetInsertObj() )
                        {
                            case OBJ_DLG_PUSHBUTTON:        nObj = SVX_SNAP_PUSHBUTTON; break;
                            case OBJ_DLG_RADIOBUTTON:       nObj = SVX_SNAP_RADIOBUTTON; break;
                            case OBJ_DLG_CHECKBOX:          nObj = SVX_SNAP_CHECKBOX; break;
                            case OBJ_DLG_LISTBOX:           nObj = SVX_SNAP_LISTBOX; break;
                            case OBJ_DLG_COMBOBOX:          nObj = SVX_SNAP_COMBOBOX; break;
                            case OBJ_DLG_GROUPBOX:          nObj = SVX_SNAP_GROUPBOX; break;
                            case OBJ_DLG_EDIT:              nObj = SVX_SNAP_EDIT; break;
                            case OBJ_DLG_FIXEDTEXT:         nObj = SVX_SNAP_FIXEDTEXT; break;
                            case OBJ_DLG_IMAGECONTROL:      nObj = SVX_SNAP_IMAGECONTROL; break;
                            case OBJ_DLG_PROGRESSBAR:       nObj = SVX_SNAP_PROGRESSBAR; break;
                            case OBJ_DLG_HSCROLLBAR:        nObj = SVX_SNAP_HSCROLLBAR; break;
                            case OBJ_DLG_VSCROLLBAR:        nObj = SVX_SNAP_VSCROLLBAR; break;
                            case OBJ_DLG_HFIXEDLINE:        nObj = SVX_SNAP_HFIXEDLINE; break;
                            case OBJ_DLG_VFIXEDLINE:        nObj = SVX_SNAP_VFIXEDLINE; break;
                            case OBJ_DLG_DATEFIELD:         nObj = SVX_SNAP_DATEFIELD; break;
                            case OBJ_DLG_TIMEFIELD:         nObj = SVX_SNAP_TIMEFIELD; break;
                            case OBJ_DLG_NUMERICFIELD:      nObj = SVX_SNAP_NUMERICFIELD; break;
                            case OBJ_DLG_CURRENCYFIELD:     nObj = SVX_SNAP_CURRENCYFIELD; break;
                            case OBJ_DLG_FORMATTEDFIELD:    nObj = SVX_SNAP_FORMATTEDFIELD; break;
                            case OBJ_DLG_PATTERNFIELD:      nObj = SVX_SNAP_PATTERNFIELD; break;
                            case OBJ_DLG_FILECONTROL:       nObj = SVX_SNAP_FILECONTROL; break;
                            default:                        nObj = 0;
                        }
#ifdef DBG_UTIL
                        if( !nObj )
                        {
                            DBG_WARNING( "SID_CHOOSE_CONTROLS: Unbekannt!" );
                        }
#endif
                        aItem.SetValue( nObj );
                    }

                    rSet.Put( aItem );
                }
            }
            break;

            case SID_SHOW_PROPERTYBROWSER:
            {
                BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
                SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
                if ( pViewFrame && !pViewFrame->HasChildWindow( SID_SHOW_PROPERTYBROWSER ) && !pEditor->GetView()->AreObjectsMarked() )
                    rSet.DisableItem( nWh );

                if ( IsReadOnly() )
                    rSet.DisableItem( nWh );
            }
            break;
        }
    }
}



void __EXPORT DialogWindow::ExecuteCommand( SfxRequest& rReq )
{
    switch ( rReq.GetSlot() )
    {
        case SID_CUT:
            if ( !IsReadOnly() )
            {
                GetEditor()->Cut();
                SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
                if ( pBindings )
                    pBindings->Invalidate( SID_DOC_MODIFIED );
            }
            break;
        case SID_DELETE:
#ifdef MAC
        case SID_BACKSPACE:
#endif
            if ( !IsReadOnly() )
            {
                GetEditor()->Delete();
                SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
                if ( pBindings )
                    pBindings->Invalidate( SID_DOC_MODIFIED );
            }
            break;
        case SID_COPY:
            GetEditor()->Copy();
            break;
        case SID_PASTE:
            if ( !IsReadOnly() )
            {
                GetEditor()->Paste();
                SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
                if ( pBindings )
                    pBindings->Invalidate( SID_DOC_MODIFIED );
            }
            break;
        case SID_CHOOSE_CONTROLS:
        {
            const SfxItemSet* pArgs = rReq.GetArgs();
            DBG_ASSERT( pArgs, "Nix Args" );

            const SfxAllEnumItem& rItem = (SfxAllEnumItem&)pArgs->Get( SID_CHOOSE_CONTROLS );
            switch( rItem.GetValue() )
            {
                case SVX_SNAP_PUSHBUTTON:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_PUSHBUTTON );
                }
                break;
                case SVX_SNAP_RADIOBUTTON:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_RADIOBUTTON );
                }
                break;
                case SVX_SNAP_CHECKBOX:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_CHECKBOX);
                }
                break;
                case SVX_SNAP_LISTBOX:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_LISTBOX );
                }
                break;
                case SVX_SNAP_COMBOBOX:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_COMBOBOX );
                }
                break;
                case SVX_SNAP_GROUPBOX:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_GROUPBOX );
                }
                break;
                case SVX_SNAP_EDIT:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_EDIT );
                }
                break;
                case SVX_SNAP_FIXEDTEXT:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_FIXEDTEXT );
                }
                break;
                case SVX_SNAP_IMAGECONTROL:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_IMAGECONTROL );
                }
                break;
                case SVX_SNAP_PROGRESSBAR:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_PROGRESSBAR );
                }
                break;
                case SVX_SNAP_HSCROLLBAR:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_HSCROLLBAR );
                }
                break;
                case SVX_SNAP_VSCROLLBAR:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_VSCROLLBAR );
                }
                break;
                case SVX_SNAP_HFIXEDLINE:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_HFIXEDLINE );
                }
                break;
                case SVX_SNAP_VFIXEDLINE:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_VFIXEDLINE );
                }
                break;
                case SVX_SNAP_DATEFIELD:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_DATEFIELD );
                }
                break;
                case SVX_SNAP_TIMEFIELD:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_TIMEFIELD );
                }
                break;
                case SVX_SNAP_NUMERICFIELD:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_NUMERICFIELD );
                }
                break;
                case SVX_SNAP_CURRENCYFIELD:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_CURRENCYFIELD );
                }
                break;
                case SVX_SNAP_FORMATTEDFIELD:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_FORMATTEDFIELD );
                }
                break;
                case SVX_SNAP_PATTERNFIELD:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_PATTERNFIELD );
                }
                break;
                case SVX_SNAP_FILECONTROL:
                {
                    GetEditor()->SetMode( DLGED_INSERT );
                    GetEditor()->SetInsertObj( OBJ_DLG_FILECONTROL );
                }
                break;
                case SVX_SNAP_SELECT:
                {
                    GetEditor()->SetMode( DLGED_SELECT );
                }
                break;
            }

            if ( rReq.GetModifier() & KEY_MOD1 )
            {
                if ( GetEditor()->GetMode() == DLGED_INSERT )
                    GetEditor()->CreateDefaultObject();
            }

            SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
            if ( pBindings )
                pBindings->Invalidate( SID_DOC_MODIFIED );
        }
        break;

        case SID_DIALOG_TESTMODE:
        {
            DlgEdMode eOldMode = GetEditor()->GetMode();
            GetEditor()->SetMode( DLGED_TEST );
            GetEditor()->SetMode( eOldMode );
            rReq.Done();
            SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
            if ( pBindings )
                pBindings->Invalidate( SID_DIALOG_TESTMODE );
            return;
        }
    }

    rReq.Done();
}

Reference< container::XNameContainer > DialogWindow::GetDialog() const
{
    return pEditor->GetDialog();
}

BOOL DialogWindow::RenameDialog( const String& rNewName )
{
    if ( !BasicIDE::RenameDialog( this, GetDocument(), GetLibName(), GetName(), rNewName ) )
        return FALSE;

    SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
    if ( pBindings )
        pBindings->Invalidate( SID_DOC_MODIFIED );

    return TRUE;
}

void DialogWindow::DisableBrowser()
{
    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
    SfxChildWindow* pChildWin = pViewFrame ? pViewFrame->GetChildWindow(SID_SHOW_PROPERTYBROWSER) : NULL;
    if( pChildWin )
        ((PropBrw*)(pChildWin->GetWindow()))->Update( 0 );
}

void DialogWindow::UpdateBrowser()
{
    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
    SfxChildWindow* pChildWin = pViewFrame ? pViewFrame->GetChildWindow(SID_SHOW_PROPERTYBROWSER) : NULL;
    if( pChildWin )
        ((PropBrw*)(pChildWin->GetWindow()))->Update(GetEditor()->GetView());
}

DlgEdModel* DialogWindow::GetModel() const
{
    return pEditor ? pEditor->GetModel() : NULL;
}

DlgEdPage* DialogWindow::GetPage() const
{
    return pEditor ? pEditor->GetPage() : NULL;
}

DlgEdView* DialogWindow::GetView() const
{
    return pEditor ? pEditor->GetView() : NULL;
}

BOOL __EXPORT DialogWindow::IsModified()
{
    return pEditor->IsModified();
}

SfxUndoManager* __EXPORT DialogWindow::GetUndoManager()
{
    return pUndoMgr;
}

String DialogWindow::GetTitle()
{
    return GetName();
}

BasicEntryDescriptor DialogWindow::CreateEntryDescriptor()
{
    ScriptDocument aDocument( GetDocument() );
    String aLibName( GetLibName() );
    LibraryLocation eLocation = aDocument.getLibraryLocation( aLibName );
    return BasicEntryDescriptor( aDocument, eLocation, aLibName, GetName(), OBJ_TYPE_DIALOG );
}

void DialogWindow::SetReadOnly( BOOL b )
{
    if ( pEditor )
    {
        if ( b )
            pEditor->SetMode( DLGED_READONLY );
        else
            pEditor->SetMode( DLGED_SELECT );
    }
}

BOOL DialogWindow::IsReadOnly()
{
    BOOL bReadOnly = FALSE;

    if ( pEditor && pEditor->GetMode() == DLGED_READONLY )
        bReadOnly = TRUE;

    return bReadOnly;
}

BOOL DialogWindow::IsPasteAllowed()
{
    return pEditor ? pEditor->IsPasteAllowed() : FALSE;
}

void DialogWindow::StoreData()
{
    if ( IsModified() )
    {
        try
        {
            Reference< container::XNameContainer > xLib = GetDocument().getLibrary( E_DIALOGS, GetLibName(), true );

            if( xLib.is() )
            {
                Reference< container::XNameContainer > xDialogModel = pEditor->GetDialog();

                if( xDialogModel.is() )
                {
                    Reference< XComponentContext > xContext;
                    Reference< beans::XPropertySet > xProps( ::comphelper::getProcessServiceFactory(), UNO_QUERY );
                    OSL_ASSERT( xProps.is() );
                    OSL_VERIFY( xProps->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DefaultContext")) ) >>= xContext );
                    Reference< XInputStreamProvider > xISP = ::xmlscript::exportDialogModel( xDialogModel, xContext );
                    xLib->replaceByName( ::rtl::OUString( GetName() ), makeAny( xISP ) );
                }
            }
        }
        catch ( uno::Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        BasicIDE::MarkDocumentModified( GetDocument() );
        pEditor->ClearModifyFlag();
    }
}

void DialogWindow::Deactivating()
{
    if ( IsModified() )
        BasicIDE::MarkDocumentModified( GetDocument() );
}

void DialogWindow::PrintData( Printer* pPrinter )
{
    pEditor->PrintData( pPrinter, CreateQualifiedName() );
}

void DialogWindow::DataChanged( const DataChangedEvent& rDCEvt )
{
    if( (rDCEvt.GetType()==DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        InitSettings( TRUE, TRUE, TRUE );
        Invalidate();
    }
    else
        IDEBaseWindow::DataChanged( rDCEvt );
}

void DialogWindow::InitSettings(BOOL bFont,BOOL bForeground,BOOL bBackground)
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    if( bFont )
    {
        Font aFont;
        aFont = rStyleSettings.GetFieldFont();
        SetPointFont( aFont );
    }

    if( bForeground || bFont )
    {
        SetTextColor( rStyleSettings.GetFieldTextColor() );
        SetTextFillColor();
    }

    if( bBackground )
        SetBackground( rStyleSettings.GetFieldColor() );
}

::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > DialogWindow::CreateAccessible()
{
    return (::com::sun::star::accessibility::XAccessible*) new AccessibleDialogWindow( this );
}
