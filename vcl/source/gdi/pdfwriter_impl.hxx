/*************************************************************************
 *
 *  $RCSfile: pdfwriter_impl.hxx,v $
 *
 *  $Revision: 1.30 $
 *
 *  last change: $Author: hr $ $Date: 2004-09-08 16:22:06 $
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
#ifndef _VCL_PDFWRITER_IMPL_HXX
#define _VCL_PDFWRITER_IMPL_HXX

#ifndef _VCL_PDFWRITER_HXX
#include <pdfwriter.hxx>
#endif

#ifndef _RTL_USTRING_HXX
#include <rtl/ustring.hxx>
#endif
#ifndef _OSL_FILE_H
#include <osl/file.h>
#endif
#ifndef _GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _SV_OUTDEV_HXX
#include <outdev.hxx>
#endif
#ifndef _SV_BITMAPEX_HXX
#include <bitmapex.hxx>
#endif
#ifndef _SV_GRADIENT_HXX
#include <gradient.hxx>
#endif
#ifndef _SV_HATCH_HXX
#include <hatch.hxx>
#endif
#ifndef _SV_WALL_HXX
#include <wall.hxx>
#endif
#ifndef _SV_OUTDATA_HXX
#include <outdata.hxx>
#endif
#ifndef _RTL_STRBUF_HXX
#include <rtl/strbuf.hxx>
#endif

#include <vector>
#include <map>
#include <hash_map>
#include <list>

class SalLayout;
class ImplLayoutArgs;
class ImplFontData;
class ImplFontSelectData;
class ImplFontMetricData;
struct FontSubsetInfo;
class ZCodec;

namespace vcl
{

class PDFSalLayout;

class PDFWriterImpl
{
    friend class PDFSalLayout;
public:
    // definition of structs
    struct BuiltinFont
    {
        const char *                m_pName;                     // Name
        const char *                m_pStyleName;                // StyleName
        const char *                m_pPSName;                   // PSName
        int                         m_nAscent;
        int                         m_nDescent;
        FontFamily                  m_eFamily;                   // Family
        CharSet                     m_eCharSet;                  // CharSet
        FontPitch                   m_ePitch;                    // Pitch
        FontWidth                   m_eWidthType;                // WidthType
        FontWeight                  m_eWeight;                   // Weight
        FontItalic                  m_eItalic;                   // Italic
        int                         m_aWidths[256];              // character metrics
    };

    struct PDFPage
    {
        PDFWriterImpl*              m_pWriter;
        sal_Int32                   m_nPageWidth;           // in inch/72
        sal_Int32                   m_nPageHeight;          // in inch/72
        PDFWriter::Orientation      m_eOrientation;
        sal_Int32                   m_nPageObject;
        sal_Int32                   m_nPageIndex;
        sal_Int32                   m_nStreamObject;
        sal_Int32                   m_nStreamLengthObject;
        sal_uInt64                  m_nBeginStreamPos;
        std::vector<sal_Int32>      m_aAnnotations;
        std::vector<sal_Int32>      m_aMCIDParents;
        PDFWriter::PageTransition   m_eTransition;
        sal_uInt32                  m_nTransTime;
        sal_uInt32                  m_nDuration;

        PDFPage( PDFWriterImpl* pWriter, sal_Int32 nPageWidth, sal_Int32 nPageHeight, PDFWriter::Orientation eOrientation );
        ~PDFPage();

        void beginStream();
        void endStream();
        bool emit( sal_Int32 nParentPage );

        // converts point from ref device coordinates to
        // page coordinates and appends the point to the buffer
        // if bNeg is true, the coordinates are inverted AFTER transformation
        // to page (useful for transformation matrices
        // if pOutPoint is set it will be updated to the emitted point
        // (in PDF map mode, that is 10th of point)
        void appendPoint( const Point& rPoint, rtl::OStringBuffer& rBuffer, bool bNeg = false, Point* pOutPoint = NULL ) const;
        // appends a rectangle
        void appendRect( const Rectangle& rRect, rtl::OStringBuffer& rBuffer ) const;
        // converts a rectangle to 10th points page space
        void convertRect( Rectangle& rRect ) const;
        // appends a polygon optionally closing it
        void appendPolygon( const Polygon& rPoly, rtl::OStringBuffer& rBuffer, bool bClose = true ) const;
        // appends a polypolygon optionally closing the subpaths
        void appendPolyPolygon( const PolyPolygon& rPolyPoly, rtl::OStringBuffer& rBuffer, bool bClose = true ) const;
        // converts a length (either vertical or horizontal; this
        // can be important if the source MapMode is not
        // symmetrical) to page length and appends it to the buffer
        // if pOutLength is set it will be updated to the emitted length
        // (in PDF map mode, that is 10th of point)
        void appendMappedLength( sal_Int32 nLength, rtl::OStringBuffer& rBuffer, bool bVertical = true, sal_Int32* pOutLength = NULL ) const;
        // the same for double values
        void appendMappedLength( double fLength, rtl::OStringBuffer& rBuffer, bool bVertical = true, sal_Int32* pOutLength = NULL ) const;
        // appends LineInfo
        void appendLineInfo( const LineInfo& rInfo, rtl::OStringBuffer& rBuffer ) const;
        // appends a horizontal waveline with vertical offset (helper for drawWaveLine)
        void appendWaveLine( sal_Int32 nLength, sal_Int32 nYOffset, sal_Int32 nDelta, rtl::OStringBuffer& rBuffer ) const;

        sal_Int32 getWidth() const { return m_nPageWidth ? m_nPageWidth : m_pWriter->m_nInheritedPageWidth; }
        sal_Int32 getHeight() const { return m_nPageHeight ? m_nPageHeight : m_pWriter->m_nInheritedPageHeight; }
    };

    friend struct PDFPage;

    struct BitmapID
    {
        Size        m_aPixelSize;
        sal_Int32   m_nSize;
        sal_Int32   m_nChecksum;
        sal_Int32   m_nMaskChecksum;

        BitmapID() : m_nSize( 0 ), m_nChecksum( 0 ), m_nMaskChecksum( 0 ) {}

        BitmapID& operator=( const BitmapID& rCopy )
        {
            m_aPixelSize    = rCopy.m_aPixelSize;
            m_nSize         = rCopy.m_nSize;
            m_nChecksum     = rCopy.m_nChecksum;
            m_nMaskChecksum = rCopy.m_nMaskChecksum;
            return *this;
        }

        bool operator==( const BitmapID& rComp )
        {
            return (m_aPixelSize == rComp.m_aPixelSize &&
                    m_nSize == rComp.m_nSize &&
                    m_nChecksum == rComp.m_nChecksum &&
                    m_nMaskChecksum == rComp.m_nMaskChecksum );
        }
    };

    struct BitmapEmit
    {
        BitmapID    m_aID;
        BitmapEx    m_aBitmap;
        sal_Int32   m_nObject;
        bool        m_bDrawMask;

        BitmapEmit() : m_bDrawMask( false ) {}
    };

    struct JPGEmit
    {
        BitmapID            m_aID;
        SvMemoryStream*     m_pStream;
        Bitmap              m_aMask;
        sal_Int32           m_nObject;

        JPGEmit() : m_pStream( NULL ) {}
        ~JPGEmit() { delete m_pStream; }
    };

    struct GradientEmit
    {
        Gradient    m_aGradient;
        Size        m_aSize;
        sal_Int32   m_nObject;
    };

    // for bitmap tilings (drawWallpaper)
    struct BitmapPatternEmit
    {
        sal_Int32   m_nObject;
        sal_Int32   m_nBitmapObject;
        Rectangle   m_aRectangle;
    };

    // for transparency group XObjects
    struct TransparencyEmit
    {
        sal_Int32           m_nObject;
        sal_Int32           m_nExtGStateObject;
        double              m_fAlpha;
        Rectangle           m_aBoundRect;
        SvMemoryStream*     m_pContentStream;
        SvMemoryStream*     m_pSoftMaskStream;

        TransparencyEmit()
                : m_nObject( 0 ),
                  m_nExtGStateObject( -1 ),
                  m_fAlpha( 0.0 ),
                  m_pContentStream( NULL ),
                  m_pSoftMaskStream( NULL )
        {}
        ~TransparencyEmit()
        {
            delete m_pContentStream;
            delete m_pSoftMaskStream;
        }
    };

    // font subsets
    struct GlyphEmit
    {
        sal_uInt8       m_nSubsetGlyphID;
        sal_Unicode     m_aUnicode;
    };
    typedef std::map< long, GlyphEmit > FontEmitMapping;
    struct FontEmit
    {
        sal_Int32           m_nFontID;
        FontEmitMapping     m_aMapping;

        FontEmit( sal_Int32 nID ) : m_nFontID( nID ) {}
    };
    typedef std::list< FontEmit > FontEmitList;
    struct Glyph
    {
        sal_Int32   m_nFontID;
        sal_uInt8   m_nSubsetGlyphID;
    };
    typedef std::map< long, Glyph > FontMapping;

    struct FontSubset
    {
        FontEmitList        m_aSubsets;
        FontMapping         m_aMapping;
    };
    typedef std::map< ImplFontData*, FontSubset > FontSubsetData;

    struct EmbedCode
    {
        sal_Unicode         m_aUnicode;
        rtl::OString        m_aName;
    };
    struct EmbedEncoding
    {
        sal_Int32                               m_nFontID;
        std::vector< EmbedCode >                m_aEncVector;
        std::map< sal_Unicode, sal_Int8 >       m_aCMap;
    };
    struct EmbedFont
    {
        sal_Int32                       m_nNormalFontID;
        std::list< EmbedEncoding >      m_aExtendedEncodings;
    };
    typedef std::map< ImplFontData*, EmbedFont > FontEmbedData;

    struct PDFDest
    {
        sal_Int32                   m_nPage;
        PDFWriter::DestAreaType     m_eType;
        Rectangle                   m_aRect;
    };

    struct PDFOutlineEntry
    {
        sal_Int32                   m_nParentID;
        sal_Int32                   m_nObject;
        sal_Int32                   m_nParentObject;
        sal_Int32                   m_nNextObject;
        sal_Int32                   m_nPrevObject;
        std::vector< sal_Int32 >    m_aChildren;
        rtl::OUString               m_aTitle;
        sal_Int32                   m_nDestID;

        PDFOutlineEntry()
                : m_nParentID( -1 ),
                  m_nObject( 0 ),
                  m_nParentObject( 0 ),
                  m_nNextObject( 0 ),
                  m_nPrevObject( 0 ),
                  m_nDestID( -1 )
        {}
    };

    struct PDFAnnotation
    {
        sal_Int32                   m_nObject;
        Rectangle                   m_aRect;
        sal_Int32                   m_nPage;

        PDFAnnotation()
                : m_nObject( -1 ),
                  m_nPage( -1 )
        {}
    };

    struct PDFLink : public PDFAnnotation
    {
        sal_Int32                   m_nDest; // set to -1 for URL, to a dest else
        rtl::OUString               m_aURL;

        PDFLink()
                : m_nDest( -1 )
        {}
    };

    struct PDFNoteEntry : public PDFAnnotation
    {
        PDFNote                     m_aContents;

        PDFNoteEntry()
        {}
    };

    typedef std::hash_map< rtl::OString, SvMemoryStream*, rtl::OStringHash > PDFAppearanceStreams;
    typedef std::hash_map< rtl::OString, PDFAppearanceStreams, rtl::OStringHash > PDFAppearanceMap;

    struct PDFWidget : public PDFAnnotation
    {
        PDFWriter::WidgetType       m_eType;
        rtl::OString                m_aName;
        rtl::OUString               m_aDescription;
        rtl::OUString               m_aText;
        USHORT                      m_nTextStyle;
        rtl::OUString               m_aValue;
        rtl::OString                m_aDAString;
        rtl::OString                m_aMKDict;
        sal_Int32                   m_nFlags;
        sal_Int32                   m_nParent; // if not 0, parent's object number
        std::vector<sal_Int32>      m_aKids; // widget children, contains object numbers
        sal_Int32                   m_nRadioGroup;
        sal_Int32                   m_nMaxLen;
        std::list<rtl::OUString>    m_aListEntries;
        PDFAppearanceMap            m_aAppearances;
        PDFWidget()
                : m_eType( PDFWriter::PushButton ),
                  m_nTextStyle( 0 ),
                  m_nFlags( 0 ),
                  m_nParent( 0 ),
                  m_nRadioGroup( -1 ),
                  m_nMaxLen( 0 )
        {}
    };

    struct PDFStructureAttribute
    {
        PDFWriter::StructAttributeValue     eValue;
        sal_Int32                           nValue;

        PDFStructureAttribute()
                : eValue( PDFWriter::Invalid ),
                  nValue( 0 )
        {}

        PDFStructureAttribute( PDFWriter::StructAttributeValue eVal )
                : eValue( eVal ),
                  nValue( 0 )
        {}

        PDFStructureAttribute( sal_Int32 nVal )
                : eValue( PDFWriter::Invalid ),
                  nValue( nVal )
        {}
    };

    typedef std::map<PDFWriter::StructAttribute, PDFStructureAttribute > PDFStructAttributes;

    struct PDFStructureElement
    {
        sal_Int32                                           m_nObject;
        PDFWriter::StructElement                            m_eType;
        sal_Int32                                           m_nOwnElement; // index into structure vector
        sal_Int32                                           m_nParentElement; // index into structure vector
        sal_Int32                                           m_nFirstPageObject;
        bool                                                m_bOpenMCSeq;
        std::list< sal_Int32 >                              m_aChildren; // indices into strucure vector
        rtl::OStringBuffer                                  m_aKids;
        PDFStructAttributes                                 m_aAttributes;
        Rectangle                                           m_aBBox;
        rtl::OUString                                       m_aActualText;
        rtl::OUString                                       m_aAltText;

        // m_aContents contains the element's marked content sequence
        // as pairs of (page nr, MCID)

        PDFStructureElement()
                : m_nObject( 0 ),
                  m_eType( PDFWriter::NonStructElement ),
                  m_nOwnElement( -1 ),
                  m_nParentElement( -1 ),
                  m_nFirstPageObject( 0 ),
                  m_bOpenMCSeq( false )
        {
        }

    };

    static const sal_Char* getStructureTag( PDFWriter::StructElement );
    static const sal_Char* getAttributeTag( PDFWriter::StructAttribute eAtr );
    static const sal_Char* getAttributeValueTag( PDFWriter::StructAttributeValue eVal );

private:
    static const BuiltinFont m_aBuiltinFonts[14];

    OutputDevice*                       m_pReferenceDevice;

    MapMode                             m_aMapMode; // PDFWriterImpl scaled units
    std::vector< PDFPage >              m_aPages;
    PDFDocInfo                          m_aDocInfo;
    /* maps object numbers to file offsets (needed for xref) */
    std::vector< sal_uInt64 >           m_aObjects;
    /* contains Bitmaps until they are written to the
     *  file stream as XObjects*/
    std::list< BitmapEmit >             m_aBitmaps;
    /* contains JPG streams until written to file     */
    std::list<JPGEmit>                  m_aJPGs;
    /* contains all dests ever set during the PDF creation,
       dest id is always the dest's position in this vector
     */
    std::vector<PDFDest>                m_aDests;
    /* contains all links ever set during PDF creation,
       link id is always the link's position in this vector
    */
    std::vector<PDFLink>                m_aLinks;
    /* maps arbitrary link ids for structure attributes to real link ids
       (for setLinkPropertyId)
    */
    std::map<sal_Int32, sal_Int32>      m_aLinkPropertyMap;
    /* contains all outline items,
       object 0 is the outline root
     */
    std::vector<PDFOutlineEntry>        m_aOutline;
    /* contains all notes set during PDF creation
     */
    std::vector<PDFNoteEntry>           m_aNotes;
    /* the root of the structure tree
     */
    std::vector<PDFStructureElement>    m_aStructure;
    /* current object in the structure hierarchy
     */
    sal_Int32                           m_nCurrentStructElement;
    /* emit strucure marks currently (aka. NonStructElement or not)
     */
    bool                                m_bEmitStructure;
    bool                                m_bNewMCID;

    /* contains all widgets used in the PDF
     */
    std::vector<PDFWidget>              m_aWidgets;
    /* maps radio group id to index of radio group control in m_aWidgets */
    std::map< sal_Int32, sal_Int32 >    m_aRadioGroupWidgets;
    /* used to store control id during beginControlAppearance/endControlAppearance */
    sal_Int32                           m_nCurrentControl;

    /* contains Bitmaps for gradient functions until they are written
     *  to the file stream */
    std::list< GradientEmit >           m_aGradients;
    /* contains bitmap tiling patterns */
    std::list< BitmapPatternEmit >      m_aTilings;
    std::list< TransparencyEmit >       m_aTransparentObjects;
    /*  contains all font subsets in use */
    FontSubsetData                      m_aSubsets;
    FontEmbedData                       m_aEmbeddedFonts;
    sal_Int32                           m_nNextFID;

    sal_Int32                           m_nInheritedPageWidth;  // in inch/72
    sal_Int32                           m_nInheritedPageHeight; // in inch/72
    PDFWriter::Orientation              m_eInheritedOrientation;
    sal_Int32                           m_nCurrentPage;

    sal_Int32                           m_nCatalogObject;
    sal_Int32                           m_nResourceDict;
    sal_Int32                           m_nFontResourceDict;

    PDFWriter::PDFWriterContext         m_aContext;
    oslFileHandle                       m_aFile;
    bool                                m_bOpen;


    /* output redirection; e.g. to accumulate content streams for
       XObjects
     */
    struct StreamRedirect
    {
        SvStream*       m_pStream;
        MapMode         m_aMapMode;
    };
    std::list< StreamRedirect >         m_aOutputStreams;

    // graphics state
    struct GraphicsState
    {
        Font            m_aFont;
        MapMode         m_aMapMode;
        Color           m_aLineColor;
        Color           m_aFillColor;
        Color           m_aTextLineColor;
        Region          m_aClipRegion;
        sal_Int32       m_nAntiAlias;
        sal_Int32       m_nLayoutMode;
        sal_Int32       m_nTransparentPercent;
        sal_uInt16      m_nFlags;

        GraphicsState() :
                m_aLineColor( COL_TRANSPARENT ),
                m_aFillColor( COL_TRANSPARENT ),
                m_aTextLineColor( COL_TRANSPARENT ),
                m_nAntiAlias( 1 ),
                m_nLayoutMode( 0 ),
                m_nTransparentPercent( 0 ),
                m_nFlags( 0xffff )
        {}
        GraphicsState( const GraphicsState& rState ) :
                m_aFont( rState.m_aFont ),
                m_aMapMode( rState.m_aMapMode ),
                m_aLineColor( rState.m_aLineColor ),
                m_aFillColor( rState.m_aFillColor ),
                m_aTextLineColor( rState.m_aTextLineColor ),
                m_aClipRegion( rState.m_aClipRegion ),
                m_nAntiAlias( rState.m_nAntiAlias ),
                m_nLayoutMode( rState.m_nLayoutMode ),
                m_nTransparentPercent( rState.m_nTransparentPercent ),
                m_nFlags( rState.m_nFlags )
        {
        }

        GraphicsState& operator=(const GraphicsState& rState )
        {
            m_aFont                 = rState.m_aFont;
            m_aMapMode              = rState.m_aMapMode;
            m_aLineColor            = rState.m_aLineColor;
            m_aFillColor            = rState.m_aFillColor;
            m_aTextLineColor        = rState.m_aTextLineColor;
            m_aClipRegion           = rState.m_aClipRegion;
            m_nAntiAlias            = rState.m_nAntiAlias;
            m_nLayoutMode           = rState.m_nLayoutMode;
            m_nTransparentPercent   = rState.m_nTransparentPercent;
            m_nFlags                = rState.m_nFlags;
            return *this;
        }
    };
    std::list< GraphicsState >              m_aGraphicsStack;
    GraphicsState                           m_aCurrentPDFState;

    ZCodec*                                 m_pCodec;
    SvMemoryStream*                         m_pMemStream;

    /* creates fonts and subsets that will be emitted later */
    void registerGlyphs( int nGlyphs, sal_Int32* pGlyphs, sal_Unicode* pUnicodes, sal_uInt8* pMappedGlyphs, sal_Int32* pMappedFontObjects, ImplFontData* pFallbackFonts[] );

    /*  emits a text object according to the passed layout */
    /* TODO: remove rText as soon as SalLayout will change so that rText is not necessary anymore */
    void drawLayout( SalLayout& rLayout, const String& rText, bool bTextLines );
    void drawRelief( SalLayout& rLayout, const String& rText, bool bTextLines );
    void drawShadow( SalLayout& rLayout, const String& rText, bool bTextLines );

    /*  writes differences between graphics stack and current real PDF
     *   state to the file
     */
    void updateGraphicsState();

    /* writes a transparency group object */
    bool writeTransparentObject( TransparencyEmit& rObject );

    /* writes an XObject of type image, may create
       a second for the mask
     */
    bool writeBitmapObject( BitmapEmit& rObject, bool bMask = false );

    bool writeJPG( JPGEmit& rEmit );

    /* tries to find the bitmap by its id and returns its emit data if exists,
       else creates a new emit data block */
    const BitmapEmit& createBitmapEmit( const BitmapEx& rBitmapEx, bool bDrawMask = false );

    /* writes the Do operation inside the content stream */
    void drawBitmap( const Point& rDestPt, const Size& rDestSize, const BitmapEmit& rBitmap, const Color& rFillColor );
    /* write the function object for a Gradient */
    bool writeGradientFunction( GradientEmit& rObject );
    /* creates a GradientEmit and returns its object number */
    sal_Int32 createGradient(  const Gradient& rGradient, const Size& rSize );

    /* writes all tilings */
    bool emitTilings();
    /* writes all gradient patterns */
    bool emitGradients();
    /* writes a builtin font object and returns its objectid (or 0 in case of failure ) */
    sal_Int32 emitBuiltinFont( ImplFontData* pFont );
    /* writes a type1 embedded font object and returns its mapping from font ids to object ids (or 0 in case of failure ) */
    std::map< sal_Int32, sal_Int32 > emitEmbeddedFont( ImplFontData* pFont, EmbedFont& rEmbed );
    /* writes a font descriptor and returns its object id (or 0) */
    sal_Int32 emitFontDescriptor( ImplFontData* pFont, FontSubsetInfo& rInfo, sal_Int32 nSubsetID, sal_Int32 nStream );
    /* writes a ToUnicode cmap, returns the corresponding stream object */
    sal_Int32 createToUnicodeCMap( sal_uInt8* pEncoding, sal_Unicode* pUnicodes, int nGlyphs );

    /* get resource dict object number */
    sal_Int32 getResourceDictObj()
    {
        if( m_nResourceDict <= 0 )
            m_nResourceDict = createObject();
        return m_nResourceDict;
    }

    /* get font dict object number */
    sal_Int32 getFontDictObj()
    {
        if( m_nFontResourceDict <= 0 )
            m_nFontResourceDict = createObject();
        return m_nFontResourceDict;
    }

    /* writes a the font dictionary and emits all font objects
     * returns object id of font directory (or 0 on error)
     */
    sal_Int32 emitFonts();
    /* writes the Resource dictionary;
     * returns dict object id (or 0 on error)
     */
    sal_Int32 emitResources();
    // appends a dest
    bool appendDest( sal_Int32 nDestID, rtl::OStringBuffer& rBuffer );
    // write all links
    bool emitLinkAnnotations();
    // write all notes
    bool emitNoteAnnotations();
    // write the appearance streams of a widget
    bool emitAppearances( PDFWidget& rWidget, rtl::OStringBuffer& rAnnotDict );
    // write all widgets
    bool emitWidgetAnnotations();
    // writes all annotation objects
    bool emitAnnotations();
    // writes the dest dict for the catalog
    sal_Int32 emitDestDict();
    // writes outline dict and tree
    sal_Int32 emitOutline();
    // puts the attribute objects of a structure element into the returned string,
    // helper for emitStructure
    rtl::OString emitStructureAttributes( PDFStructureElement& rEle );
    // writes document structure
    sal_Int32 emitStructure( PDFStructureElement& rEle );
    // writes structure parent tree
    sal_Int32 emitStructParentTree();
    // writes page tree and catalog
    bool emitCatalog();
    // writes xref and trailer
    bool emitTrailer();
    // emits info dict (if applicable), passes back the contents of all values written
    // for recommend file id computation
    sal_Int32 emitInfoDict( rtl::OString& rIDOut );

    // default appearences for widgets
    sal_Int32 findRadioGroupWidget( sal_Int32 nGroup );
    Font replaceFont( const Font& rControlFont, const Font& rAppSetFont );

    void createDefaultPushButtonAppearance( PDFWidget&, const PDFWriter::PushButtonWidget& rWidget );
    void createDefaultCheckBoxAppearance( PDFWidget&, const PDFWriter::CheckBoxWidget& rWidget );
    void createDefaultRadioButtonAppearance( PDFWidget&, const PDFWriter::RadioButtonWidget& rWidget );
    void createDefaultEditAppearance( PDFWidget&, const PDFWriter::EditWidget& rWidget );
    void createDefaultListBoxAppearance( PDFWidget&, const PDFWriter::ListBoxWidget& rWidget );

    /* adds an entry to m_aObjects and returns its index+1,
     * sets the offset to ~0
     */
    sal_Int32 createObject();
    /* sets the offset of object n to the current position of output file+1
     */
    bool updateObject( sal_Int32 n );

    bool writeBuffer( const void* pBuffer, sal_uInt64 nBytes );
    void beginCompression();
    void endCompression();
    void beginRedirect( SvStream* pStream, const Rectangle& );
    SvStream* endRedirect();

    void endPage();

    void beginStructureElementMCSeq();
    void endStructureElementMCSeq();
    /** checks whether a non struct element lies in the ancestor hierarchy
        of the current structure element

        @returns
        <true/> if no NonStructElement was found in ancestor path and tagged
        PDF output is enabled
        <false/> else
     */
    bool checkEmitStructure();

    /* draws an emphasis mark */
    void drawEmphasisMark(  long nX, long nY, const PolyPolygon& rPolyPoly, BOOL bPolyLine, const Rectangle& rRect1, const Rectangle& rRect2 );
public:
    PDFWriterImpl( const PDFWriter::PDFWriterContext& rContext );
    ~PDFWriterImpl();

    /*  for OutputDevice so the reference device can have a list
     *  that contains only suitable fonts (subsettable or builtin)
     *  produces a new font list
     */
    ImplDevFontList* filterDevFontList( ImplDevFontList* pFontList );
    /*  for OutputDevice: get layout for builtin fonts
     */
    bool isBuiltinFont( ImplFontData* pFont ) const;
    SalLayout* GetTextLayout( ImplLayoutArgs& rArgs, ImplFontSelectData* pFont );
    void getFontMetric( ImplFontSelectData* pFont, ImplFontMetricData* pMetric ) const;


    /* for documentation of public functions please see pdfwriter.hxx */

    OutputDevice* getReferenceDevice();

    /* document structure */
    sal_Int32 newPage( sal_Int32 nPageWidth , sal_Int32 nPageHeight, PDFWriter::Orientation eOrientation );
    bool emit();

    PDFWriter::PDFVersion getVersion() const { return m_aContext.Version; }
    void setDocInfo( const PDFDocInfo& rInfo );
    const PDFDocInfo& getDocInfo() const { return m_aDocInfo; }


    /* graphics state */
    void push( sal_uInt16 nFlags );
    void pop();

    void setFont( const Font& rFont )
    { m_aGraphicsStack.front().m_aFont = rFont; }

    void setMapMode( const MapMode& rMapMode );
    void setMapMode() { setMapMode( m_aMapMode ); }


    const MapMode& getMapMode() { return m_aGraphicsStack.front().m_aMapMode; }

    void setLineColor( const Color& rColor )
    { m_aGraphicsStack.front().m_aLineColor = rColor; }

    void setFillColor( const Color& rColor )
    { m_aGraphicsStack.front().m_aFillColor = rColor; }

    void setTextLineColor()
    { m_aGraphicsStack.front().m_aTextLineColor = Color( COL_TRANSPARENT ); }

    void setTextLineColor( const Color& rColor )
    { m_aGraphicsStack.front().m_aTextLineColor = rColor; }

    void setTextFillColor( const Color& rColor )
    {
        m_aGraphicsStack.front().m_aFont.SetFillColor( rColor );
        m_aGraphicsStack.front().m_aFont.SetTransparent( ImplIsColorTransparent( rColor ) );
    }
    void setTextFillColor()
    {
        m_aGraphicsStack.front().m_aFont.SetFillColor( Color( COL_TRANSPARENT ) );
        m_aGraphicsStack.front().m_aFont.SetTransparent( TRUE );
    }
    void setTextColor( const Color& rColor )
    { m_aGraphicsStack.front().m_aFont.SetColor( rColor ); }

    void clearClipRegion()
    { m_aGraphicsStack.front().m_aClipRegion.SetNull(); }

    void setClipRegion( const Region& rRegion );

    void moveClipRegion( sal_Int32 nX, sal_Int32 nY );

    bool intersectClipRegion( const Rectangle& rRect );

    bool intersectClipRegion( const Region& rRegion );

    void setLayoutMode( sal_Int32 nLayoutMode )
    { m_aGraphicsStack.front().m_nLayoutMode = nLayoutMode; }

    void setTextAlign( TextAlign eAlign )
    { m_aGraphicsStack.front().m_aFont.SetAlign( eAlign ); }

    void setAntiAlias( sal_Int32 nAntiAlias )
    { m_aGraphicsStack.front().m_nAntiAlias = nAntiAlias; }

    /* actual drawing functions */
    void drawText( const Point& rPos, const String& rText, xub_StrLen nIndex = 0, xub_StrLen nLen = STRING_LEN, bool bTextLines = true );
    void drawTextArray( const Point& rPos, const String& rText, const sal_Int32* pDXArray = NULL, xub_StrLen nIndex = 0, xub_StrLen nLen = STRING_LEN, bool bTextLines = true );
    void drawStretchText( const Point& rPos, ULONG nWidth, const String& rText,
                          xub_StrLen nIndex = 0, xub_StrLen nLen = STRING_LEN,
                          bool bTextLines = true  );
    void drawText( const Rectangle& rRect, const String& rOrigStr, USHORT nStyle, bool bTextLines = true  );
    void drawTextLine( const Point& rPos, long nWidth, FontStrikeout eStrikeout, FontUnderline eUnderline, bool bUnderlineAbove );

    void drawLine( const Point& rStart, const Point& rStop );
    void drawLine( const Point& rStart, const Point& rStop, const LineInfo& rInfo );
    void drawPolygon( const Polygon& rPoly );
    void drawPolyPolygon( const PolyPolygon& rPolyPoly );
    void drawPolyLine( const Polygon& rPoly );
    void drawPolyLine( const Polygon& rPoly, const LineInfo& rInfo );
    void drawWaveLine( const Point& rStart, const Point& rStop, sal_Int32 nDelta, sal_Int32 nLineWidth );

    void drawPixel( const Point& rPt, const Color& rColor );
    void drawPixel( const Polygon& rPts, const Color* pColors = NULL );

    void drawRectangle( const Rectangle& rRect );
    void drawRectangle( const Rectangle& rRect, sal_uInt32 nHorzRound, sal_uInt32 nVertRound );
    void drawEllipse( const Rectangle& rRect );
    void drawArc( const Rectangle& rRect, const Point& rStart, const Point& rStop, bool bWithPie, bool bWidthChord );

    void drawBitmap( const Point& rDestPoint, const Size& rDestSize, const Bitmap& rBitmap );
    void drawBitmap( const Point& rDestPoint, const Size& rDestSize, const BitmapEx& rBitmap );
    void drawMask( const Point& rDestPoint, const Size& rDestSize, const Bitmap& rBitmap, const Color& rFillColor );
    void drawJPGBitmap( SvStream& rDCTData, const Size& rSizePixel, const Rectangle& rTargetArea, const Bitmap& rMask );

    void drawGradient( const Rectangle& rRect, const Gradient& rGradient );
    void drawGradient( const PolyPolygon& rPolyPoly, const Gradient& rGradient );
    void drawHatch( const PolyPolygon& rPolyPoly, const Hatch& rHatch );
    void drawWallpaper( const Rectangle& rRect, const Wallpaper& rWall );
    void drawTransparent( const PolyPolygon& rPolyPoly, sal_uInt32 nTransparentPercent );
    void beginTransparencyGroup();
    void endTransparencyGroup( const Rectangle& rBoundingBox, sal_uInt32 nTransparentPercent );
    void endTransparencyGroup( const Rectangle& rBoundingBox, const Bitmap& rAlphaMask );

    void emitComment( const char* pComment );

    // links
    sal_Int32 createLink( const Rectangle& rRect, sal_Int32 nPageNr = -1 );
    sal_Int32 createDest( const Rectangle& rRect, sal_Int32 nPageNr = -1, PDFWriter::DestAreaType eType = PDFWriter::XYZ );
    sal_Int32 setLinkDest( sal_Int32 nLinkId, sal_Int32 nDestId );
    sal_Int32 setLinkURL( sal_Int32 nLinkId, const rtl::OUString& rURL );
    void setLinkPropertyId( sal_Int32 nLinkId, sal_Int32 nPropertyId );

    // outline
    sal_Int32 createOutlineItem( sal_Int32 nParent = 0, const rtl::OUString& rText = rtl::OUString(), sal_Int32 nDestID = -1 );
    sal_Int32 setOutlineItemParent( sal_Int32 nItem, sal_Int32 nNewParent );
    sal_Int32 setOutlineItemText( sal_Int32 nItem, const rtl::OUString& rText );
    sal_Int32 setOutlineItemDest( sal_Int32 nItem, sal_Int32 nDestID );

    // notes
    void createNote( const Rectangle& rRect, const PDFNote& rNote, sal_Int32 nPageNr = -1 );
    // structure elements
    sal_Int32 beginStructureElement( PDFWriter::StructElement eType );
    void endStructureElement();
    bool setCurrentStructureElement( sal_Int32 nElement );
    sal_Int32 getCurrentStructureElement();
    bool setStructureAttribute( enum PDFWriter::StructAttribute eAttr, enum PDFWriter::StructAttributeValue eVal );
    bool setStructureAttributeNumerical( enum PDFWriter::StructAttribute eAttr, sal_Int32 nValue );
    void setStructureBoundingBox( const Rectangle& rRect );
    void setActualText( const String& rText );
    void setAlternateText( const String& rText );

    // transitional effects
    void setAutoAdvanceTime( sal_uInt32 nSeconds, sal_Int32 nPageNr = -1 );
    void setPageTransition( PDFWriter::PageTransition eType, sal_uInt32 nMilliSec, sal_Int32 nPageNr = -1 );

    // controls
    sal_Int32 createControl( const PDFWriter::AnyWidget& rControl, sal_Int32 nPageNr = -1 );
    void beginControlAppearance( sal_Int32 nControl );
    bool endControlAppearance( PDFWriter::WidgetState eState );

    // helper: eventually begin marked content sequence and
    // emit a comment in debug case
    void MARK( const char* pString )
    {
        beginStructureElementMCSeq();
#if OSL_DEBUG_LEVEL > 1
        emitComment( pString );
#endif
    }
};

}

#endif //_VCL_PDFEXPORT_HXX
