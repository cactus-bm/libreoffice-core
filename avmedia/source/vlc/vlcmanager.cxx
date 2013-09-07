#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "vlcmanager.hxx"
#include "vlcplayer.hxx"
#include "wrapper/Instance.hxx"
#include "wrapper/EventManager.hxx"
#include "wrapper/Media.hxx"
#include "wrapper/Player.hxx"
#include "wrapper/Common.hxx"

using namespace ::com::sun::star;

namespace avmedia {
namespace vlc {

namespace
{
    const rtl::OUString VLC_IMPLEMENTATION_NAME = "com.sun.star.comp.avmedia.Manager_VLC";
    const ::rtl::OUString VLC_SERVICENAME = "com.sun.star.media.Manager_VLC";

    const char * const VLC_ARGS[] = {
        "-Vdummy",
        "--snapshot-format=png",
        "--ffmpeg-threads",
        "--verbose=-1"
    };
}

Manager::Manager( const uno::Reference< lang::XMultiServiceFactory >& rxMgr )
    : mEventHandler()
    , mxMgr( rxMgr )
{
    using namespace VLC;
    static bool success = Instance::LoadSymbols() && EventManager::LoadSymbols()
                          && Media::LoadSymbols() && Player::LoadSymbols()
                          && Common::LoadSymbols();

    m_is_vlc_found = success;
    if (m_is_vlc_found)
    {
        mInstance.reset(new Instance( sizeof( VLC_ARGS ) / sizeof( VLC_ARGS[0] ), VLC_ARGS ));
        //Check VLC version
        std::vector<std::string> verComponents;
        const std::string str(Common::Version());

        std::cout << str << std::endl;
        boost::split(verComponents,
                     str,
                     boost::is_any_of(". "));
        if (verComponents.size() < 3
            || boost::lexical_cast<int>(verComponents[0]) < 2
            || (boost::lexical_cast<int>(verComponents[1]) == 0
                && boost::lexical_cast<int>(verComponents[2]) < 8))
        {
            m_is_vlc_found = false;
        }
    }

    if (m_is_vlc_found)
        mEventHandler.create();
}

Manager::~Manager()
{
    mEventHandler.stop();
}

uno::Reference< media::XPlayer > SAL_CALL Manager::createPlayer( const rtl::OUString& rURL )
    throw (uno::RuntimeException)
{
    if ( !m_is_vlc_found )
        return uno::Reference< media::XPlayer >();

    VLCPlayer* pPlayer( new VLCPlayer( rURL,
                                       *mInstance,
                                       mEventHandler /*, mxMgr */ ) );
    mPlayer = uno::Reference< media::XPlayer >( pPlayer );

    return mPlayer;
}

rtl::OUString SAL_CALL Manager::getImplementationName()
    throw (uno::RuntimeException)
{
    return VLC_IMPLEMENTATION_NAME;
}

sal_Bool SAL_CALL Manager::supportsService( const rtl::OUString& serviceName )
    throw (uno::RuntimeException)
{
    return serviceName == VLC_SERVICENAME && m_is_vlc_found;
}

uno::Sequence< rtl::OUString > SAL_CALL Manager::getSupportedServiceNames()
    throw (uno::RuntimeException)
{
    if ( !m_is_vlc_found )
        return uno::Sequence< rtl::OUString >();

    ::uno::Sequence< OUString > aRet(1);
    aRet[0] = VLC_SERVICENAME;
    return aRet;
}

} // end namespace vlc
} // end namespace avmedia

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
