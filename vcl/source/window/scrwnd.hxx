/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scrwnd.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: ihi $ $Date: 2007-04-16 14:22:16 $
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

#include <floatwin.hxx>
#include <bitmap.hxx>
#include <image.hxx>

// -----------
// - Defines -
// -----------

#define WHEELMODE_NONE      0x00000000UL
#define WHEELMODE_VH        0x00000001UL
#define WHEELMODE_V         0x00000002UL
#define WHEELMODE_H         0x00000004UL
#define WHEELMODE_SCROLL_VH 0x00000008UL
#define WHEELMODE_SCROLL_V  0x00000010UL
#define WHEELMODE_SCROLL_H  0x00000020UL

// -------------------
// - ImplWheelWindow -
// -------------------

class Timer;

class ImplWheelWindow : public FloatingWindow
{
private:

    ImageList           maImgList;
    Bitmap              maWheelBmp;
    CommandScrollData   maCommandScrollData;
    Point               maLastMousePos;
    Point               maCenter;
    Timer*              mpTimer;
    ULONG               mnRepaintTime;
    ULONG               mnTimeout;
    ULONG               mnWheelMode;
    ULONG               mnMaxWidth;
    ULONG               mnActWidth;
    ULONG               mnActDist;
    long                mnActDeltaX;
    long                mnActDeltaY;

    void                ImplCreateImageList();
    void                ImplSetRegion( const Bitmap& rRegionBmp );
    using Window::ImplGetMousePointer;
    PointerStyle        ImplGetMousePointer( long nDistX, long nDistY );
    void                ImplDrawWheel();
    void                ImplRecalcScrollValues();

                        DECL_LINK( ImplScrollHdl, Timer* pTimer );

protected:

    virtual void        Paint( const Rectangle& rRect );
    virtual void        MouseMove( const MouseEvent& rMEvt );
    virtual void        MouseButtonUp( const MouseEvent& rMEvt );

public:

                        ImplWheelWindow( Window* pParent );
                        ~ImplWheelWindow();

    void                ImplStop();
    void                ImplSetWheelMode( ULONG nWheelMode );
    ULONG               ImplGetWheelMode() const { return mnWheelMode; }
};
