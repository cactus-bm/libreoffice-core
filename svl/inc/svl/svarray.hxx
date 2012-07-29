/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef _SVARRAY_HXX
#define _SVARRAY_HXX

/***********************************************************************
*
*   Hier folgt die Beschreibung fuer die exportierten Makros:
*
*       SV_DECL_PTRARR_SORT(nm, AE, IS, GS)
*       SV_IMPL_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Pointern sortiert ist. Basiert auf einem PTRARR
*
*       SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS)
*       SV_IMPL_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Pointern sortiert ist. Basiert auf einem PTRARR_DEL
*
*       SV_DECL_PTRARR_SORT(nm, AE, IS, GS)
*       SV_IMPL_OP_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Objecten sortiert ist. Basiert auf einem PTRARR.
*           Sortierung mit Hilfe der Object-operatoren "<" und "=="
*
*       SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS)
*       SV_IMPL_OP_PTRARR_SORT( nm,AE )
*           defieniere/implementiere ein Sort-Array mit Pointern, das nach
*           Objecten sortiert ist. Basiert auf einem PTRARR_DEL.
*           Sortierung mit Hilfe der Object-operatoren "<" und "=="
***********************************************************************/

#include "svl/svldllapi.h"

#include <string.h>     // memmove()
#include <limits.h>     // USHRT_MAX
#include <osl/diagnose.h>
#include <rtl/alloc.h>
#include <tools/solar.h>

class String;

class DummyType;
inline void* operator new( size_t, DummyType* pPtr )
{
    return pPtr;
}
inline void operator delete( void*, DummyType* ) {}

#define SV_DECL_PTRARR_GEN(nm, AE, IS, Base, AERef, VPRef, vis )\
class vis nm: public Base \
{\
public:\
    nm( sal_uInt16 nIni=IS )\
        : Base(nIni) {}\
    void Insert( const nm *pI, sal_uInt16 nP, \
            sal_uInt16 nS = 0, sal_uInt16 nE = USHRT_MAX ) {\
        Base::Insert((const Base*)pI, nP, nS, nE);\
    }\
    void Insert( const AERef aE, sal_uInt16 nP ) {\
        Base::Insert( (const VPRef )aE, nP );\
    }\
    void Insert( const AE *pE, sal_uInt16 nL, sal_uInt16 nP ) {\
        Base::Insert( (const VoidPtr*)pE, nL, nP );\
    }\
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1) {\
        Base::Remove(nP,nL);\
    }\
    const AE* GetData() const {\
        return (const AE*)Base::GetData();\
    }\
    AE operator[]( sal_uInt16 nP )const  { \
        return (AE)Base::operator[](nP); }\
    AE GetObject(sal_uInt16 nP) const { \
        return (AE)Base::GetObject(nP); }\
    \
    sal_uInt16 GetPos( const AERef aE ) const { \
        return Base::GetPos((const VPRef)aE);\
    }\
    void DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL=1 );\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_VISIBILITY(nm, AE, IS, vis)\
SV_DECL_PTRARR_GEN(nm, AE, IS, SvPtrarr, AE &, VoidPtr &, vis )

typedef void* VoidPtr;
class SVL_DLLPUBLIC SvPtrarr
{
protected:
    VoidPtr    *pData;
    sal_uInt16 nFree;
    sal_uInt16 nA;

    void _resize(size_t n);

public:
    SvPtrarr( sal_uInt16= 0 );
    ~SvPtrarr() { rtl_freeMemory( pData ); }

    VoidPtr& operator[](sal_uInt16 nP) const { return *(pData+nP); }

    void Insert( const SvPtrarr * pI, sal_uInt16 nP,
                 sal_uInt16 nS = 0, sal_uInt16 nE = USHRT_MAX )
    {
        if( USHRT_MAX == nE )
            nE = pI->nA;
        if( nS < nE )
            Insert( (const VoidPtr*)pI->pData+nS, (sal_uInt16)nE-nS, nP );
    }

    VoidPtr& GetObject(sal_uInt16 nP) const { return (*this)[nP]; }

    void Insert( const VoidPtr& aE, sal_uInt16 nP );
    void Insert( const VoidPtr *pE, sal_uInt16 nL, sal_uInt16 nP );
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1 );
    sal_uInt16 Count() const { return nA; }
    const VoidPtr* GetData() const { return (const VoidPtr*)pData; }
    sal_uInt16 GetPos( const VoidPtr & aE ) const;
};

// SORTARR - Begin

#define _SORT_CLASS_DEF(nm, AE, IS, vis)\
class vis nm : private nm##_SAR \
{\
public:\
    nm(sal_uInt16 nSize = IS)\
        : nm##_SAR(nSize) {}\
    void Insert( const nm *pI, sal_uInt16 nS=0, sal_uInt16 nE=USHRT_MAX );\
    sal_Bool Insert( const AE& aE );\
    sal_Bool Insert( const AE& aE, sal_uInt16& rP );\
    void Insert( const AE *pE, sal_uInt16 nL );\
    void Remove( sal_uInt16 nP, sal_uInt16 nL = 1 );\
    void Remove( const AE& aE, sal_uInt16 nL = 1 );\
    sal_uInt16 Count() const  {   return nm##_SAR::Count(); }\
    const AE* GetData() const { return (const AE*)pData; }\
\
/* Das Ende stehe im DECL-Makro !!! */

#define _SV_SEEK_PTR(nm,AE)\
sal_Bool nm::Seek_Entry( const AE aE, sal_uInt16* pP ) const\
{\
    register sal_uInt16 nO  = nm##_SAR::Count(),\
            nM, \
            nU = 0;\
    if( nO > 0 )\
    {\
        nO--;\
        register long rCmp = (long)aE;\
        while( nU <= nO )\
        {\
            nM = nU + ( nO - nU ) / 2;\
            if( (long)*(pData + nM) == rCmp )\
            {\
                if( pP ) *pP = nM;\
                return sal_True;\
            }\
            else if( (long)*(pData+ nM) < (long)aE  )\
                nU = nM + 1;\
            else if( nM == 0 )\
            {\
                if( pP ) *pP = nU;\
                return sal_False;\
            }\
            else\
                nO = nM - 1;\
        }\
    }\
    if( pP ) *pP = nU;\
    return sal_False;\
}

#define _SV_SEEK_PTR_TO_OBJECT( nm,AE )\
sal_Bool nm::Seek_Entry( const AE aE, sal_uInt16* pP ) const\
{\
    register sal_uInt16 nO  = nm##_SAR::Count(),\
            nM, \
            nU = 0;\
    if( nO > 0 )\
    {\
        nO--;\
        while( nU <= nO )\
        {\
            nM = nU + ( nO - nU ) / 2;\
            if( *(*((AE*)pData + nM)) == *(aE) )\
            {\
                if( pP ) *pP = nM;\
                return sal_True;\
            }\
            else if( *(*((AE*)pData + nM)) < *(aE) )\
                nU = nM + 1;\
            else if( nM == 0 )\
            {\
                if( pP ) *pP = nU;\
                return sal_False;\
            }\
            else\
                nO = nM - 1;\
        }\
    }\
    if( pP ) *pP = nU;\
    return sal_False;\
}

#define _SV_IMPL_SORTAR_ALG(nm, AE)\
void nm::Insert( const nm * pI, sal_uInt16 nS, sal_uInt16 nE )\
{\
    if( USHRT_MAX == nE )\
        nE = pI->Count();\
    sal_uInt16 nP;\
    const AE * pIArr = pI->GetData();\
    for( ; nS < nE; ++nS )\
    {\
        if( ! Seek_Entry( *(pIArr+nS), &nP) )\
                nm##_SAR::Insert( *(pIArr+nS), nP );\
        if( ++nP >= Count() )\
        {\
            nm##_SAR::Insert( pI, nP, nS+1, nE );\
            nS = nE;\
        }\
    }\
}\
\
sal_Bool nm::Insert( const AE & aE )\
{\
    sal_uInt16 nP;\
    sal_Bool bExist;\
    bExist = Seek_Entry( aE, &nP );\
    if( ! bExist )\
        nm##_SAR::Insert( aE, nP );\
    return !bExist;\
}\
sal_Bool nm::Insert( const AE & aE, sal_uInt16& rP )\
{\
    sal_Bool bExist;\
    bExist = Seek_Entry( aE, &rP );\
    if( ! bExist )\
        nm##_SAR::Insert( aE, rP );\
    return !bExist;\
}\
void nm::Insert( const AE* pE, sal_uInt16 nL)\
{\
    sal_uInt16 nP;\
    for( sal_uInt16 n = 0; n < nL; ++n )\
        if( ! Seek_Entry( *(pE+n), &nP ))\
            nm##_SAR::Insert( *(pE+n), nP );\
}\
void nm::Remove( sal_uInt16 nP, sal_uInt16 nL )\
{\
    if( nL )\
        nm##_SAR::Remove( nP, nL);\
}\
\
void nm::Remove( const AE &aE, sal_uInt16 nL )\
{\
    sal_uInt16 nP;\
    if( nL && Seek_Entry( aE, &nP ) )   \
        nm##_SAR::Remove( nP, nL);\
}\

#define _SORTARR_BLC_CASTS(nm, AE )\
    sal_uInt16 GetPos( const AE& aE ) const { \
        return SvPtrarr::GetPos((const VoidPtr&)aE);\
    }

#define _SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, vis)\
SV_DECL_PTRARR_VISIBILITY(nm##_SAR, AE, IS, vis)\
_SORT_CLASS_DEF(nm, AE, IS, vis)\
    AE operator[](sal_uInt16 nP) const {\
        return nm##_SAR::operator[]( nP );\
    }\
    AE GetObject(sal_uInt16 nP) const {\
        return nm##_SAR::GetObject( nP );\
    }\
    sal_Bool Seek_Entry( const AE aE, sal_uInt16* pP = 0 ) const;\
    void DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL=1 ); \
    _SORTARR_BLC_CASTS(nm, AE )\
\
/* Das Ende stehe im DECL-Makro !!! */

#define _SV_DECL_PTRARR_SORT(nm, AE, IS, vis)\
_SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, vis)\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_SORT(nm, AE, IS)\
_SV_DECL_PTRARR_SORT(nm, AE, IS,)

#define _SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, vis)\
_SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, vis)\
    ~nm() { DeleteAndDestroy( 0, Count() ); }\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_SORT_DEL(nm, AE, IS)\
_SV_DECL_PTRARR_SORT_DEL(nm, AE, IS,)

#define SV_IMPL_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL ) { \
        if( nL ) {\
            OSL_ENSURE( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( sal_uInt16 n=nP; n < nP + nL; n++ ) \
                delete *((AE*)pData+n); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
_SV_SEEK_PTR( nm, AE )

#define SV_IMPL_OP_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( sal_uInt16 nP, sal_uInt16 nL ) { \
        if( nL ) {\
            OSL_ENSURE( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( sal_uInt16 n=nP; n < nP + nL; n++ ) \
                delete *((AE*)pData+n); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
_SV_SEEK_PTR_TO_OBJECT( nm,AE )

#if defined(ICC) || defined(GCC) || (defined(WNT) && _MSC_VER >= 1400)
#define C40_INSERT( c, p, n ) Insert( (c const *&) p, n )
#define C40_GETPOS( c, r) GetPos( (c const *&) r )
#else
#define C40_INSERT( c, p, n ) Insert( p, n )
#define C40_GETPOS( c, r) GetPos( r )
#endif

#endif  //_SVARRAY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
