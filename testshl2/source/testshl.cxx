/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: testshl.cxx,v $
 * $Revision: 1.23 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_testshl2.hxx"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef SOLARIS
#include <sys/time.h>
#endif

#ifdef WNT
# include "winstuff.hxx"
#endif

#include <iostream>
#include <vector>
#include <memory> /* auto_ptr */
#include "sal/main.h"
#include <rtl/string.hxx>

// #include <rtl/tres.h>
#include "autoregisterhelper.hxx"
// #include "cmdlinebits.hxx"
#include "log.hxx"
#include "filehelper.hxx"
#include "getopt.hxx"
#include "cppunit/result/outputter.hxx"
#include "cppunit/joblist.hxx"

// #include <osl/time.h>

using namespace std;

#include "cppunit/result/testshlTestResult.h"
#include "cppunit/result/emacsTestResult.hxx"

// Prototype for signal handling
void setSignalFilename(GetOpt & opt);
void removeSignalFile(GetOpt & opt);
CmdLineBits st_nBits;

void initTPrint(GetOpt & _aOptions)
{
    st_nBits = FileHelper::createFlags(_aOptions);
}

void my_sleep(int sec)
{
#ifdef WNT
    WinSleep(sec * 1000);
#else
    usleep(sec * 1000000); // 10 ms
#endif
}

// -----------------------------------------------------------------------------
class ProcessHandler
{
    std::string m_sProcessIDFilename;
    void write(int);
    int  getPID();
public:
    ProcessHandler();
    void setName(std::string const& _sFilename);
    ~ProcessHandler();
};

// ------------------------------- ProcessHelper -------------------------------
ProcessHandler::ProcessHandler() {}

void ProcessHandler::setName(std::string const& _sPIDFilename)
{
    m_sProcessIDFilename = _sPIDFilename;
    write(getPID());
}

int ProcessHandler::getPID()
{
    int nPID = 0;
#ifdef WNT
    nPID = WinGetCurrentProcessId();
#else
    nPID = getpid();
#endif

    return nPID;
}

void ProcessHandler::write(int _nPID)
{
    // get own PID

    if (m_sProcessIDFilename.size() > 0)
    {
        FILE* out;
        out = fopen(m_sProcessIDFilename.c_str(), "w");
        if (!out)
        {
            fprintf(stderr, "warning: (testshl.cxx) can't write own pid.\n");
            return;
            // exit(0);
        }
        fprintf(out, "%d", _nPID);
        fclose(out);
        my_sleep(2); // short wait, so testshl2_timeout can find this pid.
    }
    else
    {
        fprintf(stderr, "warning: (testshl.cxx) PID Filename empty, must set.\n");
    }
}

ProcessHandler::~ProcessHandler()
{
    if (m_sProcessIDFilename.size() > 0)
    {
        write(0);
        my_sleep(2);
        fprintf(stderr, "hint: it is save to remove PID file.\n");
    }
}

// -----------------------------------------------------------------------------
std::auto_ptr<CppUnit::TestResult> initResult(GetOpt & _aOptions)
{
    std::auto_ptr<CppUnit::TestResult> pResult;
    if (_aOptions.hasOpt("-mode"))
    {
        if (_aOptions.getOpt("-mode").equals("emacs") == sal_True)
        {
            pResult.reset(new CppUnit::emacsTestResult(_aOptions));
        }
    }
    else
    {
        pResult.reset(new CppUnit::testshlTestResult(_aOptions));
    }
    return pResult;
}

// -----------------------------------------------------------------------------

std::auto_ptr<Outputter> initOutputter(GetOpt & _aOptions)
{
    std::auto_ptr<Outputter> pOutputter;

    if (_aOptions.hasOpt("-log"))
    {
        rtl::OString sLogFile = _aOptions.getOpt("-log");

        Log* pLog = new Log( FileHelper::convertPath( sLogFile ) );
        if ( pLog->open() != osl::FileBase::E_None )
        {
            // cerr << "could not open LogFile: " << pLog->getName().getStr() << endl;
            fprintf(stderr, "could not open LogFile: %s\n", pLog->getName().getStr());
            exit(1);
        }
        pOutputter.reset(new Outputter(pLog));
    }
    else
    {
        pOutputter.reset(new Outputter(std::cout));
    }

    return pOutputter;
}

int starttest(GetOpt & opt, AutomaticRegisterHelper const& aHelper);

void endless()
{
    while(1)
    {
        my_sleep(1);
    }
}

// ----------------------------------- Main -----------------------------------

SAL_IMPLEMENT_MAIN_WITH_ARGS(, argv)
{
    static char const * optionSet[] = {
        "-boom,         stop near error position, exception only",
        "-mode=s,       the output mode, emacs, xml, old. Default is -mode old",
        "-log=s,        destination file for logging",
        "-noerrors,     shows all tests, but not it's errors.",
        "-onlyerrors,   shows only the failed test functions",
/*      "-tc=s@,        name(s) of testcase(s) to generate", */
        "-sf=s,         absolute path and name to the signal file.",
//! new (helpful if at debug time)
        "-dntsf,        if given, do not touch signal file, also if one exist.",
        "-dnrmsf,       do not remove signalfile. ",
        "-jobonly=s,    job control file, these jobs should only executed.",
        "-jobexclude=s, job control file, but contains only job, they not execute.",
        "-jobfilter=s,  use a filter for just some tests.",
        "-onlyshowjobs, show only all jobs, do no tests.",
        "-forward=s,    this string will forwarded to the test libraries.",
        "-projectid=s,  this text is added to the date output line.",
        "-buildid=s,    this text is added to the date output line.",
        "-waitforkey,   wait until key pressed.",
        "-verbose,      be verbose.",
        "-quiet,        be quiet. No t_print(...) will create output.",
        "-pid=s,        write current process id to file",
        "-endless,      testshl runs endless, for test only!!!",
        "-whereami,     shows at runtime, which function is tested next.",
        "-noerroronexit, if this is given the program exits with return code 0 even if runtime errors occurs.",
        "-h:s,          display help or help on option",
        "-help:s,       see -h",
        NULL
    };

    ProcessHandler aCurrentProcess;

    GetOpt opt( argv, optionSet );
    initTPrint(opt);

    if ( opt.hasOpt("-pid") )
    {
        aCurrentProcess.setName(opt.getOpt("-pid").getStr());
    }

    if ( opt.hasOpt("-verbose") )
    {
        fprintf(stderr, "testshl2 $Revision: 1.23 $\n");
    }

    if ( opt.hasOpt("-endless"))                 // this exists only for self test issues
    {
        endless();
    }

    int  nExitCode = 0;
    bool bLibrary = true;

    // someone indicates that he needs help
    if ( opt.hasOpt( "-h" ) || opt.hasOpt( "-help" ) )
    {
        opt.showUsage();
        nExitCode = 0;
    }
    else
    {
        if (opt.getParams().empty())
        {
            // no library is given, but if a jobonly list is given, we should generate UNKNOWN errors.
            if (! opt.hasOpt("-jobonly"))
            {
                // std::cerr << "error: At least a library or a job file should given." << std::endl;
                fprintf(stderr, "error: At least a library or a job file should given.\n");
                opt.showUsage();
                nExitCode = 2;
            }
            else
            {
                bLibrary = false;
            }
        }
        else
        {
            setSignalFilename(opt);

            rtl::OUString suLibraryName;
            if (bLibrary)
            {
                suLibraryName = rtl::OStringToOUString(opt.getFirstParam(), osl_getThreadTextEncoding() /* RTL_TEXTENCODING_ASCII_US */ );
            }

            AutomaticRegisterHelper aHelper(suLibraryName, opt /*, &aJobs*/);

            if (aHelper.isOkToStartTests())
            {
                // start the tests
                nExitCode = starttest(opt, aHelper);
            }
            else
            {

                fprintf(stderr, "error: The library '%s' can't initialised, must quit.\n", opt.getFirstParam().getStr());
                nExitCode = 1;
            }
        }
    }

    if (opt.hasOpt("-noerroronexit"))
    {
        nExitCode = 0;
    }
    return nExitCode;
}

// -----------------------------------------------------------------------------
// this function is only inserted to give a better startpoint for breakpoints

int starttest(GetOpt & opt, AutomaticRegisterHelper const& aHelper)
{
    // create a TestResult
    std::auto_ptr<CppUnit::TestResult> pResult = initResult(opt);

    // call all test functions
    aHelper.CallAll(pResult.get());

    // create and open log
    std::auto_ptr<Outputter> pOutput = initOutputter(opt);

    // give the output
    pResult->print(*pOutput.get());

    // this is a debug extension, so you can read the output and after a key is pressed the program will end.
    if (opt.hasOpt("-waitforkey"))
    {
        fprintf(stderr, "Press return key.");
        fflush(stderr);
        getchar();
    }

    removeSignalFile(opt);

    int nExit = pResult->getExitValue();
    return nExit;
}

