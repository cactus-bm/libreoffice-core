/*************************************************************************
 *
 *  $RCSfile: fieldmappingpage.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: fs $ $Date: 2001-08-01 11:05:09 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
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
        ,m_aSeparator       ( this, ResId( FL_SEPARATOR ) )
        ,m_aExplanation     ( this, ResId( FT_FIELDASSIGMENTEXPL ) )
        ,m_aInvokeDialog    ( this, ResId( PB_INVOKE_FIELDS_DIALOG ) )
        ,m_aHint            ( this, ResId( FT_ASSIGNEDFIELDS ) )
    {
        FreeResource();

        m_aInvokeDialog.SetClickHdl( LINK( this, FieldMappingPage, OnInvokeDialog ) );
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
    sal_Bool FieldMappingPage::determineNextButtonState()
    {
        return  AddressBookSourcePage::determineNextButtonState();
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
        if ( fieldmapping::invokeDialog( getORB(), this, rSettings.sDataSourceName, rSettings.sSelectedTable, rSettings.aFieldMapping ) )
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

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *
 *  Revision 1.0 27.07.01 14:17:48  fs
 ************************************************************************/

