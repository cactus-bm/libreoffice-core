/*************************************************************************
 *
 *  $RCSfile: basides3.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: tbe $ $Date: 2001-06-15 08:45:17 $
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

#define SI_NOCONTROL
#define SI_NOSBXCONTROLS

#include <basidesh.hrc>
#include <ide_pch.hxx>

#pragma hdrstop

#define _SOLAR__PRIVATE 1

#include <basidesh.hxx>
#include <baside3.hxx>
#include <basobj.hxx>

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINER_HPP_
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
using namespace ::com::sun::star::io;


DialogWindow* BasicIDEShell::CreateDlgWin( StarBASIC* pBasic, String aDlgName )
{
    bCreatingWindow = TRUE;

    ULONG nKey = 0;
    DialogWindow* pWin = 0;

    // Vielleicht gibt es ein suspendiertes?
    pWin = FindDlgWin( pBasic, aDlgName, FALSE, TRUE );

    if ( !pWin )
    {
        BasicManager* pBasMgr = BasicIDE::FindBasicManager( pBasic );
        if ( pBasMgr )
        {
            SfxObjectShell* pShell = BasicIDE::FindDocShell( pBasMgr );
            String aLibName = pBasic->GetName();

            if ( aDlgName.Len() == 0 )
                aDlgName = BasicIDE::CreateDialogName( pShell, aLibName );

            Reference< io::XInputStreamProvider > xISP( BasicIDE::FindDialog( pShell, aLibName, aDlgName ) );
            if ( !xISP.is() )
            {
                try
                {
                    xISP = BasicIDE::CreateDialog( pShell, aLibName, aDlgName );
                }
                catch ( container::ElementExistException& e )
                {
                    ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                    DBG_ERROR( aBStr.GetBuffer() );
                }
                catch ( container::NoSuchElementException& e )
                {
                    ByteString aBStr( String(e.Message), RTL_TEXTENCODING_ASCII_US );
                    DBG_ERROR( aBStr.GetBuffer() );
                }
            }

            if ( xISP.is() )
            {
                // create dialog model
                Reference< lang::XMultiServiceFactory > xMSF = getProcessServiceFactory();
                Reference< container::XNameContainer > xDialogModel( xMSF->createInstance
                    ( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialogModel" ) ) ), UNO_QUERY );
                Reference< XInputStream > xInput( xISP->createInputStream() );
                ::xmlscript::importDialogModel( xInput, xDialogModel );

                // new dialog window
                pWin = new DialogWindow( &GetViewFrame()->GetWindow(), pBasic, pShell, aLibName, aDlgName, xDialogModel );
                nKey = InsertWindowInTable( pWin );
            }
        }
    }
    else
    {
        pWin->SetStatus( pWin->GetStatus() & ~BASWIN_SUSPENDED );
        IDEBaseWindow* pTmp = aIDEWindowTable.First();
        while ( pTmp && !nKey )
        {
            if ( pTmp == pWin )
                nKey = aIDEWindowTable.GetCurKey();
            pTmp = aIDEWindowTable.Next();
        }
        DBG_ASSERT( nKey, "CreateDlgWin: Kein Key - Fenster nicht gefunden!" );
    }

    pWin->GrabScrollBars( &aHScrollBar, &aVScrollBar );
    pTabBar->InsertPage( (USHORT)nKey, aDlgName );
    if ( !pCurWin )
        SetCurWindow( pWin, FALSE, FALSE );

    bCreatingWindow = FALSE;
    return pWin;
}

DialogWindow* BasicIDEShell::FindDlgWin( StarBASIC* pBasic, const String& rDlgName, BOOL bCreateIfNotExist, BOOL bFindSuspended )
{
    DialogWindow* pDlgWin = 0;
    IDEBaseWindow* pWin = aIDEWindowTable.First();
    while ( pWin && !pDlgWin )
    {
        if ( ( !pWin->IsSuspended() || bFindSuspended ) && pWin->IsA( TYPE( DialogWindow ) ) )
        {
            String aDlgStr( ((DialogWindow*)pWin)->GetDialogName() );
            if ( !pBasic )  // nur irgendeins finden...
                pDlgWin = (DialogWindow*)pWin;
            else if ( ( pWin->GetBasic() == pBasic ) && ( aDlgStr == rDlgName ) )
                pDlgWin = (DialogWindow*)pWin;
        }
        pWin = aIDEWindowTable.Next();
    }
    if ( !pDlgWin && bCreateIfNotExist )
        pDlgWin = CreateDlgWin( pBasic, rDlgName );

    return pDlgWin;
}

SdrView* BasicIDEShell::GetCurDlgView()
{
    if ( !pCurWin || !pCurWin->IsA( TYPE( DialogWindow ) ) )
        return NULL;

    DialogWindow* pWin = (DialogWindow*)pCurWin;
    return pWin->GetView();
}

// Nur wenn Dialogfenster oben:
void __EXPORT BasicIDEShell::ExecuteDialog( SfxRequest& rReq )
{
    if ( !pCurWin || !pCurWin->IsA( TYPE( DialogWindow ) ) )
        return;

    pCurWin->ExecuteCommand( rReq );
}

