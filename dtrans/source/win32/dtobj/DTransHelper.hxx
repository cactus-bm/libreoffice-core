/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DTransHelper.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:22:30 $
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


#ifndef _DTRANSHELPER_HXX_
#define _DTRANSHELPER_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#include <windows.h>

#ifndef _WINCLIP_HXX_
#include "..\misc\WinClip.hxx"
#endif

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

#define AUTO_INIT                 TRUE
#define NO_AUTO_INIT              FALSE
#define MEM_DESTROY_ON_RELEASE    TRUE
#define NO_MEM_DESTROY_ON_RELEASE FALSE

//------------------------------------------------------------------------
// deklarations
//------------------------------------------------------------------------


//-------------------------------------------------------------------------
// a helper class to manage a global memory area, the clients can write
// into the global memory area and extract the handle to the global mem
// note: not thread-safe
//-------------------------------------------------------------------------

class CStgTransferHelper
{
public:
    // will be thrown in case of failures
    class CStgTransferException
    {
    public:
        HRESULT m_hr;
        CStgTransferException( HRESULT hr ) : m_hr( hr ) {};
    };

public:
    CStgTransferHelper(
        sal_Bool bAutoInit = sal_False,
        HGLOBAL  hGlob = NULL,
        sal_Bool bDelStgOnRelease = sal_False );

    ~CStgTransferHelper( );

    void SAL_CALL write( const void* lpData, ULONG cb, ULONG* cbWritten = NULL );
    void SAL_CALL read( LPVOID pv, ULONG cb, ULONG* pcbRead = NULL );

    HGLOBAL SAL_CALL getHGlobal( ) const;
    void    SAL_CALL getIStream( LPSTREAM* ppStream );

    void SAL_CALL init(
        SIZE_T newSize,
        sal_uInt32 uiFlags = GHND,
        sal_Bool bDelStgOnRelease = sal_False );

    void SAL_CALL init(
        HGLOBAL hGlob,
        sal_Bool bDelStgOnRelease = sal_False );

    // returns the size of the managed memory
    sal_uInt32 SAL_CALL memSize( CLIPFORMAT cf = CF_INVALID ) const;

    // free the global memory and necessary
    // release the internal stream pointer
    void SAL_CALL cleanup( );

private:
    LPSTREAM m_lpStream;
    sal_Bool m_bDelStgOnRelease;

private:
    CStgTransferHelper( const CStgTransferHelper& );
    CStgTransferHelper& operator=( const CStgTransferHelper& );
};

//-------------------------------------------------------------------------
// something like an auto-pointer - allows access to the memory belonging
// to a HGLOBAL and automatically unlocks a global memory at destruction
// time
//-------------------------------------------------------------------------

class CRawHGlobalPtr
{
public:

    //---------------------------------------------
    // ctor
    //---------------------------------------------

    CRawHGlobalPtr( HGLOBAL hGlob ) :
        m_hGlob( hGlob ),
        m_bIsLocked( FALSE ),
        m_pGlobMem( NULL )
    {
    }


    //---------------------------------------------
    // ctor
    //---------------------------------------------

    CRawHGlobalPtr( const CStgTransferHelper& theHGlobalHelper ) :
        m_hGlob( theHGlobalHelper.getHGlobal( ) ),
        m_bIsLocked( FALSE ),
        m_pGlobMem( NULL )
    {
    }

    //---------------------------------------------
    // dtor
    //---------------------------------------------

    ~CRawHGlobalPtr( )
    {
        if ( m_bIsLocked )
            GlobalUnlock( m_hGlob );
    }

    //---------------------------------------------
    // lock the global memory (initializes a
    // pointer to this memory)
    //---------------------------------------------

    BOOL Lock( )
    {
        if ( !m_bIsLocked && ( NULL != m_hGlob ) )
        {
            m_pGlobMem = GlobalLock( m_hGlob );
            m_bIsLocked = ( NULL != m_pGlobMem );
        }

        return m_bIsLocked;
    }

    //---------------------------------------------
    // unlock the global memory (invalidates the
    // pointer to this memory)
    //---------------------------------------------

    BOOL Unlock( )
    {
        GlobalUnlock( m_hGlob );
        m_bIsLocked = FALSE;
        m_pGlobMem = NULL;

        return ( NO_ERROR == GetLastError( ) );
    }

    //---------------------------------------------
    // locks the global memory and returns a
    // pointer to this memory
    //---------------------------------------------

    LPVOID GetMemPtr( )
    {
        Lock( );
        return m_pGlobMem;
    }

    //---------------------------------------------
    // size of mem we point to
    //---------------------------------------------

    int MemSize( ) const
    {
        return GlobalSize( m_hGlob );
    }

private:
    HGLOBAL m_hGlob;
    BOOL    m_bIsLocked;
    LPVOID  m_pGlobMem;
};

#endif
