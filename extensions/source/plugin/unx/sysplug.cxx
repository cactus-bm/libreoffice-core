/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sysplug.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 13:10:58 $
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
#include "precompiled_extensions.hxx"
#include <cstdarg>

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <osl/thread.h>

#include <plugin/impl.hxx>

int UnxPluginComm::nConnCounter = 0;

UnxPluginComm::UnxPluginComm(
                             const String& mimetype,
                             const String& library,
                             XLIB_Window aParent,
                             int nDescriptor1,
                             int nDescriptor2
                             ) :
        PluginComm( ::rtl::OUStringToOString( library, osl_getThreadTextEncoding() ) ),
        PluginConnector( nDescriptor2 )
{
    char pDesc[32];
    char pWindow[32];
    sprintf( pWindow, "%d", aParent );
    sprintf( pDesc, "%d", nDescriptor1 );
    ByteString aLib( library, osl_getThreadTextEncoding() );

    char* pArgs[5];
    pArgs[0] = "pluginapp.bin";
    pArgs[1] = pDesc;
    pArgs[2] = const_cast<char*>(aLib.GetBuffer());
    pArgs[3] = pWindow;
    pArgs[4] = NULL;

#if OSL_DEBUG_LEVEL > 1
    m_nCommPID = 10;
    fprintf( stderr, "Try to launch: %s %s %s %s, descriptors are %d, %d\n", pArgs[0], pArgs[1], pArgs[2], pArgs[3], nDescriptor1, nDescriptor2 );
#endif

    if( ! ( m_nCommPID = fork() ) )
      {
         execvp( pArgs[0], pArgs );
          fprintf( stderr, "Error: could not exec %s\n", pArgs[0] );
          _exit(255);
      }

    if( m_nCommPID != -1 )
    {
        // wait for pluginapp.bin to start up
        if( ! WaitForMessage( 5000 ) )
        {
            fprintf( stderr, "Timeout on command: %s %s %s %s\n", pArgs[0], pArgs[1], pArgs[2], pArgs[3] );
            invalidate();
        }
        else
        {
            MediatorMessage* pMessage = GetNextMessage( TRUE );
            Respond( pMessage->m_nID,
                     "init ack",8,
                     NULL );
            delete pMessage;
            NPP_Initialize();
        }
    }
}

UnxPluginComm::~UnxPluginComm()
{
    NPP_Shutdown();
    if( m_nCommPID != -1 && m_nCommPID != 0 )
    {
        int status = 16777216;
        pid_t nExit = waitpid( m_nCommPID, &status, WUNTRACED );
#if OSL_DEBUG_LEVEL > 1
        fprintf( stderr, "child %d (plugin app child %d) exited with status %d\n", nExit, m_nCommPID, WEXITSTATUS(status) );
#endif
    }
}
