/*************************************************************************
 *
 *  $RCSfile: excform8.cxx,v $
 *
 *  $Revision: 1.34 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-21 13:24:11 $
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
#include "excform.hxx"

#include "cell.hxx"
#include "document.hxx"
#include "rangenam.hxx"

#ifndef SC_XLTRACER_HXX
#include "xltracer.hxx"
#endif
#ifndef SC_XILINK_HXX
#include "xilink.hxx"
#endif
#ifndef SC_XINAME_HXX
#include "xiname.hxx"
#endif


ExcelToSc8::ExcelToSc8( XclImpStream& rStrm ) :
    ExcelToSc( rStrm ),
    rLinkMan( rStrm.GetRoot().GetLinkManager() )
{
}


ExcelToSc8::~ExcelToSc8()
{
}


BOOL ExcelToSc8::Read3DTabReference( SCTAB& rFirstTab, SCTAB& rLastTab )
{
    rFirstTab = rLastTab = 0;

    UINT16 nIxti;
    aIn >> nIxti;

    return rLinkMan.GetScTabRange( rFirstTab, rLastTab, nIxti );
}


// stream seeks to first byte after <nFormulaLen>
ConvErr ExcelToSc8::Convert( const ScTokenArray*& rpTokArray, UINT32 nFormulaLen, const FORMULA_TYPE eFT )
{
    BYTE                    nOp, nLen, nByte;
    UINT16                  nUINT16;
    double                  fDouble;
    String                  aString;
    BOOL                    bError = FALSE;
    BOOL                    bArrayFormula = FALSE;
    TokenId                 nMerk0;
    const BOOL              bRangeName = eFT == FT_RangeName;
    const BOOL              bSharedFormula = eFT == FT_SharedFormula;
    const BOOL              bRNorSF = bRangeName || bSharedFormula;
    UINT32                  nExtCnt = 0;

    SingleRefData           aSRD;
    ComplRefData            aCRD;

    if( eStatus != ConvOK )
    {
        aIn.Ignore( nFormulaLen );
        return eStatus;
    }

    if( nFormulaLen == 0 )
    {
        aPool.Store( CREATE_STRING( "-/-" ) );
        aPool >> aStack;
        rpTokArray = aPool[ aStack.Get() ];
        return ConvOK;
    }

    ULONG nMaxPos = aIn.GetRecPos() + nFormulaLen;

    while( (aIn.GetRecPos() < nMaxPos) && !bError )
    {
        aIn >> nOp;

        // #98524# always reset flags
        aSRD.InitFlags();
        aCRD.InitFlags();

        switch( nOp )   //                              Buch Seite:
        {           //                                      SDK4 SDK5
            case 0x01: // Array Formula                         [325    ]
                       // Array Formula or Shared Formula       [    277]
            case 0x02: // Data Table                            [325 277]
                aIn.Ignore( 4 );

                bArrayFormula = TRUE;
                break;
            case 0x03: // Addition                              [312 264]
                aStack >> nMerk0;
                aPool <<  aStack << ocAdd << nMerk0;
                aPool >> aStack;
                break;
            case 0x04: // Subtraction                           [313 264]
                // SECOMD-TOP minus TOP
                aStack >> nMerk0;
                aPool << aStack << ocSub << nMerk0;
                aPool >> aStack;
                break;
            case 0x05: // Multiplication                        [313 264]
                aStack >> nMerk0;
                aPool << aStack << ocMul << nMerk0;
                aPool >> aStack;
                break;
            case 0x06: // Division                              [313 264]
                // divide TOP by SECOND-TOP
                aStack >> nMerk0;
                aPool << aStack << ocDiv << nMerk0;
                aPool >> aStack;
                break;
            case 0x07: // Exponetiation                         [313 265]
                // raise SECOND-TOP to power of TOP
                aStack >> nMerk0;
                aPool << aStack << ocPow << nMerk0;
                aPool >> aStack;
                break;
            case 0x08: // Concatenation                         [313 265]
                // append TOP to SECOND-TOP
                aStack >> nMerk0;
                aPool << aStack << ocAmpersand << nMerk0;
                aPool >> aStack;
                break;
            case 0x09: // Less Than                             [313 265]
                // SECOND-TOP < TOP
                aStack >> nMerk0;
                aPool << aStack << ocLess << nMerk0;
                aPool >> aStack;
                break;
            case 0x0A: // Less Than or Equal                    [313 265]
                // SECOND-TOP <= TOP
                aStack >> nMerk0;
                aPool << aStack << ocLessEqual << nMerk0;
                aPool >> aStack;
                break;
            case 0x0B: // Equal                                 [313 265]
                // SECOND-TOP == TOP
                aStack >> nMerk0;
                aPool << aStack << ocEqual << nMerk0;
                aPool >> aStack;
                break;
            case 0x0C: // Greater Than or Equal                 [313 265]
                // SECOND-TOP == TOP
                aStack >> nMerk0;
                aPool << aStack << ocGreaterEqual << nMerk0;
                aPool >> aStack;
                break;
            case 0x0D: // Greater Than                          [313 265]
                // SECOND-TOP == TOP
                aStack >> nMerk0;
                aPool << aStack << ocGreater << nMerk0;
                aPool >> aStack;
                break;
            case 0x0E: // Not Equal                             [313 265]
                // SECOND-TOP == TOP
                aStack >> nMerk0;
                aPool << aStack << ocNotEqual << nMerk0;
                aPool >> aStack;
                break;
            case 0x0F: // Intersection                          [314 265]
                aStack >> nMerk0;
                aPool << aStack << ocIntersect << nMerk0;
                aPool >> aStack;
                break;
            case 0x10: // Union                                 [314 265]
                // ocSep behelfsweise statt 'ocUnion'
                aStack >> nMerk0;
//#100928#      aPool << ocOpen << aStack << ocSep << nMerk0 << ocClose;
                aPool << aStack << ocSep << nMerk0;
                    // doesn't fit exactly, but is more Excel-like
                aPool >> aStack;
                break;
            case 0x11: // Range                                 [314 265]
            {
                aStack >> nMerk0;
                aPool << aStack << ocRange << nMerk0;
                aPool >> aStack;
#if 0
                // wenn erster und zweiter Ausdruck auf'm Stack Single Referenzen
                // sind, dann Area Reference erzeugen, ansonsten Fehlerhaften Aus-
                // druck generieren
                BOOL    bErrorToken = TRUE;
                TokenId nMerk1;

                aStack >> nMerk0;
                aStack >> nMerk1;

                if( aPool.GetType( nMerk0 ) == T_RefC &&
                    aPool.GetType( nMerk1 ) == T_RefC )
                {
                    register const SingleRefData *pRef1 = aPool.GetSRD( nMerk1 );
                    register const SingleRefData *pRef2 = aPool.GetSRD( nMerk0 );
                    if( pRef1 && pRef2 )
                    {
                        aCRD.Ref1 = *pRef1;
                        aCRD.Ref2 = *pRef2;
                        aStack << aPool.Store( aCRD );
                        bErrorToken = FALSE;
                    }
                }

                if( bErrorToken )
                {
                    aPool << ocNoName << ocOpen << nMerk1 << ocSep
                        << nMerk0 << ocClose;

                    aPool >> aStack;
                }
#endif
            }
                break;
            case 0x12: // Unary Plus                            [312 264]
                aPool << ocAdd << aStack;
                aPool >> aStack;
                break;
            case 0x13: // Unary Minus                           [312 264]
                aPool << ocNegSub << aStack;
                aPool >> aStack;
                break;
            case 0x14: // Percent Sign                          [312 264]
                aPool << aStack << ocPercentSign;
                aPool >> aStack;
                break;
            case 0x15: // Parenthesis                           [326 278]
                aPool << ocOpen << aStack << ocClose;
                aPool >> aStack;
                break;
            case 0x16: // Missing Argument                      [314 266]
                aPool << ocMissing;
                aPool >> aStack;
                GetTracer().TraceFormulaMissingArg();
                break;
            case 0x17: // String Constant                       [314 266]
                aIn >> nLen;        // und?
                aString = aIn.ReadUniString( nLen );            // reads Grbit even if nLen==0

                aStack << aPool.Store( aString );
                break;
            case 0x18:                                          // natural language formula
                {
                UINT8   nEptg;
                UINT16  nCol, nRow;
                aIn >> nEptg;
                switch( nEptg )
                {                           //  name        size    ext     type
                    case 0x01:              //  Lel         4       -       err
                        aIn.Ignore( 4 );
                        goto _ocbad;
                        break;
                    case 0x02:              //  Rw          4       -       ref
                    case 0x03:              //  Col         4       -       ref
                    case 0x06:              //  RwV         4       -       val
                    case 0x07:              //  ColV        4       -       val
                        aIn >> nRow >> nCol;

                        aSRD.InitAddress( ScAddress( static_cast<SCCOL>(nCol & 0xFF), static_cast<SCROW>(nRow), aEingPos.Tab() ) );

                        if( nEptg == 0x02 || nEptg == 0x06 )
                            aSRD.SetRowRel( TRUE );
                        else
                            aSRD.SetColRel( TRUE );

                        aSRD.CalcRelFromAbs( aEingPos );

                        aStack << aPool.StoreNlf( aSRD );
                        break;
                    case 0x0A:              //  Radical     13      -       ref
                        aIn >> nRow >> nCol;
                        aIn.Ignore( 9 );

                        aSRD.InitAddress( ScAddress( static_cast<SCCOL>(nCol & 0xFF), static_cast<SCROW>(nRow), aEingPos.Tab() ) );

                        aSRD.SetColRel( TRUE );

                        aSRD.CalcRelFromAbs( aEingPos );

                        aStack << aPool.StoreNlf( aSRD );
                        break;
                    case 0x0B:              //  RadicalS    13      x       ref
                        aIn.Ignore( 13 );
                        nExtCnt++;
                        goto _ocbad;
                        break;
                    case 0x0C:              //  RwS         4       x       ref
                        aIn.Ignore( 4 );
                        nExtCnt++;
                        goto _ocbad;
                        break;
                    case 0x0D:              //  ColS        4       x       ref
                        aIn.Ignore( 4 );
                        nExtCnt++;
                        goto _ocbad;
                        break;
                    case 0x0E:              //  RwSV        4       x       val
                        aIn.Ignore( 4 );
                        nExtCnt++;
                        goto _ocbad;
                        break;
                    case 0x0F:              //  ColSV       4       x       val
                        aIn.Ignore( 4 );
                        nExtCnt++;
                        goto _ocbad;
                        break;
                    case 0x10:              //  RadicalLel  4       -       err
                        aIn.Ignore( 4 );
                        goto _ocbad;
                        break;
                    case 0x1D:              //  SxName      4       -       val
                        aIn.Ignore( 4 );
//                      goto _ocbad;
//                      break;
                    default:
                    _ocbad:
                        aPool << ocBad;
                        aPool >> aStack;
                }
                }
                break;
            case 0x19: // Special Attribute                     [327 279]
            {
                UINT16 nData, nFakt;
                BYTE nOpt;

                aIn >> nOpt >> nData;
                nFakt = 2;

                if( nOpt & 0x04 )
                {// nFakt -> Bytes oder Words ueberlesen    AttrChoose
                    nData++;
                    aIn.Ignore( nData * nFakt );
                }
                else if( nOpt & 0x10 )                      // AttrSum
                    DoMulArgs( ocSum, 1 );
            }
                break;
            case 0x1C: // Error Value                           [314 266]
            {
                aIn >> nByte;
                DefTokenId          eOc;
                switch( nByte )
                {
                    case 0x00:
                    case 0x07:
                    case 0x17:
                    case 0x1D:
                    case 0x24:      eOc = ocStop;       break;
                    case 0x0F:
                    case 0x2A:      eOc = ocNoValue;    break;
                    default:        eOc = ocNoName;
                }
                aPool << eOc;
                if( eOc != ocStop )
                    aPool << ocOpen << ocClose;

                aPool >> aStack;
            }
                break;
            case 0x1D: // Boolean                               [315 266]
                aIn >> nByte;
                if( nByte == 0 )
                    aPool << ocFalse << ocOpen << ocClose;
                else
                    aPool << ocTrue << ocOpen << ocClose;
                aPool >> aStack;
                break;
            case 0x1E: // Integer                               [315 266]
                aIn >> nUINT16;
                aStack << aPool.Store( ( double ) nUINT16 );
                break;
            case 0x1F: // Number                                [315 266]
                aIn >> fDouble;
                aStack << aPool.Store( fDouble );
                break;
            case 0x40:
            case 0x60:
            case 0x20: // Array Constant                        [317 268]
                aIn.Ignore( 7 );
                aPool << ocBad;
                aPool >> aStack;
                break;
            case 0x41:
            case 0x61:
            case 0x21: // Function, Fixed Number of Arguments   [333 282]
            {
                sal_uInt16 nXclFunc;
                aIn >> nXclFunc;
                if( const XclFunctionInfo* pFuncInfo = maFuncProv.GetFuncInfoFromXclFunc( nXclFunc ) )
                    DoMulArgs( pFuncInfo->meOpCode, pFuncInfo->mnMaxParamCount );
                else
                    DoMulArgs( ocNoName, 0 );
            }
            break;
            case 0x42:
            case 0x62:
            case 0x22: // Function, Variable Number of Arg.     [333 283]
            {
                sal_uInt16 nXclFunc;
                sal_uInt8 nParamCount;
                aIn >> nParamCount >> nXclFunc;
                nParamCount &= 0x7F;
                if( const XclFunctionInfo* pFuncInfo = maFuncProv.GetFuncInfoFromXclFunc( nXclFunc ) )
                    DoMulArgs( pFuncInfo->meOpCode, nParamCount );
                else
                    DoMulArgs( ocNoName, 0 );
            }
            break;
            case 0x43:
            case 0x63:
            case 0x23: // Name                                  [318 269]
                aIn >> nUINT16;
            {
                aIn.Ignore( 2 );
                //Determine if this is a user-defined Macro name.
                const XclImpName* pName = GetNameManager().GetName( nUINT16 );
                if(pName && !pName->GetScRangeData())
                    aStack << aPool.Store( ocMacro, pName->GetXclName() );
                else
                    aStack << aPool.Store( nUINT16 );
            }
            break;
            case 0x44:
            case 0x64:
            case 0x24: // Cell Reference                        [319 270]
            case 0x4A:
            case 0x6A:
            case 0x2A: // Deleted Cell Reference                [323 273]
            {
                UINT16          nCol, nRow;

                aIn >> nRow >> nCol;

                aSRD.nCol = static_cast<SCCOL>(nCol);
                aSRD.nRow = nRow & 0x3FFF;
                aSRD.nRelTab = 0;
                aSRD.SetTabRel( TRUE );
                aSRD.SetFlag3D( bRangeName );

                ExcelToSc8::ExcRelToScRel( nRow, nCol, aSRD, bRangeName );

                switch ( nOp )
                {
                    case 0x4A:
                    case 0x6A:
                    case 0x2A: // Deleted Cell Reference        [323 273]
                        // no information which part is deleted, set both
                        aSRD.SetColDeleted( TRUE );
                        aSRD.SetRowDeleted( TRUE );
                }

                aStack << aPool.Store( aSRD );
            }
                break;
            case 0x45:
            case 0x65:
            case 0x25: // Area Reference                        [320 270]
            case 0x4B:
            case 0x6B:
            case 0x2B: // Deleted Area Refernce                 [323 273]
            {
                UINT16          nRowFirst, nRowLast;
                UINT16          nColFirst, nColLast;
                SingleRefData   &rSRef1 = aCRD.Ref1;
                SingleRefData   &rSRef2 = aCRD.Ref2;

                aIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;

                rSRef1.nRelTab = rSRef2.nRelTab = 0;
                rSRef1.SetTabRel( TRUE );
                rSRef2.SetTabRel( TRUE );
                rSRef1.SetFlag3D( bRangeName );
                rSRef2.SetFlag3D( bRangeName );

                ExcelToSc8::ExcRelToScRel( nRowFirst, nColFirst, aCRD.Ref1, bRangeName );
                ExcelToSc8::ExcRelToScRel( nRowLast, nColLast, aCRD.Ref2, bRangeName );

                if( IsComplColRange( nColFirst, nColLast ) )
                    SetComplCol( aCRD );
                else if( IsComplRowRange( nRowFirst, nRowLast ) )
                    SetComplRow( aCRD );

                switch ( nOp )
                {
                    case 0x4B:
                    case 0x6B:
                    case 0x2B: // Deleted Area Refernce         [323 273]
                        // no information which part is deleted, set all
                        rSRef1.SetColDeleted( TRUE );
                        rSRef1.SetRowDeleted( TRUE );
                        rSRef2.SetColDeleted( TRUE );
                        rSRef2.SetRowDeleted( TRUE );
                }

                aStack << aPool.Store( aCRD );
            }
                break;
            case 0x46:
            case 0x66:
            case 0x26: // Constant Reference Subexpression      [321 271]
                aIn.Ignore( 6 );        // mehr steht da nicht!
                break;
            case 0x47:
            case 0x67:
            case 0x27: // Erroneous Constant Reference Subexpr. [322 272]
                aIn.Ignore( 6 );    // mehr steht da nicht!
//               aPool << ocBad;
//               aPool >> aStack;
                break;
            case 0x48:
            case 0x68:
            case 0x28: // Incomplete Constant Reference Subexpr.[331 281]
                aIn.Ignore( 6 );    // mehr steht da nicht!
//               aPool << ocBad;
//               aPool >> aStack;
                break;
            case 0x49:
            case 0x69:
            case 0x29: // Variable Reference Subexpression      [331 281]
                aIn.Ignore( 2 );    // mehr steht da nicht!
                break;
            case 0x4C:
            case 0x6C:
            case 0x2C: // Cell Reference Within a Name          [323    ]
                       // Cell Reference Within a Shared Formula[    273]
            {
                UINT16      nRow, nCol;

                aIn >> nRow >> nCol;

                aSRD.nRelTab = 0;
                aSRD.SetTabRel( TRUE );
                aSRD.SetFlag3D( bRangeName );

                ExcelToSc8::ExcRelToScRel( nRow, nCol, aSRD, bRNorSF );

                aStack << aPool.Store( aSRD );
            }
                break;
            case 0x4D:
            case 0x6D:
            case 0x2D: // Area Reference Within a Name          [324    ]
            {      // Area Reference Within a Shared Formula[    274]
                UINT16                  nRowFirst, nRowLast;
                UINT16                  nColFirst, nColLast;

                aCRD.Ref1.nRelTab = aCRD.Ref2.nRelTab = 0;
                aCRD.Ref1.SetTabRel( TRUE );
                aCRD.Ref2.SetTabRel( TRUE );
                aCRD.Ref1.SetFlag3D( bRangeName );
                aCRD.Ref2.SetFlag3D( bRangeName );

                aIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;

                ExcelToSc8::ExcRelToScRel( nRowFirst, nColFirst, aCRD.Ref1, bRNorSF );
                ExcelToSc8::ExcRelToScRel( nRowLast, nColLast, aCRD.Ref2, bRNorSF );

                if( IsComplColRange( nColFirst, nColLast ) )
                    SetComplCol( aCRD );
                else if( IsComplRowRange( nRowFirst, nRowLast ) )
                    SetComplRow( aCRD );

                aStack << aPool.Store( aCRD );
            }
                break;
            case 0x4E:
            case 0x6E:
            case 0x2E: // Reference Subexpression Within a Name [332 282]
                aIn.Ignore( 2 );    // mehr steht da nicht!
//               aPool << ocBad;
//               aPool >> aStack;
                break;
            case 0x4F:
            case 0x6F:
            case 0x2F: // Incomplete Reference Subexpression... [332 282]
                aIn.Ignore( 2 );    // mehr steht da nicht!
//               aPool << ocBad;
//               aPool >> aStack;
                break;
            case 0x58:
            case 0x78:
            case 0x38: // Command-Equivalent Function           [333    ]
                aString.AssignAscii( "COMM_EQU_FUNC" );
                aIn >> nByte;
                aString += String::CreateFromInt32( nByte );
                aIn >> nByte;
                aStack << aPool.Store( aString );
                DoMulArgs( ocPush, nByte + 1 );
                break;
            case 0x59:
            case 0x79:
            case 0x39: // Name or External Name                 [    275]
            {
                sal_uInt16 nXtiIndex, nNameIdx;
                aIn >> nXtiIndex >> nNameIdx;
                aIn.Ignore( 2 );

                if( rLinkMan.IsSelfRef( nXtiIndex ) )
                {
                    // internal defined name with explicit sheet, i.e.: =Sheet1!AnyName
                    const XclImpName* pName = GetNameManager().GetName( nNameIdx );
                    if( pName && !pName->GetScRangeData() )
                        aStack << aPool.Store( ocMacro, pName->GetXclName() );
                    else
                        aStack << aPool.Store( nNameIdx );
                }
                else if( const XclImpExtName* pExtName = rLinkMan.GetExternName( nXtiIndex, nNameIdx ) )
                {
                    switch( pExtName->GetType() )
                    {
                        case xlExtName:
                        {
                            aStack << aPool.Store( ocNoName, pExtName->GetName() );
                            GetTracer().TraceFormulaExtName();
                        }
                        break;

                        case xlExtAddIn:
                        {
                            aStack << aPool.Store( ocExternal, pExtName->GetName() );
                        }
                        break;

                        case xlExtDDE:
                        {
                            String aApplic, aTopic;
                            if( rLinkMan.GetLinkData( aApplic, aTopic, nXtiIndex ) )
                            {
                                TokenId nPar1 = aPool.Store( aApplic );
                                TokenId nPar2 = aPool.Store( aTopic );
                                nMerk0 = aPool.Store( pExtName->GetName() );
                                aPool   << ocDde << ocOpen << nPar1 << ocSep << nPar2 << ocSep
                                        << nMerk0 << ocClose;
                                aPool >> aStack;
                                pExtName->CreateDdeData( GetDoc(), aApplic, aTopic );
                            }
                        }
                        break;

                        default:    // OLE link
                        {
                            aPool << ocBad;
                            aPool >> aStack;
                        }
                    }
                }
                else
                {
                    //aStack << ocNoName;
                    aPool << ocBad;
                    aPool >> aStack;
                }
            }
                break;
            case 0x5A:
            case 0x7A:
            case 0x3A: // 3-D Cell Reference                    [    275]
            case 0x5C:
            case 0x7C:
            case 0x3C: // Deleted 3-D Cell Reference            [    277]
            {
                UINT16 nRw, nGrbitCol;
                SCTAB nTabFirst, nTabLast;

                BOOL bOK = Read3DTabReference( nTabFirst, nTabLast );
                aIn >> nRw >> nGrbitCol;

                if( bOK )
                {
                    aSRD.nTab = nTabFirst;
                    aSRD.SetFlag3D( TRUE );
                    aSRD.SetTabRel( FALSE );

                    ExcRelToScRel( nRw, nGrbitCol, aSRD, bRangeName );

                    switch ( nOp )
                    {
                        case 0x5C:
                        case 0x7C:
                        case 0x3C: // Deleted 3-D Cell Reference    [    277]
                            // no information which part is deleted, set both
                            aSRD.SetColDeleted( TRUE );
                            aSRD.SetRowDeleted( TRUE );
                    }
                    if ( !ValidTab(nTabFirst) )
                        aSRD.SetTabDeleted( TRUE );

                    if( nTabLast != nTabFirst )
                    {
                        aCRD.Ref1 = aCRD.Ref2 = aSRD;
                        aCRD.Ref2.nTab = nTabLast;
                        aStack << aPool.Store( aCRD );
                    }
                    else
                        aStack << aPool.Store( aSRD );
                }
                else
                {
                    aPool << ocBad;
                    aPool >> aStack;
                }
            }
                break;
            case 0x5B:
            case 0x7B:
            case 0x3B: // 3-D Area Reference                    [    276]
            case 0x5D:
            case 0x7D:
            case 0x3D: // Deleted 3-D Area Reference            [    277]
            {
                UINT16 nRw1, nGrbitCol1, nRw2, nGrbitCol2;
                SCTAB nTabFirst, nTabLast;

                BOOL bOK = Read3DTabReference( nTabFirst, nTabLast );
                aIn >> nRw1 >> nRw2 >> nGrbitCol1 >> nGrbitCol2;

                if( bOK )
                {
                    SingleRefData   &rR1 = aCRD.Ref1;
                    SingleRefData   &rR2 = aCRD.Ref2;

                    rR1.nTab = nTabFirst;
                    rR2.nTab = nTabLast;
                    rR1.SetFlag3D( TRUE );
                    rR1.SetTabRel( FALSE );
                    rR2.SetFlag3D( nTabFirst != nTabLast );
                    rR2.SetTabRel( FALSE );

                    ExcelToSc8::ExcRelToScRel( nRw1, nGrbitCol1, aCRD.Ref1, bRangeName );
                    ExcelToSc8::ExcRelToScRel( nRw2, nGrbitCol2, aCRD.Ref2, bRangeName );

                    if( IsComplColRange( nGrbitCol1, nGrbitCol2 ) )
                        SetComplCol( aCRD );
                    else if( IsComplRowRange( nRw1, nRw2 ) )
                        SetComplRow( aCRD );

                    switch ( nOp )
                    {
                        case 0x5D:
                        case 0x7D:
                        case 0x3D: // Deleted 3-D Area Reference    [    277]
                            // no information which part is deleted, set all
                            rR1.SetColDeleted( TRUE );
                            rR1.SetRowDeleted( TRUE );
                            rR2.SetColDeleted( TRUE );
                            rR2.SetRowDeleted( TRUE );
                    }
                    if ( !ValidTab(nTabFirst) )
                        rR1.SetTabDeleted( TRUE );
                    if ( !ValidTab(nTabLast) )
                        rR2.SetTabDeleted( TRUE );

                    aStack << aPool.Store( aCRD );
                }
                else
                {
                    aPool << ocBad;
                    aPool >> aStack;
                }
            }
                break;
            default: bError = TRUE;
        }
        bError |= !aIn.IsValid();
    }

    ConvErr eRet;

    if( bError )
    {
        aPool << ocBad;
        aPool >> aStack;
        rpTokArray = aPool[ aStack.Get() ];
        eRet = ConvErrNi;
    }
    else if( aIn.GetRecPos() != nMaxPos )
    {
        aPool << ocBad;
        aPool >> aStack;
        rpTokArray = aPool[ aStack.Get() ];
        eRet = ConvErrCount;
    }
    else if( bArrayFormula )
    {
        rpTokArray = NULL;
        eRet = ConvOK;
    }
    else
    {
        rpTokArray = aPool[ aStack.Get() ];
        eRet = ConvOK;
    }

    aIn.Seek( nMaxPos );
    return eRet;
}


// stream seeks to first byte after <nFormulaLen>
ConvErr ExcelToSc8::Convert( _ScRangeListTabs& rRangeList, UINT32 nFormulaLen, const FORMULA_TYPE eFT )
{
    BYTE                    nOp, nLen;//, nByte;
    BOOL                    bError = FALSE;
    BOOL                    bArrayFormula = FALSE;
    const BOOL              bRangeName = eFT == FT_RangeName;
    const BOOL              bSharedFormula = eFT == FT_SharedFormula;
    const BOOL              bRNorSF = bRangeName || bSharedFormula;

    SingleRefData           aSRD;
    ComplRefData            aCRD;

    bExternName = FALSE;

    if( eStatus != ConvOK )
    {
        aIn.Ignore( nFormulaLen );
        return eStatus;
    }

    if( nFormulaLen == 0 )
        return ConvOK;

    ULONG nMaxPos = aIn.GetRecPos() + nFormulaLen;

    while( (aIn.GetRecPos() < nMaxPos) && !bError )
    {
        aIn >> nOp;

        // #98524# always reset flags
        aSRD.InitFlags();
        aCRD.InitFlags();

        switch( nOp )   //                              Buch Seite:
        {           //                                      SDK4 SDK5
            case 0x01: // Array Formula                         [325    ]
                       // Array Formula or Shared Formula       [    277]
                aIn.Ignore( 4 );

                bArrayFormula = TRUE;
                break;
            case 0x02: // Data Table                            [325 277]
                aIn.Ignore( 4 );
                break;
            case 0x03: // Addition                              [312 264]
            case 0x04: // Subtraction                           [313 264]
            case 0x05: // Multiplication                        [313 264]
            case 0x06: // Division                              [313 264]
            case 0x07: // Exponetiation                         [313 265]
            case 0x08: // Concatenation                         [313 265]
            case 0x09: // Less Than                             [313 265]
            case 0x0A: // Less Than or Equal                    [313 265]
            case 0x0B: // Equal                                 [313 265]
            case 0x0C: // Greater Than or Equal                 [313 265]
            case 0x0D: // Greater Than                          [313 265]
            case 0x0E: // Not Equal                             [313 265]
            case 0x0F: // Intersection                          [314 265]
            case 0x10: // Union                                 [314 265]
            case 0x11: // Range                                 [314 265]
            case 0x12: // Unary Plus                            [312 264]
            case 0x13: // Unary Minus                           [312 264]
            case 0x14: // Percent Sign                          [312 264]
            case 0x15: // Parenthesis                           [326 278]
            case 0x16: // Missing Argument                      [314 266]
                break;
            case 0x17: // String Constant                       [314 266]
                aIn >> nLen;        // und?

                aIn.IgnoreUniString( nLen );        // reads Grbit even if nLen==0
                break;
            case 0x19: // Special Attribute                     [327 279]
            {
                UINT16 nData, nFakt;
                BYTE nOpt;

                aIn >> nOpt >> nData;
                nFakt = 2;

                if( nOpt & 0x04 )
                {// nFakt -> Bytes oder Words ueberlesen    AttrChoose
                    nData++;
                    aIn.Ignore( nData * nFakt );
                }
            }
                break;
            case 0x1C: // Error Value                           [314 266]
            case 0x1D: // Boolean                               [315 266]
                aIn.Ignore( 1 );
                break;
            case 0x1E: // Integer                               [315 266]
                aIn.Ignore( 2 );
                break;
            case 0x1F: // Number                                [315 266]
                aIn.Ignore( sizeof( double ) );
                break;
            case 0x40:
            case 0x60:
            case 0x20: // Array Constant                        [317 268]
                aIn.Ignore( 7 );
                break;
            case 0x41:
            case 0x61:
            case 0x21: // Function, Fixed Number of Arguments   [333 282]
                aIn.Ignore( 2 );
                break;
            case 0x42:
            case 0x62:
            case 0x22: // Function, Variable Number of Arg.     [333 283]
                aIn.Ignore( 3 );
                break;
            case 0x43:
            case 0x63:
            case 0x23: // Name                                  [318 269]
                aIn.Ignore( 4 );
                break;
            case 0x44:
            case 0x64:
            case 0x24: // Cell Reference                        [319 270]
            {
                UINT16          nCol, nRow;

                aIn >> nRow >> nCol;

                aSRD.nCol = static_cast<SCCOL>(nCol);
                aSRD.nRow = nRow & 0x3FFF;
                aSRD.nRelTab = 0;
                aSRD.SetTabRel( TRUE );
                aSRD.SetFlag3D( bRangeName );

                ExcelToSc8::ExcRelToScRel( nRow, nCol, aSRD, bRangeName );

                rRangeList.Append( aSRD );
            }
                break;
            case 0x45:
            case 0x65:
            case 0x25: // Area Reference                        [320 270]
            {
                UINT16          nRowFirst, nRowLast;
                UINT16          nColFirst, nColLast;
                SingleRefData   &rSRef1 = aCRD.Ref1;
                SingleRefData   &rSRef2 = aCRD.Ref2;

                aIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;

                rSRef1.nRelTab = rSRef2.nRelTab = 0;
                rSRef1.SetTabRel( TRUE );
                rSRef2.SetTabRel( TRUE );
                rSRef1.SetFlag3D( bRangeName );
                rSRef2.SetFlag3D( bRangeName );

                ExcelToSc8::ExcRelToScRel( nRowFirst, nColFirst, aCRD.Ref1, bRangeName );
                ExcelToSc8::ExcRelToScRel( nRowLast, nColLast, aCRD.Ref2, bRangeName );

                if( IsComplColRange( nColFirst, nColLast ) )
                    SetComplCol( aCRD );
                else if( IsComplRowRange( nRowFirst, nRowLast ) )
                    SetComplRow( aCRD );

                rRangeList.Append( aCRD );
            }
                break;
            case 0x46:
            case 0x66:
            case 0x26: // Constant Reference Subexpression      [321 271]
            case 0x47:
            case 0x67:
            case 0x27: // Erroneous Constant Reference Subexpr. [322 272]
            case 0x48:
            case 0x68:
            case 0x28: // Incomplete Constant Reference Subexpr.[331 281]
                aIn.Ignore( 6 );    // mehr steht da nicht!
                break;
            case 0x49:
            case 0x69:
            case 0x29: // Variable Reference Subexpression      [331 281]
                aIn.Ignore( 2 );    // mehr steht da nicht!
                break;
            case 0x4A:
            case 0x6A:
            case 0x2A: // Deleted Cell Reference                [323 273]
                aIn.Ignore( 3 );
                break;
            case 0x4B:
            case 0x6B:
            case 0x2B: // Deleted Area Refernce                 [323 273]
                aIn.Ignore( 6 );
                break;
            case 0x4C:
            case 0x6C:
            case 0x2C: // Cell Reference Within a Name          [323    ]
                       // Cell Reference Within a Shared Formula[    273]
            {
                UINT16      nRow, nCol;

                aIn >> nRow >> nCol;

                aSRD.nRelTab = 0;
                aSRD.SetTabRel( TRUE );
                aSRD.SetFlag3D( bRangeName );

                ExcelToSc8::ExcRelToScRel( nRow, nCol, aSRD, bRNorSF );

                rRangeList.Append( aSRD );
            }
                break;
            case 0x4D:
            case 0x6D:
            case 0x2D: // Area Reference Within a Name          [324    ]
            {      // Area Reference Within a Shared Formula[    274]
                UINT16                  nRowFirst, nRowLast;
                UINT16                  nColFirst, nColLast;

                aCRD.Ref1.nRelTab = aCRD.Ref2.nRelTab = 0;
                aCRD.Ref1.SetTabRel( TRUE );
                aCRD.Ref2.SetTabRel( TRUE );
                aCRD.Ref1.SetFlag3D( bRangeName );
                aCRD.Ref2.SetFlag3D( bRangeName );

                aIn >> nRowFirst >> nRowLast >> nColFirst >> nColLast;

                ExcelToSc8::ExcRelToScRel( nRowFirst, nColFirst, aCRD.Ref1, bRNorSF );
                ExcelToSc8::ExcRelToScRel( nRowLast, nColLast, aCRD.Ref2, bRNorSF );

                if( IsComplColRange( nColFirst, nColLast ) )
                    SetComplCol( aCRD );
                else if( IsComplRowRange( nRowFirst, nRowLast ) )
                    SetComplRow( aCRD );

                rRangeList.Append( aCRD );
            }
                break;
            case 0x4E:
            case 0x6E:
            case 0x2E: // Reference Subexpression Within a Name [332 282]
            case 0x4F:
            case 0x6F:
            case 0x2F: // Incomplete Reference Subexpression... [332 282]
            case 0x58:
            case 0x78:
            case 0x38: // Command-Equivalent Function           [333    ]
                aIn.Ignore( 2 );
                break;
            case 0x59:
            case 0x79:
            case 0x39: // Name or External Name                 [    275]
                aIn.Ignore( 24 );
                break;
            case 0x5A:
            case 0x7A:
            case 0x3A: // 3-D Cell Reference                    [    275]
            {
                UINT16          nIxti, nRw, nGrbitCol;

                aIn >> nIxti >> nRw >> nGrbitCol;

                SCTAB nFirstScTab, nLastScTab;
                if( rLinkMan.GetScTabRange( nFirstScTab, nLastScTab, nIxti ) )
                {
                    aSRD.nTab = nFirstScTab;
                    aSRD.SetFlag3D( TRUE );
                    aSRD.SetTabRel( FALSE );

                    ExcRelToScRel( nRw, nGrbitCol, aSRD, bRangeName );

                    if( nFirstScTab != nLastScTab )
                    {
                        aCRD.Ref1 = aSRD;
                        aCRD.Ref2.nCol = aSRD.nCol;
                        aCRD.Ref2.nRow = aSRD.nRow;
                        aCRD.Ref2.nTab = nLastScTab;
                        rRangeList.Append( aCRD );
                    }
                    else
                        rRangeList.Append( aSRD );
                }
            }
                break;
            case 0x5B:
            case 0x7B:
            case 0x3B: // 3-D Area Reference                    [    276]
            {
                UINT16          nIxti, nRw1, nGrbitCol1, nRw2, nGrbitCol2;

                aIn >> nIxti >> nRw1 >> nRw2 >> nGrbitCol1 >> nGrbitCol2;

                SCTAB nFirstScTab, nLastScTab;
                if( rLinkMan.GetScTabRange( nFirstScTab, nLastScTab, nIxti ) )
                {
                    SingleRefData   &rR1 = aCRD.Ref1;
                    SingleRefData   &rR2 = aCRD.Ref2;

                    rR1.nTab = nFirstScTab;
                    rR2.nTab = nLastScTab;
                    rR1.SetFlag3D( TRUE );
                    rR1.SetTabRel( FALSE );
                    rR2.SetFlag3D( nFirstScTab != nLastScTab );
                    rR2.SetTabRel( FALSE );

                    ExcelToSc8::ExcRelToScRel( nRw1, nGrbitCol1, aCRD.Ref1, bRangeName );
                    ExcelToSc8::ExcRelToScRel( nRw2, nGrbitCol2, aCRD.Ref2, bRangeName );

                    if( IsComplColRange( nGrbitCol1, nGrbitCol2 ) )
                        SetComplCol( aCRD );
                    else if( IsComplRowRange( nRw1, nRw2 ) )
                        SetComplRow( aCRD );

                    rRangeList.Append( aCRD );
                }
            }
                break;
            case 0x5C:
            case 0x7C:
            case 0x3C: // Deleted 3-D Cell Reference            [    277]
                aIn.Ignore( 6 );
                break;
            case 0x5D:
            case 0x7D:
            case 0x3D: // Deleted 3-D Area Reference            [    277]
                aIn.Ignore( 10 );
                break;
            default:
                bError = TRUE;
        }
        bError |= !aIn.IsValid();
    }

    ConvErr eRet;

    if( bError )
        eRet = ConvErrNi;
    else if( aIn.GetRecPos() != nMaxPos )
        eRet = ConvErrCount;
    else if( bExternName )
        eRet = ConvErrExternal;
    else
        eRet = ConvOK;

    aIn.Seek( nMaxPos );
    return eRet;
}



void ExcelToSc8::ExcRelToScRel( UINT16 nRow, UINT16 nC, SingleRefData &rSRD, const BOOL bName )
{
    const BOOL      bColRel = ( nC & 0x4000 ) != 0;
    const BOOL      bRowRel = ( nC & 0x8000 ) != 0;
    const UINT8     nCol = static_cast<UINT8>(nC);

    rSRD.SetColRel( bColRel );
    rSRD.SetRowRel( bRowRel );

    if( bName )
    {
        // C O L
        if( bColRel )
            //                                                          rel Col
            rSRD.nRelCol = static_cast<SCsCOL>(static_cast<INT8>(nC));
        else
            //                                                          abs Col
            rSRD.nCol = static_cast<SCCOL>(nCol);

        // R O W
        if( bRowRel )
            //                                                          rel Row
            rSRD.nRelRow = static_cast<SCsROW>(static_cast<INT16>(nRow));
        else
            //                                                          abs Row
            rSRD.nRow = Min( static_cast<SCROW>(nRow), MAXROW);

        // T A B
        // #67965# abs needed if rel in shared formula for ScCompiler UpdateNameReference
        if ( rSRD.IsTabRel() && !rSRD.IsFlag3D() )
            rSRD.nTab = GetCurrScTab();
    }
    else
    {
        // C O L
        if ( bColRel )
            rSRD.nRelCol = static_cast<SCsCOL>(nCol) - aEingPos.Col();
        else
            rSRD.nCol = static_cast<SCCOL>(nCol);

        // R O W
        if ( bRowRel )
            rSRD.nRelRow = static_cast<SCsROW>(nRow) - aEingPos.Row();
        else
            rSRD.nRow = static_cast<SCROW>(nRow);

        // T A B
        // #i10184# abs needed if rel in shared formula for ScCompiler UpdateNameReference
        if ( rSRD.IsTabRel() && !rSRD.IsFlag3D() )
            rSRD.nTab = GetCurrScTab() + rSRD.nRelTab;
    }
}


// stream seeks to first byte after <nLen>
BOOL ExcelToSc8::GetAbsRefs( ScRangeList& r, UINT32 nLen )
{
    UINT8                   nOp;
    UINT16                  nRow1, nRow2, nCol1, nCol2;
    SCTAB                                   nTab1, nTab2;
    UINT16                  nIxti;

    UINT32                  nSeek;

    ULONG nMaxPos = aIn.GetRecPos() + nLen;

    while( aIn.IsValid() && (aIn.GetRecPos() < nMaxPos) )
    {
        aIn >> nOp;
        nSeek = 0;

        switch( nOp )
        {
            case 0x44:
            case 0x64:
            case 0x24: // Cell Reference                        [319 270]
            case 0x4C:
            case 0x6C:
            case 0x2C: // Cell Reference Within a Name          [323    ]
                       // Cell Reference Within a Shared Formula[    273]
                aIn >> nRow1 >> nCol1;

                nRow2 = nRow1;
                nCol2 = nCol1;
                nTab1 = nTab2 = GetCurrScTab();
                goto _common;
            case 0x45:
            case 0x65:
            case 0x25: // Area Reference                        [320 270]
            case 0x4D:
            case 0x6D:
            case 0x2D: // Area Reference Within a Name          [324    ]
                       // Area Reference Within a Shared Formula[    274]
                aIn >> nRow1 >> nRow2 >> nCol1 >> nCol2;

                nTab1 = nTab2 = GetCurrScTab();
                goto _common;
            case 0x5A:
            case 0x7A:
            case 0x3A: // 3-D Cell Reference                    [    275]
                aIn >> nIxti >> nRow1 >> nCol1;

                nRow2 = nRow1;
                nCol2 = nCol1;

                goto _3d_common;
            case 0x5B:
            case 0x7B:
            case 0x3B: // 3-D Area Reference                    [    276]
                aIn >> nIxti >> nRow1 >> nRow2 >> nCol1 >> nCol2;

    _3d_common:
                if( !rLinkMan.GetScTabRange( nTab1, nTab2, nIxti ) )
                    break;

                goto _common;
    _common:
                // do not check abs/rel flags, linked controls have set them!
//               if( !(( nCol1 & 0xC000 ) || ( nCol2 & 0xC000 )) )
                {
                    ScRange aScRange;
                    nCol1 &= 0x3FFF;
                    nCol2 &= 0x3FFF;
                    if( GetAddressConverter().ConvertRange( aScRange, XclRange( nCol1, nRow1, nCol2, nRow2 ), nTab1, nTab2, true ) )
                        r.Append( aScRange );
                }
                break;
            case 0x1C: // Error Value                           [314 266]
            case 0x1D: // Boolean                               [315 266]
                nSeek = 1;
                break;
            case 0x1E: // Integer                               [315 266]
            case 0x41:
            case 0x61:
            case 0x21: // Function, Fixed Number of Arguments   [333 282]
            case 0x49:
            case 0x69:
            case 0x29: // Variable Reference Subexpression      [331 281]
            case 0x4E:
            case 0x6E:
            case 0x2E: // Reference Subexpression Within a Name [332 282]
            case 0x4F:
            case 0x6F:
            case 0x2F: // Incomplete Reference Subexpression... [332 282]
            case 0x58:
            case 0x78:
            case 0x38: // Command-Equivalent Function           [333    ]
                nSeek = 2;
                break;
            case 0x42:
            case 0x62:
            case 0x22: // Function, Variable Number of Arg.     [333 283]
                nSeek = 3;
                break;
            case 0x01: // Array Formula                         [325    ]
            case 0x02: // Data Table                            [325 277]
            case 0x43:
            case 0x63:
            case 0x23: // Name                                  [318 269]
            case 0x4A:
            case 0x6A:
            case 0x2A: // Deleted Cell Reference                [323 273]
                nSeek = 4;
                break;
            case 0x46:
            case 0x66:
            case 0x26: // Constant Reference Subexpression      [321 271]
            case 0x47:
            case 0x67:
            case 0x27: // Erroneous Constant Reference Subexpr. [322 272]
            case 0x48:
            case 0x68:
            case 0x28: // Incomplete Constant Reference Subexpr.[331 281]
            case 0x5C:
            case 0x7C:
            case 0x3C: // Deleted 3-D Cell Reference            [    277]
            case 0x59:
            case 0x79:
            case 0x39: // Name or External Name                 [    275]
                nSeek = 6;
                break;
            case 0x40:
            case 0x60:
            case 0x20: // Array Constant                        [317 268]
                nSeek = 7;
                break;
            case 0x1F: // Number                                [315 266]
            case 0x4B:
            case 0x6B:
            case 0x2B: // Deleted Area Refernce                 [323 273]
                nSeek = 8;
                break;
            case 0x5D:
            case 0x7D:
            case 0x3D: // Deleted 3-D Area Reference            [    277]
                nSeek = 10;
                break;
            case 0x17: // String Constant                       [314 266]
            {
                UINT8   nLen;
                aIn >> nLen;
                aIn.IgnoreUniString( nLen );        // reads Grbit even if nLen==0
                nSeek = 0;
            }
                break;
            case 0x19: // Special Attribute                     [327 279]
            {
                UINT16  nData;
                UINT8   nOpt;
                aIn >> nOpt >> nData;
                if( nOpt & 0x04 )
                {// nFakt -> Bytes oder Words ueberlesen    AttrChoose
                    nData++;
                    nSeek = nData * 2;
            }
            }
                break;
        }

        aIn.Ignore( nSeek );
    }
    aIn.Seek( nMaxPos );

    return r.Count() != 0;
}





