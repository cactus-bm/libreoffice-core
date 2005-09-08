/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tabvwsh.hxx,v $
 *
 *  $Revision: 1.22 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:56:44 $
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

#ifndef SC_TABVWSH_HXX
#define SC_TABVWSH_HXX

#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif

#ifndef _VIEWFAC_HXX //autogen
#include <sfx2/viewfac.hxx>
#endif

#ifndef INCLUDED_SCDLLAPI_H
#include "scdllapi.h"
#endif

#ifndef SC_DBFUNC_HXX
#include "dbfunc.hxx"           // -> tabview
#endif

#ifndef SC_TARGET_HXX
#include "target.hxx"
#endif

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"         // ScRangeListRef
#endif

#ifndef SC_SHELLIDS_HXX
#include "shellids.hxx"
#endif

class FmFormShell;
class SbxObject;
class SdrOle2Obj;
class SfxBindings;
class SfxChildWindow;
class SfxModelessDialog;
class SvxBorderLine;
class SvxBoxObjectRef;
class SvxNumberInfoItem;
struct SfxChildWinInfo;

class ScArea;
class ScAuditingShell;
class ScDrawShell;
class ScDrawTextObjectBar;
class ScEditShell;
class ScInputHandler;
class ScPivotShell;
class ScDrawFormShell;
class ScCellShell;
class ScOleObjectShell;
class ScGraphicShell;
class ScMediaShell;
class ScChartShell;
class ScPageBreakShell;
class ScDPObject;
class ScNavigatorSettings;

struct ScHeaderFieldData;

namespace com { namespace sun { namespace star { namespace frame {
    class XDispatchProviderInterceptor;
} } } }

namespace svx {
    class ExtrusionBar;
    class FontworkBar;
}

enum ObjectSelectionType
{
    OST_NONE,
    OST_Cell,
    OST_Editing,
    OST_DrawText,
    OST_Drawing,
    OST_DrawForm,
    OST_Pivot,
    OST_Auditing,
    OST_OleObject,
    OST_Chart,
    OST_Graphic,
    OST_Media
};

//==================================================================


class SC_DLLPUBLIC ScTabViewShell: public SfxViewShell, public ScDBFunc
{
private:
    static USHORT           nInsertCtrlState;
    static USHORT           nInsCellsCtrlState;
    static USHORT           nInsObjCtrlState;

    ObjectSelectionType     eCurOST;
    USHORT                  nDrawSfxId;
    USHORT                  nCtrlSfxId;
    USHORT                  nFormSfxId;
    ScDrawShell*            pDrawShell;
    ScDrawTextObjectBar*    pDrawTextShell;
    ScEditShell*            pEditShell;
    ScPivotShell*           pPivotShell;
    ScAuditingShell*        pAuditingShell;
    ScDrawFormShell*        pDrawFormShell;
    ScCellShell*            pCellShell;
    ScOleObjectShell*       pOleObjectShell;
    ScChartShell*           pChartShell;
    ScGraphicShell*         pGraphicShell;
    ScMediaShell*           pMediaShell;
    ScPageBreakShell*       pPageBreakShell;
    svx::ExtrusionBar*      pExtrusionBarShell;
    svx::FontworkBar*       pFontworkBarShell;

    FmFormShell*            pFormShell;

    ScInputHandler*         pInputHandler;              // fuer OLE-Eingabezeile

    SvxBorderLine*          pCurFrameLine;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::frame::XDispatchProviderInterceptor >
                            xDisProvInterceptor;

    Point                   aWinPos;

    ScTabViewTarget         aTarget;
    ScArea*                 pPivotSource;
    ScDPObject*             pDialogDPObject;

    ScNavigatorSettings*    pNavSettings;

    // used in first Activate
    ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue > aPendingUserData;
    BOOL                    bFirstActivate;

    BOOL                    bActiveDrawSh;
    BOOL                    bActiveDrawTextSh;
    BOOL                    bActivePivotSh;
    BOOL                    bActiveAuditingSh;
    BOOL                    bActiveDrawFormSh;
    BOOL                    bActiveOleObjectSh;
    BOOL                    bActiveChartSh;
    BOOL                    bActiveGraphicSh;
    BOOL                    bActiveMediaSh;
    BOOL                    bActiveEditSh;

    BOOL                    bFormShellAtTop;            // does the FormShell need to be on top?


    BOOL                    bDontSwitch;                // EditShell nicht abschalten
    BOOL                    bInFormatDialog;            // fuer GetSelectionText
    BOOL                    bPrintSelected;             // for result of SvxPrtQryBox

    BOOL                    bReadOnly;                  // um Status-Aenderungen zu erkennen

    SbxObject*              pScSbxObject;

    BOOL                    bChartDlgIsEdit;            // Datenbereich aendern
    BOOL                    bChartAreaValid;            // wenn Chart aufgezogen wird
    String                  aEditChartName;
    ScRangeListRef          aChartSource;
    Rectangle               aChartPos;
    SCTAB                   nChartDestTab;
    USHORT                  nCurRefDlgId;

    SfxBroadcaster*         pAccessibilityBroadcaster;

    static const int        MASTERENUMCOMMANDS = 6;
    String                  aCurrShapeEnumCommand[ MASTERENUMCOMMANDS ];

private:
    SC_DLLPRIVATE void  Construct( BYTE nForceDesignMode = SC_FORCEMODE_NONE );

    SC_DLLPRIVATE void          SetMySubShell( SfxShell* pShell );
    SC_DLLPRIVATE SfxShell*     GetMySubShell() const;

    SC_DLLPRIVATE void          DoReadUserData( const String& rData );
    SC_DLLPRIVATE void          DoReadUserDataSequence( const ::com::sun::star::uno::Sequence<
                                     ::com::sun::star::beans::PropertyValue >& rSettings );

    SC_DLLPRIVATE DECL_LINK( SimpleRefClose, String* );
    SC_DLLPRIVATE DECL_LINK( SimpleRefDone, String* );
    SC_DLLPRIVATE DECL_LINK( SimpleRefAborted, String* );
    SC_DLLPRIVATE DECL_LINK( SimpleRefChange, String* );
    SC_DLLPRIVATE DECL_LINK( FormControlActivated, FmFormShell* );

protected:
    virtual void    Activate(BOOL bMDI);
    virtual void    Deactivate(BOOL bMDI);
    virtual USHORT  PrepareClose( BOOL bUI = TRUE, BOOL bForBrowsing = FALSE );

    virtual void    ShowCursor(FASTBOOL bOn);

    virtual void    Move();     // Benachrichtigung

    virtual void    AdjustPosSizePixel( const Point &rPos, const Size &rSize );     // alt

    virtual void    InnerResizePixel( const Point &rOfs, const Size &rSize );       // neu
    virtual void    OuterResizePixel( const Point &rOfs, const Size &rSize );
    virtual void    SetZoomFactor( const Fraction &rZoomX, const Fraction &rZoomY );

    virtual void    QueryObjAreaPixel( Rectangle& rRect ) const;

    virtual Size    GetOptimalSizePixel() const;

    virtual String  GetSelectionText( BOOL bWholeWord );
    virtual BOOL    HasSelection( BOOL bText ) const;
    virtual String  GetDescription() const;

    virtual void    WriteUserData(String &, BOOL bBrowse = FALSE);
    virtual void    ReadUserData(const String &, BOOL bBrowse = FALSE);
    virtual void    WriteUserDataSequence (::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse = sal_False );
    virtual void    ReadUserDataSequence (const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, sal_Bool bBrowse = sal_False );

    virtual void    UIDeactivated( SfxInPlaceClient* pClient );

    virtual FASTBOOL KeyInput( const KeyEvent &rKeyEvent );
    virtual SdrView* GetDrawView() const;

public:
                    TYPEINFO();

                    SFX_DECL_INTERFACE(SCID_TABVIEW_SHELL);
                    SFX_DECL_VIEWFACTORY(ScTabViewShell);


                    // -> Clone-Methode fuer Factory

                    ScTabViewShell( SfxViewFrame*           pViewFrame,
                                    const ScTabViewShell&   rWin );

                    // aus einer allgemeinen Shell konstruieren und
                    // soviel wie moeglich uebernehmen (SliderPos etc.):

                    ScTabViewShell( SfxViewFrame*           pViewFrame,
                                    SfxViewShell*           pOldSh );

    virtual         ~ScTabViewShell();

    Window*         GetDialogParent();

    void            ExecuteInputDirect();

    ScInputHandler* GetInputHandler() const;
    void            UpdateInputHandler( BOOL bForce = FALSE, BOOL bStopEditing = TRUE );
    BOOL            TabKeyInput(const KeyEvent& rKEvt);
    BOOL            SfxKeyInput(const KeyEvent& rKEvt);

    void            SetActive();

    SvxBorderLine*  GetDefaultFrameLine() const { return pCurFrameLine; }
    void            SetDefaultFrameLine(const SvxBorderLine* pLine );

    void            ExecuteShowNIY( SfxRequest& rReq );
    void            StateDisabled( SfxItemSet& rSet );

    void            Execute( SfxRequest& rReq );
    void            GetState( SfxItemSet& rSet );

    void            ExecuteTable( SfxRequest& rReq );
    void            GetStateTable( SfxItemSet& rSet );

    void            WindowChanged();
    void            ExecDraw(SfxRequest&);
    void            ExecDrawIns(SfxRequest& rReq);
    void            GetDrawState(SfxItemSet &rSet);
    void            GetDrawInsState(SfxItemSet &rSet);
    void            ExecGallery(SfxRequest& rReq);      // StarGallery
    void            GetGalleryState(SfxItemSet& rSet);

    void            ExecChildWin(SfxRequest& rReq);
    void            GetChildWinState( SfxItemSet& rSet );

    void            ExecImageMap( SfxRequest& rReq );
    void            GetImageMapState( SfxItemSet& rSet );

    void            ExecTbx( SfxRequest& rReq );
    void            GetTbxState( SfxItemSet& rSet );

    void            ExecuteSave( SfxRequest& rReq );
    void            GetSaveState( SfxItemSet& rSet );
    void            ExecSearch( SfxRequest& rReq );

    void            ExecuteUndo(SfxRequest& rReq);
    void            GetUndoState(SfxItemSet &rSet);

    void            ExecuteSbx( SfxRequest& rReq );
    void            GetSbxState( SfxItemSet& rSet );

    void            ExecuteObject(SfxRequest& rReq);
    void            GetObjectState(SfxItemSet &rSet);

    void            ExecDrawOpt(SfxRequest& rReq);
    void            GetDrawOptState(SfxItemSet &rSet);


    void            SetDrawShell( BOOL bActive );
    void            SetDrawTextShell( BOOL bActive );

    void            SetPivotShell( BOOL bActive );
    ScArea*         GetPivotSource(){return pPivotSource;}
    void            SetPivotSource(ScArea* pSrc){pPivotSource=pSrc;}
    void            SetDialogDPObject( const ScDPObject* pObj );
    const ScDPObject* GetDialogDPObject() const { return pDialogDPObject; }

    BOOL            GetDontSwitch(){return bDontSwitch;}
    void            SetDontSwitch(BOOL bFlag){bDontSwitch=bFlag;}


    void            SetAuditShell( BOOL bActive );
    void            SetDrawFormShell( BOOL bActive );
    void            SetEditShell(EditView* pView, BOOL bActive );
    void            SetOleObjectShell( BOOL bActive );
    void            SetChartShell( BOOL bActive );
    void            SetGraphicShell( BOOL bActive );
    void            SetMediaShell( BOOL bActive );


    void            SetDrawShellOrSub();
    void            SetCurSubShell( ObjectSelectionType eOST, BOOL bForce = FALSE );

    void            SetFormShellAtTop( BOOL bSet );

    ObjectSelectionType GetCurObjectSelectionType();

    virtual ErrCode DoVerb(long nVerb);


    void            StopEditShell();
    BOOL            IsDrawTextShell() const;
    BOOL            IsAuditShell() const;

    void            SetDrawTextUndo( SfxUndoManager* pUndoMgr );

    void            FillFieldData( ScHeaderFieldData& rData );

    void            ResetChartArea();
    void            SetChartArea( const ScRangeListRef& rSource, const Rectangle& rDest );
    BOOL            GetChartArea( ScRangeListRef& rSource, Rectangle& rDest, SCTAB& rTab ) const;

    BOOL            IsChartDlgEdit() const;
    void            SetChartDlgEdit(BOOL bFlag){bChartDlgIsEdit=bFlag;}

    void            SetEditChartName(const String& aStr){aEditChartName=aStr;}
    const String&   GetEditChartName() const;

    virtual void SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
                         const SfxHint& rHint, const TypeId& rHintType );

    ScNavigatorSettings*    GetNavigatorSettings();

    // Drucken:
    virtual SfxPrinter*     GetPrinter( BOOL bCreate = FALSE );
    virtual USHORT          SetPrinter( SfxPrinter* pNewPrinter,
                                          USHORT nDiffFlags = SFX_PRINTER_ALL );

    virtual PrintDialog*    CreatePrintDialog( Window* pParent );
    virtual SfxTabPage*     CreatePrintOptionsPage( Window *pParent, const SfxItemSet &rOptions );
    virtual void            PreparePrint( PrintDialog* pPrintDialog = NULL );
    virtual ErrCode         DoPrint( SfxPrinter *pPrinter,
                                         PrintDialog *pPrintDialog,
                                         BOOL bSilent );
    virtual USHORT          Print( SfxProgress& rProgress, PrintDialog* pPrintDialog = NULL );

    void            TestFunction( USHORT nPar );

    void            ConnectObject( SdrOle2Obj* pObj );
    BOOL            ActivateObject( SdrOle2Obj* pObj, long nVerb );

    static ScTabViewShell* GetActiveViewShell();
    SfxModelessDialog*  CreateRefDialog( SfxBindings* pB, SfxChildWindow* pCW,
                                        SfxChildWinInfo* pInfo,
                                        Window* pParent, USHORT nSlotId );

    void            UpdateOleZoom();
    inline SbxObject* GetScSbxObject() const
                    { return pScSbxObject; }
    inline void     SetScSbxObject( SbxObject* pOb )
                    { pScSbxObject = pOb; }

    FmFormShell*    GetFormShell() const    { return pFormShell; }

    void    InsertURL( const String& rName, const String& rURL, const String& rTarget,
                            USHORT nMode );
    void    InsertURLButton( const String& rName, const String& rURL, const String& rTarget,
                            const Point* pInsPos = NULL );
    void    InsertURLField( const String& rName, const String& rURL, const String& rTarget );

    BOOL    SelectObject( const String& rName );

    void    SetInFormatDialog(BOOL bFlag) {bInFormatDialog=bFlag;}
    BOOL    IsInFormatDialog() {return bInFormatDialog;}

    void    ForceMove()     { Move(); }

    void    MakeNumberInfoItem      ( ScDocument*            pDoc,
                                      ScViewData*            pViewData,
                                      SvxNumberInfoItem**    ppItem );

    void    UpdateNumberFormatter   ( ScDocument*               pDoc,
                                      const SvxNumberInfoItem&  rInfoItem );

    void    ExecuteCellFormatDlg    ( SfxRequest& rReq, USHORT nTabPage = 0xffff );

    BOOL    GetFunction( String& rFuncStr );

    void    StartSimpleRefDialog( const String& rTitle, const String& rInitVal,
                                    BOOL bCloseOnButtonUp );
    void    StopSimpleRefDialog();

    void    SetCurRefDlgId( USHORT nNew );

    void    AddAccessibilityObject( SfxListener& rObject );
    void    RemoveAccessibilityObject( SfxListener& rObject );
    void    BroadcastAccessibility( const SfxHint &rHint );
    BOOL    HasAccessibilityObjects();

};

//==================================================================


#endif

