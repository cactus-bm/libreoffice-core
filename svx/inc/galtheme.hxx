/*************************************************************************
 *
 *  $RCSfile: galtheme.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 17:25:54 $
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

#ifndef _SVX_GALTHEME_HXX_
#define _SVX_GALTHEME_HXX_

#define ENABLE_BYTESTRING_STREAM_OPERATORS

#include <tools/debug.hxx>
#include <tools/urlobj.hxx>
#include <vcl/salctype.hxx>
#include <svtools/brdcst.hxx>
#include <svtools/lstner.hxx>
#include <svtools/transfer.hxx>
#include <so3/svstor.hxx>
#include "galobj.hxx"
#include "galmisc.hxx"
#include "gallery1.hxx"

// -----------------
// - GalleryObject -
// -----------------

struct GalleryObject
{
    INetURLObject   aURL;
    sal_uInt32      nOffset;
    SgaObjKind      eObjKind;
    BOOL            bDummy;
};

DECLARE_LIST( GalleryObjectList, GalleryObject* );

// -----------------
// - GalDragParams -
// -----------------

struct GalDragParams
{
    Region      aDragRegion;
    ULONG       nDragObjPos;
    String      aThemeName;
    String      aFileName;
    SgaObjKind  eObjKind;
};

// ----------------
// - GalleryTheme -
// ----------------

class Gallery;
class GalleryProgress;

class GalleryTheme : public SfxBroadcaster
{
    friend class Gallery;
    friend class GalleryThemeCacheEntry;

private:

    GalleryObjectList           aObjectList;
    String                      aImportName;
    SvStorageRef                aSvDrawStorageRef;
    Gallery*                    pParent;
    GalleryThemeEntry*          pThm;
    ULONG                       mnLockCount;
    ULONG                       nDragPos;
    BOOL                        bDragging;
    BOOL                        bAbortActualize;

    void                        ImplCreateSvDrawStorage();
    SgaObject*                  ImplReadSgaObject( GalleryObject* pEntry );
    BOOL                        ImplWriteSgaObject( const SgaObject& rObj, ULONG nPos, GalleryObject* pExistentEntry );
    void                        ImplRead();
    void                        ImplWrite();
    const GalleryObject*        ImplGetGalleryObject( ULONG nPos ) const { return aObjectList.GetObject( nPos ); }
    INetURLObject               ImplGetURL( const GalleryObject* pObject ) const;
    INetURLObject               ImplCreateUniqueURL( SgaObjKind eObjKind, ULONG nFormat = CVT_UNKNOWN );
    void                        ImplSetModified( BOOL bModified ) { pThm->SetModified( bModified ); }
    void                        ImplBroadcast( ULONG nUpdatePos );

                                GalleryTheme();
                                GalleryTheme( Gallery* pGallery, GalleryThemeEntry* pThemeEntry );
                                ~GalleryTheme();

public:

    static GalleryThemeEntry*   CreateThemeEntry( const INetURLObject& rURL, BOOL bReadOnly );

    const ULONG                 GetObjectCount() const { return aObjectList.Count(); }

    SgaObject*                  AcquireObject( ULONG nPos );
    void                        ReleaseObject( SgaObject* pObj );

    BOOL                        InsertObject( const SgaObject& rObj, ULONG nPos = LIST_APPEND );
    BOOL                        RemoveObject( ULONG nPos );
    BOOL                        ChangeObjectPos( ULONG nOldPos, ULONG nNewPos );

    const String&               GetName() const { return IsImported() ? aImportName : pThm->GetThemeName(); }
    const String&               GetRealName() const { return pThm->GetThemeName(); }
    const String&               GetImportName() const { return aImportName; }
    void                        SetImportName(const String& rImportName) { aImportName = rImportName; }

    const INetURLObject&        GetThmURL() const { return pThm->GetThmURL(); }
    const INetURLObject&        GetSdgURL() const { return pThm->GetSdgURL(); }
    const INetURLObject&        GetSdvURL() const { return pThm->GetSdvURL(); }

    UINT32                      GetId() const { return pThm->GetId(); }
    void                        SetId( UINT32 nNewId, BOOL bResetThemeName ) { pThm->SetId( nNewId, bResetThemeName ); }

    void                        SetDragging( BOOL bSet ) { bDragging = bSet; }
    BOOL                        IsDragging() const { return bDragging; }

    void                        LockBroadcaster() { mnLockCount++; }
    void                        UnlockBroadcaster( ULONG nUpdatePos = 0 );
    BOOL                        IsBroadcasterLocked() const { return mnLockCount > 0; }

    void                        SetDragPos( ULONG nPos ) { nDragPos = nPos; }
    ULONG                       GetDragPos() const { return nDragPos; }

    BOOL                        IsThemeNameFromResource() const { return pThm->IsNameFromResource(); }

    BOOL                        IsImported() const { return pThm->IsImported(); }
    BOOL                        IsReadOnly() const { return pThm->IsReadOnly(); }
    BOOL                        IsDefault() const { return pThm->IsDefault(); }
    BOOL                        IsModified() const { return pThm->IsModified(); }

    void                        Actualize( const Link& rActualizeLink, GalleryProgress* pProgress = NULL );
    void                        AbortActualize() { bAbortActualize = TRUE; }

    Gallery*                    GetParent() const { return pParent; }
    SvStorageRef                GetSvDrawStorage() const { return aSvDrawStorageRef; }

public:

    SgaObjKind                  GetObjectKind( ULONG nPos ) const
                                {
                                    DBG_ASSERT( nPos < GetObjectCount(), "Position out of range" );
                                    return ImplGetGalleryObject( nPos )->eObjKind;
                                }


    const INetURLObject&        GetObjectURL( ULONG nPos ) const
                                {
                                    DBG_ASSERT( nPos < GetObjectCount(), "Position out of range" );
                                    return ImplGetGalleryObject( nPos )->aURL;
                                }

    BOOL                        GetThumb( ULONG nPos, Bitmap& rBmp, BOOL bProgress = FALSE );

    BOOL                        GetGraphic( ULONG nPos, Graphic& rGraphic, BOOL bProgress = FALSE );
    BOOL                        InsertGraphic( const Graphic& rGraphic, ULONG nInsertPos = LIST_APPEND );

    BOOL                        GetModel( ULONG nPos, FmFormModel& rModel, BOOL bProgress = FALSE );
    BOOL                        InsertModel( const FmFormModel& rModel, ULONG nInsertPos = LIST_APPEND );

    BOOL                        GetModelStream( ULONG nPos, SotStorageStreamRef& rModelStreamRef, BOOL bProgress = FALSE );
    BOOL                        InsertModelStream( const SotStorageStreamRef& rModelStream, ULONG nInsertPos = LIST_APPEND );

    BOOL                        GetURL( ULONG nPos, INetURLObject& rURL, BOOL bProgress = FALSE );
    BOOL                        InsertURL( const INetURLObject& rURL, ULONG nInsertPos = LIST_APPEND );
    BOOL                        InsertFileOrDirURL( const INetURLObject& rFileOrDirURL, ULONG nInsertPos = LIST_APPEND );

    BOOL                        InsertTransferable( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& rxTransferable, ULONG nInsertPos );

    void                        CopyToClipboard( Window* pWindow, ULONG nPos );
    void                        StartDrag( Window* pWindow, ULONG nPos );

public:

    SvStream&                   WriteData( SvStream& rOut ) const;
    SvStream&                   ReadData( SvStream& rIn );
};

SvStream& operator<<( SvStream& rOut, const GalleryTheme& rTheme );
SvStream& operator>>( SvStream& rIn, GalleryTheme& rTheme );

#endif
