/*************************************************************************
 *
 *  $RCSfile: sfxhelp.hxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: rt $ $Date: 2005-01-11 14:57:03 $
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
#ifndef _SFX_HELP_HXX
#define _SFX_HELP_HXX

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif

#ifndef INCLUDED_SFX2_DLLAPI_H
#include "sfx2/dllapi.h"
#endif

#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif
#ifndef _HELP_HXX //autogen
#include <vcl/help.hxx>
#endif

#include <tools/string.hxx>

class SfxHelp_Impl;
class SfxFrame;
class SFX2_DLLPUBLIC SfxHelp : public Help
{
    String          aTicket;        // for Plugins
    String          aUser;
    String          aLanguageStr;
    String          aCountryStr;
    sal_Bool        bIsDebug;
    SfxHelp_Impl*   pImp;

private:
    SAL_DLLPRIVATE virtual BOOL Start( ULONG nHelpId, const Window* pWindow );
    SAL_DLLPRIVATE virtual BOOL Start( const String& rURL, const Window* pWindow );

    SAL_DLLPRIVATE String GetHelpModuleName_Impl();
    SAL_DLLPRIVATE String CreateHelpURL_Impl( ULONG nHelpId, const String& rModuleName );
    SAL_DLLPRIVATE String CreateHelpURL_Impl( const String& aCommandURL, const String& rModuleName );

public:

                    SfxHelp();
                    ~SfxHelp();
    void            SetTicket( const String& rTicket )
                    { aTicket = rTicket;}
    void            SetUser( const String& rUser )
                    { aUser = rUser;}

    virtual XubString   GetHelpText( ULONG nHelpId, const Window* pWindow );
    virtual XubString   GetHelpText( const String&, const Window* pWindow );

    static String       CreateHelpURL( ULONG nHelpId, const String& rModuleName );
    static String       CreateHelpURL( const String& aCommandURL, const String& rModuleName );
    static void         OpenHelpAgent( SfxFrame* pFrame, ULONG nHelpId );
    static String       GetDefaultHelpModule();
};

#endif // #ifndef _SFX_HELP_HXX

