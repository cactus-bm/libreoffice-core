/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: test_string.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 03:48:50 $
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


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_testshl2.hxx"
#include <iostream>
#include <string>
#include <rtl/string.hxx>

using namespace std;

const char* g_cSeparatorList = " \t,;:#";

std::string trim(std::string const& _sStringToTrim)
{
    rtl::OString sStr(_sStringToTrim.c_str());
    sStr = sStr.trim();
    return std::string(sStr.getStr());
}

bool isInCurrentEnvironment(std::string const& _sString)
{
    int nSpace = 0;
    std::string sSearchString = _sString;
    bool bEnd = false;
    // run through the hole given string,
    std::string sEnv;

    do {
        nSpace = sSearchString.find_first_of(g_cSeparatorList);
        if (nSpace < 0)
        {
            sEnv = sSearchString;
            bEnd = true;
        }
        else
        {
            sEnv = sSearchString.substr(0, nSpace);
            sSearchString = sSearchString.substr(nSpace + 1);
        }
        if (sEnv.length() > 0)
        {
            cout << sEnv << endl;
#ifdef LINUX
            int nLinux = sEnv.find("unxlng");
            if (nLinux >= 0)
            {
                // found unxlng
                return true;
            }
#endif
#ifdef SOLARIS
            int nSolaris = sEnv.find("unxsol");
            if (nSolaris >= 0)
            {
                // found unxsol
                return true;
            }
#endif
#ifdef WNT
            int nWindows = sEnv.find("wnt");
            if (nWindows >= 0)
            {
                // found wnt
                return true;
            }
#endif
        }
    } while ( !bEnd );
    return false;
}

// ----------------------------------- Main -----------------------------------
#if (defined UNX) || (defined OS2)
int main( int argc, char* argv[] )
#else
int _cdecl main( int argc, char* argv[] )
#endif
{
    string sLine1 = "osl_Socket.ctor.ctors_001";
    string sLine = "osl_Socket.ctor.ctors_001 unxlngi5.pro unxsols4.pro wntmsci   ";

    int nSpace = sLine.find_first_of(g_cSeparatorList);
    cout << "Space: " << nSpace << endl;
    std::string sTest;

    if (nSpace > 0)
    {
        sLine = trim(sLine);
        // check environment
        sTest = sLine.substr(0, nSpace);
        std::string sRest = sLine.substr(nSpace + 1);
        if (isInCurrentEnvironment(sRest))
        {
            cout << "new string: " << sTest << endl;
        }
    }
    else
    {
        // every environment
        cout << "new string: " << sLine << endl;
    }

    // cout << "rest string: " << sRest << endl;
}
