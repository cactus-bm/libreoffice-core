/*************************************************************************
 *
 *  $RCSfile: cmdlineargs.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: cd $ $Date: 2002-10-15 08:12:30 $
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

void CommandLineArgs::AddStringListParam_Impl( StringParam eParam, const rtl::OUString& aParam )
{
    OSL_ASSERT( eParam >= 0 && eParam < CMD_STRINGPARAM_COUNT );
    if ( m_aStrParams[eParam].getLength() )
        m_aStrParams[eParam] += ::rtl::OUString::valueOf( (sal_Unicode)APPEVENT_PARAM_DELIMITER );
    m_aStrParams[eParam] += aParam;
    m_aStrSetParams[eParam] = sal_True;
}

void CommandLineArgs::SetBoolParam_Impl( BoolParam eParam, sal_Bool bValue )
{
    OSL_ASSERT( eParam >= 0 && eParam < CMD_BOOLPARAM_COUNT );
    m_aBoolParams[eParam] = bValue;
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
                        AddStringListParam_Impl( CMD_STRINGPARAM_PRINTERNAME, aArgStr );
                        bPrinterName = sal_False;
                    }
                    else
                    {
                        // handle this argument as a filename
                        if ( bOpenEvent )
                            AddStringListParam_Impl( CMD_STRINGPARAM_OPENLIST, aArgStr );
                        else if ( bPrintEvent )
                            AddStringListParam_Impl( CMD_STRINGPARAM_PRINTLIST, aArgStr );
                        else if ( bPrintToEvent )
                            AddStringListParam_Impl( CMD_STRINGPARAM_PRINTTOLIST, aArgStr );
                        else if ( bForceNewEvent )
                            AddStringListParam_Impl( CMD_STRINGPARAM_FORCENEWLIST, aArgStr );
                        else if ( bForceOpenEvent )
                            AddStringListParam_Impl( CMD_STRINGPARAM_FORCEOPENLIST, aArgStr );
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
        SetBoolParam_Impl( CMD_BOOLPARAM_MINIMIZED, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-invisible" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_INVISIBLE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-norestore" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NORESTORE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-bean" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_BEAN, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-plugin" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_PLUGIN, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-server" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_SERVER, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-headless" )) == sal_True )
    {
        // Headless means also invisibile, so set this parameter to true!
        SetBoolParam_Impl( CMD_BOOLPARAM_HEADLESS, sal_True );
        SetBoolParam_Impl( CMD_BOOLPARAM_INVISIBLE, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-quickstart" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_QUICKSTART, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-terminate_after_init" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_TERMINATEAFTERINIT, sal_True );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-nologo" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_NOLOGO, sal_True );
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 8).EqualsIgnoreCaseAscii( "-accept=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_ACCEPT, aArgStr.Copy( 8 ) );
        return sal_True;
    }
    else if ( aArgStr.CompareIgnoreCaseToAscii( "-portal," ,
                                                RTL_CONSTASCII_LENGTH( "-portal," )) == COMPARE_EQUAL )
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_PORTAL, aArgStr.Copy( RTL_CONSTASCII_LENGTH( "-portal," )) );
        return sal_True;
    }
    else if ( aArgStr.Copy( 0, 7 ).EqualsIgnoreCaseAscii( "-userid" ))
    {
        ::rtl::OUString aUserId = aArgStr;

        sal_Int32 nPos = aUserId.indexOf( '[' );
        sal_Int32 nEndpos = aUserId.lastIndexOf( ']' );
        if( nPos != -1 && nEndpos != -1 )
        {
            AddStringListParam_Impl(
                CMD_STRINGPARAM_USERDIR,
                ::rtl::Uri::decode( aUserId.copy( nPos + 1, nEndpos - nPos - 1 ),
                                    rtl_UriDecodeWithCharset,
                                    RTL_TEXTENCODING_UTF8 ) );
        }
        return sal_True;
    }
    else if ( aArgStr.Copy( 0, 15).EqualsIgnoreCaseAscii( "-clientdisplay=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_CLIENTDISPLAY, aArgStr.Copy( 15 ) );
        return sal_True;
    }
    else if ( aArgStr.Copy(0, 9).EqualsIgnoreCaseAscii( "-version=" ))
    {
        AddStringListParam_Impl( CMD_STRINGPARAM_VERSION, aArgStr.Copy( 15 ) );
        return sal_True;
    }
    else if ( aArg.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "-master" )) == sal_True )
    {
        SetBoolParam_Impl( CMD_BOOLPARAM_MASTER, sal_True );
        return sal_True;
    }

    return sal_False;
}

void CommandLineArgs::ResetParamValues()
{
    for ( int i = 0; i < CMD_BOOLPARAM_COUNT; i++ )
        m_aBoolParams[i] = sal_False;
    for ( i = 0; i < CMD_STRINGPARAM_COUNT; i++ )
        m_aStrSetParams[i] = sal_False;
}

sal_Bool CommandLineArgs::GetBoolParam( BoolParam eParam ) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );

    OSL_ASSERT( ( eParam >= 0 && eParam < CMD_BOOLPARAM_COUNT ) );
    return m_aBoolParams[eParam];
}

void CommandLineArgs::SetBoolParam( BoolParam eParam, sal_Bool bNewValue )
{
    osl::MutexGuard  aMutexGuard( m_aMutex );

    OSL_ASSERT( ( eParam >= 0 && eParam < CMD_BOOLPARAM_COUNT ) );
    m_aBoolParams[eParam] = bNewValue;
}

const rtl::OUString& CommandLineArgs::GetStringParam( BoolParam eParam ) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );

    OSL_ASSERT( ( eParam >= 0 && eParam < CMD_STRINGPARAM_COUNT ) );
    return m_aStrParams[eParam];
}

void CommandLineArgs::SetStringParam( BoolParam eParam, const rtl::OUString& aNewValue )
{
    osl::MutexGuard  aMutexGuard( m_aMutex );

    OSL_ASSERT( ( eParam >= 0 && eParam < CMD_STRINGPARAM_COUNT ) );
    m_aStrParams[eParam] = aNewValue;
}

sal_Bool CommandLineArgs::IsMaster() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_MASTER ];
}

sal_Bool CommandLineArgs::IsMinimized() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_MINIMIZED ];
}

sal_Bool CommandLineArgs::IsInvisible() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_INVISIBLE ];
}

sal_Bool CommandLineArgs::IsNoRestore() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_NORESTORE ];
}

sal_Bool CommandLineArgs::IsBean() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_BEAN ];
}

sal_Bool CommandLineArgs::IsPlugin() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_PLUGIN ];
}

sal_Bool CommandLineArgs::IsServer() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_SERVER ];
}

sal_Bool CommandLineArgs::IsHeadless() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_HEADLESS ];
}

sal_Bool CommandLineArgs::IsQuickstart() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_QUICKSTART ];
}

sal_Bool CommandLineArgs::IsTerminateAfterInit() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_TERMINATEAFTERINIT ];
}

sal_Bool CommandLineArgs::IsNoLogo() const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    return m_aBoolParams[ CMD_BOOLPARAM_NOLOGO ];
}

sal_Bool CommandLineArgs::GetPortalConnectString( ::rtl::OUString& rPara ) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_PORTAL ];
    return m_aStrSetParams[ CMD_STRINGPARAM_PORTAL ];
}

sal_Bool CommandLineArgs::GetAcceptString( ::rtl::OUString& rPara ) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_ACCEPT ];
    return m_aStrSetParams[ CMD_STRINGPARAM_ACCEPT ];
}

sal_Bool CommandLineArgs::GetUserDir( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_USERDIR ];
    return m_aStrSetParams[ CMD_STRINGPARAM_USERDIR ];
}

sal_Bool CommandLineArgs::GetClientDisplay( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_CLIENTDISPLAY ];
    return m_aStrSetParams[ CMD_STRINGPARAM_CLIENTDISPLAY ];
}

sal_Bool CommandLineArgs::GetOpenList( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_OPENLIST ];
    return m_aStrSetParams[ CMD_STRINGPARAM_OPENLIST ];
}

sal_Bool CommandLineArgs::GetForceOpenList( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_FORCEOPENLIST ];
    return m_aStrSetParams[ CMD_STRINGPARAM_FORCEOPENLIST ];
}

sal_Bool CommandLineArgs::GetForceNewList( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_FORCENEWLIST ];
    return m_aStrSetParams[ CMD_STRINGPARAM_FORCENEWLIST ];
}

sal_Bool CommandLineArgs::GetPrintList( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_PRINTLIST ];
    return m_aStrSetParams[ CMD_STRINGPARAM_PRINTLIST ];
}

sal_Bool CommandLineArgs::GetVersionString( ::rtl::OUString& rPara) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_VERSION ];
    return m_aStrSetParams[ CMD_STRINGPARAM_VERSION ];
}

sal_Bool CommandLineArgs::GetPrintToList( ::rtl::OUString& rPara ) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_PRINTTOLIST ];
    return m_aStrSetParams[ CMD_STRINGPARAM_PRINTTOLIST ];
}

sal_Bool CommandLineArgs::GetPrinterName( ::rtl::OUString& rPara ) const
{
    osl::MutexGuard  aMutexGuard( m_aMutex );
    rPara = m_aStrParams[ CMD_STRINGPARAM_PRINTERNAME ];
    return m_aStrSetParams[ CMD_STRINGPARAM_PRINTERNAME ];
}

} // namespace desktop
