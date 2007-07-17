/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: animationfunction.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 15:02:44 $
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

#ifndef INCLUDED_SLIDESHOW_ANIMATIONFUNCTION_HXX
#define INCLUDED_SLIDESHOW_ANIMATIONFUNCTION_HXX

#include <sal/config.h>
#include <boost/shared_ptr.hpp>


/* Definition of AnimationFunction interface */

namespace slideshow
{
    namespace internal
    {
        /** Interface describing an abstract animation function.

            Use this interface to model time-dependent animation
            functions of one variable.
         */
        class AnimationFunction
        {
        public:
            virtual ~AnimationFunction() {}

            /** Operator to calculate function value.

                This method calculates the function value for the
                given time instant t.

                @param t
                Current time instant, must be in the range [0,1]

                @return the function value, typically in relative
                user coordinate space ([0,1] range).
             */
            virtual double operator()( double t ) const = 0;

        };

        typedef ::boost::shared_ptr< AnimationFunction > AnimationFunctionSharedPtr;

    }
}

#endif /* INCLUDED_SLIDESHOW_ANIMATIONFUNCTION_HXX */
