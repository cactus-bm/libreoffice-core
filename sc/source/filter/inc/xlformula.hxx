/*************************************************************************
 *
 *  $RCSfile: xlformula.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-09 15:09:01 $
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

#ifndef SC_XLFORMULA_HXX
#define SC_XLFORMULA_HXX

#include <map>

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif
#ifndef SC_OPCODE_HXX
#include "opcode.hxx"
#endif

#ifndef SC_FTOOLS_HXX
#include "ftools.hxx"
#endif

// Constants ==================================================================

const size_t EXC_TOKARR_MAXLEN              = 4096;     /// Maximum size of a token array.

// Token class flags ----------------------------------------------------------

const sal_uInt8 EXC_TOKCLASS_MASK           = 0x60;
const sal_uInt8 EXC_TOKCLASS_INOP_FLAG      = 0x80;     /// Used in operators (internal flag).

const sal_uInt8 EXC_TOKCLASS_NONE           = 0x00;     /// 00-1F: Base tokens.
const sal_uInt8 EXC_TOKCLASS_REF            = 0x20;     /// 20-3F: Reference class tokens.
const sal_uInt8 EXC_TOKCLASS_VAL            = 0x40;     /// 40-5F: Value class tokens.
const sal_uInt8 EXC_TOKCLASS_ARR            = 0x60;     /// 60-7F: Array class tokens.

const sal_uInt8 EXC_TOKCLASS_ANY_IN_REFOP   = EXC_TOKCLASS_INOP_FLAG | EXC_TOKCLASS_NONE;
const sal_uInt8 EXC_TOKCLASS_REF_IN_VALOP   = EXC_TOKCLASS_INOP_FLAG | EXC_TOKCLASS_REF;
const sal_uInt8 EXC_TOKCLASS_VAL_IN_VALOP   = EXC_TOKCLASS_INOP_FLAG | EXC_TOKCLASS_VAL;
const sal_uInt8 EXC_TOKCLASS_ARR_IN_VALOP   = EXC_TOKCLASS_INOP_FLAG | EXC_TOKCLASS_ARR;

// Base tokens ----------------------------------------------------------------

const sal_uInt8 EXC_TOKID_MASK              = 0x1F;

const sal_uInt8 EXC_TOKID_NONE              = 0x00;     /// Placeholder for invalid token id.
const sal_uInt8 EXC_TOKID_EXP               = 0x01;     /// Array or shared formula reference.
const sal_uInt8 EXC_TOKID_TBL               = 0x02;     /// Multiple operation reference.
const sal_uInt8 EXC_TOKID_ADD               = 0x03;     /// Addition operator.
const sal_uInt8 EXC_TOKID_SUB               = 0x04;     /// Subtraction operator.
const sal_uInt8 EXC_TOKID_MUL               = 0x05;     /// Multiplication operator.
const sal_uInt8 EXC_TOKID_DIV               = 0x06;     /// Division operator.
const sal_uInt8 EXC_TOKID_POWER             = 0x07;     /// Power operator.
const sal_uInt8 EXC_TOKID_CONCAT            = 0x08;     /// String concatenation operator.
const sal_uInt8 EXC_TOKID_LT                = 0x09;     /// Less than operator.
const sal_uInt8 EXC_TOKID_LE                = 0x0A;     /// Less than or equal operator.
const sal_uInt8 EXC_TOKID_EQ                = 0x0B;     /// Equal operator.
const sal_uInt8 EXC_TOKID_GE                = 0x0C;     /// Greater than or equal operator.
const sal_uInt8 EXC_TOKID_GT                = 0x0D;     /// Greater than operator.
const sal_uInt8 EXC_TOKID_NE                = 0x0E;     /// Not equal operator.
const sal_uInt8 EXC_TOKID_ISECT             = 0x0F;     /// Intersection operator.
const sal_uInt8 EXC_TOKID_LIST              = 0x10;     /// List operator.
const sal_uInt8 EXC_TOKID_RANGE             = 0x11;     /// Range operator.
const sal_uInt8 EXC_TOKID_UPLUS             = 0x12;     /// Unary plus.
const sal_uInt8 EXC_TOKID_UMINUS            = 0x13;     /// Unary minus.
const sal_uInt8 EXC_TOKID_PERCENT           = 0x14;     /// Percent sign.
const sal_uInt8 EXC_TOKID_PAREN             = 0x15;     /// Parentheses.
const sal_uInt8 EXC_TOKID_MISSARG           = 0x16;     /// Missing argument.
const sal_uInt8 EXC_TOKID_STR               = 0x17;     /// String constant.
const sal_uInt8 EXC_TOKID_EXTENDED          = 0x18;     /// Natural language reference (NLR).
const sal_uInt8 EXC_TOKID_ATTR              = 0x19;     /// Special attribute.
const sal_uInt8 EXC_TOKID_SHEET             = 0x1A;     /// Start of a sheet reference (BIFF2-BIFF4).
const sal_uInt8 EXC_TOKID_ENDSHEET          = 0x1B;     /// End of a sheet reference (BIFF2-BIFF4).
const sal_uInt8 EXC_TOKID_ERR               = 0x1C;     /// Error constant.
const sal_uInt8 EXC_TOKID_BOOL              = 0x1D;     /// Boolean constant.
const sal_uInt8 EXC_TOKID_INT               = 0x1E;     /// Integer constant.
const sal_uInt8 EXC_TOKID_NUM               = 0x1F;     /// Floating-point constant.

// Base IDs of classified tokens ----------------------------------------------

const sal_uInt8 EXC_TOKID_ARRAY             = 0x00;     /// Array constant.
const sal_uInt8 EXC_TOKID_FUNC              = 0x01;     /// Function, fixed number of arguments.
const sal_uInt8 EXC_TOKID_FUNCVAR           = 0x02;     /// Function, variable number of arguments.
const sal_uInt8 EXC_TOKID_NAME              = 0x03;     /// Defined name.
const sal_uInt8 EXC_TOKID_REF               = 0x04;     /// 2D cell reference.
const sal_uInt8 EXC_TOKID_AREA              = 0x05;     /// 2D area reference.
const sal_uInt8 EXC_TOKID_MEMAREA           = 0x06;     /// Constant reference subexpression.
const sal_uInt8 EXC_TOKID_MEMERR            = 0x07;     /// Deleted reference subexpression.
const sal_uInt8 EXC_TOKID_MEMNOMEM          = 0x08;     /// Constant reference subexpression without result.
const sal_uInt8 EXC_TOKID_MEMFUNC           = 0x09;     /// Variable reference subexpression.
const sal_uInt8 EXC_TOKID_REFERR            = 0x0A;     /// Deleted 2D cell reference.
const sal_uInt8 EXC_TOKID_AREAERR           = 0x0B;     /// Deleted 2D area reference.
const sal_uInt8 EXC_TOKID_REFN              = 0x0C;     /// Relative 2D cell reference (in names).
const sal_uInt8 EXC_TOKID_AREAN             = 0x0D;     /// Relative 2D area reference (in names).
const sal_uInt8 EXC_TOKID_MEMAREAN          = 0x0E;     /// Reference subexpression (in names).
const sal_uInt8 EXC_TOKID_MEMNOMEMN         = 0x0F;     /// Reference subexpression (in names) without result.
const sal_uInt8 EXC_TOKID_FUNCCE            = 0x18;
const sal_uInt8 EXC_TOKID_NAMEX             = 0x19;     /// External reference.
const sal_uInt8 EXC_TOKID_REF3D             = 0x1A;     /// 3D cell reference.
const sal_uInt8 EXC_TOKID_AREA3D            = 0x1B;     /// 3D area reference.
const sal_uInt8 EXC_TOKID_REFERR3D          = 0x1C;     /// Deleted 3D cell reference.
const sal_uInt8 EXC_TOKID_AREAERR3D         = 0x1D;     /// Deleted 3D area reference

// specific token constants ---------------------------------------------------

const sal_uInt16 EXC_TOK_STR_MAXLEN         = 255;      /// Maximum string length of a tStr token.

const sal_uInt8 EXC_TOK_BOOL_FALSE          = 0;        /// FALSE value of a tBool token.
const sal_uInt8 EXC_TOK_BOOL_TRUE           = 1;        /// TRUE value of a tBool token.

const sal_uInt8 EXC_TOK_ATTR_VOLATILE       = 0x01;     /// Volatile function.
const sal_uInt8 EXC_TOK_ATTR_IF             = 0x02;     /// Start of true condition in IF function.
const sal_uInt8 EXC_TOK_ATTR_CHOOSE         = 0x04;     /// Jump array of CHOOSE function.
const sal_uInt8 EXC_TOK_ATTR_GOTO           = 0x08;     /// Jump to token.
const sal_uInt8 EXC_TOK_ATTR_SUM            = 0x10;     /// SUM function with one parameter.
const sal_uInt8 EXC_TOK_ATTR_ASSIGN         = 0x20;     /// BASIC style assignment.
const sal_uInt8 EXC_TOK_ATTR_SPACE          = 0x40;     /// Spaces in formula representation.

const sal_uInt8 EXC_TOK_ATTR_SPACE_SP       = 0x00;     /// Spaces before next token.
const sal_uInt8 EXC_TOK_ATTR_SPACE_BR       = 0x01;     /// Line breaks before next token.
const sal_uInt8 EXC_TOK_ATTR_SPACE_SP_OPEN  = 0x02;     /// Spaces before opening parenthesis.
const sal_uInt8 EXC_TOK_ATTR_SPACE_BR_OPEN  = 0x03;     /// Line breaks before opening parenthesis.
const sal_uInt8 EXC_TOK_ATTR_SPACE_SP_CLOSE = 0x04;     /// Spaces before closing parenthesis.
const sal_uInt8 EXC_TOK_ATTR_SPACE_BR_CLOSE = 0x05;     /// Line breaks before closing parenthesis.
const sal_uInt8 EXC_TOK_ATTR_SPACE_SP_PRE   = 0x06;     /// Spaces before formula (BIFF3).

const sal_uInt16 EXC_TOK_REF_COLREL         = 0x4000;   /// True = Column is relative.
const sal_uInt16 EXC_TOK_REF_ROWREL         = 0x8000;   /// True = Row is relative.

const sal_uInt8 EXC_TOK_EXT_ERR             = 0x01;     /// NLR: Invalid/deleted.
const sal_uInt8 EXC_TOK_EXT_ROW             = 0x02;     /// NLR: Row index.
const sal_uInt8 EXC_TOK_EXT_COL             = 0x03;     /// NLR: Column index.
const sal_uInt8 EXC_TOK_EXT_ROWV            = 0x06;     /// NLR: Value in row.
const sal_uInt8 EXC_TOK_EXT_COLV            = 0x07;     /// NLR: Value in column.
const sal_uInt8 EXC_TOK_EXT_RANGE           = 0x0A;     /// NLR: Range.
const sal_uInt8 EXC_TOK_EXT_SRANGE          = 0x0B;     /// Stacked NLR: Range.
const sal_uInt8 EXC_TOK_EXT_SROW            = 0x0C;     /// Stacked NLR: Row index.
const sal_uInt8 EXC_TOK_EXT_SCOL            = 0x0D;     /// Stacked NLR: Column index.
const sal_uInt8 EXC_TOK_EXT_SROWV           = 0x0E;     /// Stacked NLR: Value in row.
const sal_uInt8 EXC_TOK_EXT_SCOLV           = 0x0F;     /// Stacked NLR: Value in column.
const sal_uInt8 EXC_TOK_EXT_RANGEERR        = 0x10;     /// NLR: Invalid/deleted range.
const sal_uInt16 EXC_TOK_EXT_NATREL         = 0x8000;   /// True = Natural language ref is relative.

// Function data ==============================================================

const sal_uInt8 EXC_FUNC_MAXPARAM           = 30;       /// Maximum parameter count.

const sal_uInt8 EXC_FUNC_PAR_CALCONLY       = 0xFD;     /// Placeholder for a parameter existing in Calc, but not in Excel.
const sal_uInt8 EXC_FUNC_PAR_EXCELONLY      = 0xFE;     /// Placeholder for a parameter existing in Excel, but not in Calc.
const sal_uInt8 EXC_FUNC_PAR_INVALID        = 0xFF;     /// Placeholder for an invalid token class.

const sal_uInt8 EXC_FUNCINFO_CLASSCOUNT     = 5;        /// Number of token class entries.

// selected function IDs
const sal_uInt16 EXC_FUNCID_IF              = 1;
const sal_uInt16 EXC_FUNCID_SUM             = 4;
const sal_uInt16 EXC_FUNCID_AND             = 36;
const sal_uInt16 EXC_FUNCID_OR              = 37;
const sal_uInt16 EXC_FUNCID_CHOOSE          = 100;

/** Represents information for a spreadsheet function for import and export.

    The member mpnParamClass contains an array of token classes for each
    parameter of the function. The last existing (non-null) value in this array
    is used for all following parameters used in a function. Additionally to
    the three actual token classes, this array may contain the special values
    EXC_FUNC_PAR_CALCONLY, EXC_FUNC_PAR_EXCELONLY, and EXC_FUNC_PAR_INVALID.
    The former two specify parameters only existing in one of the applications.
    EXC_FUNC_PAR_INVALID is simply a terminator for the array to prevent that
    the last token class or special value is repeated for additional parameters.
 */
struct XclFunctionInfo
{
    OpCode              meOpCode;           /// Calc function opcode.
    sal_uInt16          mnXclFunc;          /// Excel function index.
    sal_uInt8           mnMinParamCount;    /// Minimum number of parameters.
    sal_uInt8           mnMaxParamCount;    /// Maximum number of parameters.
    sal_uInt8           mnRetClass;         /// Token class of the return value.
    sal_uInt8           mpnParamClass[ EXC_FUNCINFO_CLASSCOUNT ]; /// Expected token classes of parameters.
    bool                mbVolatile;         /// True = Result is volatile (i.e. NOW() function).
};

// ----------------------------------------------------------------------------

class XclRoot;

/** Provides access to function info structs for all available functions. */
class XclFunctionProvider
{
public:
    explicit            XclFunctionProvider( const XclRoot& rRoot );

    /** Returns the function data for an Excel function index, or 0 on error. */
    const XclFunctionInfo* GetFuncInfoFromXclFunc( sal_uInt16 nXclFunc ) const;
    /** Returns the function data for a Calc opcode, or 0 on error. */
    const XclFunctionInfo* GetFuncInfoFromOpCode( OpCode eOpCode ) const;

private:
    void                FillXclFuncMap( const XclFunctionInfo* pBeg, const XclFunctionInfo* pEnd );
    void                FillScFuncMap( const XclFunctionInfo* pBeg, const XclFunctionInfo* pEnd );

private:
    typedef ::std::map< sal_uInt16, const XclFunctionInfo* >    XclFuncMap;
    typedef ::std::map< OpCode, const XclFunctionInfo* >        ScFuncMap;

    XclFuncMap          maXclFuncMap;       /// Maps Excel function indexes to function data.
    ScFuncMap           maScFuncMap;        /// Maps Calc opcodes to function data.
};

// Token array ================================================================

class ScToken;
class ScTokenArray;

/** Special token array iterator for the Excel filters.

    Iterates over a Calc token array without modifying it (therefore the
    iterator can be used with constant token arrays).

    Usage: Construct a new iterator object and pass a Calc token array, or use
    the Init() function to assign another Calc token array. As long as the Is()
    function returns true, the accessor functions can be used to get the
    current Calc token.
 */
class XclTokenArrayIterator
{
public:
    explicit            XclTokenArrayIterator();
    explicit            XclTokenArrayIterator( const ScTokenArray& rScTokArr, bool bSkipSpaces );

    void                Init();
    void                Init( const ScTokenArray& rScTokArr, bool bSkipSpaces );

    inline bool         Is() const { return mppScToken != 0; }
    inline bool         operator!() const { return !Is(); }
    inline const ScToken* Get() const { return mppScToken ? *mppScToken : 0; }
    inline const ScToken* operator->() const { return Get(); }
    inline const ScToken& operator*() const { return *Get(); }

    XclTokenArrayIterator& operator++();

private:
    void                NextRawToken();
    void                SkipSpaces();

private:
    const ScToken*const* mppScTokenBeg;     /// Pointer to first token pointer of token array.
    const ScToken*const* mppScTokenEnd;     /// Pointer behind last token pointer of token array.
    const ScToken*const* mppScToken;        /// Pointer to current token pointer of token array.
    bool                mbSkipSpaces;       /// true = Skip whitespace tokens.
};

// ----------------------------------------------------------------------------

/** Contains all cell references that can be extracted from a multiple operations formula. */
struct XclMultipleOpRefs
{
    ScAddress           maFmlaScPos;        /// Position of the (first) formula cell.
    ScAddress           maColFirstScPos;
    ScAddress           maColRelScPos;
    ScAddress           maRowFirstScPos;
    ScAddress           maRowRelScPos;
    bool                mbDblRefMode;       /// true = One formula with row and column values.
};

// ----------------------------------------------------------------------------

/** A helper with Excel specific token array functions.

    The purpose to not add these functions to ScTokenArray is to prevent code
    changes in low-level Calc headers and to keep the Excel specific source
    code in the filter directory. Deriving from ScTokenArray is not viable
    because that would need expensive copy-constructions of the token arrays.
 */
class XclTokenArrayHelper
{
public:
    // token identifiers ------------------------------------------------------

    /** Returns the base token ID of the passed (classified) token ID. */
    inline static sal_uInt8 GetBaseTokenId( sal_uInt8 nTokenId ) { return nTokenId & EXC_TOKID_MASK; }
    /** Returns the classified token ID from a base ID and the token class. */
    inline static sal_uInt8 GetTokenId( sal_uInt8 nBaseId, sal_uInt8 nTokenClass );

    /** Returns the token class of the passed token ID. */
    inline static sal_uInt8 GetTokenClass( sal_uInt8 nTokenId ) { return nTokenId & EXC_TOKCLASS_MASK; }
    /** Changes the token class in the passed classified token ID. */
    inline static void ChangeTokenClass( sal_uInt8& rnTokenId, sal_uInt8 nTokenClass );

    // strings and string lists -----------------------------------------------

    /** Tries to extract a string from the passed token.
        @param rString  (out-parameter) The string contained in the token.
        @return  true = Passed token is a string token, rString parameter is valid. */
    static bool         GetTokenString( String& rString, const ScToken& rScToken );

    /** Parses the passed formula and tries to find a single string token, i.e. "abc".
        @param rString  (out-parameter) The string contained in the formula.
        @return  true = String token found, rString parameter is valid. */
    static bool         GetString( String& rString, const ScTokenArray& rScTokArr );

    /** Parses the passed formula and tries to find a string token list, i.e. "abc";"def";"ghi".
        @descr  Returns the unquoted (!) strings in a single string, separated with the
        passed character. If a comma is specified, the function will return abc,def,ghi from
        the example above.
        @param rStringList  (out-parameter) All strings contained in the formula as list.
        @param cSep  List separator character.
        @return  true = String token list found, rString parameter is valid. */
    static bool         GetStringList( String& rStringList, const ScTokenArray& rScTokArr, sal_Unicode cSep );

    /** Tries to convert a formula that consists of a single string token to a list of strings.
        @descr  Example: The formula ="abc\ndef\nghi" will be converted to the formula
        ="abc";"def";"ghi", if the LF character is specified as separator.
        @param rScTokArr  (in/out-parameter) The token array to modify.
        @param cStringSep  The separator in the source string. */
    static void         ConvertStringToList( ScTokenArray& rScTokArr, sal_Unicode cStringSep );

    // shared formulas --------------------------------------------------------

    /** Tries to extract the definition of a shared formula from the passed token array.
        @descr  Shared formulas are stored as hidden defined names in Calc. This
        function looks if the passed token array consists of the reference to
        such a hidden defined name and returns its definition on success. */
    static const ScTokenArray* GetSharedFormula( const XclRoot& rRoot, const ScTokenArray& rScTokArr );

    // multiple operations ----------------------------------------------------

    /** Parses the passed formula and tries to extract references of a multiple operation.
        @descr  Requires that the formula contains a single MULTIPLE.OPERATION function call.
        Spaces in the formula are silently ignored.
        @return  true = Multiple operation found, and all references successfully extracted. */
    static bool         GetMultipleOpRefs( XclMultipleOpRefs& rRefs, const ScTokenArray& rScTokArr );
};

// ----------------------------------------------------------------------------

inline sal_uInt8 XclTokenArrayHelper::GetTokenId( sal_uInt8 nBaseId, sal_uInt8 nTokenClass )
{
    DBG_ASSERT( !::get_flag< sal_uInt8 >( nBaseId, ~EXC_TOKID_MASK ), "XclTokenArrayHelper::GetTokenId - invalid token ID" );
    DBG_ASSERT( !::get_flag< sal_uInt8 >( nTokenClass, ~EXC_TOKCLASS_MASK ), "XclTokenArrayHelper::GetTokenId - invalid token class" );
    return nBaseId | nTokenClass;
}

inline void XclTokenArrayHelper::ChangeTokenClass( sal_uInt8& rnTokenId, sal_uInt8 nTokenClass )
{
    DBG_ASSERT( !::get_flag< sal_uInt8 >( nTokenClass, ~EXC_TOKCLASS_MASK ), "XclTokenArrayHelper::ChangeTokenClass - invalid token class" );
    ::set_flag( rnTokenId, EXC_TOKCLASS_MASK, false );
    ::set_flag( rnTokenId, nTokenClass );
}

// ============================================================================

#endif

