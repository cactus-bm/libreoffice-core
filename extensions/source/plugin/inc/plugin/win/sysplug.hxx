/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sysplug.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2007-03-26 13:09:58 $
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

#ifndef __PLUGIN_INC_WINPLUG_HXX
#define __PLUGIN_INC_WINPLUG_HXX

#include <tools/prewin.h>

#include <windows.h>
#include <tchar.h>
#include <winbase.h>

#include <tools/postwin.h>

#include <list>
#include <map>
#include <algorithm>

#pragma pack( push, 8 )
#include <npapi.h>
#include <npupp.h>
#pragma pack( pop )

#include <plugin/plcom.hxx>
#include <vcl/threadex.hxx>

//==================================================================================================
class PluginComm_Impl :
    public PluginComm,
    public ::vcl::SolarThreadExecutor

{
    enum CallType {
        eNPP_Destroy,
        eNPP_DestroyStream,
        eNPP_GetJavaClass,
        eNPP_Initialize,
        eNPP_New,
        eNPP_NewStream,
        eNPP_Print,
        eNPP_SetWindow,
        eNPP_Shutdown,
        eNPP_StreamAsFile,
        eNPP_URLNotify,
        eNPP_Write,
        eNPP_WriteReady,
        eNPP_GetValue,
        eNPP_SetValue,
        eNP_Initialize
    };

    void*               m_aArgs[ 8 ];
    CallType            m_eCall;

    virtual long        doIt();
public:
                        PluginComm_Impl( const rtl::OUString& rMIME, const rtl::OUString& rName, HWND hWnd );
    virtual             ~PluginComm_Impl();

public:
    virtual NPError     NPP_Destroy( NPP instance, NPSavedData** save );
    virtual NPError     NPP_DestroyStream( NPP instance, NPStream* stream, NPError reason );
    virtual void *      NPP_GetJavaClass();
    virtual NPError     NPP_Initialize();
    virtual NPError     NPP_New( NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc,
                                 char* argn[], char* argv[], NPSavedData *saved );
    virtual NPError     NPP_NewStream( NPP instance, NPMIMEType type, NPStream* stream,
                                       NPBool seekable, uint16* stype );
    virtual void        NPP_Print( NPP instance, NPPrint* platformPrint );
    virtual NPError     NPP_SetWindow( NPP instance, NPWindow* window );
    virtual void        NPP_Shutdown();
    virtual void        NPP_StreamAsFile( NPP instance, NPStream* stream, const char* fname );
    virtual void        NPP_URLNotify( NPP instance, const char* url,
                                       NPReason reason, void* notifyData );
    virtual int32       NPP_Write( NPP instance, NPStream* stream, int32 offset,
                                   int32 len, void* buffer );
    virtual int32       NPP_WriteReady( NPP instance, NPStream* stream );
    virtual NPError     NPP_GetValue( NPP instance, NPPVariable variable, void *ret_alue );
    virtual NPError     NPP_SetValue( NPP instance, NPNVariable variable, void *ret_alue );

private:
    BOOL                retrieveFunction( TCHAR* pName, void** ppFunc ) const;

private:
    HINSTANCE           _plDLL;

    NPPluginFuncs       _NPPfuncs;
};


#endif


