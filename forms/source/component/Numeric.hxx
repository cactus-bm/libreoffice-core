/*************************************************************************
 *
 *  $RCSfile: Numeric.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: fs $ $Date: 2000-10-19 11:52:16 $
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

#ifndef _FORMS_NUMERIC_HXX_
#define _FORMS_NUMERIC_HXX_

#ifndef _FORMS_EDITBASE_HXX_
#include "EditBase.hxx"
#endif

//.........................................................................
namespace frm
{
//.........................................................................

//==================================================================
//= ONumericModel
//==================================================================
class ONumericModel
                :public OEditBaseModel
                ,public ::comphelper::OAggregationArrayUsageHelper< ONumericModel >
{
    staruno::Any            m_aSaveValue;
    static sal_Int32    nValueHandle;

protected:
    virtual void _onValueChanged();
    virtual staruno::Sequence<staruno::Type> _getTypes();

public:
    ONumericModel(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory);
    virtual ~ONumericModel();

    // starform::XBoundComponent
    virtual sal_Bool _commit();

    // starlang::XServiceInfo
    IMPLEMENTATION_NAME(ONumericModel);
    virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

    // staruno::Reference<starbeans::XPropertySet>
    virtual staruno::Reference<starbeans::XPropertySetInfo> SAL_CALL getPropertySetInfo() throw(staruno::RuntimeException);
    virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

    // stario::XPersistObject
    virtual ::rtl::OUString SAL_CALL getServiceName();

    // starform::XReset
    virtual void _reset();

    // OAggregationArrayUsageHelper
    virtual void fillProperties(
        staruno::Sequence< starbeans::Property >& /* [out] */ _rProps,
        staruno::Sequence< starbeans::Property >& /* [out] */ _rAggregateProps
        ) const;
    IMPLEMENT_INFO_SERVICE()
};

//==================================================================
//= ONumericControl
//==================================================================
class ONumericControl: public OBoundControl
{
protected:
    virtual staruno::Sequence<staruno::Type> _getTypes();

public:
    ONumericControl(const staruno::Reference<starlang::XMultiServiceFactory>& _rxFactory);

    // starlang::XServiceInfo
    IMPLEMENTATION_NAME(ONumericControl);
    virtual StringSequence SAL_CALL getSupportedServiceNames() throw();
};

//.........................................................................
}   // namespace frm
//.........................................................................

#endif // _FORMS_NUMERIC_HXX_

