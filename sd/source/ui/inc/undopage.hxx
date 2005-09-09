/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: undopage.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:58:09 $
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

#ifndef _SD_UNDOPAGE_HXX
#define _SD_UNDOPAGE_HXX

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
#ifndef _SV_PRNTYPES_HXX //autogen
#include <vcl/prntypes.hxx>
#endif

#include "sdundo.hxx"

class SdDrawDocument;
class SdPage;

/************************************************************************/

class SdPageFormatUndoAction : public SdUndoAction
{
    SdPage*     pPage;

    Size        aOldSize;
    INT32       nOldLeft;
    INT32       nOldRight;
    INT32       nOldUpper;
    INT32       nOldLower;
    BOOL        bOldScale;
    Orientation eOldOrientation;
    USHORT      nOldPaperBin;
    BOOL        bOldFullSize;

    Size        aNewSize;
    INT32       nNewLeft;
    INT32       nNewRight;
    INT32       nNewUpper;
    INT32       nNewLower;
    BOOL        bNewScale;
    Orientation eNewOrientation;
    USHORT      nNewPaperBin;
    BOOL        bNewFullSize;

public:
    TYPEINFO();
    SdPageFormatUndoAction( SdDrawDocument* pDoc,
                            SdPage*         pThePage,
                            const Size&     rOldSz,
                            INT32           nOldLft,
                            INT32           nOldRgt,
                            INT32           nOldUpr,
                            INT32           nOldLwr,
                            BOOL            bOldScl,
                            Orientation     eOldOrient,
                            USHORT          nOPaperBin,
                            BOOL            bOFullSize,

                            const Size&     rNewSz,
                            INT32           nNewLft,
                            INT32           nNewRgt,
                            INT32           nNewUpr,
                            INT32           nNewLwr,
                            BOOL            bNewScl,
                            Orientation     eNewOrient,
                            USHORT          nNPaperBin,
                            BOOL            bNFullSize
                            ) :
        SdUndoAction(pDoc),
        pPage       (pThePage),
        aOldSize    (rOldSz),
        nOldLeft    (nOldLft),
        nOldRight   (nOldRgt),
        nOldUpper   (nOldUpr),
        nOldLower   (nOldLwr),
        bOldScale   (bOldScl),
        eOldOrientation(eOldOrient),
        nOldPaperBin (nOPaperBin),
        bOldFullSize (bOFullSize),


        aNewSize    (rNewSz),
        nNewLeft    (nNewLft),
        nNewRight   (nNewRgt),
        nNewUpper   (nNewUpr),
        nNewLower   (nNewLwr),
        bNewScale   (bNewScl),
        eNewOrientation(eNewOrient),
        nNewPaperBin (nNPaperBin),
        bNewFullSize (bNFullSize)

        {}
    virtual ~SdPageFormatUndoAction();

    virtual void Undo();
    virtual void Redo();
    virtual void Repeat();
};

/************************************************************************/

class SdPageLRUndoAction : public SdUndoAction
{
    SdPage* pPage;

    INT32   nOldLeft;
    INT32   nOldRight;
    INT32   nNewLeft;
    INT32   nNewRight;

public:
    TYPEINFO();
    SdPageLRUndoAction( SdDrawDocument* pDoc, SdPage* pThePage,
                        INT32 nOldLft, INT32 nOldRgt,
                        INT32 nNewLft, INT32 nNewRgt ) :
        SdUndoAction(pDoc),
        pPage       (pThePage),
        nOldLeft    (nOldLft),
        nOldRight   (nOldRgt),
        nNewLeft    (nNewLft),
        nNewRight   (nNewRgt)
        {}
    virtual ~SdPageLRUndoAction();

    virtual void Undo();
    virtual void Redo();
    virtual void Repeat();
};

/************************************************************************/

class SdPageULUndoAction : public SdUndoAction
{
    SdPage* pPage;

    INT32   nOldUpper;
    INT32   nOldLower;
    INT32   nNewUpper;
    INT32   nNewLower;

public:
    TYPEINFO();
    SdPageULUndoAction( SdDrawDocument* pDoc, SdPage* pThePage,
                        INT32 nOldUpr, INT32 nOldLwr,
                        INT32 nNewUpr, INT32 nNewLwr ) :
        SdUndoAction(pDoc),
        pPage       (pThePage),
        nOldUpper   (nOldUpr),
        nOldLower   (nOldLwr),
        nNewUpper   (nNewUpr),
        nNewLower   (nNewLwr)
        {}
    virtual ~SdPageULUndoAction();

    virtual void Undo();
    virtual void Redo();
    virtual void Repeat();
};



#endif      // _SD_UNDOPAGE_HXX

