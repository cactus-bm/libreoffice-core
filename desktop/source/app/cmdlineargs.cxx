/*************************************************************************
 *
 *  $RCSfile: cmdlineargs.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: ghiggins $ $Date: 2002-06-18 09:12:12 $
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

#ifndef _DESKTOP_COMMANDLINEARGS_HXX_
#include <cmdlineargs.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _RTL_URI_HXX_
#include <rtl/uri.hxx>
#endif

namespace desktop
{

CommandLineArgs::CommandLineArgs()
{
    ResetParamValues();
}

// intialize class with command line parameters from process environment
CommandLineArgs::CommandLineArgs( const ::vos::OExtCommandLine& aExtCmdLine )
{
    ResetParamValues();
    ParseCommandLine_Impl( aExtCmdLine );
}

// intialize class with command line parameters from interprocess communication (officeipcthread)
CommandLineArgs::CommandLineArgs( const ::rtl::OUString& aCmdLineArgs )
{
    ResetParamValues();
    ParseCommandLine_String( aCmdLineArgs );
}

// ----------------------------------------------------------------------------

void CommandLineArgs::ParseCommandLine_Impl( const ::vos::OExtCommandLine& aExtCmdLine )
{
    ::vos::OExtCommandLine aCmdLine;

    sal_uInt32      nCount = aCmdLine.getCommandArgCount();
    ::rtl::OUString aDummy;
    String          aArguments;

    // Extract cmdline parameters and concat them to the cmdline string format
    for( sal_uInt32 i=0; i < nCount; i++ )
    {
        aCmdLine.getCommandArg( i, aDummy );
        aArguments += String( aDummy );
        aArguments += '|';
    }

    // Parse string as a cmdline string
    ParseCommandLine_String( ::rtl::OUString( aArguments ));
}

void CommandLineArgs::ParseCommandLine_String( const ::rtl::OUString& aCmdLineString )
{
    // parse command line arguments
    sal_Bool    bPrintEvent     = sal_False;
    sal_Bool    bOpenEvent      = sal_True;
    sal_Bool    bPrintToEvent   = sal_False;
    sal_Bool    bPrinterName    = sal_False;
    sal_Bool    bForceOpenEvent = sal_False;
    sal_Bool    bForceNewEvent  = sal_False;

    sal_Int32 nIndex = 0;
    do
    {
        ::rtl::OUString aArg    = aCmdLineString.getToken( 0, '|', nIndex );
        String          aArgStr = aArg;

        if ( aArg.getLength() > 0 )
        {
            if ( !InterpretCommandLineParameter( aArg ))
            {
                if ( aArgStr.GetChar(0) == '-' )
                {
                    // handle this argument as an option
                    if ( aArgStr.EqualsIgnoreCaseAscii( "-n" ))
                    {
                        // force new documents based on the following documents
                        bForceNewEvent  = sal_True;
                        bOpenEvent      = sal_False;
                        bForceOpenEvent = sal_False;
                        bPrintToEvent   = sal_False;
                        bPrintEvent     = sal_False;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-o" ))
                    {
                        // force open documents regards if they are templates or not
                        bForceOpenEvent = sal_True;
                        bOpenEvent      = sal_False;
                        bForceNewEvent  = sal_False;
                        bPrintToEvent   = sal_False;
                        bPrintEvent     = sal_False;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-pt" ))
                    {
                        // Print to special printer
                        bPrintToEvent   = sal_True;
                        bPrinterName    = sal_True;
                        bPrintEvent     = sal_False;
                        bOpenEvent      = sal_False;
                        bForceNewEvent  = sal_False;
                        bForceOpenEvent = sal_False;
                    }
                    else if ( aArgStr.EqualsIgnoreCaseAscii( "-p" ))
                    {
                        // Print to default printer
                        bPrintEvent     = sal_True;
                        bPrintToEvent   = sal_False;
                        bOpenEvent      = sal_False;
                        bForceNewEvent  = sal_False;
                        bForceOpenEvent = sal_False;
                    }
                }
                else
                {
                    if ( bPrinterName && bPrintToEvent )
                    {
                        // first argument after "-pt" this must be the printer name
                        m_aPrinterName = aArgStr;
                        m_bPrinterName = sal_True;
                        bPrinterName = sal_False;
                    }
                    else
                    {
                        // handle this argument as a filename
                        if ( bOpenEvent )
                        {
                            // Open Event anhaengen
                            if ( m_aOpenList.getLength() )
                                m_aOpenList += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
                            m_aOpenList += aArgStr;
                            m_bOpenList = sal_True;
                        }
                        else if ( bPrintEvent )
                        {
                            // Print Event anhaengen
                            if( m_aPrintList.getLength() )
                                m_aPrintList += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
                            m_aPrintList += aArgStr;
                            m_bPrintList = sal_True;
                        }
                        else if ( bPrintToEvent )
                        {
                            if ( m_aPrintToList.getLength() )
                                m_aPrintToList += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
                            m_aPrintToList += aArgStr;
                            m_bPrintToList = sal_True;
                        }
                        else if ( bForceNewEvent )
                        {
                            if ( m_aForceNewList.getLength() )
                                m_aForceNewList += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
                            m_aForceNewList += aArgStr;
                            m_bForceNewList = sal_True;
                        }
                        else if ( bForceOpenEvent )
                        {
                            if ( m_aForceOpenList.getLength() )
                                m_aForceOpenList += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
                            m_aForceOpenList += aArgStr;
                            m_bForceOpenList = sal_True;
                        }
                    }
                }
            }
        }
    }
    while ( nIndex >= 0 );
}

sal_Bool CommandLineArgs::InterpretCommandLineParameter( const ::rtl::OUString& aArg )
{
    String aArgStr( aArg );

    if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-minimized" )) == sal_True )
    {
        m_bMinimized = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-invisible" )) == sal_True )
    {
        m_bInvisible = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-norestore" )) == sal_True )
    {
        m_bNoRestore = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-bean" )) == sal_True )
    {
        m_bBean = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-plugin" )) == sal_True )
    {
        m_bPlugin = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-server" )) == sal_True )
    {
        m_bServer = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-headless" )) == sal_True )
    {
        m_bHeadless = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-quickstart" )) == sal_True )
    {
        m_bQuickstart = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-terminate_after_init" )) == sal_True )
    {
        m_bTerminateAfterInit = sal_True;
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 8).EqualsIgnoreCaseAscii( "-accept=" ))
    {
        m_aAcceptString = aArgStr.Copy( 8 );
        m_bAcceptString = sal_True;
        return sal_True;
    }
    else if ( aArgStr.CompareIgnoreCaseToAscii( "-portal," ,
                                                RTL_CONSTASCII_LENGTH( "-portal," )) == COMPARE_EQUAL )
    {
        m_aPortalConnectString = aArgStr.Copy( RTL_CONSTASCII_LENGTH( "-portal," ));
        m_bPortalConnectString = sal_True;
        return sal_True;
    }
    else if ( aArgStr.Copy( 0, 7 ).EqualsIgnoreCaseAscii( "-userid" ))
    {
        ::rtl::OUString aUserId = aArgStr;

        sal_Int32 nPos = aUserId.indexOf( '[' );
        sal_Int32 nEndpos = aUserId.lastIndexOf( ']' );
        if( nPos != -1 && nEndpos != -1 )
        {
            m_aUserDir = ::rtl::Uri::decode(
                aUserId.copy( nPos + 1, nEndpos - nPos - 1 ),
                rtl_UriDecodeWithCharset,
                RTL_TEXTENCODING_UTF8 );
            m_bUserDir = sal_True;
        }
        return sal_True;
    }
    else if ( aArgStr.Copy( 0, 15).EqualsIgnoreCaseAscii( "-clientdisplay=" ))
    {
        m_aClientDisplay = aArgStr.Copy( 15 );
        m_bClientDisplay = sal_True;
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 9).EqualsIgnoreCaseAscii( "-version=" ))
    {
        m_aVersionString = aArgStr.Copy( 9 );
        m_bVersionString = sal_True;
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-master" )) == sal_True )
    {
        m_bMaster = sal_True;
        return sal_True;
    }

    return sal_False;
}

void CommandLineArgs::ResetParamValues()
{
    m_bMaster               = sal_False;
    m_bMinimized            = sal_False;
    m_bInvisible            = sal_False;
    m_bNoRestore            = sal_False;
    m_bBean                 = sal_False;
    m_bPlugin               = sal_False;
    m_bHeadless             = sal_False;
    m_bServer               = sal_False;
    m_bQuickstart           = sal_False;
    m_bOpenList             = sal_False;
    m_bPrintList            = sal_False;
    m_bPortalConnectString  = sal_False;
    m_bAcceptString         = sal_False;
    m_bUserDir              = sal_False;
    m_bClientDisplay        = sal_False;
    m_bTerminateAfterInit   = sal_False;
    m_bVersionString        = sal_False;
    m_bPrintToList          = sal_False;
    m_bPrinterName          = sal_False;
    m_bForceOpenList        = sal_False;
    m_bForceNewList         = sal_False;
}

} // namespace desktop
