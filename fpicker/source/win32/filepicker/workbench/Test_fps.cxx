/*************************************************************************
 *
 *  $RCSfile: Test_fps.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: tra $ $Date: 2001-06-28 11:14:43 $
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


//_________________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#ifndef _COM_SUN_STAR_REGISTRY_XSIMPLEREGISTRY_HPP_
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

//_________________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_SERVICEFACTORY_HXX_
#include <cppuhelper/servicefactory.hxx>
#endif

#ifndef _RTL_USTRING_
#include <rtl/ustring>
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILTERMANAGER_HPP_
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_FILEDIALOGRESULTS_HPP_
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERLISTENER_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerListener.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERCONTROLACCESS_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERNOTIFIER_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerNotifier.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_TEMPLATEDESCRIPTION_HPP_
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_COMMONFILEPICKERELEMENTIDS_HPP_
#include <com/sun/star/ui/dialogs/CommonFilePickerElementIds.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_EXTENDEDFILEPICKERELEMENTIDS_HPP_
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_LISTBOXCONTROLACTIONS_HPP_
#include <com/sun/star/ui/dialogs/ListboxControlActions.hpp>
#endif

#ifndef _COM_SUN_STAR_UI_DIALOGS_XFILEPREVIEW_HPP_
#include <com/sun/star/ui/dialogs/XFilePreview.hpp>
#endif

#include <osl/thread.h>

#include <stdio.h>
#include <windows.h>

#include "..\FPServiceInfo.hxx"

//
//  namesapces
//

using namespace ::rtl                   ;
using namespace ::cppu                  ;
using namespace ::com::sun::star::uno   ;
using namespace ::com::sun::star::lang  ;
using namespace ::com::sun::star::ui::dialogs   ;
using namespace ::com::sun::star::ui::dialogs::TemplateDescription;

using namespace ::com::sun::star::ui::dialogs::CommonFilePickerElementIds;
using namespace ::com::sun::star::ui::dialogs::ExtendedFilePickerElementIds;
using namespace ::com::sun::star::ui::dialogs::ListboxControlActions;

using namespace std                     ;

// forward

void TestFilterManager( Reference< XFilePicker > xFilePicker );


#define RDB_SYSPATH "D:\\Projects\\gsl\\sysui\\wntmsci7\\bin\\applicat.rdb"

//_________________________________________________________________________________________________________________________
//  global variables
//_________________________________________________________________________________________________________________________

Reference< XMultiServiceFactory >   g_xFactory;

const OUString BMP_EXTENSION = OUString::createFromAscii( "bmp" );

//-------------------------------------------------------------------------------------------------------------------------
// a test client
//-------------------------------------------------------------------------------------------------------------------------

class FilePickerListener : public WeakImplHelper1< XFilePickerListener >
{
public:

    // XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source )
        throw(::com::sun::star::uno::RuntimeException);

    // XFilePickerListener
    virtual void SAL_CALL fileSelectionChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
        throw(::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL directoryChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
        throw(::com::sun::star::uno::RuntimeException);

    virtual OUString SAL_CALL helpRequested( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
        throw(::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL controlStateChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
        throw(::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL dialogSizeChanged(  )
        throw (::com::sun::star::uno::RuntimeException);
};

void SAL_CALL FilePickerListener::disposing( const ::com::sun::star::lang::EventObject& Source )
    throw(::com::sun::star::uno::RuntimeException)
{
}

void SAL_CALL FilePickerListener::fileSelectionChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
    throw(::com::sun::star::uno::RuntimeException)
{
    try
    {
        Reference< XFilePicker > rXFilePicker( aEvent.Source, UNO_QUERY );
        Reference< XFilePreview > rXFilePreview( rXFilePicker, UNO_QUERY );

        if ( !rXFilePreview.is( ) )
            return;

        Sequence< OUString > aFileList = rXFilePicker->getFiles( );
        if ( 1 == aFileList.getLength( ) )
        {
            OUString FilePath = aFileList[0];

            // detect file extension
            sal_Int32 nIndex = FilePath.lastIndexOf( BMP_EXTENSION );
            if ( (FilePath.getLength( ) - 3) == nIndex )
            {
                 OUString FileSysPath;
                ::osl::FileBase::getSystemPathFromFileURL(
                    FilePath, FileSysPath );

                 HANDLE hFile = CreateFileW(
                     FileSysPath.getStr( ),
                     GENERIC_READ, FILE_SHARE_READ, NULL,
                     OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

                 if (hFile == INVALID_HANDLE_VALUE)
                      return;

                 DWORD dwHighSize;
                 DWORD dwFileSize = GetFileSize (hFile, &dwHighSize) ;

                 if (dwHighSize)
                 {
                      CloseHandle (hFile) ;
                      return;
                 }

                 Sequence< sal_Int8 > aDIB( dwFileSize );

                 DWORD dwBytesRead;
                 BOOL bSuccess = ReadFile (hFile, aDIB.getArray( ), dwFileSize, &dwBytesRead, NULL) ;
                 CloseHandle (hFile);

                 BITMAPFILEHEADER* pbmfh = (BITMAPFILEHEADER*)aDIB.getConstArray( );
                 if (!bSuccess || (dwBytesRead != dwFileSize)
                               || (pbmfh->bfType != * (WORD *) "BM")
                               || (pbmfh->bfSize != dwFileSize))
                 {
                    return;
                 }

                Any aAny;

                aAny <<= aDIB;
                rXFilePreview->setImage( 1, aAny );
            }
        }
    }
    catch( IllegalArgumentException& ex )
    {
        ex = ex;
    }
}

void SAL_CALL FilePickerListener::directoryChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
    throw(::com::sun::star::uno::RuntimeException)
{
    Reference< XFilePickerControlAccess > rFilePickerCtrlAccess( aEvent.Source, UNO_QUERY );
}

OUString SAL_CALL FilePickerListener::helpRequested( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
    throw(::com::sun::star::uno::RuntimeException)
{
    return OUString( );
}

void SAL_CALL FilePickerListener::controlStateChanged( const ::com::sun::star::ui::dialogs::FilePickerEvent& aEvent )
    throw(::com::sun::star::uno::RuntimeException)
{
    try
    {
        Reference< XFilePickerControlAccess > rFPCtrlAccess( aEvent.Source, UNO_QUERY );

        Any aValue;

        OUString lbString( L"Ein Eintrag 1" );
        aValue <<= lbString;
        rFPCtrlAccess->setValue( LISTBOX_VERSION, ADD_ITEM, aValue );

        lbString = OUString( L"Ein Eintrag 2" );
        aValue <<= lbString;
        rFPCtrlAccess->setValue( LISTBOX_VERSION, ADD_ITEM, aValue );

        lbString = OUString( L"Ein Eintrag 3" );
        aValue <<= lbString;
        rFPCtrlAccess->setValue( LISTBOX_VERSION, ADD_ITEM, aValue );

        sal_Int16 nSel = 1;
        aValue <<= nSel;
        rFPCtrlAccess->setValue( LISTBOX_VERSION, SET_SELECT_ITEM, aValue );

        sal_Int32 nDel = 0;
        aValue <<= nDel;
        rFPCtrlAccess->setValue( LISTBOX_VERSION, DELETE_ITEM, aValue );
    }
    catch( ... )
    {
    }
}

void SAL_CALL FilePickerListener::dialogSizeChanged( )
    throw(::com::sun::star::uno::RuntimeException)
{
}

//--------------------------------------------------------
//  main
//--------------------------------------------------------


int SAL_CALL main(int nArgc, char* Argv[], char* Env[]  )
{
    printf("Starting test of FPS-Service\n");

    //-------------------------------------------------
    // get the global service-manager
    //-------------------------------------------------

    // Get global factory for uno services.
    OUString rdbName = OUString( RTL_CONSTASCII_USTRINGPARAM( RDB_SYSPATH ) );
    Reference< XMultiServiceFactory > g_xFactory( createRegistryServiceFactory( rdbName ) );

    // Print a message if an error occured.
    if ( g_xFactory.is() == sal_False )
    {
        OSL_ENSURE(sal_False, "Can't create RegistryServiceFactory");
        return(-1);
    }

    //-------------------------------------------------
    // try to get an Interface to a XFilePicker Service
    //-------------------------------------------------

    Sequence< Any > arguments(1);
        //arguments[0] = makeAny( FILEOPEN_SIMPLE );
        //arguments[0] = makeAny( FILESAVE_SIMPLE );
        //arguments[0] = makeAny( FILESAVE_AUTOEXTENSION_PASSWORD );
        //arguments[0] = makeAny( FILESAVE_AUTOEXTENSION_PASSWORD_FILTEROPTIONS );
        //arguments[0] = makeAny( FILESAVE_AUTOEXTENSION_SELECTION );
        //arguments[0] = makeAny( FILESAVE_AUTOEXTENSION_TEMPLATE );
        //arguments[0] = makeAny( FILEOPEN_LINK_PREVIEW_IMAGE_TEMPLATE );
        //arguments[0] = makeAny( FILEOPEN_PLAY );
        arguments[0] = makeAny( FILEOPEN_READONLY_VERSION );

    Reference< XFilePicker > xFilePicker = Reference< XFilePicker >(
        g_xFactory->createInstanceWithArguments(
            OUString::createFromAscii( FILE_PICKER_SERVICE_NAME ), arguments ), UNO_QUERY );

        // install a FilePicker notifier
        Reference< XFilePickerListener > xFPListener(
            static_cast< XFilePickerListener* >( new FilePickerListener()), UNO_QUERY );

        Reference< XFilePickerNotifier > xFPNotifier( xFilePicker, UNO_QUERY );
        if ( xFPNotifier.is( ) )
            xFPNotifier->addFilePickerListener( xFPListener );

        xFilePicker->setTitle( OUString::createFromAscii("FileOpen Simple..."));
        xFilePicker->setMultiSelectionMode( sal_True );
        xFilePicker->setDefaultName( OUString::createFromAscii("d:\\test2.sxw"));

        OUString aDirURL;
        OUString aSysPath = OStringToOUString( "d:\\���", osl_getThreadTextEncoding( ) );
        ::osl::FileBase::getFileURLFromSystemPath( aSysPath, aDirURL );
        xFilePicker->setDisplayDirectory( aDirURL );

        Reference< XFilterManager > xFilterMgr( xFilePicker, UNO_QUERY );
        if ( xFilterMgr.is( ) )
        {
            xFilterMgr->appendFilter( L"Alle", L"*.*" );
            xFilterMgr->appendFilter( L"BMP", L"*.bmp" );
            xFilterMgr->appendFilter( L"SDW", L"*.sdw;*.sdc;*.sdi" );
            xFilterMgr->appendFilter( L"SXW", L"*.sxw;*.sxi" );
        }

        Reference< XFilePickerControlAccess > xFPControlAccess( xFilePicker, UNO_QUERY );

        Any aAny;
        sal_Bool bChkState = sal_False;

        aAny.setValue( &bChkState, getCppuType( (sal_Bool*)0 ) );
        xFPControlAccess->setValue( CHECKBOX_AUTOEXTENSION, 0, aAny );

        OUString aVersion( L"Version 1" );
        aAny <<= aVersion;
        xFPControlAccess->setValue( LISTBOX_VERSION, ADD_ITEM, aAny );
        xFPControlAccess->setValue( LISTBOX_VERSION, ADD_ITEM, aAny );
        xFPControlAccess->setValue( LISTBOX_VERSION, ADD_ITEM, aAny );

        xFilePicker->execute( );

        sal_Bool bCheckState;
        aAny = xFPControlAccess->getValue( CHECKBOX_AUTOEXTENSION, 0 );
        if ( aAny.hasValue( ) )
            bCheckState = *reinterpret_cast< const sal_Bool* >( aAny.getValue( ) );

        aAny = xFPControlAccess->getValue( CHECKBOX_READONLY, 0 );
        if ( aAny.hasValue( ) )
            bCheckState = *reinterpret_cast< const sal_Bool* >( aAny.getValue( ) );

        aAny = xFPControlAccess->getValue( LISTBOX_VERSION, GET_SELECTED_ITEM );
        sal_Int32 nSel;
        if ( aAny.hasValue( ) )
            aAny >>= nSel;

        aDirURL = xFilePicker->getDisplayDirectory( );
        Sequence< OUString > aFileList = xFilePicker->getFiles( );
        for ( int i = 0; i < aFileList.getLength( ); i++ )
        {
            OUString nextPath = aFileList[i];
        }

        if ( xFPNotifier.is( ) )
            xFPNotifier->removeFilePickerListener( xFPListener );

    //--------------------------------------------------
    // shutdown
    //--------------------------------------------------

    // Cast factory to XComponent
    Reference< XComponent > xComponent( g_xFactory, UNO_QUERY );

    // Print a message if an error occured.
    if ( xComponent.is() == sal_False )
    {
        OSL_ENSURE(sal_False, "Error shuting down");
    }

    // Dispose and clear factory
    xComponent->dispose();
    g_xFactory.clear();
    g_xFactory = Reference< XMultiServiceFactory >();

    printf("Test successful\n");

    return 0;
}
