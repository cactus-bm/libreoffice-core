/*************************************************************************
 *
 *  $RCSfile: npshell.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2004-09-02 14:08:39 $
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

#ifdef UNIX

#define XP_UNIX 1

#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#endif //end of UNIX

#ifdef WNT

#define _WINDOWS

#include <windows.h>
#include <direct.h>
#include <stdlib.h>
#include <io.h>
#include <sys/types.h>
#include <fcntl.h>


#endif //end of WNT


#include <stdio.h>

#include <string.h>
#include <errno.h>


#include "npapi.h"
#include "npshell.hxx"
#include "so_env.hxx"
#include "so_msg.hxx"
#include "ns_debug.hxx"


#include "nsp_func.hxx"

/***********************************************************************
 *
 * Implementations of plugin API functions
 *
 ***********************************************************************/

static NSP_Mute_Obj send_lock = NSP_New_Mute_Obj();
static NSP_PIPE_FD write_fd = 0;


long int NSP_WriteToPipe(NSP_PIPE_FD fp, void* buf, unsigned long int len)
{
    unsigned long int len_unix = 0, len_wnt = 0;

    len_unix = NSP_Write_Pipe(fp, buf, len, &len_wnt);
#ifdef UNIX
    return  len_unix;
#endif //end of UNIX
#ifdef WNT
    return  len_wnt;
#endif //end of WNT

}

// chdir to staroffice's program dir, start nspluin executable in child process, and use pipe to talk with it
int do_init_pipe()
{
    debug_fprintf(NSP_LOG_APPEND, "enter do_init_pipe 1\n");
    NSP_CHDIR(findProgramDir());
    NSP_PIPE_FD fd[2];

    if( 0 != NSP_Inherited_Pipe(fd) )
        return NPERR_GENERIC_ERROR;

    write_fd = fd[1];   // write fd
#ifdef UNIX
    int nChildPID = fork();

    if( ! nChildPID )  // child process
#endif //end of UNIX
    {
        char s_read_fd[16] = {0};
        char s_write_fd[16] = {0};
        sprintf(s_read_fd,  "%d", fd[0]);
        sprintf(s_write_fd, "%d", fd[1]);
        // transfer pipe fds to nsplugin process
        debug_fprintf(NSP_LOG_APPEND, "try to start plugin exe: %s %s %s.\n", findNsExeFile(), s_read_fd, s_write_fd);

#ifdef UNIX
        execl(findNsExeFile(), findNsExeFile(), s_read_fd, s_write_fd, NULL);
        _exit(255);
#endif //end of UNIX
#ifdef WNT
        char sArgc[NPP_BUFFER_SIZE];
        sprintf(sArgc, "%s %s", s_read_fd, s_write_fd);
        STARTUPINFO NSP_StarInfo;
        memset((void*) &NSP_StarInfo, 0, sizeof(STARTUPINFO));
        NSP_StarInfo.cb = sizeof(STARTUPINFO);
        PROCESS_INFORMATION NSP_ProcessInfo;
        memset((void*)&NSP_ProcessInfo, 0, sizeof(PROCESS_INFORMATION));
        if(!CreateProcess(findNsExeFile(), sArgc, NULL, NULL, TRUE,
                      CREATE_NO_WINDOW, NULL, NULL, &NSP_StarInfo, &NSP_ProcessInfo))
        {
            DWORD Err = GetLastError();
        }
#endif //end of WNT
    }
    NSP_Close_Pipe(fd[0]);
    return NPERR_NO_ERROR;
}

int sendMsg( PLUGIN_MSG* pMsg, unsigned int len, int iEnsure)
{
    NSP_Lock_Mute_Obj(send_lock);
    long int len_w = 0;

    debug_fprintf(NSP_LOG_APPEND, "try to send message type:%d; len: %d\n", pMsg->msg_id, len);
    /*
    debug_fprintf(NSP_LOG_APPEND, "NSPlugin Message: msg_id:%d; instance_id:%d;
        wnd_id:%d;wnd_x:%d;wnd_y:%d;wnd_w:%d;wnd_h:%d; url:%s\n",
        pMsg->msg_id, pMsg->instance_id, pMsg->wnd_id,
        pMsg->wnd_x, pMsg->wnd_y, pMsg->wnd_w, pMsg->wnd_h, pMsg->url);*/
    len_w = NSP_WriteToPipe(write_fd, (void*) pMsg, len);
    if (len_w != len){
        if(errno == EPIPE) // If pipe breaks, then init pipe again and resend the msg
        {
            if(iEnsure){
                debug_fprintf(NSP_LOG_APPEND, "send message error, plugin exec need to be restart\n");
                NSP_Close_Pipe(write_fd);
                do_init_pipe();
                len_w = NSP_WriteToPipe(write_fd, (void*) pMsg, len);
            }
        }
        else if(errno == EINTR) // If interrupted by signal, then continue to send
        {
            long  new_len;
            debug_fprintf(NSP_LOG_APPEND, "send message error, send intrrupted by singal, resend again\n");
            new_len = NSP_WriteToPipe(write_fd, (char*)pMsg+len_w, len-len_w);
            len_w = len_w + new_len;
        }
        else  // else return error
        {
            debug_fprintf(NSP_LOG_APPEND, "send message error :%s.\n", strerror(errno));
            len_w = -1;
        }
    }
    NSP_Unlock_Mute_Obj(send_lock);
    debug_fprintf(NSP_LOG_APPEND, "send message success!\n");
    return len_w;
}

char* pMimeTypes = "application/vnd.stardivision.calc:sdc:StarCalc 3.0 - 5.0;"
"application/vnd.stardivision.chart:sds:StarChart 3.0 - 5.0;"
"application/vnd.stardivision.draw:sda:StarDraw 3.0 - 5.0;"
"application/vnd.stardivision.impress:sdd:StarImpress 3.0 - 5.0;"
"application/vnd.stardivision.impress-packed:sdp:StarImpress-packed 3.0 - 5.0;"
"application/vnd.stardivision.math:smf:StarMath 3.0 - 5.0;"
"application/vnd.stardivision.writer:vor:StarWriter Template 3.0 - 5.0;"
"application/vnd.stardivision.writer-global:sgl:StarWriter Global 3.0 - 5.0;"
"application/vnd.staroffice.writer:sdw:StarWriter 3.0 - 5.0;"
"application/vnd.sun.xml.calc:sxc:StarCalc;"
"application/vnd.sun.xml.calc.template:stc:StarCalc Template;"
"application/vnd.sun.xml.draw:sxd:StarDraw;"
"application/vnd.sun.xml.draw.template:std:StarDraw Template;"
"application/vnd.sun.xml.impress:sxi:StarImpress;"
"application/vnd.sun.xml.impress.template:sti:StarImpress Template;"
"application/vnd.sun.xml.math:sxm:StarMath;"
"application/vnd.sun.xml.writer:sxw:StarWriter;"
"application/vnd.sun.xml.writer.global:sxg:StarWriter Global;"
"application/vnd.sun.xml.writer.template:stw:StarWriter Template;"
"application/msword:doc:Microsoft Word Document;"
"application/vnd.ms-excel:xls:Microsoft Excel Document;"
"application/vnd.ms-powerpoint:ppt:Microsoft Powerpoint Document;"
"application/x-vnd.oasis.openoffice.text:oot:Open Office Text;"
"application/x-vnd.oasis.openoffice-template:ott:Open Office Text Template;"
"application/x-vnd.oasis.openoffice.text-master:oom:Open Office Master Document;"
"application/x-vnd.oasis.openoffice.text-web:oth:HTML Document Template;"
"application/x-vnd.oasis.openoffice.spreadsheet:oos:Open Office Spreadsheet;"
"application/x-vnd.oasis.openoffice.spreadsheet-template:ots:Open Office Spreadsheet Template;"
"application/x-vnd.oasis.openoffice.drawing:ood:Open Office Drawing;"
"application/x-vnd.oasis.openoffice.drawing-template:otd:Open Office Drawing Template;"
"application/x-vnd.oasis.openoffice.presentation:oop:Open Office Presentation;"
"application/x-vnd.oasis.openoffice.presentation-template:otp:Open Office Presentation Template;"
"application/x-vnd.oasis.openoffice.formula:oof:Open Office Formula";

extern "C"
{
char*
NPP_GetMIMEDescription(void)
{
    debug_fprintf(NSP_LOG_APPEND, "print by Netscape Plugin,  NPP_GetMIMEDescription:%s.\n", pMimeTypes);
    return(pMimeTypes);
}

#ifdef UNIX
NPError
NPP_GetValue(void * instance, NPPVariable variable, void *value)
{
    NPError err = NPERR_NO_ERROR;

    switch (variable) {
        case NPPVpluginNameString:
            *((char **)value) = PLUGIN_NAME;
            break;
        case NPPVpluginDescriptionString:
            *((char **)value) = PLUGIN_DESCRIPTION;
            break;
        default:
            err = NPERR_GENERIC_ERROR;
    }
    debug_fprintf(NSP_LOG_APPEND, "print by Netscape Plugin,  NPP_GetValue return %d.\n", err);
    return err;
}


NPMIMEType
dupMimeType(NPMIMEType type)
{
    NPMIMEType mimetype = (NPMIMEType) NPN_MemAlloc(strlen(type)+1);
    mimetype[strlen(type)] = 0;
    if (mimetype)
        strcpy(mimetype, type);
    return(mimetype);
}
#endif // end of UNIX

NPError
NPP_Initialize(void)
{
    debug_fprintf(NSP_LOG_NEW, "NS Plugin begin initialize.\n");
    return do_init_pipe();
}

#ifdef OJI
jref
NPP_GetJavaClass()
{
    return NULL;
}
#endif

void
NPP_Shutdown(void)
{
    PLUGIN_MSG msg;
    memset((char*)&msg, 0, sizeof(PLUGIN_MSG));
    msg.msg_id = SO_SHUTDOWN;
    sendMsg(&msg, sizeof(PLUGIN_MSG), 0);
    NSP_Close_Pipe(write_fd);
}

NPError
NPP_New(NPMIMEType pluginType,
    NPP instance,
    uint16 mode,
    int16 argc,
    char* argn[],
    char* argv[],
    NPSavedData* saved)
{
    PluginInstance* This;

    debug_fprintf(NSP_LOG_APPEND, "print by Netscape Plugin, enter NPP_New.\n");
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    instance->pdata = (PluginInstance*)NPN_MemAlloc(sizeof(PluginInstance));

    memset(instance->pdata, 0 , sizeof(PluginInstance));
    This = (PluginInstance*) instance->pdata;

    if (This == NULL)
    {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }

    memset(This, 0, sizeof(PluginInstance));

    /* mode is NP_EMBED, NP_FULL, or NP_BACKGROUND (see npapi.h) */
#ifdef UNIX
    This->mode = mode;
    This->type = dupMimeType(pluginType);
    This->instance = instance;
    This->pluginsPageUrl = NULL;
    This->exists = FALSE;
#endif //end of UNIX
#ifdef WNT
    This->fWindow = (NPWindow*)NPN_MemAlloc(sizeof(NPWindow));
    memset(This->fWindow, 0, sizeof (NPWindow));
    This->fMode = mode;
  #endif //end of WNT
    PLUGIN_MSG msg;
    memset((char*)&msg, 0, sizeof(PLUGIN_MSG));
    msg.msg_id = SO_NEW_INSTANCE;
    msg.instance_id = (plugin_Int32)instance;
    if (sizeof(PLUGIN_MSG) != sendMsg(&msg, sizeof(PLUGIN_MSG), 1))
        return NPERR_GENERIC_ERROR;

    NPN_Status(instance, "......");
    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save)
{
    debug_fprintf(NSP_LOG_APPEND, "print by Nsplugin, enter NPP_Destroy.\n");
    PluginInstance* This;

    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    // Send destroy message
    PLUGIN_MSG msg;
    memset((char*)&msg, 0, sizeof(PLUGIN_MSG));
    msg.msg_id = SO_DESTROY;
    msg.instance_id = (plugin_Int32)instance;
#ifdef UNIX
    msg.wnd_id =(plugin_Int32)((PluginInstance*) instance->pdata)->window;
#endif //end of UNIX
#ifdef WNT
    msg.wnd_id =(plugin_Int32)((PluginInstance*) instance->pdata)->fhWnd;
#endif //end of WNT
    sendMsg(&msg, sizeof(PLUGIN_MSG), 0);

    // Free the instance space
    This = (PluginInstance*) instance->pdata;
    if (This != NULL) {
#ifdef UNIX
        if (This->type)
            NPN_MemFree(This->type);
        if (This->pluginsPageUrl)
            NPN_MemFree(This->pluginsPageUrl);
        if (This->pluginsFileUrl)
                NPN_MemFree(This->pluginsFileUrl);
#endif //end of UNIX
#ifdef WNT
        if(This->fWindow)
            NPN_MemFree(This->fWindow);
#endif //end of WNT
        NPN_MemFree(instance->pdata);
        instance->pdata = NULL;
    }

    return NPERR_NO_ERROR;
}


NPError
NPP_SetWindow(NPP instance, NPWindow* window)
{
    PluginInstance* This;
#ifdef UNIX
    NPSetWindowCallbackStruct *ws_info;
#endif //end of UNIX

    debug_fprintf(NSP_LOG_APPEND, "print by Netscape Plugin, received window resize.\n");
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    This = (PluginInstance*) instance->pdata;
    if (This == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    // Set window info for instance
#ifdef UNIX
    ws_info        = (NPSetWindowCallbackStruct *)window->ws_info;
    This->window   = (Window) window->window;
    This->x        = window->x;
    This->y        = window->y;
    This->width    = window->width;
    This->height   = window->height;
    This->display  = ws_info->display;
    This->visual   = ws_info->visual;
    This->depth    = ws_info->depth;
    This->colormap = ws_info->colormap;
#endif    //end of UNIX
#ifdef WNT
    This->fhWnd   = (HWND) window->window;
    This->fWindow->x        = window->x;
    This->fWindow->y        = window->y;
    This->fWindow->width    = window->width;
    This->fWindow->height   = window->height;
#endif    //end of WNT
    debug_fprintf(NSP_LOG_APPEND, "begin Set window of Office\n");
    debug_fprintf(NSP_LOG_APPEND, "W=(%d) H=(%d)\n", window->width, window->height);

    // Send SET_WINDOW message
    PLUGIN_MSG msg;
    memset((char*)&msg, 0, sizeof(msg));
    msg.msg_id = SO_SET_WINDOW;
    msg.instance_id = (plugin_Int32)instance;
    msg.wnd_id = (plugin_Int32) window->window;
    msg.wnd_x = window->x;
    msg.wnd_y = window->y;
    msg.wnd_w = window->width;
    msg.wnd_h = window->height;
    if((sizeof(PLUGIN_MSG) != sendMsg(&msg, sizeof(PLUGIN_MSG), 1)))
    {
        debug_fprintf(NSP_LOG_APPEND, "NPP_SetWindow return failure \n");
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}


NPError
NPP_NewStream(NPP instance,
          NPMIMEType type,
          NPStream *stream,
          NPBool seekable,
          uint16 *stype)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    // Notify Mozilla to fetch the remote file into local cache directory
    *stype=NP_ASFILEONLY;

    return NPERR_NO_ERROR;
}


int32 STREAMBUFSIZE = 0X0FFFFFFF;
/* If we are reading from a file in NPAsFile
 * mode so we can take any size stream in our
 * write call (since we ignore it) */

int32
NPP_WriteReady(NPP instance, NPStream *stream)
{
    return STREAMBUFSIZE;
}


int32
NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
    return len;     /* The number of bytes accepted */
}


NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;
    return NPERR_NO_ERROR;
}

// save fname to another file with the original file name
void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char* fname)
{
    debug_fprintf(NSP_LOG_APPEND, "Into Stream\n");
    char* url = (char*)stream->url;
    char filename[1024] = {0};
    char* pfilename = NULL;
    if (NULL != (pfilename = strrchr(url, '/')))
    {
        strcpy(filename, pfilename+1);
    } else {
        return;
    }

    int length = strlen(url);
    debug_fprintf(NSP_LOG_APPEND, "url: %s; length: %d\n", url, length);
    PluginInstance* This;
    This = (PluginInstance*) instance->pdata;

    debug_fprintf(NSP_LOG_APPEND, "NPP_StreamAsFile\n");

    // copy cached file to another file with original name
    char localPathNew[NPP_PATH_MAX] = {0};
    char localFileNew[NPP_PATH_MAX] = {0};
    // if the file is from local
    if (0 == STRNICMP(url, "file:///", strlen("file:///")))
    {
        sprintf(localPathNew, "%s", fname);
    }
    else // from network, on windows, fname is c:\abc123
    {
        strncpy(localPathNew, fname, sizeof(localPathNew));
        char* pRandomFilename = NULL;

#ifdef UNIX
        if(NULL != (pRandomFilename = strrchr(localPathNew, '/')))
#endif //end of UNIX
#ifdef WNT
        if(NULL != (pRandomFilename = strrchr(localPathNew, '\\')))
#endif //end of WNT
        {
            pRandomFilename[1] = 0;
        } else {
            return;
        }
        strcat(localPathNew, filename);

        sprintf(localFileNew, "file://%s", localPathNew);
        UnixToDosPath(localFileNew);
        debug_fprintf(NSP_LOG_APPEND, "fname: %s\n localPathNew: %s\nlocalFileNew: %s\n",
            fname, localPathNew, localFileNew);

        restoreUTF8(localPathNew);
        restoreUTF8(localFileNew);
        if(0 != strcmp(fname, localPathNew)) {

#ifdef WNT
        if(NULL == CopyFile(fname, localPathNew, FALSE))
            return;
#endif //end of WNT

#ifdef UNIX
        int fdSrc, fdDst;
        if((0 > (fdSrc = open(fname, O_RDONLY)))){
                return;
        }
        remove(localPathNew);
        umask(0);
        if  (0 > (fdDst = open(localPathNew, O_WRONLY|O_CREAT,
                        S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))){
            close( fdSrc);
            debug_fprintf(NSP_LOG_APPEND, "NPP_StreamAsFile:can not create cache file %s. error: %s \n",
                localPathNew, strerror(errno));
            return;
        }
        char buffer[NPP_BUFFER_SIZE] = {0};
        int ret;
        while(0 <= (ret = read(fdSrc, buffer, NPP_BUFFER_SIZE)))
        {
            if (0 == ret)
            {
                if(EINTR == errno)
                    continue;
                else

                    break;
            }
            write(fdDst, buffer, ret);
        }
        close(fdSrc);
        close(fdDst);
#endif //end of UNIX

        debug_fprintf(NSP_LOG_APPEND, "NPP_StreamAsFile:before SetURL\n");
        }
    }

    // send SO_SET_URl message to inform the new URL
    PLUGIN_MSG msg;
    memset((char*)&msg, 0, sizeof(PLUGIN_MSG));
    msg.msg_id = SO_SET_URL;
    msg.instance_id = (plugin_Int32)instance;
#ifdef UNIX
    msg.wnd_id =(plugin_Int32)(This->window);
    sprintf(msg.url, "file://%s", localPathNew);
#endif //end of UNIX
#ifdef WNT
    msg.wnd_id =(int)(This->fhWnd);
    sprintf(msg.url, "file:///%s", localPathNew);
    DosToUnixPath(msg.url);
#endif //endof WNT
    if((sizeof(PLUGIN_MSG) != sendMsg(&msg, sizeof(PLUGIN_MSG), 1)))
        debug_fprintf(NSP_LOG_APPEND, "NPP_StreamAsFile send SO_SET_URL return failure \n");

    // send SO_SET_WINDOW message
//    memset((char*)&msg, 0, sizeof(PLUGIN_MSG));
    msg.msg_id = SO_SET_WINDOW;
    msg.instance_id = (plugin_Int32)instance;
//  msg.wnd_id =(plugin_Int32)((PluginInstance*) instance->pdata)->window;
#ifdef UNIX
    msg.wnd_x = This->x;
    msg.wnd_y = This->y;
    msg.wnd_w = This->width;
    msg.wnd_h = This->height;
#endif //end of UNIX
#ifdef WNT
    msg.wnd_x = This->fWindow->x;
    msg.wnd_y = This->fWindow->y;
    msg.wnd_w = This->fWindow->width;
    msg.wnd_h = This->fWindow->height;
#endif //endof WNT
    if((sizeof(PLUGIN_MSG) != sendMsg(&msg, sizeof(PLUGIN_MSG), 1)))
        debug_fprintf(NSP_LOG_APPEND, "NPP_StreamAsFile send SO_SET_WINDOW return failure \n");
}

void
NPP_URLNotify(NPP instance, const char* url,
                NPReason reason, void* notifyData)
{
}


void
NPP_Print(NPP instance, NPPrint* printInfo)
{
    if(printInfo == NULL)
        return;

    if (instance != NULL) {
    /***** Insert NPP_Print code here *****/
        PluginInstance* This = (PluginInstance*) instance->pdata;
        PLUGIN_MSG msg;
        memset((char*)&msg, 0, sizeof(PLUGIN_MSG));
        msg.msg_id = SO_PRINT;
        msg.instance_id = (plugin_Int32)instance;
        if((sizeof(PLUGIN_MSG) != sendMsg(&msg, sizeof(PLUGIN_MSG), 1)))
            debug_fprintf(NSP_LOG_APPEND, "NPP_StreamAsFile send SO_SET_WINDOW return failure \n");
       printInfo->mode = TRUE;
    /**************************************/

        if (printInfo->mode == NP_FULL) {
            /*
             * PLUGIN DEVELOPERS:
             *  If your plugin would like to take over
             *  printing completely when it is in full-screen mode,
             *  set printInfo->pluginPrinted to TRUE and print your
             *  plugin as you see fit.  If your plugin wants Netscape
             *  to handle printing in this case, set
             *  printInfo->pluginPrinted to FALSE (the default) and
             *  do nothing.  If you do want to handle printing
             *  yourself, printOne is true if the print button
             *  (as opposed to the print menu) was clicked.
             *  On the Macintosh, platformPrint is a THPrint; on
             *  Windows, platformPrint is a structure
             *  (defined in npapi.h) containing the printer name, port,
             *  etc.
             */

    /***** Insert NPP_Print code here *****\
            void* platformPrint =
                printInfo->print.fullPrint.platformPrint;
            NPBool printOne =
                printInfo->print.fullPrint.printOne;
    \**************************************/

            /* Do the default*/
            printInfo->print.fullPrint.pluginPrinted = FALSE;
        }
        else {  /* If not fullscreen, we must be embedded */
            /*
             * PLUGIN DEVELOPERS:
             *  If your plugin is embedded, or is full-screen
             *  but you returned false in pluginPrinted above, NPP_Print
             *  will be called with mode == NP_EMBED.  The NPWindow
             *  in the printInfo gives the location and dimensions of
             *  the embedded plugin on the printed page.  On the
             *  Macintosh, platformPrint is the printer port; on
             *  Windows, platformPrint is the handle to the printing
             *  device context.
             */

    /***** Insert NPP_Print code here *****\
            NPWindow* printWindow =
                &(printInfo->print.embedPrint.window);
            void* platformPrint =
                printInfo->print.embedPrint.platformPrint;
    \**************************************/
        }
    }
}

}// end of extern "C"
