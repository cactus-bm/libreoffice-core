/*************************************************************************
 *
 *  $RCSfile: errorcodes.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $  $Date: 2004-03-08 11:40:52 $
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

#ifndef SC_ERRORCODES_HXX
#define SC_ERRORCODES_HXX

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif

namespace ScErrorCodes
{

const USHORT errIllegalChar          = 501;
const USHORT errIllegalArgument      = 502;
const USHORT errIllegalFPOperation   = 503;
const USHORT errIllegalParameter     = 504;
const USHORT errIllegalJump          = 505;
const USHORT errSeparator            = 506;
const USHORT errPair                 = 507;
const USHORT errPairExpected         = 508;
const USHORT errOperatorExpected     = 509;
const USHORT errVariableExpected     = 510;
const USHORT errParameterExpected    = 511;
const USHORT errCodeOverflow         = 512;
const USHORT errStringOverflow       = 513;
const USHORT errStackOverflow        = 514;
const USHORT errUnknownState         = 515;
const USHORT errUnknownVariable      = 516;
const USHORT errUnknownOpCode        = 517;
const USHORT errUnknownStackVariable = 518;
const USHORT errNoValue              = 519;
const USHORT errUnknownToken         = 520;
const USHORT errNoCode               = 521;
const USHORT errCircularReference    = 522;
const USHORT errNoConvergence        = 523;
const USHORT errNoRef                = 524;
const USHORT errNoName               = 525;
const USHORT errDoubleRef            = 526;
const USHORT errInterpOverflow       = 527;
// Not displayed, temporary for TrackFormulas,
// Cell depends on another cell that has errCircularReference
const USHORT errTrackFromCircRef     = 528;
// Interpreter internal: existing cell has no value but value queried
const USHORT errCellNoValue          = 529;
// Interpreter: needed AddIn not found
const USHORT errNoAddin              = 530;
// Interpreter: needed Macro not found
const USHORT errNoMacro              = 531;

// Interpreter: NA() not available condition, not a real error
const USHORT NOVALUE                 = 0x7fff;


/** Unconditionally construct a double value of NAN where the lower bits
    represent an interpreter error code. */
inline double CreateDoubleError( USHORT nErr )
{
    double fVal;
    ::rtl::math::setNan( &fVal );
    reinterpret_cast< sal_math_Double * >(&fVal)->nan_parts.fraction_lo = nErr;
    return fVal;
}


/** Recreate the error code of a coded double error, if any. */
inline USHORT GetDoubleErrorValue( double fVal )
{
    if ( ::rtl::math::isFinite( fVal ) )
        return 0;
    if ( ::rtl::math::isInf( fVal ) )
        return errIllegalFPOperation;       // normal INF
    UINT32 nErr = reinterpret_cast< sal_math_Double * >(
            &fVal)->nan_parts.fraction_lo;
    if ( nErr & 0xffff0000 )
        return errNoValue;                  // just a normal NAN
    return (USHORT)(nErr & 0x0000ffff);     // any other error
}

} // namespace ScErrorCodes

// yes, exceptionally we put a "using namespace" in a header file..
using namespace ScErrorCodes;

#endif // SC_ERRORCODES_HXX
