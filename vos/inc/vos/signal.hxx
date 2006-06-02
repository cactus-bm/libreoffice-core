/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: signal.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $ $Date: 2006-06-02 12:39:46 $
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


#ifndef _VOS_SIGNAL_HXX_
#define _VOS_SIGNAL_HXX_

#ifndef _VOS_TYPES_HXX_
#   include <vos/types.hxx>
#endif
#ifndef _VOS_OBJECT_HXX_
#   include <vos/object.hxx>
#endif
#ifndef _OSL_SIGNAL_H_
#   include <osl/signal.h>
#endif

extern oslSignalAction SAL_CALL _OSignalHandler_Function(void* pthis, oslSignalInfo* pInfo);

namespace vos
{

#if !defined ( WNT )
oslSignalAction SAL_CALL _cpp_OSignalHandler_Function(void* pthis, oslSignalInfo* pInfo);
#endif

/** OSignalHandler is an objectoriented interface for signal handlers.

    @author  Ralf Hofmann
    @version 1.0
*/

class OSignalHandler : public NAMESPACE_VOS(OObject)
{
    VOS_DECLARE_CLASSINFO(VOS_NAMESPACE(OSignalHandler, vos));

public:

    enum TSignal
    {
        TSignal_System              = osl_Signal_System,
        TSignal_Terminate           = osl_Signal_Terminate,
        TSignal_AccessViolation     = osl_Signal_AccessViolation,
        TSignal_IntegerDivideByZero = osl_Signal_IntegerDivideByZero,
        TSignal_FloatDivideByZero   = osl_Signal_FloatDivideByZero,
        TSignal_DebugBreak          = osl_Signal_DebugBreak,
        TSignal_SignalUser          = osl_Signal_User
    };

    enum TSignalAction
    {
        TAction_CallNextHandler  = osl_Signal_ActCallNextHdl,
        TAction_Ignore           = osl_Signal_ActIgnore,
        TAction_AbortApplication = osl_Signal_ActAbortApp,
        TAction_KillApplication  = osl_Signal_ActKillApp
    };

    typedef oslSignalInfo TSignalInfo;

    /// Constructor
    OSignalHandler();

    /// Destructor kills thread if neccessary
    virtual ~OSignalHandler();

    static TSignalAction SAL_CALL raise(sal_Int32 Signal, void *pData = 0);

protected:

    /// Working method which should be overridden.
    virtual TSignalAction SAL_CALL signal(TSignalInfo *pInfo) = 0;

protected:
    oslSignalHandler m_hHandler;

#if defined ( WNT )
    friend oslSignalAction SAL_CALL ::_OSignalHandler_Function(void* pthis, oslSignalInfo* pInfo);
#else
    friend oslSignalAction SAL_CALL _cpp_OSignalHandler_Function(void* pthis, oslSignalInfo* pInfo);
#endif


};

}

#endif // _VOS_SIGNAL_HXX_

