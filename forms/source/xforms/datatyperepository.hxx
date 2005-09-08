/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: datatyperepository.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:16:15 $
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

#ifndef OFFAPI_COM_SUN_STAR_XFORMS_DATATYPEREPOSITORY_HXX
#define OFFAPI_COM_SUN_STAR_XFORMS_DATATYPEREPOSITORY_HXX

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_XFORMS_XDATATYPEREPOSITORY_HPP_
#include <com/sun/star/xforms/XDataTypeRepository.hpp>
#endif
/** === end UNO includes === **/

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif

#include <map>

//........................................................................
namespace xforms
{
//........................................................................

    class OXSDDataType;
    //====================================================================
    //= ODataTypeRepository
    //====================================================================
    typedef ::cppu::WeakImplHelper1 <   ::com::sun::star::xforms::XDataTypeRepository
                                    >   ODataTypeRepository_Base;
    class ODataTypeRepository : public ODataTypeRepository_Base
    {
    private:
        typedef ::rtl::Reference< OXSDDataType >            DataType;
        typedef ::std::map< ::rtl::OUString, DataType >     Repository;

        ::osl::Mutex                                        m_aMutex;
        Repository                                          m_aRepository;

    public:
        ODataTypeRepository( );

    protected:
        ~ODataTypeRepository( );

        // XDataTypeRepository
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::xsd::XDataType > SAL_CALL getBasicDataType( sal_Int16 dataTypeClass ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::xsd::XDataType > SAL_CALL cloneDataType( const ::rtl::OUString& sourceName, const ::rtl::OUString& newName ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::container::ElementExistException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL revokeDataType( const ::rtl::OUString& typeName ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::util::VetoException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::xsd::XDataType > SAL_CALL getDataType( const ::rtl::OUString& typeName ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);

        // XEnumerationAccess (base of XDataTypeRepository)
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createEnumeration(  ) throw (::com::sun::star::uno::RuntimeException);

        // XNameAccess (base of XDataTypeRepository)
        virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (::com::sun::star::uno::RuntimeException);

        // XElementAccess (base of XEnumerationAccess and XNameAccess)
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasElements(  ) throw (::com::sun::star::uno::RuntimeException);

    private:
        ODataTypeRepository( const ODataTypeRepository& );              // never implemented
        ODataTypeRepository& operator=( const ODataTypeRepository& );   // never implemented

    private:
        /** locates the type with the given name in our repository, or throws an exception if there is no such type
        */
        Repository::iterator    implLocate( const ::rtl::OUString& _rName, bool _bAllowMiss = false ) SAL_THROW( ( ::com::sun::star::container::NoSuchElementException ) );
    };

//........................................................................
} // namespace xforms
//........................................................................

#endif // OFFAPI_COM_SUN_STAR_XFORMS_DATATYPEREPOSITORY_HXX

