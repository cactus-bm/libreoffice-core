/*************************************************************************
 *
 *  $RCSfile: cmddlg.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: pl $ $Date: 2001-05-11 16:14:08 $
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

#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _PAD_RTSETUP_HRC_
#include <rtsetup.hrc>
#endif
#ifndef _PAD_COMMANDDLG_HXX_
#include <cmddlg.hxx>
#endif
#ifndef _PAD_PADIALOG_HXX_
#include <padialog.hxx>
#endif
#ifndef _PAD_HELPER_HXX_
#include <helper.hxx>
#endif
#ifndef _SVT_FILEDLG_HXX
#include <filedlg.hxx>
#endif

#ifndef _PAD_PRTSETUP_HXX_
#include <prtsetup.hxx>
#endif

using namespace psp;
using namespace rtl;
using namespace padmin;

#define PRINTER_PERSISTENCE_GROUP "KnownPrinterCommands"
#define FAX_PERSISTENCE_GROUP "KnownFaxCommands"
#define PDF_PERSISTENCE_GROUP "KnowPdfCommands"
#define MAX_COMMANDS 50

void CommandStore::getSystemPrintCommands( ::std::list< String >& rCommands )
{
    static ::std::list< OUString > aSysCommands;
    static bool bOnce = false;
    if( ! bOnce )
    {
        bOnce = true;
        PrinterInfoManager::get().getSystemPrintCommands( aSysCommands );
    }

    ::std::list< OUString >::const_iterator it;
    for( it = aSysCommands.begin(); it != aSysCommands.end(); ++it )
        rCommands.push_back( *it );
}

void CommandStore::getSystemPdfCommands( ::std::list< String >& rCommands )
{
    static bool bOnce = false;
    static ::std::list< String > aSysCommands;

    if( ! bOnce )
    {
        bOnce = true;
        char pBuffer[1024];
        FILE* pPipe;
        String aCommand;
        rtl_TextEncoding aEncoding = gsl_getSystemTextEncoding();

        pPipe = popen( "which gs", "r" );
        if( pPipe )
        {
            fgets( pBuffer, sizeof( pBuffer ), pPipe );
            int nLen = strlen( pBuffer );
            if( nLen > 1 && strncmp( pBuffer, "no gs in", 8 ) )
            {
                pBuffer[ nLen-1 ] = 0;
                aCommand = String( ByteString( pBuffer ), aEncoding );
                aCommand.AppendAscii( " -q -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=\"(OUTFILE)\" -" );
                aSysCommands.push_back( aCommand );
            }
            pclose( pPipe );
        }

        pPipe = popen( "which distill", "r" );
        if( pPipe )
        {
            fgets( pBuffer, sizeof( pBuffer ), pPipe );
            int nLen = strlen( pBuffer );
            if( nLen > 1 && strncmp( pBuffer, "no distill in", 13 ) )
            {
                pBuffer[ nLen-1 ] = 0;
                aCommand = String( ByteString( pBuffer ), aEncoding );
                aCommand.AppendAscii( " (TMP) ; mv `echo (TMP) | sed s/\\.ps$/.pdf/` (OUTFILE)" );
                aSysCommands.push_back( aCommand );
            }
            pclose( pPipe );
        }
    }
    ::std::list< String >::const_iterator it;
    for( it = aSysCommands.begin(); it != aSysCommands.end(); ++it )
        rCommands.push_back( *it );
}



void CommandStore::getStoredCommands( const char* pGroup, ::std::list< String >& rCommands )
{
    Config& rConfig( getPadminRC() );
    rConfig.SetGroup( pGroup );
    int nKeys = rConfig.GetKeyCount();
    ::std::list< String >::const_iterator it;
    while( nKeys-- )
    {
        String aCommand( rConfig.ReadKey( ByteString::CreateFromInt32( nKeys ), RTL_TEXTENCODING_UTF8 ) );
        for( it = rCommands.begin(); it != rCommands.end() && *it != aCommand; ++it )
            ;
        if( it == rCommands.end() )
            rCommands.push_back( aCommand );
    }
}

void CommandStore::setCommands(
                               const char* pGroup,
                               const ::std::list< String >& rCommands,
                               const ::std::list< String >& rSysCommands
                               )
{
    Config& rConfig( getPadminRC() );
    rConfig.DeleteGroup( pGroup );
    rConfig.SetGroup( pGroup );
    ::std::list< String >::const_iterator it, loop;
    ::std::list< String > aWriteList;

    int nWritten = 0;
    for( it = rCommands.begin(); it != rCommands.end(); ++it )
    {
        for( loop = rSysCommands.begin(); loop != rSysCommands.end() && *loop != *it; ++loop )
            ;
        if( loop == rSysCommands.end() )
        {
            aWriteList.push_back( *it );
            nWritten++;
        }
    }
    while( nWritten > MAX_COMMANDS )
    {
        aWriteList.pop_front();
        nWritten--;
    }
    for( nWritten = 0, it = aWriteList.begin(); it != aWriteList.end(); ++it, ++nWritten )
        rConfig.WriteKey( ByteString::CreateFromInt32( nWritten ), ByteString( *it, RTL_TEXTENCODING_UTF8 ) );
}


void CommandStore::getPrintCommands( ::std::list< String >& rCommands )
{
    rCommands.clear();
    getSystemPrintCommands( rCommands );
    getStoredCommands( PRINTER_PERSISTENCE_GROUP, rCommands );
}

void CommandStore::getPdfCommands( ::std::list< String >& rCommands )
{
    rCommands.clear();
    getSystemPdfCommands( rCommands );
    getStoredCommands( PDF_PERSISTENCE_GROUP, rCommands );
}

void CommandStore::getFaxCommands( ::std::list< String >& rCommands )
{
    rCommands.clear();
    getStoredCommands( FAX_PERSISTENCE_GROUP, rCommands );
}

void CommandStore::setPrintCommands( const ::std::list< String >& rCommands )
{
    ::std::list< String > aSysCmds;
    getSystemPrintCommands( aSysCmds );
    setCommands( PRINTER_PERSISTENCE_GROUP, rCommands, aSysCmds );
}

void CommandStore::setPdfCommands( const ::std::list< String >& rCommands )
{
    ::std::list< String > aSysCmds;
    getSystemPdfCommands( aSysCmds );
    setCommands( PDF_PERSISTENCE_GROUP, rCommands, aSysCmds );
}

void CommandStore::setFaxCommands( const ::std::list< String >& rCommands )
{
    ::std::list< String > aSysCmds;
    setCommands( FAX_PERSISTENCE_GROUP, rCommands, aSysCmds );
}


RTSCommandPage::RTSCommandPage( RTSDialog* pParent ) :
        TabPage( &pParent->m_aTabControl, PaResId( RID_RTS_COMMANDPAGE ) ),
        m_pParent( pParent ),
        m_aCommandTitle( this, PaResId( RID_RTS_CMD_GROUP_INSTALL ) ),
        m_aPrinterName( this, PaResId( RID_RTS_CMD_TXT_PRTNAME ) ),
        m_aPrinterGroup( this, PaResId( RID_RTS_CMD_GROUP_DEFAULT ) ),
        m_aCommandsCB( this, PaResId( RID_RTS_CMD_CB_COMMANDS ) ),
        m_aConfigureText( this, PaResId( RID_RTS_CMD_TXT_CONFIGURE ) ),
        m_aConfigureBox( this, PaResId( RID_RTS_CMD_LB_CONFIGURE ) ),
        m_aHelpButton( this, PaResId( RID_RTS_CMD_BTN_HELP ) ),
        m_aRemovePB( this, PaResId( RID_RTS_CMD_BTN_REMOVE ) ),
        m_aConnectedTo( this, PaResId( RID_RTS_CMD_TXT_CONNECT ) ),
        m_aFaxHelp( PaResId( RID_RTS_CMD_STR_FAXHELP ) ),
        m_aPrinterHelp( PaResId( RID_RTS_CMD_STR_PRINTERHELP ) ),
        m_aPdfHelp( PaResId( RID_RTS_CMD_STR_PDFHELP ) ),
        m_aPdfDirectoryButton( this, PaResId( RID_RTS_CMD_BTN_PDFDIR ) ),
        m_aPdfDirectoryEdit( this, PaResId( RID_RTS_CMD_EDT_PDFDIR ) ),
        m_aPdfDirectoryText( this, PaResId( RID_RTS_CMD_TXT_PDFDIR ) ),
        m_aFaxSwallowBox( this, PaResId( RID_RTS_CMD_BOX_SWALLOWFAXNO ) )
{
    m_nPrinterEntry = m_aConfigureBox.InsertEntry( String( PaResId( RID_RTS_CMD_STR_CONFIGURE_PRINTER ) ) );
    m_nFaxEntry = m_aConfigureBox.InsertEntry( String( PaResId( RID_RTS_CMD_STR_CONFIGURE_FAX ) ) );
    m_nPdfEntry = m_aConfigureBox.InsertEntry( String( PaResId( RID_RTS_CMD_STR_CONFIGURE_PDF ) ) );

    FreeResource();

    CommandStore::getPrintCommands( m_aPrinterCommands );
    CommandStore::getFaxCommands( m_aFaxCommands );
    CommandStore::getPdfCommands( m_aPdfCommands );

    m_aPrinterName.SetText( m_pParent->m_aPrinter );

    m_aCommandsCB.SetDoubleClickHdl( LINK( this, RTSCommandPage, DoubleClickHdl ) );
    m_aCommandsCB.SetSelectHdl( LINK( this, RTSCommandPage, SelectHdl ) );
    m_aCommandsCB.SetModifyHdl( LINK( this, RTSCommandPage, ModifyHdl ) );
    m_aConfigureBox.SetSelectHdl( LINK( this, RTSCommandPage, SelectHdl ) );
    m_aHelpButton.SetClickHdl( LINK( this, RTSCommandPage, ClickBtnHdl ) );
    m_aRemovePB.SetClickHdl( LINK( this, RTSCommandPage, ClickBtnHdl ) );
    m_aPdfDirectoryButton.SetClickHdl( LINK( this, RTSCommandPage, ClickBtnHdl ) );

    m_aPdfDirectoryButton.Show( FALSE );
    m_aPdfDirectoryEdit.Show( FALSE );
    m_aPdfDirectoryText.Show( FALSE );
    m_aFaxSwallowBox.Show( FALSE );
    m_aCommandsCB.SetText( m_pParent->m_aJobData.m_aCommand );

    m_bWasFax = false;
    m_bWasPdf = false;
    m_aConfigureBox.SelectEntryPos( m_nPrinterEntry );
    sal_Int32 nIndex = 0;
    while( nIndex != -1 )
    {
        OUString aToken( m_pParent->m_aJobData.m_aFeatures.getToken( 0, ',', nIndex ) );
        if( ! aToken.compareToAscii( "fax", 3 ) )
        {
            m_bWasFax = true;
            m_aFaxSwallowBox.Show( TRUE );
            sal_Int32 nPos = 0;
            m_aFaxSwallowBox.Check( ! aToken.getToken( 1, '=', nPos ).compareToAscii( "swallow", 7 ) ? TRUE : FALSE );
            m_aConfigureBox.SelectEntryPos( m_nFaxEntry );
            break;
        }
        if( ! aToken.compareToAscii( "pdf=", 4 ) )
        {
            m_bWasPdf = true;
            sal_Int32 nPos = 0;
            m_aPdfDirectoryEdit.SetText( aToken.getToken( 1, '=', nPos ) );
            m_aPdfDirectoryEdit.Show( TRUE );
            m_aPdfDirectoryButton.Show( TRUE );
            m_aPdfDirectoryText.Show( TRUE );
            m_aConfigureBox.SelectEntryPos( m_nPdfEntry );
            break;
        }
    }

    String aString( m_aConnectedTo.GetText() );
    aString += String( m_pParent->m_aJobData.m_aCommand );
    m_aConnectedTo.SetText( aString );

    UpdateCommands();
}

RTSCommandPage::~RTSCommandPage()
{
}

void RTSCommandPage::save()
{
    String aCommand;
    bool bHaveFax = m_aConfigureBox.GetSelectEntryPos() == m_nFaxEntry ? true : false;
    bool bHavePdf = m_aConfigureBox.GetSelectEntryPos() == m_nPdfEntry ? true : false;
    ::std::list< String >::iterator it;

    String aFeatures;
    sal_Int32 nIndex = 0;
    String aOldPdfPath;
    bool bOldFaxSwallow = false;
    bool bFaxSwallow = m_aFaxSwallowBox.IsChecked() ? true : false;
    while( nIndex != -1 )
    {
        OUString aToken( m_pParent->m_aJobData.m_aFeatures.getToken( 0, ',', nIndex ) );
        if( aToken.compareToAscii( "fax", 3 ) &&
            aToken.compareToAscii( "pdf", 3 ) )
        {
            if( aToken.getLength() )
            {
                if( aFeatures.Len() )
                    aFeatures += ',';
                aFeatures += String( aToken );
            }
        }
        else if( ! aToken.compareToAscii( "pdf=", 4 ) )
        {
            sal_Int32 nPos = 0;
            aOldPdfPath = aToken.getToken( 1, '=', nPos );
        }
        else if( ! aToken.compareToAscii( "fax=", 4 ) )
        {
            sal_Int32 nPos = 0;
            bOldFaxSwallow = aToken.getToken( 1, '=', nPos ).compareToAscii( "swallow", 7 ) ? false : true;
        }
    }
    ::std::list< String >* pList = &m_aPrinterCommands;
    if( bHaveFax )
    {
        if( aFeatures.Len() )
            aFeatures += ',';
        aFeatures.AppendAscii( "fax=" );
        if( bFaxSwallow )
            aFeatures.AppendAscii( "swallow" );
        pList = &m_aFaxCommands;
    }
    if( bHavePdf )
    {
        if( aFeatures.Len() )
            aFeatures += ',';
        aFeatures.AppendAscii( "pdf=" );
        aFeatures.Append( m_aPdfDirectoryEdit.GetText() );
        pList = &m_aPdfCommands;
    }
    aCommand = m_aCommandsCB.GetText();
    for( it = pList->begin(); it != pList->end() && *it != aCommand; ++it )
        ;
    if( it == pList->end() )
        pList->push_back( aCommand );

    if( aCommand != String( m_pParent->m_aJobData.m_aCommand )      ||
        ( m_bWasFax && ! bHaveFax )                                 ||
        ( ! m_bWasFax && bHaveFax )                                 ||
        ( m_bWasPdf && ! bHavePdf )                                 ||
        ( ! m_bWasPdf && bHavePdf )                                 ||
        ( bHavePdf && aOldPdfPath != m_aPdfDirectoryEdit.GetText() )||
        ( bHaveFax && bFaxSwallow != bOldFaxSwallow )
        )
    {
        m_pParent->m_aJobData.m_aCommand    = aCommand;
        m_pParent->m_aJobData.m_aFeatures   = aFeatures;

        PrinterInfoManager::get().changePrinterInfo( m_pParent->m_aPrinter, m_pParent->m_aJobData );
    }
    CommandStore::setPrintCommands( m_aPrinterCommands );
    CommandStore::setFaxCommands( m_aFaxCommands );
    CommandStore::setPdfCommands( m_aPdfCommands );
}


IMPL_LINK( RTSCommandPage, SelectHdl, Control*, pBox )
{
    if( pBox == &m_aConfigureBox )
    {
        BOOL bEnable = m_aConfigureBox.GetSelectEntryPos() == m_nPdfEntry ? TRUE : FALSE;
        m_aPdfDirectoryButton.Show( bEnable );
        m_aPdfDirectoryEdit.Show( bEnable );
        m_aPdfDirectoryText.Show( bEnable );
        bEnable = m_aConfigureBox.GetSelectEntryPos() == m_nFaxEntry ? TRUE : FALSE;
        m_aFaxSwallowBox.Show( bEnable );
        UpdateCommands();
    }
    else if( pBox == &m_aCommandsCB )
    {
        m_aRemovePB.Enable( TRUE );
    }

    return 0;
}

IMPL_LINK( RTSCommandPage, ClickBtnHdl, Button*, pButton )
{
    if( pButton == & m_aPdfDirectoryButton )
    {
        PathDialog aDialog( this );
        aDialog.SetPath( m_aPdfDirectoryEdit.GetText() );
        if( aDialog.Execute() )
            m_aPdfDirectoryEdit.SetText( aDialog.GetPath() );
    }
    else if( pButton == &m_aRemovePB )
    {
        String aEntry( m_aCommandsCB.GetText() );
        ::std::list< String >* pList;
        if( m_aConfigureBox.GetSelectEntryPos() == m_nPrinterEntry )
            pList = &m_aPrinterCommands;
        else if( m_aConfigureBox.GetSelectEntryPos() == m_nFaxEntry )
            pList = &m_aFaxCommands;
        else
            pList = &m_aPdfCommands;

        pList->remove( aEntry );
        m_aCommandsCB.RemoveEntry( aEntry );
    }
    else if( pButton == &m_aHelpButton )
    {
        String aHelpText;
        if( m_aConfigureBox.GetSelectEntryPos() == m_nPrinterEntry )
            aHelpText = m_aPrinterHelp;
        else if( m_aConfigureBox.GetSelectEntryPos() == m_nFaxEntry )
            aHelpText = m_aFaxHelp;
        else if( m_aConfigureBox.GetSelectEntryPos() == m_nPdfEntry )
            aHelpText = m_aPdfHelp;

        InfoBox aBox( this, aHelpText );
        aBox.Execute();
    }
    return 0;
}

IMPL_LINK( RTSCommandPage, DoubleClickHdl, ComboBox*, pComboBox )
{
    if( pComboBox == &m_aCommandsCB )
        ConnectCommand();
    return 0;
}

IMPL_LINK( RTSCommandPage, ModifyHdl, Edit*, pEdit )
{
    if( pEdit == &m_aCommandsCB )
        m_aRemovePB.Enable( m_aCommandsCB.GetEntryPos( m_aCommandsCB.GetText() ) != LISTBOX_ENTRY_NOTFOUND );

    return 0;
}

void RTSCommandPage::UpdateCommands()
{
    m_aCommandsCB.Clear();
    ::std::list< String >::iterator it;
    if( m_aConfigureBox.GetSelectEntryPos() == m_nPrinterEntry )
    {
        for( it = m_aPrinterCommands.begin(); it != m_aPrinterCommands.end(); ++it )
            m_aCommandsCB.InsertEntry( *it );
        if( ! m_bWasFax )
            m_aCommandsCB.SetText( m_pParent->m_aJobData.m_aCommand );
        else
            m_aCommandsCB.SetText( String() );
    }
    else if( m_aConfigureBox.GetSelectEntryPos() == m_nFaxEntry )
    {
        for( it = m_aFaxCommands.begin(); it != m_aFaxCommands.end(); ++it )
            m_aCommandsCB.InsertEntry( *it );
        if( m_bWasFax )
            m_aCommandsCB.SetText( m_pParent->m_aJobData.m_aCommand );
        else
            m_aCommandsCB.SetText( String() );
    }
    else if( m_aConfigureBox.GetSelectEntryPos() == m_nPdfEntry )
    {
        for( it = m_aPdfCommands.begin(); it != m_aPdfCommands.end(); ++it )
            m_aCommandsCB.InsertEntry( *it );
        if( m_bWasPdf )
            m_aCommandsCB.SetText( m_pParent->m_aJobData.m_aCommand );
        else
            m_aCommandsCB.SetText( String() );
    }
}

void RTSCommandPage::ConnectCommand()
{
    String aString( m_aConnectedTo.GetText().GetToken( 0, ':' ) );
    aString.AppendAscii( ": " );
    aString += m_aCommandsCB.GetText();

    m_aConnectedTo.SetText( aString );
}
