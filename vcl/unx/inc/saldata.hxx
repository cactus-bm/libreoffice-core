/*************************************************************************
 *
 *  $RCSfile: saldata.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: kz $ $Date: 2003-11-18 14:37:21 $
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

#ifndef _SV_SALDATA_HXX
#define _SV_SALDATA_HXX

// -=-= includes -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <signal.h>

#ifndef _SALSTD_HXX
#include <salstd.hxx>
#endif
#ifndef _SV_SVDATA_HXX
#include <svdata.hxx>
#endif
#ifndef _SV_SALWTYPE_HXX
#include <salwtype.hxx>
#endif
#ifndef _SV_SALINST_H
#include <salinst.h>
#endif

// -=-= forwards -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class SalXLib;
class SalDisplay;
class X11SalFrame;
class SalPrinter;

// -=-= typedefs -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef SIG_PF
typedef void SIG_FUNC_TYP(int);
typedef SIG_FUNC_TYP *SIG_TYP;
#define SIG_PF SIG_TYP
#endif

DECLARE_LIST( SalDisplays, SalDisplay* )

#if defined SCO || defined LINUX || defined NETBSD || defined AIX || defined HPUX || defined FREEBSD || defined MACOSX
#include <pthread.h>
#else
typedef unsigned int pthread_t;
#endif

// -=-= SalData =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class SalData
{
            SIG_TYP             sig_[30];
            BOOL                bNoExceptions_;

            SalXLib            *pXLib_;

            SalDisplays         SalDisplays_;
            SalDisplay         *pDefDisp_;
            SalDisplay         *pCurDisp_;

            pthread_t           hMainThread_;
public:
            X11SalInstance*     pInstance_;         // pointer to instance
            X11SalFrame*        pFirstFrame_;       // pointer to first frame

public:
                                SalData();
                                ~SalData();

            void                Init();

            long                ShutDown() const;
            long                Close() const;
    inline  void                XError( Display     *pDisplay,
                                        XErrorEvent *pEvent ) const;

            SalDisplay         *GetDisplay( Display *pDisplay );
    inline  SalDisplay         *GetDisplay( long nDisplay ) const
                                    { return SalDisplays_.GetObject(nDisplay); }
    inline  SalDisplay         *GetDefDisp() const { return pDefDisp_; }
    inline  SalDisplay         *GetCurDisp() const { return pCurDisp_; }
    inline  void                SetDefDisp( SalDisplay *pDisp )
                                    { pDefDisp_ = pDisp; }
    inline  void                SetCurDisp( SalDisplay *pDisp )
                                    { pCurDisp_ = pDisp; }
    inline  void                Insert( SalDisplay *pDisplay );
    inline  void                Remove( SalDisplay *pDisplay );

    void                        DeleteDisplays(); // for shutdown

    inline  SalXLib            *GetLib() const { return pXLib_; }
    inline  pthread_t       GetMainThread() const { return hMainThread_; }

            void                StartTimer( ULONG nMS );
    inline  void                StopTimer();
            void                Timeout() const;

};

// -=-= inlines =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline void SetSalData( SalData* pData )
{ ImplGetSVData()->mpSalData = (void*)pData; }

inline SalData* GetSalData()
{ return (SalData*)ImplGetSVData()->mpSalData; }

inline void SalData::Insert( SalDisplay *pDisplay )
{ SalDisplays_.Insert( pDisplay ); }

inline void SalData::Remove( SalDisplay *pDisplay )
{ SalDisplays_.Remove( pDisplay ); }

#ifdef _SV_SALDISP_HXX
inline void SalData::XError( Display *pDisplay, XErrorEvent *pEvent ) const
{ pXLib_->XError( pDisplay, pEvent ); }
#endif

class YieldMutexReleaser
{
    ULONG               m_nYieldCount;
    SalYieldMutex*      m_pSalInstYieldMutex;
public:
    inline YieldMutexReleaser();
    inline ~YieldMutexReleaser();
};

inline YieldMutexReleaser::YieldMutexReleaser()
{
    SalData *pSalData       = GetSalData();
    m_pSalInstYieldMutex    = static_cast<SalYieldMutex*>(pSalData->pInstance_->GetYieldMutex());

    // release YieldMutex as often as acquired
    ULONG i;
    if ( m_pSalInstYieldMutex->GetThreadId() ==
         NAMESPACE_VOS(OThread)::getCurrentIdentifier() )
    {
        m_nYieldCount = m_pSalInstYieldMutex->GetAcquireCount();
        for ( i = 0; i < m_nYieldCount; i++ )
            m_pSalInstYieldMutex->release();
    }
    else
        m_nYieldCount = 0;
}

inline YieldMutexReleaser::~YieldMutexReleaser()
{
    // acquire YieldMutex again
    while ( m_nYieldCount )
    {
        m_pSalInstYieldMutex->acquire();
        m_nYieldCount--;
    }
}

#endif // _SV_SALDATA_HXX

