/*************************************************************************
 *
 *  $RCSfile: xehelper.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: hr $ $Date: 2004-09-08 16:28:29 $
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

#ifndef SC_XEHELPER_HXX
#define SC_XEHELPER_HXX

#ifndef SC_XEROOT_HXX
#include "xeroot.hxx"
#endif
#ifndef SC_XESTRING_HXX
#include "xestring.hxx"
#endif

// Export progress bar ========================================================

class ScfProgressBar;

/** The main progress bar for the export filter.

    This class encapsulates creation and initialization of sub progress
    segments. The Activate***Segment() functions activate a specific segement
    of the main progress bar. The implementation of these functions contain the
    calculation of the needed size of the segment. Following calls of the
    Progress() function increase the currently activated sub segment.
 */
class XclExpProgressBar : protected XclExpRoot
{
public:
    explicit            XclExpProgressBar( const XclExpRoot& rRoot );
    virtual             ~XclExpProgressBar();

    /** Initializes all segments and sub progress bars. */
    void                Initialize();

    /** Increases the number of existing ROW records by 1. */
    void                IncRowRecordCount();

    /** Activates the progress segment to create ROW records. */
    void                ActivateCreateRowsSegment();
    /** Activates the progress segment to finalize ROW records. */
    void                ActivateFinalRowsSegment();

    /** Increases the currently activated (sub) progress bar by 1 step. */
    void                Progress();

private:
    typedef ::std::auto_ptr< ScfProgressBar > ScfProgressBarPtr;

    ScfProgressBarPtr   mxProgress;         /// Progress bar implementation.
    ScfProgressBar*     mpSubProgress;      /// Current sub progress bar.

    ScfProgressBar*     mpSubRowCreate;     /// Sub progress bar for creating table rows.
    ScfInt32Vec         maSubSegRowCreate;  /// Segment ID's for all sheets in sub progress bar.

    ScfProgressBar*     mpSubRowFinal;      /// Sub progress bar for finalizing ROW records.
    sal_Int32           mnSegRowFinal;      /// Progress segment for finalizing ROW records.

    sal_uInt32          mnRowCount;         /// Number of created ROW records.
};

// EditEngine->String conversion ==============================================

class SvxURLField;
class XclExpHyperlink;

/** Helper to create HLINK records during creation of formatted cell strings.

    In Excel it is not possible to have more than one hyperlink in a cell. This
    helper detects multiple occurences of hyperlinks and fills a string which
    is used to create a cell note containing all URLs. Only cells containing
    one hyperlink are exported as hyperlink cells.
 */
class XclExpHyperlinkHelper : protected XclExpRoot
{
public:
    typedef ScfRef< XclExpHyperlink > XclExpHyperlinkRef;

    explicit            XclExpHyperlinkHelper( const XclExpRoot& rRoot, const ScAddress& rScPos );
                        ~XclExpHyperlinkHelper();

    /** Processes the passed URL field (tries to create a HLINK record).
        @return  The representation string of the URL field. */
    String              ProcessUrlField( const SvxURLField& rUrlField );

    /** Returns true, if a single HLINK record has been created. */
    bool                HasLinkRecord() const;
    /** Returns the craeted single HLINk record, or an empty reference. */
    XclExpHyperlinkRef  GetLinkRecord();

    /** Returns true, if multiple URLs have been processed. */
    inline bool         HasMultipleUrls() const { return mbMultipleUrls; }
    /** Returns a string containing all processed URLs. */
    inline const String& GetUrlList() { return maUrlList; }

private:
    XclExpHyperlinkRef  mxLinkRec;          /// Created HLINK record.
    ScAddress           maScPos;            /// Cell position to set at the HLINK record.
    String              maUrlList;          /// List with all processed URLs.
    bool                mbMultipleUrls;     /// true = Multiple URL fields processed.
};

// ----------------------------------------------------------------------------

class EditEngine;
class SdrTextObj;
class ScEditCell;
class ScPatternAttr;

/** This class provides methods to create an XclExpString.
    @descr  The string can be created from an edit engine text object or
    directly from a Calc edit cell. */
class XclExpStringHelper : ScfNoInstance
{
public:
    /** Creates a new unformatted string from the passed string.
        @descr  Creates a Unicode string or a byte string, depending on the
                current BIFF version contained in the passed XclExpRoot object.
        @param rString  The source string.
        @param nFlags  Modifiers for string export.
        @param nMaxLen  The maximum number of characters to store in this string.
        @return  The new string object (shared pointer). */
    static XclExpStringRef CreateString(
                            const XclExpRoot& rRoot,
                            const String& rString,
                            XclStrFlags nFlags = EXC_STR_DEFAULT,
                            sal_uInt16 nMaxLen = EXC_STR_MAXLEN );

    /** Appends an unformatted string to an Excel string object.
        @descr  Selects the correct Append() function depending on the current
                BIFF version contained in the passed XclExpRoot object.
        @param rXclString  The Excel string object.
        @param rString  The source string. */
    static void         AppendString(
                            XclExpString& rXclString,
                            const XclExpRoot& rRoot,
                            const String& rString );

    /** Appends a character to an Excel string object.
        @descr  Selects the correct Append() function depending on the current
                BIFF version contained in the passed XclExpRoot object.
        @param rXclString  The Excel string object.
        @param rString  The source string. */
    static void         AppendChar(
                            XclExpString& rXclString,
                            const XclExpRoot& rRoot,
                            sal_Unicode cChar );

    /** Creates a new formatted string from a Calc edit cell.
        @descr  Creates a Unicode string or a byte string, depending on the
                current BIFF version contained in the passed XclExpRoot object.
        @param rEditCell  The Calc edit cell object.
        @param pCellAttr  The set item containing the cell formatting.
        @param rLinkHelper  Helper object for hyperlink conversion.
        @param nFlags  Modifiers for string export.
        @param nMaxLen  The maximum number of characters to store in this string.
        @return  The new string object (shared pointer). */
    static XclExpStringRef CreateString(
                            const XclExpRoot& rRoot,
                            const ScEditCell& rEditCell,
                            const ScPatternAttr* pCellAttr,
                            XclExpHyperlinkHelper& rLinkHelper,
                            XclStrFlags nFlags = EXC_STR_DEFAULT,
                            sal_uInt16 nMaxLen = EXC_STR_MAXLEN );

    /** Creates a new formatted string from a drawing text box.
        @descr  Creates a Unicode string or a byte string, depending on the
                current BIFF version contained in the passed XclExpRoot object.
        @param rTextObj  The text box object.
        @param nFlags  Modifiers for string export.
        @param nMaxLen  The maximum number of characters to store in this string.
        @return  The new string object (shared pointer). */
    static XclExpStringRef CreateString(
                            const XclExpRoot& rRoot,
                            const SdrTextObj& rTextObj,
                            XclStrFlags nFlags = EXC_STR_DEFAULT,
                            sal_uInt16 nMaxLen = EXC_STR_MAXLEN );

    /** Creates a new formatted string from a edit text string.
        @param rEditObj  The edittext object.
        @param nFlags  Modifiers for string export.
        @param nMaxLen The maximum number of characters to store in this string.
        @return  The new string object. */
    static XclExpStringRef CreateString(
                            const XclExpRoot& rRoot,
                            const EditTextObject& rEditObj,
                            XclStrFlags nFlags = EXC_STR_DEFAULT,
                            sal_uInt16 nMaxLen = EXC_STR_MAXLEN );
};

// Header/footer conversion ===================================================

class EditEngine;

/** Converts edit engine text objects to an Excel header/footer string.
    @descr  Header/footer content is divided into three parts: Left, center and
    right portion. All formatting information will be encoded in the Excel string
    using special character seuences. A control sequence starts with the ampersand
    character.

    Supported control sequences:
    &L                      start of left portion
    &C                      start of center portion
    &R                      start of right portion
    &P                      current page number
    &N                      page count
    &D                      current date
    &T                      current time
    &A                      table name
    &F                      file name without path
    &Z                      file path without file name
    &Z&F                    file path and name
    &U                      underlining on/off
    &E                      double underlining on/off
    &S                      strikeout characters on/off
    &X                      superscript on/off
    &Y                      subscript on/off
    &"fontname,fontstyle"   use font with name 'fontname' and style 'fontstyle'
    &fontheight             set font height in points ('fontheight' is a decimal value)

    Known but unsupported control sequences:
    &G                      picture
 */
class XclExpHFConverter : protected XclExpRoot, ScfNoCopy
{
public:
    explicit            XclExpHFConverter( const XclExpRoot& rRoot );

    /** Generates the header/footer string from the passed edit engine text objects. */
    void                GenerateString(
                            const EditTextObject* pLeftObj,
                            const EditTextObject* pCenterObj,
                            const EditTextObject* pRightObj );

    /** Returns the last generated header/footer string. */
    inline const String& GetHFString() const { return maHFString; }
    /** Returns the total height of the last generated header/footer in twips. */
    inline sal_Int32    GetTotalHeight() const { return mnTotalHeight; }

private:
    /** Converts the text object contents and stores it in the passed string. */
    void                AppendPortion(
                            const EditTextObject* pTextObj,
                            sal_Unicode cPortionCode );

private:
    EditEngine&         mrEE;           /// The header/footer edit engine.
    String              maHFString;     /// The last generated header/footer string.
    sal_Int32           mnTotalHeight;  /// Total height of the last header/footer (twips).
};

// URL conversion =============================================================

/** This class contains static methods to encode a file URL.
    @descr  Excel stores URLs in a format that contains special control characters,
    i.e. for directory separators or volume names. */
class XclExpUrlHelper : ScfNoInstance
{
public:
    /** Encodes and returns the URL passed in rAbsUrl to an Excel like URL.
        @param pTableName  Optional pointer to a table name to be encoded in this URL. */
    static String       EncodeUrl( const XclExpRoot& rRoot, const String& rAbsUrl, const String* pTableName = 0 );
    /** Encodes and returns the passed DDE link to an Excel like DDE link. */
    static String       EncodeDde( const String& rApplic, const String rTopic );
};

// Cached Value Lists =========================================================

class XclExpStream;

/** The base class for cached values.
    @descr  Cached values are used to store a list or a 2D array of double,
    string and Boolean values and error codes, for instannce in the records
    CRN and EXTERNNAME or in the token tArray. */
class XclExpCachedValue
{
public:
    virtual             ~XclExpCachedValue();
    virtual void        Save( XclExpStream& rStrm ) const = 0;
};

// ----------------------------------------------------------------------------

/** A cached value that stores a double. */
class XclExpCachedDouble : public XclExpCachedValue
{
public:
    explicit inline     XclExpCachedDouble( double fVal ) : mfVal( fVal ) {}
    /** Writes the double value to stream. */
    virtual void        Save( XclExpStream& rStrm ) const;

private:
    double              mfVal;          /// The double value.
};

// ----------------------------------------------------------------------------

/** A cached value that stores a string. */
class XclExpCachedString : public XclExpCachedValue
{
public:
    explicit            XclExpCachedString( const String& rStr, XclStrFlags nFlags = EXC_STR_DEFAULT );
    /** Writes the string to stream. */
    virtual void        Save( XclExpStream& rStrm ) const;

private:
    XclExpString        maStr;
};

// ----------------------------------------------------------------------------

/** A cached value that stores an error code. */
class XclExpCachedError : public XclExpCachedValue
{
public:
    explicit            XclExpCachedError( USHORT nScError );
    /** Writes the error code to stream. */
    virtual void        Save( XclExpStream& rStrm ) const;

private:
    sal_uInt8           mnError;
};

// ----------------------------------------------------------------------------

class ScDocument;
class ScMatrix;

/** Contains cached values in a 2-dimensional array. */
class XclExpCachedMatrix
{
public:
    /** Constructs and fills a new matrix.
        @param rMatrix  The Calc value matrix.
        @param nFlags  Flags for writing strings. */
    explicit            XclExpCachedMatrix(
                            const ScMatrix& rMatrix,
                            XclStrFlags nFlags = EXC_STR_DEFAULT );

    /** Returns the byte count of all contained data. */
    sal_uInt32          GetSize() const;
    /** Writes the complete matrix to stream. */
    void                Save( XclExpStream& rStrm ) const;

private:
    typedef ScfDelList< XclExpCachedValue > XclExpCachedValueList;

    XclExpCachedValueList maValueList;  /// The list containing the cached values.
    SCSIZE              mnScCols;       /// Calc column count of the value matrix.
    SCSIZE              mnScRows;       /// Calc row count of the value matrix.
};

// ============================================================================

#endif

