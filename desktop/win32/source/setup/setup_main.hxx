/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: setup_main.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:55:10 $
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


#ifndef SETUP_MAIN_HXX
#define SETUP_MAIN_HXX

class SetupApp
{
    UINT            m_uiRet;
    DWORD           m_nOSVersion;
    DWORD           m_nMinorVersion;
    boolean         m_bIsWin9x      : 1;
    boolean         m_bNeedReboot   : 1;

public:
                    SetupApp();
    virtual        ~SetupApp();

    virtual boolean Initialize( HINSTANCE hInst ) = NULL;
    virtual boolean AlreadyRunning() const = NULL;
    virtual boolean ReadProfile() = NULL;
    virtual boolean ChooseLanguage( long& rLanguage ) = NULL;
    virtual boolean CheckVersion() = NULL;
    virtual boolean Install( long nLanguage ) = NULL;

    virtual void    DisplayError( UINT nErr ) const = NULL;

    UINT            GetError() const { return m_uiRet; }
    void            SetError( UINT nErr ) { m_uiRet = nErr; }
    boolean         IsWin9x() const { return m_bIsWin9x; }
    DWORD           GetOSVersion() const { return m_nOSVersion; }
    DWORD           GetMinorVersion() const { return m_nMinorVersion; }

    void            SetRebootNeeded( boolean bNeedReboot ) { m_bNeedReboot = bNeedReboot; }
    boolean         NeedReboot() const { return m_bNeedReboot; }
};

SetupApp* Create_SetupAppA();
SetupApp* Create_SetupAppW();

#endif
