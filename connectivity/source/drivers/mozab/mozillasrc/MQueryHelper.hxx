/*************************************************************************
 *
 *  $RCSfile: MQueryHelper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: dkenny $ $Date: 2001-11-15 10:01:12 $
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

#ifndef _CONNECTIVITY_MAB_QUERYHELPER_HXX_
#define _CONNECTIVITY_MAB_QUERYHELPER_HXX_

#include <MNSInclude.hxx>

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _OSL_CONDITN_HXX_
#include <osl/conditn.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif


namespace connectivity
{
    namespace mozab
    {
        class MQueryHelperResultEntry
        {
        private:
            mutable ::osl::Mutex        m_aMutex;

            DECLARE_STL_USTRINGACCESS_MAP(::rtl::OUString,fieldMap);

            fieldMap    m_Fields;

        public:
            MQueryHelperResultEntry();
            ~MQueryHelperResultEntry();

            void insert( rtl::OUString &key, rtl::OUString &value );
            rtl::OUString getValue( const rtl::OUString &key ) const;
        };

        class MQueryHelper : public nsIAbDirectoryQueryResultListener
        {
        private:
            typedef std::vector< MQueryHelperResultEntry* > resultsArray;

            mutable ::osl::Mutex        m_aMutex;
            ::osl::Condition    m_aCondition;
            resultsArray        m_aResults;
            sal_Int32           m_nIndex;
            sal_Bool            m_bHasMore;
            sal_Bool            m_bAtEnd;
            sal_Bool            m_bErrorCondition;
            sal_Bool            m_bQueryComplete;

            void            append(MQueryHelperResultEntry* resEnt );

            void            clear_results();

            void            clearResultOrComplete();
            void            notifyResultOrComplete();
            sal_Bool        waitForResultOrComplete( ::rtl::OUString& _rError );

        public:

            NS_DECL_ISUPPORTS
            NS_DECL_NSIABDIRECTORYQUERYRESULTLISTENER

                                            MQueryHelper();

                                            ~MQueryHelper();

            void                            reset();

            void                            rewind();

            MQueryHelperResultEntry*   next( ::rtl::OUString& _rError );

            MQueryHelperResultEntry*   getByIndex( sal_Int32 nRow, ::rtl::OUString& _rError );

            sal_Bool                   isError() const;

            sal_Bool                   hasMore() const;

            sal_Bool                   atEnd() const;

            sal_Bool                   queryComplete() const;

            sal_Bool                   waitForQueryComplete( rtl::OUString& _rError );

            sal_Bool                   waitForRow( sal_Int32 rowNum, rtl::OUString& _rError );

            sal_Int32                  getResultCount() const;

            sal_uInt32                 getRealCount() const;

            void                       notifyQueryError() ;
        };
    }
}
#endif // _CONNECTIVITY_MAB_QUERYHELPER_HXX_

