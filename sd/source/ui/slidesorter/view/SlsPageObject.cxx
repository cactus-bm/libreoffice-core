/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SlsPageObject.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:27:43 $
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

#include "view/SlsPageObject.hxx"

#include "model/SlsPageDescriptor.hxx"
#include "controller/SlsPageObjectFactory.hxx"

using namespace ::sdr::contact;
using namespace ::sd::slidesorter::model;


namespace sd { namespace slidesorter { namespace view {


PageObject::PageObject (
    const Rectangle& rRectangle,
    SdrPage* pPage,
    PageDescriptor& rDescriptor)
    : SdrPageObj (rRectangle, pPage),
      mrDescriptor (rDescriptor)
{
}




PageObject::~PageObject (void)
{
}




PageDescriptor& PageObject::GetDescriptor (void) const
{
    return mrDescriptor;
}




sdr::contact::ViewContact* PageObject::CreateObjectSpecificViewContact (void)
{
    return mrDescriptor.GetPageObjectFactory().CreateViewContact (
        this,
        mrDescriptor);
}



} } } // end of namespace ::sd::slidesorter::view
