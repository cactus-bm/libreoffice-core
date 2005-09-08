/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: targeting.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 00:27:19 $
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

#ifndef __FRAMEWORK_MACROS_DEBUG_TARGETING_HXX_
#define __FRAMEWORK_MACROS_DEBUG_TARGETING_HXX_

//*****************************************************************************************************************
//  special macros for targeting of frames
//*****************************************************************************************************************

#ifdef  ENABLE_TARGETINGDEBUG

    //_____________________________________________________________________________________________________________
    //  includes
    //_____________________________________________________________________________________________________________

    #ifndef _RTL_STRBUF_HXX_
    #include <rtl/strbuf.hxx>
    #endif

    /*_____________________________________________________________________________________________________________
        LOGFILE_TARGETING

        For follow macros we need a special log file. If user forget to specify anyone, we must do it for him!
    _____________________________________________________________________________________________________________*/

    #ifndef LOGFILE_TARGETING
        #define LOGFILE_TARGETSTEPS     "targetsteps.log"
        #define LOGFILE_TARGETPARAM     "targetparam.log"
    #endif

    /*_____________________________________________________________________________________________________________
        LOG_PARAMETER_FINDFRAME( SSERVICE, SFRAMENAME, STARGETNAME, NSEARCHFLAGS )

        Log format for parameter e.g.: Desktop::findFrame( "frame1", 23 ) my name is "desktop"
        Log format for steps     e.g.: desktop--

        With this macro you can log informations about search parameter of method "findFrame()" of an service.
        Use it at beginning of search only!
    _____________________________________________________________________________________________________________*/

    #define LOG_PARAMETER_FINDFRAME( SSERVICE, SFRAMENAME, STARGETNAME, NSEARCHFLAGS )                          \
                /* Use new scope to prevent code against multiple variable defines! */                          \
                {                                                                                               \
                    ::rtl::OStringBuffer sBufferParam(256);                                                     \
                    ::rtl::OStringBuffer sBufferSteps(256);                                                     \
                    sBufferParam.append( SSERVICE                                               );              \
                    sBufferParam.append( "::findFrame( \""                                      );              \
                    sBufferParam.append( U2B( STARGETNAME )                                     );              \
                    sBufferParam.append( "\", "                                                 );              \
                    sBufferParam.append( ::rtl::OString::valueOf( sal_Int32( NSEARCHFLAGS ) )   );              \
                    sBufferParam.append( " ) my name is \""                                     );              \
                    sBufferParam.append( U2B( SFRAMENAME )                                      );              \
                    sBufferParam.append( "\"\n"                                                 );              \
                    sBufferSteps.append( U2B( SFRAMENAME )                                      );              \
                    sBufferSteps.append( "--"                                                   );              \
                    WRITE_LOGFILE( LOGFILE_TARGETPARAM, sBufferParam.makeStringAndClear() )                     \
                    WRITE_LOGFILE( LOGFILE_TARGETSTEPS, sBufferSteps.makeStringAndClear() )                     \
                }

    /*_____________________________________________________________________________________________________________
        LOG_PARAMETER_QUERYDISPATCH( SSERVICE, SFRAMENAME, AURL, STARGETNAME, NSEARCHFLAGS )

        With this macro you can log informations about search parameter of method "queryDispatch()" of an service.
        Use it at beginning of search only!
    _____________________________________________________________________________________________________________*/

    #define LOG_PARAMETER_QUERYDISPATCH( SSERVICE, SFRAMENAME, AURL, STARGETNAME, NSEARCHFLAGS )                        \
                /* Use new scope to prevent code against multiple variable defines! */                                  \
                {                                                                                                       \
                    ::rtl::OStringBuffer sBuffer(1024);                                                                 \
                    sBuffer.append( "[ "                                                    );                          \
                    sBuffer.append( U2B( SFRAMENAME )                                       );                          \
                    sBuffer.append( "] "                                                    );                          \
                    sBuffer.append( SSERVICE                                                );                          \
                    sBuffer.append( "::queryDispatch( \""                                   );                          \
                    sBuffer.append( U2B( AURL.Complete )                                    );                          \
                    sBuffer.append( "\", \""                                                );                          \
                    sBuffer.append( U2B( STARGETNAME )                                      );                          \
                    sBuffer.append( "\", "                                                  );                          \
                    sBuffer.append( ::rtl::OString::valueOf( sal_Int32( NSEARCHFLAGS ) )    );                          \
                    sBuffer.append( " )\n"                                                  );                          \
                    WRITE_LOGFILE( LOGFILE_TARGETPARAM, sBuffer.makeStringAndClear() )                                  \
                }

    /*_____________________________________________________________________________________________________________
        LOG_PARAMETER_LOADCOMPONENTFROMURL( SSERVICE, SFRAMENAME, SURL, STARGETNAME, NSEARCHFLAGS, SEQPARAMETER )

        With this macro you can log informations about search parameter of method "loadComponentFromURL()" of an service.
        Use it at beginning of search only!
    _____________________________________________________________________________________________________________*/

    #define LOG_PARAMETER_LOADCOMPONENTFROMURL( SSERVICE, SFRAMENAME, SURL, STARGETNAME, NSEARCHFLAGS, SEQPARAMETER )   \
                /* Use new scope to prevent code against multiple variable defines! */                                  \
                {                                                                                                       \
                    ::rtl::OStringBuffer sBuffer(1024);                                                                 \
                    sBuffer.append( "[ "                                                    );                          \
                    sBuffer.append( U2B( SFRAMENAME )                                       );                          \
                    sBuffer.append( "] "                                                    );                          \
                    sBuffer.append( SSERVICE                                                );                          \
                    sBuffer.append( "::loadComponentFromURL( \""                            );                          \
                    sBuffer.append( U2B( SURL )                                             );                          \
                    sBuffer.append( "\", \""                                                );                          \
                    sBuffer.append( U2B( STARGETNAME )                                      );                          \
                    sBuffer.append( "\", "                                                  );                          \
                    sBuffer.append( ::rtl::OString::valueOf( sal_Int32( NSEARCHFLAGS ) )    );                          \
                    sBuffer.append( ", "                                                    );                          \
                    sal_Int32 nCount = SEQPARAMETER.getLength();                                                        \
                    for( sal_Int32 nParameter=0; nParameter<nCount; ++nParameter )                                      \
                    {                                                                                                   \
                        sBuffer.append( U2B( SEQPARAMETER[nParameter].Name ));                                          \
                        if( nParameter<(nCount-1) )                                                                     \
                        {                                                                                               \
                            sBuffer.append( " | " );                                                                    \
                        }                                                                                               \
                    }                                                                                                   \
                    sBuffer.append( " )\n"                                                  );                          \
                    WRITE_LOGFILE( LOGFILE_TARGETPARAM, sBuffer.makeStringAndClear() )                                  \
                }

    /*_____________________________________________________________________________________________________________
        LOG_RESULT_FINDFRAME( SSERVICE, SFRAMENAME, XFRAME )

        With this macro you can log informations about search result of "findFrame()".
        Use it at the end of this method only!
    _____________________________________________________________________________________________________________*/

    #define LOG_RESULT_FINDFRAME( SSERVICE, SFRAMENAME, XFRAME )                                                        \
                /* Use new scope to prevent code against multiple variable defines! */                                  \
                {                                                                                                       \
                    ::rtl::OStringBuffer sBufferParam(256);                                                             \
                    ::rtl::OStringBuffer sBufferSteps(256);                                                             \
                    sBufferParam.append( SSERVICE               );                                                      \
                    sBufferParam.append( "::findFrame() at \""  );                                                      \
                    sBufferParam.append( U2B( SFRAMENAME )      );                                                      \
                    sBufferParam.append( "\" "                  );                                                      \
                    if( XFRAME.is() == sal_True )                                                                       \
                    {                                                                                                   \
                        sBufferParam.append( "return with valid frame.\n"       );                                      \
                        sBufferSteps.append( "OK ["                             );                                      \
                        sBufferSteps.append( U2B( XFRAME->getName() )           );                                      \
                        sBufferSteps.append( "]\n"                              );                                      \
                    }                                                                                                   \
                    else                                                                                                \
                    {                                                                                                   \
                        sBufferParam.append( "return with NULL frame!\n");                                              \
                        sBufferSteps.append( "??\n"                     );                                              \
                    }                                                                                                   \
                    WRITE_LOGFILE( LOGFILE_TARGETPARAM, sBufferParam.makeStringAndClear() )                             \
                    WRITE_LOGFILE( LOGFILE_TARGETSTEPS, sBufferSteps.makeStringAndClear() )                             \
                }

    /*_____________________________________________________________________________________________________________
        LOG_RESULT_QUERYDISPATCH( SSERVICE, SFRAMENAME, XDISPATCHER )

        With this macro you can log informations about search result of "queryDispatch()".
        Use it at the end of this method only!
    _____________________________________________________________________________________________________________*/

    #define LOG_RESULT_QUERYDISPATCH( SSERVICE, SFRAMENAME, XDISPATCHER )                                               \
                /* Use new scope to prevent code against multiple variable defines! */                                  \
                {                                                                                                       \
                    ::rtl::OStringBuffer sBuffer(1024);                                                                 \
                    sBuffer.append( "[ "                );                                                              \
                    sBuffer.append( U2B( SFRAMENAME )   );                                                              \
                    sBuffer.append( "] "                );                                                              \
                    sBuffer.append( SSERVICE            );                                                              \
                    if( XDISPATCHER.is() == sal_True )                                                                  \
                    {                                                                                                   \
                        sBuffer.append( "::queryDispatch() return with valid dispatcher."   );                          \
                    }                                                                                                   \
                    else                                                                                                \
                    {                                                                                                   \
                        sBuffer.append( "::queryDispatch() return with NULL dispatcher!"    );                          \
                    }                                                                                                   \
                    sBuffer.append( "\n" );                                                                             \
                    WRITE_LOGFILE( LOGFILE_TARGETPARAM, sBuffer.makeStringAndClear() )                                  \
                }

    /*_____________________________________________________________________________________________________________
        LOG_RESULT_LOADCOMPONENTFROMURL( SSERVICE, SFRAMENAME, XCOMPONENT )

        With this macro you can log informations about search result of "loadComponentFromURL()".
        Use it at the end of this method only!
    _____________________________________________________________________________________________________________*/

    #define LOG_RESULT_LOADCOMPONENTFROMURL( SSERVICE, SFRAMENAME, XCOMPONENT )                                         \
                /* Use new scope to prevent code against multiple variable defines! */                                  \
                {                                                                                                       \
                    ::rtl::OStringBuffer sBuffer(1024);                                                                 \
                    sBuffer.append( "[ "                );                                                              \
                    sBuffer.append( U2B( SFRAMENAME )   );                                                              \
                    sBuffer.append( "] "                );                                                              \
                    sBuffer.append( SSERVICE            );                                                              \
                    if( XCOMPONENT.is() == sal_True )                                                                   \
                    {                                                                                                   \
                        sBuffer.append( "::loadComponentFromURL() return with valid component." );                      \
                    }                                                                                                   \
                    else                                                                                                \
                    {                                                                                                   \
                        sBuffer.append( "::loadComponentFromURL() return with NULL component!"  );                      \
                    }                                                                                                   \
                    sBuffer.append( "\n" );                                                                             \
                    WRITE_LOGFILE( LOGFILE_TARGETPARAM, sBuffer.makeStringAndClear() )                                  \
                }

#else   // #ifdef ENABLE_TARGETINGDEBUG

    /*_____________________________________________________________________________________________________________
        If right testmode is'nt set - implements these macro empty!
    _____________________________________________________________________________________________________________*/

    #undef  LOGFILE_TARGETPARAM
    #undef  LOGFILE_TARGETSTEPS
    #define LOG_PARAMETER_FINDFRAME( SSERVICE, SFRAMENAME, STARGETNAME, NSEARCHFLAGS )
    #define LOG_PARAMETER_QUERYDISPATCH( SSERVICE, SFRAMENAME, AURL, STARGETNAME, NSEARCHFLAGS )
    #define LOG_PARAMETER_LOADCOMPONENTFROMURL( SSERVICE, SFRAMENAME, SURL, STARGETNAME, NSEARCHFLAGS, SEQPARAMETER )
    #define LOG_RESULT_FINDFRAME( SSERVICE, SFRAMENAME, XFRAME )
    #define LOG_RESULT_QUERYDISPATCH( SSERVICE, SFRAMENAME, XDISPATCHER )
    #define LOG_RESULT_LOADCOMPONENTFROMURL( SSERVICE, SFRAMENAME, XCOMPONENT )

#endif  // #ifdef ENABLE_TARGETINGDEBUG

//*****************************************************************************************************************
//  end of file
//*****************************************************************************************************************

#endif  // #ifndef __FRAMEWORK_MACROS_DEBUG_TARGETING_HXX_
