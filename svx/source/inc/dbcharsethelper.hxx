/*************************************************************************
 *
 *  $RCSfile: dbcharsethelper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: fs $ $Date: 2002-09-12 14:15:28 $
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

#ifndef SVX_DBCHARSETHELPER_HXX
#define SVX_DBCHARSETHELPER_HXX

#ifndef SVX_DBTOOLSCLIENT_HXX
#include "dbtoolsclient.hxx"
#endif

//........................................................................
namespace svxform
{
//........................................................................

    //====================================================================
    //= ODataAccessCharsetHelper
    //====================================================================
    class ODataAccessCharsetHelper : public ODbtoolsClient
    {
    protected:
        mutable ::rtl::Reference< ::connectivity::simple::IDataAccessCharSet >  m_xCharsetHelper;
        //add by BerryJia for fixing Bug97420 Time:2002-9-12-11:00(PRC time)
        void create() const;
    public:
        ODataAccessCharsetHelper( );

        inline sal_Int32    getSupportedTextEncodings( ::std::vector< rtl_TextEncoding >& _rEncs ) const
        {
            if ( !m_xCharsetHelper.is() )
                create();
            if ( m_xCharsetHelper.is() )
                return m_xCharsetHelper->getSupportedTextEncodings( _rEncs );
            return 0;
        }
    };

//........................................................................
}   // namespace svxform
//........................................................................

#endif // SVX_DBCHARSETCLIENT_HXX

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2001/08/13 15:05:20  fs
 *  initial checkin - wrapper for accessing the dbcharset functionality without linking against dbtools
 *
 *
 *  Revision 1.0 13.08.01 16:58:49  fs
 ************************************************************************/

