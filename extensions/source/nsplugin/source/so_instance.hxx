/*************************************************************************
 *
 *  $RCSfile: so_instance.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-20 10:08:32 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef __SO_INSTANCE_HXX__
#define __SO_INSTANCE_HXX__

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XACTIVEDATASOURCE_HPP_
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif

#ifndef _COM_SUN_STAR_BRIDGE_XUNOURLRESOLVER_HPP_
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_XNAMINGSERVICE_HPP_
#include <com/sun/star/uno/XNamingService.hpp>
#endif

#ifndef _COM_SUN_STAR_CONNECTION_CONNECTIONSETUPEXCEPTION_HPP_
#include <com/sun/star/connection/ConnectionSetupException.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif

#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <rtl/ustring.hxx>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XDispatchHelper.hpp>

#include "ns_debug.hxx"

typedef sal_Int32 NSP_HWND;


class SoPluginInstance
{

private:
    // Service manager of remote Soffice
    static ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxRemoteMSF;
    // Service manager of local Soffice
    static ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxLocalMSF;
    // Dir where Soffice is in, ie. /Soffice7/program
    static char sSO_Dir[NPP_PATH_MAX];
    int m_nWidth;
    int m_nHeight;
    int m_nX;
    int m_nY;
    int m_nFlag;            // Set to 12 during initialization
    ::rtl::OUString m_sURL; // URL of the document to be loaded
    sal_Bool m_bInit;       // If the Plugin instance is initilaized.
    NSP_HWND m_hParent;       // Windows handle of parent window
    long m_pParent;       // ID of this instance - get from NPP
    // StarOffice window
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > m_xUnoWin;
    // StarOffice component window
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > m_xComponent;
    // StarOffice frame for this Plugin
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > m_xFrame;
    // Frames of StarOffice
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrames > m_xFrames;
    // Dispatcher of frame
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchHelper > m_xDispatcher;
    // DispatchProvider of frame
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > m_xDispatchProvider;

    int LoadDocument(NSP_HWND hParent);
    sal_Bool Connect(void);

    long m_dParentStyl;       // Old Windows style of parent window


public:
    SoPluginInstance(long iInstance);
    virtual ~SoPluginInstance(void);
    virtual sal_Bool SetURL(char* aURL);
    virtual sal_Bool IsInit(void){return m_bInit;};
    virtual sal_Bool SetWindow(NSP_HWND hParent, int x, int y, int w, int h) ;
    virtual sal_Bool Destroy(void) ;
    virtual sal_Bool Print(void) ;

    static sal_Bool ShutDown(void);
    static sal_Bool SetSODir(char * sDir);
    static char* GetSODir(void) {return sSO_Dir;};
    void SetPosSize(int x, int y, int w, int h);
    void Setflag(int n);
    long GetParent(void) {return m_pParent;};
};

#endif
