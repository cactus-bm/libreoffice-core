/*************************************************************************
 *
 *  $RCSfile: tcommuni.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-20 12:26:23 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2002
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2002 by Sun Microsystems, Inc.
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include <svtools/communi.hxx>
#include <tools/link.hxx>
#include <svtools/svarray.hxx>

#include <vos/thread.hxx>

#ifndef _STRING_LIST
DECLARE_LIST( StringList, String * );
#define _STRING_LIST
#endif

#ifndef _PROCESS_HXX
#include <basic/process.hxx>
#endif

class Process;

class CommunicationManagerClientViaSocketTT : public CommunicationManagerClientViaSocket
{
public:
    CommunicationManagerClientViaSocketTT();

    virtual BOOL StartCommunication();
    virtual BOOL StartCommunication( String aApp, String aParams, Environment *pChildEnv );

    BOOL KillApplication();

protected:
    virtual BOOL RetryConnect();
    BOOL bApplicationStarted;
    Time aFirstRetryCall;
    String aAppPath;
    String aAppParams;
    Environment aAppEnv;
    Process *pProcess;
};


String GetHostConfig();
ULONG GetTTPortConfig();
ULONG GetUnoPortConfig();

