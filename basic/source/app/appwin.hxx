/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: appwin.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 21:12:15 $
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

#ifndef _APPWIN_HXX
#define _APPWIN_HXX

//#include <sb.hxx>
#ifndef _SV_DOCKWIN_HXX
#include <vcl/dockwin.hxx>
#endif
#ifndef _FSYS_HXX //autogen
#include <tools/fsys.hxx>
#endif

#include "app.hxx"
#include "dataedit.hxx"

typedef USHORT QueryBits;
#define QUERY_NONE              ( QueryBits ( 0x00 ) )
#define QUERY_DIRTY             ( QueryBits ( 0x01 ) )
#define QUERY_DISK_CHANGED      ( QueryBits ( 0x02 ) )
#define QUERY_ALL               ( QUERY_DIRTY | QUERY_DISK_CHANGED )
#define SAVE_NOT_DIRTY          ( QueryBits ( 0x04 ) )

#define SAVE_RES_SAVED          TRUE
#define SAVE_RES_NOT_SAVED      FALSE
#define SAVE_RES_ERROR          3
#define SAVE_RES_CANCEL         4


#define SINCE_LAST_LOAD         1
#define SINCE_LAST_ASK_RELOAD   2

#define HAS_BEEN_LOADED         1       // includes ASKED_RELOAD
#define ASKED_RELOAD            2


#define TT_WIN_STATE_MAX        0x01
#define TT_WIN_STATE_FLOAT      0x02
#define TT_WIN_STATE_HIDE       0x04

class BasicFrame;

class AppWin : public DockingWindow, public SfxListener     // Dokumentfenster
{
    friend class MsgEdit;
protected:
    static short nNumber;           // fortlaufende Nummer
    static short nCount;            // Anzahl Editfenster
    static String *pNoName;         // "Untitled"
    FileStat aLastAccess;           // Wann wurde die geladene Dateiversion ver�ndert
    USHORT nSkipReload;             // Manchmal darf kein Reload erfolgen
    BOOL bHasFile;                  // Ansonsten hat reload auch keinen Sinn
    BOOL bReloadAborted;            // Wird gesetzt, wenn reload abgelehnt wurde, so da� beim Schlie�en nochmal gefragt werden kann

    short nId;                      // ID-Nummer( "Unbenannt n" )
    BasicFrame* pFrame;             // Parent-Window
//  Icon* pIcon;                    // Dokument-Icon
    String aFind;                   // Suchstring
    String aReplace;                // Ersetze-String
    BOOL bFind;                     // TRUE, wenn Suchen und nicht Ersetzen
    void RequestHelp( const HelpEvent& );// Hilfe-Handler
    void GetFocus();                // aktivieren
    virtual USHORT ImplSave();      // Datei speichern
    USHORT nWinState;               // Maximized, Iconized oder Normal
    Point nNormalPos;               // Position wenn Normal
    Size nNormalSize;               // Gr��e wenn Normal
    virtual long    PreNotify( NotifyEvent& rNEvt );
    USHORT nWinId;

public:
    TYPEINFO();
    AppWin( BasicFrame* );
    ~AppWin();
    DataEdit* pDataEdit;                // Daten-Flaeche
    virtual USHORT GetLineNr()=0;       // Aktuelle Zeilennummer
    virtual long InitMenu( Menu* );     // Initialisierung des Menues
    virtual long DeInitMenu( Menu* );   // r�cksetzen, so da� wieder alle Shortcuts enabled sind
    virtual void Command( const CommandEvent& rCEvt );  // Kommando-Handler
    virtual void Resize();              // Aenderung Fenstergroesse
    virtual void Help();                // Hilfe aktivieren
    virtual BOOL Load( const String& ); // Datei laden
    virtual void PostLoad(){}           // Nachbearbeiten des geladenen (Source am Modul setzen)
    virtual USHORT SaveAs();                // Datei unter neuem Namen speichern
    virtual void PostSaveAs(){}         // Nachbearbeiten des Moduls ...
    virtual void Find();                // Text suchen
    virtual void Replace();             // Text ersetzen
    virtual void Repeat();              // Suche wiederholen
    virtual BOOL Close();               // Fenster schliessen
    virtual void Activate();            // Fenster wurde aktiviert
    virtual FileType GetFileType()=0;   // Liefert den Filetype
    virtual BOOL ReloadAllowed(){ return TRUE; } // Erm�glicht dem Dok tempor�r NEIN zu sagen
    virtual void Reload();              // Reload nach �nderung auf Platte
    virtual void LoadIniFile(){;}       // (re)load ini file after change
    void CheckReload();                 // Pr�ft und Fragt ob reloaded werden soll
    BOOL DiskFileChanged( USHORT nWhat );   // Pr�ft ob die Datei sich ver�ndert hat
    void UpdateFileInfo( USHORT nWhat );    // Merkt sich den aktuellen Zustand der Datei
    BOOL IsSkipReload();                // Soll reload getestet werden
    void SkipReload( BOOL bSkip = TRUE );
    USHORT GetWinState(){ return nWinState; }
    void Maximize();
    void Restore();
    void Minimize( BOOL bMinimize );
    void Cascade( USHORT nNr );

    USHORT QuerySave( QueryBits nBits = QUERY_ALL );        // Speichern
    BOOL IsModified()               { return pDataEdit->IsModified(); }
    BasicFrame* GetBasicFrame() { return pFrame; }
    virtual void    TitleButtonClick( USHORT nButton );
    virtual void    SetText( const XubString& rStr );

    USHORT GetWinId() { return nWinId; }
    void SetWinId( USHORT nId ) { nWinId = nId; }
};

DECLARE_LIST( EditList, AppWin* );

#endif
