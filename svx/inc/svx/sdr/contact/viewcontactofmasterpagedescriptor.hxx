/*************************************************************************
 *
 *  $RCSfile: viewcontactofmasterpagedescriptor.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2004-12-13 08:54:06 $
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

#ifndef _SDR_CONTACT_VIEWCONTACTOFMASTERPAGEDESCRIPTOR_HXX
#define _SDR_CONTACT_VIEWCONTACTOFMASTERPAGEDESCRIPTOR_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _SDR_CONTACT_VIEWCONTACT_HXX
#include <svx/sdr/contact/viewcontact.hxx>
#endif

#ifndef _SDR_MASTERPAGEDESCRIPTOR_HXX
#include <sdrmasterpagedescriptor.hxx>
#endif

//////////////////////////////////////////////////////////////////////////////
// predeclarations
class SfxItemSet;
class Bitmap;
class MapMode;

namespace sdr
{
    namespace contact
    {
        class OwnMasterPagePainter;
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace contact
    {
        class ViewContactOfMasterPageDescriptor : public ViewContact
        {
        protected:
            // the owner of this ViewContact. Set from constructor and not
            // to be changed in any way.
            sdr::MasterPageDescriptor&                      mrMasterPageDescriptor;

            // the painter for the MasterPage content
            ::sdr::contact::OwnMasterPagePainter*           mpMasterPagePainter;

            // Get the correct BackgroundObject
            SdrObject* GetBackgroundObject() const;

            // Get the LayerId of the BackgroundObject
            sal_uInt8 GetBackgroundObjectLayerId() const;

            // Create a Object-Specific ViewObjectContact, set ViewContact and
            // ObjectContact. Always needs to return something. Default is to create
            // a standard ViewObjectContact containing the given ObjectContact and *this
            virtual ViewObjectContact& CreateObjectSpecificViewObjectContact(ObjectContact& rObjectContact);

            // method to recalculate the PaintRectangle if the validity flag shows that
            // it is invalid. The flag is set from GetPaintRectangle, thus the implementation
            // only needs to refresh maPaintRectangle itself.
            virtual void CalcPaintRectangle();

        public:
            // basic constructor
            ViewContactOfMasterPageDescriptor(sdr::MasterPageDescriptor& rDescriptor);

            // The destructor.
            virtual ~ViewContactOfMasterPageDescriptor();

            // access to MasterPageDescriptor
            sdr::MasterPageDescriptor& GetMasterPageDescriptor() const
            {
                return mrMasterPageDescriptor;
            }

            // When ShouldPaintObject() returns sal_True, the object itself is painted and
            // PaintObject() is called.
            virtual sal_Bool ShouldPaintObject(DisplayInfo& rDisplayInfo, const ViewObjectContact& rAssociatedVOC);

            // #115593# Paint this object. This is before evtl. SubObjects get painted. It needs to return
            // sal_True when something was pained and the paint output rectangle in rPaintRectangle.
            virtual sal_Bool PaintObject(DisplayInfo& rDisplayInfo, Rectangle& rPaintRectangle, const ViewObjectContact& rAssociatedVOC);
            virtual ViewContact* GetParentContact() const;

            // React on changes of the object of this ViewContact
            virtual void ActionChanged();

            // Interface method for receiving buffered MasterPage render data from
            // VOCOfMasterPageDescriptor. Called from instances of VOCOfMasterPageDescriptor.
            void OfferBufferedData(const Bitmap& rBitmap, const MapMode& rMapMode);

            // Interface method for VOCOfMasterPageDescriptor to ask for buffered data. If
            // the page is the sane and the MapMode is the same, return the Bitmap.
            Bitmap RequestBufferedData(const MapMode& rMapMode);

            // #i37869# Support method to paint borders and grids which are overpainted from
            // this MasterPage content to let the MasterPage appear as page background
            void PaintBackgroundPageBordersAndGrids(DisplayInfo& rDisplayInfo);
        };
    } // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_CONTACT_VIEWCONTACTOFMASTERPAGEDESCRIPTOR_HXX

// eof
