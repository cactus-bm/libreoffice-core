/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: atlwindow.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:31:21 $
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
#ifndef _WINDOW_HXX_
#define _WINDOW_HXX_
#include <atlbase.h>
extern CComModule _Module;
#include<atlcom.h>
#include<atlctl.h>
#include <com/sun/star/datatransfer/dnd/XDropTarget.hpp>
#include <com/sun/star/datatransfer/dnd/XDragSource.hpp>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/uno/Reference.h>
#include "../../source/inc/DtObjFactory.hxx"


using namespace com::sun::star::uno;
using namespace com::sun::star::datatransfer::dnd;
using namespace com::sun::star::datatransfer;

struct ThreadData
{
    Reference<XDragSource> source;
    Reference<XTransferable> transferable;
    HANDLE evtThreadReady;
};

class AWindow: public CWindowImpl<AWindow, CWindow,
      CWinTraits<WS_CAPTION |WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0> >
{
    TCHAR m_strName[80];
    Reference<XDropTarget> m_xDropTarget;
    Reference<XDragSource> m_xDragSource;
    BOOL m_isMTA;

    HWND m_hwndEdit;

    CDTransObjFactory m_aDataConverter;

public:
    AWindow(LPCTSTR strName)
    {
        RECT rcPos= {0,0,200,200};
        Create(0, rcPos, strName);
    }
    AWindow(LPCTSTR strName, RECT pos, BOOL mta=FALSE): m_isMTA( mta)
    {
        Create(0, pos, strName);
    }

    ~AWindow()
    {
        if(m_hWnd)
            DestroyWindow();
    }


    BEGIN_MSG_MAP(AWindow)
        MESSAGE_HANDLER( WM_CLOSE, OnClose)
        MESSAGE_HANDLER( WM_CREATE, OnCreate)
        MESSAGE_RANGE_HANDLER( WM_MOUSEFIRST,  WM_MOUSELAST, OnMouseAction)
        MESSAGE_HANDLER( WM_TIMER, OnTimer)
        MESSAGE_HANDLER( WM_SIZE, OnSize)
        MESSAGE_HANDLER( WM_SETFOCUS, OnFocus)

    END_MSG_MAP()

    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseAction(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

};

#endif
