/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: valuenodeaccess.hxx,v $
 * $Revision: 1.8 $
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

#ifndef CONFIGMGR_VALUENODEACCESS_HXX
#define CONFIGMGR_VALUENODEACCESS_HXX

#include "nodeaccess.hxx"

namespace configmgr
{
// -----------------------------------------------------------------------------
    namespace data
    {
    // -------------------------------------------------------------------------
        class ValueNodeAccess
    {
    public:
            ValueNodeAccess(const sharable::ValueNode *_pNodeRef)
                : m_pData(((sharable::Node *)_pNodeRef)->valueData()) {}

            explicit
            ValueNodeAccess(NodeAccess const & _aNode)
                : m_pData(check(_aNode)) {}

            static bool isInstance(NodeAccess const & _aNode)
            {
                return check(_aNode) != NULL;
            }

            bool isValid() const { return m_pData != NULL; }

            configuration::Name getName() const;
            node::Attributes getAttributes() const;

            bool isNull()      const { return data().isNull(); }
            bool isDefault()   const;
            bool isLocalized() const;

            bool hasUsableDefault()   const { return data().hasUsableDefault(); }

            uno::Type   getValueType()  const { return data().getValueType(); }
            uno::Any    getValue()      const;
            uno::Any    getUserValue()      const;
            uno::Any    getDefaultValue()   const;

        static void setValue(ValueNodeAddress _aValueNode, uno::Any const& _aValue);
        static void setToDefault(ValueNodeAddress _aValueNode);
        static void changeDefault(ValueNodeAddress _aValueNode, uno::Any const& _aValue);

            sharable::ValueNode& data() const { return *m_pData; }
            operator ValueNodeAddress () const { return (ValueNodeAddress)m_pData; }

            operator NodeAccess() const { return NodeAccess(NodeAddress(m_pData)); }
            bool operator == (const NodeAddress &rAddr) const { return NodeAddress(m_pData) == rAddr; }
            bool operator == (const ValueNodeAddress &rAddr) const { return m_pData == rAddr; }
        private:
            static ValueNodeAddress check(sharable::Node *pNode)
        { return pNode ? const_cast<ValueNodeAddress>(pNode->valueData()) : NULL; }
            static ValueNodeAddress check(NodeAccess const&aRef)
        { return check(static_cast<sharable::Node *>(aRef)); }

            ValueNodeAddress m_pData;
        };

    // -------------------------------------------------------------------------

        inline
        configuration::Name ValueNodeAccess::getName() const
        { return NodeAccess::wrapName( data().info.getName() ); }

        inline
        node::Attributes ValueNodeAccess::getAttributes() const
        { return sharable::node(data()).getAttributes(); }

        inline
        bool ValueNodeAccess::isDefault()   const
        { return data().info.isDefault(); }

        inline
        bool ValueNodeAccess::isLocalized()   const
        { return data().info.isLocalized(); }

        inline
        uno::Any    ValueNodeAccess::getValue()      const
        { return data().getValue(); }

        inline
        uno::Any    ValueNodeAccess::getUserValue()    const
        { return data().getUserValue(); }

        inline
        uno::Any    ValueNodeAccess::getDefaultValue()    const
        { return data().getDefaultValue(); }

    // -------------------------------------------------------------------------
    }
// -----------------------------------------------------------------------------
} // namespace configmgr

#endif // CONFIGMGR_VALUENODEACCESS_HXX

