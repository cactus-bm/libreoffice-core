/*************************************************************************
 *
 *  $RCSfile: MtaOleClipb.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2003-10-06 14:36:43 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _MTAOLECLIPB_HXX_
#define _MTAOLECLIPB_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#include <objidl.h>

//--------------------------------------------------------
// the Mta-Ole clipboard class is for internal use only!
// only one instance of this class should be created, the
// user has to ensure this!
// the class is not thread-safe because it will be used
// only from within the clipboard service and the methods
// of the clipboard service are already synchronized
//--------------------------------------------------------

class CMtaOleClipboard
{
public:
    typedef void ( WINAPI *LPFNC_CLIPVIEWER_CALLBACK_t )( void );

public:
    CMtaOleClipboard( );
    ~CMtaOleClipboard( );

    // clipboard functions
    HRESULT setClipboard( IDataObject* pIDataObject );
    HRESULT getClipboard( IDataObject** ppIDataObject );
    HRESULT flushClipboard( );

    // register/unregister a clipboard viewer; there can only
    // be one at a time; parameter NULL means unregister
    // a clipboard viewer
    // returns true on success else false; use GetLastError( ) in
    // false case
    sal_Bool registerClipViewer( LPFNC_CLIPVIEWER_CALLBACK_t pfncClipViewerCallback );

private:
    unsigned int run( );

    // create a hidden windows which serves as an request
    // target; so we guarantee synchronization
    void createMtaOleReqWnd( );

    // message support
    sal_Bool postMessage( UINT msg, WPARAM wParam = 0, LPARAM lParam = 0 );
    LRESULT  sendMessage( UINT msg, WPARAM wParam = 0, LPARAM lParam = 0 );

    //---------------------------------------------------------------
    // message handler functions; remeber these functions are called
    // from a different thread context!
    //---------------------------------------------------------------

    LRESULT  onSetClipboard( IDataObject* pIDataObject );
    LRESULT  onGetClipboard( LPSTREAM* ppStream );
    LRESULT  onFlushClipboard( );
    sal_Bool onRegisterClipViewer( LPFNC_CLIPVIEWER_CALLBACK_t pfncClipViewerCallback );

    // win32 clipboard-viewer support
    LRESULT onChangeCBChain( HWND hWndRemove, HWND hWndNext );
    LRESULT onDrawClipboard( );

    static LRESULT CALLBACK mtaOleReqWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    static unsigned int WINAPI oleThreadProc( LPVOID pParam );

    static unsigned int WINAPI clipboardChangedNotifierThreadProc( LPVOID pParam );

    sal_Bool WaitForThreadReady( ) const;

private:
    HANDLE                      m_hOleThread;
    unsigned                    m_uOleThreadId;
    HANDLE                      m_hEvtThrdReady;
    HWND                        m_hwndMtaOleReqWnd;
    ATOM                        m_MtaOleReqWndClassAtom;
    HWND                        m_hwndNextClipViewer;
    LPFNC_CLIPVIEWER_CALLBACK_t m_pfncClipViewerCallback;
    sal_Bool                    m_bInRegisterClipViewer;

    sal_Bool                    m_bRunClipboardNotifierThread;
    HANDLE                      m_hClipboardChangedNotifierThread;
    HANDLE                      m_hClipboardChangedNotifierEvents[2];
    HANDLE&                     m_hClipboardChangedEvent;
    HANDLE&                     m_hTerminateClipboardChangedNotifierEvent;
    osl::Mutex                  m_ClipboardChangedEventCountMutex;
    sal_Int32                   m_ClipboardChangedEventCount;

    osl::Mutex                  m_pfncClipViewerCallbackMutex;

    static CMtaOleClipboard*    s_theMtaOleClipboardInst;

// not allowed
private:
    CMtaOleClipboard( const CMtaOleClipboard& );
    CMtaOleClipboard& operator=( const CMtaOleClipboard& );

    friend LRESULT CALLBACK mtaOleReqWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif
