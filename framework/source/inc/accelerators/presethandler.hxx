/*************************************************************************
 *
 *  $RCSfile: presethandler.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 14:53:27 $
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

#ifndef __FRAMEWORK_ACCELERATORS_PRESETHANDLER_HXX_
#define __FRAMEWORK_ACCELERATORS_PRESETHANDLER_HXX_

//__________________________________________
// own includes

#ifndef __FRAMEWORK_ACCELERATORS_STORAGEHOLDER_HXX_
#include <accelerators/storageholder.hxx>
#endif

#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
#include <threadhelp/threadhelpbase.hxx>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

#ifndef __FRAMEWORK_STDTYPES_H_
#include <stdtypes.h>
#endif

//__________________________________________
// interface includes

#ifndef __COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif

#ifndef __COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

//__________________________________________
// other includes

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _SALHELPER_SINGLETONREF_HXX_
#include <salhelper/singletonref.hxx>
#endif

#ifndef _COMPHELPER_LOCALE_HXX_
#include <comphelper/locale.hxx>
#endif

//__________________________________________
// definition

namespace framework
{

//__________________________________________
/**
    TODO document me

        <layer>/global/<resourcetype>/<preset>.xml
        <layer>/modules/<moduleid>/<resourcetype>/<preset>.xml

        RESOURCETYPE        PRESET        TARGET
                            (share)       (user)
        "accelerator"       "default"     "current"
                            "word"
                            "excel"

        "menubar"           "default"     "menubar"

 */
class PresetHandler : private ThreadHelpBase // attention! Must be the first base class to guarentee right initialize lock ...
{
    //-------------------------------------------
    // const

    public:

        static ::rtl::OUString PRESET_DEFAULT();
        static ::rtl::OUString TARGET_CURRENT();

        static ::rtl::OUString RESOURCETYPE_MENUBAR();
        static ::rtl::OUString RESOURCETYPE_TOOLBAR();
        static ::rtl::OUString RESOURCETYPE_ACCELERATOR();
        static ::rtl::OUString RESOURCETYPE_STATUSBAR();

    //-------------------------------------------
    // types

    public:

        //---------------------------------------
        /** @short  this handler can provide different
                    types of configuration.

            @descr  Means: a global or a module dependend
                    or ... configuration.
         */
        enum EConfigType
        {
            E_GLOBAL,
            E_MODULES,
            E_DOCUMENT
        };

    private:

        //---------------------------------------
        /** @short  because a concurrent access to the same storage from different implementations
                    isnt supported, we have to share it with others.

            @descr  This struct makes it possible to use any shared storage
                    in combination with a SingletonRef<> template ...

                    Attention: Because these struct is shared it must be
                    used within a synchronized section. Thats why this struct
                    uses a base class ThreadHelpBase and can be locked
                    from outside doing so!
         */
        struct TSharedStorages : public ThreadHelpBase
        {
            public:

                StorageHolder m_lStoragesShare;
                StorageHolder m_lStoragesUser;

                TSharedStorages()
                    : m_lStoragesShare(::comphelper::getProcessServiceFactory())
                    , m_lStoragesUser (::comphelper::getProcessServiceFactory())
                {};

                virtual ~TSharedStorages() {};
        };

    //-------------------------------------------
    // member

    private:

        //---------------------------------------
        /** @short  can be used to create on needed uno resources. */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        //---------------------------------------
        /** @short  knows the type of provided configuration.

            @descr  e.g. global, modules, ...
         */
        EConfigType m_eConfigType;

        //---------------------------------------
        /** @short  specify the type of resource, which configuration sets
                    must be provided here.

            @descr  e.g. menubars, toolbars, accelerators
         */
        ::rtl::OUString m_sResourceType;

        //---------------------------------------
        /** @short  specify the application module for a module
                    dependend configuration.

            @descr  Will be used only, if m_sResourceType is set to
                    "module". Further it must be a valid module identifier
                    then ...
         */
        ::rtl::OUString m_sModule;

        //---------------------------------------
        /** @short  provides access to the:
                    a) shared root storages
                    b) shared "inbetween" storages
                    of the share and user layer. */
        ::salhelper::SingletonRef< TSharedStorages > m_aSharedStorages;

        //---------------------------------------
        /** @short  if we run in document mode, we cant use the global root storages!
                    We have to use a special document storage explicitly. */
        StorageHolder m_lDocumentStorages;

        //---------------------------------------
        /** @short  holds the folder storage of the share layer alive,
                    where the current configuration set exists.

            @descr  Note: If this preset handler works in document mode
                    this member is meaned relative to the document root ...
                    not to the share layer root!

                    Further is defined, that m_xWorkingStorageUser
                    is equals to m_xWorkingStorageShare then!
         */
        css::uno::Reference< css::embed::XStorage > m_xWorkingStorageShare;

        //---------------------------------------
        /** @short  holds the folder storage of the user layer alive,
                    where the current configuration set exists.

            @descr  Note: If this preset handler works in document mode
                    this member is meaned relative to the document root ...
                    not to the user layer root!

                    Further is defined, that m_xWorkingStorageUser
                    is equals to m_xWorkingStorageShare then!
         */
        css::uno::Reference< css::embed::XStorage > m_xWorkingStorageUser;

        //---------------------------------------
        /** @short  knows the names of all presets inside the current
                    working storage of the share layer. */
        OUStringList m_lPresets;

        //---------------------------------------
        /** @short  knows the names of all targets inside the current
                    working storage of the user layer. */
        OUStringList m_lTargets;

        //---------------------------------------
        /** @short  its the current office locale and will be used
                    to handle localized presets.

            @descr  Default is "x-notranslate" which disable any
                    localized handling inside this class! */
        ::comphelper::Locale m_aLocale;

        //---------------------------------------
        /** @short  knows the relative path from the root. */
        ::rtl::OUString m_sRelPath;

    //-------------------------------------------
    // native interface

    public:

        //---------------------------------------
        /** @short  does nothing real.

            @descr  Because this class should be useable in combination
                    with ::salhelper::SingletonRef template this ctor
                    cant have any special parameters!

            @param  xSMGR
                    points to an uno service manager, which is used internaly
                    to create own needed uno resources.
         */
        PresetHandler(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR);

        //---------------------------------------
        /** @short  copy ctor */
        PresetHandler(const PresetHandler& rCopy);

        //---------------------------------------
        /** @short  closes all open storages ... if user forgot that .-) */
        virtual ~PresetHandler();

        //---------------------------------------
        /** @short  return access to the internaly used and cached root storage.

            @descr  These root storages are the base of all further opened
                    presets and targets. They are provided here only, to support
                    older implementations, which base on them ...

                    getOrCreate...() - What does it mean?
                    Such root storage will be created one times only and
                    cached then internaly till the last instance of such PresetHandler
                    dies.

            @return com::sun::star::embed::XStorage
                    which represent a root storage.
         */
        css::uno::Reference< css::embed::XStorage > getOrCreateRootStorageShare();
        css::uno::Reference< css::embed::XStorage > getOrCreateRootStorageUser();

        //---------------------------------------
        /** @short  provides access to the current working storages.

            @descr  Working storages are the "lowest" storages, where the
                    preset and target files exists.

            @return com::sun::star::embed::XStorage
                    which the current working storage.
         */
        css::uno::Reference< css::embed::XStorage > getWorkingStorageShare();
        css::uno::Reference< css::embed::XStorage > getWorkingStorageUser();

        //---------------------------------------
        /** @short  check if there is a parent storage well known for
                    the specified child storage and return it.

            @param  xChild
                    the child storage where a paranet storage should be searched for.

            @return com::sun::star::embed::XStorage
                    A valid storage if a paranet exists. NULL otherwise.
         */
        css::uno::Reference< css::embed::XStorage > getParentStorageShare(const css::uno::Reference< css::embed::XStorage >& xChild);
        css::uno::Reference< css::embed::XStorage > getParentStorageUser (const css::uno::Reference< css::embed::XStorage >& xChild);

        //---------------------------------------
        /** @short  free all internal structures and let this handler
                    work on a new type of configuration sets.

            @param  eConfigType
                    differ between global or module dependend configuration.

            @param  sResourceType
                    differ between menubar/toolbar/accelerator/... configuration.

            @param  sModule
                    if sResourceType is set to a module dependend configuration,
                    it address the current application module.

            @param  xDocumentRoot
                    if sResourceType is set to E_DOCUMENT, this value points to the
                    root storage inside the document, where we can save our
                    configuration files. Note: Thats not the real root of the document ...
                    its only a sub storage. But we interpret it as our root storage.

            @param  aLocale
                    in case this configuration supports localized entries,
                    the current locale must be set.

                    Localzation will be represented as directory structure
                    of provided presets. Means: you call us with a preset name "default";
                    and we use e.g. "/en-US/default.xml" internaly.

                    If no localization exists for this preset set, this class
                    will work in default mode - means "no locale" - automaticly.
                    e.g. "/default.xml"

            @throw  com::sun::star::uno::RuntimeException(!)
                    if the specified resource couldn't be located.
         */
        void connectToResource(      EConfigType                                  eConfigType   ,
                               const ::rtl::OUString&                             sResourceType ,
                               const ::rtl::OUString&                             sModule       ,
                               const css::uno::Reference< css::embed::XStorage >& xDocumentRoot ,
                               const ::comphelper::Locale&                        aLocale       = ::comphelper::Locale(::comphelper::Locale::X_NOTRANSLATE()));

        //---------------------------------------
        /** @short  return a list of all presets, which are available
                    in the share layer of the current configuration set.

            @descr  Note: The returned strings are ALIAS names of the presets.
                    Because there is no special configuration, which map
                    ALIAS to real PRESET names, we strip the extension
                    of every file. Further all PRESET and ALIAS names
                    are used case insensitive!

            @return A list of all current available presets.
         */
        css::uno::Sequence< ::rtl::OUString > getAllPresetNames() const;

        //---------------------------------------
        /** @short  return a list of all targets, which are available
                    in the user layer of the current configuration set.

            @descr  Note: The returned strings are ALIAS names of the targets.
                    Because there is no special configuration, which map
                    ALIAS to real TARGET names, we strip the extension
                    of every file. Further all TARGET and ALIAS names
                    are used case insensitive!

            @return A list of all current available targets.
         */
        css::uno::Sequence< ::rtl::OUString > getAllTargetNames() const;

        //---------------------------------------
        /** @short  can be used to know, if a required item exists.

            @descr  Of course this operation can work only, if connectToResource()
                    was called before.

            @param  sPreset/sTarget
                    specify the searched item. (ALIAS name!)

            @return TRUE if the requested item exists.
         */
        sal_Bool existsPreset(const ::rtl::OUString& sPreset) const;
        sal_Bool existsTarget(const ::rtl::OUString& sTarget) const;

        //---------------------------------------
        /** @short  try to copy the specified preset from the share
                    layer to the user layer and establish it as the
                    specified target.

            @descr  Means: copy share/.../<preset>.xml user/.../<target>.xml
                    Note: The target will be overwritten completly or
                    created as new by this operation!

            @param  sPreset
                    the ALIAS name of an existing preset.

            @param  sTarget
                    the ALIAS name of the target.

            @throw  com::sun::star::container::NoSuchElementException
                    if the specified preset does not exists.

            @throw  com::sun::star::io::IOException
                    if copying failed.
         */
        void copyPresetToTarget(const ::rtl::OUString& sPreset,
                                const ::rtl::OUString& sTarget);

        //---------------------------------------
        /** @short  open the specified preset as stream object
                    and return it.

            @descr  Note: Because presets resist inside the share
                    layer, they will be opened readonly everytimes.

            @param  sPreset
                    the ALIAS name of an existing preset.

            @return The opened preset stream ... or NULL if the preset does not exists.
         */
        css::uno::Reference< css::io::XStream > openPreset(const ::rtl::OUString& sPreset);

        //---------------------------------------
        /** @short  open the specified target as stream object
                    and return it.

            @descr  Note: Targets resist inside the user
                    layer. Normaly they are opened in read/write mode.
                    But it will be opened readonly automaticly if that isnt possible
                    (may be the file is write protected on the system ...).

            @param  sTarget
                    the ALIAS name of the target.

            @param  bCreateIfMissing
                    create target file, if it does not still exists.
                    Note: That does not means reseting of an existing file!

            @return The opened target stream ... or NULL if the target does not exists
                    or couldnt be created as new one.
         */
        css::uno::Reference< css::io::XStream > openTarget(const ::rtl::OUString& sTarget         ,
                                                                 sal_Bool         bCreateIfMissing);

        //---------------------------------------
        /** TODO */
        void removeTarget(const ::rtl::OUString& sTarget);

        //---------------------------------------
        /** @short  do anything which is neccessary to flush all changes
                    back to disk.

            @descr  We have to call commit on all cached sub storages on the
                    path from the root storage upside down to the working storage
                    (which are not realy used, but required to be holded alive!).
         */
        void commitUserChanges();

        //---------------------------------------
        /** TODO */
        void addStorageListener(IStorageListener* pListener);
        void removeStorageListener(IStorageListener* pListener);

    //-------------------------------------------
    // helper

    private:

        //---------------------------------------
        /** @short  open a config path ignoring errors (catching exceptions).

            @descr  We catch only normal exceptions here - no runtime exceptions.

            @param  sPath
                    the configuration path, which should be opened.

            @param  eMode
                    the open mode (READ/READWRITE)

            @param  bShare
                    force using of the share layer instead of the user layer.

            @return An opened storage in case method was successfully - null otherwise.
         */
        css::uno::Reference< css::embed::XStorage > impl_openPathIgnoringErrors(const ::rtl::OUString& sPath ,
                                                                                      sal_Int32        eMode ,
                                                                                      sal_Bool         bShare);

        //---------------------------------------
        /** @short  open a config path ignoring errors (catching exceptions).

            @descr  We catch only normal exceptions here - no runtime exceptions.
                    Further the path itself is tries in different versions (using locale
                    specific attributes).
                    e.g. "path/e-US" => "path/en" => "path/de"

            @param  sPath
                    the configuration path, which should be opened.
                    Its further used as out parameter too, so we can return the localized
                    path to the calli!

            @param  eMode
                    the open mode (READ/READWRITE)

            @param  bShare
                    force using of the share layer instead of the user layer.

            @param  aLocale
                    the start locale ...

            @return An opened storage in case method was successfully - null otherwise.
         */
        css::uno::Reference< css::embed::XStorage > impl_openLocalizedPathIgnoringErrors(      ::rtl::OUString&      sPath  ,
                                                                                               sal_Int32             eMode  ,
                                                                                               sal_Bool              bShare ,
                                                                                         const ::comphelper::Locale& aLocale);
};

} // namespace framework

#endif // __FRAMEWORK_ACCELERATORS_PRESETHANDLER_HXX_
