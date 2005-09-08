/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pageproperties.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:15:40 $
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

#ifndef _SDR_PROPERTIES_PAGEPROPERTIES_HXX
#include <svx/sdr/properties/pageproperties.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _SVDOBJ_HXX
#include <svdobj.hxx>
#endif

#ifndef _SVDPOOL_HXX
#include <svdpool.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace properties
    {
        // create a new itemset
        SfxItemSet& PageProperties::CreateObjectSpecificItemSet(SfxItemPool& rPool)
        {
            // overloaded to legally return a valid ItemSet
            return *(new SfxItemSet(rPool));
        }

        PageProperties::PageProperties(SdrObject& rObj)
        :   EmptyProperties(rObj)
        {
        }

        PageProperties::PageProperties(const PageProperties& rProps, SdrObject& rObj)
        :   EmptyProperties(rProps, rObj)
        {
        }

        PageProperties::~PageProperties()
        {
        }

        BaseProperties& PageProperties::Clone(SdrObject& rObj) const
        {
            return *(new PageProperties(*this, rObj));
        }

        // get itemset. Overloaded here to allow creating the empty itemset
        // without asserting
        const SfxItemSet& PageProperties::GetObjectItemSet() const
        {
            if(!mpEmptyItemSet)
            {
                ((PageProperties*)this)->mpEmptyItemSet = &(((PageProperties*)this)->CreateObjectSpecificItemSet(*GetSdrObject().GetObjectItemPool()));
            }

            DBG_ASSERT(mpEmptyItemSet, "Could not create an SfxItemSet(!)");

            return *mpEmptyItemSet;
        }

        void PageProperties::ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem)
        {
            // #86481# simply ignore item setting on page objects
        }

        SfxStyleSheet* PageProperties::GetStyleSheet() const
        {
            // overloaded to legally return a 0L pointer here
            return 0L;
        }

        void PageProperties::ClearObjectItem(const sal_uInt16 nWhich)
        {
            // simply ignore item clearing on page objects
        }
    } // end of namespace properties
} // end of namespace sdr

////////////////////////////////////////////////////////////////////////////////////////////////////

// eof
