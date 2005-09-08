/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: LTables.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 05:48:22 $
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

#ifndef _CONNECTIVITY_EVOAB_LTABLES_HXX_
#define _CONNECTIVITY_EVOAB_LTABLES_HXX_

#ifndef _CONNECTIVITY_FILE_TABLES_HXX_
#include "file/FTables.hxx"
#endif

namespace connectivity
{
    namespace evoab
    {
                //      namespace ::com::sun::star::sdbcx             = ::com::sun::star::sdbcx;
        typedef file::OTables OEvoabTables_BASE;

        class OEvoabTables : public OEvoabTables_BASE
        {
        protected:
            virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
        public:
            OEvoabTables(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >& _rMetaData,::cppu::OWeakObject& _rParent, ::osl::Mutex& _rMutex,
                const TStringVector &_rVector) : OEvoabTables_BASE(_rMetaData,_rParent,_rMutex,_rVector)
            {}
        };
    }
}
#endif // _CONNECTIVITY_EVOAB_LTABLES_HXX_

