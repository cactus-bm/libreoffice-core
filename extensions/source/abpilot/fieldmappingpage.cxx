/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fieldmappingpage.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: hr $ $Date: 2005-09-23 12:49:54 $
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

#ifndef EXTENSIONS_ABP_FIELDMAPPINGPAGE_HXX
#include "fieldmappingpage.hxx"
#endif
#ifndef EXTENSIONS_ABP_FIELDMAPPINGIMPL_HXX
#include "fieldmappingimpl.hxx"
#endif
#ifndef EXTENSIONS_ABP_ADDRESSSETTINGS_HXX
#include "addresssettings.hxx"
#endif
#ifndef EXTENSIONS_ABSPILOT_HXX
#include "abspilot.hxx"
#endif

//.........................................................................
namespace abp
{
//.........................................................................

    //=====================================================================
    //= FieldMappingPage
    //=====================================================================
    //---------------------------------------------------------------------
    FieldMappingPage::FieldMappingPage( OAddessBookSourcePilot* _pParent )
        :AddressBookSourcePage( _pParent, ModuleRes( RID_PAGE_FIELDMAPPING ) )
        ,m_aExplanation     ( this, ResId( FT_FIELDASSIGMENTEXPL ) )
        ,m_aInvokeDialog    ( this, ResId( PB_INVOKE_FIELDS_DIALOG ) )
        ,m_aHint            ( this, ResId( FT_ASSIGNEDFIELDS ) )
    {
        FreeResource();

        m_aInvokeDialog.SetClickHdl( LINK( this, FieldMappingPage, OnInvokeDialog ) );

        // check the size of the InvokeDialog button - some languages are very ... gossipy here ....
        // 96349 - 09.01.2002 - fs@openoffice.org
        sal_Int32 nTextWidth = m_aInvokeDialog.GetTextWidth( m_aInvokeDialog.GetText() );

        sal_Int32 nBorderSpace = m_aInvokeDialog.LogicToPixel( Point( 4, 0 ), MAP_APPFONT ).X();
        sal_Int32 nSpace = m_aInvokeDialog.GetOutputSizePixel().Width() - 2 * nBorderSpace;

        if ( nSpace < nTextWidth )
        {
            Size aButtonSize = m_aInvokeDialog.GetSizePixel();
            aButtonSize.Width() += nTextWidth - nSpace;
            m_aInvokeDialog.SetSizePixel( aButtonSize );
        }
    }

    //---------------------------------------------------------------------
    void FieldMappingPage::ActivatePage()
    {
        AddressBookSourcePage::ActivatePage();
        m_aInvokeDialog.GrabFocus();
    }

    //---------------------------------------------------------------------
    void FieldMappingPage::DeactivatePage()
    {
        AddressBookSourcePage::DeactivatePage();
    }

    //---------------------------------------------------------------------
    void FieldMappingPage::initializePage()
    {
        AddressBookSourcePage::initializePage();
        implUpdateHint();
    }

    //---------------------------------------------------------------------
    sal_Bool FieldMappingPage::commitPage(COMMIT_REASON _eReason)
    {
        return AddressBookSourcePage::commitPage(_eReason);
    }

    //---------------------------------------------------------------------
    void FieldMappingPage::implUpdateHint()
    {
        const AddressSettings& rSettings = getSettings();
        String sHint;
        if ( 0 == rSettings.aFieldMapping.size() )
            sHint = String( ModuleRes( RID_STR_NOFIELDSASSIGNED ) );
        m_aHint.SetText( sHint );
    }

    //---------------------------------------------------------------------
    IMPL_LINK( FieldMappingPage, OnInvokeDialog, void*, NOTINTERESTEDIN )
    {
        AddressSettings& rSettings = getSettings();

        // invoke the dialog doing the mapping
        if ( fieldmapping::invokeDialog( getORB(), this, getDialog()->getDataSource().getDataSource(), rSettings ) )
        {
            if ( rSettings.aFieldMapping.size() )
                getDialog()->travelNext();
            else
                implUpdateHint();
        }

        return 0L;
    }

//.........................................................................
}   // namespace abp
//.........................................................................

