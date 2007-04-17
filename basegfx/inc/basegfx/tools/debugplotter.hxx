/*************************************************************************
 *
 *  $RCSfile: debugplotter.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-17 13:54:19 $
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

#ifndef _BGFX_TOOLS_DEBUGPLOTTER_HXX
#define _BGFX_TOOLS_DEBUGPLOTTER_HXX

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif
#ifndef _BGFX_VECTOR_B2DVECTOR_HXX
#include <basegfx/vector/b2dvector.hxx>
#endif
#ifndef _BGFX_RANGE_B2DRANGE_HXX
#include <basegfx/range/b2drange.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif
#ifndef _BGFX_POLYGON_B2DPOLYPOLYGON_HXX
#include <basegfx/polygon/b2dpolypolygon.hxx>
#endif

#ifndef _RTL_STRING_HXX_
#include <rtl/string.hxx>
#endif

#include <boost/utility.hpp> // for noncopyable
#include <vector>
#include <utility>

#include <iostream>


namespace basegfx
{
    class B2DCubicBezier;

    /** Generates debug output for various basegfx data types.

        Use this class to produce debug (trace) output for various
        basegfx geometry data types. By default, this class outputs
        via OSL_TRACE (i.e. to stderr), and uses the gnuplot output
        format.

        To be able to generate one coherent block of output, this
        class delays actual writing to its destructor
     */
    class DebugPlotter : private ::boost::noncopyable
    {
    public:
        /** Create new debug output object

            @param pTitle
            Title of the debug output, will appear in trace output
         */
        explicit DebugPlotter( const sal_Char* pTitle );

        /** Create new debug output object

            @param pTitle
            Title of the debug output, will appear in trace output

            @param rOutputStream
            Stream to write output to. Must stay valid over the
            lifetime of this object!
         */
        DebugPlotter( const sal_Char* pTitle,
                      ::std::ostream& rOutputStream );

        ~DebugPlotter();

        void plot( const B2DPoint&          rPoint,
                   const sal_Char*          pTitle );
        void plot( const B2DVector&         rVec,
                   const sal_Char*          pTitle );
        void plot( const B2DCubicBezier&    rBezier,
                   const sal_Char*          pTitle );
        void plot( const B2DRange&          rRange,
                   const sal_Char*          pTitle );
        void plot( const B2DPolygon&        rPoly,
                   const sal_Char*          pTitle );
        void plot( const B2DPolyPolygon&    rPoly,
                   const sal_Char*          pTitle );

    private:
        void print( const sal_Char* );

        ::rtl::OString                                              maTitle;
        ::std::vector< ::std::pair< B2DPoint, ::rtl::OString > >    maPoints;
        ::std::vector< ::std::pair< B2DVector, ::rtl::OString > >   maVectors;
        ::std::vector< ::std::pair< B2DRange, ::rtl::OString > >    maRanges;
        ::std::vector< ::std::pair< B2DPolygon, ::rtl::OString > >  maPolygons;

        ::std::ostream*                                             mpOutputStream;
    };
}

#endif /* _BGFX_TOOLS_DEBUGPLOTTER_HXX */
