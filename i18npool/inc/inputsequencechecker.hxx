/*************************************************************************
 *
 *  $RCSfile: inputsequencechecker.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: bustamam $ $Date: 2002-03-26 13:36:40 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
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
#ifndef _I18N_INPUTCHECKER_HXX_
#define _I18N_INPUTCHECKER_HXX_

#include <comphelper/processfactory.hxx>
#include <cppuhelper/implbase2.hxx> // helper for implementations
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <drafts/com/sun/star/i18n/XInputSequenceChecker.hpp>

#include <tools/list.hxx>

namespace com { namespace sun { namespace star { namespace i18n {

//  ----------------------------------------------------
//  class InputSequenceCheckerImpl
//  ----------------------------------------------------
class InputSequenceCheckerImpl : public cppu::WeakImplHelper2
<
    drafts::com::sun::star::i18n::XInputSequenceChecker,
    com::sun::star::lang::XServiceInfo
>
{
public:
    InputSequenceCheckerImpl( const com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory >& rxMSF );
    InputSequenceCheckerImpl();
    ~InputSequenceCheckerImpl();

    virtual sal_Bool SAL_CALL checkInputSequence(const rtl::OUString& Text, sal_Int32 nStartPos,
        sal_Unicode inputChar, sal_Int16 inputCheckMode) throw(com::sun::star::uno::RuntimeException);

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName() throw( com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
        throw( com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw( com::sun::star::uno::RuntimeException );

protected:
    sal_Int16 checkMode;
    sal_Char *serviceName;

private :
    struct lookupTableItem {
        lookupTableItem(const sal_Char* rLanguage, const com::sun::star::uno::Reference < drafts::com::sun::star::i18n::XInputSequenceChecker >& rxISC) :
            aLanguage(rLanguage), xISC(rxISC) {}
        const sal_Char* aLanguage;
        com::sun::star::uno::Reference < drafts::com::sun::star::i18n::XInputSequenceChecker > xISC;
    };
    List lookupTable;
    lookupTableItem *cachedItem;

    com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory > xMSF;

    com::sun::star::uno::Reference< drafts::com::sun::star::i18n::XInputSequenceChecker >& SAL_CALL getInputSequenceChecker(sal_Char* rLanguage)
        throw (com::sun::star::uno::RuntimeException);
    sal_Char* SAL_CALL getLanguageByScripType(sal_Unicode cChar, sal_Unicode nChar);
};

} } } }

#endif // _I18N_BREAKITERATOR_HXX_
