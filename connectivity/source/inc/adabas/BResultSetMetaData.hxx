/*************************************************************************
 *
 *  $RCSfile: BResultSetMetaData.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: oj $ $Date: 2002-08-23 13:17:06 $
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
 *  Source License Version 1.1 (the License); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an AS IS basis,
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
#ifndef CONNECTIVITY_ADABAS_RESULTSETMETADATA_HXX
#define CONNECTIVITY_ADABAS_RESULTSETMETADATA_HXX

#ifndef _CONNECTIVITY_ODBC_ORESULTSETMETADATA_HXX_
#include "odbc/OResultSetMetaData.hxx"
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif

namespace connectivity
{
    namespace adabas
    {
        //**************************************************************
        //************ Class: ResultSetMetaData
        //**************************************************************

        typedef odbc::OResultSetMetaData OAdabasResultSetMetaData_BASE;
        class OAdabasResultSetMetaData :    public  OAdabasResultSetMetaData_BASE
        {
            ::vos::ORef<OSQLColumns>    m_aSelectColumns;
        public:
            // ein Konstruktor, der fuer das Returnen des Objektes benoetigt wird:
            OAdabasResultSetMetaData(odbc::OConnection* _pConnection, SQLHANDLE _pStmt ,const ::vos::ORef<OSQLColumns>& _rSelectColumns);
            OAdabasResultSetMetaData(odbc::OConnection* _pConnection, SQLHANDLE _pStmt ,const ::std::vector<sal_Int32> & _vMapping);
            virtual ~OAdabasResultSetMetaData();

            virtual sal_Int32 SAL_CALL getColumnType( sal_Int32 column ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            virtual sal_Int32 SAL_CALL isNullable( sal_Int32 column ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        };
    }
}
#endif // CONNECTIVITY_ADABAS_RESULTSETMETADATA_HXX
