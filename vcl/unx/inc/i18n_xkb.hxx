/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: i18n_xkb.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-19 19:46:19 $
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

#ifndef _SAL_I18N_XKBDEXTENSION_HXX
#define _SAL_I18N_XKBDEXTENSION_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _VCL_DLLAPI_H
#include <dllapi.h>
#endif

class VCL_DLLPUBLIC SalI18N_KeyboardExtension
{
private:

    sal_Bool            mbUseExtension;
    sal_uInt32          mnDefaultGroup;
    sal_uInt32          mnGroup;
    int                 mnEventBase;
    int                 mnErrorBase;
    Display*            mpDisplay;

public:

                        SalI18N_KeyboardExtension( Display *pDisplay );
    inline              ~SalI18N_KeyboardExtension();

    inline sal_Bool     UseExtension() const ;      // server and client support the
                                                    // extension
    inline void         UseExtension( sal_Bool bState );// used to disable the Extension

    void                Dispatch( XEvent *pEvent ); // keep track of group changes

    sal_uInt32          LookupKeysymInGroup(    sal_uInt32 nKeyCode,
                                                  sal_uInt32 nShiftState,
                                                  sal_uInt32 nGroup ) const ;

    inline sal_uInt32   LookupKeysymInDefaultGroup(
                                                sal_uInt32 nKeyCode,
                                                sal_uInt32 nShiftState ) const ;
    inline sal_uInt32   GetGroup() const ;          // the current keyboard group
    inline sal_uInt32   GetDefaultGroup() const ;   // base group, usually group 1
    inline int          GetEventBase() const ;

protected:

                        SalI18N_KeyboardExtension(); // disabled
};

inline
SalI18N_KeyboardExtension::~SalI18N_KeyboardExtension()
{
}

inline sal_Bool
SalI18N_KeyboardExtension::UseExtension() const
{
    return mbUseExtension;
}

inline void
SalI18N_KeyboardExtension::UseExtension( sal_Bool bState )
{
    mbUseExtension = mbUseExtension && bState;
}

inline sal_uInt32
SalI18N_KeyboardExtension::LookupKeysymInDefaultGroup( sal_uInt32 nKeyCode,
                                                       sal_uInt32 nShiftState ) const
{
    return LookupKeysymInGroup( nKeyCode, nShiftState, mnDefaultGroup );
}

inline sal_uInt32
SalI18N_KeyboardExtension::GetGroup() const
{
    return mnGroup;
}

inline sal_uInt32
SalI18N_KeyboardExtension::GetDefaultGroup() const
{
    return mnDefaultGroup;
}

inline int
SalI18N_KeyboardExtension::GetEventBase() const
{
    return mnEventBase;
}

#endif // _SAL_I18N_XKBDEXTENSION_HXX

