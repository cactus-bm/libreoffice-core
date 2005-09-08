/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: customshapeproperties.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:11:38 $
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

#ifndef _SDR_PROPERTIES_CUSTOMSHAPEPROPERTIES_HXX
#include <svx/sdr/properties/customshapeproperties.hxx>
#endif

#ifndef _SVDOASHP_HXX
#include <svdoashp.hxx>
#endif
#ifndef _EEITEMID_HXX
#include <eeitemid.hxx>
#endif
#ifndef _SDTAGITM_HXX
#include <sdtagitm.hxx>
#endif
#ifndef _SFX_WHITER_HXX
#include <svtools/whiter.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace properties
    {
        SfxItemSet& CustomShapeProperties::CreateObjectSpecificItemSet(SfxItemPool& rPool)
        {
            return *(new SfxItemSet(rPool,

                // ranges from SdrAttrObj
                SDRATTR_START, SDRATTR_SHADOW_LAST,
                SDRATTR_MISC_FIRST, SDRATTR_MISC_LAST,
                SDRATTR_TEXTDIRECTION, SDRATTR_TEXTDIRECTION,

                // Graphic Attributes
                SDRATTR_GRAF_FIRST, SDRATTR_GRAF_LAST,

                // 3d Properties
                SDRATTR_3D_FIRST, SDRATTR_3D_LAST,

                // CustomShape properties
                SDRATTR_CUSTOMSHAPE_FIRST, SDRATTR_CUSTOMSHAPE_LAST,

                // range from SdrTextObj
                EE_ITEMS_START, EE_ITEMS_END,

                // end
                0, 0));
        }
        sal_Bool CustomShapeProperties::AllowItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem ) const
        {
            sal_Bool bAllowItemChange = sal_True;
            if ( !pNewItem )
            {
                if ( ( nWhich >= SDRATTR_CUSTOMSHAPE_FIRST ) && ( nWhich <= SDRATTR_CUSTOMSHAPE_LAST ) )
                    bAllowItemChange = sal_False;
            }
            if ( bAllowItemChange )
                bAllowItemChange = TextProperties::AllowItemChange( nWhich, pNewItem );
            return bAllowItemChange;
        }
        void CustomShapeProperties::ClearObjectItem(const sal_uInt16 nWhich)
        {
            if ( !nWhich )
            {
                SfxWhichIter aIter( *mpItemSet );
                sal_uInt16 nWhich = aIter.FirstWhich();
                while( nWhich )
                {
                    TextProperties::ClearObjectItem( nWhich );
                    nWhich = aIter.NextWhich();
                }
            }
            else
                TextProperties::ClearObjectItem( nWhich );
        }
        void CustomShapeProperties::ClearObjectItemDirect(const sal_uInt16 nWhich)
        {
            if ( !nWhich )
            {
                SfxWhichIter aIter( *mpItemSet );
                sal_uInt16 nWhich = aIter.FirstWhich();
                while( nWhich )
                {
                    TextProperties::ClearObjectItemDirect( nWhich );
                    nWhich = aIter.NextWhich();
                }
            }
            else
                TextProperties::ClearObjectItemDirect( nWhich );
        }
        void CustomShapeProperties::ItemSetChanged(const SfxItemSet& rSet)
        {
            SdrObjCustomShape& rObj = (SdrObjCustomShape&)GetSdrObject();

            if( SFX_ITEM_SET == rSet.GetItemState( SDRATTR_TEXT_AUTOGROWHEIGHT ) )
            {
                rObj.bTextFrame = ((SdrTextAutoGrowHeightItem&)rSet.Get( SDRATTR_TEXT_AUTOGROWHEIGHT )).GetValue() != 0;
            }

            // call parent
            TextProperties::ItemSetChanged(rSet);

            // local changes, removing cached objects
            rObj.InvalidateRenderGeometry();
        }
        void CustomShapeProperties::ItemChange(const sal_uInt16 nWhich, const SfxPoolItem* pNewItem)
        {
            SdrTextObj& rObj = (SdrTextObj&)GetSdrObject();
            OutlinerParaObject* pParaObj = rObj.GetOutlinerParaObject();

            if( pNewItem && ( SDRATTR_TEXT_AUTOGROWHEIGHT == nWhich ) )
            {
                rObj.bTextFrame = ((SdrTextAutoGrowHeightItem*)pNewItem)->GetValue() != 0;
            }
            // call parent
            TextProperties::ItemChange( nWhich, pNewItem );
        }
        void CustomShapeProperties::ForceDefaultAttributes()
        {
/* SJ: Following is is no good if creating customshapes leading to objects that are white after loading via xml

            SdrTextObj& rObj = (SdrTextObj&)GetSdrObject();
            sal_Bool bTextFrame(rObj.IsTextFrame());

            // force ItemSet
            GetObjectItemSet();

            if(bTextFrame)
            {
                mpItemSet->Put(XLineStyleItem(XLINE_NONE));
                mpItemSet->Put(XFillColorItem(String(), Color(COL_WHITE)));
                mpItemSet->Put(XFillStyleItem(XFILL_NONE));
            }
            else
            {
                mpItemSet->Put(SvxAdjustItem(SVX_ADJUST_CENTER));
                mpItemSet->Put(SdrTextHorzAdjustItem(SDRTEXTHORZADJUST_CENTER));
                mpItemSet->Put(SdrTextVertAdjustItem(SDRTEXTVERTADJUST_CENTER));
            }
*/
        }
        CustomShapeProperties::CustomShapeProperties(SdrObject& rObj)
        :   TextProperties(rObj)
        {
        }

        CustomShapeProperties::CustomShapeProperties(const CustomShapeProperties& rProps, SdrObject& rObj)
        :   TextProperties(rProps, rObj)
        {
        }

        CustomShapeProperties::~CustomShapeProperties()
        {
        }

        BaseProperties& CustomShapeProperties::Clone(SdrObject& rObj) const
        {
            return *(new CustomShapeProperties(*this, rObj));
        }
    } // end of namespace properties
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
