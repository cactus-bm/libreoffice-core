#ifndef _CHART2_BARCHART_HXX
#define _CHART2_BARCHART_HXX

#include "VSeriesPlotter.hxx"
#include "DatapointGeometry.hxx"

//.............................................................................
namespace chart
{
//.............................................................................
class BarPositionHelper;

enum Geometry3D { GEOMETRY_UNKNOWN
                , GEOMETRY_CUBOID
                , GEOMETRY_CYLINDER
                , GEOMETRY_CONE
                , GEOMETRY_PYRAMID };

class BarChart : public VSeriesPlotter
{
    //-------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------
public:
    BarChart( const ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::XChartType >& xChartTypeModel );
    virtual ~BarChart();

    //-------------------------------------------------------------------------
    // chart2::XPlotter
    //-------------------------------------------------------------------------

    virtual void SAL_CALL createShapes();
    /*
    virtual ::rtl::OUString SAL_CALL getCoordinateSystemTypeID(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setScales( const ::com::sun::star::uno::Sequence< ::com::sun::star::chart2::ExplicitScaleData >& rScales ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setTransformation( const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XTransformation >& xTransformationToLogicTarget, const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XTransformation >& xTransformationToFinalPage ) throw (::com::sun::star::uno::RuntimeException);
    */

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------

private: //methods
    //no default constructor
    BarChart();

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createDataPoint3D_Bar(
                          const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry
                        , const ::com::sun::star::uno::Reference<
                                ::com::sun::star::beans::XPropertySet >& xObjectProperties
                        , Geometry3D eGeometry );
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >
        createDataPoint2D_Bar(
                          const ::com::sun::star::uno::Reference<
                                ::com::sun::star::drawing::XShapes >& xTarget
                        , const DataPointGeometry& rGeometry
                        , const ::com::sun::star::uno::Reference<
                                ::com::sun::star::beans::XPropertySet >& xObjectProperties );

    ::com::sun::star::awt::Point getLabelScreenPositionAndAlignment(
                        LabelAlignment& rAlignment, bool bMiddlePosition
                        , const DataPointGeometry& rTransformedGeom ) const;

private: //member
    BarPositionHelper*                   m_pPosHelper;
};
//.............................................................................
} //namespace chart
//.............................................................................
#endif
