/*************************************************************************
 *
 *  $RCSfile: transobj.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: nn $ $Date: 2001-05-11 18:26:59 $
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

#ifndef SC_TRANSOBJ_HXX
#define SC_TRANSOBJ_HXX

#ifndef _TRANSFER_HXX
#include <svtools/transfer.hxx>
#endif

#ifndef _EMBOBJ_HXX
#include <so3/embobj.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

class ScDocShell;
class ScMarkData;

namespace com { namespace sun { namespace star {
    namespace sheet {
        class XSheetCellRanges;
    }
}}}


class ScTransferObj : public TransferableHelper
{
private:
    ScDocument*                     pDoc;
    ScRange                         aBlock;
    TransferableDataHelper          aOleData;
    TransferableObjectDescriptor    aObjDesc;
    SvEmbeddedObjectRef             aDocShellRef;
    com::sun::star::uno::Reference<com::sun::star::sheet::XSheetCellRanges> xDragSourceRanges;
    USHORT                          nDragHandleX;
    USHORT                          nDragHandleY;
    USHORT                          nDragSourceFlags;
    BOOL                            bDragWasInternal;

    void        InitDocShell();
    static void StripRefs( ScDocument* pDoc, USHORT nStartX, USHORT nStartY,
                            USHORT nEndX, USHORT nEndY,
                            ScDocument* pDestDoc=0,
                            USHORT nSubX=0, USHORT nSubY=0 );
    static void PaintToDev( OutputDevice* pDev, ScDocument* pDoc, double nPrintFactor,
                            const ScRange& rBlock, BOOL bMetaFile );
    static void GetAreaSize( ScDocument* pDoc, USHORT nTab1, USHORT nTab2, USHORT& nRow, USHORT& nCol );

public:
            ScTransferObj( ScDocument* pClipDoc, const TransferableObjectDescriptor& rDesc );
    virtual ~ScTransferObj();

    virtual void        AddSupportedFormats();
    virtual sal_Bool    GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor );
    virtual sal_Bool    WriteObject( SotStorageStreamRef& rxOStm, void* pUserObject, sal_uInt32 nUserObjectId,
                                        const ::com::sun::star::datatransfer::DataFlavor& rFlavor );
    virtual void        ObjectReleased();
    virtual void        DragFinished( sal_Int8 nDropAction );

    ScDocument*         GetDocument()           { return pDoc; }        // owned by ScTransferObj
    const ScRange&      GetRange() const        { return aBlock; }
    USHORT              GetDragHandleX() const  { return nDragHandleX; }
    USHORT              GetDragHandleY() const  { return nDragHandleY; }
    USHORT              GetDragSourceFlags() const  { return nDragSourceFlags; }
    ScDocShell*         GetSourceDocShell();
    ScDocument*         GetSourceDocument();
    ScMarkData          GetSourceMarkData();

    void                SetDragHandlePos( USHORT nX, USHORT nY );
    void                SetDragSource( ScDocShell* pSourceShell, const ScMarkData& rMark );
    void                SetDragSourceFlags( USHORT nFlags );
    void                SetDragWasInternal();

    static ScTransferObj* GetOwnClipboard( Window* pUIWin );
};

#endif

