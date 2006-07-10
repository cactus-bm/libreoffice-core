/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbfunc.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 14:08:39 $
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

#ifndef SC_DBFUNC_HXX
#define SC_DBFUNC_HXX

#include "viewfunc.hxx"

struct ScSortParam;
struct ScQueryParam;
class ScDBData;
class ScDBCollection;
class ScDPObject;
class ScDPSaveData;
class StrCollection;
struct ScDPNumGroupInfo;

// ---------------------------------------------------------------------------

class ScDBFunc : public ScViewFunc
{
private:
    void            GetSelectedMemberList( StrCollection& rEntries, long& rDimension );

public:
                    ScDBFunc( Window* pParent, ScDocShell& rDocSh, ScTabViewShell* pViewShell );
                    ScDBFunc( Window* pParent, const ScDBFunc& rDBFunc, ScTabViewShell* pViewShell );
                    ~ScDBFunc();

                    //  nur UISort wiederholt bei Bedarf die Teilergebnisse

    void            UISort( const ScSortParam& rSortParam,
                          BOOL bRecord = TRUE );

    void            Sort( const ScSortParam& rSortParam,
                          BOOL bRecord = TRUE, BOOL bPaint = TRUE );
    void            Query( const ScQueryParam& rQueryParam,
                           const ScRange* pAdvSource, BOOL bRecord );
    void            DoSubTotals( const ScSubTotalParam& rParam, BOOL bRecord = TRUE,
                            const ScSortParam* pForceNewSort = NULL );

    void            ToggleAutoFilter();
    void            HideAutoFilter();

    void            RepeatDB( BOOL bRecord = TRUE );

    BOOL            ImportData( const ScImportParam& rParam, BOOL bRecord = TRUE );

    void            GotoDBArea( const String& rDBName );

                    // DB-Bereich vom Cursor
    ScDBData*       GetDBData( BOOL bMarkArea = TRUE, ScGetDBMode eMode = SC_DB_MAKE );

    void            NotifyCloseDbNameDlg( const ScDBCollection& rNewColl, const List& rDelAreaList );

    void            Consolidate( const ScConsolidateParam& rParam, BOOL bRecord = TRUE );

    void            MakePivotTable( const ScDPSaveData& rData, const ScRange& rDest, BOOL bNewTable,
                                    const ScDPObject& rSource, BOOL bApi = FALSE );
    void            DeletePivotTable();
    void            RecalcPivotTable();

    BOOL            HasSelectionForDateGroup( ScDPNumGroupInfo& rOldInfo, sal_Int32& rParts );
    BOOL            HasSelectionForNumGroup( ScDPNumGroupInfo& rOldInfo );
    void            GroupDataPilot();
    void            DateGroupDataPilot( const ScDPNumGroupInfo& rInfo, sal_Int32 nParts );
    void            NumGroupDataPilot( const ScDPNumGroupInfo& rInfo );
    void            UngroupDataPilot();
    void            DataPilotInput( const ScAddress& rPos, const String& rString );

    BOOL            HasSelectionForDrillDown( USHORT& rOrientation );
    void            SetDataPilotDetails( BOOL bShow, const String* pNewDimensionName = NULL );

    void            MakeOutline( BOOL bColumns, BOOL bRecord = TRUE );
    void            RemoveOutline( BOOL bColumns, BOOL bRecord = TRUE );
    void            RemoveAllOutlines( BOOL bRecord = TRUE );
    void            TestRemoveOutline( BOOL& rCol, BOOL& rRow );

    void            AutoOutline( BOOL bRecord = TRUE );

    void            SelectLevel( BOOL bColumns, USHORT nLevel,
                                    BOOL bRecord = TRUE, BOOL bPaint = TRUE );
    void            ShowOutline( BOOL bColumns, USHORT nLevel, USHORT nEntry,
                                    BOOL bRecord = TRUE, BOOL bPaint = TRUE );
    void            HideOutline( BOOL bColumns, USHORT nLevel, USHORT nEntry,
                                    BOOL bRecord = TRUE, BOOL bPaint = TRUE );

    void            ShowMarkedOutlines( BOOL bRecord = TRUE );
    void            HideMarkedOutlines( BOOL bRecord = TRUE );
    BOOL            OutlinePossible(BOOL bHide);

    void            UpdateCharts(BOOL bAllCharts = FALSE);      // Default: am Cursor

    static USHORT   DoUpdateCharts( const ScAddress& rPos, ScDocument* pDoc, Window* pActiveWin, BOOL bAllCharts );
};



#endif

