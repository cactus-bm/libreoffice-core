/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XTDo.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 06:08:14 $
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


#ifndef _XTDATAOBJECT_HXX_
#define _XTDATAOBJECT_HXX_


//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#if defined _MSC_VER
#pragma warning(push,1)
#endif
#include <windows.h>
#include <ole2.h>
#include <objidl.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

#include <vector>

// forward declaration
//class CWinClipbImpl;
class EnumFormatEtc;

/*--------------------------------------------------------------------------
    - the function principle of the windows clipboard:
      a data provider offers all formats he can deliver on the clipboard
      a clipboard client ask for the available formats on the clipboard
      and decides if there is a format he can use
      if there is one, he requests the data in this format

    - This class inherits from IDataObject an so can be placed on the
      OleClipboard. The class wrapps a transferable object which is the
      original DataSource
    - DataFlavors offerd by this transferable will be translated into
      appropriate clipboard formats
    - if the transferable contains text data always text and unicodetext
      will be offered or vice versa
    - text data will be automaticaly converted between text und unicode text
    - although the transferable may support text in different charsets
      (codepages) only text in one codepage can be offered by the clipboard

----------------------------------------------------------------------------*/

class CXTDataObject : public IDataObject
{
public:
    CXTDataObject( );

    //-----------------------------------------------------------------
    // ole interface implementation
    //-----------------------------------------------------------------

    //IUnknown interface methods
    STDMETHODIMP           QueryInterface(REFIID iid, LPVOID* ppvObject);
    STDMETHODIMP_( ULONG ) AddRef( );
    STDMETHODIMP_( ULONG ) Release( );

    // IDataObject interface methods
    STDMETHODIMP GetData( LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium );
    STDMETHODIMP GetDataHere( LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium );
    STDMETHODIMP QueryGetData( LPFORMATETC pFormatetc );
    STDMETHODIMP GetCanonicalFormatEtc( LPFORMATETC pFormatectIn, LPFORMATETC pFormatetcOut );
    STDMETHODIMP SetData( LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium, BOOL fRelease );
    STDMETHODIMP EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC** ppenumFormatetc );
    STDMETHODIMP DAdvise( LPFORMATETC pFormatetc, DWORD advf, LPADVISESINK pAdvSink, DWORD* pdwConnection );
    STDMETHODIMP DUnadvise( DWORD dwConnection );
    STDMETHODIMP EnumDAdvise( LPENUMSTATDATA* ppenumAdvise );

    operator IDataObject*( );

private:

private:
    LONG m_nRefCnt;
};

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

class CEnumFormatEtc : public IEnumFORMATETC
{
public:
    CEnumFormatEtc( LPUNKNOWN pUnkDataObj );

    // IUnknown
    STDMETHODIMP           QueryInterface( REFIID iid, LPVOID* ppvObject );
    STDMETHODIMP_( ULONG ) AddRef( );
    STDMETHODIMP_( ULONG ) Release( );

    //IEnumFORMATETC
    STDMETHODIMP Next( ULONG celt, LPFORMATETC rgelt, ULONG* pceltFetched );
    STDMETHODIMP Skip( ULONG celt );
    STDMETHODIMP Reset( );
    STDMETHODIMP Clone( IEnumFORMATETC** ppenum );

private:
    LONG                                m_nRefCnt;
    LPUNKNOWN                           m_pUnkDataObj;
    ULONG                               m_nCurrPos;
};

typedef CEnumFormatEtc *PCEnumFormatEtc;

#endif
