/*************************************************************************
 *
 *  $RCSfile: ftpcontent.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: abi $ $Date: 2002-10-21 13:13:00 $
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

#ifndef _FTP_FTPCONTENT_HXX
#define _FTP_FTPCONTENT_HXX

#include <vos/ref.hxx>
#include <ucbhelper/contenthelper.hxx>
#include <com/sun/star/ucb/InsertCommandArgument.hpp>
#include <com/sun/star/ucb/XContentCreator.hpp>
#include "ftpurl.hxx"


namespace com { namespace sun { namespace star { namespace beans {
    struct Property;
    struct PropertyValue;
} } } }

namespace com { namespace sun { namespace star { namespace sdbc {
    class XRow;
} } } }


namespace ftp
{

//=========================================================================

// UNO service name for the content.
#define MYUCP_CONTENT_SERVICE_NAME "com.sun.star.ucb.FTPContent"

//=========================================================================

    struct ContentProperties
    {
        ::rtl::OUString aTitle;         // Title
        ::rtl::OUString aContentType;   // ContentType
        sal_Bool        bIsDocument;    // IsDocument
        sal_Bool        bIsFolder;      // IsFolder

        ContentProperties()
            : bIsDocument( sal_True ), bIsFolder( sal_False ) {}
    };

//=========================================================================

    class FTPContentProvider;

//=========================================================================

    class FTPContent
        : public ::ucb::ContentImplHelper,
          public com::sun::star::ucb::XContentCreator
    {
    public:

        FTPContent( const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
                    FTPContentProvider* pProvider,
                    const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XContentIdentifier >& Identifier);

        FTPContent( const ::com::sun::star::uno::Reference<
                    ::com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
                    FTPContentProvider* pProvider,
                    const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XContentIdentifier >& Identifier,
                    const com::sun::star::ucb::ContentInfo& aInfo);


        virtual ~FTPContent();

        // XInterface
        XINTERFACE_DECL()

        // XTypeProvider
        XTYPEPROVIDER_DECL()

        // XServiceInfo
        XSERVICEINFO_DECL()

        // XContent
        virtual rtl::OUString SAL_CALL
        getContentType()
            throw( com::sun::star::uno::RuntimeException );

        // XCommandProcessor
        virtual com::sun::star::uno::Any SAL_CALL
        execute( const com::sun::star::ucb::Command& aCommand,
                 sal_Int32 CommandId,
                 const com::sun::star::uno::Reference<
                 com::sun::star::ucb::XCommandEnvironment >& Environment )
            throw( com::sun::star::uno::Exception,
                   com::sun::star::ucb::CommandAbortedException,
                   com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL
        abort(sal_Int32 CommandId)
            throw( com::sun::star::uno::RuntimeException);

        // XContentCreator
        virtual com::sun::star::uno::Sequence<
        com::sun::star::ucb::ContentInfo > SAL_CALL
        queryCreatableContentsInfo(  )
            throw (com::sun::star::uno::RuntimeException);

        virtual com::sun::star::uno::Reference<
        com::sun::star::ucb::XContent > SAL_CALL
        createNewContent( const com::sun::star::ucb::ContentInfo& Info )
            throw (com::sun::star::uno::RuntimeException);

        // XChild

        virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XInterface > SAL_CALL
        getParent(  )
            throw (::com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL
        setParent( const ::com::sun::star::uno::Reference<
                   ::com::sun::star::uno::XInterface >& Parent )
            throw (::com::sun::star::lang::NoSupportException,
                   ::com::sun::star::uno::RuntimeException);


    private:

        FTPContentProvider *m_pFCP;
        FTPURL              m_aFTPURL;
        bool                m_bInserted;
        bool                m_bTitleSet;
        com::sun::star::ucb::ContentInfo m_aInfo;

        virtual com::sun::star::uno::Sequence< com::sun::star::beans::Property >
        getProperties( const com::sun::star::uno::Reference<
                       com::sun::star::ucb::XCommandEnvironment > & xEnv );


        virtual com::sun::star::uno::Sequence<
        com::sun::star::ucb::CommandInfo>
        getCommands(const com::sun::star::uno::Reference<
                    com::sun::star::ucb::XCommandEnvironment > & xEnv);


        virtual ::rtl::OUString getParentURL();

        com::sun::star::uno::Reference<com::sun::star::sdbc::XRow>
        getPropertyValues(
            const com::sun::star::uno::Sequence<
            com::sun::star::beans::Property>& seqProp,
            const com::sun::star::uno::Reference<
            com::sun::star::ucb::XCommandEnvironment >& Environment
        );

        com::sun::star::uno::Sequence<com::sun::star::uno::Any>
        setPropertyValues(
            const ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyValue>& seqPropVal);

        void insert(const com::sun::star::ucb::InsertCommandArgument&,
                    const com::sun::star::uno::Reference<
                    com::sun::star::ucb::XCommandEnvironment>&);
    };

}

#endif
