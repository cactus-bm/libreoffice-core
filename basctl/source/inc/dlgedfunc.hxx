/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dlgedfunc.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 20:19:45 $
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

#ifndef _BASCTL_DLGEDFUNC_HXX
#define _BASCTL_DLGEDFUNC_HXX

#ifndef _SV_TIMER_HXX
#include <vcl/timer.hxx>
#endif

class DlgEditor;
class Timer;
class MouseEvent;
class Point;

//============================================================================
// DlgEdFunc
//============================================================================

class DlgEdFunc /* : public LinkHdl */
{
protected:
    DlgEditor* pParent;
    Timer        aScrollTimer;

    DECL_LINK( ScrollTimeout, Timer * );
    void    ForceScroll( const Point& rPos );

public:
    DlgEdFunc( DlgEditor* pParent );
    virtual ~DlgEdFunc();

    virtual BOOL MouseButtonDown( const MouseEvent& rMEvt );
    virtual BOOL MouseButtonUp( const MouseEvent& rMEvt );
    virtual BOOL MouseMove( const MouseEvent& rMEvt );
    virtual BOOL KeyInput( const KeyEvent& rKEvt );
};

//============================================================================
// DlgEdFuncInsert
//============================================================================

class DlgEdFuncInsert : public DlgEdFunc
{
public:
    DlgEdFuncInsert( DlgEditor* pParent );
    ~DlgEdFuncInsert();

    virtual BOOL MouseButtonDown( const MouseEvent& rMEvt );
    virtual BOOL MouseButtonUp( const MouseEvent& rMEvt );
    virtual BOOL MouseMove( const MouseEvent& rMEvt );
};

//============================================================================
// DlgEdFuncSelect
//============================================================================

class DlgEdFuncSelect : public DlgEdFunc
{
protected:
    BOOL    bMarkAction;

public:
    DlgEdFuncSelect( DlgEditor* pParent );
    ~DlgEdFuncSelect();

    virtual BOOL MouseButtonDown( const MouseEvent& rMEvt );
    virtual BOOL MouseButtonUp( const MouseEvent& rMEvt );
    virtual BOOL MouseMove( const MouseEvent& rMEvt );
};


#endif //_BASCTL_DLGEDFUNC_HXX
