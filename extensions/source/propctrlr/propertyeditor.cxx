/*************************************************************************
 *
 *  $RCSfile: propertyeditor.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 12:11:04 $
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

#ifndef _EXTENSIONS_PROPCTRLR_PROPERTYEDITOR_HXX_
#include "propertyeditor.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _EXTENSIONS_PROPCTRLR_BROWSERPAGE_HXX_
#include "browserpage.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_LINEDESCRIPTOR_HXX_
#include "linedescriptor.hxx"
#endif

//............................................................................
namespace pcr
{
//............................................................................

    //==================================================================
    // class OPropertyEditor
    //==================================================================
    DBG_NAME(OPropertyEditor)
    //------------------------------------------------------------------
    OPropertyEditor::OPropertyEditor( Window* pParent, WinBits nWinStyle)
            :Control(pParent, nWinStyle)
            ,m_aTabControl(this)
            ,m_nNextId(1)
    {
        DBG_CTOR(OPropertyEditor,NULL);

        m_aTabControl.Show();
        m_aTabControl.SetDeactivatePageHdl(LINK(this, OPropertyEditor, OnPageDeactivate));
        m_aTabControl.SetActivatePageHdl(LINK(this, OPropertyEditor, OnPageActivate));
        m_aTabControl.SetBackground(GetBackground());
        m_aTabControl.SetPaintTransparent(sal_True);
    }

    //------------------------------------------------------------------
    OPropertyEditor::~OPropertyEditor()
    {
        Hide();
        ClearAll();
        DBG_DTOR(OPropertyEditor,NULL);
    }

    //------------------------------------------------------------------
    void OPropertyEditor::ClearAll()
    {
        m_nNextId=1;
        sal_uInt16 nCount = m_aTabControl.GetPageCount();
        for(long i = nCount-1; i >= 0; --i)
        {
            sal_uInt16 nID = m_aTabControl.GetPageId((sal_uInt16)i);
            OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(nID));
            if (pPage)
            {
                pPage->EnableInput(sal_False);
                m_aTabControl.RemovePage(nID);
                delete pPage;
            }
        }
        m_aTabControl.Clear();

        {
            MapStringToPageId aEmpty;
            m_aPropertyPageIds.swap( aEmpty );
        }

        while ( !m_aHiddenPages.empty() )
        {
            delete m_aHiddenPages.begin()->second.pPage;
            m_aHiddenPages.erase( m_aHiddenPages.begin() );
        }
    }

    // #95343# ---------------------------------------------------------
    sal_Int32 OPropertyEditor::getMinimumWidth()
    {
        sal_uInt16 nCount = m_aTabControl.GetPageCount();
        sal_Int32 nPageMinWidth = 0;
        for(long i = nCount-1; i >= 0; --i)
        {
            sal_uInt16 nID = m_aTabControl.GetPageId((sal_uInt16)i);
            OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(nID));
            if (pPage)
            {
                sal_Int32 nCurPageMinWidth = pPage->getMinimumWidth();
                if( nCurPageMinWidth > nPageMinWidth )
                    nPageMinWidth = nCurPageMinWidth;
            }
        }
        return nPageMinWidth+6;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::CommitModified()
    {
        // commit all of my pages, if necessary

        sal_uInt16 nCount = m_aTabControl.GetPageCount();
        for ( sal_uInt16 i=0; i<nCount; ++i )
        {
            sal_uInt16 nID = m_aTabControl.GetPageId( i );
            OBrowserPage* pPage = static_cast< OBrowserPage* >( m_aTabControl.GetTabPage( nID ) );

            if ( pPage && pPage->getListBox() && pPage->getListBox()->IsModified() )
                pPage->getListBox()->CommitModified();
        }
    }

    //------------------------------------------------------------------
    void OPropertyEditor::GetFocus()
    {
        m_aTabControl.GrabFocus();
    }

    //------------------------------------------------------------------
    OBrowserPage* OPropertyEditor::getPage( const ::rtl::OUString& _rPropertyName )
    {
        OBrowserPage* pPage = NULL;
        MapStringToPageId::const_iterator aPropertyPageIdPos = m_aPropertyPageIds.find( _rPropertyName );
        if ( aPropertyPageIdPos != m_aPropertyPageIds.end() )
            pPage = static_cast< OBrowserPage* >( m_aTabControl.GetTabPage( aPropertyPageIdPos->second ) );
        return pPage;
    }

    //------------------------------------------------------------------
    const OBrowserPage* OPropertyEditor::getPage( const ::rtl::OUString& _rPropertyName ) const
    {
        return const_cast< OPropertyEditor* >( this )->getPage( _rPropertyName );
    }

    //------------------------------------------------------------------
    OBrowserPage* OPropertyEditor::getPage( sal_uInt16& _rPageId )
    {
        if ( EDITOR_PAGE_CURRENT == _rPageId )
            _rPageId = m_aTabControl.GetCurPageId();
        return static_cast< OBrowserPage* >( m_aTabControl.GetTabPage( _rPageId ) );
    }

    //------------------------------------------------------------------
    const OBrowserPage* OPropertyEditor::getPage( sal_uInt16& _rPageId ) const
    {
        return const_cast< OPropertyEditor* >( this )->getPage( _rPageId );
    }

    //------------------------------------------------------------------
    void OPropertyEditor::Resize()
    {
        Size aSize( GetOutputSizePixel() );
        aSize.Width() -= 6;
        aSize.Height() -= 6;
        m_aTabControl.SetPosSizePixel( Point( 3, 3 ), aSize );

    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::AppendPage(const String & _rText,sal_uInt32 _nHelpId)
    {
        // obtain a new id
        sal_uInt16 nId = m_nNextId++;
        // insert the id
        m_aTabControl.InsertPage(nId, _rText);

        // create a new page
        OBrowserPage* pPage = new OBrowserPage(&m_aTabControl);
        pPage->SetText( _rText );
        // some knittings
        pPage->SetSizePixel(m_aTabControl.GetTabPageSizePixel());
        pPage->getListBox()->setListener(m_pListener);
        pPage->SetHelpId(_nHelpId);

        // immediately activate the page
        m_aTabControl.SetTabPage(nId, pPage);
        m_aTabControl.SetCurPageId(nId);

        return nId;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::SetHelpId( sal_uInt32 nHelpId )
    {
        Control::SetHelpId(0);
        m_aTabControl.SetHelpId(nHelpId);
    }

    //------------------------------------------------------------------
    void OPropertyEditor::RemovePage(sal_uInt16 nID)
    {
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(nID));

        if (pPage)
            pPage->EnableInput(sal_False);
        m_aTabControl.RemovePage(nID);
        if (pPage)
            delete pPage;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::SetPage(sal_uInt16 nId)
    {
        m_aTabControl.SetCurPageId(nId);
    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::GetCurPage()
    {
        if(m_aTabControl.GetPageCount()>0)
            return m_aTabControl.GetCurPageId();
        else
            return 0;
    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::CalcVisibleLines()
    {
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(m_aTabControl.GetCurPageId()));
        if (pPage)
            return pPage->getListBox()->CalcVisibleLines();
        else return 0;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::EnableUpdate()
    {
        // forward this to all our pages
        sal_uInt16 nCount = m_aTabControl.GetPageCount();
        for (sal_uInt16 i=0;i<nCount;++i)
        {
            sal_uInt16 nID = m_aTabControl.GetPageId(i);
            OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(nID));
            if (pPage)
                pPage->getListBox()->EnableUpdate();
        }
    }

    //------------------------------------------------------------------
    void OPropertyEditor::DisableUpdate()
    {
        // forward this to all our pages
        sal_uInt16 nCount = m_aTabControl.GetPageCount();
        for (sal_uInt16 i=0;i<nCount;++i)
        {
            sal_uInt16 nID = m_aTabControl.GetPageId(i);
            OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(nID));
            if (pPage)
                pPage->getListBox()->DisableUpdate();
        }
    }

    //------------------------------------------------------------------
    void OPropertyEditor::SetLineListener(IPropertyLineListener* _pListener)
    {
        m_pListener = _pListener;

        // forward the new listener to our pages
        sal_uInt16 nCount = m_aTabControl.GetPageCount();
        for (sal_uInt16 i=0;i<nCount;++i)
        {
            sal_uInt16 nID = m_aTabControl.GetPageId(i);
            OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(nID));
            if (pPage)
                pPage->getListBox()->setListener(m_pListener);
        }

    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::InsertEntry( const OLineDescriptor& rData, sal_uInt16 nPos, sal_uInt16 _nPageId )
    {
        // let the current page handle this
        sal_uInt16 nEntry = LISTBOX_ENTRY_NOTFOUND;
        OBrowserPage* pPage = getPage( _nPageId );
        if ( pPage )
        {
            nEntry = pPage->getListBox()->InsertEntry( rData, nPos );

            OSL_ENSURE( m_aPropertyPageIds.find( rData.sName ) == m_aPropertyPageIds.end(),
                "OPropertyEditor::InsertEntry: property already present in the map!" );
            m_aPropertyPageIds.insert( MapStringToPageId::value_type( rData.sName, _nPageId ) );
        }

        return nEntry;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::RemoveEntry( const ::rtl::OUString& _rName )
    {
        OBrowserPage* pPage = getPage( _rName );
        if ( pPage )
        {
            OSL_VERIFY( pPage->getListBox()->RemoveEntry( _rName ) );

            OSL_ENSURE( m_aPropertyPageIds.find( _rName ) != m_aPropertyPageIds.end(),
                "OPropertyEditor::RemoveEntry: property not present in the map!" );
            m_aPropertyPageIds.erase( _rName );
        }
    }

    //------------------------------------------------------------------
    void OPropertyEditor::ChangeEntry( const OLineDescriptor& rData )
    {
        OBrowserPage* pPage = getPage( rData.sName );
        if ( pPage )
            pPage->getListBox()->ChangeEntry( rData, EDITOR_LIST_REPLACE_EXISTING );
    }

    //------------------------------------------------------------------
    void OPropertyEditor::SetPropertyValue( const ::rtl::OUString & rEntryName, const ::rtl::OUString & rValue )
    {
        OBrowserPage* pPage = getPage( rEntryName );
        if ( pPage )
            pPage->getListBox()->SetPropertyValue( rEntryName, rValue );
    }

    //------------------------------------------------------------------
    ::rtl::OUString OPropertyEditor::GetPropertyValue( const ::rtl::OUString& rEntryName ) const
    {
        ::rtl::OUString aString;
        const OBrowserPage* pPage = getPage( rEntryName );
        if ( pPage )
            aString = pPage->getListBox()->GetPropertyValue( rEntryName );
        return aString;
    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::GetPropertyPos( const ::rtl::OUString& rEntryName ) const
    {
        sal_uInt16 nVal=LISTBOX_ENTRY_NOTFOUND;
        const OBrowserPage* pPage = getPage( rEntryName );
        if ( pPage )
            nVal = pPage->getListBox()->GetPropertyPos( rEntryName );
        return nVal;
    }

    //------------------------------------------------------------------
    sal_Bool OPropertyEditor::IsPropertyInputEnabled( const ::rtl::OUString& _rEntryName ) const
    {
        // "yes" if and only if all pages say "yes" (since pages which do not know this property
        // will answer "yes")

        for ( USHORT i = 0; i < m_aTabControl.GetPageCount(); ++i )
        {
            OBrowserPage* pPage = static_cast< OBrowserPage* >( m_aTabControl.GetTabPage( m_aTabControl.GetPageId( i ) ) );
            if ( pPage )
                if ( !pPage->getListBox()->IsPropertyInputEnabled( _rEntryName) )
                    return sal_False;
        }
        return sal_True;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::ShowPropertyPage( sal_uInt16 _nPageId, bool _bShow )
    {
        if ( !_bShow )
        {
            sal_uInt16 nPagePos = m_aTabControl.GetPagePos( _nPageId );
            if ( TAB_PAGE_NOTFOUND == nPagePos )
                return;
            DBG_ASSERT( m_aHiddenPages.find( _nPageId ) == m_aHiddenPages.end(), "OPropertyEditor::ShowPropertyPage: page already hidden!" );

            m_aHiddenPages[ _nPageId ] = HiddenPage( nPagePos, m_aTabControl.GetTabPage( _nPageId ) );
          m_aTabControl.RemovePage( _nPageId );
        }
        else
        {
            ::std::map< sal_uInt16, HiddenPage >::iterator aPagePos = m_aHiddenPages.find( _nPageId );
            if ( aPagePos == m_aHiddenPages.end() )
                return;

          aPagePos->second.pPage->SetSizePixel( m_aTabControl.GetTabPageSizePixel() );
          m_aTabControl.InsertPage( aPagePos->first, aPagePos->second.pPage->GetText(), aPagePos->second.nPos );
            m_aTabControl.SetTabPage( aPagePos->first, aPagePos->second.pPage );

            m_aHiddenPages.erase( aPagePos );
        }
    }

    //------------------------------------------------------------------
    void OPropertyEditor::EnablePropertyControls( const ::rtl::OUString& _rEntryName, bool _bEnableInput, bool _bEnablePrimaryButton, bool _bEnableSecondaryButton )
    {
        for ( USHORT i = 0; i < m_aTabControl.GetPageCount(); ++i )
        {
            OBrowserPage* pPage = static_cast< OBrowserPage* >( m_aTabControl.GetTabPage( m_aTabControl.GetPageId( i ) ) );
            if ( pPage )
                pPage->getListBox()->EnablePropertyControls( _rEntryName, _bEnableInput, _bEnablePrimaryButton, _bEnableSecondaryButton );
        }
    }

    //------------------------------------------------------------------
    void OPropertyEditor::EnablePropertyLine( const ::rtl::OUString& _rEntryName, bool _bEnable )
    {
        for ( USHORT i = 0; i < m_aTabControl.GetPageCount(); ++i )
        {
            OBrowserPage* pPage = static_cast< OBrowserPage* >( m_aTabControl.GetTabPage( m_aTabControl.GetPageId( i ) ) );
            if ( pPage )
                pPage->getListBox()->EnablePropertyLine( _rEntryName, _bEnable );
        }

    }

    //------------------------------------------------------------------
    IBrowserControl* OPropertyEditor::GetPropertyControl(const ::rtl::OUString& rEntryName)
    {
        // let the current page handle this
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(m_aTabControl.GetCurPageId()));
        if (pPage)
            return pPage->getListBox()->GetPropertyControl(rEntryName);
        return NULL;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::SetFirstVisibleEntry(sal_uInt16 nPos)
    {
        // let the current page handle this
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(m_aTabControl.GetCurPageId()));
        if(pPage)
            pPage->getListBox()->SetFirstVisibleEntry(nPos);
    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::GetFirstVisibleEntry()
    {
        // let the current page handle this
        sal_uInt16 nEntry=LISTBOX_ENTRY_NOTFOUND;
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(m_aTabControl.GetCurPageId()));
        if(pPage)
            nEntry=pPage->getListBox()->GetFirstVisibleEntry();
        return nEntry;
    }

    //------------------------------------------------------------------
    void OPropertyEditor::SetSelectedEntry(sal_uInt16 nPos)
    {
        // let the current page handle this
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(m_aTabControl.GetCurPageId()));
        if(pPage)
            pPage->getListBox()->SetSelectedEntry(nPos);
    }

    //------------------------------------------------------------------
    sal_uInt16 OPropertyEditor::GetSelectedEntry()
    {
        sal_uInt16 nEntry=LISTBOX_ENTRY_NOTFOUND;
        // let the current page handle this
        OBrowserPage* pPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage(m_aTabControl.GetCurPageId()));
        if(pPage)
            nEntry=pPage->getListBox()->GetSelectedEntry();
        return nEntry;
    }

    //------------------------------------------------------------------
    IMPL_LINK(OPropertyEditor, OnPageActivate, TabControl*, EMPTYARG)
    {
        if (m_aPageActivationHandler.IsSet())
            m_aPageActivationHandler.Call(NULL);
        return 0L;
    }

    //------------------------------------------------------------------
    IMPL_LINK(OPropertyEditor, OnPageDeactivate, TabControl*, EMPTYARG)
    {
        // commit the data on the current (to-be-decativated) tab page
        // (79404)
        sal_Int32 nCurrentId = m_aTabControl.GetCurPageId();
        OBrowserPage* pCurrentPage = static_cast<OBrowserPage*>(m_aTabControl.GetTabPage((sal_uInt16)nCurrentId));
        OBrowserListBox* pListBox = pCurrentPage ? pCurrentPage->getListBox() : NULL;
        IBrowserControl* pControl = pListBox ? pListBox->GetCurrentPropertyControl() : NULL;
        if (pControl && pControl->IsModified())
            pControl->CommitModified();

        return 1L;
    }

//............................................................................
} // namespace pcr
//............................................................................


