/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svddrgm1.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 16:34:51 $
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

#ifndef _SVDDRGM1_HXX
#define _SVDDRGM1_HXX

#ifndef _XPOLY_HXX
#include "xpoly.hxx"
#endif

#ifndef _SVDHDL_HXX
#include "svdhdl.hxx"
#endif

#ifndef _SVDDRGV_HXX
#include "svddrgv.hxx"
#endif

#ifndef _SVDDRGMT_HXX
#include "svddrgmt.hxx"
#endif

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SdrDragView;
class SdrDragStat;

//************************************************************
//   SdrDragMovHdl
//************************************************************

class SdrDragMovHdl : public SdrDragMethod
{
    FASTBOOL                    bMirrObjShown;

public:
    TYPEINFO();
    SdrDragMovHdl(SdrDragView& rNewView): SdrDragMethod(rNewView), bMirrObjShown(FALSE) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual void Brk();
    virtual Pointer GetPointer() const;
    virtual void Show();
    virtual void Hide();
};

//************************************************************
//   SdrDragObjOwn
//************************************************************

class SdrDragObjOwn : public SdrDragMethod
{
public:
    TYPEINFO();
    SdrDragObjOwn(SdrDragView& rNewView): SdrDragMethod(rNewView) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
    virtual void DrawXor(XOutputDevice& rXOut, FASTBOOL bFull) const;

    //SJ: added following method, otherwise the object won't be
    //able to notice that dragging has been cancelled.
    virtual void Brk();
};

//************************************************************
//   SdrDragMove
//************************************************************

class SdrDragMove : public SdrDragMethod
{
    long                        nBestXSnap;
    long                        nBestYSnap;
    FASTBOOL                    bXSnapped;
    FASTBOOL                    bYSnapped;

private:
    void ImpCheckSnap(const Point& rPt);

public:
    TYPEINFO();
    SdrDragMove(SdrDragView& rNewView): SdrDragMethod(rNewView) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovAllPoints();
    virtual void MovPoint(Point& rPnt, const Point& rPvOfs);
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual FASTBOOL IsMoveOnly() const;
    virtual Pointer GetPointer() const;
};

//************************************************************
//   SdrDragResize
//************************************************************

class SdrDragResize : public SdrDragMethod
{
protected:
    Fraction                    aXFact;
    Fraction                    aYFact;

public:
    TYPEINFO();
    SdrDragResize(SdrDragView& rNewView): SdrDragMethod(rNewView), aXFact(1,1), aYFact(1,1) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovPoint(Point& rPnt, const Point& rPvOfs);
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
};

//************************************************************
//   SdrDragRotate
//************************************************************

class SdrDragRotate : public SdrDragMethod
{
protected:
    double                      nSin;
    double                      nCos;
    long                        nWink0;
    long                        nWink;
    FASTBOOL                    bRight;

public:
    TYPEINFO();
    SdrDragRotate(SdrDragView& rNewView): SdrDragMethod(rNewView),nSin(0.0),nCos(1.0),nWink0(0),nWink(0),bRight(FALSE) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovPoint(Point& rPnt, const Point& rPvOfs);
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
};

//************************************************************
//   SdrDragShear
//************************************************************

class SdrDragShear : public SdrDragMethod
{
    Fraction                    aFact;
    long                        nWink0;
    long                        nWink;
    double                      nTan;
    FASTBOOL                    bVertical;   // Vertikales verzerren
    FASTBOOL                    bResize;     // Shear mit Resize
    FASTBOOL                    bUpSideDown; // Beim Shear/Slant gespiegelt
    FASTBOOL                    bSlant;

public:
    TYPEINFO();
    SdrDragShear(SdrDragView& rNewView,FASTBOOL bSlant1): SdrDragMethod(rNewView),
        aFact(1,1),nWink0(0),nWink(0),
        nTan(0.0),
        bVertical(FALSE),bResize(FALSE),bUpSideDown(FALSE),
        bSlant(bSlant1) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovPoint(Point& rPnt, const Point& rPvOfs);
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
};

//************************************************************
//   SdrDragMirror
//************************************************************

class SdrDragMirror : public SdrDragMethod
{
    Point                       aDif;
    long                        nWink;
    FASTBOOL                    bMirrored;
    FASTBOOL                    bSide0;

private:
    FASTBOOL ImpCheckSide(const Point& rPnt) const;

public:
    TYPEINFO();
    SdrDragMirror(SdrDragView& rNewView): SdrDragMethod(rNewView),nWink(0),bMirrored(FALSE),bSide0(FALSE) { }

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovPoint(Point& rPnt, const Point& rPvOfs);
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
};

//************************************************************
//   SdrDragGradient
//************************************************************

class SdrDragGradient : public SdrDragMethod
{
    // Handles to work on
    SdrHdlGradient*             pIAOHandle;

    // is this for gradient (or for transparence) ?
    unsigned                    bIsGradient : 1;

public:
    TYPEINFO();
    SdrDragGradient(SdrDragView& rNewView, BOOL bGrad = TRUE);

    BOOL IsGradient() const { return bIsGradient; }

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
    virtual void Brk();
};

//************************************************************
//   SdrDragCrook
//************************************************************

class SdrDragCrook : public SdrDragMethod
{
    Rectangle                   aMarkRect;
    Point                       aMarkCenter;
    Point                       aCenter;
    Point                       aStart;
    Fraction                    aFact;
    Point                       aRad;
    FASTBOOL                    bContortionAllowed;
    FASTBOOL                    bNoContortionAllowed;
    FASTBOOL                    bContortion;
    FASTBOOL                    bResizeAllowed;
    FASTBOOL                    bResize;
    FASTBOOL                    bRotateAllowed;
    FASTBOOL                    bRotate;
    FASTBOOL                    bVertical;
    FASTBOOL                    bValid;
    FASTBOOL                    bLft;
    FASTBOOL                    bRgt;
    FASTBOOL                    bUpr;
    FASTBOOL                    bLwr;
    FASTBOOL                    bAtCenter;
    long                        nWink;
    long                        nMarkSize;
    SdrCrookMode                eMode;

public:
    TYPEINFO();
    SdrDragCrook(SdrDragView& rNewView): SdrDragMethod(rNewView),aFact(1,1),
        bContortionAllowed(FALSE),bNoContortionAllowed(FALSE),bContortion(FALSE),
        bResizeAllowed(FALSE),bResize(FALSE),bRotateAllowed(FALSE),bRotate(FALSE),
        bVertical(FALSE),bValid(FALSE),bLft(FALSE),bRgt(FALSE),bUpr(FALSE),bLwr(FALSE),bAtCenter(FALSE),
        nWink(0),nMarkSize(0),eMode(SDRCROOK_ROTATE) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovAllPoints();
    void MovPointCrook(Point& rPnt, const Point& rPvOfs, Point* pC1, Point* pC2); // MovPoint -> MovPointCrook
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
    virtual void DrawXor(XOutputDevice& rXOut, FASTBOOL bFull) const;
};

//************************************************************
//   SdrDragDistort
//************************************************************

class SdrDragDistort : public SdrDragMethod
{
    Rectangle                   aMarkRect;
    XPolygon                    aDistortedRect;
    USHORT                      nPolyPt;
    FASTBOOL                    bContortionAllowed;
    FASTBOOL                    bNoContortionAllowed;
    FASTBOOL                    bContortion;

public:
    TYPEINFO();
    SdrDragDistort(SdrDragView& rNewView): SdrDragMethod(rNewView),nPolyPt(0),
        bContortionAllowed(FALSE),bNoContortionAllowed(FALSE),bContortion(FALSE) {}

    virtual void TakeComment(String& rStr) const;

    virtual FASTBOOL Beg();
    virtual void MovAllPoints();
    virtual void Mov(const Point& rPnt);
    virtual FASTBOOL End(FASTBOOL bCopy);
    virtual Pointer GetPointer() const;
    virtual void DrawXor(XOutputDevice& rXOut, FASTBOOL bFull) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_SVDDRGM1_HXX

