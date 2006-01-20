/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: KDEInit.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2006-01-20 08:37:32 $
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

#ifndef CONNECTIVITY_KAB_KDEINIT_H
#include "KDEInit.h"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdeversion.h>

namespace connectivity
{
    namespace kab
    {
        // ===============================================================
        // = KDEInit
        // ===============================================================
        class KDEInit
        {
        private:
            /// KDE application if we own it
            static KApplication*    s_pKApplication;
            static bool             s_bDidInsertCatalogue;

        public:
            static void Init();
            static void Shutdown();
        };

        // ---------------------------------------------------------------
        KApplication* KDEInit::s_pKApplication = NULL;
        bool          KDEInit::s_bDidInsertCatalogue = false;

        // ---------------------------------------------------------------
        void KDEInit::Init()
        {
            // TODO: All this is not thread-safe

            // we create a KDE application only if it is not already done
            if (KApplication::kApplication() == NULL)
            {
                OSL_ENSURE(s_pKApplication == NULL, "KDEInit::Init: inconsistency in the application pointers!");

                // version 0.1
                char *kabargs[1] = {"libkab1"};
                KCmdLineArgs::init(1, kabargs, "KAddressBook", *kabargs, "Address Book driver", "0.1");

                s_pKApplication = new KApplication(false, false);
            }

            // set language
            rtl_Locale *pProcessLocale;
            osl_getProcessLocale(&pProcessLocale);
            // sal_Unicode and QChar are (currently) both 16 bits characters
            QString aLanguage(
                (const QChar *) pProcessLocale->Language->buffer,
                (int) pProcessLocale->Language->length);
            KGlobal::locale()->setLanguage(aLanguage);

            // load KDE address book's localized messages
            KGlobal::locale()->insertCatalogue("kaddressbook");
            s_bDidInsertCatalogue = true;
        }

        // ---------------------------------------------------------------
        void KDEInit::Shutdown()
        {
            if ( s_bDidInsertCatalogue )
                // this guard is necessary, since KDE 3.3 seems to crash if we remove a catalogue
                // which we did not previously insert
                KGlobal::locale()->removeCatalogue("kaddressbook");

            if ( s_pKApplication != NULL )
            {
                delete s_pKApplication;
                s_pKApplication = NULL;
            }
        }
    }
}

// =======================================================================
namespace
{
    double normalizeVersion( unsigned int major, unsigned int minor )
    {
        return major + 1.0 * minor / 1000;
    }
}

// -----------------------------------------------------------------------
extern "C" void SAL_CALL initKApplication()
{
    ::connectivity::kab::KDEInit::Init();
}

// -----------------------------------------------------------------------
extern "C" void SAL_CALL shutdownKApplication()
{
    ::connectivity::kab::KDEInit::Shutdown();
}
// -----------------------------------------------------------------------
/** checks whether the KDE version on the system we're running at is supported
    by the driver

    Has to be called before any other code from this library, in particular,
    it has to be called before initKApplication()

    If this function returns <code>0</code>, then no other code from this library
    has to be called, else the results are unpredictable.

    @return
        <ul><li><code>0</code> if the KDE version is supportednon</li>
            <li>a negative value if the version is too old</li>
            <li>a positive value if the version is too new to know whether it works with this driver</li>
        </ul>

    #i60062# / 2006-01-06 / frank.schoenheit@sun.com
*/
extern "C" int SAL_CALL matchKDEVersion()
{
    double nMinVersion = normalizeVersion( MIN_KDE_VERSION_MAJOR, MIN_KDE_VERSION_MINOR );
    double nCurVersion = normalizeVersion( ::KDE::versionMajor(), ::KDE::versionMinor() );
    double nMaxVersion = normalizeVersion( MAX_KDE_VERSION_MAJOR, MAX_KDE_VERSION_MINOR );

    if ( nCurVersion < nMinVersion )
        return -1;
    if ( nCurVersion > nMaxVersion )
        return 1;

    return 0;
}
