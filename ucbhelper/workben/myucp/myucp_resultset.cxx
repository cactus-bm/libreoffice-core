/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: myucp_resultset.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-05 15:00:17 $
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
#include "precompiled_ucbhelper.hxx"

/**************************************************************************
                                TODO
 **************************************************************************

 - This implementation is not a dynamic result set!!! It only implements
   the necessary interfaces, but never recognizes/notifies changes!!!

 *************************************************************************/

#include "myucp_datasupplier.hxx"
#include "myucp_resultset.hxx"

using namespace com::sun::star;

// @@@ Adjust namespace name.
namespace myucp {

//=========================================================================
//=========================================================================
//
// DynamicResultSet Implementation.
//
//=========================================================================
//=========================================================================

DynamicResultSet::DynamicResultSet(
    const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
    const rtl::Reference< Content >& rxContent,
    const ucb::OpenCommandArgument2& rCommand,
    const uno::Reference< ucb::XCommandEnvironment >& rxEnv )
: ResultSetImplHelper( rxSMgr, rCommand ),
  m_xContent( rxContent ),
  m_xEnv( rxEnv )
{
}

//=========================================================================
//
// Non-interface methods.
//
//=========================================================================

void DynamicResultSet::initStatic()
{
    m_xResultSet1
        = new ::ucbhelper::ResultSet( m_xSMgr,
                                      m_aCommand.Properties,
                                      new DataSupplier( m_xSMgr,
                                                        m_xContent,
                                                        m_aCommand.Mode ),
                                      m_xEnv );
}

//=========================================================================
void DynamicResultSet::initDynamic()
{
    m_xResultSet1
        = new ::ucbhelper::ResultSet( m_xSMgr,
                                      m_aCommand.Properties,
                                      new DataSupplier( m_xSMgr,
                                                        m_xContent,
                                                        m_aCommand.Mode ),
                                      m_xEnv );
    m_xResultSet2 = m_xResultSet1;
}

}
