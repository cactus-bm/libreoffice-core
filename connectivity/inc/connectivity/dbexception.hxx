/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbexception.hxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: rt $ $Date: 2008-01-30 07:45:57 $
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

#ifndef _DBHELPER_DBEXCEPTION_HXX_
#define _DBHELPER_DBEXCEPTION_HXX_

#ifndef _COM_SUN_STAR_SDBC_SQLEXCEPTION_HPP_
#include <com/sun/star/sdbc/SQLException.hpp>
#endif

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

//----------------------------------------------------------------------------------
/** standard SQLStates to be used with an SQLException

    Extend this list whenever you need a new state ...

    @see http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcodbc_error_codes.asp
*/
enum StandardSQLState
{
    SQL_WRONG_PARAMETER_NUMBER,     // 07001
    SQL_INVALID_DESCRIPTOR_INDEX,   // 07009
    SQL_UNABLE_TO_CONNECT,          // 08001
    SQL_NUMERIC_OUT_OF_RANGE,       // 22003
    SQL_INVALID_DATE_TIME,          // 22007
    SQL_INVALID_CURSOR_STATE,       // 24000
    SQL_TABLE_OR_VIEW_EXISTS,       // 42S01
    SQL_TABLE_OR_VIEW_NOT_FOUND,    // 42S02
    SQL_INDEX_ESISTS,               // 42S11
    SQL_INDEX_NOT_FOUND,            // 42S12
    SQL_COLUMN_EXISTS,              // 42S21
    SQL_COLUMN_NOT_FOUND,           // 42S22
    SQL_GENERAL_ERROR,              // HY000
    SQL_INVALID_SQL_DATA_TYPE,      // HY004
    SQL_OPERATION_CANCELED,         // HY008
    SQL_FUNCTION_SEQUENCE_ERROR,    // HY010
    SQL_INVALID_CURSOR_POSITION,    // HY109
    SQL_INVALID_BOOKMARK_VALUE,     // HY111
    SQL_FEATURE_NOT_IMPLEMENTED,    // HYC00
    SQL_FUNCTION_NOT_SUPPORTED,     // IM001
    SQL_CONNECTION_DOES_NOT_EXIST,  // 08003

    SQL_ERROR_UNSPECIFIED = SAL_MAX_ENUM    // special value indicating that an SQLState is not to be specified
};

//==============================================================================
//= Special exception if cancel is pressed in DBA UI
//==============================================================================
enum OOoBaseErrorCode
{
    ParameterInteractionCancelled = 1
};

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

    /** convenience constructor

    If your error processing relies on SQLExceptions, and SQLExceptionInfos, you still may
    need to display an error which consists of a simple message string only.
    In those cases, you can use this constructor, which behaves as if you would have used
    an SQLException containing exactly the given error message.
    */
    SQLExceptionInfo( const ::rtl::OUString& _rSimpleErrorMessage );

    SQLExceptionInfo(const SQLExceptionInfo& _rCopySource);

    SQLExceptionInfo(const ::com::sun::star::sdb::SQLErrorEvent& _rError);
            // use for events got via XSQLErrorListener::errorOccured
    SQLExceptionInfo(const ::com::sun::star::uno::Any& _rError);
            // use with the Reason member of an SQLErrorEvent or with NextElement of an SQLException

    /** prepends a plain error message to the chain of exceptions
        @param  _rSimpleErrorMessage
            the error message to prepend
        @param  _pAsciiSQLState
            the SQLState of the to-be-constructed SQLException, or NULL if this should be defaulted to HY000
        @param  _nErrorCode
            the ErrorCode of the to-be-constructed SQLException
    */
    void    prepend( const ::rtl::OUString& _rErrorMessage, const sal_Char* _pAsciiSQLState = NULL, const sal_Int32 _nErrorCode = 0 );

    /** appends a plain message to the chain of exceptions
        @param  _eType
            the type of exception to append. Must be SQL_EXCEPTION, SQL_WARNING, SQL_CONTEXT, for all other
            values, the behavior is undefined.
        @param  _rErrorMessage
            the message to append
        @param  _pAsciiSQLState
            the SQLState of the exception to append
        @param  _nErrorCode
            the error code of the exception to append
    */
    void    append( TYPE _eType, const ::rtl::OUString& _rErrorMessage, const sal_Char* _pAsciiSQLState = NULL, const sal_Int32 _nErrorCode = 0 );

    /** throws (properly typed) the exception contained in the object
        @precond
            isValid() returns <TRUE/>
        @throws SQLException
        @throws RuntimeException
            if the instance does not contain an SQLException
    */
    void    doThrow();

    const SQLExceptionInfo& operator=(const ::com::sun::star::sdbc::SQLException& _rError);
    const SQLExceptionInfo& operator=(const ::com::sun::star::sdbc::SQLWarning& _rError);
    const SQLExceptionInfo& operator=(const ::com::sun::star::sdb::SQLContext& _rError);
    const SQLExceptionInfo& operator=(const ::com::sun::star::sdb::SQLErrorEvent& _rErrorEvent);
    const SQLExceptionInfo& operator=(const ::com::sun::star::uno::Any& _rCaughtSQLException);

    sal_Bool    isKindOf(TYPE _eType) const;
        // not just a simple comparisation ! e.g. getType() == SQL_CONTEXT implies isKindOf(SQL_EXCEPTION) == sal_True !
    sal_Bool    isValid() const { return m_eType != UNDEFINED; }
    TYPE        getType() const { return m_eType; }

    operator const ::com::sun::star::sdbc::SQLException*    () const;
    operator const ::com::sun::star::sdbc::SQLWarning*      () const;
    operator const ::com::sun::star::sdb::SQLContext*       () const;

    const ::com::sun::star::uno::Any& get() const { return m_aContent; }

protected:
    void implDetermineType();
};

//==============================================================================
//= SQLExceptionIteratorHelper - iterating through an SQLException chain
//==============================================================================

class SQLExceptionIteratorHelper
{
protected:
    const ::com::sun::star::sdbc::SQLException* m_pCurrent;
    SQLExceptionInfo::TYPE                      m_eCurrentType;

public:
    /** constructs an iterator instance from an SQLException

        @param _rChainStart
            the start of the exception chain to iterate. Must live as long as the iterator
            instances lives, at least.
    */
    SQLExceptionIteratorHelper( const ::com::sun::star::sdbc::SQLException& _rChainStart );

    /** constructs an iterator instance from an SQLWarning

        @param _rChainStart
            the start of the exception chain to iterate. Must live as long as the iterator
            instances lives, at least.
    */
    SQLExceptionIteratorHelper( const ::com::sun::star::sdbc::SQLWarning& _rChainStart );

    /** constructs an iterator instance from an SQLContext

        @param _rChainStart
            the start of the exception chain to iterate. Must live as long as the iterator
            instances lives, at least.
    */
    SQLExceptionIteratorHelper( const ::com::sun::star::sdb::SQLContext& _rChainStart );

    /** constructs an iterator instance from an SQLExceptionInfo

        @param _rErrorInfo
            the start of the exception chain to iterate. Must live as long as the iterator
            instances lives, at least.
    */
    SQLExceptionIteratorHelper( const SQLExceptionInfo& _rErrorInfo );

    /** determines whether there are more elements in the exception chain
    */
    sal_Bool                                    hasMoreElements() const { return ( m_pCurrent != NULL ); }

    /** returns the type of the current element in the exception chain
    */
    SQLExceptionInfo::TYPE                      currentType() const { return m_eCurrentType; }

    /** retrieves the current element in the chain, or <NULL/> if the chain has been completely
        traveled.
    */
    const ::com::sun::star::sdbc::SQLException* current() const { return m_pCurrent; }

    /** retrieves the current element in the chain, or <NULL/> if the chain has been completely
        traveled.

        In opposite to the second <member>current</member>, this version allows typed access to
        the respective SQLException.
    */
    void                                        current( SQLExceptionInfo& _out_rInfo ) const;

    /** proceeds to the next element in the chain

        @return the current element in the chain, as <b>before</em> the chain move.
    */
    const ::com::sun::star::sdbc::SQLException* next();

    /** proceeds to the next element in the chain

        In opposite to the second <member>current</member>, this version allows typed access to
        the respective SQLException.
    */
    void                                        next( SQLExceptionInfo& _out_rInfo );
};

//==================================================================================
//= StandardExceptions
//==================================================================================
//----------------------------------------------------------------------------------
/** returns a standard error string for a given SQLState

    @param _eState
        describes the state whose description is to retrieve. Must not be SQL_ERROR_UNSPECIFIED.
    @raises RuntimeException
        in case of an internal error
*/
::rtl::OUString getStandardSQLState( StandardSQLState _eState );

//----------------------------------------------------------------------------------
/** returns a standard ASCII string for a given SQLState

    @param _eState
        describes the state whose description is to retrieve. Must not be SQL_ERROR_UNSPECIFIED.
    @return
        a non-<NULL/> pointer to an ASCII character string denoting the requested SQLState
    @raises RuntimeException
        in case of an internal error
*/
const sal_Char* getStandardSQLStateAscii( StandardSQLState _eState );

//----------------------------------------------------------------------------------
void throwFunctionNotSupportedException(
        const ::rtl::OUString& _rMsg,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _Context,
        const ::com::sun::star::uno::Any& _Next = ::com::sun::star::uno::Any()
    )
    throw ( ::com::sun::star::sdbc::SQLException );

//----------------------------------------------------------------------------------
/** throws an exception with SQL state IM001, saying that a certain function is not supported
*/
void throwFunctionNotSupportedException(
        const sal_Char* _pAsciiFunctionName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext,
        const ::com::sun::star::uno::Any* _pNextException = NULL
    )
    throw ( ::com::sun::star::sdbc::SQLException );

//----------------------------------------------------------------------------------
/** throws a function sequence (HY010) exception
*/
void throwFunctionSequenceException(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _Context,
        const ::com::sun::star::uno::Any& _Next = ::com::sun::star::uno::Any()
    )
    throw ( ::com::sun::star::sdbc::SQLException );

//----------------------------------------------------------------------------------
/** throw a invalid index sqlexception
*/
void throwInvalidIndexException(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _Context,
        const ::com::sun::star::uno::Any& _Next = ::com::sun::star::uno::Any()
    )
    throw ( ::com::sun::star::sdbc::SQLException );

//----------------------------------------------------------------------------------
/** throw a generic SQLException, i.e. one with an SQLState of HY000, an ErrorCode of 0 and no NextException
*/
void throwGenericSQLException(
        const ::rtl::OUString& _rMsg,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxSource
    )
    throw (::com::sun::star::sdbc::SQLException);

//----------------------------------------------------------------------------------
/** throw a generic SQLException, i.e. one with an SQLState of HY000, an ErrorCode of 0 and no NextException
*/
void throwGenericSQLException(
        const ::rtl::OUString& _rMsg,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxSource,
        const ::com::sun::star::uno::Any& _rNextException
    )
    throw (::com::sun::star::sdbc::SQLException);

//----------------------------------------------------------------------------------
/** throw a SQLException with SQLState HYC00 (Optional feature not implemented)
    @param _pAsciiFeatureName
        an ASCII description of the feature which is not implemented. It's recommended that the feature
        name is built from the name of the interface plus its method, for instance "XParameters::updateBinaryStream"
    @param _rxContext
        the context of the exception
    @param _pNextException
        the next exception to chain into the thrown exception, if any
*/
void throwFeatureNotImplementedException(
        const sal_Char* _pAsciiFeatureName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext,
        const ::com::sun::star::uno::Any* _pNextException = NULL
    )
    throw (::com::sun::star::sdbc::SQLException);

//----------------------------------------------------------------------------------
/** throws an SQLException
*/
void throwSQLException(
        const sal_Char* _pAsciiMessage,
        const sal_Char* _pAsciiState,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext,
        const sal_Int32 _nErrorCode = 0,
        const ::com::sun::star::uno::Any* _pNextException = NULL
    )
    throw (::com::sun::star::sdbc::SQLException);

//----------------------------------------------------------------------------------
/** throws an SQLException
*/
void throwSQLException(
        const sal_Char* _pAsciiMessage,
        StandardSQLState _eSQLState,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext,
        const sal_Int32 _nErrorCode = 0,
        const ::com::sun::star::uno::Any* _pNextException = NULL
    )
    throw (::com::sun::star::sdbc::SQLException);

//----------------------------------------------------------------------------------
/** throws an SQLException
*/
void throwSQLException(
        const ::rtl::OUString& _rMessage,
        StandardSQLState _eSQLState,
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxContext,
        const sal_Int32 _nErrorCode = 0,
        const ::com::sun::star::uno::Any* _pNextException = NULL
    )
    throw (::com::sun::star::sdbc::SQLException);

//.........................................................................
}   // namespace dbtools
//.........................................................................

#endif // _DBHELPER_DBEXCEPTION_HXX_


