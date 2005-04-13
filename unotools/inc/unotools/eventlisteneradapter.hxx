/*************************************************************************
 *
 *  $RCSfile: eventlisteneradapter.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 12:25:44 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
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
#ifndef INCLUDED_UNOTOOLSDLLAPI_H
#include "unotools/unotoolsdllapi.h"
#endif

#ifndef _UNOTOOLS_EVENTLISTENERADAPTER_HXX_
#define _UNOTOOLS_EVENTLISTENERADAPTER_HXX_

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

//.........................................................................
namespace utl
{
//.........................................................................

    struct OEventListenerAdapterImpl;
    //=====================================================================
    //= OEventListenerAdapter
    //=====================================================================
    /** base class for non-UNO dispose listeners
    */
    class UNOTOOLS_DLLPUBLIC OEventListenerAdapter
    {
        friend class OEventListenerImpl;

    private:
        UNOTOOLS_DLLPRIVATE OEventListenerAdapter( const OEventListenerAdapter& _rSource ); // never implemented
        UNOTOOLS_DLLPRIVATE const OEventListenerAdapter& operator=( const OEventListenerAdapter& _rSource );    // never implemented

    protected:
        OEventListenerAdapterImpl*  m_pImpl;

    protected:
                OEventListenerAdapter();
        virtual ~OEventListenerAdapter();

        void startComponentListening( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& _rxComp );
        void stopComponentListening( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& _rxComp );
        void stopAllComponentListening(  );

        virtual void _disposing( const ::com::sun::star::lang::EventObject& _rSource ) = 0;
    };

//.........................................................................
}   // namespace utl
//.........................................................................

#endif // _UNOTOOLS_EVENTLISTENERADAPTER_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2.74.1  2005/02/28 04:33:53  mnicel
 *  Issue number:  40092
 *  Part of visibility work
 *
 *  Revision 1.2  2004/01/06 18:40:05  vg
 *  INTEGRATION: CWS vclcleanup02 (1.1.120); FILE MERGED
 *  2003/12/04 15:38:35 mt 1.1.120.1: #i23061# DTOR now virtual to avoid gcc WAll warnings
 *
 *  Revision 1.1.120.1  2003/12/04 15:38:35  mt
 *  #i23061# DTOR now virtual to avoid gcc WAll warnings
 *
 *  Revision 1.1  2001/04/19 16:13:06  fs
 *  -initial checkin - base class for non-UNO dispose listeners
 *
 *
 *  Revision 1.0 19.04.01 16:20:10  fs
 ************************************************************************/

