/*************************************************************************
 *
 *  $RCSfile: ATables.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: vg $ $Date: 2005-03-10 15:36:29 $
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
#ifndef _CONNECTIVITY_ADO_TABLES_HXX_
#define _CONNECTIVITY_ADO_TABLES_HXX_

#ifndef _CONNECTIVITY_SDBCX_COLLECTION_HXX_
#include "connectivity/sdbcx/VCollection.hxx"
#endif
#ifndef _CONNECTIVITY_ADO_AWRAPADOX_HXX_
#include "ado/Awrapadox.hxx"
#endif
#ifndef _CONNECTIVITY_ADO_CATALOG_HXX_
#include "ado/ACatalog.hxx"
#endif

namespace connectivity
{
    namespace ado
    {
        class OCatalog;
        class OTables : public sdbcx::OCollection
        {
            WpADOTables m_aCollection;
            OCatalog*   m_pCatalog;
        protected:
            virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
            virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException);
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createEmptyObject();
            void setComments(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
            // return a object which is the copy of the descriptor
            virtual sdbcx::ObjectType cloneObject(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _xDescriptor);
            virtual void appendObject( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
            virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);
        public:
            OTables(OCatalog* _pParent, ::osl::Mutex& _rMutex,
                const TStringVector &_rVector,
                const WpADOTables& _rCollection,
                sal_Bool _bCase) : sdbcx::OCollection(*_pParent,_bCase,_rMutex,_rVector)
                ,m_pCatalog(_pParent)
                ,m_aCollection(_rCollection)
            {
                OSL_ENSURE(m_aCollection.IsValid(),"Collection isn't valid");
            }
            void appendNew(const ::rtl::OUString& _rsNewTable);
        };
    }
}
#endif // _CONNECTIVITY_ADO_TABLES_HXX_

