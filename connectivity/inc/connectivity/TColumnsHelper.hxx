/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TColumnsHelper.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 04:58:27 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef CONNECTIVITY_COLUMNSHELPER_HXX
#define CONNECTIVITY_COLUMNSHELPER_HXX

#ifndef _CONNECTIVITY_SDBCX_COLLECTION_HXX_
#include "connectivity/sdbcx/VCollection.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#endif
#ifndef _CONNECTIVITY_SDBCX_IREFRESHABLE_HXX_
#include "connectivity/sdbcx/IRefreshable.hxx"
#endif


namespace connectivity
{
    class OTableHelper;
    class OColumnsHelperImpl;
    /** contains generell column handling to creat default columns and default sql statements.
    */
    class OColumnsHelper : public sdbcx::OCollection
    {
        OColumnsHelperImpl* m_pImpl;
    protected:
        OTableHelper*   m_pTable;

        virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
        virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createEmptyObject();
        virtual sdbcx::ObjectType cloneObject(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _xDescriptor);
        virtual void appendObject( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
        virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);
    public:
        OColumnsHelper( ::cppu::OWeakObject& _rParent
                        ,sal_Bool _bCase
                        ,::osl::Mutex& _rMutex
                        ,const TStringVector &_rVector
                        ,sal_Bool _bUseHardRef = sal_True
                    );
        virtual ~OColumnsHelper();

        /** set the parent of the columns. Can also be <NULL/>.
            @param  _pTable
                The parent.
        */
        inline void setParent(OTableHelper* _pTable) { m_pTable = _pTable;}
    };
}
#endif // CONNECTIVITY_COLUMNSHELPER_HXX

