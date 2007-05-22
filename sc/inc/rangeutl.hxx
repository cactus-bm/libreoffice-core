/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rangeutl.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 19:39:20 $
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

#ifndef SC_RANGEUTL_HXX
#define SC_RANGEUTL_HXX

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#ifndef INCLUDED_SCDLLAPI_H
#include "scdllapi.h"
#endif

#ifndef _COM_SUN_STAR_TABLE_CELLADDRESS_HPP_
#include <com/sun/star/table/CellAddress.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_CELLRANGEADDRESS_HPP_
#include <com/sun/star/table/CellRangeAddress.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

//------------------------------------------------------------------------

class SvStream;

class ScArea;
class ScDocument;
class ScRange;
class ScRangeName;
class ScRangeList;
class ScDBCollection;

enum RutlNameScope { RUTL_NONE=0, RUTL_NAMES, RUTL_DBASE };

//------------------------------------------------------------------------

class SC_DLLPUBLIC ScRangeUtil
{
public:
                ScRangeUtil()  {}
                ~ScRangeUtil() {}

    BOOL    MakeArea            ( const String&     rAreaStr,
                                  ScArea&           rArea,
                                  ScDocument*       pDoc,
                                  SCTAB         nTab,
                                  ScAddress::Details const & rDetails = ScAddress::detailsOOOa1 ) const;

    void    CutPosString        ( const String&     theAreaStr,
                                  String&           thePosStr ) const;

    BOOL    IsAbsTabArea        ( const String&     rAreaStr,
                                  ScDocument*       pDoc,
                                  ScArea***         pppAreas    = 0,
                                  USHORT*           pAreaCount  = 0,
                                  BOOL              bAcceptCellRef = FALSE,
                                  ScAddress::Details const & rDetails = ScAddress::detailsOOOa1 ) const;

    BOOL    IsAbsArea           ( const String& rAreaStr,
                                  ScDocument*   pDoc,
                                  SCTAB     nTab,
                                  String*       pCompleteStr = 0,
                                  ScRefAddress* pStartPos    = 0,
                                  ScRefAddress* pEndPos      = 0,
                                  ScAddress::Details const & rDetails = ScAddress::detailsOOOa1 ) const;

    BOOL    IsRefArea           ( const String&,
                                  ScDocument*,
                                  SCTAB,
                                  String* = 0,
                                  ScRefAddress* = 0 ) const
                                      { return FALSE; }

    BOOL    IsAbsPos            ( const String& rPosStr,
                                  ScDocument*   pDoc,
                                  SCTAB     nTab,
                                  String*       pCompleteStr = 0,
                                  ScRefAddress* pPosTripel   = 0,
                                  ScAddress::Details const & rDetails = ScAddress::detailsOOOa1 ) const;

    BOOL    MakeRangeFromName   ( const String& rName,
                                    ScDocument*     pDoc,
                                    SCTAB           nCurTab,
                                    ScRange&        rRange,
                                  RutlNameScope eScope=RUTL_NAMES,
                                  ScAddress::Details const & rDetails = ScAddress::detailsOOOa1 ) const;
};

//------------------------------------------------------------------------

class ScRangeStringConverter
{
public:

// helper methods
    static void         AssignString(
                            ::rtl::OUString& rString,
                            const ::rtl::OUString& rNewStr,
                            sal_Bool bAppendStr,
                            sal_Unicode cSeperator = ' ');

    static void         AppendString(
                            ::rtl::OUString& rString,
                            const ::rtl::OUString& rNewStr,
                            sal_Unicode cSeperator = ' ');

    static sal_Int32    IndexOf(
                            const ::rtl::OUString& rString,
                            sal_Unicode cSearchChar,
                            sal_Int32 nOffset,
                            sal_Unicode cQuote = '\'');

    static sal_Int32    IndexOfDifferent(
                            const ::rtl::OUString& rString,
                            sal_Unicode cSearchChar,
                            sal_Int32 nOffset );

    static sal_Int32    GetTokenCount(
                            const ::rtl::OUString& rString,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');

    static void         GetTokenByOffset(
                            ::rtl::OUString& rToken,
                            const ::rtl::OUString& rString,
                            sal_Int32& nOffset,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');

// String to Range core
    static sal_Bool     GetAddressFromString(
                            ScAddress& rAddress,
                            const ::rtl::OUString& rAddressStr,
                            const ScDocument* pDocument,
                            sal_Int32& nOffset,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');
    static sal_Bool     GetRangeFromString(
                            ScRange& rRange,
                            const ::rtl::OUString& rRangeStr,
                            const ScDocument* pDocument,
                            sal_Int32& nOffset,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');
    static sal_Bool     GetRangeListFromString(
                            ScRangeList& rRangeList,
                            const ::rtl::OUString& rRangeListStr,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');

    static sal_Bool     GetAreaFromString(
                            ScArea& rArea,
                            const ::rtl::OUString& rRangeStr,
                            const ScDocument* pDocument,
                            sal_Int32& nOffset,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');

// String to Range API
    static sal_Bool     GetAddressFromString(
                            ::com::sun::star::table::CellAddress& rAddress,
                            const ::rtl::OUString& rAddressStr,
                            const ScDocument* pDocument,
                            sal_Int32& nOffset,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');
    static sal_Bool     GetRangeFromString(
                            ::com::sun::star::table::CellRangeAddress& rRange,
                            const ::rtl::OUString& rRangeStr,
                            const ScDocument* pDocument,
                            sal_Int32& nOffset,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');
    static sal_Bool     GetRangeListFromString(
                            ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress >& rRangeSeq,
                            const ::rtl::OUString& rRangeListStr,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Unicode cQuote = '\'');

// Range to String core
    static void         GetStringFromAddress(
                            ::rtl::OUString& rString,
                            const ScAddress& rAddress,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Bool bAppendStr = sal_False,
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D) );
    static void         GetStringFromRange(
                            ::rtl::OUString& rString,
                            const ScRange& rRange,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Bool bAppendStr = sal_False,
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D) );
    static void         GetStringFromRangeList(
                            ::rtl::OUString& rString,
                            const ScRangeList* pRangeList,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D));

    static void         GetStringFromArea(
                            ::rtl::OUString& rString,
                            const ScArea& rArea,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Bool bAppendStr = sal_False,
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D) );

// Range to String API
    static void         GetStringFromAddress(
                            ::rtl::OUString& rString,
                            const ::com::sun::star::table::CellAddress& rAddress,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Bool bAppendStr = sal_False,
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D) );
    static void         GetStringFromRange(
                            ::rtl::OUString& rString,
                            const ::com::sun::star::table::CellRangeAddress& rRange,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_Bool bAppendStr = sal_False,
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D) );
    static void         GetStringFromRangeList(
                            ::rtl::OUString& rString,
                            const ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress >& rRangeSeq,
                            const ScDocument* pDocument,
                            sal_Unicode cSeperator = ' ',
                            sal_uInt16 nFormatFlags = (SCA_VALID | SCA_TAB_3D) );
};

//------------------------------------------------------------------------

class ScArea
{
public:
            ScArea( SCTAB tab      = 0,
                    SCCOL colStart = 0,
                    SCROW rowStart = 0,
                    SCCOL colEnd   = 0,
                    SCROW rowEnd   = 0 );

            ScArea( const ScArea& r );

    void    Clear       ();
    ScArea& operator=   ( const ScArea& r );
    BOOL    operator==  ( const ScArea& r ) const;
    BOOL    operator!=  ( const ScArea& r ) const  { return !( operator==(r) ); }
    void    GetString   ( String& rStr,
                          BOOL bAbsolute = TRUE, ScDocument* pDoc = NULL,
                          ScAddress::Details const & rDetails = ScAddress::detailsOOOa1 ) const;

public:
    SCTAB nTab;
    SCCOL nColStart;
    SCROW nRowStart;
    SCCOL nColEnd;
    SCROW nRowEnd;
};

SvStream& operator<< ( SvStream& rStream, const ScArea& rArea );
SvStream& operator>> ( SvStream& rStream, ScArea& rArea );

//------------------------------------------------------------------------

//
//  gibt Bereiche mit Referenz und alle DB-Bereiche zurueck
//

class SC_DLLPUBLIC ScAreaNameIterator
{
private:
    ScRangeName*    pRangeName;
    ScDBCollection* pDBCollection;
    BOOL            bFirstPass;
    USHORT          nPos;
    String          aStrNoName;

public:
            ScAreaNameIterator( ScDocument* pDoc );
            ~ScAreaNameIterator() {}

    BOOL    Next( String& rName, ScRange& rRange );
    BOOL    WasDBName() const   { return !bFirstPass; }
};


#endif // SC_RANGEUTL_HXX

