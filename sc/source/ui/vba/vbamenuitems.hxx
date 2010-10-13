#ifndef SC_VBA_MENUITEMS_HXX
#define SC_VBA_MENUITEMS_HXX

#include <ooo/vba/excel/XMenuItems.hpp>
#include <ooo/vba/excel/XMenuItem.hpp>
#include <ooo/vba/XCommandBarControls.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbacollectionimpl.hxx>

typedef CollTestImplHelper< ov::excel::XMenuItems > MenuItems_BASE;

class ScVbaMenuItems : public MenuItems_BASE
{
private:
    css::uno::Reference< ov::XCommandBarControls > m_xCommandBarControls;

public:
    ScVbaMenuItems( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< ov::XCommandBarControls >& xCommandBarControls ) throw( css::uno::RuntimeException );

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );

    // Methods
    virtual sal_Int32 SAL_CALL getCount() throw(css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& Index, const css::uno::Any& /*Index2*/ ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< ov::excel::XMenuItem > SAL_CALL Add( const rtl::OUString& Caption, const css::uno::Any& OnAction, const css::uno::Any& ShortcutKey, const css::uno::Any& Before, const css::uno::Any& Restore, const css::uno::Any& StatusBar, const css::uno::Any& HelpFile, const css::uno::Any& HelpContextID ) throw (css::script::BasicErrorException, css::uno::RuntimeException);
    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif//SC_VBA_MENUITEMS_HXX
