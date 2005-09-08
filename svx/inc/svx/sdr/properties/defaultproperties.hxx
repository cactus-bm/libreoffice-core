/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: defaultproperties.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 20:05:09 $
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

#ifndef _SDR_PROPERTIES_DEFAULTPROPERTIES_HXX
#define _SDR_PROPERTIES_DEFAULTPROPERTIES_HXX

#ifndef _SDR_PROPERTIES_PROPERTIES_HXX
#include <svx/sdr/properties/properties.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace properties
    {
        class SVX_DLLPUBLIC DefaultProperties : public BaseProperties
        {
        protected:
            // the to be used ItemSet
            SfxItemSet*                                     mpItemSet;

            // create a new itemset
            virtual SfxItemSet& CreateObjectSpecificItemSet(SfxItemPool& rPool);

            // test changeability for a single item
            virtual sal_Bool AllowItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem = 0) const;

            // Do the ItemChange, may do special handling
            virtual void ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem = 0);

            // Called after ItemChange() is done for all items.
            virtual void PostItemChange(const sal_uInt16 nWhich);

            // react on ItemSet changes
            virtual void ItemSetChanged(const SfxItemSet& rSet);

        public:
            // basic constructor
            DefaultProperties(SdrObject& rObj);

            // constructor for copying, but using new object
            DefaultProperties(const DefaultProperties& rProps, SdrObject& rObj);

            // destructor
            virtual ~DefaultProperties();

            // Clone() operator, normally just calls the local copy constructor
            virtual BaseProperties& Clone(SdrObject& rObj) const;

            // get itemset
            virtual const SfxItemSet& GetObjectItemSet() const;

            // set single item
            virtual void SetObjectItem(const SfxPoolItem& rItem);

            // set single item direct, do not do any notifies or things like that
            virtual void SetObjectItemDirect(const SfxPoolItem& rItem);

            // clear single item
            virtual void ClearObjectItem(const sal_uInt16 nWhich = 0);

            // clear single item direct, do not do any notifies or things like that.
            // Also supports complete deleteion of items when default parameter 0 is used.
            virtual void ClearObjectItemDirect(const sal_uInt16 nWhich = 0);

            // set complete item set
            virtual void SetObjectItemSet(const SfxItemSet& rSet);

            // set a new StyleSheet and broadcast
            virtual void SetStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr);

            // get the installed StyleSheet
            virtual SfxStyleSheet* GetStyleSheet() const;

            // pre/post-process saving
            //BFS01virtual void PreProcessSave();
            //BFS01virtual void PostProcessSave();

            // force default attributes for a specific object type, called from
            // DefaultProperties::GetObjectItemSet() if a new ItemSet is created.
            // Default implementation does nothing.
            virtual void ForceDefaultAttributes();

            // Scale the included ItemSet.
            virtual void Scale(const Fraction& rScale);
        };
    } // end of namespace properties
} // end of namespace sdr

#endif //_SDR_PROPERTIES_DEFAULTPROPERTIES_HXX

// eof
