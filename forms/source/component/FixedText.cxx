/*************************************************************************
 *
 *  $RCSfile: FixedText.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 16:29:05 $
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
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/


#ifndef _FORMS_FIXEDTEXT_HXX_
#include "FixedText.hxx"
#endif
#ifndef _FRM_SERVICES_HXX_
#include "services.hxx"
#endif
#ifndef _FRM_PROPERTY_HRC_
#include "property.hrc"
#endif
#ifndef _FRM_PROPERTY_HXX_
#include "property.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

//.........................................................................
namespace frm
{

//------------------------------------------------------------------------------
InterfaceRef SAL_CALL OFixedTextModel_CreateInstance(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory) throw (staruno::RuntimeException)
{
    return *(new OFixedTextModel(_rxFactory));
}

//------------------------------------------------------------------------------
staruno::Sequence<staruno::Type> OFixedTextModel::_getTypes()
{
    static staruno::Sequence<staruno::Type> aTypes = OControlModel::_getTypes();
    return aTypes;
}

//------------------------------------------------------------------
OFixedTextModel::OFixedTextModel(const staruno::Reference<starlang::XMultiServiceFactory>& _rFactory)
        :OControlModel(_rFactory, VCL_CONTROLMODEL_FIXEDTEXT)

{
    m_nClassId = starform::FormComponentType::FIXEDTEXT;
}

//------------------------------------------------------------------------------
StringSequence SAL_CALL OFixedTextModel::getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException)
{
    StringSequence aSupported = OControlModel::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString* pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_FIXEDTEXT;
    return aSupported;
}

//------------------------------------------------------------------------------
staruno::Reference<starbeans::XPropertySetInfo> SAL_CALL OFixedTextModel::getPropertySetInfo() throw(staruno::RuntimeException)
{
    staruno::Reference<starbeans::XPropertySetInfo> xInfo(createPropertySetInfo(getInfoHelper()));
    return xInfo;
}

//------------------------------------------------------------------------------
cppu::IPropertyArrayHelper& OFixedTextModel::getInfoHelper()
{
    return *const_cast<OFixedTextModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
void OFixedTextModel::fillProperties(
        staruno::Sequence< starbeans::Property >& _rProps,
        staruno::Sequence< starbeans::Property >& _rAggregateProps ) const
{
    FRM_BEGIN_PROP_HELPER(4)
        RemoveProperty(_rAggregateProps, PROPERTY_TABSTOP);

        DECL_PROP1(NAME,    rtl::OUString,  BOUND);
        DECL_PROP2(CLASSID, sal_Int16,  READONLY, TRANSIENT);
        DECL_PROP1(TAG,     rtl::OUString,  BOUND);
        DECL_PROP1(HELPTEXT,rtl::OUString,  BOUND);
    FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OFixedTextModel::getServiceName() throw(staruno::RuntimeException)
{
    return FRM_COMPONENT_FIXEDTEXT; // old (non-sun) name for compatibility !
}

//------------------------------------------------------------------------------
void SAL_CALL OFixedTextModel::write(const staruno::Reference<stario::XObjectOutputStream>& _rxOutStream)
    throw(stario::IOException, staruno::RuntimeException)
{
    OControlModel::write(_rxOutStream);

    // Version
    _rxOutStream->writeShort(0x0002);
    _rxOutStream << m_aHelpText;
}

//------------------------------------------------------------------------------
void SAL_CALL OFixedTextModel::read(const staruno::Reference<stario::XObjectInputStream>& _rxInStream) throw(stario::IOException, staruno::RuntimeException)
{
    OControlModel::read(_rxInStream);

    // Version
    sal_Int16 nVersion = _rxInStream->readShort();
    if (nVersion > 1)
        _rxInStream >> m_aHelpText;
}

//.........................................................................
}
//.........................................................................

