/*************************************************************************
 *
 *  $RCSfile: so_main.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-08-20 10:08:44 $
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
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif //end of UNIX

#ifdef WNT
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#endif //end of WNT

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "ns_debug.hxx"
#include "so_msg.hxx"
#include "so_instance.hxx"
#include "so_env.hxx"

#include "nsp_func.hxx"



#define MAX_NODE_NUM 1024
SoPluginInstance* lpInstance[MAX_NODE_NUM];


long int NSP_ReadFromPipe(NSP_PIPE_FD fp, void* buf, unsigned long int len)
{
    unsigned long int len_unix = 0, len_wnt = 0;

    len_unix = NSP_Read_Pipe(fp, buf, len, &len_wnt);
#ifdef UNIX
    return  len_unix;
#endif //end of UNIX
#ifdef WNT
    return  len_wnt;
#endif //end of WNT

}

int find_free_node()
{
    for(int i=0; i<MAX_NODE_NUM; i++)
    {
        if(NULL == lpInstance[i])
            return i;
    }
    return -1;
}

int find_cur_node(sal_Int32 cur_id)
{
    for(int i=0; i<MAX_NODE_NUM; i++)
    {
        if(lpInstance[i] == NULL)
            continue;
        if(cur_id == lpInstance[i]->GetParent())
            return i;
    }
    return -1;
}

sal_Bool dump_plugin_message(PLUGIN_MSG* pMsg)
{
    if (!pMsg)
        return sal_False;
    debug_fprintf(NSP_LOG_APPEND, "NSPlugin Message: msg_id:%d; instance_id:%d;wnd_id:%d;wnd_x:%d;wnd_y:%d;wnd_w:%d;wnd_h:%d; url:%s\n",
        pMsg->msg_id, pMsg->instance_id, pMsg->wnd_id,
        pMsg->wnd_x, pMsg->wnd_y, pMsg->wnd_w, pMsg->wnd_h, pMsg->url);
    return sal_True;
}

int prepareEnviron()
{
    // if child process inherit the chdir() property from parent process, if yes, no getNewLibraryPath() needed
    const char* pNewLibraryPath = getNewLibraryPath();
    putenv( (char*) pNewLibraryPath );
    SoPluginInstance::SetSODir((char *)findProgramDir());

    return 0;
}

int Set_Window(PLUGIN_MSG* pMsg)
{
    dump_plugin_message(pMsg);
    int cur_no;
    if( -1 == (cur_no = find_cur_node(pMsg->instance_id)))
        return -1;
    if(lpInstance[cur_no]->SetWindow(pMsg->wnd_id,
        pMsg->wnd_x, pMsg->wnd_y, pMsg->wnd_w, pMsg->wnd_h))
        return 0;
    else
        return -1;
}

int Set_URL(PLUGIN_MSG* pMsg)
{
    dump_plugin_message(pMsg);
    int cur_no;
    if( -1 == (cur_no = find_cur_node(pMsg->instance_id)))
        return -1;
    if(lpInstance[cur_no]->SetURL(pMsg->url))
        return 0;
    else
        return -1;
}

int New_Instance(PLUGIN_MSG* pMsg)
{
    dump_plugin_message(pMsg);
    int free_no;
    if( -1 == (free_no = find_free_node()))
        return -1;
    lpInstance[free_no] = new SoPluginInstance(pMsg->instance_id);
    return 0;
}

int Destroy(PLUGIN_MSG* pMsg)
{
    dump_plugin_message(pMsg);
    int cur_no;
    if( -1 == (cur_no = find_cur_node(pMsg->instance_id)))
        return -1;
    if(lpInstance[cur_no] != NULL)
    {
        lpInstance[cur_no]->Destroy();
        debug_fprintf(NSP_LOG_APPEND, "print by Nsplugin, begin delete.\n");
        delete(lpInstance[cur_no]);
        lpInstance[cur_no] = NULL;
    }
    return 0;
}

int Print(PLUGIN_MSG* pMsg)
{
    dump_plugin_message(pMsg);
    int cur_no;
    if( -1 == (cur_no = find_cur_node(pMsg->instance_id)))
        return -1;
    if(lpInstance[cur_no] != NULL)
    {
        lpInstance[cur_no]->Print();
    }
    return 0;
}

int Shutdown()
{
    for(int cur_no=0; cur_no<MAX_NODE_NUM; cur_no++)
    {
        if(lpInstance[cur_no] == NULL)
            continue;
        lpInstance[cur_no]->Destroy();
        debug_fprintf(NSP_LOG_APPEND, "print by Nsplugin, begin delete.\n");
        delete(lpInstance[cur_no]);
        lpInstance[cur_no] = NULL;
    }
    return -1;
}

int dispatchMsg(PLUGIN_MSG* pMsg)
{
    switch(pMsg->msg_id)
    {
        case SO_SET_WINDOW:
            return Set_Window(pMsg);
        case SO_NEW_INSTANCE:
            return New_Instance(pMsg);
        case SO_SET_URL:
            return Set_URL(pMsg);
        case SO_DESTROY:
            return Destroy(pMsg);
        case SO_SHUTDOWN:
            Shutdown();
            return -1;
         case SO_PRINT:
            Print(pMsg);
            return 0;
        default:
            return -1;
            break;
    }
}

sal_Bool start_office(NSP_PIPE_FD read_fd)
{
    int my_sock;
    struct sockaddr_in dst_addr;
#ifdef WNT
    {
        WSADATA wsaData;
        WORD wVersionRequested;

        wVersionRequested = MAKEWORD(2,0);
        if(WSAStartup(wVersionRequested, &wsaData))
        {
           NSP_Close_Pipe(read_fd);
           debug_fprintf(NSP_LOG_APPEND, "Can not init socket in Windows.\n");
           return sal_False;
        }
    }
#endif //end of WNT
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port   = htons(SO_SERVER_PORT);
    dst_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int count = 0;
    int ret = 0;

    my_sock=socket(PF_INET, SOCK_STREAM, 0);

    // if Star Office has been stared, we need not to start it again
    ret = connect(my_sock, (struct sockaddr *)&dst_addr, sizeof(dst_addr));
    if(ret == 0)
    {
        NSP_CloseSocket(my_sock);
        debug_fprintf(NSP_LOG_APPEND, "Staroffice already start\n");
        return sal_True;
    }
    {
        debug_fprintf(NSP_LOG_APPEND, "try to star Staroffice\n");
        char para[128] = {0};
        sprintf(para, "-accept=socket,host=0,port=%d;urp", SO_SERVER_PORT);
#ifdef UNIX
        int nChildPID = fork();
        if( ! nChildPID )  // child process
       {
            NSP_CloseSocket(my_sock);
            NSP_Close_Pipe(read_fd);
            execl("/bin/sh", "/bin/sh", "soffice", "-nologo", "-nodefault", para, NULL);
            _exit(255);
        }
#endif //end of UNIX
#ifdef WNT
            STARTUPINFO NSP_StarInfo;
            memset((void*) &NSP_StarInfo, 0, sizeof(STARTUPINFO));
            NSP_StarInfo.cb = sizeof(STARTUPINFO);
            PROCESS_INFORMATION NSP_ProcessInfo;
            memset((void*)&NSP_ProcessInfo, 0, sizeof(PROCESS_INFORMATION));
            sprintf(para, " -nologo -nodefault -accept=socket,host=0,port=%d;urp", SO_SERVER_PORT);
            SECURITY_ATTRIBUTES  NSP_access = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};
            CreateProcess(findSofficeExecutable(), para, NULL, NULL, FALSE,
                        0 , NULL, NULL, &NSP_StarInfo, &NSP_ProcessInfo);
#endif //end of WNT
    }

    // try to connect to background SO, thus judge if it is ready
    while(0 > connect(my_sock, (struct sockaddr *)&dst_addr, sizeof(dst_addr)))
    {
        NSP_Sleep(1);
        if (count++ >= 120)
        {
            NSP_CloseSocket(my_sock);
            debug_fprintf(NSP_LOG_APPEND, "print by nsplugin, con star remote StarOffice\n");
            return sal_False;
        }
        debug_fprintf(NSP_LOG_APPEND, "print by nsplugin,  Current count: %d\n", count);
    }
    NSP_CloseSocket(my_sock);
    NSP_Sleep(5);

    prepareEnviron();

    return sal_True;
}


static  NSP_PIPE_FD la_read_fd = 0;

int main(int argc, char** argv)
{
   // Sleep(20*1000);
    debug_fprintf(NSP_LOG_APPEND, "start of main\n");
    memset(lpInstance, 0, sizeof(lpInstance));
    char readbuf[NPP_BUFFER_SIZE];

    NSP_PIPE_FD fd_pipe[2];
    int iPipe[2];
#ifdef UNIX
     if(argc < 3)
    {
        debug_fprintf(NSP_LOG_APPEND, "print by nsplugin, command error; too little argument to start plugin exec\n");
        return sal_False;
    }
    iPipe[0] = atoi(argv[1]);
    iPipe[1] = atoi(argv[2]);
#endif  //end of UNIX
#ifdef WNT
    //sscanf( GetCommandLine(), "%d %d", &iPipe[0],  &iPipe[1] );
    iPipe[0] = atoi(argv[0]);
    iPipe[1] = atoi(argv[1]);
#endif  //end of WNT

    // fd_pipe[0]: read, fd_pipe[0]: write
    fd_pipe[0] = (NSP_PIPE_FD) iPipe[0] ;
    fd_pipe[1] = (NSP_PIPE_FD) iPipe[1] ;
    NSP_Close_Pipe(fd_pipe[1]);

    la_read_fd = fd_pipe[0];
    if(la_read_fd < 0)
    {
        debug_fprintf(NSP_LOG_APPEND, "print by nsplugin, command error: bad read file id:%s \n", la_read_fd);
        return 0;
    }

    if(!start_office(la_read_fd))
    {
        NSP_Close_Pipe(la_read_fd);
        return -1;
    }

    PLUGIN_MSG nMsg;
    int len;
    while(1)
    {
        memset(&nMsg, 0, sizeof(PLUGIN_MSG));
        len = NSP_ReadFromPipe(la_read_fd, (char*)&nMsg, sizeof(PLUGIN_MSG));
        if(len != sizeof(PLUGIN_MSG))
            break;
        debug_fprintf(NSP_LOG_APPEND, "Read message from pipe type %d \n", nMsg.msg_id);
        if(-1 == dispatchMsg(&nMsg))
        {
            debug_fprintf(NSP_LOG_APPEND, "plugin will shutdown\n");
            break;
        }
    }
    NSP_Close_Pipe(la_read_fd);
    _exit(0);
}

extern "C"{
    sal_Bool restart_office(void){
        return start_office(la_read_fd);
    }
}
