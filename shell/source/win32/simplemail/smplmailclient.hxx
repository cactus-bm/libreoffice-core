/*************************************************************************
 *
 *  $RCSfile: smplmailclient.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: tra $ $Date: 2001-10-15 10:50:05 $
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

#ifndef _SMPLMAILCLIENT_HXX_
#define _SMPLMAILCLIENT_HXX_

//_______________________________________________________________________________________________________________________
//  includes of other projects
//_______________________________________________________________________________________________________________________

#ifndef _CPPUHELPER_COMPBASE1_HXX_
#include <cppuhelper/compbase1.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_SYS_SHELL_XSYSTEMSHELLEXECUTE_HPP_
#include <com/sun/star/system/XSimpleMailClient.hpp>
#endif

#ifndef _SIMPLEMAPI_HXX_
#include "simplemapi.hxx"
#endif

#ifndef _GENERICBUFFER_HXX_
#include "genericbuffer.hxx"
#endif

//----------------------------------------------------------
// class declaration
//----------------------------------------------------------

class CSmplMailClient :
    public  cppu::WeakImplHelper1< com::sun::star::system::XSimpleMailClient >
{
public:
    CSmplMailClient( LHANDLE hMapiSession );
    ~CSmplMailClient( );

    //------------------------------------------------
    //
    //------------------------------------------------

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::system::XSimpleMailMessage > SAL_CALL createSimpleMailMessage(  )
        throw (::com::sun::star::uno::RuntimeException);

    //------------------------------------------------
    //
    //------------------------------------------------

    virtual void SAL_CALL sendSimpleMailMessage( const ::com::sun::star::uno::Reference< ::com::sun::star::system::XSimpleMailMessage >& xSimpleMailMessage, sal_Int32 aFlag )
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

private:
    void validateParameter( const ::com::sun::star::uno::Reference< ::com::sun::star::system::XSimpleMailMessage >& xSimpleMailMessage, sal_Int32 aFlag );
    void initMapiMessage( const ::com::sun::star::uno::Reference< ::com::sun::star::system::XSimpleMailMessage >& xSimpleMailMessage, MapiMessage& aMapiMessage );
    void initMapiSendMailFlags( sal_Int32 aFlags, FLAGS& aMapiFlags );

    inline
    sal_uInt32 calcNumRecipients(
        const ::com::sun::star::uno::Reference< ::com::sun::star::system::XSimpleMailMessage >& xSimpleMailMessage );

    inline
    void initRecipientList( const ::com::sun::star::uno::Sequence< rtl::OUString >& aRecipList,
                            ULONG ulRecipClass,
                            size_t& nPos );

    inline
    void initAttachementList(
        const ::com::sun::star::uno::Reference< ::com::sun::star::system::XSimpleMailMessage >& xSimpleMailMessage );

    rtl::OUString getMapiErrorMsg( ULONG ulMapiError );

private:
    LHANDLE                         m_hMapiSession;
    CSimpleMapi*                    m_pSimpleMapi;
    rtl::OString                    m_Subject;
    rtl::OString                    m_SmtpAddressOriginator;
    std::vector< rtl::OString >     m_RecipsSmtpAddressList;
    std::vector< rtl::OString >     m_AttchmtsSysPathList;
    MapiRecipDesc                   m_MsgOriginator;
    TGenericBuffer< MapiRecipDesc > m_RecipientList;
    TGenericBuffer< MapiFileDesc >  m_AttachementList;
};

#endif
