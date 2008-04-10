/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: excform.hxx,v $
 * $Revision: 1.21 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#ifndef _EXCFORM_HXX
#define _EXCFORM_HXX

#include "xlformula.hxx"
#include "xiroot.hxx"
#include "formel.hxx"


class ScRangeList;


class ExcelToSc : public ExcelConverterBase, protected XclImpRoot
{
protected:
    enum ExtensionType { EXTENSION_ARRAY, EXTENSION_NLR, EXTENSION_MEMAREA };
    typedef ::std::vector< ExtensionType >          ExtensionTypeVec;

    BOOL                bExternName;    // wenn External Name gefunden wurde
    static const UINT16 nRowMask;
    static const UINT16 nLastInd;       // letzter Index fuer Excel->SC-
                                        // Token Umsetzung
    XclFunctionProvider maFuncProv;
    const XclBiff       meBiff;

    // ---------------------------------------------------------------
    void                DoMulArgs( DefTokenId eId, sal_uInt8 nNumArgs, sal_uInt8 mnMinParamCount = 0 );

    void                ExcRelToScRel( UINT16 nRow, UINT8 nCol, SingleRefData&, const BOOL bName );

public:
                        ExcelToSc( const XclImpRoot& rRoot );
    virtual             ~ExcelToSc();
    virtual ConvErr     Convert( const ScTokenArray*&, XclImpStream& rStrm, sal_Size nFormulaLen,
                                 bool bAllowArrays, const FORMULA_TYPE eFT = FT_CellFormula );

    virtual ConvErr     Convert( _ScRangeListTabs&, XclImpStream& rStrm, sal_Size nFormulaLen, const FORMULA_TYPE eFT = FT_CellFormula );
    virtual BOOL        GetAbsRefs( ScRangeList& rRangeList, XclImpStream& rStrm, sal_Size nLen );

    void                GetDummy( const ScTokenArray*& );
    const ScTokenArray* GetBoolErr( XclBoolError );
    BOOL                GetShrFmla( const ScTokenArray*&, XclImpStream& rStrm, sal_Size nFormulaLen );

#if 0
                            // return = TRUE -> String-Record folgt!
    static BOOL         SetCurVal( ScFormulaCell& rCell, double& rCurVal );
#endif
    static void         SetError( ScFormulaCell& rCell, const ConvErr eErr );

    static inline BOOL  IsComplColRange( const UINT16 nCol1, const UINT16 nCol2 );
    static inline BOOL  IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 );

    void                SetComplCol( ComplRefData& );
    void                SetComplRow( ComplRefData& );

    void                ReadExtensions( const ExtensionTypeVec& rExtensions,
                                        XclImpStream& aIn );
    void                ReadExtensionArray( unsigned int n,
                                            XclImpStream& aIn );
    void                ReadExtensionNlr( XclImpStream& aIn );
    void                ReadExtensionMemArea( XclImpStream& aIn );
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

    void                ExcRelToScRel8( UINT16 nRow, UINT16 nCol, SingleRefData&,
                            const BOOL bName );

                        // this function must read 2 bytes from stream and adjust <nBytesLeft>
    virtual BOOL        Read3DTabReference( XclImpStream& rStrm, SCTAB& rFirstTab, SCTAB& rLastTab );

public:
                        ExcelToSc8( const XclImpRoot& rRoot );
    virtual             ~ExcelToSc8();

    virtual ConvErr     Convert( const ScTokenArray*& rpTokArray, XclImpStream& rStrm, sal_Size nFormulaLen, bool bAllowArrays, const FORMULA_TYPE eFT = FT_CellFormula );

    virtual ConvErr     Convert( _ScRangeListTabs&, XclImpStream& rStrm, sal_Size nFormulaLen, const FORMULA_TYPE eFT = FT_CellFormula );

    static inline BOOL  IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 );

    virtual BOOL        GetAbsRefs( ScRangeList& rRangeList, XclImpStream& rStrm, sal_Size nLen );
};


inline BOOL ExcelToSc8::IsComplRowRange( const UINT16 nRow1, const UINT16 nRow2 )
{
    return ( nRow1 == 0x0000 ) && ( nRow2 == 0xFFFF );
}





#endif
