/*************************************************************************
 *
 *  $RCSfile: listselectiondlg.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-03-19 12:03:43 $
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

#ifndef EXTENSIONS_SOURCE_PROPCTRLR_LISTSELECTIONDLG_HXX
#include "listselectiondlg.hxx"
#endif
#ifndef EXTENSIONS_SOURCE_PROPCTRLR_LISTSELECTIONDLG_HRC
#include "listselectiondlg.hrc"
#endif

#ifndef _EXTENSIONS_PROPCTRLR_MODULEPRC_HXX_
#include "modulepcr.hxx"
#endif
#ifndef _EXTENSIONS_FORMCTRLR_PROPRESID_HRC_
#include "formresid.hrc"
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_
#include "formstrings.hxx"
#endif

#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif

/** === begin UNO includes === **/
/** === end UNO includes === **/

//........................................................................
namespace pcr
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;

    //====================================================================
    //= ListSelectionDialog
    //====================================================================
    //--------------------------------------------------------------------
    ListSelectionDialog::ListSelectionDialog( Window* _pParent, const Reference< XPropertySet >& _rxListBox,
            const ::rtl::OUString& _rPropertyName, const String& _rPropertyUIName )
        :ModalDialog( _pParent, ModuleRes( RID_DLG_SELECTION ) )
        ,m_aLabel   ( this, ResId( FT_ENTRIES ) )
        ,m_aEntries ( this, ResId( LB_ENTRIES ) )
        ,m_aOK      ( this, ResId( PB_OK      ) )
        ,m_aCancel  ( this, ResId( PB_CANCEL  ) )
        ,m_aHelp    ( this, ResId( PB_HELP    ) )
        ,m_xListBox     ( _rxListBox     )
        ,m_sPropertyName( _rPropertyName )
    {
        FreeResource();

        OSL_PRECOND( m_xListBox.is(), "ListSelectionDialog::ListSelectionDialog: invalid list box!" );

        SetText( _rPropertyUIName );
        m_aLabel.SetText( _rPropertyUIName );

        initialize( );
    }

    //------------------------------------------------------------------------
    short ListSelectionDialog::Execute()
    {
        short nResult = ModalDialog::Execute();

        if ( RET_OK == nResult )
            commitSelection();

        return nResult;
    }

    //--------------------------------------------------------------------
    void ListSelectionDialog::initialize( )
    {
        if ( !m_xListBox.is() )
            return;

        m_aEntries.SetStyle( GetStyle() | WB_SIMPLEMODE );

        try
        {
            // initialize the multi-selection flag
            sal_Bool bMultiSelection = sal_False;
            OSL_VERIFY( m_xListBox->getPropertyValue( PROPERTY_MULTISELECTION ) >>= bMultiSelection );
            m_aEntries.EnableMultiSelection( bMultiSelection );

            // fill the list box with all entries
            Sequence< ::rtl::OUString > aListEntries;
            OSL_VERIFY( m_xListBox->getPropertyValue( PROPERTY_STRINGITEMLIST ) >>= aListEntries );
            fillEntryList( aListEntries );

            // select entries according to the property
            Sequence< sal_Int16 > aSelection;
            OSL_VERIFY( m_xListBox->getPropertyValue( m_sPropertyName ) >>= aSelection );
            selectEntries( aSelection );
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "ListSelectionDialog::initialize: caught an exception!" );
        }
    }

    //--------------------------------------------------------------------
    void ListSelectionDialog::commitSelection()
    {
        if ( !m_xListBox.is() )
            return;

        Sequence< sal_Int16 > aSelection;
        collectSelection( aSelection );

        try
        {
            m_xListBox->setPropertyValue( m_sPropertyName, makeAny( aSelection ) );
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "ListSelectionDialog::commitSelection: caught an exception!" );
        }
    }

    //--------------------------------------------------------------------
    void ListSelectionDialog::fillEntryList( const Sequence< ::rtl::OUString >& _rListEntries )
    {
        m_aEntries.Clear();
        const ::rtl::OUString* _pListEntries = _rListEntries.getConstArray();
        const ::rtl::OUString* _pListEntriesEnd = _rListEntries.getConstArray() + _rListEntries.getLength();
        for ( ; _pListEntries < _pListEntriesEnd; ++_pListEntries )
            m_aEntries.InsertEntry( *_pListEntries );
    }

    //--------------------------------------------------------------------
    void ListSelectionDialog::collectSelection( Sequence< sal_Int16 >& /* [out] */ _rSelection )
    {
        sal_uInt16 nSelectedCount = m_aEntries.GetSelectEntryCount( );
        _rSelection.realloc( nSelectedCount );
        sal_Int16* pSelection = _rSelection.getArray();
        for ( sal_uInt16 selected = 0; selected < nSelectedCount; ++selected, ++pSelection )
            *pSelection = static_cast< sal_Int16 >( m_aEntries.GetSelectEntryPos( selected ) );
    }

    //--------------------------------------------------------------------
    void ListSelectionDialog::selectEntries( const Sequence< sal_Int16 >& /* [in ] */ _rSelection )
    {
        m_aEntries.SetNoSelection();
        const sal_Int16* pSelection = _rSelection.getConstArray();
        const sal_Int16* pSelectionEnd = _rSelection.getConstArray() + _rSelection.getLength();
        for ( ; pSelection != pSelectionEnd; ++pSelection )
            m_aEntries.SelectEntryPos( *pSelection );
    }

//........................................................................
}   // namespace pcr
//........................................................................
