/*************************************************************************
 *
 *  $RCSfile: appuno.cxx,v $
 *
 *  $Revision: 1.44 $
 *
 *  last change: $Author: mba $ $Date: 2002-04-15 09:58:14 $
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

#include <svtools/sbx.hxx>

#ifndef _SFXRECTITEM_HXX //autogen
#include <svtools/rectitem.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _WLDCRD_HXX //autogen
#include <tools/wldcrd.hxx>
#endif

#include <tools/urlobj.hxx>

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
#ifndef _BASIC_SBUNO_HXX
#include <basic/sbuno.hxx>
#endif

#include <svtools/sbxcore.hxx>
#include <svtools/ownlist.hxx>
#include <svtools/lckbitem.hxx>
#include <svtools/stritem.hxx>
#include <svtools/intitem.hxx>
#include <svtools/eitem.hxx>

#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORFACTORY_HPP_
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#endif
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
#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTEVENT_HPP_
#include <com/sun/star/frame/DispatchResultEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_DISPATCHRESULTSTATE_HPP_
#include <com/sun/star/frame/DispatchResultState.hpp>
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
#include "fcontnr.hxx"
#include "frmload.hxx"
#include "frame.hxx"
#include "sfxbasic.hxx"
#include "objsh.hxx"
#include "objuno.hxx"
#include "unoctitm.hxx"
#include "dispatch.hxx"
#include "doctemplates.hxx"
#include "shutdownicon.hxx"
#include "scriptcont.hxx"
#include "dlgcont.hxx"
#include "objshimp.hxx"

#define FRAMELOADER_SERVICENAME         "com.sun.star.frame.FrameLoader"
#define PROTOCOLHANDLER_SERVICENAME     "com.sun.star.frame.ProtocolHandler"

static const String sTemplateRegionName   = String::CreateFromAscii( "TemplateRegionName"   );
static const String sTemplateName   = String::CreateFromAscii( "TemplateName"   );
static const String sAsTemplate     = String::CreateFromAscii( "AsTemplate"     );
static const String sOpenNewView    = String::CreateFromAscii( "OpenNewView"    );
static const String sViewId         = String::CreateFromAscii( "ViewId"         );
static const String sPluginMode     = String::CreateFromAscii( "PluginMode"     );
static const String sReadOnly       = String::CreateFromAscii( "ReadOnly"       );
static const String sFrameName      = String::CreateFromAscii( "FrameName"      );
static const String sContentType    = String::CreateFromAscii( "ContentType"    );
static const String sPostData       = String::CreateFromAscii( "PostData"       );
static const String sPosSize        = String::CreateFromAscii( "PosSize"        );
static const String sCharacterSet   = String::CreateFromAscii( "CharacterSet"   );
static const String sInputStream    = String::CreateFromAscii( "InputStream"    );
static const String sOutputStream   = String::CreateFromAscii( "OutputStream"    );
static const String sHidden         = String::CreateFromAscii( "Hidden"         );
static const String sPreview        = String::CreateFromAscii( "Preview"        );
static const String sSilent         = String::CreateFromAscii( "Silent"         );
static const String sJumpMark       = String::CreateFromAscii( "JumpMark"       );
static const String sURL            = String::CreateFromAscii( "URL"            );
static const String sOrigURL        = String::CreateFromAscii( "OriginalURL"    );
static const String sSalvageURL     = String::CreateFromAscii( "SalvagedFile"   );
static const String sStatusInd      = String::CreateFromAscii( "StatusIndicator" );
static const String sModel          = String::CreateFromAscii( "Model" );
static const String sFrame          = String::CreateFromAscii( "Frame" );
static const String sViewData       = String::CreateFromAscii( "ViewData" );
static const String sFilterData     = String::CreateFromAscii( "FilterData" );
static const String sSelectionOnly  = String::CreateFromAscii( "SelectionOnly" );

void TransformParameters( sal_uInt16 nSlotId, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rArgs, SfxAllItemSet& rSet, const SfxSlot* pSlot )
{
    if ( !pSlot )
        pSlot = SFX_SLOTPOOL().GetSlot( nSlotId );

    if ( !pSlot )
        return;

    if ( nSlotId == SID_OPENURL )
        nSlotId = SID_OPENDOC;
    if ( nSlotId == SID_SAVEASURL )
        nSlotId = SID_SAVEASDOC;

    sal_Int32 nCount = rArgs.getLength();
    const ::com::sun::star::beans::PropertyValue* pPropsVal = rArgs.getConstArray();
    if ( pSlot->GetType()->Type() != TYPE(SfxVoidItem) )
    {
        // slot is a property
        SfxPoolItem* pItem = pSlot->GetType()->CreateItem();
        pItem->SetWhich( nSlotId );
        const SfxType* pType = pSlot->GetType();
        USHORT nSubCount = pType->nAttribs;
        if ( nSubCount == 0 )
        {
            // simple property
            DBG_ASSERT( nCount==1, "Wrong number of parameters!" );
            const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[0];
            String aName = rProp.Name;
            if ( aName.CompareToAscii( pSlot->pUnoName ) == COMPARE_EQUAL )
            {
                if( pItem->PutValue( rProp.Value ) )
                    rSet.Put( *pItem );
                else
                    DBG_ERROR("Property not convertable!");
            }
            else
                DBG_ERROR("Property name does not match!");
        }
        else
        {
            // complex property; collect sub items and reconstruct complex item
            for ( sal_uInt16 n=0; n<nCount; n++ )
            {
                const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
                const char* pName = ByteString( rProp.Name.getStr(), RTL_TEXTENCODING_UTF8 ).GetBuffer();
                for ( USHORT nSub=0; nSub<nSubCount; nSub++ )
                {
                    // search sub item by name
                    if ( !strcmp( pType->aAttrib[nSub].pName, pName ) )
                        pItem->PutValue( rProp.Value, (BYTE) (sal_Int8) pType->aAttrib[nSub].nAID );
                }

                rSet.Put( *pItem );
            }
        }

        delete pItem;
    }
    else
    {
        // slot is a method
        for ( sal_uInt16 nArgs=0; nArgs<pSlot->nArgDefCount; nArgs++ )
        {
            const SfxFormalArgument &rArg = pSlot->GetFormalArgument( nArgs );
            SfxPoolItem* pItem = rArg.CreateItem();
            pItem->SetWhich( rArg.nSlotId );
            const SfxType* pType = rArg.pType;
            USHORT nSubCount = pType->nAttribs;
            if ( nSubCount == 0 )
            {
                for ( sal_uInt16 n=0; n<nCount; n++ )
                {
                    const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
                    String aName = rProp.Name;
                    if ( aName.CompareToAscii(rArg.pName) == COMPARE_EQUAL )
                    {
                        if( pItem->PutValue( rProp.Value ) )
                            rSet.Put( *pItem );
                        else
                            DBG_ERROR("Property not convertable!");
                        break;
                    }
                }
            }
            else
            {
                // complex argument; collect sub items and reconstruct complex item
                for ( sal_uInt16 n=0; n<nCount; n++ )
                {
                    const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
                    const char* pName = ByteString( rProp.Name.getStr(), RTL_TEXTENCODING_UTF8 ).GetBuffer();
                    for ( USHORT nSub=0; nSub<nSubCount; nSub++ )
                    {
                        // search sub item by name
                        if ( !strcmp( pType->aAttrib[nSub].pName, pName ) )
                            pItem->PutValue( rProp.Value, (BYTE) (sal_Int8) pType->aAttrib[nSub].nAID );
                    }
                }

                rSet.Put( *pItem );
            }

            delete pItem;
        }

        // special additional parameters for some slots
        if ( nSlotId == SID_NEWWINDOW )
        {
            for ( sal_uInt16 n=0; n<nCount; n++ )
            {
                const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
                String aName = rProp.Name;
                if ( aName == sFrame )
                    rSet.Put( SfxUnoAnyItem( SID_FILLFRAME, rProp.Value ) );
                else if ( aName == sHidden && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_HIDDEN, *((sal_Bool*)rProp.Value.getValue()) ) );
            }
        }
        else if ( nSlotId == SID_OPENDOC || nSlotId == SID_EXPORTDOC || nSlotId == SID_SAVEASDOC || nSlotId == SID_SAVETO )
        {
            for ( sal_uInt16 n=0; n<nCount; n++ )
            {
                const ::com::sun::star::beans::PropertyValue& rProp = pPropsVal[n];
                String aName = rProp.Name;
                if ( aName == sModel )
                    rSet.Put( SfxUnoAnyItem( SID_DOCUMENT, rProp.Value ) );
                else if ( aName == sStatusInd )
                    rSet.Put( SfxUnoAnyItem( SID_PROGRESS_STATUSBAR_CONTROL, rProp.Value ) );
                else if ( aName == sViewData )
                    rSet.Put( SfxUnoAnyItem( SID_VIEW_DATA, rProp.Value ) );
                else if ( aName == sFilterData )
                    rSet.Put( SfxUnoAnyItem( SID_FILTER_DATA, rProp.Value ) );
                else if ( aName == sInputStream && rProp.Value.getValueType() == ::getCppuType( (Reference < XInputStream >*)0 ) )
                    rSet.Put( SfxUnoAnyItem( SID_INPUTSTREAM, rProp.Value ) );
                else if ( aName == sOutputStream && rProp.Value.getValueType() == ::getCppuType( (Reference < XOutputStream >*)0 ) )
                    rSet.Put( SfxUnoAnyItem( SID_OUTPUTSTREAM, rProp.Value ) );
                else if ( aName == sPostData && rProp.Value.getValueType() == ::getCppuType( (Reference < XInputStream >*)0 ) )
                    rSet.Put( SfxUnoAnyItem( SID_POSTDATA, rProp.Value ) );
                else if ( aName == sAsTemplate && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_TEMPLATE, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sOpenNewView && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_OPEN_NEW_VIEW, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sViewId && rProp.Value.getValueType() == ::getCppuType((const sal_Int16*)0) )
                    rSet.Put( SfxUInt16Item( SID_VIEW_ID, *((sal_Int16*)rProp.Value.getValue()) ) );
                else if ( aName == sPluginMode && rProp.Value.getValueType() == ::getCppuType((const sal_Int16*)0) )
                    rSet.Put( SfxUInt16Item( SID_PLUGIN_MODE, *((sal_Int16*)rProp.Value.getValue()) ) );
                else if ( aName == sReadOnly && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_DOC_READONLY, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sSelectionOnly && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_SELECTION, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sHidden && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_HIDDEN, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sSilent && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_SILENT, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sPreview && rProp.Value.getValueType() == ::getBooleanCppuType() )
                    rSet.Put( SfxBoolItem( SID_PREVIEW, *((sal_Bool*)rProp.Value.getValue()) ) );
                else if ( aName == sURL && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                    rSet.Put( SfxStringItem( SID_OPENURL, *((::rtl::OUString*)rProp.Value.getValue()) ) );
                else if ( aName == sOrigURL && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                    rSet.Put( SfxStringItem( SID_ORIGURL, *((::rtl::OUString*)rProp.Value.getValue()) ) );
                else if ( aName == sSalvageURL && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                    rSet.Put( SfxStringItem( SID_DOC_SALVAGE, *((::rtl::OUString*)rProp.Value.getValue()) ) );
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
                else if ( aName == sCharacterSet && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                    rSet.Put( SfxStringItem( SID_CHARSET, *((::rtl::OUString*)rProp.Value.getValue()) ) );
                else if ( aName == sPosSize && rProp.Value.getValueType() == ::getCppuType((const ::rtl::OUString*)0) )
                {
                    String aPar = *((::rtl::OUString*)rProp.Value.getValue());
                    Size aSize;
                    Point aPos;
                    DBG_ASSERT( sal_False, "TransformParameters()\nProperty \"PosSize\" isn't supported yet!\n" );
                }
            }
        }
    }
}

void TransformItems( sal_uInt16 nSlotId, const SfxItemSet& rSet, ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rArgs, const SfxSlot* pSlot )
{
    if ( !pSlot )
        pSlot = SFX_SLOTPOOL().GetSlot( nSlotId );

    if ( !pSlot)
        return;

    if ( nSlotId == SID_OPENURL )
        nSlotId = SID_OPENDOC;
    if ( nSlotId == SID_SAVEASURL )
        nSlotId = SID_SAVEASDOC;

    // find number of properties
    sal_Int32 nProps=0;

#ifdef DEBUG
    sal_Int32 nItems=0;
#endif

    const SfxType *pType = pSlot->GetType();
    if ( !pSlot->IsMode(SFX_SLOT_METHOD) )
    {
        // slot is a property
//        USHORT nWhich = rPool.GetWhich(nSlotId);
        if ( rSet.GetItemState( nSlotId ) == SFX_ITEM_SET ) //???
        {
            USHORT nSubCount = pType->nAttribs;
            if ( nSubCount )
                // it's a complex property, we want it split into simple types
                nProps = nSubCount;
            else
                nProps++;
        }

#ifdef DEBUG
        nItems++;
#endif
    }
    else
    {
        // slot is a method
        USHORT nFormalArgs = pSlot->GetFormalArgumentCount();
        for ( USHORT nArg=0; nArg<nFormalArgs; ++nArg )
        {
            // check every formal argument of the method
            const SfxFormalArgument &rArg = pSlot->GetFormalArgument( nArg );
//            USHORT nWhich = rPool.GetWhich( pArg->nSlotId );
            if ( rSet.GetItemState( rArg.nSlotId ) == SFX_ITEM_SET ) //???
            {
                USHORT nSubCount = rArg.pType->nAttribs;
                if ( nSubCount )
                    // argument has a complex type, we want it split into simple types
                    nProps += nSubCount;
                else
                    nProps++;
#ifdef DEBUG
                nItems++;
#endif
            }
        }

        if ( nSlotId == SID_OPENDOC || nSlotId == SID_EXPORTDOC || nSlotId == SID_SAVEASDOC || nSlotId == SID_SAVETO )
        {
            sal_Int32 nAdditional=0;
            if ( rSet.GetItemState( SID_PROGRESS_STATUSBAR_CONTROL ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_OPENURL ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_ORIGURL ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_DOC_SALVAGE ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_INPUTSTREAM ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_OUTPUTSTREAM ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_TEMPLATE ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_OPEN_NEW_VIEW ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_VIEW_ID ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_VIEW_DATA ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_FILTER_DATA ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_PLUGIN_MODE ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_DOC_READONLY ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_SELECTION ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_CONTENTTYPE ) == SFX_ITEM_SET )
                nAdditional++;
    //        if ( rSet.GetItemState( SID_VIEW_POS_SIZE ) == SFX_ITEM_SET )
    //            nAdditional++;
            if ( rSet.GetItemState( SID_POSTDATA ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_CHARSET ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_TARGETNAME ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_TEMPLATE_NAME ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_TEMPLATE_REGIONNAME ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_HIDDEN ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_PREVIEW ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_SILENT ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_JUMPMARK ) == SFX_ITEM_SET )
                nAdditional++;
            if ( rSet.GetItemState( SID_DOCUMENT ) == SFX_ITEM_SET )
                nAdditional++;

            nProps += nAdditional;
#ifdef DEBUG
            nItems += nAdditional;
#endif
        }
    }

#ifdef DEBUG
    DBG_ASSERT( rSet.Count() == nItems, "Unknown item detected!" );
    const USHORT *pRanges = rSet.GetRanges();
    while ( *pRanges )
    {
        for(USHORT nWhich = *pRanges++; nWhich <= *pRanges; ++nWhich)
        {
            const SfxPoolItem* pIt = rSet.GetItem( nWhich );
            pIt = 0;
        }
    }
#endif

    // convert every item into a property
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue> aSequ( nProps );
    ::com::sun::star::beans::PropertyValue *pValue = aSequ.getArray();
    nProps = 0;
    if ( !pSlot->IsMode(SFX_SLOT_METHOD) )
    {
        // slot is a property
//        USHORT nWhich = rPool.GetWhich(nSlotId);
        SFX_ITEMSET_ARG( &rSet, pItem, SfxPoolItem, nSlotId, sal_False );
        if ( pItem ) //???
        {
            USHORT nSubCount = pType->nAttribs;
            if ( !nSubCount )
            {
                //rPool.FillVariable( *pItem, *pVar, eUserMetric );
                pValue[nProps].Name = String( String::CreateFromAscii( pSlot->pUnoName ) ) ;
                if ( pItem && !pItem->QueryValue( pValue[nProps].Value ) )
                    DBG_ERROR("Item not convertable!");
            }
            else
            {
                // complex type, add a property value for every member of the struct
                for ( USHORT n=1; n<=nSubCount; ++n )
                {
                    //rPool.FillVariable( *pItem, *pVar, eUserMetric );
                    DBG_ASSERT( pType->aAttrib[n-1].nAID <= 255, "Member ID out of range" );
                    pValue[nProps].Name = String( String::CreateFromAscii( pType->aAttrib[n-1].pName ) ) ;
                    if ( pItem && !pItem->QueryValue( pValue[nProps++].Value, (BYTE) (sal_Int8) pType->aAttrib[n-1].nAID ) )
                        DBG_ERROR("Item not convertable!");
                }
            }
        }
    }
    else
    {
        // slot is a method
        USHORT nFormalArgs = pSlot->GetFormalArgumentCount();
        for ( USHORT nArg=0; nArg<nFormalArgs; ++nArg )
        {
            const SfxFormalArgument &rArg = pSlot->GetFormalArgument( nArg );
//            USHORT nWhich = rPool.GetWhich( nArg.nSlotId );
            SFX_ITEMSET_ARG( &rSet, pItem, SfxPoolItem, rArg.nSlotId, sal_False );
            if ( pItem ) //???
            {
                USHORT nSubCount = rArg.pType->nAttribs;
                if ( !nSubCount )
                {
                    //rPool.FillVariable( *pItem, *pVar, eUserMetric );
                    pValue[nProps].Name = String( String::CreateFromAscii( rArg.pName ) ) ;
                    if ( pItem && !pItem->QueryValue( pValue[nProps++].Value ) )
                        DBG_ERROR("Item not convertable!");
                }
                else
                {
                    // complex type, add a property value for every member of the struct
                    for ( USHORT n = 1; n <= nSubCount; ++n )
                    {
                        //rPool.FillVariable( rItem, *pVar, eUserMetric );
                        DBG_ASSERT( rArg.pType->aAttrib[n-1].nAID <= 255, "Member ID out of range" );
                        pValue[nProps].Name = String( String::CreateFromAscii( rArg.pType->aAttrib[n-1].pName ) ) ;
                        if ( pItem && !pItem->QueryValue( pValue[nProps++].Value, (BYTE) (sal_Int8) rArg.pType->aAttrib[n-1].nAID ) )
                            DBG_ERROR("Item not convertable!");
                    }
                }
            }
        }

        if ( nSlotId == SID_OPENDOC || nSlotId == SID_EXPORTDOC || nSlotId == SID_SAVEASDOC || nSlotId == SID_SAVETO )
        {
            const SfxPoolItem *pItem=0;
            if ( rSet.GetItemState( SID_PROGRESS_STATUSBAR_CONTROL, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sStatusInd;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_VIEW_DATA, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sViewData;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_FILTER_DATA, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sFilterData;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_DOCUMENT, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sModel;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_INPUTSTREAM, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sInputStream;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_OUTPUTSTREAM, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sOutputStream;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_POSTDATA, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sPostData;
                pValue[nProps++].Value = ( ((SfxUnoAnyItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_TEMPLATE, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sAsTemplate;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_OPEN_NEW_VIEW, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sOpenNewView;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_VIEW_ID, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sViewId;
                pValue[nProps++].Value <<= ( (sal_Int16) ((SfxUInt16Item*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_PLUGIN_MODE, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sPluginMode;
                pValue[nProps++].Value <<= ( (sal_Int16) ((SfxUInt16Item*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_DOC_READONLY, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sReadOnly;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_SELECTION, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sSelectionOnly;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_HIDDEN, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sHidden;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_SILENT, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sSilent;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_PREVIEW, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sPreview;
                pValue[nProps++].Value <<= ( ((SfxBoolItem*)pItem)->GetValue() );
            }
            if ( rSet.GetItemState( SID_TARGETNAME, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sFrameName;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue()) );
            }
            if ( rSet.GetItemState( SID_ORIGURL, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sOrigURL;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue()) );
            }
            if ( rSet.GetItemState( SID_DOC_SALVAGE, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sSalvageURL;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue()) );
            }
            if ( rSet.GetItemState( SID_CONTENTTYPE, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sContentType;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
            }
            if ( rSet.GetItemState( SID_TEMPLATE_NAME, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sTemplateName;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
            }
            if ( rSet.GetItemState( SID_TEMPLATE_REGIONNAME, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sTemplateRegionName;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
            }
            if ( rSet.GetItemState( SID_JUMPMARK, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sJumpMark;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
            }
            if ( rSet.GetItemState( SID_OPENURL, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sURL;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
            }

            SFX_ITEMSET_ARG( &rSet, pRectItem, SfxRectangleItem, SID_VIEW_POS_SIZE, sal_False );
            if ( pRectItem )
            {
    //            pValue[nProps].Name = sPosSize;
    //            Rectangle aRect = pRectItem->GetValue();
                DBG_ERROR("PosSizeItem not supported yet!");
            }

            if ( rSet.GetItemState( SID_CHARSET, sal_False, &pItem ) == SFX_ITEM_SET )
            {
                pValue[nProps].Name = sCharacterSet;
                pValue[nProps++].Value <<= (  ::rtl::OUString(((SfxStringItem*)pItem)->GetValue())  );
            }

        }
    }

    rArgs = aSequ;
}

SFX_IMPL_XINTERFACE_2( SfxMacroLoader, OWeakObject, ::com::sun::star::frame::XNotifyingDispatch, ::com::sun::star::frame::XDispatch )
SFX_IMPL_XTYPEPROVIDER_2( SfxMacroLoader, ::com::sun::star::frame::XNotifyingDispatch, ::com::sun::star::frame::XDispatch )
SFX_IMPL_XSERVICEINFO( SfxMacroLoader, PROTOCOLHANDLER_SERVICENAME, "com.sun.star.comp.sfx2.SfxMacroLoader" )
SFX_IMPL_SINGLEFACTORY( SfxMacroLoader )

// -----------------------------------------------------------------------
void SAL_CALL SfxMacroLoader::dispatchWithNotification( const ::com::sun::star::util::URL&                                                          aURL      ,
                                                        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >&            lArgs     ,
                                                        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& xListener )
              throw (::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    sal_uInt32 nPropertyCount = lArgs.getLength();
    ::rtl::OUString aReferer;
    for( sal_uInt32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
    {
        if( lArgs[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("Referer")) )
        {
            lArgs[nProperty].Value >>= aReferer;
            break;
        }
    }

    // call from UI?
    ErrCode nErr = ERRCODE_NONE;
    if ( aReferer.compareToAscii("private:select", 12) == 0 )
        nErr = loadMacro( aURL.Complete, SfxObjectShell::Current() );
    else
        nErr = loadMacro( aURL.Complete );

    nErr = loadMacro( aURL.Complete );
    if( xListener.is() )
    {
        // always call dispatchFinished(), because we didn't load a document but
        // executed a macro instead!
        ::com::sun::star::frame::DispatchResultEvent aEvent;

        aEvent.Source = static_cast< ::cppu::OWeakObject* >(this);
        if( nErr == ERRCODE_NONE )
            aEvent.State = ::com::sun::star::frame::DispatchResultState::SUCCESS;
        else
            aEvent.State = ::com::sun::star::frame::DispatchResultState::FAILURE;

        xListener->dispatchFinished( aEvent ) ;
    }
}

// -----------------------------------------------------------------------
void SAL_CALL SfxMacroLoader::dispatch( const ::com::sun::star::util::URL&                                               aURL  ,
                                        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArgs )
              throw (::com::sun::star::uno::RuntimeException)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    sal_uInt32 nPropertyCount = lArgs.getLength();
    ::rtl::OUString aReferer;
    for( sal_uInt32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
    {
        if( lArgs[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("Referer")) )
        {
            lArgs[nProperty].Value >>= aReferer;
            break;
        }
    }

    // call from UI?
    if ( aReferer.compareToAscii("private:select", 12) == 0 )
        loadMacro( aURL.Complete, SfxObjectShell::Current() );
    else
        loadMacro( aURL.Complete );
}

// -----------------------------------------------------------------------
void SAL_CALL SfxMacroLoader::addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl ,
                                                 const ::com::sun::star::util::URL&                                                  aURL     )
              throw (::com::sun::star::uno::RuntimeException)
{
    /* TODO
            How we can handle different listener for further coming or currently running dispatch() jobs
            without any inconsistency!
     */
}

// -----------------------------------------------------------------------
void SAL_CALL SfxMacroLoader::removeStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl ,
                                                    const ::com::sun::star::util::URL&                                                  aURL     )
        throw (::com::sun::star::uno::RuntimeException)
{
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
ErrCode SfxMacroLoader::loadMacro( const ::rtl::OUString& rURL, SfxObjectShell* pSh )
    throw ( ::com::sun::star::uno::RuntimeException )
{
    SfxApplication* pApp = SFX_APP();
    pApp->EnterBasicCall();
    SfxObjectShell* pCurrent = pSh;
    if ( !pCurrent )
        // all not full qualified names use the BASIC of the given or current document
        pCurrent = SfxObjectShell::Current();

    // 'macro:///lib.mod.proc(args)' => macro of App-BASIC
    // 'macro://[docname|.]/lib.mod.proc(args)' => macro of current or qualified document
    // 'macro://obj.method(args)' => direct API call, execute it via App-BASIC
    String aMacro( rURL );
    sal_uInt16 nHashPos = aMacro.Search( '/', 8 );
    sal_uInt16 nArgsPos = aMacro.Search( '(' );
    BasicManager *pAppMgr = SFX_APP()->GetBasicManager();
    BasicManager *pBasMgr = 0;
    ErrCode nErr = ERRCODE_NONE;

    // should a macro function be executed ( no direct API call)?
    if ( STRING_NOTFOUND != nHashPos && nHashPos < nArgsPos )
    {
        // find BasicManager
        SfxObjectShell* pDoc = NULL;
        String aBasMgrName( INetURLObject::decode(aMacro.Copy( 8, nHashPos-8 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET) );
        if ( !aBasMgrName.Len() )
            pBasMgr = pAppMgr;
        else if ( aBasMgrName.EqualsAscii(".") )
        {
            // current/actual document
            pDoc = pCurrent;
            pBasMgr = pDoc->GetBasicManager();
        }
        else
        {
            // full qualified name, find document by name
            for ( SfxObjectShell *pObjSh = SfxObjectShell::GetFirst();
                    pObjSh && !pBasMgr;
                    pObjSh = SfxObjectShell::GetNext(*pObjSh) )
                if ( aBasMgrName == pObjSh->GetTitle(SFX_TITLE_APINAME) )
                {
                    pDoc = pObjSh;
                    pBasMgr = pDoc->GetBasicManager();
                }
        }

        if ( pBasMgr )
        {
            if ( pSh && pDoc )
            {
                // security check for macros from document basic if an SFX context (pSh) is given
                pDoc->AdjustMacroMode( String() );
                if ( pDoc->Get_Impl()->nMacroMode == eNEVER_EXECUTE )
                    // check forbids execution
                    return ERRCODE_IO_ACCESSDENIED;;
            }

            // find BASIC method
            String aQualifiedMethod( INetURLObject::decode(aMacro.Copy( nHashPos+1 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET) );
            String aArgs;
            if ( STRING_NOTFOUND != nArgsPos )
            {
                // remove arguments from macro name
                aArgs = aQualifiedMethod.Copy( nArgsPos - nHashPos - 1 );
                aQualifiedMethod.Erase( nArgsPos - nHashPos - 1 );
            }

            SbxMethod *pMethod = SfxQueryMacro( pBasMgr, aQualifiedMethod );
            if ( pMethod )
            {
                // arguments must be quoted
                String aQuotedArgs;
                if ( aArgs.Len()<2 || aArgs.GetBuffer()[1] == '\"')
                    // no args or already quoted args
                    aQuotedArgs = aArgs;
                else
                {
                    // quote parameters
                    aArgs.Erase(0,1);
                    aArgs.Erase( aArgs.Len()-1,1);

                    aQuotedArgs = '(';

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

                SbxBaseRef xOldVar;
                SbxVariable *pCompVar = NULL;
                if ( pSh )
                {
                    // mark document: it executes a macro, so it's in a modal mode
                    pSh->SetMacroMode_Impl( TRUE );
                    if ( pBasMgr == pAppMgr )
                    {
                        // document is executed via AppBASIC, adjust "ThisComponent" variable
                        StarBASIC* pBas = pAppMgr->GetLib(0);
                        pCompVar = pBas->Find( DEFINE_CONST_UNICODE("ThisComponent"), SbxCLASS_OBJECT );
                        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                                xInterface ( pSh->GetModel() , ::com::sun::star::uno::UNO_QUERY );
                        ::com::sun::star::uno::Any aAny;
                        aAny <<= xInterface;
                        if ( pCompVar )
                        {
                            xOldVar = pCompVar->GetObject();
                            pCompVar->PutObject( GetSbUnoObject( DEFINE_CONST_UNICODE("ThisComponent"), aAny ) );
                        }
                        else
                        {
                            SbxObjectRef xUnoObj = GetSbUnoObject( DEFINE_CONST_UNICODE("ThisComponent"), aAny );
                            xUnoObj->SetFlag( SBX_DONTSTORE );
                            pBas->Insert( xUnoObj );
                            pCompVar = pBas->Find( DEFINE_CONST_UNICODE("ThisComponent"), SbxCLASS_OBJECT );
                        }
                    }
                }

                // add quoted arguments and do the call
                String aCall( '[' );
                aCall += pMethod->GetName();
                aCall += aQuotedArgs;
                aCall += ']';

                // execute function using its Sbx parent,
                pMethod->GetParent()->Execute( aCall );
                nErr = SbxBase::GetError();
                if ( pCompVar )
                    // reset "ThisComponent" to prior value
                    pCompVar->PutObject( xOldVar );

                if ( pSh )
                    // remove flag for modal mode
                    pSh->SetMacroMode_Impl( FALSE );
            }
            else
                nErr = ERRCODE_BASIC_PROC_UNDEFINED;
        }
        else
            nErr = ERRCODE_IO_NOTEXISTS;
    }
    else
    {
        // direct API call on a specified object
        String aCall( '[' );
        aCall += INetURLObject::decode(aMacro.Copy(6), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET);
        aCall += ']';
        pAppMgr->GetLib(0)->Execute( aCall );
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

    // quickstart wrapper service
    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += ShutdownIcon::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.office.Quickstart") );

    // script library container service
    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxScriptLibraryContainer::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.script.ScriptLibraryContainer") );

    // application script library container service
    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxApplicationScriptLibraryContainer::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.script.ApplicationScriptLibraryContainer") );

    // dialog library container service
    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxDialogLibraryContainer::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.script.DialogLibraryContainer") );

    // application dialog library container service
    aImpl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/"));
    aImpl += SfxApplicationDialogLibraryContainer::impl_getStaticImplementationName();

    aTempStr = aImpl;
    aTempStr += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/UNO/SERVICES"));
    xNewKey = xKey->createKey( aTempStr );
    xNewKey->createKey( ::rtl::OUString::createFromAscii("com.sun.star.script.ApplicationDialogLibraryContainer") );

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
        IF_NAME_CREATECOMPONENTFACTORY( SfxFrameLoader_Impl )
        IF_NAME_CREATECOMPONENTFACTORY( SfxMacroLoader )
        IF_NAME_CREATECOMPONENTFACTORY( SfxStandaloneDocumentInfoObject )
        IF_NAME_CREATECOMPONENTFACTORY( SfxAppDispatchProvider )
        IF_NAME_CREATECOMPONENTFACTORY( SfxDocTplService )
        IF_NAME_CREATECOMPONENTFACTORY( ShutdownIcon )
        IF_NAME_CREATECOMPONENTFACTORY( SfxScriptLibraryContainer )
        IF_NAME_CREATECOMPONENTFACTORY( SfxDialogLibraryContainer )
        IF_NAME_CREATECOMPONENTFACTORY( SfxApplicationScriptLibraryContainer )
        IF_NAME_CREATECOMPONENTFACTORY( SfxApplicationDialogLibraryContainer )

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


