/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: graphicproperties.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 05:43:05 $
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
#include "precompiled_svx.hxx"

#ifndef _SDR_PROPERTIES_GRAPHICPROPERTIES_HXX
#include <svx/sdr/properties/graphicproperties.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _SFXSTYLE_HXX
#include <svtools/style.hxx>
#endif

#ifndef _SVDDEF_HXX
#include <svddef.hxx>
#endif

#ifndef _EEITEM_HXX
#include <eeitem.hxx>
#endif

#ifndef _SVDOGRAF_HXX
#include <svdograf.hxx>
#endif

#define ITEMID_GRF_CROP 0

#ifndef _SDGCPITM_HXX
#include <sdgcpitm.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace properties
    {
        // create a new itemset
        SfxItemSet& GraphicProperties::CreateObjectSpecificItemSet(SfxItemPool& rPool)
        {
            return *(new SfxItemSet(rPool,

                // range from SdrAttrObj
                SDRATTR_START, SDRATTR_SHADOW_LAST,
                SDRATTR_MISC_FIRST, SDRATTR_MISC_LAST,
                SDRATTR_TEXTDIRECTION, SDRATTR_TEXTDIRECTION,

                // range from SdrGrafObj
                SDRATTR_GRAF_FIRST, SDRATTR_GRAF_LAST,

                // range from SdrTextObj
                EE_ITEMS_START, EE_ITEMS_END,

                // end
                0, 0));
        }

        GraphicProperties::GraphicProperties(SdrObject& rObj)
        :   RectangleProperties(rObj)
        {
        }

        GraphicProperties::GraphicProperties(const GraphicProperties& rProps, SdrObject& rObj)
        :   RectangleProperties(rProps, rObj)
        {
        }

        GraphicProperties::~GraphicProperties()
        {
        }

        BaseProperties& GraphicProperties::Clone(SdrObject& rObj) const
        {
            return *(new GraphicProperties(*this, rObj));
        }

        void GraphicProperties::ItemSetChanged(const SfxItemSet& rSet)
        {
            SdrGrafObj& rObj = (SdrGrafObj&)GetSdrObject();

            // local changes
            rObj.SetXPolyDirty();

            // #i29367# Update GraphicAttr, too. This was formerly
            // triggered by SdrGrafObj::SFX_NOTIFY, which is no longer
            // called nowadays. BTW: strictly speaking, the whole
            // ImpSetAttrToGrafInfo/ImpSetGrafInfoToAttr stuff could
            // be dumped, when SdrGrafObj::aGrafInfo is removed and
            // always created on the fly for repaint.
            rObj.ImpSetAttrToGrafInfo();

            // call parent
            RectangleProperties::ItemSetChanged(rSet);
        }

        void GraphicProperties::SetStyleSheet(SfxStyleSheet* pNewStyleSheet, sal_Bool bDontRemoveHardAttr)
        {
            SdrGrafObj& rObj = (SdrGrafObj&)GetSdrObject();

            // local changes
            rObj.SetXPolyDirty();

            // call parent
            RectangleProperties::SetStyleSheet(pNewStyleSheet, bDontRemoveHardAttr);

            // local changes
            rObj.ImpSetAttrToGrafInfo();
        }

//BFS01     void GraphicProperties::PreProcessSave()
//BFS01     {
//BFS01         // call parent
//BFS01         RectangleProperties::PreProcessSave();
//BFS01
//BFS01         // force ItemSet
//BFS01         GetObjectItemSet();
//BFS01
//BFS01         // prepare SetItems for storage
//BFS01         const SfxItemSet& rSet = *mpItemSet;
//BFS01         const SfxItemSet* pParent = mpStyleSheet ? &(mpStyleSheet->GetItemSet()) : 0L;
//BFS01
//BFS01         SdrGrafSetItem aGrafAttr(rSet.GetPool());
//BFS01         aGrafAttr.GetItemSet().Put(rSet);
//BFS01         aGrafAttr.GetItemSet().SetParent(pParent);
//BFS01         mpItemSet->Put(aGrafAttr);
//BFS01     }

//BFS01     void GraphicProperties::PostProcessSave()
//BFS01     {
//BFS01         // call parent
//BFS01         RectangleProperties::PostProcessSave();
//BFS01
//BFS01         // remove SetItems from local itemset
//BFS01         if(mpItemSet)
//BFS01         {
//BFS01             mpItemSet->ClearItem(SDRATTRSET_GRAF);
//BFS01         }
//BFS01     }

        void GraphicProperties::ForceDefaultAttributes()
        {
            // call parent
            RectangleProperties::ForceDefaultAttributes();

            // force ItemSet
            GetObjectItemSet();

            mpItemSet->Put( SdrGrafLuminanceItem( 0 ) );
            mpItemSet->Put( SdrGrafContrastItem( 0 ) );
            mpItemSet->Put( SdrGrafRedItem( 0 ) );
            mpItemSet->Put( SdrGrafGreenItem( 0 ) );
            mpItemSet->Put( SdrGrafBlueItem( 0 ) );
            mpItemSet->Put( SdrGrafGamma100Item( 100 ) );
            mpItemSet->Put( SdrGrafTransparenceItem( 0 ) );
            mpItemSet->Put( SdrGrafInvertItem( FALSE ) );
            mpItemSet->Put( SdrGrafModeItem( GRAPHICDRAWMODE_STANDARD ) );
            mpItemSet->Put( SdrGrafCropItem( 0, 0, 0, 0 ) );

            // #i25616#
            mpItemSet->Put( XFillStyleItem(XFILL_NONE) );
            mpItemSet->Put( XLineStyleItem(XLINE_NONE) );
        }
    } // end of namespace properties
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
