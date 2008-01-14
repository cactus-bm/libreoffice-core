/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: spinfld.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: ihi $ $Date: 2008-01-14 13:03:59 $
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

#ifndef _SV_SPINFLD_HXX
#define _SV_SPINFLD_HXX

#ifndef _SV_SV_H
#include <vcl/sv.h>
#endif
#ifndef _VCL_DLLAPI_H
#include <vcl/dllapi.h>
#endif

#ifndef _SV_EDIT_HXX
#include <vcl/edit.hxx>
#endif
#ifndef _SV_TIMER_HXX
#include <vcl/timer.hxx>
#endif

// -------------
// - SpinField -
// -------------

class VCL_DLLPUBLIC SpinField : public Edit
{
protected:
    Edit*           mpEdit;
    AutoTimer       maRepeatTimer;
    Rectangle       maUpperRect;
    Rectangle       maLowerRect;
    Rectangle       maDropDownRect; // noch nicht angebunden...
    Link            maUpHdlLink;
    Link            maDownHdlLink;
    Link            maFirstHdlLink;
    Link            maLastHdlLink;
    BOOL            mbRepeat:1,
                    mbSpin:1,
                    mbInitialUp:1,
                    mbInitialDown:1,
                    mbNoSelect:1,
                    mbUpperIn:1,
                    mbLowerIn:1,
                    mbInDropDown:1;

    using Window::ImplInit;
    SAL_DLLPRIVATE void   ImplInit( Window* pParent, WinBits nStyle );

private:
    DECL_DLLPRIVATE_LINK( ImplTimeout, Timer* );
    SAL_DLLPRIVATE void   ImplInitSpinFieldData();
    SAL_DLLPRIVATE void   ImplCalcButtonAreas( OutputDevice* pDev, const Size& rOutSz, Rectangle& rDDArea, Rectangle& rSpinUpArea, Rectangle& rSpinDownArea );

protected:
                    SpinField( WindowType nTyp );

    virtual long    Notify( NotifyEvent& rNEvt );
    virtual void    Command( const CommandEvent& rCEvt );

    void            EndDropDown();

    virtual void    FillLayoutData() const;
    Rectangle *     ImplFindPartRect( const Point& rPt );
public:
                    SpinField( Window* pParent, WinBits nWinStyle = 0 );
                    SpinField( Window* pParent, const ResId& rResId );
                    ~SpinField();

    virtual BOOL    ShowDropDown( BOOL bShow );

    virtual void    Up();
    virtual void    Down();
    virtual void    First();
    virtual void    Last();

    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt );
    virtual void    MouseMove( const MouseEvent& rMEvt );
    virtual void    Paint( const Rectangle& rRect );
    virtual void    Draw( OutputDevice* pDev, const Point& rPos, const Size& rSize, ULONG nFlags );
    virtual void    Resize();
    virtual void    StateChanged( StateChangedType nType );
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );
    virtual long    PreNotify( NotifyEvent& rNEvt );

    void            SetUpHdl( const Link& rLink ) { maUpHdlLink = rLink; }
    const Link&     GetUpHdl() const { return maUpHdlLink; }
    void            SetDownHdl( const Link& rLink ) { maDownHdlLink = rLink; }
    const Link&     GetDownHdl() const { return maDownHdlLink; }
    void            SetFirstHdl( const Link& rLink ) { maFirstHdlLink = rLink; }
    const Link&     GetFirstHdl() const { return maFirstHdlLink; }
    void            SetLastHdl( const Link& rLink ) { maLastHdlLink = rLink; }
    const Link&     GetLastHdl() const { return maLastHdlLink; }

    virtual Size    CalcMinimumSize() const;
    virtual Size    GetOptimalSize(WindowSizeType eType) const;
    virtual Size    CalcSize( USHORT nChars ) const;
};

#endif // _SV_SPINFLD_HXX
