/*************************************************************************
 *
 *  $RCSfile: dbaexchange.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: oj $ $Date: 2002-10-25 08:34:28 $
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

#ifndef _SVX_DBAEXCHANGE_HXX_
#include "dbaexchange.hxx"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XSQLQUERYCOMPOSERFACTORY_HPP_
#include <com/sun/star/sdb/XSQLQueryComposerFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XSQLQUERYCOMPOSERFACTORY_HPP_
#include <com/sun/star/sdb/XSQLQueryComposerFactory.hpp>
#endif
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _SOT_FORMATS_HXX
#include <sot/formats.hxx>
#endif
#ifndef _SOT_EXCHANGE_HXX
#include <sot/exchange.hxx>
#endif
#ifndef _COMPHELPER_PROPERTSETINFO_HXX_
#include <comphelper/propertysetinfo.hxx>
#endif
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif

//........................................................................
namespace svx
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::sdb;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::datatransfer;
    using namespace ::svxform;
    using namespace ::comphelper;

    //====================================================================
    //= OColumnTransferable
    //====================================================================
    //--------------------------------------------------------------------
    OColumnTransferable::OColumnTransferable(const ::rtl::OUString& _rDatasource, const sal_Int32 _nCommandType,
            const ::rtl::OUString& _rCommand, const ::rtl::OUString& _rFieldName, sal_Int32 _nFormats)
        :m_nFormatFlags(_nFormats)
    {
        implConstruct(_rDatasource, _nCommandType, _rCommand, _rFieldName);
    }

    //--------------------------------------------------------------------
    OColumnTransferable::OColumnTransferable(const Reference< XPropertySet >& _rxForm,
            const ::rtl::OUString& _rFieldName, const Reference< XPropertySet >& _rxColumn,
            const Reference< XConnection >& _rxConnection, sal_Int32 _nFormats)
        :m_nFormatFlags(_nFormats)
    {
        OSL_ENSURE(_rxForm.is(), "OColumnTransferable::OColumnTransferable: invalid form!");
        // collect the necessary information from the form
        ::rtl::OUString sCommand;
        sal_Int32       nCommandType = CommandType::TABLE;
        ::rtl::OUString sDatasource;

        sal_Bool        bTryToParse = sal_True;
        try
        {
            _rxForm->getPropertyValue(FM_PROP_COMMANDTYPE)  >>= nCommandType;
            _rxForm->getPropertyValue(FM_PROP_COMMAND)      >>= sCommand;
            _rxForm->getPropertyValue(FM_PROP_DATASOURCE)   >>= sDatasource;
            bTryToParse = ::cppu::any2bool(_rxForm->getPropertyValue(FM_PROP_ESCAPE_PROCESSING));
        }
        catch(Exception&)
        {
            OSL_ENSURE(sal_False, "OColumnTransferable::OColumnTransferable: could not collect essential data source attributes !");
        }

        // If the data source is an SQL-statement and simple enough (means "select <field list> from <table> where ....")
        // we are able to fake the drag information we are about to create.
        if (bTryToParse && (CommandType::COMMAND == nCommandType))
        {
            try
            {
                // need a query composer for this
                Reference< XSQLQueryComposerFactory > xComposerFac;
                _rxForm->getPropertyValue(FM_PROP_ACTIVE_CONNECTION) >>= xComposerFac;
                Reference< XSQLQueryComposer > xComposer;
                if (xComposerFac.is())
                    xComposer = xComposerFac->createQueryComposer();

                if (xComposer.is())
                {
                    ::rtl::OUString sActiveCommand;
                    _rxForm->getPropertyValue(FM_PROP_ACTIVECOMMAND) >>= sActiveCommand;
                    xComposer->setQuery(sActiveCommand);
                    Reference< XTablesSupplier > xSupTab(xComposer, UNO_QUERY);
                    if(xSupTab.is())
                    {
                        Reference< XNameAccess > xNames = xSupTab->getTables();
                        if (xNames.is())
                        {
                            Sequence< ::rtl::OUString > aTables = xNames->getElementNames();
                            if (1 == aTables.getLength())
                            {
                                sCommand        = aTables[0];
                                nCommandType    = CommandType::TABLE;
                            }
                        }
                    }
                }
            }
            catch(Exception&)
            {
                OSL_ENSURE(sal_False, "OColumnTransferable::OColumnTransferable: could not collect essential data source attributes (part two) !");
            }
        }

        implConstruct(sDatasource, nCommandType, sCommand, _rFieldName);

        if ((m_nFormatFlags & CTF_COLUMN_DESCRIPTOR) == CTF_COLUMN_DESCRIPTOR)
        {
            if (_rxColumn.is())
                m_aDescriptor[daColumnObject] <<= _rxColumn;
            if (_rxConnection.is())
                m_aDescriptor[daConnection] <<= _rxConnection;
        }
    }

    //--------------------------------------------------------------------
    sal_uInt32 OColumnTransferable::getDescriptorFormatId()
    {
        static sal_uInt32 s_nFormat = (sal_uInt32)-1;
        if ((sal_uInt32)-1 == s_nFormat)
        {
            s_nFormat = SotExchange::RegisterFormatName(String::CreateFromAscii("application/x-openoffice;windows_formatname=\"dbaccess.ColumnDescriptorTransfer\""));
            OSL_ENSURE((sal_uInt32)-1 != s_nFormat, "OColumnTransferable::getDescriptorFormatId: bad exchange id!");
        }
        return s_nFormat;
    }

    //--------------------------------------------------------------------
    void OColumnTransferable::implConstruct(const ::rtl::OUString& _rDatasource, const sal_Int32 _nCommandType,
        const ::rtl::OUString& _rCommand, const ::rtl::OUString& _rFieldName)
    {
        const sal_Unicode       cSeparator = sal_Unicode(11);
        const ::rtl::OUString   sSeparator(&cSeparator, 1);

        m_sCompatibleFormat = ::rtl::OUString();
        m_sCompatibleFormat += _rDatasource;
        m_sCompatibleFormat += sSeparator;
        m_sCompatibleFormat += _rCommand;
        m_sCompatibleFormat += sSeparator;

        sal_Unicode cCommandType;
        switch (_nCommandType)
        {
            case CommandType::TABLE:
                cCommandType = '0';
                break;
            case CommandType::QUERY:
                cCommandType = '1';
                break;
            default:
                cCommandType = '2';
                break;
        }
        m_sCompatibleFormat += ::rtl::OUString(&cCommandType, 1);
        m_sCompatibleFormat += sSeparator;
        m_sCompatibleFormat += _rFieldName;

        m_aDescriptor.clear();
        if ((m_nFormatFlags & CTF_COLUMN_DESCRIPTOR) == CTF_COLUMN_DESCRIPTOR)
        {
            m_aDescriptor[daDataSource]     <<= _rDatasource;
            m_aDescriptor[daCommand]        <<= _rCommand;
            m_aDescriptor[daCommandType]    <<= _nCommandType;
            m_aDescriptor[daColumnName]     <<= _rFieldName;
        }
    }

    //--------------------------------------------------------------------
    void OColumnTransferable::AddSupportedFormats()
    {
        if (CTF_CONTROL_EXCHANGE & m_nFormatFlags)
            AddFormat(SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE);

        if (CTF_FIELD_DESCRIPTOR & m_nFormatFlags)
            AddFormat(SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE);

        if (CTF_COLUMN_DESCRIPTOR & m_nFormatFlags)
            AddFormat(getDescriptorFormatId());
    }

    //--------------------------------------------------------------------
    sal_Bool OColumnTransferable::GetData( const DataFlavor& _rFlavor )
    {
        const sal_uInt32 nFormatId = SotExchange::GetFormat(_rFlavor);
        switch (nFormatId)
        {
            case SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE:
            case SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE:
                return SetString(m_sCompatibleFormat, _rFlavor);
        }
        if (nFormatId == getDescriptorFormatId())
            return SetAny( makeAny( m_aDescriptor.createPropertyValueSequence() ), _rFlavor );

        return sal_False;
    }

    //--------------------------------------------------------------------
    sal_Bool OColumnTransferable::canExtractColumnDescriptor(const DataFlavorExVector& _rFlavors, sal_Int32 _nFormats)
    {
        sal_Bool bFieldFormat       = 0 != (_nFormats & CTF_FIELD_DESCRIPTOR);
        sal_Bool bControlFormat     = 0 != (_nFormats & CTF_CONTROL_EXCHANGE);
        sal_Bool bDescriptorFormat  = 0 != (_nFormats & CTF_COLUMN_DESCRIPTOR);
        for (   DataFlavorExVector::const_iterator aCheck = _rFlavors.begin();
                aCheck != _rFlavors.end();
                ++aCheck
            )
        {
            if (bFieldFormat && (SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE == aCheck->mnSotId))
                return sal_True;
            if (bControlFormat && (SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE == aCheck->mnSotId))
                return sal_True;
            if (bDescriptorFormat && (getDescriptorFormatId() == aCheck->mnSotId))
                return sal_True;
        }

        return sal_False;
    }

    //--------------------------------------------------------------------
    ODataAccessDescriptor OColumnTransferable::extractColumnDescriptor(const TransferableDataHelper& _rData)
    {
        if (_rData.HasFormat(getDescriptorFormatId()))
        {
            // the object has a real descriptor object (not just the old compatible format)

            // extract the any from the transferable
            DataFlavor aFlavor;
#ifdef _DEBUG
            sal_Bool bSuccess =
#endif
            SotExchange::GetFormatDataFlavor(getDescriptorFormatId(), aFlavor);
            OSL_ENSURE(bSuccess, "OColumnTransferable::extractColumnDescriptor: invalid data format (no flavor)!");

            Any aDescriptor = _rData.GetAny(aFlavor);

            // extract the property value sequence
            Sequence< PropertyValue > aDescriptorProps;
#ifdef _DEBUG
            bSuccess =
#endif
            aDescriptor >>= aDescriptorProps;
            OSL_ENSURE(bSuccess, "OColumnTransferable::extractColumnDescriptor: invalid clipboard format!");

            // build the real descriptor
            return ODataAccessDescriptor(aDescriptorProps);
        }

        // only the old (compatible) format exists -> use the other extract method ...
        ::rtl::OUString sDatasource, sCommand, sFieldName;
        sal_Int32 nCommandType = CommandType::COMMAND;

        ODataAccessDescriptor aDescriptor;
        if (extractColumnDescriptor(_rData, sDatasource, nCommandType, sCommand, sFieldName))
        {
            // and build an own descriptor
            aDescriptor[daDataSource]   <<= sDatasource;
            aDescriptor[daCommand]      <<= sCommand;
            aDescriptor[daCommandType]  <<= nCommandType;
            aDescriptor[daColumnName]   <<= sFieldName;
        }
        return aDescriptor;
    }

    //--------------------------------------------------------------------
    sal_Bool OColumnTransferable::extractColumnDescriptor(const TransferableDataHelper& _rData,
        ::rtl::OUString& _rDatasource, sal_Int32& _nCommandType, ::rtl::OUString& _rCommand, ::rtl::OUString& _rFieldName)
    {
        if (_rData.HasFormat(getDescriptorFormatId()))
        {
            ODataAccessDescriptor aDescriptor = extractColumnDescriptor(_rData);
            aDescriptor[daDataSource]   >>= _rDatasource;
            aDescriptor[daCommand]      >>= _rCommand;
            aDescriptor[daCommandType]  >>= _nCommandType;
            aDescriptor[daColumnName]   >>= _rFieldName;
            return sal_True;
        }

        // check if we have a (string) format we can use ....
        SotFormatStringId   nRecognizedFormat = 0;
        if (_rData.HasFormat(SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE))
            nRecognizedFormat = SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE;
        if (_rData.HasFormat(SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE))
            nRecognizedFormat = SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE;
        if (!nRecognizedFormat)
            return sal_False;

        String sFieldDescription;
        const_cast<TransferableDataHelper&>(_rData).GetString(nRecognizedFormat, sFieldDescription);

        const sal_Unicode cSeparator = sal_Unicode(11);
        _rDatasource    = sFieldDescription.GetToken(0, cSeparator);
        _rCommand       = sFieldDescription.GetToken(1, cSeparator);
        _nCommandType   = sFieldDescription.GetToken(2, cSeparator).ToInt32();
        _rFieldName     = sFieldDescription.GetToken(3, cSeparator);

        return sal_True;
    }

    //--------------------------------------------------------------------
    void OColumnTransferable::addDataToContainer( TransferDataContainer* _pContainer )
    {
        OSL_ENSURE( _pContainer, "OColumnTransferable::addDataToContainer: invalid container!" );
        if ( _pContainer )
        {
            if ( m_nFormatFlags & CTF_FIELD_DESCRIPTOR )
                _pContainer->CopyAny( SOT_FORMATSTR_ID_SBA_FIELDDATAEXCHANGE, makeAny( m_sCompatibleFormat ) );

            if ( m_nFormatFlags & CTF_CONTROL_EXCHANGE )
                _pContainer->CopyAny( SOT_FORMATSTR_ID_SBA_CTRLDATAEXCHANGE, makeAny( m_sCompatibleFormat ) );

            if ( m_nFormatFlags & CTF_COLUMN_DESCRIPTOR )
            {
                Any aContent = makeAny( m_aDescriptor.createPropertyValueSequence() );
                _pContainer->CopyAny( getDescriptorFormatId(), aContent );
            }
        }
    }

    //====================================================================
    //= ODataAccessObjectTransferable
    //====================================================================
    //--------------------------------------------------------------------
    ODataAccessObjectTransferable::ODataAccessObjectTransferable(
                    const ::rtl::OUString&  _rDatasource,
                    const sal_Int32         _nCommandType,
                    const ::rtl::OUString&  _rCommand,
                    const Reference< XConnection >& _rxConnection)
    {
        // build the descriptor (the property sequence)
        m_aDescriptor[daDataSource]     <<= _rDatasource;
        m_aDescriptor[daConnection]     <<= _rxConnection;
        m_aDescriptor[daCommand]        <<= _rCommand;
        m_aDescriptor[daCommandType]    <<= _nCommandType;

        // extract the single values from the sequence
        ::rtl::OUString sDatasourceName;
        ::rtl::OUString sObjectName;
        sal_Bool bEscapeProcessing = sal_True;
        sDatasourceName = _rDatasource;
        sObjectName = _rCommand;

        // for compatibility: create a string which can be used for the SOT_FORMATSTR_ID_SBA_DATAEXCHANGE format

        sal_Bool bTreatAsStatement = (CommandType::COMMAND == _nCommandType);
            // statements are - in this old and ugly format - described as queries

        const sal_Unicode       cSeparator = sal_Unicode(11);
        const ::rtl::OUString   sSeparator(&cSeparator, 1);

        const sal_Unicode       cTableMark = '1';
        const sal_Unicode       cQueryMark = '0';

        // build the descriptor string
        m_sCompatibleObjectDescription += sDatasourceName;
        m_sCompatibleObjectDescription += sSeparator;
        m_sCompatibleObjectDescription += bTreatAsStatement ? ::rtl::OUString() : sObjectName;
        m_sCompatibleObjectDescription += sSeparator;
        switch (_nCommandType)
        {
            case CommandType::TABLE:
                m_sCompatibleObjectDescription += ::rtl::OUString(&cTableMark, 1);
                break;
            case CommandType::QUERY:
                m_sCompatibleObjectDescription += ::rtl::OUString(&cQueryMark, 1);
                break;
            case CommandType::COMMAND:
                m_sCompatibleObjectDescription += ::rtl::OUString(&cQueryMark, 1);
                // think of it as a query
                break;
        }
        m_sCompatibleObjectDescription += sSeparator;
        m_sCompatibleObjectDescription += bTreatAsStatement ? sObjectName : ::rtl::OUString();
        m_sCompatibleObjectDescription += sSeparator;
    }

    // -----------------------------------------------------------------------------
    ODataAccessObjectTransferable::ODataAccessObjectTransferable(const Reference< XPropertySet >& _rxLivingForm)
    {
        // collect some properties of the form
        ::rtl::OUString sDatasourceName;
        sal_Int32       nObjectType = CommandType::COMMAND;
        ::rtl::OUString sObjectName;
        Reference< XConnection > xConnection;
        try
        {
            _rxLivingForm->getPropertyValue(FM_PROP_COMMANDTYPE) >>= nObjectType;
            _rxLivingForm->getPropertyValue(FM_PROP_COMMAND) >>= sObjectName;
            _rxLivingForm->getPropertyValue(FM_PROP_DATASOURCE) >>= sDatasourceName;
            _rxLivingForm->getPropertyValue(FM_PROP_ACTIVE_CONNECTION) >>= xConnection;
        }
        catch(Exception&)
        {
            OSL_ENSURE(sal_False, "ODataAccessObjectTransferable::ODataAccessObjectTransferable: could not collect essential form attributes !");
            return;
        }

        m_aDescriptor[daDataSource]     <<= sDatasourceName;
        m_aDescriptor[daCommandType]    <<= nObjectType;
        m_aDescriptor[daCommand]        <<= sObjectName;

         if ( xConnection.is() )     // #104474# OJ
            m_aDescriptor[daConnection] <<= xConnection;

        sal_Bool bIsStatement = CommandType::COMMAND == nObjectType;
        String sObjectKind = (CommandType::TABLE == nObjectType) ? String('1') : String('0');

        // check if the SQL-statement is modified
        sal_Bool bHasFilterOrSort(sal_False);
        ::rtl::OUString sCompleteStatement;
        try
        {
            ::rtl::OUString sFilter, sSort;
            if (::cppu::any2bool(_rxLivingForm->getPropertyValue(FM_PROP_APPLYFILTER)))
                _rxLivingForm->getPropertyValue(FM_PROP_FILTER_CRITERIA) >>= sFilter;
            _rxLivingForm->getPropertyValue(FM_PROP_SORT) >>= sSort;
            bHasFilterOrSort = (sFilter.getLength()>0) || (sSort.getLength()>0);

            _rxLivingForm->getPropertyValue(FM_PROP_ACTIVECOMMAND) >>= sCompleteStatement;

            // create a composer
            Reference< XSQLQueryComposerFactory > xFactory( xConnection, UNO_QUERY );
            Reference< XSQLQueryComposer > xComposer;
            if (xFactory.is())
                xComposer = xFactory->createQueryComposer();

            // let the composer compose
            if (xComposer.is())
            {
                xComposer->setQuery(sCompleteStatement);
                xComposer->setFilter(sFilter);
                xComposer->setOrder(sSort);
                sCompleteStatement = xComposer->getComposedQuery();
            }
            // Usually, I would expect the result of the composing to be the same as the ActiveCommand property
            // But this code here is pretty old, and I don't know wha the side effects are if I remove it now ...
        }
        catch(Exception&)
        {
            OSL_ENSURE(sal_False, "ODataAccessObjectTransferable::ODataAccessObjectTransferable: could not collect essential form attributes (part two) !");
            return;
        }

        // build the object description (as string)
        const sal_Unicode       cSeparator(11);
        const ::rtl::OUString   sSeparator(&cSeparator, 1);

        m_sCompatibleObjectDescription = sDatasourceName;
        m_sCompatibleObjectDescription  += sSeparator;
        m_sCompatibleObjectDescription  += bIsStatement ? ::rtl::OUString() : sObjectName;
        m_sCompatibleObjectDescription  += sSeparator;
        m_sCompatibleObjectDescription  += sObjectKind;
        m_sCompatibleObjectDescription  += sSeparator;
        m_sCompatibleObjectDescription  +=
                (CommandType::QUERY == nObjectType) && !bHasFilterOrSort
                ? ::rtl::OUString()
            : sCompleteStatement;
            // compatibility says : always add the statement, but don't if it is a "pure" query
        m_sCompatibleObjectDescription  += sSeparator;
    }

    // -----------------------------------------------------------------------------
    void ODataAccessObjectTransferable::AddSupportedFormats()
    {
        sal_Int32 nObjectType = CommandType::COMMAND;
        m_aDescriptor[daCommandType] >>= nObjectType;
        switch (nObjectType)
        {
            case CommandType::TABLE:
                AddFormat(SOT_FORMATSTR_ID_DBACCESS_TABLE);
                break;
            case CommandType::QUERY:
                AddFormat(SOT_FORMATSTR_ID_DBACCESS_QUERY);
                break;
            case CommandType::COMMAND:
                AddFormat(SOT_FORMATSTR_ID_DBACCESS_COMMAND);
                break;
        }

        sal_Int32 nDescriptorLen = m_sCompatibleObjectDescription.getLength();
        if (nDescriptorLen)
        {
            if (m_sCompatibleObjectDescription.getStr()[nDescriptorLen] == 11)
                m_sCompatibleObjectDescription = m_sCompatibleObjectDescription.copy(0, nDescriptorLen - 1);

            if (nDescriptorLen)
                AddFormat(SOT_FORMATSTR_ID_SBA_DATAEXCHANGE);
        }
    }

    // -----------------------------------------------------------------------------
    sal_Bool ODataAccessObjectTransferable::GetData( const DataFlavor& rFlavor )
    {
        ULONG nFormat = SotExchange::GetFormat(rFlavor);
        switch (nFormat)
        {
            case SOT_FORMATSTR_ID_DBACCESS_TABLE:
            case SOT_FORMATSTR_ID_DBACCESS_QUERY:
            case SOT_FORMATSTR_ID_DBACCESS_COMMAND:
                return SetAny( makeAny(m_aDescriptor.createPropertyValueSequence()), rFlavor );

            case SOT_FORMATSTR_ID_SBA_DATAEXCHANGE:
                return SetString(m_sCompatibleObjectDescription, rFlavor);
        }
        return sal_False;
    }

    // -----------------------------------------------------------------------------
    sal_Bool ODataAccessObjectTransferable::canExtractObjectDescriptor(const DataFlavorExVector& _rFlavors)
    {
        for (   DataFlavorExVector::const_iterator aCheck = _rFlavors.begin();
                aCheck != _rFlavors.end();
                ++aCheck
            )
        {
            if (SOT_FORMATSTR_ID_DBACCESS_TABLE == aCheck->mnSotId)
                return sal_True;
            if (SOT_FORMATSTR_ID_DBACCESS_QUERY == aCheck->mnSotId)
                return sal_True;
            if (SOT_FORMATSTR_ID_DBACCESS_COMMAND == aCheck->mnSotId)
                return sal_True;
        }
        return sal_False;
    }

    // -----------------------------------------------------------------------------
    ODataAccessDescriptor ODataAccessObjectTransferable::extractObjectDescriptor(const TransferableDataHelper& _rData)
    {
        sal_Int32 nKnownFormatId = 0;
        if ( _rData.HasFormat( SOT_FORMATSTR_ID_DBACCESS_TABLE ) )
            nKnownFormatId = SOT_FORMATSTR_ID_DBACCESS_TABLE;
        if ( _rData.HasFormat( SOT_FORMATSTR_ID_DBACCESS_QUERY ) )
            nKnownFormatId = SOT_FORMATSTR_ID_DBACCESS_QUERY;
        if ( _rData.HasFormat( SOT_FORMATSTR_ID_DBACCESS_COMMAND ) )
            nKnownFormatId = SOT_FORMATSTR_ID_DBACCESS_COMMAND;

        if (0 != nKnownFormatId)
        {
            // extract the any from the transferable
            DataFlavor aFlavor;
#ifdef _DEBUG
            sal_Bool bSuccess =
#endif
            SotExchange::GetFormatDataFlavor(nKnownFormatId, aFlavor);
            OSL_ENSURE(bSuccess, "OColumnTransferable::extractColumnDescriptor: invalid data format (no flavor)!");

            Any aDescriptor = _rData.GetAny(aFlavor);

            // extract the property value sequence
            Sequence< PropertyValue > aDescriptorProps;
#ifdef _DEBUG
            bSuccess =
#endif
            aDescriptor >>= aDescriptorProps;
            OSL_ENSURE(bSuccess, "OColumnTransferable::extractColumnDescriptor: invalid clipboard format!");

            // build the real descriptor
            return ODataAccessDescriptor(aDescriptorProps);
        }

        OSL_ENSURE( sal_False, "OColumnTransferable::extractColumnDescriptor: unsupported formats only!" );
        return ODataAccessDescriptor();
    }

    // -----------------------------------------------------------------------------
    void ODataAccessObjectTransferable::addCompatibleSelectionDescription( const Sequence< Any >& _rSelRows )
    {
        const sal_Unicode       cSeparator(11);
        const ::rtl::OUString   sSeparator(&cSeparator, 1);

        const Any* pSelRows = _rSelRows.getConstArray();
        const Any* pSelRowsEnd = pSelRows + _rSelRows.getLength();
        for (pSelRows; pSelRows<pSelRowsEnd; ++pSelRows)
        {
            sal_Int32 nSelectedRow;
#ifdef _DEBUG
            sal_Bool bSuccess =
#endif
            *pSelRows >>= nSelectedRow;
            OSL_ENSURE( bSuccess, "ODataAccessObjectTransferable::addCompatibleSelectionDescription: invalid row number!" );

            m_sCompatibleObjectDescription += ::rtl::OUString::valueOf((sal_Int32)nSelectedRow);
            m_sCompatibleObjectDescription += sSeparator;
        }
    }

    // -----------------------------------------------------------------------------
    void ODataAccessObjectTransferable::ObjectReleased()
    {
        m_aDescriptor.clear();
    }


//........................................................................
}   // namespace svx
//........................................................................

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2001/08/08 09:25:12  fs
 *  #88849# OColumnTransferable: don't fake the to-be-dragged statement if the data source is no CommandType::COMMAND
 *
 *  Revision 1.7  2001/08/07 08:57:14  fs
 *  #90660# +addDataToContainer
 *
 *  Revision 1.6  2001/08/02 15:23:27  fs
 *  #90525# +ODataAccessObjectTransferable
 *
 *  Revision 1.5  2001/04/27 09:22:38  fs
 *  #86303# use another content type for the own clipboard format
 *
 *  Revision 1.4  2001/04/18 10:42:09  fs
 *  added connection to the ctor
 *
 *  Revision 1.3  2001/04/11 12:40:59  fs
 *  added a data access descriptor format
 *
 *  Revision 1.2  2001/03/27 14:38:28  fs
 *  swap the order in which the clipboard formats are added
 *
 *  Revision 1.1  2001/03/26 15:05:04  fs
 *  initial checkin - helper classes for data access related DnD
 *
 *
 *  Revision 1.0 23.03.01 12:59:54  fs
 ************************************************************************/

