/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: bastype3.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 00:27:33 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_basctl.hxx"


#include <ide_pch.hxx>

#define _SI_NOSBXCONTROLS

#include <basic/sbx.hxx>
#include <bastype2.hxx>
#include <basobj.hxx>
#include <baside2.hrc>
#include <iderid.hxx>
#include <bastypes.hxx>
#include <basdoc.hxx>

#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINER_HPP_
#include <com/sun/star/script/XLibraryContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINERPASSWORD_HPP_
#include <com/sun/star/script/XLibraryContainerPassword.hpp>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;


SV_DECL_VARARR( EntryArray, SvLBoxEntry*, 4, 4 );

SV_IMPL_VARARR( EntryArray, SvLBoxEntry*);


void __EXPORT BasicTreeListBox::RequestingChilds( SvLBoxEntry* pEntry )
{
    BasicEntryDescriptor aDesc( GetEntryDescriptor( pEntry ) );
    SfxObjectShell* pShell( aDesc.GetShell() );
    LibraryLocation eLocation( aDesc.GetLocation() );
    BasicEntryType eType( aDesc.GetType() );

    if ( eType == OBJ_TYPE_SHELL )
    {
        ImpCreateLibEntries( pEntry, pShell, eLocation );
    }
    else if ( eType == OBJ_TYPE_LIBRARY )
    {
        String aLibName( aDesc.GetLibName() );
        ::rtl::OUString aOULibName( aLibName );

        // check password
        BOOL bOK = TRUE;
        Reference< script::XLibraryContainer > xModLibContainer( BasicIDE::GetModuleLibraryContainer( pShell ), UNO_QUERY );
        if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
        {
            Reference< script::XLibraryContainerPassword > xPasswd( xModLibContainer, UNO_QUERY );
            if ( xPasswd.is() && xPasswd->isLibraryPasswordProtected( aOULibName ) && !xPasswd->isLibraryPasswordVerified( aOULibName ) )
            {
                String aPassword;
                bOK = QueryPassword( xModLibContainer, aLibName, aPassword );
            }
        }

        if ( bOK )
        {
            // load module library
            BOOL bModLibLoaded = FALSE;
            if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) )
            {
                if ( !xModLibContainer->isLibraryLoaded( aOULibName ) )
                {
                    EnterWait();
                    xModLibContainer->loadLibrary( aOULibName );
                    LeaveWait();
                }
                bModLibLoaded = xModLibContainer->isLibraryLoaded( aOULibName );
            }

            // load dialog library
            BOOL bDlgLibLoaded = FALSE;
            Reference< script::XLibraryContainer > xDlgLibContainer( BasicIDE::GetDialogLibraryContainer( pShell ), UNO_QUERY );
            if ( xDlgLibContainer.is() && xDlgLibContainer->hasByName( aOULibName ) )
            {
                if ( !xDlgLibContainer->isLibraryLoaded( aOULibName ) )
                {
                    EnterWait();
                    xDlgLibContainer->loadLibrary( aOULibName );
                    LeaveWait();
                }
                bDlgLibLoaded = xDlgLibContainer->isLibraryLoaded( aOULibName );
            }

            if ( bModLibLoaded || bDlgLibLoaded )
            {
                // create the sub entries
                ImpCreateLibSubEntries( pEntry, pShell, aLibName );

                // exchange image
                bool bDlgMode = ( nMode & BROWSEMODE_DIALOGS ) && !( nMode & BROWSEMODE_MODULES );
                Image aImage( IDEResId( bDlgMode ? RID_IMG_DLGLIB : RID_IMG_MODLIB ) );
                Image aImageHC( IDEResId( bDlgMode ? RID_IMG_DLGLIB_HC : RID_IMG_MODLIB_HC ) );
                SetEntryBitmaps( pEntry, aImage, aImageHC );
            }
            else
            {
                DBG_ERROR( "BasicTreeListBox::RequestingChilds: Error loading library!" );
            }
        }
    }
    else
        DBG_ERROR( "BasicTreeListBox::RequestingChilds: Unknown Type!" );
}

void __EXPORT BasicTreeListBox::ExpandedHdl()
{
    SvLBoxEntry* pEntry = GetHdlEntry();
    DBG_ASSERT( pEntry, "Was wurde zugeklappt?" );
    // Die OnDemand erzeugten Childs loeschen,
    // SubChilds werden automatisch geloescht.
    if ( !IsExpanded( pEntry ) && pEntry->HasChildsOnDemand() )
    {
        SvLBoxEntry* pChild = FirstChild( pEntry );
        while ( pChild )
        {
            GetModel()->Remove( pChild );   // Ruft auch den DTOR
            pChild = FirstChild( pEntry );
        }
    }
}

void BasicTreeListBox::ScanAllEntries()
{
    ScanEntry( 0, LIBRARY_LOCATION_USER );
    ScanEntry( 0, LIBRARY_LOCATION_SHARE );
    SfxObjectShell* pDocShell = SfxObjectShell::GetFirst();
    while ( pDocShell )
    {
        // only if there's a corresponding window (not for remote documents)
        if ( SfxViewFrame::GetFirst( pDocShell ) && !pDocShell->ISA( BasicDocShell ) && !pDocShell->IsInPrepareClose() )
            ScanEntry( pDocShell, LIBRARY_LOCATION_DOCUMENT );

        pDocShell = SfxObjectShell::GetNext( *pDocShell );
    }
}

void BasicTreeListBox::ExpandTree( SvLBoxEntry* pRootEntry )
{
    DBG_ASSERT( pRootEntry, "Keine Wurzel ?" );

    Expand( pRootEntry );

    SvLBoxEntry* pLibEntry = FirstChild( pRootEntry );
    while ( pLibEntry )
    {
        // Nur die mit Childs, sonst waere ChildsOnDemand ueberfluessig
        if ( !IsEntryProtected( pLibEntry ) && GetChildCount( pLibEntry ) )
            Expand( pLibEntry );

        pLibEntry = NextSibling( pLibEntry );
    }
}

void BasicTreeListBox::ExpandAllTrees()
{
    ULONG nRootPos = 0;
    SvLBoxEntry* pRootEntry = GetEntry( nRootPos );
    while ( pRootEntry )
    {
        ExpandTree( pRootEntry );
        pRootEntry = GetEntry( ++nRootPos );
    }
}

SbxVariable* BasicTreeListBox::FindVariable( SvLBoxEntry* pEntry )
{
    if ( !pEntry )
        return 0;

    String aLib, aModOrObj, aSubOrPropOrSObj, aPropOrSubInSObj;
    SfxObjectShell* pShell = 0;
    EntryArray aEntries;

    while ( pEntry )
    {
        USHORT nDepth = GetModel()->GetDepth( pEntry );
        switch ( nDepth )
        {
            case 4:
            case 3:
            case 2:
            case 1:
            {
                aEntries.C40_INSERT( SvLBoxEntry, pEntry, 0 );
            }
            break;
            case 0:
            {
                pShell = ((BasicShellEntry*)pEntry->GetUserData())->GetShell();
            }
            break;
        }
        pEntry = GetParent( pEntry );
    }

    SbxVariable* pVar = 0;
    if ( aEntries.Count() )
    {
        for ( USHORT n = 0; n < aEntries.Count(); n++ )
        {
            SvLBoxEntry* pLE = aEntries[n];
            DBG_ASSERT( pLE, "Entrie im Array nicht gefunden" );
            BasicEntry* pBE = (BasicEntry*)pLE->GetUserData();
            DBG_ASSERT( pBE, "Keine Daten im Eintrag gefunden!" );
            String aName( GetEntryText( pLE ) );

            switch ( pBE->GetType() )
            {
                case OBJ_TYPE_LIBRARY:
                {
                    BasicManager* pBasMgr = pShell ? pShell->GetBasicManager() : SFX_APP()->GetBasicManager();
                    if ( pBasMgr )
                        pVar = pBasMgr->GetLib( aName );
                }
                break;
                case OBJ_TYPE_MODULE:
                {
                    DBG_ASSERT( pVar && pVar->IsA( TYPE(StarBASIC) ), "FindVariable: Ungueltiges Basic" );
                    pVar = ((StarBASIC*)pVar)->FindModule( aName );
                }
                break;
                case OBJ_TYPE_METHOD:
                {
                    DBG_ASSERT( pVar && ( (pVar->IsA( TYPE(SbModule) )) || (pVar->IsA( TYPE(SbxObject) )) ), "FindVariable: Ungueltiges Modul/Objekt" );
                    pVar = ((SbxObject*)pVar)->GetMethods()->Find( aName, SbxCLASS_METHOD );
                }
                break;
                case OBJ_TYPE_DIALOG:
                {
                    // sbx dialogs removed
                }
                break;
                default:
                {
                    DBG_ERROR( "FindVariable: Unbekannter Typ!" );
                    pVar = 0;
                }
                break;
            }
            if ( !pVar )
                break;
        }
    }

    return pVar;
}

BasicEntryDescriptor BasicTreeListBox::GetEntryDescriptor( SvLBoxEntry* pEntry )
{
    SfxObjectShell* pShell = 0;
    LibraryLocation eLocation = LIBRARY_LOCATION_UNKNOWN;
    String aLibName;
    String aName;
    String aMethodName;
    BasicEntryType eType = OBJ_TYPE_UNKNOWN;

    if ( !pEntry )
        return BasicEntryDescriptor( pShell, eLocation, aLibName, aName, aMethodName, eType );

    EntryArray aEntries;

    while ( pEntry )
    {
        USHORT nDepth = GetModel()->GetDepth( pEntry );
        switch ( nDepth )
        {
            case 4:
            case 3:
            case 2:
            case 1:
            {
                aEntries.C40_INSERT( SvLBoxEntry, pEntry, 0 );
            }
            break;
            case 0:
            {
                BasicShellEntry* pBasicShellEntry = (BasicShellEntry*)pEntry->GetUserData();
                if ( pBasicShellEntry )
                {
                    pShell = pBasicShellEntry->GetShell();
                    eLocation = pBasicShellEntry->GetLocation();
                    eType = OBJ_TYPE_SHELL;
                }
            }
            break;
        }
        pEntry = GetParent( pEntry );
    }

    if ( aEntries.Count() )
    {
        for ( USHORT n = 0; n < aEntries.Count(); n++ )
        {
            SvLBoxEntry* pLE = aEntries[n];
            DBG_ASSERT( pLE, "Entrie im Array nicht gefunden" );
            BasicEntry* pBE = (BasicEntry*)pLE->GetUserData();
            DBG_ASSERT( pBE, "Keine Daten im Eintrag gefunden!" );

            switch ( pBE->GetType() )
            {
                case OBJ_TYPE_LIBRARY:
                {
                    aLibName = GetEntryText( pLE );
                    eType = pBE->GetType();
                }
                break;
                case OBJ_TYPE_MODULE:
                {
                    aName = GetEntryText( pLE );
                    eType = pBE->GetType();
                }
                break;
                case OBJ_TYPE_METHOD:
                {
                    aMethodName = GetEntryText( pLE );
                    eType = pBE->GetType();
                }
                break;
                case OBJ_TYPE_DIALOG:
                {
                    aName = GetEntryText( pLE );
                    eType = pBE->GetType();
                }
                break;
                default:
                {
                    DBG_ERROR( "GetEntryDescriptor: Unbekannter Typ!" );
                    eType = OBJ_TYPE_UNKNOWN;
                }
                break;
            }

            if ( eType == OBJ_TYPE_UNKNOWN )
                break;
        }
    }

    return BasicEntryDescriptor( pShell, eLocation, aLibName, aName, aMethodName, eType );
}

USHORT BasicTreeListBox::ConvertType( BasicEntryType eType )
{
    USHORT nType = OBJ_TYPE_UNKNOWN;

    switch ( eType )
    {
        case OBJ_TYPE_SHELL:
        {
            nType = BASICIDE_TYPE_SHELL;
        }
        break;
        case OBJ_TYPE_LIBRARY:
        {
            nType = BASICIDE_TYPE_LIBRARY;
        }
        break;
        case OBJ_TYPE_MODULE:
        {
            nType = BASICIDE_TYPE_MODULE;
        }
        break;
        case OBJ_TYPE_DIALOG:
        {
            nType = BASICIDE_TYPE_DIALOG;
        }
        break;
        case OBJ_TYPE_METHOD:
        {
            nType = BASICIDE_TYPE_METHOD;
        }
        break;
    }

    return nType;
}

bool BasicTreeListBox::IsValidEntry( SvLBoxEntry* pEntry )
{
    bool bIsValid = false;

    BasicEntryDescriptor aDesc( GetEntryDescriptor( pEntry ) );
    SfxObjectShell* pShell( aDesc.GetShell() );
    LibraryLocation eLocation( aDesc.GetLocation() );
    String aLibName( aDesc.GetLibName() );
    String aName( aDesc.GetName() );
    String aMethodName( aDesc.GetMethodName() );
    BasicEntryType eType( aDesc.GetType() );

    switch ( eType )
    {
        case OBJ_TYPE_SHELL:
        {
            bIsValid = ( !pShell || ( BasicIDE::HasShell( pShell ) &&
                                      GetRootEntryName( pShell, eLocation ) == GetEntryText( pEntry ) &&
                                      !pShell->IsInPrepareClose() ) );
        }
        break;
        case OBJ_TYPE_LIBRARY:
        {
            bIsValid = BasicIDE::HasModuleLibrary( pShell, aLibName ) || BasicIDE::HasDialogLibrary( pShell, aLibName );
        }
        break;
        case OBJ_TYPE_MODULE:
        {
            bIsValid = BasicIDE::HasModule( pShell, aLibName, aName );
        }
        break;
        case OBJ_TYPE_DIALOG:
        {
            bIsValid = BasicIDE::HasDialog( pShell, aLibName, aName );
        }
        break;
        case OBJ_TYPE_METHOD:
        {
            bIsValid = BasicIDE::HasMethod( pShell, aLibName, aName, aMethodName );
        }
        break;
    }

    return bIsValid;
}

SbModule* BasicTreeListBox::FindModule( SvLBoxEntry* pEntry )
{
    SbxVariable* pVar = FindVariable( pEntry );
    if ( pVar && pVar->IsA( TYPE(SbModule ) ) )
        return (SbModule*)pVar;
    return 0;
}

SvLBoxEntry* BasicTreeListBox::FindRootEntry( SfxObjectShell* pShell, LibraryLocation eLocation )
{
    ULONG nRootPos = 0;
    SvLBoxEntry* pRootEntry = GetEntry( nRootPos );
    while ( pRootEntry )
    {
        DBG_ASSERT( (((BasicEntry*)pRootEntry->GetUserData())->GetType() == OBJ_TYPE_SHELL ), "Kein Shelleintrag?" );
        BasicShellEntry* pBasicShellEntry = (BasicShellEntry*)pRootEntry->GetUserData();
        if ( pBasicShellEntry && pBasicShellEntry->GetShell() == pShell && pBasicShellEntry->GetLocation() == eLocation )
            return pRootEntry;
        pRootEntry = GetEntry( ++nRootPos );
    }
    return 0;
}

String CreateMgrAndLibStr( const String& rMgrName, const String& rLibName )
{
    String aName( '[' );
    aName += rMgrName;
    aName += String( RTL_CONSTASCII_USTRINGPARAM( "]." ) );
    aName += rLibName;
    return aName;
}
