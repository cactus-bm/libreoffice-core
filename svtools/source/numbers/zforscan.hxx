/*************************************************************************
 *
 *  $RCSfile: zforscan.hxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: er $ $Date: 2002-10-29 18:20:14 $
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
#ifndef _ZFORSCAN_HXX
#define _ZFORSCAN_HXX

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _DATE_HXX //autogen
#include <tools/date.hxx>
#endif
#ifndef _LANG_HXX //autogen
#include <tools/lang.hxx>
#endif
#ifndef _COLOR_HXX //autogen
#include <vcl/color.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_NFKEYTAB_HXX
#include "nfkeytab.hxx"
#endif

class SvNumberFormatter;
struct ImpSvNumberformatInfo;


#define SC_MAX_ANZ_FORMAT_STRINGS   100
#define SC_MAX_ANZ_STANDARD_FARBEN  10
#define FLAG_STANDARD_IN_FORMAT     1000
                                            // Hack: nThousand=1000
                                            // => Standard kommt im
                                            // Format vor

enum Sc_SymbolType
{
    SYMBOLTYPE_STRING = -1,                     // literal string in output
    SYMBOLTYPE_DEL    = -2,                     // special character
    SYMBOLTYPE_BLANK  = -3,                     // blank for '_'
    SYMBOLTYPE_STAR   = -4,                     // *-character
    SYMBOLTYPE_DIGIT  = -5,                     // digit place holder
    SYMBOLTYPE_DECSEP = -6,                     // decimal separator
    SYMBOLTYPE_THSEP  = -7,                     // group AKA thousand separator
    SYMBOLTYPE_EXP    = -8,                     // exponent E
    SYMBOLTYPE_FRAC   = -9,                     // fraction /
    SYMBOLTYPE_EMPTY  = -10,                    // deleted symbols
    SYMBOLTYPE_FRACBLANK = -11,                 // delimiter between integer and fraction
    SYMBOLTYPE_COMMENT = -12,                   // comment is following
    SYMBOLTYPE_CURRENCY = -13,                  // currency symbol
    SYMBOLTYPE_CURRDEL  = -14,                  // currency symbol delimiter [$]
    SYMBOLTYPE_CURREXT  = -15,                  // currency symbol extension -xxx
    SYMBOLTYPE_CALENDAR = -16,                  // calendar ID
    SYMBOLTYPE_CALDEL   = -17                   // calendar delimiter [~]
};


class ImpSvNumberformatScan
{
public:

    ImpSvNumberformatScan( SvNumberFormatter* pFormatter );
    ~ImpSvNumberformatScan();
    void ChangeIntl();                          // tauscht Keywords aus

    void ChangeNullDate(USHORT nDay, USHORT nMonth, USHORT nYear);
                                                // tauscht Referenzdatum aus
    void ChangeStandardPrec(short nPrec);       // tauscht Standardprecision aus

    xub_StrLen ScanFormat( String& rString, String& rComment ); // Aufruf der Scan-Analyse

    void CopyInfo(ImpSvNumberformatInfo* pInfo,
                     USHORT nAnz);              // Kopiert die FormatInfo
    USHORT GetAnzResStrings() const             { return nAnzResStrings; }

    const CharClass& GetChrCls() const          { return *pFormatter->GetCharClass(); }
    const LocaleDataWrapper& GetLoc() const     { return *pFormatter->GetLocaleData(); }
    CalendarWrapper& GetCal() const             { return *pFormatter->GetCalendar(); }

    const String* GetKeywords() const
        {
            if ( bKeywordsNeedInit )
                InitKeywords();
            return sKeyword;
        }
    // Keywords used in output like TRUE and FALSE
    const String& GetSpecialKeyword( NfKeywordIndex eIdx ) const
        {
            if ( !sKeyword[eIdx].Len() )
                InitSpecialKeyword( eIdx );
            return sKeyword[eIdx];
        }
    const String& GetTrueString() const     { return GetSpecialKeyword( NF_KEY_TRUE ); }
    const String& GetFalseString() const    { return GetSpecialKeyword( NF_KEY_FALSE ); }
    const String& GetColorString() const    { return GetKeywords()[NF_KEY_COLOR]; }
    const String& GetRedString() const      { return GetKeywords()[NF_KEY_RED]; }
    const String& GetBooleanString() const  { return GetKeywords()[NF_KEY_BOOLEAN]; }
    const String& GetErrorString() const    { return sErrStr; }

    Date* GetNullDate() const                   { return pNullDate; }
    const String& GetStandardName() const
        {
            if ( bKeywordsNeedInit )
                InitKeywords();
            return sNameStandardFormat;
        }
    short GetStandardPrec() const               { return nStandardPrec; }
    const Color& GetRedColor() const            { return StandardColor[4]; }
    Color* GetColor(String& sStr);          // Setzt Hauptfarben oder
                                                // definierte Farben

    // the compatibility currency symbol for old automatic currency formats
    const String& GetCurSymbol() const
        {
            if ( bCompatCurNeedInit )
                InitCompatCur();
            return sCurSymbol;
        }

    // the compatibility currency abbreviation for CCC format code
    const String& GetCurAbbrev() const
        {
            if ( bCompatCurNeedInit )
                InitCompatCur();
            return sCurAbbrev;
        }

    // the compatibility currency symbol upper case for old automatic currency formats
    const String& GetCurString() const
        {
            if ( bCompatCurNeedInit )
                InitCompatCur();
            return sCurString;
        }

    void SetConvertMode(LanguageType eTmpLge, LanguageType eNewLge,
            BOOL bSystemToSystem = FALSE )
    {
        bConvertMode = TRUE;
        eNewLnge = eNewLge;
        eTmpLnge = eTmpLge;
        bConvertSystemToSystem = bSystemToSystem;
    }
    void SetConvertMode(BOOL bMode) { bConvertMode = bMode; }
                                                // Veraendert nur die Bool-Variable
                                                // (zum temporaeren Unterbrechen des
                                                // Convert-Modus)
    const BOOL GetConvertMode()     { return bConvertMode; }
    const LanguageType GetNewLnge() { return eNewLnge; }
                                                // Lesezugriff auf ConvertMode
                                                // und Konvertierungsland/Spr.
    const LanguageType GetTmpLnge() { return eTmpLnge; }
                                                // Lesezugriff auf
                                                // und Ausgangsland/Spr.
    SvNumberFormatter* GetNumberformatter() { return pFormatter; }
                                                // Zugriff auf Formatierer
                                                // (fuer zformat.cxx)


private:                            // ---- privater Teil
    static String theEnglishColors[SC_MAX_ANZ_STANDARD_FARBEN];
    NfKeywordTable sKeyword;                    // Schluesselworte der Syntax
    Color StandardColor[SC_MAX_ANZ_STANDARD_FARBEN];
                                                // Array der Standardfarben
    Date* pNullDate;                            // 30Dec1899
    String sNameStandardFormat;             // "Standard"
    short nStandardPrec;                        // default Precision fuer Standardformat (2)
    SvNumberFormatter* pFormatter;              // Pointer auf die Formatliste

    String sStrArray[SC_MAX_ANZ_FORMAT_STRINGS];// Array der Symbole
    short nTypeArray[SC_MAX_ANZ_FORMAT_STRINGS];// Array der Infos
                                                // externe Infos:
    USHORT nAnzResStrings;                      // Anzahl der Ergebnissymbole
#if !(defined SOLARIS && defined X86)
    short eScannedType;                         // Typ gemaess Scan
#else
    int eScannedType;                           // wg. Optimierung
#endif
    BOOL bThousand;                             // Mit Tausenderpunkt
    USHORT nThousand;                           // Zaehlt ....-Folgen
    USHORT nCntPre;                             // Zaehlt Vorkommastellen
    USHORT nCntPost;                            // Zaehlt Nachkommastellen
    USHORT nCntExp;                             // Zaehlt Exp.Stellen, AM/PM
                                                // interne Infos:
    USHORT nAnzStrings;                         // Anzahl der Symbole
    USHORT nRepPos;                             // Position eines '*'
    USHORT nExpPos;                             // interne Position des E
    USHORT nBlankPos;                           // interne Position des Blank
    short nDecPos;                              // interne Pos. des ,
    BOOL bExp;                                  // wird bei Lesen des E gesetzt
    BOOL bFrac;                                 // wird bei Lesen des / gesetzt
    BOOL bBlank;                                // wird bei ' '(Fraction) ges.
    BOOL bDecSep;                               // Wird beim ersten , gesetzt
    mutable BOOL bKeywordsNeedInit;             // Locale dependent keywords need to be initialized
    mutable BOOL bCompatCurNeedInit;            // Locale dependent compatibility currency need to be initialized
    String sCurSymbol;                          // Currency symbol for compatibility format codes
    String sCurString;                          // Currency symbol in upper case
    String sCurAbbrev;                          // Currency abbreviation
    String sErrStr;                             // String fuer Fehlerausgaben

    BOOL bConvertMode;                          // Wird im Convert-Mode gesetzt
                                                // Land/Sprache, in die der
    LanguageType eNewLnge;                      // gescannte String konvertiert
                                                // wird (fuer Excel Filter)
                                                // Land/Sprache, aus der der
    LanguageType eTmpLnge;                      // gescannte String konvertiert
                                                // wird (fuer Excel Filter)
    BOOL bConvertSystemToSystem;                // Whether the conversion is
                                                // from one system locale to
                                                // another system locale (in
                                                // this case the automatic
                                                // currency symbol is converted
                                                // too).

    xub_StrLen nCurrPos;                        // Position des Waehrungssymbols

    void InitKeywords() const;
    void InitSpecialKeyword( NfKeywordIndex eIdx ) const;
    void InitCompatCur() const;

#ifdef _ZFORSCAN_CXX                // ----- private Methoden -----
    void SetDependentKeywords();
                                                // Setzt die Sprachabh. Keyw.
    void SkipStrings(USHORT& i,xub_StrLen& nPos);// Ueberspringt StringSymbole
    USHORT PreviousKeyword(USHORT i);           // Gibt Index des vorangeh.
                                                // Schluesselworts oder 0
    USHORT NextKeyword(USHORT i);               // Gibt Index des naechsten
                                                // Schluesselworts oder 0
    sal_Unicode PreviousChar(USHORT i);             // Gibt letzten Buchstaben
                                                // vor der Position,
                                                // skipt EMPTY, STRING, STAR, BLANK
    sal_Unicode NextChar(USHORT i);                 // Gibt ersten Buchst. danach
    short PreviousType( USHORT i );             // Gibt Typ vor Position,
                                                // skipt EMPTY
    BOOL IsLastBlankBeforeFrac(USHORT i);       // True <=> es kommt kein ' '
                                                // mehr bis zum '/'
    void Reset();                               // Reset aller Variablen
                                                // vor Analysestart
    short GetKeyWord( const String& sSymbol,    // determine keyword at nPos
        xub_StrLen nPos );                      // return 0 <=> not found

    inline BOOL IsAmbiguousE( short nKey )      // whether nKey is ambiguous E of NF_KEY_E/NF_KEY_EC
        {
            return (nKey == NF_KEY_EC || nKey == NF_KEY_E) &&
                (GetKeywords()[NF_KEY_EC] == GetKeywords()[NF_KEY_E]);
        }

    // if 0 at strArray[i] is of S,00 or SS,00 or SS"any"00 in ScanType() or FinalScan()
    BOOL Is100SecZero( USHORT i, BOOL bHadDecSep );

    short Next_Symbol(const String& rStr,
                        xub_StrLen& nPos,
                      String& sSymbol);       // Naechstes Symbol
    xub_StrLen Symbol_Division(const String& rString);// lexikalische Voranalyse
    xub_StrLen ScanType(const String& rString); // Analyse des Formattyps
    xub_StrLen FinalScan( String& rString, String& rComment );  // Endanalyse mit Vorgabe
                                                // des Typs
    // -1:= error, return nPos in FinalScan; 0:= no calendar, 1:= calendar found
    int FinalScanGetCalendar( xub_StrLen& nPos, USHORT& i, USHORT& nAnzResStrings );

    static inline BOOL StringEqualsChar( const String& rStr, sal_Unicode ch )
        { return rStr.GetChar(0) == ch && rStr.Len() == 1; }
        // Yes, for efficiency get the character first and then compare length
        // because in most places where this is used the string is one char.

    // remove "..." and \... quotes from rStr, return how many chars removed
    static xub_StrLen RemoveQuotes( String& rStr );

#endif //_ZFORSCAN_CXX
};



#endif  // _ZFORSCAN_HXX
