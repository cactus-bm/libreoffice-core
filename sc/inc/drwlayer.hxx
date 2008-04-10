/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: drwlayer.hxx,v $
 * $Revision: 1.21 $
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

#ifndef SC_DRWLAYER_HXX
#define SC_DRWLAYER_HXX

#include <vcl/graph.hxx>
#ifndef _FM_FMMODEL_HXX
#include <svx/fmmodel.hxx>
#endif
#include "global.hxx"

class ScDocument;
class SfxViewShell;
class SfxObjectShell;
class ScDrawObjData;
class ScIMapInfo;
class ScMacroInfo;
class IMapObject;
class ScMarkData;
class SdrOle2Obj;
class ScRange;
class ScAddress;

// -----------------------------------------------------------------------

class ScTabDeletedHint : public SfxHint
{
private:
    SCTAB   nTab;
public:
            TYPEINFO();
            ScTabDeletedHint( SCTAB nTabNo = SCTAB_MAX );
    virtual ~ScTabDeletedHint();

    SCTAB   GetTab()    { return nTab; }
};

class ScTabSizeChangedHint : public SfxHint
{
private:
    SCTAB   nTab;
public:
            TYPEINFO();
            ScTabSizeChangedHint( SCTAB nTabNo = SCTAB_MAX );
    virtual ~ScTabSizeChangedHint();

    SCTAB   GetTab()    { return nTab; }
};

// -----------------------------------------------------------------------


class ScDrawLayer: public FmFormModel
{
private:
//REMOVE        SotStorageRef   xPictureStorage;
    String          aName;
    ScDocument*     pDoc;
    SdrUndoGroup*   pUndoGroup;
    BOOL            bRecording;
    BOOL            bAdjustEnabled;
    BOOL            bHyphenatorSet;

private:
    void            MoveAreaTwips( SCTAB nTab, const Rectangle& rArea, const Point& rMove,
                                const Point& rTopLeft );
    void            MoveCells( SCTAB nTab, SCCOL nCol1,SCROW nRow1, SCCOL nCol2,SCROW nRow2,
                                SCsCOL nDx,SCsROW nDy );
    void            RecalcPos( SdrObject* pObj, ScDrawObjData* pData, BOOL bNegativePage );

public:
                    ScDrawLayer( ScDocument* pDocument, const String& rName );
    virtual         ~ScDrawLayer();

    virtual SdrPage*  AllocPage(FASTBOOL bMasterPage);
    virtual SdrModel* AllocModel() const;
    virtual void    SetChanged( sal_Bool bFlg = sal_True );

    virtual Window* GetCurDocViewWin();
    virtual SvStream* GetDocumentStream(SdrDocumentStreamInfo& rStreamInfo) const;

    virtual SdrLayerID GetControlExportLayerId( const SdrObject & ) const;

//REMOVE        void            ReleasePictureStorage();

    BOOL            HasObjects() const;

    BOOL            ScAddPage( SCTAB nTab );
    void            ScRemovePage( SCTAB nTab );
    void            ScRenamePage( SCTAB nTab, const String& rNewName );
    void            ScMovePage( USHORT nOldPos, USHORT nNewPos );
                    // inkl. Inhalt, bAlloc=FALSE -> nur Inhalt
    void            ScCopyPage( USHORT nOldPos, USHORT nNewPos, BOOL bAlloc );

    ScDocument*     GetDocument() const { return pDoc; }

    void            UpdateBasic();              // DocShell-Basic in DrawPages setzen
    void            UseHyphenator();

    BOOL            GetPrintArea( ScRange& rRange, BOOL bSetHor, BOOL bSetVer ) const;

                    //      automatische Anpassungen

    void            EnableAdjust( BOOL bSet = TRUE )    { bAdjustEnabled = bSet; }

    void            BeginCalcUndo();
    SdrUndoGroup*   GetCalcUndo();
    BOOL            IsRecording()           { return bRecording; }
    void            AddCalcUndo( SdrUndoAction* pUndo );

    void            MoveArea( SCTAB nTab, SCCOL nCol1,SCROW nRow1, SCCOL nCol2,SCROW nRow2,
                                SCsCOL nDx,SCsROW nDy, BOOL bInsDel );
    void            WidthChanged( SCTAB nTab, SCCOL nCol, long nDifTwips );
    void            HeightChanged( SCTAB nTab, SCROW nRow, long nDifTwips );

    BOOL            HasObjectsInRows( SCTAB nTab, SCROW nStartRow, SCROW nEndRow );

    void            DeleteObjectsInArea( SCTAB nTab, SCCOL nCol1,SCROW nRow1,
                                            SCCOL nCol2,SCROW nRow2 );
    void            DeleteObjectsInSelection( const ScMarkData& rMark );
    void            DeleteObjects( SCTAB nTab );

    void            CopyToClip( ScDocument* pClipDoc, SCTAB nTab, const Rectangle& rRange );
    void            CopyFromClip( ScDrawLayer* pClipModel,
                                    SCTAB nSourceTab, const Rectangle& rSourceRange,
                                    const ScAddress& rDestPos, const Rectangle& rDestRange );

    void            SetPageSize( USHORT nPageNo, const Size& rSize );
    ULONG           GetDefTextHeight() const;

                    //  mirror or move between positive and negative positions for RTL
    void            MirrorRTL( SdrObject* pObj );
    static void     MirrorRectRTL( Rectangle& rRect );      // for bounding rectangles etc.

                    //  GetVisibleName: name for navigator etc: GetPersistName or GetName
                    //  (ChartListenerCollection etc. must use GetPersistName directly)
    static String   GetVisibleName( SdrObject* pObj );

    SdrObject*      GetNamedObject( const String& rName, USHORT nId, SCTAB& rFoundTab ) const;
                    // if pnCounter != NULL, the search for a name starts with this index + 1,
                    // and the index really used is returned.
    String          GetNewGraphicName( long* pnCounter = NULL ) const;
    void            EnsureGraphicNames();

    // Verankerung setzen und ermitteln
    static void     SetAnchor( SdrObject*, ScAnchorType );
    static ScAnchorType GetAnchor( const SdrObject* );

    // Positionen fuer Detektivlinien
    static ScDrawObjData* GetObjData( SdrObject* pObj, BOOL bCreate=FALSE );

    // The sheet information in ScDrawObjData isn't updated when sheets are inserted/deleted.
    // Use this method to get an object with positions on the specified sheet (should be the
    // sheet on which the object is inserted).
    static ScDrawObjData* GetObjDataTab( SdrObject* pObj, SCTAB nTab );

    // Image-Map
    static ScIMapInfo* GetIMapInfo( SdrObject* pObj );

    static IMapObject* GetHitIMapObject( SdrObject* pObject,
                            const Point& rWinPoint, const Window& rCmpWnd );

    static ScMacroInfo* GetMacroInfo( SdrObject* pObj, BOOL bCreate = FALSE );

private:
    static SfxObjectShell* pGlobalDrawPersist;          // fuer AllocModel
public:
    static void     SetGlobalDrawPersist(SfxObjectShell* pPersist);
protected:
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > createUnoModel();
};


#endif


