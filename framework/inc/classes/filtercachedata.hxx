/*************************************************************************
 *
 *  $RCSfile: filtercachedata.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: as $ $Date: 2001-07-20 08:05:54 $
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

#ifndef __FRAMEWORK_CLASSES_FILTERCACHEDATA_HXX_
#define __FRAMEWORK_CLASSES_FILTERCACHEDATA_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_CLASSES_CHECKEDITERATOR_HXX_
#include <classes/checkediterator.hxx>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

#ifndef __FRAMEWORK_MACROS_DEBUG_HXX_
#include <macros/debug.hxx>
#endif

#include <classes/wildcard.hxx>

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#ifndef _RTL_USTRING_
#include <rtl/ustring.hxx>
#endif

#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#ifndef __SGI_STL_HASH_MAP
#include <hash_map>
#endif

#ifndef __SGI_STL_VECTOR
#include <vector>
#endif

#ifndef __SGI_STL_ITERATOR
#include <iterator>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//  exported const
//_________________________________________________________________________________________________________________

#define PACKAGENAME_TYPEDETECTION_STANDARD          DECLARE_ASCII("Office.TypeDetection"                            )   /// Names of our configuration files.
#define PACKAGENAME_TYPEDETECTION_ADDITIONAL        DECLARE_ASCII("Office.TypeDetectionAdditional"                  )
#define CFG_PATH_SEPERATOR                          DECLARE_ASCII("/"                                               )   /// seperator for configuration pathes
#define PROPERTY_SEPERATOR                          sal_Unicode(',')                                                    /// seperator for own formated property strings of types and filters
#define LIST_SEPERATOR                              sal_Unicode(';')                                                    /// seperator for own formated lists as part of our own formated type- or filter-string
#define LOCALE_FALLBACK                             DECLARE_ASCII("en-US"                                           )   /// fallback, if configuration can't give us current set locale ...
#define DEFAULT_FILTERCACHE_VERSION                 6                                                                   /// these implmentation of FilterCache support different version of TypeDetection.xml! This define the current set default one.
#define DEFAULT_FILTERCACHE_MODE                    CONFIG_MODE_DELAYED_UPDATE                                          /// ConfigItems could run in different modes: supported values are ... { CONFIG_MODE_IMMEDIATE_UPDATE, CONFIG_MODE_DELAYED_UPDATE, CONFIG_MODE_ALL_LOCALES }
#define CFG_ENCODING_OPEN                           DECLARE_ASCII("[\'"                                             )   /// used to start encoding of set names
#define CFG_ENCODING_CLOSE                          DECLARE_ASCII("\']"                                             )   /// used to finish encoding of set names

//*****************************************************************************************************************
// We know some default values ...
//*****************************************************************************************************************
#define NAME_DEFAULTDETECTOR                        DECLARE_ASCII("com.sun.star.comp.office.FilterDetect"           )
#define NAME_GENERICLOADER                          DECLARE_ASCII("com.sun.star.comp.office.FrameLoader"            )
#define UINAME_GENERICLOADER                        DECLARE_ASCII("com.sun.star.comp.office.FrameLoader"            )
#define TYPELIST_DEFAULTDETECTOR                    DECLARE_ASCII("*"                                               )
#define TYPELIST_GENERICLOADER                      DECLARE_ASCII("*"                                               )

//*****************************************************************************************************************
// This are all supported set-names of our filter configuration.
//*****************************************************************************************************************
#define SUBLIST_TYPES                               DECLARE_ASCII("Types"                                           )
#define SUBLIST_FILTERS                             DECLARE_ASCII("Filters"                                         )
#define SUBLIST_DETECTSERVICES                      DECLARE_ASCII("DetectServices"                                  )
#define SUBLIST_FRAMELOADERS                        DECLARE_ASCII("FrameLoaders"                                    )
#define SUBLIST_CONTENTHANDLERS                     DECLARE_ASCII("ContentHandlers"                                 )
#define SUBLIST_PROTOCOLHANDLERS                    DECLARE_ASCII("ProtocolHandlers"                                )
#define SUBLIST_DEFAULTS                            DECLARE_ASCII("Defaults"                                        )

#define TEMPLATENAME_TYPE                           DECLARE_ASCII("Type"                                            )
#define TEMPLATENAME_FILTER                         DECLARE_ASCII("Filter"                                          )
#define TEMPLATENAME_DETECTSERVICE                  DECLARE_ASCII("DetectService"                                   )
#define TEMPLATENAME_FRAMELOADER                    DECLARE_ASCII("FrameLoader"                                     )
#define TEMPLATENAME_CONTENTHANDLER                 DECLARE_ASCII("ContentHandler"                                  )
#define TEMPLATENAME_PROTOCOLHANDLER                DECLARE_ASCII("ProtocolHandler"                                 )

//*****************************************************************************************************************
// These defines declare all supported names of configuration key names.
// They are not sorted and could be shared by different base configuration items.
//*****************************************************************************************************************
#define SUBKEY_PREFERRED                            DECLARE_ASCII("Preferred"                                       )
#define SUBKEY_INSTALLED                            DECLARE_ASCII("Installed"                                       )
#define SUBKEY_NAME                                 DECLARE_ASCII("Name"                                            )
#define SUBKEY_UINAME                               DECLARE_ASCII("UIName"                                          )
#define SUBKEY_MEDIATYPE                            DECLARE_ASCII("MediaType"                                       )
#define SUBKEY_CLIPBOARDFORMAT                      DECLARE_ASCII("ClipboardFormat"                                 )
#define SUBKEY_URLPATTERN                           DECLARE_ASCII("URLPattern"                                      )
#define SUBKEY_EXTENSIONS                           DECLARE_ASCII("Extensions"                                      )
#define SUBKEY_DOCUMENTICONID                       DECLARE_ASCII("DocumentIconID"                                  )
#define SUBKEY_TYPE                                 DECLARE_ASCII("Type"                                            )
#define SUBKEY_DOCUMENTSERVICE                      DECLARE_ASCII("DocumentService"                                 )
#define SUBKEY_FILTERSERVICE                        DECLARE_ASCII("FilterService"                                   )
#define SUBKEY_FLAGS                                DECLARE_ASCII("Flags"                                           )
#define SUBKEY_USERDATA                             DECLARE_ASCII("UserData"                                        )
#define SUBKEY_FILEFORMATVERSION                    DECLARE_ASCII("FileFormatVersion"                               )
#define SUBKEY_TEMPLATENAME                         DECLARE_ASCII("TemplateName"                                    )
#define SUBKEY_TYPES                                DECLARE_ASCII("Types"                                           )
#define SUBKEY_ORDER                                DECLARE_ASCII("Order"                                           )
#define SUBKEY_DEFAULTDETECTOR                      DECLARE_ASCII("DetectService"                                   )
#define SUBKEY_GENERICLOADER                        DECLARE_ASCII("FrameLoader"                                     )
#define SUBKEY_DATA                                 DECLARE_ASCII("Data"                                            )
#define SUBKEY_PROTOCOLS                            DECLARE_ASCII("Protocols"                                       )

//*****************************************************************************************************************
// These defines declare all supported property names for our name container interface.
// They are not sorted by using ... and could be shared by different methods and access operations.
//*****************************************************************************************************************
#define PROPERTY_PREFERRED                          DECLARE_ASCII("Preferred"                                       )
#define PROPERTY_INSTALLED                          DECLARE_ASCII("Installed"                                       )
#define PROPERTY_UINAME                             DECLARE_ASCII("UIName"                                          )
#define PROPERTY_UINAMES                            DECLARE_ASCII("UINames"                                         )
#define PROPERTY_MEDIATYPE                          DECLARE_ASCII("MediaType"                                       )
#define PROPERTY_CLIPBOARDFORMAT                    DECLARE_ASCII("ClipboardFormat"                                 )
#define PROPERTY_URLPATTERN                         DECLARE_ASCII("URLPattern"                                      )
#define PROPERTY_EXTENSIONS                         DECLARE_ASCII("Extensions"                                      )
#define PROPERTY_DOCUMENTICONID                     DECLARE_ASCII("DocumentIconID"                                  )
#define PROPERTY_TYPE                               DECLARE_ASCII("Type"                                            )
#define PROPERTY_DOCUMENTSERVICE                    DECLARE_ASCII("DocumentService"                                 )
#define PROPERTY_FILTERSERVICE                      DECLARE_ASCII("FilterService"                                   )
#define PROPERTY_FLAGS                              DECLARE_ASCII("Flags"                                           )
#define PROPERTY_USERDATA                           DECLARE_ASCII("UserData"                                        )
#define PROPERTY_FILEFORMATVERSION                  DECLARE_ASCII("FileFormatVersion"                               )
#define PROPERTY_TEMPLATENAME                       DECLARE_ASCII("TemplateName"                                    )
#define PROPERTY_TYPES                              DECLARE_ASCII("Types"                                           )
#define PROPERTY_ORDER                              DECLARE_ASCII("Order"                                           )
#define PROPERTY_PROTOCOLS                          DECLARE_ASCII("Protocols"                                       )

//*****************************************************************************************************************
// These values specify count of supported properties at our NameContainer interface!
// Attention: It's not the count of properties of a type, filter ... written in configuration.
//            That value is named as SUBKEYCOUNT_...! See there for further informations.
//*****************************************************************************************************************
/*
#define PROPCOUNT_TYPE                              8
#define PROPCOUNT_FILTER                            10
#define PROPCOUNT_DETECTOR                          1
#define PROPCOUNT_LOADER                            3
#define PROPCOUNT_CONTENTHANDLER                    1
#define PROPCOUNT_PROTOCOLHANDLER                   1
*/
//*****************************************************************************************************************
// These values specify count of properties of every configuration item.
// We support different versions and so we must handle different counts of type- and filter-properties.
// Attention: Look for different values on PROPCOUNT_... and SUBKEYCOUNT_...!
//*****************************************************************************************************************
/*
#define SUBKEYCOUNT_TYPE_VERSION_1                  7
#define SUBKEYCOUNT_TYPE_VERSION_2                  SUBKEYCOUNT_TYPE_VERSION_1
#define SUBKEYCOUNT_TYPE_VERSION_3                  2
#define SUBKEYCOUNT_TYPE_VERSION_4                  SUBKEYCOUNT_TYPE_VERSION_3
#define SUBKEYCOUNT_TYPE_VERSION_5                  SUBKEYCOUNT_TYPE_VERSION_3
#define SUBKEYCOUNT_TYPE_VERSION_6                  SUBKEYCOUNT_TYPE_VERSION_3
#define SUBKEYCOUNT_TYPE_VERSION_7                  SUBKEYCOUNT_TYPE_VERSION_3
#define SUBKEYCOUNT_FILTER_VERSION_1                9
#define SUBKEYCOUNT_FILTER_VERSION_2                10
#define SUBKEYCOUNT_FILTER_VERSION_3                3
#define SUBKEYCOUNT_FILTER_VERSION_4                SUBKEYCOUNT_FILTER_VERSION_3
#define SUBKEYCOUNT_FILTER_VERSION_5                SUBKEYCOUNT_FILTER_VERSION_3
#define SUBKEYCOUNT_FILTER_VERSION_6                SUBKEYCOUNT_FILTER_VERSION_3
#define SUBKEYCOUNT_FILTER_VERSION_7                SUBKEYCOUNT_FILTER_VERSION_3
#define SUBKEYCOUNT_DETECTOR                        1
#define SUBKEYCOUNT_LOADER                          2
#define SUBKEYCOUNT_CONTENTHANDLER                  1
#define SUBKEYCOUNT_PROTOCOLHANDLER                 1
*/
/*
#define CFGPROPERTY_NODEPATH                        DECLARE_ASCII("nodepath"                                        )   // describe path of cfg entry
#define CFGPROPERTY_LAZYWRITE                       DECLARE_ASCII("lazywrite"                                       )   // true->async. update; false->sync. update
#define CFGPROPERTY_DEPTH                           DECLARE_ASCII("depth"                                           )   // depth of view
#define CFGPROPERTY_NOCACHE                         DECLARE_ASCII("nocache"                                         )   // use cache or not
#define CFGPROPERTY_USER                            DECLARE_ASCII("user"                                            )   // specify user
#define CFGPROPERTY_LOCALE                          DECLARE_ASCII("locale"                                          )   // set locale of cfg entry
#define CFGPROPERTY_SERVERTYPE                      DECLARE_ASCII("servertype"                                      )   // specify type of used configuration (fatoffice, network, webtop)
#define CFGPROPERTY_SOURCEPATH                      DECLARE_ASCII("sourcepath"                                      )   // specify path to "share/config/registry" files
#define CFGPROPERTY_UPDATEPATH                      DECLARE_ASCII("updatepath"                                      )   // specify path to "user/config/registry" files
*/
//_________________________________________________________________________________________________________________
//  exported definitions
//_________________________________________________________________________________________________________________

enum EModifyState
{
    E_UNTOUCHED ,
    E_ADDED     ,
    E_CHANGED   ,
    E_REMOVED
};

enum EFilterPackage
{
    E_STANDARD  ,
    E_ADDITIONAL
};

//*****************************************************************************************************************
// Hash code function for using in all hash maps of follow implementation.
struct StringHashFunction
{
    size_t operator()(const ::rtl::OUString& sString) const
    {
        return sString.hashCode();
    }
};

//*****************************************************************************************************************
// A generic string list to hold different string informations with a fast access to it.
// Implment new free function to clear memory realy!
//*****************************************************************************************************************
class StringList : public ::std::vector< ::rtl::OUString >
{
    public:
        inline void push_front( const ::rtl::OUString& sElement )
        {
            insert( begin(), sElement );
        }

        inline void free()
        {
            StringList().swap( *this );
        }
};

class StringHash : public ::std::hash_map<  ::rtl::OUString                     ,
                                            ::rtl::OUString                     ,
                                            StringHashFunction                  ,
                                            ::std::equal_to< ::rtl::OUString >  >
{
    public:
        void free()
        {
            StringHash().swap( *this );
        }
};

//*****************************************************************************************************************
// These struct define a type, which present the type of a file.
// He is used for easy filter detection without file stream detection!
// The internal name is the keyname of an item with these structure in our hash map or our configuration set!
//*****************************************************************************************************************
struct FileType
{
    //-------------------------------------------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        inline               FileType   (                               ) { impl_clear();               }
        inline               FileType   (   const   FileType&   rCopy   ) { impl_copy( rCopy );         }
        inline              ~FileType   (                               ) { impl_clear();               }
        inline FileType&    operator=   (   const   FileType&   rCopy   ) { return impl_copy( rCopy );  }
        inline void         free        (                               ) { impl_clear();               }

    //-------------------------------------------------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        inline void impl_clear()
        {
            bPreferred          = sal_False         ;
            sName               = ::rtl::OUString() ;
            sMediaType          = ::rtl::OUString() ;
            sClipboardFormat    = ::rtl::OUString() ;
            nDocumentIconID     = 0                 ;
            lUINames.free   ();
            lURLPattern.free();
            lExtensions.free();
        }

        inline FileType& impl_copy( const FileType& rCopy )
        {
            bPreferred          = rCopy.bPreferred      ;
            sName               = rCopy.sName           ;
            lUINames            = rCopy.lUINames        ;
            sMediaType          = rCopy.sMediaType      ;
            sClipboardFormat    = rCopy.sClipboardFormat;
            nDocumentIconID     = rCopy.nDocumentIconID ;
            lURLPattern         = rCopy.lURLPattern     ;
            lExtensions         = rCopy.lExtensions     ;
            return (*this);
        }

    //-------------------------------------------------------------------------------------------------------------
    // public member
    //-------------------------------------------------------------------------------------------------------------
    public:

        sal_Bool            bPreferred          ;
        ::rtl::OUString     sName               ;
        StringHash          lUINames            ;
        ::rtl::OUString     sMediaType          ;
        ::rtl::OUString     sClipboardFormat    ;
        sal_Int32           nDocumentIconID     ;
        StringList          lURLPattern         ;
        StringList          lExtensions         ;
};

//*****************************************************************************************************************
// These struct describe a filter which is registered for one type.
// He hold information about services which present the document himself (like a item) and a filter service which
// filter a file in these document.
// The internal name is the keyname of an item with these structure in our hash map or our configuration set!
//*****************************************************************************************************************
struct Filter
{
    //-------------------------------------------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        inline           Filter     (                           ) { impl_clear();               }
        inline           Filter     (   const   Filter& rCopy   ) { impl_copy( rCopy );         }
        inline          ~Filter     (                           ) { impl_clear();               }
        inline Filter&  operator=   (   const   Filter& rCopy   ) { return impl_copy( rCopy );  }
        inline void     free        (                           ) { impl_clear();               }

    //-------------------------------------------------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        inline void impl_clear()
        {
            nOrder              = 0                ;
            sName               = ::rtl::OUString();
            sType               = ::rtl::OUString();
            sDocumentService    = ::rtl::OUString();
            sFilterService      = ::rtl::OUString();
            nFlags              = 0                ;
            nFileFormatVersion  = 0                ;
            sTemplateName       = ::rtl::OUString();
            lUINames.free   ();
            lUserData.free  ();
        }

        inline Filter& impl_copy( const Filter& rCopy )
        {
            nOrder              = rCopy.nOrder              ;
            sName               = rCopy.sName               ;
            sType               = rCopy.sType               ;
            lUINames            = rCopy.lUINames            ;
            sDocumentService    = rCopy.sDocumentService    ;
            sFilterService      = rCopy.sFilterService      ;
            nFlags              = rCopy.nFlags              ;
            nFileFormatVersion  = rCopy.nFileFormatVersion  ;
            sTemplateName       = rCopy.sTemplateName       ;
            lUserData           = rCopy.lUserData           ;
            return (*this);
        }

    //-------------------------------------------------------------------------------------------------------------
    // public member
    //-------------------------------------------------------------------------------------------------------------
    public:

        sal_Int32           nOrder              ;
        ::rtl::OUString     sName               ;
        ::rtl::OUString     sType               ;
        StringHash          lUINames            ;
        ::rtl::OUString     sDocumentService    ;
        ::rtl::OUString     sFilterService      ;
        sal_Int32           nFlags              ;
        StringList          lUserData           ;
        sal_Int32           nFileFormatVersion  ;
        ::rtl::OUString     sTemplateName       ;
};

//*****************************************************************************************************************
// Programmer can register his own services for an content detection of different types.
// The implementation or service name of these is the keyname of an item with these structure
// in our hash map or our configuration set!
//*****************************************************************************************************************
struct Detector
{
    //-------------------------------------------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        inline               Detector   (                               ) { impl_clear();               }
        inline               Detector   (   const   Detector&   rCopy   ) { impl_copy( rCopy );         }
        inline              ~Detector   (                               ) { impl_clear();               }
        inline Detector&    operator=   (   const   Detector&   rCopy   ) { return impl_copy( rCopy );  }
        inline void         free        (                               ) { impl_clear();               }

    //-------------------------------------------------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        inline void impl_clear()
        {
            sName = ::rtl::OUString();
            lTypes.free();
        }

        inline Detector& impl_copy( const Detector& rCopy )
        {
            sName  = rCopy.sName  ;
            lTypes = rCopy.lTypes ;
            return (*this);
        }

    //-------------------------------------------------------------------------------------------------------------
    // public member
    //-------------------------------------------------------------------------------------------------------------
    public:

        ::rtl::OUString     sName       ;
        StringList          lTypes      ;
};

//*****************************************************************************************************************
// Programmer can register his own services for loading documents in a frame.
// The implementation or service name of these is the keyname of an item with these structure
// in our hash map or our configuration set!
//*****************************************************************************************************************
struct Loader
{
    //-------------------------------------------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        inline           Loader     (                           ) { impl_clear();               }
        inline           Loader     (   const   Loader& rCopy   ) { impl_copy( rCopy );         }
        inline          ~Loader     (                           ) { impl_clear();               }
        inline Loader&  operator=   (   const   Loader& rCopy   ) { return impl_copy( rCopy );  }
        inline void     free        (                           ) { impl_clear();               }

    //-------------------------------------------------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        inline void impl_clear()
        {
            sName = ::rtl::OUString();
            lUINames.free   ();
            lTypes.free     ();
        }

        inline Loader& impl_copy( const Loader& rCopy )
        {
            sName       = rCopy.sName       ;
            lUINames    = rCopy.lUINames    ;
            lTypes      = rCopy.lTypes      ;
            return (*this);
        }

    //-------------------------------------------------------------------------------------------------------------
    // public member
    //-------------------------------------------------------------------------------------------------------------
    public:

        ::rtl::OUString sName       ;
        StringHash      lUINames    ;
        StringList      lTypes      ;
};

//*****************************************************************************************************************
// Programmer can register his own services to handle a FileType and intercept dispatches.
// Don't forget: It's not a FrameLoader - it's a ContentHandler! (normaly without any UI)
//*****************************************************************************************************************
struct ContentHandler
{
    //-------------------------------------------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        inline                   ContentHandler(                              ) { impl_clear();               }
        inline                   ContentHandler( const ContentHandler& rCopy  ) { impl_copy( rCopy );         }
        inline                  ~ContentHandler(                              ) { impl_clear();               }
        inline ContentHandler&   operator=     ( const ContentHandler& rCopy  ) { return impl_copy( rCopy );  }
        inline void              free          (                              ) { impl_clear();               }

    //-------------------------------------------------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        inline void impl_clear()
        {
            sName = ::rtl::OUString();
            lTypes.free();
        }

        inline ContentHandler& impl_copy( const ContentHandler& rCopy )
        {
            sName  = rCopy.sName ;
            lTypes = rCopy.lTypes;
            return (*this);
        }

    //-------------------------------------------------------------------------------------------------------------
    // public member
    //-------------------------------------------------------------------------------------------------------------
    public:

        ::rtl::OUString     sName   ;
        StringList          lTypes  ;
};

//*****************************************************************************************************************
// Programmer can register his own services to handle different protocols and intercept dispatches.
// Don't forget: It doesn't mean "handling of documents" ... these services could handle protocols ...
// e.g. "mailto:*", "file://*"
//*****************************************************************************************************************
struct ProtocolHandler
{
    //-------------------------------------------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------------------------------------------
    public:

        inline                   ProtocolHandler(                              ) { impl_clear();               }
        inline                   ProtocolHandler( const ProtocolHandler& rCopy ) { impl_copy( rCopy );         }
        inline                  ~ProtocolHandler(                              ) { impl_clear();               }
        inline ProtocolHandler&  operator=      ( const ProtocolHandler& rCopy ) { return impl_copy( rCopy );  }
        inline void              free           (                              ) { impl_clear();               }

    //-------------------------------------------------------------------------------------------------------------
    // private methods
    //-------------------------------------------------------------------------------------------------------------
    private:

        inline void impl_clear()
        {
            sName = ::rtl::OUString();
            lProtocols.free();
        }

        inline ProtocolHandler& impl_copy( const ProtocolHandler& rCopy )
        {
            sName       = rCopy.sName       ;
            lProtocols  = rCopy.lProtocols  ;
            return (*this);
        }

    //-------------------------------------------------------------------------------------------------------------
    // public member
    //-------------------------------------------------------------------------------------------------------------
    public:

        ::rtl::OUString     sName       ;
        StringList          lProtocols  ;
};

//*****************************************************************************************************************
// We need different hash maps for different tables of our configuration management.
// Follow maps convert <names> to <properties> of type, filter, detector, loader ...
// and could be used in a generic way
//*****************************************************************************************************************
template< class HashType >
class SetNodeHash : public ::std::hash_map< ::rtl::OUString                    ,
                                            HashType                           ,
                                            StringHashFunction                 ,
                                            ::std::equal_to< ::rtl::OUString > >
{
    //-------------------------------------------------------------------------------------------------------------
    // interface
    //-------------------------------------------------------------------------------------------------------------
    public:
        //---------------------------------------------------------------------------------------------------------
        // The only way to free ALL memory realy!
        //---------------------------------------------------------------------------------------------------------
        inline void free()
        {
            SetNodeHash().swap( *this );
            lAddedItems.free  ();
            lChangedItems.free();
            lRemovedItems.free();
        }

        //---------------------------------------------------------------------------------------------------------
        // Append changed, added or removed items to special lists
        // Neccessary for saving changes
        //---------------------------------------------------------------------------------------------------------
        inline void appendChange( const ::rtl::OUString& sItemName ,
                                        EModifyState     eState    )
        {
            switch( eState )
            {
                case E_ADDED   :  lAddedItems.push_back  ( sItemName );
                                    break;
                case E_CHANGED :  lChangedItems.push_back( sItemName );
                                    break;
                case E_REMOVED :  lRemovedItems.push_back( sItemName );
                                    break;
            }
}

    //-------------------------------------------------------------------------------------------------------------
    // member
    //-------------------------------------------------------------------------------------------------------------
    public:
        StringList  lAddedItems    ;
        StringList  lChangedItems  ;
        StringList  lRemovedItems  ;
};

//*****************************************************************************************************************
// Use these hashes to implement different tables which assign types to frame loader or detect services.
// It's an optimism to find registered services faster!
// The preferred hash maps file extensions to preferred types to find these ones faster.
//*****************************************************************************************************************
class PerformanceHash   :   public  ::std::hash_map<    ::rtl::OUString                     ,
                                                        StringList                          ,
                                                        StringHashFunction                  ,
                                                        ::std::equal_to< ::rtl::OUString >  >
{
    public:
        //---------------------------------------------------------------------------------------------------------
        //  try to free all used memory REALY!
        //---------------------------------------------------------------------------------------------------------
        inline void free()
        {
            PerformanceHash().swap( *this );
        }

        //---------------------------------------------------------------------------------------------------------
        //  normaly a complete string must match our hash key values ...
        //  But sometimes we need a search by using these key values as pattern!
        //  The in/out parameter "pStepper" is used to return a pointer to found element in hash ...
        //  and could be used for further searches again, which should be started at next element!
        //  We stop search at the end of hash. You can start it again by setting it to the begin by himself.
        //---------------------------------------------------------------------------------------------------------
        inline sal_Bool findPatternKey( const ::rtl::OUString& sSearchValue ,
                                              const_iterator&  pStepper     )
        {
            sal_Bool bFound = sal_False;

            // If this is the forst call - start search on first element.
            // Otherwise start search on further elements!
            if( pStepper != begin() )
            {
                ++pStepper;
            }

            while(
                    ( pStepper != end()     )   &&
                    ( bFound   == sal_False )
                )
            {
                bFound = Wildcard::match( sSearchValue, pStepper->first );
                // If element was found - break loop by setting right return value
                // and don't change "pStepper". He must point to found element!
                // Otherwise step to next one.
                if( bFound == sal_False )
                    ++pStepper;
            }
            return bFound;
        }
};

//*****************************************************************************************************************
// Define easy usable types
//*****************************************************************************************************************
typedef SetNodeHash< FileType >                                     FileTypeHash                ;
typedef SetNodeHash< Filter >                                       FilterHash                  ;
typedef SetNodeHash< Detector >                                     DetectorHash                ;
typedef SetNodeHash< Loader >                                       LoaderHash                  ;
typedef SetNodeHash< ContentHandler >                               ContentHandlerHash          ;
typedef SetNodeHash< ProtocolHandler >                              ProtocolHandlerHash         ;
typedef StringHash                                                  PreferredHash               ;
typedef StringList                                                  OrderList                   ;

typedef StringList::iterator                                        StringListIterator          ;
typedef StringList::const_iterator                                  ConstStringListIterator     ;
typedef StringHash::const_iterator                                  ConstStringHashIterator     ;
typedef FileTypeHash::const_iterator                                ConstTypeIterator           ;
typedef FilterHash::const_iterator                                  ConstFilterIterator         ;
typedef DetectorHash::const_iterator                                ConstDetectorIterator       ;
typedef LoaderHash::const_iterator                                  ConstLoaderIterator         ;
typedef ContentHandlerHash::const_iterator                          ConstContentHandlerIterator ;
typedef ProtocolHandlerHash::const_iterator                         ConstProtocolHandlerIterator;
typedef PerformanceHash::const_iterator                             ConstPerformanceIterator    ;
typedef PreferredHash::const_iterator                               ConstPreferredIterator      ;

typedef CheckedIterator< StringList >                               CheckedStringListIterator   ;
typedef CheckedIterator< FileTypeHash >                             CheckedTypeIterator         ;
typedef CheckedIterator< PerformanceHash >                          CheckedPerformanceIterator  ;

//*****************************************************************************************************************
// Use private static data container to hold all values of configuration!
//*****************************************************************************************************************
class DataContainer
{
    public:
        void free                   (                                                                   );
        void mergeData              (   const   DataContainer&      rData                               );

        void addType                (   const   FileType&           aType       , sal_Bool bSetModified );
        void addFilter              (   const   Filter&             aFilter     , sal_Bool bSetModified );
        void addDetector            (   const   Detector&           aDetector   , sal_Bool bSetModified );
        void addLoader              (   const   Loader&             aLoader     , sal_Bool bSetModified );
        void addContentHandler      (   const   ContentHandler&     aHandler    , sal_Bool bSetModified );
        void addProtocolHandler     (   const   ProtocolHandler&    aHandler    , sal_Bool bSetModified );

        void replaceType            (   const   FileType&           aType       , sal_Bool bSetModified );
        void replaceFilter          (   const   Filter&             aFilter     , sal_Bool bSetModified );
        void replaceDetector        (   const   Detector&           aDetector   , sal_Bool bSetModified );
        void replaceLoader          (   const   Loader&             aLoader     , sal_Bool bSetModified );
        void replaceContentHandler  (   const   ContentHandler&     aHandler    , sal_Bool bSetModified );
        void replaceProtocolHandler (   const   ProtocolHandler&    aHandler    , sal_Bool bSetModified );

        void removeType             (   const   ::rtl::OUString&    sName       , sal_Bool bSetModified );
        void removeFilter           (   const   ::rtl::OUString&    sName       , sal_Bool bSetModified );
        void removeDetector         (   const   ::rtl::OUString&    sName       , sal_Bool bSetModified );
        void removeLoader           (   const   ::rtl::OUString&    sName       , sal_Bool bSetModified );
        void removeContentHandler   (   const   ::rtl::OUString&    sName       , sal_Bool bSetModified );
        void removeProtocolHandler  (   const   ::rtl::OUString&    sName       , sal_Bool bSetModified );

        static void             convertStringSequenceToVector              ( const css::uno::Sequence< ::rtl::OUString >&              lSource         ,
                                                                                   StringList&                                         lDestination    );
        static void             convertStringVectorToSequence              ( const StringList&                                         lSource         ,
                                                                                   css::uno::Sequence< ::rtl::OUString >&              lDestination    );
        static void             convertFileTypeToPropertySequence          ( const FileType&                                           aSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    ,
                                                                             const ::rtl::OUString&                                    sCurrentLocale  );
        static void             convertFilterToPropertySequence            ( const Filter&                                             aSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    ,
                                                                             const ::rtl::OUString&                                    sCurrentLocale  );
        static void             convertDetectorToPropertySequence          ( const Detector&                                           aSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    );
        static void             convertLoaderToPropertySequence            ( const Loader&                                             aSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    ,
                                                                             const ::rtl::OUString&                                    sCurrentLocale  );
        static void             convertContentHandlerToPropertySequence    ( const ContentHandler&                                     aSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    );
        static void             convertProtocolHandlerToPropertySequence   ( const ProtocolHandler&                                    aSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    );
        static void             convertPropertySequenceToFilter            ( const css::uno::Sequence< css::beans::PropertyValue >&    lSource         ,
                                                                                   Filter&                                             aDestination    ,
                                                                             const ::rtl::OUString&                                    sCurrentLocale  );
        static void             convertStringHashToSequence                ( const StringHash&                                         lSource         ,
                                                                                   css::uno::Sequence< css::beans::PropertyValue >&    lDestination    );
        static void             convertSequenceToStringHash                ( const css::uno::Sequence< css::beans::PropertyValue >&    lSource         ,
                                                                                   StringHash&                                         lDestination    );
        static void             extractLocalizedStrings                    ( const ::rtl::OUString&                                    sCurrentLocale  ,
                                                                             const css::uno::Any&                                      aCFGValue       ,
                                                                                   StringHash&                                         lLocales        );
        static void             packLocalizedStrings                       ( const ::rtl::OUString&                                    sCurrentLocale  ,
                                                                                   css::uno::Any&                                      aCFGValue       ,
                                                                             const StringHash&                                         lLocales        );
        static ::rtl::OUString  getLocalelizedString                       ( const StringHash&                                         lLocales        ,
                                                                             const ::rtl::OUString&                                    sLocale         );
        static void             setLocalelizedString                       (       StringHash&                                         lLocales        ,
                                                                             const ::rtl::OUString&                                    sLocale         ,
                                                                             const ::rtl::OUString&                                    sValue          );
        static void             correctExtensions                          (       StringList&                                         lExtensions     );

    public:

        FileTypeHash            m_aTypeCache                ;     /// hold all informations about registered file types
        FilterHash              m_aFilterCache              ;     /// hold all informations about registered filters
        DetectorHash            m_aDetectorCache            ;     /// hold all informations about registered detect services
        LoaderHash              m_aLoaderCache              ;     /// hold all informations about registered loader services
        ContentHandlerHash      m_aContentHandlerCache      ;     /// hold all informations about registered content handler services
        ProtocolHandlerHash     m_aProtocolHandlerCache     ;     /// hold all informations about registered protocol handler services
        PerformanceHash         m_aFastFilterCache          ;     /// hold all registered filter for a special file type
        PerformanceHash         m_aFastDetectorCache        ;     /// hold all registered detect services for a special file type
        PerformanceHash         m_aFastLoaderCache          ;     /// hold all registered loader services for a special file type
        PerformanceHash         m_aFastContentHandlerCache  ;     /// hold all registered content handler services for a special file type
        PerformanceHash         m_aFastProtocolHandlerCache ;     /// hold all registered protocol handler services for a special protocol pattern
        PreferredHash           m_aPreferredTypesCache      ;     /// assignment of extensions to preferred types for this ~
        Detector                m_aDefaultDetector          ;     /// informations about our default deep detection service
        Loader                  m_aGenericLoader            ;     /// informations about our default frame loader
        ::rtl::OUString         m_sLocale                   ;     /// current set locale of configuration to handle right UIName from set of all UINames!
        sal_Bool                m_bIsModified               ;     /// Was cache modified since last flush()?
};

/*-************************************************************************************************************//**
    @short          capsulate configuration access for fiter configuration
    @descr          We use the ConfigItem mechanism to read/write values from/to configuration.
                    This implementation could be used to handle standard AND additional filter configurations in the same way.
                    We set a data container pointer for filling or reading ... this class use it temp.
                    After successfuly calling of read(), we can use filled container directly or merge it with an existing one.
                    After successfuly calling of write() all values of given data container are flushed to our configuration.

    @implements     -
    @base           ConfigItem

    @devstatus      ready to use
    @threadsafe     no
*//*-*************************************************************************************************************/
class FilterCFGAccess : public ::utl::ConfigItem
{
    //-------------------------------------------------------------------------------------------------------------
    //  interface
    //-------------------------------------------------------------------------------------------------------------
    public:
                                    FilterCFGAccess ( const ::rtl::OUString& sPath                                  ,
                                                            sal_Int32        nVersion = DEFAULT_FILTERCACHE_VERSION ,
                                                            sal_Int16        nMode    = DEFAULT_FILTERCACHE_MODE    ); // open configuration
        virtual                     ~FilterCFGAccess(                                                               );
        void                        read            (       DataContainer&   rData                                  ); // read values from configuration into given struct
        void                        write           (       DataContainer&   rData                                  ); // write values from given struct to configuration
        static   ::rtl::OUString    encodeTypeData  ( const FileType&        aType                                  ); // build own formated string of type properties
        static   void               decodeTypeData  ( const ::rtl::OUString& sData                                  ,
                                                            FileType&        aType                                  );
        static   ::rtl::OUString    encodeFilterData( const Filter&          aFilter                                ); // build own formated string of filter properties
        static   void               decodeFilterData( const ::rtl::OUString& sData                                  ,
                                                            Filter&          aFilter                                );
        static   ::rtl::OUString    encodeStringList( const StringList&      lList                                  ); // build own formated string of StringList
        static   StringList         decodeStringList( const ::rtl::OUString& sValue                                 );

    //-------------------------------------------------------------------------------------------------------------
    //  internal helper
    //-------------------------------------------------------------------------------------------------------------
    private:
        void impl_initKeyCounts        (                                            );    // set right key counts, which are used at reading/writing of set node properties
        void impl_removeNodes          (       StringList&          rChangesList    ,     // helper to remove list of set nodes
                                         const ::rtl::OUString&     sTemplateType   ,
                                         const ::rtl::OUString&     sSetName        );

        void impl_loadTypes            ( DataContainer&             rData           );    // helper to load configuration parts
        void impl_loadFilters          ( DataContainer&             rData           );
        void impl_loadDetectors        ( DataContainer&             rData           );
        void impl_loadLoaders          ( DataContainer&             rData           );
        void impl_loadContentHandlers  ( DataContainer&             rData           );
        void impl_loadProtocolHandlers ( DataContainer&             rData           );
        void impl_loadDefaults         ( DataContainer&             rData           );

        void impl_saveTypes            ( DataContainer&             rData           );    // helper to save configuration parts
        void impl_saveFilters          ( DataContainer&             rData           );
        void impl_saveDetectors        ( DataContainer&             rData           );
        void impl_saveLoaders          ( DataContainer&             rData           );
        void impl_saveContentHandlers  ( DataContainer&             rData           );
        void impl_saveProtocolHandlers ( DataContainer&             rData           );

    //-------------------------------------------------------------------------------------------------------------
    //  debug checks
    //-------------------------------------------------------------------------------------------------------------
    private:
        static sal_Bool implcp_ctor ( const ::rtl::OUString& sPath    ,     // methods to check incoming parameter on our interface methods!
                                            sal_Int32        nVersion ,
                                            sal_Int16        nMode    );
        static sal_Bool implcp_read ( const DataContainer&   rData    );
        static sal_Bool implcp_write( const DataContainer&   rData    );

    //-------------------------------------------------------------------------------------------------------------
    //  member
    //-------------------------------------------------------------------------------------------------------------
    private:
        EFilterPackage  m_ePackage                     ;   // ... not realy used yet! should split configuration in STANDARD and ADDITIONAL filter
        sal_Int32       m_nVersion                     ;   // file format version of configuration! (neccessary for "xml2xcd" transformation!)
        sal_Int32       m_nKeyCountTypes               ;   // follow key counts present count of configuration properties for types/filters ... and depends from m_nVersion - must be set right!
        sal_Int32       m_nKeyCountFilters             ;
        sal_Int32       m_nKeyCountDetectors           ;
        sal_Int32       m_nKeyCountLoaders             ;
        sal_Int32       m_nKeyCountContentHandlers     ;
        sal_Int32       m_nKeyCountProtocolHandlers    ;
};

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_CLASSES_FILTERCACHEDATA_HXX_
