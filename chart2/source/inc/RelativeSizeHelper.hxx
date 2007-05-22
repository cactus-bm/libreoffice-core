/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: RelativeSizeHelper.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 18:21:55 $
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
#ifndef CHART2_RELATIVESIZEHELPER_HXX
#define CHART2_RELATIVESIZEHELPER_HXX

#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

namespace chart
{

class RelativeSizeHelper
{
public:
    static double calculate(
        double fValue,
        const ::com::sun::star::awt::Size & rOldReferenceSize,
        const ::com::sun::star::awt::Size & rNewReferenceSize );

    static void adaptFontSizes(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySet > & xTargetProperties,
        const ::com::sun::star::awt::Size & rOldReferenceSize,
        const ::com::sun::star::awt::Size & rNewReferenceSize );

private:
    // not implemented
    RelativeSizeHelper();
};

} //  namespace chart

// CHART2_RELATIVESIZEHELPER_HXX
#endif
