/*************************************************************************
 *
 *  $RCSfile: template.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: jb $ $Date: 2001-10-26 10:55:13 $
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

#ifndef CONFIGMGR_CONFIGTEMPLATE_HXX_
#define CONFIGMGR_CONFIGTEMPLATE_HXX_

#include "apitypes.hxx"
#include "configexcept.hxx"
#include "configpath.hxx"

#include <vos/ref.hxx>

namespace configmgr
{
//-----------------------------------------------------------------------------
    class ITemplateProvider;
    class ISubtree;
    class OOptions;
//-----------------------------------------------------------------------------
    namespace configuration
    {
//-----------------------------------------------------------------------------

        class Name;
        class AbsolutePath;
        //---------------------------------------------------------------------

        typedef com::sun::star::uno::Type       UnoType;
        typedef com::sun::star::uno::Any        UnoAny;

//-----------------------------------------------------------------------------

        struct TemplateProvider_Impl;
        class TemplateProvider
        {
            friend class SetElementFactory;
            friend class TemplateImplHelper;

            vos::ORef<TemplateProvider_Impl>        m_aImpl;
        public:
            TemplateProvider(); // creates an empty (invalid) template instance provider
            TemplateProvider(ITemplateProvider& rProvider, vos::ORef< OOptions > const& xOptions);
            TemplateProvider(TemplateProvider const& aOther);
            TemplateProvider& operator=(TemplateProvider const& aOther);
            ~TemplateProvider();

            bool isValid() const { return !!m_aImpl.isValid(); }
        };
//-----------------------------------------------------------------------------

        struct SpecialTemplateProvider_Impl;
        class SpecialTemplateProvider
        {
            friend class TemplateImplHelper;

            vos::ORef<SpecialTemplateProvider_Impl>  m_aImpl;
        public:
            explicit
            SpecialTemplateProvider();
            SpecialTemplateProvider(SpecialTemplateProvider const& aOther);
            SpecialTemplateProvider& operator=(SpecialTemplateProvider const& aOther);
            ~SpecialTemplateProvider();

            bool isValid() const { return !!m_aImpl.isValid(); }
        };
//-----------------------------------------------------------------------------

        class Template;
        typedef vos::ORef<Template> TemplateHolder;

        /// provides information about the elements of a <type>Node</type> that is a Container ("set").
        class Template : public vos::OReference
        {
            Name        m_aName;
            Name        m_aModule;
            UnoType     m_aInstanceType;
        private:
            explicit Template(Name const& aName, Name const& aModule,UnoType const& aType);

        public:
        /// checks if the type of an instance of this is known
            bool            isInstanceTypeKnown() const;

        /// checks if this is a 'value' template <p> PRE: the instance type is known </p>
            bool            isInstanceValue() const;

        /// get the UNO type for instances (primarily (only ?) for 'value' templates) <p> PRE: the instance type is known </p>
            UnoType         getInstanceType() const;

        /// get the path where the template is located
            OUString        getPathString() const;

        /// get the local name of the template
            Name            getName() const { return m_aName; }

        /// get the package name of the template
            Name            getModule() const { return m_aModule; }

            friend class TemplateImplHelper;
        };

        /// make a template instance that matches the given (simple) type
        TemplateHolder makeSimpleTemplate(UnoType const& aType, SpecialTemplateProvider const& aProvider);
        /// make a template instance that matches the given path. Assume that it represents a (complex) tree structure.
        TemplateHolder makeTreeTemplate(OUString const& sName, OUString const& sModule, SpecialTemplateProvider const& aProvider);
        /// make a template instance that matches the elements of the given set. Ensures that the element type is known
        TemplateHolder makeSetElementTemplate(ISubtree const& aSet, TemplateProvider const& aProvider);
//-----------------------------------------------------------------------------
    }
}

#endif // CONFIGMGR_CONFIGTEMPLATE_HXX_
