/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: moduldlg.hxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 20:07:59 $
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

#ifndef _MODULDLG_HXX
#define _MODULDLG_HXX

#ifndef _SVHEADER_HXX
#include <svheader.hxx>
#endif

#include <bastype2.hxx>

#ifndef _SV_DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SVTABBX_HXX //autogen
#include <svtools/svtabbx.hxx>
#endif

#ifndef _SV_TABDLG_HXX //autogen
#include <vcl/tabdlg.hxx>
#endif

#ifndef _SV_TABPAGE_HXX //autogen
#include <vcl/tabpage.hxx>
#endif


#include <vcl/tabctrl.hxx>
#include <vcl/lstbox.hxx>

class StarBASIC;


#define NEWOBJECTMODE_LIB       1
#define NEWOBJECTMODE_MOD       2
#define NEWOBJECTMODE_DLG       3
#define NEWOBJECTMODE_METH      4

class NewObjectDialog : public ModalDialog
{
private:
    FixedText       aText;
    Edit            aEdit;
    OKButton        aOKButton;
    CancelButton    aCancelButton;

    DECL_LINK(OkButtonHandler, Button *);

public:
    NewObjectDialog(Window * pParent, USHORT nMode, bool bCheckName = false);
                ~NewObjectDialog();

    String      GetObjectName() const { return aEdit.GetText(); }
    void        SetObjectName( const String& rName ) { aEdit.SetText( rName ); aEdit.SetSelection( Selection( 0, rName.Len() ) );}
};


class ExtBasicTreeListBox : public BasicTreeListBox
{
protected:
    virtual BOOL    EditingEntry( SvLBoxEntry* pEntry, Selection& rSel  );
    virtual BOOL    EditedEntry( SvLBoxEntry* pEntry, const String& rNewText );

    virtual DragDropMode    NotifyStartDrag( TransferDataContainer& rData, SvLBoxEntry* pEntry );
    virtual BOOL            NotifyAcceptDrop( SvLBoxEntry* pEntry );

    virtual BOOL    NotifyMoving( SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                        SvLBoxEntry*& rpNewParent, ULONG& rNewChildPos );
    virtual BOOL    NotifyCopying( SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                        SvLBoxEntry*& rpNewParent, ULONG& rNewChildPos );
    BOOL            NotifyCopyingMoving( SvLBoxEntry* pTarget, SvLBoxEntry* pEntry,
                        SvLBoxEntry*& rpNewParent, ULONG& rNewChildPos, BOOL bMove );

public:
    ExtBasicTreeListBox( Window* pParent, const ResId& rRes );
    ~ExtBasicTreeListBox();
};

#define LIBMODE_CHOOSER     1
#define LIBMODE_MANAGER     2

class BasicCheckBox : public SvTabListBox
{
private:
    USHORT              nMode;
    SvLBoxButtonData*   pCheckButton;
    SfxObjectShell*     m_pShell;
    void                Init();

public:
                    BasicCheckBox( Window* pParent, const ResId& rResId );
                    ~BasicCheckBox();

    SvLBoxEntry*    InsertEntry( const String& rStr, ULONG nPos = LISTBOX_APPEND );
    void            RemoveEntry( ULONG nPos );
    SvLBoxEntry*    FindEntry( const String& rName );

    void            SelectEntryPos( ULONG nPos, BOOL bSelect = TRUE );
    ULONG           GetSelectEntryPos() const;

    ULONG           GetCheckedEntryCount() const;
    void            CheckEntryPos( ULONG nPos, BOOL bCheck = TRUE );
    BOOL            IsChecked( ULONG nPos ) const;

    virtual void    InitEntry( SvLBoxEntry*, const XubString&, const Image&, const Image& );
    virtual BOOL    EditingEntry( SvLBoxEntry* pEntry, Selection& rSel );
    virtual BOOL    EditedEntry( SvLBoxEntry* pEntry, const String& rNewText );

    void            SetShell( SfxObjectShell* pShell ) { m_pShell = pShell; }

    void            SetMode( USHORT n );
    USHORT          GetMode() const         { return nMode; }
};

class LibDialog: public ModalDialog
{
private:
    OKButton        aOKButton;
    CancelButton    aCancelButton;
    FixedText       aStorageName;
    BasicCheckBox   aLibBox;
    FixedLine       aFixedLine;
    CheckBox        aReferenceBox;
    CheckBox        aReplaceBox;

public:
                    LibDialog( Window* pParent );
                    ~LibDialog();

    void            SetStorageName( const String& rName );

    BasicCheckBox&  GetLibBox()                 { return aLibBox; }
    BOOL            IsReference() const         { return aReferenceBox.IsChecked(); }
    BOOL            IsReplace() const           { return aReplaceBox.IsChecked(); }

    void            EnableReference( BOOL b )   { aReferenceBox.Enable( b ); }
    void            EnableReplace( BOOL b )     { aReplaceBox.Enable( b ); }
};


class OrganizeDialog : public TabDialog
{
private:
    TabControl              aTabCtrl;
    BasicEntryDescriptor    m_aCurEntry;

public:
                    OrganizeDialog( Window* pParent, INT16 tabId, BasicEntryDescriptor& rDesc );
                    ~OrganizeDialog();

    virtual short   Execute();

    DECL_LINK( ActivatePageHdl, TabControl * );
};

class ObjectPage: public TabPage
{
protected:
    FixedText           aLibText;
    ExtBasicTreeListBox aBasicBox;
    PushButton          aEditButton;
    CancelButton        aCloseButton;
    PushButton          aNewModButton;
    PushButton          aNewDlgButton;
    PushButton          aDelButton;

    DECL_LINK( BasicBoxHighlightHdl, BasicTreeListBox * );
    DECL_LINK( ButtonHdl, Button * );
    void                CheckButtons();
    bool                GetSelection( SfxObjectShell*& rpShell, String& rLibName );
    void                DeleteCurrent();
    void                NewModule();
    void                NewDialog();
    void                EndTabDialog( USHORT nRet );

    TabDialog*          pTabDlg;

    virtual void        ActivatePage();
    virtual void        DeactivatePage();

public:
                        ObjectPage( Window* pParent, const ResId& rResId, USHORT nMode );

    void                SetCurrentEntry( BasicEntryDescriptor& rDesc );
    void                SetTabDlg( TabDialog* p ) { pTabDlg = p;}
};


class SvxPasswordDialog;

class LibPage: public TabPage
{
protected:
    FixedText           aBasicsText;
    ListBox             aBasicsBox;
    FixedText           aLibText;
    BasicCheckBox       aLibBox;
    PushButton          aEditButton;
    CancelButton        aCloseButton;
    PushButton          aPasswordButton;
    PushButton          aNewLibButton;
    PushButton          aInsertLibButton;
    PushButton          aDelButton;

    SfxObjectShell*     m_pCurShell;
    LibraryLocation     m_eCurLocation;

    DECL_LINK( TreeListHighlightHdl, SvTreeListBox * );
    DECL_LINK( BasicSelectHdl, ListBox * );
    DECL_LINK( ButtonHdl, Button * );
    DECL_LINK( CheckPasswordHdl, SvxPasswordDialog * );
    void                CheckButtons();
    void                DeleteCurrent();
    void                NewLib();
    void                InsertLib();
    void                EndTabDialog( USHORT nRet );
    void                FillListBox();
    void                InsertListBoxEntry( SfxObjectShell* pShell, LibraryLocation eLocation );
    void                SetCurLib();
    SvLBoxEntry*        ImpInsertLibEntry( const String& rLibName, ULONG nPos );
    virtual void        ActivatePage();
    virtual void        DeactivatePage();

    TabDialog*          pTabDlg;

public:
                        LibPage( Window* pParent );
    virtual             ~LibPage();

    void                SetTabDlg( TabDialog* p ) { pTabDlg = p;}
};

// Helper functions
SbModule* createModImpl( Window* pWin, SfxObjectShell* pShell,
    BasicTreeListBox& rBasicBox, const String& rLibName, String aModName, bool bMain = false );
void createLibImpl( Window* pWin, SfxObjectShell* pShell,
                    BasicCheckBox* pLibBox, BasicTreeListBox* pBasicBox );

#endif // _MODULDLG_HXX
