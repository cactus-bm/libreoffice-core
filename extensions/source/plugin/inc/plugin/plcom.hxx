/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: plcom.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:53:35 $
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
#ifndef __PLUGIN_INC_PLCOM_HXX
#define __PLUGIN_INC_PLCOM_HXX

#include <tools/string.hxx>
#include <list>

class PluginComm
{
protected:
    int                         m_nRefCount;
    ::rtl::OString              m_aLibName;
    std::list< String >         m_aFilesToDelete;
public:
    PluginComm( const ::rtl::OString& rLibName );
    virtual ~PluginComm();

    int getRefCount() { return m_nRefCount; }
    void addRef() { m_nRefCount++; }
    void decRef() { m_nRefCount--; if( ! m_nRefCount ) delete this; }

    const ::rtl::OString& getLibName() { return m_aLibName; }
    void setLibName( const ::rtl::OString& rName ) { m_aLibName = rName; }

    void addFileToDelete( const String& filename )
        { m_aFilesToDelete.push_back( filename ); }

    virtual NPError NPP_Destroy( NPP instance, NPSavedData** save ) = 0;
    virtual NPError NPP_DestroyStream( NPP instance, NPStream* stream,
                               NPError reason ) = 0;
    virtual void* NPP_GetJavaClass() = 0;
    virtual NPError NPP_Initialize() = 0;
    virtual NPError NPP_New( NPMIMEType pluginType, NPP instance,
                             uint16 mode, int16 argc,
                             char* argn[], char* argv[],
                             NPSavedData *saved ) = 0;
    virtual NPError NPP_NewStream( NPP instance, NPMIMEType type,
                                   NPStream* stream,
                                   NPBool seekable, uint16* stype ) = 0;
    virtual void NPP_Print( NPP instance, NPPrint* platformPrint ) = 0;
    virtual NPError NPP_SetWindow( NPP instance, NPWindow* window ) = 0;
    virtual void NPP_Shutdown() = 0;
    virtual void NPP_StreamAsFile( NPP instance, NPStream* stream,
                                   const char* fname ) = 0;
    virtual void NPP_URLNotify( NPP instance, const char* url,
                                NPReason reason, void* notifyData ) = 0;
    virtual int32 NPP_Write( NPP instance, NPStream* stream, int32 offset,
                             int32 len, void* buffer ) = 0;
    virtual int32 NPP_WriteReady( NPP instance, NPStream* stream ) = 0;
    virtual NPError NPP_GetValue( NPP instance, NPPVariable  variable, void* value ) = 0;
    virtual NPError NPP_SetValue( NPP instance, NPNVariable variable,
                                 void *value) = 0;
};

#endif
