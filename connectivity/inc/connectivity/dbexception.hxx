/*************************************************************************
 *
 *  $RCSfile: dbexception.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: fs $ $Date: 2001-04-19 07:04:21 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _DBHELPER_DBEXCEPTION_HXX_
#define _DBHELPER_DBEXCEPTION_HXX_

#ifndef _COM_SUN_STAR_SDBC_SQLEXCEPTION_HPP_
#include <com/sun/star/sdbc/SQLException.hpp>
#endif
//#ifndef _COM_SUN_STAR_SDBC_SQLWARNING_HPP_
//#include <com/sun/star/sdbc/SQLWarning.hpp>
//#endif
//#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
//#include <com/sun/star/sdb/SQLContext.hpp>
//#endif

namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace sdb
            {
                class SQLContext;
                struct SQLErrorEvent;
            }
            namespace sdbc
            {
                class SQLWarning;
            }
        }
    }
}
//.........................................................................
namespace dbtools
{
//.........................................................................

//==============================================================================
//= SQLExceptionInfo - encapsulating the type info of an SQLException-derived class
//==============================================================================

class SQLExceptionInfo
{
public:
    enum TYPE { SQL_EXCEPTION, SQL_WARNING, SQL_CONTEXT, UNDEFINED };

private:
    ::com::sun::star::uno::Any  m_aContent;
    TYPE            m_eType;    // redundant (could be derived from m_aContent.getValueType())

public:
    SQLExceptionInfo();
    SQLExceptionInfo(const ::com::sun::star::sdbc::SQLException& _rError);
    SQLExceptionInfo(const ::com::sun::star::sdbc::SQLWarning& _rError);
    SQLExceptionInfo(const ::com::sun::star::sdb::SQLContext& _rError);
        // these ctors don't make much sense anymore ... Smart-UNO had some kind of self-made rtti for exceptions,
        // so we needed only the first ctor to correctly determine the exception type, but now with UNO3
        // you have to catch _all_ kinds of exceptions derived from  SQLException and use the appropriate ctor ...

    SQLExceptionInfo(const SQLExceptionInfo& _rCopySource);

    SQLExceptionInfo(const ::com::sun::star::sdb::SQLErrorEvent& _rError);
            // use for events got via XSQLErrorListener::errorOccured
    SQLExceptionInfo(const ::com::sun::star::uno::Any& _rError);
            // use with the Reason member of an SQLErrorEvent or with NextElement of an SQLException

    const SQLExceptionInfo& operator=(const ::com::sun::star::sdbc::SQLException& _rError);
    const SQLExceptionInfo& operator=(const ::com::sun::star::sdbc::SQLWarning& _rError);
    const SQLExceptionInfo& operator=(const ::com::sun::star::sdb::SQLContext& _rError);

    sal_Bool    isKindOf(TYPE _eType) const;
        // not just a simple comparisation ! e.g. getType() == SQL_CONTEXT implies isKindOf(SQL_EXCEPTION) == sal_True !
    sal_Bool    isValid() const { return m_eType != UNDEFINED; }
    TYPE        getType() const { return m_eType; }

    operator const ::com::sun::star::sdbc::SQLException*    () const;
    operator const ::com::sun::star::sdbc::SQLWarning*  () const;
    operator const ::com::sun::star::sdb::SQLContext*       () const;

    ::com::sun::star::uno::Any get() const { return m_aContent; }

protected:
    void implDetermineType();
};

//==============================================================================
//= SQLExceptionIteratorHelper - iterating through an SQLException chain
//==============================================================================

class SQLExceptionIteratorHelper
{
public:
    // specifying the type of the elements to include
    enum NODES_INCLUDED { NI_EXCEPTIONS, NI_WARNINGS, NI_CONTEXTINFOS };
        // as ContextInfos are derived from Warnings and Warnings from Exceptions this is sufficient ...

protected:
    const ::com::sun::star::sdbc::SQLException* m_pCurrent;
    SQLExceptionInfo::TYPE          m_eCurrentType;

    NODES_INCLUDED                  m_eMask;

public:
    SQLExceptionIteratorHelper(const ::com::sun::star::sdbc::SQLException* _pStart, NODES_INCLUDED _eMask = NI_EXCEPTIONS);
    SQLExceptionIteratorHelper(const ::com::sun::star::sdbc::SQLWarning* _pStart, NODES_INCLUDED _eMask = NI_EXCEPTIONS);
    SQLExceptionIteratorHelper(const ::com::sun::star::sdb::SQLContext* _pStart, NODES_INCLUDED _eMask = NI_EXCEPTIONS);
        // same note as above for the SQLExceptionInfo ctors
    SQLExceptionIteratorHelper(const SQLExceptionInfo& _rStart, NODES_INCLUDED _eMask = NI_EXCEPTIONS);

    sal_Bool                                    hasMoreElements() const { return (m_pCurrent != NULL); }
    const ::com::sun::star::sdbc::SQLException* next();
    void                                        next(SQLExceptionInfo& _rOutInfo);
};

//==================================================================================
//= StandardExceptions
//==================================================================================
// please use throwFunctionSequenceException instead of this class. The UNO<->* bridges sometimes have
// trouble wrapping exception classes which are not defined in IDLs.
class FunctionSequenceException : public ::com::sun::star::sdbc::SQLException
{
public:
    FunctionSequenceException(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _Context,
        const ::com::sun::star::uno::Any& _Next = ::com::sun::star::uno::Any());
};

//----------------------------------------------------------------------------------
/** throw a function sequence exception
*/
void throwFunctionSequenceException(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _Context,
        const ::com::sun::star::uno::Any& _Next = ::com::sun::star::uno::Any());

//----------------------------------------------------------------------------------
/** throw a generic SQLException, i.e. one with an SQLState of S1000, an ErrorCode of 0 and no NextException
*/
inline void throwGenericSQLException(const ::rtl::OUString& _rMsg, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxSource)
    throw (::com::sun::star::sdbc::SQLException)
{
    throw ::com::sun::star::sdbc::SQLException(_rMsg, _rxSource, ::rtl::OUString::createFromAscii("S1000"), 0, ::com::sun::star::uno::Any());
}
//.........................................................................
}   // namespace dbtools
//.........................................................................

#endif _DBHELPER_DBEXCEPTION_HXX_


/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2001/03/01 17:01:18  fs
 *  operator= for SQLExceptionInfo, new ctor for SQLExceptionIteratorHelper, new next method
 *
 *  Revision 1.2  2000/10/24 15:19:40  oj
 *  make strings unique for lib's
 *
 *  Revision 1.1  2000/10/05 08:56:37  fs
 *  moved the files from unotools to here
 *
 *
 *  Revision 1.0 29.09.00 08:31:15  fs
 ************************************************************************/

