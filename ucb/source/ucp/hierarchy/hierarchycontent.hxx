/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: hierarchycontent.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 15:46:05 $
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

#ifndef _HIERARCHYCONTENT_HXX
#define _HIERARCHYCONTENT_HXX

#include <list>

#ifndef _RTL_REF_HXX_
#include <rtl/ref.hxx>
#endif

#ifndef _COM_SUN_STAR_UCB_XCONTENTCREATOR_HPP_
#include <com/sun/star/ucb/XContentCreator.hpp>
#endif

#ifndef _UCBHELPER_CONTENTHELPER_HXX
#include <ucbhelper/contenthelper.hxx>
#endif

#ifndef _HIERARCHYDATA_HXX
#include "hierarchydata.hxx"
#endif
#ifndef _HIERARCHYPROVIDER_HXX
#include "hierarchyprovider.hxx"
#endif

namespace com { namespace sun { namespace star { namespace beans {
    struct Property;
    struct PropertyValue;
} } } }

namespace com { namespace sun { namespace star { namespace sdbc {
    class XRow;
} } } }

namespace com { namespace sun { namespace star { namespace ucb {
    struct TransferInfo;
} } } }

namespace hierarchy_ucp
{

//=========================================================================

#define HIERARCHY_ROOT_FOLDER_CONTENT_SERVICE_NAME \
                            "com.sun.star.ucb.HierarchyRootFolderContent"
#define HIERARCHY_FOLDER_CONTENT_SERVICE_NAME \
                            "com.sun.star.ucb.HierarchyFolderContent"
#define HIERARCHY_LINK_CONTENT_SERVICE_NAME \
                            "com.sun.star.ucb.HierarchyLinkContent"

//=========================================================================

class HierarchyContentProperties
{
public:
    HierarchyContentProperties() {};

    HierarchyContentProperties( const HierarchyEntryData::Type & rType )
    : m_aData( rType ),
      m_aContentType( rType == HierarchyEntryData::FOLDER
        ? rtl::OUString::createFromAscii( HIERARCHY_FOLDER_CONTENT_TYPE )
        : rtl::OUString::createFromAscii( HIERARCHY_LINK_CONTENT_TYPE ) ) {}

    HierarchyContentProperties( const HierarchyEntryData & rData )
    : m_aData( rData ),
      m_aContentType( rData.getType() == HierarchyEntryData::FOLDER
        ? rtl::OUString::createFromAscii( HIERARCHY_FOLDER_CONTENT_TYPE )
        : rtl::OUString::createFromAscii( HIERARCHY_LINK_CONTENT_TYPE ) ) {}

    const rtl::OUString & getName() const { return m_aData.getName(); }
    void setName( const rtl::OUString & rName ) { m_aData.setName( rName ); };

    const rtl::OUString & getTitle() const { return m_aData.getTitle(); }
    void setTitle( const rtl::OUString & rTitle )
    { m_aData.setTitle( rTitle ); };

    const rtl::OUString & getTargetURL() const
    { return m_aData.getTargetURL(); }
    void setTargetURL( const rtl::OUString & rURL )
    { m_aData.setTargetURL( rURL ); };

    const rtl::OUString & getContentType() const { return m_aContentType; }

    sal_Bool getIsFolder() const
    { return m_aData.getType() == HierarchyEntryData::FOLDER; }

    sal_Bool getIsDocument() const { return !getIsFolder(); }

    const HierarchyEntryData & getHierarchyEntryData() const { return m_aData; }

private:
    HierarchyEntryData m_aData;
    rtl::OUString m_aContentType;
};

//=========================================================================

class HierarchyContentProvider;

class HierarchyContent : public ::ucb::ContentImplHelper,
                         public com::sun::star::ucb::XContentCreator
{
    enum ContentKind  { LINK, FOLDER, ROOT };
    enum ContentState { TRANSIENT,  // created via CreateNewContent,
                                       // but did not process "insert" yet
                        PERSISTENT, // processed "insert"
                        DEAD        // processed "delete"
                      };

    HierarchyContentProperties m_aProps;
    ContentKind                m_eKind;
    ContentState               m_eState;
    HierarchyContentProvider*  m_pProvider;
    bool                       m_bCheckedReadOnly;
    bool                       m_bIsReadOnly;

private:
    HierarchyContent(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            HierarchyContentProvider* pProvider,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier,
            const HierarchyContentProperties& rProps );
    HierarchyContent(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            HierarchyContentProvider* pProvider,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier,
            const com::sun::star::ucb::ContentInfo& Info );

    virtual com::sun::star::uno::Sequence< com::sun::star::beans::Property >
    getProperties( const com::sun::star::uno::Reference<
                    com::sun::star::ucb::XCommandEnvironment > & xEnv );
    virtual com::sun::star::uno::Sequence< com::sun::star::ucb::CommandInfo >
    getCommands( const com::sun::star::uno::Reference<
                    com::sun::star::ucb::XCommandEnvironment > & xEnv );
    virtual ::rtl::OUString getParentURL();

    static sal_Bool hasData(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            HierarchyContentProvider* pProvider,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier );
    sal_Bool hasData(
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier )
    { return hasData( m_xSMgr, m_pProvider, Identifier ); }
    static sal_Bool loadData(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            HierarchyContentProvider* pProvider,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier,
            HierarchyContentProperties& rProps );
    sal_Bool storeData();
    sal_Bool renameData( const com::sun::star::uno::Reference<
                            com::sun::star::ucb::XContentIdentifier >& xOldId,
                         const com::sun::star::uno::Reference<
                            com::sun::star::ucb::XContentIdentifier >& xNewId );
    sal_Bool removeData();

    void setKind( const com::sun::star::uno::Reference<
                    com::sun::star::ucb::XContentIdentifier >& Identifier );

    bool isReadOnly();

    sal_Bool isFolder() const { return ( m_eKind > LINK ); }

    ::com::sun::star::uno::Reference<
        ::com::sun::star::ucb::XContentIdentifier >
    makeNewIdentifier( const rtl::OUString& rTitle );

    typedef rtl::Reference< HierarchyContent > HierarchyContentRef;
    typedef std::list< HierarchyContentRef > HierarchyContentRefList;
    void queryChildren( HierarchyContentRefList& rChildren );

    sal_Bool exchangeIdentity(
                const ::com::sun::star::uno::Reference<
                        ::com::sun::star::ucb::XContentIdentifier >& xNewId );

    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRow >
    getPropertyValues( const ::com::sun::star::uno::Sequence<
                             ::com::sun::star::beans::Property >& rProperties );
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >
    setPropertyValues(
            const ::com::sun::star::uno::Sequence<
                    ::com::sun::star::beans::PropertyValue >& rValues,
            const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XCommandEnvironment > & xEnv )
        throw( ::com::sun::star::uno::Exception );

    void insert( sal_Int32 nNameClashResolve,
                 const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XCommandEnvironment > & xEnv )
        throw( ::com::sun::star::uno::Exception );

    void destroy( sal_Bool bDeletePhysical,
                  const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XCommandEnvironment > & xEnv )
        throw( ::com::sun::star::uno::Exception );

    void transfer( const ::com::sun::star::ucb::TransferInfo& rInfo,
                   const ::com::sun::star::uno::Reference<
                    ::com::sun::star::ucb::XCommandEnvironment > & xEnv )
        throw( ::com::sun::star::uno::Exception );

public:
    // Create existing content. Fail, if not already exists.
    static HierarchyContent* create(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            HierarchyContentProvider* pProvider,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier );

    // Create new content. Fail, if already exists.
    static HierarchyContent* create(
            const com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory >& rxSMgr,
            HierarchyContentProvider* pProvider,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier >& Identifier,
            const com::sun::star::ucb::ContentInfo& Info );

    virtual ~HierarchyContent();

    // XInterface
    XINTERFACE_DECL()

    // XTypeProvider
    XTYPEPROVIDER_DECL()

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL
    getImplementationName()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
    getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException );

    // XContent
    virtual rtl::OUString SAL_CALL
    getContentType()
        throw( com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContentIdentifier > SAL_CALL
    getIdentifier()
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
    abort( sal_Int32 CommandId )
        throw( com::sun::star::uno::RuntimeException );

    //////////////////////////////////////////////////////////////////////
    // Additional interfaces
    //////////////////////////////////////////////////////////////////////

    // XContentCreator
    virtual com::sun::star::uno::Sequence<
                com::sun::star::ucb::ContentInfo > SAL_CALL
    queryCreatableContentsInfo()
        throw( com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XContent > SAL_CALL
    createNewContent( const com::sun::star::ucb::ContentInfo& Info )
        throw( com::sun::star::uno::RuntimeException );

    //////////////////////////////////////////////////////////////////////
    // Non-interface methods.
    //////////////////////////////////////////////////////////////////////

    static ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRow >
    getPropertyValues( const ::com::sun::star::uno::Reference<
                        ::com::sun::star::lang::XMultiServiceFactory >& rSMgr,
                       const ::com::sun::star::uno::Sequence<
                           ::com::sun::star::beans::Property >& rProperties,
                       const HierarchyContentProperties& rData,
                       HierarchyContentProvider* pProvider,
                       const ::rtl::OUString& rContentId );
};

} // namespace hierarchy_ucp

#endif /* !_HIERARCHYCONTENT_HXX */
