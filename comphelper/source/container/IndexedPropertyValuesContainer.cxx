
#ifndef _COM_SUN_STAR_CONTAINER_XIndexCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef __SGI_STL_VECTOR
#include <vector>
#endif

using namespace com::sun::star;

typedef std::vector < uno::Sequence< beans::PropertyValue > > IndexedPropertyValues;

uno::Sequence< rtl::OUString > SAL_CALL IndexedPropertyValuesContainer_getSupportedServiceNames() throw();
rtl::OUString SAL_CALL IndexedPropertyValuesContainer_getImplementationName() throw();
uno::Reference< uno::XInterface > SAL_CALL IndexedPropertyValuesContainer_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception );

class IndexedPropertyValuesContainer : public cppu::WeakImplHelper2< container::XIndexContainer, lang::XServiceInfo >
{
public:
    IndexedPropertyValuesContainer() throw();
    virtual ~IndexedPropertyValuesContainer() throw();

    // XIndexContainer
    virtual void SAL_CALL insertByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
        throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
            ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeByIndex( sal_Int32 nIndex )
        throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
            ::com::sun::star::uno::RuntimeException);

    // XIndexReplace
    virtual void SAL_CALL replaceByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
        throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
            ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(  )
        throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 nIndex )
        throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
            ::com::sun::star::uno::RuntimeException);

    // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  )
        throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  )
        throw(::com::sun::star::uno::RuntimeException);

    //XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

private:
    IndexedPropertyValues maProperties;
};

IndexedPropertyValuesContainer::IndexedPropertyValuesContainer() throw()
{
}

IndexedPropertyValuesContainer::~IndexedPropertyValuesContainer() throw()
{
}

// XIndexContainer
void SAL_CALL IndexedPropertyValuesContainer::insertByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
    throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
        ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize(maProperties.size());
    if ((nSize >= nIndex) && (nIndex >= 0))
    {
        uno::Sequence<beans::PropertyValue> aProps;
        if (!(aElement >>= aProps))
            throw lang::IllegalArgumentException();
        if (nSize == nIndex)
            maProperties.push_back(aProps);
        else
        {
            IndexedPropertyValues::iterator aItr;
            if ((nIndex * 2) < nSize)
            {
                aItr = maProperties.begin();
                sal_Int32 i(0);
                while(i < nIndex)
                {
                    i++;
                    aItr++;
                }
            }
            else
            {
                aItr = maProperties.end();
                sal_Int32 i(nSize - 1);
                while(i > nIndex)
                {
                    i--;
                    aItr--;
                }
            }
            maProperties.insert(aItr, aProps);
        }
    }
    else
        throw lang::IndexOutOfBoundsException();
}

void SAL_CALL IndexedPropertyValuesContainer::removeByIndex( sal_Int32 nIndex )
    throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
        ::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize(maProperties.size());
    if ((nIndex < nSize) && (nIndex >= 0))
    {
        IndexedPropertyValues::iterator aItr;
        if ((nIndex * 2) < nSize)
        {
            aItr = maProperties.begin();
            sal_Int32 i(0);
            while(i < nIndex)
            {
                i++;
                aItr++;
            }
        }
        else
        {
            aItr = maProperties.end();
            sal_Int32 i(nSize - 1);
            while(i > nIndex)
            {
                i--;
                aItr--;
            }
        }
        maProperties.erase(aItr);
    }
    else
        throw lang::IndexOutOfBoundsException();
}

// XIndexReplace
void SAL_CALL IndexedPropertyValuesContainer::replaceByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
    throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
        ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize(maProperties.size());
    if ((nIndex < nSize) && (nIndex >= 0))
    {
        uno::Sequence<beans::PropertyValue> aProps;
        if (!(aElement >>= aProps))
            throw lang::IllegalArgumentException();
        maProperties[nIndex] = aProps;
    }
    else
        throw lang::IndexOutOfBoundsException();
}

// XIndexAccess
sal_Int32 SAL_CALL IndexedPropertyValuesContainer::getCount(  )
    throw(::com::sun::star::uno::RuntimeException)
{
    return maProperties.size();
}

::com::sun::star::uno::Any SAL_CALL IndexedPropertyValuesContainer::getByIndex( sal_Int32 nIndex )
    throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
        ::com::sun::star::uno::RuntimeException)
{
    sal_Int32 nSize(maProperties.size());
    if (!((nIndex < nSize) && (nIndex >= 0)))
        throw lang::IndexOutOfBoundsException();

    uno::Any aAny;
    aAny <<= maProperties[nIndex];
    return aAny;
}

// XElementAccess
::com::sun::star::uno::Type SAL_CALL IndexedPropertyValuesContainer::getElementType(  )
    throw(::com::sun::star::uno::RuntimeException)
{
    return ::getCppuType((uno::Sequence<beans::PropertyValue> *)0);
}

sal_Bool SAL_CALL IndexedPropertyValuesContainer::hasElements(  )
    throw(::com::sun::star::uno::RuntimeException)
{
    return maProperties.size() != 0;
}

//XServiceInfo
::rtl::OUString SAL_CALL IndexedPropertyValuesContainer::getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException)
{
    return IndexedPropertyValuesContainer_getImplementationName();
}

sal_Bool SAL_CALL IndexedPropertyValuesContainer::supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException)
{
    rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.IndexedPropertyValues" ) );
    return aServiceName == ServiceName;
}

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL IndexedPropertyValuesContainer::getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException)
{
    return IndexedPropertyValuesContainer_getSupportedServiceNames();
}


uno::Sequence< rtl::OUString > SAL_CALL IndexedPropertyValuesContainer_getSupportedServiceNames() throw()
{
    const rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.IndexedPropertyValues" ) );
    const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

rtl::OUString SAL_CALL IndexedPropertyValuesContainer_getImplementationName() throw()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IndexedPropertyValuesContainer" ) );
}

uno::Reference< uno::XInterface > SAL_CALL IndexedPropertyValuesContainer_createInstance(
                const uno::Reference< lang::XMultiServiceFactory > & rSMgr ) throw( uno::Exception )
{
    return (cppu::OWeakObject*)new IndexedPropertyValuesContainer();
}

