/*************************************************************************
 *
 *  $RCSfile: salframe.h,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 13:39:18 $
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

#ifndef _SV_SALFRAME_H
#define _SV_SALFRAME_H

#ifndef _SV_SV_H
#include <sv.h>
#endif

#ifndef _SV_SYSDATA_HXX
#include <sysdata.hxx>
#endif

#ifndef _SV_SALFRAME_HXX
#include <salframe.hxx>
#endif

class WinSalGraphics;

// ----------------
// - WinSalFrame -
// ----------------

class WinSalFrame : public SalFrame
{
public:
    HWND                    mhWnd;                  // Window handle
    HCURSOR                 mhCursor;               // cursor handle
    HIMC                    mhDefIMEContext;        // default IME-Context
    WinSalGraphics*         mpGraphics;             // current frame graphics
    WinSalGraphics*         mpGraphics2;            // current frame graphics for other threads
    WinSalFrame*            mpNextFrame;            // pointer to next frame
    HMENU                   mSelectedhMenu;         // the menu where highlighting is currently going on
    HMENU                   mLastActivatedhMenu;    // the menu that was most recently opened
    SystemEnvData           maSysData;              // system data
    SalFrameState           maState;                // frame state
    int                     mnShowState;            // show state
    long                    mnWidth;                // client width in pixeln
    long                    mnHeight;               // client height in pixeln
    int                     mnMinWidth;             // min. client width in pixeln
    int                     mnMinHeight;            // min. client height in pixeln
    int                     mnMaxWidth;             // max. client width in pixeln
    int                     mnMaxHeight;            // max. client height in pixeln
    RECT                    maFullScreenRect;       // fullscreen rect
    int                     mnFullScreenShowState;  // fullscreen restore show state
    UINT                    mnInputLang;            // current Input Language
    UINT                    mnInputCodePage;        // current Input CodePage
    ULONG                   mnStyle;                // style
    BOOL                    mbGraphics;             // is Graphics used
    BOOL                    mbCaption;              // has window a caption
    BOOL                    mbBorder;               // has window a border
    BOOL                    mbFixBorder;            // has window a fixed border
    BOOL                    mbSizeBorder;           // has window a sizeable border
    BOOL                    mbNoIcon;               // is an window without an icon
    BOOL                    mbFloatWin;             // is a FloatingWindow
    BOOL                    mbFullScreen;           // TRUE: in full screen mode
    BOOL                    mbPresentation;         // TRUE: Presentation Mode running
    BOOL                    mbInShow;               // innerhalb eines Show-Aufrufs
    BOOL                    mbRestoreMaximize;      // Restore-Maximize
    BOOL                    mbInMoveMsg;            // Move-Message wird verarbeitet
    BOOL                    mbInSizeMsg;            // Size-Message wird verarbeitet
    BOOL                    mbFullScreenToolWin;    // WS_EX_TOOLWINDOW reset in FullScreenMode
    BOOL                    mbDefPos;               // default-position
    BOOL                    mbOverwriteState;       // TRUE: WindowState darf umgesetzt werden
    BOOL                    mbIME;                  // TRUE: We are in IME Mode
    BOOL                    mbHandleIME;            // TRUE: Wir handeln die IME-Messages
    BOOL                    mbSpezIME;              // TRUE: Spez IME
    BOOL                    mbAtCursorIME;          // TRUE: Wir behandeln nur einige IME-Messages
    BOOL                    mbCandidateMode;        // TRUE: Wir befinden uns im Candidate-Modus
    static BOOL             mbInReparent;           // TRUE: ignore focus lost and gain due to reparenting

public:
    WinSalFrame();
    virtual ~WinSalFrame();

    virtual SalGraphics*        GetGraphics();
    virtual void                ReleaseGraphics( SalGraphics* pGraphics );
    virtual BOOL                PostEvent( void* pData );
    virtual void                SetTitle( const XubString& rTitle );
    virtual void                SetIcon( USHORT nIcon );
    virtual void                                SetMenu( SalMenu* pSalMenu );
    virtual void                                DrawMenuBar();
    virtual void                Show( BOOL bVisible, BOOL bNoActivate = FALSE );
    virtual void                Enable( BOOL bEnable );
    virtual void                SetMinClientSize( long nWidth, long nHeight );
    virtual void                SetMaxClientSize( long nWidth, long nHeight );
    virtual void                SetPosSize( long nX, long nY, long nWidth, long nHeight, USHORT nFlags );
    virtual void                GetClientSize( long& rWidth, long& rHeight );
    virtual void                GetWorkArea( Rectangle& rRect );
    virtual SalFrame*           GetParent() const;
    virtual void                SetWindowState( const SalFrameState* pState );
    virtual BOOL                GetWindowState( SalFrameState* pState );
    virtual void                ShowFullScreen( BOOL bFullScreen );
    virtual void                StartPresentation( BOOL bStart );
    virtual void                SetAlwaysOnTop( BOOL bOnTop );
    virtual void                ToTop( USHORT nFlags );
    virtual void                SetPointer( PointerStyle ePointerStyle );
    virtual void                CaptureMouse( BOOL bMouse );
    virtual void                SetPointerPos( long nX, long nY );
    virtual void                Flush();
    virtual void                Sync();
    virtual void                SetInputContext( SalInputContext* pContext );
    virtual void                EndExtTextInput( USHORT nFlags );
    virtual String              GetKeyName( USHORT nKeyCode );
    virtual String              GetSymbolKeyName( const XubString& rFontName, USHORT nKeyCode );
    virtual LanguageType        GetInputLanguage();
    virtual SalBitmap*          SnapShot();
    virtual void                UpdateSettings( AllSettings& rSettings );
    virtual void                Beep( SoundType eSoundType );
    virtual const SystemEnvData*    GetSystemData() const;
    virtual SalPointerState     GetPointerState();
    virtual void                SetParent( SalFrame* pNewParent );
    virtual bool                SetPluginParent( SystemParentData* pNewParent );
    virtual void                SetBackgroundBitmap( SalBitmap* );
};

void ImplSalGetWorkArea( HWND hWnd, RECT *pRect, const RECT *pParentRect );

#endif // _SV_SALFRAME_H
