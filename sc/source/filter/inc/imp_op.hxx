/*************************************************************************
 *
 *  $RCSfile: imp_op.hxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-08 16:28:01 $
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


#ifndef _IMP_OP_HXX
#define _IMP_OP_HXX


#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

#ifndef SC_XIROOT_HXX
#include "xiroot.hxx"
#endif
#ifndef SC_XISTREAM_HXX
#include "xistream.hxx"
#endif
#ifndef SC_XISTYLE_HXX
#include "xistyle.hxx"
#endif
#ifndef _FLTTYPES_HXX
#include "flttypes.hxx"
#endif
#ifndef _NAMEBUFF_HXX
#include "namebuff.hxx"
#endif
#ifndef _ROOT_HXX
#include "root.hxx"
#endif
#ifndef _OTLNBUFF_HXX
#include "otlnbuff.hxx"
#endif
#ifndef _COLROWST_HXX
#include "colrowst.hxx"
#endif
#ifndef _EXCDEFS_HXX
#include "excdefs.hxx"
#endif


class SfxItemSet;
class SvStream;

class ScFormulaCell;
class SdrObject;
class ScDocument;
class ScToken;
class ScToken2;
class ScExtDocOptions;
class _ScRangeListTabs;

class XF_Buffer;
class ValueFormBuffer;
class ExcelToSc;


class ImportTyp
{
protected:
    CharSet             eQuellChar;     // Quell-Zeichensatz
    ScDocument*         pD;             // Dokument
    ScExtDocOptions*    pExtOpt;        // optionale extended Options

public:
                        ImportTyp( ScDocument*, CharSet eSrc );
    virtual             ~ImportTyp();

    virtual FltError    Read( void );

    ScExtDocOptions&    GetExtOpt( void );
};




class SvInPlaceObjectRef;




struct ExcelChartData
{
    Rectangle               aRect;              // Ecken
    String                  aTitle, aXTitle, aYTitle, aZTitle;
    String                  aLastLabel;         // letzter SERIESTEXT-Label
    SfxItemSet*             pAttrs;             // Attribute
    ExcelChartData*         pNext;              // wer weiss schon...
    UINT16                  nRow1, nCol1, nRow2, nCol2, nTab1, nTab2;   // Quellbereich
    UINT16                  nBaseTab;
    UINT32                  nObjNum;

                            ExcelChartData( ScDocument*, const Point&, const Point&,
                                            const UINT16 nBaseTab );
                            ~ExcelChartData();
};

class OutlineDataBuffer
{
private:
    OutlineBuffer*                      pColOutlineBuff;
    OutlineBuffer*                      pRowOutlineBuff;
    ColRowSettings*                     pColRowBuff;
    UINT16 nTab;

public:
    OutlineDataBuffer(RootData& rRootData, UINT16 nTab);

    inline ColRowSettings* GetColRowBuff() const { return pColRowBuff ; }
    inline OutlineBuffer* GetColOutline()  const { return pColOutlineBuff; }
    inline OutlineBuffer* GetRowOutline()  const { return pRowOutlineBuff; }
    void Apply(ScDocument* pD);
};

class ImportExcel : private XclImpRootData, public ImportTyp, public XclImpRoot
{
private:
    ExcelChartData*         pChart;             // aktuelle Chart-Daten
    ExcelChartData*         pUsedChartFirst;    // benutzte Chart-Daten, erster
    ExcelChartData*         pUsedChartLast;     // benutzte Chart-Daten, letzter

protected:
    static const double     fExcToTwips;        // Umrechnung 1/256 Zeichen -> Twips

    RootData*               pExcRoot;

    XclImpStream            maStrm;             // input stream
    XclImpStream&           aIn;                // input stream
    String                  maPassword;

    NameBuffer*             pExtNameBuff;       // ... externe Namen (Ind.-Basis=1)
    _ScRangeListTabs*       pPrintRanges;
    _ScRangeListTabs*       pPrintTitles;
    ExcelToSc*              pFormConv;          // Formel-Konverter

    SfxItemSet*             pStyleSheetItemSet; // aktuelle Seitenvorlage

    OutlineBuffer*          pColOutlineBuff;
    OutlineBuffer*          pRowOutlineBuff;
    ColRowSettings*         pColRowBuff;        // Col/Row-Einstellungen 1 Tabelle

    typedef ScfDelList< OutlineDataBuffer >        OutlineListBuffer;
    OutlineListBuffer*      pOutlineListBuffer;

    UINT16                  nIxfeIndex;         // merkt sich Angabe im IXFE-Record
    UINT16                  nLastXF;            // letzter XF in Formula-Record
    UINT16                  nBdshtTab;          // Counter fuer Boundsheet
    UINT16                  nFirstVisTab;       // index of first visible tab for WINDOW2
    ScFormulaCell*          pLastFormCell;      // fuer String-Records

    BOOL                    bTabTruncated;      // wenn Bereichsueberschreitung zum
                                                //  Abschneiden von Zellen fuehrt

    sal_Bool                bFitToPage;         /// Stores fit to page setting from WSBOOL record.
    sal_Bool                bHasHeader;         /// Stores whether header for current sheet exists.
    sal_Bool                bHasFooter;         /// Stores whether footer for current sheet exists.

    // Record-Funktionen
    void                    Dimensions( void );             // 0x00
    void                    Blank25( void );                // 0x01
    void                    Integer( void );                // 0x02
    void                    Number25( void );               // 0x03
    void                    Label25( void );                // 0x04
    void                    Boolerr25( void );              // 0x05
    void                    Formula25( void );              // 0x06     -> excform.cxx
    void                    RecString( void );              // 0x07, 0x0207
    void                    Row25( void );                  // 0x08
    void                    Bof2( void );                   // 0x09
    void                    Eof( void );                    // 0x0A
    void                    DocProtect( void );             // 0x12
    void                    Protect( void );                // 0x12 Sheet Protection
    BOOL                    Password( void );               // 0x13
    void                    Header( void );                 // 0x14
    void                    Footer( void );                 // 0x15
    void                    Externsheet( void );            // 0x17
    void                    Name25( void );                 // 0x18
    void                    Verticalpagebreaks( void );     // 0x1A
    void                    Horizontalpagebreaks( void );   // 0x1B
    void                    Note( void );                   // 0x1C
    void                    Selection( void );              // 0x1D
    void                    Columndefault( void );          // 0x20
    void                    Array25( void );                // 0x21
    void                    Rec1904( void );                // 0x22
    void                    Externname25( void );           // 0x23
    void                    Colwidth( void );               // 0x24
    void                    Defrowheight2( void );          // 0x25
    void                    Leftmargin( void );             // 0x26
    void                    Rightmargin( void );            // 0x27
    void                    Topmargin( void );              // 0x28
    void                    Bottommargin( void );           // 0x29
    void                    Printheaders( void );           // 0x2A
    void                    Prntgrdlns( void );             // 0x2B
    BOOL                    Filepass( void );               // 0x2F
//      void                Window1( void );                // 0x3D
    void                    Pane( void );                   // 0x41
    void                    Codepage( void );               // 0x42
    void                    Ixfe( void );                   // 0x44
    void                    DefColWidth( void );            // 0x55
    void                    Builtinfmtcnt( void );          // 0x56
    void                    Obj( void );                    // 0x5D
    void                    Colinfo( void );                // 0x7D
    void                    Rk( void );                     // 0x7E
    void                    Wsbool( void );                 // 0x81
    void                    Gridset( void );                // 0x82
    void                    Hcenter( void );                // 0x83
    void                    Vcenter( void );                // 0x84
    void                    Boundsheet( void );             // 0x85
    void                    Country( void );                // 0x8C
    void                    Hideobj( void );                // 0x8D
    void                    Bundleheader( void );           // 0x8F
    void                    Palette( void );                // 0x92
    void                    Standardwidth( void );          // 0x99
    void                    Scl( void );                    // 0xA0
    void                    Setup( void );                  // 0xA1
    void                    Setup5( void );                 // 0xA1
    void                    Shrfmla( void );                // 0xBC
    void                    Mulrk( void );                  // 0xBD
    void                    Mulblank( void );               // 0xBE
    void                    Rstring( void );                // 0xD6
    void                    Olesize( void );                // 0xDE
    void                    Blank34( void );                // 0x0201
    void                    Number34( void );               // 0x0203
    void                    Label34( void );                // 0x0204
    void                    Boolerr34( void );              // 0x0205
    void                    Formula3( void );               // 0x0206       -> excform.cxx
                                                            // 0x0207 -> 0x07
    void                    Row34( void );                  // 0x0208
    void                    Bof3( void );                   // 0x0209
    void                    Name34( void );                 // 0x0218
    void                    Array34( void );                // 0x0221
    void                    Externname34( void );           // 0x0223
    void                    Defrowheight345( void );        // 0x0225
    void                    TableOp( void );                // 0x0236
    void                    Window2_5( void );              // 0x023E
    //void                  Rk( void );                     // 0x027E -> 0x7E
    void                    Formula4( void );               // 0x0406       -> excform.cxx
    void                    Bof4( void );                   // 0x0409
    void                    Bof5( void );                   // 0x0809
    // ---------------------------------------------------------------
    void                    SetLineStyle( SfxItemSet&, short, short, short );
    void                    SetFillStyle( SfxItemSet&, short, short, short );
    SdrObject*              LineObj( SfxItemSet&, const Point&, const Point& );
    SdrObject*              RectObj( SfxItemSet&, const Point&, const Point& );
    SdrObject*              BeginChartObj( SfxItemSet&, const Point&, const Point& );
    void                    EndChartObj( void );
    void                    ChartSelection( void );
    void                    ChartSeriesText( void );
    void                    ChartObjectLink( void );
    void                    Chart3D( void );
    void                    ChartAi( void );
    void                    ChartAlruns( void );
    void                    ChartArea( void );
    void                    ChartAreaformat( void );
    void                    ChartAttachedlabel( void );
    void                    ChartAxesused( void );
    void                    ChartAxis( void );
    void                    ChartAxislineformat( void );
    void                    ChartAxisparent( void );
    void                    ChartBar( void );
    void                    ChartCatserrange( void );
    void                    ChartChart( void );
    void                    ChartChartformat( void );
    void                    ChartChartline( void );
    void                    ChartDataformat( void );
    void                    ChartDefaulttext( void );
    void                    ChartDropbar( void );
    void                    ChartFontx( void );
    void                    ChartFrame( void );
    void                    ChartIfmt( void );
    void                    ChartLegend( void );
    void                    ChartLegendxn( void );
    void                    ChartLine( void );
    void                    ChartLineform( void );
    void                    ChartMarkerformat( void );
//      void                ChartObjectlink( void );
    void                    ChartPicf( void );
    void                    ChartPie( void );
    void                    ChartPieformat( void );
    void                    ChartPlotarea( void );
    void                    ChartPos( void );
    void                    ChartRadar( void );
    void                    ChartRadararea( void );
    void                    ChartSbaseref( void );
    void                    ChartSerauxerrbar( void );
    void                    ChartSerauxtrend( void );
    void                    ChartSerfmt( void );
    void                    ChartSeries( void );
    void                    ChartSerieslist( void );
    void                    ChartSeriestext( void );
    void                    ChartSerparent( void );
    void                    ChartSertocrt( void );
    void                    ChartShtprops( void );
    void                    ChartSurface( void );
    void                    ChartText( void );
    void                    ChartTick( void );
    void                    ChartValuerange( void );

    // ---------------------------------------------------------------
    void                    Formula( UINT16 nCol, UINT16 nRow, UINT16 nTab,
                                UINT16 nXF, UINT16 nFormLen, double &rCurVal,
                                BYTE nFlag, BOOL bShrFmla );
                                            //      -> excform.cxx

    void                    EndSheet( void );
    void                    NeueTabelle( void );
    const ScTokenArray*     ErrorToFormula( BYTE bErrOrVal, BYTE nError,
                                double& rVal );
    void                    GetHF( BOOL bHeader );
    virtual void            GetHFString( String& rStr );

                                // nSide -> IMPEXC_MARGINSIDE_*
    String                  GetPageStyleName( UINT16 nTab );
    EditTextObject*         CreateFormText( BYTE, const String&, const UINT16 );
    virtual void            EndAllChartObjects( void );     // -> excobj.cxx

    virtual void            AdjustRowHeight();
    virtual void            PostDocLoad( void );
    virtual void            SetTextCell( const UINT16 nCol, const UINT16 nRow,
                                        String& rUnconvertedText, const UINT16 nXF );
                                            // Achtung: rUnconvertedText wird moeglicherweise veraendert

    /** Sets a margin item into an item set.
        @param rItemSet  The destination item set.
        @param fMargin  The Excel margin value in inches.
        @param eType  The margin type. */
    void                    SetMarginItem( SfxItemSet& rItemSet, double fMarginInch, XclMarginType eType );

public:
                            ImportExcel( SvStream&, ScDocument*, const String& rBasePath );

    virtual                 ~ImportExcel( void );

    /** Sets a password for stream decryption. */
    inline void             SetPassword( const String& rPassword ) { maPassword = rPassword; }

    virtual FltError        Read( void );
};

#endif

