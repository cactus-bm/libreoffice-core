/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: excform.hxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:16:24 $
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


#ifndef _EXCFORM_HXX
#define _EXCFORM_HXX

#ifndef SC_XLFORMULA_HXX
#include "xlformula.hxx"
#endif
#ifndef SC_XIROOT_HXX
#include "xiroot.hxx"
#endif

#ifndef _FORMEL_HXX
#include "formel.hxx"
#endif


class ScRangeList;


class ExcelToSc : public ExcelConverterBase, protected XclImpRoot
{
protected:
    BOOL                bExternName;    // wenn External Name gefunden wurde
    static const UINT16 nRowMask;
    static const UINT16 nLastInd;       // letzter Index fuer Excel->SC-
                                        // Token Umsetzung
    XclFunctionProvider maFuncProv;
    const XclBiff       meBiff;

    // ---------------------------------------------------------------
    void                DoMulArgs( DefTokenId, BYTE );

    void                ExcRelToScRel( UINT16 nRow, UINT8 nCol, SingleRefData&, const BOOL bName );

public:
                        ExcelToSc( XclImpStream& rStrm );
    virtual             ~ExcelToSc();
    virtual ConvErr     Convert( const ScTokenArray*&, UINT32 nFormulaLen, const FORMULA_TYPE eFT = FT_CellFormula );

    virtual ConvErr     Convert( _ScRangeListTabs&, UINT32 nFormulaLen, const FORMULA_TYPE eFT = FT_CellFormula );
    virtual BOOL        GetAbsRefs( ScRangeList& rRangeList, UINT32 nLen );

    void                GetDummy( const ScTokenArray*& );
    const ScTokenArray* GetBoolErr( XclBoolError );
    BOOL                GetShrFmla( const ScTokenArray*&, UINT32 nFormulaLen );

    static BOOL         SetCurVal( ScFormulaCell& rCell, double& rCurVal );
                            // return = TRUE -> String-Record folgt!
    static void         SetError( ScFormulaCell& rCell, const ConvErr eErr );

    static inline BOOL  IsComplColRange( const UINT16 nCol1, const UINT16 nCol2 );
    static inline BOOL  IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 );

    void                SetComplCol( ComplRefData& );
    void                SetComplRow( ComplRefData& );
};


inline BOOL ExcelToSc::IsComplColRange( const UINT16 nCol1, const UINT16 nCol2 )
{
    return ( nCol1 == 0x00 ) && ( nCol2 == 0xFF );
}


inline BOOL ExcelToSc::IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 )
{
    return ( ( nRow1 & 0x3FFF ) == 0x0000 ) && ( ( nRow2 & 0x3FFF ) == 0x3FFF );
}


class XclImpLinkManager;

class ExcelToSc8 : public ExcelToSc
{
private:
    const XclImpLinkManager&    rLinkMan;

    void                ExcRelToScRel( UINT16 nRow, UINT16 nCol, SingleRefData&,
                            const BOOL bName );

                        // this function must read 2 bytes from stream and adjust <nBytesLeft>
    virtual BOOL        Read3DTabReference( SCTAB& rFirstTab, SCTAB& rLastTab );

public:
                        ExcelToSc8( XclImpStream& rStrm );
    virtual             ~ExcelToSc8();

    virtual ConvErr     Convert( const ScTokenArray*& rpTokArray, UINT32 nFormulaLen, const FORMULA_TYPE eFT = FT_CellFormula );

    virtual ConvErr     Convert( _ScRangeListTabs&, UINT32 nFormulaLen, const FORMULA_TYPE eFT = FT_CellFormula );

    static inline BOOL  IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 );

    virtual BOOL        GetAbsRefs( ScRangeList& rRangeList, UINT32 nLen );
};


inline BOOL ExcelToSc8::IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 )
{
    return ( nRow1 == 0x0000 ) && ( nRow2 == 0xFFFF );
}





#endif
