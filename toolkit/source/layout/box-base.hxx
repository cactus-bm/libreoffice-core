/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: box-base.hxx,v $
 *
 * $Revision: 1.2 $
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

#ifndef CORE_BOX_BASE_HXX
#define CORE_BOX_BASE_HXX

#include "container.hxx"

#include <list>

namespace layoutimpl
{

class Box_Base : public Container
{
public:
    // Children properties
    struct ChildData
    {
        css::uno::Reference< css::awt::XLayoutConstrains > mxChild;
        css::uno::Reference< css::beans::XPropertySet > mxProps;
        css::awt::Size maRequisition;
        virtual bool isVisible();

        ChildData( css::uno::Reference< css::awt::XLayoutConstrains > const& xChild );
        virtual ~ChildData() { };
    };

    struct ChildProps: public PropHelper
    {
        //ChildProps( ChildProps* );
        PROPHELPER_SET_INFO
    };

protected:
    std::list< ChildData* > maChildren;


    virtual ChildData *createChild( css::uno::Reference< css::awt::XLayoutConstrains > const& xChild ) = 0;
    virtual ChildProps *createChildProps( ChildData* pData ) = 0;

    ChildData *removeChildData( std::list< ChildData *>, css::uno::Reference< css::awt::XLayoutConstrains > const& Child );

public:
    // css::awt::XLayoutContainer
    virtual void SAL_CALL addChild( const css::uno::Reference< css::awt::XLayoutConstrains >& Child )
        throw (css::uno::RuntimeException, css::awt::MaxChildrenException);
    virtual void SAL_CALL removeChild( const css::uno::Reference< css::awt::XLayoutConstrains >& Child )
        throw (css::uno::RuntimeException);

    virtual css::uno::Sequence< css::uno::Reference
                                < css::awt::XLayoutConstrains > > SAL_CALL getChildren()
        throw (css::uno::RuntimeException);

    virtual css::uno::Reference< css::beans::XPropertySet > SAL_CALL getChildProperties(
        const css::uno::Reference< css::awt::XLayoutConstrains >& Child )
        throw (css::uno::RuntimeException);
};

} //  namespace layoutimpl

#endif /* CORE_BOX__BASE HXX */
