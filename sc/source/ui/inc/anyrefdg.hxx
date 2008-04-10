/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: anyrefdg.hxx,v $
 * $Revision: 1.11 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef SC_ANYREFDG_HXX
#define SC_ANYREFDG_HXX

#ifndef _IMAGEBTN_HXX
#include <vcl/imagebtn.hxx>
#endif
#ifndef _EDIT_HXX
#include <vcl/edit.hxx>
#endif
#ifndef _ACCEL_HXX
#include <vcl/accel.hxx>
#endif
#include <sfx2/basedlgs.hxx>
#include "address.hxx"

class SfxObjectShell;
class ScRange;
class ScDocument;
class ScTabViewShell;
class ScAnyRefDlg;
class ScRefButton;
class ScFormulaCell;
class ScCompiler;
class ScRangeList;

//============================================================================

class ScRefEdit : public Edit
{
private:
    Timer               aTimer;
    ScAnyRefDlg*        pAnyRefDlg;         // parent dialog
    BOOL                bSilentFocus;       // for SilentGrabFocus()

    DECL_LINK( UpdateHdl, Timer* );

protected:
    virtual void        KeyInput( const KeyEvent& rKEvt );
    virtual void        GetFocus();
    virtual void        LoseFocus();

public:
                        ScRefEdit( ScAnyRefDlg* pParent, const ResId& rResId );
                        ScRefEdit( Window* pParent, const ResId& rResId );
    virtual             ~ScRefEdit();

    void                SetRefString( const XubString& rStr );
    using Edit::SetText;
    virtual void        SetText( const XubString& rStr );
    virtual void        Modify();

    void                StartUpdateData();

    void                SilentGrabFocus();  // does not update any references

    void                SetRefDialog( ScAnyRefDlg* pDlg );
    inline ScAnyRefDlg* GetRefDialog() { return pAnyRefDlg; }
};

//============================================================================

class ScRefButton : public ImageButton
{
private:
    Image               aImgRefStart;   /// Start reference input
    Image               aImgRefStartHC; /// Start reference input (high contrast)
    Image               aImgRefDone;    /// Stop reference input
    Image               aImgRefDoneHC;  /// Stop reference input (high contrast)
    ScAnyRefDlg*        pAnyRefDlg;     // parent dialog
    ScRefEdit*          pRefEdit;       // zugeordnetes Edit-Control

protected:
    virtual void        Click();
    virtual void        KeyInput( const KeyEvent& rKEvt );
    virtual void        GetFocus();
    virtual void        LoseFocus();

public:
                        ScRefButton( ScAnyRefDlg* pParent, const ResId& rResId, ScRefEdit* pEdit );
                        ScRefButton( Window* pParent, const ResId& rResId );

    void                SetReferences( ScAnyRefDlg* pDlg, ScRefEdit* pEdit );

    void                SetStartImage();
    void                SetEndImage();
    inline void         DoRef() { Click(); }
};


//============================================================================

class ScAnyRefDlg : public SfxModelessDialog
{
    friend class        ScRefButton;
    friend class        ScRefEdit;

private:
    SfxBindings*        pMyBindings;
    ScRefEdit*          pRefEdit;               // aktives Eingabefeld
    ScRefButton*        pRefBtn;                // Button dazu
    String              sOldDialogText;         // Originaltitel des Dialogfensters
    Size                aOldDialogSize;         // Originalgroesse Dialogfenster
    Point               aOldEditPos;            // Originalposition des Eingabefeldes
    Size                aOldEditSize;           // Originalgroesse des Eingabefeldes
    Point               aOldButtonPos;          // Originalpositiuon des Buttons
    BOOL*               pHiddenMarks;           // Merkfeld fuer versteckte Controls
    Accelerator*        pAccel;                 // fuer Enter/Escape
    BOOL                bAccInserted;
    BOOL                bHighLightRef;
    BOOL                bEnableColorRef;
    ScFormulaCell*      pRefCell;
    ScCompiler*         pRefComp;
    Window*             pActiveWin;
    Timer               aTimer;
    String              aDocName;               // document on which the dialog was opened
    SCTAB               nRefTab;                // used for ShowReference

    DECL_LINK( UpdateFocusHdl, Timer* );
    DECL_LINK( AccelSelectHdl, Accelerator* );

protected:
    BOOL                DoClose( USHORT nId );

    void                EnableSpreadsheets( BOOL bFlag = TRUE, BOOL bChilds = TRUE );
    void                SetDispatcherLock( BOOL bLock );

    virtual long        PreNotify( NotifyEvent& rNEvt );

    virtual void        RefInputStart( ScRefEdit* pEdit, ScRefButton* pButton = NULL );
    virtual void        RefInputDone( BOOL bForced = FALSE );
    void                ShowSimpleReference( const XubString& rStr );
    void                ShowFormulaReference( const XubString& rStr );

    bool                ParseWithNames( ScRangeList& rRanges, const String& rStr, ScDocument* pDoc );

public:
                        ScAnyRefDlg( SfxBindings* pB, SfxChildWindow* pCW,
                                     Window* pParent, USHORT nResId);
    virtual             ~ScAnyRefDlg();

    virtual void        SetReference( const ScRange& rRef, ScDocument* pDoc ) = 0;
    virtual void        AddRefEntry();

    virtual BOOL        IsRefInputMode() const;
    virtual BOOL        IsTableLocked() const;
    virtual BOOL        IsDocAllowed( SfxObjectShell* pDocSh ) const;

    void                ShowReference( const XubString& rStr );
    void                HideReference( BOOL bDoneRefMode = TRUE );

    void                ToggleCollapsed( ScRefEdit* pEdit, ScRefButton* pButton = NULL );
    void                ReleaseFocus( ScRefEdit* pEdit, ScRefButton* pButton = NULL );

    void                ViewShellChanged( ScTabViewShell* pScViewShell );
    void                SwitchToDocument();
    SfxBindings&        GetBindings();

    virtual void        SetActive() = 0;
//  virtual BOOL        Close();
    virtual void        StateChanged( StateChangedType nStateChange );
};


//============================================================================

#endif // SC_ANYREFDG_HXX

