/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: tokenarray.hxx,v $
 * $Revision: 1.12.32.2 $
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

#ifndef SC_TOKENARRAY_HXX
#define SC_TOKENARRAY_HXX

#include "token.hxx"
#include <tools/solar.h>
#include "scdllapi.h"

// RecalcMode access only via TokenArray SetRecalcMode / IsRecalcMode...

typedef BYTE ScRecalcMode;
// Only one of the exclusive bits can be set,
// handled by TokenArray SetRecalcMode... methods
#define RECALCMODE_NORMAL       0x01    // exclusive
#define RECALCMODE_ALWAYS       0x02    // exclusive, always
#define RECALCMODE_ONLOAD       0x04    // exclusive, always after load
#define RECALCMODE_ONLOAD_ONCE  0x08    // exclusive, once after load
#define RECALCMODE_FORCED       0x10    // combined, also if cell isn't visible
#define RECALCMODE_ONREFMOVE    0x20    // combined, if reference was moved
#define RECALCMODE_EMASK        0x0F    // mask of exclusive bits
// If new bits are to be defined, AddRecalcMode has to be adjusted!

struct ScRawToken;

class SC_DLLPUBLIC ScTokenArray
{
    friend class ScCompiler;
    friend class ScTokenIterator;

    ScToken**       pCode;                  // Token code array
    ScToken**       pRPN;                   // RPN array
    USHORT          nLen;                   // Length of token array
    USHORT          nRPN;                   // Length of RPN array
    USHORT          nIndex;                 // Current step index
    USHORT          nError;                 // Error code
    short           nRefs;                  // Count of cell references
    ScRecalcMode    nMode;                  // Flags to indicate when to recalc this code
    BOOL            bHyperLink; // If HYPERLINK() occurs in the formula.

    void                    Assign( const ScTokenArray& );

    /// Also used by the compiler. The token MUST had been allocated with new!
    ScToken*                Add( ScToken* );
    inline  void            SetCombinedBitsRecalcMode( ScRecalcMode nBits )
                            { nMode |= (nBits & ~RECALCMODE_EMASK); }
    inline  ScRecalcMode    GetCombinedBitsRecalcMode() const
                            { return nMode & ~RECALCMODE_EMASK; }
                            /** Exclusive bits already set in nMode are
                                zero'ed, nVal may contain combined bits, but
                                only one exclusive bit may be set! */
    inline  void            SetMaskedRecalcMode( ScRecalcMode nBits )
                            { nMode = GetCombinedBitsRecalcMode() | nBits; }

    BOOL                    ImplGetReference( ScRange& rRange, BOOL bValidOnly ) const;

public:
    ScTokenArray();
    /// Assignment with references to ScToken entries (not copied!)
    ScTokenArray( const ScTokenArray& );
   ~ScTokenArray();
    ScTokenArray* Clone() const;    /// True copy!
    void Clear();
    void DelRPN();
    ScToken* First() { nIndex = 0; return Next(); }
    ScToken* Next();
    ScToken* FirstNoSpaces() { nIndex = 0; return NextNoSpaces(); }
    ScToken* NextNoSpaces();
    ScToken* GetNextName();
    ScToken* GetNextDBArea();
    ScToken* GetNextReference();
    ScToken* GetNextReferenceRPN();
    ScToken* GetNextReferenceOrName();
    ScToken* GetNextColRowName();
    ScToken* GetNextOpCodeRPN( OpCode );
    /// Peek at nIdx-1 if not out of bounds, decrements nIdx if successful. Returns NULL if not.
    ScToken* PeekPrev( USHORT & nIdx );
    ScToken* PeekNext();
    ScToken* PeekPrevNoSpaces();    /// Only after Reset/First/Next/Last/Prev!
    ScToken* PeekNextNoSpaces();    /// Only after Reset/First/Next/Last/Prev!
    ScToken* FirstRPN() { nIndex = 0; return NextRPN(); }
    ScToken* NextRPN();
    ScToken* LastRPN() { nIndex = nRPN; return PrevRPN(); }
    ScToken* PrevRPN();

    BOOL    HasOpCode( OpCode ) const;
    BOOL    HasOpCodeRPN( OpCode ) const;
//UNUSED2008-05  /// Token of type svIndex
//UNUSED2008-05  BOOL    HasName() const;

    /// Token of type svIndex or opcode ocColRowName
    BOOL    HasNameOrColRowName() const;
    /// Exactly and only one range (valid or deleted)
    BOOL    IsReference( ScRange& rRange ) const;
    /// Exactly and only one valid range (no #REF!s)
    BOOL    IsValidReference( ScRange& rRange ) const;

    ScToken** GetArray() const  { return pCode; }
    ScToken** GetCode()  const  { return pRPN; }
    USHORT    GetLen() const     { return nLen; }
    USHORT    GetCodeLen() const { return nRPN; }
    void      Reset()            { nIndex = 0; }
    USHORT    GetCodeError() const      { return nError; }
    void      SetCodeError( USHORT n )  { nError = n; }
    short     GetRefs()  const { return nRefs;  }
    void      SetHyperLink( BOOL bVal ) { bHyperLink = bVal; }
    BOOL      IsHyperLink() const       { return bHyperLink; }

    inline  ScRecalcMode    GetRecalcMode() const { return nMode; }
                            /** Bits aren't set directly but validated and
                                maybe handled according to priority if more
                                than one exclusive bit was set. */
            void            AddRecalcMode( ScRecalcMode nBits );

    inline  void            ClearRecalcMode() { nMode = RECALCMODE_NORMAL; }
    inline  void            SetRecalcModeNormal()
                                { SetMaskedRecalcMode( RECALCMODE_NORMAL ); }
    inline  void            SetRecalcModeAlways()
                                { SetMaskedRecalcMode( RECALCMODE_ALWAYS ); }
    inline  void            SetRecalcModeOnLoad()
                                { SetMaskedRecalcMode( RECALCMODE_ONLOAD ); }
    inline  void            SetRecalcModeOnLoadOnce()
                                { SetMaskedRecalcMode( RECALCMODE_ONLOAD_ONCE ); }
    inline  void            SetRecalcModeForced()
                                { nMode |= RECALCMODE_FORCED; }
    inline  void            ClearRecalcModeForced()
                                { nMode &= ~RECALCMODE_FORCED; }
    inline  void            SetRecalcModeOnRefMove()
                                { nMode |= RECALCMODE_ONREFMOVE; }
    inline  void            ClearRecalcModeOnRefMove()
                                { nMode &= ~RECALCMODE_ONREFMOVE; }
    inline  BOOL            IsRecalcModeNormal() const
                                { return (nMode & RECALCMODE_NORMAL) != 0; }
    inline  BOOL            IsRecalcModeAlways() const
                                { return (nMode & RECALCMODE_ALWAYS) != 0; }
    inline  BOOL            IsRecalcModeOnLoad() const
                                { return (nMode & RECALCMODE_ONLOAD) != 0; }
    inline  BOOL            IsRecalcModeOnLoadOnce() const
                                { return (nMode & RECALCMODE_ONLOAD_ONCE) != 0; }
    inline  BOOL            IsRecalcModeForced() const
                                { return (nMode & RECALCMODE_FORCED) != 0; }
    inline  BOOL            IsRecalcModeOnRefMove() const
                                { return (nMode & RECALCMODE_ONREFMOVE) != 0; }

                            /** Get OpCode of the most outer function */
    inline OpCode           GetOuterFuncOpCode();
                            /** Determines the extent of direct adjacent
                                references. Only use with real functions, e.g.
                                GetOuterFuncOpCode() == ocSum ! */
    BOOL                    GetAdjacentExtendOfOuterFuncRefs( SCCOLROW& nExtend,
                                const ScAddress& rPos, ScDirection );

                            /** Operators +,-,*,/,^,&,=,<>,<,>,<=,>=
                                with DoubleRef in Formula? */
    BOOL                    HasMatrixDoubleRefOps();

    ScToken* AddToken( const ScRawToken& );
    ScToken* AddToken( const ScToken& );
    ScToken* AddOpCode( OpCode eCode );
    ScToken* AddString( const sal_Unicode* pStr );
    ScToken* AddString( const String& rStr );
    ScToken* AddDouble( double fVal );
    /** ScSingleRefToken with ocPush. */
    ScToken* AddSingleReference( const ScSingleRefData& rRef );
    /** ScSingleRefOpToken with ocMatRef. */
    ScToken* AddMatrixSingleReference( const ScSingleRefData& rRef );
    ScToken* AddDoubleReference( const ScComplexRefData& rRef );
    ScToken* AddName( USHORT n );
    ScToken* AddMatrix( ScMatrix* p );
    ScToken* AddExternalName( sal_uInt16 nFileId, const String& rName );
    ScToken* AddExternalSingleReference( sal_uInt16 nFileId, const String& rTabName, const SingleRefData& rRef );
    ScToken* AddExternalDoubleReference( sal_uInt16 nFileId, const String& rTabName, const ComplRefData& rRef );
    ScToken* AddExternal( const sal_Unicode* pStr );
    /** Xcl import may play dirty tricks with OpCode!=ocExternal.
        Others don't use! */
    ScToken* AddExternal( const String& rStr, OpCode eOp = ocExternal );
    /** ScSingleRefOpToken with ocColRowName. */
    ScToken* AddColRowName( const ScSingleRefData& rRef );
    ScToken* AddBad( const sal_Unicode* pStr );     /// ocBad with String
    ScToken* AddBad( const String& rStr );          /// ocBad with String
    ScToken* MergeArray( );
    /** Merge very last SingleRef+ocRange+SingleRef combination into DoubleRef
        and adjust pCode array, or do nothing if conditions not met.
        Unconditionally returns last token from the resulting pCode array, or
        NULL if there is no pCode (which actually would be caller's fault). */
    ScToken* MergeRangeReference( const ScAddress & rPos );

    /// Assignment with references to ScToken entries (not copied!)
    ScTokenArray& operator=( const ScTokenArray& );

    /// Make 3D references point to old referenced position even if relative
            void            ReadjustRelative3DReferences(
                                const ScAddress& rOldPos,
                                const ScAddress& rNewPos );

    /** Determines if this formula needs any changes to convert it to something
        previous versions of OOo could consume (Plain Old Formula). */
            bool            NeedsPofRewrite();

    /** Rewrites to Plain Old Formula, substituting missing parameters. The
        ScTokenArray* returned is new'ed. */
            ScTokenArray*   RewriteMissingToPof();

    /** Determines if this formula may be followed by a reference. */
    bool                    MayReferenceFollow();
};

inline OpCode ScTokenArray::GetOuterFuncOpCode()
{
    if ( pRPN && nRPN )
        return pRPN[nRPN-1]->GetOpCode();
    return ocNone;
}

struct ImpTokenIterator
{
    ImpTokenIterator* pNext;
    const ScTokenArray* pArr;
    short nPC;
    short nStop;

    DECL_FIXEDMEMPOOL_NEWDEL( ImpTokenIterator );
};

class ScTokenIterator
{
    friend class ScInterpreter;     // for Jump()
    ImpTokenIterator* pCur;
    /** Jump or subroutine call.
        Program counter values will be incremented before code is executed =>
        positions are to be passed with -1 offset.
        @param nStart
            Start on code at position nStart+1 (yes, pass with offset -1)
        @param nNext
            After subroutine continue with instruction at position nNext+1
        @param nStop
            Stop before reaching code at position nStop. If not specified the
            default is to either run the entire code, or to stop if an ocSep or
            ocClose is encountered, which are only present in ocIf or ocChose
            jumps.
      */
    void Jump( short nStart, short nNext, short nStop = SHRT_MAX );
    void Push( const ScTokenArray* );
    void Pop();
public:
    ScTokenIterator( const ScTokenArray& );
   ~ScTokenIterator();
    void    Reset();
    const   ScToken* First();
    const   ScToken* Next();
    bool    IsEndOfPath() const;    /// if a jump or subroutine path is done
    bool    HasStacked() const { return pCur->pNext != 0; }
    short   GetPC() const { return pCur->nPC; }
};


#endif // SC_TOKENARRAY_HXX

