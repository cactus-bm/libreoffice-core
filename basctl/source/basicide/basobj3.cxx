/*************************************************************************
 *
 *  $RCSfile: basobj3.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: tbe $ $Date: 2001-09-25 09:12:40 $
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


#include <ide_pch.hxx>

#pragma hdrstop
#include <vector>
#include <algorithm>
#ifndef _SBXCLASS_HXX //autogen
#include <svtools/sbx.hxx>
#endif
#define _SVSTDARR_STRINGS
#include <svtools/svstdarr.hxx>

#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif

#include <iderdll.hxx>
#include <iderdll2.hxx>
#include <iderid.hxx>
#include <basobj.hxx>
#include <basidesh.hxx>
#include <basidesh.hrc>
#include <objdlg.hxx>
#include <bastypes.hxx>
#include <basdoc.hxx>

#include <baside2.hxx>
#include <baside3.hxx>
#include <basicmod.hxx>

#ifndef _BASCTL_DLGED_HXX
#include "dlged.hxx"
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRYARYCONTAINER_HPP_
#include <com/sun/star/script/XLibraryContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _XMLSCRIPT_XMLDLG_IMEXP_HXX_
#include <xmlscript/xmldlg_imexp.hxx>
#endif

using namespace comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;


#define LINE_SEP    0x0A

//----------------------------------------------------------------------------

extern "C" {
    long basicide_handle_basic_error( void* pPtr )
    {
        return BasicIDE::HandleBasicError( (StarBASIC*)pPtr );
    }
}

//----------------------------------------------------------------------------

SbMethod* BasicIDE::CreateMacro( SbModule* pModule, const String& rMacroName )
{
    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
    SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;

    if( pDispatcher )
    {
        pDispatcher->Execute( SID_BASICIDE_STOREALLMODULESOURCES );
    }

    if ( pModule->GetMethods()->Find( rMacroName, SbxCLASS_METHOD ) )
        return 0;

    String aMacroName( rMacroName );
    if ( aMacroName.Len() == 0 )
    {
        if ( !pModule->GetMethods()->Count() )
            aMacroName = String( RTL_CONSTASCII_USTRINGPARAM( "Main" ) );
        else
        {
            BOOL bValid = FALSE;
            String aStdMacroText( RTL_CONSTASCII_USTRINGPARAM( "Macro" ) );
            //String aStdMacroText( IDEResId( RID_STR_STDMACRONAME ) );
            USHORT nMacro = 1;
            while ( !bValid )
            {
                aMacroName = aStdMacroText;
                aMacroName += String::CreateFromInt32( nMacro );
                // Pruefen, ob vorhanden...
                bValid = pModule->GetMethods()->Find( aMacroName, SbxCLASS_METHOD ) ? FALSE : TRUE;
                nMacro++;
            }
        }
    }

    String aSource( pModule->GetSource() );
    aSource.ConvertLineEnd( LINEEND_LF );

    // Nicht zu viele Leerzeilen erzeugen...
    if ( aSource.Len() > 2 )
    {
        if ( aSource.GetChar( aSource.Len() - 1 )  != LINE_SEP )
            aSource += String( RTL_CONSTASCII_USTRINGPARAM( "\n\n" ) );
        else if ( aSource.GetChar( aSource.Len() - 2 ) != LINE_SEP )
            aSource += String( RTL_CONSTASCII_USTRINGPARAM( "\n" ) );
        else if ( aSource.GetChar( aSource.Len() - 3 ) == LINE_SEP )
            aSource.Erase( (USHORT)(aSource.Len()-1), 1 );
    }

    String aSubStr;
    aSubStr = String( RTL_CONSTASCII_USTRINGPARAM( "Sub " ) );
    aSubStr += aMacroName;
    aSubStr += String( RTL_CONSTASCII_USTRINGPARAM( "\n\nEnd Sub" ) );

    aSource += aSubStr;
    pModule->SetSource( aSource );

    // update module in library
    SbxObject* pParent = pModule->GetParent();
    StarBASIC* pBasic = PTR_CAST(StarBASIC,pParent);
    DBG_ASSERT(pBasic, "BasicIDE::CreateMacro: No Basic found!");
    if ( pBasic )
    {
        BasicManager* pBasMgr = BasicIDE::FindBasicManager( pBasic );
        DBG_ASSERT(pBasMgr, "BasicIDE::CreateMacro: No BasicManager found!");
        if ( pBasMgr )
        {
            SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
            String aLibName = pBasic->GetName();
            String aModName = pModule->GetName();
            ::rtl::OUString aModule = pModule->GetSource();

            BasicIDE::UpdateModule( pShell, aLibName, aModName, aModule );
        }
    }

    SbMethod* pMethod = (SbMethod*)pModule->GetMethods()->Find( aMacroName, SbxCLASS_METHOD );

    if( pDispatcher )
    {
        pDispatcher->Execute( SID_BASICIDE_UPDATEALLMODULESOURCES );
    }

    if ( pBasic )
        BasicIDE::MarkDocShellModified( pBasic );

    return pMethod;
}

//----------------------------------------------------------------------------

Reference< script::XLibraryContainer > BasicIDE::GetDialogLibraryContainer( SfxObjectShell* pShell )
{
    // get library container
    Reference< script::XLibraryContainer > xLibContainer;
    if( pShell )
    {
        // document
        xLibContainer = Reference< script::XLibraryContainer >( pShell->GetDialogContainer(), UNO_QUERY );
    }
    else
    {
        // application
        xLibContainer = Reference< script::XLibraryContainer >( SFX_APP()->GetDialogContainer(), UNO_QUERY );
    }

    return xLibContainer;
}

//----------------------------------------------------------------------------

BOOL BasicIDE::HasDialogLibrary( SfxObjectShell* pShell, const String& rLibName )
{
    BOOL bHasDialogLibrary = FALSE;

    // get library container
    Reference< script::XLibraryContainer > xLibContainer = GetDialogLibraryContainer( pShell );

    // check if library container has dialog library
    ::rtl::OUString aOULibName( rLibName );
    if( xLibContainer.is() && xLibContainer->hasByName( aOULibName ) )
    {
        bHasDialogLibrary = TRUE;
    }

    return bHasDialogLibrary;
}

//----------------------------------------------------------------------------

Reference< container::XNameContainer > BasicIDE::GetDialogLibrary( SfxObjectShell* pShell, const String& rLibName, BOOL bLoadLibrary )
    throw(NoSuchElementException)
{
    // get library container
    Reference< script::XLibraryContainer > xLibContainer = GetDialogLibraryContainer( pShell );

    // get library
    Reference< container::XNameContainer > xLib;
    ::rtl::OUString aOULibName( rLibName );
    if( xLibContainer.is() && xLibContainer->hasByName( aOULibName ) )
    {
        Any aElement = xLibContainer->getByName( aOULibName );
        aElement >>= xLib;
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::GetDialogLibrary: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    // load library
    if( bLoadLibrary && !xLibContainer->isLibraryLoaded( aOULibName ) )
        xLibContainer->loadLibrary( aOULibName );

    return xLib;
}

//----------------------------------------------------------------------------

Reference< ::com::sun::star::container::XNameContainer > BasicIDE::CreateDialogLibrary( SfxObjectShell* pShell, const String& rLibName )
    throw(ElementExistException)
{
    // get library container
    Reference< script::XLibraryContainer > xLibContainer = GetDialogLibraryContainer( pShell );

    // create dialog library
    Reference< container::XNameContainer > xLib;
    ::rtl::OUString aOULibName( rLibName );
    if( xLibContainer.is() && !xLibContainer->hasByName( aOULibName ) )
    {
        xLib = xLibContainer->createLibrary( aOULibName );
    }
    else
    {
        throw ElementExistException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::CreateDialogLibrary: ElementExistException!") ),
            Reference<XInterface>() );
    }

    return xLib;
}

//----------------------------------------------------------------------------

Sequence< ::rtl::OUString > BasicIDE::GetDialogNames( SfxObjectShell* pShell, const String& rLibName )
    throw(NoSuchElementException)
{
    Sequence< ::rtl::OUString > aSeqDlgNames;
    Reference< container::XNameContainer > xLib = GetDialogLibrary( pShell, rLibName );
    if( xLib.is() )
    {
        Sequence< ::rtl::OUString > aDlgNames = xLib->getElementNames();
        sal_Int32 nDlgCount = aDlgNames.getLength();
        const ::rtl::OUString* pDlgNames = aDlgNames.getConstArray();

        // sort dialog names
        ::std::vector<String> aDlgList( nDlgCount );
        for ( sal_Int32 i = 0 ; i < nDlgCount ; i++ )
            aDlgList[ i ] = pDlgNames[ i ];
        ::std::sort( aDlgList.begin() , aDlgList.end() , StringCompareLessThan );

        // copy to sequence
        aSeqDlgNames.realloc( nDlgCount );
        for ( i = 0 ; i < nDlgCount ; i++ )
            aSeqDlgNames.getArray()[ i ] = aDlgList[ i ];
    }

    return aSeqDlgNames;
}

//----------------------------------------------------------------------------

BOOL BasicIDE::HasDialog( SfxObjectShell* pShell, const String& rLibName, const String& rDlgName )
{
    BOOL bHasDialog = FALSE;

    Reference< container::XNameContainer > xLib;
    try
    {
        // get library
        xLib = GetDialogLibrary( pShell, rLibName, TRUE );

        // check if library has dialog
        ::rtl::OUString aOUDlgName( rDlgName );
        if( xLib.is() && xLib->hasByName( aOUDlgName ) )
        {
            bHasDialog = TRUE;
        }
    }
    catch ( container::NoSuchElementException& e )
    {
        ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aBStr.GetBuffer() );
    }

    return bHasDialog;
}

//----------------------------------------------------------------------------

Reference< io::XInputStreamProvider > BasicIDE::GetDialog( SfxObjectShell* pShell, const String& rLibName, const String& rDlgName )
    throw(NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetDialogLibrary( pShell, rLibName, TRUE );

    // get dialog
    Reference< io::XInputStreamProvider > xISP;
    ::rtl::OUString aOUDlgName( rDlgName );
    if( xLib.is() && xLib->hasByName( aOUDlgName ) )
    {
        Any aElement = xLib->getByName( aOUDlgName );
        aElement >>= xISP;
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::GetDialog: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    return xISP;
}

//----------------------------------------------------------------------------

String BasicIDE::CreateDialogName( SfxObjectShell* pShell, const String& rLibName )
{
    String aDlgName;
    String aDlgStdName( RTL_CONSTASCII_USTRINGPARAM( "Dialog" ) );
    //String aDlgStdName( IDEResId( RID_STR_STDDIALOGNAME ) );
    BOOL bValid = FALSE;
    USHORT i = 1;
    while ( !bValid )
    {
        aDlgName = aDlgStdName;
        aDlgName += String::CreateFromInt32( i );
        if ( !BasicIDE::HasDialog( pShell, rLibName, aDlgName ) )
            bValid = TRUE;

        i++;
    }

    return aDlgName;
}

//----------------------------------------------------------------------------

Reference< io::XInputStreamProvider > BasicIDE::CreateDialog( SfxObjectShell* pShell, const String& rLibName, const String& rDlgName )
    throw(ElementExistException, NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetDialogLibrary( pShell, rLibName, TRUE );

    // create dialog
    Reference< io::XInputStreamProvider > xISP;
    ::rtl::OUString aOUDlgName( rDlgName );
    if( xLib.is() && !xLib->hasByName( aOUDlgName ) )
    {
        // create new dialog model
        Reference< lang::XMultiServiceFactory > xMSF = getProcessServiceFactory();
        Reference< container::XNameContainer > xDialogModel( xMSF->createInstance
            ( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialogModel" ) ) ), UNO_QUERY );

        // set name property
        Reference< beans::XPropertySet > xDlgPSet( xDialogModel, UNO_QUERY );
        Any aName;
        aName <<= aOUDlgName;
        xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Name" ) ), aName );

        // export dialog model
        Reference< XComponentContext > xContext;
        Reference< beans::XPropertySet > xProps( xMSF, UNO_QUERY );
        OSL_ASSERT( xProps.is() );
        OSL_VERIFY( xProps->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DefaultContext")) ) >>= xContext );
        xISP = ::xmlscript::exportDialogModel( xDialogModel, xContext );
        Any aElement;
        aElement <<= xISP;

        // insert dialog into library
        xLib->insertByName( aOUDlgName, aElement );

        // doc shell modified
        BasicIDE::MarkDocShellModified( pShell );   // here?
    }
    else
    {
        throw ElementExistException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::CreateDialog: ElementExistException!") ),
            Reference<XInterface>() );
    }

    return xISP;
}

//----------------------------------------------------------------------------

void BasicIDE::RenameDialog( SfxObjectShell* pShell, const String& rLibName, const String& rOldName, const String& rNewName )
    throw(ElementExistException, NoSuchElementException)
{
    ::rtl::OUString aOUOldName( rOldName );
    ::rtl::OUString aOUNewName( rNewName );

    // get library
    Reference< container::XNameContainer > xLib = GetDialogLibrary( pShell, rLibName, TRUE );

    // rename dialog
    if( xLib.is() && xLib->hasByName( aOUOldName ) )
    {
        if ( xLib->hasByName( aOUNewName ) )
        {
            throw ElementExistException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::RenameDialog: ElementExistException!") ),
                Reference<XInterface>() );
        }

        // get dialog
        Any aElement = xLib->getByName( aOUOldName );

        // remove dialog from dialog container
        xLib->removeByName( aOUOldName );

        // create dialog model
        Reference< lang::XMultiServiceFactory > xMSF = getProcessServiceFactory();
        Reference< container::XNameContainer > xDialogModel( xMSF->createInstance
            ( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialogModel" ) ) ), UNO_QUERY );

        Reference< io::XInputStreamProvider > xISP;
        aElement >>= xISP;
        if( xISP.is() )
        {
            // import dialog model
            Reference< XComponentContext > xContext;
            Reference< beans::XPropertySet > xProps( xMSF, UNO_QUERY );
            OSL_ASSERT( xProps.is() );
            OSL_VERIFY( xProps->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DefaultContext")) ) >>= xContext );
            Reference< io::XInputStream > xInput( xISP->createInputStream() );
            ::xmlscript::importDialogModel( xInput, xDialogModel, xContext );

            // set new name as property
            Reference< beans::XPropertySet > xDlgPSet( xDialogModel, UNO_QUERY );
            Any aName;
            aName <<= aOUNewName;
            xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Name" ) ), aName );

            // export dialog model
            xISP = ::xmlscript::exportDialogModel( xDialogModel, xContext );
            aElement <<= xISP;
        }

        // insert dialog by new name in dialog container
        xLib->insertByName( aOUNewName, aElement );
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::RenameDialog: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    if ( pIDEShell )
    {
        IDEBaseWindow* pWin = pIDEShell->FindWindow( pShell, rLibName, rOldName, BASICIDE_TYPE_DIALOG, FALSE );
        if ( pWin )
        {
            // set new name in window
            pWin->SetName( rNewName );

            // get dialog model from dialog editor and set new name as property
            Reference< container::XNameContainer > xDlgModel = ((DialogWindow*)pWin)->GetEditor()->GetDialog();
            if( xDlgModel.is() )
            {
                Reference< beans::XPropertySet > xPSet( xDlgModel, UNO_QUERY );
                Any aName;
                aName <<= aOUNewName;
                xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Name" ) ), aName );
            }

            // update property browser
            ((DialogWindow*)pWin)->UpdateBrowser();

            // update tabwriter
            USHORT nId = (USHORT)(pIDEShell->GetIDEWindowTable()).GetKey( pWin );
            DBG_ASSERT( nId, "No entry in Tabbar!" );
            if ( nId )
            {
                BasicIDETabBar* pTabBar = (BasicIDETabBar*)pIDEShell->GetTabBar();
                pTabBar->SetPageText( nId, rNewName );
                pTabBar->Sort();
                pTabBar->MakeVisible( pTabBar->GetCurPageId() );
            }
        }
    }
}

//----------------------------------------------------------------------------

void BasicIDE::RemoveDialog( SfxObjectShell* pShell, const String& rLibName, const String& rDlgName )
    throw(NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetDialogLibrary( pShell, rLibName, TRUE );

    // remove dialog
    ::rtl::OUString aOUDlgName( rDlgName );
    if( xLib.is() && xLib->hasByName( aOUDlgName ) )
    {
        xLib->removeByName( aOUDlgName );
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::RemoveDialog: NoSuchElementException!") ),
            Reference<XInterface>() );
    }
}

//----------------------------------------------------------------------------

void BasicIDE::InsertDialog( SfxObjectShell* pShell, const String& rLibName, const String& rDlgName,
                             const Reference< io::XInputStreamProvider >& xISP )
    throw(ElementExistException, NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetDialogLibrary( pShell, rLibName, TRUE );

    // insert dialog into library
    ::rtl::OUString aOUDlgName( rDlgName );
    if( xLib.is() && !xLib->hasByName( aOUDlgName ) )
    {
        Any aElement;
        aElement <<= xISP;
        xLib->insertByName( aOUDlgName, aElement );
    }
    else
    {
        throw ElementExistException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::InsertDialog: ElementExistException!") ),
            Reference<XInterface>() );
    }
}

//----------------------------------------------------------------------------

StarBASIC* BasicIDE::FindBasic( const SbxVariable* pVar )
{
    const SbxVariable* pSbx = pVar;
    while ( pSbx && !pSbx->ISA( StarBASIC ) )
        pSbx = pSbx->GetParent();

    DBG_ASSERT( !pSbx || pSbx->ISA( StarBASIC ), "Find Basic: Kein Basic!" );
    return (StarBASIC*)pSbx;
}

//----------------------------------------------------------------------------

BasicManager* BasicIDE::FindBasicManager( StarBASIC* pLib )
{
    BasicManager* pBasicMgr = SFX_APP()->GetBasicManager();
    SfxObjectShell* pDocShell = 0;
    while ( pBasicMgr )
    {
        Sequence< ::rtl::OUString > aLibNames = GetLibraryNames( pDocShell );
        sal_Int32 nLibCount = aLibNames.getLength();
        const ::rtl::OUString* pLibNames = aLibNames.getConstArray();

        for ( sal_Int32 i = 0 ; i < nLibCount ; i++ )
        {
            StarBASIC* pL = pBasicMgr->GetLib( pLibNames[ i ] );
            if ( pL == pLib )
                return pBasicMgr;
        }

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }
    return 0;
}

//----------------------------------------------------------------------------

BasicManager* BasicIDE::FindBasicManager( const String& aBasMgr )
{
    BasicManager* pBasicMgr = SFX_APP()->GetBasicManager();
    SfxObjectShell* pDocShell = 0;
    while ( pBasicMgr )
    {
        String aMgr;
        if (pDocShell)
            aMgr = pDocShell->GetTitle( SFX_TITLE_FILENAME );
        else
            aMgr = Application::GetAppName();

        if ( aMgr == aBasMgr )
            return pBasicMgr;

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }
    return 0;
}

//----------------------------------------------------------------------------

String BasicIDE::FindTitle( BasicManager* pBasicManager, USHORT nSFXTitleType )
{
    BasicManager* pBasicMgr = SFX_APP()->GetBasicManager();
    SfxObjectShell* pDocShell = 0;
    String aTitle;
    while ( pBasicMgr )
    {
        if ( pBasicMgr == pBasicManager )
        {
            if (pDocShell)
                aTitle = pDocShell->GetTitle( nSFXTitleType );
            else
                aTitle = Application::GetAppName();
            return aTitle;
        }

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }
    return aTitle;
}

//----------------------------------------------------------------------------

SfxObjectShell* BasicIDE::FindDocShell( BasicManager* pBasMgr )
{
    SfxObjectShell* pDocShell = SfxObjectShell::GetFirst();
    while ( pDocShell )
    {
        if ( ( pDocShell->GetBasicManager() != SFX_APP()->GetBasicManager() ) &&
             ( pDocShell->GetBasicManager() == pBasMgr ) )
        {
            return pDocShell;
        }
        pDocShell = SfxObjectShell::GetNext( *pDocShell );
    }
    return 0;
}

//----------------------------------------------------------------------------

void BasicIDE::MarkDocShellModified( StarBASIC* pBasic )
{
        // BasicManager suchen, der zur Lib passt...
        BasicManager* pBasMgr = BasicIDE::FindBasicManager( pBasic );
        // Koennte z.B. nach CloseDoc schon weg sein...
        if ( pBasMgr )
        {
            SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
            // Muss ja nicht aus einem Document kommen...
            if ( pShell )
                pShell->SetModified();
            else
                IDE_DLL()->GetShell()->SetAppBasicModified();

            SfxBindings& rBindings = BasicIDE::GetBindings();
            rBindings.Invalidate( SID_SAVEDOC );
            rBindings.Update( SID_SAVEDOC );
        }

        // Objectcatalog updaten...
        BasicIDEShell* pShell = IDE_DLL()->GetShell();
        ObjectCatalog* pObjCatalog = pShell ? pShell->GetObjectCatalog() : 0;
        if ( pObjCatalog )
            pObjCatalog->UpdateEntries();
}

//----------------------------------------------------------------------------

void BasicIDE::MarkDocShellModified( SfxObjectShell* pShell )
{
    // Muss ja nicht aus einem Document kommen...
    if ( pShell )
        pShell->SetModified();
    else
        IDE_DLL()->GetShell()->SetAppBasicModified();

    SfxBindings& rBindings = BasicIDE::GetBindings();
    rBindings.Invalidate( SID_SAVEDOC );
    rBindings.Update( SID_SAVEDOC );

    // Objectcatalog updaten...
    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    ObjectCatalog* pObjCatalog = pIDEShell ? pIDEShell->GetObjectCatalog() : 0;
    if ( pObjCatalog )
        pObjCatalog->UpdateEntries();
}

//----------------------------------------------------------------------------

void BasicIDE::RunMethod( SbMethod* pMethod )
{
    SbxValues aRes;
    aRes.eType = SbxVOID;
    pMethod->Get( aRes );
}

//----------------------------------------------------------------------------

void BasicIDE::StopBasic()
{
    StarBASIC::Stop();
    BasicIDEShell* pShell = IDE_DLL()->GetShell();
    if ( pShell )
    {
        IDEWindowTable& rWindows = pShell->GetIDEWindowTable();
        IDEBaseWindow* pWin = rWindows.First();
        while ( pWin )
        {
            // BasicStopped von Hand rufen, da das Stop-Notify ggf. sonst nicht
            // durchkommen kann.
            pWin->BasicStopped();
            pWin = rWindows.Next();
        }
    }
    BasicIDE::BasicStopped();
}

//----------------------------------------------------------------------------

void BasicIDE::BasicStopped( BOOL* pbAppWindowDisabled,
        BOOL* pbDispatcherLocked, USHORT* pnWaitCount,
        SfxUInt16Item** ppSWActionCount, SfxUInt16Item** ppSWLockViewCount )
{
    // Nach einem Error oder dem expliziten abbrechen des Basics muessen
    // ggf. einige Locks entfernt werden...

    if ( pbAppWindowDisabled )
        *pbAppWindowDisabled = FALSE;
    if ( pbDispatcherLocked )
        *pbDispatcherLocked = FALSE;
    if ( pnWaitCount )
        *pnWaitCount = 0;
    if ( ppSWActionCount )
        *ppSWActionCount = 0;
    if ( ppSWLockViewCount )
        *ppSWLockViewCount = 0;

    // AppWait ?
    USHORT nWait = 0;
    while ( Application::IsWait() )
    {
        Application::LeaveWait();
        nWait++;
    }
    if ( pnWaitCount )
        *pnWaitCount = nWait;

    // Interactive = FALSE ?
    if ( SFX_APP()->IsDispatcherLocked() )
    {
        SFX_APP()->LockDispatcher( FALSE );
        if ( pbDispatcherLocked )
            *pbDispatcherLocked = TRUE;
    }
    Window* pDefParent = Application::GetDefDialogParent();
    if ( pDefParent && !pDefParent->IsEnabled() )
    {
        // Aber nicht wenn sich noch ein Dialog im Testmodus befindet!
//      if ( pDefParent == Application::GetAppWindow() )
        {
//          Application::GetAppWindow()->Enable( TRUE );
            pDefParent->Enable( TRUE );
            if ( pbAppWindowDisabled )
                *pbAppWindowDisabled = TRUE;
        }
    }

}

//----------------------------------------------------------------------------

void BasicIDE::InvalidateDebuggerSlots()
{
    SfxBindings& rBindings = BasicIDE::GetBindings();
    rBindings.Invalidate( SID_BASICSTOP );
    rBindings.Update( SID_BASICSTOP );
    rBindings.Invalidate( SID_BASICRUN );
    rBindings.Update( SID_BASICRUN );
    rBindings.Invalidate( SID_BASICCOMPILE );
    rBindings.Update( SID_BASICCOMPILE );
    rBindings.Invalidate( SID_BASICSTEPOVER );
    rBindings.Update( SID_BASICSTEPOVER );
    rBindings.Invalidate( SID_BASICSTEPINTO );
    rBindings.Update( SID_BASICSTEPINTO );
    rBindings.Invalidate( SID_BASICSTEPOUT );
    rBindings.Update( SID_BASICSTEPOUT );
    rBindings.Invalidate( SID_BASICIDE_TOGGLEBRKPNT );
    rBindings.Update( SID_BASICIDE_TOGGLEBRKPNT );
    rBindings.Invalidate( SID_BASICIDE_STAT_POS );
    rBindings.Update( SID_BASICIDE_STAT_POS );
}

//----------------------------------------------------------------------------

long BasicIDE::HandleBasicError( StarBASIC* pBasic )
{
    BASIC_MOD()->Load();

    BasicIDE::BasicStopped();

    // no error output during macro choosing
    if ( IDE_DLL()->GetExtraData()->ChoosingMacro() )
        return 1;
    if ( IDE_DLL()->GetExtraData()->ShellInCriticalSection() )
        return 2;

    long nRet = 0;
    BasicIDEShell* pShell = 0;
    if ( SvtModuleOptions().IsBasicIDE() )
    {
        BasicManager* pBasicManager = BasicIDE::FindBasicManager( pBasic );
        if ( pBasicManager )
        {
            USHORT nLib = pBasicManager->GetLibId( pBasic );
            // TODO: check password
            //if ( !pBasicManager->HasPassword( nLib ) ||
            //      pBasicManager->IsPasswordVerified( nLib ) )
            //{
                pShell = IDE_DLL()->GetShell();
                if ( !pShell )
                {
                    SfxViewFrame* pCurFrame = SfxViewFrame::Current();
                    DBG_ASSERT( pCurFrame != NULL, "No current view frame!" );
                    SfxDispatcher* pDispatcher = pCurFrame ? pCurFrame->GetDispatcher() : NULL;
                    if( pDispatcher )
                    {
                        pDispatcher->Execute( SID_BASICIDE_APPEAR, SFX_CALLMODE_SYNCHRON );
                    }
                    pShell = IDE_DLL()->GetShell();
                }
            //}
        }
    }

    if ( pShell )
        nRet = pShell->CallBasicErrorHdl( pBasic );
    else
        ErrorHandler::HandleError( StarBASIC::GetErrorCode() );

    return nRet;
}

//----------------------------------------------------------------------------

SvStrings* BasicIDE::CreateBasicLibBoxEntries()
{
    SvStrings* pStrings = new SvStrings;
    pStrings->Insert( new String( IDEResId( RID_STR_ALL ) ), 0 );

    BasicManager* pBasicMgr = SFX_APP()->GetBasicManager();
    SfxObjectShell* pDocShell = 0;
    while ( pBasicMgr )
    {
        // Nur, wenn es ein dazugehoeriges Fenster gibt, damit nicht die
        // Gecachten Docs, die nicht sichtbar sind ( Remot-Dokumente )
        if ( !pDocShell || ( ( pBasicMgr != SFX_APP()->GetBasicManager() )
                                && !pDocShell->ISA(BasicDocShell)
                                && ( SfxViewFrame::GetFirst( pDocShell ) ) ) )
        {
            String aBasMgr;
            if (pDocShell)
                aBasMgr = pDocShell->GetTitle( SFX_TITLE_FILENAME );
            else
                aBasMgr = Application::GetAppName();

            // get a sorted list of library names
            Sequence< ::rtl::OUString > aLibNames = BasicIDE::GetLibraryNames( pDocShell );
            sal_Int32 nLibCount = aLibNames.getLength();
            const ::rtl::OUString* pLibNames = aLibNames.getConstArray();

            for ( sal_Int32 i = 0 ; i < nLibCount ; i++ )
            {
                String aLibName = pLibNames[ i ];
                String* pEntry = new String( CreateMgrAndLibStr( aBasMgr, aLibName ) );
                pStrings->Insert( pEntry, pStrings->Count() );
            }
        }

        if ( pDocShell  )
            pDocShell = SfxObjectShell::GetNext( *pDocShell );
        else
            pDocShell = SfxObjectShell::GetFirst();

        pBasicMgr = ( pDocShell ? pDocShell->GetBasicManager() : 0 );
    }
    return pStrings;
}

//----------------------------------------------------------------------------

SfxBindings& BasicIDE::GetBindings()
{
    BasicIDEDLL* pIDEDLL = IDE_DLL();
    if ( pIDEDLL && pIDEDLL->GetShell() )
    {
        // #63960# fuer BasicIDE die Bindings der APP
        // 07/00 Now the APP Dispatcher is 'dead', SFX changes...
//      return SFX_APP()->GetAppBindings();
        return pIDEDLL->GetShell()->GetFrame()->GetBindings();
    }
    SfxViewFrame* pCurFrame = SfxViewFrame::Current();
    DBG_ASSERT( pCurFrame != NULL, "No current view frame!" );
    return pCurFrame->GetBindings();
}

//----------------------------------------------------------------------------


