/*************************************************************************
 *
 *  $RCSfile: joburl.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: cd $ $Date: 2002-10-14 11:11:04 $
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

//________________________________
//  my own includes

#ifndef __FRAMEWORK_JOBS_JOBURL_HXX_
#include <jobs/joburl.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
#include <threadhelp/readguard.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_WRITEGUARD_HXX_
#include <threadhelp/writeguard.hxx>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

//________________________________
//  interface includes

//________________________________
//  includes of other projects

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

//________________________________
//  namespace

namespace framework{

//________________________________
//  non exported const

//________________________________
//  non exported definitions

//________________________________
//  declarations

//________________________________
/**
    @short      special ctor
    @descr      It initialize this new instance with a (hopyfully) valid job URL.
                This URL will be parsed. After that we set our members right,
                so other interface methods of this class can be used to get
                all items of this URL. Of course it will be possible to know,
                if this URL was valid too.

    @param      sURL
                    the job URL for parsing
*/
JobURL::JobURL( /*IN*/ const ::rtl::OUString& sURL )
    : ThreadHelpBase( &Application::GetSolarMutex() )
{
    m_eRequest = E_UNKNOWN;

    // syntax: vnd.sun.star.job:{[event=<name>],[alias=<name>],[service=<name>]}

    // check for "vnd.sun.star.job:"
    if (sURL.matchIgnoreAsciiCaseAsciiL(JOBURL_PROTOCOL_STR,JOBURL_PROTOCOL_LEN,0))
    {
        sal_Int32 t = JOBURL_PROTOCOL_LEN;
        do
        {
            // seperate all token of "{[event=<name>],[alias=<name>],[service=<name>]}"
            ::rtl::OUString sToken = sURL.getToken(0, JOBURL_PART_SEPERATOR, t);

            // check for "event="
            if (sToken.matchIgnoreAsciiCaseAsciiL(JOBURL_EVENT_STR,JOBURL_EVENT_LEN,0))
            {
                // if an event occures more then once ...
                // mark this instance as non valid and break further operations
                if ((m_eRequest & E_EVENT) == E_EVENT)
                {
                    m_eRequest = E_UNKNOWN;
                    return;
                }

                // set the part value
                m_sEvent    = sToken.copy(JOBURL_EVENT_LEN);
                m_eRequest |= E_EVENT;

                // but don't forget to validate it too
                if (m_sEvent.getLength()<1)
                {
                    m_eRequest = E_UNKNOWN;
                    return;
                }
            }
            else
            // check for "alias="
            if (sToken.matchIgnoreAsciiCaseAsciiL(JOBURL_ALIAS_STR,JOBURL_ALIAS_LEN,0))
            {
                // if an alias occures more then once ...
                // mark this instance as non valid and break further operations
                if ((m_eRequest & E_ALIAS) == E_ALIAS)
                {
                    m_eRequest = E_UNKNOWN;
                    return;
                }

                // set the part value
                m_sAlias    = sToken.copy(JOBURL_ALIAS_LEN);
                m_eRequest |= E_ALIAS;

                // but don't forget to validate it too
                if (m_sAlias.getLength()<1)
                {
                    m_eRequest = E_UNKNOWN;
                    return;
                }
            }
            else
            // check for "service="
            if (sToken.matchIgnoreAsciiCaseAsciiL(JOBURL_SERVICE_STR,JOBURL_SERVICE_LEN,0))
            {
                // if an service occures more then once ...
                // mark this instance as non valid and break further operations
                if ((m_eRequest & E_SERVICE) == E_SERVICE)
                {
                    m_eRequest = E_UNKNOWN;
                    return;
                }

                // set the part value
                m_sService  = sToken.copy(JOBURL_SERVICE_LEN);
                m_eRequest |= E_SERVICE;

                // but don't forget to validate it too
                if (m_sService.getLength()<1)
                {
                    m_eRequest = E_UNKNOWN;
                    return;
                }
            }
            // found and unexpected token
            // URL is not valid then.
            else
            {
                m_eRequest = E_UNKNOWN;
                return;
            }
        }
        while(t!=-1);
    }
}

//________________________________
/**
    @short      knows, if this job URL object hold a valid URL inside

    @return     <TRUE/> if it represent a valid job URL.
*/
sal_Bool JobURL::isValid() const
{
    /* SAFE { */
    ReadGuard aReadLock(m_aLock);
    return (m_eRequest!=E_UNKNOWN);
}

//________________________________
/**
    @short      get the event item of this job URL
    @descr      Because the three possible parts of such URL (event, alias, service)
                can't be combined, this method can(!) return a valid value - but it's
                not a must. Thats why the return value must be used too, to detect a missing
                event value.

    @param      sEvent
                    returns the possible existing event value
                    e.g. "vnd.sun.star.job:event=myEvent" returns "myEvent"

    @return     <TRUE/> if an event part of the job URL exist and the out parameter
                sEvent was filled.

    @attention  The out parameter will be reseted everytime. Don't use it if method returns <FALSE/>!
*/
sal_Bool JobURL::getEvent( /*OUT*/ ::rtl::OUString& sEvent ) const
{
    /* SAFE { */
    ReadGuard aReadLock(m_aLock);

             sEvent = ::rtl::OUString();
    sal_Bool bSet   = ((m_eRequest & E_EVENT) == E_EVENT);
    if (bSet)
        sEvent = m_sEvent;

    aReadLock.unlock();
    /* } SAFE */

    return bSet;
}

//________________________________
/**
    @short      get the alias item of this job URL
    @descr      Because the three possible parts of such URL (event, alias, service)
                can't be combined, this method can(!) return a valid value - but it's
                not a must. Thats why the return value must be used too, to detect a missing
                alias value.

    @param      sAlias
                    returns the possible existing alias value
                    e.g. "vnd.sun.star.job:alias=myAlias" returns "myAlias"

    @return     <TRUE/> if an alias part of the job URL exist and the out parameter
                sAlias was filled.

    @attention  The out parameter will be reseted everytime. Don't use it if method returns <FALSE/>!
*/
sal_Bool JobURL::getAlias( /*OUT*/ ::rtl::OUString& sAlias ) const
{
    /* SAFE { */
    ReadGuard aReadLock(m_aLock);

             sAlias = ::rtl::OUString();
    sal_Bool bSet   = ((m_eRequest & E_ALIAS) == E_ALIAS);
    if (bSet)
        sAlias = m_sAlias;

    aReadLock.unlock();
    /* } SAFE */

    return bSet;
}

//________________________________
/**
    @short      get the service item of this job URL
    @descr      Because the three possible parts of such URL (event, service, service)
                can't be combined, this method can(!) return a valid value - but it's
                not a must. Thats why the return value must be used too, to detect a missing
                service value.

    @param      sAlias
                    returns the possible existing service value
                    e.g. "vnd.sun.star.job:service=com.sun.star.Service" returns "com.sun.star.Service"

    @return     <TRUE/> if an service part of the job URL exist and the out parameter
                sService was filled.

    @attention  The out parameter will be reseted everytime. Don't use it if method returns <FALSE/>!
*/
sal_Bool JobURL::getService( /*OUT*/ ::rtl::OUString& sService ) const
{
    /* SAFE { */
    ReadGuard aReadLock(m_aLock);

             sService = ::rtl::OUString();
    sal_Bool bSet     = ((m_eRequest & E_SERVICE) == E_SERVICE);
    if (bSet)
        sService = m_sService;

    aReadLock.unlock();
    /* } SAFE */

    return bSet;
}

//________________________________
/**
    @short      special debug method
    @descr      It's the entry point method to start a self component check for this class.
                It's used for internal purposes only and never a part of a legal product.
                Use it for testing and debug only!
*/
#ifdef ENABLE_COMPONENT_SELF_CHECK

#define LOGFILE_JOBURL  "joburl.log"

void JobURL::impldbg_checkIt()
{
    // check simple URL's
    JobURL::impldbg_checkURL("vnd.sun.star.job:event=onMyEvent"    , E_EVENT  , "onMyEvent", ""       , ""           );
    JobURL::impldbg_checkURL("vnd.sun.star.job:alias=myAlias"      , E_ALIAS  , ""         , "myAlias", ""           );
    JobURL::impldbg_checkURL("vnd.sun.star.job:service=css.Service", E_SERVICE, ""         , ""       , "css.Service");
    JobURL::impldbg_checkURL("vnd.sun.star.job:service=,"          , E_UNKNOWN, ""         , ""       , ""           );

    // check combinations
    // Note: No additional spaces or tabs are allowed after a seperator occured.
    // Tab and spaces before a seperator will be used as value!
    JobURL::impldbg_checkURL("vnd.sun.star.job:event=onMyEvent,alias=myAlias,service=css.Service"  , E_EVENT | E_ALIAS | E_SERVICE , "onMyEvent", "myAlias", "css.Service" );
    JobURL::impldbg_checkURL("vnd.sun.star.job:service=css.Service,alias=myAlias"                  , E_ALIAS | E_SERVICE           , ""         , "myAlias", "css.Service" );
    JobURL::impldbg_checkURL("vnd.sun.star.job:service=css.Service ,alias=myAlias"                 , E_ALIAS | E_SERVICE           , ""         , "myAlias", "css.Service ");
    JobURL::impldbg_checkURL("vnd.sun.star.job:service=css.Service, alias=myAlias"                 , E_UNKNOWN                     , ""         , ""       , ""            );
    JobURL::impldbg_checkURL("vnd.sun.star.job : event=onMyEvent"                                  , E_UNKNOWN                     , ""         , ""       , ""            );
    JobURL::impldbg_checkURL("vnd.sun.star.job:event=onMyEvent,event=onMyEvent,service=css.Service", E_UNKNOWN                     , ""         , ""       , ""            );

    // check upper/lower case
    // fix parts of the URL are case insensitive (e.g. "vnd.SUN.star.job:eVEnt=")
    // values are case sensitive                 (e.g. "myAlias"                )
    JobURL::impldbg_checkURL("vnd.SUN.star.job:eVEnt=onMyEvent,aliAs=myAlias,serVice=css.Service", E_EVENT | E_ALIAS | E_SERVICE , "onMyEvent", "myAlias", "css.Service" );
    JobURL::impldbg_checkURL("vnd.SUN.star.job:eVEnt=onMyEVENT,aliAs=myALIAS,serVice=css.SERVICE", E_EVENT | E_ALIAS | E_SERVICE , "onMyEVENT", "myALIAS", "css.SERVICE" );

    // check stupid URLs
    JobURL::impldbg_checkURL("vnd.sun.star.jobs:service=css.Service"    , E_UNKNOWN, "", "", "");
    JobURL::impldbg_checkURL("vnd.sun.star.job service=css.Service"     , E_UNKNOWN, "", "", "");
    JobURL::impldbg_checkURL("vnd.sun.star.job:service,css.Service"     , E_UNKNOWN, "", "", "");
    JobURL::impldbg_checkURL("vnd.sun.star.job:service,"                , E_UNKNOWN, "", "", "");
    JobURL::impldbg_checkURL("vnd.sun.star.job:,alias,service,event="   , E_UNKNOWN, "", "", "");
    JobURL::impldbg_checkURL("vnd.sun.star.job:alias=a,service=s,event=", E_UNKNOWN, "", "", "");
}

//________________________________
/**
    @short      helper debug method
    @descr      It uses the given parameter to create a new instance of a JobURL.
                They results will be compared with the exepected ones.
                The a log will be written, which contains some detailed informations
                for this sub test.

    @param      pURL
                    the job URL, which should be checked

    @param      eExpectedPart
                    the expected result

    @param      pExpectedEvent
                    the expected event value

    @param      pExpectedAlias
                    the expected alias value

    @param      pExpectedService
                    the expected service value
*/
void JobURL::impldbg_checkURL( /*IN*/ const sal_Char*  pURL             ,
                               /*IN*/       sal_uInt32 eExpectedPart    ,
                               /*IN*/ const sal_Char*  pExpectedEvent   ,
                               /*IN*/ const sal_Char*  pExpectedAlias   ,
                               /*IN*/ const sal_Char*  pExpectedService )
{
    ::rtl::OUString sEvent  ;
    ::rtl::OUString sAlias  ;
    ::rtl::OUString sService;
    ::rtl::OUString sURL    (::rtl::OUString::createFromAscii(pURL));
    sal_Bool        bOK     = sal_True;

    JobURL aURL(sURL);

    // check if URL is invalid
    if (eExpectedPart==E_UNKNOWN)
        bOK = !aURL.isValid();

    // check if URL has the expected event part
    if (
        (bOK                                 ) &&
        ((eExpectedPart & E_EVENT) == E_EVENT)
       )
    {
        bOK = (
                (aURL.isValid()                          ) &&
                (aURL.getEvent(sEvent)                   ) &&
                (sEvent.getLength()>0                    ) &&
                (sEvent.compareToAscii(pExpectedEvent)==0)
              );
    }

    // check if URL has no event part
    if (
        (bOK                                 ) &&
        ((eExpectedPart & E_EVENT) != E_EVENT)
       )
    {
        bOK = (
                (!aURL.getEvent(sEvent)) &&
                (sEvent.getLength()==0 )
              );
    }

    // check if URL has the expected alias part
    if (
        (bOK                                 ) &&
        ((eExpectedPart & E_ALIAS) == E_ALIAS)
       )
    {
        bOK = (
                (aURL.isValid()                          ) &&
                (aURL.getAlias(sAlias)                   ) &&
                (sAlias.getLength()>0                    ) &&
                (sAlias.compareToAscii(pExpectedAlias)==0)
              );
    }

    // check if URL has the no alias part
    if (
        (bOK                                 ) &&
        ((eExpectedPart & E_ALIAS) != E_ALIAS)
       )
    {
        bOK = (
                (!aURL.getAlias(sAlias)) &&
                (sAlias.getLength()==0 )
              );
    }

    // check if URL has the expected service part
    if (
        (bOK                                     ) &&
        ((eExpectedPart & E_SERVICE) == E_SERVICE)
       )
    {
        bOK = (
                (aURL.isValid()                              ) &&
                (aURL.getService(sService)                   ) &&
                (sService.getLength()>0                      ) &&
                (sService.compareToAscii(pExpectedService)==0)
              );
    }

    // check if URL has the no service part
    if (
        (bOK                                     ) &&
        ((eExpectedPart & E_SERVICE) != E_SERVICE)
       )
    {
        bOK = (
                (!aURL.getService(sService)) &&
                (sService.getLength()==0   )
              );
    }

    ::rtl::OUStringBuffer sMsg(256);

    sMsg.appendAscii("\"" );
    sMsg.append     (sURL );
    sMsg.appendAscii("\" ");

    if (bOK)
    {
        sMsg.appendAscii("... OK\n");
    }
    else
    {
        sMsg.appendAscii("... failed\n");
        sMsg.appendAscii("expected was: ");
        if (eExpectedPart==E_UNKNOWN)
            sMsg.appendAscii("E_UNKNOWN");
        if ((eExpectedPart & E_EVENT) == E_EVENT)
        {
            sMsg.appendAscii("| E_EVENT e=\"");
            sMsg.appendAscii(pExpectedEvent  );
            sMsg.appendAscii("\""            );
        }
        if ((eExpectedPart & E_ALIAS) == E_ALIAS)
        {
            sMsg.appendAscii("| E_ALIAS a=\"");
            sMsg.appendAscii(pExpectedAlias  );
            sMsg.appendAscii("\""            );
        }
        if ((eExpectedPart & E_SERVICE) == E_SERVICE)
        {
            sMsg.appendAscii("| E_SERVICE s=\"");
            sMsg.appendAscii(pExpectedService  );
            sMsg.appendAscii("\""              );
        }
        sMsg.appendAscii("\tbut it was  : "     );
        sMsg.append     (aURL.impldbg_toString());
        sMsg.appendAscii("\n"                   );
    }

    WRITE_LOGFILE(LOGFILE_JOBURL, U2B(sMsg.makeStringAndClear()))
}

//________________________________
/**
    @short      helper debug method
    @descr      It returns a representation of the internal object state
                as string notation.

    @returns    The formated string representation.
*/
::rtl::OUString JobURL::impldbg_toString() const
{
    /* SAFE { */
    ReadGuard aReadLock(m_aLock);

    ::rtl::OUStringBuffer sBuffer(256);

    if (m_eRequest==E_UNKNOWN)
        sBuffer.appendAscii("E_UNKNOWN");
    if ((m_eRequest & E_EVENT) == E_EVENT)
        sBuffer.appendAscii("| E_EVENT");
    if ((m_eRequest & E_ALIAS) == E_ALIAS)
        sBuffer.appendAscii("| E_ALIAS");
    if ((m_eRequest & E_SERVICE) == E_SERVICE)
        sBuffer.appendAscii("| E_SERVICE");
    sBuffer.appendAscii("{ e=\""   );
    sBuffer.append     (m_sEvent   );
    sBuffer.appendAscii("\" - a=\"");
    sBuffer.append     (m_sAlias   );
    sBuffer.appendAscii("\" - s=\"");
    sBuffer.append     (m_sService );
    sBuffer.appendAscii("\" }"     );

    aReadLock.unlock();
    /* } SAFE */

    return sBuffer.makeStringAndClear();
}

#endif // ENABLE_COMPONENT_SELF_CHECK

} // namespace framework
