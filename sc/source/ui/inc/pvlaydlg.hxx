/*************************************************************************
 *
 *  $RCSfile: pvlaydlg.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-26 18:06:13 $
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

#ifndef SC_PVLAYDLG_HXX
#define SC_PVLAYDLG_HXX

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SCRBAR_HXX //autogen
#include <vcl/scrbar.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
#ifndef _MOREBTN_HXX //autogen
#include <vcl/morebtn.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

#ifndef SC_ANYREFDG_HXX
#include "anyrefdg.hxx"
#endif

#ifndef SC_FIELDWND_HXX
#include "fieldwnd.hxx"
#endif


/*==========================================================================*\

    Eine Instanz der Klasse ScPivotLayoutDlg ist ein (semi-)modaler
    Dialog, in dem mit der Maus Felder mit Spaltenueberschriften den
    drei Pivot-Kategorien "Spalte", "Zeile" und "Daten" zugeordnet
    werden koennen.

    Der Dialog erhaelt in der Struktur LabelData Informationen ueber
    diese Ueberschriften (Name, Art (Zahl/String) und Funktionsmaske).
    Weiterhin werden drei PivotFeld-Arrays uebergeben, mit denen die
    drei Kategorie-Fenster initialisiert werden. Ein Kategorie-Fenster
    wird durch eine Instanz der Klasse FieldWindow dargestellt. Ein
    solches Fenster ist fuer die Darstellung der Datenstrukturen am
    Schirm zustaendig. Es meldet Mausaktionen an den Dialog weiter und
    bietet entsprechende Methoden zur Veraenderung der Darstellung.
    Der Dialog sorgt fuer den Abgleich der interenen Datenstrukturen mit
    der Bildschirmdarstellung. Ein weiteres FieldWindow (Select) bietet
    alle Tabellenueberschriften zur Auswahl an, ist also "read-only".

\*==========================================================================*/

//============================================================================

class ScViewData;
class ScDocument;
class ScRangeData;
struct FuncData;
class ScDPObject;

//============================================================================

#define FUNC_COUNT 11

class ScDPLayoutDlg : public ScAnyRefDlg
{
public:
                            ScDPLayoutDlg(
                                SfxBindings* pB,
                                SfxChildWindow* pCW,
                                Window* pParent,
                                const ScDPObject* pDPObject );
    virtual                 ~ScDPLayoutDlg();

    virtual void            SetReference( const ScRange& rRef, ScDocument* pDoc );
    virtual BOOL            IsRefInputMode() const { return bRefInputMode; }
    virtual void            SetActive();
    virtual BOOL            Close();

    void                    NotifyDoubleClick    ( ScDPFieldType eType, long nFieldIndex );
    PointerStyle            NotifyMouseButtonDown( ScDPFieldType eType, long nFieldIndex );
    void                    NotifyMouseButtonUp  ( const Point& rAt );
    PointerStyle            NotifyMouseMove      ( const Point& rAt );
    void                    NotifyFieldFocus     ( ScDPFieldType eType, BOOL bGotFocus );
    void                    NotifyMoveField      ( ScDPFieldType eToType );
    void                    NotifyRemoveField    ( ScDPFieldType eType, long nFieldIndex );
    BOOL                    NotifyMoveSlider     ( USHORT nKeyCode );   // return TRUE, if position changed

protected:
    virtual void            Deactivate();

private:
    FixedLine               aFlLayout;
    FixedText               aFtCol;
    ScDPFieldWindow         aWndCol;
    FixedText               aFtRow;
    ScDPFieldWindow         aWndRow;
    FixedText               aFtData;
    ScDPFieldWindow         aWndData;
    ScDPFieldWindow         aWndSelect;
    ScrollBar               aSlider;
    FixedInfo               aFtInfo;

    FixedLine               aFlAreas;
    ListBox                 aLbOutPos;
    FixedText               aFtOutArea;
    ScRefEdit               aEdOutPos;
    ScRefButton             aRbOutPos;
    CheckBox                aBtnIgnEmptyRows;
    CheckBox                aBtnDetectCat;
    CheckBox                aBtnTotalCol;
    CheckBox                aBtnTotalRow;

    OKButton                aBtnOk;
    CancelButton            aBtnCancel;
    HelpButton              aBtnHelp;
    PushButton              aBtnRemove;
    PushButton              aBtnOptions;
    MoreButton              aBtnMore;

    const String            aStrUndefined;
    const String            aStrNewTable;
    String*                 aFuncNameArr[FUNC_COUNT];

    ScDPFieldType           eDnDFromType;
    long                    nDnDFromIndex;
    BOOL                    bIsDrag;

    Rectangle               aRectRow;
    Rectangle               aRectCol;
    Rectangle               aRectData;
    Rectangle               aRectSelect;

    LabelData**             aLabelDataArr; // (nCol, Feldname, Zahl/Text)
    long                    nLabelCount;

    ScDPFieldType           eLastActiveType;        /// Type of last active area.
    long                    nOffset;                /// Offset of first field in TYPE_SELECT area.

    FuncData*               aSelectArr[MAX_LABELS]; // (nCol, nFuncMask)
    FuncData*               aColArr   [MAX_FIELDS]; // (nCol, nFuncMask)
    FuncData*               aRowArr   [MAX_FIELDS]; // (nCol, nFuncMask)
    FuncData*               aDataArr  [MAX_FIELDS]; // (nCol, nFuncMask)

    BOOL                    bShowAll[MAX_LABELS];

    ScDPObject*             pDlgDPObject;
    ScPivotParam            thePivotData;
    ScViewData*             pViewData;
    ScDocument*             pDoc;
    BOOL                    bRefInputMode;

#ifdef _PVLAYDLG_CXX
private:
    ScDPFieldWindow&        GetFieldWindow  ( ScDPFieldType eType );
    void                    Init            ();
    void                    InitWndSelect   ( LabelData** ppLabelArr, long nLabels );
    void                    InitWnd         ( PivotField* pArr, long nCount, ScDPFieldType eType );
    void                    InitFocus       ();
    void                    CalcWndSizes    ();
    Point                   DlgPos2WndPos   ( const Point& rPt, Window& rWnd );
    LabelData*              GetLabelData    ( short nCol, long* pPos = NULL );
    String                  GetLabelString  ( short nCol );
    String                  GetFuncString   ( USHORT& rFuncMask, BOOL bIsValue = TRUE );
    BOOL                    Contains        ( FuncData** pArr, short nCol, long& nAt );
    void                    Remove          ( FuncData** pArr, long nAt );
    void                    Insert          ( FuncData** pArr, const FuncData& rFData, long nAt );

    void                    AddField        ( long nFromIndex,
                                              ScDPFieldType eToType, const Point& rAtPos );
    void                    MoveField       ( ScDPFieldType eFromType, long nFromIndex,
                                              ScDPFieldType eToType, const Point&  rAtPos );
    void                    RemoveField     ( ScDPFieldType eRemType, long nRemIndex );

    BOOL                    GetPivotArrays  ( PivotField*   pColArr,
                                              PivotField*   pRowArr,
                                              PivotField*   pDataArr,
                                              USHORT&       rColCount,
                                              USHORT&       rRowCount,
                                              USHORT&       rDataCount );
    // Handler
    DECL_LINK( ClickHdl, PushButton * );
    DECL_LINK( ScrollHdl, ScrollBar * );
    DECL_LINK( SelAreaHdl, ListBox * );
    DECL_LINK( MoreClickHdl, MoreButton * );
    DECL_LINK( EdModifyHdl, Edit * );
    DECL_LINK( OkHdl, OKButton * );
    DECL_LINK( CancelHdl, CancelButton * );
#endif
};



#endif // SC_PVLAYDLG_HXX

