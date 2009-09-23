/*************************************************************************
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2009 by Sun Microsystems, Inc.
*
* OpenOffice.org - a multi-platform office productivity suite
*
* $RCSfile: code,v $
*
* $Revision: 1.4 $
*
* This file is part of OpenOffice.org.
*
* OpenOffice.org is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* only, as published by the Free Software Foundation.
*
* OpenOffice.org is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License version 3 for more details
* (a copy is included in the LICENSE file that accompanied this code).
*
* You should have received a copy of the GNU Lesser General Public License
* version 3 along with OpenOffice.org.  If not, see
* <http://www.openoffice.org/license.html>
* for a copy of the LGPLv3 License.
************************************************************************/

#ifndef INCLUDED_CONFIGMGR_SOURCE_ROOTACCESS_HXX
#define INCLUDED_CONFIGMGR_SOURCE_ROOTACCESS_HXX

#include "sal/config.h"

#include <set>

#include "com/sun/star/lang/WrappedTargetException.hpp"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/util/ChangesSet.hpp"
#include "com/sun/star/util/XChangesBatch.hpp"
#include "com/sun/star/util/XChangesNotifier.hpp"
#include "rtl/ref.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#include "access.hxx"
#include "path.hxx"

namespace com { namespace sun { namespace star {
    namespace uno {
        class Any;
        class Type;
    }
    namespace util { class XChangesListener; }
} } }

namespace configmgr {

class Broadcaster;
class Node;
struct Modifications;

class RootAccess:
    public Access, public com::sun::star::util::XChangesNotifier,
    public com::sun::star::util::XChangesBatch
{
public:
    RootAccess(
        rtl::OUString const & pathRepresenation, rtl::OUString const & locale,
        bool update);

    virtual Path getAbsolutePath();

    virtual void initGlobalBroadcaster(
        Modifications const & modifications, Broadcaster * broadcaster);

    virtual void SAL_CALL acquire() throw ();

    virtual void SAL_CALL release() throw ();

    rtl::OUString getLocale() const;

    bool isUpdate() const;

private:
    virtual ~RootAccess();

    virtual Path getRelativePath();

    virtual rtl::OUString getRelativePathRepresentation();

    virtual rtl::Reference< Node > getNode();

    virtual bool isFinalized();

    virtual rtl::OUString getNameInternal();

    virtual rtl::Reference< RootAccess > getRootAccess();

    virtual rtl::Reference< Access > getParentAccess();

    virtual void addSupportedServiceNames(
        std::vector< rtl::OUString > * services);

    virtual void initDisposeBroadcaster(Broadcaster * broadcaster);

    virtual void clearListeners() throw ();

    virtual void initLocalBroadcaster(
        Modifications const & modifications, Broadcaster * broadcaster);

    virtual com::sun::star::uno::Any SAL_CALL queryInterface(
        com::sun::star::uno::Type const & aType)
        throw (com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL addChangesListener(
        com::sun::star::uno::Reference< com::sun::star::util::XChangesListener >
            const & aListener)
        throw (com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL removeChangesListener(
        com::sun::star::uno::Reference< com::sun::star::util::XChangesListener >
            const & aListener)
        throw (com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL commitChanges()
        throw (
            com::sun::star::lang::WrappedTargetException,
            com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL hasPendingChanges()
        throw (com::sun::star::uno::RuntimeException);

    virtual com::sun::star::util::ChangesSet SAL_CALL getPendingChanges()
        throw (com::sun::star::uno::RuntimeException);

    typedef
        std::multiset<
            com::sun::star::uno::Reference<
                com::sun::star::util::XChangesListener > >
        ChangesListeners;

    rtl::OUString pathRepresentation_;
    rtl::OUString locale_;
    bool update_;
    Path path_;
    rtl::Reference< Node > node_;
    rtl::OUString name_;
    bool finalized_;
    ChangesListeners changesListeners_;
};

}

#endif
