/*************************************************************************
 *
 *  $RCSfile: MDatabaseMetaDataHelper.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2004-03-17 10:42:00 $
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
 *  Contributor(s): Willem van Dorp, Darren Kenny
 *
 *
 ************************************************************************/

// Mozilla includes.
#include <MNSInclude.hxx>

#ifndef _CONNECTIVITY_MAB_DATABASEMETADATAHELPER_HXX_
#include "MDatabaseMetaDataHelper.hxx"
#endif
#ifndef _CONNECTIVITY_MAB_CONVERSIONS_HXX_
#include "MTypeConverter.hxx"
#endif
#ifndef CONNECTIVITY_MOZAB_MCONFIGACCESS_HXX
#include "MConfigAccess.hxx"
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif

#include <MNSInit.hxx>
#include <MNameMapper.hxx>

// More Mozilla includes for LDAP Connection Test
#include "prprf.h"
#include "nsILDAPURL.h"
#include "nsILDAPMessage.h"
#include "nsILDAPMessageListener.h"
#include "nsILDAPErrors.h"
#include "nsILDAPConnection.h"
#include "nsILDAPOperation.h"


#if OSL_DEBUG_LEVEL > 1
# define OUtoCStr( x ) ( ::rtl::OUStringToOString ( (x), RTL_TEXTENCODING_ASCII_US).getStr())
#else /* OSL_DEBUG_LEVEL */
# define OUtoCStr( x ) ("dummy")
#endif /* OSL_DEBUG_LEVEL */


static NS_DEFINE_CID(kRDFServiceCID, NS_RDFSERVICE_CID);
static NS_DEFINE_CID(kAbDirFactoryServiceCID, NS_ABDIRFACTORYSERVICE_CID);
static NS_DEFINE_CID(kAddrBookSessionCID, NS_ADDRBOOKSESSION_CID);

using namespace connectivity::mozab;
using namespace connectivity;
using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbc;

namespace connectivity
{
    namespace mozab
    {
        static const char * const s_pADDRESSBOOKROOTDIR = "moz-abdirectory://";
    }
}

// -------------------------------------------------------------------------
MDatabaseMetaDataHelper::MDatabaseMetaDataHelper()
    : m_bProfileExists(sal_False)
{
    OSL_TRACE( "IN MDatabaseMetaDataHelper::MDatabaseMetaDataHelper()\n" );
    sal_Bool rv = MNS_Init(m_bProfileExists);

    OSL_TRACE( "\tMAB_NS_Init returned %s\n", rv?"True":"False" );
    OSL_TRACE( "\tOUT MDatabaseMetaDataHelper::MDatabaseMetaDataHelper()\n" );

}
// -------------------------------------------------------------------------
MDatabaseMetaDataHelper::~MDatabaseMetaDataHelper()
{
    m_aTableNames.clear();
    m_aTableTypes.clear();

    sal_Bool rv = MNS_Term();

    OSL_TRACE( "IN/OUT MDatabaseMetaDataHelper::~MDatabaseMetaDataHelper()\n" );
}

//enum all sub folders
static nsresult enumSubs(nsIAbDirectory * parentDir,nsISupportsArray * array)
{
    nsresult rv = NS_OK ;
    nsCOMPtr<nsIEnumerator> subDirectories;
    rv = parentDir->GetChildNodes(getter_AddRefs(subDirectories));
    if (NS_FAILED(rv)) { return rv; }

    while (NS_ENUMERATOR_FALSE == subDirectories->IsDone()) {
        nsCOMPtr<nsISupports> newDirSupports ;

        rv = subDirectories->CurrentItem(getter_AddRefs(newDirSupports)) ;
                NS_ENSURE_SUCCESS(rv, rv);
                subDirectories->Next();
        nsCOMPtr<nsIAbDirectory> childDir = do_QueryInterface(newDirSupports, &rv) ;
                NS_ENSURE_SUCCESS(rv, rv);
        array->AppendElement(childDir);

        rv = enumSubs(childDir,array);
    }
    return rv;
}

#include <prmem.h> //need for PR_FREEIF
static nsresult insertPABDescription()
{
    nsresult rv = NS_OK ;
    // mozilla loads the descriptions of pab and history from a file named addressbook.properties
    // we never deliver it with OOo,so mozab does not auot load personal addressbook and Collected addresses
    // If we want to deliver addressbook.properties with OOo,we have to deal with hole mozilla chrome mechanism.
    // So I insert the descriptiones as user prefes,they will cover the default value.

    //"Personal Address Book" "Collected Addresses" are loaded from officecfg
    nsCOMPtr<nsIPref> prefs = do_GetService(NS_PREF_CONTRACTID, &rv);
    NS_ENSURE_SUCCESS(rv, rv);

    nsCAutoString pabPrefName(NS_LITERAL_CSTRING("ldap_2.servers.pab.description"));

    sal_Bool bSetted=sal_False;
    char *value = nsnull;
    if (0 == prefs->CopyCharPref(pabPrefName.get(), &value))
    {
        if (!(value == nsnull || value[0] == 0))
        {
            bSetted = sal_True;
        }
    }
    PR_FREEIF (value);
    value=nsnull;
    if (bSetted == sal_True)
    {
        const char* pPabDescription = static_cast< const char* >( getPabDescription( ) );
        rv = prefs->SetCharPref (pabPrefName.get(), pPabDescription);
        NS_ENSURE_SUCCESS(rv, rv);
    }

    nsCAutoString hisPrefName(NS_LITERAL_CSTRING("ldap_2.servers.history.description"));

    bSetted=sal_False;
    if (0 == prefs->CopyCharPref(hisPrefName.get(), &value))
    {
        if (!(value == nsnull || value[0] == 0))
        {
            bSetted = sal_True;
        }
    }
    PR_FREEIF (value);
    value=nsnull;
    if (bSetted == sal_True)
    {
        const char* pHisDescription = static_cast< const char* >( getHisDescription( ) );

        rv = prefs->SetCharPref (hisPrefName.get(), pHisDescription);
        NS_ENSURE_SUCCESS(rv, rv);
    }
    return rv;
}
// -------------------------------------------------------------------------
// Case where we get a parent uri, and need to list its children.
static nsresult getSubsFromParent(const rtl::OString& aParent, nsIEnumerator **aSubs, sal_Bool aProfileExists)
{
    if (aSubs == nsnull) { return NS_ERROR_NULL_POINTER ; }
    *aSubs = nsnull ;
    nsresult retCode = NS_OK ;
    nsCOMPtr<nsISupportsArray> array ;

    NS_NewISupportsArray(getter_AddRefs(array)) ;
    if (aProfileExists) {

        retCode = insertPABDescription();
        NS_ENSURE_SUCCESS(retCode, retCode) ;

        nsCOMPtr<nsIRDFService> rdfService(do_GetService(kRDFServiceCID, &retCode)) ;
        NS_ENSURE_SUCCESS(retCode, retCode) ;
        nsCOMPtr<nsIRDFResource> rdfResource ;

        OSL_TRACE("uri: %s\n", aParent.getStr()) ;
        retCode = rdfService->GetResource(aParent.getStr(), getter_AddRefs(rdfResource)) ;
        NS_ENSURE_SUCCESS(retCode, retCode) ;
        nsCOMPtr<nsIAbDirectory> directory = do_QueryInterface(rdfResource, &retCode) ;
        nsCOMPtr<nsIEnumerator> tempEnum ;

        NS_ENSURE_SUCCESS(retCode, retCode) ;
        retCode = directory->GetChildNodes(getter_AddRefs(tempEnum)) ;
        NS_ENSURE_SUCCESS(retCode, retCode) ;

        if (NS_SUCCEEDED(tempEnum->First())) {
            do {
                nsCOMPtr<nsIAbDirectory> element ;
                retCode = tempEnum->CurrentItem(getter_AddRefs(element)) ;
                if (NS_SUCCEEDED(retCode))
                   {
                       array->AppendElement(element) ;
                       enumSubs(element,array);
                   }
            } while (NS_SUCCEEDED(tempEnum->Next())) ;
        }

    }
    array->Enumerate(aSubs) ;
    return retCode ;
}

static nsresult enumSubs(nsISimpleEnumerator * subDirs,nsISupportsArray * array)
{
    PRBool hasMore = PR_TRUE ;
    nsresult rv = NS_OK ;
    while (NS_SUCCEEDED(subDirs->HasMoreElements(&hasMore)) && hasMore) {
        nsCOMPtr<nsISupports> newDirSupports ;

        rv = subDirs->GetNext(getter_AddRefs(newDirSupports)) ;
                NS_ENSURE_SUCCESS(rv, rv);
        nsCOMPtr<nsIAbDirectory> childDir = do_QueryInterface(newDirSupports, &rv) ;
                NS_ENSURE_SUCCESS(rv, rv);
        array->AppendElement(childDir);

        rv = enumSubs(childDir,array);
    }
    return rv;
}

// Case where we get a factory uri and need to have it build the directories.
static const char *kPropertyName = "uri" ;

static nsresult getSubsFromFactory(const rtl::OString& aFactory, nsIEnumerator **aSubs)
{
    if (aSubs == nsnull) { return NS_ERROR_NULL_POINTER ; }
    *aSubs = nsnull ;
    nsresult retCode = NS_OK ;

    nsCOMPtr<nsIAbDirFactoryService> dirFactoryService = do_GetService(NS_ABDIRFACTORYSERVICE_CONTRACTID, &retCode);
    NS_ENSURE_SUCCESS(retCode, retCode) ;
    nsCOMPtr<nsIAbDirFactory> dirFactory ;

    retCode = dirFactoryService->GetDirFactory(aFactory.getStr(), getter_AddRefs(dirFactory)) ;
    NS_ENSURE_SUCCESS (retCode, retCode) ;
    nsCOMPtr <nsIAbDirectoryProperties> properties;
    properties = do_CreateInstance(NS_ABDIRECTORYPROPERTIES_CONTRACTID, &retCode);
    NS_ENSURE_SUCCESS(retCode, retCode) ;

    retCode = properties->SetURI(aFactory.getStr());
    NS_ENSURE_SUCCESS(retCode,retCode);

    nsCOMPtr<nsISimpleEnumerator> subDirs ;
    retCode = dirFactory->CreateDirectory(properties, getter_AddRefs(subDirs));
    NS_ENSURE_SUCCESS(retCode, retCode) ;
    nsCOMPtr<nsISupportsArray> array ;

    NS_NewISupportsArray(getter_AddRefs(array)) ;
    retCode = enumSubs(subDirs,array);
    NS_ENSURE_SUCCESS(retCode, retCode) ;
    array->Enumerate(aSubs) ;
    return retCode ;
}

// Case where the uri itself is the directory we're looking for.
static nsresult getSubsFromURI(const rtl::OString& aUri, nsIEnumerator **aSubs)
{
    if (aSubs == nsnull) { return NS_ERROR_NULL_POINTER ; }
    *aSubs = nsnull ;
    nsresult retCode = NS_OK ;

    nsCOMPtr<nsIRDFService> rdfService(do_GetService(kRDFServiceCID, &retCode)) ;
    NS_ENSURE_SUCCESS(retCode, retCode) ;
    nsCOMPtr<nsIRDFResource> rdfResource ;

    retCode = rdfService->GetResource(aUri.getStr(), getter_AddRefs(rdfResource)) ;
    NS_ENSURE_SUCCESS(retCode, retCode) ;
    nsCOMPtr<nsIAbDirectory> directory = do_QueryInterface(rdfResource, &retCode) ;

    NS_ENSURE_SUCCESS(retCode, retCode) ;
    nsAutoString nameAB ;

    nameAB.AssignWithConversion("LDAP Directory") ;
    directory->SetDirName(nameAB.get()) ;
    nsCOMPtr<nsISupportsArray> array ;

    NS_NewISupportsArray(getter_AddRefs(array)) ;
    array->AppendElement(directory) ;
    array->Enumerate(aSubs) ;
    return retCode ;
}

void MDatabaseMetaDataHelper::setAbSpecificError( OConnection* _pCon, sal_Bool bGivenURI )
{
    if ( ! bGivenURI ) {
        m_aErrorString = ::rtl::OUString::createFromAscii("No Mozilla Addressbook Directories Exist");
    }
    else {
        if (_pCon->usesFactory()) {
            if ( _pCon->isOutlookExpress() ) {
                m_aErrorString = ::rtl::OUString::createFromAscii("No Outlook Express Addressbook Exists");
            }
            else {
                m_aErrorString = ::rtl::OUString::createFromAscii("No Outlook (MAPI) Addressbook Exists");
            }
        }
        else {
            if (_pCon->isLDAP()) {
                m_aErrorString = ::rtl::OUString::createFromAscii("Unable to connect to LDAP Server");
            }
            else {
                m_aErrorString = ::rtl::OUString::createFromAscii("No Mozilla Addressbook Directories Exist");
            }
        }
    }
}

sal_Bool MDatabaseMetaDataHelper::getTableStrings( OConnection*                        _pCon,
                                                   ::std::vector< ::rtl::OUString >&   _rStrings,
                                                   ::std::vector< ::rtl::OUString >&   _rTypes,
                                                   sal_Bool                            forceLoad )
{
    sal_Bool                                    bGivenURI;
    sal_Bool                                    bIsRootDir;
    rtl::OUString                               sAbURI;
    OString                                     sAbURIString;

    OSL_TRACE( "IN MDatabaseMetaDataHelper::getTableStrings( 0x%08X, %s)\n", _pCon, forceLoad?"True":"False" );

    // Only do the query if we have to - performance degrades otherwise
    if ( ! forceLoad && m_aTableNames.size() > 0 ) {
        _rStrings = m_aTableNames;
        _rTypes   = m_aTableTypes;
        return( sal_True );
    }

    m_aTableNames.clear();
    m_aTableTypes.clear();

    sAbURI = _pCon->getMozURI();

    OSL_TRACE("AbURI = %s\n", OUtoCStr( sAbURI ) );
    if ( sAbURI.getLength() == 0 )
        bGivenURI = sal_False;
    else {
        sAbURIString = ::rtl::OUStringToOString( sAbURI,
                                                 RTL_TEXTENCODING_ASCII_US);
        bGivenURI = sal_True;
    }

    if ( bGivenURI )
        OSL_TRACE("Using URI %s to getTables()\n", sAbURIString.getStr() );
    else
        OSL_TRACE("Using URI of ROOTDIR to getTables()\n");

    // Get the mozilla addressbooks from the base directory.
    nsresult rv = NS_OK;
    nsCOMPtr<nsIEnumerator> subDirectories;

    if (!bGivenURI) {
        rv = getSubsFromParent(s_pADDRESSBOOKROOTDIR, getter_AddRefs(subDirectories), m_bProfileExists) ;
        if ( NS_FAILED( rv ) ) {
            setAbSpecificError( _pCon, bGivenURI );
            return( sal_False );
        }
    }
    else {
        if (_pCon->usesFactory()) {
            rv = getSubsFromFactory(sAbURIString, getter_AddRefs(subDirectories)) ;
            if ( NS_FAILED( rv ) ) {
                setAbSpecificError( _pCon, bGivenURI );
                return sal_False;
            }
        }
        else {
            if (_pCon->isLDAP()) {
                rv = getSubsFromURI(sAbURIString, getter_AddRefs(subDirectories)) ;
                if ( NS_FAILED( rv ) ) {
                    setAbSpecificError( _pCon, bGivenURI );
                    return sal_False;
                }
            }
            else {
                rv = getSubsFromParent(sAbURIString, getter_AddRefs(subDirectories), m_bProfileExists);
                if ( NS_FAILED( rv ) ) {
                    setAbSpecificError( _pCon, bGivenURI );
                    return sal_False;
                }
            }
        }
    }
    // Catch all (just in case), should be caught before this
    if ( NS_FAILED( rv ) ) {
        OSL_ASSERT( "Shouldn't have reached this line with a failure");
        setAbSpecificError( _pCon, bGivenURI );
        return sal_False;
    }
    // At this point we have a list containing the nsIAbDirectory we need to map as tables
    rv = subDirectories -> First();
    if ( NS_FAILED( rv ) ) {
        setAbSpecificError( _pCon, bGivenURI );
        return sal_False;
    }
    else {
        bIsRootDir = sal_True;
    }

    PRUnichar              *name = nsnull;
    PRBool                  bIsMailList = PR_FALSE;

    ::rtl::OUString aTableName;

    MNameMapper *nmap = _pCon->getNameMapper();
    nmap->reset();
    do {
        nsCOMPtr<nsIAbDirectory> subDirectory;

        if ( bIsRootDir ) {
            nsCOMPtr<nsISupports> item;
            rv = subDirectories -> CurrentItem(getter_AddRefs(item));
            if ( NS_FAILED( rv ) ) {
                m_aErrorString = ::rtl::OUString::createFromAscii("Problem Getting Addressbook Entry");
                return sal_False;
            }

            subDirectory = do_QueryInterface(item, &rv);
        }
        subDirectory -> GetIsMailList(&bIsMailList);
        // For now we're not interested in mailing lists.
        rv = subDirectory -> GetDirName(&name);
        if ( NS_FAILED( rv ) ) {
            m_aErrorString = ::rtl::OUString::createFromAscii("Problem Getting Addressbook Directory Name");
            return sal_False;
        }
        MTypeConverter::prUnicharToOUString(name, aTableName);
        OSL_TRACE("TableName = >%s<\n", OUtoCStr( aTableName ) );

            ODatabaseMetaDataResultSet::ORow aRow(3);

            // Insert table into map
            if ( aTableName.getLength() == 0 ) {
                aTableName = rtl::OUString::createFromAscii("AddressBook");
            }

            OSL_TRACE("TableName = >%s<\n", OUtoCStr( aTableName ) );

        rv = nmap->add( aTableName, subDirectory);
        if (!NS_FAILED(rv)) //failed means we have added this directory
        {
            //map mailing lists as views
            m_aTableNames.push_back( aTableName ); // Table name
            if (!bIsMailList) {
                ::rtl::OUString aTableType(::rtl::OUString::createFromAscii("TABLE"));
                m_aTableTypes.push_back( aTableType ); // Table type
            }
            else
            {
                ::rtl::OUString aTableType(::rtl::OUString::createFromAscii("VIEW"));
                m_aTableTypes.push_back( aTableType ); // Table type
            }
        }
        if ( bIsRootDir ) {
            rv = subDirectories -> Next();
        }
    } while ( bIsRootDir && rv == NS_OK);

    OSL_TRACE( "\tOUT MDatabaseMetaDataHelper::getTableStrings()\n" );
    _rStrings = m_aTableNames;
    _rTypes   = m_aTableTypes;

    return( sal_True );
}

sal_Bool MDatabaseMetaDataHelper::getTables( OConnection* _pCon,
                                             const ::rtl::OUString& tableNamePattern,
                                             const Sequence< ::rtl::OUString >& types,
                                             ODatabaseMetaDataResultSet::ORows& _rRows)
{
    static ODatabaseMetaDataResultSet::ORows    aRows;

    OSL_TRACE( "IN MDatabaseMetaDataHelper::getTables()\n" );

    ODatabaseMetaDataResultSet::ORows().swap(aRows); // this makes real clear where memory is freed as well
    aRows.clear();

    ::std::vector< ::rtl::OUString > tables;
    ::std::vector< ::rtl::OUString > tabletypes;
    ::rtl::OUString matchAny = rtl::OUString::createFromAscii("%");;

    if ( !getTableStrings( _pCon, tables,tabletypes ) )
        return sal_False;

    const ::rtl::OUString* pTArray = types.getConstArray();

    for ( sal_Int32 i = 0; i < tables.size(); i++ ) {
        ODatabaseMetaDataResultSet::ORow aRow(3);

        ::rtl::OUString aTableName  = tables[i];
        ::rtl::OUString aTableType      = tabletypes[i];
        OSL_TRACE("TableName = >%s<\n", OUtoCStr( aTableName ) );


        // return tables to caller
        if (match( tableNamePattern, aTableName, '\0' ) &&
                      ( 0 == types.getLength() ||   //types is null or types include aTableType or types include "%"
                       0 != ::comphelper::findValue( types, aTableType, sal_True ).getLength() ||
                       0 != ::comphelper::findValue( types, matchAny, sal_True ).getLength())) {
            if ( aTableName.getLength() == 0 ) {
                aTableName = rtl::OUString::createFromAscii("AddressBook");
            }

            OSL_TRACE( "TableName = %s ; TableType = %s\n", OUtoCStr(aTableName), OUtoCStr(aTableType) );

            aRow.push_back( new ORowSetValueDecorator( aTableName ) ); // Table name
            aRow.push_back( new ORowSetValueDecorator( aTableType ) );     // Table type
            aRow.push_back( ODatabaseMetaDataResultSet::getEmptyValue() );                 // Remarks
            aRows.push_back(aRow);
        }
    }

    OSL_TRACE( "\tOUT MDatabaseMetaDataHelper::getTables()\n" );
    _rRows = aRows;
    return(sal_True);
}

//-------------------------------------------------------------------

#define NS_LDAPCONNECTION_CONTRACTID     "@mozilla.org/network/ldap-connection;1"
#define NS_LDAPOPERATION_CONTRACTID      "@mozilla.org/network/ldap-operation;1"
#define NS_LDAPMESSAGE_CONTRACTID      "@mozilla.org/network/ldap-message;1"
#define NS_LDAPURL_CONTRACTID       "@mozilla.org/network/ldap-url;1"

namespace connectivity {
    namespace mozab {
        class MLDAPMessageListener : public nsILDAPMessageListener {
            NS_DECL_ISUPPORTS
            NS_DECL_NSILDAPMESSAGELISTENER

            MLDAPMessageListener(nsILDAPConnection* _ldapConnection);
            ~MLDAPMessageListener();

            sal_Bool    connected();
            void        setPassword( const ::rtl::OUString &aPassword);
        protected:
            nsCOMPtr<nsILDAPConnection> m_LDAPConnection;

            ::osl::Mutex        m_aMutex;
            ::osl::Condition    m_aCondition;

            sal_Bool    m_IsComplete;
            sal_Bool    m_GoodConnection;
            rtl::OUString   m_sPassword;

            void        setConnectionStatus( sal_Bool _good );
        };
    }
}

NS_IMPL_THREADSAFE_ISUPPORTS1(MLDAPMessageListener, nsILDAPMessageListener)

MLDAPMessageListener::MLDAPMessageListener(nsILDAPConnection* _ldapConnection)
    : mRefCnt( 0 )
    , m_IsComplete( sal_False )
    , m_GoodConnection( sal_False )
    , m_LDAPConnection( _ldapConnection )
{
    m_aCondition.reset();
}

MLDAPMessageListener::~MLDAPMessageListener()
{
}

sal_Bool MLDAPMessageListener::connected()
{
    TimeValue               timeValue = { 10, 0 };  // 10 Seconds 0 NanoSecond timeout
    osl::Condition::Result  rv = ::osl::Condition::result_ok;

    // Can't hold mutex or condition would never get set...
    while( m_aCondition.check() == sal_False || rv  == ::osl::Condition::result_error ) {
        rv = m_aCondition.wait( &timeValue );
        if ( rv == ::osl::Condition::result_timeout ) {
            return sal_False;
        }
    }

    return m_GoodConnection;
}

void MLDAPMessageListener::setPassword( const ::rtl::OUString &aPassword)
{
   //::osl::MutexGuard aGuard( m_aMutex );
   m_sPassword = aPassword;
}
void MLDAPMessageListener::setConnectionStatus( sal_Bool _good )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    m_IsComplete = sal_True;
    m_GoodConnection = _good;

    m_aCondition.set();
}

NS_IMETHODIMP MLDAPMessageListener::OnLDAPInit( nsresult aStatus )
{
    nsresult rv;

    // Make sure that the Init() worked properly
    if ( NS_FAILED(aStatus ) ) {
        setConnectionStatus( sal_False );
        return aStatus;
    }

    // Initiate the LDAP operation
    nsCOMPtr<nsILDAPOperation> ldapOperation =
        do_CreateInstance(NS_LDAPOPERATION_CONTRACTID, &rv);
    if ( NS_FAILED( rv ) ) {
        setConnectionStatus( sal_False );
        return rv;
    }

    rv = ldapOperation->Init(m_LDAPConnection, this);
    if ( NS_FAILED( rv ) ) {
        setConnectionStatus( sal_False );
        return rv;
    }

    const sal_Unicode* password=nsnull;
    if (m_sPassword.getLength() != 0)
    {
        password = m_sPassword.getStr();
    }

    // Bind
    rv = ldapOperation->SimpleBind(password);
    if ( NS_FAILED( rv ) ) {
        setConnectionStatus( sal_False );
        return rv;
    }
    // rv = ldapOperation->Abandon();
    // NS_ENSURE_SUCCESS(rv, rv);

    return rv;
}

NS_IMETHODIMP MLDAPMessageListener::OnLDAPMessage( nsILDAPMessage* aMessage )
{
    nsresult rv;

    PRInt32 messageType;
    rv = aMessage->GetType(&messageType);
    NS_ENSURE_SUCCESS(rv, rv);
    PRInt32 errCode;
    switch (messageType)
    {
    case nsILDAPMessage::RES_BIND:
        rv = aMessage->GetErrorCode(&errCode);
        // if the login failed
        if (errCode != nsILDAPErrors::SUCCESS) {
            setConnectionStatus( sal_False );
        }
        else
            setConnectionStatus( sal_True );
        break;
    case nsILDAPMessage::RES_SEARCH_RESULT:
        setConnectionStatus( sal_True );
        break;
    default:
        break;
    }

    return NS_OK;
}

//-------------------------------------------------------------------

sal_Bool
MDatabaseMetaDataHelper::testLDAPConnection( OConnection* _pCon )
{
    const sal_Unicode QUERY_CHAR = '?';
    const sal_Char*   MOZ_SCHEMA = "moz-abldapdirectory://";
    const sal_Char*   LDAP_SCHEMA = "ldap://";

    rtl::OString   sAbURI;
    rtl::OUString   sAbBindDN;
    rtl::OUString   sAbPassword;
    sal_Bool      useSSL    = _pCon->getUseSSL();

    nsresult       rv;

    sAbURI = OUStringToOString( _pCon->getMozURI(), RTL_TEXTENCODING_ASCII_US );
    sAbBindDN   = _pCon->getBindDN();
    sAbPassword = _pCon->getPassword();


    sal_Int32 pos = sAbURI.indexOf( MOZ_SCHEMA );
    if ( pos != -1 ) {
        sAbURI = sAbURI.replaceAt (pos, strlen( MOZ_SCHEMA ), OString(LDAP_SCHEMA) );
    }

    pos = sAbURI.indexOf( QUERY_CHAR );
    if ( pos != -1 ) {
        sal_Int32 len =  sAbURI.getLength();
        sAbURI = sAbURI.replaceAt( pos, len - pos, OString("") );
    }

    nsCOMPtr<nsILDAPURL> url;
    url = do_CreateInstance(NS_LDAPURL_CONTRACTID, &rv);
    if ( NS_FAILED(rv) )
        return sal_False;

    rv = url->SetSpec( nsDependentCString(sAbURI.getStr()) );
    if ( NS_FAILED(rv) )
        return sal_False;

    nsCAutoString host;
    rv = url->GetAsciiHost(host);
    if ( NS_FAILED(rv) )
        return sal_False;

    PRInt32 port;
    rv = url->GetPort(&port);
    if ( NS_FAILED(rv) )
        return sal_False;

    nsXPIDLCString dn;
    rv = url->GetDn(getter_Copies (dn));
    if ( NS_FAILED(rv) )
        return sal_False;



    // Get the ldap connection
    nsCOMPtr<nsILDAPConnection> ldapConnection;
    ldapConnection = do_CreateInstance(NS_LDAPCONNECTION_CONTRACTID, &rv);
    if ( NS_FAILED(rv) )
        return sal_False;

    // Initiate LDAP message listener
    nsCOMPtr<nsILDAPMessageListener> messageListener;
    MLDAPMessageListener* _messageListener =
        new MLDAPMessageListener ( ldapConnection );
    if (_messageListener == NULL)
            return sal_False;

    _messageListener->setPassword(sAbPassword);
    messageListener = _messageListener;

    const sal_Unicode* bindDN=nsnull;
    if (sAbBindDN.getLength() != 0)
    {
        bindDN = sAbBindDN.getStr();
    }
    // Now lets initialize the LDAP connection properly.
    rv = ldapConnection->Init(host.get(), port, useSSL, bindDN,
                              messageListener);
    if ( NS_FAILED(rv) )
        return sal_False;

    if ( ! _messageListener->connected() ) {
        setAbSpecificError( _pCon, sal_True );
        return sal_False;
    }

    return sal_True;
}
