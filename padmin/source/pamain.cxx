/*************************************************************************
 *
 *  $RCSfile: pamain.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: pl $ $Date: 2001-05-08 11:56:36 $
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

#include <stdio.h>
#include <unistd.h>

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_WRKWIN_HXX
#include <vcl/wrkwin.hxx>
#endif
#ifndef _VCL_UNOWRAP_HXX
#include <vcl/unowrap.hxx>
#endif
#ifndef _PAD_PADIALOG_HXX_
#include <padialog.hxx>
#endif
#ifndef _PAD_HELPER_HXX_
#include <helper.hxx>
#endif

#ifndef _CPPUHELPER_SERVICEFACTORY_HXX_
#include <cppuhelper/servicefactory.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

using namespace padmin;
using namespace rtl;
using namespace cppu;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace comphelper;

// -----------------------------------------------------------------------

class MyApp : public Application
{
public:
    void            Main();
    virtual USHORT  Exception( USHORT nError );
};

MyApp aMyApp;


// -----------------------------------------------------------------------

USHORT MyApp::Exception( USHORT nError )
{
    switch( nError & EXC_MAJORTYPE )
    {
        case EXC_RSCNOTLOADED:
            Abort( String::CreateFromAscii( "Error: could not load language resources.\nPlease check your installation.\n" ) );
            break;
    }
    return 0;
}

void MyApp::Main()
{
    PADialog* pPADialog;

    //-------------------------------------------------
    // create the global service-manager
    //-------------------------------------------------

    OUString aReadRdbName( Application::GetAppFileName() );
    int nLastSlash = aReadRdbName.lastIndexOf( '/' );
    if( nLastSlash >= 0 )
        aReadRdbName = aReadRdbName.copy( 0, nLastSlash+1 );
    else
        aReadRdbName = OUString();
    aReadRdbName += OUString::createFromAscii( "applicat.rdb" );

    OUString aWriteRdbName( OUString::createFromAscii( "/tmp/padmin" ) );
    aWriteRdbName += OUString::valueOf( (sal_Int32)getpid() );
    aWriteRdbName += OUString::createFromAscii( ".rdb" );

#ifdef DEBUG
    printf( "using write registry %s\n", OUStringToOString( aWriteRdbName, RTL_TEXTENCODING_ISO_8859_1 ).getStr() );
    printf( "using read registry %s\n", OUStringToOString( aReadRdbName, RTL_TEXTENCODING_ISO_8859_1 ).getStr() );
#endif
    Reference< XMultiServiceFactory > xFactory( createRegistryServiceFactory( aWriteRdbName, aReadRdbName ) );
    if( xFactory.is() )
        setProcessServiceFactory( xFactory );
#ifdef DEBUG
    else
        fprintf( stderr, "could not create service factory\n" );
#endif

    pPADialog = PADialog::Create( NULL , FALSE );
    Application::SetDisplayName( pPADialog->GetText() );
    pPADialog->Execute();
    delete pPADialog;

    OString aTmp( OUStringToOString( aWriteRdbName, RTL_TEXTENCODING_ISO_8859_1 ) );
    unlink( aTmp.getStr() );
}
