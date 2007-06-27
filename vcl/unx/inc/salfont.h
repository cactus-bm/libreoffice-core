/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: salfont.h,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 20:40:57 $
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

#ifndef _SV_SALFONT_H
#define _SV_SALFONT_H

// -=-= exports =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class   SalFontCache;
struct  SalFontDimension;
class   SalFontFamily;
class   SalFontFamilyList;
class   SalFontStruct;
class   SalFontStructList;
class   SalFonts;

// -=-= includes -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef _SALSTD_HXX
#include <salstd.hxx>
#endif
#ifndef _SV_OUTFONT_HXX
#include <vcl/outfont.hxx>
#endif

// -=-= forwards =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
typedef ULONG XFP_FLAGS;

class   SalDisplay;
class   SalFontCacheItem;

// -=-= SalFontCache -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
DECLARE_LIST( SalFontCache, SalFontCacheItem* )

// -=-= SalFontDimension -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
struct SalFontDimension
{
            USHORT      nHeight_;               // [pixel]
            USHORT      nPtHeight_;             // [point/10]
            USHORT      nAverage_;              // [pixel/10]
            USHORT      nXRes_;                 // [dpi]
            USHORT      nYRes_;                 // [dpi]
            USHORT      nSlant_;                // [pixel]
//          size_t      nUnderlineThickness_;   // [pixel]
//          size_t      nUnderlinePosition_;    // [pixel]
//          size_t      nStrikeoutAscent_;      // [pixel]
//          size_t      nStrikeoutDescent_;     // [pixel]
//          Subscript, Superscript, Capital, Space ...

    inline              SalFontDimension( USHORT nA = 0, USHORT nH = 0 );

    inline  BOOL        IsScalable() const;
    inline  USHORT      GetWidth() const { return (nAverage_ + 5) / 10; }
    inline  Size        GetSize() const;
    inline  void        SetSize( const Size & rSize );
    inline  BOOL        operator == ( const SalFontDimension &r ) const;
    inline  BOOL        operator != ( const SalFontDimension &r ) const;
    inline  BOOL        operator >= ( const SalFontDimension &r ) const;
};

inline SalFontDimension::SalFontDimension( USHORT nA, USHORT nH )
    : nHeight_( nH ), nAverage_( nA )
{ nPtHeight_ = nXRes_ = nYRes_ = nSlant_ = 0; }

inline BOOL SalFontDimension::IsScalable() const
{ return !nHeight_ && !nPtHeight_ && !nAverage_; }

inline Size SalFontDimension::GetSize() const
{ return Size( (nAverage_ + 5) / 10, nHeight_ ); }

inline void SalFontDimension::SetSize( const Size & rSize )
{ nAverage_ = (USHORT)rSize.Width() * 10; nHeight_ = (USHORT)rSize.Height(); }

inline BOOL SalFontDimension::operator == ( const SalFontDimension &r ) const
{ return nHeight_ == r.nHeight_ && (!r.nAverage_ || nAverage_ == r.nAverage_); }

inline BOOL SalFontDimension::operator != ( const SalFontDimension &r ) const
{ return !(*this == r); }

inline BOOL SalFontDimension::operator >= ( const SalFontDimension &r ) const
{ return nHeight_ > r.nHeight_
         || (nHeight_ == r.nHeight_ && nAverage_ >= r.nAverage_); }

// -=-= SalFontStruct =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class SalFontStruct : public ImplFontMetricData
{
    friend class SalDisplay;
    friend class SalGraphicsData;

            SalFontCacheItem*pCache_;

#if (OSL_DEBUG_LEVEL > 1) || defined  DBG_UTIL
            ByteString      aFontName_;
#endif
            USHORT          nHeightCount_;      // Anzahl der Hoehen-Eintraege
            SalFontDimension*pDimensions_;      // Hoehen-Array
            USHORT          nWeight_;

            USHORT          nFoundry_;          // properties indexies
            USHORT          nFamily_;
            USHORT          nWeightName_;
            USHORT          nSlant_;
            USHORT          nSetWidthName_;
            ByteString      aAddStyleName_;
            USHORT          nSpacing_;
            USHORT          nCharSet_;
            USHORT          nFaceName_;
            BOOL            mbValidFontDescription; // valid xlfd entries

            void            Init();
            BOOL            Init( SalDisplay*       pDisp,
                                  const char*       pFontName,
                                  SalFontDimension& rDim );

            ByteString      GetXFontName( const SalFontDimension& );

    inline  void            SetFoundry( USHORT n )
        { nFoundry_ = n; }
    inline  void            SetFamily( USHORT n )
        { meFamily = sal_FamilyToSal( nFamily_ = n ); }
    inline  void            SetWeightName( USHORT n )
        { meWeight = sal_WeightToSal( nWeightName_ = n ); }
    inline  void            SetSlant( USHORT n )
        { meItalic = sal_ItalicToSal( nSlant_ = n ); }
    inline  void            SetSetWidthName( USHORT n )
        { nSetWidthName_ = n; }
    inline  void            SetAddStyleName( const ByteString& rAddStyle )
        { aAddStyleName_ = rAddStyle; aAddStyleName_.ToLowerAscii(); }
    inline  void            SetSpacing( USHORT n )
        { mePitch = sal_PitchToSal( nSpacing_ = n ); }
    inline  void            SetAverage( long n )
        { mnWidth = (n + 5) / 10; }
    void                    SetCharSet( USHORT n );

                            SalFontStruct( const SalFontStruct& rFont );
public:
                            SalFontStruct( SalDisplay*          pDisp,
                                           const char*          pFontName,
                                           SalFontDimension&    rDim );

                            ~SalFontStruct();

    inline  void            Cache( SalFontCacheItem *p ) { pCache_ = p; }
    inline  SalFontCacheItem*IsCache() const { return pCache_; }
    inline  BOOL            IsScalable() const { return TYPE_SCALABLE==meType; }
    inline  SalFontDimension*GetDim() const { return pDimensions_; }
    inline  BOOL            IsValid() const { return mbValidFontDescription; }
#ifdef DBG_UTIL
            const ByteString&   GetName() const { return aFontName_; }
#endif

            ImplFontData   *GetDevFontData();
            SalFontCacheItem*Load( SalDisplay *pDisp, const SalFontDimension &rDim );
            CharSet GetCharSet() { return meCharSet; }

};

// -=-= SalFontStructList =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
DECLARE_LIST( SalFontStructList, SalFontStruct* )

#endif // _SV_SALFONT_H

