/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: StatisticsItemConverter.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 17:56:39 $
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
#ifndef CHART_STATISTICSITEMCONVERTER_HXX
#define CHART_STATISTICSITEMCONVERTER_HXX

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#include "ItemConverter.hxx"
#include "GraphicPropertyItemConverter.hxx"
#include "chartview/NumberFormatterWrapper.hxx"

#include <vector>

class SdrModel;

namespace chart
{
namespace wrapper
{

class StatisticsItemConverter :
        public ::comphelper::ItemConverter
{
public:
    StatisticsItemConverter(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XModel > & xChartModel,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySet > & rPropertySet,
        SfxItemPool& rItemPool );
    virtual ~StatisticsItemConverter();

protected:
    virtual const USHORT * GetWhichPairs() const;
    virtual bool GetItemProperty( tWhichIdType nWhichId, tPropertyNameWithMemberId & rOutProperty ) const;

    virtual void FillSpecialItem( USHORT nWhichId, SfxItemSet & rOutItemSet ) const
        throw( ::com::sun::star::uno::Exception );
    virtual bool ApplySpecialItem( USHORT nWhichId, const SfxItemSet & rItemSet )
        throw( ::com::sun::star::uno::Exception );

private:
    ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XModel >  m_xModel;
};

} //  namespace wrapper
} //  namespace chart

// CHART_STATISTICSITEMCONVERTER_HXX
#endif
