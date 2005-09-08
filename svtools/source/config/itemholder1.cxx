/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: itemholder1.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 14:40:15 $
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

#include "itemholder1.hxx"

//-----------------------------------------------
// includes

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#include <moduleoptions.hxx>
#include <addxmltostorageoptions.hxx>
#include <extendedsecurityoptions.hxx>
#include <fontoptions.hxx>
#include <historyoptions.hxx>
#include <inetoptions.hxx>
#include <internaloptions.hxx>
#include <languageoptions.hxx>
#include <localisationoptions.hxx>
#include <menuoptions.hxx>
#include <miscopt.hxx>
#include <moduleoptions.hxx>
#include <saveopt.hxx>
#include <securityoptions.hxx>
#include <startoptions.hxx>

//-----------------------------------------------
// namespaces

namespace css = ::com::sun::star;

//-----------------------------------------------
// declarations

//-----------------------------------------------
ItemHolder1::ItemHolder1()
    : ItemHolderMutexBase()
{
    try
    {
        css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = ::comphelper::getProcessServiceFactory();
        css::uno::Reference< css::lang::XComponent > xCfg(
            xSMGR->createInstance(::rtl::OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider")),
            css::uno::UNO_QUERY);
        if (xCfg.is())
            xCfg->addEventListener(static_cast< css::lang::XEventListener* >(this));
    }
// #i37892  got errorhandling from   ConfigManager::GetConfigurationProvider()
#ifdef DBG_UTIL
    catch(css::uno::Exception& rEx)
    {
        static sal_Bool bMessage = sal_True;
        if(bMessage)
        {
            bMessage = sal_False;
            ::rtl::OString sMsg("CreateInstance with arguments exception: ");
            sMsg += ::rtl::OString(rEx.Message.getStr(),
                        rEx.Message.getLength(),
                        RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(sal_False, sMsg.getStr());
        }
    }
#else
    catch(css::uno::Exception&){}
#endif
}

//-----------------------------------------------
ItemHolder1::~ItemHolder1()
{
    impl_releaseAllItems();
}

//-----------------------------------------------
ItemHolder1* ItemHolder1::getGlobalItemHolder()
{
    static ItemHolder1* pHolder = new ItemHolder1();
    return pHolder;
}

//-----------------------------------------------
void ItemHolder1::holdConfigItem(EItem eItem)
{
    ::osl::ResettableMutexGuard aLock(m_aLock);

    TItems::const_iterator pIt;
    for (  pIt  = m_lItems.begin();
           pIt != m_lItems.end()  ;
         ++pIt                    )
    {
        const TItemInfo& rInfo = *pIt;
        if (rInfo.eItem == eItem)
            return;
    }

    TItemInfo aNewItem;
    aNewItem.eItem = eItem;
    impl_newItem(aNewItem);
    if (aNewItem.pItem)
        m_lItems.push_back(aNewItem);
}

//-----------------------------------------------
void SAL_CALL ItemHolder1::disposing(const css::lang::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    impl_releaseAllItems();
}

//-----------------------------------------------
void ItemHolder1::impl_releaseAllItems()
{
    ::osl::ResettableMutexGuard aLock(m_aLock);

    TItems::iterator pIt;
    for (  pIt  = m_lItems.begin();
           pIt != m_lItems.end()  ;
         ++pIt                    )
    {
        TItemInfo& rInfo = *pIt;
        impl_deleteItem(rInfo);
    }
    m_lItems.clear();
}

//-----------------------------------------------
void ItemHolder1::impl_newItem(TItemInfo& rItem)
{
    switch(rItem.eItem)
    {
        case E_MODULEOPTIONS :
            rItem.pItem = new SvtModuleOptions();
            break;

        case E_ADDXMLTOSTORAGEOPTIONS :
            rItem.pItem = new SvtAddXMLToStorageOptions();
            break;

        case E_EXTENDEDSECURITYOPTIONS :
            rItem.pItem = new SvtExtendedSecurityOptions();
            break;

        case E_FONTOPTIONS :
            rItem.pItem = new SvtFontOptions();
            break;

        case E_HISTORYOPTIONS :
            rItem.pItem = new SvtHistoryOptions();
            break;

        case E_INETOPTIONS :
            rItem.pItem = new SvtInetOptions();
            break;

        case E_INTERNALOPTIONS :
            rItem.pItem = new SvtInternalOptions();
            break;

        case E_LOCALISATIONOPTIONS :
            rItem.pItem = new SvtLocalisationOptions();
            break;

        case E_MENUOPTIONS :
            rItem.pItem = new SvtMenuOptions();
            break;

        case E_MISCOPTIONS :
            rItem.pItem = new SvtMiscOptions();
            break;

        case E_SAVEOPTIONS :
            rItem.pItem = new SvtSaveOptions();
            break;

        case E_SECURITYOPTIONS :
            rItem.pItem = new SvtSecurityOptions();
            break;

        case E_STARTOPTIONS :
            rItem.pItem = new SvtStartOptions();
            break;
    }
}

//-----------------------------------------------
void ItemHolder1::impl_deleteItem(TItemInfo& rItem)
{
    if (!rItem.pItem)
        return;

    switch(rItem.eItem)
    {
        case E_MODULEOPTIONS :
            delete (SvtModuleOptions*)rItem.pItem;
            break;

        case E_ADDXMLTOSTORAGEOPTIONS :
            delete (SvtAddXMLToStorageOptions*)rItem.pItem;
            break;

        case E_EXTENDEDSECURITYOPTIONS :
            delete (SvtExtendedSecurityOptions*)rItem.pItem;
            break;

        case E_FONTOPTIONS :
            delete (SvtFontOptions*)rItem.pItem;
            break;

        case E_HISTORYOPTIONS :
            delete (SvtHistoryOptions*)rItem.pItem;
            break;

        case E_INETOPTIONS :
            delete (SvtInetOptions*)rItem.pItem;
            break;

        case E_INTERNALOPTIONS :
            delete (SvtInternalOptions*)rItem.pItem;
            break;

        case E_LOCALISATIONOPTIONS :
            delete (SvtLocalisationOptions*)rItem.pItem;
            break;

        case E_MENUOPTIONS :
            delete (SvtMenuOptions*)rItem.pItem;
            break;

        case E_MISCOPTIONS :
            delete (SvtMiscOptions*)rItem.pItem;
            break;

        case E_SAVEOPTIONS :
            delete (SvtSaveOptions*)rItem.pItem;
            break;

        case E_SECURITYOPTIONS :
            delete (SvtSecurityOptions*)rItem.pItem;
            break;

        case E_STARTOPTIONS :
            delete (SvtStartOptions*)rItem.pItem;
            break;
    }

    rItem.pItem = 0;
}
