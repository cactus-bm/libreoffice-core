/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 Red Hat, Inc., Stephan Bergmann <sbergman@redhat.com>
 * (initial developer)
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#ifndef INCLUDED_UNOTOOLS_CONFIGURATION_HXX
#define INCLUDED_UNOTOOLS_CONFIGURATION_HXX

#include "sal/config.h"

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "sal/types.h"
#include "unotools/unotoolsdllapi.h"

namespace com { namespace sun { namespace star {
    namespace configuration { class XReadWriteAccess; }
    namespace container {
        class XHierarchicalNameAccess;
        class XNameAccess;
        class XNameContainer;
    }
    namespace uno { class XComponentContext; }
} } }
namespace rtl { class OUString; }

namespace unotools {

namespace detail { class ConfigurationWrapper; }

/// A batch of configuration changes that is committed as a whole.
///
/// Client code needs to call commit explicitly; otherwise the changes are lost
/// when the instance is destroyed.
///
/// This is the only class from this header file that client code should use
/// directly.
class UNOTOOLS_DLLPUBLIC ConfigurationChanges: private boost::noncopyable {
public:
    static boost::shared_ptr< ConfigurationChanges > create(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context);

    ~ConfigurationChanges();

    void commit() const;

private:
    SAL_DLLPRIVATE ConfigurationChanges(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context);

    SAL_DLLPRIVATE void setPropertyValue(
        rtl::OUString const & path, com::sun::star::uno::Any const & value)
        const;

    SAL_DLLPRIVATE
    com::sun::star::uno::Reference< com::sun::star::container::XNameContainer >
    getSet(rtl::OUString const & path) const;

    com::sun::star::uno::Reference<
        com::sun::star::configuration::XReadWriteAccess > access_;

    friend class detail::ConfigurationWrapper;
};

namespace detail {

/// @internal
class UNOTOOLS_DLLPUBLIC ConfigurationWrapper: private boost::noncopyable {
public:
    static ConfigurationWrapper const & get(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context);

    SAL_DLLPRIVATE explicit ConfigurationWrapper(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context);

    SAL_DLLPRIVATE ~ConfigurationWrapper();

    com::sun::star::uno::Any getPropertyValue(rtl::OUString const & path) const;

    void setPropertyValue(
        boost::shared_ptr< ConfigurationChanges > const & batch,
        rtl::OUString const & path, com::sun::star::uno::Any const & value)
        const;

    com::sun::star::uno::Any getLocalizedPropertyValue(
        rtl::OUString const & path) const;

    void setLocalizedPropertyValue(
        boost::shared_ptr< ConfigurationChanges > const & batch,
        rtl::OUString const & path, com::sun::star::uno::Any const & value)
        const;

    com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >
    getSetReadOnly(rtl::OUString const & path) const;

    com::sun::star::uno::Reference< com::sun::star::container::XNameContainer >
    getSetReadWrite(
        boost::shared_ptr< ConfigurationChanges > const & batch,
        rtl::OUString const & path) const;

    boost::shared_ptr< ConfigurationChanges > createChanges() const;

private:
    rtl::OUString extendLocalizedPath(rtl::OUString const & path) const;

    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
        context_;

    com::sun::star::uno::Reference<
        com::sun::star::container::XHierarchicalNameAccess > access_;
};

}

/// A type-safe wrapper around a (non-localized) configuration property.
///
/// Automatically generated headers for the various configuration properties
/// derive from this template and make available its member functions to access
/// each given configuration property.
template< typename T, typename U > struct ConfigurationProperty:
    private boost::noncopyable
{
    /// Get the value of the given (non-localized) configuration property.
    ///
    /// For nillable properties, U is of type boost::optional<U'>.
    static U get(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context)
    {
        // Folding this into one statement causes a bogus error at least with
        // Red Hat GCC 4.6.2-1:
        com::sun::star::uno::Any a(
            detail::ConfigurationWrapper::get(context).getPropertyValue(
                T::path()));
        return a.get< U >();
    }

    /// Set the value of the given (non-localized) configuration property, via a
    /// given changes batch.
    ///
    /// For nillable properties, U is of type boost::optional<U'>.
    static void set(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context,
        boost::shared_ptr< ConfigurationChanges > const & batch,
        U const & value)
    {
        detail::ConfigurationWrapper::get(context).setPropertyValue(
            batch, T::path(), com::sun::star::uno::makeAny(value));
    }

private:
    ConfigurationProperty(); // not defined
    ~ConfigurationProperty(); // not defined
};

/// A type-safe wrapper around a localized configuration property.
///
/// Automatically generated headers for the various localized configuration
/// properties derive from this template and make available its member functions
/// to access each given localized configuration property.
template< typename T, typename U > struct ConfigurationLocalizedProperty:
    private boost::noncopyable
{
    /// Get the value of the given localized configuration property, for the
    /// locale currently set at the
    /// com.sun.star.configuration.theDefaultProvider.
    ///
    /// For nillable properties, U is of type boost::optional<U'>.
    static U get(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context)
    {
        // Folding this into one statement causes a bogus error at least with
        // Red Hat GCC 4.6.2-1:
        com::sun::star::uno::Any a(
            detail::ConfigurationWrapper::get(context).
            getLocalizedPropertyValue(T::path()));
        return a.get< U >();
    }

    /// Set the value of the given localized configuration property, for the
    /// locale currently set at the
    /// com.sun.star.configuration.theDefaultProvider, via a given changes
    /// batch.
    ///
    /// For nillable properties, U is of type boost::optional<U'>.
    static void set(
        com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context,
        boost::shared_ptr< ConfigurationChanges > const & batch,
        U const & value)
    {
        detail::ConfigurationWrapper::get(context).setLocalizedPropertyValue(
            batch, T::path(), com::sun::star::uno::makeAny(value));
    }

private:
    ConfigurationLocalizedProperty(); // not defined
    ~ConfigurationLocalizedProperty(); // not defined
};

/// A type-safe wrapper around a configuration set.
///
/// Automatically generated headers for the various configuration sets derive
/// from this template and make available its member functions to access each
/// given configuration set.
template< typename T > struct ConfigurationSet: private boost::noncopyable {
    /// Get read-only access to the given configuration set.
    static
    com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >
    get(com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context)
    {
        return detail::ConfigurationWrapper::get(context).getSetReadOnly(
            T::path());
    }

    /// Get read/write access to the given configuration set, storing any
    /// modifications via the given changes batch.
    static
    com::sun::star::uno::Reference< com::sun::star::container::XNameContainer >
    get(com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >
            const & context,
        boost::shared_ptr< ConfigurationChanges > const & batch)
    {
        return detail::ConfigurationWrapper::get(context).getSetReadWrite(
            batch, T::path());
    }

private:
    ConfigurationSet(); // not defined
    ~ConfigurationSet(); // not defined
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
