/*************************************************************************
 *
 *  $RCSfile: excform.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2003-05-21 08:01:40 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/


#ifndef _EXCFORM_HXX
#define _EXCFORM_HXX

#ifndef SC_XLTOOLS_HXX
#include "xltools.hxx"
#endif

#ifndef _FORMEL_HXX
#include "formel.hxx"
#endif


class ScRangeList;
class ScfUInt16List;


#ifdef MWERKS
class ExcelToSc : public ExcelConverterBase, public ExcRoot
#else
class ExcelToSc : public ExcelConverterBase, protected ExcRoot
#endif
{
protected:
    BOOL                bExternName;    // wenn External Name gefunden wurde
    static const UINT16 nRowMask;
    static const UINT16 nLastInd;       // letzter Index fuer Excel->SC-
                                        // Token Umsetzung
    // ---------------------------------------------------------------
    inline void         DoDefArgs( UINT16 );

    void                DoMulArgs( DefTokenId, BYTE );

    static DefTokenId   IndexToToken( UINT16 );

    static BYTE         IndexToAnzahl( UINT16 );

    void                ExcRelToScRel( UINT16 nRow, UINT8 nCol, SingleRefData&, const BOOL bName );

public:
                        ExcelToSc( RootData* pRD, XclImpStream& aStr );
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


inline void ExcelToSc::DoDefArgs( UINT16 nIndex )
{
    DoMulArgs( IndexToToken( nIndex ), IndexToAnzahl( nIndex ) );
}


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
    virtual BOOL        Read3DTabReference( UINT16& rFirstTab, UINT16& rLastTab );

public:
                        ExcelToSc8( RootData* pRD, XclImpStream& aStr );
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
