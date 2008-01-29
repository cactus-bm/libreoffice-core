/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: namebuff.hxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: rt $ $Date: 2008-01-29 15:29:58 $
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

#ifndef _NAMEBUFF_HXX
#define _NAMEBUFF_HXX

#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef SC_COMPILER_HXX
#include "compiler.hxx"
#endif

#ifndef _ROOT_HXX
#include "root.hxx"
#endif

#include "rangenam.hxx"
#include <hash_map>
#include <list>

class ScDocument;
class ScTokenArray;
class NameBuffer;




class StringHashEntry
{
private:
    friend class NameBuffer;
    String          aString;
    UINT32          nHash;

    static UINT32   MakeHashCode( const String& );
public:
    inline          StringHashEntry( const String& );
    inline          StringHashEntry( void );
    inline void     operator =( const sal_Char* );
    inline void     operator =( const String& );
    inline void     operator =( const StringHashEntry& );
    inline BOOL     operator ==( const StringHashEntry& ) const;
};


inline StringHashEntry::StringHashEntry( void )
{
}


inline StringHashEntry::StringHashEntry( const String& r ) : aString( r )
{
    nHash = MakeHashCode( r );
}


inline void StringHashEntry::operator =( const sal_Char* p )
{
    aString.AssignAscii( p );
    nHash = MakeHashCode( aString );
}


inline void StringHashEntry::operator =( const String& r )
{
    aString = r;
    nHash = MakeHashCode( r );
}


inline void StringHashEntry::operator =( const StringHashEntry& r )
{
    nHash = r.nHash;
    aString = r.aString;
}


inline BOOL StringHashEntry::operator ==( const StringHashEntry& r ) const
{
    return ( nHash == r.nHash && aString ==  r.aString );
}



class NameBuffer : private List, public ExcRoot
{
private:
    UINT16                  nBase;      // Index-Basis
public:
//    inline                  NameBuffer( void );   //#94039# prevent empty rootdata
    inline                  NameBuffer( RootData* );
    inline                  NameBuffer( RootData*, UINT16 nNewBase );

    virtual                 ~NameBuffer();
    inline const String*    Get( UINT16 nIndex );
    inline UINT16           GetLastIndex( void );
    inline void             SetBase( UINT16 nNewBase = 0 );
    void                    operator <<( const String& rNewString );
    void                    Reset( void );
    BOOL                    Find( const sal_Char* pRefName, UINT16& rIndex );
};

//#94039# prevent empty rootdata
//inline NameBuffer::NameBuffer( void )
//{
//    nBase = 0;
//}


inline NameBuffer::NameBuffer( RootData* p ) : ExcRoot( p )
{
    nBase = 0;
}


inline NameBuffer::NameBuffer( RootData* p, UINT16 nNewBase ) : ExcRoot( p )
{
    nBase = nNewBase;
}


inline const String* NameBuffer::Get( UINT16 n )
{
    if( n < nBase )
        return NULL;
    else
    {
        StringHashEntry* pObj = ( StringHashEntry* ) List::GetObject( n );

        if( pObj )
            return &pObj->aString;
        else
            return NULL;
    }
}


inline UINT16 NameBuffer::GetLastIndex( void )
{
    DBG_ASSERT( Count() + nBase <= 0xFFFF, "*NameBuffer::GetLastIndex(): Ich hab' die Nase voll!" );

    return ( UINT16 ) ( Count() + nBase );
}


inline void NameBuffer::SetBase( UINT16 nNewBase )
{
    nBase = nNewBase;
}




class ShrfmlaBuffer : public ExcRoot
{
    struct ScAddressHashFunc : public std::unary_function< const ScAddress &, size_t >
    {
        size_t operator() (const ScAddress &addr) const;
    };
    typedef std::hash_map <ScAddress, USHORT, ScAddressHashFunc> ShrfmlaHash;
    typedef std::list <ScRange>                                  ShrfmlaList;

    ShrfmlaHash  index_hash;
    ShrfmlaList  index_list;
    size_t                  mnCurrIdx;

public:
                            ShrfmlaBuffer( RootData* pRD );
    virtual                 ~ShrfmlaBuffer();
    void                    Clear();
    void                    Store( const ScRange& rRange, const ScTokenArray& );
    USHORT                  Find (const ScAddress & rAddress ) const;

    static String           CreateName( const ScRange& );
    };




class RangeNameBufferWK3 : private List
{
private:
    struct ENTRY
        {
        StringHashEntry     aStrHashEntry;
        ComplRefData        aComplRefDataRel;
        String              aScAbsName;
        UINT16              nAbsInd;        // == 0 -> noch keine Abs-Name!
        UINT16              nRelInd;
        BOOL                bSingleRef;
                            ENTRY( const String& rName, const String& rScName, const ComplRefData& rCRD ) :
                                aStrHashEntry( rName ),
                                aComplRefDataRel( rCRD ),
                                aScAbsName( rScName )
                            {
                                nAbsInd = 0;
                                aScAbsName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "_ABS" ) );
                            }
        };

    ScTokenArray*           pScTokenArray;
    UINT16                  nIntCount;
public:
                            RangeNameBufferWK3( void );
    virtual                 ~RangeNameBufferWK3();
    void                    Add( const String& rName, const ComplRefData& rCRD );
    inline void             Add( const String& rName, const ScRange& aScRange );
    BOOL                    FindRel( const String& rRef, UINT16& rIndex );
    BOOL                    FindAbs( const String& rRef, UINT16& rIndex );
};


inline void RangeNameBufferWK3::Add( const String& rName, const ScRange& aScRange )
{
    ComplRefData        aCRD;
    SingleRefData*      pSRD;
    const ScAddress*    pScAddr;

    pSRD = &aCRD.Ref1;
    pScAddr = &aScRange.aStart;
    pSRD->SetFlag3D( TRUE );
    pSRD->nCol = pScAddr->Col();
    pSRD->nRow = pScAddr->Row();
    pSRD->nTab = pScAddr->Tab();

    // zunaechst ALLE Refs nur absolut
    pSRD->SetColRel( FALSE );
    pSRD->SetRowRel( FALSE );
    pSRD->SetTabRel( FALSE );

    pSRD = &aCRD.Ref2;
    pScAddr = &aScRange.aEnd;
    pSRD->SetFlag3D( TRUE );
    pSRD->nCol = pScAddr->Col();
    pSRD->nRow = pScAddr->Row();
    pSRD->nTab = pScAddr->Tab();

    // zunaechst ALLE Refs nur absolut
    pSRD->SetColRel( FALSE );
    pSRD->SetRowRel( FALSE );
    pSRD->SetTabRel( FALSE );

    Add( rName, aCRD );
}




class ExtSheetBuffer : private List, public ExcRoot
{
private:
    struct Cont
        {
        String      aFile;
        String      aTab;
        UINT16      nTabNum;    // 0xFFFF -> noch nicht angelegt
                                // 0xFFFE -> versucht anzulegen, ging aber schief
                                // 0xFFFD -> soll im selben Workbook sein, findet's aber nicht
        BOOL        bSWB;
        BOOL        bLink;
                    Cont( const String& rFilePathAndName, const String& rTabName ) :
                        aFile( rFilePathAndName ),
                        aTab( rTabName )
                    {
                        nTabNum = 0xFFFF;   // -> Tabelle noch nicht erzeugt
                        bSWB = bLink = FALSE;
                    }
                    Cont( const String& rFilePathAndName, const String& rTabName,
                        const BOOL bSameWB ) :
                        aFile( rFilePathAndName ),
                        aTab( rTabName )
                    {
                        nTabNum = 0xFFFF;   // -> Tabelle noch nicht erzeugt
                        bSWB = bSameWB;
                        bLink = FALSE;
                    }
        };
public:
    inline          ExtSheetBuffer( RootData* );
    virtual         ~ExtSheetBuffer();

    void            Add( const String& rFilePathAndName,
                        const String& rTabName, const BOOL bSameWorkbook = FALSE );

    BOOL            GetScTabIndex( UINT16 nExcSheetIndex, UINT16& rIn_LastTab_Out_ScIndex );
    BOOL            IsLink( const UINT16 nExcSheetIndex ) const;
    BOOL            GetLink( const UINT16 nExcSheetIndex, String &rAppl, String &rDoc ) const;
    BOOL            IsExternal( UINT16 nExcSheetIndex ) const;

    void            Reset( void );
};


inline ExtSheetBuffer::ExtSheetBuffer( RootData* p ) : ExcRoot( p )
{
}




struct ExtName
{
    String          aName;
    UINT32          nStorageId;
    UINT16          nFlags;

    inline          ExtName( const String& r ) : aName( r ), nStorageId( 0 ) {}

    BOOL            IsDDE( void ) const;
    BOOL            IsOLE( void ) const;
    BOOL            IsName( void ) const;
};




class ExtNameBuff : private List, protected ExcRoot
{
private:
    static const sal_Char*  pJoostTest;
protected:
public:
    inline          ExtNameBuff( RootData* );
    virtual         ~ExtNameBuff();

    void            AddDDE( const String& rName );
    void            AddOLE( const String& rName, UINT32 nStorageId );
    void            AddName( const String& rName );

    const ExtName*  GetName( const UINT16 nExcelIndex ) const;

    void            Reset( void );
};


inline ExtNameBuff::ExtNameBuff( RootData* p ) : ExcRoot( p )
{
}


#endif


