/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: bmpdemo.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: thb $ $Date: 2006-05-31 09:49:44 $
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

#ifndef  _USE_MATH_DEFINES
#define  _USE_MATH_DEFINES  // needed by Visual C++ for math constants
#endif
#include <math.h>

#include <comphelper/processfactory.hxx>
#include <comphelper/regpathhelper.hxx>
#include <cppuhelper/servicefactory.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>

#include <ucbhelper/contentbroker.hxx>
#include <ucbhelper/configurationkeys.hxx>

#include <vcl/window.hxx>
#include <vcl/svapp.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/unowrap.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/bmpacc.hxx>

#include <basegfx/polygon/b2dpolypolygonrasterconverter.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/numeric/ftools.hxx>
#include <basegfx/vector/b2isize.hxx>
#include <basegfx/point/b2ipoint.hxx>

#include <basebmp/color.hxx>
#include <basebmp/scanlineformats.hxx>
#include <basebmp/bitmapdevice.hxx>

#include <rtl/bootstrap.hxx>

#include <vigra/metaprogramming.hxx>
#include <vigra/static_assert.hxx>
#include <vigra/resizeimage.hxx>
#include <vigra/basicimageview.hxx>

#include <boost/static_assert.hpp>
#include <algorithm>

using namespace ::com::sun::star;


namespace
{

/// template meta function: add const qualifier, if given 2nd type has it
template<typename A, typename B> struct clone_const
{
    typedef B type;
};
template<typename A, typename B> struct clone_const<const A,B>
{
    typedef const B type;
};

template< class DestIterator, class DestAccessor > class Renderer :
        public basegfx::B2DPolyPolygonRasterConverter
{
private:
    typename DestIterator::value_type fillColor_;
    typename DestIterator::value_type clearColor_;
    DestIterator                      begin_;
    DestAccessor                      accessor_;

public:
    Renderer(const basegfx::B2DPolyPolygon&     rPolyPolyRaster,
             typename DestIterator::value_type  fillColor,
             typename DestIterator::value_type  clearColor,
             DestIterator                       begin,
             DestIterator                       end,
             DestAccessor                       accessor ) :
        B2DPolyPolygonRasterConverter(rPolyPolyRaster,
                                      basegfx::B2DRange(0,0,
                                                        end.x - end.x,
                                                        begin.y - begin.y )),
        fillColor_( fillColor ),
        clearColor_( clearColor ),
        begin_( begin ),
        accessor_( accessor )
    {
    }

    virtual void span(const double& rfXLeft,
                      const double& rfXRight,
                      sal_Int32     nY,
                      bool          bOn )
    {
        DestIterator currIter( begin_ + vigra::Diff2D(0,nY) );
        typename DestIterator::row_iterator rowIter( currIter.rowIterator() +
                                                     basegfx::fround(rfXLeft) );
        typename DestIterator::row_iterator rowEnd( currIter.rowIterator() +
                                                    basegfx::fround(rfXRight) );
        if( bOn )
            while( rowIter != rowEnd )
            {
                accessor_.set(fillColor_, rowIter);
                ++rowIter;
            }
        else
            while( rowIter != rowEnd )
            {
                accessor_.set(accessor_(rowIter)*clearColor_, rowIter);
                ++rowIter;
            }
    }
};

template< class DestIterator, class DestAccessor >
    std::auto_ptr< Renderer< DestIterator, DestAccessor > > makeRenderer(
        const basegfx::B2DPolyPolygon&                          rPolyPolyRaster,
        typename DestIterator::value_type                       fillColor,
        typename DestIterator::value_type                       clearColor,
        vigra::triple<DestIterator, DestIterator, DestAccessor> dest )
{
    return std::auto_ptr< Renderer< DestIterator, DestAccessor > >(
        new Renderer< DestIterator, DestAccessor >(rPolyPolyRaster,
                                                   fillColor,
                                                   clearColor,
                                                   dest.first,
                                                   dest.second,
                                                   dest.third));
}


// changed semantics re. DirectionSelector<StridedArrayTag>: stride
// now counts in <em>raw</em> bytes!
template< typename T > class StridedArrayIterator
{
public:
    typedef typename clone_const<T, unsigned char>::type  internal_type;

    StridedArrayIterator(int stride, T* ptr = 0) :
        stride_(stride),
        current_(reinterpret_cast<internal_type*>(ptr))
    {}

    /// Copy from other StridedArrayIterator, plus given offset
    StridedArrayIterator( StridedArrayIterator const& rSrc,
                          int                         offset ) :
        stride_(rSrc.stride_),
        current_(reinterpret_cast<internal_type*>(
                     reinterpret_cast<T*>(rSrc.current_)+offset))
    {}

    void operator++() {current_ += stride_; }
    void operator++(int) {current_ += stride_; }
    void operator--() {current_ -= stride_; }
    void operator--(int) {current_ -= stride_; }
    void operator+=(int dy) {current_ += dy*stride_; }
    void operator-=(int dy) {current_ -= dy*stride_; }

    bool operator==(StridedArrayIterator const & rhs) const
    { return (current_ == rhs.current_); }

    bool operator!=(StridedArrayIterator const & rhs) const
    { return (current_ != rhs.current_); }

    bool operator<(StridedArrayIterator const & rhs) const
    { return (current_ < rhs.current_); }

    bool operator<=(StridedArrayIterator const & rhs) const
    { return (current_ <= rhs.current_); }

    bool operator>(StridedArrayIterator const & rhs) const
    { return (current_ > rhs.current_); }

    bool operator>=(StridedArrayIterator const & rhs) const
    { return (current_ >= rhs.current_); }

    int operator-(StridedArrayIterator const & rhs) const
    { return (current_ - rhs.current_) / stride_; }

    T* operator()() const
    { return reinterpret_cast<T*>(current_); }

    T* operator()(int d) const
    { return reinterpret_cast<T*>(current_ + d*stride_); }

    int            stride_;
    internal_type* current_;
};

/// template meta function: remove const qualifier from plain type
template <typename T> struct remove_const
{
    typedef T type;
};
template <typename T> struct remove_const<const T>
{
    typedef T type;
};

/// returns true, if given number is strictly less than 0
template< typename T > inline bool is_negative( T x )
{
    return x < 0;
}

/// Overload for ints (branch-free)
inline bool is_negative( int x )
{
    // force logic shift (result for signed shift right is undefined)
    return static_cast<unsigned int>(x) >> (sizeof(int)*8-1);
}

/// Get bitmask for data at given intra-word position, for given bit depth
template< typename data_type, int bits_per_pixel, bool MsbFirst, typename difference_type > inline data_type get_mask( difference_type d )
{
    BOOST_STATIC_ASSERT(bits_per_pixel > 0);
    BOOST_STATIC_ASSERT(sizeof(data_type)*8 % bits_per_pixel == 0);
    BOOST_STATIC_ASSERT(sizeof(data_type)*8 / bits_per_pixel > 1);
    BOOST_STATIC_ASSERT(vigra::TypeTraits<data_type>::isPOD::asBool);

    const unsigned int nIntraWordPositions( sizeof(data_type)*8 / bits_per_pixel );

    //      create bits_per_pixel 1s      shift to intra-word position
    return ((~(~0 << bits_per_pixel)) << bits_per_pixel*(MsbFirst ?
                                                         (nIntraWordPositions-1 - (d % nIntraWordPositions)) :
                                                         (d % nIntraWordPositions)));
}

template< int num_intraword_positions, int bits_per_pixel, bool MsbFirst, typename difference_type > inline difference_type get_shift( difference_type remainder )
{
    return bits_per_pixel*(MsbFirst ?
                           (num_intraword_positions - 1 - remainder) :
                           remainder);
}

template< typename Datatype,
          typename Valuetype,
          int      bits_per_pixel,
          bool     MsbFirst > class PackedPixelColumnIterator
{
public:
    // no reference, no index_reference type here
    typedef Datatype                                    data_type;
    typedef Valuetype                                   value_type;
    typedef int                                         difference_type;
    typedef image_traverser_tag                         iterator_category;

    typedef typename remove_const<data_type>::type      mask_type;
    typedef data_type*                                  pointer;
    typedef StridedArrayIterator< data_type >           MoveY;

    enum {
        /** The number of pixel within a single data_type value
         */
        num_intraword_positions=sizeof(data_type)*8/bits_per_pixel,
        /** Bit mask for one pixel (least significant bits)
         */
        bit_mask=~(~0 << bits_per_pixel)
    };

private:
    MoveY           y;
    mask_type       mask_;
    difference_type shift_;

    void inc()
    {
        ++y;
    }

    void dec()
    {
        --y;
    }

    bool equal( PackedPixelColumnIterator const & rhs ) const
    {
        return rhs.y == y;
    }

    bool less( PackedPixelColumnIterator const & rhs ) const
    {
        return y < rhs.y;
    }

public:
    PackedPixelColumnIterator() :
        y(0),
        mask_( get_mask<data_type, bits_per_pixel, MsbFirst, difference_type>(0) ),
        shift_( get_shift<num_intraword_positions, bits_per_pixel, MsbFirst, difference_type>(0) )
    {}

    PackedPixelColumnIterator( const MoveY& base, difference_type remainder ) :
        y(base),
        mask_( get_mask<data_type, bits_per_pixel, MsbFirst>(remainder) ),
        shift_( get_shift<num_intraword_positions, bits_per_pixel, MsbFirst>(remainder) )
    {}

    PackedPixelColumnIterator& operator+=( difference_type d )
    {
        y += d;
        return *this;
    }

    PackedPixelColumnIterator& operator-=( difference_type d )
    {
        y -= d;
        return *this;
    }

    PackedPixelColumnIterator operator+( difference_type d )
    {
        PackedPixelColumnIterator res(*this);
        res += d;
        return res;
    }

    PackedPixelColumnIterator operator-( difference_type d )
    {
        PackedPixelColumnIterator res(*this);
        res -= d;
        return res;
    }

    PackedPixelColumnIterator& operator++()
    {
        inc();
        return *this;
    }

    PackedPixelColumnIterator& operator--()
    {
        dec();
        return *this;
    }

    PackedPixelColumnIterator operator++(int)
    {
        PackedPixelColumnIterator res(*this);
        res.inc();
        return res;
    }

    PackedPixelColumnIterator operator--(int)
    {
        PackedPixelColumnIterator res(*this);
        res.dec();
        return res;
    }

    bool operator==(PackedPixelColumnIterator const & rhs) const
    {
        return equal( rhs );
    }

    bool operator!=(PackedPixelColumnIterator const & rhs) const
    {
        return !equal( rhs );
    }

    bool operator<(PackedPixelColumnIterator const & rhs) const
    {
        return less(rhs);
    }

    bool operator<=(PackedPixelColumnIterator const & rhs) const
    {
        return !less(rhs);
    }

    bool operator>(PackedPixelColumnIterator const & rhs) const
    {
        return rhs.less(*this);
    }

    bool operator>=(PackedPixelColumnIterator const & rhs) const
    {
        return !rhs.less(*this);
    }

    difference_type operator-(PackedPixelColumnIterator const & rhs) const
    {
        return y - rhs.y;
    }

    value_type get() const
    {
        // TODO(Q3): use traits to get unsigned type for data_type (if
        // not already)
        return static_cast<unsigned int>(*y() & mask_) >> shift_;
    }

    value_type get(difference_type d) const
    {
        // TODO(Q3): use traits to get unsigned type for data_type (if
        // not already)
        return static_cast<unsigned int>(*y(d) & mask_) >> shift_;
    }

    void set( value_type v ) const
    {
        const value_type pixel_value( (v << shift_) & mask_ );
        *y() = (*y() & ~mask_) | pixel_value;
    }

    void set( value_type v, difference_type d ) const
    {
        const value_type pixel_value( (v << shift_) & mask_ );
        *y(d) = (*y(d) & ~mask_) | pixel_value;
    }
};

template< typename Datatype,
          typename Valuetype,
          int      bits_per_pixel,
          bool     MsbFirst > class PackedPixelRowIterator
{
public:
    // no reference, no index_reference type here
    typedef Datatype                                    data_type;
    typedef Valuetype                                   value_type;
    typedef int                                         difference_type;
    typedef image_traverser_tag                         iterator_category;

    typedef typename remove_const<data_type>::type      mask_type;
    typedef data_type*                                  pointer;

    enum {
        /** The number of pixel within a single data_type value
         */
        num_intraword_positions=sizeof(data_type)*8/bits_per_pixel,
        /** Bit mask for one pixel (least significant bits)
         */
        bit_mask=~(~0 << bits_per_pixel)
    };

private:
    pointer         data_;
    mask_type       mask_;
    difference_type remainder_;

    void update_mask()
    {
        mask_ = get_mask<data_type, bits_per_pixel, MsbFirst>(remainder_);
    }

    void inc()
    {
        const difference_type newValue( remainder_ + 1 );
        const difference_type data_offset( newValue / num_intraword_positions );

        data_ += data_offset;
        remainder_ = newValue % num_intraword_positions;

        const mask_type shifted_mask(
            MsbFirst ?
            // TODO(Q3): use traits to get unsigned type for data_type
            // (if not already)
            static_cast<unsigned int>(mask_) >> bits_per_pixel :
            mask_ << bits_per_pixel );

        // data_offset is 0 for shifted mask, and 1 for wrapped-around mask
        mask_ = (1-data_offset)*shifted_mask + data_offset*(MsbFirst ?
                                                            bit_mask << bits_per_pixel*(num_intraword_positions-1) :
                                                            bit_mask);
    }

    void dec()
    {
        const difference_type newValue( remainder_ - 1 );
        const bool            isNegative( is_negative(newValue) );
        const difference_type newRemainder( newValue % num_intraword_positions );

        // calc  data_ += newValue / num_intraword_positions;
        //       remainder_ = newRemainder;
        // for newValue >= 0, and
        //       data_ += newValue / num_intraword_positions - 1;
        //       remainder_ = num_intraword_positions - newRemainder;
        // (to force remainder_ to be positive).
        // This is branch-free, if is_negative() is branch-free
        const difference_type data_offset( newValue / num_intraword_positions - isNegative );
        data_     += data_offset;
        remainder_ = newRemainder + isNegative*num_intraword_positions;

        const mask_type shifted_mask(
            MsbFirst ?
            mask_ << bits_per_pixel :
            // TODO(Q3): use traits to get unsigned type for data_type
            // (if not already)
            static_cast<unsigned int>(mask_) >> bits_per_pixel );

        // data_offset is 0 for shifted mask, and 1 for wrapped-around mask
        mask_ = (1-data_offset)*shifted_mask + data_offset*(MsbFirst ?
                                                            bit_mask :
                                                            bit_mask << bits_per_pixel*(num_intraword_positions-1));
    }

    bool equal( PackedPixelRowIterator const & rhs ) const
    {
        return rhs.data_ == data_ && rhs.remainder_ == remainder_;
    }

    bool less( PackedPixelRowIterator const & rhs ) const
    {
        return data_ == rhs.data_ ?
            (remainder_ < rhs.remainder_) :
            (data_ < rhs.data_);
    }

public:
    PackedPixelRowIterator() :
        data_(0),
        mask_( get_mask<data_type, bits_per_pixel, MsbFirst, difference_type>(0) ),
        remainder_(0)
    {}

    explicit PackedPixelRowIterator( pointer base ) :
        data_(base),
        mask_( get_mask<data_type, bits_per_pixel, MsbFirst, difference_type>(0) ),
        remainder_(0)
    {}

    PackedPixelRowIterator& operator+=( difference_type d )
    {
        const difference_type newValue( remainder_ + d );

        data_ += newValue / num_intraword_positions;
        remainder_ = newValue % num_intraword_positions;
        update_mask();

        return *this;
    }

    PackedPixelRowIterator& operator-=( difference_type d )
    {
        const difference_type newValue( remainder_ - d );
        const bool            isNegative( is_negative(newValue) );
        const difference_type newRemainder( newValue % num_intraword_positions );

        // calc  data_ += newValue / num_intraword_positions;
        //       remainder_ = newRemainder;
        // for newValue >= 0, and
        //       data_ += newValue / num_intraword_positions - 1;
        //       remainder_ = num_intraword_positions - newRemainder;
        // (to force remainder_ to be positive).
        // This is branch-free, if is_negative() is branch-free
        data_     += newValue / num_intraword_positions - isNegative;
        remainder_ = newRemainder + isNegative*(num_intraword_positions - 2*newRemainder);
        update_mask();

        return *this;
    }

    PackedPixelRowIterator operator+( difference_type d )
    {
        PackedPixelRowIterator res(*this);
        res += d;
        return res;
    }

    PackedPixelRowIterator operator-( difference_type d )
    {
        PackedPixelRowIterator res(*this);
        res -= d;
        return res;
    }

    PackedPixelRowIterator& operator++()
    {
        inc();
        return *this;
    }

    PackedPixelRowIterator& operator--()
    {
        dec();
        return *this;
    }

    PackedPixelRowIterator operator++(int)
    {
        PackedPixelRowIterator res(*this);
        res.inc();
        return res;
    }

    PackedPixelRowIterator operator--(int)
    {
        PackedPixelRowIterator res(*this);
        res.dec();
        return res;
    }

    bool operator==(PackedPixelRowIterator const & rhs) const
    {
        return equal( rhs );
    }

    bool operator!=(PackedPixelRowIterator const & rhs) const
    {
        return !equal( rhs );
    }

    bool operator<(PackedPixelRowIterator const & rhs) const
    {
        return less(rhs);
    }

    bool operator<=(PackedPixelRowIterator const & rhs) const
    {
        return !less(rhs);
    }

    bool operator>(PackedPixelRowIterator const & rhs) const
    {
        return rhs.less(*this);
    }

    bool operator>=(PackedPixelRowIterator const & rhs) const
    {
        return !rhs.less(*this);
    }

    difference_type operator-(PackedPixelRowIterator const & rhs) const
    {
        return (data_ - rhs.data_)*num_intraword_positions + (remainder_ - rhs.remainder_);
    }

    value_type get() const
    {
        // TODO(Q3): use traits to get unsigned type for data_type (if
        // not already)
        return static_cast<unsigned int>(*data_ & mask_) >>
            get_shift<num_intraword_positions, bits_per_pixel, MsbFirst>(remainder_);
    }

    value_type get(difference_type d) const
    {
        PackedPixelRowIterator tmp(*this);
        tmp += d;
        return tmp.get();
    }

    void set( value_type v ) const
    {
        const value_type pixel_value(
            (v <<
             get_shift<num_intraword_positions, bits_per_pixel, MsbFirst>(remainder_))
            & mask_ );
        *data_ = (*data_ & ~mask_) | pixel_value;
    }

    void set( value_type v, difference_type d ) const
    {
        PackedPixelRowIterator tmp(*this);
        tmp += d;
        tmp.set(v);
    }
};

template< typename Datatype,
          typename Valuetype,
          int      bits_per_pixel,
          bool     MsbFirst > class PackedPixelIterator
{
public:
    // no reference, no index_reference type here
    typedef Datatype                                    data_type;
    typedef Valuetype                                   value_type;
    typedef vigra::Diff2D                               difference_type;
    typedef image_traverser_tag                         iterator_category;
    typedef PackedPixelRowIterator<data_type,
                                   value_type,
                                   bits_per_pixel,
                                   MsbFirst>            row_iterator;
    typedef PackedPixelColumnIterator<data_type,
                                      value_type,
                                      bits_per_pixel,
                                      MsbFirst>         column_iterator;

    typedef data_type*                                  pointer;
    typedef int                                         MoveX;
    typedef StridedArrayIterator< data_type >           MoveY;

    enum {
        /** The number of pixel within a single data_type value
         */
        num_intraword_positions=sizeof(data_type)*8/bits_per_pixel,
        /** Bit mask for one pixel (least significant bits)
         */
        bit_mask=~(~0 << bits_per_pixel)
    };

    // TODO(F2): direction of iteration (ImageIterator can be made to
    // run backwards)

private:
    pointer current() const
    {
        return y() + (x / num_intraword_positions);
    }

    pointer current(int dx, int dy) const
    {
        return y(dy) + ((x+dx)/num_intraword_positions);
    }

    bool equal(PackedPixelIterator const & rhs) const
    {
        return (x == rhs.x) && (y == rhs.y);
    }

public:
    PackedPixelIterator() :
        x(0),
        y(0)
    {}

    PackedPixelIterator(pointer base, int ystride) :
        x(0),
        y(ystride,base)
    {}

    bool operator==(PackedPixelIterator const & rhs) const
    {
        return equal(rhs);
    }

    bool operator!=(PackedPixelIterator const & rhs) const
    {
        return !equal(rhs);
    }

    difference_type operator-(PackedPixelIterator const & rhs) const
    {
        return difference_type(x - rhs.x, y - rhs.y);
    }

    MoveX x;
    MoveY y;

    PackedPixelIterator & operator+=(difference_type const & s)
    {
        x += s.x;
        y += s.y;
        return *this;
    }

    PackedPixelIterator & operator-=(difference_type const & s)
    {
        x -= s.x;
        y -= s.y;
        return *this;
    }

    PackedPixelIterator operator+(difference_type const & s) const
    {
        PackedPixelIterator ret(*this);
        ret += s;
        return ret;
    }

    PackedPixelIterator operator-(difference_type const & s) const
    {
        PackedPixelIterator ret(*this);
        ret -= s;
        return ret;
    }

    row_iterator rowIterator() const
    {
        return row_iterator(current());
    }

    column_iterator columnIterator() const
    {
        return column_iterator(MoveY(y,
                                     x / num_intraword_positions),
                               x % num_intraword_positions);
    }

    value_type get() const
    {
        const int remainder( x() % num_intraword_positions );

        // TODO(Q3): use traits to get unsigned type for data_type (if
        // not already)
        return (static_cast<unsigned int>(*current() &
                                          get_mask<data_type, bits_per_pixel, MsbFirst>(remainder))
                >> (MsbFirst ?
                    (num_intraword_positions - remainder) :
                    remainder));
    }

    value_type get(difference_type const & d) const
    {
        const int remainder( x(d.x) % num_intraword_positions );

        // TODO(Q3): use traits to get unsigned type for data_type (if
        // not already)
        return (static_cast<unsigned int>(*current(d.x,d.y) &
                                          get_mask<data_type, bits_per_pixel, MsbFirst>(remainder))
                >> get_shift<num_intraword_positions, bits_per_pixel, MsbFirst>(remainder));
    }

    void set( value_type v ) const
    {
        const int remainder( x() % num_intraword_positions );
        const int mask( get_mask<data_type, bits_per_pixel, MsbFirst>(remainder) );
        const value_type pixel_value(
            (v <<
             get_shift<num_intraword_positions, bits_per_pixel, MsbFirst>(remainder))
            & mask );
        pointer p = current();
        *p = (*p & ~mask) | pixel_value;
    }

    void set( value_type v, difference_type const & d ) const
    {
        const int remainder( x(d.x) % num_intraword_positions );
        const int mask( get_mask<data_type, bits_per_pixel, MsbFirst>(remainder) );
        const value_type pixel_value(
            (v <<
             get_shift<num_intraword_positions, bits_per_pixel, MsbFirst>(remainder))
             & mask );
        pointer p = current(d.x,d.y);
        *p = (*p & ~mask) | pixel_value;
    }
};


/** Access (possibly packed-pixel) data via palette indirection
 */
template< typename Valuetype, typename Datatype > class PaletteImageAccessor
{
  public:
    typedef Valuetype                                   value_type;
    typedef Datatype                                    data_type;
    typedef typename remove_const<data_type>::type      count_type;


private:
    const BitmapColor* palette;
    count_type         num_entries;

    double norm( BitmapColor const& rLHS,
                 BitmapColor const& rRHS ) const
    {
        // convert RGBValue's linear space to a normed linear space
        return sqrt(
            vigra::sq(rLHS.GetRed()-rRHS.GetRed()) +
            vigra::sq(rLHS.GetGreen()-rRHS.GetGreen()) +
            vigra::sq(rLHS.GetBlue()-rRHS.GetBlue()) );
    }

    data_type find_best_match(value_type const& v) const
    {
        // TODO(F3): not generic!!!
        const BitmapColor aTmpCol(v.red(),
                                  v.green(),
                                  v.blue());

        // TODO(P3): use table-based/octree approach here!
        const BitmapColor* best_entry;
        const BitmapColor* palette_end( palette+num_entries );
        if( (best_entry=std::find( palette, palette_end, aTmpCol)) != palette_end )
            return best_entry-palette;

        // TODO(F3): HACK. Need palette traits, and an error function
        // here. We blatantly assume value_type is a normed linear
        // space.
        const BitmapColor* curr_entry( palette );
        best_entry = curr_entry;
        while( curr_entry != palette_end )
        {
            if( norm(*curr_entry,*best_entry) > norm(*curr_entry,aTmpCol) )
                best_entry = curr_entry;

            ++curr_entry;
        }

        return best_entry-palette;
    }

    value_type toCol( BitmapColor const& rCol ) const
    {
        return value_type(rCol.GetRed(),rCol.GetGreen(),rCol.GetBlue());
    }

public:
    PaletteImageAccessor() :
        palette(0),
        num_entries(0)
    {}

    PaletteImageAccessor( const BitmapColor* pPalette,
                          data_type          entries ) :
        palette(pPalette),
        num_entries(entries)
    {}

    template< class Iterator >
    value_type operator()(Iterator const& i) const { return toCol(palette[i.get()]); }
    value_type operator()(data_type const* i) const { return toCol(palette[*i]); }

    template< class Iterator, class Difference >
    value_type operator()(Iterator const& i, Difference const& diff) const
    {
        return toCol(palette[i.get(diff)]);
    }

    template< typename V, class Iterator >
    void set(V const& value, Iterator const& i) const
    {
        i.set(
            find_best_match(
                vigra::detail::RequiresExplicitCast<value_type>::cast(value) ));
    }

    template< typename V, class Iterator, class Difference >
    void set(V const& value, Iterator const& i, Difference const& diff) const
    {
        i.set(
            find_best_match(
                vigra::detail::RequiresExplicitCast<value_type>::cast(value)),
            diff );
    }
};

}


class TestApp : public Application
{
public:
    virtual void Main();
    virtual USHORT  Exception( USHORT nError );
};

class TestWindow : public Dialog
{
    public:
        TestWindow() : Dialog( (Window *) NULL )
        {
            SetText( rtl::OUString::createFromAscii( "VIGRA test" ) );
            SetSizePixel( Size( 1024, 500 ) );
            EnablePaint( true );
            Show();
        }
        virtual ~TestWindow() {}
        virtual void MouseButtonUp( const MouseEvent& rMEvt )
        {
            //TODO: do something cool
            EndDialog();
        }
        virtual void Paint( const Rectangle& rRect );
};

void TestWindow::Paint( const Rectangle& rRect )
{
    {
        const basegfx::B2ISize aSize(11,11);
        basebmp::BitmapDeviceSharedPtr pDevice( basebmp::createBitmapDevice( aSize,
                                                                             true,
                                                                             basebmp::Format::THIRTYTWO_BIT_TC_MASK ));

        const basegfx::B2IPoint aPt1(1,1);
        const basegfx::B2IPoint aPt2(1,9);
        const basebmp::Color aCol(0);
        pDevice->clear( aCol );
        const basebmp::Color aCol2(0xFFFFFFFF);
        pDevice->drawLine( aPt1, aPt2, aCol2, basebmp::DrawMode_PAINT );
    }

    enum{ srcBitDepth=1, dstBitDepth=4 };
    typedef vigra::RGBValue< sal_uInt8 > RGBVal;

    Bitmap aSourceBitmap( Size(100,100),
                          srcBitDepth );
    DrawBitmap( Point(0,350), aSourceBitmap );

    // Fill bitmap with rhombus
    {
        ScopedBitmapWriteAccess pWriteAccess( aSourceBitmap.AcquireWriteAccess(),
                                              aSourceBitmap );
        pWriteAccess->SetFillColor(0xFF0000);
        Polygon aPoly(5);
        aPoly[0] = Point(50,0);
        aPoly[1] = Point(100,50);
        aPoly[2] = Point(50,100);
        aPoly[3] = Point(0,50);
        aPoly[4] = Point(50,0);
        pWriteAccess->FillPolygon(aPoly);
    }

    Bitmap aDestBitmap( Size(300,300),
                        dstBitDepth );

    {
        ScopedBitmapReadAccess  pReadAccess( aSourceBitmap.AcquireReadAccess(),
                                             aSourceBitmap );
        ScopedBitmapWriteAccess pWriteAccess( aDestBitmap.AcquireWriteAccess(),
                                              aDestBitmap );

        const sal_Int32     nSrcWidth( pReadAccess->Width() );
        const sal_Int32     nSrcHeight( pReadAccess->Height() );
        const bool          bSrcTopDown( pReadAccess->IsTopDown() );
        const sal_uInt32    nSrcScanlineFormat( pReadAccess->GetScanlineFormat() );
        const sal_uInt32    nSrcScanlineStride( pReadAccess->GetScanlineSize() );
        const sal_uInt16    nSrcBitCount( pReadAccess->GetBitCount() );
        const sal_uInt16    nSrcPaletteEntries( pReadAccess->GetPaletteEntryCount() );
        const BitmapColor*  pSrcPalette( &pReadAccess->GetPalette()[0] );
        const BYTE*         pSrcBuffer = pReadAccess->GetBuffer();

        const sal_Int32     nDstWidth( pWriteAccess->Width() );
        const sal_Int32     nDstHeight( pWriteAccess->Height() );
        const bool          bDstTopDown( pWriteAccess->IsTopDown() );
        const sal_uInt32    nDstScanlineFormat( pWriteAccess->GetScanlineFormat() );
        const sal_uInt32    nDstScanlineStride( pWriteAccess->GetScanlineSize() );
        const sal_uInt16    nDstBitCount( pWriteAccess->GetBitCount() );
        const sal_uInt16    nDstPaletteEntries( pWriteAccess->GetPaletteEntryCount() );
        const BitmapColor*  pDstPalette( &pWriteAccess->GetPalette()[0] );
        BYTE*               pDstBuffer = pWriteAccess->GetBuffer();

        typedef PackedPixelIterator< const BYTE,
            BYTE,
            srcBitDepth,
            true > SrcPixelIterator;
        typedef PaletteImageAccessor< RGBVal, BYTE > SrcImageAccessor;

        typedef PackedPixelIterator< BYTE,
            BYTE,
            dstBitDepth,
            true > DstPixelIterator;
        typedef PaletteImageAccessor< RGBVal, BYTE > DstImageAccessor;

        const SrcPixelIterator aStartImage( pSrcBuffer,nSrcScanlineStride );
        const SrcPixelIterator aEndImage( aStartImage + vigra::Diff2D(nSrcWidth,nSrcHeight) );

        const DstPixelIterator aStartDstImage( pDstBuffer,nDstScanlineStride );
        const DstPixelIterator aEndDstImage( aStartDstImage + vigra::Diff2D(nDstWidth,nDstHeight) );

        vigra::resizeImageNoInterpolation(
            //vigra::resizeImageLinearInterpolation(
            vigra::make_triple(
                aStartImage,
                aEndImage,
                SrcImageAccessor(
                    pSrcPalette,
                    nSrcPaletteEntries)),
            vigra::make_triple(
                aStartDstImage,
                aEndDstImage,
                DstImageAccessor(
                    pDstPalette,
                    nDstPaletteEntries)));
    }

    DrawBitmap( Point(), aDestBitmap );


    Bitmap aDestBitmap2( Size(300,300),
                        dstBitDepth );

    {
        ScopedBitmapReadAccess  pReadAccess( aSourceBitmap.AcquireReadAccess(),
                                             aSourceBitmap );
        ScopedBitmapWriteAccess pWriteAccess( aDestBitmap2.AcquireWriteAccess(),
                                              aDestBitmap2 );

        const sal_Int32     nSrcWidth( pReadAccess->Width() );
        const sal_Int32     nSrcHeight( pReadAccess->Height() );
        const bool          bSrcTopDown( pReadAccess->IsTopDown() );
        const sal_uInt32    nSrcScanlineFormat( pReadAccess->GetScanlineFormat() );
        const sal_uInt32    nSrcScanlineStride( pReadAccess->GetScanlineSize() );
        const sal_uInt16    nSrcBitCount( pReadAccess->GetBitCount() );
        const sal_uInt16    nSrcPaletteEntries( pReadAccess->GetPaletteEntryCount() );
        const BitmapColor*  pSrcPalette( &pReadAccess->GetPalette()[0] );
        const BYTE*         pSrcBuffer = pReadAccess->GetBuffer();

        const sal_Int32     nDstWidth( pWriteAccess->Width() );
        const sal_Int32     nDstHeight( pWriteAccess->Height() );
        const bool          bDstTopDown( pWriteAccess->IsTopDown() );
        const sal_uInt32    nDstScanlineFormat( pWriteAccess->GetScanlineFormat() );
        const sal_uInt32    nDstScanlineStride( pWriteAccess->GetScanlineSize() );
        const sal_uInt16    nDstBitCount( pWriteAccess->GetBitCount() );
        const sal_uInt16    nDstPaletteEntries( pWriteAccess->GetPaletteEntryCount() );
        const BitmapColor*  pDstPalette( &pWriteAccess->GetPalette()[0] );
        BYTE*               pDstBuffer = pWriteAccess->GetBuffer();

        typedef vigra::RGBValue< sal_uInt8 > RGBVal;
        typedef PackedPixelIterator< const BYTE,
            BYTE,
            srcBitDepth,
            true > SrcPixelIterator;
        typedef PaletteImageAccessor< RGBVal,BYTE > SrcImageAccessor;

        typedef PackedPixelIterator< BYTE,
            BYTE,
            dstBitDepth,
            true > DstPixelIterator;
        typedef PaletteImageAccessor< RGBVal,BYTE > DstImageAccessor;

        const SrcPixelIterator aStartImage( pSrcBuffer,nSrcScanlineStride );
        const SrcPixelIterator aEndImage( aStartImage + vigra::Diff2D(nSrcWidth,nSrcHeight) );

        const DstPixelIterator aStartDstImage( pDstBuffer,nDstScanlineStride );
        const DstPixelIterator aEndDstImage( aStartDstImage + vigra::Diff2D(nDstWidth,nDstHeight) );

        vigra::resizeImageLinearInterpolation(
            vigra::make_triple(
                aStartImage,
                aEndImage,
                SrcImageAccessor(
                    pSrcPalette,
                    nSrcPaletteEntries)),
            vigra::make_triple(
                aStartDstImage,
                aEndDstImage,
                DstImageAccessor(
                    pDstPalette,
                    nDstPaletteEntries)));
    }

    DrawBitmap( Point(310,0), aDestBitmap2 );


    Bitmap aDestBitmap3( Size(300,300),
                         24 );

    {
        ScopedBitmapReadAccess  pReadAccess( aSourceBitmap.AcquireReadAccess(),
                                             aSourceBitmap );
        ScopedBitmapWriteAccess pWriteAccess( aDestBitmap3.AcquireWriteAccess(),
                                              aDestBitmap3 );

        const sal_Int32     nSrcWidth( pReadAccess->Width() );
        const sal_Int32     nSrcHeight( pReadAccess->Height() );
        const bool          bSrcTopDown( pReadAccess->IsTopDown() );
        const sal_uInt32    nSrcScanlineFormat( pReadAccess->GetScanlineFormat() );
        const sal_uInt32    nSrcScanlineStride( pReadAccess->GetScanlineSize() );
        const sal_uInt16    nSrcBitCount( pReadAccess->GetBitCount() );
        const sal_uInt16    nSrcPaletteEntries( pReadAccess->GetPaletteEntryCount() );
        const BitmapColor*  pSrcPalette( &pReadAccess->GetPalette()[0] );
        const BYTE*         pSrcBuffer = pReadAccess->GetBuffer();

        const sal_Int32     nDstWidth( pWriteAccess->Width() );
        const sal_Int32     nDstHeight( pWriteAccess->Height() );
        const bool          bDstTopDown( pWriteAccess->IsTopDown() );
        const sal_uInt32    nDstScanlineFormat( pWriteAccess->GetScanlineFormat() );
        const sal_uInt32    nDstScanlineStride( pWriteAccess->GetScanlineSize() );
        const sal_uInt16    nDstBitCount( pWriteAccess->GetBitCount() );
        BYTE*               pDstBuffer = pWriteAccess->GetBuffer();

        vigra::BasicImageView< RGBVal > aDestImage(
            reinterpret_cast<RGBVal*>(pDstBuffer),
            nDstWidth,
            nDstHeight,
            0 /*nDstScanlineStride should be an integer multiple of 4*/ );

        typedef PackedPixelIterator< const BYTE,
            BYTE,
            srcBitDepth,
            true > SrcPixelIterator;
        typedef PaletteImageAccessor< RGBVal,BYTE > SrcImageAccessor;

        const SrcPixelIterator aStartImage( pSrcBuffer,nSrcScanlineStride );
        const SrcPixelIterator aEndImage( aStartImage + vigra::Diff2D(nSrcWidth,nSrcHeight) );

        // resize bitmap
        vigra::resizeImageSplineInterpolation(
            vigra::make_triple(
                aStartImage,
                aEndImage,
                SrcImageAccessor(
                    pSrcPalette,
                    nSrcPaletteEntries)),
            vigra::destImageRange(aDestImage));

        // add ellipse to bitmap
        basegfx::B2DPolygon aPoly(
            basegfx::tools::createPolygonFromEllipse(
                basegfx::B2DPoint(150,150),
                50, 80 ));
        aPoly = basegfx::tools::adaptiveSubdivideByCount(aPoly);
        makeRenderer( basegfx::B2DPolyPolygon( aPoly ),
                      RGBVal(0,0xFF,0),
                      RGBVal(0xFF,0xFF,0xFF),
                      vigra::destImageRange(aDestImage) )->rasterConvert(
                          basegfx::FillRule_NONZERO_WINDING_NUMBER );
    }

    DrawBitmap( Point(620,0), aDestBitmap3 );
    DrawBitmap( Point(310,350), aSourceBitmap );
}

USHORT TestApp::Exception( USHORT nError )
{
    switch( nError & EXC_MAJORTYPE )
    {
        case EXC_RSCNOTLOADED:
            Abort( String::CreateFromAscii( "Error: could not load language resources.\nPlease check your installation.\n" ) );
            break;
    }
    return 0;
}

void TestApp::Main()
{
    //-------------------------------------------------
    // create the global service-manager
    //-------------------------------------------------
    uno::Reference< lang::XMultiServiceFactory > xFactory;
    try
    {
        uno::Reference< uno::XComponentContext > xCtx = ::cppu::defaultBootstrap_InitialComponentContext();
        xFactory = uno::Reference< lang::XMultiServiceFactory >(  xCtx->getServiceManager(),
                                                                  uno::UNO_QUERY );
        if( xFactory.is() )
            ::comphelper::setProcessServiceFactory( xFactory );
    }
    catch( uno::Exception& )
    {
    }

    if( !xFactory.is() )
    {
        OSL_TRACE( "Could not bootstrap UNO, installation must be in disorder. Exiting.\n" );
        exit( 1 );
    }

    // Create UCB.
    uno::Sequence< uno::Any > aArgs( 2 );
    aArgs[ 0 ] <<= rtl::OUString::createFromAscii( UCB_CONFIGURATION_KEY1_LOCAL );
    aArgs[ 1 ] <<= rtl::OUString::createFromAscii( UCB_CONFIGURATION_KEY2_OFFICE );
    ::ucb::ContentBroker::initialize( xFactory, aArgs );

    TestWindow pWindow;
    pWindow.Execute();

    // clean up UCB
    ::ucb::ContentBroker::deinitialize();
}

TestApp aDemoApp;
