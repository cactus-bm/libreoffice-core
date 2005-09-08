/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: updatesvc.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 03:35:47 $
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

#include "updatesvc.hxx"

#ifndef CONFIGMGR_API_FACTORY_HXX_
#include "confapifactory.hxx"
#endif
#ifndef CONFIGMGR_BACKEND_EMPTYLAYER_HXX
#include "emptylayer.hxx"
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XUPDATABLELAYER_HPP_
#include <com/sun/star/configuration/backend/XUpdatableLayer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XLAYERHANDLER_HPP_
#include <com/sun/star/configuration/backend/XLayerHandler.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

// -----------------------------------------------------------------------------

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace backend
    {
// -----------------------------------------------------------------------------
        namespace uno   = ::com::sun::star::uno;
        namespace lang  = ::com::sun::star::lang;
        namespace beans = ::com::sun::star::beans;
        namespace backenduno = ::com::sun::star::configuration::backend;
// -----------------------------------------------------------------------------

AsciiServiceName const aUpdateMergerServices[] =
{
    "com.sun.star.configuration.backend.LayerUpdateMerger",
    0
};
const ServiceImplementationInfo aUpdateMergerSI =
{
    "com.sun.star.comp.configuration.backend.LayerUpdateMerger",
    aUpdateMergerServices,
    0
};
// -----------------------------------------------------------------------------

const ServiceRegistrationInfo* getUpdateMergerServiceInfo()
{ return getRegistrationInfo(& aUpdateMergerSI); }
// -----------------------------------------------------------------------------

inline
ServiceInfoHelper UpdateService::getServiceInfo()
{
    return & aUpdateMergerSI;
}
// -----------------------------------------------------------------------------


UpdateService::UpdateService(CreationArg _xContext)
: m_xServiceFactory(_xContext->getServiceManager(),uno::UNO_QUERY)
, m_aSourceMode(merge)
{
    if (!m_xServiceFactory.is())
    {
        OUString sMessage( RTL_CONSTASCII_USTRINGPARAM("Configuration Update Merger: Context has no service manager (or missing interface)"));
        throw uno::RuntimeException(sMessage,NULL);
    }
}
// -----------------------------------------------------------------------------

// XInitialization

void SAL_CALL
    UpdateService::initialize( const uno::Sequence< uno::Any >& aArguments )
        throw (uno::Exception, uno::RuntimeException)
{
    sal_Int16 const nCount = static_cast<sal_Int16>(aArguments.getLength());

    if (sal_Int32(nCount) != aArguments.getLength())
    {
        OUString sMessage( RTL_CONSTASCII_USTRINGPARAM("Too many arguments to initialize a Configuration Update Merger"));
        throw lang::IllegalArgumentException(sMessage,*this,0);
    }

    for (sal_Int16 i = 0; i < nCount; ++i)
    {
        uno::Reference< backenduno::XUpdatableLayer > xUpdLayer;
        if (aArguments[i] >>= xUpdLayer)
        {
            m_xSourceLayer = xUpdLayer.get();
            m_xLayerWriter.clear();

            OSL_ASSERT( uno::Reference< backenduno::XUpdatableLayer >::query(m_xSourceLayer).is() || !xUpdLayer.is() );

            continue;
        }

        if (aArguments[i] >>= m_xSourceLayer)
            continue;

        if (aArguments[i] >>= m_xLayerWriter)
            continue;

        beans::NamedValue aExtraArg;
        if (aArguments[i] >>= aExtraArg)
        {
            OSL_VERIFY( setImplementationProperty(aExtraArg.Name, aExtraArg.Value) );

            continue;
        }

        OUString sMessage( RTL_CONSTASCII_USTRINGPARAM("Cannot use argument to initialize a Configuration Update Merger"
                                                        "- XLayer, XLayerHandler or XUpdatableLayer expected"));
        throw lang::IllegalArgumentException(sMessage,*this,i);
    }
}

// -----------------------------------------------------------------------------

sal_Bool UpdateService::setImplementationProperty(OUString const & aName, uno::Any const & aValue)
{
    if (aName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Overwrite")))
    {
        sal_Bool bOverwrite;
        if (aValue >>= bOverwrite)
        {
            if (!bOverwrite)
                m_aSourceMode = protect;

            else if (protect == m_aSourceMode)
                m_aSourceMode = merge;

            return true;
        }
    }

    else if (aName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Truncate")))
    {
        sal_Bool bTruncate;
        if (aValue >>= bTruncate)
        {
            if (!bTruncate)
                m_aSourceMode = merge;

            else if (merge == m_aSourceMode)
                m_aSourceMode = truncate;

            return true;
        }
    }

    return false;
}
// -----------------------------------------------------------------------------

bool UpdateService::validateSourceLayerAndCheckNotEmpty() SAL_THROW( (lang::IllegalAccessException) )
{
    switch (m_aSourceMode)
    {
    case merge:     // TODO: check for readonly layer
                    return true;

    case protect:   if (!checkEmptyLayer(m_xSourceLayer))
                        raiseIllegalAccessException("UpdateService: Layer already exists");
                    // else fall through

    case truncate:  return false;

    default:        OSL_ASSERT(!"not reached");
                    return true;
    }
}
// -----------------------------------------------------------------------------

UpdateService::Layer UpdateService::getSourceLayer() SAL_THROW( (lang::IllegalAccessException) )
{
    if ( validateSourceLayerAndCheckNotEmpty() )
        return m_xSourceLayer;
    else
        return createEmptyLayer();
}
// -----------------------------------------------------------------------------

void UpdateService::raiseIllegalAccessException(sal_Char const * pMsg)
    SAL_THROW( (lang::IllegalAccessException) )
{
    OUString sMsg = OUString::createFromAscii(pMsg);
    throw lang::IllegalAccessException(sMsg,*this);
}
// -----------------------------------------------------------------------------

void UpdateService::writeUpdatedLayer(Layer const & _xLayer)
{
    OSL_ENSURE( _xLayer.is(), "UpdateService: Trying to write NULL XLayer");

    if (!_xLayer.is())
    {
        OUString sMessage( RTL_CONSTASCII_USTRINGPARAM("Update Merger - Internal error: trying to write a NULL Layer"));
        throw uno::RuntimeException(sMessage,*this);
    }

    // use our layer writer, if we have one
    if ( m_xLayerWriter.is() )
    {
        _xLayer->readData( m_xLayerWriter );
        return;
    }

    // look for an updatable layer otherwise
    uno::Reference< backenduno::XUpdatableLayer > xUpdLayer(m_xSourceLayer, uno::UNO_QUERY);
    if (xUpdLayer.is())
    {
        xUpdLayer->replaceWith( _xLayer );
        return;
    }

    OUString sMessage( RTL_CONSTASCII_USTRINGPARAM("Update Merger: Cannot write merge results - no recipient available."));
    throw uno::RuntimeException(sMessage,*this);
}
// -----------------------------------------------------------------------------

// XServiceInfo

::rtl::OUString SAL_CALL
    UpdateService::getImplementationName(  )
        throw (uno::RuntimeException)
{
    return getServiceInfo().getImplementationName( );
}
// -----------------------------------------------------------------------------


sal_Bool SAL_CALL
    UpdateService::supportsService( const ::rtl::OUString& ServiceName )
        throw (uno::RuntimeException)
{
    return getServiceInfo().supportsService( ServiceName );
}
// -----------------------------------------------------------------------------


uno::Sequence< ::rtl::OUString > SAL_CALL
    UpdateService::getSupportedServiceNames(  )
        throw (uno::RuntimeException)
{
    return getServiceInfo().getSupportedServiceNames( );
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
    } // namespace

// -----------------------------------------------------------------------------
} // namespace

