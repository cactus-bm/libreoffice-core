#include <vlc/libvlc.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_events.h>

#include "EventManager.hxx"
#include "SymbolLoader.hxx"

typedef void ( *libvlc_callback_t ) ( const struct libvlc_event_t *, void * );

namespace VLC
{
    namespace
    {
        libvlc_event_manager_t* ( *libvlc_media_player_event_manager ) ( libvlc_media_player_t *p_mi );
        int ( *libvlc_event_attach ) ( libvlc_event_manager_t *p_event_manager,
                                       libvlc_event_type_t i_event_type,
                                       libvlc_callback_t f_callback,
                                       void *user_data );
        void ( *libvlc_event_detach ) ( libvlc_event_manager_t *p_event_manager,
                                        libvlc_event_type_t i_event_type,
                                        libvlc_callback_t f_callback,
                                        void *p_user_data );

        ApiMap VLC_EVENT_MANAGER_API[] =
        {
            SYM_MAP( libvlc_media_player_event_manager ),
            SYM_MAP( libvlc_event_attach ),
            SYM_MAP( libvlc_event_detach )
        };
    }

void EventManagerEventHandler( const libvlc_event_t *event, void *pData )
{
    EventManager *instance = static_cast<EventManager*>( pData );
    switch ( event->type )
    {
    case libvlc_MediaPlayerPaused:
        instance->mOnPaused();
        break;
    case libvlc_MediaPlayerEndReached:
        instance->mOnEndReached();
        break;
    }
}

EventManager::EventManager(VLC::Player& player)
{
    InitApiMap( VLC_EVENT_MANAGER_API );
    mManager = libvlc_media_player_event_manager( player );
}

EventManager::~EventManager()
{
}

void EventManager::registerSignal(int signal, const Callback& callback)
{
    if (callback.empty())
        libvlc_event_detach( mManager, signal, EventManagerEventHandler, this );
    else
        libvlc_event_attach( mManager, signal, EventManagerEventHandler, this );
}

void EventManager::onPaused(const EventManager::Callback& callback)
{
    mOnPaused = callback;
    registerSignal(libvlc_MediaPlayerPaused, callback);
}

void EventManager::onEndReached(const Callback& callback)
{
    mOnEndReached = callback;
    registerSignal(libvlc_MediaPlayerEndReached, callback);
}

}