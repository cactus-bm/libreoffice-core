/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: embeddoc.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2006-02-09 13:37:27 $
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

#ifndef _EMBEDDOC_HXX_
#define _EMBEDDOC_HXX_
#if defined(_MSC_VER) && (_MSC_VER >= 1300)
#undef _DEBUG
#endif

#include "common.h"
#include <oleidl.h>

#include <hash_map>

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/SEQUENCE.h>
#endif

#include "docholder.hxx"

typedef ::std::hash_map< DWORD, IAdviseSink* > AdviseSinkHashMap;
typedef ::std::hash_map< DWORD, IAdviseSink* >::iterator AdviseSinkHashMapIterator;

class GDIMetaFile;
class CIIAObj;


class EmbedDocument_Impl
    : public IPersistStorage,
      public IDataObject,
      public IOleObject,
      public IOleInPlaceObject,
      public IPersistFile,
      public IDispatch
{
protected:
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >
                fillArgsForLoading_Impl( ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > xStream,
                                         DWORD nStreamMode,
                                         LPCOLESTR pFilePath = NULL );
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >
                fillArgsForStoring_Impl( ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > xStream );

    HRESULT SaveTo_Impl( IStorage* pStg );

    sal_uInt64 getMetaFileHandle_Impl( sal_Bool isEnhMeta );

public:
    EmbedDocument_Impl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& smgr,
                        const GUID* guid );
    ~EmbedDocument_Impl();

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR * ppvObj);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    /* IPersistMethod */
    STDMETHOD(GetClassID)(CLSID *pClassID);

    /* IPersistStorage methods */
    STDMETHOD(IsDirty) ();
    STDMETHOD(InitNew) ( IStorage *pStg );
    STDMETHOD(Load) ( IStorage* pStr );
    STDMETHOD(Save) ( IStorage *pStgSave, BOOL fSameAsLoad );
    STDMETHOD(SaveCompleted) ( IStorage *pStgNew );
    STDMETHOD(HandsOffStorage) (void);

    /* IDataObject methods */
    STDMETHOD(GetData) ( FORMATETC * pFormatetc, STGMEDIUM * pMedium );
    STDMETHOD(GetDataHere) ( FORMATETC * pFormatetc, STGMEDIUM * pMedium );
    STDMETHOD(QueryGetData) ( FORMATETC * pFormatetc );
    STDMETHOD(GetCanonicalFormatEtc) ( FORMATETC * pFormatetcIn, FORMATETC * pFormatetcOut );
    STDMETHOD(SetData) ( FORMATETC * pFormatetc, STGMEDIUM * pMedium, BOOL fRelease );
    STDMETHOD(EnumFormatEtc) ( DWORD dwDirection, IEnumFORMATETC ** ppFormatetc );
    STDMETHOD(DAdvise) ( FORMATETC * pFormatetc, DWORD advf, IAdviseSink * pAdvSink, DWORD * pdwConnection );
    STDMETHOD(DUnadvise) ( DWORD dwConnection );
    STDMETHOD(EnumDAdvise) ( IEnumSTATDATA ** ppenumAdvise );

    /* IOleObject methods */
    STDMETHOD(SetClientSite) ( IOleClientSite* pSite );
    STDMETHOD(GetClientSite) ( IOleClientSite** pSite );
    STDMETHOD(SetHostNames) ( LPCOLESTR szContainerApp, LPCOLESTR szContainerObj );
    STDMETHOD(Close) ( DWORD dwSaveOption);
    STDMETHOD(SetMoniker) ( DWORD dwWhichMoniker, IMoniker *pmk );
    STDMETHOD(GetMoniker) ( DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk );
    STDMETHOD(InitFromData) ( IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved );
    STDMETHOD(GetClipboardData) ( DWORD dwReserved, IDataObject **ppDataObject );
    STDMETHOD(DoVerb) ( LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect );
    STDMETHOD(EnumVerbs) ( IEnumOLEVERB **ppEnumOleVerb );
    STDMETHOD(Update) ();
    STDMETHOD(IsUpToDate) ();
    STDMETHOD(GetUserClassID) ( CLSID *pClsid );
    STDMETHOD(GetUserType) ( DWORD dwFormOfType, LPOLESTR *pszUserType );
    STDMETHOD(SetExtent) ( DWORD dwDrawAspect, SIZEL *psizel );
    STDMETHOD(GetExtent) ( DWORD dwDrawAspect, SIZEL *psizel );
    STDMETHOD(Advise) ( IAdviseSink *pAdvSink, DWORD *pdwConnection );
    STDMETHOD(Unadvise) ( DWORD dwConnection );
    STDMETHOD(EnumAdvise) ( IEnumSTATDATA **ppenumAdvise );
    STDMETHOD(GetMiscStatus) ( DWORD dwAspect, DWORD *pdwStatus );
    STDMETHOD(SetColorScheme) ( LOGPALETTE *pLogpal );

    /* IOleInPlaceObject methods */
    STDMETHOD(GetWindow)(HWND *);
    STDMETHOD(ContextSensitiveHelp)(BOOL);
    STDMETHOD(InPlaceDeactivate)();
    STDMETHOD(UIDeactivate)();
    STDMETHOD(SetObjectRects)(LPCRECT, LPCRECT);
    STDMETHOD(ReactivateAndUndo)();

    /* IPersistFile methods */
    STDMETHOD(Load) ( LPCOLESTR pszFileName, DWORD dwMode );
    STDMETHOD(Save) ( LPCOLESTR pszFileName, BOOL fRemember );
    STDMETHOD(SaveCompleted) ( LPCOLESTR pszFileName );
    STDMETHOD(GetCurFile) ( LPOLESTR *ppszFileName );

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount) ( unsigned int FAR*  pctinfo );
    STDMETHOD(GetTypeInfo) ( unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo );
    STDMETHOD(GetIDsOfNames) ( REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId );
    STDMETHOD(Invoke) ( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr );

    // c++ - methods

    void notify( bool bDataChanged = true );
    HRESULT SaveObject();
    HRESULT ShowObject();
    GUID GetGUID() const { return m_guid; }

protected:
    oslInterlockedCount                 m_refCount;

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xFactory;

    DocumentHolder*                     m_pDocHolder;
    ::rtl::OUString                     m_aFileName;

    CComPtr< IStorage >                 m_pMasterStorage;
    CComPtr< IStream >                  m_pOwnStream;
    CComPtr< IStream >                  m_pExtStream;
    GUID                                m_guid;

    sal_Bool                            m_bIsDirty;

    CComPtr< IOleClientSite >           m_pClientSite;
    CComPtr< IDataAdviseHolder >        m_pDAdviseHolder;

    AdviseSinkHashMap                   m_aAdviseHashMap;
    DWORD                               m_nAdviseNum;
};

#endif //_EMBEDDOC_HXX_

