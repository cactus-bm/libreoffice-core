#include <string.h>
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _VOS_DIAGNOSE_HXX_
#include <vos/diagnose.hxx>
#endif
#ifndef _TREEVIEW_TVREAD_HXX_
#include "tvread.hxx"
#endif
#ifdef SYSTEM_EXPAT
#include <expat.h>
#else
#ifndef XmlParse_INCLUDED
#include <expat/xmlparse.h>
#endif
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONFIGMANAGER_HPP_
#include <com/sun/star/frame/XConfigManager.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATE_HPP_
#include <com/sun/star/beans/PropertyState.hpp>
#endif

namespace treeview {


    class TVDom
    {
        friend class TVChildTarget;
        friend class TVRead;

    public:

        TVDom( TVDom* arent = 0 )
            : kind( other ),
              parent( arent ),
              childs( 0 )
        {
        }

        ~TVDom()
        {
            for( unsigned i = 0; i < childs.size(); ++i )
                delete childs[i];
        }


        TVDom* newChild()
        {
            childs.push_back( new TVDom( this ) );
            return childs.back();
        }


        TVDom* getParent() const
        {
            if( parent )
                return parent;
            else
                return const_cast<TVDom*>(this);    // I am my own parent, if I am the root
        }

        enum Kind {
            tree_view,
            tree_node,
            tree_leaf,
            other
        };

        bool isLeaf() const { return kind == TVDom::tree_leaf; }
        void setKind( Kind ind ) { kind = ind; }
        Kind getKind( ) const { return kind; }


        void setApplication( const char* appl )
        {
            application = rtl::OUString( (sal_Char*)(appl),
                                         strlen( appl ),
                                         RTL_TEXTENCODING_UTF8 );
        }

        void setTitle( const char* itle )
        {
            title += rtl::OUString( (sal_Char*)(itle),
                                    strlen( itle ),
                                    RTL_TEXTENCODING_UTF8 );
        }

        void setTitle( const XML_Char* itle,int len )
        {
            title += rtl::OUString( (sal_Char*)(itle),
                                    len,
                                    RTL_TEXTENCODING_UTF8 );
        }

        void setId( const char* d )
        {
            id = rtl::OUString( (sal_Char*)(d),
                                strlen( d ),
                                RTL_TEXTENCODING_UTF8 );
        }

        void setAnchor( const char* nchor )
        {
            anchor = rtl::OUString( (sal_Char*)(nchor),
                                    strlen( nchor ),
                                    RTL_TEXTENCODING_UTF8 );
        }

        rtl::OUString getTargetURL()
        {
            if( ! targetURL.getLength() )
            {
                sal_Int32 len;
                for ( const TVDom* p = this;; p = p->parent )
                {
                    len = p->application.getLength();
                    if ( len != 0 )
                        break;
                }

                rtl::OUStringBuffer strBuff( 22 + len + id.getLength() );
                strBuff.appendAscii(
                                    "vnd.sun.star.help://"
                                    ).append(id);

                targetURL = strBuff.makeStringAndClear();
            }

            return targetURL;
        }

    private:

        Kind   kind;
        rtl::OUString  application;
        rtl::OUString  title;
        rtl::OUString  id;
        rtl::OUString  anchor;
        rtl::OUString  targetURL;

        TVDom *parent;
        std::vector< TVDom* > childs;
    };

}



using namespace treeview;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;
using namespace com::sun::star::frame;
using namespace com::sun::star::container;


ConfigData::ConfigData()
    : prodName( rtl::OUString::createFromAscii( "%PRODUCTNAME" ) ),
      prodVersion( rtl::OUString::createFromAscii( "%PRODUCTVERSION" ) ),
      vendName( rtl::OUString::createFromAscii( "%VENDORNAME" ) ),
      vendVersion( rtl::OUString::createFromAscii( "%VENDORVERSION" ) ),
      vendShort( rtl::OUString::createFromAscii( "%VENDORSHORT" ) )
{
}

void SAL_CALL ConfigData::replaceName( rtl::OUString& oustring ) const
{
    sal_Int32 idx = -1,k = 0,off;
    bool cap = false;
    rtl::OUStringBuffer aStrBuf( 0 );

    while( ( idx = oustring.indexOf( sal_Unicode('%'),++idx ) ) != -1 )
    {
        if( oustring.indexOf( prodName,idx ) == idx )
            off = PRODUCTNAME;
        else if( oustring.indexOf( prodVersion,idx ) == idx )
            off = PRODUCTVERSION;
        else if( oustring.indexOf( vendName,idx ) == idx )
            off = VENDORNAME;
        else if( oustring.indexOf( vendVersion,idx ) == idx )
            off = VENDORVERSION;
        else if( oustring.indexOf( vendShort,idx ) == idx )
            off = VENDORSHORT;
        else
            off = -1;

        if( off != -1 )
        {
            if( ! cap )
            {
                cap = true;
                aStrBuf.ensureCapacity( 256 );
            }

            aStrBuf.append( &oustring.getStr()[k],idx - k );
            aStrBuf.append( m_vReplacement[off] );
            k = idx + m_vAdd[off];
        }
    }

    if( cap )
    {
        if( k < oustring.getLength() )
            aStrBuf.append( &oustring.getStr()[k],oustring.getLength()-k );
        oustring = aStrBuf.makeStringAndClear();
    }
}




//////////////////////////////////////////////////////////////////////////
// XInterface
//////////////////////////////////////////////////////////////////////////


void SAL_CALL
TVBase::acquire(
    void )
    throw()
{
  OWeakObject::acquire();
}


void SAL_CALL
TVBase::release(
              void )
  throw()
{
  OWeakObject::release();
}


Any SAL_CALL
TVBase::queryInterface(
    const Type& rType )
    throw( RuntimeException )
{
    Any aRet = cppu::queryInterface( rType,
                                     SAL_STATIC_CAST( XTypeProvider*, this ),
                                     SAL_STATIC_CAST( XNameAccess*, this ),
                                     SAL_STATIC_CAST( XHierarchicalNameAccess*, this ),
                                     SAL_STATIC_CAST( XChangesNotifier*, this ),
                                     SAL_STATIC_CAST( XComponent*, this ) );

    return aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType );
}


////////////////////////////////////////////////////////////////////////////////
//
// XTypeProvider methods.

XTYPEPROVIDER_IMPL_5( TVBase,
                         XTypeProvider,
                      XNameAccess,
                      XHierarchicalNameAccess,
                      XChangesNotifier,
                      XComponent );






// TVRead


TVRead::TVRead()
{
}



TVRead::TVRead( const ConfigData& configData,TVDom* tvDom )
{
    if( ! tvDom )
        return;

    Title = tvDom->title;
    configData.replaceName( Title );
    if( tvDom->isLeaf() )
    {
        TargetURL = ( tvDom->getTargetURL() + configData.appendix );
        if( tvDom->anchor.getLength() )
            TargetURL += ( rtl::OUString::createFromAscii( "#" ) +
                           tvDom->anchor );
    }
    else
        Children = new TVChildTarget( configData,tvDom );
}



TVRead::~TVRead()
{
}






// XNameAccess

Any SAL_CALL
TVRead::getByName( const rtl::OUString& aName )
    throw( NoSuchElementException,
           WrappedTargetException,
           RuntimeException )
{
    bool found( true );
    Any aAny;
    if( aName.compareToAscii( "Title" ) == 0 )
        aAny <<= Title;
    else if( aName.compareToAscii( "TargetURL" ) == 0 )
        aAny <<= TargetURL;
    else if( aName.compareToAscii( "Children" ) == 0 )
    {
        cppu::OWeakObject* p = Children.get();
        aAny <<= Reference< XInterface >( p );
    }
    else
        found = false;

    if( found )
        return aAny;

    throw NoSuchElementException();
}




Sequence< rtl::OUString > SAL_CALL
TVRead::getElementNames( )
    throw( RuntimeException )
{
    Sequence< rtl::OUString > seq( 3 );

    seq[0] = rtl::OUString::createFromAscii( "Title" );
    seq[1] = rtl::OUString::createFromAscii( "TargetURL" );
    seq[2] = rtl::OUString::createFromAscii( "Children" );

    return seq;
}



sal_Bool SAL_CALL
TVRead::hasByName( const rtl::OUString& aName )
    throw( RuntimeException )
{
    if( aName.compareToAscii( "Title" ) == 0        ||
        aName.compareToAscii( "TargetURL" ) == 0    ||
        aName.compareToAscii( "Children" ) == 0 )
        return true;

    return false;
}


// XHierarchicalNameAccess

Any SAL_CALL
TVRead::getByHierarchicalName( const rtl::OUString& aName )
    throw( NoSuchElementException,
           RuntimeException )
{
    sal_Int32 idx;
    rtl::OUString name( aName );

    if( ( idx = name.indexOf( sal_Unicode( '/' ) ) ) != -1  &&
        name.copy( 0,idx ).compareToAscii( "Children" ) == 0 )
        return Children->getByHierarchicalName( name.copy( 1 + idx ) );

    return getByName( name );
}




sal_Bool SAL_CALL
TVRead::hasByHierarchicalName( const rtl::OUString& aName )
    throw( RuntimeException )
{
    sal_Int32 idx;
    rtl::OUString name( aName );

       if( ( idx = name.indexOf( sal_Unicode( '/' ) ) ) != -1  &&
        name.copy( 0,idx ).compareToAscii( "Children" ) == 0 )
        return Children->hasByHierarchicalName( name.copy( 1 + idx ) );

    return hasByName( name );
}



/**************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmlhelp.hxx"
/*                                                                        */
/*                      TVChildTarget                                     */
/*                                                                        */
/**************************************************************************/




extern "C" void start_handler(void *userData,
                   const XML_Char *name,
                   const XML_Char **atts)
{
    TVDom::Kind kind;

    if( strcmp( name,"help_section" ) == 0  ||
        strcmp( name,"node" ) == 0 )
        kind = TVDom::tree_node;
    else if( strcmp( name,"topic" ) == 0 )
        kind = TVDom::tree_leaf;
    else
        return;

    TVDom **tvDom = static_cast< TVDom** >( userData );
    TVDom  *p;
    p = *tvDom;

    *tvDom = p->newChild();
    p = *tvDom;

    p->setKind( kind );
    while( *atts )
    {
        if( strcmp( *atts,"application" ) == 0 )
            p->setApplication( *(atts+1) );
        else if( strcmp( *atts,"title" ) == 0 )
            p->setTitle( *(atts+1) );
        else if( strcmp( *atts,"id" ) == 0 )
            p->setId( *(atts+1) );
        else if( strcmp( *atts,"anchor" ) == 0 )
            p->setAnchor( *(atts+1) );

        atts+=2;
    }
}


extern "C" void end_handler(void *userData,
                 const XML_Char *name )
{
    (void)name;

    TVDom **tvDom = static_cast< TVDom** >( userData );
    *tvDom = (*tvDom)->getParent();
}


extern "C" void data_handler( void *userData,
                   const XML_Char *s,
                   int len)
{
    TVDom **tvDom = static_cast< TVDom** >( userData );
    if( (*tvDom)->isLeaf() )
        (*tvDom)->setTitle( s,len );
}



TVChildTarget::TVChildTarget( const ConfigData& configData,TVDom* tvDom )
{
    Elements.resize( tvDom->childs.size() );
    for( unsigned i = 0; i < Elements.size(); ++i )
        Elements[i] = new TVRead( configData,tvDom->childs[i] );
}





TVChildTarget::TVChildTarget( const Reference< XMultiServiceFactory >& xMSF )
{
    ConfigData configData = init( xMSF );

    if( ! configData.locale.getLength()  ||
        ! configData.system.getLength() )
        return;

    sal_uInt64  ret,len = 0;
    int j = 0;

    // Count number of items
    while( configData.fileurl[j].getLength() )
        ++j;

    TVDom tvDom;
    TVDom* pTVDom = &tvDom;

    while( j )
    {
        len = configData.filelen[--j];
        char* s = new char[ int(len) ];  // the buffer to hold the installed files
        osl::File aFile( configData.fileurl[j] );
        aFile.open( OpenFlag_Read );
        aFile.read( s,len,ret );
        aFile.close();

        XML_Parser parser = XML_ParserCreate( 0 );
        XML_SetElementHandler( parser,
                               start_handler,
                               end_handler );
        XML_SetCharacterDataHandler( parser,
                                     data_handler);
        XML_SetUserData( parser,&pTVDom ); // does not return this

        int parsed = XML_Parse( parser,s,int( len ),j==0 );
        (void)parsed;

        XML_ParserFree( parser );
        delete[] s;
    }

    // now TVDom holds the relevant information

    Elements.resize( tvDom.childs.size() );
    for( unsigned i = 0; i < Elements.size(); ++i )
        Elements[i] = new TVRead( configData,tvDom.childs[i] );
}


TVChildTarget::~TVChildTarget()
{
}



Any SAL_CALL
TVChildTarget::getByName( const rtl::OUString& aName )
    throw( NoSuchElementException,
           WrappedTargetException,
           RuntimeException )
{
    rtl::OUString num( aName.getStr()+2,aName.getLength()-4 );
    sal_Int32 idx = num.toInt32() - 1;
    if( idx < 0 || Elements.size() <= sal_uInt32( idx ) )
        throw NoSuchElementException();

    Any aAny;
    cppu::OWeakObject* p = Elements[idx].get();
    aAny <<= Reference< XInterface >( p );
    return aAny;
}




Sequence< rtl::OUString > SAL_CALL
TVChildTarget::getElementNames( )
    throw( RuntimeException )
{
    Sequence< rtl::OUString > seq( Elements.size() );
    for( unsigned i = 0; i < Elements.size(); ++i )
        seq[i] = rtl::OUString::valueOf( sal_Int32( 1+i ) );

    return seq;
}



sal_Bool SAL_CALL
TVChildTarget::hasByName( const rtl::OUString& aName )
    throw( RuntimeException )
{
    rtl::OUString num( aName.getStr()+2,aName.getLength()-4 );
    sal_Int32 idx = num.toInt32() - 1;
    if( idx < 0 || Elements.size() <= sal_uInt32( idx ) )
        return false;

    return true;
}



// XHierarchicalNameAccess

Any SAL_CALL
TVChildTarget::getByHierarchicalName( const rtl::OUString& aName )
    throw( NoSuchElementException,
           RuntimeException )
{
    sal_Int32 idx;
    rtl::OUString name( aName );

    if( ( idx = name.indexOf( sal_Unicode( '/' ) ) ) != -1 )
    {
        rtl::OUString num( name.getStr()+2,idx-4 );
        sal_Int32 pref = num.toInt32() - 1;

        if( pref < 0 || Elements.size() <= sal_uInt32( pref ) )
            throw NoSuchElementException();

        return Elements[pref]->getByHierarchicalName( name.copy( 1 + idx ) );
    }
    else
        return getByName( name );
}



sal_Bool SAL_CALL
TVChildTarget::hasByHierarchicalName( const rtl::OUString& aName )
    throw( RuntimeException )
{
    sal_Int32 idx;
    rtl::OUString name( aName );

       if( ( idx = name.indexOf( sal_Unicode( '/' ) ) ) != -1 )
    {
        rtl::OUString num( name.getStr()+2,idx-4 );
        sal_Int32 pref = num.toInt32() - 1;
        if( pref < 0 || Elements.size() <= sal_uInt32( pref ) )
            return false;

        return Elements[pref]->hasByHierarchicalName( name.copy( 1 + idx ) );
    }
    else
        return hasByName( name );
}






ConfigData TVChildTarget::init( const Reference< XMultiServiceFactory >& xSMgr )
{
    ConfigData configData;
    Reference< XMultiServiceFactory >  sProvider( getConfiguration(xSMgr) );

    /**********************************************************************/
    /*                       reading Office.Common                        */
    /**********************************************************************/

    Reference< XHierarchicalNameAccess > xHierAccess( getHierAccess( sProvider,
                                                                     "org.openoffice.Office.Common" ) );
    rtl::OUString system( getKey( xHierAccess,"Help/System" ) );
    sal_Bool showBasic( getBooleanKey(xHierAccess,"Help/ShowBasic") );
    rtl::OUString instPath( getKey( xHierAccess,"Path/Current/Help" ) );
    if( ! instPath.getLength() )
      // try to determine path from default
      instPath = rtl::OUString::createFromAscii( "$(instpath)/help" );

    // replace anything like $(instpath);
    subst( xSMgr,instPath );


    /**********************************************************************/
    /*                       reading Webtop.Common                        */
    /**********************************************************************/

    xHierAccess = getHierAccess( sProvider,
                                 "org.openoffice.Webtop.Common" );
    rtl::OUString vendorName( getKey(  xHierAccess,"Product/ooName" ) );

    rtl::OUString setupversion( getKey(  xHierAccess,"Product/ooSetupVersion" ) );
    rtl::OUString setupextension( getKey(  xHierAccess,"Product/ooSetupExtension") );
    rtl::OUString vendorVersion( setupversion +
                                 rtl::OUString::createFromAscii( " " ) +
                                 setupextension );
    rtl::OUString vendorShort = vendorName;

    /**********************************************************************/
    /*                       reading setup                                */
    /**********************************************************************/

    xHierAccess = getHierAccess( sProvider,
                                 "org.openoffice.Setup" );

    rtl::OUString productName( getKey(  xHierAccess,"Product/ooName" ) );
    setupversion = getKey(  xHierAccess,"Product/ooSetupVersion" );
    setupextension = getKey(  xHierAccess,"Product/ooSetupExtension");
    rtl::OUString productVersion( setupversion +
                                  rtl::OUString::createFromAscii( " " ) +
                                  setupextension );
    rtl::OUString locale( getKey( xHierAccess,"L10N/ooLocale" ) );


    // Determine fileurl from url and locale
    rtl::OUString url;
    osl::FileBase::RC errFile = osl::FileBase::getFileURLFromSystemPath( instPath,url );
    if( errFile != osl::FileBase::E_None ) return configData;
    if( url.lastIndexOf( sal_Unicode( '/' ) ) != url.getLength() - 1 )
        url += rtl::OUString::createFromAscii( "/" );
    rtl::OUString ret;
    sal_Int32 idx;
    osl::DirectoryItem aDirItem;
    if( osl::FileBase::E_None == osl::DirectoryItem::get( url + locale,aDirItem ) )
        ret = locale;
    else if( ( ( idx = locale.indexOf( '-' ) ) != -1 ||
               ( idx = locale.indexOf( '_' ) ) != -1 ) &&
             osl::FileBase::E_None == osl::DirectoryItem::get( url + locale.copy( 0,idx ),
                                                               aDirItem ) )
        ret = locale.copy( 0,idx );

    url = url + ret;

    // first of all, try do determine whether there are any *.tree files present

    osl::Directory aDirectory( url );
    osl::FileStatus aFileStatus( FileStatusMask_FileName | FileStatusMask_FileSize | FileStatusMask_FileURL );
    if( osl::Directory::E_None == aDirectory.open() )
    {
        int idx_ = 0,j = 0;
        rtl::OUString aFileUrl, aFileName;
        while( aDirectory.getNextItem( aDirItem ) == osl::FileBase::E_None &&
               aDirItem.getFileStatus( aFileStatus ) == osl::FileBase::E_None &&
               aFileStatus.isValid( FileStatusMask_FileURL ) &&
               aFileStatus.isValid( FileStatusMask_FileName ) )
          {
            aFileUrl = aFileStatus.getFileURL();
            aFileName = aFileStatus.getFileName();
            idx_ = aFileName.lastIndexOf( sal_Unicode( '.' ) );
            if( idx_ == -1 )
              continue;

            const sal_Unicode* str = aFileName.getStr();

            if( aFileName.getLength() == idx_ + 5                   &&
                ( str[idx_ + 1] == 't' || str[idx_ + 1] == 'T' )    &&
                ( str[idx_ + 2] == 'r' || str[idx_ + 2] == 'R' )    &&
                ( str[idx_ + 3] == 'e' || str[idx_ + 3] == 'E' )    &&
                ( str[idx_ + 4] == 'e' || str[idx_ + 4] == 'E' ) )
              {
                OSL_ENSURE( j < MAX_MODULE_COUNT,"too many modules installed" );
                OSL_ENSURE( aFileStatus.isValid( FileStatusMask_FileSize ),
                            "invalid file size" );

                rtl::OUString baseName = aFileName.copy(0,idx_).toAsciiLowerCase();
                if(! showBasic && baseName.compareToAscii("sbasic") == 0 )
                  continue;

                configData.filelen[j] = aFileStatus.getFileSize();
                configData.fileurl[j++] = aFileUrl ;
              }
          }
        aDirectory.close();
    }

    configData.m_vAdd[0] = 12;
    configData.m_vAdd[1] = 15;
    configData.m_vAdd[2] = 11;
    configData.m_vAdd[3] = 14;
    configData.m_vAdd[4] = 12;
    configData.m_vReplacement[0] = productName;
    configData.m_vReplacement[1] = productVersion;
    configData.m_vReplacement[2] = vendorName;
    configData.m_vReplacement[3] = vendorVersion;
    configData.m_vReplacement[4] = vendorShort;

       configData.system = system;
    configData.locale = locale;
    configData.appendix =
        rtl::OUString::createFromAscii( "?Language=" ) +
        configData.locale +
        rtl::OUString::createFromAscii( "&System=" ) +
        configData.system +
        rtl::OUString::createFromAscii( "&UseDB=no" ) ;

    return configData;
}









Reference< XMultiServiceFactory >
TVChildTarget::getConfiguration(const Reference< XMultiServiceFactory >& m_xSMgr) const
{
    Reference< XMultiServiceFactory > sProvider;
    if( m_xSMgr.is() )
    {
        Any aAny;
        aAny <<= rtl::OUString::createFromAscii( "plugin" );
        PropertyValue aProp( rtl::OUString::createFromAscii( "servertype" ),
                             -1,
                             aAny,
                             PropertyState_DIRECT_VALUE );

        Sequence< Any > seq(1);
        seq[0] <<= aProp;

        try
        {
            rtl::OUString sProviderService =
                rtl::OUString::createFromAscii( "com.sun.star.configuration.ConfigurationProvider" );
            sProvider =
                Reference< XMultiServiceFactory >(
                    m_xSMgr->createInstanceWithArguments( sProviderService,seq ),
                    UNO_QUERY );
        }
        catch( const com::sun::star::uno::Exception& )
        {
            OSL_ENSURE( sProvider.is(),"cant instantiate configuration" );
        }
    }

    return sProvider;
}



Reference< XHierarchicalNameAccess >
TVChildTarget::getHierAccess( const Reference< XMultiServiceFactory >& sProvider,
                              const char* file ) const
{
    Reference< XHierarchicalNameAccess > xHierAccess;

    if( sProvider.is() )
    {
        Sequence< Any > seq(1);
        rtl::OUString sReaderService =
            rtl::OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" );

        seq[0] <<= rtl::OUString::createFromAscii( file );

        try
        {
            xHierAccess =
                Reference< XHierarchicalNameAccess >
                ( sProvider->createInstanceWithArguments( sReaderService,seq ),
                  UNO_QUERY );
        }
        catch( const com::sun::star::uno::Exception& )
        {
        }
    }

    return xHierAccess;
}



rtl::OUString
TVChildTarget::getKey( const Reference< XHierarchicalNameAccess >& xHierAccess,
                       const char* key ) const
{
    rtl::OUString instPath;
    if( xHierAccess.is() )
    {
        Any aAny;
        try
        {
            aAny =
                xHierAccess->getByHierarchicalName( rtl::OUString::createFromAscii( key ) );
        }
        catch( const com::sun::star::container::NoSuchElementException& )
        {
        }
        aAny >>= instPath;
    }
    return instPath;
}


sal_Bool
TVChildTarget::getBooleanKey(const Reference<
                             XHierarchicalNameAccess >& xHierAccess,
                             const char* key) const
{
  sal_Bool ret = sal_False;
  if( xHierAccess.is() )
    {
      Any aAny;
      try
        {
          aAny =
            xHierAccess->getByHierarchicalName(
                                               rtl::OUString::createFromAscii(key));
        }
      catch( const com::sun::star::container::NoSuchElementException& )
        {
        }
      aAny >>= ret;
    }
  return ret;
}


void TVChildTarget::subst( const Reference< XMultiServiceFactory >& m_xSMgr,
                           rtl::OUString& instpath ) const
{
    Reference< XConfigManager >  xCfgMgr;
    if( m_xSMgr.is() )
    {
        try
        {
            xCfgMgr =
                Reference< XConfigManager >(
                    m_xSMgr->createInstance( rtl::OUString::createFromAscii( "com.sun.star.config.SpecialConfigManager" ) ),
                    UNO_QUERY );
        }
        catch( const com::sun::star::uno::Exception& )
        {
            OSL_ENSURE( xCfgMgr.is()," cant instantiate the special config manager " );
        }
    }

    OSL_ENSURE( xCfgMgr.is(), "specialconfigmanager not found\n" );

    if( xCfgMgr.is() )
        instpath = xCfgMgr->substituteVariables( instpath );
}
