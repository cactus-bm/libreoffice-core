/*************************************************************************
 *
 *  $RCSfile: unoshap4.cxx,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: vg $ $Date: 2005-02-21 16:52:56 $
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

#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_XLINKAGESUPPORT_HPP_
#include <com/sun/star/embed/XLinkageSupport.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif

#define _SVX_USE_UNOGLOBALS_

#ifndef _SVDOOLE2_HXX
#include "svdoole2.hxx"
#endif
#ifndef _SVDOMEDIA_HXX
#include "svdomedia.hxx"
#endif

#ifndef SVX_LIGHT
#ifndef _SOT_CLSIDS_HXX
#include <sot/clsids.hxx>
#endif
#ifndef _SFX_FRMDESCRHXX
#include <sfx2/frmdescr.hxx>
#endif
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif

#include <sfx2/objsh.hxx>
#include <sfx2/docfile.hxx>

#ifndef _SVDMODEL_HXX
#include "svdmodel.hxx"
#endif

#ifndef _SVX_SHAPEIMPL_HXX
#include "shapeimpl.hxx"
#endif

#include "unoshprp.hxx"
#include "unoapi.hxx"

#ifndef _SVDGLOB_HXX
#include "svdglob.hxx"
#endif
#include "svdstr.hrc"

///////////////////////////////////////////////////////////////////////

using namespace ::osl;
using namespace ::vos;
using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;

///////////////////////////////////////////////////////////////////////
SvxOle2Shape::SvxOle2Shape( SdrObject* pObject ) throw()
: SvxShape( pObject, aSvxMapProvider.GetMap(SVXMAP_OLE2)  )
{
}

SvxOle2Shape::SvxOle2Shape( SdrObject* pObject, const SfxItemPropertyMap* pPropertySet ) throw ()
: SvxShape( pObject, pPropertySet  )
{
}

SvxOle2Shape::~SvxOle2Shape() throw()
{
}

::com::sun::star::uno::Any SAL_CALL SvxOle2Shape::queryAggregation( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException)
{
    Any aAny;
    SvxShape::queryAggregation( rType, aAny );
    return aAny;
}

//XPropertySet
void SAL_CALL SvxOle2Shape::setPropertyValue( const OUString& aPropertyName, const Any& aValue )    throw( UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    if( aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "CLSID" ) ) )
    {
        OUString aCLSID;
        if( aValue >>= aCLSID )
        {
            // init a ole object with a global name
            if( pObj )
            {
                SvGlobalName aClassName;
                if( aClassName.MakeId( aCLSID ) )
                {
                    if( createObject( aClassName ) )
                        return;
                }
            }
        }

        throw IllegalArgumentException();
    }
    else if( aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "ThumbnailGraphicURL" ) ) )
    {
        OUString aURL;
        if( aValue >>= aURL )
        {
            SdrOle2Obj* pOle = PTR_CAST( SdrOle2Obj, pObj );
            if( pOle )
            {
                GraphicObject aGrafObj( CreateGraphicObjectFromURL( aURL ) );
                pOle->SetGraphic( &aGrafObj.GetGraphic() );
            }
            return;
        }

        throw IllegalArgumentException();
    }
    else if( aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( UNO_NAME_OLE2_PERSISTNAME ) ) )
    {
        OUString aPersistName;

        if( aValue >>= aPersistName )
        {
            SdrOle2Obj* pOle = PTR_CAST( SdrOle2Obj, pObj );

            if( pOle )
                pOle->SetPersistName( aPersistName );

            return;
        }

        throw IllegalArgumentException();
    }
    else if( aPropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "LinkURL" ) ) )
    {
        OUString aLinkURL;
        if ( aValue >>= aLinkURL )
        {
            if( pObj )
                createLink( aLinkURL );

            return;
        }

        throw IllegalArgumentException();
    }

    SvxShape::setPropertyValue( aPropertyName, aValue );
}

Any SAL_CALL SvxOle2Shape::getPropertyValue( const OUString& PropertyName ) throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    if( PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "ThumbnailGraphicURL" ) ) )
    {
        OUString    aURL;
        SdrOle2Obj* pOle = PTR_CAST( SdrOle2Obj, pObj );

        if( pOle )
        {
            Graphic* pGraphic = pOle->GetGraphic();

            // if there isn't already a preview graphic set, check if we need to generate
            // one if model says so
            if( pGraphic == NULL && !pOle->IsEmptyPresObj() && pModel->IsSaveOLEPreview() )
                pGraphic = pOle->GetGraphic();

            if( pGraphic )
            {
                GraphicObject aObj( *pGraphic );
                aURL = OUString(RTL_CONSTASCII_USTRINGPARAM(UNO_NAME_GRAPHOBJ_URLPREFIX));
                aURL += OUString::createFromAscii( aObj.GetUniqueID().GetBuffer() );
            }
        }
        return makeAny( aURL );
    }
    else if( PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( UNO_NAME_OLE2_PERSISTNAME ) ) )
    {
        OUString    aPersistName;
        SdrOle2Obj* pOle = PTR_CAST( SdrOle2Obj, pObj );

        if( pOle )
        {
            aPersistName = pOle->GetPersistName();
            if( aPersistName.getLength() )
            {
                SfxObjectShell *pPersist = pObj->GetModel()->GetPersist();
                if( (NULL == pPersist) || !pPersist->GetEmbeddedObjectContainer().HasEmbeddedObject( static_cast< SdrOle2Obj* >( pObj )->GetPersistName() ) )
                    aPersistName = OUString();
            }
        }

        return makeAny( aPersistName );
    }
    else if( PropertyName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "LinkURL" ) ) )
    {
        OUString    aLinkURL;
        SdrOle2Obj* pOle = PTR_CAST( SdrOle2Obj, pObj );

        if( pOle )
        {
            uno::Reference< embed::XLinkageSupport > xLink( pOle->GetObjRef(), uno::UNO_QUERY );
            if ( xLink.is() && xLink->isLink() )
                aLinkURL = xLink->getLinkURL();
        }

        return makeAny( aLinkURL );
    }

    return SvxShape::getPropertyValue( PropertyName );
}


sal_Bool SvxOle2Shape::createObject( const SvGlobalName &aClassName )
{
    sal_Bool            bOk = sal_False;

    if ( !static_cast< SdrOle2Obj* >( pObj )->IsEmpty() )
        return sal_False;

    // create storage and inplace object
    SfxObjectShell*     pPersist = pModel->GetPersist();
    ::rtl::OUString              aPersistName;
    OUString            aTmpStr;
    Any                 aAny( getPropertyValue( OUString::createFromAscii( UNO_NAME_OLE2_PERSISTNAME ) ) );
    if( aAny >>= aTmpStr )
        aPersistName = aTmpStr;

    //TODO/LATER: how to cope with creation failure?!
    uno::Reference < embed::XEmbeddedObject > xObj =
            pPersist->GetEmbeddedObjectContainer().CreateEmbeddedObject( aClassName.GetByteSequence(), aPersistName );
    if( xObj.is() )
    {
        aAny <<= ( aTmpStr = aPersistName );
        setPropertyValue( OUString::createFromAscii( UNO_NAME_OLE2_PERSISTNAME ), aAny );

        // the object is inserted during setting of PersistName property usually
        if ( static_cast< SdrOle2Obj* >( pObj )->IsEmpty() )
            static_cast< SdrOle2Obj* >( pObj )->SetObjRef( xObj );

        Rectangle aRect = static_cast< SdrOle2Obj* >( pObj )->GetLogicRect();
        if ( aRect.GetWidth() == 100 && aRect.GetHeight() == 100 )
        {
            // default size
            awt::Size aSz = xObj->getVisualAreaSize( static_cast< SdrOle2Obj* >( pObj )->GetAspect() );
            aRect.SetSize( Size( aSz.Width, aSz.Height ) );
            static_cast< SdrOle2Obj* >( pObj )->SetLogicRect( aRect );
        }
        else
        {
            awt::Size aSz;
            Size aSize = static_cast< SdrOle2Obj* >( pObj )->GetLogicRect().GetSize();
            aSz.Width = aSize.Width();
            aSz.Height = aSize.Height();
            xObj->setVisualAreaSize(  static_cast< SdrOle2Obj* >( pObj )->GetAspect(), aSz );
        }
    }

    return xObj.is();
}

sal_Bool SvxOle2Shape::createLink( const ::rtl::OUString& aLinkURL )
{
    sal_Bool bResult = sal_False;

    if ( !static_cast< SdrOle2Obj* >( pObj )->IsEmpty() )
        return sal_False;

    ::rtl::OUString aPersistName;

    SfxObjectShell* pPersist = pModel->GetPersist();

    uno::Sequence< beans::PropertyValue > aMediaDescr( 1 );
    aMediaDescr[0].Name = ::rtl::OUString::createFromAscii( "URL" );
    aMediaDescr[0].Value <<= aLinkURL;

    if ( pPersist->GetMedium() )
    {
        uno::Reference< task::XInteractionHandler > xInteraction = pPersist->GetMedium()->GetInteractionHandler();
        if ( xInteraction.is() )
        {
            aMediaDescr.realloc( 2 );
            aMediaDescr[1].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "InteractionHandler" ) );
            aMediaDescr[1].Value <<= xInteraction;
        }
    }

    //TODO/LATER: how to cope with creation failure?!
    uno::Reference< embed::XEmbeddedObject > xObj =
            pPersist->GetEmbeddedObjectContainer().InsertEmbeddedLink( aMediaDescr , aPersistName );

    if( xObj.is() )
    {
        setPropertyValue( OUString::createFromAscii( UNO_NAME_OLE2_PERSISTNAME ), uno::makeAny( aPersistName ) );

        // the object is inserted during setting of PersistName property usually
        if ( static_cast< SdrOle2Obj* >( pObj )->IsEmpty() )
            static_cast< SdrOle2Obj* >( pObj )->SetObjRef( xObj );

        Rectangle aRect = static_cast< SdrOle2Obj* >( pObj )->GetLogicRect();
        if ( aRect.GetWidth() == 100 && aRect.GetHeight() == 100 )
        {
            // default size
            awt::Size aSz = xObj->getVisualAreaSize( static_cast< SdrOle2Obj* >( pObj )->GetAspect() );
            aRect.SetSize( Size( aSz.Width, aSz.Height ) );
            static_cast< SdrOle2Obj* >( pObj )->SetLogicRect( aRect );
        }
        else
        {
            awt::Size aSz;
            Size aSize = static_cast< SdrOle2Obj* >( pObj )->GetLogicRect().GetSize();
            aSz.Width = aSize.Width();
            aSz.Height = aSize.Height();
            xObj->setVisualAreaSize(  static_cast< SdrOle2Obj* >( pObj )->GetAspect(), aSz );
        }
    }

    return xObj.is();
}

///////////////////////////////////////////////////////////////////////
#ifndef SVX_LIGHT
SvxAppletShape::SvxAppletShape( SdrObject* pObject ) throw()
: SvxOle2Shape( pObject, aSvxMapProvider.GetMap(SVXMAP_APPLET)  )
{
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.AppletShape" ) ) );
}

SvxAppletShape::~SvxAppletShape() throw()
{
}

void SvxAppletShape::Create( SdrObject* pNewObj, SvxDrawPage* pNewPage ) throw ()
{
    SvxShape::Create( pNewObj, pNewPage );
    const SvGlobalName aAppletClassId( SO3_APPLET_CLASSID );
    createObject(aAppletClassId);
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.AppletShape" ) ) );
}

//XPropertySet
void SAL_CALL SvxAppletShape::setPropertyValue( const OUString& aPropertyName, const Any& aValue )  throw( UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException )
{
    OGuard aGuard( Application::GetSolarMutex() );

    sal_Bool bOwn = sal_False;

    const SfxItemPropertyMap* pMap = aPropSet.getPropertyMapEntry(aPropertyName);

    if( pMap && pObj && pModel )
    {
        if( pMap->nWID >= OWN_ATTR_APPLET_DOCBASE && pMap->nWID <= OWN_ATTR_APPLET_ISSCRIPT )
        {
            if ( !svt::EmbeddedObjectRef::TryRunningState( ((SdrOle2Obj*)pObj)->GetObjRef() ) )
                return;

            uno::Reference < beans::XPropertySet > xSet( ((SdrOle2Obj*)pObj)->GetObjRef()->getComponent(), uno::UNO_QUERY );
            if ( !xSet.is() )
                return;

            switch( pMap->nWID )
            {
                case OWN_ATTR_APPLET_CODEBASE:
                case OWN_ATTR_APPLET_NAME:
                case OWN_ATTR_APPLET_CODE:
                case OWN_ATTR_APPLET_COMMANDS:
                case OWN_ATTR_APPLET_ISSCRIPT:
                case OWN_ATTR_APPLET_DOCBASE:
                    // allow exceptions to pass through
                    xSet->setPropertyValue( aPropertyName, aValue );
                    bOwn = sal_True;
                    break;
                default:
                    throw IllegalArgumentException();
            }
        }
    }

    if( !bOwn )
        SvxOle2Shape::setPropertyValue( aPropertyName, aValue );

    if( pModel )
    {
        SfxObjectShell* pPersist = pModel->GetPersist();
        if( pPersist && !pPersist->IsEnableSetModified() )
        {
            SdrOle2Obj* pOle = static_cast< SdrOle2Obj* >( pObj );
            if( pOle && !pOle->IsEmpty() )
            {
                uno::Reference < util::XModifiable > xMod( ((SdrOle2Obj*)pObj)->GetObjRef(), uno::UNO_QUERY );
                if( xMod.is() )
                    // TODO/MBA: what's this?!
                    xMod->setModified( sal_False );
            }
        }
    }
}

Any SAL_CALL SvxAppletShape::getPropertyValue( const OUString& PropertyName ) throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    const SfxItemPropertyMap* pMap = aPropSet.getPropertyMapEntry(PropertyName);

    if( pMap && pObj && pModel )
    {
        if( pMap->nWID >= OWN_ATTR_APPLET_DOCBASE && pMap->nWID <= OWN_ATTR_APPLET_ISSCRIPT )
        {
            if ( !svt::EmbeddedObjectRef::TryRunningState( ((SdrOle2Obj*)pObj)->GetObjRef() ) )
                return uno::Any();

            uno::Reference < beans::XPropertySet > xSet( ((SdrOle2Obj*)pObj)->GetObjRef()->getComponent(), uno::UNO_QUERY );
            if ( !xSet.is() )
                return uno::Any();

            switch( pMap->nWID )
            {
                case OWN_ATTR_APPLET_CODEBASE:
                case OWN_ATTR_APPLET_NAME:
                case OWN_ATTR_APPLET_CODE:
                case OWN_ATTR_APPLET_COMMANDS:
                case OWN_ATTR_APPLET_ISSCRIPT:
                    return xSet->getPropertyValue( PropertyName );
                    break;
                default:
                    throw IllegalArgumentException();
            }
        }
    }

    return SvxOle2Shape::getPropertyValue( PropertyName );
}

///////////////////////////////////////////////////////////////////////

SvxPluginShape::SvxPluginShape( SdrObject* pObject ) throw()
: SvxOle2Shape( pObject, aSvxMapProvider.GetMap(SVXMAP_PLUGIN)  )
{
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.PluginShape" ) ) );
}

SvxPluginShape::~SvxPluginShape() throw()
{
}

void SvxPluginShape::Create( SdrObject* pNewObj, SvxDrawPage* pNewPage ) throw ()
{
    SvxShape::Create( pNewObj, pNewPage );
    const SvGlobalName aPluginClassId( SO3_PLUGIN_CLASSID );
    createObject(aPluginClassId);
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.PluginShape" ) ) );
}

//XPropertySet
void SAL_CALL SvxPluginShape::setPropertyValue( const OUString& aPropertyName, const Any& aValue )
    throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    sal_Bool bOwn = sal_False;

    const SfxItemPropertyMap* pMap = aPropSet.getPropertyMapEntry(aPropertyName);

    if( pMap && pObj && pModel )
    {
        if( pMap->nWID >= OWN_ATTR_PLUGIN_MIMETYPE && pMap->nWID <= OWN_ATTR_PLUGIN_COMMANDS )
        {
            if ( !svt::EmbeddedObjectRef::TryRunningState( ((SdrOle2Obj*)pObj)->GetObjRef() ) )
                return;

            uno::Reference < beans::XPropertySet > xSet( ((SdrOle2Obj*)pObj)->GetObjRef()->getComponent(), uno::UNO_QUERY );
            if ( !xSet.is() )
                return;

            switch( pMap->nWID )
            {
                case OWN_ATTR_PLUGIN_MIMETYPE:
                case OWN_ATTR_PLUGIN_URL:
                case OWN_ATTR_PLUGIN_COMMANDS:
                    // allow exceptions to pass through
                    xSet->setPropertyValue( aPropertyName, aValue );
                    bOwn = sal_True;
                    break;
                default:
                    throw IllegalArgumentException();
            }
        }
    }

    if( !bOwn )
        SvxOle2Shape::setPropertyValue( aPropertyName, aValue );

    if( pModel )
    {
        SfxObjectShell* pPersist = pModel->GetPersist();
        if( pPersist && !pPersist->IsEnableSetModified() )
        {
            SdrOle2Obj* pOle = static_cast< SdrOle2Obj* >( pObj );
            if( pOle && !pOle->IsEmpty() )
            {
                uno::Reference < util::XModifiable > xMod( ((SdrOle2Obj*)pObj)->GetObjRef(), uno::UNO_QUERY );
                if( xMod.is() )
                    // TODO/MBA: what's this?!
                    xMod->setModified( sal_False );
            }
        }
    }
}

Any SAL_CALL SvxPluginShape::getPropertyValue( const OUString& PropertyName ) throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    const SfxItemPropertyMap* pMap = aPropSet.getPropertyMapEntry(PropertyName);

    if( pMap && pObj && pModel )
    {
        if( pMap->nWID >= OWN_ATTR_PLUGIN_MIMETYPE && pMap->nWID <= OWN_ATTR_PLUGIN_COMMANDS )
        {
            if ( !svt::EmbeddedObjectRef::TryRunningState( ((SdrOle2Obj*)pObj)->GetObjRef() ) )
                return uno::Any();

            uno::Reference < beans::XPropertySet > xSet( ((SdrOle2Obj*)pObj)->GetObjRef()->getComponent(), uno::UNO_QUERY );
            if ( !xSet.is() )
                return uno::Any();

            switch( pMap->nWID )
            {
                case OWN_ATTR_PLUGIN_MIMETYPE:
                case OWN_ATTR_PLUGIN_URL:
                case OWN_ATTR_PLUGIN_COMMANDS:
                    return xSet->getPropertyValue( PropertyName );
                    break;
                default:
                    throw IllegalArgumentException();
            }
        }
    }

    return SvxOle2Shape::getPropertyValue( PropertyName );
}

///////////////////////////////////////////////////////////////////////

SvxFrameShape::SvxFrameShape( SdrObject* pObject ) throw()
: SvxOle2Shape( pObject, aSvxMapProvider.GetMap(SVXMAP_FRAME)  )
{
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.FrameShape" ) ) );
}

SvxFrameShape::~SvxFrameShape() throw()
{
}

void SvxFrameShape::Create( SdrObject* pNewObj, SvxDrawPage* pNewPage ) throw ()
{
    SvxShape::Create( pNewObj, pNewPage );
    const SvGlobalName aIFrameClassId( SO3_IFRAME_CLASSID );
    createObject(aIFrameClassId);
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.FrameShape" ) ) );
}

//XPropertySet
void SAL_CALL SvxFrameShape::setPropertyValue( const OUString& aPropertyName, const Any& aValue )
    throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    sal_Bool bOwn = sal_False;

    const SfxItemPropertyMap* pMap = aPropSet.getPropertyMapEntry(aPropertyName);

    Any aAny;
    if( pMap && pObj && pModel )
    {
        if( pMap->nWID >= OWN_ATTR_FRAME_URL && pMap->nWID <= OWN_ATTR_FRAME_MARGIN_HEIGHT )
        {
            if ( !svt::EmbeddedObjectRef::TryRunningState( ((SdrOle2Obj*)pObj)->GetObjRef() ) )
                return;

            uno::Reference < beans::XPropertySet > xSet( ((SdrOle2Obj*)pObj)->GetObjRef()->getComponent(), uno::UNO_QUERY );
            if ( !xSet.is() )
                return;

            switch( pMap->nWID )
            {
                //TODO/LATER: more properties!
                case OWN_ATTR_FRAME_URL:
                case OWN_ATTR_FRAME_NAME:
                case OWN_ATTR_FRAME_ISAUTOSCROLL:
                case OWN_ATTR_FRAME_ISBORDER:
                case OWN_ATTR_FRAME_MARGIN_WIDTH:
                case OWN_ATTR_FRAME_MARGIN_HEIGHT:
                    // allow exceptions to pass through
                    xSet->setPropertyValue( aPropertyName, aValue );
                    bOwn = sal_True;
                    break;
                default:
                    throw IllegalArgumentException();
            }
        }
    }

    if( !bOwn )
        SvxOle2Shape::setPropertyValue( aPropertyName, aValue );

    if( pModel )
    {
        SfxObjectShell* pPersist = pModel->GetPersist();
        if( pPersist && !pPersist->IsEnableSetModified() )
        {
            SdrOle2Obj* pOle = static_cast< SdrOle2Obj* >( pObj );
            if( pOle && !pOle->IsEmpty() )
            {
                uno::Reference < util::XModifiable > xMod( ((SdrOle2Obj*)pObj)->GetObjRef(), uno::UNO_QUERY );
                if( xMod.is() )
                    // TODO/MBA: what's this?!
                    xMod->setModified( sal_False );
            }
        }
    }
}

Any SAL_CALL SvxFrameShape::getPropertyValue( const OUString& PropertyName ) throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    OGuard aGuard( Application::GetSolarMutex() );

    const SfxItemPropertyMap* pMap = aPropSet.getPropertyMapEntry(PropertyName);

    Any aAny;
    if( pMap && pObj && pModel )
    {
        if( pMap->nWID >= OWN_ATTR_FRAME_URL && pMap->nWID <= OWN_ATTR_FRAME_MARGIN_HEIGHT )
        {
            if ( !svt::EmbeddedObjectRef::TryRunningState( ((SdrOle2Obj*)pObj)->GetObjRef() ) )
                return uno::Any();

            uno::Reference < beans::XPropertySet > xSet( ((SdrOle2Obj*)pObj)->GetObjRef()->getComponent(), uno::UNO_QUERY );
            if ( !xSet.is() )
                return uno::Any();

            switch( pMap->nWID )
            {
                //TODO/LATER: more properties!
                case OWN_ATTR_FRAME_URL:
                case OWN_ATTR_FRAME_NAME:
                case OWN_ATTR_FRAME_ISAUTOSCROLL:
                case OWN_ATTR_FRAME_ISBORDER:
                case OWN_ATTR_FRAME_MARGIN_WIDTH:
                case OWN_ATTR_FRAME_MARGIN_HEIGHT:
                    return xSet->getPropertyValue( PropertyName );
                    break;
                default:
                    throw IllegalArgumentException();
            }
        }
    }

    return SvxOle2Shape::getPropertyValue( PropertyName );
}

/***********************************************************************
*                                                                      *
***********************************************************************/

SvxMediaShape::SvxMediaShape( SdrObject* pObj ) throw()
:   SvxShape( pObj, aSvxMapProvider.GetMap(SVXMAP_MEDIA) )
{
    SetShapeType( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.MediaShape" ) ) );
}

//----------------------------------------------------------------------
SvxMediaShape::~SvxMediaShape() throw()
{
}

//----------------------------------------------------------------------

void SAL_CALL SvxMediaShape::setPropertyValue( const OUString& rPropertyName, const Any& rValue )
    throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    OGuard                      aGuard( Application::GetSolarMutex() );
    const SfxItemPropertyMap*   pMap = aPropSet.getPropertyMapEntry( rPropertyName );
    bool                        bOwn = false;

    if( pMap && pObj && pModel )
    {
        SdrMediaObj* pMedia = PTR_CAST( SdrMediaObj, pObj );

        if( pMedia && ( pMap->nWID >= OWN_ATTR_MEDIA_URL ) && ( pMap->nWID <= OWN_ATTR_MEDIA_ZOOM ) )
        {
            ::avmedia::MediaItem aItem;

            switch( pMap->nWID )
            {
                case OWN_ATTR_MEDIA_URL:
                {
                    OUString aURL;

                    if( rValue >>= aURL )
                        aItem.setURL( aURL );
                }
                break;

                case( OWN_ATTR_MEDIA_LOOP ):
                {
                    sal_Bool bLoop;

                    if( rValue >>= bLoop )
                        aItem.setLoop( bLoop );
                }
                break;

                case( OWN_ATTR_MEDIA_MUTE ):
                {
                    sal_Bool bMute;

                    if( rValue >>= bMute )
                        aItem.setMute( bMute );
                }
                break;

                case( OWN_ATTR_MEDIA_VOLUMEDB ):
                {
                    sal_Int16 nVolumeDB;

                    if( rValue >>= nVolumeDB )
                        aItem.setVolumeDB( nVolumeDB );
                }
                break;

                case( OWN_ATTR_MEDIA_ZOOM ):
                {
                    ::com::sun::star::media::ZoomLevel eLevel;

                    if( rValue >>= eLevel )
                        aItem.setZoom( eLevel );
                }
                break;

                default:
                    throw IllegalArgumentException();
            }

            pMedia->setMediaProperties( aItem );
            bOwn = true;
        }
    }

    if( !bOwn )
        SvxShape::setPropertyValue( rPropertyName, rValue );
}

//----------------------------------------------------------------------

Any SAL_CALL SvxMediaShape::getPropertyValue( const OUString& rPropertyName )
    throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    OGuard                      aGuard( Application::GetSolarMutex() );
    const SfxItemPropertyMap*   pMap = aPropSet.getPropertyMapEntry( rPropertyName );

    if( pMap && pObj && pModel )
    {
        SdrMediaObj* pMedia = PTR_CAST( SdrMediaObj, pObj );

        if( pMedia && ( pMap->nWID >= OWN_ATTR_MEDIA_URL ) && ( pMap->nWID <= OWN_ATTR_MEDIA_ZOOM ) )
        {
            const ::avmedia::MediaItem aItem( pMedia->getMediaProperties() );

            switch( pMap->nWID )
            {
                case OWN_ATTR_MEDIA_URL:
                    return makeAny( aItem.getURL() );

                case( OWN_ATTR_MEDIA_LOOP ):
                    return makeAny( (sal_Bool) aItem.isLoop() );

                case( OWN_ATTR_MEDIA_MUTE ):
                    return makeAny( (sal_Bool) aItem.isMute() );

                case( OWN_ATTR_MEDIA_VOLUMEDB ):
                    return makeAny( (sal_Int16) aItem.getVolumeDB() );

                case( OWN_ATTR_MEDIA_ZOOM ):
                    return makeAny( aItem.getZoom() );

                default:
                    throw IllegalArgumentException();
            }
        }
    }

    return SvxShape::getPropertyValue( rPropertyName );
}

#endif
