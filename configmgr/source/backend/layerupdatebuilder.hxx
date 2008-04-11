/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: layerupdatebuilder.hxx,v $
 * $Revision: 1.6 $
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
 *
 ************************************************************************/

#ifndef CONFIGMGR_BACKEND_LAYERUPDATEBUILDER_HXX
#define CONFIGMGR_BACKEND_LAYERUPDATEBUILDER_HXX

#include "layerupdate.hxx"
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Type.hxx>
#include <rtl/ustring.hxx>

namespace com { namespace sun { namespace star { namespace configuration { namespace backend {
    struct TemplateIdentifier;
} } } } }

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace backend
    {
// -----------------------------------------------------------------------------
        using rtl::OUString;
        namespace uno = ::com::sun::star::uno;
        namespace backenduno = ::com::sun::star::configuration::backend;
// -----------------------------------------------------------------------------
        class NodeUpdate;
        class PropertyUpdate;

        class LayerUpdateBuilder
        {
            LayerUpdate     m_aUpdate;
            NodeUpdate      * m_pCurrentNode;
            PropertyUpdate  * m_pCurrentProp;
        public:
            LayerUpdateBuilder();

        public:
            /// initialize the update
            bool init();

            bool modifyNode(OUString const & _aName, sal_Int16 _nFlags, sal_Int16 _nFlagsMask, sal_Bool _bReset);
            bool replaceNode(OUString const & _aName, sal_Int16 _nFlags, backenduno::TemplateIdentifier const * _pTemplate = NULL);
            bool finishNode();

            bool removeNode(OUString const & _aName);

            bool modifyProperty(OUString const & _aName, sal_Int16 _nFlags, sal_Int16 _nFlagsMask, uno::Type const & _aType);
            bool setPropertyValue(uno::Any const & _aValue);
            bool setPropertyValueForLocale(uno::Any const & _aValue, OUString const & _aLocale);
            bool resetPropertyValue();
            bool resetPropertyValueForLocale(OUString const & _aLocale);
            bool finishProperty();

            bool addNullProperty(OUString const & _aName, sal_Int16 _nFlags, uno::Type const & _aType);
            bool addProperty(OUString const & _aName, sal_Int16 _nFlags, uno::Any const & _aValue);
            bool resetProperty(OUString const & _aName);

            bool finish();
            void clear();

            bool isEmpty()      const;
            bool isActive()     const;
            bool isComplete()   const;

            bool isPropertyActive()     const;

            LayerUpdate const & result() const;
        private:
            LayerUpdate & data();
        };
// -----------------------------------------------------------------------------

    } // namespace backend
// -----------------------------------------------------------------------------

} // namespace configmgr
#endif




