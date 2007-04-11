/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: glyphcache.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2007-04-11 17:55:02 $
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

#ifndef _SV_GLYPHCACHE_HXX
#define _SV_GLYPHCACHE_HXX

#ifndef _VCL_DLLAPI_H
#include <vcl/dllapi.h>
#endif

class GlyphCache;
class GlyphMetric;
class GlyphData;
class ServerFont;
class GlyphCachePeer;
class ServerFontLayoutEngine;
class ServerFontLayout;
class ExtraKernInfo;
struct ImplKernPairData;

#include <tools/gen.hxx>
#include <hash_map>
#include <hash_set>

namespace basegfx { class B2DPolyPolygon; }

class RawBitmap;

#include <vcl/outfont.hxx>

class ServerFontLayout;
#include <vcl/sallayout.hxx>

// =======================================================================

class VCL_DLLPUBLIC GlyphCache
{
public:
                                GlyphCache( GlyphCachePeer& );
                                ~GlyphCache();

    static GlyphCache&      GetInstance();
    void                        LoadFonts();

    void                        ClearFontPath();
    void                        AddFontPath( const String& rFontPath );
    void                        AddFontFile( const rtl::OString& rNormalizedName,
                                    int nFaceNum, sal_IntPtr nFontId, const ImplDevFontAttributes&,
                                    const ExtraKernInfo* = NULL );
    void                        AnnounceFonts( ImplDevFontList* ) const;

    ServerFont*                 CacheFont( const ImplFontSelectData& );
    void                        UncacheFont( ServerFont& );

protected:
    GlyphCachePeer&             mrPeer;

private:
    friend class ServerFont;
    // used by ServerFont class only
    void                        AddedGlyph( ServerFont&, GlyphData& );
    void                        RemovingGlyph( ServerFont&, GlyphData&, int nGlyphIndex );
    void                        UsingGlyph( ServerFont&, GlyphData& );
    void                        GrowNotify();

private:
    ULONG                       CalcByteCount() const;
    void                        GarbageCollect();

    // the GlyphCache's FontList matches a font request to a serverfont instance
    // the FontList key's mpFontData member is reinterpreted as integer font id
    struct IFSD_Equal{  bool operator()( const ImplFontSelectData&, const ImplFontSelectData& ) const; };
    struct IFSD_Hash{ size_t operator()( const ImplFontSelectData& ) const; };
    typedef ::std::hash_map<ImplFontSelectData,ServerFont*,IFSD_Hash,IFSD_Equal > FontList;
    FontList                    maFontList;

    ULONG                       mnMaxSize;      // max overall cache size in bytes
    mutable ULONG               mnBytesUsed;
    mutable long                mnLruIndex;
    mutable int                 mnGlyphCount;
    ServerFont*                 mpCurrentGCFont;

    class FreetypeManager*      mpFtManager;
};

// =======================================================================

class GlyphMetric
{
public:
    Point                   GetOffset() const   { return maOffset; }
    Point                   GetDelta() const    { return maDelta; }
    Size                    GetSize() const     { return maSize; }
    long                    GetCharWidth() const { return mnAdvanceWidth; }

protected:
    friend class GlyphData;
    void                    SetOffset( int nX, int nY ) { maOffset = Point( nX, nY); }
    void                    SetDelta( int nX, int nY )  { maDelta = Point( nX, nY); }
    void                    SetSize( const Size& s )    { maSize = s; }
    void                    SetCharWidth( long nW )     { mnAdvanceWidth = nW; }

private:
    long                    mnAdvanceWidth;
    Point                   maDelta;
    Point                   maOffset;
    Size                    maSize;
};

// -----------------------------------------------------------------------

// the glyph specific data needed by a GlyphCachePeer is usually trivial,
// not attaching it to the corresponding GlyphData would be overkill
struct ExtGlyphData
{
    int     meInfo;
    void*   mpData;

    ExtGlyphData() : meInfo(0), mpData(NULL) {}
};

// -----------------------------------------------------------------------

class GlyphData
{
public:
    const GlyphMetric&      GetMetric() const           { return maMetric; }
    Size                    GetSize() const             { return maMetric.GetSize(); }

    void                    SetSize( const Size& s)     { maMetric.SetSize( s ); }
    void                    SetOffset( int nX, int nY ) { maMetric.SetOffset( nX, nY ); }
    void                    SetDelta( int nX, int nY )  { maMetric.SetDelta( nX, nY ); }
    void                    SetCharWidth( long nW )     { maMetric.SetCharWidth( nW ); }

    void                    SetLruValue( int n ) const  { mnLruValue = n; }
    long                    GetLruValue() const         { return mnLruValue;}

    ExtGlyphData&           ExtDataRef()                { return maExtData; }
    const ExtGlyphData&     ExtDataRef() const          { return maExtData; }

private:
    GlyphMetric             maMetric;
    ExtGlyphData            maExtData;

    // used by GlyphCache for cache LRU algorithm
    mutable long            mnLruValue;
};

// =======================================================================

class VCL_DLLPUBLIC ServerFont
{
public:
    virtual const ::rtl::OString*   GetFontFileName() const { return NULL; }
    virtual int                 GetFontFaceNumber() const   { return 0; }
    virtual bool                TestFont() const            { return true; }

    const ImplFontSelectData&   GetFontSelData() const      { return maFontSelData; }

    virtual void                FetchFontMetric( ImplFontMetricData&, long& rFactor ) const = 0;
    virtual ULONG               GetKernPairs( ImplKernPairData** ) const      { return 0; }
    virtual int                 GetGlyphKernValue( int, int ) const           { return 0; }
    virtual ULONG               GetFontCodeRanges( sal_uInt32* ) const { return 0; }
    Point                       TransformPoint( const Point& ) const;

    GlyphData&                  GetGlyphData( int nGlyphIndex );
    const GlyphMetric&          GetGlyphMetric( int nGlyphIndex )
                                { return GetGlyphData( nGlyphIndex ).GetMetric(); }

    virtual int                 GetGlyphIndex( sal_UCS4 ) const = 0;
    virtual bool                GetGlyphOutline( int nGlyphIndex, ::basegfx::B2DPolyPolygon& ) const = 0;
    virtual bool                GetAntialiasAdvice( void ) const = 0;
    bool                        IsGlyphInvisible( int nGlyphIndex );
    virtual bool                GetGlyphBitmap1( int nGlyphIndex, RawBitmap& ) const = 0;
    virtual bool                GetGlyphBitmap8( int nGlyphIndex, RawBitmap& ) const = 0;

    void                        SetExtended( int nInfo, void* ppVoid );
    int                         GetExtInfo() { return mnExtInfo; }
    void*                       GetExtPointer() { return mpExtData; }

protected:
    friend class GlyphCache;
    friend class ServerFontLayout;
                                ServerFont( const ImplFontSelectData& );
    virtual                     ~ServerFont();

    void                        AddRef() const      { ++mnRefCount; }
    long                        GetRefCount() const { return mnRefCount; }
    long                        Release() const;
    ULONG                       GetByteCount() const { return mnBytesUsed; }

    virtual void                InitGlyphData( int nGlyphIndex, GlyphData& ) const = 0;
    virtual void                GarbageCollect( long );
    void                        ReleaseFromGarbageCollect();

    virtual ServerFontLayoutEngine* GetLayoutEngine() { return NULL; }

private:
    typedef ::std::hash_map<int,GlyphData> GlyphList;
    mutable GlyphList           maGlyphList;

    const ImplFontSelectData    maFontSelData;

    // info for GlyphcachePeer
    int                         mnExtInfo;
    void*                       mpExtData;

    // used by GlyphCache for cache LRU algorithm
    mutable long                mnRefCount;
    mutable ULONG               mnBytesUsed;

    ServerFont*                 mpPrevGCFont;
    ServerFont*                 mpNextGCFont;

protected:
    // 16.16 fixed point values used for a rotated font
    long                        mnCos;
    long                        mnSin;
private:
    int                         mnZWJ;
    int                         mnZWNJ;
    bool                        mbCollectedZW;
};

// =======================================================================

// a class for cache entries for physical font instances that are based on serverfonts
class VCL_DLLPUBLIC ImplServerFontEntry : public ImplFontEntry
{
private:
    ServerFont*    mpServerFont;

public:
                   ImplServerFontEntry( ImplFontSelectData& );
    virtual        ~ImplServerFontEntry();
    void           SetServerFont( ServerFont* p) { mpServerFont = p; }
};

// =======================================================================

class VCL_DLLPUBLIC ServerFontLayout : public GenericSalLayout
{
private:
    ServerFont&     mrServerFont;

    // enforce proper copy semantic
    SAL_DLLPRIVATE  ServerFontLayout( const ServerFontLayout& );
    SAL_DLLPRIVATE  ServerFontLayout& operator=( const ServerFontLayout& );

public:
                    ServerFontLayout( ServerFont& );
    virtual bool    LayoutText( ImplLayoutArgs& );
    virtual void    AdjustLayout( ImplLayoutArgs& );
    virtual void    DrawText( SalGraphics& ) const;
    ServerFont&     GetServerFont() const   { return mrServerFont; }
};

// =======================================================================

class ServerFontLayoutEngine
{
public:
    virtual         ~ServerFontLayoutEngine() {}
    virtual bool    operator()( ServerFontLayout&, ImplLayoutArgs& );
};

// =======================================================================

class GlyphCachePeer
{
protected:
                    GlyphCachePeer() : mnBytesUsed(0) {}
    virtual         ~GlyphCachePeer() {}

public:
    sal_Int32       GetByteCount() const { return mnBytesUsed; }
    virtual void    RemovingFont( ServerFont& ) {}
    virtual void    RemovingGlyph( ServerFont&, GlyphData&, int ) {}

protected:
    sal_Int32       mnBytesUsed;
};

// =======================================================================

class VCL_DLLPUBLIC RawBitmap
{
public:
                    RawBitmap();
                    ~RawBitmap();
    bool            Rotate( int nAngle );

public:
    unsigned char*  mpBits;
    ULONG           mnAllocated;

    ULONG           mnWidth;
    ULONG           mnHeight;

    ULONG           mnScanlineSize;
    ULONG           mnBitCount;

    int             mnXOffset;
    int             mnYOffset;
};

// =======================================================================

inline void ServerFont::SetExtended( int nInfo, void* pVoid )
{
    mnExtInfo = nInfo;
    mpExtData = pVoid;
}

// =======================================================================

// ExtraKernInfo allows an on-demand query of extra kerning info #i29881#
// The kerning values have to be scaled to match the font size before use
class VCL_DLLPUBLIC ExtraKernInfo
{
public:
    ExtraKernInfo( sal_IntPtr nFontId );
    virtual ~ExtraKernInfo() {}

    bool    HasKernPairs() const;
    int     GetUnscaledKernPairs( ImplKernPairData** ) const;
    int     GetUnscaledKernValue( sal_Unicode cLeft, sal_Unicode cRight ) const;

protected:
    mutable bool mbInitialized;
    virtual void Initialize() const = 0;

protected:
    sal_IntPtr     mnFontId;

    // container to map a unicode pair to an unscaled kerning value
    struct PairEqual{ int operator()(const ImplKernPairData& rA, const ImplKernPairData& rB) const
                          { return (rA.mnChar1 == rB.mnChar1) && (rA.mnChar2 == rB.mnChar2); } };
    struct PairHash{ int operator()(const ImplKernPairData& rA) const
                         { return (rA.mnChar1) * 256 ^ rA.mnChar2; } };
    typedef std::hash_set< ImplKernPairData, PairHash, PairEqual > UnicodeKernPairs;
    mutable UnicodeKernPairs maUnicodeKernPairs;
};

// =======================================================================

#endif // _SV_GLYPHCACHE_HXX
