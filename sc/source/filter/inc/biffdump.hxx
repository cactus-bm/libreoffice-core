/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: biffdump.hxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-22 12:06:27 $
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

#ifndef SC_BIFFDUMP_HXX
#define SC_BIFFDUMP_HXX

#if OSL_DEBUG_LEVEL > 1

// ============================================================================

// 1 = Compile with BIFF dumper.
#define EXC_INCL_DUMPER 0

// ============================================================================

#if EXC_INCL_DUMPER

#include <vector>
#include <map>

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif
#ifndef _SVSTOR_HXX
#include <so3/svstor.hxx>
#endif

#ifndef _EXCFORM_HXX
#include "excform.hxx"
#endif
#ifndef SC_XIROOT_HXX
#include "xiroot.hxx"
#endif


#define MODE_DUMP               0x0000
#define MODE_SKIP               0x0001

#define MODE_HEX                0x0002
#define MODE_BODY               0x0000
#define MODE_NAMEONLY           0x0008

#define MODE_PLAIN              0x0000
#define MODE_DETAIL             0x0004


#define CT_EOL                  0x01
#define CT_NUM                  0x02
#define CT_ALPHA                0x04
#define CT_ALPHANUM             0x08
#define CT_HEX                  0x10
#define CT_LOWERALPHA           0x20
#define CT_UPPERALPHA           0x40
#define CT_BLANK                0x80


#define DELANDNULL(p)           {delete p;p=NULL;}

inline void CopyStrpOnStrp( ByteString*& rpToString, const ByteString* pFromString );


struct DUMP_ERR
{
    DUMP_ERR*           pNext;
    UINT32              nLine;
    ByteString          aText;
    ByteString*         pHint;

    inline              DUMP_ERR( const UINT32 nLine, const ByteString& rText, const ByteString& rHint );
    inline              DUMP_ERR( const UINT32 nLine, const ByteString& rText );
                        ~DUMP_ERR();
};




enum _KEYWORD           { KW_Unknown, Skipdump, Contload, Parsep, Maxbodylines,
                            Include, Exclude, Hex, Body, Comment, Output, Title,
                            NameOnly, ClearFile, SkipOffset, ReadContRecs, NoWarnings,
                            BlankLine, ExportBookStream };




struct IdRange
{
    UINT16                      nFirst;
    UINT16                      nLast;
    IdRange( const UINT16 nF, const UINT16 nL ) : nFirst( nF ), nLast( nL ) {}
};




class IdRangeList : protected List
{
private:
protected:
public:
    virtual                 ~IdRangeList();

    List::Count;

    inline const IdRange*   First( void );
    inline const IdRange*   Next( void );
    inline const IdRange*   Get( const UINT32 n ) const;

    inline void             Append( const UINT16 nFirst, const UINT16 nNext );

    void                    Clear( void );
    };




class Biff8RecDumper : public XclImpRoot
{
protected:
    typedef ::std::vector< ByteString >             ByteStringVec;
    typedef ::std::map< sal_uInt32, sal_uInt32 >    StrmPortionMap;
    typedef ::std::pair< sal_uInt32, sal_uInt16 >   XclDumpCtrlPortion;
    typedef ::std::vector< XclDumpCtrlPortion >     XclDumpCtrlPortionVec;


    static const sal_Char*      pLevelPreString;
    static const sal_Char*      pLevelPreStringNT;
    const sal_Char*             pLevelPre;
    UINT16                      nLevelCnt;

    ByteString*                 pTitle;
    ByteString*                 pOutName;

    SvFileStream*               pDumpStream;
    XclImpStream*               pIn;

    StrmPortionMap              maCtlsPosMap;       /// Control data in 'Ctls' stream.
    XclDumpCtrlPortionVec       maCtrlStorages;
    XclDumpCtrlPortionVec       maCtrlPortions;
    ByteStringVec               maNames;            /// Defined names.

    UINT32                      nMaxBodyLines;
    BOOL                        bEndLoading;
    BOOL                        bSkip;
    BOOL                        bSkipOffset;
    BOOL                        bReadContRecs;
    BOOL                        bWarnings;
    BOOL                        bClearFile;
    BOOL                        bBlankLine;
    BOOL                        bExportBookStream;
    BOOL                        bBIFF8;
    bool                        bEncrypted;

    UINT32                      nFieldCnt;
    UINT32                      nItemCnt;
    UINT32                      nTabIndexCnt;
    sal_uInt32                  mnEscherPos;
    UINT16                      nFontIndex;
    sal_uInt16                  mnSubstream;

    static const UINT16         nRecCnt;
    UINT16*                     pDumpModes;
    ByteString**                ppRecNames;

    static UINT32               nInstances;

    static sal_Char*            pBlankLine;
    static const UINT16         nLenBlankLine;

    static UINT8*               pCharType;
    static UINT8*               pCharVal;

    DUMP_ERR*                   pFirst;
    DUMP_ERR*                   pLast;
    DUMP_ERR*                   pAct;

    sal_Char                    cParSep;
    sal_Char                    cComm1;
    sal_Char                    cComm2;

    void                        Print( const ByteString& rStr );
    void                        Print( const sal_Char* pStr );

    UINT16                      DumpXF( XclImpStream& rIn, const sal_Char* pPre );
    void                        DumpValidPassword( XclImpStream& rIn, const sal_Char* pPre );
    void                        RecDump( BOOL bSubStream = FALSE );
    void                        EscherDump( const ULONG nL, bool bDumpOffset );
    void                        ObjDump( const ULONG nL );
    void                        ContDump( const ULONG nL );
    void                        FormulaDump( const UINT16 nL, const FORMULA_TYPE eFT );

    void                        DumpBinary( SvStream& rInStrm, ULONG nSize = STREAM_SEEK_TO_END );

    void                        DumpControlContents( SvStream& rInStrm, sal_uInt16 nCtrlType );
    void                        DumpControlContainer( SvStream& rInStrm, sal_uInt16 nCtrlType );

    void                        DumpBinaryStream( SotStorageRef xStrg, const String& rStrmName, const String& rStrgPath );
    void                        DumpTextStream( SotStorageRef xStrg, const String& rStrmName, const String& rStrgPath );
    void                        DumpRecordStream( SotStorageRef xStrg, const String& rStrmName, const String& rStrgPath );
    void                        DumpCtlsStream();
    void                        DumpControlFrameStream( SotStorageRef xInStrg, sal_uInt16 nCtrlType, const String& rStrgPath );
    void                        DumpControlObjectsStream( SotStorageRef xInStrg, const String& rStrgPath );

    void                        DumpAnyStorage( SotStorageRef xParentStrg, const String& rStrgName, const String& rStrgPath );
    void                        DumpUserFormStorage( SotStorageRef xParentStrg, const String& rStrgName, sal_uInt16 nCtrlType, const String& rStrgPath );
    void                        DumpVbaProjectStorage();

    void                        PreDumpDecrypted( ULONG nL );
    static const sal_Char*      GetBlanks( const UINT16 nNumOfBlanks );
    static BOOL                 IsLineEnd( const sal_Char c, sal_Char& rNext, SvStream& rIn, INT32& rLeft );
    void                        Init( void );
    static _KEYWORD             GetKeyType( const ByteString& rString );
    BOOL                        ExecCommand( const UINT32 nLine, const ByteString& rCommand,
                                                const ByteString* pVal = NULL );
    BOOL                        ExecSetVal( const UINT32 nLine, const ByteString& rId,
                                            const ByteString* pName, const ByteString* pInExClude,
                                            const ByteString* pHexBody );

    inline void                 SetMode( const UINT16 nRecNum, const UINT16 nMode );
    inline UINT16               GetMode( const UINT16 nRecNum ) const;
    inline UINT16*              GetModeRef( const UINT16 nRecNum ) const;
    inline BOOL                 HasMode( const UINT16 nRecNum, const UINT16 nRefMode ) const;
    inline BOOL                 HasModeDump( const UINT16 nRecNum ) const;
    inline BOOL                 HasModeSkip( const UINT16 nRecNum ) const;
    inline BOOL                 HasModeHex( const UINT16 nRecNum ) const;
    inline BOOL                 HasModeBody( const UINT16 nRecNum ) const;
    inline BOOL                 HasModePlain( const UINT16 nRecNum ) const;
    inline BOOL                 HasModeDetail( const UINT16 nRecNum ) const;
    inline BOOL                 HasModeNameOnly( const UINT16 nRecNum ) const;
    void                        SetFlag( const UINT16 nFirst, const UINT16 nLast, const UINT16 nFlags );
    void                        ClrFlag( const UINT16 nFirst, const UINT16 nLast, const UINT16 nFlags );

    inline void                 SetName( const UINT16 nRecNum, const sal_Char* pName );
    inline void                 SetName( const UINT16 nRecNum, const ByteString& rName );
    void                        SetName( const UINT16 nRecNum, ByteString* pName );
    inline const ByteString*    GetName( const UINT16 nRecNum );

    inline static BOOL          IsNum( const sal_Char c );
    inline static BOOL          IsAlpha( const sal_Char c );
    inline static BOOL          IsAlphaNum( const sal_Char c );
    inline static BOOL          IsHex( const sal_Char c );
    inline static BOOL          IsEndOfLine( const sal_Char c );
    inline static BOOL          IsBlank( const sal_Char c );
    static UINT32               GetVal( const ByteString& rString );
    BOOL                        FillIdRangeList( const UINT32 nLine, IdRangeList& rRangeList,
                                                const ByteString& rVals );
    inline static UINT8         GetVal( const sal_Char c );
    BOOL                        CreateOutStream( const UINT32 nL = 0xFFFFFFFF );
    static SvFileStream*        CreateInStream( const sal_Char* pName );
    static SvFileStream*        CreateInStream( const sal_Char* pPath, const sal_Char* pName );

    void                        AddError( const UINT32 nLine, const ByteString& rText, const ByteString& rHint );
    inline void                 AddError( const UINT32 nLine, const sal_Char* pText, const ByteString& rHint );
    inline void                 AddError( const UINT32 nLine, const ByteString& rText );
    inline void                 AddError( const UINT32 nLine, const sal_Char* pText );
    inline const DUMP_ERR*      FirstErr( void );
    inline const DUMP_ERR*      NextErr( void );
public:
                                Biff8RecDumper( const XclImpRoot& rRoot, BOOL bBIFF8 );
                                ~Biff8RecDumper();
    BOOL                        Dump( XclImpStream& rIn );
                                // = TRUE -> nicht weiter laden

    inline static BOOL          IsPrintable( const UINT8 nC );
    };




inline void CopyStrpOnStrp( ByteString*& rp, const ByteString* p )
{
    if( p )
    {
        if( rp )
            *rp = *p;
        else
            rp = new ByteString( *p );
    }
    else if( rp )
    {
        delete rp;
        rp = NULL;
    }
}




inline DUMP_ERR::DUMP_ERR( const UINT32 n, const ByteString& rT, const ByteString& rH ) :
    nLine( n ), aText( rT ), pNext( NULL )
{
    pHint = new ByteString( rH );
}


inline DUMP_ERR::DUMP_ERR( const UINT32 n, const ByteString& rT ) :
    nLine( n ), aText( rT ), pHint( NULL ), pNext( NULL )
{
}




inline const IdRange* IdRangeList::First( void )
{
    return ( const IdRange* ) List::First();
}


inline const IdRange* IdRangeList::Next( void )
{
    return ( const IdRange* ) List::Next();
}


inline const IdRange* IdRangeList::Get( const UINT32 n ) const
{
    return ( const IdRange* ) List::GetObject( n );
}


inline void IdRangeList::Append( const UINT16 n1, const UINT16 n2 )
{
    List::Insert( new IdRange( n1, n2 ), LIST_APPEND );
}




inline void Biff8RecDumper::SetMode( const UINT16 n, const UINT16 nM )
{
    if( n < nRecCnt )
        pDumpModes[ n ] = nM;
}


inline UINT16 Biff8RecDumper::GetMode( const UINT16 n ) const
{
    if( n < nRecCnt )
        return pDumpModes[ n ];
    else
        return 0xFFFF;
}


inline UINT16* Biff8RecDumper::GetModeRef( const UINT16 n ) const
{
    if( n < nRecCnt )
        return pDumpModes + n;
    else
        return NULL;
}


inline BOOL Biff8RecDumper::HasMode( const UINT16 n, const UINT16 nM ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & nM ) == nM;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModeDump( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_SKIP ) == 0;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModeSkip( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_SKIP ) != 0;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModeHex( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_HEX ) != 0;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModeBody( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_HEX ) == 0;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModePlain( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_DETAIL ) == 0;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModeDetail( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_DETAIL ) != 0;
    else
        return FALSE;
}


inline BOOL Biff8RecDumper::HasModeNameOnly( const UINT16 n ) const
{
    if( n < nRecCnt )
        return ( pDumpModes[ n ] & MODE_NAMEONLY ) != 0;
    else
        return FALSE;
}


inline void Biff8RecDumper::SetName( const UINT16 n, const sal_Char* p )
{
    if( n < nRecCnt )
    {
        if( ppRecNames[ n ] )
            *ppRecNames[ n ] = p;
        else
            ppRecNames[ n ] = new ByteString( p );
    }
}


inline void Biff8RecDumper::SetName( const UINT16 n, const ByteString& r )
{
    if( n < nRecCnt )
    {
        if( ppRecNames[ n ] )
            *ppRecNames[ n ] = r;
        else
            ppRecNames[ n ] = new ByteString( r );
    }
}


inline const ByteString* Biff8RecDumper::GetName( const UINT16 n )
{
    if( n < nRecCnt )
        return ppRecNames[ n ];
    else
        return NULL;
}


inline BOOL Biff8RecDumper::IsNum( const sal_Char c )
{
    return ( pCharType[ ( UINT8 ) c ] & CT_NUM ) != 0;
}


inline BOOL Biff8RecDumper::IsAlpha( const sal_Char c )
{
    return ( pCharType[ ( UINT8 ) c ] & CT_ALPHA ) != 0;
}


inline BOOL Biff8RecDumper::IsAlphaNum( const sal_Char c )
{
    return ( pCharType[ ( UINT8 ) c ] & CT_ALPHANUM ) != 0;
}


inline BOOL Biff8RecDumper::IsHex( const sal_Char c )
{
    return ( pCharType[ ( UINT8 ) c ] & CT_HEX ) != 0;
}


inline BOOL Biff8RecDumper::IsEndOfLine( const sal_Char c )
{
    return ( pCharType[ ( UINT8 ) c ] & CT_EOL ) != 0;
}


inline BOOL Biff8RecDumper::IsBlank( const sal_Char c )
{
    return ( pCharType[ ( UINT8 ) c ] & CT_BLANK ) != 0;
}


inline UINT8 Biff8RecDumper::GetVal( const sal_Char c )
{
    return pCharVal[ ( UINT8 ) c ];
}


inline void Biff8RecDumper::AddError( const UINT32 n, const sal_Char* p, const ByteString& r )
{
    AddError( n, ByteString( p ), r );
}


inline void Biff8RecDumper::AddError( const UINT32 nLine, const ByteString& rText )
{
}


inline void Biff8RecDumper::AddError( const UINT32 nLine, const sal_Char* pText )
{
}


inline const DUMP_ERR* Biff8RecDumper::FirstErr( void )
{
    pAct = pFirst;
    return pAct;
}


inline const DUMP_ERR* Biff8RecDumper::NextErr( void )
{
    if( pAct )
        pAct = pAct->pNext;

    return pAct;
}


inline BOOL Biff8RecDumper::IsPrintable( const UINT8 n )
{
    return n >= ' ';
}


#endif
#else
// never use the dumper in product builds
#define EXC_INCL_DUMPER 0
#endif
#endif

