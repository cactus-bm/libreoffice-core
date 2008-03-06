/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: typeselectionpage.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-06 18:39:25 $
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
#include "precompiled_extensions.hxx"

#ifndef EXTENSIONS_ABP_TYPESELECTIONPAGE_HXX
#include "typeselectionpage.hxx"
#endif
#ifndef EXTENSIONS_ABP_ADDRESSSETTINGS_HXX
#include "addresssettings.hxx"
#endif
#ifndef EXTENSIONS_ABSPILOT_HXX
#include "abspilot.hxx"
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDRIVERACCESS_HPP_
#include <com/sun/star/sdbc/XDriverAccess.hpp>
#endif

//.........................................................................
namespace abp
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::sdbc;

    //=====================================================================
    //= TypeSelectionPage
    //=====================================================================
    //---------------------------------------------------------------------
    TypeSelectionPage::TypeSelectionPage( OAddessBookSourcePilot* _pParent )
        :AddressBookSourcePage(_pParent, ModuleRes(RID_PAGE_SELECTABTYPE))
        ,m_aHint            (this,  ModuleRes(FT_TYPE_HINTS))
        ,m_aTypeSep         (this,  ModuleRes(FL_TYPE))
        ,m_aMORK            (this,  ModuleRes(RB_MORK))
        ,m_aThunderbird     (this,  ModuleRes(RB_THUNDERBIRD))
        ,m_aEvolutionGroupwise (this,   ModuleRes(RB_EVOLUTION_GROUPWISE))
        ,m_aEvolutionLdap      (this,   ModuleRes(RB_EVOLUTION_LDAP))
        ,m_aEvolution       (this,  ModuleRes(RB_EVOLUTION))
        ,m_aKab             (this,  ModuleRes(RB_KAB))
        ,m_aMacab           (this,  ModuleRes(RB_MACAB))
        ,m_aLDAP            (this,  ModuleRes(RB_LDAP))
        ,m_aOutlook         (this,  ModuleRes(RB_OUTLOOK))
        ,m_aOE              (this,  ModuleRes(RB_OUTLOOKEXPRESS))
        ,m_aOther           (this,  ModuleRes(RB_OTHER))
    {
        Point aTopLeft;
        Size  aItemSize;

        FreeResource();

        aTopLeft = m_aMORK.GetPosPixel();
        aItemSize = m_aMORK.GetSizePixel();

        bool bWithMozilla = true, bUnx = true;
        bool bHaveEvolution = true, bHaveKab = true;
        bool bHaveMacab = true;

#ifndef WITH_MOZILLA
        bWithMozilla = false;
#endif
#ifndef UNX
        bUnx = false;
        bHaveEvolution = false;
        bHaveKab = false;
        bHaveMacab = false;
#else
        Reference< XDriverAccess> xManager(_pParent->getORB()->createInstance(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdbc.DriverManager"))), UNO_QUERY);

        // check whether Evolution is available
        Reference< XDriver > xDriver( xManager->getDriverByURL(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("sdbc:address:evolution:local"))) );
        if ( !xDriver.is() )
            bHaveEvolution = false;

        // check whether KDE address book is available
        xDriver = xManager->getDriverByURL(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("sdbc:address:kab")));
        if ( !xDriver.is() )
            bHaveKab = false;

        // check whether Mac OS X address book is available
        xDriver = xManager->getDriverByURL(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("sdbc:address:macab")));
        if ( !xDriver.is() )
            bHaveMacab = false;
#endif

        // Items are displayed in list order
        m_aAllTypes.push_back( ButtonItem( &m_aEvolution, AST_EVOLUTION, bHaveEvolution ) );
        m_aAllTypes.push_back( ButtonItem( &m_aEvolutionGroupwise, AST_EVOLUTION_GROUPWISE, bHaveEvolution ) );
        m_aAllTypes.push_back( ButtonItem( &m_aEvolutionLdap, AST_EVOLUTION_LDAP, bHaveEvolution ) );
        m_aAllTypes.push_back( ButtonItem( &m_aMORK, AST_MORK, bWithMozilla ) );
        m_aAllTypes.push_back( ButtonItem( &m_aThunderbird, AST_THUNDERBIRD, bWithMozilla ) );
        m_aAllTypes.push_back( ButtonItem( &m_aKab, AST_KAB, bHaveKab ) );
        m_aAllTypes.push_back( ButtonItem( &m_aMacab, AST_MACAB, bHaveMacab ) );
        m_aAllTypes.push_back( ButtonItem( &m_aLDAP, AST_LDAP, bWithMozilla ) );
        m_aAllTypes.push_back( ButtonItem( &m_aOutlook, AST_OUTLOOK, bWithMozilla && !bUnx ) );
        m_aAllTypes.push_back( ButtonItem( &m_aOE, AST_OE, bWithMozilla && !bUnx ) );
        m_aAllTypes.push_back( ButtonItem( &m_aOther, AST_OTHER, true ) );

        Link aTypeSelectionHandler = LINK(this, TypeSelectionPage, OnTypeSelected );
        for ( ::std::vector< ButtonItem >::const_iterator loop = m_aAllTypes.begin();
              loop != m_aAllTypes.end(); ++loop )
        {
            ButtonItem aItem = *loop;
            if (!aItem.m_bVisible)
                aItem.m_pItem->Hide();
            else
            {
                aItem.m_pItem->SetPosPixel( aTopLeft );
                aTopLeft.Y() += (aItemSize.Height() * 5) / 4;
                aItem.m_pItem->SetClickHdl( aTypeSelectionHandler );
                aItem.m_pItem->Show();
            }
        }
    }

    //---------------------------------------------------------------------
    void TypeSelectionPage::ActivatePage()
    {
        AddressBookSourcePage::ActivatePage();

        for ( ::std::vector< ButtonItem >::const_iterator loop = m_aAllTypes.begin();
              loop != m_aAllTypes.end(); ++loop )
        {
            ButtonItem aItem = (*loop);
            if( aItem.m_pItem->IsChecked() && aItem.m_bVisible )
            {
                aItem.m_pItem->GrabFocus();
                break;
            }
        }

        getDialog()->enableButtons(WZB_PREVIOUS, sal_False);
    }

    //---------------------------------------------------------------------
    void TypeSelectionPage::DeactivatePage()
    {
        AddressBookSourcePage::DeactivatePage();
        getDialog()->enableButtons(WZB_PREVIOUS, sal_True);
    }

    //---------------------------------------------------------------------
    void TypeSelectionPage::selectType( AddressSourceType _eType )
    {
        for ( ::std::vector< ButtonItem >::const_iterator loop = m_aAllTypes.begin();
              loop != m_aAllTypes.end(); ++loop )
        {
            ButtonItem aItem = (*loop);
            aItem.m_pItem->Check( _eType == aItem.m_eType );
        }
    }

    //---------------------------------------------------------------------
    AddressSourceType TypeSelectionPage::getSelectedType() const
    {
        for ( ::std::vector< ButtonItem >::const_iterator loop = m_aAllTypes.begin();
              loop != m_aAllTypes.end(); ++loop )
        {
            ButtonItem aItem = (*loop);
            if ( aItem.m_pItem->IsChecked() )
                return aItem.m_eType;
        }

        return AST_INVALID;
    }

    //---------------------------------------------------------------------
    void TypeSelectionPage::initializePage()
    {
        AddressBookSourcePage::initializePage();

        const AddressSettings& rSettings = getSettings();
        selectType(rSettings.eType);
    }

    //---------------------------------------------------------------------
    sal_Bool TypeSelectionPage::commitPage( CommitPageReason _eReason )
    {
        if (!AddressBookSourcePage::commitPage(_eReason))
            return sal_False;

        if (AST_INVALID == getSelectedType( ))
        {
            if ( _eReason != eValidateNoUI )
            {
                ErrorBox aError(this, ModuleRes(RID_ERR_NEEDTYPESELECTION));
                aError.Execute();
            }
            return sal_False;
        }

        AddressSettings& rSettings = getSettings();
        rSettings.eType = getSelectedType();

        return sal_True;
    }

    //---------------------------------------------------------------------
    bool TypeSelectionPage::canAdvance() const
    {
        return  AddressBookSourcePage::canAdvance()
            &&  (AST_INVALID != getSelectedType());
    }

    //---------------------------------------------------------------------
    IMPL_LINK( TypeSelectionPage, OnTypeSelected, void*, /*NOTINTERESTEDIN*/ )
    {
        getDialog()->typeSelectionChanged( getSelectedType() );
        updateDialogTravelUI();
        return 0L;
    }

//.........................................................................
}   // namespace abp
//.........................................................................

