/*************************************************************************
 *
 *  $RCSfile: sbcomp.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-15 16:35:10 $
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

#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#pragma hdrstop
#include <svtools/sbx.hxx>
#include "sbcomp.hxx"
#include "image.hxx"


// For debugging only
// #define DBG_SAVE_DISASSEMBLY

#ifdef DBG_SAVE_DISASSEMBLY
static bool dbg_bDisassemble = true;

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess3.hpp>
#include <com/sun/star/io/XTextOutputStream.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>

using namespace comphelper;
using namespace rtl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::ucb;
using namespace com::sun::star::io;

void dbg_SaveDisassembly( SbModule* pModule )
{
    bool bDisassemble = dbg_bDisassemble;
    if( bDisassemble )
    {
        Reference< XSimpleFileAccess3 > xSFI;
        Reference< XTextOutputStream > xTextOut;
        Reference< XOutputStream > xOut;
        Reference< XMultiServiceFactory > xSMgr = getProcessServiceFactory();
        if( xSMgr.is() )
        {
            Reference< XSimpleFileAccess3 > xSFI = Reference< XSimpleFileAccess3 >( xSMgr->createInstance
                ( OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ), UNO_QUERY );
            if( xSFI.is() )
            {
                String aFile( RTL_CONSTASCII_USTRINGPARAM("file:///d:/BasicAsm_") );
                StarBASIC* pBasic = (StarBASIC*)pModule->GetParent();
                if( pBasic )
                {
                    aFile += pBasic->GetName();
                    aFile.AppendAscii( "_" );
                }
                aFile += pModule->GetName();
                aFile.AppendAscii( ".txt" );

                // String aFile( RTL_CONSTASCII_USTRINGPARAM("file:///d:/BasicAsm.txt") );
                if( xSFI->exists( aFile ) )
                    xSFI->kill( aFile );
                xOut = xSFI->openFileWrite( aFile );
                Reference< XInterface > x = xSMgr->createInstance( OUString::createFromAscii( "com.sun.star.io.TextOutputStream" ) );
                Reference< XActiveDataSource > xADS( x, UNO_QUERY );
                xADS->setOutputStream( xOut );
                xTextOut = Reference< XTextOutputStream >( x, UNO_QUERY );
            }
        }

        if( xTextOut.is() )
        {
            String aDisassemblyStr;
            pModule->Disassemble( aDisassemblyStr );
            xTextOut->writeString( aDisassemblyStr );
        }
        xOut->closeOutput();
    }
}
#endif

// Diese Routine ist hier definiert, damit der Compiler als eigenes Segment
// geladen werden kann.

BOOL SbModule::Compile()
{
    if( pImage )
        return TRUE;
    StarBASIC* pBasic = PTR_CAST(StarBASIC,GetParent());
    if( !pBasic )
        return FALSE;
    SbxBase::ResetError();
    // Aktuelles Modul!
    SbModule* pOld = pCMOD;
    pCMOD = this;

    SbiParser* pParser = new SbiParser( (StarBASIC*) GetParent(), this );
    while( pParser->Parse() ) {}
    if( !pParser->GetErrors() )
        pParser->aGen.Save();
    delete pParser;
    // fuer den Disassembler
    if( pImage )
        pImage->aOUSource = aOUSource;

    pCMOD = pOld;

    // Beim Compilieren eines Moduls werden die Modul-globalen
    // Variablen aller Module ungueltig
    BOOL bRet = IsCompiled();
    if( bRet )
    {
        pBasic->ClearAllModuleVars();

        SbxObject* pParent = pBasic->GetParent();
        if( pParent )
            pBasic = PTR_CAST(StarBASIC,pParent);
        if( pBasic )
            pBasic->ClearAllModuleVars();
    }

#ifdef DBG_SAVE_DISASSEMBLY
    dbg_SaveDisassembly( this );
#endif

    return bRet;
}

/**************************************************************************
*
*   Syntax-Highlighting
*
**************************************************************************/

void StarBASIC::Highlight( const String& rSrc, SbTextPortions& rList )
{
    SbiTokenizer aTok( rSrc );
    aTok.Hilite( rList );
}

