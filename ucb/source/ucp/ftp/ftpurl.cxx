/*************************************************************************
 *
 *  $RCSfile: ftpurl.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2002-09-18 15:22:01 $
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

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#include <memory>
#include <rtl/ustrbuf.hxx>
#include <com/sun/star/ucb/OpenMode.hpp>
#include <string.h>

#include "ftpstrcont.hxx"
#include "ftpurl.hxx"
#include "ftphandleprovider.hxx"
#include "ftpinpstr.hxx"
#include "ftpcfunc.hxx"

using namespace ftp;
using namespace com::sun::star::ucb;
using namespace com::sun::star::uno;
using namespace com::sun::star::io;



FTPURL::FTPURL(const rtl::OUString& url,
               FTPHandleProvider* pFCP)
    throw(
        malformed_exception
    )
    : m_aIdent(url),
      m_aUsername(rtl::OUString::createFromAscii("anonymous")),
      m_aPort(rtl::OUString::createFromAscii("21")),
      m_pFCP(pFCP)
{
    parse();
}


FTPURL::~FTPURL()
{
}


void FTPURL::parse()
    throw(
        malformed_exception
    )
{
    rtl::OString aIdent(m_aIdent.getStr(),
                        m_aIdent.getLength(),
                        RTL_TEXTENCODING_UTF8);
    char *buffer = new char[1+aIdent.getLength()];

    const char* p2 = aIdent.getStr();

    if(strncmp("ftp://",p2,6))
        throw malformed_exception();

    p2 += 6;

    char ch;
    char *p1 = buffer;  // determine "username:password@host:port"
    while((ch = *p2++) != '/' && ch)
        *p1++ = ch;
    *p1 = 0;

    rtl::OUString aExpr(rtl::OUString(buffer,strlen(buffer),
                                      RTL_TEXTENCODING_UTF8));

    sal_Int32 l = aExpr.indexOf(sal_Unicode('@'));
    m_aHost = aExpr.copy(1+l);

    if(l != -1) {
        // Now username and password.
        aExpr = aExpr.copy(0,l);
        l = aExpr.indexOf(sal_Unicode(':'));
        if(l != -1)
            m_aPassword = aExpr.copy(1+l);
        if(l > 0)
            // Overwritte only if the username is not empty.
            m_aUsername = aExpr.copy(0,l);
    }

    l = m_aHost.indexOf(sal_Unicode(':'));
    if(l != -1) {
        if(1+l<m_aHost.getLength())
            m_aPort = m_aHost.copy(1+l);
        m_aHost = m_aHost.copy(0,l);
    }

    while(ch) {  // now determine the pathsegments ...
        p1 = buffer;
        while((ch = *p2++) != '/' && ch)
            *p1++ = ch;
        *p1 = 0;

        if(buffer[0]) {
            if(strcmp(buffer,"..") == 0 &&
               m_aPathSegmentVec.size() &&
               ! m_aPathSegmentVec.back().equalsAscii(".."))
                m_aPathSegmentVec.pop_back();
            else if(strcmp(buffer,".") == 0)
                ; // Ignore
            else
                // This is a legal name.
                m_aPathSegmentVec.push_back(
                    rtl::OUString(buffer,
                                  strlen(buffer),
                                  RTL_TEXTENCODING_UTF8));
        }
    }

    delete[] buffer;

    // ... and rebuild the url as one without ellipses,
    // and more important, as one without username and
    // password. ( These are set together with the command. )

    rtl::OUStringBuffer bff(m_aIdent.getLength());
    bff.appendAscii("ftp://")
        .append(m_aHost)
          .append(sal_Unicode(':'))
          .append(m_aPort);
    for(unsigned i = 0; i < m_aPathSegmentVec.size(); ++i)
        bff.append(sal_Unicode('/')).append(m_aPathSegmentVec[i]);

    m_aIdent = bff.makeStringAndClear();
}



/** Listing of a directory.
 */

namespace ftp {

    enum OS {
        FTP_DOS,FTP_UNIX,FTP_VMS,FTP_UNKNOWN
    };

}

std::vector<FTPDirentry> FTPURL::list(
    sal_Int16 nMode
) const
    throw(
        no_such_directory_exception
    )
{
    CURL *curl = m_pFCP->handle();

    std::auto_ptr<FTPInputStream> control(new FTPInputStream()); // control
    FTPInputStreamContainer controlContainer(control.get());
    curl_easy_setopt(curl,
                     CURLOPT_HEADERFUNCTION,
                     ftp_write);
    curl_easy_setopt(curl,
                     CURLOPT_WRITEHEADER,
                     &controlContainer);


       curl_easy_setopt(curl,CURLOPT_NOBODY,false);  // data
    std::auto_ptr<FTPInputStream> data(new FTPInputStream());
    FTPInputStreamContainer dataContainer(data.get());
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ftp_write);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&dataContainer);

    rtl::OUString url(ident() +               // setting the URL
                      rtl::OUString::createFromAscii("/"));

    curl_easy_setopt(curl,
                     CURLOPT_URL,
                     rtl::OString(url.getStr(),
                                  url.getLength(),
                                  // Only ASCII in URLs => UTF8 ok
                                  RTL_TEXTENCODING_UTF8).getStr());

    // Setting username:password
    // Setting username:password
    rtl::OUString combi(username()  +
                        rtl::OUString::createFromAscii(":") +
                        password());
    rtl::OString aUserPsswd(combi.getStr(),
                            combi.getLength(),
                            RTL_TEXTENCODING_UTF8);
    curl_easy_setopt(curl,
                     CURLOPT_USERPWD,
                     aUserPsswd.getStr());

    curl_easy_setopt(curl,CURLOPT_POSTQUOTE,0);

    CURLcode err = curl_easy_perform(curl);
    if(err != CURLE_OK)
        throw no_such_directory_exception(err);

    // now evaluate the error messages

    sal_uInt32 len = (sal_uInt32) data->getLength();
    char* fwd = (char*) data->getBuffer();
    char *p1, *p2;
    p1 = p2 = fwd;

    OS osKind(FTP_UNKNOWN);
    std::vector<FTPDirentry> resvec;
    FTPDirentry aDirEntry;

    while(true) {
        while(p2-fwd < int(len) && *p2 != '\n') ++p2;
        if(p2-fwd == int(len)) break;

        *p2 = 0;
        switch(osKind) {
            // While FTP knows the 'system'-command,
            // which returns the operating system type,
            // this is not usable here: There are Windows-server
            // formatting the output like UNIX-ls command.
            case FTP_DOS:
                FTPDirectoryParser::parseDOS(aDirEntry,p1);
                break;
            case FTP_UNIX:
                FTPDirectoryParser::parseUNIX(aDirEntry,p1);
                break;
            case FTP_VMS:
                FTPDirectoryParser::parseVMS(aDirEntry,p1);
                break;
            default:
                if(FTPDirectoryParser::parseUNIX(aDirEntry,p1))
                    osKind = FTP_UNIX;
                else if(FTPDirectoryParser::parseDOS(aDirEntry,p1))
                    osKind = FTP_DOS;
                else if(FTPDirectoryParser::parseVMS(aDirEntry,p1))
                    osKind = FTP_VMS;
        }
        aDirEntry.m_aName = aDirEntry.m_aName.trim();
        if(osKind != int(FTP_UNKNOWN) &&
           !aDirEntry.m_aName.equalsAscii("..") &&
           !aDirEntry.m_aName.equalsAscii(".")) {
            if(1 + url.lastIndexOf(sal_Unicode('/')) ==
               url.getLength())
                aDirEntry.m_aURL =
                    url +
                    aDirEntry.m_aName;
            else
                aDirEntry.m_aURL =
                    url +
                    sal_Unicode('/') +
                    aDirEntry.m_aName;
            sal_Bool isDir =
                sal_Bool(aDirEntry.m_nMode&INETCOREFTP_FILEMODE_ISDIR);
            switch(nMode) {
                case OpenMode::DOCUMENTS:
                    if(!isDir) {
                        resvec.push_back(aDirEntry);
                        aDirEntry.clear();
                    }
                    break;
                case OpenMode::FOLDERS:
                    if(isDir) {
                        resvec.push_back(aDirEntry);
                        aDirEntry.clear();
                    }
                    break;
                default:
                    resvec.push_back(aDirEntry);
                    aDirEntry.clear();
            };
        }

        p1 = p2 + 1;
    }

    if(osKind == int(FTP_UNKNOWN))
        throw no_such_directory_exception(FTPCouldNotDetermineSystem);

    return resvec;
}



FTPDirentry FTPURL::direntry(
    const rtl::OUString& passwd
) const
    throw(
        no_such_directory_exception
    )
{
    CURL *curl = m_pFCP->handle();


    std::auto_ptr<FTPInputStream> control(new FTPInputStream()); // control
    FTPInputStreamContainer controlContainer(control.get());
    curl_easy_setopt(curl,
                     CURLOPT_HEADERFUNCTION,
                     ftp_write);
    curl_easy_setopt(curl,
                     CURLOPT_WRITEHEADER,
                     &controlContainer);

    curl_easy_setopt(curl,CURLOPT_NOBODY,TRUE); // no data => no transfer

    rtl::OUString url(ident() +                      // url
                      rtl::OUString::createFromAscii("/"));
    curl_easy_setopt(curl,
                     CURLOPT_URL,
                     rtl::OString(url.getStr(),
                                  url.getLength(),
                                  // Only ASCII in URLs => UTF8 ok
                                  RTL_TEXTENCODING_UTF8).getStr());
    // Setting username:password
    rtl::OUString combi(username()  +
                        rtl::OUString::createFromAscii(":") +
                        password());
    rtl::OString aUserPsswd(combi.getStr(),
                            combi.getLength(),
                            RTL_TEXTENCODING_UTF8);
    curl_easy_setopt(curl,
                     CURLOPT_USERPWD,
                     aUserPsswd.getStr());

    // post request
    struct curl_slist *slist = 0;
    slist = curl_slist_append(slist,"PWD");
    curl_easy_setopt(curl,CURLOPT_POSTQUOTE,slist);

    CURLcode err = curl_easy_perform(curl);
    // clean up
    curl_slist_free_all(slist);

    /* now decide on the error codes:*/
    if(err == CURLE_FTP_ACCESS_DENIED)
        throw no_such_directory_exception(err);


    rtl::OUString title;
    if(err == CURLE_OK) {
        // get the title
        sal_uInt32 len = (sal_uInt32) control->getLength();
        char* fwd = (char*) control->getBuffer();
        title = rtl::OUString(fwd,len,RTL_TEXTENCODING_UTF8);

        // the buffer now contains the name of the file;
        // analyze the output:
        // Format of current working directory:
        // 257 "/bla" is current directory
        sal_Int32 index1 = title.lastIndexOf(
            rtl::OUString::createFromAscii("257"));
        index1 = 1+title.indexOf(sal_Unicode('"'),index1);
        sal_Int32 index2 = title.indexOf(sal_Unicode('"'),index1);
        title = title.copy(index1,index2-index1);
        if(!title.equalsAscii("/")) {
            index1 = title.lastIndexOf(sal_Unicode('/'));
            title = title.copy(1+index1);
        }
    }
    /** The ideas is to have always a title,
     *  either determined by 'pwd' or else from the URL.
     */
    else if(m_aPathSegmentVec.size())
        // Determine title form url
        title = m_aPathSegmentVec.back();
    else
        // title is root.
        title = rtl::OUString::createFromAscii("/");

    FTPDirentry aDirentry;
    // init aDirentry
    aDirentry.m_aName = title;
    aDirentry.m_nMode = INETCOREFTP_FILEMODE_ISDIR;
    aDirentry.m_nSize = 0;

    try {
        if(!title.equalsAscii("/")) {
            // try to open it the parent directory
            FTPURL aURL(url + rtl::OUString::createFromAscii("../"),
                        m_pFCP);

            std::vector<FTPDirentry> list = aURL.list(OpenMode::ALL);

            for(unsigned i = 0; i < list.size(); ++i) {
                if(list[i].m_aName == title) {
                    aDirentry = list[i];
                    break;
                }
            }
        }
    } catch(const no_such_directory_exception&) {
        // no hope, we cannot get any propertyvalues
    }

    return aDirentry;
}
