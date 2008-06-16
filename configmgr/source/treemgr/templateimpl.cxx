/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: templateimpl.cxx,v $
 * $Revision: 1.24 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_configmgr.hxx"

#include <stdio.h>
#include "templateimpl.hxx"
#include "treeprovider.hxx"
#include "nodevisitor.hxx"
#include "treeaccessor.hxx"
#include "nodeaccess.hxx"
#include "valuenodeaccess.hxx"
#include "setnodeaccess.hxx"
#include "strdecl.hxx"
#include "typeconverter.hxx"
#include "localizedtreeactions.hxx"
#include "treeactions.hxx"
#include "apitypes.hxx"

#ifndef INCLUDED_MAP
#include <map>
#define INCLUDED_MAP
#endif

namespace configmgr
{
    namespace configuration
    {
//-----------------------------------------------------------------------------

UnoType TemplateName::resolveSimpleTypeName(Name const& aName)
{
    OUString sTypeName = aName.toString();
    return parseTemplateName(sTypeName);
}
//-----------------------------------------------------------------------------
#if OSL_DEBUG_LEVEL > 0
Name TemplateName::makeNativeTypeModuleName()
{
    OUString aModuleName( TEMPLATE_MODULE_NATIVE_VALUE );
    return makeName(aModuleName, Name::NoValidate());
}

//-----------------------------------------------------------------------------
Name TemplateName::makeLocalizedTypeModuleName()
{
    OUString aModuleName( TEMPLATE_MODULE_LOCALIZED_VALUE );
    return makeName(aModuleName, Name::NoValidate());
}
#endif
//-----------------------------------------------------------------------------
bool TemplateName::isSimpleTypeName() const
{
    bool bIsSimple = (aModule.toString().compareToAscii(TEMPLATE_MODULE_NATIVE_PREFIX,
                                                        TEMPLATE_MODULE_NATIVE_PREFIX.getLength()) == 0);

    OSL_ENSURE(!bIsSimple ||
                aModule == makeNativeTypeModuleName() ||
                aModule == makeLocalizedTypeModuleName(),
                "ERROR: Invalid template module with native prefix found");

    return bIsSimple;
}
//-----------------------------------------------------------------------------

UnoType TemplateName::resolveToSimpleType() const
{
    UnoType aType;
    if ( isSimpleTypeName() )
    {
        aType = resolveSimpleTypeName( aName );
    }
    else
        OSL_ENSURE(false, "TemplateName::resolveToSimpleType must be called only for simple type name pairs");
    return aType;
}
//-----------------------------------------------------------------------------
// class TemplateImplHelper
//-----------------------------------------------------------------------------

TemplateHolder TemplateImplHelper::createNew (TemplateName const& aNames,UnoType const& aType)
{
    return new Template(aNames.aName, aNames.aModule, aType);
}
//-----------------------------------------------------------------------------

TemplateHolder TemplateImplHelper::makeElementTemplateWithType(TemplateName const& _aNames, TemplateProvider const& _aProvider, data::SetNodeAccess const& _aSet)
{
    OSL_ENSURE(_aProvider.m_aImpl.is(), "ERROR: Cannot find a template without a provider");

    if (_aProvider.m_aImpl.is())
        return _aProvider.m_aImpl->makeElementTemplateWithType(_aNames,_aSet);

    else
        return TemplateHolder(0);
}
//-----------------------------------------------------------------------------

void TemplateImplHelper::assignActualType (Template& aTemplate,UnoType const& aType)
{
    OSL_PRECOND( aType != getNoTypeAvailable(), "ERROR: Assigning NO type to a template" );

    if (!aTemplate.isInstanceTypeKnown())
        aTemplate.m_aInstanceType = aType;

    OSL_ENSURE(aTemplate.isInstanceTypeKnown(), "ERROR: Could not assign given type to a template");
    OSL_ENSURE(aTemplate.getInstanceType() == aType, "ERROR: Trying to change instance type of a template");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class TemplateProvider_Impl
//-----------------------------------------------------------------------------

TemplateProvider_Impl::TemplateProvider_Impl(TemplateManagerRef const & xProvider, RequestOptions const& aOptions)
: m_xProvider(xProvider)
, m_aOptions(aOptions)
, m_aRepository()
{
}
//-----------------------------------------------------------------------------

data::TreeSegment TemplateProvider_Impl::instantiate(TemplateHolder const& aTemplate)
{
    data::TreeSegment pRet;
    if (aTemplate.is())
    {
        data::TreeAccessor aTemplateData = m_xProvider->requestTemplate(aTemplate->getName(), aTemplate->getModule());

        pRet = cloneExpandedForLocale(aTemplateData, m_aOptions.getLocale());
    }
    return pRet;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
namespace
{
    using namespace data;
//-----------------------------------------------------------------------------
    struct TypeDetector : SetVisitor
    {
        enum State
        {
            Contradicting = -1,
            NotFound = 0,
            SomeValue,
            VariousValue,
            SomeTree
        };

        State   result;
        UnoType type;

        TypeDetector() : result(NotFound), type() {}

       protected:
            using SetVisitor::handle;

    private: // NodeAction implementation
        Result handle(ValueNodeAccess const& _aValueNode);
        Result handle(NodeAccess const& _aNonValueNode);
    };
//-----------------------------------------------------------------------------
    static UnoType detectNodeType(TreeAccessor const& _aElement)
    {
        if (_aElement == NULL)
            throw configuration::Exception("Could not load required template to detect set elements");

        TypeDetector aDetector;
        aDetector.visitTree( _aElement );

        switch(aDetector.result)
        {
        case TypeDetector::SomeTree:        // found tree
        case TypeDetector::VariousValue:    // found an Any
        case TypeDetector::SomeValue:       // found a particular value type
            break;

#ifdef DBG_UTIL
        case TypeDetector::NotFound:        OSL_ENSURE(false,"Impossible Result: Node not handled");        if (false) // dirty abuse of case
        case TypeDetector::Contradicting:   OSL_ENSURE(false,"Impossible Result: Node contradicts itself"); if (false) // dirty abuse of case
#endif // DBG_UTIL
        default:                            OSL_ENSURE(false,"Impossible Result: Unknown result code");

            throw configuration::Exception("INTERNAL ERROR: Could not detect set element type from loaded instance");
        }
        return aDetector.type;
    }

    //-------------------------------------------------------------------------
    static bool detectElementType(UnoType& aType, data::SetNodeAccess const& _aSet)
    {
        TypeDetector aDetector;
        aDetector.visitElements( _aSet );

        bool bResult = false;
        switch(aDetector.result)
        {
        case TypeDetector::SomeTree:        // found tree
        case TypeDetector::VariousValue:    // found an Any
            aType = aDetector.type;
            bResult = true;
            break;

        case TypeDetector::SomeValue:       // found a value or an any
        case TypeDetector::NotFound:        // found no element
            break;

        case TypeDetector::Contradicting:
            OSL_ENSURE(false,"Invalid Set: contains values and subtrees");
            break;

        default: OSL_ENSURE(false,"Unreachable code");  break;
        }
        return bResult;
    }
//-----------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
TemplateHolder TemplateProvider_Impl::makeElementTemplateWithType(TemplateName const& _aNames, data::SetNodeAccess const& _aSet)
{
    typedef TemplateRepository::value_type Entry;

    TemplateRepository::iterator it = m_aRepository.find(_aNames);

    if (it == m_aRepository.end() || !it->second->isInstanceTypeKnown())
    {
        UnoType aType;
        if (_aNames.isSimpleTypeName()) // native type found
        {
            aType = _aNames.resolveToSimpleType();

            if (aType == TemplateImplHelper::getNoTypeAvailable())
                throw configuration::Exception("INTERNAL ERROR: Could not resolve native type");
        }

        else if (!detectElementType(aType,_aSet))
        {
            OSL_ASSERT(_aNames.aName == _aSet.getElementTemplateName());
            OSL_ASSERT(_aNames.aModule == _aSet.getElementTemplateModule());

            data::TreeAccessor aTemplateData = m_xProvider->requestTemplate(_aNames.aName, _aNames.aModule);

            aType = detectNodeType(aTemplateData); // throws if necessary
        }
        OSL_ASSERT( aType != TemplateImplHelper::getNoTypeAvailable() );

        if (it == m_aRepository.end())
            it = m_aRepository.insert( Entry( _aNames, TemplateImplHelper::createNew(_aNames,aType) ) ).first;

        else
            TemplateImplHelper::assignActualType(*it->second, aType);

        OSL_ENSURE(it->second->isInstanceTypeKnown(), "No type assigned to Template");
        OSL_ENSURE(it->second->getInstanceType() == aType, "Inconsistent type found for Template");
    }

#ifdef DBG_UTIL
    else
    {
        OSL_ENSURE(it->second->isInstanceTypeKnown(), "No type assigned to Template");
        UnoType aTestType;
        if (detectElementType(aTestType,_aSet))
            OSL_ENSURE(it->second->getInstanceType() == aTestType, "Inconsistent type found for Template");
    }
#endif // DBG_UTIL

    return it->second;

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace
{
//-----------------------------------------------------------------------------
    TypeDetector::Result TypeDetector::handle(ValueNodeAccess const& _aValueNode)
    {
        UnoType aFoundType = _aValueNode.getValueType();

        bool isNullType = (aFoundType.getTypeClass() == uno::TypeClass_VOID);
        bool isAnyType  = (aFoundType.getTypeClass() == uno::TypeClass_ANY);

        switch (this->result) // transition depends on previous state
        {
        case NotFound:
            this->type = aFoundType;

            if (isAnyType)
                this->result = VariousValue;

            else if (!isNullType)
                this->result = SomeValue;

            break;

        case SomeValue:
            if (!isNullType && this->type != aFoundType)
            {
                this->result = VariousValue;
                this->type =  configapi::getAnyType();
                OSL_ASSERT(type.getTypeClass() == uno::TypeClass_ANY);
            }
            break;

        case VariousValue: // remain unchanged - type already is 'Any'
            break;

        case SomeTree: OSL_ENSURE(false, "Found value node does not match previous (tree) sibling");
        default:
            this->result = Contradicting;
            break;
        }
        return CONTINUE; // always continue to detect errors in data
    }
//-----------------------------------------------------------------------------
    TypeDetector::Result TypeDetector::handle(NodeAccess const& _aNonValueNode)
    {
            { (void)_aNonValueNode; }
        OSL_ENSURE(!ValueNodeAccess::isInstance(_aNonValueNode),"Value node dipatched to wrong handler");
        switch (this->result) // transition depends on previous state
        {
        case NotFound:
            this->type = configapi::getUnoInterfaceType();
            this->result = SomeTree;
            break;

        case SomeTree: // remain unchanged - type already is Tree
            break;

        case SomeValue:
        case VariousValue:  OSL_ENSURE(false, "Found Subtree node does not match previous (value) sibling");
        default:
            this->result = Contradicting;
            break;
        }
        return CONTINUE; // always continue to detect errors in data
    }

//-----------------------------------------------------------------------------
} // anonymous
//-----------------------------------------------------------------------------
    } // configuration
} // configmgr

