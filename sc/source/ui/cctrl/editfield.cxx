/*************************************************************************
 *
 *  $RCSfile: editfield.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-23 09:28:51 $
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

#ifdef SC_DLLIMPLEMENTATION
#undef SC_DLLIMPLEMENTATION
#endif

#ifndef SC_EDITFIELD_HXX
#include "editfield.hxx"
#endif

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

// ============================================================================

namespace {

sal_Unicode lclGetDecSep()
{
    return ScGlobal::GetpLocaleData()->getNumDecimalSep().GetChar( 0 );
}

sal_Unicode lclGetGroupSep()
{
    return ScGlobal::GetpLocaleData()->getNumThousandSep().GetChar( 0 );
}

} // namespace

// ============================================================================

ScDoubleField::ScDoubleField( Window* pParent, WinBits nStyle ) :
    Edit( pParent, nStyle )
{
}

ScDoubleField::ScDoubleField( Window* pParent, const ResId& rResId ) :
    Edit( pParent, rResId )
{
}

bool ScDoubleField::GetValue( double& rfValue ) const
{
    String aStr( GetText() );
    aStr.EraseLeadingAndTrailingChars( ' ' );
    bool bOk = aStr.Len() > 0;
    if( bOk )
    {
        rtl_math_ConversionStatus eStatus;
        sal_Int32 nEnd;
        rfValue = rtl::math::stringToDouble( aStr, lclGetDecSep(), lclGetGroupSep(), &eStatus, &nEnd );
        bOk = (eStatus == rtl_math_ConversionStatus_Ok) && (nEnd == static_cast< sal_Int32 >( aStr.Len() ));
    }
    return bOk;
}

void ScDoubleField::SetValue( double fValue, sal_Int32 nDecPlaces, bool bEraseTrailingDecZeros )
{
    SetText( ::rtl::math::doubleToUString( fValue, rtl_math_StringFormat_G,
        nDecPlaces, lclGetDecSep(), bEraseTrailingDecZeros ) );
}

// ============================================================================

