/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: model.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:53:04 $
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
#ifndef __PLUGIN_MODEL_HXX
#define __PLUGIN_MODEL_HXX

#ifndef _COM_SUN_STAR_IO_XOBJECTINPUTSTREAM_HPP_
#include <com/sun/star/io/XObjectInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XPERSISTOBJECT_HPP_
#include <com/sun/star/io/XPersistObject.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XOBJECTOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XObjectOutputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICENAME_HPP_
#include <com/sun/star/lang/XServiceName.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLMODEL_HPP_
#include <com/sun/star/awt/XControlModel.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROL_HPP_
#include <com/sun/star/awt/XControl.hpp>
#endif

#include <cppuhelper/propshlp.hxx>
#include <cppuhelper/weakagg.hxx>
#include <rtl/alloc.h>

#include <list>

using namespace com::sun::star::uno;

class BroadcasterHelperHolder
{
protected:
    ::cppu::OBroadcastHelper  m_aHelper;
public:
    BroadcasterHelperHolder( osl::Mutex& rMutex ) :
            m_aHelper( rMutex ) {}
    ~BroadcasterHelperHolder() {}

    ::cppu::OBroadcastHelper& getHelper() { return m_aHelper; }

};

class PluginModel : public BroadcasterHelperHolder,
                    public cppu::OPropertySetHelper,
                    public cppu::OPropertyArrayHelper,
                    public cppu::OWeakAggObject,
                    public com::sun::star::lang::XComponent,
                    public com::sun::star::io::XPersistObject,
                    public com::sun::star::awt::XControlModel
{
  private:
    rtl::OUString m_aCreationURL;
    rtl::OUString m_aMimeType;

    std::list< Reference< com::sun::star::lang::XEventListener > >
        m_aDisposeListeners;
  public:
    // these are here to force memory de/allocation to sal lib.
    static void * SAL_CALL operator new( size_t nSize ) throw()
        { return rtl_allocateMemory( nSize ); }
    static void SAL_CALL operator delete( void * pMem ) throw()
        { rtl_freeMemory( pMem ); }

    PluginModel();
    PluginModel( const rtl::OUString& rURL, const rtl::OUString& rMimeType );
    virtual ~PluginModel();


    const rtl::OUString& getCreationURL() { return m_aCreationURL; }
    void setMimeType( const rtl::OUString& rMime ) { m_aMimeType = rMime; }

    // XInterface
    virtual Any SAL_CALL queryInterface( const Type& rType ) throw( com::sun::star::uno::RuntimeException )
        { return OWeakAggObject::queryInterface( rType ); }
    virtual void SAL_CALL acquire()  throw()
    { OWeakAggObject::acquire(); }
    virtual void SAL_CALL release()  throw()
    { OWeakAggObject::release(); }

    virtual Any SAL_CALL queryAggregation( const Type& ) throw( com::sun::star::uno::RuntimeException );


    // com::sun::star::lang::XTypeProvider

    // com::sun::star::lang::XServiceInfo
    ::rtl::OUString SAL_CALL getImplementationName() throw(  );

    sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName) throw(  );
    Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw(  );
    static Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames_Static(void) throw(  );
    static rtl::OUString SAL_CALL getImplementationName_Static() throw(  )
    {
        /** the soplayer uses this name in its source! maybe not after 5.2 */
        return rtl::OUString::createFromAscii( "com.sun.star.extensions.PluginModel" );
    }

    // OPropertySetHelper
    virtual cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
    virtual sal_Bool  SAL_CALL convertFastPropertyValue( Any & rConvertedValue,
                                                         Any & rOldValue,
                                                         sal_Int32 nHandle,
                                                         const Any& rValue ) throw();
    virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle,
                                                            const Any& rValue )
        throw();
    virtual void SAL_CALL getFastPropertyValue( Any& rValue, sal_Int32 nHandle ) const throw();
    virtual Reference< com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo() throw();

    // com::sun::star::io::XPersistObject
    virtual rtl::OUString SAL_CALL getServiceName() throw();
    virtual void SAL_CALL write(const Reference< com::sun::star::io::XObjectOutputStream > & OutStream) throw();
    virtual void SAL_CALL read(const Reference< com::sun::star::io::XObjectInputStream > & InStream) throw();

    // com::sun::star::lang::XComponent
    virtual void SAL_CALL addEventListener( const Reference< com::sun::star::lang::XEventListener > & l ) throw();
    virtual void SAL_CALL removeEventListener( const Reference< com::sun::star::lang::XEventListener > & l ) throw();
    virtual void SAL_CALL dispose() throw();
};
Reference< XInterface >  SAL_CALL PluginModel_CreateInstance( const Reference< com::sun::star::lang::XMultiServiceFactory >  & ) throw( Exception );

#endif // __PLUGIN_MODEL_HXX
