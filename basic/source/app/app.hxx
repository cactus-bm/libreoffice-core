/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: app.hxx,v $
 * $Revision: 1.13 $
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

#ifndef _BASICAPP_HXX
#define _BASICAPP_HXX

#include <vcl/svapp.hxx>
#ifndef _HELP_HXX //autogen
#include <vcl/help.hxx>
#endif
#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#include <vcl/timer.hxx>
#include <svtools/brdcst.hxx>
#include <svtools/lstner.hxx>

class BasicFrame;
#include <basic/mybasic.hxx>

class EditList;
class AppWin;
class AppEdit;
class AppBasEd;
class MsgEdit;
class AppError;
class StatusLine;
class BasicPrinter;
struct TTLogMsg;

class BasicApp : public Application {
    short       nWait;              // Wait-Zaehler
public:
//  Help*       pHelp;              // Hilfesystem
    BasicFrame* pFrame;             // Frame Window
//  MenuBar*    pMainMenu;          // Hauptmenue
    Accelerator*    pMainAccel;     // Acceleratoren

    void  Main( );

    void  LoadIniFile();
    void  SetFocus();
    void  Wait( BOOL );
    DECL_LINK( LateInit, void * );

#ifdef DBG_UTIL
    DbgPrintLine DbgPrintMsgBox;
#endif
};


typedef USHORT FileType;

#define FT_NO_FILE          (FileType)0x00      // Ein Fehler ist aufgetreten ...
#define FT_BASIC_SOURCE     (FileType)0x01
#define FT_BASIC_INCLUDE    (FileType)0x02
#define FT_RESULT_FILE      (FileType)0x04
#define FT_RESULT_FILE_TXT  (FileType)0x08
#define FT_BASIC_LIBRARY    (FileType)0x10

struct WinInfoRec;
class DisplayHidDlg;

class FloatingExecutionStatus;

class BasicFrame : public WorkWindow, public SfxBroadcaster, public SfxListener
{
using SystemWindow::Notify;
using Window::Command;

virtual BOOL Close();               // Schliessen
    BOOL CloseAll();                // Alle Fenster schliessen
    BOOL CompileAll();              // Alle Texte compilieren
    AutoTimer aLineNum;             // Zeigt die Zeilennummer an
virtual void Resize();
virtual void Move();
virtual void GetFocus();
    void LoadLibrary();
    void SaveLibrary();
    BOOL bIsAutoRun;
    DisplayHidDlg* pDisplayHidDlg;

//  BreakPoint *pRunToCursorBP;

    SbxVariable *pEditVar;



    Timer aCheckFiles;      // Pr�fen der Dateien auf �nderungen
    BOOL bAutoReload;
    BOOL bAutoSave;
    DECL_LINK( CheckAllFiles, Timer* );

    MyBasicRef  pBasic;             // BASIC-Engine

    String aAppName;                // Inhalt der Titelteile der App:
    String aAppFile;                // AppName AppFile [AppMode]
    String aAppMode;
    void UpdateTitle();
    DECL_LINK( CloseButtonClick, void* );
    DECL_LINK( FloatButtonClick, void* );
    DECL_LINK( HideButtonClick, void* );

    FloatingExecutionStatus *pExecutionStatus;

public:
    BOOL IsAutoRun();
    void SetAutoRun( BOOL bAuto );
    BOOL bInBreak;                  // TRUE, wenn im Break-Handler
    StatusLine* pStatus;            // Statuszeile
    EditList*   pList;              // List der Edit-Fenster
    AppWin*     pWork;              // aktuelles Edit-Fenster
    BasicPrinter* pPrn;             // Drucker
    BOOL bDisas;                    // TRUE: disassemble
    USHORT nFlags;                  // Debugging-Flags
    USHORT nMaximizedWindows;       // Anzahl der Fenster, die maximized sind
    void FocusWindow( AppWin *pWin );
    void WinMax_Restore();
    void WinShow_Hide();
    void RemoveWindow( AppWin *pWin );
    void AddWindow( AppWin *pWin );
    void WindowRenamed( AppWin *pWin );

    BasicFrame();
   ~BasicFrame();
    MyBasic& Basic()                { return *pBasic; }
    void AddToLRU(String const& aFile);
    void LoadLRU();
    DECL_LINK( InitMenu, Menu * );
    DECL_LINK( DeInitMenu, Menu * );
    DECL_LINK( HighlightMenu, Menu * );
    DECL_LINK( MenuCommand, Menu * );
    DECL_LINK( Accel, Accelerator * );
    DECL_LINK( ShowLineNr, AutoTimer * );
    MsgEdit* GetMsgTree( String aLogFileName );
    DECL_LINK( Log, TTLogMsg * );
    DECL_LINK( WinInfo, WinInfoRec * );
    BOOL LoadFile( String aFilename );
    long Command( short,BOOL=FALSE );// Kommando-Handler
    virtual void Command( const CommandEvent& rCEvt );      // Kommando-Handler
    BOOL SaveAll();                 // Alle Fenster speichern
    BOOL QueryFileName( String& rName, FileType nFileType, BOOL bSave );// Dateinamen ermitteln
    DECL_LINK( ModuleWinExists, String* );
    DECL_LINK( WriteString, String* );
    AppBasEd* CreateModuleWin( SbModule* pMod );
    AppBasEd* FindModuleWin( const String& );
    AppError* FindErrorWin( const String& );
    AppWin* FindWin( const String& );
    AppWin* FindWin( USHORT nWinId );
    AppWin* IsWinValid( AppWin* pMaybeWin );
    USHORT BreakHandler();          // Break-Handler-Callback

    void SetEditVar( SbxVariable *pVar ){ pEditVar = pVar;}
    SbxVariable* GetEditVar(){ return pEditVar;}
    BOOL IsAutoReload() { return bAutoReload; }
    BOOL IsAutoSave() { return bAutoSave; }
    void LoadIniFile();

    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    void SetAppMode( const String &aNewMode ){ aAppMode = aNewMode; UpdateTitle(); }

    String GenRealString( const String &aResString );

};

extern BasicApp aBasicApp;

#endif
