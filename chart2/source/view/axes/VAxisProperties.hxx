/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: VAxisProperties.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 19:10:24 $
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
#ifndef _CHART2_VAXIS_PROPERTIES_HXX
#define _CHART2_VAXIS_PROPERTIES_HXX

#include "TickmarkProperties.hxx"
#include "PlottingPositionHelper.hxx"
#include "LabelAlignment.hxx"

#ifndef _COM_SUN_STAR_CHART2_XAXIS_HPP_
#include <com/sun/star/chart2/XAxis.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_AXISTYPE_HPP_
#include <com/sun/star/chart2/AxisType.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART2_DATA_XTEXTUALDATASEQUENCE_HPP_
#include <com/sun/star/chart2/data/XTextualDataSequence.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_RECTANGLE_HPP_
#include <com/sun/star/awt/Rectangle.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_TEXTVERTICALADJUST_HPP_
#include <com/sun/star/drawing/TextVerticalAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_TEXTHORIZONTALADJUST_HPP_
#include <com/sun/star/drawing/TextHorizontalAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

#include <vector>

//.............................................................................
namespace chart
{
//.............................................................................

//-----------------------------------------------------------------------------
/**
*/

//These properties describe how a couple of labels are arranged one to another.
//The couple can contain all labels for all tickmark depth or just the labels for one single depth or
//the labels from an coherent range of tick depths (e.g. the major and first minor tickmarks should be handled together).
//... only allow side by side for different tick depth
enum AxisLabelStaggering
{
      SIDE_BY_SIDE
    , STAGGER_EVEN
    , STAGGER_ODD
    , STAGGER_AUTO
};

struct AxisLabelProperties
{
    AxisLabelProperties();

    ::com::sun::star::awt::Size         m_aFontReferenceSize;//reference size to calculate the font height
    ::com::sun::star::awt::Rectangle    m_aMaximumSpaceForLabels;//Labels need to be clipped in order to fit into this rectangle

    sal_Int32            nNumberFormatKey;

    AxisLabelStaggering  eStaggering;

    sal_Bool             bLineBreakAllowed;
    sal_Bool             bOverlapAllowed;

    sal_Bool             bStackCharacters;
    double               fRotationAngleDegree;

    sal_Int32   nRhythm; //show only each nth label with n==nRhythm
    bool        bRhythmIsFix; //states wether the given rythm is fix or may be changed

    //methods:
    void init( const ::com::sun::star::uno::Reference<
                ::com::sun::star::chart2::XAxis >&  xAxisModel );

    sal_Bool            getIsStaggered() const;
};

struct AxisProperties
{
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XAxis > m_xAxisModel;

    sal_Int32   m_nDimensionIndex;
    bool        m_bIsMainAxis;//not secondary axis
    bool        m_bSwapXAndY;
    double*     m_pfMainLinePositionAtOtherAxis;
    double*     m_pfExrtaLinePositionAtOtherAxis;

    //this direction is used to indicate in which direction inner tickmarks are to be drawn
    double          m_fInnerDirectionSign;
    bool            m_bLabelsOutside;
    LabelAlignment  m_aLabelAlignment;
    sal_Bool        m_bDisplayLabels;

    sal_Int32       m_nNumberFormatKey;


//    enum RelativeLabelPosition { NONE, LEFTORBOTTOM_OF_DIAGRAM, RIGHTORTOP_OF_DIAGRAM,
//                        LEFTORBOTTOM_OF_AXIS, RIGHTORTOP_OF_AXIS };
//    RelativeLabelPosition m_eRelativeLabelPosition;

    /*
    0: no tickmarks         1: inner tickmarks
    2: outer tickmarks      3: inner and outer tickmarks
    */
    sal_Int32                           m_nMajorTickmarks;
    sal_Int32                           m_nMinorTickmarks;
    ::std::vector<TickmarkProperties>   m_aTickmarkPropertiesList;

    VLineProperties                     m_aLineProperties;

    //for category axes ->
    sal_Int32                                        m_nAxisType;//REALNUMBER, CATEGORY etc. type ::com::sun::star::chart2::AxisType
    ::com::sun::star::uno::Reference<
        ::com::sun::star::chart2::data::XTextualDataSequence >
                                                    m_xAxisTextProvider; //for categries or series names
    //position of main tickmarks in respect to the indicated value: at value or between neighboured indicated values
    bool                                             m_bTickmarksAtIndicatedValue;
    //<- category axes

    //methods:

    AxisProperties( const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XAxis >& xAxisModel
                  , const ::com::sun::star::uno::Reference<
                        ::com::sun::star::chart2::data::XTextualDataSequence >& xAxisTextProvider );
    AxisProperties( const AxisProperties& rAxisProperties );
    virtual ~AxisProperties();
    virtual void init(bool bCartesian=false);//init from model data (m_xAxisModel)

private:
    AxisProperties();

protected:
    virtual TickmarkProperties  makeTickmarkProperties( sal_Int32 nDepth ) const;
    VLineProperties      makeLinePropertiesForDepth( sal_Int32 nDepth ) const;
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif
