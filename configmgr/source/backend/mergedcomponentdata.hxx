/*************************************************************************
 *
 *  $RCSfile: mergedcomponentdata.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2003-05-26 08:05:43 $
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

/* PLEASE DON'T DELETE ANY COMMENT LINES, ALSO IT'S UNNECESSARY. */

#ifndef CONFIGMGR_BACKEND_MERGEDCOMPONENTDATA_HXX
#define CONFIGMGR_BACKEND_MERGEDCOMPONENTDATA_HXX

#ifndef _CONFIGMGR_TREE_VALUENODE_HXX
#include "valuenode.hxx"
#endif

#include <com/sun/star/configuration/backend/TemplateIdentifier.hpp>

#ifndef INCLUDED_MEMORY
#include <memory>
#define INCLUDED_MEMORY
#endif

namespace configmgr
{
// -----------------------------------------------------------------------------
    class OTreeNodeFactory;
// -----------------------------------------------------------------------------
    namespace backend
    {
// -----------------------------------------------------------------------------
        namespace backenduno = ::com::sun::star::configuration::backend;

        using  backenduno::TemplateIdentifier;

        using ::rtl::OUString;

// -----------------------------------------------------------------------------

        class MergedComponentData
        {
        public:
            MergedComponentData();
            ~MergedComponentData();

            bool hasSchema()    const;
            bool hasTemplates() const;
            bool hasTemplate(OUString const & _aTemplateName) const;

            OUString getTemplateAccessor (TemplateIdentifier const & _aTemplateName) const;

            ISubtree const * getSchemaTree() const     { return m_pSchemaTree.get(); }
            ISubtree const * getTemplatesTree() const  { return m_pTemplatesTree.get(); }
            ISubtree const * findTemplate(OUString const & _aTemplateName) const;

            ISubtree * getSchemaTree()      { return m_pSchemaTree.get(); }
            ISubtree * getTemplatesTree()   { return m_pTemplatesTree.get(); }

            std::auto_ptr<INode> instantiateTemplate(OUString const & _aName, OUString const & _aTemplateName) const;

            void clear();

            ISubtree  * setSchemaRoot(std::auto_ptr<ISubtree>  _aSchemaRoot);
            ISubtree  * addTemplate(std::auto_ptr<ISubtree>  _aNode, TemplateIdentifier const & aTemplate);
            void  setTemplatesTree(std::auto_ptr<ISubtree>  _aTemplateTree);

            std::auto_ptr<ISubtree> extractSchemaTree();
            std::auto_ptr<ISubtree> extractTemplatesTree();
            std::auto_ptr<INode>    extractTemplateNode(OUString const & _aTemplateName);
        private:
            std::auto_ptr<ISubtree> m_pSchemaTree;
            std::auto_ptr<ISubtree> m_pTemplatesTree;

        };
// -----------------------------------------------------------------------------

    } // namespace backend
// -----------------------------------------------------------------------------

} // namespace configmgr
#endif




