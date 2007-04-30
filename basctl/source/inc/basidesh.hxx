/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: basidesh.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-30 07:35:38 $
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
#ifndef _BASIDESH_HXX
#define _BASIDESH_HXX

#ifndef _SBXITEM_HXX
#include "sbxitem.hxx"
#endif

#include "scriptdocument.hxx"

#ifndef _VIEWFAC_HXX //autogen
#include <sfx2/viewfac.hxx>
#endif
#ifndef _SFX_SHELL_HXX //autogen
#include <sfx2/shell.hxx>
#endif
#ifndef _SCRBAR_HXX //autogen
#include <vcl/scrbar.hxx>
#endif
#ifndef _TABLE_HXX //autogen
#include <tools/table.hxx>
#endif
#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif
#include <svx/ifaceids.hxx>

//----------------------------------------------------------------------------

const ULONG BASICIDE_UI_FEATURE_SHOW_BROWSER = 0x00000001;

//----------------------------------------------------------------------------

class ModulWindow;
class ModulWindowLayout;
class DialogWindow;
class SdrView;
class ObjectCatalog;
class BasicIDETabBar;
class TabBar;
class IDEBaseWindow;
class SbxObject;
class SbModule;
class StarBASIC;
class LocalizationMgr;
struct BasicIDEShell_Impl;

#if _SOLAR__PRIVATE
DECLARE_TABLE( IDEWindowTable, IDEBaseWindow* )
#else
typedef Table IDEWindowTable;
#endif

class BasicIDEShell: public SfxViewShell
{
friend class JavaDebuggingListenerImpl;
friend class LocalizationMgr;
friend class BasicIDE;

    ObjectCatalog*      pObjectCatalog;

    IDEWindowTable      aIDEWindowTable;
    USHORT              nCurKey;
    IDEBaseWindow*      pCurWin;
    ScriptDocument      m_aCurDocument;
    String              m_aCurLibName;
    LocalizationMgr*    m_pCurLocalizationMgr;

    ScrollBar           aHScrollBar;
    ScrollBar           aVScrollBar;
    ScrollBarBox        aScrollBarBox;
    BasicIDETabBar*     pTabBar;
    BOOL                bTabBarSplitted;
    BOOL                bCreatingWindow;
    ModulWindowLayout*  pModulLayout;
    BOOL                m_bAppBasicModified;

#if _SOLAR__PRIVATE
    void                Init();
    void                InitTabBar();
    void                InitScrollBars();
    void                CheckWindows();
    void                RemoveWindows( const ScriptDocument& rDocument, const String& rLibName, BOOL bDestroy );
    void                UpdateWindows();
    void                ShowObjectDialog( BOOL bShow, BOOL bCreateOrDestroy );
    void                InvalidateBasicIDESlots();
    void                StoreAllWindowData( BOOL bPersistent = TRUE );
    void                SetMDITitle();
    void                EnableScrollbars( BOOL bEnable );
    void                SetCurLib( const ScriptDocument& rDocument, String aLibName, bool bUpdateWindows = true , bool bCheck = true );
    void                SetCurLibForLocalization( const ScriptDocument& rDocument, String aLibName );

    void                ImplStartListening( StarBASIC* pBasic );

    DECL_LINK( TabBarHdl, TabBar* );
    DECL_LINK( AccelSelectHdl, Accelerator* );
    DECL_LINK( ObjectDialogCancelHdl, ObjectCatalog * );
    DECL_LINK( TabBarSplitHdl, TabBar * );
#endif

protected:
    virtual void        AdjustPosSizePixel( const Point &rPos, const Size &rSize );
    virtual void        OuterResizePixel( const Point &rPos, const Size &rSize );
    virtual Size        GetOptimalSizePixel() const;
    USHORT              InsertWindowInTable( IDEBaseWindow* pNewWin );
    virtual USHORT      PrepareClose( BOOL bUI, BOOL bForBrowsing );

    void                SetCurWindow( IDEBaseWindow* pNewWin, BOOL bUpdateTabBar = FALSE, BOOL bRememberAsCurrent = TRUE );
    void                RemoveWindow( IDEBaseWindow* pWindow, BOOL bDestroy, BOOL bAllowChangeCurWindow = TRUE );
    void                ArrangeTabBar();

    ModulWindow*        CreateBasWin( const ScriptDocument& rDocument, const String& rLibName, const String& rModName );
    DialogWindow*       CreateDlgWin( const ScriptDocument& rDocument, const String& rLibName, const String& rDlgName );

    ModulWindow*        FindBasWin( const ScriptDocument& rDocument, const String& rLibName, const String& rModName, BOOL bCreateIfNotExist, BOOL bFindSuspended = FALSE );
    DialogWindow*       FindDlgWin( const ScriptDocument& rDocument, const String& rLibName, const String& rDlgName, BOOL bCreateIfNotExist, BOOL bFindSuspended = FALSE );
    ModulWindow*        ShowActiveModuleWindow( StarBASIC* pBasic );

    virtual void        SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
                                const SfxHint& rHint, const TypeId& rHintType );

    virtual void        Activate(BOOL bMDI);
    virtual void        Deactivate(BOOL bMDI);

    virtual void        Move();
    virtual void        ShowCursor( FASTBOOL bOn = TRUE );

    void                CreateModulWindowLayout();
    void                DestroyModulWindowLayout();
    void                UpdateModulWindowLayout( bool bBasicStopped );

    sal_Bool            HasBasic() const;

public:
                        TYPEINFO();
                        SFX_DECL_INTERFACE( SVX_INTERFACE_BASIDE_VIEWSH )
                        SFX_DECL_VIEWFACTORY(BasicIDEShell);

                        BasicIDEShell( SfxViewFrame *pFrame, Window *);
                        BasicIDEShell( SfxViewFrame *pFrame, SfxViewShell *pOldSh );
                        BasicIDEShell( SfxViewFrame *pFrame, const BasicIDEShell &rOrig );
                        ~BasicIDEShell();

    IDEBaseWindow*      GetCurWindow() const    { return pCurWin; }
    const ScriptDocument&
                        GetCurDocument() const { return m_aCurDocument; }
    const String&       GetCurLibName() const { return m_aCurLibName; }
    ObjectCatalog*      GetObjectCatalog() const    { return pObjectCatalog; }
    LocalizationMgr*    GetCurLocalizationMgr() const { return m_pCurLocalizationMgr; }

    ScrollBar&          GetHScrollBar()         { return aHScrollBar; }
    ScrollBar&          GetVScrollBar()         { return aVScrollBar; }
    ScrollBarBox&       GetScrollBarBox()       { return aScrollBarBox; }
    TabBar*             GetTabBar()             { return (TabBar*)pTabBar; }
    IDEWindowTable&     GetIDEWindowTable()     { return aIDEWindowTable; }

    SdrView*            GetCurDlgView();

    SfxUndoManager*     GetUndoManager();

    virtual USHORT          Print( SfxProgress &rProgress, BOOL bIsAPI, PrintDialog *pPrintDialog = 0 );
    virtual SfxPrinter*     GetPrinter( BOOL bCreate );
    virtual USHORT          SetPrinter( SfxPrinter *pNewPrinter, USHORT nDiffFlags = SFX_PRINTER_ALL );
    virtual String          GetSelectionText( BOOL bCompleteWords );
    virtual BOOL            HasSelection( BOOL bText ) const;

    void                GetState( SfxItemSet& );
    void                ExecuteGlobal( SfxRequest& rReq );
    void                ExecuteCurrent( SfxRequest& rReq );
    void                ExecuteBasic( SfxRequest& rReq );
    void                ExecuteDialog( SfxRequest& rReq );

    virtual sal_Bool    HasUIFeature( sal_uInt32 nFeature );

    long                CallBasicErrorHdl( StarBASIC* pBasic );
    long                CallBasicBreakHdl( StarBASIC* pBasic );

    ModulWindowLayout*  GetLayoutWindow() const { return pModulLayout; }

    IDEBaseWindow*      FindWindow( const ScriptDocument& rDocument, const String& rLibName = String(), const String& rName = String(), USHORT nType = BASICIDE_TYPE_UNKNOWN, BOOL bFindSuspended = FALSE );
    IDEBaseWindow*      FindApplicationWindow();
    BOOL                NextPage( BOOL bPrev = FALSE );

    BOOL                IsAppBasicModified() const { return m_bAppBasicModified; }
    void                SetAppBasicModified( BOOL bModified = TRUE ) { m_bAppBasicModified = bModified; }
};

#endif // _BASIDESH_HXX
