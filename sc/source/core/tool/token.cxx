/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: token.cxx,v $
 * $Revision: 1.33.32.3 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"



// INCLUDE ---------------------------------------------------------------

#if STLPORT_VERSION<321
#include <stddef.h>
#else
#include <cstddef>
#endif
#include <cstdio>

#include <string.h>
#include <tools/mempool.hxx>
#include <tools/debug.hxx>

#include "token.hxx"
#include "tokenarray.hxx"
#include "compiler.hxx"
#include "compiler.hrc"
#include "rechead.hxx"
#include "parclass.hxx"
#include "jumpmatrix.hxx"

using ::std::vector;

// ImpTokenIterator wird je Interpreter angelegt, mehrfache auch durch
// SubCode via ScTokenIterator Push/Pop moeglich
IMPL_FIXEDMEMPOOL_NEWDEL( ImpTokenIterator, 32, 16 )

// Align MemPools on 4k boundaries - 64 bytes (4k is a MUST for OS/2)

// Since RawTokens are temporary for the compiler, don't align on 4k and waste memory.
// ScRawToken size is FixMembers + MAXSTRLEN ~= 264
IMPL_FIXEDMEMPOOL_NEWDEL( ScRawToken, 8, 4 )
// Some ScDoubleRawToken, FixMembers + sizeof(double) ~= 16
const USHORT nMemPoolDoubleRawToken = 0x0400 / sizeof(ScDoubleRawToken);
IMPL_FIXEDMEMPOOL_NEWDEL( ScDoubleRawToken, nMemPoolDoubleRawToken, nMemPoolDoubleRawToken )

// Need a whole bunch of ScSingleRefToken
const USHORT nMemPoolSingleRefToken = (0x4000 - 64) / sizeof(ScSingleRefToken);
IMPL_FIXEDMEMPOOL_NEWDEL( ScSingleRefToken, nMemPoolSingleRefToken, nMemPoolSingleRefToken )
// Need a lot of ScDoubleToken
const USHORT nMemPoolDoubleToken = (0x3000 - 64) / sizeof(ScDoubleToken);
IMPL_FIXEDMEMPOOL_NEWDEL( ScDoubleToken, nMemPoolDoubleToken, nMemPoolDoubleToken )
// Need a lot of ScByteToken
const USHORT nMemPoolByteToken = (0x3000 - 64) / sizeof(ScByteToken);
IMPL_FIXEDMEMPOOL_NEWDEL( ScByteToken, nMemPoolByteToken, nMemPoolByteToken )
// Need quite a lot of ScDoubleRefToken
const USHORT nMemPoolDoubleRefToken = (0x2000 - 64) / sizeof(ScDoubleRefToken);
IMPL_FIXEDMEMPOOL_NEWDEL( ScDoubleRefToken, nMemPoolDoubleRefToken, nMemPoolDoubleRefToken )
// Need several ScStringToken
const USHORT nMemPoolStringToken = (0x1000 - 64) / sizeof(ScStringToken);
IMPL_FIXEDMEMPOOL_NEWDEL( ScStringToken, nMemPoolStringToken, nMemPoolStringToken )


// --- helpers --------------------------------------------------------------

inline BOOL lcl_IsReference( OpCode eOp, StackVar eType )
{
    return
        (eOp == ocPush && (eType == svSingleRef || eType == svDoubleRef))
        || (eOp == ocColRowNameAuto && eType == svDoubleRef)
        || (eOp == ocColRowName && eType == svSingleRef)
        || (eOp == ocMatRef && eType == svSingleRef)
        ;
}


// --- class ScRawToken -----------------------------------------------------

xub_StrLen ScRawToken::GetStrLen( const sal_Unicode* pStr )
{
    if ( !pStr )
        return 0;
    register const sal_Unicode* p = pStr;
    while ( *p )
        p++;
    return sal::static_int_cast<xub_StrLen>( p - pStr );
}


void ScRawToken::SetOpCode( OpCode e )
{
    eOp   = e;
    switch (eOp)
    {
        case ocIf:
            eType = svJump;
            nJump[ 0 ] = 3; // If, Else, Behind
            break;
        case ocChose:
            eType = svJump;
            nJump[ 0 ] = MAXJUMPCOUNT+1;
            break;
        case ocMissing:
            eType = svMissing;
            break;
        case ocSep:
        case ocOpen:
        case ocClose:
        case ocArrayRowSep:
        case ocArrayColSep:
        case ocArrayOpen:
        case ocArrayClose:
            eType = svSep;
            break;
        default:
            eType = svByte;
            sbyte.cByte = 0;
            sbyte.bHasForceArray = ScParameterClassification::HasForceArray( eOp);
    }
    nRefCnt = 0;
}

void ScRawToken::SetString( const sal_Unicode* pStr )
{
    eOp   = ocPush;
    eType = svString;
    if ( pStr )
    {
        xub_StrLen nLen = GetStrLen( pStr ) + 1;
        if( nLen > MAXSTRLEN )
            nLen = MAXSTRLEN;
        memcpy( cStr, pStr, GetStrLenBytes( nLen ) );
        cStr[ nLen-1 ] = 0;
    }
    else
        cStr[0] = 0;
    nRefCnt = 0;
}

void ScRawToken::SetSingleReference( const ScSingleRefData& rRef )
{
    eOp       = ocPush;
    eType     = svSingleRef;
    aRef.Ref1 =
    aRef.Ref2 = rRef;
    nRefCnt   = 0;
}

void ScRawToken::SetDoubleReference( const ScComplexRefData& rRef )
{
    eOp   = ocPush;
    eType = svDoubleRef;
    aRef  = rRef;
    nRefCnt = 0;
}

void ScRawToken::SetDouble(double rVal)
{
    eOp   = ocPush;
    eType = svDouble;
    nValue = rVal;
    nRefCnt = 0;
}

void ScRawToken::SetName( USHORT n )
{
    eOp    = ocName;
    eType  = svIndex;
    nIndex = n;
    nRefCnt = 0;
}

void ScRawToken::SetExternalSingleRef( sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef )
{
    eOp = ocExternalRef;
    eType = svExternalSingleRef;
    nRefCnt = 0;

    extref.nFileId = nFileId;
    extref.aRef.Ref1 =
    extref.aRef.Ref2 = rRef;

    xub_StrLen n = rTabName.Len();
    memcpy(extref.cTabName, rTabName.GetBuffer(), n*sizeof(sal_Unicode));
    extref.cTabName[n] = 0;
}

void ScRawToken::SetExternalDoubleRef( sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef )
{
    eOp = ocExternalRef;
    eType = svExternalDoubleRef;
    nRefCnt = 0;

    extref.nFileId = nFileId;
    extref.aRef = rRef;

    xub_StrLen n = rTabName.Len();
    memcpy(extref.cTabName, rTabName.GetBuffer(), n*sizeof(sal_Unicode));
    extref.cTabName[n] = 0;
}

void ScRawToken::SetExternalName( sal_uInt16 nFileId, const String& rName )
{
    eOp = ocExternalRef;
    eType = svExternalName;
    nRefCnt = 0;

    extname.nFileId = nFileId;

    xub_StrLen n = rName.Len();
    memcpy(extname.cName, rName.GetBuffer(), n*sizeof(sal_Unicode));
    extname.cName[n] = 0;
}

//UNUSED2008-05  void ScRawToken::SetInt(int rVal)
//UNUSED2008-05  {
//UNUSED2008-05      eOp   = ocPush;
//UNUSED2008-05      eType = svDouble;
//UNUSED2008-05      nValue = (double)rVal;
//UNUSED2008-05      nRefCnt = 0;
//UNUSED2008-05
//UNUSED2008-05  }
//UNUSED2008-05  void ScRawToken::SetMatrix( ScMatrix* p )
//UNUSED2008-05  {
//UNUSED2008-05      eOp   = ocPush;
//UNUSED2008-05      eType = svMatrix;
//UNUSED2008-05      pMat  = p;
//UNUSED2008-05      nRefCnt = 0;
//UNUSED2008-05  }
//UNUSED2008-05
//UNUSED2008-05  ScComplexRefData& ScRawToken::GetReference()
//UNUSED2008-05  {
//UNUSED2008-05      DBG_ASSERT( lcl_IsReference( eOp, GetType() ), "GetReference: no Ref" );
//UNUSED2008-05      return aRef;
//UNUSED2008-05  }
//UNUSED2008-05
//UNUSED2008-05  void ScRawToken::SetReference( ScComplexRefData& rRef )
//UNUSED2008-05  {
//UNUSED2008-05      DBG_ASSERT( lcl_IsReference( eOp, GetType() ), "SetReference: no Ref" );
//UNUSED2008-05      aRef = rRef;
//UNUSED2008-05      if( GetType() == svSingleRef )
//UNUSED2008-05          aRef.Ref2 = aRef.Ref1;
//UNUSED2008-05  }

void ScRawToken::SetExternal( const sal_Unicode* pStr )
{
    eOp   = ocExternal;
    eType = svExternal;
    xub_StrLen nLen = GetStrLen( pStr ) + 1;
    if( nLen >= MAXSTRLEN )
        nLen = MAXSTRLEN-1;
    // Platz fuer Byte-Parameter lassen!
    memcpy( cStr+1, pStr, GetStrLenBytes( nLen ) );
    cStr[ nLen+1 ] = 0;
    nRefCnt = 0;
}

USHORT lcl_ScRawTokenOffset()
{
    // offset of sbyte in ScRawToken
    // offsetof(ScRawToken, sbyte) gives a warning with gcc, because ScRawToken is no POD

    ScRawToken aToken;
    return static_cast<USHORT>( reinterpret_cast<char*>(&aToken.sbyte) - reinterpret_cast<char*>(&aToken) );
}

ScRawToken* ScRawToken::Clone() const
{
    ScRawToken* p;
    if ( eType == svDouble )
    {
        p = (ScRawToken*) new ScDoubleRawToken;
        p->eOp = eOp;
        p->eType = eType;
        p->nValue = nValue;
    }
    else
    {
        static USHORT nOffset = lcl_ScRawTokenOffset();     // offset of sbyte
        USHORT n = nOffset;

        if (eOp == ocExternalRef)
        {
            switch (eType)
            {
                case svExternalSingleRef:
                case svExternalDoubleRef: n += sizeof(extref); break;
                case svExternalName:      n += sizeof(extname); break;
                default:
                {
                    DBG_ERROR1( "unknown ScRawToken::Clone() external type %d", int(eType));
                }
            }
        }
        else
        {
            switch( eType )
            {
                case svSep:         break;
                case svByte:        n += sizeof(ScRawToken::sbyte); break;
                case svDouble:      n += sizeof(double); break;
                case svString:      n = sal::static_int_cast<USHORT>( n + GetStrLenBytes( cStr ) + GetStrLenBytes( 1 ) ); break;
                case svSingleRef:
                case svDoubleRef:   n += sizeof(aRef); break;
                case svMatrix:      n += sizeof(ScMatrix*); break;
                case svIndex:       n += sizeof(USHORT); break;
                case svJump:        n += nJump[ 0 ] * 2 + 2; break;
                case svExternal:    n = sal::static_int_cast<USHORT>( n + GetStrLenBytes( cStr+1 ) + GetStrLenBytes( 2 ) ); break;
                default:
                {
                    DBG_ERROR1( "unknown ScRawToken::Clone() type %d", int(eType));
                }
            }
        }
        p = (ScRawToken*) new BYTE[ n ];
        memcpy( p, this, n * sizeof(BYTE) );
    }
    p->nRefCnt = 0;
    p->bRaw = FALSE;
    return p;
}


ScToken* ScRawToken::CreateToken() const
{
#ifndef PRODUCT
#define IF_NOT_OPCODE_ERROR(o,c) if (eOp!=o) DBG_ERROR1( #c "::ctor: OpCode %d lost, converted to " #o "; maybe inherit from ScOpToken instead!", int(eOp))
#else
#define IF_NOT_OPCODE_ERROR(o,c)
#endif
    switch ( GetType() )
    {
        case svByte :
            return new ScByteToken( eOp, sbyte.cByte, sbyte.bHasForceArray );
        case svDouble :
            IF_NOT_OPCODE_ERROR( ocPush, ScDoubleToken);
            return new ScDoubleToken( nValue );
        case svString :
            if (eOp == ocPush)
                return new ScStringToken( String( cStr ) );
            else
                return new ScStringOpToken( eOp, String( cStr ) );
        case svSingleRef :
            if (eOp == ocPush)
                return new ScSingleRefToken( aRef.Ref1 );
            else
                return new ScSingleRefOpToken( eOp, aRef.Ref1 );
        case svDoubleRef :
            if (eOp == ocPush)
                return new ScDoubleRefToken( aRef );
            else
                return new ScDoubleRefOpToken( eOp, aRef );
        case svMatrix :
            IF_NOT_OPCODE_ERROR( ocPush, ScMatrixToken);
            return new ScMatrixToken( pMat );
        case svIndex :
            return new ScIndexToken( eOp, nIndex );
        case svExternalSingleRef:
            {
                String aTabName(extref.cTabName);
                return new ScExternalSingleRefToken(extref.nFileId, aTabName, extref.aRef.Ref1);
            }
        case svExternalDoubleRef:
            {
                String aTabName(extref.cTabName);
                return new ScExternalDoubleRefToken(extref.nFileId, aTabName, extref.aRef);
            }
        case svExternalName:
            {
                String aName(extname.cName);
                return new ScExternalNameToken( extname.nFileId, aName );
            }
        case svJump :
            return new ScJumpToken( eOp, (short*) nJump );
        case svExternal :
            return new ScExternalToken( eOp, sbyte.cByte, String( cStr+1 ) );
        case svFAP :
            return new ScFAPToken( eOp, sbyte.cByte, NULL );
        case svMissing :
            IF_NOT_OPCODE_ERROR( ocMissing, ScMissingToken);
            return new ScMissingToken;
        case svSep :
            return new ScOpToken( eOp, svSep );
        case svUnknown :
            return new ScUnknownToken( eOp );
        default:
            {
                DBG_ERROR1( "unknown ScRawToken::CreateToken() type %d", int(GetType()));
                return new ScUnknownToken( ocBad );
            }
    }
#undef IF_NOT_OPCODE_ERROR
}


void ScRawToken::Delete()
{
    if ( bRaw )
        delete this;                            // FixedMemPool ScRawToken
    else
    {   // created per Clone
        switch ( eType )
        {
            case svDouble :
                delete (ScDoubleRawToken*) this;    // FixedMemPool ScDoubleRawToken
            break;
            default:
                delete [] (BYTE*) this;
        }
    }
}


// --- class ScToken --------------------------------------------------------

ScSingleRefData lcl_ScToken_InitSingleRef()
{
    ScSingleRefData aRef;
    aRef.InitAddress( ScAddress() );
    return aRef;
}

ScComplexRefData lcl_ScToken_InitDoubleRef()
{
    ScComplexRefData aRef;
    aRef.Ref1 = lcl_ScToken_InitSingleRef();
    aRef.Ref2 = aRef.Ref1;
    return aRef;
}

String          ScToken::aDummyString;


ScToken::~ScToken()
{
}


OpCode ScToken::GetOpCode() const
{
    return ocPush;
}


BOOL ScToken::IsFunction() const
{
    OpCode eOp = GetOpCode();
    return (eOp != ocPush && eOp != ocBad && eOp != ocColRowName &&
            eOp != ocColRowNameAuto && eOp != ocName && eOp != ocDBArea &&
           (GetByte() != 0                                                  // x parameters
        || (SC_OPCODE_START_NO_PAR <= eOp && eOp < SC_OPCODE_STOP_NO_PAR)   // no parameter
        || (ocIf == eOp ||  ocChose ==  eOp     )                           // @ jump commands
        || (SC_OPCODE_START_1_PAR <= eOp && eOp < SC_OPCODE_STOP_1_PAR)     // one parameter
        || (SC_OPCODE_START_2_PAR <= eOp && eOp < SC_OPCODE_STOP_2_PAR)     // x parameters (cByte==0 in
                                                                            // FuncAutoPilot)
        || eOp == ocMacro || eOp == ocExternal                  // macros, AddIns
        || eOp == ocAnd || eOp == ocOr                          // former binary, now x parameters
        || eOp == ocNot || eOp == ocNeg                         // unary but function
        || (eOp >= ocInternalBegin && eOp <= ocInternalEnd)     // internal
        ));
}


BYTE ScToken::GetParamCount() const
{
    OpCode eOp = GetOpCode();
    if ( eOp < SC_OPCODE_STOP_DIV && eOp != ocExternal && eOp != ocMacro &&
            eOp != ocIf && eOp != ocChose && eOp != ocPercentSign )
        return 0;       // parameters and specials
                        // ocIf and ocChose not for FAP, have cByte then
//2do: BOOL parameter whether FAP or not?
    else if ( GetByte() )
        return GetByte();   // all functions, also ocExternal and ocMacro
    else if (SC_OPCODE_START_BIN_OP <= eOp && eOp < SC_OPCODE_STOP_BIN_OP)
        return 2;           // binary
    else if ((SC_OPCODE_START_UN_OP <= eOp && eOp < SC_OPCODE_STOP_UN_OP)
            || eOp == ocPercentSign)
        return 1;           // unary
    else if (SC_OPCODE_START_NO_PAR <= eOp && eOp < SC_OPCODE_STOP_NO_PAR)
        return 0;           // no parameter
    else if (SC_OPCODE_START_1_PAR <= eOp && eOp < SC_OPCODE_STOP_1_PAR)
        return 1;           // one parameter
    else if ( eOp == ocIf || eOp == ocChose )
        return 1;           // only the condition counts as parameter
    else
        return 0;           // all the rest, no Parameter, or
                            // if so then it should be in cByte
}


BOOL ScToken::IsMatrixFunction() const
{
    OpCode eOp = GetOpCode();
    switch ( eOp )
    {
        case ocDde :
        case ocGrowth :
        case ocTrend :
        case ocRKP :
        case ocRGP :
        case ocFrequency :
        case ocMatTrans :
        case ocMatMult :
        case ocMatInv :
        case ocMatrixUnit :
            return TRUE;
        default:
        {
            // added to avoid warnings
        }
    }
    return FALSE;
}


ScToken* ScToken::Clone() const
{
    OpCode nOp = GetOpCode();
    switch ( GetType() )
    {
        case svByte :
            return new ScByteToken( *static_cast<const ScByteToken*>(this) );
        case svDouble :
            return new ScDoubleToken( *static_cast<const ScDoubleToken*>(this) );
        case svString :
            if (nOp == ocPush)
                return new ScStringToken( *static_cast<const ScStringToken*>(this) );
            else
                return new ScStringOpToken( *static_cast<const ScStringOpToken*>(this) );
        case svSingleRef :
            if (nOp == ocPush)
                return new ScSingleRefToken( *static_cast<const ScSingleRefToken*>(this) );
            else
                return new ScSingleRefOpToken( *static_cast<const ScSingleRefOpToken*>(this) );
        case svDoubleRef :
            if (nOp == ocPush)
                return new ScDoubleRefToken( *static_cast<const ScDoubleRefToken*>(this) );
            else
                return new ScDoubleRefOpToken( *static_cast<const ScDoubleRefOpToken*>(this) );
        case svMatrix :
            return new ScMatrixToken( *static_cast<const ScMatrixToken*>(this) );
        case svIndex :
            return new ScIndexToken( *static_cast<const ScIndexToken*>(this) );
        case svJump :
            return new ScJumpToken( *static_cast<const ScJumpToken*>(this) );
        case svJumpMatrix :
            return new ScJumpMatrixToken( *static_cast<const ScJumpMatrixToken*>(this) );
        case svRefList :
            return new ScRefListToken( *static_cast<const ScRefListToken*>(this) );
        case svExternal :
            return new ScExternalToken( *static_cast<const ScExternalToken*>(this) );
        case svExternalSingleRef :
            return new ScExternalSingleRefToken( *static_cast<const ScExternalSingleRefToken*>(this) );
        case svExternalDoubleRef :
            return new ScExternalDoubleRefToken( *static_cast<const ScExternalDoubleRefToken*>(this) );
        case svExternalName :
            return new ScExternalNameToken( *static_cast<const ScExternalNameToken*>(this) );
        case svFAP :
            return new ScFAPToken( *static_cast<const ScFAPToken*>(this) );
        case svMissing :
            return new ScMissingToken( *static_cast<const ScMissingToken*>(this) );
        case svError :
            return new ScErrorToken( *static_cast<const ScErrorToken*>(this) );
        case svEmptyCell :
            return new ScEmptyCellToken( *static_cast<const ScEmptyCellToken*>(this) );
        case svSep :
            return new ScOpToken( *static_cast<const ScOpToken*>(this) );
        case svUnknown :
            return new ScUnknownToken( *static_cast<const ScUnknownToken*>(this) );
        default:
            DBG_ERROR1( "unknown ScToken::Clone() type %d", int(GetType()));
            return new ScUnknownToken( ocBad );
    }
}

BOOL ScToken::operator==( const ScToken& rToken ) const
{
    // don't compare reference count!
    return  eType == rToken.eType && GetOpCode() == rToken.GetOpCode();
}


//  TextEqual: if same formula entered (for optimization in sort)
BOOL ScToken::TextEqual( const ScToken& rToken ) const
{
    if ( eType == svSingleRef || eType == svDoubleRef )
    {
        //  in relative Refs only compare relative parts

        if ( eType != rToken.eType || GetOpCode() != rToken.GetOpCode() )
            return FALSE;

        ScComplexRefData aTemp1;
        if ( eType == svSingleRef )
        {
            aTemp1.Ref1 = GetSingleRef();
            aTemp1.Ref2 = aTemp1.Ref1;
        }
        else
            aTemp1 = GetDoubleRef();

        ScComplexRefData aTemp2;
        if ( rToken.eType == svSingleRef )
        {
            aTemp2.Ref1 = rToken.GetSingleRef();
            aTemp2.Ref2 = aTemp2.Ref1;
        }
        else
            aTemp2 = rToken.GetDoubleRef();

        ScAddress aPos;
        aTemp1.SmartRelAbs(aPos);
        aTemp2.SmartRelAbs(aPos);

        //  memcmp doesn't work because of the alignment byte after bFlags.
        //  After SmartRelAbs only absolute parts have to be compared.
        return aTemp1.Ref1.nCol   == aTemp2.Ref1.nCol   &&
               aTemp1.Ref1.nRow   == aTemp2.Ref1.nRow   &&
               aTemp1.Ref1.nTab   == aTemp2.Ref1.nTab   &&
               aTemp1.Ref1.bFlags == aTemp2.Ref1.bFlags &&
               aTemp1.Ref2.nCol   == aTemp2.Ref2.nCol   &&
               aTemp1.Ref2.nRow   == aTemp2.Ref2.nRow   &&
               aTemp1.Ref2.nTab   == aTemp2.Ref2.nTab   &&
               aTemp1.Ref2.bFlags == aTemp2.Ref2.bFlags;
    }
    else
        return *this == rToken;     // else normal operator==
}


BOOL ScToken::Is3DRef() const
{
    switch ( eType )
    {
        case svDoubleRef :
            if ( GetSingleRef2().IsFlag3D() )
                return TRUE;
        //! fallthru
        case svSingleRef :
            if ( GetSingleRef().IsFlag3D() )
                return TRUE;
            break;
        default:
        {
            // added to avoid warnings
        }
    }
    return FALSE;
}


//UNUSED2008-05  BOOL ScToken::IsRPNReferenceAbsName() const
//UNUSED2008-05  {
//UNUSED2008-05      if ( GetRef() == 1 && GetOpCode() == ocPush )
//UNUSED2008-05      {   // only in RPN and not ocColRowNameAuto or similar
//UNUSED2008-05          switch ( GetType() )
//UNUSED2008-05          {
//UNUSED2008-05              case svDoubleRef :
//UNUSED2008-05                  if ( !GetSingleRef2().IsRelName() )
//UNUSED2008-05                      return TRUE;
//UNUSED2008-05                  //! fallthru
//UNUSED2008-05              case svSingleRef :
//UNUSED2008-05                  if ( !GetSingleRef().IsRelName() )
//UNUSED2008-05                      return TRUE;
//UNUSED2008-05                  break;
//UNUSED2008-05              default:
//UNUSED2008-05                  {
//UNUSED2008-05                      // added to avoid warnings
//UNUSED2008-05                  }
//UNUSED2008-05          }
//UNUSED2008-05      }
//UNUSED2008-05      return FALSE;
//UNUSED2008-05  }


// static
ScTokenRef ScToken::ExtendRangeReference( ScToken & rTok1, ScToken & rTok2,
        const ScAddress & rPos, bool bReuseDoubleRef )
{
    ScToken *p1, *p2;
    StackVar sv1, sv2;
    // Doing a RangeOp with RefList is probably utter nonsense, but Xcl
    // supports it, so do we.
    if (((p1 = &rTok1) == 0) || ((p2 = &rTok2) == 0) ||
            ((sv1 = p1->GetType()) != svSingleRef && sv1 != svDoubleRef && sv1 != svRefList &&
             sv1 != svExternalSingleRef && sv1 != svExternalDoubleRef) ||
            ((sv2 = p2->GetType()) != svSingleRef && sv2 != svDoubleRef && sv2 != svRefList))
        return NULL;

    ScTokenRef xRes;
    bool bExternal = (sv1 == svExternalSingleRef);
    if ((sv1 == svSingleRef || bExternal) && sv2 == svSingleRef)
    {
        // Range references like Sheet1.A1:A2 are generalized and built by
        // first creating a DoubleRef from the first SingleRef, effectively
        // generating Sheet1.A1:A1, and then extending that with A2 as if
        // Sheet1.A1:A1:A2 was encountered, so the mechanisms to adjust the
        // references apply as well.
<<<<<<< .working

        /* Given the current structure of external references an external
         * reference can only be extended if the second reference does not
         * point to a different sheet. 'file'#Sheet1.A1:A2 is ok,
         * 'file'#Sheet1.A1:Sheet2.A2 is not. Since we can't determine from a
         * svSingleRef whether the sheet would be different from the one given
         * in the external reference, we have to bail out if there is any sheet
         * specified. NOTE: Xcl does handle external 3D references as in
         * '[file]Sheet1:Sheet2'!A1:A2
         *
         * FIXME: For OOo syntax be smart and remember an external singleref
         * encountered and if followed by ocRange and singleref, create an
         * external singleref for the second singleref. Both could then be
         * merged here. For Xcl syntax already parse an external range
         * reference entirely, cumbersome. */

        const ScSingleRefData& rRef2 = p2->GetSingleRef();
        if (bExternal && rRef2.IsFlag3D())
            return NULL;

        ScComplexRefData aRef;
        aRef.Ref1 = aRef.Ref2 = p1->GetSingleRef();
        aRef.Ref2.SetFlag3D( false);
        aRef.Extend( rRef2, rPos);
        if (bExternal)
            xRes = new ScExternalDoubleRefToken( p1->GetIndex(), p1->GetString(), aRef);
        else
            xRes = new ScDoubleRefToken( aRef);
    }
    else
    {
        bExternal |= (sv1 == svExternalDoubleRef);
        const ScRefList* pRefList = NULL;
        if (sv1 == svDoubleRef)
        {
            xRes = (bReuseDoubleRef && p1->GetRef() == 1 ? p1 : p1->Clone());
            sv1 = svUnknown;    // mark as handled
        }
        else if (sv2 == svDoubleRef)
        {
            xRes = (bReuseDoubleRef && p2->GetRef() == 1 ? p2 : p2->Clone());
            sv2 = svUnknown;    // mark as handled
        }
        else if (sv1 == svRefList)
            pRefList = p1->GetRefList();
        else if (sv2 == svRefList)
            pRefList = p2->GetRefList();
        if (pRefList)
        {
            if (!pRefList->size())
                return NULL;
            if (bExternal)
                return NULL;    // external reference list not possible
            xRes = new ScDoubleRefToken( (*pRefList)[0] );
        }
        if (!xRes)
            return NULL;    // shouldn't happen..
        StackVar sv[2] = { sv1, sv2 };
        ScToken* pt[2] = { p1, p2 };
        ScComplexRefData& rRef = xRes->GetDoubleRef();
        for (size_t i=0; i<2; ++i)
        {
            switch (sv[i])
            {
                case svSingleRef:
                    rRef.Extend( pt[i]->GetSingleRef(), rPos);
                    break;
                case svDoubleRef:
                    rRef.Extend( pt[i]->GetDoubleRef(), rPos);
                    break;
                case svRefList:
                    {
                        const ScRefList* p = pt[i]->GetRefList();
                        if (!p->size())
                            return NULL;
                        ScRefList::const_iterator it( p->begin());
                        ScRefList::const_iterator end( p->end());
                        for ( ; it != end; ++it)
                        {
                            rRef.Extend( *it, rPos);
                        }
                    }
                    break;
                case svExternalSingleRef:
                    if (rRef.Ref1.IsFlag3D() || rRef.Ref2.IsFlag3D())
                        return NULL;    // no other sheets with external refs
                    else
                        rRef.Extend( pt[i]->GetSingleRef(), rPos);
                    break;
                case svExternalDoubleRef:
                    if (rRef.Ref1.IsFlag3D() || rRef.Ref2.IsFlag3D())
                        return NULL;    // no other sheets with external refs
                    else
                        rRef.Extend( pt[i]->GetDoubleRef(), rPos);
                    break;
                default:
                    ;   // nothing, prevent compiler warning
            }
        }
    }
    return xRes;
}


// --- virtual dummy methods -------------------------------------------------

BYTE ScToken::GetByte() const
{
    // ok to be called for any derived class
    return 0;
}

void ScToken::SetByte( BYTE )
{
    DBG_ERRORFILE( "ScToken::SetByte: virtual dummy called" );
}

bool ScToken::HasForceArray() const
{
    // ok to be called for any derived class
    return false;
}

void ScToken::SetForceArray( bool )
{
    DBG_ERRORFILE( "ScToken::SetForceArray: virtual dummy called" );
}

double ScToken::GetDouble() const
{
    DBG_ERRORFILE( "ScToken::GetDouble: virtual dummy called" );
    return 0.0;
}

double & ScToken::GetDoubleAsReference()
{
    DBG_ERRORFILE( "ScToken::GetDouble: virtual dummy called" );
    static double fVal = 0.0;
    return fVal;
}

const String& ScToken::GetString() const
{
    DBG_ERRORFILE( "ScToken::GetString: virtual dummy called" );
    return aDummyString;
}

const ScSingleRefData& ScToken::GetSingleRef() const
{
    DBG_ERRORFILE( "ScToken::GetSingleRef: virtual dummy called" );
    static ScSingleRefData aDummySingleRef = lcl_ScToken_InitSingleRef();
    return aDummySingleRef;
}

ScSingleRefData& ScToken::GetSingleRef()
{
    DBG_ERRORFILE( "ScToken::GetSingleRef: virtual dummy called" );
    static ScSingleRefData aDummySingleRef = lcl_ScToken_InitSingleRef();
    return aDummySingleRef;
}

const ScComplexRefData& ScToken::GetDoubleRef() const
{
    DBG_ERRORFILE( "ScToken::GetDoubleRef: virtual dummy called" );
    static ScComplexRefData aDummyDoubleRef = lcl_ScToken_InitDoubleRef();
    return aDummyDoubleRef;
}

ScComplexRefData& ScToken::GetDoubleRef()
{
    DBG_ERRORFILE( "ScToken::GetDoubleRef: virtual dummy called" );
    static ScComplexRefData aDummyDoubleRef = lcl_ScToken_InitDoubleRef();
    return aDummyDoubleRef;
}

const ScSingleRefData& ScToken::GetSingleRef2() const
{
    DBG_ERRORFILE( "ScToken::GetSingleRef2: virtual dummy called" );
    static ScSingleRefData aDummySingleRef = lcl_ScToken_InitSingleRef();
    return aDummySingleRef;
}

ScSingleRefData& ScToken::GetSingleRef2()
{
    DBG_ERRORFILE( "ScToken::GetSingleRef2: virtual dummy called" );
    static ScSingleRefData aDummySingleRef = lcl_ScToken_InitSingleRef();
    return aDummySingleRef;
}

void ScToken::CalcAbsIfRel( const ScAddress& /* rPos */ )
{
    DBG_ERRORFILE( "ScToken::CalcAbsIfRel: virtual dummy called" );
}

void ScToken::CalcRelFromAbs( const ScAddress& /* rPos */ )
{
    DBG_ERRORFILE( "ScToken::CalcRelFromAbs: virtual dummy called" );
}

const ScMatrix* ScToken::GetMatrix() const
{
    DBG_ERRORFILE( "ScToken::GetMatrix: virtual dummy called" );
    return NULL;
}

ScMatrix* ScToken::GetMatrix()
{
    DBG_ERRORFILE( "ScToken::GetMatrix: virtual dummy called" );
    return NULL;
}

USHORT ScToken::GetIndex() const
{
    DBG_ERRORFILE( "ScToken::GetIndex: virtual dummy called" );
    return 0;
}

void ScToken::SetIndex( USHORT )
{
    DBG_ERRORFILE( "ScToken::SetIndex: virtual dummy called" );
}

short* ScToken::GetJump() const
{
    DBG_ERRORFILE( "ScToken::GetJump: virtual dummy called" );
    return NULL;
}

ScJumpMatrix* ScToken::GetJumpMatrix() const
{
    DBG_ERRORFILE( "ScToken::GetJumpMatrix: virtual dummy called" );
    return NULL;
}

const String& ScToken::GetExternal() const
{
    DBG_ERRORFILE( "ScToken::GetExternal: virtual dummy called" );
    return aDummyString;
}

ScToken* ScToken::GetFAPOrigToken() const
{
    DBG_ERRORFILE( "ScToken::GetFAPOrigToken: virtual dummy called" );
    return NULL;
}

const ScRefList* ScToken::GetRefList() const
{
    DBG_ERRORFILE( "ScToken::GetRefList: virtual dummy called" );
    return NULL;
}

ScRefList* ScToken::GetRefList()
{
    DBG_ERRORFILE( "ScToken::GetRefList: virtual dummy called" );
    return NULL;
}

USHORT ScToken::GetError() const
{
    DBG_ERRORFILE( "ScToken::GetError: virtual dummy called" );
    return 0;
}

void ScToken::SetError( USHORT )
{
    DBG_ERRORFILE( "ScToken::SetError: virtual dummy called" );
}

// ==========================================================================
// real implementations of virtual functions
// --------------------------------------------------------------------------


OpCode ScOpToken::GetOpCode() const                     { return eOp; }


BYTE ScByteToken::GetByte() const                       { return nByte; }
void ScByteToken::SetByte( BYTE n )                     { nByte = n; }
bool ScByteToken::HasForceArray() const                 { return bHasForceArray; }
void ScByteToken::SetForceArray( bool b )               { bHasForceArray = b; }
BOOL ScByteToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && nByte == r.GetByte() &&
        bHasForceArray == r.HasForceArray();
}


ScToken* ScFAPToken::GetFAPOrigToken() const            { return pOrigToken; }
BOOL ScFAPToken::operator==( const ScToken& r ) const
{
    return ScByteToken::operator==( r ) && pOrigToken == r.GetFAPOrigToken();
}


double      ScDoubleToken::GetDouble() const            { return fDouble; }
double &    ScDoubleToken::GetDoubleAsReference()       { return fDouble; }
BOOL ScDoubleToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && fDouble == r.GetDouble();
}


const String& ScStringToken::GetString() const          { return aString; }
BOOL ScStringToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && aString == r.GetString();
}


const String& ScStringOpToken::GetString() const             { return aString; }
BOOL ScStringOpToken::operator==( const ScToken& r ) const
{
    return ScByteToken::operator==( r ) && aString == r.GetString();
}


const ScSingleRefData&    ScSingleRefToken::GetSingleRef() const  { return aSingleRef; }
ScSingleRefData&          ScSingleRefToken::GetSingleRef()        { return aSingleRef; }
void                    ScSingleRefToken::CalcAbsIfRel( const ScAddress& rPos )
                            { aSingleRef.CalcAbsIfRel( rPos ); }
void                    ScSingleRefToken::CalcRelFromAbs( const ScAddress& rPos )
                            { aSingleRef.CalcRelFromAbs( rPos ); }
BOOL ScSingleRefToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && aSingleRef == r.GetSingleRef();
}


const ScSingleRefData&    ScSingleRefOpToken::GetSingleRef() const  { return aSingleRef; }
ScSingleRefData&          ScSingleRefOpToken::GetSingleRef()        { return aSingleRef; }
void                    ScSingleRefOpToken::CalcAbsIfRel( const ScAddress& rPos )
                            { aSingleRef.CalcAbsIfRel( rPos ); }
void                    ScSingleRefOpToken::CalcRelFromAbs( const ScAddress& rPos )
                            { aSingleRef.CalcRelFromAbs( rPos ); }
BOOL ScSingleRefOpToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && aSingleRef == r.GetSingleRef();
}


const ScSingleRefData&    ScDoubleRefToken::GetSingleRef() const  { return aDoubleRef.Ref1; }
ScSingleRefData&          ScDoubleRefToken::GetSingleRef()        { return aDoubleRef.Ref1; }
const ScComplexRefData&     ScDoubleRefToken::GetDoubleRef() const  { return aDoubleRef; }
ScComplexRefData&           ScDoubleRefToken::GetDoubleRef()        { return aDoubleRef; }
const ScSingleRefData&    ScDoubleRefToken::GetSingleRef2() const { return aDoubleRef.Ref2; }
ScSingleRefData&          ScDoubleRefToken::GetSingleRef2()       { return aDoubleRef.Ref2; }
void                    ScDoubleRefToken::CalcAbsIfRel( const ScAddress& rPos )
                            { aDoubleRef.CalcAbsIfRel( rPos ); }
void                    ScDoubleRefToken::CalcRelFromAbs( const ScAddress& rPos )
                            { aDoubleRef.CalcRelFromAbs( rPos ); }
BOOL ScDoubleRefToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && aDoubleRef == r.GetDoubleRef();
}


const ScSingleRefData&    ScDoubleRefOpToken::GetSingleRef() const  { return aDoubleRef.Ref1; }
ScSingleRefData&          ScDoubleRefOpToken::GetSingleRef()        { return aDoubleRef.Ref1; }
const ScComplexRefData&     ScDoubleRefOpToken::GetDoubleRef() const  { return aDoubleRef; }
ScComplexRefData&           ScDoubleRefOpToken::GetDoubleRef()        { return aDoubleRef; }
const ScSingleRefData&    ScDoubleRefOpToken::GetSingleRef2() const { return aDoubleRef.Ref2; }
ScSingleRefData&          ScDoubleRefOpToken::GetSingleRef2()       { return aDoubleRef.Ref2; }
void                    ScDoubleRefOpToken::CalcAbsIfRel( const ScAddress& rPos )
                            { aDoubleRef.CalcAbsIfRel( rPos ); }
void                    ScDoubleRefOpToken::CalcRelFromAbs( const ScAddress& rPos )
                            { aDoubleRef.CalcRelFromAbs( rPos ); }
BOOL ScDoubleRefOpToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && aDoubleRef == r.GetDoubleRef();
}


const ScRefList*        ScRefListToken::GetRefList() const  { return &aRefList; }
      ScRefList*        ScRefListToken::GetRefList()        { return &aRefList; }
void                    ScRefListToken::CalcAbsIfRel( const ScAddress& rPos )
{
    for (ScRefList::iterator it( aRefList.begin()); it != aRefList.end(); ++it)
        (*it).CalcAbsIfRel( rPos);
}
void                    ScRefListToken::CalcRelFromAbs( const ScAddress& rPos )
{
    for (ScRefList::iterator it( aRefList.begin()); it != aRefList.end(); ++it)
        (*it).CalcRelFromAbs( rPos);
}
BOOL ScRefListToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && &aRefList == r.GetRefList();
}


const ScMatrix* ScMatrixToken::GetMatrix() const        { return pMatrix; }
ScMatrix*       ScMatrixToken::GetMatrix()              { return pMatrix; }
BOOL ScMatrixToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && pMatrix == r.GetMatrix();
}


USHORT  ScIndexToken::GetIndex() const                  { return nIndex; }
void    ScIndexToken::SetIndex( USHORT n )              { nIndex = n; }
BOOL ScIndexToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && nIndex == r.GetIndex();
}

// ============================================================================

ScExternalSingleRefToken::ScExternalSingleRefToken( sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& r ) :
    ScOpToken(ocExternalRef, svExternalSingleRef),
    mnFileId(nFileId),
    maTabName(rTabName),
    maSingleRef(r)
{
}

ScExternalSingleRefToken::ScExternalSingleRefToken( const ScExternalSingleRefToken& r ) :
    ScOpToken(r),
    mnFileId(r.mnFileId),
    maTabName(r.maTabName),
    maSingleRef(r.maSingleRef)
{
}

ScExternalSingleRefToken::~ScExternalSingleRefToken()
{
}

USHORT ScExternalSingleRefToken::GetIndex() const
{
    return mnFileId;
}

const String& ScExternalSingleRefToken::GetString() const
{
    return maTabName;
}

const ScSingleRefData& ScExternalSingleRefToken::GetSingleRef() const
{
    return maSingleRef;
}

ScSingleRefData& ScExternalSingleRefToken::GetSingleRef()
{
    return maSingleRef;
}

BOOL ScExternalSingleRefToken::operator ==( const ScToken& r ) const
{
    if (!ScToken::operator==(r))
        return false;

    if (mnFileId != r.GetIndex())
        return false;

    if (maTabName != r.GetString())
        return false;

    return maSingleRef == r.GetSingleRef();
}

// ============================================================================

ScExternalDoubleRefToken::ScExternalDoubleRefToken( sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& r ) :
    ScOpToken(ocExternalRef, svExternalDoubleRef),
    mnFileId(nFileId),
    maTabName(rTabName),
    maDoubleRef(r)
{
}

ScExternalDoubleRefToken::ScExternalDoubleRefToken( const ScExternalDoubleRefToken& r ) :
    ScOpToken(r),
    mnFileId(r.mnFileId),
    maTabName(r.maTabName),
    maDoubleRef(r.maDoubleRef)
{
}

ScExternalDoubleRefToken::~ScExternalDoubleRefToken()
{
}

USHORT ScExternalDoubleRefToken::GetIndex() const
{
    return mnFileId;
}

const String& ScExternalDoubleRefToken::GetString() const
{
    return maTabName;
}

const ScSingleRefData& ScExternalDoubleRefToken::GetSingleRef() const
{
    return maDoubleRef.Ref1;
}

ScSingleRefData& ScExternalDoubleRefToken::GetSingleRef()
{
    return maDoubleRef.Ref1;
}

const ScSingleRefData& ScExternalDoubleRefToken::GetSingleRef2() const
{
    return maDoubleRef.Ref2;
}

ScSingleRefData& ScExternalDoubleRefToken::GetSingleRef2()
{
    return maDoubleRef.Ref2;
}

const ScComplexRefData& ScExternalDoubleRefToken::GetDoubleRef() const
{
    return maDoubleRef;
}

ScComplexRefData& ScExternalDoubleRefToken::GetDoubleRef()
{
    return maDoubleRef;
}

BOOL ScExternalDoubleRefToken::operator ==( const ScToken& r ) const
{
    if (!ScToken::operator==(r))
        return false;

    if (mnFileId != r.GetIndex())
        return false;

    if (maTabName != r.GetString())
        return false;

    return maDoubleRef == r.GetDoubleRef();
}

// ============================================================================

ScExternalNameToken::ScExternalNameToken( sal_uInt16 nFileId, const String& rName ) :
    ScOpToken(ocExternalRef, svExternalName),
    mnFileId(nFileId),
    maName(rName)
{
}

ScExternalNameToken::ScExternalNameToken( const ScExternalNameToken& r ) :
    ScOpToken(r),
    mnFileId(r.mnFileId),
    maName(r.maName)
{
}

ScExternalNameToken::~ScExternalNameToken() {}

USHORT ScExternalNameToken::GetIndex() const
{
    return mnFileId;
}

const String& ScExternalNameToken::GetString() const
{
    return maName;
}

BOOL ScExternalNameToken::operator==( const ScToken& r ) const
{
    if ( !ScToken::operator==(r) )
        return false;

    if (mnFileId != r.GetIndex())
        return false;

    xub_StrLen nLen = maName.Len();
    const String& rName = r.GetString();
    if (nLen != rName.Len())
        return false;

    const sal_Unicode* p1 = maName.GetBuffer();
    const sal_Unicode* p2 = rName.GetBuffer();
    for (xub_StrLen j = 0; j < nLen; ++j)
    {
        if (p1[j] != p2[j])
            return false;
    }
    return true;
}

// ============================================================================

short* ScJumpToken::GetJump() const                     { return pJump; }
BOOL ScJumpToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && pJump[0] == r.GetJump()[0] &&
        memcmp( pJump+1, r.GetJump()+1, pJump[0] * sizeof(short) ) == 0;
}
ScJumpToken::~ScJumpToken()
{
    delete [] pJump;
}


ScJumpMatrix* ScJumpMatrixToken::GetJumpMatrix() const  { return pJumpMatrix; }
BOOL ScJumpMatrixToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && pJumpMatrix == r.GetJumpMatrix();
}
ScJumpMatrixToken::~ScJumpMatrixToken()
{
    delete pJumpMatrix;
}


const String&   ScExternalToken::GetExternal() const    { return aExternal; }
BYTE            ScExternalToken::GetByte() const        { return nByte; }
void            ScExternalToken::SetByte( BYTE n )      { nByte = n; }
BOOL ScExternalToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) && nByte == r.GetByte() &&
        aExternal == r.GetExternal();
}


USHORT          ScErrorToken::GetError() const          { return nError; }
void            ScErrorToken::SetError( USHORT nErr )   { nError = nErr; }
BOOL ScErrorToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) &&
        nError == static_cast< const ScErrorToken & >(r).GetError();
}


double          ScEmptyCellToken::GetDouble() const     { return 0.0; }
const String &  ScEmptyCellToken::GetString() const     { return aDummyString; }
BOOL ScEmptyCellToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) &&
        bInherited == static_cast< const ScEmptyCellToken & >(r).IsInherited() &&
        bDisplayedAsString == static_cast< const ScEmptyCellToken & >(r).IsDisplayedAsString();
}


double          ScMatrixCellResultToken::GetDouble() const  { return xUpperLeft->GetDouble(); }
const String &  ScMatrixCellResultToken::GetString() const  { return xUpperLeft->GetString(); }
const ScMatrix* ScMatrixCellResultToken::GetMatrix() const  { return xMatrix; }
// Non-const GetMatrix() is private and unused but must be implemented to
// satisfy vtable linkage.
      ScMatrix* ScMatrixCellResultToken::GetMatrix()
{
    return const_cast<ScMatrix*>(xMatrix.operator->());
}
BOOL ScMatrixCellResultToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) &&
        xUpperLeft == static_cast<const ScMatrixCellResultToken &>(r).xUpperLeft &&
        xMatrix == static_cast<const ScMatrixCellResultToken &>(r).xMatrix;
}


BOOL ScMatrixFormulaCellToken::operator==( const ScToken& r ) const
{
    const ScMatrixFormulaCellToken* p = dynamic_cast<const ScMatrixFormulaCellToken*>(&r);
    return p && ScMatrixCellResultToken::operator==( r ) &&
        nCols == p->nCols && nRows == p->nRows;
}
void ScMatrixFormulaCellToken::Assign( const ScToken& r )
{
    if (this == &r)
        return;
    const ScMatrixCellResultToken* p = dynamic_cast<const ScMatrixCellResultToken*>(&r);
    if (p)
        ScMatrixCellResultToken::Assign( *p);
    else
    {
        DBG_ASSERT( r.GetType() != svMatrix, "ScMatrixFormulaCellToken::operator=: assigning ScMatrixToken to ScMatrixFormulaCellToken is not proper, use ScMatrixCellResultToken instead");
        if (r.GetType() == svMatrix)
        {
            xUpperLeft = NULL;
            xMatrix = r.GetMatrix();
        }
        else
        {
            xUpperLeft = &r;
            xMatrix = NULL;
        }
    }
}
void ScMatrixFormulaCellToken::SetUpperLeftDouble( double f )
{
    switch (GetUpperLeftType())
    {
        case svDouble:
            {
                const ScToken* pT = xUpperLeft;
                const_cast<ScToken*>(pT)->GetDoubleAsReference() = f;
            }
            break;
        case svUnknown:
            if (!xUpperLeft)
            {
                xUpperLeft = new ScDoubleToken( f);
                break;
            }
            // fall thru
        default:
            {
                DBG_ERRORFILE("ScMatrixFormulaCellToken::SetUpperLeftDouble: not modifying unhandled token type");
            }
    }
}


double          ScHybridCellToken::GetDouble() const    { return fDouble; }
const String &  ScHybridCellToken::GetString() const    { return aString; }
BOOL ScHybridCellToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r ) &&
        fDouble == r.GetDouble() && aString == r.GetString() &&
        aFormula == static_cast<const ScHybridCellToken &>(r).GetFormula();
}


double          ScMissingToken::GetDouble() const       { return 0.0; }
const String&   ScMissingToken::GetString() const       { return aDummyString; }
BOOL ScMissingToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r );
}


BOOL ScUnknownToken::operator==( const ScToken& r ) const
{
    return ScToken::operator==( r );
}


//////////////////////////////////////////////////////////////////////////

ScToken* ScTokenArray::GetNextReference()
{
    while( nIndex < nLen )
    {
        ScToken* t = pCode[ nIndex++ ];
        switch( t->GetType() )
        {
            case svSingleRef:
            case svDoubleRef:
            case svExternalSingleRef:
            case svExternalDoubleRef:
                return t;
            default:
            {
                // added to avoid warnings
            }
        }
    }
    return NULL;
}

ScToken* ScTokenArray::GetNextColRowName()
{
    while( nIndex < nLen )
    {
        ScToken* t = pCode[ nIndex++ ];
        if ( t->GetOpCode() == ocColRowName )
            return t;
    }
    return NULL;
}

ScToken* ScTokenArray::GetNextReferenceRPN()
{
    while( nIndex < nRPN )
    {
        ScToken* t = pRPN[ nIndex++ ];
        switch( t->GetType() )
        {
            case svSingleRef:
            case svDoubleRef:
            case svExternalSingleRef:
            case svExternalDoubleRef:
                return t;
            default:
            {
                // added to avoid warnings
            }
        }
    }
    return NULL;
}

ScToken* ScTokenArray::GetNextReferenceOrName()
{
    for( ScToken* t = Next(); t; t = Next() )
    {
        switch( t->GetType() )
        {
            case svSingleRef:
            case svDoubleRef:
            case svIndex:
            case svExternalSingleRef:
            case svExternalDoubleRef:
            case svExternalName:
                return t;
            default:
            {
                // added to avoid warnings
            }
        }
    }
    return NULL;
}

ScToken* ScTokenArray::GetNextName()
{
    for( ScToken* t = Next(); t; t = Next() )
    {
        if( t->GetType() == svIndex )
            return t;
    }
    return NULL;
}

ScToken* ScTokenArray::GetNextDBArea()
{
    for( ScToken* t = Next(); t; t = Next() )
    {
        if ( t->GetOpCode() == ocDBArea )
            return t;
    }
    return NULL;
}

ScToken* ScTokenArray::GetNextOpCodeRPN( OpCode eOp )
{
    while( nIndex < nRPN )
    {
        ScToken* t = pRPN[ nIndex++ ];
        if ( t->GetOpCode() == eOp )
            return t;
    }
    return NULL;
}

ScToken* ScTokenArray::Next()
{
    if( pCode && nIndex < nLen )
        return pCode[ nIndex++ ];
    else
        return NULL;
}

ScToken* ScTokenArray::NextNoSpaces()
{
    if( pCode )
    {
        while( (nIndex < nLen) && (pCode[ nIndex ]->GetOpCode() == ocSpaces) )
            ++nIndex;
        if( nIndex < nLen )
            return pCode[ nIndex++ ];
    }
    return NULL;
}

ScToken* ScTokenArray::NextRPN()
{
    if( pRPN && nIndex < nRPN )
        return pRPN[ nIndex++ ];
    else
        return NULL;
}

ScToken* ScTokenArray::PrevRPN()
{
    if( pRPN && nIndex )
        return pRPN[ --nIndex ];
    else
        return NULL;
}

void ScTokenArray::DelRPN()
{
    if( nRPN )
    {
        ScToken** p = pRPN;
        for( USHORT i = 0; i < nRPN; i++ )
        {
            (*p++)->DecRef();
        }
        delete [] pRPN;
    }
    pRPN = NULL;
    nRPN = nIndex = 0;
}

ScToken* ScTokenArray::PeekPrev( USHORT & nIdx )
{
    if (0 < nIdx && nIdx <= nLen)
        return pCode[--nIdx];
    return NULL;
}

ScToken* ScTokenArray::PeekNext()
{
    if( pCode && nIndex < nLen )
        return pCode[ nIndex ];
    else
        return NULL;
}

ScToken* ScTokenArray::PeekNextNoSpaces()
{
    if( pCode && nIndex < nLen )
    {
        USHORT j = nIndex;
        while ( pCode[j]->GetOpCode() == ocSpaces && j < nLen )
            j++;
        if ( j < nLen )
            return pCode[ j ];
        else
            return NULL;
    }
    else
        return NULL;
}

ScToken* ScTokenArray::PeekPrevNoSpaces()
{
    if( pCode && nIndex > 1 )
    {
        USHORT j = nIndex - 2;
        while ( pCode[j]->GetOpCode() == ocSpaces && j > 0 )
            j--;
        if ( j > 0 || pCode[j]->GetOpCode() != ocSpaces )
            return pCode[ j ];
        else
            return NULL;
    }
    else
        return NULL;
}

BOOL ScTokenArray::HasOpCode( OpCode eOp ) const
{
    for ( USHORT j=0; j < nLen; j++ )
    {
        if ( pCode[j]->GetOpCode() == eOp )
            return TRUE;
    }
    return FALSE;
}

BOOL ScTokenArray::HasOpCodeRPN( OpCode eOp ) const
{
    for ( USHORT j=0; j < nRPN; j++ )
    {
        if ( pRPN[j]->GetOpCode() == eOp )
            return TRUE;
    }
    return FALSE;
}

//UNUSED2008-05  BOOL ScTokenArray::HasName() const
//UNUSED2008-05  {
//UNUSED2008-05      for ( USHORT j=0; j < nLen; j++ )
//UNUSED2008-05      {
//UNUSED2008-05          if ( pCode[j]->GetType() == svIndex )
//UNUSED2008-05              return TRUE;
//UNUSED2008-05      }
//UNUSED2008-05      return FALSE;
//UNUSED2008-05  }

BOOL ScTokenArray::HasNameOrColRowName() const
{
    for ( USHORT j=0; j < nLen; j++ )
    {
        if( pCode[j]->GetType() == svIndex || pCode[j]->GetOpCode() == ocColRowName )
            return TRUE;
    }
    return FALSE;
}

BOOL ScTokenArray::ImplGetReference( ScRange& rRange, BOOL bValidOnly ) const
{
    BOOL bIs = FALSE;
    if ( pCode && nLen == 1 )
    {
        const ScToken* pToken = pCode[0];
        if ( pToken )
        {
            if ( pToken->GetType() == svSingleRef )
            {
                const ScSingleRefData& rRef = ((const ScSingleRefToken*)pToken)->GetSingleRef();
                rRange.aStart = rRange.aEnd = ScAddress( rRef.nCol, rRef.nRow, rRef.nTab );
                bIs = !bValidOnly || !rRef.IsDeleted();
            }
            else if ( pToken->GetType() == svDoubleRef )
            {
                const ScComplexRefData& rCompl = ((const ScDoubleRefToken*)pToken)->GetDoubleRef();
                const ScSingleRefData& rRef1 = rCompl.Ref1;
                const ScSingleRefData& rRef2 = rCompl.Ref2;
                rRange.aStart = ScAddress( rRef1.nCol, rRef1.nRow, rRef1.nTab );
                rRange.aEnd   = ScAddress( rRef2.nCol, rRef2.nRow, rRef2.nTab );
                bIs = !bValidOnly || (!rRef1.IsDeleted() && !rRef2.IsDeleted());
            }
        }
    }
    return bIs;
}

BOOL ScTokenArray::IsReference( ScRange& rRange ) const
{
    return ImplGetReference( rRange, FALSE );
}

BOOL ScTokenArray::IsValidReference( ScRange& rRange ) const
{
    return ImplGetReference( rRange, TRUE );
}

////////////////////////////////////////////////////////////////////////////

ScTokenArray::ScTokenArray()
{
    pCode = NULL; pRPN = NULL;
    nError = nLen = nIndex = nRPN = nRefs = 0;
    bHyperLink = FALSE;
    ClearRecalcMode();
}

ScTokenArray::ScTokenArray( const ScTokenArray& rArr )
{
    Assign( rArr );
}

ScTokenArray::~ScTokenArray()
{
    Clear();
}

void ScTokenArray::Assign( const ScTokenArray& r )
{
    nLen   = r.nLen;
    nRPN   = r.nRPN;
    nIndex = r.nIndex;
    nError = r.nError;
    nRefs  = r.nRefs;
    nMode  = r.nMode;
    bHyperLink = r.bHyperLink;
    pCode  = NULL;
    pRPN   = NULL;
    ScToken** pp;
    if( nLen )
    {
        pp = pCode = new ScToken*[ nLen ];
        memcpy( pp, r.pCode, nLen * sizeof( ScToken* ) );
        for( USHORT i = 0; i < nLen; i++ )
            (*pp++)->IncRef();
    }
    if( nRPN )
    {
        pp = pRPN = new ScToken*[ nRPN ];
        memcpy( pp, r.pRPN, nRPN * sizeof( ScToken* ) );
        for( USHORT i = 0; i < nRPN; i++ )
            (*pp++)->IncRef();
    }
}

ScTokenArray& ScTokenArray::operator=( const ScTokenArray& rArr )
{
    Clear();
    Assign( rArr );
    return *this;
}

ScTokenArray* ScTokenArray::Clone() const
{
    ScTokenArray* p = new ScTokenArray;
    p->nLen = nLen;
    p->nRPN = nRPN;
    p->nRefs = nRefs;
    p->nMode = nMode;
    p->nError = nError;
    p->bHyperLink = bHyperLink;
    ScToken** pp;
    if( nLen )
    {
        pp = p->pCode = new ScToken*[ nLen ];
        memcpy( pp, pCode, nLen * sizeof( ScToken* ) );
        for( USHORT i = 0; i < nLen; i++, pp++ )
        {
            *pp = (*pp)->Clone();
            (*pp)->IncRef();
        }
    }
    if( nRPN )
    {
        pp = p->pRPN = new ScToken*[ nRPN ];
        memcpy( pp, pRPN, nRPN * sizeof( ScToken* ) );
        for( USHORT i = 0; i < nRPN; i++, pp++ )
        {
            ScToken* t = *pp;
            if( t->GetRef() > 1 )
            {
                ScToken** p2 = pCode;
                USHORT nIdx = 0xFFFF;
                for( USHORT j = 0; j < nLen; j++, p2++ )
                {
                    if( *p2 == t )
                    {
                        nIdx = j; break;
                    }
                }
                if( nIdx == 0xFFFF )
                    *pp = t->Clone();
                else
                    *pp = p->pCode[ nIdx ];
            }
            else
                *pp = t->Clone();
            (*pp)->IncRef();
        }
    }
    return p;
}

void ScTokenArray::Clear()
{
    if( nRPN ) DelRPN();
    if( pCode )
    {
        ScToken** p = pCode;
        for( USHORT i = 0; i < nLen; i++ )
        {
            (*p++)->DecRef();
        }
        delete [] pCode;
    }
    pCode = NULL; pRPN = NULL;
    nError = nLen = nIndex = nRPN = nRefs = 0;
    bHyperLink = FALSE;
    ClearRecalcMode();
}

ScToken* ScTokenArray::AddToken( const ScRawToken& r )
{
    return Add( r.CreateToken() );
}

ScToken* ScTokenArray::AddToken( const ScToken& r )
{
    return Add( r.Clone() );
}

// Utility function to ensure that there is strict alternation of values and
// seperators.
static bool
checkArraySep( bool & bPrevWasSep, bool bNewVal )
{
    bool bResult = (bPrevWasSep == bNewVal);
    bPrevWasSep = bNewVal;
    return bResult;
}

ScToken* ScTokenArray::MergeArray( )
{
    int nCol = -1, nRow = 0;
    int i, nPrevRowSep = -1, nStart = 0;
    bool bPrevWasSep = false; // top of stack is ocArrayClose
    ScToken* t;
    bool bNumeric = false;  // numeric value encountered in current element

    // (1) Iterate from the end to the start to find matrix dims
    // and do basic validation.
    for ( i = nLen ; i-- > nStart ; )
    {
        t = pCode[i];
        switch ( t->GetOpCode() )
        {
            case ocPush :
                if( checkArraySep( bPrevWasSep, false ) )
                {
                    return NULL;
                }

                // no references or nested arrays
                if ( t->GetType() != svDouble  && t->GetType() != svString )
                {
                    return NULL;
                }
                bNumeric = (t->GetType() == svDouble);
            break;

            case ocMissing :
            case ocTrue :
            case ocFalse :
                if( checkArraySep( bPrevWasSep, false ) )
                {
                    return NULL;
                }
                bNumeric = false;
            break;

            case ocArrayColSep :
            case ocSep :
                if( checkArraySep( bPrevWasSep, true ) )
                {
                    return NULL;
                }
                bNumeric = false;
            break;

            case ocArrayClose :
                // not possible with the , but check just in case
                // something changes in the future
                if( i != (nLen-1))
                {
                    return NULL;
                }

                if( checkArraySep( bPrevWasSep, true ) )
                {
                    return NULL;
                }

                nPrevRowSep = i;
                bNumeric = false;
            break;

            case ocArrayOpen :
                nStart = i; // stop iteration
                // fall through to ArrayRowSep

            case ocArrayRowSep :
                if( checkArraySep( bPrevWasSep, true ) )
                {
                    return NULL;
                }

                if( nPrevRowSep < 0 ||              // missing ocArrayClose
                    ((nPrevRowSep - i) % 2) == 1)   // no complex elements
                {
                    return NULL;
                }

                if( nCol < 0 )
                {
                    nCol = (nPrevRowSep - i) / 2;
                }
                else if( (nPrevRowSep - i)/2 != nCol)   // irregular array
                {
                    return NULL;
                }

                nPrevRowSep = i;
                nRow++;
                bNumeric = false;
            break;

            case ocNegSub :
            case ocAdd :
                // negation or unary plus must precede numeric value
                if( !bNumeric )
                {
                    return NULL;
                }
                --nPrevRowSep;      // shorten this row by 1
                bNumeric = false;   // one level only, no --42
            break;

            case ocSpaces :
                // ignore spaces
                --nPrevRowSep;      // shorten this row by 1
            break;

            default :
                // no functions or operators
                return NULL;
        }
    }
    if( nCol <= 0 || nRow <= 0 )
        return NULL;

    // fprintf (stderr, "Array (cols = %d, rows = %d)\n", nCol, nRow );

    int nSign = 1;
    ScMatrix* pArray = new ScMatrix( nCol, nRow );
    for ( i = nStart, nCol = 0, nRow = 0 ; i < nLen ; i++ )
    {
        t = pCode[i];

        switch ( t->GetOpCode() )
        {
            case ocPush :
                if ( t->GetType() == svDouble )
                {
                    pArray->PutDouble( t->GetDouble() * nSign, nCol, nRow );
                    nSign = 1;
                }
                else if ( t->GetType() == svString )
                {
                    pArray->PutString( t->GetString(), nCol, nRow );
                }
            break;

            case ocMissing :
                pArray->PutEmpty( nCol, nRow );
            break;

            case ocTrue :
                pArray->PutBoolean( true, nCol, nRow );
            break;

            case ocFalse :
                pArray->PutBoolean( false, nCol, nRow );
            break;

            case ocArrayColSep :
            case ocSep :
                nCol++;
            break;

            case ocArrayRowSep :
                nRow++; nCol = 0;
            break;

            case ocNegSub :
                nSign = -nSign;
            break;

            default :
                break;
        }
        pCode[i] = NULL;
        t->DecRef();
    }
    nLen = USHORT( nStart );
    return AddMatrix( pArray );
}


ScToken* ScTokenArray::MergeRangeReference( const ScAddress & rPos )
{
    if (!pCode || !nLen)
        return NULL;
    USHORT nIdx = nLen;
    ScToken *p1, *p2, *p3;      // ref, ocRange, ref
    // The actual types are checked in ExtendRangeReference().
    if (((p3 = PeekPrev(nIdx)) != 0) &&
            (((p2 = PeekPrev(nIdx)) != 0) && p2->GetOpCode() == ocRange) &&
            ((p1 = PeekPrev(nIdx)) != 0))
    {
        ScTokenRef p = ScToken::ExtendRangeReference( *p1, *p3, rPos, true);
        if (p)
        {
            p->IncRef();
            p1->DecRef();
            p2->DecRef();
            p3->DecRef();
            nLen -= 2;
            pCode[ nLen-1 ] = p;
            nRefs--;
        }
    }
    return pCode[ nLen-1 ];
}


ScToken* ScTokenArray::Add( ScToken* t )
{
    if( !pCode )
        pCode = new ScToken*[ MAXCODE ];
    if( nLen < MAXCODE-1 )
    {
        // fprintf (stderr, "Add : %d\n", t->GetOpCode());
        pCode[ nLen++ ] = t;
        if( t->GetOpCode() == ocPush
            && ( t->GetType() == svSingleRef || t->GetType() == svDoubleRef ) )
            nRefs++;
        t->IncRef();
        if( t->GetOpCode() == ocArrayClose )
            return MergeArray();
        return t;
    }
    else
    {
        t->Delete();
        if ( nLen == MAXCODE-1 )
        {
            t = new ScByteToken( ocStop );
            pCode[ nLen++ ] = t;
            t->IncRef();
        }
        return NULL;
    }
}

ScToken* ScTokenArray::AddOpCode( OpCode e )
{
    ScRawToken t;
    t.SetOpCode( e );
    return AddToken( t );
}

ScToken* ScTokenArray::AddString( const sal_Unicode* pStr )
{
    return AddString( String( pStr ) );
}

ScToken* ScTokenArray::AddString( const String& rStr )
{
    return Add( new ScStringToken( rStr ) );
}

ScToken* ScTokenArray::AddDouble( double fVal )
{
    return Add( new ScDoubleToken( fVal ) );
}

ScToken* ScTokenArray::AddSingleReference( const ScSingleRefData& rRef )
{
    return Add( new ScSingleRefToken( rRef ) );
}

ScToken* ScTokenArray::AddMatrixSingleReference( const ScSingleRefData& rRef )
{
    return Add( new ScSingleRefOpToken( ocMatRef, rRef ) );
}

ScToken* ScTokenArray::AddDoubleReference( const ScComplexRefData& rRef )
{
    return Add( new ScDoubleRefToken( rRef ) );
}

ScToken* ScTokenArray::AddName( USHORT n )
{
    return Add( new ScIndexToken( ocName, n ) );
}

ScToken* ScTokenArray::AddExternal( const sal_Unicode* pStr )
{
    return AddExternal( String( pStr ) );
}

ScToken* ScTokenArray::AddExternal( const String& rStr,
        OpCode eOp /* = ocExternal */ )
{
    return Add( new ScExternalToken( eOp, rStr ) );
}

ScToken* ScTokenArray::AddMatrix( ScMatrix* p )
{
    return Add( new ScMatrixToken( p ) );
}

ScToken* ScTokenArray::AddExternalName( sal_uInt16 nFileId, const String& rName )
{
    return Add( new ScExternalNameToken(nFileId, rName) );
}

ScToken* ScTokenArray::AddExternalSingleReference( sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef )
{
    return Add( new ScExternalSingleRefToken(nFileId, rTabName, rRef) );
}

ScToken* ScTokenArray::AddExternalDoubleReference( sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef )
{
    return Add( new ScExternalDoubleRefToken(nFileId, rTabName, rRef) );
}

ScToken* ScTokenArray::AddColRowName( const ScSingleRefData& rRef )
{
    return Add( new ScSingleRefOpToken( ocColRowName, rRef ) );
}

ScToken* ScTokenArray::AddBad( const sal_Unicode* pStr )
{
    return AddBad( String( pStr ) );
}

ScToken* ScTokenArray::AddBad( const String& rStr )
{
    return Add( new ScStringOpToken( ocBad, rStr ) );
}


BOOL ScTokenArray::GetAdjacentExtendOfOuterFuncRefs( SCCOLROW& nExtend,
        const ScAddress& rPos, ScDirection eDir )
{
    SCCOL nCol = 0;
    SCROW nRow = 0;
    switch ( eDir )
    {
        case DIR_BOTTOM :
            if ( rPos.Row() < MAXROW )
                nRow = (nExtend = rPos.Row()) + 1;
            else
                return FALSE;
        break;
        case DIR_RIGHT :
            if ( rPos.Col() < MAXCOL )
                nCol = static_cast<SCCOL>(nExtend = rPos.Col()) + 1;
            else
                return FALSE;
        break;
        case DIR_TOP :
            if ( rPos.Row() > 0 )
                nRow = (nExtend = rPos.Row()) - 1;
            else
                return FALSE;
        break;
        case DIR_LEFT :
            if ( rPos.Col() > 0 )
                nCol = static_cast<SCCOL>(nExtend = rPos.Col()) - 1;
            else
                return FALSE;
        break;
        default:
            DBG_ERRORFILE( "unknown Direction" );
            return FALSE;
    }
    if ( pRPN && nRPN )
    {
        ScToken* t = pRPN[nRPN-1];
        if ( t->GetType() == svByte )
        {
            BYTE nParamCount = t->GetByte();
            if ( nParamCount && nRPN > nParamCount )
            {
                BOOL bRet = FALSE;
                USHORT nParam = nRPN - nParamCount - 1;
                for ( ; nParam < nRPN-1; nParam++ )
                {
                    ScToken* p = pRPN[nParam];
                    switch ( p->GetType() )
                    {
                        case svSingleRef :
                        {
                            ScSingleRefData& rRef = p->GetSingleRef();
                            rRef.CalcAbsIfRel( rPos );
                            switch ( eDir )
                            {
                                case DIR_BOTTOM :
                                    if ( rRef.nRow == nRow
                                            && rRef.nRow > nExtend )
                                    {
                                        nExtend = rRef.nRow;
                                        bRet = TRUE;
                                    }
                                break;
                                case DIR_RIGHT :
                                    if ( rRef.nCol == nCol
                                            && static_cast<SCCOLROW>(rRef.nCol)
                                            > nExtend )
                                    {
                                        nExtend = rRef.nCol;
                                        bRet = TRUE;
                                    }
                                break;
                                case DIR_TOP :
                                    if ( rRef.nRow == nRow
                                            && rRef.nRow < nExtend )
                                    {
                                        nExtend = rRef.nRow;
                                        bRet = TRUE;
                                    }
                                break;
                                case DIR_LEFT :
                                    if ( rRef.nCol == nCol
                                            && static_cast<SCCOLROW>(rRef.nCol)
                                            < nExtend )
                                    {
                                        nExtend = rRef.nCol;
                                        bRet = TRUE;
                                    }
                                break;
                            }
                        }
                        break;
                        case svDoubleRef :
                        {
                            ScComplexRefData& rRef = p->GetDoubleRef();
                            rRef.CalcAbsIfRel( rPos );
                            switch ( eDir )
                            {
                                case DIR_BOTTOM :
                                    if ( rRef.Ref1.nRow == nRow
                                            && rRef.Ref2.nRow > nExtend )
                                    {
                                        nExtend = rRef.Ref2.nRow;
                                        bRet = TRUE;
                                    }
                                break;
                                case DIR_RIGHT :
                                    if ( rRef.Ref1.nCol == nCol &&
                                            static_cast<SCCOLROW>(rRef.Ref2.nCol)
                                            > nExtend )
                                    {
                                        nExtend = rRef.Ref2.nCol;
                                        bRet = TRUE;
                                    }
                                break;
                                case DIR_TOP :
                                    if ( rRef.Ref2.nRow == nRow
                                            && rRef.Ref1.nRow < nExtend )
                                    {
                                        nExtend = rRef.Ref1.nRow;
                                        bRet = TRUE;
                                    }
                                break;
                                case DIR_LEFT :
                                    if ( rRef.Ref2.nCol == nCol &&
                                            static_cast<SCCOLROW>(rRef.Ref1.nCol)
                                            < nExtend )
                                    {
                                        nExtend = rRef.Ref1.nCol;
                                        bRet = TRUE;
                                    }
                                break;
                            }
                        }
                        break;
                        default:
                        {
                            // added to avoid warnings
                        }
                    } // switch
                } // for
                return bRet;
            }
        }
    }
    return FALSE;
}


void ScTokenArray::AddRecalcMode( ScRecalcMode nBits )
{
    //! Reihenfolge ist wichtig
    if ( nBits & RECALCMODE_ALWAYS )
        SetRecalcModeAlways();
    else if ( !IsRecalcModeAlways() )
    {
        if ( nBits & RECALCMODE_ONLOAD )
            SetRecalcModeOnLoad();
        else if ( nBits & RECALCMODE_ONLOAD_ONCE && !IsRecalcModeOnLoad() )
            SetRecalcModeOnLoadOnce();
    }
    SetCombinedBitsRecalcMode( nBits );
}


BOOL ScTokenArray::HasMatrixDoubleRefOps()
{
    if ( pRPN && nRPN )
    {
        // RPN-Interpreter Simulation
        // als Ergebnis jeder Funktion wird einfach ein Double angenommen
        ScToken** pStack = new ScToken* [nRPN];
        ScToken* pResult = new ScDoubleToken( 0.0 );
        short sp = 0;
        for ( USHORT j = 0; j < nRPN; j++ )
        {
            ScToken* t = pRPN[j];
            OpCode eOp = t->GetOpCode();
            BYTE nParams = t->GetParamCount();
            switch ( eOp )
            {
                case ocAdd :
                case ocSub :
                case ocMul :
                case ocDiv :
                case ocPow :
                case ocPower :
                case ocAmpersand :
                case ocEqual :
                case ocNotEqual :
                case ocLess :
                case ocGreater :
                case ocLessEqual :
                case ocGreaterEqual :
                {
                    for ( BYTE k = nParams; k; k-- )
                    {
                        if ( sp >= k && pStack[sp-k]->GetType() == svDoubleRef )
                        {
                            pResult->Delete();
                            delete [] pStack;
                            return TRUE;
                        }
                    }
                }
                break;
                default:
                {
                    // added to avoid warnings
                }
            }
            if ( eOp == ocPush || lcl_IsReference( eOp, t->GetType() )  )
                pStack[sp++] = t;
            else if ( eOp == ocIf || eOp == ocChose )
            {   // Jumps ignorieren, vorheriges Result (Condition) poppen
                if ( sp )
                    --sp;
            }
            else
            {   // pop parameters, push result
                sp = sal::static_int_cast<short>( sp - nParams );
                if ( sp < 0 )
                {
                    DBG_ERROR( "ScTokenArray::HasMatrixDoubleRefOps: sp < 0" );
                    sp = 0;
                }
                pStack[sp++] = pResult;
            }
        }
        pResult->Delete();
        delete [] pStack;
    }

    return FALSE;
}


void ScTokenArray::ReadjustRelative3DReferences( const ScAddress& rOldPos,
        const ScAddress& rNewPos )
{
    for ( USHORT j=0; j<nLen; ++j )
    {
        switch ( pCode[j]->GetType() )
        {
            case svDoubleRef :
            {
                ScSingleRefData& rRef2 = pCode[j]->GetSingleRef2();
                // Also adjust if the reference is of the form Sheet1.A2:A3
                if ( rRef2.IsFlag3D() || pCode[j]->GetSingleRef().IsFlag3D() )
                {
                    rRef2.CalcAbsIfRel( rOldPos );
                    rRef2.CalcRelFromAbs( rNewPos );
                }
            }
            //! fallthru
            case svSingleRef :
            {
                ScSingleRefData& rRef1 = pCode[j]->GetSingleRef();
                if ( rRef1.IsFlag3D() )
                {
                    rRef1.CalcAbsIfRel( rOldPos );
                    rRef1.CalcRelFromAbs( rNewPos );
                }
            }
            break;
            default:
            {
                // added to avoid warnings
            }
        }
    }
}


// --- POF (plain old formula) rewrite of a token array ---------------------

/* TODO: When both POF OOoXML and ODFF are to be supported differently, the
 * ScMissingContext and ScTokenArray::*Pof* methods should go to a convention
 * on its own.
 */

#if 0
// static function can't be compiled if not used (warning)
//#if OSL_DEBUG_LEVEL > 0
static void DumpTokArr( ScTokenArray *pCode )
{
    fprintf (stderr, "TokenArr: ");
    for ( ScToken *pCur = pCode->First(); pCur; pCur = pCode->Next() )
        fprintf( stderr, "t%d,o%d ",
                pCur->GetType(), pCur->GetOpCode() );
    fprintf (stderr, "\n");
}
#endif

class ScMissingContext
{
    public:
            const ScToken*  mpFunc;
            int             mnCurArg;

                    void    Clear() { mpFunc = NULL; mnCurArg = 0; }
            inline  bool    AddDefaultArg( ScTokenArray* pNewArr, int nArg, double f ) const;
    static  inline  bool    IsRewriteNeeded( OpCode eOp );
                    bool    AddMissingExternal( ScTokenArray* pNewArr ) const;
                    bool    AddMissing( ScTokenArray *pNewArr ) const;
                    void    AddMoreArgs( ScTokenArray *pNewArr ) const;
};

inline bool ScMissingContext::AddDefaultArg( ScTokenArray* pNewArr, int nArg, double f ) const
{
    if (mnCurArg == nArg)
    {
        pNewArr->AddDouble( f );
        return true;
    }
    return false;
}

inline bool ScMissingContext::IsRewriteNeeded( OpCode eOp )
{
    switch (eOp)
    {
        case ocMissing:
        case ocLog:
        case ocAddress:
            return true;
        default:
            return false;
    }
}

bool ScMissingContext::AddMissingExternal( ScTokenArray *pNewArr ) const
{
    const String &rName = mpFunc->GetExternal();

    // initial (fast) check:
    sal_Unicode nLastChar = rName.GetChar( rName.Len() - 1);
    if ( nLastChar != 't' && nLastChar != 'm' )
        return false;

    if (rName.EqualsIgnoreCaseAscii(
                "com.sun.star.sheet.addin.Analysis.getAccrint" ))
    {
        return AddDefaultArg( pNewArr, 4, 1000.0 );
    }
    if (rName.EqualsIgnoreCaseAscii(
                "com.sun.star.sheet.addin.Analysis.getAccrintm" ))
    {
        return AddDefaultArg( pNewArr, 3, 1000.0 );
    }
    return false;
}

bool ScMissingContext::AddMissing( ScTokenArray *pNewArr ) const
{
    if ( !mpFunc )
        return false;

    bool bRet = false;
    switch ( mpFunc->GetOpCode() )
    {
        case ocFixed:
            return AddDefaultArg( pNewArr, 1, 2.0 );
            //break;
        case ocBetaDist:
        case ocBetaInv:
        case ocRMZ:  // PMT
            return AddDefaultArg( pNewArr, 3, 0.0 );
            //break;
        case ocZinsZ: // IPMT
        case ocKapz:  // PPMT
            return AddDefaultArg( pNewArr, 4, 0.0 );
            //break;
        case ocBW: // PV
        case ocZW: // FV
            bRet |= AddDefaultArg( pNewArr, 2, 0.0 ); // pmt
            bRet |= AddDefaultArg( pNewArr, 3, 0.0 ); // [fp]v
            break;
        case ocZins: // RATE
            bRet |= AddDefaultArg( pNewArr, 1, 0.0 ); // pmt
            bRet |= AddDefaultArg( pNewArr, 3, 0.0 ); // fv
            bRet |= AddDefaultArg( pNewArr, 4, 0.0 ); // type
            break;
        case ocExternal:
            return AddMissingExternal( pNewArr );
            //break;

            // --- more complex cases ---

        case ocOffset:
            // FIXME: rather tough.
            // if arg 3 (height) ommitted, export arg1 (rows)
            break;
        default:
            break;
    }

    return bRet;
}

void ScMissingContext::AddMoreArgs( ScTokenArray *pNewArr ) const
{
    if ( !mpFunc )
        return;

    // Log
    if ( mpFunc->GetOpCode() == ocLog && mnCurArg < 1 )
    {
        pNewArr->AddOpCode( ocSep );
        pNewArr->AddDouble( 10.0 );
    }
}


bool ScTokenArray::NeedsPofRewrite()
{
    for ( ScToken *pCur = First(); pCur; pCur = Next() )
    {
        if (ScMissingContext::IsRewriteNeeded( pCur->GetOpCode()))
            return true;
    }
    return false;
}


ScTokenArray * ScTokenArray::RewriteMissingToPof()
{
    const size_t nAlloc = 256;
    ScMissingContext aCtx[ nAlloc ];
    int aOpCodeAddressStack[ nAlloc ];  // use of ADDRESS() function
    const int nOmitAddressArg = 3;      // ADDRESS() 4th parameter A1/R1C1
    USHORT nTokens = GetLen() + 1;
    ScMissingContext* pCtx = (nAlloc < nTokens ? new ScMissingContext[nTokens] : &aCtx[0]);
    int* pOcas = (nAlloc < nTokens ? new int[nTokens] : &aOpCodeAddressStack[0]);
    // Never go below 0, never use 0, mpFunc always NULL.
    pCtx[0].Clear();
    int nFn = 0;
    int nOcas = 0;

    ScTokenArray *pNewArr = new ScTokenArray;
    // At least RECALCMODE_ALWAYS needs to be set.
    pNewArr->AddRecalcMode( GetRecalcMode());

    for ( ScToken *pCur = First(); pCur; pCur = Next() )
    {
        bool bAdd = true;
        // Don't write the expression of the new inserted ADDRESS() parameter.
        // Do NOT omit the new second parameter of INDIRECT() though. If that
        // was done for both, INDIRECT() actually could calculate different and
        // valid (but wrong) results with the then changed return value of
        // ADDRESS(). Better let it generate an error instead.
        for (int i = nOcas; i-- > 0 && bAdd; )
        {
            if (pCtx[ pOcas[ i ] ].mnCurArg == nOmitAddressArg)
            {
                // Omit erverything except a trailing separator, the leading
                // separator is omitted below. The other way around would leave
                // an extraneous separator if no parameter followed.
                if (!(pOcas[ i ] == nFn && pCur->GetOpCode() == ocSep))
                    bAdd = false;
            }
            //fprintf( stderr, "ocAddress %d arg %d%s\n", (int)i, (int)pCtx[ pOcas[ i ] ].mnCurArg, (bAdd ? "" : " omitted"));
        }
        switch ( pCur->GetOpCode() )
        {
            case ocOpen:
                ++nFn;      // all following operations on _that_ function
                pCtx[ nFn ].mpFunc = PeekPrevNoSpaces();
                pCtx[ nFn ].mnCurArg = 0;
                if (pCtx[ nFn ].mpFunc && pCtx[ nFn ].mpFunc->GetOpCode() == ocAddress)
                    pOcas[ nOcas++ ] = nFn;     // entering ADDRESS()
                break;
            case ocClose:
                pCtx[ nFn ].AddMoreArgs( pNewArr );
                DBG_ASSERT( nFn > 0, "ScTokenArray::RewriteMissingToPof: underflow");
                if (nOcas > 0 && pOcas[ nOcas-1 ] == nFn)
                    --nOcas;                    // leaving ADDRESS()
                if (nFn > 0)
                    --nFn;
                break;
            case ocSep:
                pCtx[ nFn ].mnCurArg++;
                // Omit leading separator of ADDRESS() parameter.
                if (nOcas && pOcas[ nOcas-1 ] == nFn && pCtx[ nFn ].mnCurArg == nOmitAddressArg)
                {
                    bAdd = false;
                    //fprintf( stderr, "ocAddress %d sep %d omitted\n", (int)nOcas-1, nOmitAddressArg);
                }
                break;
            case ocMissing:
                if (bAdd)
                    bAdd = !pCtx[ nFn ].AddMissing( pNewArr );
                break;
            default:
                break;
        }
        if (bAdd)
            pNewArr->AddToken( *pCur );
    }

    if (pOcas != &aOpCodeAddressStack[0])
        delete [] pOcas;
    if (pCtx != &aCtx[0])
        delete [] pCtx;

    return pNewArr;
}

bool ScTokenArray::MayReferenceFollow()
{
    if ( pCode && nLen > 0 )
    {
        // ignore trailing spaces
        USHORT i = nLen - 1;
        while ( i > 0 && pCode[i]->GetOpCode() == SC_OPCODE_SPACES )
        {
            --i;
        }
        if ( i > 0 || pCode[i]->GetOpCode() != SC_OPCODE_SPACES )
        {
            OpCode eOp = pCode[i]->GetOpCode();
            if ( (SC_OPCODE_START_BIN_OP <= eOp && eOp < SC_OPCODE_STOP_BIN_OP ) ||
                 (SC_OPCODE_START_UN_OP <= eOp && eOp < SC_OPCODE_STOP_UN_OP ) ||
                 eOp == SC_OPCODE_OPEN || eOp == SC_OPCODE_SEP )
            {
                return true;
            }
        }
    }
    return false;
}


/*----------------------------------------------------------------------*/

ScTokenIterator::ScTokenIterator( const ScTokenArray& rArr )
{
    pCur = NULL;
    Push( &rArr );
}

ScTokenIterator::~ScTokenIterator()
{
    while( pCur )
        Pop();
}

void ScTokenIterator::Push( const ScTokenArray* pArr )
{
    ImpTokenIterator* p = new ImpTokenIterator;
    p->pArr  = pArr;
    p->nPC   = -1;
    p->nStop = SHRT_MAX;
    p->pNext = pCur;
    pCur     = p;
}

void ScTokenIterator::Pop()
{
    ImpTokenIterator* p = pCur;
    if( p )
    {
        pCur = p->pNext;
        delete p;
    }
}

void ScTokenIterator::Reset()
{
    while( pCur->pNext )
        Pop();
    pCur->nPC = -1;
}

const ScToken* ScTokenIterator::First()
{
    Reset();
    return Next();
}

const ScToken* ScTokenIterator::Next()
{
    const ScToken* t = NULL;
    ++pCur->nPC;
    if( pCur->nPC < pCur->pArr->nRPN && pCur->nPC < pCur->nStop )
    {
        t = pCur->pArr->pRPN[ pCur->nPC ];
        // such an OpCode ends an IF() or CHOOSE() path
        if( t->GetOpCode() == ocSep || t->GetOpCode() == ocClose )
            t = NULL;
    }
    if( !t && pCur->pNext )
    {
        Pop();
        t = Next();
    }
    return t;
}

//! The nPC counts after a Push() are -1

void ScTokenIterator::Jump( short nStart, short nNext, short nStop )
{
    pCur->nPC = nNext;
    if( nStart != nNext )
    {
        Push( pCur->pArr );
        pCur->nPC = nStart;
        pCur->nStop = nStop;
    }
}

bool ScTokenIterator::IsEndOfPath() const
{
    USHORT nTest = pCur->nPC + 1;
    if( nTest < pCur->pArr->nRPN  && nTest < pCur->nStop )
    {
        const ScToken* t = pCur->pArr->pRPN[ nTest ];
        // such an OpCode ends an IF() or CHOOSE() path
        return t->GetOpCode() == ocSep || t->GetOpCode() == ocClose;
    }
    return true;
}

