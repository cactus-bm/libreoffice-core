/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: YUser.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 01:53:42 $
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

#ifndef _CONNECTIVITY_MYSQL_USER_HXX_
#include "mysql/YUser.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include "connectivity/dbtools.hxx"
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include "connectivity/dbexception.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBCX_PRIVILEGE_HPP_
#include <com/sun/star/sdbcx/Privilege.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_PRIVILEGEOBJECT_HPP_
#include <com/sun/star/sdbcx/PrivilegeObject.hpp>
#endif
#ifndef CONNECTIVITY_CONNECTION_HXX
#include "TConnection.hxx"
#endif

using namespace connectivity;
using namespace connectivity::mysql;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
// -------------------------------------------------------------------------
OMySQLUser::OMySQLUser( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection) : connectivity::sdbcx::OUser(sal_True)
                ,m_xConnection(_xConnection)
{
    construct();
}
// -------------------------------------------------------------------------
OMySQLUser::OMySQLUser(   const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection,
                const ::rtl::OUString& _Name
            ) : connectivity::sdbcx::OUser(_Name,sal_True)
                ,m_xConnection(_xConnection)
{
    construct();
}
// -------------------------------------------------------------------------
void OMySQLUser::refreshGroups()
{
}
// -------------------------------------------------------------------------
OUserExtend::OUserExtend(   const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection) : OMySQLUser(_xConnection)
{
    construct();
}
// -------------------------------------------------------------------------
OUserExtend::OUserExtend(   const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection,const ::rtl::OUString& _Name) : OMySQLUser(_xConnection,_Name)
{
    construct();
}
// -------------------------------------------------------------------------
typedef connectivity::sdbcx::OUser  OUser_TYPEDEF;
void OUserExtend::construct()
{
    registerProperty(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PASSWORD),    PROPERTY_ID_PASSWORD,0,&m_Password,::getCppuType(reinterpret_cast< ::rtl::OUString*>(NULL)));
}
// -----------------------------------------------------------------------------
cppu::IPropertyArrayHelper* OUserExtend::createArrayHelper() const
{
    Sequence< Property > aProps;
    describeProperties(aProps);
    return new cppu::OPropertyArrayHelper(aProps);
}
// -------------------------------------------------------------------------
cppu::IPropertyArrayHelper & OUserExtend::getInfoHelper()
{
    return *OUserExtend_PROP::getArrayHelper();
}
typedef connectivity::sdbcx::OUser_BASE OUser_BASE_RBHELPER;
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL OMySQLUser::getPrivileges( const ::rtl::OUString& objName, sal_Int32 objType ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OUser_BASE_RBHELPER::rBHelper.bDisposed);

    sal_Int32 nRights,nRightsWithGrant;
    findPrivilegesAndGrantPrivileges(objName,objType,nRights,nRightsWithGrant);
    return nRights;
}
// -----------------------------------------------------------------------------
void OMySQLUser::findPrivilegesAndGrantPrivileges(const ::rtl::OUString& objName, sal_Int32 objType,sal_Int32& nRights,sal_Int32& nRightsWithGrant) throw(SQLException, RuntimeException)
{
    nRightsWithGrant = nRights = 0;
    // first we need to create the sql stmt to select the privs
    Reference<XDatabaseMetaData> xMeta = m_xConnection->getMetaData();
    ::rtl::OUString sCatalog,sSchema,sTable;
    ::dbtools::qualifiedNameComponents(xMeta,objName,sCatalog,sSchema,sTable,::dbtools::eInDataManipulation);
    Reference<XResultSet> xRes;
    switch(objType)
    {
        case PrivilegeObject::TABLE:
        case PrivilegeObject::VIEW:
            {
                Any aCatalog;
                if ( sCatalog.getLength() )
                    aCatalog <<= sCatalog;
                xRes = xMeta->getTablePrivileges(aCatalog,sSchema,sTable);
            }
            break;

        case PrivilegeObject::COLUMN:
            {
                Any aCatalog;
                if ( sCatalog.getLength() )
                    aCatalog <<= sCatalog;
                xRes = xMeta->getColumnPrivileges(aCatalog,sSchema,sTable,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")));
            }
            break;
    }

    if ( xRes.is() )
    {
        static const ::rtl::OUString sSELECT    = ::rtl::OUString::createFromAscii("SELECT");
        static const ::rtl::OUString sINSERT    = ::rtl::OUString::createFromAscii("INSERT");
        static const ::rtl::OUString sUPDATE    = ::rtl::OUString::createFromAscii("UPDATE");
        static const ::rtl::OUString sDELETE    = ::rtl::OUString::createFromAscii("DELETE");
        static const ::rtl::OUString sREAD      = ::rtl::OUString::createFromAscii("READ");
        static const ::rtl::OUString sCREATE    = ::rtl::OUString::createFromAscii("CREATE");
        static const ::rtl::OUString sALTER     = ::rtl::OUString::createFromAscii("ALTER");
        static const ::rtl::OUString sREFERENCE = ::rtl::OUString::createFromAscii("REFERENCE");
        static const ::rtl::OUString sDROP      = ::rtl::OUString::createFromAscii("DROP");
        static const ::rtl::OUString sYes       = ::rtl::OUString::createFromAscii("YES");

        nRightsWithGrant = nRights = 0;

        Reference<XRow> xCurrentRow(xRes,UNO_QUERY);
        while( xCurrentRow.is() && xRes->next() )
        {
            ::rtl::OUString sGrantee    = xCurrentRow->getString(5);
            ::rtl::OUString sPrivilege  = xCurrentRow->getString(6);
            ::rtl::OUString sGrantable  = xCurrentRow->getString(7);

            if (!m_Name.equalsIgnoreAsciiCase(sGrantee))
                continue;

            if (sPrivilege.equalsIgnoreAsciiCase(sSELECT))
            {
                nRights |= Privilege::SELECT;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::SELECT;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sINSERT))
            {
                nRights |= Privilege::INSERT;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::INSERT;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sUPDATE))
            {
                nRights |= Privilege::UPDATE;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::UPDATE;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sDELETE))
            {
                nRights |= Privilege::DELETE;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::DELETE;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sREAD))
            {
                nRights |= Privilege::READ;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::READ;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sCREATE))
            {
                nRights |= Privilege::CREATE;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::CREATE;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sALTER))
            {
                nRights |= Privilege::ALTER;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::ALTER;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sREFERENCE))
            {
                nRights |= Privilege::REFERENCE;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::REFERENCE;
            }
            else if (sPrivilege.equalsIgnoreAsciiCase(sDROP))
            {
                nRights |= Privilege::DROP;
                if ( sGrantable.equalsIgnoreAsciiCase(sYes) )
                    nRightsWithGrant |= Privilege::DROP;
            }
        }
        ::comphelper::disposeComponent(xRes);
    }
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OMySQLUser::getGrantablePrivileges( const ::rtl::OUString& objName, sal_Int32 objType ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OUser_BASE_RBHELPER::rBHelper.bDisposed);

    sal_Int32 nRights,nRightsWithGrant;
    findPrivilegesAndGrantPrivileges(objName,objType,nRights,nRightsWithGrant);
    return nRightsWithGrant;
}
// -------------------------------------------------------------------------
void SAL_CALL OMySQLUser::grantPrivileges( const ::rtl::OUString& objName, sal_Int32 objType, sal_Int32 objPrivileges ) throw(SQLException, RuntimeException)
{
    if ( objType != PrivilegeObject::TABLE )
        ::dbtools::throwSQLException( "Privilege not granted: Only table privileges can be granted", "01007", *this );

    ::osl::MutexGuard aGuard(m_aMutex);

    ::rtl::OUString sPrivs = getPrivilegeString(objPrivileges);
    if(sPrivs.getLength())
    {
        ::rtl::OUString sGrant;
        sGrant += ::rtl::OUString::createFromAscii("GRANT ");
        sGrant += sPrivs;
        sGrant += ::rtl::OUString::createFromAscii(" ON ");
        Reference<XDatabaseMetaData> xMeta = m_xConnection->getMetaData();
        sGrant += ::dbtools::quoteTableName(xMeta,objName,::dbtools::eInDataManipulation);
        sGrant += ::rtl::OUString::createFromAscii(" TO ");
        sGrant += m_Name;

        Reference<XStatement> xStmt = m_xConnection->createStatement();
        if(xStmt.is())
            xStmt->execute(sGrant);
        ::comphelper::disposeComponent(xStmt);
    }
}
// -------------------------------------------------------------------------
void SAL_CALL OMySQLUser::revokePrivileges( const ::rtl::OUString& objName, sal_Int32 objType, sal_Int32 objPrivileges ) throw(SQLException, RuntimeException)
{
    if ( objType != PrivilegeObject::TABLE )
        ::dbtools::throwSQLException( "Privilege not revoked: Only table privileges can be revoked", "01006", *this );

    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OUser_BASE_RBHELPER::rBHelper.bDisposed);
    ::rtl::OUString sPrivs = getPrivilegeString(objPrivileges);
    if(sPrivs.getLength())
    {
        ::rtl::OUString sGrant;
        sGrant += ::rtl::OUString::createFromAscii("REVOKE ");
        sGrant += sPrivs;
        sGrant += ::rtl::OUString::createFromAscii(" ON ");
        Reference<XDatabaseMetaData> xMeta = m_xConnection->getMetaData();
        sGrant += ::dbtools::quoteTableName(xMeta,objName,::dbtools::eInDataManipulation);
        sGrant += ::rtl::OUString::createFromAscii(" FROM ");
        sGrant += m_Name;

        Reference<XStatement> xStmt = m_xConnection->createStatement();
        if(xStmt.is())
            xStmt->execute(sGrant);
        ::comphelper::disposeComponent(xStmt);
    }
}
// -----------------------------------------------------------------------------
// XUser
void SAL_CALL OMySQLUser::changePassword( const ::rtl::OUString& /*oldPassword*/, const ::rtl::OUString& newPassword ) throw(SQLException, RuntimeException)
{
    ::osl::MutexGuard aGuard(m_aMutex);
    checkDisposed(OUser_BASE_RBHELPER::rBHelper.bDisposed);
    ::rtl::OUString sAlterPwd;
    sAlterPwd = ::rtl::OUString::createFromAscii("SET PASSWORD FOR ");
    sAlterPwd += m_Name;
    sAlterPwd += ::rtl::OUString::createFromAscii("@\"%\" = PASSWORD('") ;
    sAlterPwd += newPassword;
    sAlterPwd += ::rtl::OUString::createFromAscii("')") ;


    Reference<XStatement> xStmt = m_xConnection->createStatement();
    if ( xStmt.is() )
    {
        xStmt->execute(sAlterPwd);
        ::comphelper::disposeComponent(xStmt);
    }
}
// -----------------------------------------------------------------------------
::rtl::OUString OMySQLUser::getPrivilegeString(sal_Int32 nRights) const
{
    ::rtl::OUString sPrivs;
    if((nRights & Privilege::INSERT) == Privilege::INSERT)
        sPrivs += ::rtl::OUString::createFromAscii("INSERT");

    if((nRights & Privilege::DELETE) == Privilege::DELETE)
    {
        if(sPrivs.getLength())
            sPrivs += ::rtl::OUString::createFromAscii(",");
        sPrivs += ::rtl::OUString::createFromAscii("DELETE");
    }

    if((nRights & Privilege::UPDATE) == Privilege::UPDATE)
    {
        if(sPrivs.getLength())
            sPrivs += ::rtl::OUString::createFromAscii(",");
        sPrivs += ::rtl::OUString::createFromAscii("UPDATE");
    }

    if((nRights & Privilege::ALTER) == Privilege::ALTER)
    {
        if(sPrivs.getLength())
            sPrivs += ::rtl::OUString::createFromAscii(",");
        sPrivs += ::rtl::OUString::createFromAscii("ALTER");
    }

    if((nRights & Privilege::SELECT) == Privilege::SELECT)
    {
        if(sPrivs.getLength())
            sPrivs += ::rtl::OUString::createFromAscii(",");
        sPrivs += ::rtl::OUString::createFromAscii("SELECT");
    }

    if((nRights & Privilege::REFERENCE) == Privilege::REFERENCE)
    {
        if(sPrivs.getLength())
            sPrivs += ::rtl::OUString::createFromAscii(",");
        sPrivs += ::rtl::OUString::createFromAscii("REFERENCES");
    }

    return sPrivs;
}
// -----------------------------------------------------------------------------

