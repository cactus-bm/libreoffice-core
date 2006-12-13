/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: delayevent.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2006-12-13 15:13:39 $
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
#include "precompiled_slideshow.hxx"

#include <osl/diagnose.h>
#include "delayevent.hxx"

namespace slideshow {
namespace internal {

bool Delay::fire()
{
    OSL_ASSERT( isCharged() );
    if (isCharged()) {
        mbWasFired = true;
        maFunc();
        maFunc.clear(); // early release of payload
    }
    return true;
}

bool Delay::isCharged() const
{
    return !mbWasFired;
}

double Delay::getActivationTime( double nCurrentTime ) const
{
    return nCurrentTime + mnTimeout;
}

void Delay::dispose()
{
    // don't clear unconditionally, because it may currently be executed:
    if (isCharged()) {
        mbWasFired = true;
        maFunc.clear(); // release of payload
    }
}

} // namespace internal
} // namespace presentation

