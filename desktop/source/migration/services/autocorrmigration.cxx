/*************************************************************************
 *
 *  $RCSfile: autocorrmigration.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-05-13 08:09:43 $
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

#ifndef _DESKTOP_AUTOCORRMIGRATION_HXX_
#include "autocorrmigration.hxx"
#endif

#ifndef _ISOLANG_HXX
#include <tools/isolang.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _UTL_BOOTSTRAP_HXX
#include <unotools/bootstrap.hxx>
#endif


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


//.........................................................................
namespace migration
{
//.........................................................................


    static ::rtl::OUString sSourceSubDir = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/user/autocorr" ) );
    static ::rtl::OUString sTargetSubDir = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/user/autocorr" ) );
    static ::rtl::OUString sBaseName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/acor" ) );
    static ::rtl::OUString sSuffix = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".dat" ) );


    // =============================================================================
    // component operations
    // =============================================================================

    ::rtl::OUString AutocorrectionMigration_getImplementationName()
    {
        static ::rtl::OUString* pImplName = 0;
        if ( !pImplName )
        {
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if ( !pImplName )
            {
                static ::rtl::OUString aImplName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.desktop.migration.Autocorrection" ) );
                pImplName = &aImplName;
            }
        }
        return *pImplName;
    }

    // -----------------------------------------------------------------------------

    Sequence< ::rtl::OUString > AutocorrectionMigration_getSupportedServiceNames()
    {
        static Sequence< ::rtl::OUString >* pNames = 0;
        if ( !pNames )
        {
            ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
            if ( !pNames )
            {
                static Sequence< ::rtl::OUString > aNames(1);
                aNames.getArray()[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.migration.Autocorrection" ) );
                pNames = &aNames;
            }
        }
        return *pNames;
    }

    // =============================================================================
    // AutocorrectionMigration
    // =============================================================================

    AutocorrectionMigration::AutocorrectionMigration()
    {
    }

    // -----------------------------------------------------------------------------

    AutocorrectionMigration::~AutocorrectionMigration()
    {
    }

    // -----------------------------------------------------------------------------

    TStringVectorPtr AutocorrectionMigration::getFiles( const ::rtl::OUString& rBaseURL ) const
    {
        TStringVectorPtr aResult( new TStringVector );
        ::osl::Directory aDir( rBaseURL);

        if ( aDir.open() == ::osl::FileBase::E_None )
        {
            // iterate over directory content
            TStringVector aSubDirs;
            ::osl::DirectoryItem aItem;
            while ( aDir.getNextItem( aItem ) == ::osl::FileBase::E_None )
            {
                ::osl::FileStatus aFileStatus( FileStatusMask_Type | FileStatusMask_FileURL );
                if ( aItem.getFileStatus( aFileStatus ) == ::osl::FileBase::E_None )
                {
                    if ( aFileStatus.getFileType() == ::osl::FileStatus::Directory )
                        aSubDirs.push_back( aFileStatus.getFileURL() );
                    else
                        aResult->push_back( aFileStatus.getFileURL() );
                }
            }

            // iterate recursive over subfolders
            TStringVector::const_iterator aI = aSubDirs.begin();
            while ( aI != aSubDirs.end() )
            {
                TStringVectorPtr aSubResult = getFiles( *aI );
                aResult->insert( aResult->end(), aSubResult->begin(), aSubResult->end() );
                ++aI;
            }
        }

        return aResult;
    }

    // -----------------------------------------------------------------------------

    ::osl::FileBase::RC AutocorrectionMigration::checkAndCreateDirectory( INetURLObject& rDirURL )
    {
        ::osl::FileBase::RC aResult = ::osl::Directory::create( rDirURL.GetMainURL( INetURLObject::DECODE_TO_IURI ) );
        if ( aResult == ::osl::FileBase::E_NOENT )
        {
            INetURLObject aBaseURL( rDirURL );
            aBaseURL.removeSegment();
            checkAndCreateDirectory( aBaseURL );
            return ::osl::Directory::create( rDirURL.GetMainURL( INetURLObject::DECODE_TO_IURI ) );
        }
        else
        {
            return aResult;
        }
    }

    // -----------------------------------------------------------------------------

    void AutocorrectionMigration::copyFiles()
    {
        ::rtl::OUString sTargetDir;
        ::utl::Bootstrap::PathStatus aStatus = ::utl::Bootstrap::locateUserInstallation( sTargetDir );
        if ( aStatus == ::utl::Bootstrap::PATH_EXISTS )
        {
            sTargetDir += sTargetSubDir;
            TStringVectorPtr aFileList = getFiles( m_sSourceDir );
            TStringVector::const_iterator aI = aFileList->begin();
            while ( aI != aFileList->end() )
            {
                ::rtl::OUString sSourceLocalName = aI->copy( m_sSourceDir.getLength() );
                sal_Int32 nStart = sBaseName.getLength();
                sal_Int32 nEnd = sSourceLocalName.lastIndexOf ( sSuffix );
                ::rtl::OUString sLanguageType = sSourceLocalName.copy( nStart, nEnd - nStart );
                ::rtl::OUString sIsoName = ConvertLanguageToIsoString( (LanguageType) sLanguageType.toInt32() );
                ::rtl::OUString sTargetLocalName = sBaseName;
                sTargetLocalName += ::rtl::OUString::createFromAscii( "_" );
                sTargetLocalName += sIsoName;
                sTargetLocalName += sSuffix;
                ::rtl::OUString sTargetName = sTargetDir + sTargetLocalName;
                INetURLObject aURL( sTargetName );
                aURL.removeSegment();
                checkAndCreateDirectory( aURL );
                ::osl::FileBase::RC aResult = ::osl::File::copy( *aI, sTargetName );
                if ( aResult != ::osl::FileBase::E_None )
                {
                    ::rtl::OString aMsg( "AutocorrectionMigration::copyFiles: cannot copy " );
                    aMsg += ::rtl::OUStringToOString( *aI, RTL_TEXTENCODING_UTF8 ) + " to "
                         +  ::rtl::OUStringToOString( sTargetName, RTL_TEXTENCODING_UTF8 );
                    OSL_ENSURE( sal_False, aMsg.getStr() );
                }
                ++aI;
            }
        }
        else
        {
            OSL_ENSURE( sal_False, "AutocorrectionMigration::copyFiles: no user installation!" );
        }
    }

    // -----------------------------------------------------------------------------
    // XServiceInfo
    // -----------------------------------------------------------------------------

    ::rtl::OUString AutocorrectionMigration::getImplementationName() throw (RuntimeException)
    {
        return AutocorrectionMigration_getImplementationName();
    }

    // -----------------------------------------------------------------------------

    sal_Bool AutocorrectionMigration::supportsService( const ::rtl::OUString& rServiceName ) throw (RuntimeException)
    {
        Sequence< ::rtl::OUString > aNames( getSupportedServiceNames() );
        const ::rtl::OUString* pNames = aNames.getConstArray();
        const ::rtl::OUString* pEnd = pNames + aNames.getLength();
        for ( ; pNames != pEnd && !pNames->equals( rServiceName ); ++pNames )
            ;

        return pNames != pEnd;
    }

    // -----------------------------------------------------------------------------

    Sequence< ::rtl::OUString > AutocorrectionMigration::getSupportedServiceNames() throw (RuntimeException)
    {
        return AutocorrectionMigration_getSupportedServiceNames();
    }

    // -----------------------------------------------------------------------------
    // XInitialization
    // -----------------------------------------------------------------------------

    void AutocorrectionMigration::initialize( const Sequence< Any >& aArguments ) throw (Exception, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        const Any* pIter = aArguments.getConstArray();
        const Any* pEnd = pIter + aArguments.getLength();
        for ( ; pIter != pEnd ; ++pIter )
        {
            beans::NamedValue aValue;
            *pIter >>= aValue;
            if ( aValue.Name.equalsAscii( "UserData" ) )
            {
                sal_Bool bSuccess = aValue.Value >>= m_sSourceDir;
                OSL_ENSURE( bSuccess == sal_True, "AutocorrectionMigration::initialize: argument UserData has wrong type!" );
                m_sSourceDir += sSourceSubDir;
                break;
            }
        }
    }

    // -----------------------------------------------------------------------------
    // XJob
    // -----------------------------------------------------------------------------

    Any AutocorrectionMigration::execute( const Sequence< beans::NamedValue >& Arguments )
        throw (lang::IllegalArgumentException, Exception, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        copyFiles();

        return Any();
    }

    // =============================================================================
    // component operations
    // =============================================================================

    Reference< XInterface > SAL_CALL AutocorrectionMigration_create(
        Reference< XComponentContext > const & xContext )
        SAL_THROW( () )
    {
        return static_cast< lang::XTypeProvider * >( new AutocorrectionMigration() );
    }

    // -----------------------------------------------------------------------------

//.........................................................................
}   // namespace migration
//.........................................................................
