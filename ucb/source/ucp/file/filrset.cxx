/*************************************************************************
 *
 *  $RCSfile: filrset.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: abi $ $Date: 2001-06-29 15:00:12 $
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
#ifndef _COM_SUN_STAR_UCB_WELCOMEDYNAMICRESULTSETSTRUCT_HPP_
#include <com/sun/star/ucb/WelcomeDynamicResultSetStruct.hpp>
#endif
#ifndef _FILID_HXX_
#include "filid.hxx"
#endif
#ifndef _SHELL_HXX_
#include "shell.hxx"
#endif
#ifndef _FILPRP_HXX_
#include "filprp.hxx"
#endif
#ifndef _FILRSET_HXX_
#include "filrset.hxx"
#endif

#ifndef _COM_SUN_STAR_UCB_OPENMODE_HPP_
#include <com/sun/star/ucb/OpenMode.hpp>
#endif
#ifndef _PROV_HXX_
#include "prov.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_LISTACTIONTYPE_HPP_
#include <com/sun/star/ucb/ListActionType.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XSOURCEINITIALIZATION_HPP_
#include <com/sun/star/ucb/XSourceInitialization.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCACHEDDYNAMICRESULTSETSTUBFACTORY_HPP_
#include <com/sun/star/ucb/XCachedDynamicResultSetStubFactory.hpp>
#endif

#ifndef _UCBHELPER_RESULTSETMETADATA_HXX
#include <ucbhelper/resultsetmetadata.hxx>
#endif

using namespace fileaccess;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::ucb;


XResultSet_impl::XResultSet_impl( shell* pMyShell,
                                  const rtl::OUString& aUnqPath,
                                  sal_Int32 OpenMode,
                                  const Sequence< beans::Property >& seq,
                                  const Sequence< NumberedSortingInfo >& seqSort )
    : m_pMyShell( pMyShell ),
      m_xProvider( pMyShell->m_pProvider ),
      m_sProperty( seq ),
      m_sSortingInfo( seqSort ),
      m_aBaseDirectory( aUnqPath ),
      m_aFolder( aUnqPath ),
      m_pDisposeEventListeners( 0 ),
      m_pRowCountListeners( 0 ),
      m_pIsFinalListeners( 0 ),
      m_nRow( -1 ),
      m_bStatic( false ),
      m_bRowCountFinal( false ),
      m_nOpenMode( OpenMode ),
      m_nErrorCode( TASKHANDLER_NO_ERROR ),
      m_nMinorErrorCode( TASKHANDLER_NO_ERROR )
{
    m_bFaked = m_pMyShell->m_bFaked && m_aBaseDirectory.compareToAscii( "//./" ) == 0;
    if( m_bFaked )
    {
        m_nIsOpen = true;
    }
    else
    {
        osl::FileBase::RC err = m_aFolder.open();
        if(  err != osl::FileBase::E_None )
        {
            m_nIsOpen = false;
            m_aFolder.close();

            m_nErrorCode = TASKHANDLING_OPEN_FOR_DIRECTORYLISTING;
            m_nMinorErrorCode = err;
        }
        else
            m_nIsOpen = true;
    }

    m_pMyShell->registerNotifier( m_aBaseDirectory,this );
}


XResultSet_impl::~XResultSet_impl()
{
    m_pMyShell->deregisterNotifier( m_aBaseDirectory,this );

    if( m_nIsOpen && ! m_bFaked )
        m_aFolder.close();

    delete m_pDisposeEventListeners;
    delete m_pRowCountListeners;
    delete m_pIsFinalListeners;
}



sal_Int32 SAL_CALL XResultSet_impl::CtorSuccess()
{
    return m_nErrorCode;
}



sal_Int32 SAL_CALL XResultSet_impl::getMinorError()
{
    return m_nMinorErrorCode;
}


void SAL_CALL
XResultSet_impl::acquire(
    void )
    throw( RuntimeException )
{
    OWeakObject::acquire();
}


void SAL_CALL
XResultSet_impl::release(
    void )
    throw( RuntimeException )
{
    OWeakObject::release();
}



Any SAL_CALL
XResultSet_impl::queryInterface(
    const Type& rType )
    throw( RuntimeException )
{
    Any aRet = cppu::queryInterface( rType,
                                          SAL_STATIC_CAST( lang::XComponent*, this),
                                          SAL_STATIC_CAST( lang::XEventListener*, this),
                                          SAL_STATIC_CAST( sdbc::XRow*, this),
                                          SAL_STATIC_CAST( sdbc::XResultSet*, this),
                                          SAL_STATIC_CAST( sdbc::XCloseable*, this),
                                          SAL_STATIC_CAST( sdbc::XResultSetMetaDataSupplier*, this),
                                          SAL_STATIC_CAST( beans::XPropertySet*, this ),
                                          SAL_STATIC_CAST( XContentAccess*, this),
                                          SAL_STATIC_CAST( XDynamicResultSet*,this) );
    return aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType );
}


void SAL_CALL
XResultSet_impl::disposing( const lang::EventObject& Source )
    throw( RuntimeException )
{
    // To do, but what
}


XTYPEPROVIDER_IMPL_10( XResultSet_impl,
                       lang::XTypeProvider,
                       lang::XTypeProvider,
                       lang::XEventListener,
                       sdbc::XRow,
                       sdbc::XResultSet,
                       XDynamicResultSet,
                       sdbc::XCloseable,
                       sdbc::XResultSetMetaDataSupplier,
                       beans::XPropertySet,
                       XContentAccess )


void SAL_CALL
XResultSet_impl::addEventListener(
    const Reference< lang::XEventListener >& Listener )
    throw( RuntimeException )
{
    vos::OGuard aGuard( m_aMutex );

    if ( ! m_pDisposeEventListeners )
        m_pDisposeEventListeners =
            new cppu::OInterfaceContainerHelper( m_aEventListenerMutex );

    m_pDisposeEventListeners->addInterface( Listener );
}


void SAL_CALL
XResultSet_impl::removeEventListener(
    const Reference< lang::XEventListener >& Listener )
    throw( RuntimeException )
{
    vos::OGuard aGuard( m_aMutex );

    if ( m_pDisposeEventListeners )
        m_pDisposeEventListeners->removeInterface( Listener );
}



void SAL_CALL
XResultSet_impl::dispose()
    throw( RuntimeException )
{
    vos::OGuard aGuard( m_aMutex );

    lang::EventObject aEvt;
    aEvt.Source = static_cast< lang::XComponent * >( this );

    if ( m_pDisposeEventListeners && m_pDisposeEventListeners->getLength() )
    {
        m_pDisposeEventListeners->disposeAndClear( aEvt );
    }
    if( m_pRowCountListeners && m_pRowCountListeners->getLength() )
    {
        m_pRowCountListeners->disposeAndClear( aEvt );
    }
    if( m_pIsFinalListeners && m_pIsFinalListeners->getLength() )
    {
        m_pIsFinalListeners->disposeAndClear( aEvt );
    }
}



void XResultSet_impl::rowCountChanged()
{
    sal_Int32 aOldValue,aNewValue;
    Sequence< Reference< XInterface > > seq;
    {
        vos::OGuard aGuard( m_aMutex );
        if( m_pRowCountListeners )
            seq = m_pRowCountListeners->getElements();
        aNewValue = m_aItems.size();
        aOldValue = aNewValue-1;
    }
    beans::PropertyChangeEvent aEv;
    aEv.PropertyName = rtl::OUString::createFromAscii( "RowCount" );
    aEv.Further = false;
    aEv.PropertyHandle = -1;
    aEv.OldValue <<= aOldValue;
    aEv.NewValue <<= aNewValue;
    for( sal_Int32 i = 0; i < seq.getLength(); ++i )
    {
        Reference< beans::XPropertyChangeListener > listener( seq[i],UNO_QUERY );
        if( listener.is() )
            listener->propertyChange( aEv );
    }
}


void XResultSet_impl::isFinalChanged()
{
    Sequence< Reference< XInterface > > seq;
    {
        vos::OGuard aGuard( m_aMutex );
        if( m_pIsFinalListeners )
            seq = m_pIsFinalListeners->getElements();
        m_bRowCountFinal = true;
    }
    beans::PropertyChangeEvent aEv;
    aEv.PropertyName = rtl::OUString::createFromAscii( "IsRowCountFinal" );
    aEv.Further = false;
    aEv.PropertyHandle = -1;
    sal_Bool fval = false;
    sal_Bool tval = true;
    aEv.OldValue <<= fval;
    aEv.NewValue <<= tval;
    for( sal_Int32 i = 0; i < seq.getLength(); ++i )
    {
        Reference< beans::XPropertyChangeListener > listener( seq[i],UNO_QUERY );
        if( listener.is() )
            listener->propertyChange( aEv );
    }
}


sal_Bool SAL_CALL
XResultSet_impl::OneMoreFaked( void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    sal_uInt32 k = m_aItems.size();
    if( k < m_pMyShell->m_vecMountPoint.size() &&
        ( m_nOpenMode == OpenMode::ALL || m_nOpenMode == OpenMode::FOLDERS ) )
    {
        sal_Bool IsRegular;
        rtl::OUString aUnqPath = m_pMyShell->m_vecMountPoint[k].m_aDirectory;
        osl::DirectoryItem aDirItem;
        osl::DirectoryItem::get( aUnqPath,aDirItem );
        Reference< sdbc::XRow > aRow = m_pMyShell->getv( -1,this,m_sProperty,aDirItem,aUnqPath,IsRegular );
        vos::OGuard aGuard( m_aMutex );
        m_aItems.push_back( aRow );
        m_aIdents.push_back( Reference< XContentIdentifier >() );
        m_aUnqPath.push_back( aUnqPath );
        rowCountChanged();
        return true;
    }

    return false;
}


sal_Bool SAL_CALL
XResultSet_impl::OneMore(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    if( ! m_nIsOpen ) return false;

    if( m_bFaked )
        return OneMoreFaked();

    osl::FileBase::RC err;
    sal_Bool IsRegular;
    rtl::OUString aUnqPath;
    osl::DirectoryItem  m_aDirIte;
    Reference< sdbc::XRow > aRow;

    while( true )
    {
        err = m_aFolder.getNextItem( m_aDirIte );

        if( err == osl::FileBase::E_NOENT || err == osl::FileBase::E_INVAL )
        {
            m_aFolder.close();
            isFinalChanged();
            return ( m_nIsOpen = false );
        }
        else if( err == osl::FileBase::E_None )
        {
            aRow = m_pMyShell->getv( -1,this,m_sProperty,m_aDirIte,aUnqPath,IsRegular );

            if( m_nOpenMode == OpenMode::DOCUMENTS && IsRegular )
            {
                vos::OGuard aGuard( m_aMutex );
                m_aItems.push_back( aRow );
                m_aIdents.push_back( Reference< XContentIdentifier >() );
                m_aUnqPath.push_back( aUnqPath );
                rowCountChanged();
                return true;

            }
            else if( m_nOpenMode == OpenMode::DOCUMENTS && ! IsRegular )
            {
                continue;
            }
            else if( m_nOpenMode == OpenMode::FOLDERS && ! IsRegular )
            {
                vos::OGuard aGuard( m_aMutex );
                m_aItems.push_back( aRow );
                m_aIdents.push_back( Reference< XContentIdentifier >() );
                m_aUnqPath.push_back( aUnqPath );
                rowCountChanged();
                return true;
            }
            else if( m_nOpenMode == OpenMode::FOLDERS && IsRegular )
            {
                continue;
            }
            else
            {
                vos::OGuard aGuard( m_aMutex );
                m_aItems.push_back( aRow );
                m_aIdents.push_back( Reference< XContentIdentifier >() );
                m_aUnqPath.push_back( aUnqPath );
                rowCountChanged();
                return true;
            }
        }
        else  // error fetching anything
        {
            throw sdbc::SQLException();
            return false;
        }
    }
}




/*
  sal_Bool SAL_CALL
  XResultSet_impl::OneMore(
  void )
  throw( sdbc::SQLException,
  RuntimeException )
  {
  if( m_bFaked )
  return OneMoreFaked();

  osl::DirectoryItem                  m_aDirIte;

  if( ! m_nIsOpen ) return false;

  osl::FileBase::RC err = m_aFolder.getNextItem( m_aDirIte );

  if( err == osl::FileBase::E_NOENT || err == osl::FileBase::E_INVAL )
  {
  m_aFolder.close();
  isFinalChanged();
  return ( m_nIsOpen = false );
  }
  else if( err == osl::FileBase::E_None )
  {
  sal_Bool IsRegular;
  rtl::OUString aUnqPath;
  Reference< sdbc::XRow > aRow = m_pMyShell->getv( -1,this,m_sProperty,m_aDirIte,aUnqPath,IsRegular );

  if( m_nOpenMode == OpenMode::DOCUMENTS )
  {
  if( IsRegular )
  {
  vos::OGuard aGuard( m_aMutex );
  m_aItems.push_back( aRow );
  m_aIdents.push_back( Reference< XContentIdentifier >() );
  m_aUnqPath.push_back( aUnqPath );
  rowCountChanged();
  return true;
  }
  else
  {
  return OneMore();
  }
  }
  else if( m_nOpenMode == OpenMode::FOLDERS )
  {
  if( ! IsRegular )
  {
  vos::OGuard aGuard( m_aMutex );
  m_aItems.push_back( aRow );
  m_aIdents.push_back( Reference< XContentIdentifier >() );
  m_aUnqPath.push_back( aUnqPath );
  rowCountChanged();
  return true;
  }
  else
  {
  return OneMore();
  }
  }
  else
  {
  vos::OGuard aGuard( m_aMutex );
  m_aItems.push_back( aRow );
  m_aIdents.push_back( Reference< XContentIdentifier >() );
  m_aUnqPath.push_back( aUnqPath );
  rowCountChanged();
  return true;
  }
  }
  else
  {
  throw sdbc::SQLException();
  return false;
  }
  }
*/


sal_Bool SAL_CALL
XResultSet_impl::next(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    sal_Bool test;
    if( ++m_nRow < m_aItems.size() ) test = true;
    else test = OneMore();
    return test;
}


sal_Bool SAL_CALL
XResultSet_impl::isBeforeFirst(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    return m_nRow == -1;
}


sal_Bool SAL_CALL
XResultSet_impl::isAfterLast(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    return m_nRow >= m_aItems.size();   // Cannot happen, if m_aFolder.isOpen()
}


sal_Bool SAL_CALL
XResultSet_impl::isFirst(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    return m_nRow == 0;
}


sal_Bool SAL_CALL
XResultSet_impl::isLast(
    void  )
    throw( sdbc::SQLException,
           RuntimeException)
{
    if( m_nRow ==  m_aItems.size() - 1 )
        return ! OneMore();
    else
        return false;
}


void SAL_CALL
XResultSet_impl::beforeFirst(
    void  )
    throw( sdbc::SQLException,
           RuntimeException)
{
    m_nRow = -1;
}


void SAL_CALL
XResultSet_impl::afterLast(
    void  )
    throw( sdbc::SQLException,
           RuntimeException )
{
    m_nRow = m_aItems.size();
    while( OneMore() )
        ++m_nRow;
}


sal_Bool SAL_CALL
XResultSet_impl::first(
    void  )
    throw( sdbc::SQLException,
           RuntimeException)
{
    m_nRow = -1;
    return next();
}


sal_Bool SAL_CALL
XResultSet_impl::last(
    void  )
    throw( sdbc::SQLException,
           RuntimeException )
{
    m_nRow = m_aItems.size() - 1;
    while( OneMore() )
        ++m_nRow;
    return true;
}


sal_Int32 SAL_CALL
XResultSet_impl::getRow(
    void )
    throw( sdbc::SQLException,
           RuntimeException)
{
    // Test, whether behind last row
    if( -1 == m_nRow || m_nRow >= m_aItems.size() )
        return 0;
    else
        return m_nRow+1;
}


//  sal_Bool SAL_CALL
//  XResultSet_impl::absolute(
//      sal_Int32 row )
//      throw( sdbc::SQLException,
//             RuntimeException)
//  {
//      if( !row )
//        throw sdbc::SQLException();

//      if( row >= 0 )
//      {
//          m_nRow = -1;
//          while( row-- ) next();
//      }
//      else
//      {
//          row = - row - 1;
//          last();
//          while( row-- ) --m_nRow;
//      }

//      return 0<= m_nRow && m_nRow < m_aItems.size();
//  }


sal_Bool SAL_CALL XResultSet_impl::absolute( sal_Int32 row )
    throw( sdbc::SQLException, RuntimeException)
{
    if( row >= 0 )
    {
        m_nRow = row - 1;
        if( row >= m_aItems.size() )
            while( row-- && OneMore() )
                ;
    }
    else
    {
        last();
        m_nRow += ( row + 1 );
        if( m_nRow < -1 )
            m_nRow = -1;
    }

    return 0<= m_nRow && m_nRow < m_aItems.size();
}




sal_Bool SAL_CALL
XResultSet_impl::relative(
    sal_Int32 row )
    throw( sdbc::SQLException,
           RuntimeException)
{
    if( isAfterLast() || isBeforeFirst() )
        throw sdbc::SQLException();
    if( row > 0 )
        while( row-- ) next();
    else if( row < 0 )
        while( row++ && m_nRow > - 1 ) previous();

    return 0 <= m_nRow && m_nRow < m_aItems.size();
}



sal_Bool SAL_CALL
XResultSet_impl::previous(
    void )
    throw( sdbc::SQLException,
           RuntimeException)
{
    if( m_nRow > m_aItems.size() )
        m_nRow = m_aItems.size();  // Correct Handling of afterLast
    if( 0 <= m_nRow ) -- m_nRow;

    return 0 <= m_nRow && m_nRow < m_aItems.size();
}


void SAL_CALL
XResultSet_impl::refreshRow(
    void )
    throw( sdbc::SQLException,
           RuntimeException)
{
    // get the row from the filesystem
    return;
}


sal_Bool SAL_CALL
XResultSet_impl::rowUpdated(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    return false;
}

sal_Bool SAL_CALL
XResultSet_impl::rowInserted(
    void  )
    throw( sdbc::SQLException,
           RuntimeException )
{
    return false;
}

sal_Bool SAL_CALL
XResultSet_impl::rowDeleted(
    void  )
    throw( sdbc::SQLException,
           RuntimeException )
{
    return false;
}


Reference< XInterface > SAL_CALL
XResultSet_impl::getStatement(
    void  )
    throw( sdbc::SQLException,
           RuntimeException )
{
    Reference< XInterface > test( 0 );
    return test;
}


// XCloseable

void SAL_CALL
XResultSet_impl::close(
    void )
    throw( sdbc::SQLException,
           RuntimeException)
{
    if( m_nIsOpen )
    {
        if( ! m_bFaked )
            m_aFolder.close();

        isFinalChanged();
        vos::OGuard aGuard( m_aMutex );
        m_nIsOpen = false;
    }
}



rtl::OUString SAL_CALL
XResultSet_impl::queryContentIdentifierString(
    void )
    throw( RuntimeException )
{
    Reference< XContentIdentifier > xContentId = queryContentIdentifier();

    if( xContentId.is() )
        return xContentId->getContentIdentifier();
    else
        return rtl::OUString();
}


Reference< XContentIdentifier > SAL_CALL
XResultSet_impl::queryContentIdentifier(
    void )
    throw( RuntimeException )
{
    if( 0 <= m_nRow && m_nRow < m_aItems.size() )
    {
        if( ! m_aIdents[m_nRow].is() )
        {
            FileContentIdentifier* p = new FileContentIdentifier( m_pMyShell,m_aUnqPath[  m_nRow ] );
            m_aIdents[m_nRow] = Reference< XContentIdentifier >(p);
        }
        return m_aIdents[m_nRow];
    }
    return Reference< XContentIdentifier >();
}


Reference< XContent > SAL_CALL
XResultSet_impl::queryContent(
    void )
    throw( RuntimeException )
{
    if( 0 <= m_nRow && m_nRow < m_aItems.size() )
        return m_pMyShell->m_pProvider->queryContent( queryContentIdentifier() );
    else
        return Reference< XContent >();
}


// XDynamicResultSet


// virtual
Reference< sdbc::XResultSet > SAL_CALL
XResultSet_impl::getStaticResultSet()
    throw( ListenerAlreadySetException,
           RuntimeException )
{
    vos::OGuard aGuard( m_aMutex );

    if ( m_xListener.is() )
        throw ListenerAlreadySetException();

    return Reference< sdbc::XResultSet >( this );
}

//=========================================================================
// virtual
void SAL_CALL
XResultSet_impl::setListener(
    const Reference< XDynamicResultSetListener >& Listener )
    throw( ListenerAlreadySetException,
           RuntimeException )
{
    vos::OClearableGuard aGuard( m_aMutex );

    if ( m_xListener.is() )
        throw ListenerAlreadySetException();

    m_xListener = Listener;

    //////////////////////////////////////////////////////////////////////
    // Create "welcome event" and send it to listener.
    //////////////////////////////////////////////////////////////////////

    // Note: We only have the implementation for a static result set at the
    //       moment (src590). The dynamic result sets passed to the listener
    //       are a fake. This implementation will never call "notify" at the
    //       listener to propagate any changes!!!

    Any aInfo;
    aInfo <<= WelcomeDynamicResultSetStruct( this, /* "old" */
                                             this /* "new" */ );

    Sequence< ListAction > aActions( 1 );
    aActions.getArray()[ 0 ] = ListAction( 0, // Position; not used
                                           0, // Count; not used
                                           ListActionType::WELCOME,
                                           aInfo );
    aGuard.clear();

    Listener->notify(
        ListEvent( static_cast< cppu::OWeakObject * >( this ), aActions ) );
}

//=========================================================================
// virtual
void SAL_CALL
XResultSet_impl::connectToCache(
    const Reference< XDynamicResultSet > & xCache )
    throw( ListenerAlreadySetException,
           AlreadyInitializedException,
           ServiceNotFoundException,
           RuntimeException )
{
    Reference< lang::XMultiServiceFactory > mxSMgr
        = m_pMyShell->m_xMultiServiceFactory;

    if( m_xListener.is() )
        throw ListenerAlreadySetException();
    if( m_bStatic )
        throw ListenerAlreadySetException();

    Reference< XSourceInitialization > xTarget( xCache, UNO_QUERY );
    if( xTarget.is() && mxSMgr.is() )
    {
        Reference< XCachedDynamicResultSetStubFactory > xStubFactory;
        try
        {
            xStubFactory = Reference< XCachedDynamicResultSetStubFactory >(
                mxSMgr->createInstance(
                    rtl::OUString::createFromAscii(
                        "com.sun.star.ucb.CachedDynamicResultSetStubFactory" ) ),
                UNO_QUERY );
        }
        catch ( Exception const & )
        {
        }

        if( xStubFactory.is() )
        {
            xStubFactory->connectToCache(
                this, xCache,m_sSortingInfo, NULL );
            return;
        }
    }
    throw ServiceNotFoundException();
}

//=========================================================================
// virtual
sal_Int16 SAL_CALL
XResultSet_impl::getCapabilities()
  throw( RuntimeException )
{
    // Never set ContentResultSetCapability::SORTED
    //  - Underlying content cannot provide sorted data...
    return 0;
}

// XResultSetMetaDataSupplier
Reference< sdbc::XResultSetMetaData > SAL_CALL
XResultSet_impl::getMetaData(
    void )
    throw( sdbc::SQLException,
           RuntimeException )
{
    for ( sal_Int32 n = 0; n < m_sProperty.getLength(); ++n )
    {
        if ( m_sProperty.getConstArray()[ n ].Name.compareToAscii( "Title" ) == 0 )
        {
            // @@@ #82177# - Determine correct value!
            sal_Bool bCaseSensitiveChildren = sal_False;

            std::vector< ::ucb::ResultSetColumnData >
                                    aColumnData( m_sProperty.getLength() );
            aColumnData[ n ].isCaseSensitive = bCaseSensitiveChildren;

            ::ucb::ResultSetMetaData* p =
                new ::ucb::ResultSetMetaData(
                    m_pMyShell->m_xMultiServiceFactory, m_sProperty, aColumnData );
            return Reference< sdbc::XResultSetMetaData >( p );
        }
    }

    ::ucb::ResultSetMetaData* p =
            new ::ucb::ResultSetMetaData(
                m_pMyShell->m_xMultiServiceFactory, m_sProperty );
    return Reference< sdbc::XResultSetMetaData >( p );
}



// XPropertySet
Reference< beans::XPropertySetInfo > SAL_CALL
XResultSet_impl::getPropertySetInfo()
    throw( RuntimeException)
{

    Sequence< beans::Property > seq(2);
    seq[0].Name = rtl::OUString::createFromAscii( "RowCount" );
    seq[0].Handle = -1;
    seq[0].Type = getCppuType( static_cast< sal_Int32* >(0) );
    seq[0].Attributes = beans::PropertyAttribute::READONLY;

    seq[0].Name = rtl::OUString::createFromAscii( "IsRowCountFinal" );
    seq[0].Handle = -1;
    seq[0].Type = getCppuType( static_cast< sal_Bool* >(0) );
    seq[0].Attributes = beans::PropertyAttribute::READONLY;

    XPropertySetInfo_impl* p = new XPropertySetInfo_impl( m_pMyShell,
                                                          seq );
    return Reference< beans::XPropertySetInfo > ( p );
}



void SAL_CALL XResultSet_impl::setPropertyValue(
    const rtl::OUString& aPropertyName, const Any& aValue )
    throw( beans::UnknownPropertyException,
           beans::PropertyVetoException,
           lang::IllegalArgumentException,
           lang::WrappedTargetException,
           RuntimeException)
{
    if( aPropertyName == rtl::OUString::createFromAscii( "IsRowCountFinal" ) ||
        aPropertyName == rtl::OUString::createFromAscii( "RowCount" ) )
        return;
    throw beans::UnknownPropertyException();
}


Any SAL_CALL XResultSet_impl::getPropertyValue(
    const rtl::OUString& PropertyName )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           RuntimeException)
{
    if( PropertyName == rtl::OUString::createFromAscii( "IsRowCountFinal" ) )
    {
        Any aAny;
        aAny <<= m_bRowCountFinal;
        return aAny;
    }
    else if ( PropertyName == rtl::OUString::createFromAscii( "RowCount" ) )
    {
        Any aAny;
        sal_Int32 count = m_aItems.size();
        aAny <<= count;
        return aAny;
    }
    else
        throw beans::UnknownPropertyException();
}


void SAL_CALL XResultSet_impl::addPropertyChangeListener(
    const rtl::OUString& aPropertyName,
    const Reference< beans::XPropertyChangeListener >& xListener )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           RuntimeException)
{
    if( aPropertyName == rtl::OUString::createFromAscii( "IsRowCountFinal" ) )
    {
        vos::OGuard aGuard( m_aMutex );
        if ( ! m_pIsFinalListeners )
            m_pIsFinalListeners =
                new cppu::OInterfaceContainerHelper( m_aEventListenerMutex );

        m_pIsFinalListeners->addInterface( xListener );
    }
    else if ( aPropertyName == rtl::OUString::createFromAscii( "RowCount" ) )
    {
        vos::OGuard aGuard( m_aMutex );
        if ( ! m_pRowCountListeners )
            m_pRowCountListeners =
                new cppu::OInterfaceContainerHelper( m_aEventListenerMutex );
        m_pRowCountListeners->addInterface( xListener );
    }
    else
        throw beans::UnknownPropertyException();
}


void SAL_CALL XResultSet_impl::removePropertyChangeListener(
    const rtl::OUString& aPropertyName,
    const Reference< beans::XPropertyChangeListener >& aListener )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           RuntimeException)
{
    if( aPropertyName == rtl::OUString::createFromAscii( "IsRowCountFinal" ) &&
        m_pIsFinalListeners )
    {
        vos::OGuard aGuard( m_aMutex );
        m_pIsFinalListeners->removeInterface( aListener );
    }
    else if ( aPropertyName == rtl::OUString::createFromAscii( "RowCount" ) &&
              m_pRowCountListeners )
    {
        vos::OGuard aGuard( m_aMutex );

        m_pRowCountListeners->removeInterface( aListener );
    }
    else
        throw beans::UnknownPropertyException();
}

void SAL_CALL XResultSet_impl::addVetoableChangeListener(
    const rtl::OUString& PropertyName,
    const Reference< beans::XVetoableChangeListener >& aListener )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           RuntimeException)
{
}


void SAL_CALL XResultSet_impl::removeVetoableChangeListener(
    const rtl::OUString& PropertyName,
    const Reference< beans::XVetoableChangeListener >& aListener )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException,
           RuntimeException)
{
}



