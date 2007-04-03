/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ConfigurationUpdater.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-04-03 15:46:53 $
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

#ifndef SD_FRAMEWORK_CONFIGURATION_UPDATER_HXX
#define SD_FRAMEWORK_CONFIGURATION_UPDATER_HXX

#ifndef _COM_SUN_STAR_DRAWING_FRAMEWORK_XRESOURCEID_HPP_
#include <com/sun/star/drawing/framework/XResourceId.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_FRAMEWORK_XCONFIGURATION_HPP_
#include <com/sun/star/drawing/framework/XConfiguration.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_FRAMEWORK_XCONTROLLERMANAGER_HPP_
#include <com/sun/star/drawing/framework/XControllerManager.hpp>
#endif
#include <vcl/timer.hxx>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace sd { namespace framework {

class ConfigurationClassifier;
class ConfigurationUpdaterLock;

/** This is a helper class for the ConfigurationController.  It handles the
    update of the current configuration so that it looks like a requested
    configuration.  An update is made by activating or deactivating drawing
    framework resources.

    When an update is not successfull, i.e. the current configuration is not
    equivalent to the requested configuration after the update, then a timer
    is started to repeat the update after a short time.
*/
class ConfigurationUpdater
{
public:
    /** Create a new ConfigurationUpdater object that notifies configuration
        changes and the start and end of updates via the given broadcaster.
    */
    ConfigurationUpdater (const ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfigurationControllerBroadcaster>& rxBroadcaster);
    ~ConfigurationUpdater (void);

    /** This method is typically called once, when the controller manager is
        accessible to the caller.
    */
    void SetControllerManager(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::framework::XControllerManager>& rxControllerManager);

    /** Request an update of the current configuration so that it looks like
        the given requested configuration.  It check whether an update of
        the current configuration can be done.  Calls UpdateConfiguration()
        if that is the case.  Otherwise it schedules a later call to
        UpdateConfiguration().
    */
    void RequestUpdate (const ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfiguration>& rxRequestedConfiguration);

    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfiguration> GetCurrentConfiguration (void) const;

    friend class ConfigurationUpdaterLock;
    /** Return a lock of the called ConfigurationUpdater.  While the
        returned object exists no update of the current configuration is
        made.
    */
    ::boost::shared_ptr<ConfigurationUpdaterLock> GetLock (void);

private:
    /** A reference to the XControllerManager is kept so that
        UpdateConfiguration() has access to the other sub controllers.
    */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XControllerManager> mxControllerManager;

    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfigurationControllerBroadcaster>
            mxBroadcaster;

    /** The current configuration holds the resources that are currently
        active.  It is modified during an update.
    */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfiguration> mxCurrentConfiguration;

    /** The requested configuration holds the resources that have been
        requested to activate or to deactivate since the last update.  It is
        (usually) not modified during an update.  This configuration is
        maintained by the ConfigurationController and given to the
        ConfigurationUpdater in the RequestUpdate() method.
    */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::framework::XConfiguration> mxRequestedConfiguration;

    /** This flag is set to </TRUE> when an update of the current
        configurtion was requested (because the last request in the queue
        was processed) but could not be exected because the
        ConfigurationController was locked.  A call to UpdateConfiguration()
        resets the flag to </FALSE>.
    */
    bool mbUpdatePending;

    /** This flag is set to </TRUE> while the UpdateConfiguration() method
        is running.  It is used to prevent reentrance problems with this
        method.
    */
    bool mbUpdateBeingProcessed;

    /** The ConfigurationController is locked when this count has a value
        larger then zero.  If the controller is locked then updates of the
        current configuration are not made.
    */
    sal_Int32 mnLockCount;

    /** This timer is used to check from time to time whether the requested
        configuration and the current configuration are identcal and request
        an update when they are not.
        This is used to overcome problems with resources that become
        available asynchronously.
    */
    Timer maUpdateTimer;

    /** The number of failed updates (those after which the current
        configuration is not equivalent to the requested configuration) is
        used to determine how long to wait before another update is made.
    */
    sal_Int32 mnFailedUpdateCount;

    /** This method does the main work of an update.  It calls the sub
        controllers that are responsible for the various types of resources
        and tells them to update their active resources.  It notifies
        listeners about the start and end of the configuration update.
    */
    void UpdateConfiguration (void);

    /** Basically calls UpdaterStart() andUpdateEnd() and makes some debug
        output.
    */
    void UpdateCore (const ConfigurationClassifier& rClassifier);

    /** Call the updateStart() method at all resource controllers.
    */
    void UpdateStart (
        const ::std::vector<com::sun::star::uno::Reference<
            com::sun::star::drawing::framework::XResourceId> >& rResourcesToDeactivate);

    /** Call the updateEnd() method at all resource controllers.  The order
        in which the resource controllers are called is the opposite of that
        used in UpdateStart().
    */
    void UpdateEnd (
        const ::std::vector<com::sun::star::uno::Reference<
            com::sun::star::drawing::framework::XResourceId> >& rResourcesToActivate);

    /** Check the success of a recently executed configuration update.
        When the update failed then start the timer.
    */
    void CheckUpdateSuccess (void);

    /** This method sets the mbUpdateBeingProcessed member that is used to
        prevent reentrance problems.  This method allows function objects
        easyly and safely to modify the variable.
    */
    void SetUpdateBeingProcessed (bool bValue);

    /** Return whether it is possible to do an update of the configuration.
        This takes into account whether another update is currently being
        executed, the lock count, and whether the configuration controller
        is still valid.
    */
    bool IsUpdatePossible (void);

    /** Lock updates of the current configuration.  For intermediate requests
        for updates mbUpdatePending is set to <TRUE/>.
    */
    void LockUpdates (void);

    /** When an update was requested since the last LockUpdates() call then
        RequestUpdate() is called.
    */
    void UnlockUpdates (void);

    DECL_LINK(TimeoutHandler, Timer*);
};

} } // end of namespace sd::framework

#endif
