/*************************************************************************
 *
 *  $RCSfile: basobj2.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: tbe $ $Date: 2001-07-25 07:19:18 $
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

#ifndef _SBXCLASS_HXX //autogen
#include <svtools/sbx.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif
#include <basobj.hxx>
#include <iderdll.hxx>
#include <iderdll2.hxx>
#include <iderid.hxx>
#include <macrodlg.hxx>
#include <moduldlg.hxx>
#include <basidesh.hxx>
#include <baside2.hxx>
#include <basicmod.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;

//----------------------------------------------------------------------------

extern "C" {
    rtl_uString* basicide_choose_macro( BOOL bExecute, BOOL bChooseOnly, rtl_uString* pMacroDesc )
    {
        ::rtl::OUString aMacroDesc( pMacroDesc );
        ::rtl::OUString aMacroURL = BasicIDE::ChooseMacro( bExecute, bChooseOnly, aMacroDesc );
        rtl_uString* pMacroURL = aMacroURL.pData;
        rtl_uString_acquire( pMacroURL );

        return pMacroURL;
    }
}

//----------------------------------------------------------------------------

SfxMacro* BasicIDE::CreateMacro()
{
    DBG_ERROR( "BasicIDE::CreateMacro() - war eigentlich nur fuer Macro-Recording!" );
    IDE_DLL()->GetExtraData()->ChoosingMacro() = TRUE;
    SFX_APP()->EnterBasicCall();
    Window* pParent = Application::GetDefDialogParent();
    SfxMacro* pMacro = 0;
    MacroChooser* pChooser = new MacroChooser( pParent, TRUE );
    Window* pOldModalDialogParent = Application::GetDefDialogParent();
    Application::SetDefDialogParent( pChooser );
//  pChooser->SetMode( MACROCHOOSER_RECORDING );
    short nRetValue = pChooser->Execute();

    Application::SetDefDialogParent( pOldModalDialogParent );
    delete pChooser;

    SFX_APP()->LeaveBasicCall();
    IDE_DLL()->GetExtraData()->ChoosingMacro() = FALSE;

    return pMacro;
}

//----------------------------------------------------------------------------

/*
SbMethod* BasicIDE::ChooseMacro( BOOL bExecute, BOOL bChooseOnly )
{
    IDE_DLL()->GetExtraData()->ChoosingMacro() = TRUE;
    SFX_APP()->EnterBasicCall();

    SbMethod* pMethod = NULL;

    Window* pParent = Application::GetDefDialogParent();

    MacroChooser* pChooser = new MacroChooser( pParent, TRUE );
    if ( bChooseOnly || !SvtModuleOptions().IsBasicIDE() )
        pChooser->SetMode( MACROCHOOSER_CHOOSEONLY );

    short nRetValue = pChooser->Execute();

    IDE_DLL()->GetExtraData()->ChoosingMacro() = FALSE;

    switch ( nRetValue )
    {
        case MACRO_OK_RUN:
        {
            pMethod = pChooser->GetMacro();
            if ( pMethod && bExecute )
            {
                pMethod->AddRef();  // festhalten, bis Event abgearbeitet.
                Application::PostUserEvent( LINK( IDE_DLL()->GetExtraData(), BasicIDEData, ExecuteMacroEvent ), pMethod );
            }
        }
        break;
    }

    delete pChooser;

    SFX_APP()->LeaveBasicCall();

    return pMethod;
}
*/

//----------------------------------------------------------------------------

/*
SbMethod* BasicIDE::ChooseMacro( BOOL bExecute, BOOL bChooseOnly, const String& rPreferredMacroDesciption )
{
    if ( rPreferredMacroDesciption.Len() )
        IDE_DLL()->GetExtraData()->GetLastMacro() = rPreferredMacroDesciption;
    return BasicIDE::ChooseMacro( bExecute, bChooseOnly );
}
*/

//----------------------------------------------------------------------------

void BasicIDE::Organize()
{
    Window* pParent = Application::GetDefDialogParent();
    OrganizeDialog* pDlg = new OrganizeDialog( pParent );
    if ( IDE_DLL()->GetShell() )
    {
        IDEBaseWindow* pWin = IDE_DLL()->GetShell()->GetCurWindow();
        if ( pWin )
            pDlg->SetCurrentModule( pWin->CreateSbxDescription() );
    }
    pDlg->Execute();
    delete pDlg;
}

//----------------------------------------------------------------------------

BOOL BasicIDE::IsValidSbxName( const String& rName )
{
    for ( USHORT nChar = 0; nChar < rName.Len(); nChar++ )
    {
        BOOL bValid = ( ( rName.GetChar(nChar) >= 'A' && rName.GetChar(nChar) <= 'Z' ) ||
                        ( rName.GetChar(nChar) >= 'a' && rName.GetChar(nChar) <= 'z' ) ||
                        ( rName.GetChar(nChar) >= '0' && rName.GetChar(nChar) <= '9' && nChar ) ||
                        ( rName.GetChar(nChar) == '_' ) );
        if ( !bValid )
            return FALSE;
    }
    return TRUE;
}

//----------------------------------------------------------------------------

SbMethod* BasicIDE::FindMacro( SbModule* pModule, const String& rMacroName )
{
    return (SbMethod*)pModule->GetMethods()->Find( rMacroName, SbxCLASS_METHOD );
}

//----------------------------------------------------------------------------

SbModule* BasicIDE::FindModule( StarBASIC* pBasic, const String& rModName )
{
    return pBasic->FindModule( rModName );
}

//----------------------------------------------------------------------------

USHORT BasicIDE::GetBasicDialogCount()
{
    return IDE_DLL()->GetExtraData()->GetBasicDialogCount();
}

//----------------------------------------------------------------------------

void BasicIDE::IncBasicDialogCount()
{
    IDE_DLL()->GetExtraData()->IncBasicDialogCount();
}

//----------------------------------------------------------------------------

void BasicIDE::DecBasicDialogCount()
{
    DBG_ASSERT( GetBasicDialogCount(), "DecBasicDialogCount() - Count allready 0!" );
    IDE_DLL()->GetExtraData()->DecBasicDialogCount();
}

//----------------------------------------------------------------------------

::rtl::OUString BasicIDE::ChooseMacro( BOOL bExecute, BOOL bChooseOnly, const ::rtl::OUString& rMacroDesc )
{
    BASIC_MOD()->Load();

    if ( rMacroDesc.getLength() )
        IDE_DLL()->GetExtraData()->GetLastMacro() = String( rMacroDesc );

    IDE_DLL()->GetExtraData()->ChoosingMacro() = TRUE;
    SFX_APP()->EnterBasicCall();

    String aMacroURL;
    SbMethod* pMethod = NULL;

    Window* pParent = Application::GetDefDialogParent();

    MacroChooser* pChooser = new MacroChooser( pParent, TRUE );
    if ( bChooseOnly || !SvtModuleOptions().IsBasicIDE() )
        pChooser->SetMode( MACROCHOOSER_CHOOSEONLY );

    short nRetValue = pChooser->Execute();

    IDE_DLL()->GetExtraData()->ChoosingMacro() = FALSE;

    switch ( nRetValue )
    {
        case MACRO_OK_RUN:
        {
            pMethod = pChooser->GetMacro();

            if ( pMethod )
            {
                SbModule* pModule = pMethod->GetModule();
                DBG_ASSERT(pModule, "BasicIDE::ChooseMacro: No Module found!");
                if ( pModule )
                {
                    StarBASIC* pBasic = (StarBASIC*)pModule->GetParent();
                    DBG_ASSERT(pBasic, "BasicIDE::ChooseMacro: No Basic found!");
                    if ( pBasic )
                    {
                        BasicManager* pBasMgr = BasicIDE::FindBasicManager( pBasic );
                        DBG_ASSERT(pBasMgr, "BasicIDE::ChooseMacro: No BasicManager found!");
                        if ( pBasMgr )
                        {
                            SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
                            aMacroURL = String::CreateFromAscii("macro://");
                            if ( pShell )
                                aMacroURL += pShell->GetTitle( SFX_TITLE_APINAME );
                            aMacroURL += '/';
                            aMacroURL += pBasic->GetName();
                            aMacroURL += '.';
                            aMacroURL += pModule->GetName();
                            aMacroURL += '.';
                            aMacroURL += pMethod->GetName();
                            aMacroURL += String::CreateFromAscii("()");

                            /*
                            if ( bChooseOnly )  // !bExecute ?
                            {
                                //if ( pShell )
                                //{
                                //  String aBasMgrName = pShell->GetTitle( SFX_TITLE_APINAME );

                                sal_uInt16 nHashPos = aMacroURL.Search( '/', 8 );
                                String aBasMgrName( INetURLObject::decode(aMacroURL.Copy( 8, nHashPos-8 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET) );

                                if ( aBasMgrName.Len() )
                                {
                                    String aCurrentName = SfxObjectShell::Current()->GetTitle(SFX_TITLE_APINAME);

                                    if ( aBasMgrName == aCurrentName )
                                    {
                                        aMacroURL.SearchAndReplace( aBasMgrName , '.' );
                                    }
                                    else
                                    {
                                        aMacroURL = String();
                                        ErrorBox( NULL, WB_OK | WB_DEF_OK, String( IDEResId( RID_STR_ERRORSELECTMACRO ) ) ).Execute();
                                    }
                                }
                            }
                            */
                        }
                    }
                }
            }

            if ( pMethod && bExecute )
            {
                pMethod->AddRef();  // festhalten, bis Event abgearbeitet.
                Application::PostUserEvent( LINK( IDE_DLL()->GetExtraData(), BasicIDEData, ExecuteMacroEvent ), pMethod );
            }
        }
        break;
    }

    delete pChooser;

    SFX_APP()->LeaveBasicCall();

    return ::rtl::OUString( aMacroURL );
}

//----------------------------------------------------------------------------

Sequence< ::rtl::OUString > BasicIDE::GetMethodsFromModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName )
    throw(NoSuchElementException )
{
    // get module
    ::rtl::OUString aOUSource = GetModule( pShell, rLibName, rModName );

    SbModuleRef xModule = new SbModule( rModName );
    xModule->SetSource( String( aOUSource ) );
    USHORT nCount = xModule->GetMethods()->Count();
    Sequence< ::rtl::OUString > aSeqMethods( nCount );

    for ( USHORT i = 0; i < nCount; i++ )
    {
        SbMethod* pMethod = (SbMethod*)xModule->GetMethods()->Get( i );
        DBG_ASSERT( pMethod, "Method not found! (NULL)" );
        aSeqMethods.getArray()[ i ] = pMethod->GetName();
    }

    return aSeqMethods;
}

//----------------------------------------------------------------------------

Reference< container::XNameContainer > BasicIDE::GetModuleLibrary( SfxObjectShell* pShell, const String& rLibName, BOOL bLoadLibrary )
    throw(NoSuchElementException)
{
    // get library container
    Reference< script::XLibraryContainer > xLibContainer;
    if( pShell )
    {
        // document
        xLibContainer = Reference< script::XLibraryContainer >( pShell->GetBasicContainer(), UNO_QUERY );
    }
    else
    {
        // application
        xLibContainer = Reference< script::XLibraryContainer >( SFX_APP()->GetBasicContainer(), UNO_QUERY );
    }

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
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::GetModuleLibrary: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    // load library
    if( bLoadLibrary && !xLibContainer->isLibraryLoaded( aOULibName ) )
        xLibContainer->loadLibrary( aOULibName );

    return xLib;
}

//----------------------------------------------------------------------------

BOOL BasicIDE::HasModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName )
{
    BOOL bHasModule = FALSE;

    Reference< container::XNameContainer > xLib;
    try
    {
        // get library
        xLib = GetModuleLibrary( pShell, rLibName, TRUE );

        // check if library has module
        ::rtl::OUString aOUModName( rModName );
        if( xLib.is() && xLib->hasByName( aOUModName ) )
        {
            bHasModule = TRUE;
        }
    }
    catch ( container::NoSuchElementException& e )
    {
        ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
        DBG_ERROR( aBStr.GetBuffer() );
    }

    return bHasModule;
}

//----------------------------------------------------------------------------

::rtl::OUString BasicIDE::GetModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName )
    throw(NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetModuleLibrary( pShell, rLibName, TRUE );

    // get module
    ::rtl::OUString aOUSource;
    ::rtl::OUString aOUModName( rModName );
    if( xLib.is() && xLib->hasByName( aOUModName ) )
    {
        Any aElement = xLib->getByName( aOUModName );
        aElement >>= aOUSource;
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::GetModule: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    return aOUSource;
}

//----------------------------------------------------------------------------

/*
::rtl::OUString BasicIDE::FindModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName )
    throw(NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetModuleLibrary( pShell, rLibName, TRUE );

    // find module
    ::rtl::OUString aOUModSource;
    ::rtl::OUString aOUModName( rModName );
    if( xLib.is() && xLib->hasByName( aOUModName ) )
    {
        Any aElement = xLib->getByName( aOUModName );
        aElement >>= aOUModSource;
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::FindModule: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    return aOUModSource;
}
*/

//----------------------------------------------------------------------------

String BasicIDE::CreateModuleName( SfxObjectShell* pShell, const String& rLibName )
{
    String aModName;
    String aModStdName( RTL_CONSTASCII_USTRINGPARAM( "Module" ) );
    //String aModStdName( IDEResId( RID_STR_STDMODULENAME ) );
    BOOL bValid = FALSE;
    USHORT i = 1;
    while ( !bValid )
    {
        aModName = aModStdName;
        aModName += String::CreateFromInt32( i );
        if ( !BasicIDE::HasModule( pShell, rLibName, aModName ) )
            bValid = TRUE;

        i++;
    }

    return aModName;
}

//----------------------------------------------------------------------------

::rtl::OUString BasicIDE::CreateModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName, BOOL bCreateMain )
    throw(ElementExistException, NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetModuleLibrary( pShell, rLibName, TRUE );

    // create module
    ::rtl::OUString aOUSource;
    ::rtl::OUString aOUModName( rModName );
    if( xLib.is() && !xLib->hasByName( aOUModName ) )
    {
        // create new module
        aOUSource = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "REM  *****  BASIC  *****\n\n" ) );
        if ( bCreateMain )
            aOUSource += ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Sub Main\n\nEnd Sub" ) );

        // insert module into library
        Any aElement;
        aElement <<= aOUSource;
        xLib->insertByName( aOUModName, aElement );

        // doc shell modified
        BasicIDE::MarkDocShellModified( pShell );   // here?
    }
    else
    {
        throw ElementExistException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::CreateModule: ElementExistException!") ),
            Reference<XInterface>() );
    }

    return aOUSource;
}

//----------------------------------------------------------------------------

void BasicIDE::RenameModule( SfxObjectShell* pShell, const String& rLibName, const String& rOldName, const String& rNewName )
    throw(ElementExistException, NoSuchElementException)
{
    ::rtl::OUString aOUOldName( rOldName );
    ::rtl::OUString aOUNewName( rNewName );

    // get library
    Reference< container::XNameContainer > xLib = GetModuleLibrary( pShell, rLibName, TRUE );

    // rename module
    if( xLib.is() && xLib->hasByName( aOUOldName ) )
    {
        if ( xLib->hasByName( aOUNewName ) )
        {
            throw ElementExistException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::RenameModule: ElementExistException!") ),
                Reference<XInterface>() );
        }

        // get module
        Any aElement = xLib->getByName( aOUOldName );

        // remove module from module container
        xLib->removeByName( aOUOldName );

        // insert module by new name in module container
        xLib->insertByName( aOUNewName, aElement );
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::RenameModule: NoSuchElementException!") ),
            Reference<XInterface>() );
    }

    BasicIDEShell* pIDEShell = IDE_DLL()->GetShell();
    if ( pIDEShell )
    {
        IDEBaseWindow* pWin = pIDEShell->FindWindow( pShell, rLibName, rOldName, BASICIDE_TYPE_MODULE, FALSE );
        if ( pWin )
        {
            // set new name in module window
            ((ModulWindow*)pWin)->SetModName( rNewName );

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

void BasicIDE::RemoveModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName )
    throw(NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetModuleLibrary( pShell, rLibName, TRUE );

    // remove module
    ::rtl::OUString aOUModName( rModName );
    if( xLib.is() && xLib->hasByName( aOUModName ) )
    {
        xLib->removeByName( aOUModName );
    }
    else
    {
        throw NoSuchElementException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::RemoveModule: NoSuchElementException!") ),
            Reference<XInterface>() );
    }
}

//----------------------------------------------------------------------------

void BasicIDE::InsertModule( SfxObjectShell* pShell, const String& rLibName, const String& rModName, ::rtl::OUString& aModule )
    throw(ElementExistException, NoSuchElementException)
{
    // get library
    Reference< container::XNameContainer > xLib = GetModuleLibrary( pShell, rLibName, TRUE );

    // insert module into library
    ::rtl::OUString aOUModName( rModName );
    if( xLib.is() && !xLib->hasByName( aOUModName ) )
    {
        Any aElement;
        aElement <<= aModule;
        xLib->insertByName( aOUModName, aElement );
    }
    else
    {
        throw ElementExistException(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("BasicIDE::InsertModule: ElementExistException!") ),
            Reference<XInterface>() );
    }
}

//----------------------------------------------------------------------------
