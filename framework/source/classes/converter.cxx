/*************************************************************************
 *
 *  $RCSfile: converter.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: as $ $Date: 2002-05-02 11:38:17 $
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

#ifndef __FRAMEWORK_CLASSES_CONVERTER_HXX_
#include <classes/converter.hxx>
#endif

namespace framework{

//-----------------------------------------------------------------------------
/**
 * pack every property item of source list into an any entry of destination list
 * Resulting list will have follow format then: "sequence< Any(PropertyValue) >".
 * If one item couldn't be converted it will be ignored - means target list can
 * be smaller then source list. Source list isn't changed anytime.
 *
 * algorithm:
 *      (a) reserve enough space on destination list for all possible entries of
 *          source list
 *      (b) try to pack every property of source into an any of destination list
 *          (b1) count successfully packed entries only
 *      (c) use this count of packed entries to resize destination list
 *          Because we getted enough space before - that will remove unused items
 *          of destination list at the end of it only.
 */
css::uno::Sequence< css::uno::Any > Converter::convert_seqProp2seqAny( const css::uno::Sequence< css::beans::PropertyValue >& lSource )
{
    sal_Int32 nCount = lSource.getLength();
    css::uno::Sequence< css::uno::Any > lDestination(nCount);

    for (sal_Int32 nItem=0; nItem<nCount; ++nItem)
        lDestination[nItem]<<=lSource[nItem];

    return lDestination;
}

//-----------------------------------------------------------------------------
/**
 * do the same like convert_seqProp2seqAny() before - but reverse.
 * It try to unpack PropertyValue items from given Any's.
 */
css::uno::Sequence< css::beans::PropertyValue > Converter::convert_seqAny2seqProp( const css::uno::Sequence< css::uno::Any >& lSource )
{
    sal_Int32 nCount = lSource.getLength();
    sal_Int32 nRealCount = 0;
    css::uno::Sequence< css::beans::PropertyValue > lDestination(nCount);

    for (sal_Int32 nItem=0; nItem<nCount; ++nItem)
    {
        if (lSource[nItem]>>=lDestination[nItem])
            ++nRealCount;
    }

    if (nRealCount!=nCount)
        lDestination.realloc(nRealCount);

    return lDestination;
}

//-----------------------------------------------------------------------------
/**
 * converts a sequence of unicode strings into a vector of such items
 */
OUStringList Converter::convert_seqOUString2OUStringList( const css::uno::Sequence< ::rtl::OUString >& lSource )
{
    OUStringList lDestination;
    sal_Int32 nCount = lSource.getLength();

    for (sal_Int32 nItem=0; nItem<nCount; ++nItem )
    {
        lDestination.push_back(lSource[nItem]);
    }

    return lDestination;
}

}       //  namespace framework
