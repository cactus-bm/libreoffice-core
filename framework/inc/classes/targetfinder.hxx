/*************************************************************************
 *
 *  $RCSfile: targetfinder.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: as $ $Date: 2002-05-23 12:49:46 $
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

#ifndef __FRAMEWORK_CLASSES_TARGETFINDER_HXX_
#define __FRAMEWORK_CLASSES_TARGETFINDER_HXX_

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_MACROS_DEBUG_HXX_
#include <macros/debug.hxx>
#endif

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

#ifndef __FRAMEWORK_TARGETS_H_
#include <targets.h>
#endif

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

//_________________________________________________________________________________________________________________
//  other includes
//_________________________________________________________________________________________________________________

#ifndef _RTL_USTRING_
#include <rtl/ustring.hxx>
#endif

//_________________________________________________________________________________________________________________
//  const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//  declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
    @short          We need some informations about our caller, With these enum he can specify his frame type ...
                    (Frame/Task/PlugInFrame/Desktop ...)
*//*-*************************************************************************************************************/
enum EFrameType
{
    E_UNKNOWNFRAME  ,
    E_DESKTOP       ,
    E_PLUGINFRAME   ,
    E_TASK          ,
    E_FRAME
};

/*-************************************************************************************************************//**
    @short          valid result values to classify targeting
*//*-*************************************************************************************************************/
enum ETargetClass
{
    //-------------------------------------------------------------------------------------------------------------
    // useable by classifyFindFrame() and classifyQueryDispatch()
    //-------------------------------------------------------------------------------------------------------------

    E_UNKNOWN       ,   /// occure if you call us without valid flag combinations!
    E_DEFAULT       ,   /// default handling for opening new documents
    E_CREATETASK    ,   /// create new task (supported by desktop only!)
    E_SELF          ,   /// you are the target himself
    E_PARENT        ,   /// your parent is the target
    E_BEAMER        ,   /// an existing beamer is the target (create new one if it not already exist!)
    E_TASKS         ,   /// special (but exclusiv) search for tasks only (supported at desktop only - but can combined with CREATE!)
    E_FORWARD_UP    ,   /// forward call to your parent
    E_DEEP_DOWN     ,   /// search at your children (search children of direct children before another direcht children!)
    E_FLAT_DOWN     ,   /// search at your children (search at all direct children first;  children of direcht children then!)
    E_DEEP_BOTH     ,   /// combination of E_DEEP_DOWN and E_FORWARD_UP ( search down first!)
    E_FLAT_BOTH     ,   /// combination of E_FLAT_DOWN and E_FORWARD_UP ( search down first!)

    //-------------------------------------------------------------------------------------------------------------
    // useable by classifyQueryDispatch() only
    //-------------------------------------------------------------------------------------------------------------

    E_MENUBAR       ,   /// a menu bar is supported by a task only and should be forwarded to her internal dispatch helper! (valid for classifyQueryDispatch() only!)
    E_HELPAGENT         /// same like menu bar!
};

/*-************************************************************************************************************//**
    @short          hold information about environment of frame, which use TargetFinder::classify...()
    @descr          Follow TargetFinder::classify...() methods need some informations about the environment of
                    a frame to specify search direction. Use methods of this TargetInfo to collect this informations
                    and use it on classify...().

    @implements     -
    @base           -

    @devstatus      ready to use
    @threadsafe     not neccessary
*//*-*************************************************************************************************************/
struct TargetInfo
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------
    public:
                   TargetInfo  ( const css::uno::Reference< css::frame::XFrame >& xFrame     ,
                                 const ::rtl::OUString&                           sTarget    ,
                                       sal_Int32                                  nFlags     );

                   TargetInfo  ( const ::rtl::OUString&                           sTarget    ,
                                       sal_Int32                                  nFlags     ,
                                       EFrameType                                 eType      ,
                                       sal_Bool                                   bChildrens ,
                                       sal_Bool                                   bParent    ,
                                 const ::rtl::OUString&                           sFrame     ,
                                 const ::rtl::OUString&                           sParent    );

        static EFrameType getFrameType( const css::uno::Reference< css::frame::XFrame >& xFrame     );

    //-------------------------------------------------------------------------------------------------------------
    //  private methods
    //-------------------------------------------------------------------------------------------------------------
    private:
        sal_Bool impl_getCreateFlag( sal_Int32 nSearchFlags );

    //-------------------------------------------------------------------------------------------------------------
    //  debug and test methods
    //-------------------------------------------------------------------------------------------------------------
    #ifdef ENABLE_ASSERTIONS
    private:
        static sal_Bool implcp_ctor        ( const css::uno::Reference< css::frame::XFrame >& xFrame     ,
                                             const ::rtl::OUString&                           sTarget    ,
                                                   sal_Int32                                  nFlags     );

        static sal_Bool implcp_ctor        ( const ::rtl::OUString&                           sTarget    ,
                                                   sal_Int32                                  nFlags     ,
                                                   EFrameType                                 eType      ,
                                                   sal_Bool                                   bChildrens ,
                                                   sal_Bool                                   bParent    ,
                                             const ::rtl::OUString&                           sFrame     ,
                                             const ::rtl::OUString&                           sParent    );

        static sal_Bool implcp_getFrameType( const css::uno::Reference< css::frame::XFrame >& xFrame     );
    #endif

    //-------------------------------------------------------------------------------------------------------------
    //  public variables!
    //  faster access for TargetFinder::classify...()
    //-------------------------------------------------------------------------------------------------------------
    public:
        EFrameType         eFrameType        ;  /// your node type (desktop, task ,frame) Its neccessary to select right search algorithm.
        ::rtl::OUString    sTargetName       ;  /// is the search parameter to find right frame by name or special value!
        sal_Int32          nSearchFlags      ;  /// is an optional parameter to regulate search direction if no special target name was given.
        sal_Bool           bChildrenExist    ;  /// Say us - if some children exist. Otherwise down search is ignored!
        ::rtl::OUString    sFrameName        ;  /// If SELF flag is set we can break search earlier if this name is the target!
        sal_Bool           bParentExist      ;  /// Say us - if a parent exist. Otherwise upper search is ignored!
        ::rtl::OUString    sParentName       ;  /// If PARENT flag is set we can break search earlier if this name is the target!
        sal_Bool           bCreationAllowed  ;  /// we set it TRUE if flag CREATE is set. You must search for given target, but could create a new tree node if search will fail!

};  // struct TargetInfo

/*-************************************************************************************************************//**
    @short          implement helper to implement code for targeting only one time!
    @descr          We need checking of a target name in combination with given search flags at differnt places.
                    These helper analyze the parameter and recommend the direction for searching.
                    We can use a method to classify search direction and another one to change our search parameter
                    to make it right. You can use these - but you don't must do it!

    @implements     -
    @base           -

    @devstatus      ready to use
    @threadsafe     not neccessary
*//*-*************************************************************************************************************/
class TargetFinder
{
    //-------------------------------------------------------------------------------------------------------------
    //  public methods
    //-------------------------------------------------------------------------------------------------------------
    public:
        static ETargetClass classifyFindFrame    ( TargetInfo& aInfo );
        static ETargetClass classifyQueryDispatch( TargetInfo& aInfo );

    //-------------------------------------------------------------------------------------------------------------
    //  private methods
    //-------------------------------------------------------------------------------------------------------------
    private:
        static ETargetClass impl_classifyForDesktop_findFrame         (         sal_Bool            bParentExist        ,
                                                                        const   ::rtl::OUString&    sTargetName         ,
                                                                                sal_Int32           nSearchFlags        );

        static ETargetClass impl_classifyForPlugInFrame_findFrame     (         sal_Bool            bParentExist        ,
                                                                                sal_Bool            bChildrenExist      ,
                                                                        const   ::rtl::OUString&    sFrameName          ,
                                                                        const   ::rtl::OUString&    sTargetName         ,
                                                                                sal_Int32           nSearchFlags        );

        static ETargetClass impl_classifyForTask_findFrame            (         sal_Bool            bParentExist        ,
                                                                                sal_Bool            bChildrenExist      ,
                                                                        const   ::rtl::OUString&    sFrameName          ,
                                                                        const   ::rtl::OUString&    sTargetName         ,
                                                                                sal_Int32           nSearchFlags        );

        static ETargetClass impl_classifyForFrame_findFrame           (         sal_Bool            bParentExist        ,
                                                                                sal_Bool            bChildrenExist      ,
                                                                        const   ::rtl::OUString&    sFrameName          ,
                                                                        const   ::rtl::OUString&    sParentName         ,
                                                                        const   ::rtl::OUString&    sTargetName         ,
                                                                                sal_Int32           nSearchFlags        );

    //-------------------------------------------------------------------------------------------------------------
    //  debug and test methods
    //-------------------------------------------------------------------------------------------------------------
    #ifdef ENABLE_ASSERTIONS
    private:
        static sal_Bool implcp_classifyFindFrame    ( const TargetInfo& aInfo );
        static sal_Bool implcp_classifyQueryDispatch( const TargetInfo& aInfo );
    #endif

};      //  class TargetFinder

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_CLASSES_TARGETFINDER_HXX_
