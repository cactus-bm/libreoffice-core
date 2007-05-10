/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: unoshap2.cxx,v $
 *
 *  $Revision: 1.62 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 09:16:48 $
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

#define _SVX_USE_UNOGLOBALS_

#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_VERTICALALIGNMENT_HPP_
#include <com/sun/star/style/VerticalAlignment.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_TEXTVERTICALADJUST_HPP_
#include <com/sun/star/drawing/TextVerticalAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_TEXTALIGN_HPP_
#include <com/sun/star/awt/TextAlign.hpp>  //added by BerryJia for fixing Bug102407 2002-11-4
#endif
#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHADJUST_HPP_
#include <com/sun/star/style/ParagraphAdjust.hpp>   //added by BerryJia for fixing Bug102407 2002-11-4
#endif
#ifndef _COM_SUN_STAR_DRAWING_POINTSEQUENCESEQUENCE_HPP_
#include <com/sun/star/drawing/PointSequenceSequence.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POINTSEQUENCE_HPP_
#include <com/sun/star/drawing/PointSequence.hpp>
#endif
#ifndef _COM_SUN_STAR_GRAPHIC_XGRAPHIC_HPP_
#include <com/sun/star/graphic/XGraphic.hpp>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _UNOTOOLS_LOCALFILEHELPER_HXX
#include <unotools/localfilehelper.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_ //autogen
#include <vos/mutex.hxx>
#endif
#ifndef _FLTCALL_HXX
#include <svtools/fltcall.hxx>
#endif
#ifndef _SVX_IMPGRF_HXX
#include "impgrf.hxx"
#endif

#include <boost/scoped_ptr.hpp>

#include <rtl/uuid.h>
#include <rtl/memory.h>
#include <tools/urlobj.hxx>

#include "unoprnms.hxx"
#include "unoshape.hxx"
#include "unopage.hxx"
#include "svdobj.hxx"
#include "svdpage.hxx"
#include "svdmodel.hxx"
#include "svdouno.hxx"
#include "shapeimpl.hxx"
#include "svdoashp.hxx"

// #i29181#
#ifndef _SVDVITER_HXX
#include "svdviter.hxx"
#endif
#ifndef _SVDVIEW_HXX //autogen
#include "svdview.hxx"
#endif

#ifndef _BGFX_MATRIX_B2DHOMMATRIX_HXX
#include <basegfx/matrix/b2dhommatrix.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGON_HXX
#include <basegfx/polygon/b2dpolygon.hxx>
#endif

#ifndef _BGFX_POINT_B2DPOINT_HXX
#include <basegfx/point/b2dpoint.hxx>
#endif

#ifndef _BGFX_POLYGON_B2DPOLYGONTOOLS_HXX
#include <basegfx/polygon/b2dpolygontools.hxx>
#endif

using ::rtl::OUString;
using namespace ::osl;
using namespace ::vos;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;

#define INTERFACE_TYPE( xint ) \
    ::getCppuType((const Reference< xint >*)0)

#define QUERYINT( xint ) \
    if( rType == ::getCppuType((const Reference< xint >*)0) ) \
        aAny <<= Reference< xint >(this)

class GDIMetaFile;
class SvStream;
sal_Bool ConvertGDIMetaFileToWMF( const GDIMetaFile & rMTF, SvStream & rTargetStream,
                              FilterConfigItem* pFilterConfigItem = NULL, sal_Bool bPlaceable = sal_True );

/***********************************************************************
* class SvxShapeGroup                                                  *
***********************************************************************/

SvxShapeGroup::SvxShapeGroup( SdrObject* pObj, SvxDrawPage* pDrawPage  )  throw() :
    SvxShape( pObj, aSvxMapProvider.GetMap(SVXMAP_GROUP) ),
    mxPage( pDrawPage )
{
}

//----------------------------------------------------------------------
SvxShapeGroup::~SvxShapeGroup() throw()
{
}

//----------------------------------------------------------------------
void SvxShapeGroup::Create( SdrObject* pNewObj, SvxDrawPage* pNewPage ) throw()
{
    SvxShape::Create( pNewObj, pNewPage );
    mxPage = pNewPage;
}

//----------------------------------------------------------------------
uno::Any SAL_CALL SvxShapeGroup::queryInterface( const uno::Type & rType )
    throw(uno::RuntimeException)
{
    return SvxShape::queryInterface( rType );
}

uno::Any SAL_CALL SvxShapeGroup::queryAggregation( const uno::Type & rType ) throw(uno::RuntimeException)
{
    uno::Any aAny;

    QUERYINT( drawing::XShapeGroup );
    else QUERYINT( drawing::XShapes );
    else QUERYINT( container::XIndexAccess );
    else QUERYINT( container::XElementAccess );
    else
        return SvxShape::queryAggregation( rType );

    return aAny;
}

void SAL_CALL SvxShapeGroup::acquire() throw ( )
{
    SvxShape::acquire();
}

void SAL_CALL SvxShapeGroup::release() throw ( )
{
    SvxShape::release();
}

uno::Sequence< uno::Type > SAL_CALL SvxShapeGroup::getTypes()
    throw (uno::RuntimeException)
{
    return SvxShape::getTypes();
}

uno::Sequence< sal_Int8 > SAL_CALL SvxShapeGroup::getImplementationId()
    throw (uno::RuntimeException)
{
    static uno::Sequence< sal_Int8 > aId;
    if( aId.getLength() == 0 )
    {
        aId.realloc( 16 );
        rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
    }
    return aId;
}

// ::com::sun::star::drawing::XShape

//----------------------------------------------------------------------
OUString SAL_CALL SvxShapeGroup::getShapeType()
    throw( uno::RuntimeException )
{
    return SvxShape::getShapeType();
}

//------------------------------------------------------------------1----
awt::Point SAL_CALL SvxShapeGroup::getPosition() throw(uno::RuntimeException)
{
    return SvxShape::getPosition();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeGroup::setPosition( const awt::Point& Position ) throw(uno::RuntimeException)
{
    SvxShape::setPosition(Position);
}

//----------------------------------------------------------------------

awt::Size SAL_CALL SvxShapeGroup::getSize() throw(uno::RuntimeException)
{
    return SvxShape::getSize();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeGroup::setSize( const awt::Size& rSize )
    throw(beans::PropertyVetoException, uno::RuntimeException)
{
    SvxShape::setSize( rSize );
}

// drawing::XShapeGroup

//----------------------------------------------------------------------
void SAL_CALL SvxShapeGroup::enterGroup(  ) throw(uno::RuntimeException)
{
    // Todo
//  pDrView->EnterMarkedGroup();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeGroup::leaveGroup(  ) throw(uno::RuntimeException)
{
    // Todo
//  pDrView->LeaveOneGroup();
}

//----------------------------------------------------------------------

// XShapes
void SAL_CALL SvxShapeGroup::add( const uno::Reference< drawing::XShape >& xShape )
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    SvxShape* pShape = SvxShape::getImplementation( xShape );

    if( mpObj.is()&& mxPage.is() && pShape )
    {
        SdrObject* pSdrShape = pShape->GetSdrObject();
        if( pSdrShape == NULL )
            pSdrShape = mxPage->_CreateSdrObject( xShape );

        if( pSdrShape->IsInserted() )
            pSdrShape->GetObjList()->RemoveObject( pSdrShape->GetOrdNum() );

        mpObj->GetSubList()->InsertObject( pSdrShape );
        pSdrShape->SetModel(mpObj->GetModel());

        // #85922# It makes no sense to set the layer asked
        // from the group object since these is an iteration
        // over the contained objects. In consequence, this
        // statement erases all layer information from the draw
        // objects. Layers need to be set at draw objects directly
        // and have nothing to do with grouping at all.
        // pSdrShape->SetLayer(pObject->GetLayer());

        // Establish connection between new SdrObject and its wrapper before
        // inserting the new shape into the group.  There a new wrapper
        // would be created when this connection would not already exist.
        if(pShape)
            pShape->Create( pSdrShape, mxPage.get() );

        if( mpModel )
            mpModel->SetChanged();
    }
    else
    {
        DBG_ERROR("could not add XShape to group shape!");
    }
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeGroup::remove( const uno::Reference< drawing::XShape >& xShape )
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    SdrObject* pSdrShape = NULL;
    SvxShape* pShape = SvxShape::getImplementation( xShape );

    if( pShape )
        pSdrShape = pShape->GetSdrObject();

    if( !mpObj.is() || pSdrShape == NULL || pSdrShape->GetObjList()->GetOwnerObj() != mpObj.get() )
        throw uno::RuntimeException();

    SdrObjList& rList = *pSdrShape->GetObjList();

    const sal_uInt32 nObjCount = rList.GetObjCount();
    sal_uInt32 nObjNum = 0;
    while( nObjNum < nObjCount )
    {
        if(rList.GetObj( nObjNum ) == pSdrShape )
            break;
        nObjNum++;
    }

    if( nObjNum < nObjCount )
    {
        // #i29181#
        // If the SdrObject which is about to be deleted is in any selection,
        // deselect it first.
        SdrViewIter aIter( pSdrShape );

        for ( SdrView* pView = aIter.FirstView(); pView; pView = aIter.NextView() )
        {
            if(CONTAINER_ENTRY_NOTFOUND != pView->TryToFindMarkedObject(pSdrShape))
            {
                pView->MarkObj(pSdrShape, pView->GetSdrPageView(), sal_True, sal_False);
            }
        }

        delete rList.NbcRemoveObject( nObjNum );
        pShape->InvalidateSdrObject();
    }
    else
    {
        DBG_ASSERT( 0, "Fatality! SdrObject is not belonging to its SdrObjList! [CL]" );
    }

    if( mpModel )
        mpModel->SetChanged();
}

// XIndexAccess

//----------------------------------------------------------------------
sal_Int32 SAL_CALL SvxShapeGroup::getCount() throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    sal_Int32 nRetval = 0;

    if(mpObj.is() && mpObj->GetSubList())
        nRetval = mpObj->GetSubList()->GetObjCount();
    else
        throw uno::RuntimeException();

    return nRetval;
}

//----------------------------------------------------------------------
uno::Any SAL_CALL SvxShapeGroup::getByIndex( sal_Int32 Index )
    throw( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    if( !mpObj.is() || mpObj->GetSubList() == NULL )
        throw uno::RuntimeException();

    if( mpObj->GetSubList()->GetObjCount() <= (sal_uInt32)Index )
        throw lang::IndexOutOfBoundsException();

    SdrObject* pDestObj = mpObj->GetSubList()->GetObj( Index );

    if(pDestObj == NULL)
        throw lang::IndexOutOfBoundsException();

    Reference< drawing::XShape > xShape( pDestObj->getUnoShape(), uno::UNO_QUERY );
    return uno::makeAny( xShape );
}

// ::com::sun::star::container::XElementAccess

//----------------------------------------------------------------------
uno::Type SAL_CALL SvxShapeGroup::getElementType() throw( uno::RuntimeException )
{
    return ::getCppuType(( const Reference< drawing::XShape >*)0);
}

//----------------------------------------------------------------------
sal_Bool SAL_CALL SvxShapeGroup::hasElements() throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    return mpObj.is() && mpObj->GetSubList() && (mpObj->GetSubList()->GetObjCount() > 0);
}

//----------------------------------------------------------------------
// ::com::sun::star::lang::XServiceInfo

uno::Sequence< OUString > SAL_CALL SvxShapeGroup::getSupportedServiceNames()
    throw(uno::RuntimeException)
{
    return SvxShape::getSupportedServiceNames();
}

/***********************************************************************
*                                                                      *
***********************************************************************/

SvxShapeConnector::SvxShapeConnector( SdrObject* pObj )  throw() :
    SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_CONNECTOR) )
{
}

//----------------------------------------------------------------------
SvxShapeConnector::~SvxShapeConnector() throw()
{
}

//----------------------------------------------------------------------

uno::Any SAL_CALL SvxShapeConnector::queryInterface( const uno::Type & rType )
    throw(uno::RuntimeException)
{
    return SvxShapeText::queryInterface( rType );
}

uno::Any SAL_CALL SvxShapeConnector::queryAggregation( const uno::Type & rType )
    throw(uno::RuntimeException)
{
    uno::Any aAny;

    QUERYINT( drawing::XConnectorShape );
    else
        return SvxShapeText::queryAggregation( rType );

    return aAny;
}

void SAL_CALL SvxShapeConnector::acquire() throw ( )
{
    SvxShapeText::acquire();
}

void SAL_CALL SvxShapeConnector::release() throw ( )
{
    SvxShapeText::release();
}
// XTypeProvider

uno::Sequence< uno::Type > SAL_CALL SvxShapeConnector::getTypes()
    throw (uno::RuntimeException)
{
    return SvxShape::getTypes();
}

uno::Sequence< sal_Int8 > SAL_CALL SvxShapeConnector::getImplementationId()
    throw (uno::RuntimeException)
{
    static uno::Sequence< sal_Int8 > aId;
    if( aId.getLength() == 0 )
    {
        aId.realloc( 16 );
        rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
    }
    return aId;
}

// ::com::sun::star::drawing::XShape

//----------------------------------------------------------------------
OUString SAL_CALL SvxShapeConnector::getShapeType()
    throw( uno::RuntimeException )
{
    return SvxShapeText::getShapeType();
}

//------------------------------------------------------------------1----
awt::Point SAL_CALL SvxShapeConnector::getPosition() throw(uno::RuntimeException)
{
    return SvxShapeText::getPosition();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeConnector::setPosition( const awt::Point& Position ) throw(uno::RuntimeException)
{
    SvxShapeText::setPosition(Position);
}

//----------------------------------------------------------------------

awt::Size SAL_CALL SvxShapeConnector::getSize() throw(uno::RuntimeException)
{
    return SvxShapeText::getSize();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeConnector::setSize( const awt::Size& rSize )
    throw(beans::PropertyVetoException, uno::RuntimeException)
{
    SvxShapeText::setSize( rSize );
}

//----------------------------------------------------------------------

// XConnectorShape

void SAL_CALL SvxShapeConnector::connectStart( const uno::Reference< drawing::XConnectableShape >& xShape, drawing::ConnectionType ) throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    Reference< drawing::XShape > xRef( xShape, UNO_QUERY );
    SvxShape* pShape = SvxShape::getImplementation( xRef );

    if( pShape )
        mpObj->ConnectToNode( sal_True, pShape->mpObj.get() );

    if( mpModel )
        mpModel->SetChanged();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeConnector::connectEnd( const uno::Reference< drawing::XConnectableShape >& xShape, drawing::ConnectionType  )
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    Reference< drawing::XShape > xRef( xShape, UNO_QUERY );
    SvxShape* pShape = SvxShape::getImplementation( xRef );

    if( mpObj.is() && pShape )
        mpObj->ConnectToNode( sal_False, pShape->mpObj.get() );

    if( mpModel )
        mpModel->SetChanged();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeConnector::disconnectBegin( const uno::Reference< drawing::XConnectableShape >&  )
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is())
        mpObj->DisconnectFromNode( sal_True );

    if( mpModel )
        mpModel->SetChanged();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeConnector::disconnectEnd( const uno::Reference< drawing::XConnectableShape >& )
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is())
        mpObj->DisconnectFromNode( sal_False );

    if( mpModel )
        mpModel->SetChanged();
}

//----------------------------------------------------------------------
// ::com::sun::star::lang::XServiceInfo
//----------------------------------------------------------------------
uno::Sequence< OUString > SAL_CALL SvxShapeConnector::getSupportedServiceNames() throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}

/***********************************************************************
* class SvxShapeControl                                                *
***********************************************************************/


SvxShapeControl::SvxShapeControl( SdrObject* pObj )  throw() :
    SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_CONTROL) )
{
    setShapeKind( OBJ_UNO );
}

//----------------------------------------------------------------------
SvxShapeControl::~SvxShapeControl() throw()
{
}

//----------------------------------------------------------------------
uno::Any SAL_CALL SvxShapeControl::queryInterface( const uno::Type & rType )
    throw(uno::RuntimeException)
{
    return SvxShapeText::queryInterface( rType );
}

uno::Any SAL_CALL SvxShapeControl::queryAggregation( const uno::Type & rType ) throw(uno::RuntimeException)
{
    uno::Any aAny;

    QUERYINT( drawing::XControlShape );
    else
        return SvxShapeText::queryAggregation( rType );

    return aAny;
}

void SAL_CALL SvxShapeControl::acquire() throw ( )
{
    SvxShapeText::acquire();
}

void SAL_CALL SvxShapeControl::release() throw ( )
{
    SvxShapeText::release();
}
// XTypeProvider

uno::Sequence< uno::Type > SAL_CALL SvxShapeControl::getTypes()
    throw (uno::RuntimeException)
{
    return SvxShape::getTypes();
}

uno::Sequence< sal_Int8 > SAL_CALL SvxShapeControl::getImplementationId()
    throw (uno::RuntimeException)
{
    static uno::Sequence< sal_Int8 > aId;
    if( aId.getLength() == 0 )
    {
        aId.realloc( 16 );
        rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
    }
    return aId;
}

// ::com::sun::star::drawing::XShape

//----------------------------------------------------------------------
OUString SAL_CALL SvxShapeControl::getShapeType()
    throw( uno::RuntimeException )
{
    return SvxShapeText::getShapeType();
}

//------------------------------------------------------------------1----
awt::Point SAL_CALL SvxShapeControl::getPosition() throw(uno::RuntimeException)
{
    return SvxShapeText::getPosition();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeControl::setPosition( const awt::Point& Position ) throw(uno::RuntimeException)
{
    SvxShapeText::setPosition(Position);
}

//----------------------------------------------------------------------

awt::Size SAL_CALL SvxShapeControl::getSize() throw(uno::RuntimeException)
{
    return SvxShapeText::getSize();
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeControl::setSize( const awt::Size& rSize )
    throw(beans::PropertyVetoException, uno::RuntimeException)
{
    SvxShapeText::setSize( rSize );
}

//----------------------------------------------------------------------
// XControlShape

Reference< awt::XControlModel > SAL_CALL SvxShapeControl::getControl()
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    Reference< awt::XControlModel > xModel;

    SdrUnoObj* pUnoObj = dynamic_cast< SdrUnoObj * >(mpObj.get());
    if( pUnoObj )
        xModel = pUnoObj->GetUnoControlModel();

    return xModel;
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapeControl::setControl( const Reference< awt::XControlModel >& xControl )
    throw( uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    SdrUnoObj* pUnoObj = dynamic_cast< SdrUnoObj * >(mpObj.get());
    if( pUnoObj )
        pUnoObj->SetUnoControlModel( xControl );

    if( mpModel )
        mpModel->SetChanged();
}

// XServiceInfo
uno::Sequence< OUString > SAL_CALL SvxShapeControl::getSupportedServiceNames() throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}

static struct
{
    const sal_Char* mpAPIName;
    sal_uInt16 mnAPINameLen;

    const sal_Char* mpFormName;
    sal_uInt16 mnFormNameLen;
}
SvxShapeControlPropertyMapping[] =
{
    // Warning: The first entry must be FontSlant because the any needs to be converted
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_POSTURE), MAP_CHAR_LEN("FontSlant")  }, //  const sal_Int16 => ::com::sun::star::awt::FontSlant
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_FONTNAME), MAP_CHAR_LEN("FontName") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_FONTSTYLENAME), MAP_CHAR_LEN("FontStyleName") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_FONTFAMILY), MAP_CHAR_LEN("FontFamily") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_FONTCHARSET), MAP_CHAR_LEN("FontCharset") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_HEIGHT), MAP_CHAR_LEN("FontHeight") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_FONTPITCH), MAP_CHAR_LEN("FontPitch" ) },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_WEIGHT), MAP_CHAR_LEN("FontWeight" ) },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_UNDERLINE), MAP_CHAR_LEN("FontUnderline") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_STRIKEOUT), MAP_CHAR_LEN("FontStrikeout") },
    { MAP_CHAR_LEN("CharKerning"), MAP_CHAR_LEN("FontKerning") },
    { MAP_CHAR_LEN("CharWordMode"), MAP_CHAR_LEN("FontWordLineMode" ) },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_CHAR_COLOR),   MAP_CHAR_LEN("TextColor") },
    { MAP_CHAR_LEN("CharRelief"),   MAP_CHAR_LEN("FontRelief") },
    { MAP_CHAR_LEN("CharUnderlineColor"),   MAP_CHAR_LEN("TextLineColor") },
    { MAP_CHAR_LEN(UNO_NAME_EDIT_PARA_ADJUST), MAP_CHAR_LEN("Align") },
    { MAP_CHAR_LEN("TextVerticalAdjust"), MAP_CHAR_LEN("VerticalAlign") },
    { MAP_CHAR_LEN("ControlBackground"), MAP_CHAR_LEN("BackgroundColor") },
    { MAP_CHAR_LEN("ControlSymbolColor"), MAP_CHAR_LEN("SymbolColor") },
    { MAP_CHAR_LEN("ControlBorder"), MAP_CHAR_LEN("Border") },
    { MAP_CHAR_LEN("ControlBorderColor"), MAP_CHAR_LEN("BorderColor") },
    { MAP_CHAR_LEN("ControlTextEmphasis"),  MAP_CHAR_LEN("FontEmphasisMark") },
    { NULL,0, NULL, 0 }
};

void SvxShapeControl::convertPropertyName( const OUString& rApiName, OUString& rInternalName )
{
    sal_uInt16 i = 0;
    while( SvxShapeControlPropertyMapping[i].mpAPIName )
    {
        if( rApiName.reverseCompareToAsciiL( SvxShapeControlPropertyMapping[i].mpAPIName, SvxShapeControlPropertyMapping[i].mnAPINameLen ) == 0 )
        {
            rInternalName = OUString( SvxShapeControlPropertyMapping[i].mpFormName, SvxShapeControlPropertyMapping[i].mnFormNameLen, RTL_TEXTENCODING_ASCII_US );
        }
        ++i;
    }
}

//added by BerryJia for fixing Bug102407 2002-11-04
static struct
{
    sal_Int16 nAPIValue;
    sal_Int16 nFormValue;
}
SvxShapeControlPropertyValueMapping[] =
{
    // note that order matters:
    // valueAlignToParaAdjust and valueParaAdjustToAlign search this map from the _beginning_
    // and use the first matching entry
    {style::ParagraphAdjust_LEFT,           (sal_Int16)awt::TextAlign::LEFT},
    {style::ParagraphAdjust_CENTER,         (sal_Int16)awt::TextAlign::CENTER},
    {style::ParagraphAdjust_RIGHT,          (sal_Int16)awt::TextAlign::RIGHT},
    {style::ParagraphAdjust_BLOCK,          (sal_Int16)awt::TextAlign::RIGHT},
    {style::ParagraphAdjust_STRETCH,        (sal_Int16)awt::TextAlign::LEFT},
    {-1,-1}
};

void SvxShapeControl::valueAlignToParaAdjust(Any& rValue)
{
    sal_Int16 nValue = sal_Int16();
    rValue >>= nValue;
    sal_uInt16 i = 0;
    while (-1 != SvxShapeControlPropertyValueMapping[i].nFormValue)
    {
        if (nValue == SvxShapeControlPropertyValueMapping[i].nFormValue)
        {
            rValue <<= (SvxShapeControlPropertyValueMapping[i].nAPIValue);
            return;
        }
        i++;
    }
}

void SvxShapeControl::valueParaAdjustToAlign(Any& rValue)
{
    sal_Int32 nValue = 0;
    rValue >>= nValue;
    sal_uInt16 i = 0;
    while (-1 != SvxShapeControlPropertyValueMapping[i].nAPIValue)
    {
        if ( nValue == SvxShapeControlPropertyValueMapping[i].nAPIValue)
        {
            rValue <<= (SvxShapeControlPropertyValueMapping[i].nFormValue);
            return;
        }
        i++;
    }
}

namespace
{
    void convertVerticalAdjustToVerticalAlign( Any& _rValue ) SAL_THROW( ( lang::IllegalArgumentException ) )
    {
        if ( !_rValue.hasValue() )
            return;

        drawing::TextVerticalAdjust eAdjust = drawing::TextVerticalAdjust_TOP;
        style::VerticalAlignment    eAlign  = style::VerticalAlignment_TOP;
        if ( !( _rValue >>= eAdjust ) )
            throw lang::IllegalArgumentException();
        switch ( eAdjust )
        {
        case drawing::TextVerticalAdjust_TOP:    eAlign = style::VerticalAlignment_TOP; break;
        case drawing::TextVerticalAdjust_BOTTOM: eAlign = style::VerticalAlignment_BOTTOM; break;
        default:                                 eAlign = style::VerticalAlignment_MIDDLE; break;
        }
        _rValue <<= eAlign;
    }

    void convertVerticalAlignToVerticalAdjust( Any& _rValue )
    {
        if ( !_rValue.hasValue() )
            return;
        style::VerticalAlignment    eAlign  = style::VerticalAlignment_TOP;
        drawing::TextVerticalAdjust eAdjust = drawing::TextVerticalAdjust_TOP;
        OSL_VERIFY( _rValue >>= eAlign );
        switch ( eAlign )
        {
        case style::VerticalAlignment_TOP:    eAdjust = drawing::TextVerticalAdjust_TOP; break;
        case style::VerticalAlignment_BOTTOM: eAdjust = drawing::TextVerticalAdjust_BOTTOM; break;
        default:                              eAdjust = drawing::TextVerticalAdjust_CENTER; break;
        }
        _rValue <<= eAdjust;
    }
}

void SAL_CALL SvxShapeControl::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException, com::sun::star::beans::PropertyVetoException, com::sun::star::lang::IllegalArgumentException)
{
    OUString aFormsName;
    convertPropertyName( aPropertyName, aFormsName );
    if( aFormsName.getLength() )
    {
        uno::Reference< beans::XPropertySet > xControl( getControl(), uno::UNO_QUERY );
        if( xControl.is() )
        {
            uno::Reference< beans::XPropertySetInfo > xInfo( xControl->getPropertySetInfo() );
            if( xInfo.is() && xInfo->hasPropertyByName( aFormsName ) )
            {
                uno::Any aConvertedValue( aValue );
                if ( aFormsName.equalsAscii( "FontSlant" ) )
                {
                    awt::FontSlant nSlant;
                    if( !(aValue >>= nSlant ) )
                        throw lang::IllegalArgumentException();
                    aConvertedValue <<= (sal_Int16)nSlant;
                }
                else if ( aFormsName.equalsAscii( "Align" ) )
                {
                    valueParaAdjustToAlign( aConvertedValue );
                }
                else if ( aFormsName.equalsAscii( "VerticalAlign" ) )
                {
                    convertVerticalAdjustToVerticalAlign( aConvertedValue );
                }

                xControl->setPropertyValue( aFormsName, aConvertedValue );
            }
        }
    }
    else
    {
        SvxShape::setPropertyValue( aPropertyName, aValue );
    }
}

uno::Any SAL_CALL SvxShapeControl::getPropertyValue( const OUString& aPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OUString aFormsName;
    convertPropertyName( aPropertyName, aFormsName );
    if( aFormsName.getLength() )
    {
        uno::Reference< beans::XPropertySet > xControl( getControl(), uno::UNO_QUERY );

        uno::Any aValue;
        if( xControl.is() )
        {
            uno::Reference< beans::XPropertySetInfo > xInfo( xControl->getPropertySetInfo() );
            if( xInfo.is() && xInfo->hasPropertyByName( aFormsName ) )
            {
                aValue = xControl->getPropertyValue( aFormsName );
                if ( aFormsName.equalsAscii( "FontSlant" ) )
                {
                    awt::FontSlant eSlant = awt::FontSlant_NONE;
                    sal_Int16 nSlant = sal_Int16();
                    if ( aValue >>= nSlant )
                    {
                        eSlant = (awt::FontSlant)nSlant;
                    }
                    else
                    {
                        OSL_VERIFY( aValue >>= eSlant );
                    }
                    aValue <<= eSlant;
                }
                else if ( aFormsName.equalsAscii( "Align" ) )
                {
                    valueAlignToParaAdjust( aValue );
                }
                else if ( aFormsName.equalsAscii( "VerticalAlign" ) )
                {
                    convertVerticalAlignToVerticalAdjust( aValue );
                }
            }
        }

        return aValue;
    }
    else
    {
        return SvxShape::getPropertyValue( aPropertyName );
    }

}

// XPropertyState
beans::PropertyState SAL_CALL SvxShapeControl::getPropertyState( const ::rtl::OUString& PropertyName ) throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    OUString aFormsName;
    convertPropertyName( PropertyName, aFormsName );
    if( aFormsName.getLength() )
    {
        uno::Reference< beans::XPropertyState > xControl( getControl(), uno::UNO_QUERY );
        uno::Reference< beans::XPropertySet > xPropSet( getControl(), uno::UNO_QUERY );

        if( xControl.is() && xPropSet.is() )
        {
            uno::Reference< beans::XPropertySetInfo > xInfo( xPropSet->getPropertySetInfo() );
            if( xInfo.is() && xInfo->hasPropertyByName( aFormsName ) )
            {
                return xControl->getPropertyState( aFormsName );
            }
        }

        return beans::PropertyState_DEFAULT_VALUE;
    }
    else
    {
        return SvxShape::getPropertyState( PropertyName );
    }
}

void SAL_CALL SvxShapeControl::setPropertyToDefault( const ::rtl::OUString& PropertyName ) throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    OUString aFormsName;
    convertPropertyName( PropertyName, aFormsName );
    if( aFormsName.getLength() )
    {
        uno::Reference< beans::XPropertyState > xControl( getControl(), uno::UNO_QUERY );
        uno::Reference< beans::XPropertySet > xPropSet( getControl(), uno::UNO_QUERY );

        if( xControl.is() && xPropSet.is() )
        {
            uno::Reference< beans::XPropertySetInfo > xInfo( xPropSet->getPropertySetInfo() );
            if( xInfo.is() && xInfo->hasPropertyByName( aFormsName ) )
            {
                xControl->setPropertyToDefault( aFormsName );
            }
        }
    }
    else
    {
        SvxShape::setPropertyToDefault( PropertyName );
    }
}

uno::Any SAL_CALL SvxShapeControl::getPropertyDefault( const ::rtl::OUString& aPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    OUString aFormsName;
    convertPropertyName( aPropertyName, aFormsName );
    if( aFormsName.getLength() )
    {
        uno::Reference< beans::XPropertyState > xControl( getControl(), uno::UNO_QUERY );

        if( xControl.is() )
        {
            Any aDefault( xControl->getPropertyDefault( aFormsName ) );
            if ( aFormsName.equalsAscii( "FontSlant" ) )
            {
                sal_Int16 nSlant( 0 );
                aDefault >>= nSlant;
                aDefault <<= (awt::FontSlant)nSlant;
            }
            else if ( aFormsName.equalsAscii( "Align" ) )
            {
                valueAlignToParaAdjust( aDefault );
            }
            else if ( aFormsName.equalsAscii( "VerticalAlign" ) )
            {
                convertVerticalAlignToVerticalAdjust( aDefault );
            }
            return aDefault;
        }

        throw beans::UnknownPropertyException();
    }
    else
    {
        return SvxShape::getPropertyDefault( aPropertyName );
    }
}


/***********************************************************************
* class SvxShapeDimensioning                                           *
***********************************************************************/

//----------------------------------------------------------------------
SvxShapeDimensioning::SvxShapeDimensioning( SdrObject* pObj ) throw()
:   SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_DIMENSIONING) )
{
}

//----------------------------------------------------------------------
SvxShapeDimensioning::~SvxShapeDimensioning() throw()
{
}

// ::com::sun::star::lang::XServiceInfo
uno::Sequence< OUString > SAL_CALL SvxShapeDimensioning::getSupportedServiceNames() throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}

/***********************************************************************
*                                                                      *
***********************************************************************/

//----------------------------------------------------------------------
SvxShapeCircle::SvxShapeCircle( SdrObject* pObj ) throw()
:   SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_CIRCLE) )
{
}

//----------------------------------------------------------------------
SvxShapeCircle::~SvxShapeCircle() throw()
{
}

// ::com::sun::star::lang::XServiceInfo
// XServiceInfo
uno::Sequence< OUString > SAL_CALL SvxShapeCircle::getSupportedServiceNames() throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}

/***********************************************************************
*                                                                      *
***********************************************************************/

#include "svdopath.hxx"

//----------------------------------------------------------------------
SvxShapePolyPolygon::SvxShapePolyPolygon( SdrObject* pObj , drawing::PolygonKind eNew )
 throw( com::sun::star::beans::PropertyVetoException, com::sun::star::lang::IllegalArgumentException)
: SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_POLYPOLYGON) )
, mePolygonKind( eNew )
{
}

//----------------------------------------------------------------------
SvxShapePolyPolygon::~SvxShapePolyPolygon() throw()
{
}

basegfx::B2DPolyPolygon SAL_CALL ImplSvxPointSequenceSequenceToB2DPolyPolygon( const drawing::PointSequenceSequence* pOuterSequence) throw()
{
    basegfx::B2DPolyPolygon aRetval;

    // Zeiger auf innere sequences holen
    const drawing::PointSequence* pInnerSequence = pOuterSequence->getConstArray();
    const drawing::PointSequence* pInnerSeqEnd   = pInnerSequence + pOuterSequence->getLength();

    for(;pInnerSequence != pInnerSeqEnd; ++pInnerSequence)
    {
        // Neues Polygon vorbereiten
        basegfx::B2DPolygon aNewPolygon;

        // Zeiger auf Arrays holen
        const awt::Point* pArray    = pInnerSequence->getConstArray();
        const awt::Point* pArrayEnd = pArray + pInnerSequence->getLength();

        for(;pArray != pArrayEnd;++pArray)
        {
            aNewPolygon.append(basegfx::B2DPoint(pArray->X, pArray->Y));
        }

        // check for closed state flag
        basegfx::tools::checkClosed(aNewPolygon);

        // Neues Teilpolygon einfuegen
        aRetval.append(aNewPolygon);
    }

    return aRetval;
}

//----------------------------------------------------------------------

void SAL_CALL SvxShapePolyPolygon::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException, com::sun::star::beans::PropertyVetoException, com::sun::star::lang::IllegalArgumentException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    if( aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYPOLYGON)))
    {
        if( !aValue.getValue() || aValue.getValueType() != ::getCppuType((const drawing::PointSequenceSequence*)0) )
            throw lang::IllegalArgumentException();

        basegfx::B2DPolyPolygon aNewPolyPolygon(ImplSvxPointSequenceSequenceToB2DPolyPolygon( (drawing::PointSequenceSequence*)aValue.getValue()));
        SetPolygon(aNewPolyPolygon);
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("Geometry")))
    {
        if( !aValue.getValue() || aValue.getValueType() != ::getCppuType((const drawing::PointSequenceSequence*)0) )
            throw lang::IllegalArgumentException();

        if( mpObj.is() )
        {
            basegfx::B2DPolyPolygon aNewPolyPolygon;
            basegfx::B2DHomMatrix aNewHomogenMatrix;

            mpObj->TRGetBaseGeometry(aNewHomogenMatrix, aNewPolyPolygon);
            aNewPolyPolygon = ImplSvxPointSequenceSequenceToB2DPolyPolygon((drawing::PointSequenceSequence*)aValue.getValue());
            mpObj->TRSetBaseGeometry(aNewHomogenMatrix, aNewPolyPolygon);
        }
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYGON)))
    {
        drawing::PointSequence* pSequence = (drawing::PointSequence*)aValue.getValue();

        if( !pSequence || aValue.getValueType() != ::getCppuType((const drawing::PointSequence*)0 ) )
            throw lang::IllegalArgumentException();

        // Neues Polygon vorbereiten
        basegfx::B2DPolygon aNewPolygon;

        // Zeiger auf Arrays holen
        // Zeiger auf Arrays holen
        const awt::Point* pArray    = pSequence->getConstArray();
        const awt::Point* pArrayEnd = pArray + pSequence->getLength();

        for(;pArray != pArrayEnd;++pArray)
        {
            aNewPolygon.append(basegfx::B2DPoint(pArray->X, pArray->Y));
        }

        // check for closed state flag
        basegfx::tools::checkClosed(aNewPolygon);

        // Polygon setzen
        SetPolygon(basegfx::B2DPolyPolygon(aNewPolygon));
    }
    else
    {
        SvxShape::setPropertyValue(aPropertyName, aValue);
    }

    if( mpModel )
        mpModel->SetChanged();
}

void SAL_CALL B2DPolyPolygonToSvxPointSequenceSequence( const basegfx::B2DPolyPolygon& rPolyPoly, drawing::PointSequenceSequence& rRetval )
{
    if( (sal_uInt32)rRetval.getLength() != rPolyPoly.count() )
        rRetval.realloc( rPolyPoly.count() );

    // Zeiger auf aeussere Arrays holen
    drawing::PointSequence* pOuterSequence = rRetval.getArray();

    for(sal_uInt32 a(0L); a < rPolyPoly.count(); a++)
    {
        // Einzelpolygon holen
        const basegfx::B2DPolygon aPoly(rPolyPoly.getB2DPolygon(a));

        // #i75974# take closed stae into account, the API polygon still uses the old closed definition
        // with last/first point are identical (cannot hold information about open polygons with identical
        // first and last point, though)
        const sal_uInt32 nPointCount(aPoly.count());
        const bool bIsClosed(aPoly.isClosed());

        // Platz in Arrays schaffen
        pOuterSequence->realloc(bIsClosed ? nPointCount + 1 : nPointCount);

        // Pointer auf arrays holen
        awt::Point* pInnerSequence = pOuterSequence->getArray();

        for(sal_uInt32 b(0L); b < nPointCount; b++)
        {
            const basegfx::B2DPoint aPoint(aPoly.getB2DPoint(b));
            *pInnerSequence = awt::Point( basegfx::fround(aPoint.getX()), basegfx::fround(aPoint.getY()) );
            pInnerSequence++;
        }

        // #i75974# copy first point
        if(bIsClosed)
        {
            *pInnerSequence = *pOuterSequence->getArray();
        }

        pOuterSequence++;
    }
}

//----------------------------------------------------------------------

uno::Any SAL_CALL SvxShapePolyPolygon::getPropertyValue( const OUString& aPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYPOLYGON)))
    {
        // PolyPolygon in eine struct PolyPolygon packen
        const basegfx::B2DPolyPolygon& rPolyPoly = GetPolygon();
        drawing::PointSequenceSequence aRetval( rPolyPoly.count() );

        B2DPolyPolygonToSvxPointSequenceSequence( rPolyPoly, aRetval );

        return uno::Any( &aRetval, ::getCppuType((const drawing::PointSequenceSequence*)0) );
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("Geometry")))
    {
        // pack a PolyPolygon in struct PolyPolygon
        basegfx::B2DPolyPolygon aNewPolyPolygon;
        basegfx::B2DHomMatrix aNewHomogenMatrix;
        SdrObject* pObject = mpObj.get();

        if(pObject)
        {
            mpObj->TRGetBaseGeometry(aNewHomogenMatrix, aNewPolyPolygon);
        }

        drawing::PointSequenceSequence aRetval(aNewPolyPolygon.count());
        B2DPolyPolygonToSvxPointSequenceSequence(aNewPolyPolygon, aRetval);
        return uno::Any( aRetval );
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYGON)))
    {
        // PolyPolygon in eine struct PolyPolygon packen
        const basegfx::B2DPolyPolygon& rPolyPoly = GetPolygon();

        sal_Int32 nCount = 0;
        if( rPolyPoly.count() > 0 )
        {
            nCount = rPolyPoly.getB2DPolygon(0L).count();
        }

        drawing::PointSequence aRetval( nCount );

        if( nCount > 0 )
        {
            // Einzelpolygon holen
            const basegfx::B2DPolygon aPoly(rPolyPoly.getB2DPolygon(0L));

            // Pointer auf arrays holen
            awt::Point* pSequence = aRetval.getArray();

            for(sal_Int32 b=0;b<nCount;b++)
            {
                const basegfx::B2DPoint aPoint(aPoly.getB2DPoint(b));
                *pSequence++ = awt::Point( basegfx::fround(aPoint.getX()), basegfx::fround(aPoint.getY()) );
            }
        }

        return uno::Any( aRetval );
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYGONKIND)))
    {
        return Any( GetPolygonKind() );
    }
    else
    {
        return SvxShape::getPropertyValue(aPropertyName);
    }
}

//----------------------------------------------------------------------
drawing::PolygonKind SvxShapePolyPolygon::GetPolygonKind() const throw()
{
    return mePolygonKind;
}

//----------------------------------------------------------------------
void SvxShapePolyPolygon::SetPolygon(const basegfx::B2DPolyPolygon& rNew) throw()
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is())
        ((SdrPathObj*)mpObj.get())->SetPathPoly(rNew);
}

//----------------------------------------------------------------------
basegfx::B2DPolyPolygon SvxShapePolyPolygon::GetPolygon() const throw()
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is())
    {
        return ((SdrPathObj*)mpObj.get())->GetPathPoly();
    }
    else
    {
        return basegfx::B2DPolyPolygon();
    }
}

// ::com::sun::star::lang::XServiceInfo
uno::Sequence< OUString > SAL_CALL SvxShapePolyPolygon::getSupportedServiceNames() throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}

/***********************************************************************
* class SvxShapePolyPolygonBezier                                      *
***********************************************************************/

#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONBEZIERCOORDS_HPP_
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_FLAGSEQUENCE_HPP_
#include <com/sun/star/drawing/FlagSequence.hpp>
#endif
//----------------------------------------------------------------------
SvxShapePolyPolygonBezier::SvxShapePolyPolygonBezier( SdrObject* pObj , drawing::PolygonKind eNew ) throw()
:   SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_POLYPOLYGONBEZIER) )
,   mePolygonKind( eNew )
{
}

//----------------------------------------------------------------------
SvxShapePolyPolygonBezier::~SvxShapePolyPolygonBezier() throw()
{
}

basegfx::B2DPolyPolygon SvxConvertPolyPolygonBezierToB2DPolyPolygon( const drawing::PolyPolygonBezierCoords* pSourcePolyPolygon)
    throw( IllegalArgumentException )
{
    basegfx::B2DPolyPolygon aNewPolyPolygon;
    sal_Int32 nOuterSequenceCount = pSourcePolyPolygon->Coordinates.getLength();
    if(pSourcePolyPolygon->Flags.getLength() != nOuterSequenceCount)
        throw IllegalArgumentException();

    // Zeiger auf innere sequences holen
    const drawing::PointSequence* pInnerSequence = pSourcePolyPolygon->Coordinates.getConstArray();
    const drawing::FlagSequence* pInnerSequenceFlags = pSourcePolyPolygon->Flags.getConstArray();
    sal_Bool bIsCurve(FALSE);
    sal_Bool bCurveValid(TRUE);
    sal_Bool bCurveTestActive(FALSE);

    for(sal_Int32 a(0); bCurveValid && a < nOuterSequenceCount; a++)
    {
        sal_Int32 nInnerSequenceCount = pInnerSequence->getLength();

        if(pInnerSequenceFlags->getLength() != nInnerSequenceCount)
            throw IllegalArgumentException();

        // Neues XPolygon vorbereiten
        basegfx::B2DPolygon aNewPolygon;
        sal_uInt32 nLastPointIndex(0L);
        bool bFirstControlPointSet(false);

        // Zeiger auf Arrays holen
        const awt::Point* pArray = pInnerSequence->getConstArray();
        const drawing::PolygonFlags* pArrayFlags = pInnerSequenceFlags->getConstArray();

        for(sal_Int32 b(0); bCurveValid && b < nInnerSequenceCount; b++)
        {
            // coordinate data
            const basegfx::B2DPoint aNewCoordinatePair(pArray->X, pArray->Y);
            pArray++;

            // flag data
            XPolyFlags ePolyFlag = (XPolyFlags)((sal_uInt16)*pArrayFlags++);

            // set curve flag
            if(!bIsCurve && ePolyFlag == XPOLY_CONTROL)
                bIsCurve = TRUE;

            // curve testing
            if(bIsCurve && bCurveValid && (bCurveTestActive || ePolyFlag == XPOLY_CONTROL))
            {
                if(!bCurveTestActive)
                {
                    // first control point found, test it
                    if(b == 0)
                    {
                        // no curve startpoint possible
                        bCurveValid = FALSE;
                    }
                    else
                    {
                        // test type of prev point
                        XPolyFlags ePrevPolyFlag = (XPolyFlags)((sal_uInt16)*(pArrayFlags - 2));
                        if(ePrevPolyFlag == XPOLY_CONTROL)
                        {
                            // curve startpoint is a control point,
                            // this is not allowed (three control points in a row)
                            bCurveValid = FALSE;
                        }
                    }

                    // next curve test state
                    bCurveTestActive = TRUE;
                }
                else
                {
                    // prev was a valid curve start, this should be the second
                    // curve control point, test it
                    if(ePolyFlag != XPOLY_CONTROL)
                    {
                        // no second curve control point
                        bCurveValid = FALSE;
                    }
                    else if(b == nInnerSequenceCount-1)
                    {
                        // no curve endpoint possible
                        bCurveValid = FALSE;
                    }
                    else
                    {
                        // test type of next point
                        XPolyFlags eNextPolyFlag = (XPolyFlags)((sal_uInt16)*pArrayFlags);
                        if(eNextPolyFlag == XPOLY_CONTROL)
                        {
                            // curve endpoint is the next control point,
                            // this is not allowed (three control points in a row)
                            bCurveValid = FALSE;
                        }
                    }

                    // end curve test for this segment
                    bCurveTestActive = FALSE;
                }
            }

            if(ePolyFlag == XPOLY_CONTROL)
            {
                if(bFirstControlPointSet)
                {
                    aNewPolygon.setControlPointB(nLastPointIndex, aNewCoordinatePair);
                    bFirstControlPointSet = false;
                }
                else
                {
                    aNewPolygon.setControlPointA(nLastPointIndex, aNewCoordinatePair);
                    bFirstControlPointSet = true;
                }
            }
            else
            {
                // add coordinate
                nLastPointIndex = aNewPolygon.count();
                bFirstControlPointSet = false;
                aNewPolygon.append(aNewCoordinatePair);
            }
        }

        pInnerSequence++;
        pInnerSequenceFlags++;

        // #i72807# API import uses old line start/end-equal definition for closed,
        // so we need to correct this to closed state here
        basegfx::tools::checkClosed(aNewPolygon);

        // Neues Teilpolygon einfuegen
        aNewPolyPolygon.append(aNewPolygon);
    }

    // throw exception if polygon data is an invalid curve definition
    if(bIsCurve && !bCurveValid)
        throw IllegalArgumentException();

    return aNewPolyPolygon;
}

//----------------------------------------------------------------------
void SAL_CALL SvxShapePolyPolygonBezier::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException, com::sun::star::beans::PropertyVetoException, com::sun::star::lang::IllegalArgumentException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYPOLYGONBEZIER)))
    {
        if( !aValue.getValue() || aValue.getValueType() != ::getCppuType((const drawing::PolyPolygonBezierCoords*)0) )
            throw IllegalArgumentException();

        basegfx::B2DPolyPolygon aNewPolyPolygon(SvxConvertPolyPolygonBezierToB2DPolyPolygon( (drawing::PolyPolygonBezierCoords*)aValue.getValue()));
        SetPolygon(aNewPolyPolygon);
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("Geometry")))
    {
        if( !aValue.getValue() || aValue.getValueType() != ::getCppuType((const drawing::PolyPolygonBezierCoords*)0) )
            throw IllegalArgumentException();

        if( mpObj.is() )
        {
            basegfx::B2DPolyPolygon aNewPolyPolygon;
            basegfx::B2DHomMatrix aNewHomogenMatrix;

            mpObj->TRGetBaseGeometry(aNewHomogenMatrix, aNewPolyPolygon);
            aNewPolyPolygon = SvxConvertPolyPolygonBezierToB2DPolyPolygon((drawing::PolyPolygonBezierCoords*)aValue.getValue());
            mpObj->TRSetBaseGeometry(aNewHomogenMatrix, aNewPolyPolygon);
        }
    }
    else
    {
        SvxShape::setPropertyValue(aPropertyName, aValue);
    }

    if( mpModel )
        mpModel->SetChanged();
}

void SvxConvertB2DPolyPolygonToPolyPolygonBezier( const basegfx::B2DPolyPolygon& rPolyPoly, drawing::PolyPolygonBezierCoords& rRetval )
{
    const PolyPolygon aPolyPoly(rPolyPoly);

    // Polygone innerhalb vrobereiten
    rRetval.Coordinates.realloc((sal_Int32)aPolyPoly.Count());
    rRetval.Flags.realloc((sal_Int32)aPolyPoly.Count());

    // Zeiger auf aeussere Arrays holen
    drawing::PointSequence* pOuterSequence = rRetval.Coordinates.getArray();
    drawing::FlagSequence*  pOuterFlags = rRetval.Flags.getArray();

    for(sal_uInt16 a=0;a<aPolyPoly.Count();a++)
    {
        // Einzelpolygon holen
        const Polygon& rPoly = aPolyPoly[a];

        // Platz in Arrays schaffen
        pOuterSequence->realloc((sal_Int32)rPoly.GetSize());
        pOuterFlags->realloc((sal_Int32)rPoly.GetSize());

        // Pointer auf arrays holen
        awt::Point* pInnerSequence = pOuterSequence->getArray();
        drawing::PolygonFlags* pInnerFlags = pOuterFlags->getArray();

        for(sal_uInt16 b=0;b<rPoly.GetSize();b++)
        {
            *pInnerSequence++ = awt::Point( rPoly[b].X(), rPoly[b].Y() );
            *pInnerFlags++ = (drawing::PolygonFlags)((sal_uInt16)rPoly.GetFlags(b));
        }

        pOuterSequence++;
        pOuterFlags++;
    }
}

//----------------------------------------------------------------------
uno::Any SAL_CALL SvxShapePolyPolygonBezier::getPropertyValue( const OUString& aPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYPOLYGONBEZIER)))
    {
        // PolyPolygon in eine struct PolyPolygon packen
        const basegfx::B2DPolyPolygon& rPolyPoly = GetPolygon();
        drawing::PolyPolygonBezierCoords aRetval;
        SvxConvertB2DPolyPolygonToPolyPolygonBezier(rPolyPoly, aRetval );

        return uno::Any( aRetval );
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("Geometry")))
    {
        if( !mpObj.is() )
            throw lang::DisposedException();

        // PolyPolygon in eine struct PolyPolygon packen
        basegfx::B2DPolyPolygon aNewPolyPolygon;
        basegfx::B2DHomMatrix aNewHomogenMatrix;
        mpObj.get()->TRGetBaseGeometry(aNewHomogenMatrix, aNewPolyPolygon);
        drawing::PolyPolygonBezierCoords aRetval;
        SvxConvertB2DPolyPolygonToPolyPolygonBezier(aNewPolyPolygon, aRetval);

        return uno::Any( aRetval );
    }
    else if(aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_POLYGONKIND)))
    {
        return uno::Any( mePolygonKind );
    }
    else
    {
        return SvxShape::getPropertyValue(aPropertyName);
    }
}

//----------------------------------------------------------------------
drawing::PolygonKind SvxShapePolyPolygonBezier::GetPolygonKind() const throw()
{
    return mePolygonKind;
}

//----------------------------------------------------------------------
void SvxShapePolyPolygonBezier::SetPolygon(const basegfx::B2DPolyPolygon& rNew) throw()
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is())
        ((SdrPathObj*)mpObj.get())->SetPathPoly(rNew);
}

//----------------------------------------------------------------------
basegfx::B2DPolyPolygon SvxShapePolyPolygonBezier::GetPolygon() const throw()
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is())
    {
        return ((SdrPathObj*)mpObj.get())->GetPathPoly();
    }
    else
    {
        return basegfx::B2DPolyPolygon();
    }
}


// ::com::sun::star::lang::XServiceInfo
uno::Sequence< OUString > SAL_CALL SvxShapePolyPolygonBezier::getSupportedServiceNames() throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}

/***********************************************************************
* class SvxGraphicObject                                               *
***********************************************************************/

#ifndef _COM_SUN_STAR_AWT_XBITMAP_HPP_
#include <com/sun/star/awt/XBitmap.hpp>
#endif
#ifndef _SV_CVTGRF_HXX
#include <vcl/cvtgrf.hxx>
#endif
#ifndef _SVDOGRAF_HXX
#include <svdograf.hxx>
#endif
#ifndef SVX_LIGHT
#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _SFX_FCONTNR_HXX
#include <sfx2/fcontnr.hxx>
#endif
#endif

#include "toolkit/unohlp.hxx"

//----------------------------------------------------------------------
SvxGraphicObject::SvxGraphicObject( SdrObject* pObj ) throw()
:   SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_GRAPHICOBJECT) )
{
}

//----------------------------------------------------------------------
SvxGraphicObject::~SvxGraphicObject() throw()
{
}

//----------------------------------------------------------------------
void SAL_CALL SvxGraphicObject::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException, com::sun::star::beans::PropertyVetoException, com::sun::star::lang::IllegalArgumentException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_FILLBITMAP)))
    {
        if( aValue.getValueType() == ::getCppuType(( const uno::Sequence< sal_Int8 >*)0) )
        {
            uno::Sequence<sal_Int8>* pSeq( (uno::Sequence<sal_Int8>*)aValue.getValue() );
            SvMemoryStream  aMemStm;
            Graphic         aGraphic;

            aMemStm.SetBuffer( (char*)pSeq->getConstArray(), pSeq->getLength(), sal_False, pSeq->getLength() );

            if( GraphicConverter::Import( aMemStm, aGraphic ) == ERRCODE_NONE )
            {
                ((SdrGrafObj*)mpObj.get())->SetGraphic(aGraphic);
            }
        }
        else if( (aValue.getValueType() == awt::XBitmap::static_type()) || (aValue.getValueType() == graphic::XGraphic::static_type()))
        {
            Reference< graphic::XGraphic> xGraphic( aValue, UNO_QUERY );
            if( xGraphic.is() )
            {
                ((SdrGrafObj*)mpObj.get())->SetGraphic(Graphic(xGraphic));
            }
            else
            {
                // Bitmap in das Objekt packen
                Reference< awt::XBitmap > xBmp( aValue, UNO_QUERY );
                if( xBmp.is() )
                {
                    // Bitmap einsetzen
                    Graphic aGraphic(VCLUnoHelper::GetBitmap( xBmp ));
                    ((SdrGrafObj*)mpObj.get())->SetGraphic(aGraphic);
                }
            }
        }
    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_GRAFURL)))
    {
        OUString aURL;
        if(!(aValue >>= aURL))
            throw lang::IllegalArgumentException();

        if( aURL.compareToAscii( UNO_NAME_GRAPHOBJ_URLPREFIX, RTL_CONSTASCII_LENGTH( UNO_NAME_GRAPHOBJ_URLPREFIX ) ) == 0 )
        {
            // graphic manager url
            aURL = aURL.copy( sizeof( UNO_NAME_GRAPHOBJ_URLPREFIX ) - 1 );
            String aTmpStr(aURL);
            ByteString aUniqueID( aTmpStr, RTL_TEXTENCODING_UTF8 );
            GraphicObject aGrafObj( aUniqueID );

            // #101808# since loading a graphic can cause a reschedule of the office
            //          it is possible that our shape is removed while where in this
            //          method.
            if( mpObj.is() )
            {
                ((SdrGrafObj*)mpObj.get())->ReleaseGraphicLink();
                ((SdrGrafObj*)mpObj.get())->SetGraphicObject( aGrafObj );
            }
        }
        else if( aURL.compareToAscii( UNO_NAME_GRAPHOBJ_URLPKGPREFIX, RTL_CONSTASCII_LENGTH( UNO_NAME_GRAPHOBJ_URLPKGPREFIX ) ) != 0 )
        {
            // normal link
            String              aFilterName;
            const SfxFilter*    pSfxFilter = NULL;
            SfxMedium           aSfxMedium( aURL, STREAM_READ | STREAM_SHARE_DENYNONE, FALSE );

            SFX_APP()->GetFilterMatcher().GuessFilter( aSfxMedium, &pSfxFilter, SFX_FILTER_IMPORT, SFX_FILTER_NOTINSTALLED | SFX_FILTER_EXECUTABLE );

            if( !pSfxFilter )
            {
                INetURLObject aURLObj( aURL );

                if( aURLObj.GetProtocol() == INET_PROT_NOT_VALID )
                {
                    String aValidURL;

                    if( ::utl::LocalFileHelper::ConvertPhysicalNameToURL( aURL, aValidURL ) )
                        aURLObj = INetURLObject( aValidURL );
                }

                if( aURLObj.GetProtocol() != INET_PROT_NOT_VALID )
                {
                    GraphicFilter* pGrfFilter = GetGrfFilter();
                    aFilterName = pGrfFilter->GetImportFormatName( pGrfFilter->GetImportFormatNumberForShortName( aURLObj.getExtension() ) );
                }
            }
            else
                aFilterName = pSfxFilter->GetFilterName();

            // #101808# since loading a graphic can cause a reschedule of the office
            //          it is possible that our shape is removed while where in this
            //          method.
            if( mpObj.is() )
                ((SdrGrafObj*)mpObj.get())->SetGraphicLink( aURL, aFilterName );
        }
        else
        {
        }

    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_GRAFSTREAMURL)))
    {
        OUString aStreamURL;

        if( !( aValue >>= aStreamURL ) )
            throw lang::IllegalArgumentException();


        if( aStreamURL.compareToAscii( UNO_NAME_GRAPHOBJ_URLPKGPREFIX, RTL_CONSTASCII_LENGTH( UNO_NAME_GRAPHOBJ_URLPKGPREFIX ) ) != 0 )
        {
            aStreamURL = OUString();
        }

        ((SdrGrafObj*)mpObj.get())->SetGrafStreamURL( aStreamURL );
        ((SdrGrafObj*)mpObj.get())->ForceSwapOut();
    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_GRAPHIC)))
    {
        Reference< graphic::XGraphic > xGraphic( aValue, UNO_QUERY );

        if( !xGraphic.is() )
            throw lang::IllegalArgumentException();

        static_cast< SdrGrafObj*>( mpObj.get() )->SetGraphic( xGraphic );
    }
    else
    {
        SvxShape::setPropertyValue(aPropertyName, aValue);
    }
    if( mpModel )
        mpModel->SetChanged();
}

//----------------------------------------------------------------------
uno::Any SAL_CALL SvxGraphicObject::getPropertyValue( const OUString& aPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    if(mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_FILLBITMAP)))
    {
        const Graphic& rGraphic = ((SdrGrafObj*)mpObj.get())->GetGraphic();

        if(rGraphic.GetType() != GRAPHIC_GDIMETAFILE)
        {
            // Objekt in eine Bitmap packen
            Reference< ::com::sun::star::awt::XBitmap >  xBitmap( VCLUnoHelper::CreateBitmap(((SdrGrafObj*)mpObj.get())->GetGraphic().GetBitmapEx()) );
            return uno::Any( xBitmap );
        }
        else
        {
            SvMemoryStream aDestStrm( 65535, 65535 );

            ConvertGDIMetaFileToWMF( rGraphic.GetGDIMetaFile(), aDestStrm, NULL, sal_False );
            uno::Sequence<sal_Int8> aSeq((sal_Int8*)aDestStrm.GetData(), aDestStrm.GetSize());
            return uno::Any( aSeq );
        }
    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_GRAFURL)) )
    {
        if( ((SdrGrafObj*)mpObj.get())->IsLinkedGraphic() )
        {
            return uno::Any( OUString( ((SdrGrafObj*)mpObj.get())->GetFileName() ) );
        }
        else
        {
            const GraphicObject& rGrafObj = ((SdrGrafObj*)mpObj.get())->GetGraphicObject();
            OUString aURL( RTL_CONSTASCII_USTRINGPARAM(UNO_NAME_GRAPHOBJ_URLPREFIX));
            aURL += OUString::createFromAscii( rGrafObj.GetUniqueID().GetBuffer() );
            return uno::Any( aURL );
        }
    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_GRAFSTREAMURL)) )
    {
        const OUString  aStreamURL( ( (SdrGrafObj*) mpObj.get() )->GetGrafStreamURL() );
        uno::Any aAny;

        if( aStreamURL.getLength() )
            aAny <<= aStreamURL;

        return aAny;
    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(UNO_NAME_GRAPHOBJ_GRAPHIC)) )
    {
        Reference< graphic::XGraphic > xGraphic( static_cast< SdrGrafObj* >( mpObj.get() )->GetGraphic().GetXGraphic() );
        return uno::Any( xGraphic );
    }
    else if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("GraphicStream" ) ) )
    {
        return Any( static_cast< SdrGrafObj* >( mpObj.get() )->getInputStream() );
    }
    else
    {
        return SvxShape::getPropertyValue(aPropertyName);
    }
}

// ::com::sun::star::lang::XServiceInfo
uno::Sequence< OUString > SAL_CALL SvxGraphicObject::getSupportedServiceNames()
    throw( uno::RuntimeException )
{
    return SvxShapeText::getSupportedServiceNames();
}


//basegfx::B2DPolygon SvxConvertPolyPolygonBezierToB2DPolygon( const drawing::PolyPolygonBezierCoords* pSourcePolyPolygon)
//  throw( lang::IllegalArgumentException )
//{
//  sal_Int32 nOuterSequenceCount = pSourcePolyPolygon->Coordinates.getLength();
//  if( nOuterSequenceCount != 1 || pSourcePolyPolygon->Flags.getLength() != nOuterSequenceCount)
//      throw lang::IllegalArgumentException();
//
//  // Zeiger auf innere sequences holen
//  const drawing::PointSequence* pInnerSequence = pSourcePolyPolygon->Coordinates.getConstArray();
//  const drawing::FlagSequence* pInnerSequenceFlags = pSourcePolyPolygon->Flags.getConstArray();
//
//  sal_Int32 nInnerSequenceCount = pInnerSequence->getLength();
//
//  if(pInnerSequenceFlags->getLength() != nInnerSequenceCount)
//      throw lang::IllegalArgumentException();
//
//  // Zeiger auf Arrays holen
//  const awt::Point* pArray = pInnerSequence->getConstArray();
//  const drawing::PolygonFlags* pArrayFlags = pInnerSequenceFlags->getConstArray();
//  XPolygon aNewPolygon;
//
//  for(sal_Int32 b=0;b<nInnerSequenceCount;b++)
//  {
//      aNewPolygon[(USHORT)b] = Point( pArray->X, pArray->Y );
//      pArray++;
//      aNewPolygon.SetFlags((USHORT)b, (XPolyFlags)((sal_uInt16)*pArrayFlags++));
//  }
//
//  return aNewPolygon.getB2DPolygon();
//}

//void SvxConvertB2DPolygonToPolyPolygonBezier( const basegfx::B2DPolygon& rPolygon, drawing::PolyPolygonBezierCoords& rRetval ) throw()
//{
//  const XPolygon aPolygon(rPolygon);
//
//  // Polygone innerhalb vrobereiten
//  rRetval.Coordinates.realloc(1);
//  rRetval.Flags.realloc(1);
//
//  // Zeiger auf aeussere Arrays holen
//  drawing::PointSequence* pOuterSequence = rRetval.Coordinates.getArray();
//  drawing::FlagSequence*  pOuterFlags = rRetval.Flags.getArray();
//
//  // Platz in Arrays schaffen
//  pOuterSequence->realloc((sal_Int32)aPolygon.GetPointCount());
//  pOuterFlags->realloc((sal_Int32)aPolygon.GetPointCount());
//
//  // Pointer auf arrays holen
//  awt::Point* pInnerSequence = pOuterSequence->getArray();
//  drawing::PolygonFlags* pInnerFlags = pOuterFlags->getArray();
//
//  for(sal_uInt16 b=0;b<aPolygon.GetPointCount();b++)
//  {
//      *pInnerSequence++ = awt::Point( aPolygon[b].X(), aPolygon[b].Y() );
//      *pInnerFlags++ = (drawing::PolygonFlags)((sal_uInt16)aPolygon.GetFlags(b));
//  }
//}

///////////////////////////////////////////////////////////////////////

SvxShapeCaption::SvxShapeCaption( SdrObject* pObj ) throw()
: SvxShapeText( pObj, aSvxMapProvider.GetMap(SVXMAP_CAPTION) )
{
}

SvxShapeCaption::~SvxShapeCaption() throw()
{
}

/***********************************************************************
* class SvxCustomShape                                                   *
***********************************************************************/

SvxCustomShape::SvxCustomShape( SdrObject* pObj )  throw() :
    SvxShapeText( pObj, aSvxMapProvider.GetMap( SVXMAP_CUSTOMSHAPE ) )
{
}

//----------------------------------------------------------------------
SvxCustomShape::~SvxCustomShape() throw()
{
}

//----------------------------------------------------------------------

void SvxCustomShape::Create( SdrObject* pNewObj, SvxDrawPage* pNewPage ) throw()
{
    SvxShapeText::Create( pNewObj, pNewPage );
}

//----------------------------------------------------------------------

uno::Sequence< uno::Type > SAL_CALL SvxCustomShape::getTypes()
    throw (uno::RuntimeException)
{
    return SvxShapeText::getTypes();
}

uno::Sequence< sal_Int8 > SAL_CALL SvxCustomShape::getImplementationId()
    throw (uno::RuntimeException)
{
    static uno::Sequence< sal_Int8 > aId;
    if( aId.getLength() == 0 )
    {
        aId.realloc( 16 );
        rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
    }
    return aId;
}

// ::com::sun::star::drawing::XShape

//----------------------------------------------------------------------
OUString SAL_CALL SvxCustomShape::getShapeType()
    throw( uno::RuntimeException )
{
    return SvxShape::getShapeType();
}

//------------------------------------------------------------------1----
awt::Point SAL_CALL SvxCustomShape::getPosition() throw(uno::RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );
    if ( mpModel && mpObj.is() )
    {
        SdrAShapeObjGeoData aCustomShapeGeoData;
        ((SdrObjCustomShape*)mpObj.get())->SaveGeoData( aCustomShapeGeoData );

        sal_Bool bMirroredX = sal_False;
        sal_Bool bMirroredY = sal_False;

        if ( mpObj.is() )
        {
            bMirroredX = ( ((SdrObjCustomShape*)mpObj.get())->IsMirroredX() );
            bMirroredY = ( ((SdrObjCustomShape*)mpObj.get())->IsMirroredY() );
        }
        // get aRect, this is the unrotated snaprect
        Rectangle aRect(((SdrObjCustomShape*)mpObj.get())->GetLogicRect());
        Rectangle aRectangle( aRect );

        if ( bMirroredX || bMirroredY )
        {   // we have to retrieve the unmirrored rect

            GeoStat aNewGeo( aCustomShapeGeoData.aGeo );
            if ( bMirroredX )
            {
                Polygon aPol( Rect2Poly( aRect, aNewGeo ) );
                Rectangle aBoundRect( aPol.GetBoundRect() );

                Point aRef1( ( aBoundRect.Left() + aBoundRect.Right() ) >> 1, aBoundRect.Top() );
                Point aRef2( aRef1.X(), aRef1.Y() + 1000 );
                USHORT i;
                USHORT nPntAnz=aPol.GetSize();
                for (i=0; i<nPntAnz; i++)
                {
                    MirrorPoint(aPol[i],aRef1,aRef2);
                }
                // Polygon wenden und etwas schieben
                Polygon aPol0(aPol);
                aPol[0]=aPol0[1];
                aPol[1]=aPol0[0];
                aPol[2]=aPol0[3];
                aPol[3]=aPol0[2];
                aPol[4]=aPol0[1];
                Poly2Rect(aPol,aRectangle,aNewGeo);
            }
            if ( bMirroredY )
            {
                Polygon aPol( Rect2Poly( aRectangle, aNewGeo ) );
                Rectangle aBoundRect( aPol.GetBoundRect() );

                Point aRef1( aBoundRect.Left(), ( aBoundRect.Top() + aBoundRect.Bottom() ) >> 1 );
                Point aRef2( aRef1.X() + 1000, aRef1.Y() );
                USHORT i;
                USHORT nPntAnz=aPol.GetSize();
                for (i=0; i<nPntAnz; i++)
                {
                    MirrorPoint(aPol[i],aRef1,aRef2);
                }
                // Polygon wenden und etwas schieben
                Polygon aPol0(aPol);
                aPol[0]=aPol0[1];
                aPol[1]=aPol0[0];
                aPol[2]=aPol0[3];
                aPol[3]=aPol0[2];
                aPol[4]=aPol0[1];
                Poly2Rect( aPol, aRectangle, aNewGeo );
            }
        }
        Point aPt( aRectangle.TopLeft() );

        if( mpModel->IsWriter() )
            aPt -= mpObj->GetAnchorPos();

        ForceMetricTo100th_mm(aPt);
        return ::com::sun::star::awt::Point( aPt.X(), aPt.Y() );
    }
    else
        return SvxShape::getPosition();
}

//----------------------------------------------------------------------
void SAL_CALL SvxCustomShape::setPosition( const awt::Point& Position ) throw(uno::RuntimeException)
{
    SvxShapeText::setPosition(Position);
}

//----------------------------------------------------------------------

awt::Size SAL_CALL SvxCustomShape::getSize() throw(uno::RuntimeException)
{
    return SvxShapeText::getSize();
}

//----------------------------------------------------------------------
void SAL_CALL SvxCustomShape::setSize( const awt::Size& rSize )
    throw(beans::PropertyVetoException, uno::RuntimeException)
{
    SvxShapeText::setSize( rSize );
}

//----------------------------------------------------------------------
// ::com::sun::star::lang::XServiceInfo

uno::Sequence< OUString > SAL_CALL SvxCustomShape::getSupportedServiceNames()
    throw(uno::RuntimeException)
{
    return SvxShapeText::getSupportedServiceNames();
}

//----------------------------------------------------------------------
void SAL_CALL SvxCustomShape::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException, com::sun::star::beans::PropertyVetoException, com::sun::star::lang::IllegalArgumentException)
{
    OGuard aGuard( Application::GetSolarMutex() );
    SdrObject* pObject = mpObj.get();

    sal_Bool bCustomShapeGeometry = pObject && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "CustomShapeGeometry" ) );

    sal_Bool bMirroredX = sal_False;
    sal_Bool bMirroredY = sal_False;

    if ( bCustomShapeGeometry )
    {
        bMirroredX = ( ((SdrObjCustomShape*)pObject)->IsMirroredX() );
        bMirroredY = ( ((SdrObjCustomShape*)pObject)->IsMirroredY() );
    }

    SvxShape::setPropertyValue( aPropertyName, aValue );

    if ( bCustomShapeGeometry )
    {
        Rectangle aRect( pObject->GetSnapRect() );

        // #i38892#
        bool bNeedsMirrorX = ((SdrObjCustomShape*)pObject)->IsMirroredX() != bMirroredX;
        bool bNeedsMirrorY = ((SdrObjCustomShape*)pObject)->IsMirroredY() != bMirroredY;

        boost::scoped_ptr< SdrGluePointList > pListCopy;
        if( bNeedsMirrorX || bNeedsMirrorY )
        {
            const SdrGluePointList* pList = pObject->GetGluePointList();
            if( pList )
                pListCopy.reset( new SdrGluePointList(*pList) );
        }

        if ( bNeedsMirrorX )
        {
            Point aTop( ( aRect.Left() + aRect.Right() ) >> 1, aRect.Top() );
            Point aBottom( aTop.X(), aTop.Y() + 1000 );
            ((SdrObjCustomShape*)pObject)->NbcMirror( aTop, aBottom );
            // NbcMirroring is flipping the current mirror state,
            // so we have to set the correct state again
            ((SdrObjCustomShape*)pObject)->SetMirroredX( bMirroredX ? sal_False : sal_True );
        }
        if ( bNeedsMirrorY )
        {
            Point aLeft( aRect.Left(), ( aRect.Top() + aRect.Bottom() ) >> 1 );
            Point aRight( aLeft.X() + 1000, aLeft.Y() );
            ((SdrObjCustomShape*)pObject)->NbcMirror( aLeft, aRight );
            // NbcMirroring is flipping the current mirror state,
            // so we have to set the correct state again
            ((SdrObjCustomShape*)pObject)->SetMirroredY( bMirroredY ? sal_False : sal_True );
        }

        if( pListCopy )
        {
            SdrGluePointList* pNewList = const_cast< SdrGluePointList* >( pObject->GetGluePointList() );
            if(pNewList)
                *pNewList = *pListCopy;
        }
    }
}

uno::Any SAL_CALL SvxCustomShape::getPropertyValue( const OUString& aPropertyName )
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    if( mpObj.is() && aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "RotateAngle" ) ) )
    {
        double fAngle = ((SdrObjCustomShape*)mpObj.get())->GetObjectRotation();
        fAngle *= 100;
        return Any( (sal_Int32)fAngle );
    }
    else
    {
        return SvxShape::getPropertyValue( aPropertyName );
    }

}

