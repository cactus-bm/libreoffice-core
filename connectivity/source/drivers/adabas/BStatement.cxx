/*************************************************************************
 *
 *  $RCSfile: BStatement.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: oj $ $Date: 2002-08-23 13:20:13 $
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
#ifndef CONNECTIVITY_ADABAS_STATEMENT_HXX
#include "adabas/BStatement.hxx"
#endif
#ifndef CONNECTIVITY_ADABAS_RESULTSET_HXX
#include "adabas/BResultSet.hxx"
#endif
#ifndef _COM_SUN_STAR_LANG_DISPOSEDEXCEPTION_HPP_
#include <com/sun/star/lang/DisposedException.hpp>
#endif


using namespace connectivity::adabas;
using namespace connectivity::odbc;
//------------------------------------------------------------------------------
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdbcx;
using namespace com::sun::star::container;
using namespace com::sun::star::io;
using namespace com::sun::star::util;

typedef ::connectivity::odbc::OStatement OAdabasStatement_BASE;
// -----------------------------------------------------------------------------
OResultSet* OAdabasStatement::createResulSet()
{
    return new OAdabasResultSet(m_aStatementHandle,this,m_aSelectColumns);
}
// -----------------------------------------------------------------------------
void OAdabasStatement::setUsingBookmarks(sal_Bool _bUseBookmark)
{
    // adabas doesn't support bookmarks
}
// -----------------------------------------------------------------------------
void OAdabasStatement::setResultSetConcurrency(sal_Int32 _par0)
{
}
// -----------------------------------------------------------------------------
void OAdabasStatement::setResultSetType(sal_Int32 _par0)
{
}
// -----------------------------------------------------------------------------
sal_Bool SAL_CALL OAdabasStatement::execute( const ::rtl::OUString& sql ) throw(SQLException, RuntimeException)
{
    m_aSelectColumns = m_pOwnConnection->createSelectColumns(sql);
    return OAdabasStatement_BASE::execute(sql);
}
// -----------------------------------------------------------------------------



