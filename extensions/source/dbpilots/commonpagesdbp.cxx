/*************************************************************************
 *
 *  $RCSfile: commonpagesdbp.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: pjunck $ $Date: 2004-10-22 12:25:31 $
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

#ifndef _EXTENSIONS_DBP_COMMONPAGESDBP_HXX_
#include "commonpagesdbp.hxx"
#endif
#ifndef _EXTENSIONS_DBP_DBPRESID_HRC_
#include "dbpresid.hrc"
#endif
#ifndef _EXTENSIONS_COMPONENT_MODULE_HXX_
#include "componentmodule.hxx"
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XCOMPLETEDCONNECTION_HPP_
#include <com/sun/star/sdb/XCompletedConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XQUERIESSUPPLIER_HPP_
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
#include <com/sun/star/sdb/SQLContext.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_SQLWARNING_HPP_
#include <com/sun/star/sdbc/SQLWarning.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SVTOOLS_LOCALRESACCESS_HXX_
#include <svtools/localresaccess.hxx>
#endif
#ifndef _COMPHELPER_INTERACTION_HXX_
#include <comphelper/interaction.hxx>
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif
#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif
#ifndef _SFX_DOCFILT_HACK_HXX
#include <sfx2/docfilt.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif
#ifndef SVTOOLS_FILENOTATION_HXX_
#include <svtools/filenotation.hxx>
#endif
//.........................................................................
namespace dbp
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::sdb;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::task;
    using namespace ::comphelper;

    //=====================================================================
    //= OTableSelectionPage
    //=====================================================================
    //---------------------------------------------------------------------
    OTableSelectionPage::OTableSelectionPage(OControlWizard* _pParent)
        :OControlWizardPage(_pParent, ModuleRes(RID_PAGE_TABLESELECTION))
        ,m_aData            (this, ResId(FL_DATA))
        ,m_aExplanation     (this, ResId(FT_EXPLANATION))
        ,m_aDatasourceLabel (this, ResId(FT_DATASOURCE))
        ,m_aDatasource      (this, ResId(LB_DATASOURCE))
        ,m_aSearchDatabase  (this, ResId(PB_FORMDATASOURCE))
        ,m_aTableLabel      (this, ResId(FT_TABLE))
        ,m_aTable           (this, ResId(LB_TABLE))
    {
        FreeResource();

        implCollectDatasource();

        m_aDatasource.SetSelectHdl(LINK(this, OTableSelectionPage, OnListboxSelection));
        m_aTable.SetSelectHdl(LINK(this, OTableSelectionPage, OnListboxSelection));
        m_aTable.SetDoubleClickHdl(LINK(this, OTableSelectionPage, OnListboxDoubleClicked));
        m_aSearchDatabase.SetClickHdl(LINK(this, OTableSelectionPage, OnSearchClicked));

        m_aDatasource.SetDropDownLineCount(10);
    }

    //---------------------------------------------------------------------
    void OTableSelectionPage::ActivatePage()
    {
        OControlWizardPage::ActivatePage();
        m_aDatasource.GrabFocus();
    }

    //---------------------------------------------------------------------
    sal_Bool OTableSelectionPage::determineNextButtonState()
    {
        if (!OControlWizardPage::determineNextButtonState())
            return sal_False;

        if (0 == m_aDatasource.GetSelectEntryCount())
            return sal_False;

        if (0 == m_aTable.GetSelectEntryCount())
            return sal_False;

        return sal_True;
    }

    //---------------------------------------------------------------------
    void OTableSelectionPage::initializePage()
    {
        OControlWizardPage::initializePage();

        const OControlWizardContext& rContext = getContext();
        try
        {
            ::rtl::OUString sDataSourceName;
            rContext.xForm->getPropertyValue(::rtl::OUString::createFromAscii("DataSourceName")) >>= sDataSourceName;

            Reference< XConnection > xConnection;
            bool bEmbedded = ::dbtools::isEmbeddedInDatabase( rContext.xForm, xConnection );
            if ( bEmbedded )
            {
                m_aDatasource.Hide();
                m_aDatasourceLabel.Hide();
                m_aSearchDatabase.Hide();
                m_aTableLabel.SetPosPixel(m_aDatasourceLabel.GetPosPixel());
                m_aTable.SetPosPixel(m_aDatasource.GetPosPixel());
                m_aDatasource.InsertEntry(sDataSourceName);
            }
            m_aDatasource.SelectEntry(sDataSourceName);

            implFillTables(xConnection);

            ::rtl::OUString sCommand;
            OSL_VERIFY( rContext.xForm->getPropertyValue( ::rtl::OUString::createFromAscii("Command") ) >>= sCommand );
            sal_Int32 nCommandType = CommandType::TABLE;
            OSL_VERIFY( rContext.xForm->getPropertyValue( ::rtl::OUString::createFromAscii("CommandType") ) >>= nCommandType );

            // search the entry of the given type with the given name
            XubString sLookup( sCommand );
            for ( USHORT nLookup = 0; nLookup < m_aTable.GetEntryCount(); ++nLookup )
            {
                if ( m_aTable.GetEntry( nLookup ) == sLookup )
                    if ( reinterpret_cast< sal_Int32 >( m_aTable.GetEntryData( nLookup ) ) == nCommandType )
                    {
                        m_aTable.SelectEntryPos( nLookup );
                        break;
                    }
            }
        }
        catch(Exception&)
        {
            DBG_ERROR("OTableSelectionPage::initializePage: caught an exception!");
        }
    }

    //---------------------------------------------------------------------
    sal_Bool OTableSelectionPage::commitPage(IWizardPage::COMMIT_REASON _eReason)
    {
        if (!OControlWizardPage::commitPage(_eReason))
            return sal_False;

        const OControlWizardContext& rContext = getContext();
        try
        {
            Reference< XConnection > xOldConn = getFormConnection();

            ::rtl::OUString sDataSource = m_aDatasource.GetSelectEntry();
            ::rtl::OUString sCommand = m_aTable.GetSelectEntry();
            sal_Int32 nCommandType = reinterpret_cast< sal_Int32 >( m_aTable.GetEntryData( m_aTable.GetSelectEntryPos() ) );

            rContext.xForm->setPropertyValue( ::rtl::OUString::createFromAscii("DataSourceName"), makeAny( sDataSource ) );
            rContext.xForm->setPropertyValue( ::rtl::OUString::createFromAscii("Command"), makeAny( sCommand ) );
            rContext.xForm->setPropertyValue( ::rtl::OUString::createFromAscii("CommandType"), makeAny( nCommandType ) );

            setFormConnection( xOldConn, sal_False );

            if (!updateContext())
                return sal_False;
        }
        catch(Exception&)
        {
            DBG_ERROR("OTableSelectionPage::commitPage: caught an exception!");
        }

        return sal_True;
    }

    //---------------------------------------------------------------------
    IMPL_LINK( OTableSelectionPage, OnSearchClicked, PushButton*, _pButton )
    {
        ::sfx2::FileDialogHelper aFileDlg(WB_3DLOOK);
        aFileDlg.SetDisplayDirectory( SvtPathOptions().GetWorkPath() );

        static const String s_sDatabaseType = String::CreateFromAscii("StarOffice XML (Base)");
        const SfxFilter* pFilter = SfxFilter::GetFilterByName( s_sDatabaseType);
        OSL_ENSURE(pFilter,"Filter: StarOffice XML (Base) could not be found!");
        if ( pFilter )
        {
            aFileDlg.AddFilter(pFilter->GetFilterName(),pFilter->GetDefaultExtension());
        }

        if (0 == aFileDlg.Execute())
        {
            String sDataSourceName = aFileDlg.GetPath();
            ::svt::OFileNotation aFileNotation(sDataSourceName);
            sDataSourceName = aFileNotation.get(::svt::OFileNotation::N_SYSTEM);
            m_aDatasource.InsertEntry(sDataSourceName);
            m_aDatasource.SelectEntry(sDataSourceName);
            LINK(this, OTableSelectionPage, OnListboxSelection).Call(&m_aDatasource);
        }
        return 0L;
    }
    //---------------------------------------------------------------------
    IMPL_LINK( OTableSelectionPage, OnListboxDoubleClicked, ListBox*, _pBox )
    {
        if (_pBox->GetSelectEntryCount())
            getDialog()->travelNext();
        return 0L;
    }

    //---------------------------------------------------------------------
    IMPL_LINK( OTableSelectionPage, OnListboxSelection, ListBox*, _pBox )
    {
        if (&m_aDatasource == _pBox)
        {   // new data source selected
            implFillTables();
        }
        else
        {
        }

        implCheckNextButton();

        return 0L;
    }

    //---------------------------------------------------------------------
    namespace
    {
        void    lcl_fillEntries( ListBox& _rListBox, const Sequence< ::rtl::OUString >& _rNames, const Image& _rImage, sal_Int32 _nCommandType )
        {
            const ::rtl::OUString* pNames = _rNames.getConstArray();
            const ::rtl::OUString* pNamesEnd = _rNames.getConstArray() + _rNames.getLength();
            sal_uInt16 nPos = 0;
            while ( pNames != pNamesEnd )
            {
                nPos = _rListBox.InsertEntry( *pNames++, _rImage );
                _rListBox.SetEntryData( nPos, reinterpret_cast< void* >( _nCommandType ) );
            }
        }
    }

    //---------------------------------------------------------------------
    void OTableSelectionPage::implFillTables(const Reference< XConnection >& _rxConn)
    {
        m_aTable.Clear();

        WaitObject aWaitCursor(this);
        // get the default SDB interaction handler
        Reference< XMultiServiceFactory > xORB = getServiceFactory();
        Reference< XInteractionHandler > xHandler;
        const ::rtl::OUString sInteractionHandlerServiceName = ::rtl::OUString::createFromAscii("com.sun.star.sdb.InteractionHandler");
        try
        {
            if (xORB.is())
                xHandler = Reference< XInteractionHandler >(xORB->createInstance(sInteractionHandlerServiceName), UNO_QUERY);
        }
        catch(Exception&) { }
        if (!xHandler.is() && xORB.is())
        {
            ShowServiceNotAvailableError(this, sInteractionHandlerServiceName, sal_True);
            return;
        }

        // will be the table tables of the selected data source
        Sequence< ::rtl::OUString > aTableNames;
        Sequence< ::rtl::OUString > aQueryNames;

        // connect to the data source
        Any aSQLException;
        Reference< XConnection > xConn = _rxConn;
        if ( !xConn.is() )
        {
            if (!m_xDSContext.is())
                return;
            // connect to the data source
            try
            {
                ::rtl::OUString sCurrentDatasource = m_aDatasource.GetSelectEntry();
                if (sCurrentDatasource.getLength())
                {
                    // obtain the DS object
                    Reference< XCompletedConnection > xDatasource;
                    // check if I know this one otherwise transform it into a file URL
                    if ( !m_xDSContext->hasByName(sCurrentDatasource) )
                    {
                        ::svt::OFileNotation aFileNotation(sCurrentDatasource);
                        sCurrentDatasource = aFileNotation.get(::svt::OFileNotation::N_URL);
                    }

                    if (m_xDSContext->getByName(sCurrentDatasource) >>= xDatasource)
                    {   // connect
                        xConn = xDatasource->connectWithCompletion(xHandler);
                        setFormConnection( xConn );
                    }
                    else
                        DBG_ERROR("OTableSelectionPage::implFillTables: invalid data source object returned by the context");
                }
            }
            catch(SQLContext& e) { aSQLException <<= e; }
            catch(SQLWarning& e) { aSQLException <<= e; }
            catch(SQLException& e) { aSQLException <<= e; }
            catch (Exception&)
            {
                DBG_ERROR("OTableSelectionPage::implFillTables: could not fill the table list!");
            }
        }

        // will be the table tables of the selected data source
        if ( xConn.is() )
        {
            try
            {
                // get the tables
                Reference< XTablesSupplier > xSupplTables(xConn, UNO_QUERY);
                if ( xSupplTables.is() )
                {
                    Reference< XNameAccess > xTables(xSupplTables->getTables(), UNO_QUERY);
                    if (xTables.is())
                        aTableNames = xTables->getElementNames();
                }

                // and the queries
                Reference< XQueriesSupplier > xSuppQueries( xConn, UNO_QUERY );
                if ( xSuppQueries.is() )
                {
                    Reference< XNameAccess > xQueries( xSuppQueries->getQueries(), UNO_QUERY );
                    if ( xQueries.is() )
                        aQueryNames = xQueries->getElementNames();
                }

                setFormConnection( xConn );
            }
            catch(SQLContext& e) { aSQLException <<= e; }
            catch(SQLWarning& e) { aSQLException <<= e; }
            catch(SQLException& e) { aSQLException <<= e; }
            catch (Exception&)
            {
                DBG_ERROR("OTableSelectionPage::implFillTables: could not fill the table list!");
            }
        }


        if (aSQLException.hasValue() && xHandler.is())
        {   // an SQLException (or derivee) was thrown ...
            Reference< XInteractionRequest > xRequest = new OInteractionRequest(aSQLException);
            try
            {
                xHandler->handle(xRequest);
            }
            catch(Exception&) { }
            return;
        }

        Image aTableImage, aQueryImage;
        {
            ::svt::OLocalResourceAccess aLocalResAccess( ModuleRes( RID_PAGE_TABLESELECTION ), RSC_TABPAGE );

            bool bIsHiContrast = m_aTable.GetBackground().GetColor().IsDark();
            aTableImage = Image( bIsHiContrast ? IMG_TABLE_HC : IMG_TABLE );
            aQueryImage = Image( bIsHiContrast ? IMG_QUERY_HC : IMG_QUERY );
        }
        lcl_fillEntries( m_aTable, aTableNames, aTableImage, CommandType::TABLE );
        lcl_fillEntries( m_aTable, aQueryNames, aQueryImage, CommandType::QUERY );
    }

    //---------------------------------------------------------------------
    void OTableSelectionPage::implCollectDatasource()
    {
        try
        {
            m_xDSContext = getContext().xDatasourceContext;
            if (m_xDSContext.is())
                fillListBox(m_aDatasource, m_xDSContext->getElementNames());
        }
        catch (Exception&)
        {
            DBG_ERROR("OTableSelectionPage::implCollectDatasource: could not collect the data source names!");
        }
    }

    //=====================================================================
    //= OMaybeListSelectionPage
    //=====================================================================
    //---------------------------------------------------------------------
    OMaybeListSelectionPage::OMaybeListSelectionPage( OControlWizard* _pParent, const ResId& _rId )
        :OControlWizardPage(_pParent, _rId)
        ,m_pYes(NULL)
        ,m_pNo(NULL)
        ,m_pList(NULL)
    {
    }

    //---------------------------------------------------------------------
    void OMaybeListSelectionPage::announceControls(RadioButton& _rYesButton, RadioButton& _rNoButton, ListBox& _rSelection)
    {
        m_pYes = &_rYesButton;
        m_pNo = &_rNoButton;
        m_pList = &_rSelection;

        m_pYes->SetClickHdl(LINK(this, OMaybeListSelectionPage, OnRadioSelected));
        m_pNo->SetClickHdl(LINK(this, OMaybeListSelectionPage, OnRadioSelected));
        implEnableWindows();
    }

    //---------------------------------------------------------------------
    IMPL_LINK( OMaybeListSelectionPage, OnRadioSelected, RadioButton*, NOTINTERESTEDIN )
    {
        implEnableWindows();
        return 0L;
    }

    //---------------------------------------------------------------------
    void OMaybeListSelectionPage::implInitialize(const String& _rSelection)
    {
        DBG_ASSERT(m_pYes, "OMaybeListSelectionPage::implInitialize: no controls announced!");
        sal_Bool bIsSelection = (0 != _rSelection.Len());
        m_pYes->Check(bIsSelection);
        m_pNo->Check(!bIsSelection);
        m_pList->Enable(bIsSelection);

        m_pList->SelectEntry(bIsSelection ? _rSelection : String());
    }

    //---------------------------------------------------------------------
    void OMaybeListSelectionPage::implCommit(String& _rSelection)
    {
        _rSelection = m_pYes->IsChecked() ? m_pList->GetSelectEntry() : String();
    }

    //---------------------------------------------------------------------
    void OMaybeListSelectionPage::implEnableWindows()
    {
        m_pList->Enable(m_pYes->IsChecked());
    }

    //---------------------------------------------------------------------
    void OMaybeListSelectionPage::ActivatePage()
    {
        OControlWizardPage::ActivatePage();

        DBG_ASSERT(m_pYes, "OMaybeListSelectionPage::ActivatePage: no controls announced!");
        if (m_pYes->IsChecked())
            m_pList->GrabFocus();
        else
            m_pNo->GrabFocus();
    }

    //=====================================================================
    //= ODBFieldPage
    //=====================================================================
    //---------------------------------------------------------------------
    ODBFieldPage::ODBFieldPage( OControlWizard* _pParent )
        :OMaybeListSelectionPage(_pParent, ModuleRes(RID_PAGE_OPTION_DBFIELD))
        ,m_aFrame           (this, ResId(FL_DATABASEFIELD_EXPL))
        ,m_aDescription     (this, ResId(FT_DATABASEFIELD_EXPL))
        ,m_aQuestion        (this, ResId(FT_DATABASEFIELD_QUEST))
        ,m_aStoreYes        (this, ResId(RB_STOREINFIELD_YES))
        ,m_aStoreNo         (this, ResId(LB_STOREINFIELD))
        ,m_aStoreWhere      (this, ResId(RB_STOREINFIELD_NO))
    {
        FreeResource();
        announceControls(m_aStoreYes, m_aStoreNo, m_aStoreWhere);
        m_aStoreWhere.SetDropDownLineCount(10);
    }

    //---------------------------------------------------------------------
    void ODBFieldPage::initializePage()
    {
        OMaybeListSelectionPage::initializePage();

        // fill the fields page
        fillListBox(m_aStoreWhere, getContext().aFieldNames);

        implInitialize(getDBFieldSetting());
    }

    //---------------------------------------------------------------------
    sal_Bool ODBFieldPage::commitPage(IWizardPage::COMMIT_REASON _eReason)
    {
        if (!OMaybeListSelectionPage::commitPage(_eReason))
            return sal_False;

        implCommit(getDBFieldSetting());

        return sal_True;
    }

//.........................................................................
}   // namespace dbp
//.........................................................................

