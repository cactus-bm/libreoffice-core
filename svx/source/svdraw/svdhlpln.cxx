/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdhlpln.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 13:10:10 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"

#include "svdhlpln.hxx"

#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

#ifndef _OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif

#ifndef _SV_WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif

#ifndef _SV_LINEINFO_HXX
#include <vcl/lineinfo.hxx>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

Pointer SdrHelpLine::GetPointer() const
{
    switch (eKind) {
        case SDRHELPLINE_VERTICAL  : return Pointer(POINTER_ESIZE);
        case SDRHELPLINE_HORIZONTAL: return Pointer(POINTER_SSIZE);
        default                    : return Pointer(POINTER_MOVE);
    } // switch
}

// #i27493#
// Helper method to draw a hor or ver two-colored dashed line
void SdrHelpLine::ImpDrawDashedTwoColorLine(OutputDevice& rOut, sal_Int32 nStart, sal_Int32 nEnd, sal_Int32 nFixPos,
    sal_Int32 nStepWidth, Color aColA, Color aColB, sal_Bool bHorizontal) const
{
    for(sal_Int32 a(nStart), b(0L); a < nEnd; a += nStepWidth, b++)
    {
        rOut.SetLineColor((b % 2) ? aColA : aColB);
        sal_Int32 nNextPos(a + nStepWidth - 1L);
        if(nNextPos > nEnd)
            nNextPos = nEnd;
        if(bHorizontal)
            rOut.DrawLine(Point(a, nFixPos), Point(nNextPos, nFixPos));
        else
            rOut.DrawLine(Point(nFixPos, a), Point(nFixPos, nNextPos));
    }
}

void SdrHelpLine::Draw(OutputDevice& rOut, const Point& rOfs) const
{
    // #i27493# New implementation for 8. Will be replaced later when we have overlays.
    Point aPnt(rOut.LogicToPixel(aPos + rOfs));
    const sal_Int32 xPos(aPnt.X());
    const sal_Int32 yPos(aPnt.Y());
    const sal_Bool bOldMapModeWasEnabled(rOut.IsMapModeEnabled());
    const Size aOutSizePixel(rOut.GetOutputSizePixel());
    rOut.EnableMapMode(sal_False);

    switch(eKind)
    {
        case SDRHELPLINE_VERTICAL :
        {
            ImpDrawDashedTwoColorLine(rOut, 0L, aOutSizePixel.Height(),
                xPos, 4L , Color(COL_BLACK), Color(COL_WHITE), sal_False);
            break;
        }

        case SDRHELPLINE_HORIZONTAL :
        {
            ImpDrawDashedTwoColorLine(rOut, 0L, aOutSizePixel.Width(),
                yPos, 4L , Color(COL_BLACK), Color(COL_WHITE), sal_True);
            break;
        }

        case SDRHELPLINE_POINT :
        {
            ImpDrawDashedTwoColorLine(rOut, xPos - SDRHELPLINE_POINT_PIXELSIZE, xPos + SDRHELPLINE_POINT_PIXELSIZE,
                yPos, 4L , Color(COL_BLACK), Color(COL_WHITE), sal_True);
            ImpDrawDashedTwoColorLine(rOut, yPos - SDRHELPLINE_POINT_PIXELSIZE, yPos + SDRHELPLINE_POINT_PIXELSIZE,
                xPos, 4L , Color(COL_BLACK), Color(COL_WHITE), sal_False);
            break;
        }
    }

    rOut.EnableMapMode(bOldMapModeWasEnabled);
}

FASTBOOL SdrHelpLine::IsHit(const Point& rPnt, USHORT nTolLog, const OutputDevice& rOut) const
{
    Size a1Pix(rOut.PixelToLogic(Size(1,1)));
    FASTBOOL bXHit=rPnt.X()>=aPos.X()-nTolLog && rPnt.X()<=aPos.X()+nTolLog+a1Pix.Width();
    FASTBOOL bYHit=rPnt.Y()>=aPos.Y()-nTolLog && rPnt.Y()<=aPos.Y()+nTolLog+a1Pix.Height();
    switch (eKind) {
        case SDRHELPLINE_VERTICAL  : return bXHit;
        case SDRHELPLINE_HORIZONTAL: return bYHit;
        case SDRHELPLINE_POINT: {
            if (bXHit || bYHit) {
                Size aRad(rOut.PixelToLogic(Size(SDRHELPLINE_POINT_PIXELSIZE,SDRHELPLINE_POINT_PIXELSIZE)));
                return rPnt.X()>=aPos.X()-aRad.Width() && rPnt.X()<=aPos.X()+aRad.Width()+a1Pix.Width() &&
                       rPnt.Y()>=aPos.Y()-aRad.Height() && rPnt.Y()<=aPos.Y()+aRad.Height()+a1Pix.Height();
            }
        } break;
    } // switch
    return FALSE;
}

Rectangle SdrHelpLine::GetBoundRect(const OutputDevice& rOut) const
{
    Rectangle aRet(aPos,aPos);
    Point aOfs(rOut.GetMapMode().GetOrigin());
    Size aSiz(rOut.GetOutputSize());
    switch (eKind) {
        case SDRHELPLINE_VERTICAL  : aRet.Top()=-aOfs.Y(); aRet.Bottom()=-aOfs.Y()+aSiz.Height(); break;
        case SDRHELPLINE_HORIZONTAL: aRet.Left()=-aOfs.X(); aRet.Right()=-aOfs.X()+aSiz.Width();  break;
        case SDRHELPLINE_POINT     : {
            Size aRad(rOut.PixelToLogic(Size(SDRHELPLINE_POINT_PIXELSIZE,SDRHELPLINE_POINT_PIXELSIZE)));
            aRet.Left()  -=aRad.Width();
            aRet.Right() +=aRad.Width();
            aRet.Top()   -=aRad.Height();
            aRet.Bottom()+=aRad.Height();
        } break;
    } // switch
    return aRet;
}

bool SdrHelpLine::IsVisibleEqual( const SdrHelpLine& rHelpLine, const OutputDevice& rOut ) const
{
    if( eKind == rHelpLine.eKind)
    {
        Point aPt1(rOut.LogicToPixel(aPos)), aPt2(rOut.LogicToPixel(rHelpLine.aPos));
        switch( eKind )
        {
            case SDRHELPLINE_POINT:
                return aPt1 == aPt2;
            case SDRHELPLINE_VERTICAL:
                return aPt1.X() == aPt2.X();
            case SDRHELPLINE_HORIZONTAL:
                return aPt1.Y() == aPt2.Y();
        }
    }
    return false;
}

//BFS01SvStream& operator<<(SvStream& rOut, const SdrHelpLine& rHL)
//BFS01{
//BFS01 SdrIOHeader aHead(rOut,STREAM_WRITE,SdrIOHlpLID);
//BFS01 rOut<<UINT16(rHL.eKind);
//BFS01 rOut<<rHL.aPos;
//BFS01 return rOut;
//BFS01}

//BFS01SvStream& operator>>(SvStream& rIn, SdrHelpLine& rHL)
//BFS01{
//BFS01 SdrIOHeader aHead(rIn,STREAM_READ);
//BFS01 UINT16 nDum;
//BFS01 rIn>>nDum;
//BFS01 rHL.eKind=(SdrHelpLineKind)nDum;
//BFS01 rIn>>rHL.aPos;
//BFS01 return rIn;
//BFS01}

void SdrHelpLineList::Clear()
{
    USHORT nAnz=GetCount();
    for (USHORT i=0; i<nAnz; i++) {
        delete GetObject(i);
    }
    aList.Clear();
}

void SdrHelpLineList::operator=(const SdrHelpLineList& rSrcList)
{
    Clear();
    USHORT nAnz=rSrcList.GetCount();
    for (USHORT i=0; i<nAnz; i++) {
        Insert(rSrcList[i]);
    }
}

bool SdrHelpLineList::operator==(const SdrHelpLineList& rSrcList) const
{
    FASTBOOL bEqual=FALSE;
    USHORT nAnz=GetCount();
    if (nAnz==rSrcList.GetCount()) {
        bEqual=TRUE;
        for (USHORT i=0; i<nAnz && bEqual; i++) {
            if (*GetObject(i)!=*rSrcList.GetObject(i)) {
                bEqual=FALSE;
            }
        }
    }
    return bEqual;
}

void SdrHelpLineList::DrawAll(OutputDevice& rOut, const Point& rOfs) const
{

    sal_uInt16 nAnz = GetCount();
    sal_uInt16 i,j;
    SdrHelpLine *pHL, *pHL2;

    for(i=0; i<nAnz; i++)
    {
        pHL = GetObject(i);

        // check if we already drawn a help line like this one
        if( pHL )
        {
            for(j=0;j<i;j++)
            {
                pHL2 = GetObject(j);
                if( pHL2 && pHL->IsVisibleEqual( *pHL2, rOut) )
                {
                    pHL = NULL;
                    break;
                }
            }
        }

        if( pHL )
            pHL->Draw(rOut,rOfs);
    }
}

USHORT SdrHelpLineList::HitTest(const Point& rPnt, USHORT nTolLog, const OutputDevice& rOut) const
{
    USHORT nAnz=GetCount();
    for (USHORT i=nAnz; i>0;) {
        i--;
        if (GetObject(i)->IsHit(rPnt,nTolLog,rOut)) return i;
    }
    return SDRHELPLINE_NOTFOUND;
}

//BFS01SvStream& operator<<(SvStream& rOut, const SdrHelpLineList& rHLL)
//BFS01{
//BFS01 SdrIOHeader aHead(rOut,STREAM_WRITE,SdrIOHLstID);
//BFS01 USHORT nAnz=rHLL.GetCount();
//BFS01 rOut<<nAnz;
//BFS01 for (USHORT i=0; i<nAnz; i++) {
//BFS01     rOut<<rHLL[i];
//BFS01 }
//BFS01 return rOut;
//BFS01}

//BFS01SvStream& operator>>(SvStream& rIn, SdrHelpLineList& rHLL)
//BFS01{
//BFS01 SdrIOHeader aHead(rIn,STREAM_READ);
//BFS01 rHLL.Clear();
//BFS01 USHORT nAnz;
//BFS01 rIn>>nAnz;
//BFS01 for (USHORT i=0; i<nAnz; i++) {
//BFS01     SdrHelpLine* pHL=new SdrHelpLine;
//BFS01     rIn>>*pHL;
//BFS01     rHLL.aList.Insert(pHL,CONTAINER_APPEND);
//BFS01 }
//BFS01 return rIn;
//BFS01}

// eof
