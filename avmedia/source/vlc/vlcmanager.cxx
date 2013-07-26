#include "vlcmanager.hxx"
#include "vlcplayer.hxx"

using namespace ::com::sun::star;

namespace avmedia {
namespace vlc {

const rtl::OUString VLC_IMPLEMENTATION_NAME = "com.sun.star.comp.avmedia.Manager_VLC";
const ::rtl::OUString VLC_SERVICENAME = "com.sun.star.media.Manager_VLC";

Manager::Manager( const uno::Reference< lang::XMultiServiceFactory >& rxMgr )
    : mxMgr( rxMgr )
{
}

Manager::~Manager()
{
}

uno::Reference< media::XPlayer > SAL_CALL Manager::createPlayer( const rtl::OUString& rURL )
    throw (uno::RuntimeException)
{
    VLCPlayer* pPlayer( new VLCPlayer( rURL/*, mxMgr */ ) );
    uno::Reference< media::XPlayer > xRet( pPlayer );

    return xRet;
}

rtl::OUString SAL_CALL Manager::getImplementationName()
    throw (uno::RuntimeException)
{
    return VLC_IMPLEMENTATION_NAME;
}

sal_Bool SAL_CALL Manager::supportsService( const rtl::OUString& serviceName )
    throw (uno::RuntimeException)
{
    return serviceName == VLC_SERVICENAME;
}

uno::Sequence< rtl::OUString > SAL_CALL Manager::getSupportedServiceNames()
    throw (uno::RuntimeException)
{
    ::uno::Sequence< OUString > aRet(1);
    aRet[0] = VLC_SERVICENAME;
    return aRet;
}

} // end namespace vlc
} // end namespace avmedia

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
