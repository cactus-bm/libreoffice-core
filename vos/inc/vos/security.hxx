/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: security.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:03:58 $
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

#ifndef _VOS_SECURITY_HXX_
#define _VOS_SECURITY_HXX_

#ifndef _VOS_OBJECT_HXX_
#   include <vos/object.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#   include <rtl/ustring.hxx>
#endif
#ifndef _OSL_SECURITY_H_
#   include <osl/security.h>
#endif

#ifdef _USE_NAMESPACE
namespace vos
{
#endif

/** capsulate security informations for one user.
    A object of this class is used to execute a process with the rights an
    security options of a scecified user.
    @see OProcess::executeProcess
    @author  Bernd Hofner
    @version 1.0

*/
class OSecurity : public OObject
{
    VOS_DECLARE_CLASSINFO(VOS_NAMESPACE(OSecurity, vos));

public:
    /// constructor
    OSecurity();
    /// destructor
    virtual ~OSecurity();
    /** get the security information for one user.
        The underlying operating system is asked for this information.
        @param strName [in] denotes the name of the user
        @param strPasswd [in] denotes the password of this user
        @return True, if the specified user is known by the underlying operating system,
        otherwise False
    */
    sal_Bool SAL_CALL logonUser(const ::rtl::OUString& strName,
                                const ::rtl::OUString& strPasswd);
    /** get the security information for one user.
        This method will try to login the user at the denoted file server.
        If a network resource named \\server\username exists and this resource
        could be connected by this user, the methos will return true and getHomeDir
        will return \\server\username.
        @param strName [in] denotes the name of the user
        @param strPasswd [in] denotes the password of this user
        @return True, if the specified user is known by file server and the
        could be connected, otherwise False
    */
    sal_Bool SAL_CALL logonUser(const ::rtl::OUString& strName,
                                const ::rtl::OUString& strPasswd,
                                const ::rtl::OUString& strFileServer);

    /** get the ident of the logged in user.
        @param strName [out] is the buffer which returns the name
        @param max [in] is the size of this buffer
        @return True, if any user is successfuly logged in, otherwise False
    */
    sal_Bool SAL_CALL getUserIdent(::rtl::OUString& strIdent) const;

    /** get the name of the logged in user.
        @param strName [out] is the buffer which returns the name
        @param max [in] is the size of this buffer
        @return True, if any user is successfuly logged in, otherwise False
    */
    sal_Bool SAL_CALL getUserName(::rtl::OUString& strName) const;

    /** get the home directory of the logged in user.
        @param strDirectory [out] is the buffer which returns the directory name
        @param max [in] is the size of this buffer
        @return True, if any user is successfuly logged in, otherwise False
    */
    sal_Bool SAL_CALL getHomeDir(::rtl::OUString& strDirectory) const;

    /** get the directory for configuration data of the logged in user.
        @param strDirectory [out] is the buffer which returns the directory name
        @param max [in] is the size of this buffer
        @return True, if any user is successfuly logged in, otherwise False
    */
    sal_Bool SAL_CALL getConfigDir(::rtl::OUString& strDirectory) const;

    /** Query if the user who is logged inhas administrator rigths.
        @return True, if the user has administrator rights, otherwise false.
    */
    sal_Bool SAL_CALL isAdministrator() const;

    virtual SAL_CALL operator oslSecurity() const;

protected:

    oslSecurity m_oslSecurity;
};

#ifdef _USE_NAMESPACE
}
#endif

#endif  // _VOS_SECURITY_HXX_

