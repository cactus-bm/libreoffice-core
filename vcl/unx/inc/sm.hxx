/*************************************************************************
 *
 *  $RCSfile: sm.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: pl $ $Date: 2001-10-19 13:19:20 $
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
#ifndef _VCL_SM_HXX
#define _VCL_SM_HXX

#include <salunx.h>
#include <X11/SM/SMlib.h>

class ICEConnectionObserver
{
    static BOOL bIsWatching;

    static void ICEWatchProc( IceConn connection, IcePointer client_data,
                              Bool opening, IcePointer* watch_data );
public:

    // YieldProcs for SalDisplay
    static int Pending( int fd, void* data );
    static int Queued( int fd, void* data );
    static int HandleEvents( int fd, void* data );

    static void activate();
    static void deactivate();
};

class SessionManagerClient
{
    static SmcConn      aSmcConnection;
    static ByteString   aClientID;

    static void SaveYourselfProc(       SmcConn connection,
                                        SmPointer client_data,
                                        int save_type,
                                        Bool shutdown,
                                        int interact_style,
                                        Bool fast );
    static void DieProc(                SmcConn connection,
                                        SmPointer client_data );
    static void SaveCompleteProc(       SmcConn connection,
                                        SmPointer client_data );
    static void ShutdownCanceledProc(   SmcConn connection,
                                        SmPointer client_data );

    static const ByteString& getPreviousSessionID();
public:
    static void open();
    static void close();

    static String getExecName();
    static const ByteString&  getSessionID() { return aClientID; }
};

#endif
