/*************************************************************************
 *
 *  $RCSfile: basicimporthandler.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-06-18 15:47:26 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "basicimporthandler.hxx"
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_COMPONENTCHANGEEVENT_HPP_
#include <com/sun/star/configuration/backend/ComponentChangeEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XBACKENDCHANGESNOTIFIER_HPP_
#include <com/sun/star/configuration/backend/XBackendChangesNotifier.hpp>
#endif
// -----------------------------------------------------------------------------

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace backend
    {
// -----------------------------------------------------------------------------

BasicImportHandler::BasicImportHandler(
    Backend const & xBackend,OUString const & aEntity, const sal_Bool&  bNotify)
: m_xBackend(xBackend)
, m_aComponentName()
, m_aEntity(aEntity)
, m_bSendNotification(bNotify)
{
    OSL_ENSURE( m_xBackend.is(), "Creating an import handler without a target backend" );
}
// -----------------------------------------------------------------------------

BasicImportHandler::~BasicImportHandler()
{
}
// -----------------------------------------------------------------------------

void SAL_CALL
    BasicImportHandler::startLayer(  )
        throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException)
{
    m_aComponentName = OUString();
}
// -----------------------------------------------------------------------------

void SAL_CALL
    BasicImportHandler::endLayer(  )
        throw (MalformedDataException, lang::WrappedTargetException, uno::RuntimeException)
{
    if ( m_bSendNotification)
    {
        backenduno::ComponentChangeEvent aEvent;
        aEvent.Source=*this;
        aEvent.Component = m_aComponentName;

        uno::Reference<backenduno::XBackendChangesListener> xListener(m_xBackend, uno::UNO_QUERY);
        if( xListener.is())
        {
            xListener->componentDataChanged(aEvent);
        }
        else
        {
            OSL_ENSURE(false, "ImportMergeHandler: target backend does not support notifications");
        }
    }
    m_aComponentName = OUString();
}
// -----------------------------------------------------------------------------

bool BasicImportHandler::startComponent( const OUString& aName )
{
    if (hasComponent()) return false;

    m_aComponentName = aName;
    return true;
}
// -----------------------------------------------------------------------------

void BasicImportHandler::raiseMalformedDataException(sal_Char const * pMsg)
{
    OUString sMsg = OUString::createFromAscii(pMsg);

    throw MalformedDataException(sMsg, *this, uno::Any());
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
    } // namespace

// -----------------------------------------------------------------------------
} // namespace

