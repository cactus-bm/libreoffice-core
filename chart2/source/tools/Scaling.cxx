/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Scaling.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 13:29:41 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_chart2.hxx"
#ifndef _CHART2_SCALING_HXX
#include "Scaling.hxx"
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#include "com/sun/star/uno/RuntimeException.hpp"

namespace
{

static const ::rtl::OUString lcl_aServiceName_Logarithmic(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart2.LogarithmicScaling" ));
static const ::rtl::OUString lcl_aServiceName_Exponential(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart2.ExponentialScaling" ));
static const ::rtl::OUString lcl_aServiceName_Linear(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart2.LinearScaling" ));
static const ::rtl::OUString lcl_aServiceName_Power(
    RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.chart2.PowerScaling" ));
}

//.............................................................................
namespace chart
{
//.............................................................................
using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;

LogarithmicScaling::LogarithmicScaling() :
        m_fBase( 10.0 ),
        m_fLogOfBase( log( 10.0 ) )
{
}

LogarithmicScaling::LogarithmicScaling( double fBase ) :
        m_fBase( fBase ),
        m_fLogOfBase( log( fBase ) )
{
}

LogarithmicScaling::~LogarithmicScaling()
{
}

    double SAL_CALL
LogarithmicScaling::doScaling( double value )
    throw (uno::RuntimeException)
{
    double fResult;
    if( ::rtl::math::isNan( value ) || ::rtl::math::isInf( value ) )
        ::rtl::math::setNan( & fResult );
    else
        fResult = log( value ) / m_fLogOfBase;
    return fResult;
}

    uno::Reference< XScaling > SAL_CALL
LogarithmicScaling::getInverseScaling()
    throw (uno::RuntimeException)
{
    return new ExponentialScaling( m_fBase );
}

    ::rtl::OUString SAL_CALL
LogarithmicScaling::getServiceName()
    throw (uno::RuntimeException)
{
    return lcl_aServiceName_Logarithmic;
}

// ----------------------------------------

ExponentialScaling::ExponentialScaling() :
        m_fBase( 10.0 )
{
}

ExponentialScaling::ExponentialScaling( double fBase ) :
        m_fBase( fBase )
{
}

ExponentialScaling::~ExponentialScaling()
{
}

    double SAL_CALL
ExponentialScaling::doScaling( double value )
    throw (uno::RuntimeException)
{
    double fResult;
    if( ::rtl::math::isNan( value ) || ::rtl::math::isInf( value ) )
        ::rtl::math::setNan( & fResult );
    else
        fResult = pow( m_fBase, value );
    return fResult;
}

    uno::Reference< XScaling > SAL_CALL
ExponentialScaling::getInverseScaling()
    throw (uno::RuntimeException)
{
    return new LogarithmicScaling( m_fBase );
}

    ::rtl::OUString SAL_CALL
ExponentialScaling::getServiceName()
    throw (uno::RuntimeException)
{
    return lcl_aServiceName_Exponential;
}

// ----------------------------------------

LinearScaling::LinearScaling( double fSlope, double fOffset ) :
        m_fSlope( fSlope ),
        m_fOffset( fOffset )
{}

LinearScaling::~LinearScaling()
{}

double SAL_CALL LinearScaling::doScaling( double value )
    throw (uno::RuntimeException)
{
    double fResult;
    if( ::rtl::math::isNan( value ) || ::rtl::math::isInf( value ) )
        ::rtl::math::setNan( & fResult );
    else
        fResult = m_fOffset + m_fSlope * value;
    return fResult;
}

uno::Reference< XScaling > SAL_CALL
    LinearScaling::getInverseScaling()
    throw (uno::RuntimeException)
{
    // ToDo: ApproxEqual ?
    if( m_fSlope == 0 )
        throw uno::RuntimeException();

    return new LinearScaling( 1.0 / m_fSlope, m_fOffset / m_fSlope );
}

    ::rtl::OUString SAL_CALL
LinearScaling::getServiceName()
    throw (uno::RuntimeException)
{
    return lcl_aServiceName_Linear;
}

// ----------------------------------------

PowerScaling::PowerScaling( double fExponent ) :
        m_fExponent( fExponent )
{}

PowerScaling::~PowerScaling()
{}

double SAL_CALL PowerScaling::doScaling( double value )
    throw (uno::RuntimeException)
{
    double fResult;
    if( ::rtl::math::isNan( value ) || ::rtl::math::isInf( value ) )
        ::rtl::math::setNan( & fResult );
    else
        fResult = pow( value, m_fExponent );
    return fResult;
}

uno::Reference< XScaling > SAL_CALL
    PowerScaling::getInverseScaling()
    throw (uno::RuntimeException)
{
    // ToDo: ApproxEqual ?
    if( m_fExponent == 0 )
        throw uno::RuntimeException();

    return new PowerScaling( 1.0 / m_fExponent );
}

    ::rtl::OUString SAL_CALL
PowerScaling::getServiceName()
    throw (uno::RuntimeException)
{
    return lcl_aServiceName_Power;
}

//.............................................................................
} //namespace chart
//.............................................................................
