/*************************************************************************
 *
 *  $RCSfile: LinearRegressionCurveCalculator.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: bm $ $Date: 2003-12-17 16:43:14 $
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
#include "LinearRegressionCurveCalculator.hxx"
#include "macros.hxx"
#include "RegressionCalculationHelper.hxx"

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

using namespace ::com::sun::star;
using namespace ::drafts::com::sun::star;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

namespace chart
{

LinearRegressionCurveCalculator::LinearRegressionCurveCalculator() :
        m_fSlope( 0.0 ),
        m_fIntercept( 0.0 ),
        m_fCorrelationCoeffitient( 0.0 )
{
    ::rtl::math::setNan( & m_fSlope );
    ::rtl::math::setNan( & m_fIntercept );
    ::rtl::math::setNan( & m_fCorrelationCoeffitient );
}

LinearRegressionCurveCalculator::~LinearRegressionCurveCalculator()
{}

// ____ XRegressionCurve ____
void SAL_CALL LinearRegressionCurveCalculator::recalculateRegression(
    const uno::Sequence< double >& aXValues,
    const uno::Sequence< double >& aYValues )
    throw (uno::RuntimeException)
{
    RegressionCalculationHelper::tDoubleVectorPair aValues(
        RegressionCalculationHelper::cleanup(
            aXValues, aYValues,
            RegressionCalculationHelper::isValid()));

    const size_t nMax = aValues.first.size();
    if( nMax == 0 )
    {
        ::rtl::math::setNan( & m_fSlope );
        ::rtl::math::setNan( & m_fIntercept );
        ::rtl::math::setNan( & m_fCorrelationCoeffitient );
        return;
    }

    const double fN = static_cast< double >( nMax );
    double fSumX = 0.0, fSumY = 0.0, fSumXSq = 0.0, fSumYSq = 0.0, fSumXY = 0.0;
    for( size_t i = 0; i < nMax; ++i )
    {
        fSumX   += aValues.first[i];
        fSumY   += aValues.second[i];
        fSumXSq += aValues.first[i]  * aValues.first[i];
        fSumYSq += aValues.second[i] * aValues.second[i];
        fSumXY  += aValues.first[i]  * aValues.second[i];
    }

    m_fSlope = (fN * fSumXY - fSumX * fSumY) / ( fN * fSumXSq - fSumX * fSumX );
    m_fIntercept = (fSumY - m_fSlope * fSumX) / fN;

    m_fCorrelationCoeffitient = ( fN * fSumXY - fSumX * fSumY ) /
        sqrt( ( fN * fSumXSq - fSumX * fSumX ) *
              ( fN * fSumYSq - fSumY * fSumY ) );
}

double SAL_CALL LinearRegressionCurveCalculator::getCurveValue( double x )
    throw (lang::IllegalArgumentException,
           uno::RuntimeException)
{
    double fResult;
    ::rtl::math::setNan( & fResult );

    if( ! ( ::rtl::math::isNan( m_fSlope ) ||
            ::rtl::math::isNan( m_fIntercept )))
    {
        fResult = m_fSlope * x + m_fIntercept;
    }

    return fResult;
}

double SAL_CALL LinearRegressionCurveCalculator::getCorrelationCoefficient()
    throw (uno::RuntimeException)
{
    return m_fCorrelationCoeffitient;
}

OUString SAL_CALL LinearRegressionCurveCalculator::getRepresentation()
    throw (uno::RuntimeException)
{
    OUStringBuffer aBuf( C2U( "f(x) = " ));

    bool bHaveSlope = false;

    if( m_fSlope != 0.0 )
    {
        aBuf.append( NUMBER_TO_STR( m_fSlope ));
        aBuf.append( sal_Unicode( ' ' ));
        aBuf.append( sal_Unicode( 0x00b7 ));
        aBuf.append( sal_Unicode( ' ' ));
        aBuf.append( sal_Unicode( 'x' ));
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
