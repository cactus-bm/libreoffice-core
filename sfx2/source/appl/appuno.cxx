/*************************************************************************
 *
 *  $RCSfile: appuno.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: mba $ $Date: 2001-04-02 08:57:28 $
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

#include "appuno.hxx"

#ifndef _SFXRECTITEM_HXX //autogen
#include <svtools/rectitem.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _WLDCRD_HXX //autogen
#include <tools/wldcrd.hxx>
#endif
#ifndef _SV_CONFIG_HXX //autogen
#include <vcl/config.hxx>
#endif
#ifndef _INET_CONFIG_HXX //autogen
#include <inet/inetcfg.hxx>
#endif
#ifndef __SBX_SBXMETHOD_HXX
#include <svtools/sbxmeth.hxx>
#endif
#ifndef _SB_SBMETH_HXX
#include <basic/sbmeth.hxx>
#endif
#ifndef _SBX_SBXOBJECT_HXX
#include <svtools/sbxobj.hxx>
#endif
#ifndef _SB_SBERRORS_HXX
#include <basic/sberrors.hxx>
#endif
#ifndef _BASMGR_HXX
#include <basic/basmgr.hxx>
#endif

#include <svtools/ownlist.hxx>
#include <svtools/lckbitem.hxx>
#include <svtools/stritem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/eitem.hxx>

#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMEACTIONLISTENER_HPP_
#include <com/sun/star/frame/XFrameActionListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMEACTIONEVENT_HPP_
#include <com/sun/star/frame/FrameActionEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMEACTION_HPP_
#include <com/sun/star/frame/FrameAction.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINER_HPP_
#include <com/sun/star/container/XContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEREPLACE_HPP_
#include <com/sun/star/container/XNameReplace.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERLISTENER_HPP_
#include <com/sun/star/container/XContainerListener.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XSET_HPP_
#include <com/sun/star/container/XSet.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_CONTAINEREVENT_HPP_
#include <com/sun/star/container/ContainerEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTOPWINDOW_HPP_
#include <com/sun/star/awt/XTopWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_REGISTRY_REGISTRYVALUETYPE_HPP_
#include <com/sun/star/registry/RegistryValueType.hpp>
#endif
#include <comphelper/processfactory.hxx>
#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XBUTTON_HPP_
#include <com/sun/star/awt/XButton.hpp>
#endif

#include <tools/cachestr.hxx>
#include <osl/mutex.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::registry;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::io;
using namespace ::rtl;

#pragma hdrstop

#include "sfxtypes.hxx"
#include "sfxuno.hxx"
#include "appdata.hxx"
#include "app.hxx"
#include "sfxsids.hrc"
#include "msg.hxx"
#include "msgpool.hxx"
#include "request.hxx"
#include "module.hxx"
#include "downloadcontroller.hxx"
#include "ucbhelp.hxx"
#include "fcontnr.hxx"
#include "frmload.hxx"
#include "frame.hxx"
#include "sfxbasic.hxx"
#include "objsh.hxx"
#include "objuno.hxx"
#include "filepicker.hxx"
#include "unoctitm.hxx"
#include "dispatch.hxx"
#include "doctemplates.hxx"

#define FRAMELOADER_SERVICENAME     "com.sun.star.frame.FrameLoader"

void TransformParameters( sal_uInt16 nSlotId, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rArgs, SfxAllItemSet& rSet, const SfxSlot* pSlot )
{
    if ( !pSlot )
        pSlot = SFX_SLOTPOOL().GetSlot( nSlotId );

    if ( nSlotId == SID_OPENURL )
        nSlotId = SID_OPENDOC;

    sal_Int32 nCount = rArgs.getLength();
    const ::com::sun::star::beans::PropertyValue* pPropsVal = rArgs.getConstArray();
    for ( sal_uInt16 n=0; n<nCount; n++ )
    {
        const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
        String aName = rProp.Name ;

        sal_uInt16 nArgs;
        for ( nArgs=0; nArgs<pSlot->nArgDefCount; nArgs++ )
        {
            const SfxFormalArgument* pArg = pSlot->pFirstArgDef + nArgs;
            if ( aName.CompareToAscii(pArg->pName) == COMPARE_EQUAL )
            {
                SfxPoolItem* pItem = pArg->CreateItem();
                pItem->SetWhich( pArg->nSlotId );
                if ( pItem->PutValue( rProp.Value ) )
                    rSet.Put( *pItem );
                delete pItem;
                break;
            }
        }

        if ( nArgs >= pSlot->nArgDefCount && nSlotId == SID_OPENDOC )
        {
            static const String sTemplateRegionName   = String::CreateFromAscii( "TemplateRegionName"   );
            static const String sTemplateName   = String::CreateFromAscii( "TemplateName"   );
            static const String sAsTemplate     = String::CreateFromAscii( "AsTemplate"     );
            static const String sOpenNewView    = String::CreateFromAscii( "OpenNewView"    );
            static const String sViewId         = String::CreateFromAscii( "ViewId"         );
            static const String sPluginMode     = String::CreateFromAscii( "PluginMode"     );
            static const String sReadOnly       = String::CreateFromAscii( "ReadOnly"       );
            static const String sPostString     = String::CreateFromAscii( "PostString"     );
            static const String sFrameName      = String::CreateFromAscii( "FrameName"      );
            static const String sContentType    = String::CreateFromAscii( "ContentType"    );
            static const String sPostData       = String::CreateFromAscii( "PostData"       );
            static const String sPosSize        = String::CreateFromAscii( "PosSize"        );
            static const String sCharacterSet   = String::CreateFromAscii( "CharacterSet"   );
            static const String sInputStream    = String::CreateFromAscii( "InputStream"    );
            static const String sHidden         = String::CreateFromAscii( "Hidden"         );
            static const String sPreview        = String::CreateFromAscii( "Preview"        );
            static const String sSilent         = String::CreateFromAscii( "Silent"         );
            static const String sJumpMark       = String::CreateFromAscii( "JumpMark"       );
            static const String sURL            = String::CreateFromAscii( "URL"            );

            if ( aName == sInputStream && rProp.Value.getValueType() == ::getCppuType( (Reference < XInputStream >*)0 ) )
                rSet.Put( SfxUsrAnyItem( SID_INPUTSTREAM, rProp.Value ) );

            // AsTemplate-Property?
            if ( aName == sAsTemplate && rProp.Value.getValueType() == ::getBooleanCppuType() )
                rSet.Put( SfxBoolItem( SID_TEMPLATE, *((sal_Bool*)rProp.Value.getValue()) ) );

            // OpenNewView-Parameter ?
            else if ( aName == sOpenNewView && rProp.Value.getValueType() == ::getBooleanCppuType() )

                rSet.Put( SfxBoolItem( SID_OPEN_NEW_VIEW, *((sal_Bool*)rProp.Value.getValue()) ) );

            // ViewId-Parameter ?
            else if ( aName == sViewId && rProp.Value.getValueType() == ::getCppuType((const sal_Int16*)0) )
                rSet.Put( SfxUInt16Item( SID_VIEW_ID, *((sal_Int16*)rProp.Value.getValue()) ) );

            // PluginMode-Parameter ?
            else if ( aName == sPluginMode && rProp.Value.getValueType() == ::getCppuType((const sal_Int16*)0) )
                rSet.Put( SfxUInt16Item( SID_PLUGIN_MODE, *((sal_Int16*)rProp.Value.getValue()) ) );

            // ReadOnly-Property?
            else if ( aName == sReadOnly && rProp.Value.getValueType() == ::getBooleanCppuType() )
                rSet.Put( SfxBoolItem( SID_DOC_READONLY, *((sal_Bool*)rProp.Value.getValue()) ) );

            else if ( aName == sHidden && rProp.Value.getValueType() == ::getBooleanCppuType() )
                rSet.Put( SfxBoolItem( SID_HIDDEN, *((sal_Bool*)rProp.Value.getValue()) ) );

             else if ( aName == sSilent && rProp.Value.getValueType() == ::getBooleanCppuType() )
                rSet.Put( SfxBoolItem( SID_SILENT, *((sal_Bool*)rProp.Value.getValue()) ) );

            else if ( aName == sPreview && rProp.Value.getValueType() == ::getBooleanCppuType() )
                rSet.Put( SfxBoolItem( SID_PREVIEW, *((sal_Bool*)rProp.Value.getValue()) ) );

            // PostString-Property?
            else if ( aName == sPostString && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_POSTSTRING, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sURL && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_OPENURL, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sFrameName && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_TARGETNAME, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sContentType && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_CONTENTTYPE, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sTemplateName && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_TEMPLATE_NAME, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sTemplateRegionName && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_TEMPLATE_REGIONNAME, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sJumpMark && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_JUMPMARK, *((::rtl::OUString*)rProp.Value.getValue()) ) );

            else if ( aName == sPostData && rProp.Value.getValueType() == ::getCppuType((const ::com::sun::star::uno::Sequence<sal_Int8>*)0) )
            {
                SvCacheStream* pStream = new SvCacheStream;
                ::com::sun::star::uno::Sequence<sal_Int8> aSequ = *((::com::sun::star::uno::Sequence<sal_Int8>*) rProp.Value.getValue());
                pStream->Write( (void*) aSequ.getConstArray(), aSequ.getLength() );
                SfxRefItem aItem( SID_POSTLOCKBYTES, new SvLockBytes( pStream, sal_True ) );
                rSet.Put( aItem );
            }

            // PosSize-Property?
            else if ( aName == sPosSize && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
            {
                String aPar = *((::rtl::OUString*)rProp.Value.getValue());
                Size aSize;
                Point aPos;
                DBG_ASSERT( sal_False, "TransformParameters()\nProperty \"PosSize\" isn't supported yet!\n" );
            }

            // CharacterSet-Property?
            else if ( aName == sCharacterSet && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                rSet.Put( SfxStringItem( SID_CHARSET, *((::rtl::OUString*)rProp.Value.getValue()) ) );
        }
    }
}

void TransformItems( sal_uInt16 nSlotId, const SfxItemSet& rSet, ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rArgs, const SfxSlot* pSlot )
{
    if ( !pSlot )
        pSlot = SFX_SLOTPOOL().GetSlot( nSlotId );

    if ( nSlotId == SID_OPENURL )
        nSlotId = SID_OPENDOC;

    sal_uInt16 nArgs;
    sal_uInt16 nItems=0;
    for ( nArgs=0; nArgs<pSlot->nArgDefCount; nArgs++ )
    {
        const SfxFormalArgument* pArg = pSlot->pFirstArgDef + nArgs;
        if ( rSet.GetItemState( pArg->nSlotId ) == SFX_ITEM_SET )
            nItems++;
    }

    if ( nSlotId == SID_OPENDOC )
    {
        if ( rSet.GetItemState( SID_OPENURL ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_INPUTSTREAM ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_TEMPLATE ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_OPEN_NEW_VIEW ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_VIEW_ID ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_PLUGIN_MODE ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_DOC_READONLY ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_POSTSTRING ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_CONTENTTYPE ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_VIEW_POS_SIZE ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_POSTLOCKBYTES ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_CHARSET ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_TARGETNAME ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_TEMPLATE_NAME ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_TEMPLATE_REGIONNAME ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_HIDDEN ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_PREVIEW ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_SILENT ) == SFX_ITEM_SET )
            nItems++;
        if ( rSet.GetItemState( SID_JUMPMARK ) == SFX_ITEM_SET )
            nItems++;
    }

    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue> aSequ( nItems );
    ::com::sun::star::beans::PropertyValue *pValue = aSequ.getArray();
    for ( nArgs=0, nItems=0; nArgs<pSlot->nArgDefCount; nArgs++ )
    {
        const SfxFormalArgument* pArg = pSlot->pFirstArgDef + nArgs;
        SFX_ITEMSET_ARG( &rSet, pItem, SfxPoolItem, pArg->nSlotId, sal_False );
        if ( pItem )
        {
            pValue[nItems].Name = String( String::CreateFromAscii( pArg->pName ) ) ;
            pItem->QueryValue( pValue[nItems++].Value );
        }
    }

    if ( nSlotId == SID_OPENDOC )
    {
        static const String sTemplateRegionName   = String::CreateFromAscii( "TemplateRegionName"   );
        static const String sTemplateName   = String::CreateFromAscii( "TemplateName"   );
        static const String sAsTemplate     = String::CreateFromAscii( "AsTemplate"     );
        static const String sOpenNewView    = String::CreateFromAscii( "OpenNewView"    );
        static const String sViewId         = String::CreateFromAscii( "ViewId"         );
        static const String sPluginMode     = String::CreateFromAscii( "PluginMode"     );
        static const String sReadOnly       = String::CreateFromAscii( "ReadOnly"       );
        static const String sPostString     = String::CreateFromAscii( "PostString"     );
        static const String sFrameName      = String::CreateFromAscii( "FrameName"      );
        static const String sContentType    = String::CreateFromAscii( "ContentType"    );
        static const String sPostData       = String::CreateFromAscii( "PostData"       );
        static const String sPosSize        = String::CreateFromAscii( "PosSize"        );
        static const String sCharacterSet   = String::CreateFromAscii( "CharacterSet"   );
        static const String sInputStream    = String::CreateFromAscii( "InputStream"    );
        static const String sHidden         = String::CreateFromAscii( "Hidden"         );
        static const String sPreview        = String::CreateFromAscii( "Preview"        );
        static const String sSilent         = String::CreateFromAscii( "Silent"         );
        static const String sJumpMark       = String::CreateFromAscii( "JumpMark"       );
        static const String sURL            = String::CreateFromAscii( "URL"            );

        const SfxPoolItem *pItem=0;
        if ( rSet.GetItemState( SID_INPUTSTREAM, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sInputStream;
            pValue[nItems++].Value = ( ((SfxUsrAnyItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_TEMPLATE, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sAsTemplate;
            pValue[nItems++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_OPEN_NEW_VIEW, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sOpenNewView;
            pValue[nItems++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_VIEW_ID, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sViewId;
            pValue[nItems++].Value <<= ( (sal_uInt16) ((SfxUInt16Item*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_PLUGIN_MODE, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sPluginMode;
            pValue[nItems++].Value <<= ( (sal_uInt16) ((SfxUInt16Item*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_DOC_READONLY, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sReadOnly;
            pValue[nItems++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_HIDDEN, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sHidden;
            pValue[nItems++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_SILENT, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sSilent;
            pValue[nItems++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_PREVIEW, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sPreview;
            pValue[nItems++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
        }
        if ( rSet.GetItemState( SID_POSTSTRING, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sPostString;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue()) );
        }
        if ( rSet.GetItemState( SID_TARGETNAME, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sFrameName;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue()) );
        }
        if ( rSet.GetItemState( SID_CONTENTTYPE, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sContentType;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
        }
        if ( rSet.GetItemState( SID_TEMPLATE_NAME, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sTemplateName;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
        }
        if ( rSet.GetItemState( SID_TEMPLATE_REGIONNAME, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sTemplateRegionName;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
        }
        if ( rSet.GetItemState( SID_JUMPMARK, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sJumpMark;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
        }
        if ( rSet.GetItemState( SID_OPENURL, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sURL;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
        }

        SFX_ITEMSET_ARG( &rSet, pRectItem, SfxRectangleItem, SID_VIEW_POS_SIZE, sal_False );
        if ( pRectItem )
        {
            pValue[nItems].Name = sPosSize;
            Rectangle aRect = pRectItem->GetValue();
            DBG_ASSERT(sal_False, "TransformItems()\nSfxIniManager::GetString used to set property \"PosSize\" ...!\n");
        }

        SFX_ITEMSET_ARG( &rSet, pRefItem, SfxRefItem, SID_POSTLOCKBYTES, sal_False );
        if ( pRefItem )
        {
            // Mit TLX-Spezialhack fuer die Pointercasts
            pValue[nItems].Name = sPostData;
            SvLockBytes* pBytes = new SvLockBytes;
            int nDiff = (char*)pBytes - (char*)(SvRefBase*)pBytes;
            SvLockBytes* pLB = (SvLockBytes*)(
                    (char*)(SvRefBase*)&((SfxRefItem*)pRefItem )->GetValue() + nDiff );
            delete pBytes;
            SfxLockBytesItem aLock( SID_POSTLOCKBYTES, pLB );
            aLock.QueryValue( pValue[nItems++].Value );
        }

        if ( rSet.GetItemState( SID_CHARSET, sal_False, &pItem ) == SFX_ITEM_SET )
        {
            pValue[nItems].Name = sCharacterSet;
            pValue[nItems++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
        }
    }

    rArgs = aSequ;
}

SV_IMPL_PTRARR( SfxComponentKeyArr_Impl, SfxComponentKeyPtr_Impl );


#if 0 // (mba)
#ifdef SOLAR_JAVA
SFX_IMPL_XINTERFACE_1( SfxJavaLoader, OWeakObject, ::com::sun::star::frame::XFrameLoader )
SFX_IMPL_XTYPEPROVIDER_1( SfxJavaLoader, ::com::sun::star::frame::XFrameLoader )
SFX_IMPL_XSERVICEINFO( SfxJavaLoader, FRAMELOADER_SERVICENAME, "com.sun.star.comp.sfx2.JavaLoader" )
SFX_IMPL_SINGLEFACTORY( SfxJavaLoader )

void SAL_CALL SfxJavaLoader::load(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > & rFrame, const ::rtl::OUString& rURL,
                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rArgs,
                const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLoadEventListener > & rListener) throw ( ::com::sun::star::uno::RuntimeException )
{
    xFrame = rFrame;
    xListener = rListener;
    aURL.Complete = rURL;
    aArgs = rArgs;

    SfxURLTransformer aTrans ( ::comphelper::getProcessServiceFactory() );
    aTrans.parseStrict( aURL );

    String aName( aURL.Path );
    aName.Erase( 0,5 ); // java/

    SvCommandList aList;
    String aParam( aURL.Arguments );
    aParam += ' ';
    if ( aParam.Len() )
    {
        const char* pStr = aParam.GetBuffer();
        const char* pStart = pStr;
        sal_Bool bEscaped = sal_False, bSeparated = sal_True;

        while ( *pStr )
        {
            if ( *pStr != ' ' || bEscaped )
            {
                if ( *pStr == '"' )
                {
                    if ( *(pStr+1) != '"' )
                        bEscaped = ! bEscaped;
                    else
                        pStr++;
                }

                bSeparated = sal_False;
            }
            else
            {
                if ( !bSeparated )
                {
                    String aPar( pStart, pStr-pStart );
                    pStart = pStr+1;
                    bSeparated = sal_True;

                    if ( aPar.GetTokenCount( '=' ) > 1 )
                    {
                        String aName = aPar.GetToken( 0, '=' );
                        aPar.Erase( 0, aName.Len() + 1 );
                        aList.Append( aName, aPar );
                    }
                    else
                        aList.Append( aPar, "" );
                }
            }

            pStr++;
        }
    }

    pApp = new JavaApplication;
    pApp->Init(aName, aList, this);
//      pApp = new JavaApplication( aName, aList, this );
}

// -----------------------------------------------------------------------

void SAL_CALL SfxJavaLoader::cancel(void) throw ( ::com::sun::star::uno::RuntimeException )
{
    if ( pApp )
        pApp->cancel();
    xListener = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLoadEventListener > ();
    xFrame = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > ();
}

// -----------------------------------------------------------------------

void SfxJavaLoader::status( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  & xObject, short nObjectType )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >  xComp;

    if ( ( nObjectType == OBJECTTYPE_AWTCOMPONENT || nObjectType == OBJECTTYPE_AWTAPPLET ) && xObject.is() )
        xComp = ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > ( xObject, ::com::sun::star::uno::UNO_QUERY );

    if ( xComp.is() )
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >  xCtrl( xComp, ::com::sun::star::uno::UNO_QUERY );
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >  xPeer( xFrame->getContainerWindow(), ::com::sun::star::uno::UNO_QUERY );
        xCtrl->createPeer( Application::GetVCLToolkit(), xPeer );
        xComp->setPosSize( 0, 0, 100, 100, PosSize_POSSIZE );
        xFrame->setComponent( xComp, ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController > () );
    }

    LoadFinished( xComp.is() );
}

void SfxJavaLoader::abort( const String & rReason )
{
    DELETEZ( pApp );
    LoadFinished( sal_False );
}

void SfxJavaLoader::LoadFinished( sal_Bool bOK )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrameLoader >  xRef( this );
    DELETEZ( pApp );

    if ( bOK )
        xListener->loadFinished( this );
    else
        xListener->loadCancelled( this );
}

#endif
#endif // (mba)

SFX_IMPL_XINTERFACE_1( DownloaderLoader, OWeakObject, ::com::sun::star::frame::XFrameLoader )
SFX_IMPL_XTYPEPROVIDER_1( DownloaderLoader, ::com::sun::star::frame::XFrameLoader )
SFX_IMPL_XSERVICEINFO( DownloaderLoader, FRAMELOADER_SERVICENAME, "com.sun.star.comp.sfx2.DownloaderLoader" )
SFX_IMPL_SINGLEFACTORY( DownloaderLoader )

// -----------------------------------------------------------------------
void SAL_CALL DownloaderLoader::load (   const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > &                    rFrame      ,
                                const ::rtl::OUString& rURL,
                                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >&    rArgs       ,
                                const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLoadEventListener > &        rListener   ) throw ( ::com::sun::star::uno::RuntimeException )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >   aRef    =   rFrame->getContainerWindow () ;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >  xMgr( ::comphelper::getProcessServiceFactory() ) ;

    DownloadController* pController = new DownloadController(xMgr) ;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >       xController     = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >  ( pController ) ;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >          xControl        ( xMgr->createInstance (DEFINE_CONST_UNICODE("com.sun.star.awt.XProgressMonitor")    ), ::com::sun::star::uno::UNO_QUERY ) ;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >           xWindow         ( xControl                                                       , ::com::sun::star::uno::UNO_QUERY ) ;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >       xPeer           ( aRef                                                           , ::com::sun::star::uno::UNO_QUERY ) ;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XButton >           xButton         ( xControl                                                       , ::com::sun::star::uno::UNO_QUERY ) ;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >   xActionListener ( xController                                                    , ::com::sun::star::uno::UNO_QUERY ) ;

    DBG_ASSERT ( xControl.is()       , "DownloaderLoader::load(..)\nXProgressMonitor has no valid interface ::com::sun::star::awt::XControl!\n"                ) ;
    DBG_ASSERT ( xWindow.is()        , "DownloaderLoader::load(..)\nXProgressMonitor has no valid interface ::com::sun::star::awt::XWindow!\n"                 ) ;
    DBG_ASSERT ( xPeer.is()          , "DownloaderLoader::load(..)\nXFrame->getContainerWindow() has no valid interface ::com::sun::star::awt::XWindowPeer!\n" ) ;
    DBG_ASSERT ( xButton.is()        , "DownloaderLoader::load(..)\nXProgressMonitor has no valid interface ::com::sun::star::awt::XButton!\n"                 ) ;
    DBG_ASSERT ( xActionListener.is(), "DownloaderLoader::load(..)\nDownloadController has no valid interface ::com::sun::star::awt::XActionListener!\n"       ) ;

    // dont't forget to create peer BEFORE call "setComponent" at FRAME !!!
    // (else frame calls this method without valid toolkit and parent!!! => CRASH )
    xControl->createPeer        ( xPeer->getToolkit(), xPeer    ) ;

    // try to start download
    String aTempURL = rURL ;
    sal_Bool bOK = pController->StartDownload ( aTempURL ) ;
    if ( bOK)
    {
        rFrame->setComponent        ( xWindow, xController                  );
        xController->attachFrame    ( rFrame                                );
        xButton->addActionListener  ( xActionListener                       );
        xButton->setActionCommand   ( DEFINE_CONST_UNICODE("BreakDownload") );

        if ( rListener.is() )
            rListener->loadFinished (this) ;
    }
    else if (rListener.is() )
        // no download - no download monitor !
        rListener->loadCancelled( this ) ;
}

// -----------------------------------------------------------------------
DownloaderLoader::DownloaderLoader( com::sun::star::uno::Reference < class com::sun::star::lang::XMultiServiceFactory > const &)
{
}

// -----------------------------------------------------------------------
DownloaderLoader::~DownloaderLoader()
{
}

// -----------------------------------------------------------------------

void SAL_CALL DownloaderLoader::cancel(void) throw ( ::com::sun::star::uno::RuntimeException )
{
}

SFX_IMPL_XINTERFACE_1( SfxMacroLoader, OWeakObject, ::com::sun::star::frame::XFrameLoader )
SFX_IMPL_XTYPEPROVIDER_1( SfxMacroLoader, ::com::sun::star::frame::XFrameLoader )
SFX_IMPL_XSERVICEINFO( SfxMacroLoader, FRAMELOADER_SERVICENAME, "com.sun.star.comp.sfx2.SfxMacroLoader" )
SFX_IMPL_SINGLEFACTORY( SfxMacroLoader )

// -----------------------------------------------------------------------
void SAL_CALL SfxMacroLoader::load (const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > &rFrame,
                                    const ::rtl::OUString& rURL,
                                    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > &rArgs,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLoadEventListener > &rListener )
    throw ( ::com::sun::star::uno::RuntimeException )
{
    ErrCode nErr = loadMacro( rURL );

    if ( rListener.is() )
    {
        // always call loadCancelled, because we didn't load a document but
        // executed a macro instead!
        rListener->loadCancelled( this ) ;
    }
}

// -----------------------------------------------------------------------
SfxMacroLoader::SfxMacroLoader( com::sun::star::uno::Reference < class com::sun::star::lang::XMultiServiceFactory > const &)
{
}

// -----------------------------------------------------------------------
SfxMacroLoader::~SfxMacroLoader()
{
}

// -----------------------------------------------------------------------

void SAL_CALL SfxMacroLoader::cancel(void) throw ( ::com::sun::star::uno::RuntimeException )
{
}


// -----------------------------------------------------------------------
ErrCode SfxMacroLoader::loadMacro( const ::rtl::OUString& rURL )
    throw ( ::com::sun::star::uno::RuntimeException )
{
    SfxApplication* pApp = SFX_APP();

    pApp->EnterBasicCall();

    // macro:-::com::sun::star::util::URL analysiern
    // 'macro:///lib.mod.proc(args)' => Macro via App-BASIC-Mgr
    // 'macro://[docname|.]/lib.mod.proc(args)' => Macro via zugehoerigen Doc-BASIC-Mgr
    // 'macro://obj.method(args)' => Object via App-BASIC-Mgr
    String aMacro( rURL );
    sal_uInt16 nHashPos = aMacro.Search( '/', 8 );
    sal_uInt16 nArgsPos = aMacro.Search( '(' );
    BasicManager *pBasMgr = 0;
    ErrCode nErr = ERRCODE_NONE;

    // wird Macro angesprochen (also KEIN Object)?
    if ( STRING_NOTFOUND != nHashPos && nHashPos < nArgsPos )
    {
        // BasManager ermitteln
        String aBasMgrName( INetURLObject::decode(aMacro.Copy( 8, nHashPos-8 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET) );
        if ( !aBasMgrName.Len() )
            pBasMgr = pApp->GetBasicManager();
        else if ( aBasMgrName.EqualsAscii(".") )
            pBasMgr = SfxObjectShell::Current()->GetBasicManager();
        else
            for ( SfxObjectShell *pObjSh = SfxObjectShell::GetFirst();
                    pObjSh && !pBasMgr;
                    pObjSh = SfxObjectShell::GetNext(*pObjSh) )
                if ( aBasMgrName == pObjSh->GetTitle(SFX_TITLE_APINAME) )
                    pBasMgr = pObjSh->GetBasicManager();
        if ( pBasMgr )
        {
            // Funktion suchen
            String aQualifiedMethod( INetURLObject::decode(aMacro.Copy( nHashPos+1 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET) );
            String aArgs;
            if ( STRING_NOTFOUND != nArgsPos )
            {
                aArgs = aQualifiedMethod.Copy( nArgsPos - nHashPos - 1 );
                aQualifiedMethod.Erase( nArgsPos - nHashPos - 1 );
            }

            SbxMethod *pMethod = SfxQueryMacro( pBasMgr, aQualifiedMethod );

            // falls gefunden Funktion ueber ihren Parent ausfuehren
            if ( pMethod )
            {
                String aQuotedArgs;
                if ( aArgs.Len()<2 || aArgs.GetBuffer()[1] == '\"')
                    aQuotedArgs = aArgs;
                else
                {
                    // Klammern entfernen
                    aArgs.Erase(0,1);
                    aArgs.Erase( aArgs.Len()-1,1);

                    aQuotedArgs = '(';

                    // Alle Parameter mit T"uddelchen
                    sal_uInt16 nCount = aArgs.GetTokenCount(',');
                    for ( sal_uInt16 n=0; n<nCount; n++ )
                    {
                        aQuotedArgs += '\"';
                        aQuotedArgs += aArgs.GetToken( n, ',' );
                        aQuotedArgs += '\"';
                        if ( n<nCount-1 )
                            aQuotedArgs += ',';
                    }

                    aQuotedArgs += ')';
                }

                String aCall( '[' );
                aCall += pMethod->GetName();
                aCall += aQuotedArgs;
                aCall += ']';
                pMethod->GetParent()->Execute( aCall );
                nErr = SbxBase::GetError();
            }
            else
                nErr = ERRCODE_BASIC_PROC_UNDEFINED;
        }
        else
            nErr = ERRCODE_IO_NOTEXISTS;
    }
    else
    {
        // (optional Objekt-qualifizierte) Basic-Funktion ausfuehren
        String aCall( '[' );
        aCall += INetURLObject::decode(aMacro.Copy(6), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET);
        aCall += ']';
        pApp->GetBasicManager()->GetLib(0)->Execute( aCall );
        nErr = SbxBase::GetError();
    }

    pApp->LeaveBasicCall();
    SbxBase::ResetError();

    return nErr;
}

SFX_IMPL_XSERVICEINFO( SfxAppDispatchProvider, "com.sun.star.frame.DispatchProvider", "com.sun.star.comp.sfx2.AppDispatchProvider" )                                                                \
SFX_IMPL_ONEINSTANCEFACTORY( SfxAppDispatchProvider );

Reference < XDispatch > SAL_CALL SfxAppDispatchProvider::queryDispatch( const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& sTargetFrameName,
                    FrameSearchFlags eSearchFlags ) throw( RuntimeException )
{
    USHORT nId = 0;
    Reference < XDispatch > xDisp;
    if ( aURL.Protocol.compareToAscii( "slot:" ) == COMPARE_EQUAL ||
         aURL.Protocol.compareToAscii( "commandId:" ) == COMPARE_EQUAL )
    {
        nId = (USHORT) aURL.Path.toInt32();
    }

    if ( nId && SFX_APP()->GetAppDispatcher_Impl()->HasSlot_Impl( nId ) )
        xDisp = new SfxOfficeDispatch( SFX_APP()->GetAppDispatcher_Impl(), nId, aURL ) ;

    return xDisp;
}

Sequence< Reference < XDispatch > > SAL_CALL SfxAppDispatchProvider::queryDispatches( const Sequence < DispatchDescriptor >& seqDescriptor )
                        throw( RuntimeException )
{
    return Sequence< Reference < XDispatch > >();
}

// -----------------------------------------------------------------------

#define IMPLEMENTATION_NAME "com.sun.comp.jsimport.IchitaroImportFilter"
#define SERVICE_NAME        "com.sun.star.document.ImportFilter"

extern "C" {

void SAL_CALL component_getImplementationEnvironment(   const   sal_Char**          ppEnvironmentTypeName   ,
                                                                uno_Environment**   ppEnvironment           )
{
    *ppEnvironmentTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME ;
}

sal_Bool SAL_CALL component_writeInfo(  void*   pServiceManager ,
                                        void*   pRegistryKey    )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::registry::XRegistryKey >        xKey( reinterpret_cast< ::com::sun::star::registry::XRegistryKey* >( pRegistryKey ) )   ;

    // Eigentliche Implementierung und ihre Services registrieren
    ::rtl::OUString aImpl;
    ::rtl::OUString aTempStr;
    ::rtl::OUString aKeyStr;
    Reference< XRegistryKey > xNewKey;
    Reference< XRegistryKey > xLoaderKey;

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxAppDispatchProvider::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.frame.DispatchProvider") );

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxStandaloneDocumentInfoObject::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.document.StandaloneDocumentInfo") );

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += DownloaderLoader::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.frame.FrameLoader") );

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxFrameLoader_Impl::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    Sequence < ::rtl::OUString > aServices = SfxFrameLoader_Impl::impl_getStaticSupportedServiceNames();
    sal_Int32 nCount = aServices.getLength();
    for ( sal_Int16 i=0; i<nCount; i++ )
        xNewKey->createKey( aServices.getConstArray()[i] );

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/Loader"));
    xNewKey = xKey->createKey( aTempStr );

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/Loader"));
    xLoaderKey = xKey->createKey( aTempStr );
    xNewKey = xLoaderKey->createKey( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Pattern")) );
    xNewKey->setAsciiValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("private:factory/*" )) );

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxMacroLoader::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.frame.FrameLoader") );

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/Loader"));
    xNewKey = xKey->createKey( aTempStr );

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/Loader"));
    xLoaderKey = xKey->createKey( aTempStr );
    xNewKey = xLoaderKey->createKey( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Pattern")) );
    xNewKey->setAsciiValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("macro:*" )) );

    // - sfx document templates
    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxDocTplService::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.frame.DocumentTemplates") );

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxFilePicker::impl_getStaticImplementationNameOpen();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.ui.FileOpen") );

    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxFilePicker::impl_getStaticImplementationNameSave();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.ui.FileSave") );
#if 0
    if (pRegistryKey)
    {
            try
            {
                Reference< XRegistryKey > xKey(
                    reinterpret_cast< XRegistryKey * >( pRegistryKey ) );

                Reference< XRegistryKey > xNewKey = xKey->createKey(
                    ::rtl::OUString::createFromAscii( "/" IMPLEMENTATION_NAME "/UNO/SERVICES" ) );
                xNewKey->createKey( ::rtl::OUString::createFromAscii( SERVICE_NAME ) );

                return sal_True;
            }
            catch (InvalidRegistryException &)
            {
                OSL_ENSURE( sal_False, "### InvalidRegistryException!" );
            }
    }
#endif
    return sal_True;
}

void* SAL_CALL component_getFactory(    const   sal_Char*   pImplementationName ,
                                                void*       pServiceManager     ,
                                                void*       pRegistryKey        )
{
    // Set default return value for this operation - if it failed.
    void* pReturn = NULL ;

    if  (
            ( pImplementationName   !=  NULL ) &&
            ( pServiceManager       !=  NULL )
        )
    {
        // Define variables which are used in following macros.
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory >   xFactory                                                                                                ;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    xServiceManager( reinterpret_cast< ::com::sun::star::lang::XMultiServiceFactory* >( pServiceManager ) ) ;

        //=============================================================================
        //  Add new macro line to handle new service.
        //
        //  !!! ATTENTION !!!
        //      Write no ";" at end of line and dont forget "else" ! (see macro)
        //=============================================================================
        IF_NAME_CREATECOMPONENTFACTORY( DownloaderLoader )
        IF_NAME_CREATECOMPONENTFACTORY( SfxFrameLoader_Impl )
        IF_NAME_CREATECOMPONENTFACTORY( SfxMacroLoader )
        IF_NAME_CREATECOMPONENTFACTORY( SfxStandaloneDocumentInfoObject )
        IF_NAME_CREATECOMPONENTFACTORY( SfxAppDispatchProvider )
        IF_NAME_CREATECOMPONENTFACTORY( SfxDocTplService )
        if ( SfxFilePicker::impl_getStaticImplementationNameOpen().equals( UNOOUSTRING::createFromAscii( pImplementationName ) ) )
        {
            xFactory = SfxFilePicker::impl_createFactoryOpen( xServiceManager );
        }
        if ( SfxFilePicker::impl_getStaticImplementationNameSave().equals( UNOOUSTRING::createFromAscii( pImplementationName ) ) )
        {
            xFactory = SfxFilePicker::impl_createFactorySave( xServiceManager );
        }

        // Factory is valid - service was found.
        if ( xFactory.is() )
        {
            xFactory->acquire();
            pReturn = xFactory.get();
        }
#if 0
        if (!xFactory.is() && pServiceManager )
        {
                ::rtl::OUString aImplementationName = ::rtl::OUString::createFromAscii( pImplementationName );
                if (aImplementationName == ::rtl::OUString::createFromAscii( IMPLEMENTATION_NAME ) )
                {
                    xFactory = ::cppu::createSingleFactory( xServiceManager, aImplementationName,
                                                IchitaroImportFilter_CreateInstance,
                                                IchitaroImportFilter::getSupportedServiceNames_Static() );
                }
                if (xFactory.is())
                {
                    xFactory->acquire();
                    pReturn = xFactory.get();
                }
        }
#endif
    }
    // Return with result of this operation.
    return pReturn ;
}
} // extern "C"


