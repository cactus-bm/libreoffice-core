/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: formel.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: vg $ $Date: 2007-02-27 12:33:39 $
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


#ifndef _FORMEL_HXX
#define _FORMEL_HXX


#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _TOKSTACK_HXX
#include "tokstack.hxx"
#endif
#ifndef _ROOT_HXX
#include "root.hxx"
#endif
#ifndef SC_SCGLOB_HXX
#include <global.hxx>
#endif
#ifndef SC_COMPILER_HXX
#include <compiler.hxx>
#endif


// ----- forwards --------------------------------------------------------

class XclImpStream;
class ScTokenArray;
class ScFormulaCell;
struct SingleRefData;
struct ComplRefData;




//------------------------------------------------------------------------

enum ConvErr
{
    ConvOK = 0,
    ConvErrNi,      // nicht implemntierter/unbekannter Opcode aufgetreten
    ConvErrNoMem,   // Fehler beim Speicheranfordern
    ConvErrExternal,// Add-Ins aus Excel werden nicht umgesetzt
    ConvErrCount    // Nicht alle Bytes der Formel 'erwischt'
};


enum FORMULA_TYPE
{
    FT_CellFormula,
    FT_RangeName,
    FT_SharedFormula
};




//--------------------------------------------------------- class ScRangeList -

class _ScRangeList : protected List
{
private:
protected:
public:
    virtual                 ~_ScRangeList();
    inline void             Append( const ScRange& rRange );
    inline void             Append( ScRange* pRange );
    inline void             Append( const SingleRefData& rSRD );
    inline void             Append( const ComplRefData& rCRD );

    using                   List::Count;
    inline BOOL             HasRanges( void ) const;

    inline const ScRange*   First( void );
    inline const ScRange*   Next( void );
};


inline void _ScRangeList::Append( const ScRange& r )
{
    List::Insert( new ScRange( r ), LIST_APPEND );
}


inline void _ScRangeList::Append( ScRange* p )
{
    List::Insert( p, LIST_APPEND );
}


inline BOOL _ScRangeList::HasRanges( void ) const
{
    return Count() > 0;
}


inline const ScRange* _ScRangeList::First( void )
{
    return ( const ScRange* ) List::First();
}


inline const ScRange* _ScRangeList::Next( void )
{
    return ( const ScRange* ) List::Next();
}


inline void _ScRangeList::Append( const SingleRefData& r )
{
    List::Insert( new ScRange( r.nCol, r.nRow, r.nTab ), LIST_APPEND );
}


inline void _ScRangeList::Append( const ComplRefData& r )
{
    List::Insert(   new ScRange(    r.Ref1.nCol, r.Ref1.nRow, r.Ref1.nTab,
                                    r.Ref2.nCol, r.Ref2.nRow, r.Ref2.nTab ),
                    LIST_APPEND );
}




//----------------------------------------------------- class ScRangeListTabs -

class _ScRangeListTabs
{
private:
protected:
    BOOL                        bHasRanges;
    _ScRangeList**              ppTabLists;
    _ScRangeList*               pAct;
    UINT16                      nAct;
public:
                                _ScRangeListTabs( void );
    virtual                     ~_ScRangeListTabs();

    void                        Append( SingleRefData aSRD, const BOOL bLimit = TRUE );
    void                        Append( ComplRefData aCRD, const BOOL bLimit = TRUE );

    inline BOOL                 HasRanges( void ) const;

    const ScRange*              First( const UINT16 nTab = 0 );
    const ScRange*              Next( void );
//      const ScRange*              NextContinue( void );
    inline const _ScRangeList*  GetActList( void ) const;
};


inline BOOL _ScRangeListTabs::HasRanges( void ) const
{
    return bHasRanges;
}


inline const _ScRangeList* _ScRangeListTabs::GetActList( void ) const
{
    return pAct;
}




class ConverterBase
{
protected:
    TokenPool           aPool;          // User Token + Predefined Token
    TokenStack          aStack;
    ScAddress           aEingPos;
    ConvErr             eStatus;
    sal_Char*           pBuffer;        // Universal-Puffer
    UINT16              nBufferSize;    // ...und seine Groesse

                        ConverterBase( UINT16 nNewBuffer );
    virtual             ~ConverterBase();

    void                Reset();

public:
    inline SCCOL        GetEingabeCol( void ) const { return aEingPos.Col(); }
    inline SCROW        GetEingabeRow( void ) const { return aEingPos.Row(); }
    inline SCTAB        GetEingabeTab( void ) const { return aEingPos.Tab(); }
    inline ScAddress    GetEingPos( void ) const    { return aEingPos; }
};



class ExcelConverterBase : public ConverterBase
{
protected:
                        ExcelConverterBase( UINT16 nNewBuffer );
    virtual             ~ExcelConverterBase();

public:
    void                Reset();
    void                Reset( const ScAddress& rEingPos );

    virtual ConvErr     Convert( const ScTokenArray*& rpErg, XclImpStream& rStrm, sal_Size nFormulaLen,
                                    const FORMULA_TYPE eFT = FT_CellFormula ) = 0;
    virtual ConvErr     Convert( _ScRangeListTabs&, XclImpStream& rStrm, sal_Size nFormulaLen,
                                    const FORMULA_TYPE eFT = FT_CellFormula ) = 0;
};



class LotusConverterBase : public ConverterBase
{
protected:
    SvStream&           aIn;
    INT32               nBytesLeft;

    inline void         Ignore( const long nSeekRel );
    inline void         Read( sal_Char& nByte );
    inline void         Read( BYTE& nByte );
    inline void         Read( UINT16& nUINT16 );
    inline void         Read( INT16& nINT16 );
    inline void         Read( double& fDouble );
        inline void                     Read( UINT32& nUINT32 );

                        LotusConverterBase( SvStream& rStr, UINT16 nNewBuffer );
    virtual             ~LotusConverterBase();

public:
    void                Reset( INT32 nLen );
    void                Reset( INT32 nLen, const ScAddress& rEingPos );
    void                Reset( const ScAddress& rEingPos );

    virtual ConvErr     Convert( const ScTokenArray*& rpErg, INT32& nRest,
                                    const FORMULA_TYPE eFT = FT_CellFormula ) = 0;

protected:
    using               ConverterBase::Reset;
};


inline void LotusConverterBase::Ignore( const long nSeekRel )
{
    aIn.SeekRel( nSeekRel );
    nBytesLeft -= nSeekRel;
}

inline void LotusConverterBase::Read( sal_Char& nByte )
{
    aIn >> nByte;
    nBytesLeft--;
}

inline void LotusConverterBase::Read( BYTE& nByte )
{
    aIn >> nByte;
    nBytesLeft--;
}

inline void LotusConverterBase::Read( UINT16& nUINT16 )
{
    aIn >> nUINT16;
    nBytesLeft -= 2;
}

inline void LotusConverterBase::Read( INT16& nINT16 )
{
    aIn >> nINT16;
    nBytesLeft -= 2;
}

inline void LotusConverterBase::Read( double& fDouble )
{
    aIn >> fDouble;
    nBytesLeft -= 8;
}

inline void LotusConverterBase::Read( UINT32& nUINT32 )
{
    aIn >> nUINT32;
    nBytesLeft -= 4;
}

#endif


