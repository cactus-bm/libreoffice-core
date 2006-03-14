/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: objectinspectormodel.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-14 11:27:08 $
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

#ifndef EXTENSIONS_PROPCTRLR_MODULEPRC_HXX
#include "modulepcr.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_PCROMPONENTCONTEXT_HXX
#include "pcrcomponentcontext.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PCRCOMMON_HXX_
#include "pcrcommon.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_INSPECTION_XOBJECTINSPECTORMODEL_HPP_
#include <com/sun/star/inspection/XObjectInspectorModel.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_ALREADYINITIALIZEDEXCEPTION_HPP_
#include <com/sun/star/ucb/AlreadyInitializedException.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif
/** === end UNO includes === **/

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

//........................................................................
namespace pcr
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::inspection::XObjectInspectorModel;
    using ::com::sun::star::lang::XInitialization;
    using ::com::sun::star::lang::XServiceInfo;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::inspection::PropertyCategoryDescriptor;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::ucb::AlreadyInitializedException;
    /** === end UNO using === **/

    //====================================================================
    //= ObjectInspectorModel
    //====================================================================
    typedef ::cppu::WeakImplHelper3 <   XObjectInspectorModel
                                    ,   XInitialization
                                    ,   XServiceInfo
                                    >   ObjectInspectorModel_Base;

    class ObjectInspectorModel : public ObjectInspectorModel_Base
    {
    private:
        ComponentContext        m_aContext;
        Sequence< Any >         m_aFactories;

    public:
        ObjectInspectorModel( const Reference< XComponentContext >& _rxContext );

        // XObjectInspectorModel
        virtual Sequence< Any > SAL_CALL getHandlerFactories() throw (RuntimeException);
        virtual Sequence< PropertyCategoryDescriptor > SAL_CALL describeCategories(  ) throw (RuntimeException);
        virtual ::sal_Int32 SAL_CALL getPropertyOrderIndex( const ::rtl::OUString& PropertyName ) throw (RuntimeException);

        // XInitialization
        virtual void SAL_CALL initialize( const Sequence< Any >& aArguments ) throw (Exception, RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (RuntimeException);
        virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (RuntimeException);
        virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (RuntimeException);

        // XServiceInfo - static versions
        static ::rtl::OUString getImplementationName_static(  ) throw(RuntimeException);
        static Sequence< ::rtl::OUString > getSupportedServiceNames_static(  ) throw(RuntimeException);
        static Reference< XInterface > SAL_CALL
                        Create(const Reference< XComponentContext >&);

    protected:
        void    createDefault();
        void    createWithHandlerFactories( const Sequence< Any >& _rFactories );
    };

    //====================================================================
    //= ObjectInspectorModel
    //====================================================================
    ObjectInspectorModel::ObjectInspectorModel( const Reference< XComponentContext >& _rxContext )
        :m_aContext( _rxContext )
    {
    }

    //--------------------------------------------------------------------
    Sequence< Any > SAL_CALL ObjectInspectorModel::getHandlerFactories() throw (RuntimeException)
    {
        return m_aFactories;
    }

    //--------------------------------------------------------------------
    Sequence< PropertyCategoryDescriptor > SAL_CALL ObjectInspectorModel::describeCategories(  ) throw (RuntimeException)
    {
        // no category info provided by this default implementation
        return Sequence< PropertyCategoryDescriptor >( );
    }

    //--------------------------------------------------------------------
    ::sal_Int32 SAL_CALL ObjectInspectorModel::getPropertyOrderIndex( const ::rtl::OUString& PropertyName ) throw (RuntimeException)
    {
        // no ordering provided by this default implementation
        return 0;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ObjectInspectorModel::initialize( const Sequence< Any >& _arguments ) throw (Exception, RuntimeException)
    {
        if ( m_aFactories.getLength() )
            throw AlreadyInitializedException();

        StlSyntaxSequence< Any > arguments( _arguments );
        if ( arguments.empty() )
        {   // constructor: "createDefault()"
            createDefault();
            return;
        }

        Sequence< Any > factories;
        if ( ( arguments.size() == 1 ) && ( arguments[0] >>= factories ) )
        {   // constructor: "createWithHandlerFactories(any[])"
            createWithHandlerFactories( factories );
            return;
        }

        throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ObjectInspectorModel::getImplementationName(  ) throw (RuntimeException)
    {
        return getImplementationName_static();
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL ObjectInspectorModel::supportsService( const ::rtl::OUString& ServiceName ) throw (RuntimeException)
    {
        StlSyntaxSequence< ::rtl::OUString > aSupported( getSupportedServiceNames() );
        for (   StlSyntaxSequence< ::rtl::OUString >::const_iterator check = aSupported.begin();
                check != aSupported.end();
                ++check
            )
            if ( check->equals( ServiceName ) )
                return sal_True;

        return sal_False;
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL ObjectInspectorModel::getSupportedServiceNames(  ) throw (RuntimeException)
    {
        return getSupportedServiceNames_static();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString ObjectInspectorModel::getImplementationName_static(  ) throw(RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "org.openoffice.comp.extensions.ObjectInspectorModel" ) );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > ObjectInspectorModel::getSupportedServiceNames_static(  ) throw(RuntimeException)
    {
        ::rtl::OUString sService( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.inspection.ObjectInspectorModel" ) );
        return Sequence< ::rtl::OUString >( &sService, 1 );
    }

    //--------------------------------------------------------------------
    Reference< XInterface > SAL_CALL ObjectInspectorModel::Create(const Reference< XComponentContext >& _rxContext )
    {
        return *( new ObjectInspectorModel( _rxContext ) );
    }

    //--------------------------------------------------------------------
    void ObjectInspectorModel::createDefault()
    {
        m_aFactories.realloc( 1 );
        m_aFactories[0] <<= ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.inspection.GenericPropertyHandler" ) );
    }

    //--------------------------------------------------------------------
    void ObjectInspectorModel::createWithHandlerFactories( const Sequence< Any >& _rFactories )
    {
        if ( _rFactories.getLength() == 0 )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 0 );

        m_aFactories = _rFactories;
    }

//........................................................................
} // namespace pcr
//........................................................................

//------------------------------------------------------------------------
extern "C" void SAL_CALL createRegistryInfo_ObjectInspectorModel()
{
    ::pcr::OAutoRegistration< ::pcr::ObjectInspectorModel > aObjectInspectorModelRegistration;
}
