/*************************************************************************
 *
 *  $RCSfile: LogarithmicRegressionCurveCalculator.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: fa $ $Date: 2004-03-08 16:03:50 $
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
 *  Copyright: 2003 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#include "LogarithmicRegressionCurveCalculator.hxx"
#include "macros.hxx"
#include "RegressionCalculationHelper.hxx"

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

using namespace ::com::sun::star;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

namespace chart
{

LogarithmicRegressionCurveCalculator::LogarithmicRegressionCurveCalculator() :
        m_fSlope( 0.0 ),
        m_fIntercept( 0.0 ),
        m_fCorrelationCoeffitient( 0.0 )
{
    ::rtl::math::setNan( & m_fSlope );
    ::rtl::math::setNan( & m_fIntercept );
    ::rtl::math::setNan( & m_fCorrelationCoeffitient );
}

LogarithmicRegressionCurveCalculator::~LogarithmicRegressionCurveCalculator()
{}

// ____ XRegressionCurve ____
void SAL_CALL LogarithmicRegressionCurveCalculator::recalculateRegression(
    const uno::Sequence< double >& aXValues,
    const uno::Sequence< double >& aYValues )
    throw (uno::RuntimeException)
{
    RegressionCalculationHelper::tDoubleVectorPair aValues(
        RegressionCalculationHelper::cleanup(
            aXValues, aYValues,
            RegressionCalculationHelper::isValidAndXPositive()));

    const size_t nMax = aValues.first.size();
    if( nMax == 0 )
    {
        ::rtl::math::setNan( & m_fSlope );
        ::rtl::math::setNan( & m_fIntercept );
        ::rtl::math::setNan( & m_fCorrelationCoeffitient );
        return;
    }

    double fAverageX = 0.0, fAverageY = 0.0;
    size_t i = 0;
    for( i = 0; i < nMax; ++i )
    {
        fAverageX += log( aValues.first[i] );
        fAverageY += aValues.second[i];
    }

    const double fN = static_cast< double >( nMax );
    fAverageX /= fN;
    fAverageY /= fN;

    double fQx = 0.0, fQy = 0.0, fQxy = 0.0;
    for( i = 0; i < nMax; ++i )
    {
        double fDeltaX = log( aValues.first[i] ) - fAverageX;
        double fDeltaY = aValues.second[i] - fAverageY;

        fQx  += fDeltaX * fDeltaX;
        fQy  += fDeltaY * fDeltaY;
        fQxy += fDeltaX * fDeltaY;
    }

    m_fSlope = fQxy / fQx;
    m_fIntercept = fAverageY - m_fSlope * fAverageX;
    m_fCorrelationCoeffitient = fQxy / sqrt( fQx * fQy );
}

double SAL_CALL LogarithmicRegressionCurveCalculator::getCurveValue( double x )
    throw (lang::IllegalArgumentException,
           uno::RuntimeException)
{
    double fResult;
    ::rtl::math::setNan( & fResult );

    if( ! ( ::rtl::math::isNan( m_fSlope ) ||
            ::rtl::math::isNan( m_fIntercept )))
    {
        fResult = m_fSlope * log( x ) + m_fIntercept;
    }

    return fResult;
}

double SAL_CALL LogarithmicRegressionCurveCalculator::getCorrelationCoefficient()
    throw (uno::RuntimeException)
{
    return m_fCorrelationCoeffitient;
}

OUString SAL_CALL LogarithmicRegressionCurveCalculator::getRepresentation()
    throw (uno::RuntimeException)
{
    OUStringBuffer aBuf( C2U( "f(x) = " ));

    bool bHaveSlope = false;

    if( m_fSlope != 0.0 )
    {
        if( ! ::rtl::math::approxEqual( m_fSlope, 1.0 ))
        {
            aBuf.append( NUMBER_TO_STR( m_fSlope ));
            aBuf.append( sal_Unicode( ' ' ));
            aBuf.append( sal_Unicode( 0x00b7 ));
            aBuf.append( sal_Unicode( ' ' ));
        }
        aBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "log(x)" ));
        bHaveSlope = true;
    }

    if( m_fIntercept != 0.0 )
    {
        if( ! bHaveSlope )
        {
            aBuf.append( NUMBER_TO_STR( m_fIntercept ));
        }
        else
        {
            if( m_fIntercept < 0.0 )
            {
                aBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM( " - " ));
                aBuf.append( NUMBER_TO_STR( fabs( m_fIntercept )));
            }
            else
            {
                aBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM( " + " ));
                aBuf.append( NUMBER_TO_STR( m_fIntercept ));
            }
        }
    }

    return aBuf.makeStringAndClear();
}

} //  namespace chart
