/*************************************************************************
 *
 *  $RCSfile: objstor.cxx,v $
 *
 *  $Revision: 1.149 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-19 11:15:42 $
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

#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _EXTATTR_HXX //autogen
#include <svtools/extattr.hxx>
#endif
#ifndef _ZCODEC_HXX //autogen
#include <tools/zcodec.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XFILTER_HPP_
#include <com/sun/star/document/XFilter.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XIMPORTER_HPP_
#include <com/sun/star/document/XImporter.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XEXPORTER_HPP_
#include <com/sun/star/document/XExporter.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_FILTEROPTIONSREQUEST_HPP_
#include <com/sun/star/document/FilterOptionsRequest.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XINTERACTIONFILTEROPTIONS_HPP_
#include <com/sun/star/document/XInteractionFilterOptions.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif

#ifndef  _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_MACROEXECMODE_HPP_
#include <com/sun/star/document/MacroExecMode.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_EXTENDEDFILEPICKERELEMENTIDS_HPP_
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPICKERCONTROLACCESS_HPP_
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif
#ifndef  _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYACCESS_HPP_
#include <com/sun/star/beans/XPropertyAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ASPECTS_HPP_
#include <com/sun/star/embed/Aspects.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDPERSIST_HPP_
#include <com/sun/star/embed/XEmbedPersist.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XLINKAGESUPPORT_HPP_
#include <com/sun/star/embed/XLinkageSupport.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ENTRYINITMODES_HPP_
#include <com/sun/star/embed/EntryInitModes.hpp>
#endif

#ifndef _COM_SUN_STAR_IO_XTRUNCATE_HPP_
#include <com/sun/star/io/XTruncate.hpp>
#endif

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#pragma hdrstop

#ifndef _SFXECODE_HXX
#include <svtools/sfxecode.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_SECURITYOPTIONS_HXX
#include <svtools/securityoptions.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _CACHESTR_HXX
#include <tools/cachestr.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_ADDXMLTOSTORAGEOPTIONS_HXX
#include <svtools/addxmltostorageoptions.hxx>
#endif
#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif

#include <svtools/saveopt.hxx>
#include <svtools/useroptions.hxx>
#include <svtools/pathoptions.hxx>
#include <tools/urlobj.hxx>
#include <unotools/localfilehelper.hxx>
#include <unotools/ucbhelper.hxx>
#include <unotools/tempfile.hxx>
#include <ucbhelper/content.hxx>
#include <sot/storinfo.hxx>
#include <sot/exchange.hxx>
#include <sot/formats.hxx>
#include <shell/systemshell.hxx>
#include <comphelper/storagehelper.hxx>
#include <comphelper/seqstream.hxx>
#include <vcl/bitmapex.hxx>
#include <svtools/embedhlp.hxx>

#include "objsh.hxx"
#include "childwin.hxx"
#include "sfxdir.hxx"
#include "request.hxx"
#include "sfxresid.hxx"
#include "docfile.hxx"
#include "fltfnc.hxx"
#include "docfilt.hxx"
#include "docinf.hxx"
#include "docfac.hxx"
#include "objshimp.hxx"
#include "sfxtypes.hxx"
#include "appdata.hxx"
#include "doc.hrc"
#include "sfxsids.hrc"
#include "module.hxx"
#include "dispatch.hxx"
#include "openflag.hxx"
#include "helper.hxx"
#include "dlgcont.hxx"
#include "filedlghelper.hxx"
#include "scriptcont.hxx"
#include "event.hxx"
#include "fltoptint.hxx"
#include "viewfrm.hxx"
#include "graphhelp.hxx"

#include "../appl/app.hrc"

extern sal_uInt32 CheckPasswd_Impl( SfxObjectShell*, SfxItemPool&, SfxMedium* );

using namespace ::com::sun::star;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ui::dialogs;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::document;
using namespace ::rtl;
using namespace ::cppu;

//=========================================================================

sal_Bool SfxObjectShell::Save()
{
    return SaveChildren();
}

//--------------------------------------------------------------------------

sal_Bool SfxObjectShell::SaveAs( SfxMedium& rMedium )
{
    return SaveAsChildren( rMedium );
}

//-------------------------------------------------------------------------

sal_Bool GetPasswd_Impl( const SfxItemSet* pSet, String& rPasswd )
{
    const SfxPoolItem* pItem = NULL;
    if ( pSet && SFX_ITEM_SET == pSet->GetItemState( SID_PASSWORD, sal_True, &pItem ) )
    {
        DBG_ASSERT( pItem->IsA( TYPE(SfxStringItem) ), "wrong item type" );
        rPasswd = ( (const SfxStringItem*)pItem )->GetValue();
        return sal_True;
    }
    return sal_False;
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::PutURLContentsToVersionStream_Impl(
                                            ::rtl::OUString aURL,
                                            const uno::Reference< embed::XStorage >& xDocStorage,
                                            ::rtl::OUString aStreamName )
{
    sal_Bool bResult = sal_False;
    try
    {
        uno::Reference< embed::XStorage > xVersion = xDocStorage->openStorageElement(
                                                        ::rtl::OUString::createFromAscii( "Versions" ),
                                                        embed::ElementModes::READWRITE );

        DBG_ASSERT( xVersion.is(),
                "The method must throw an exception if the storage can not be opened!\n" );
        if ( !xVersion.is() )
            throw uno::RuntimeException();

        uno::Reference< io::XStream > xVerStream = xVersion->openStreamElement(
                                                                aStreamName,
                                                                embed::ElementModes::READWRITE );
        DBG_ASSERT( xVerStream.is(), "The method must throw an exception if the storage can not be opened!\n" );
        if ( !xVerStream.is() )
            throw uno::RuntimeException();

        uno::Reference< io::XOutputStream > xOutStream = xVerStream->getOutputStream();
        uno::Reference< io::XTruncate > xTrunc( xOutStream, uno::UNO_QUERY );

        DBG_ASSERT( xTrunc.is(), "The output stream must exist and implement XTruncate interface!\n" );
        if ( !xTrunc.is() )
            throw RuntimeException();

        uno::Reference< io::XInputStream > xTmpInStream =
            ::comphelper::OStorageHelper::GetInputStreamFromURL( aURL );
        DBG_ASSERT( xTmpInStream.is(), "The method must create the stream or throw an exception!\n" );
        if ( !xTmpInStream.is() )
            throw uno::RuntimeException();

        xTrunc->truncate();
        ::comphelper::OStorageHelper::CopyInputToOutput( xTmpInStream, xOutStream );
        xOutStream->closeOutput();

        uno::Reference< embed::XTransactedObject > xTransact( xVersion, uno::UNO_QUERY );
        DBG_ASSERT( xTransact.is(), "The storage must implement XTransacted interface!\n" );
        if ( xTransact.is() )
            xTransact->commit();

        bResult = sal_True;
    }
    catch( uno::Exception& )
    {
        // TODO/LATER: handle the error depending on exception
        SetError( ERRCODE_IO_GENERAL );
    }

    return bResult;
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::CreateVersionByName_Impl( SfxMedium& rMedium,
                                                const ::rtl::OUString& aVersionName,
                                                const ::rtl::OUString& aPasswd )
{
    sal_Bool bResult = sal_False;

    ::rtl::OUString aTempURL = ::utl::TempFile().GetURL();

    DBG_ASSERT( aTempURL.getLength(), "Can't create a temporary file!\n" );
    if ( aTempURL.getLength() )
    {
        // the storage existance must be checked by the caller
        uno::Reference< embed::XStorage > xMedStorage = rMedium.GetStorage();

        try
        {
            uno::Reference< embed::XStorage > xTempStorage =
                ::comphelper::OStorageHelper::GetStorageFromURL( aTempURL, embed::ElementModes::READWRITE );

              // save the new version to the storage, perhaps also with password if the root storage is password protected
            if ( aPasswd.getLength() )
            {
                // the following code must throw an exception in case of failure
                ::comphelper::OStorageHelper::SetCommonStoragePassword( xTempStorage, aPasswd );
            }

              // save again, now as a version, use the target medium as a transport medium
              // this should be changed in the future, using a different medium seems to be better
              rMedium.SetStorage_Impl( xTempStorage );
              if ( SaveAsOwnFormat( rMedium ) )
            {
                uno::Reference< embed::XTransactedObject > xTransact( xTempStorage, uno::UNO_QUERY );
                DBG_ASSERT( xTransact.is(), "The storage implementation must implement XTransactedObject!" );
                if ( xTransact.is() )
                    xTransact->commit();

                bResult = sal_True;
            }

            // the temporary storage will be closed by refcounting
        }
        catch ( uno::Exception& )
        {
            DBG_ERROR( "Setting of common encryption key failed!" );
            aTempURL = ::rtl::OUString();

            // TODO/LATER: may need error code setting based on exception
            SetError( ERRCODE_IO_GENERAL );
        }

              // reconnect medium to "old" storage
        rMedium.SetStorage_Impl( xMedStorage );

              // compress stream and store it into the root storage
        if ( bResult )
            bResult = PutURLContentsToVersionStream_Impl( aTempURL, xMedStorage, aVersionName );

        ::utl::UCBContentHelper::Kill( aTempURL );
    }
    else
    {
        // TODO/LATER: error handling?
        SetError( ERRCODE_IO_GENERAL );
    }

    return bResult;
}

//-------------------------------------------------------------------------
SvGlobalName SfxObjectShell::GetClassName() const
{
    return GetFactory().GetClassId();
}

//-------------------------------------------------------------------------
void SfxObjectShell::SetupStorage( const uno::Reference< embed::XStorage >& xStorage, sal_Int32 nVersion ) const
{
    uno::Reference< beans::XPropertySet > xProps( xStorage, uno::UNO_QUERY );

    if ( xProps.is() )
    {
        SvGlobalName aName;
        String aFullTypeName, aShortTypeName, aAppName;
        ULONG nClipFormat;

        FillClass( &aName, &nClipFormat, &aAppName,
                    &aFullTypeName, &aShortTypeName, nVersion );

        datatransfer::DataFlavor aDataFlavor;
        SotExchange::GetFormatDataFlavor( nClipFormat, aDataFlavor );
        if ( aDataFlavor.MimeType.getLength() )
        {
            // basic doesn't have a media type!
            try {
                xProps->setPropertyValue( ::rtl::OUString::createFromAscii( "MediaType" ),
                                            uno::makeAny( aDataFlavor.MimeType ) );
            } catch( uno::Exception& )
            {}
        }
    }
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::GeneralInit_Impl( const uno::Reference< embed::XStorage >& xStorage,
                                            sal_Bool bTypeMustBeSetAlready )
{
    if ( pImp->m_bIsInit )
        return sal_False;

    pImp->m_bIsInit = sal_True;
    if ( xStorage.is() )
    {
        // no notification is required the storage is set the first time
        pImp->m_xDocStorage = xStorage;

        try {
            uno::Reference < beans::XPropertySet > xPropSet( xStorage, uno::UNO_QUERY_THROW );
            Any a = xPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "MediaType" ) ) );
            ::rtl::OUString aMediaType;
            if ( !(a>>=aMediaType) || !aMediaType.getLength() )
            {
                if ( bTypeMustBeSetAlready )
                    OSL_ENSURE( sal_False, "The mediatype must be set already!\n" );
                SetupStorage( xStorage, SOFFICE_FILEFORMAT_CURRENT );
            }
        }
        catch ( uno::Exception& )
        {
            OSL_ENSURE( sal_False, "Can't check storage's mediatype!\n" );
        }
    }
    else
        pImp->m_bCreateTempStor = sal_True;

    return sal_True;
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::InitNew( const uno::Reference< embed::XStorage >& xStorage )
{
    return GeneralInit_Impl( xStorage, sal_False );
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::Load( SfxMedium& rMedium )
{
    return GeneralInit_Impl( rMedium.GetStorage(), sal_True );
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::DoInitNew_Impl( const ::rtl::OUString& rName )

/*  [Beschreibung]
*/

{
    if ( rName.getLength() )
    {
//REMOVE            ModifyBlocker_Impl aBlock( this );
//REMOVE            pMedium = new SfxMedium( rName, SFX_STREAM_READONLY_MAKECOPY, sal_False );
//REMOVE            if ( InitNew( pMedium->GetStorage() ) )
//REMOVE            {
//REMOVE                bIsTmp = !( pMedium->GetStorage() );
//REMOVE                if ( SFX_CREATE_MODE_EMBEDDED == eCreateMode )
//REMOVE                    SetTitle( String( SfxResId( STR_NONAME ) ));
//REMOVE
//REMOVE                uno::Reference< frame::XModel >  xModel ( GetModel(), uno::UNO_QUERY );
//REMOVE                if ( xModel.is() )
//REMOVE                {
//REMOVE                    SfxItemSet *pSet = GetMedium()->GetItemSet();
//REMOVE                    uno::Sequence< beans::PropertyValue > aArgs;
//REMOVE                    TransformItems( SID_OPENDOC, *pSet, aArgs );
//REMOVE                    sal_Int32 nLength = aArgs.getLength();
//REMOVE                    aArgs.realloc( nLength + 1 );
//REMOVE                    aArgs[nLength].Name = DEFINE_CONST_UNICODE("Title");
//REMOVE                    aArgs[nLength].Value <<= ::rtl::OUString( GetTitle( SFX_TITLE_DETECT ) );
//REMOVE                    xModel->attachResource( ::rtl::OUString(), aArgs );
//REMOVE                }
//REMOVE
//REMOVE                SetActivateEvent_Impl( SFX_EVENT_CREATEDOC );
//REMOVE                return sal_True;
//REMOVE            }
//REMOVE            return sal_False;

        DBG_ERROR( "This code is intended to be removed, the caller part must be checked!\n" );
        return DoInitNew(0);
    }
    else
        return DoInitNew(0);
}


sal_Bool SfxObjectShell::DoInitNew( SfxMedium* pMed )
/*  [Beschreibung]

    Diese von SvPersist geerbte virtuelle Methode wird gerufen, um
    die SfxObjectShell-Instanz aus einem Storage (pStor != 0) bzw.
    (pStor == 0) ganz neu zu initialisieren.

    Wie alle Do...-Methoden liegt hier eine Steuerung vor, die eigentliche
    Implementierung erfolgt, indem die ebenfalls virtuellen Methode
    InitNew(SvStorate*) von der SfxObjectShell-Subclass implementiert wird.

    F"ur pStor == 0 wird ein die SfxObjectShell-Instanz mit einem leeren
    SfxMedium verbunden, sonst mit einem SfxMedium, welches auf den
    als Parameter "ubergeben SvStorage verweist.

    Erst nach InitNew() oder Load() ist das Objekt korrekt initialisiert.

    [R"uckgabewert]
    sal_True            Das Objekt wurde initialisiert.
    sal_False           Das Objekt konnte nicht initialisiert werden
*/

{
    ModifyBlocker_Impl aBlock( this );
    pMedium = pMed;
    if ( !pMedium )
    {
        bIsTmp = sal_True;
        pMedium = new SfxMedium;
    }

    pMedium->CanDisposeStorage_Impl( sal_True );

    if ( InitNew( pMed ? pMed->GetStorage() : uno::Reference < embed::XStorage >() ) )
    {
        // empty documents always get their macros from the user, so there is no reason to restrict access
        pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
        if ( SFX_CREATE_MODE_EMBEDDED == eCreateMode )
            SetTitle( String( SfxResId( STR_NONAME ) ));

        uno::Reference< frame::XModel >  xModel ( GetModel(), uno::UNO_QUERY );
        if ( xModel.is() )
        {
            SfxItemSet *pSet = GetMedium()->GetItemSet();
/*
            const SfxFilter* pFilter = GetFactory().GetFilterContainer()->GetAnyFilter( SFX_FILTER_IMPORT | SFX_FILTER_EXPORT );
            if ( pFilter )
                pSet->Put( SfxStringItem( SID_FILTER_NAME, pFilter->GetFilterName() ) );
*/
            uno::Sequence< beans::PropertyValue > aArgs;
            TransformItems( SID_OPENDOC, *pSet, aArgs );
            sal_Int32 nLength = aArgs.getLength();
            aArgs.realloc( nLength + 1 );
            aArgs[nLength].Name = DEFINE_CONST_UNICODE("Title");
            aArgs[nLength].Value <<= ::rtl::OUString( GetTitle( SFX_TITLE_DETECT ) );
            xModel->attachResource( ::rtl::OUString(), aArgs );
        }

        SetActivateEvent_Impl( SFX_EVENT_CREATEDOC );
        return sal_True;
    }

    return sal_False;
}

//REMOVE    //-------------------------------------------------------------------------
//REMOVE
//REMOVE    void SfxObjectShell::DoHandsOffNoMediumClose()
//REMOVE    {
//REMOVE        const SfxFilter *pFilter = pMedium->GetFilter();
//REMOVE        if( !pFilter || pFilter->IsOwnFormat() || ( pFilter->GetFilterFlags() & SFX_FILTER_PACKED ) )
//REMOVE            HandsOff();
//REMOVE
//REMOVE        // Force document library containers to release storage
//REMOVE        SotStorageRef xDummyStorage;
//REMOVE        SfxDialogLibraryContainer* pDialogCont = pImp->pDialogLibContainer;
//REMOVE        if( pDialogCont )
//REMOVE            pDialogCont->setStorage( xDummyStorage );
//REMOVE
//REMOVE        SfxScriptLibraryContainer* pBasicCont = pImp->pBasicLibContainer;
//REMOVE        if( pBasicCont )
//REMOVE            pBasicCont->setStorage( xDummyStorage );
//REMOVE    }

//REMOVE    //-------------------------------------------------------------------------
//REMOVE
//REMOVE    void SfxObjectShell::DoHandsOff()
//REMOVE
//REMOVE    /*  [Beschreibung]
//REMOVE
//REMOVE        Diese von SvPersist geerbte virtuelle Methode wird gerufen, um
//REMOVE        das Objekt aufzufordern, den ihm zugeteilten SvStorage freizugeben,
//REMOVE        insbesondere Substorages und Streams zu schlie"sen.
//REMOVE
//REMOVE        Als Do...-Methode liegt hier nur die Steuerung. Der Implementierer
//REMOVE        von Subclasses kann die ebenfalls virtuelle Methode HandsOff()
//REMOVE        implementieren, um seine Substorages und Streams zu schlie"sen.
//REMOVE
//REMOVE        Nach dem Aufruf dieser Methode, ist dem Objekt kein SfxMedium mehr
//REMOVE        zugeordnet, bis SaveCompleted() durchlaufen ist.
//REMOVE    */
//REMOVE
//REMOVE    {
//REMOVE        DoHandsOffNoMediumClose();
//REMOVE        pMedium->Close();
//REMOVE    //  DELETEZ( pMedium );
//REMOVE    }

//REMOVE    //-------------------------------------------------------------------------
//REMOVE
//REMOVE    sal_Bool SfxObjectShell::DoLoad(
//REMOVE        const String& rFileName, StreamMode nStreamMode, StorageMode nStorageMode)
//REMOVE    {
//REMOVE        // Es wird nur die IPersistStorage-Schnittstelle angeboten
//REMOVE        ModifyBlocker_Impl aBlock( this );
//REMOVE        SvStorageRef xStor = new SvStorage( rFileName, nStreamMode | STREAM_WRITE, nStorageMode );
//REMOVE        if( !xStor.Is() )
//REMOVE            xStor = new SvStorage( rFileName, nStreamMode, nStorageMode );
//REMOVE
//REMOVE        if ( SVSTREAM_OK == xStor->GetError() )
//REMOVE        {
//REMOVE            SfxMedium* pMedium = new SfxMedium( xStor );
//REMOVE            pMedium->SetName( rFileName );
//REMOVE            pMedium->Init_Impl();
//REMOVE
//REMOVE            // Muss !!!
//REMOVE            SetFileName( rFileName );
//REMOVE
//REMOVE            if( DoLoad( pMedium ) )
//REMOVE            {
//REMOVE                if ( SFX_CREATE_MODE_EMBEDDED == eCreateMode )
//REMOVE                    SetTitle( rFileName );
//REMOVE                return sal_True;
//REMOVE            }
//REMOVE        }
//REMOVE        return sal_False;
//REMOVE    }

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::DoLoad( SfxMedium *pMed )

/*  [Beschreibung]

    Diese Methode steuert das Laden des Objektes aus dem von dem
    "ubergebenen SfxMedium beschriebenen Medium. Hinterher ist das Objekt
    mit diesem SfxMedium verbunden.

    Handelt es sich bei dem SfxMedium um einen Storage im eigenen Format,
    wird die virtuelle Methode SvPersit::Load(SvStorage*) gerufen, welche
    die Implementierer von Subclasses "uberladen m"ussen, um das Objekt
    aus einem eigenen Storage zu laden (=> Swapping m"oeglich).

    Handelt es sich bei dem SfxMedium um einen Storage in einem fremden
    Format, oder um ein Flat-File, dann wird die virtuelle Methode
    <SfxObjectShell::ConvertFrom(SfxMedium*)> gerufen, welche die
    Implementierer von Subclasses "uberladen m"ussen, um das Objekt
    aus dem SfxMedium zu konvertieren. W"ahrend der Bearbeitung ist
    das Objekt dann mit einem tempor"aren SvStorage verbunden.

    Erst nach InitNew() oder Load() ist das Objekt korrekt
    initialisiert.

    [R"uckgabewert]
    sal_True                Das Objekt wurde geladen.
    sal_False           Das Objekt konnte nicht geladen werden
*/

{
    SfxApplication *pSfxApp = SFX_APP();
    ModifyBlocker_Impl aBlock( this );

    if ( SFX_CREATE_MODE_EMBEDDED != eCreateMode )
        GetpApp()->ShowStatusText( SfxResId(STR_DOC_LOADING) );

    pMedium = pMed;
    pMedium->CanDisposeStorage_Impl( sal_True );

    sal_Bool bOk = sal_False;
    const SfxFilter* pFilter = pMed->GetFilter();
    SfxItemSet* pSet = pMedium->GetItemSet();
    if( !pImp->nEventId )
    {
        SFX_ITEMSET_ARG(
            pSet, pTemplateItem, SfxBoolItem,
            SID_TEMPLATE, sal_False);
        SetActivateEvent_Impl(
            ( pTemplateItem && pTemplateItem->GetValue() )
            ? SFX_EVENT_CREATEDOC : SFX_EVENT_OPENDOC );
    }


    SFX_ITEMSET_ARG( pSet, pBaseItem, SfxStringItem,
                     SID_BASEURL, sal_False);
    String aBaseURL;
    SFX_ITEMSET_ARG( pMedium->GetItemSet(), pSalvageItem, SfxStringItem, SID_DOC_SALVAGE, sal_False);
    if( pBaseItem )
        aBaseURL = pBaseItem->GetValue();
    else
    {
        if ( pSalvageItem )
        {
            String aName( pMed->GetPhysicalName() );
            ::utl::LocalFileHelper::ConvertPhysicalNameToURL( aName, aBaseURL );
        }
        else
            aBaseURL = pMed->GetBaseURL();
    }

    SfxApplication* pApp = SFX_APP();
    pImp->nLoadedFlags = 0;

    //TODO/LATER: make a clear strategy how to handle "UsesStorage" etc.
    sal_Bool bHasStorage = IsOwnStorageFormat_Impl( *pMedium );
    if ( pMedium->GetFilter() )
    {
        sal_uInt32 nError = HandleFilter( pMedium, this );
        if ( nError != ERRCODE_NONE )
            SetError( nError );
    }

    bool bShowBrokenSignatureWarningAlready = false;
    if ( GetError() == ERRCODE_NONE && bHasStorage && ( !pFilter || !( pFilter->GetFilterFlags() & SFX_FILTER_STARONEFILTER ) ) )
    {
        uno::Reference< embed::XStorage > xStorage = pMed->GetStorage();
        if( pMed->GetLastStorageCreationState() == ERRCODE_NONE )
        {
            DBG_ASSERT( pFilter, "No filter for storage found!" );

            try
            {
                if ( xStorage->getElementNames().getLength() )
                {
                    BOOL bHasMacros =
                            ( xStorage->hasByName( ::rtl::OUString::createFromAscii("Basic") )
                                && xStorage->isStorageElement( ::rtl::OUString::createFromAscii("Basic") ) )
                            ||  ( xStorage->hasByName( ::rtl::OUString::createFromAscii("Scripts") )
                                && xStorage->isStorageElement( ::rtl::OUString::createFromAscii("Scripts") ) );

                    if ( bHasMacros )
                    {
                        // --> PB 2004-11-09 #i35190#
                        if ( GetDocumentSignatureState() == SIGNATURESTATE_SIGNATURES_BROKEN )
                        {
                            // if the signature is broken, show here the warning before
                            // the macro warning
                            WarningBox aBox( NULL, SfxResId( RID_XMLSEC_WARNING_BROKENSIGNATURE ) );
                            aBox.Execute();
                            bShowBrokenSignatureWarningAlready = true;
                        }
                        // <--
                        AdjustMacroMode( String() );
                        if ( SvtSecurityOptions().GetMacroSecurityLevel() >= 2
                            && MacroExecMode::NEVER_EXECUTE == pImp->nMacroMode )
                        {
                            WarningBox aBox( NULL, SfxResId( MSG_WARNING_MACRO_ISDISABLED ) );
                            aBox.Execute();
                        }
                    }
                    else
                    {
                        // if macros will be added by the user later, the security check is obsolete
                        pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
                    }
                }
                else
                    SetError( ERRCODE_IO_BROKENPACKAGE );
            }
            catch( uno::Exception& )
            {
                // TODO/LATER: may need error code setting based on exception
                SetError( ERRCODE_IO_GENERAL );
            }

            // Load
            if ( !GetError() )
            {
                pImp->nLoadedFlags = 0;
                bOk = xStorage.is() && LoadOwnFormat( *pMed );
                if ( bOk )
                {
                    SfxDocumentInfo& rDocInfo = GetDocInfo();
//REMOVE                        GetDocInfo().Load( xStor );
                    bHasName = sal_True;

                    // --> PB 2004-08-20 #i33095#
                    if ( !IsReadOnly() && rDocInfo.IsLoadReadonly() )
                        SetReadOnlyUI();
                    // <--
                }
                else
                    SetError( ERRCODE_ABORT );
            }
        }
        else
            SetError( pMed->GetLastStorageCreationState() );
    }
    else if ( GetError() == ERRCODE_NONE && InitNew(0) )
    {
        // Name vor ConvertFrom setzen, damit GetSbxObject() schon funktioniert
        bHasName = sal_True;
        SetName( SfxResId( STR_NONAME ) );

        // Importieren
        sal_Bool bHasStorage = IsOwnStorageFormat_Impl( *pMedium );
        if( !bHasStorage )
            pMedium->GetInStream();
        else
            pMedium->GetStorage();

        pImp->nLoadedFlags = 0;
        if ( pMedium->GetFilter() &&  ( pMedium->GetFilter()->GetFilterFlags() & SFX_FILTER_STARONEFILTER ) )
        {
            bOk = ImportFrom(*pMedium);
            FinishedLoading( SFX_LOADED_ALL );
        }
        else
        {
            bOk = ConvertFrom(*pMedium);
        }

        if ( HasMacros_Impl() )
        {
            // no signing in alien formats!
            AdjustMacroMode( String() );
            if ( SvtSecurityOptions().GetMacroSecurityLevel() >= 2
                && MacroExecMode::NEVER_EXECUTE == pImp->nMacroMode )
            {
                WarningBox aBox( NULL, SfxResId( MSG_WARNING_MACRO_ISDISABLED ) );
                aBox.Execute();
            }
        }
        else
        {
            // if macros will be added by the user later, the security check is obsolete
            pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
        }

    }

    if ( bOk )
    {
        try
        {
            ::ucb::Content aContent( pMedium->GetName(), com::sun::star::uno::Reference < XCommandEnvironment >() );
            com::sun::star::uno::Reference < XPropertySetInfo > xProps = aContent.getProperties();
            if ( xProps.is() )
            {
                ::rtl::OUString aAuthor( RTL_CONSTASCII_USTRINGPARAM("Author") );
                ::rtl::OUString aKeywords( RTL_CONSTASCII_USTRINGPARAM("Keywords") );
                ::rtl::OUString aSubject( RTL_CONSTASCII_USTRINGPARAM("Subject") );
                Any aAny;
                ::rtl::OUString aValue;
                SfxDocumentInfo& rInfo = GetDocInfo();
                if ( xProps->hasPropertyByName( aAuthor ) )
                {
                    aAny = aContent.getPropertyValue( aAuthor );
                    if ( ( aAny >>= aValue ) )
                        rInfo.SetCreated( SfxStamp( String( aValue ) ) );
                }
                if ( xProps->hasPropertyByName( aKeywords ) )
                {
                    aAny = aContent.getPropertyValue( aKeywords );
                    if ( ( aAny >>= aValue ) )
                        rInfo.SetKeywords( aValue );
                }
                if ( xProps->hasPropertyByName( aSubject ) )
                {
                    aAny = aContent.getPropertyValue( aSubject );
                    if ( ( aAny >>= aValue ) )
                        rInfo.SetTheme( aValue );
                }
            }
        }
        catch( Exception& )
        {
        }

        ::rtl::OUString aTitle = GetTitle( SFX_TITLE_DETECT );

        // Falls nicht asynchron geladen wird selbst FinishedLoading aufrufen
        if ( !( pImp->nLoadedFlags & SFX_LOADED_MAINDOCUMENT ) &&
            ( !pMedium->GetFilter() || pMedium->GetFilter()->UsesStorage() )
            )
            FinishedLoading( SFX_LOADED_MAINDOCUMENT );

        if ( pSalvageItem )
        {
            pImp->aTempName = pMedium->GetPhysicalName();
            pMedium->GetItemSet()->ClearItem( SID_DOC_SALVAGE );
            pMedium->GetItemSet()->ClearItem( SID_FILE_NAME );
            pMedium->GetItemSet()->Put( SfxStringItem( SID_FILE_NAME, pMedium->GetOrigURL() ) );
        }
        else
        {
            pMedium->GetItemSet()->ClearItem( SID_PROGRESS_STATUSBAR_CONTROL );
            pMedium->GetItemSet()->ClearItem( SID_DOCUMENT );
        }

        pMedium->GetItemSet()->ClearItem( SID_REFERER );
        uno::Reference< frame::XModel >  xModel ( GetModel(), uno::UNO_QUERY );
        if ( xModel.is() )
        {
            ::rtl::OUString aURL = GetMedium()->GetOrigURL();
            SfxItemSet *pSet = GetMedium()->GetItemSet();
            if ( !GetMedium()->IsReadOnly() )
                pSet->ClearItem( SID_INPUTSTREAM );
            uno::Sequence< beans::PropertyValue > aArgs;
            TransformItems( SID_OPENDOC, *pSet, aArgs );
            xModel->attachResource( aURL, aArgs );
        }

        if( IsOwnStorageFormat_Impl(*pMed) && pMed->GetFilter() )
        {
//???? dv           DirEntry aDirEntry( pMed->GetPhysicalName() );
//???? dv           SetFileName( aDirEntry.GetFull() );
        }
        Broadcast( SfxSimpleHint(SFX_HINT_NAMECHANGED) );
    }

    if ( SFX_CREATE_MODE_EMBEDDED != eCreateMode )
    {
        GetpApp()->HideStatusText();

        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pAsTempItem, SfxBoolItem, SID_TEMPLATE, sal_False);
        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pPreviewItem, SfxBoolItem, SID_PREVIEW, sal_False);
        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pHiddenItem, SfxBoolItem, SID_HIDDEN, sal_False);
        if( bOk && pMedium->GetOrigURL().Len()
         && !( pAsTempItem && pAsTempItem->GetValue() )
         && !( pPreviewItem && pPreviewItem->GetValue() )
         && !( pHiddenItem && pHiddenItem->GetValue() ) )
        {
            INetURLObject aUrl( pMedium->GetOrigURL() );

            if ( aUrl.GetProtocol() == INET_PROT_FILE )
            {
                const SfxFilter* pFilter = pMedium->GetOrigFilter();
                SystemShell::AddToRecentDocumentList( aUrl.GetURLNoPass( INetURLObject::NO_DECODE ), (pFilter) ? pFilter->GetMimeType() : String() );
            }
        }
    }

    // MAV: the code below is moved here since this is the only central place where the object shell is visible
    //      in case of pick list for example OpenDocExec_Impl() is not used.

    // xmlsec05, check with SFX team
    // Check if there is a broken signature...
    // After EA change to interaction handler...
    if ( !bShowBrokenSignatureWarningAlready
        && GetDocumentSignatureState() == SIGNATURESTATE_SIGNATURES_BROKEN )
    {
        WarningBox aBox( NULL, SfxResId( RID_XMLSEC_WARNING_BROKENSIGNATURE ) );
        aBox.Execute();
    }

    return bOk;
}

sal_uInt32 SfxObjectShell::HandleFilter( SfxMedium* pMedium, SfxObjectShell* pDoc )
{
    sal_uInt32 nError = ERRCODE_NONE;
    SfxItemSet* pSet = pMedium->GetItemSet();
    SFX_ITEMSET_ARG( pSet, pOptions, SfxStringItem, SID_FILE_FILTEROPTIONS, sal_False );
    SFX_ITEMSET_ARG( pSet, pData, SfxUnoAnyItem, SID_FILTER_DATA, sal_False );
    if ( !pData && !pOptions )
    {
        com::sun::star::uno::Reference< XMultiServiceFactory > xServiceManager = ::comphelper::getProcessServiceFactory();
        com::sun::star::uno::Reference< XNameAccess > xFilterCFG;
        if( xServiceManager.is() )
        {
            xFilterCFG = com::sun::star::uno::Reference< XNameAccess >(
                xServiceManager->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.document.FilterFactory" ) ),
                UNO_QUERY );
        }

        if( xFilterCFG.is() )
        {
            BOOL bAbort = FALSE;
            try {
                const SfxFilter* pFilter = pMedium->GetFilter();
                Sequence < PropertyValue > aProps;
                Any aAny = xFilterCFG->getByName( pFilter->GetName() );
                if ( aAny >>= aProps )
                {
                    ::rtl::OUString aServiceName;
                    sal_Int32 nPropertyCount = aProps.getLength();
                    for( sal_Int32 nProperty=0; nProperty < nPropertyCount; ++nProperty )
                        if( aProps[nProperty].Name.equals( ::rtl::OUString::createFromAscii("UIComponent")) )
                        {
                            ::rtl::OUString aServiceName;
                            aProps[nProperty].Value >>= aServiceName;
                            if( aServiceName.getLength() )
                            {
                                com::sun::star::uno::Reference< XInteractionHandler > rHandler = pMedium->GetInteractionHandler();
                                if( rHandler.is() )
                                {
                                    // we need some properties in the media descriptor, so we have to make sure that they are in
                                    Any aAny;
                                    aAny <<= pMedium->GetInputStream();
                                    if ( pSet->GetItemState( SID_INPUTSTREAM ) < SFX_ITEM_SET )
                                    pSet->Put( SfxUnoAnyItem( SID_INPUTSTREAM, aAny ) );
                                    if ( pSet->GetItemState( SID_FILE_NAME ) < SFX_ITEM_SET )
                                        pSet->Put( SfxStringItem( SID_FILE_NAME, pMedium->GetName() ) );
                                    if ( pSet->GetItemState( SID_FILTER_NAME ) < SFX_ITEM_SET )
                                        pSet->Put( SfxStringItem( SID_FILTER_NAME, pFilter->GetName() ) );

                                    Sequence< PropertyValue > rProperties;
                                    TransformItems( SID_OPENDOC, *pSet, rProperties, NULL );
                                    RequestFilterOptions* pFORequest = new RequestFilterOptions( pDoc->GetModel(), rProperties );

                                    com::sun::star::uno::Reference< XInteractionRequest > rRequest( pFORequest );
                                    rHandler->handle( rRequest );

                                    if ( !pFORequest->isAbort() )
                                    {
                                           SfxAllItemSet aNewParams( pDoc->GetPool() );
                                           TransformParameters( SID_OPENDOC,
                                                             pFORequest->getFilterOptions(),
                                                             aNewParams,
                                                             NULL );

                                           SFX_ITEMSET_ARG( &aNewParams,
                                                         pOptions,
                                                         SfxStringItem,
                                                         SID_FILE_FILTEROPTIONS,
                                                         sal_False );
                                           if ( pOptions )
                                               pSet->Put( *pOptions );

                                           SFX_ITEMSET_ARG( &aNewParams,
                                                         pData,
                                                         SfxUnoAnyItem,
                                                         SID_FILTER_DATA,
                                                         sal_False );
                                           if ( pData )
                                               pSet->Put( *pData );
                                    }
                                    else
                                        bAbort = TRUE;
                                }
                            }

                            break;
                        }
                }

                if( bAbort )
                {
                    // filter options were not entered
                    nError = ERRCODE_ABORT;
                }
            }
            catch( NoSuchElementException& )
            {
                // the filter name is unknown
                nError = ERRCODE_IO_INVALIDPARAMETER;
            }
            catch( Exception& )
            {
                nError = ERRCODE_ABORT;
            }
        }
    }

    return nError;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsOwnStorageFormat_Impl(const SfxMedium &rMedium) const
{
    return !rMedium.GetFilter() || // Embedded
           ( rMedium.GetFilter()->IsOwnFormat() &&
             rMedium.GetFilter()->UsesStorage() &&
             rMedium.GetFilter()->GetVersion() >= SOFFICE_FILEFORMAT_60 );
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::DoSave()
// DoSave wird nur noch ueber OLE aufgerufen. Sichern eigener Dokumente im SFX
// laeuft uber DoSave_Impl, um das Anlegen von Backups zu ermoeglichen.
// Save in eigenes Format jetzt auch wieder Hierueber
{
    sal_Bool bOk = sal_False ;
    {
        ModifyBlocker_Impl aBlock( this );

        pImp->bIsSaving = sal_True;

        String aPasswd;
        if ( IsOwnStorageFormat_Impl( *GetMedium() ) )
        {
            if ( GetPasswd_Impl( GetMedium()->GetItemSet(), aPasswd ) )
            {
                try
                {
                    //TODO/MBA: GetOutputStorage?! Special mode, because it's "Save"?!
                    ::comphelper::OStorageHelper::SetCommonStoragePassword( GetMedium()->GetStorage(), aPasswd );
                    bOk = sal_True;
                }
                catch( uno::Exception& )
                {
                    SetError( ERRCODE_IO_GENERAL );
                }

                DBG_ASSERT( bOk, "The root storage must allow to set common password!\n" );
            }
            else
                bOk = sal_True;
        }

        if ( bOk )
            bOk = Save();

        bOk = pMedium->Commit();
    }

//#88046
//    if ( bOk )
//        SetModified( sal_False );
    return bOk;
}

void Lock_Impl( SfxObjectShell* pDoc, BOOL bLock )
{
    SfxViewFrame *pFrame= SfxViewFrame::GetFirst( pDoc );
    while ( pFrame )
    {
        pFrame->GetDispatcher()->Lock( bLock );
        pFrame->Enable( !bLock );
        pFrame = SfxViewFrame::GetNext( *pFrame, pDoc );
    }

}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::SaveTo_Impl
(
     SfxMedium &rMedium, // Medium, in das gespeichert werden soll
     const SfxItemSet* pSet
)

/*  [Beschreibung]

    Schreibt den aktuellen Inhalt in das Medium rMedium.
    Ist das Zielmedium kein Storage, so wird ueber ein temporaeres
    Medium gespeichert, sonst direkt, da das Medium transacted
    geschaltet ist, wenn wir es selbst geoeffnet haben und falls wir
    Server sind entweder der Container einen transacted Storage zur
    Verfuegung stellt oder selbst einen temporaeren Storage erzeugt hat.
*/

{
    ModifyBlocker_Impl aMod(this);

    const SfxFilter *pFilter = rMedium.GetFilter();
    if ( !pFilter )
    {
        // if no filter was set, use the default filter
        // this should be changed in the feature, it should be an error!
        DBG_ERROR("No filter set!");
        pFilter = GetFactory().GetFilterContainer()->GetAnyFilter( SFX_FILTER_IMPORT | SFX_FILTER_EXPORT );
        rMedium.SetFilter(pFilter);
    }

    sal_Bool bOwnSource = IsOwnStorageFormat_Impl( *pMedium );
    sal_Bool bOwnTarget = IsOwnStorageFormat_Impl( rMedium );

    //TODO/LATER: use UCB for case sensitive/insensitive file name comparison
#ifdef WNT
    if ( pMedium && ( rMedium.GetName().EqualsIgnoreCaseAscii( pMedium->GetName() ) )
#else
    // on UNIX the case must not be ignored
    if ( pMedium && ( rMedium.GetName().Equals( pMedium->GetName() ) )
#endif
      && rMedium.GetName().CompareIgnoreCaseToAscii( "private:stream", 14 ) != COMPARE_EQUAL )
    {
        // the target file is the same as original ( Save procedure )

        // before we overwrite the original file, we will make a backup if there is a demand for that
        // if the backup is not created here it will be created internally and will be removed in case of successful saving
        const sal_Bool bDoBackup = SvtSaveOptions().IsBackup();
        if ( bDoBackup )
        {
            pMedium->DoBackup_Impl();
            if ( pMedium->GetError() )
            {
                SetError( pMedium->GetErrorCode() );
                pMedium->ResetError();
            }
        }

        if ( bOwnSource && bOwnTarget )
        {
            // The active storage must be switched. The simple saving is not enough.
            // The problem is that the target medium contains target MediaDescriptor.

                // In future the switch of the persistance could be done on stream level:
                // a new wrapper service will be implemented that allows to exchange
                // persistance on the fly. So the real persistance will be set
                // to that stream only after successful commit of the storage.
                // TODO/LATER:
                // create wrapper stream based on the URL
                // create a new storage based on this stream
                // store to this new storage
                // commit the new storage
                // call saveCompleted based with this new storage ( get rid of old storage and "frees" URL )
                // commit the wrapper stream ( the stream will connect the URL only on commit, after that it will hold it )
                // if the last step is failed the stream should stay to be transacted and should be commited on any flush
                // so we can forget the stream in any way and the next storage commit will flush it
            if( ConnectTmpStorage_Impl( pMedium->GetStorage() ) )
            {
                pMedium->Close();

                // TODO/LATER: for now the medium must be closed since it can already contain streams from old medium
                //             in future those streams should not be copied in case a valid target url is provided,
                //             if the url is not provided ( means the document is based on a stream ) this code is not
                //             reachable.
                rMedium.Close();
                rMedium.GetOutputStorage();
            }
        }
        else if ( !bOwnSource && !bOwnTarget )
        {
            // the source and the target formats are alien
            // just disconnect the stream from the source format
            // so that the target medium can use it

            pMedium->CloseInStream();
            pMedium->CloseOutStream();

            rMedium.Close();
            rMedium.CreateTempFileNoCopy();
            rMedium.GetOutStream();
        }
        else if ( !bOwnSource && bOwnTarget )
        {
            // the source format is an alien one but the target
            // format is an own one so just disconnect the source
            // medium

            pMedium->CloseInStream();
            pMedium->CloseOutStream();

            rMedium.Close();
            rMedium.GetOutputStorage();
        }
        else // means if ( bOwnSource && !bOwnTarget )
        {
            // the source format is an own one but the target is
            // an alien format, just connect the source to temporary
            // storage

            if( ConnectTmpStorage_Impl( pMedium->GetStorage() ) )
            {
                pMedium->Close();

                rMedium.Close();
                rMedium.CreateTempFileNoCopy();
                rMedium.GetOutStream();
            }
        }
    }

    if ( bOwnTarget )
    {
        rMedium.GetOutputStorage();

        // If the filter is a "cross export" filter ( f.e. a filter for exporting an impress document from
        // a draw document ), the ClassId of the destination storage is different from the ClassId of this
        // document. It can be retrieved from the default filter for the desired target format
        long nFormat = rMedium.GetFilter()->GetFormat();
        SfxFilterMatcher& rMatcher = SFX_APP()->GetFilterMatcher();
        const SfxFilter *pFilt = rMatcher.GetFilter4ClipBoardId( nFormat );
        if ( pFilt )
        {
            if ( pFilt->GetServiceName() != rMedium.GetFilter()->GetServiceName() )
            {
//REMOVE            rMedium.GetStorage()->SetClass( SvFactory::GetServerName( nFormat ), nFormat, pFilt->GetTypeName() );
                datatransfer::DataFlavor aDataFlavor;
                SotExchange::GetFormatDataFlavor( nFormat, aDataFlavor );

                try
                {
                    uno::Reference< beans::XPropertySet > xProps( rMedium.GetStorage(), uno::UNO_QUERY );
                    DBG_ASSERT( xProps.is(), "The storage implementation must implement XPropertySet!" );
                    if ( !xProps.is() )
                        throw uno::RuntimeException();

                    xProps->setPropertyValue( ::rtl::OUString::createFromAscii( "MediaType" ),
                                            uno::makeAny( aDataFlavor.MimeType ) );
                }
                catch( uno::Exception& )
                {
                }
            }
        }
    }

    // TODO/LATER: error handling
    if( rMedium.GetErrorCode() || pMedium->GetErrorCode() || GetErrorCode() )
        return sal_False;

    sal_Bool bOldStat = pImp->bForbidReload;
    pImp->bForbidReload = sal_True;

    // lock user interface while saving the document
    Lock_Impl( this, sal_True );

    sal_Bool bOk = sal_False;
    // TODO/LATER: get rid of bOk

    if( bOwnTarget && !( pFilter->GetFilterFlags() & SFX_FILTER_STARONEFILTER ) )
    {
        uno::Reference< embed::XStorage > xMedStorage = rMedium.GetStorage();
        if ( !xMedStorage.is() )
        {
            // no saving without storage, unlock UI and return
            Lock_Impl( this, sal_False );
            pImp->bForbidReload = bOldStat;
            return sal_False;
        }

        // transfer password from the parameters to the storage
        String aPasswd;
        sal_Bool bPasswdProvided = sal_False;
        if ( GetPasswd_Impl( rMedium.GetItemSet(), aPasswd ) )
        {
            bPasswdProvided = sal_True;
            try {
                ::comphelper::OStorageHelper::SetCommonStoragePassword( xMedStorage, aPasswd );
                bOk = sal_True;
            }
            catch( uno::Exception& )
            {
                DBG_ERROR( "Setting of common encryption key failed!" );
                SetError( ERRCODE_IO_GENERAL );
            }
        }
        else
            bOk = sal_True;

        const SfxFilter* pFilter = rMedium.GetFilter();

        if ( bOk )
        {
            bOk = sal_False;
            if ( xMedStorage == GetStorage() )
                // usual save procedure
                bOk = Save();
            else
                // save to target
                bOk = SaveAsOwnFormat( rMedium );
        }

        if ( GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
        {
            // store the thumbnail representation image
            // TODO: handle the case when document is encrypted and/or signed
            if ( !GenerateAndStoreThumbnail( bPasswdProvided,
                                            sal_False,
                                            pFilter->IsOwnTemplateFormat(),
                                            xMedStorage ) )
            {
                // TODO: error handling
                OSL_ENSURE( sal_False, "Couldn't store thumbnail representation!" );
            }
        }

        if ( bOk )
        {
               if ( pImp->bIsSaving )
            {
                try {
                    const SfxVersionTableDtor *pList = rMedium.GetVersionList();
                    if ( pList && pList->Count() )
                    {
                        // copy the version streams
                        ::rtl::OUString aVersionsName( RTL_CONSTASCII_USTRINGPARAM( "Versions" ) );
                        uno::Reference< embed::XStorage > xNewVerStor = xMedStorage->openStorageElement(
                                                        aVersionsName,
                                                        embed::ElementModes::READWRITE );
                        uno::Reference< embed::XStorage > xOldVerStor = GetStorage()->openStorageElement(
                                                        aVersionsName,
                                                        embed::ElementModes::READ );
                        if ( !xNewVerStor.is() || !xOldVerStor.is() )
                            throw uno::RuntimeException();

                        sal_uInt32 n = 0;
                        SfxVersionInfo* pInfo = pList->GetObject(n++);
                        while( pInfo )
                        {
                            const String& rName = pInfo->aName;
                            if ( xOldVerStor->hasByName( rName ) )
                                xOldVerStor->copyElementTo( rName, xNewVerStor, rName );
                            pInfo = pList->GetObject(n++);
                        }

                        uno::Reference< embed::XTransactedObject > xTransact( xNewVerStor, uno::UNO_QUERY );
                        if ( xTransact.is() )
                            xTransact->commit();
                    }
                }
                catch( uno::Exception& )
                {
                    DBG_ERROR( "Couldn't copy versions!\n" );
                    bOk = sal_False;
                    // TODO/LATER: a specific error could be set
                }
            }

            // look for a "version" parameter
            const SfxStringItem *pVersionItem = pSet ? (const SfxStringItem*)
                SfxRequest::GetItem( pSet, SID_DOCINFO_COMMENTS, sal_False, TYPE(SfxStringItem) ) : NULL;

            if ( bOk && pVersionItem )
            {
                // store a version also
                const SfxStringItem *pAuthorItem = pSet ? (const SfxStringItem*)
                    SfxRequest::GetItem( pSet, SID_DOCINFO_AUTHOR, sal_False, TYPE(SfxStringItem) ) : NULL;

                // version comment
                SfxVersionInfo aInfo;
                aInfo.aComment = pVersionItem->GetValue();

                // version author
                String aAuthor;
                if ( pAuthorItem )
                    aAuthor = pAuthorItem->GetValue();
                else
                    // if not transferred as a parameter, get it from user settings
                    aAuthor = SvtUserOptions().GetFullName();

                // time stamp for version
                aInfo.aCreateStamp.SetName( aAuthor );

                if ( bOk )
                {
                    // add new version information into the versionlist and save the versionlist
                    // the version list must have been transferred from the "old" medium before
                    rMedium.AddVersion_Impl( aInfo );
                    rMedium.SaveVersionList_Impl( sal_True );

                    bOk = CreateVersionByName_Impl( rMedium, aInfo.aName, aPasswd );
                }
            }
               else if ( bOk && pImp->bIsSaving )
            {
                rMedium.SaveVersionList_Impl( sal_True );
            }
        }
    }
    else
    {
        // it's a "SaveAs" in an alien format
        if ( rMedium.GetFilter() && ( rMedium.GetFilter()->GetFilterFlags() & SFX_FILTER_STARONEFILTER ) )
            bOk = ExportTo( rMedium );
        else
            bOk = ConvertTo( rMedium );

        // after saving the document, the temporary object storage must be updated
        // if the old object storage was not a temporary one, it will be updated also, because it will be used
        // as a source for copying the objects into the new temporary storage that will be created below
        // updating means: all child objects must be stored into it
        // ( same as on loading, where these objects are copied to the temporary storage )
        // but don't commit these changes, because in the case when the old object storage is not a temporary one,
        // all changes will be written into the original file !

        if( bOk )
            bOk = SaveChildren();
//REMOVE                bOk = SaveChilds() && SaveCompletedChilds( uno::Reference< embed::XStorage >() );
    }

    sal_Bool bCopyTo = sal_False;
    SfxItemSet *pMedSet = rMedium.GetItemSet();
    if( pMedSet )
    {
        SFX_ITEMSET_ARG( pMedSet, pSaveToItem, SfxBoolItem, SID_SAVETO, sal_False );
        bCopyTo =   GetCreateMode() == SFX_CREATE_MODE_EMBEDDED ||
                    pSaveToItem && pSaveToItem->GetValue();
    }

    if ( bOk )
    {
        // transfer data to its destinated location
        // the medium commits the storage or the stream it is based on
        RegisterTransfer( rMedium );
        bOk = rMedium.Commit();

        if ( bOk )
        {
            // if the target medium is an alien format and the "old" medium was an own format and the "old" medium
            // has a name, the object storage must be exchanged, because now we need a new temporary storage
            // as object storage
            if ( !bCopyTo && IsOwnStorageFormat_Impl(*pMedium) && !IsOwnStorageFormat_Impl(rMedium) )
            {
                if ( pMedium->GetName().Len() )
                {
                    // copy storage of old medium to new temporary storage and take this over
                    if( !ConnectTmpStorage_Impl( pMedium->GetStorage() ) )
                        bOk = sal_False;
                }
            }
        }
        else
        {
            // if commit is failed the objectshell must be based on the storage that contains all the changes
            // so just do not switch
            // the problem is that the old medium must be preserved but the new storage must be used
            // for it.
            if ( IsOwnStorageFormat_Impl(rMedium) )
                rMedium.MoveStorageTo_Impl( pMedium );
            else
                rMedium.MoveTempTo_Impl( pMedium );
        }
    }

    // unlock user interface
    Lock_Impl( this, sal_False );
    pImp->bForbidReload = bOldStat;

    if ( bOk )
    {
        try
        {
            ::ucb::Content aContent( rMedium.GetName(), com::sun::star::uno::Reference < XCommandEnvironment >() );
            com::sun::star::uno::Reference < XPropertySetInfo > xProps = aContent.getProperties();
            if ( xProps.is() )
            {
                ::rtl::OUString aAuthor( RTL_CONSTASCII_USTRINGPARAM("Author") );
                ::rtl::OUString aKeywords( RTL_CONSTASCII_USTRINGPARAM("Keywords") );
                ::rtl::OUString aSubject( RTL_CONSTASCII_USTRINGPARAM("Subject") );
                Any aAny;
                if ( xProps->hasPropertyByName( aAuthor ) )
                {
                    aAny <<= ::rtl::OUString( GetDocInfo().GetCreated().GetName() );
                    aContent.setPropertyValue( aAuthor, aAny );
                }
                if ( xProps->hasPropertyByName( aKeywords ) )
                {
                    aAny <<= ::rtl::OUString( GetDocInfo().GetKeywords() );
                    aContent.setPropertyValue( aKeywords, aAny );
                }
                if ( xProps->hasPropertyByName( aSubject ) )
                {
                    aAny <<= ::rtl::OUString( GetDocInfo().GetTheme() );
                    aContent.setPropertyValue( aSubject, aAny );
                }
            }
        }
        catch( Exception& )
        {
        }
    }

    return bOk;
}

//------------------------------------------------------------------------

sal_Bool SfxObjectShell::ConnectTmpStorage_Impl( const uno::Reference< embed::XStorage >& xStorage )

/*   [Beschreibung]

     Arbeitet die Applikation auf einem temporaeren Storage,
     so darf der temporaere Storage nicht aus dem SaveCompleted
     genommen werden. Daher wird in diesem Fall schon hier an
     den neuen Storage connected. SaveCompleted tut dann nichts.

     */

{
    sal_Bool bResult = sal_False;

    if ( xStorage.is() )
    {
        try
        {
            uno::Reference< embed::XStorage > xTmpStorage = ::comphelper::OStorageHelper::GetTemporaryStorage();

            DBG_ASSERT( xTmpStorage.is(), "If a storage can not be created an exception must be thrown!\n" );
            if ( !xTmpStorage.is() )
                throw uno::RuntimeException();

            // TODO/LATER: may be it should be done in SwitchPersistence also
            // TODO/LATER: find faster way to copy storage; perhaps sharing with backup?!
            xStorage->copyToStorage( xTmpStorage );
            //CopyStoragesOfUnknownMediaType( xStorage, xTmpStorage );
            bResult = SaveCompleted( xTmpStorage );

            SfxDialogLibraryContainer* pDialogCont = pImp->pDialogLibContainer;
            if( pDialogCont )
                pDialogCont->setStorage( xTmpStorage );

            SfxScriptLibraryContainer* pBasicCont = pImp->pBasicLibContainer;
            if( pBasicCont )
                pBasicCont->setStorage( xTmpStorage );

            bResult = sal_True;
        }
        catch( uno::Exception& )
        {
            // TODO/LATER: may need error code setting based on exception
            SetError( ERRCODE_IO_GENERAL );
        }
    }

    return bResult;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::DoSaveObjectAs( SfxMedium& rMedium, BOOL bCommit )
{
    sal_Bool bOk = sal_False;
    {
        ModifyBlocker_Impl aBlock( this );

        uno::Reference < embed::XStorage > xNewStor = rMedium.GetStorage();
        if ( !xNewStor.is() )
            return sal_False;

        uno::Reference < beans::XPropertySet > xPropSet( xNewStor, uno::UNO_QUERY );
        if ( xPropSet.is() )
        {
            Any a = xPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "MediaType" ) ) );
            ::rtl::OUString aMediaType;
            if ( !(a>>=aMediaType) || !aMediaType.getLength() )
            {
                OSL_ENSURE( sal_False, "The mediatype must be set already!\n" );
                SetupStorage( xNewStor, SOFFICE_FILEFORMAT_CURRENT );
            }

            pImp->bIsSaving = sal_False;
            bOk = SaveAsOwnFormat( rMedium );

            if ( bCommit )
            {
                try {
                    uno::Reference< embed::XTransactedObject > xTransact( xNewStor, uno::UNO_QUERY_THROW );
                    xTransact->commit();
                }
                catch( uno::Exception& )
                {
                    DBG_ERROR( "The strotage was not commited on DoSaveAs!\n" );
                }
            }
        }
    }

    return bOk;
}

//-------------------------------------------------------------------------
// TODO/LATER: may be the call must be removed completelly
sal_Bool SfxObjectShell::DoSaveAs( SfxMedium &rMedium )
{
    // hier kommen nur Root-Storages rein, die via Temp-File gespeichert werden
    rMedium.CreateTempFileNoCopy();
    SetError(rMedium.GetErrorCode());
    if ( GetError() )
        return sal_False;

    sal_Bool bRet = SaveTo_Impl( rMedium, NULL );
    if ( !bRet )
        SetError(rMedium.GetErrorCode());
    return bRet;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::DoSaveCompleted( SfxMedium* pNewMed )
{
    sal_Bool bOk = sal_True;
    sal_Bool bMedChanged = pNewMed && pNewMed!=pMedium;
/*  sal_Bool bCreatedTempStor = pNewMed && pMedium &&
        IsOwnStorageFormat_Impl(*pMedium) &&
        !IsOwnStorageFormat_Impl(*pNewMed) &&
        pMedium->GetName().Len();
*/
    DBG_ASSERT( !pNewMed || pNewMed->GetError() == ERRCODE_NONE, "DoSaveCompleted: Medium has error!" );

    // delete Medium (and Storage!) after all notifications
    SfxMedium* pOld = pMedium;
    if ( bMedChanged )
    {
        pMedium = pNewMed;
        pMedium->CanDisposeStorage_Impl( sal_True );
    }

    const SfxFilter *pFilter = pMedium ? pMedium->GetFilter() : 0;
    if ( pNewMed )
    {
        if( bMedChanged )
        {
            if( pNewMed->GetName().Len() )
                bHasName = sal_True;
            Broadcast( SfxSimpleHint(SFX_HINT_NAMECHANGED) );
        }

        uno::Reference< embed::XStorage > xStorage;
        if ( !pFilter || IsOwnStorageFormat_Impl( *pMedium ) )
        {
            uno::Reference < embed::XStorage > xOld = GetStorage();
            xStorage = pMedium->GetStorage();
            bOk = SaveCompleted( xStorage );
            if ( bOk && (!pOld || !pOld->HasStorage_Impl() || xOld != pOld->GetStorage() ) )
            {
                // old own storage was not controlled by old Medium -> dispose it
                try {
                    xOld->dispose();
                } catch( uno::Exception& )
                {
                    // the storage is disposed already
                    // can happen during reload scenario when the medium has disposed it during the closing
                    // will be fixed in one of the next milestones
                }
            }
        }
        else
        {
//REMOVE                if( pFilter->UsesStorage() )
//REMOVE                    pMedium->GetStorage();
//REMOVE                else if( pMedium->GetOpenMode() & STREAM_WRITE )
            if( pMedium->GetOpenMode() & STREAM_WRITE )
                pMedium->GetInStream();
            xStorage = GetStorage();
        }

        // TODO/LATER: may be this code will be replaced, but not sure
        // Set storage in document library containers
        SfxDialogLibraryContainer* pDialogCont = pImp->pDialogLibContainer;
        if( pDialogCont )
            pDialogCont->setStorage( xStorage );

        SfxScriptLibraryContainer* pBasicCont = pImp->pBasicLibContainer;
        if( pBasicCont )
            pBasicCont->setStorage( xStorage );
    }
    else
    {
        if( pMedium )
        {
            const SfxFilter* pFilter = pMedium->GetFilter();
            if( pFilter && !IsOwnStorageFormat_Impl( *pMedium ) && (pMedium->GetOpenMode() & STREAM_WRITE ))
            {
                pMedium->ReOpen();
                bOk = SaveCompletedChildren( sal_False );
            }
            else
                bOk = SaveCompleted( NULL );
        }
        // entweder Save oder ConvertTo
        else
            bOk = SaveCompleted( NULL );
    }

    if ( bOk && pNewMed )
    {
        if( bMedChanged )
        {
            delete pOld;
            // Titel neu setzen
            if ( pNewMed->GetName().Len() && SFX_CREATE_MODE_EMBEDDED != eCreateMode )
                InvalidateName();
            SetModified(sal_False); // nur bei gesetztem Medium zur"ucksetzen
            Broadcast( SfxSimpleHint(SFX_HINT_MODECHANGED) );
        }
    }

    return bOk;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::ConvertFrom
(
    SfxMedium&  rMedium     /*  <SfxMedium>, welches die Quell-Datei beschreibt
                                (z.B. Dateiname, <SfxFilter>, Open-Modi etc.) */
)

/*  [Beschreibung]

    Diese Methode wird zum Laden von Dokumenten "uber alle Filter gerufen,
    die nicht SFX_FILTER_OWN sind oder f"ur die kein Clipboard-Format
    registriert wurde (also kein Storage-Format benutzen). Mit anderen Worten:
    mit dieser Methode wird importiert.

    Das hier zu "offende File sollte "uber 'rMedium' ge"offnet werden,
    um die richtigen Open-Modi zu gew"ahrleisten. Insbesondere wenn das
    Format beibehalten wird (nur m"oglich bei SFX_FILTER_SIMULATE oder
    SFX_FILTER_ONW) mu\s die Datei STREAM_SHARE_DENYWRITE ge"offnet werden.


    [R"uckgabewert]

    sal_Bool                sal_True
                        Das Dokument konnte geladen werden.

                        sal_False
                        Das Dokument konnte nicht geladen werden, ein
                        Fehlercode ist mit <SvMedium::GetError()const> zu
                        erhalten.


    [Beispiel]

    sal_Bool DocSh::ConvertFrom( SfxMedium &rMedium )
    {
        SvStreamRef xStream = rMedium.GetInStream();
        if( xStream.is() )
        {
            xStream->SetBufferSize(4096);
            *xStream >> ...;

            // NICHT 'rMedium.CloseInStream()' rufen! File gelockt halten!
            return SVSTREAM_OK == rMedium.GetError();
        }

        return sal_False;
    }


    [Querverweise]

    <SfxObjectShell::ConvertTo(SfxMedium&)>
    <SFX_FILTER_REGISTRATION>
*/
{
    return sal_False;
}

sal_Bool SfxObjectShell::ImportFrom( SfxMedium& rMedium )
{
    ::rtl::OUString aTypeName( rMedium.GetFilter()->GetTypeName() );
    ::rtl::OUString aFilterName( rMedium.GetFilter()->GetFilterName() );

    uno::Reference< lang::XMultiServiceFactory >  xMan = ::comphelper::getProcessServiceFactory();
    uno::Reference < lang::XMultiServiceFactory > xFilterFact (
                xMan->createInstance( DEFINE_CONST_UNICODE( "com.sun.star.document.FilterFactory" ) ), uno::UNO_QUERY );

    uno::Sequence < beans::PropertyValue > aProps;
    uno::Reference < container::XNameAccess > xFilters ( xFilterFact, uno::UNO_QUERY );
    if ( xFilters->hasByName( aFilterName ) )
    {
        xFilters->getByName( aFilterName ) >>= aProps;
        rMedium.GetItemSet()->Put( SfxStringItem( SID_FILTER_NAME, aFilterName ) );
    }

    ::rtl::OUString aFilterImplName;
    sal_Int32 nFilterProps = aProps.getLength();
    for ( sal_Int32 nFilterProp = 0; nFilterProp<nFilterProps; nFilterProp++ )
    {
        const beans::PropertyValue& rFilterProp = aProps[nFilterProp];
        if ( rFilterProp.Name.compareToAscii("FilterService") == COMPARE_EQUAL )
        {
            rFilterProp.Value >>= aFilterImplName;
            break;
        }
    }

    uno::Reference< document::XFilter > xLoader;
    if ( aFilterImplName.getLength() )
    {
        try{
        xLoader = uno::Reference< document::XFilter >
            ( xFilterFact->createInstanceWithArguments( aFilterName, uno::Sequence < uno::Any >() ), uno::UNO_QUERY );
        }catch(const uno::Exception&)
            { xLoader.clear(); }
    }
    if ( xLoader.is() )
    {
        uno::Reference< lang::XComponent >  xComp( GetModel(), uno::UNO_QUERY );
        uno::Reference< document::XImporter > xImporter( xLoader, uno::UNO_QUERY );
        xImporter->setTargetDocument( xComp );

        uno::Sequence < beans::PropertyValue > lDescriptor;
        rMedium.GetItemSet()->Put( SfxStringItem( SID_FILE_NAME, rMedium.GetName() ) );
        TransformItems( SID_OPENDOC, *rMedium.GetItemSet(), lDescriptor );

        com::sun::star::uno::Sequence < com::sun::star::beans::PropertyValue > aArgs ( lDescriptor.getLength() + 1);
        com::sun::star::beans::PropertyValue * pNewValue = aArgs.getArray();
        const com::sun::star::beans::PropertyValue * pOldValue = lDescriptor.getConstArray();
        const OUString sInputStream ( RTL_CONSTASCII_USTRINGPARAM ( "InputStream" ) );

        sal_Bool bHasInputStream = sal_False;
        sal_Int32 i;
        sal_Int32 nEnd = lDescriptor.getLength();

        for ( i = 0; i < nEnd; i++ )
        {
            pNewValue[i] = pOldValue[i];
            if ( pOldValue [i].Name == sInputStream )
                bHasInputStream = sal_True;
        }
        if ( !bHasInputStream )
        {
            pNewValue[i].Name = sInputStream;
            pNewValue[i].Value <<= com::sun::star::uno::Reference < com::sun::star::io::XInputStream > ( new utl::OSeekableInputStreamWrapper ( *rMedium.GetInStream() ) );
        }
        else
            aArgs.realloc ( i );

        return xLoader->filter( aArgs );
    }

    return sal_False;
}

sal_Bool SfxObjectShell::ExportTo( SfxMedium& rMedium )
{
    ::rtl::OUString aTypeName( rMedium.GetFilter()->GetTypeName() );
    ::rtl::OUString aFilterName( rMedium.GetFilter()->GetFilterName() );
    uno::Reference< document::XExporter > xExporter;

    {
        uno::Reference< lang::XMultiServiceFactory >  xMan = ::comphelper::getProcessServiceFactory();
        uno::Reference < lang::XMultiServiceFactory > xFilterFact (
                xMan->createInstance( DEFINE_CONST_UNICODE( "com.sun.star.document.FilterFactory" ) ), uno::UNO_QUERY );

        uno::Sequence < beans::PropertyValue > aProps;
        uno::Reference < container::XNameAccess > xFilters ( xFilterFact, uno::UNO_QUERY );
        if ( xFilters->hasByName( aFilterName ) )
            xFilters->getByName( aFilterName ) >>= aProps;

        ::rtl::OUString aFilterImplName;
        sal_Int32 nFilterProps = aProps.getLength();
        for ( sal_Int32 nFilterProp = 0; nFilterProp<nFilterProps; nFilterProp++ )
        {
            const beans::PropertyValue& rFilterProp = aProps[nFilterProp];
            if ( rFilterProp.Name.compareToAscii("FilterService") == COMPARE_EQUAL )
            {
                rFilterProp.Value >>= aFilterImplName;
                break;
            }
        }

        if ( aFilterImplName.getLength() )
        {
            try{
            xExporter = uno::Reference< document::XExporter >
                ( xFilterFact->createInstanceWithArguments( aFilterName, uno::Sequence < uno::Any >() ), uno::UNO_QUERY );
            }catch(const uno::Exception&)
                { xExporter.clear(); }
        }
    }

    if ( xExporter.is() )
    {
        uno::Reference< lang::XComponent >  xComp( GetModel(), uno::UNO_QUERY );
        uno::Reference< document::XFilter > xFilter( xExporter, uno::UNO_QUERY );
        xExporter->setSourceDocument( xComp );

        com::sun::star::uno::Sequence < com::sun::star::beans::PropertyValue > aOldArgs;
        SfxItemSet* pItems = rMedium.GetItemSet();
        TransformItems( SID_SAVEASDOC, *pItems, aOldArgs );

        const com::sun::star::beans::PropertyValue * pOldValue = aOldArgs.getConstArray();
        com::sun::star::uno::Sequence < com::sun::star::beans::PropertyValue > aArgs ( aOldArgs.getLength() + 1 );
        com::sun::star::beans::PropertyValue * pNewValue = aArgs.getArray();

        // put in the REAL file name, and copy all PropertyValues
        const OUString sOutputStream ( RTL_CONSTASCII_USTRINGPARAM ( "OutputStream" ) );
        BOOL bHasStream = FALSE;
        sal_Int32 i;
        sal_Int32 nEnd = aOldArgs.getLength();

        for ( i = 0; i < nEnd; i++ )
        {
            pNewValue[i] = pOldValue[i];
            if ( pOldValue[i].Name.equalsAsciiL ( RTL_CONSTASCII_STRINGPARAM ( "FileName" ) ) )
                pNewValue[i].Value <<= OUString ( rMedium.GetName() );
            if ( pOldValue[i].Name == sOutputStream )
                bHasStream = sal_True;
        }

        if ( !bHasStream )
        {
            pNewValue[i].Name = sOutputStream;
            pNewValue[i].Value <<= com::sun::star::uno::Reference < com::sun::star::io::XOutputStream > ( new utl::OOutputStreamWrapper ( *rMedium.GetOutStream() ) );
        }
        else
            aArgs.realloc ( i );

        return xFilter->filter( aArgs );
    }

    return sal_False;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::ConvertTo
(
    SfxMedium&  rMedium     /*  <SfxMedium>, welches die Ziel-Datei beschreibt
                                (z.B. Dateiname, <SfxFilter>, Open-Modi etc.) */
)

/*  [Beschreibung]

    Diese Methode wird zum Speichern von Dokumenten "uber alle Filter gerufen,
    die nicht SFX_FILTER_OWN sind oder f"ur die kein Clipboard-Format
    registriert wurde (also kein Storage-Format benutzen). Mit anderen Worten:
    mit dieser Methode wird exportiert.

    Das hier zu "offende File sollte "uber 'rMedium' ge"offnet werden,
    um die richtigen Open-Modi zu gew"ahrleisten. Insbesondere wenn das
    Format beibehalten wird (nur m"oglich bei SFX_FILTER_SIMULATE oder
    SFX_FILTER_ONW) mu\s die Datei auch nach dem Speichern im Modus
    STREAM_SHARE_DENYWRITE ge"offnet bleiben.


    [R"uckgabewert]

    sal_Bool                sal_True
                        Das Dokument konnte gespeichert werden.

                        sal_False
                        Das Dokument konnte nicht gespeichert werden, ein
                        Fehlercode ist mit <SvMedium::GetError()const> zu
                        erhalten.


    [Beispiel]

    sal_Bool DocSh::ConvertTo( SfxMedium &rMedium )
    {
        SvStreamRef xStream = rMedium.GetOutStream();
        if ( xStream.is() )
        {
            xStream->SetBufferSize(4096);
            *xStream << ...;

            rMedium.CloseOutStream(); // "offnet automatisch wieder den InStream
            return SVSTREAM_OK == rMedium.GetError();
        }
        return sal_False ;
    }


    [Querverweise]

    <SfxObjectShell::ConvertFrom(SfxMedium&)>
    <SFX_FILTER_REGISTRATION>
*/

{
    return sal_False;
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetEAs_Impl( SfxMedium &rMedium )
{
    //!! wenn OV eine entsprechende Funktionalitaet zur Verfuegung stellt,
    // besser auf der geoeffneten Datei arbeiten
    SvEaMgr *pMgr = rMedium.GetEaMgr();
    SvEaMgr *pOld = GetMedium()->GetEaMgr();
    if ( !pMgr )
        return;

    if ( pOld )
        pOld->Clone( *pMgr );

    String aBuffer;
    pMgr->SetComment( GetDocInfo().GetComment() );

    pMgr->SetFileType( rMedium.GetFilter()->GetTypeName().GetToken( 0, ';' ) );
    if ( SvEaMgr::GetAppCreator(aBuffer) )
        pMgr->SetCreator(aBuffer);

    if ( rMedium.GetLongName().Len() )
        pMgr->SetLongName(rMedium.GetLongName());
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::DoSave_Impl( const SfxItemSet* pArgs )
{
    SfxMedium *pMedium = GetMedium();
    const SfxFilter* pFilter = pMedium->GetFilter();

    // copy the original itemset, but remove the "version" item, because pMediumTmp
    // is a new medium "from scratch", so no version should be stored into it
    SfxItemSet* pSet = pMedium->GetItemSet() ? new SfxAllItemSet(*pMedium->GetItemSet()): 0;
    pSet->ClearItem( SID_VERSION );

    // create a medium as a copy; this medium is only for writingm, because it uses the same name as the original one
    // writing is done through a copy, that will be transferred to the target ( of course after calling HandsOff )
    SfxMedium* pMediumTmp = new SfxMedium( pMedium->GetName(), pMedium->GetOpenMode(), pMedium->IsDirect(), pFilter, pSet );
    pMediumTmp->SetLongName( pMedium->GetLongName() );
//    pMediumTmp->CreateTempFileNoCopy();
    if ( pMediumTmp->GetErrorCode() != ERRCODE_NONE )
    {
        SetError( pMediumTmp->GetError() );
        delete pMediumTmp;
        return sal_False;
    }

    // copy version list from "old" medium to target medium, so it can be used on saving
    pMediumTmp->TransferVersionList_Impl( *pMedium );
/*
    if ( pFilter && ( pFilter->GetFilterFlags() & SFX_FILTER_PACKED ) )
        SetError( GetMedium()->Unpack_Impl( pMedium->GetPhysicalName() ) );
*/

    // an interaction handler here can aquire only in case of GUI Saving
    // and should be removed after the saving is done
    com::sun::star::uno::Reference< XInteractionHandler > xInteract;
    SFX_ITEMSET_ARG( pArgs, pxInteractionItem, SfxUnoAnyItem, SID_INTERACTIONHANDLER, sal_False );
    if ( pxInteractionItem && ( pxInteractionItem->GetValue() >>= xInteract ) && xInteract.is() )
        pMediumTmp->GetItemSet()->Put( SfxUnoAnyItem( SID_INTERACTIONHANDLER, makeAny( xInteract ) ) );

    sal_Bool bSaved = sal_False;
    if( !GetError() && SaveTo_Impl( *pMediumTmp, pArgs ) )
    {
        bSaved = sal_True;

        if( pMediumTmp->GetItemSet() )
        {
            pMediumTmp->GetItemSet()->ClearItem( SID_INTERACTIONHANDLER );
            pMediumTmp->GetItemSet()->ClearItem( SID_PROGRESS_STATUSBAR_CONTROL );
        }

        SetError(pMediumTmp->GetErrorCode());

//REMOVE            if ( !IsHandsOff() )
//REMOVE                DoHandsOff();
//REMOVE            pMediumTmp->Close();

        sal_Bool bOpen = DoSaveCompleted( pMediumTmp );
        DBG_ASSERT(bOpen,"Fehlerbehandlung fuer DoSaveCompleted nicht implementiert");
    }
    else
    {
        // transfer error code from medium to objectshell
        SetError( pMediumTmp->GetError() );

        // reconnect to object storage
//REMOVE            if ( IsHandsOff() )
//REMOVE            {
//REMOVE                if ( !DoSaveCompleted( pMedium ) )
//REMOVE                    DBG_ERROR("Case not handled - no way to get a storage!");
//REMOVE            }
//REMOVE            else
            DoSaveCompleted( 0 );

        if( pMedium->GetItemSet() )
        {
            pMedium->GetItemSet()->ClearItem( SID_INTERACTIONHANDLER );
            pMedium->GetItemSet()->ClearItem( SID_PROGRESS_STATUSBAR_CONTROL );
        }

        delete pMediumTmp;
    }

    SetModified( !bSaved );
    return bSaved;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::Save_Impl( const SfxItemSet* pSet )
{
    if ( IsReadOnly() )
    {
        SetError( ERRCODE_SFX_DOCUMENTREADONLY );
        return sal_False;
    }

    DBG_CHKTHIS(SfxObjectShell, 0);
    SfxApplication *pSfxApp = SFX_APP();

    pImp->bIsSaving = sal_True;
    sal_Bool bSaved = FALSE;
    SFX_ITEMSET_ARG( GetMedium()->GetItemSet(), pSalvageItem, SfxStringItem, SID_DOC_SALVAGE, sal_False);
    if ( pSalvageItem )
    {
        SFX_ITEMSET_ARG( GetMedium()->GetItemSet(), pFilterItem, SfxStringItem, SID_FILTER_NAME, sal_False);
        String aFilterName;
        const SfxFilter *pFilter = NULL;
        if ( pFilterItem )
            pFilter = SfxFilterMatcher( String::CreateFromAscii( GetFactory().GetShortName()) ).GetFilter4FilterName( aFilterName );

        SfxMedium *pMed = new SfxMedium(
            pSalvageItem->GetValue(), STREAM_READWRITE | STREAM_SHARE_DENYWRITE | STREAM_TRUNC, sal_False, pFilter );

        SFX_ITEMSET_ARG( GetMedium()->GetItemSet(), pPasswordItem, SfxStringItem, SID_PASSWORD, sal_False );
        if ( pPasswordItem )
            pMed->GetItemSet()->Put( *pPasswordItem );

        bSaved = DoSaveAs( *pMed );
        if ( bSaved )
            bSaved = DoSaveCompleted( pMed );
        else
            delete pMed;
    }
    else
        bSaved = DoSave_Impl( pSet );
    return bSaved;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::CommonSaveAs_Impl
(
    const INetURLObject&   aURL,
    const String&   aFilterName,
    SfxItemSet*     aParams
)
{
    if( aURL.HasError() )
    {
        SetError( ERRCODE_IO_INVALIDPARAMETER );
        return sal_False;
    }

    // gibt es schon ein Doc mit dem Namen?
    SfxObjectShell* pDoc = 0;
    for ( SfxObjectShell* pTmp = SfxObjectShell::GetFirst();
            pTmp && !pDoc;
            pTmp = SfxObjectShell::GetNext(*pTmp) )
    {
        if( ( pTmp != this ) && pTmp->GetMedium() )
        {
            INetURLObject aCompare( pTmp->GetMedium()->GetName() );
            if ( aCompare == aURL )
                pDoc = pTmp;
        }
    }
    if ( pDoc )
    {
        // dann Fehlermeldeung: "schon offen"
        SetError(ERRCODE_SFX_ALREADYOPEN);
        return sal_False;
    }

    DBG_ASSERT( aURL.GetProtocol() != INET_PROT_NOT_VALID, "Illegal URL!" );
    DBG_ASSERT( aParams->Count() != 0, "fehlerhafte Parameter");

    SFX_ITEMSET_ARG( aParams, pSaveToItem, SfxBoolItem, SID_SAVETO, sal_False );
    sal_Bool bSaveTo = pSaveToItem ? pSaveToItem->GetValue() : sal_False;

    const SfxFilter* pFilter = GetFactory().GetFilterContainer()->GetFilter4FilterName( aFilterName );
    if ( !pFilter
        || !pFilter->CanExport()
        || !bSaveTo && !pFilter->CanImport() )
    {
        SetError( ERRCODE_IO_INVALIDPARAMETER );
        return sal_False;
    }

    pImp->bPasswd = aParams && SFX_ITEM_SET == aParams->GetItemState(SID_PASSWORD);

    SfxMedium *pActMed = GetMedium();
    const INetURLObject aActName(pActMed->GetName());

    BOOL bWasReadonly = IsReadOnly();

    if ( aURL == aActName && aURL != INetURLObject( OUString::createFromAscii( "private:stream" ) )
        && IsReadOnly() )
    {
        SetError(ERRCODE_SFX_DOCUMENTREADONLY);
        return sal_False;
    }

//REMOVE        if ( aURL == aActName && aURL != INetURLObject( OUString::createFromAscii( "private:stream" ) ) )
//REMOVE        {
//REMOVE            if ( IsReadOnly() )
//REMOVE            {
//REMOVE                SetError(ERRCODE_SFX_DOCUMENTREADONLY);
//REMOVE                return sal_False;
//REMOVE            }
//REMOVE
//REMOVE            // gleicher Filter? -> Save()
//REMOVE            const SfxFilter *pFilter = pActMed->GetFilter();
//REMOVE            if ( pFilter && pFilter->GetFilterName() == aFilterName )
//REMOVE            {
//REMOVE                pImp->bIsSaving=sal_False;
//REMOVE                if ( aParams )
//REMOVE                {
//REMOVE                    SfxItemSet* pSet = pMedium->GetItemSet();
//REMOVE                    pSet->ClearItem( SID_PASSWORD );
//REMOVE                    pSet->Put( *aParams );
//REMOVE                }
//REMOVE
//REMOVE                SFX_APP()->NotifyEvent(SfxEventHint(SFX_EVENT_SAVEDOC,this));
//REMOVE                BOOL bRet = DoSave_Impl();
//REMOVE                SFX_APP()->NotifyEvent(SfxEventHint(SFX_EVENT_SAVEDOCDONE,this));
//REMOVE                return bRet;
//REMOVE            }
//REMOVE        }

    // this notification should be already sent by caller in sfxbasemodel
    // SFX_APP()->NotifyEvent(SfxEventHint( bSaveTo? SFX_EVENT_SAVETODOC : SFX_EVENT_SAVEASDOC,this));

    if( SFX_ITEM_SET != aParams->GetItemState(SID_UNPACK) && SvtSaveOptions().IsSaveUnpacked() )
        aParams->Put( SfxBoolItem( SID_UNPACK, sal_False ) );

    if ( PreDoSaveAs_Impl(aURL.GetMainURL( INetURLObject::NO_DECODE ),aFilterName,aParams))
    {
        pImp->bWaitingForPicklist = sal_True;

        // Daten am Medium updaten
        SfxItemSet *pSet = GetMedium()->GetItemSet();
        pSet->ClearItem( SID_INTERACTIONHANDLER );
        pSet->ClearItem( SID_PROGRESS_STATUSBAR_CONTROL );

        if ( !bSaveTo )
        {
            pSet->ClearItem( SID_REFERER );
            pSet->ClearItem( SID_POSTDATA );
            pSet->ClearItem( SID_TEMPLATE );
            pSet->ClearItem( SID_DOC_READONLY );
            pSet->ClearItem( SID_CONTENTTYPE );
            pSet->ClearItem( SID_CHARSET );
            pSet->ClearItem( SID_FILTER_NAME );
            pSet->ClearItem( SID_OPTIONS );
            //pSet->ClearItem( SID_FILE_FILTEROPTIONS );
            pSet->ClearItem( SID_VERSION );
            //pSet->ClearItem( SID_USE_FILTEROPTIONS );
            pSet->ClearItem( SID_EDITDOC );
            pSet->ClearItem( SID_OVERWRITE );

            SFX_ITEMSET_GET( (*aParams), pFilterItem, SfxStringItem, SID_FILTER_NAME, sal_False );
            if ( pFilterItem )
                pSet->Put( *pFilterItem );

            SFX_ITEMSET_GET( (*aParams), pOptionsItem, SfxStringItem, SID_OPTIONS, sal_False );
            if ( pOptionsItem )
                pSet->Put( *pOptionsItem );

            SFX_ITEMSET_GET( (*aParams), pFilterOptItem, SfxStringItem, SID_FILE_FILTEROPTIONS, sal_False );
            if ( pFilterOptItem )
                pSet->Put( *pFilterOptItem );
        }

        if ( bWasReadonly && !bSaveTo )
            Broadcast( SfxSimpleHint(SFX_HINT_MODECHANGED) );

        return sal_True;
    }
    else
        return sal_False;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::PreDoSaveAs_Impl
(
    const String&   rFileName,
    const String&   aFilterName,
    SfxItemSet*     pParams
)
{
    // copy all items stored in the itemset of the current medium
    SfxAllItemSet* pMergedParams = new SfxAllItemSet( *pMedium->GetItemSet() );

    // in "SaveAs" title and password will be cleared ( maybe the new itemset contains new values, otherwise they will be empty )
    pMergedParams->ClearItem( SID_PASSWORD );
    pMergedParams->ClearItem( SID_DOCINFO_TITLE );

    pMergedParams->ClearItem( SID_INPUTSTREAM );
    pMergedParams->ClearItem( SID_STREAM );
    pMergedParams->ClearItem( SID_CONTENT );
    pMergedParams->ClearItem( SID_DOC_READONLY );

    pMergedParams->ClearItem( SID_REPAIRPACKAGE );

    // "SaveAs" will never store any version information - it's a complete new file !
    pMergedParams->ClearItem( SID_VERSION );

    // merge the new parameters into the copy
    // all values present in both itemsets will be overwritten by the new parameters
    if( pParams )
        pMergedParams->Put( *pParams );
    //DELETEZ( pParams );

#ifdef DBG_UTIL
    if ( pMergedParams->GetItemState( SID_DOC_SALVAGE) >= SFX_ITEM_SET )
        DBG_ERROR("Salvage item present in Itemset, check the parameters!");
#endif

    // should be unneccessary - too hot to handle!
    pMergedParams->ClearItem( SID_DOC_SALVAGE );

    // take over the new merged itemset
    pParams = pMergedParams;

    // create a medium for the target URL
    SfxMedium *pNewFile = new SfxMedium( rFileName, STREAM_READWRITE | STREAM_SHARE_DENYWRITE | STREAM_TRUNC, sal_False, 0, pParams );

    // set filter; if no filter is given, take the default filter of the factory
    if ( aFilterName.Len() )
        pNewFile->SetFilter( GetFactory().GetFilterContainer()->GetFilter4FilterName( aFilterName ) );
    else
        pNewFile->SetFilter( GetFactory().GetFilterContainer()->GetAnyFilter( SFX_FILTER_IMPORT | SFX_FILTER_EXPORT ) );

//REMOVE        // saving is alway done using a temporary file
//REMOVE        pNewFile->CreateTempFileNoCopy();
    if ( pNewFile->GetErrorCode() != ERRCODE_NONE )
    {
        // creating temporary file failed ( f.e. floppy disk not inserted! )
        SetError( pNewFile->GetError() );
        delete pNewFile;
        return sal_False;
    }

    // check if a "SaveTo" is wanted, no "SaveAs"
    SFX_ITEMSET_ARG( pParams, pSaveToItem, SfxBoolItem, SID_SAVETO, sal_False );
    sal_Bool bCopyTo = GetCreateMode() == SFX_CREATE_MODE_EMBEDDED || pSaveToItem && pSaveToItem->GetValue();

    // distinguish between "Save" and "SaveAs"
    pImp-> bIsSaving = sal_False;
/*
    if ( GetMedium()->GetFilter() && ( GetMedium()->GetFilter()->GetFilterFlags() & SFX_FILTER_PACKED ) )
    {
        SfxMedium *pMed = bCopyTo ? pMedium : pNewFile;
        pNewFile->SetError( GetMedium()->Unpack_Impl( pMed->GetPhysicalName() ) );
    }
*/
    // Save the document ( first as temporary file, then transfer to the target URL by committing the medium )
    sal_Bool bOk = sal_False;
    if ( !pNewFile->GetErrorCode() && SaveTo_Impl( *pNewFile, NULL ) )
    {
        bOk = sal_True;

        // transfer a possible error from the medium to the document
        SetError( pNewFile->GetErrorCode() );

        // notify the document that saving was done successfully
//REMOVE            if ( bCopyTo )
//REMOVE            {
//REMOVE                if ( IsHandsOff() )
//REMOVE                    bOk = DoSaveCompleted( pMedium );
//REMOVE            }
//REMOVE            else
        if ( !bCopyTo )
        {
            // Muss !!!
//REMOVE                if ( bToOwnFormat )
//REMOVE                    SetFileName( pNewFile->GetPhysicalName() );

            bOk = DoSaveCompleted( pNewFile );
        }
        else
            bOk = DoSaveCompleted(0);

        if( bOk )
        {
            if( !bCopyTo )
                SetModified( sal_False );
        }
        else
        {
            // TODO/LATER: the code below must be dead since the storage commit makes all the stuff
            //       and the DoSaveCompleted call should not be able to fail in general

            DBG_ASSERT( !bCopyTo, "Error while reconnecting to medium, can't be handled!");
            SetError( pNewFile->GetErrorCode() );

            if ( !bCopyTo )
            {
                // reconnect to the old medium
                BOOL bRet = DoSaveCompleted( pMedium );
                DBG_ASSERT( bRet, "Error in DoSaveCompleted, can't be handled!");
            }

            // TODO/LATER: disconnect the new file from the storage for the case when pure saving is done
            //       if storing has corrupted the file, probably it must be restored either here or
            //       by the storage
            DELETEZ( pNewFile );
        }

        // TODO/LATER: there is no need in the following code in case HandsOff is not used,
        //             hope we will not have to introduce it back
//REMOVE            String aPasswd;
//REMOVE            if ( IsOwnStorageFormat_Impl( *GetMedium() ) && GetPasswd_Impl( GetMedium()->GetItemSet(), aPasswd ) )
//REMOVE            {
//REMOVE                try
//REMOVE                {
//REMOVE                    // the following code must throw an exception in case of failure
//REMOVE                    ::comphelper::OStorageHelper::SetCommonStoragePassword( GetMedium->GetStorage(), aPasswd );
//REMOVE                }
//REMOVE                catch( uno::Exception& )
//REMOVE                {
//REMOVE                    // TODO: handle the error
//REMOVE                }
//REMOVE            }
    }
    else
    {
        SetError( pNewFile->GetErrorCode() );

//REMOVE            // reconnect to the old storage
//REMOVE            if ( IsHandsOff() )
//REMOVE                DoSaveCompleted( pMedium );
//REMOVE            else
        DoSaveCompleted( 0 );

        DELETEZ( pNewFile );
    }

    if( !bOk )
        SetModified( sal_True );

    if ( bCopyTo )
        DELETEZ( pNewFile );

    return bOk;
}

//------------------------------------------------------------------------

sal_Bool SfxObjectShell::LoadFrom( SfxMedium& rMedium )
{
    DBG_ERROR( "Base implementation, must not be called in general!" );
    return sal_True;
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::IsInformationLost()
{
    Sequence< PropertyValue > aProps = GetModel()->getArgs();
    ::rtl::OUString aFilterName;
    ::rtl::OUString aPreusedFilterName;
    for ( sal_Int32 nInd = 0; nInd < aProps.getLength(); nInd++ )
    {
        if ( aProps[nInd].Name.equalsAscii( "FilterName" ) )
            aProps[nInd].Value >>= aFilterName;
        else if ( aProps[nInd].Name.equalsAscii( "PreusedFilterName" ) )
            aProps[nInd].Value >>= aPreusedFilterName;
    }

    // if current filter can lead to information loss and it was used
    // for the latest store then the user should be asked to store in own format
    if ( aFilterName.getLength() && aFilterName.equals( aPreusedFilterName ) )
    {
        const SfxFilter *pFilt = GetMedium()->GetFilter();
        DBG_ASSERT( pFilt && aFilterName.equals( pFilt->GetName() ), "MediaDescriptor contains wrong filter!\n" );
        return ( pFilt && pFilt->IsAlienFormat() && !(pFilt->GetFilterFlags() & SFX_FILTER_SILENTEXPORT ) );
    }

    return sal_False;
}

//-------------------------------------------------------------------------
sal_Bool SfxObjectShell::CanReload_Impl()

/*  [Beschreibung]

    Interne Methode zum Feststellen, ob eine erneutes Laden des
    Dokuments (auch als RevertToSaved oder LastVersion bekannt)
    m"oglich ist.
*/

{
    return pMedium && HasName() && !IsInModalMode() && !pImp->bForbidReload;
}

//-------------------------------------------------------------------------

sal_uInt16 SfxObjectShell::GetHiddenInformationState( sal_uInt16 nStates )
{
    sal_uInt16 nState = 0;
    if ( nStates & HIDDENINFORMATION_DOCUMENTVERSIONS )
    {
        const SfxVersionTableDtor* pTable = GetMedium()->GetVersionList();
        if ( pTable )
            nState |= HIDDENINFORMATION_DOCUMENTVERSIONS;
    }

    return nState;
}

sal_Int16 SfxObjectShell::QueryHiddenInformation( HiddenWarningFact eFact, Window* pParent )
{
    sal_Int16 nRet = RET_YES;
    USHORT nResId = 0;
    SvtSecurityOptions::EOption eOption = static_cast< SvtSecurityOptions::EOption >( -1 );

    switch ( eFact )
    {
        case WhenSaving :
        {
            nResId = STR_HIDDENINFO_CONTINUE_SAVING;
            eOption = SvtSecurityOptions::E_DOCWARN_SAVEORSEND;
            break;
        }
        case WhenPrinting :
        {
            nResId = STR_HIDDENINFO_CONTINUE_PRINTING;
            eOption = SvtSecurityOptions::E_DOCWARN_PRINT;
            break;
        }
        case WhenSigning :
        {
            nResId = STR_HIDDENINFO_CONTINUE_SIGNING;
            eOption = SvtSecurityOptions::E_DOCWARN_SIGNING;
            break;
        }
        case WhenCreatingPDF :
        {
            nResId = STR_HIDDENINFO_CONTINUE_CREATEPDF;
            eOption = SvtSecurityOptions::E_DOCWARN_CREATEPDF;
            break;
        }
        default:
        {
            DBG_ERRORFILE( "SfxObjectShell::DetectHiddenInformation(): what fact?" );
        }
    }

    if ( eOption != -1 && SvtSecurityOptions().IsOptionSet( eOption ) )
    {
        String sMessage( SfxResId( STR_HIDDENINFO_CONTAINS ) );
        sal_uInt16 nWantedStates = HIDDENINFORMATION_RECORDEDCHANGES | HIDDENINFORMATION_NOTES;
        if ( eFact != WhenPrinting )
            nWantedStates |= HIDDENINFORMATION_DOCUMENTVERSIONS;
        sal_uInt16 nStates = GetHiddenInformationState( nWantedStates );
        bool bWarning = false;

        if ( ( nStates & HIDDENINFORMATION_RECORDEDCHANGES ) == HIDDENINFORMATION_RECORDEDCHANGES )
        {
            sMessage += String( SfxResId( STR_HIDDENINFO_RECORDCHANGES ) );
            sMessage += '\n';
            bWarning = true;
        }
        if ( ( nStates & HIDDENINFORMATION_NOTES ) == HIDDENINFORMATION_NOTES )
        {
            sMessage += String( SfxResId( STR_HIDDENINFO_NOTES ) );
            sMessage += '\n';
            bWarning = true;
        }
        if ( ( nStates & HIDDENINFORMATION_DOCUMENTVERSIONS ) == HIDDENINFORMATION_DOCUMENTVERSIONS )
        {
            sMessage += String( SfxResId( STR_HIDDENINFO_DOCVERSIONS ) );
            sMessage += '\n';
            bWarning = true;
        }

        if ( bWarning )
        {
            sMessage += '\n';
            sMessage += String( SfxResId( nResId ) );
            WarningBox aWBox( pParent, WB_YES_NO | WB_DEF_NO, sMessage );
            nRet = aWBox.Execute();
        }
    }

    return nRet;
}

sal_Bool SfxObjectShell::HasSecurityOptOpenReadOnly() const
{
    return sal_True;
}

sal_Bool SfxObjectShell::IsSecurityOptOpenReadOnly() const
{
    const SfxDocumentInfo& rDocInfo = const_cast< SfxObjectShell* >( this )->GetDocInfo();
    return rDocInfo.IsLoadReadonly();
}

void SfxObjectShell::SetSecurityOptOpenReadOnly( sal_Bool _b )
{
    SfxDocumentInfo& rDocInfo = GetDocInfo();
    if ( rDocInfo.IsLoadReadonly() != _b )
    {
        GetDocInfo().SetLoadReadonly( _b );
        SetModified( sal_True );
    }
}

sal_Bool SfxObjectShell::LoadOwnFormat( SfxMedium& rMedium )
{
    uno::Reference< embed::XStorage > xStorage = rMedium.GetStorage();
    if ( xStorage.is() )
    {
//REMOVE            if ( rMedium.GetFileVersion() )
//REMOVE                xStor->SetVersion( rMedium.GetFileVersion() );

        // Password
        SFX_ITEMSET_ARG( rMedium.GetItemSet(), pPasswdItem, SfxStringItem, SID_PASSWORD, sal_False );
        if ( pPasswdItem || ERRCODE_IO_ABORT != CheckPasswd_Impl( this, SFX_APP()->GetPool(), pMedium ) )
        {
            String aPasswd;
            if ( GetPasswd_Impl(pMedium->GetItemSet(), aPasswd) )
            {
                try
                {
                    // the following code must throw an exception in case of failure
                    ::comphelper::OStorageHelper::SetCommonStoragePassword( xStorage, aPasswd );
                }
                catch( uno::Exception& )
                {
                    // TODO/LATER: handle the error code
                }
            }

            // load document
            return Load( rMedium );
        }
        return sal_False;
    }
    else
        return sal_False;
}

sal_Bool SfxObjectShell::SaveAsOwnFormat( SfxMedium& rMedium )
{
    uno::Reference< embed::XStorage > xStorage = rMedium.GetStorage();
    if( xStorage.is() )
    {
        sal_Int32 nVersion = rMedium.GetFilter()->GetVersion();
        SetupStorage( xStorage, nVersion );
//REMOVE            xStor->SetVersion( nVersion );

        // Initialize Basic
        GetBasicManager();

        // Save dialog container
//REMOVE            if( nVersion >= 6200 )

        SfxDialogLibraryContainer* pDialogCont = pImp->pDialogLibContainer;
        if( pDialogCont )
            pDialogCont->storeLibrariesToStorage( xStorage );

        SfxScriptLibraryContainer* pBasicCont = pImp->pBasicLibContainer;
        if( pBasicCont )
            pBasicCont->storeLibrariesToStorage( xStorage );

        // TODO/LATER: replace with the new code after resync to docking1 cws???
//REMOVE            // Konfiguration schreiben
//REMOVE            if ( GetConfigManager() )
//REMOVE            {
//REMOVE                {
//REMOVE                    SotStorageRef xCfgStor = pImp->pCfgMgr->GetConfigurationStorage( xStor );
//REMOVE                    if ( pImp->pCfgMgr->StoreConfiguration( xCfgStor ) )
//REMOVE                        xCfgStor->Commit();
//REMOVE                }
//REMOVE            }

        const SfxFilter* pFilter = rMedium.GetFilter();
        return SaveAs( rMedium );
    }
    else return sal_False;
}

uno::Reference< embed::XStorage > SfxObjectShell::GetStorage()
{
    if ( !pImp->m_xDocStorage.is() )
    {
        OSL_ENSURE( pImp->m_bCreateTempStor, "The storage must exist already!\n" );
        try {
            // no notification is required the storage is set the first time
            pImp->m_xDocStorage = ::comphelper::OStorageHelper::GetTemporaryStorage();
            OSL_ENSURE( pImp->m_xDocStorage.is(), "The method must either return storage or throw an exception!" );

            SetupStorage( pImp->m_xDocStorage, SOFFICE_FILEFORMAT_CURRENT );
            pImp->m_bCreateTempStor = sal_False;
            SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_STORAGECHANGED, this ) );
        }
        catch( uno::Exception& )
        {
            // TODO/LATER: error handling?
        }
    }

    OSL_ENSURE( pImp->m_xDocStorage.is(), "The document storage must be created!" );
    return pImp->m_xDocStorage;
}

void InsertStreamIntoPicturesStorage_Impl( const uno::Reference< embed::XStorage >& xDocStor,
                                            const uno::Reference< io::XInputStream >& xInStream,
                                            const ::rtl::OUString& aStreamName )
{
    OSL_ENSURE( aStreamName.getLength() && xInStream.is() && xDocStor.is(), "Misuse of the method!\n" );

    try
    {
        uno::Reference< embed::XStorage > xPictures = xDocStor->openStorageElement(
                                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Pictures" ) ),
                                    embed::ElementModes::READWRITE );
        uno::Reference< io::XStream > xObjReplStr = xPictures->openStreamElement(
                                    aStreamName,
                                    embed::ElementModes::READWRITE | embed::ElementModes::TRUNCATE );
        uno::Reference< io::XOutputStream > xOutStream(
                                    xObjReplStr->getInputStream(), uno::UNO_QUERY_THROW );

        ::comphelper::OStorageHelper::CopyInputToOutput( xInStream, xOutStream );
        xOutStream->closeOutput();

        uno::Reference< embed::XTransactedObject > xTransact( xPictures, uno::UNO_QUERY );
        if ( xTransact.is() )
            xTransact->commit();
    }
    catch( uno::Exception& )
    {
        OSL_ENSURE( sal_False, "The pictures storage is not available!\n" );
    }
}

sal_Bool SfxObjectShell::SaveChildren()
{
    sal_Bool bResult = sal_True;

    sal_Bool bOasis = ( SotStorage::GetVersion( GetStorage() ) > SOFFICE_FILEFORMAT_60 );
    uno::Sequence < ::rtl::OUString > aNames = GetEmbeddedObjectContainer().GetObjectNames();

    for ( sal_Int32 n=0; n<aNames.getLength(); n++ )
    {
        uno::Reference < embed::XEmbeddedObject > xObj = GetEmbeddedObjectContainer().GetEmbeddedObject( aNames[n] );
        OSL_ENSURE( xObj.is(), "An empty entry in the embedded objects list!\n" );
        if ( xObj.is() )
        {
            sal_Int32 nCurState = xObj->getCurrentState();
            if ( bOasis && nCurState != embed::EmbedStates::LOADED && nCurState != embed::EmbedStates::RUNNING )
            {
                // means that the object is active
                // the image must be regenerated
                ::rtl::OUString aMediaType;

                // TODO/LATER: another aspect could be used
                uno::Reference < io::XInputStream > xStream =
                            svt::EmbeddedObjectRef::GetGraphicReplacementStream(
                                                        embed::Aspects::MSOLE_CONTENT,
                                                        xObj,
                                                        &aMediaType );
                   if ( xStream.is() )
                       GetEmbeddedObjectContainer().InsertGraphicStream( xStream, aNames[n], aMediaType );
            }

            uno::Reference< embed::XEmbedPersist > xPersist( xObj, uno::UNO_QUERY );
            if ( xPersist.is() )
            {
                try
                {
                    //TODO/LATER: only storing if changed!
                    xPersist->storeOwn();
                }
                catch( uno::Exception& )
                {
                    // TODO/LATER: error handling
                    bResult = sal_False;
                    break;
                }
            }

            if ( !bOasis )
            {
                // copy replacement images for linked objects
                try
                {
                    uno::Reference< embed::XLinkageSupport > xLink( xObj, uno::UNO_QUERY );
                    if ( xLink.is() && xLink->isLink() )
                    {
                        ::rtl::OUString aMediaType;
                        uno::Reference < io::XInputStream > xInStream =
                                GetEmbeddedObjectContainer().GetGraphicStream( xObj, &aMediaType );
                        if ( xInStream.is() )
                            InsertStreamIntoPicturesStorage_Impl( GetStorage(), xInStream, aNames[n] );
                    }
                }
                catch( uno::Exception& )
                {
                }
            }
        }
    }

    if ( bResult )
    {
        try
        {
            ::rtl::OUString aObjReplElement( RTL_CONSTASCII_USTRINGPARAM( "ObjectReplacements" ) );
            if ( !bOasis && GetStorage()->hasByName( aObjReplElement ) && GetStorage()->isStorageElement( aObjReplElement ) )
            {
                ClearEmbeddedObjects();
                GetStorage()->removeElement( aObjReplElement );
            }
        }
        catch( uno::Exception& )
        {
            // TODO/LATER: error handling
            bResult = sal_False;
        }
    }

    return bResult;
}

sal_Bool SfxObjectShell::SaveAsChildren( SfxMedium& rMedium )
{
    sal_Bool bResult = sal_True;

    uno::Reference < embed::XStorage > xStorage = rMedium.GetStorage();
    if ( !xStorage.is() )
        return sal_False;

    if ( xStorage == GetStorage() )
        return SaveChildren();

    sal_Bool bOasis = sal_True;
    try
    {
        bOasis = ( SotStorage::GetVersion( xStorage ) > SOFFICE_FILEFORMAT_60 );

        comphelper::EmbeddedObjectContainer aCnt( xStorage );
        uno::Sequence < ::rtl::OUString > aNames = GetEmbeddedObjectContainer().GetObjectNames();
        for ( sal_Int32 n=0; n<aNames.getLength(); n++ )
        {
            uno::Reference < embed::XEmbeddedObject > xObj = GetEmbeddedObjectContainer().GetEmbeddedObject( aNames[n] );
            OSL_ENSURE( xObj.is(), "An empty entry in the embedded objects list!\n" );
            if ( xObj.is() )
            {
                sal_Bool bSwitchBackToLoaded = sal_False;
                uno::Reference< embed::XLinkageSupport > xLink( xObj, uno::UNO_QUERY );
                if ( bOasis || xLink.is() && xLink->isLink() )
                {
                    uno::Reference < io::XInputStream > xStream;
                    ::rtl::OUString aMediaType;

                    sal_Int32 nCurState = xObj->getCurrentState();
                    if ( nCurState == embed::EmbedStates::LOADED || nCurState == embed::EmbedStates::RUNNING )
                    {
                        // means that the object is not active
                        // copy replacement image from old to new container
                        xStream = GetEmbeddedObjectContainer().GetGraphicStream( xObj, &aMediaType );
                    }

                    if ( !xStream.is() )
                    {
                        // the image must be regenerated
                        // TODO/LATER: another aspect could be used
                        if ( xObj->getCurrentState() == embed::EmbedStates::LOADED )
                                bSwitchBackToLoaded = sal_True;

                        xStream = svt::EmbeddedObjectRef::GetGraphicReplacementStream(
                                                                embed::Aspects::MSOLE_CONTENT,
                                                                xObj,
                                                                &aMediaType );
                    }

                    if ( xStream.is() )
                    {
                        if ( bOasis )
                            aCnt.InsertGraphicStream( xStream, aNames[n], aMediaType );
                        else
                        {
                            // it is a linked object exported into SO7 format
                            InsertStreamIntoPicturesStorage_Impl( xStorage, xStream, aNames[n] );
                        }
                    }
                }

                uno::Reference< embed::XEmbedPersist > xPersist( xObj, uno::UNO_QUERY );
                if ( xPersist.is() )
                {
                    uno::Sequence< beans::PropertyValue > aArgs(1);
                    aArgs[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "StoreVisualReplacement" ) );
                    aArgs[0].Value <<= (sal_Bool)( !bOasis );
                    xPersist->storeAsEntry( xStorage,
                                            xPersist->getEntryName(),
                                            uno::Sequence< beans::PropertyValue >(),
                                            aArgs );
                }

                if ( bSwitchBackToLoaded )
                    // switch back to loaded state; that way we have a minimum cache confusion
                    xObj->changeState( embed::EmbedStates::LOADED );
            }
        }
    }
    catch ( uno::Exception& )
    {
        // TODO/LATER: error handling
        bResult = sal_False;
    }

    // the old SO6 format does not store graphical replacements
    if ( !bOasis && bResult )
    {
        try
        {
            ::rtl::OUString aObjReplElement( RTL_CONSTASCII_USTRINGPARAM( "ObjectReplacements" ) );
            if ( xStorage->hasByName( aObjReplElement ) && xStorage->isStorageElement( aObjReplElement ) )
                xStorage->removeElement( aObjReplElement );
        }
        catch ( uno::Exception& )
        {
            // TODO/LATER: error handling;
            bResult = sal_False;
        }
    }

    if ( bResult )
        bResult = CopyStoragesOfUnknownMediaType( GetStorage(), xStorage );

    return bResult;
}

sal_Bool SfxObjectShell::SaveCompletedChildren( sal_Bool bSuccess )
{
    sal_Bool bResult = sal_True;

    uno::Sequence < ::rtl::OUString > aNames = GetEmbeddedObjectContainer().GetObjectNames();
    for ( sal_Int32 n=0; n<aNames.getLength(); n++ )
    {
        uno::Reference < embed::XEmbeddedObject > xObj = GetEmbeddedObjectContainer().GetEmbeddedObject( aNames[n] );
        OSL_ENSURE( xObj.is(), "An empty entry in the embedded objects list!\n" );
        if ( xObj.is() )
        {
            uno::Reference< embed::XEmbedPersist > xPersist( xObj, uno::UNO_QUERY );
            if ( xPersist.is() )
            {
                try
                {
                    xPersist->saveCompleted( bSuccess );
                }
                catch( uno::Exception& )
                {
                    // TODO/LATER: error handling
                    bResult = sal_False;
                    break;
                }
            }
        }
    }

    return bResult;
}

sal_Bool SfxObjectShell::SwitchChildrenPersistance( const uno::Reference< embed::XStorage >& xStorage )
{
    if ( !xStorage.is() )
    {
        // TODO/LATER: error handling
        return sal_False;
    }

    sal_Bool bResult = sal_True;

    uno::Sequence < ::rtl::OUString > aNames = GetEmbeddedObjectContainer().GetObjectNames();
    for ( sal_Int32 n=0; n<aNames.getLength(); n++ )
    {
        uno::Reference < embed::XEmbeddedObject > xObj = GetEmbeddedObjectContainer().GetEmbeddedObject( aNames[n] );
        OSL_ENSURE( xObj.is(), "An empty entry in the embedded objects list!\n" );
        if ( xObj.is() )
        {
            uno::Reference< embed::XEmbedPersist > xPersist( xObj, uno::UNO_QUERY );
            if ( xPersist.is() )
            {
                try
                {
                    xPersist->setPersistentEntry( xStorage,
                                                  aNames[n],
                                                  embed::EntryInitModes::NO_INIT,
                                                  uno::Sequence< beans::PropertyValue >(),
                                                  uno::Sequence< beans::PropertyValue >() );

                }
                catch( uno::Exception& )
                {
                    // TODO/LATER: error handling
                    bResult = sal_False;
                    break;
                }
            }
        }
    }

    return bResult;
}

// Never call this method directly, always use the DoSaveCompleted call
sal_Bool SfxObjectShell::SaveCompleted( const uno::Reference< embed::XStorage >& xStorage )
{
    sal_Bool bResult = sal_False;
    sal_Bool bSendNotification = sal_False;
    uno::Reference< embed::XStorage > xOldStorageHolder;

    if ( !xStorage.is() || xStorage == GetStorage() )
    {
        // no persistence change
        bResult = SaveCompletedChildren( sal_False );
    }
    else
    {
        if ( pImp->mpObjectContainer )
            GetEmbeddedObjectContainer().SwitchPersistence( xStorage );
        bResult = SwitchChildrenPersistance( xStorage );
    }

    if ( bResult )
    {
        if ( xStorage.is() && pImp->m_xDocStorage != xStorage )
        {
            xOldStorageHolder = pImp->m_xDocStorage;
            pImp->m_xDocStorage = xStorage;
            bSendNotification = sal_True;

            if ( IsEnableSetModified() )
                SetModified( sal_False );
        }
    }

    if ( bSendNotification )
    {
        // this notification will be sent by caller from sfxbasemodel
        // SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_SAVEASDOC, this ) );
    }

    return bResult;
}


sal_Bool StoragesOfUnknownMediaTypeAreCopied_Impl( const uno::Reference< embed::XStorage >& xSource,
                                                    const uno::Reference< embed::XStorage >& xTarget )
{
    OSL_ENSURE( xSource.is() && xTarget.is(), "Source and/or target storages are not available!\n" );
    if ( !xSource.is() || !xTarget.is() || xSource == xTarget )
        return sal_True;

    try
    {
        uno::Sequence< ::rtl::OUString > aSubElements = xSource->getElementNames();
        for ( sal_Int32 nInd = 0; nInd < aSubElements.getLength(); nInd++ )
        {
            if ( xSource->isStorageElement( aSubElements[nInd] ) )
            {
                uno::Reference< embed::XStorage > xSubStorage;
                try {
                    xSubStorage = xSource->openStorageElement( aSubElements[nInd], embed::ElementModes::READ );
                } catch( uno::Exception& )
                {}

                if ( !xSubStorage.is() )
                {
                    // TODO/LATER: as optimization in future a substorage of target storage could be used
                    //             instead of the temporary storage; this substorage should be removed later
                    //             if the MimeType is wrong
                    xSubStorage = ::comphelper::OStorageHelper::GetTemporaryStorage();
                    xSource->copyStorageElementLastCommitTo( aSubElements[nInd], xSubStorage );
                }

                uno::Reference< beans::XPropertySet > xProps( xSubStorage, uno::UNO_QUERY_THROW );
                ::rtl::OUString aMediaType;
                xProps->getPropertyValue( ::rtl::OUString::createFromAscii( "MediaType" ) ) >>= aMediaType;

                // TODO/LATER: there should be a way to detect whether an object with such a MediaType can exist
                //             probably it should be placed in the MimeType-ClassID table or in standalone table
                if ( aMediaType.getLength()
                  && aMediaType.compareToAscii( "application/vnd.sun.star.oleobject" ) != COMPARE_EQUAL )
                {
                    ::com::sun::star::datatransfer::DataFlavor aDataFlavor;
                    aDataFlavor.MimeType = aMediaType;
                    sal_uInt32 nFormat = SotExchange::GetFormat( aDataFlavor );

                    switch ( nFormat )
                    {
                        case SOT_FORMATSTR_ID_STARWRITER_60 :
                        case SOT_FORMATSTR_ID_STARWRITERWEB_60 :
                        case SOT_FORMATSTR_ID_STARWRITERGLOB_60 :
                        case SOT_FORMATSTR_ID_STARDRAW_60 :
                        case SOT_FORMATSTR_ID_STARIMPRESS_60 :
                        case SOT_FORMATSTR_ID_STARCALC_60 :
                        case SOT_FORMATSTR_ID_STARCHART_60 :
                        case SOT_FORMATSTR_ID_STARMATH_60 :
                        case SOT_FORMATSTR_ID_STARWRITER_8:
                        case SOT_FORMATSTR_ID_STARWRITERWEB_8:
                        case SOT_FORMATSTR_ID_STARWRITERGLOB_8:
                        case SOT_FORMATSTR_ID_STARDRAW_8:
                        case SOT_FORMATSTR_ID_STARIMPRESS_8:
                        case SOT_FORMATSTR_ID_STARCALC_8:
                        case SOT_FORMATSTR_ID_STARCHART_8:
                        case SOT_FORMATSTR_ID_STARMATH_8:
                            break;

                        default:
                        {
                            if ( !xTarget->hasByName( aSubElements[nInd] ) )
                                return sal_False;
                        }
                    }
                }
            }
        }
    }
    catch( uno::Exception& )
    {
        OSL_ENSURE( sal_False, "Cant check storage consistency!\n" );
    }

    return sal_True;
}


sal_Bool SfxObjectShell::SwitchPersistance( const uno::Reference< embed::XStorage >& xStorage )
{
    sal_Bool bResult = sal_False;
    if ( xStorage.is() )
    {
        if ( pImp->mpObjectContainer )
            GetEmbeddedObjectContainer().SwitchPersistence( xStorage );
        bResult = SwitchChildrenPersistance( xStorage );

        // TODO/LATER: substorages that have unknown mimetypes probably should be copied to the target storage here
        OSL_ENSURE( StoragesOfUnknownMediaTypeAreCopied_Impl( pImp->m_xDocStorage, xStorage ),
                    "Some of substorages with unknown mimetypes is lost!" );
    }

    if ( bResult )
    {
        if ( pImp->m_xDocStorage != xStorage )
            DoSaveCompleted( new SfxMedium( xStorage, GetMedium()->GetBaseURL() ) );

        if ( IsEnableSetModified() )
            SetModified( sal_True ); // ???
    }

    return bResult;
}

sal_Bool SfxObjectShell::CopyStoragesOfUnknownMediaType( const uno::Reference< embed::XStorage >& xSource,
                                                         const uno::Reference< embed::XStorage >& xTarget )
{
    // This method does not commit the target storage and should not do it
    sal_Bool bResult = sal_True;

    try
    {
        uno::Sequence< ::rtl::OUString > aSubElements = xSource->getElementNames();
        const ::rtl::OUString* pSubElements = aSubElements.getConstArray();
        for ( sal_Int32 nInd = 0; nInd < aSubElements.getLength(); nInd++ )
        {
            if ( xSource->isStorageElement( aSubElements[nInd] ) )
            {
                uno::Reference< embed::XStorage > xSubStorage;
                try {
                    xSubStorage = xSource->openStorageElement( aSubElements[nInd], embed::ElementModes::READ );
                } catch( uno::Exception& )
                {}

                if ( !xSubStorage.is() )
                {
                    // TODO/LATER: as optimization in future a substorage of target storage could be used
                    //             instead of the temporary storage; this substorage should be removed later
                    //             if the MimeType is wrong
                    xSubStorage = ::comphelper::OStorageHelper::GetTemporaryStorage();
                    xSource->copyStorageElementLastCommitTo( aSubElements[nInd], xSubStorage );
                }

                uno::Reference< beans::XPropertySet > xProps( xSubStorage, uno::UNO_QUERY_THROW );
                ::rtl::OUString aMediaType;
                xProps->getPropertyValue( ::rtl::OUString::createFromAscii( "MediaType" ) ) >>= aMediaType;

                // TODO/LATER: there should be a way to detect whether an object with such a MediaType can exist
                //             probably it should be placed in the MimeType-ClassID table or in standalone table
                if ( aMediaType.getLength()
                  && aMediaType.compareToAscii( "application/vnd.sun.star.oleobject" ) != COMPARE_EQUAL )
                {
                    ::com::sun::star::datatransfer::DataFlavor aDataFlavor;
                    aDataFlavor.MimeType = aMediaType;
                    sal_uInt32 nFormat = SotExchange::GetFormat( aDataFlavor );

                    switch ( nFormat )
                    {
                        case SOT_FORMATSTR_ID_STARWRITER_60 :
                        case SOT_FORMATSTR_ID_STARWRITERWEB_60 :
                        case SOT_FORMATSTR_ID_STARWRITERGLOB_60 :
                        case SOT_FORMATSTR_ID_STARDRAW_60 :
                        case SOT_FORMATSTR_ID_STARIMPRESS_60 :
                        case SOT_FORMATSTR_ID_STARCALC_60 :
                        case SOT_FORMATSTR_ID_STARCHART_60 :
                        case SOT_FORMATSTR_ID_STARMATH_60 :
                        case SOT_FORMATSTR_ID_STARWRITER_8:
                        case SOT_FORMATSTR_ID_STARWRITERWEB_8:
                        case SOT_FORMATSTR_ID_STARWRITERGLOB_8:
                        case SOT_FORMATSTR_ID_STARDRAW_8:
                        case SOT_FORMATSTR_ID_STARIMPRESS_8:
                        case SOT_FORMATSTR_ID_STARCALC_8:
                        case SOT_FORMATSTR_ID_STARCHART_8:
                        case SOT_FORMATSTR_ID_STARMATH_8:
                            break;

                        default:
                        {
                            OSL_ENSURE(
                                aSubElements[nInd].equalsAscii( "Configurations2" ) || !xTarget->hasByName( aSubElements[nInd] ),
                                "The target storage is an output storage, the element should not exist in the target!\n" );

                            if ( !xTarget->hasByName( aSubElements[nInd] ) )
                            {
                                uno::Reference< embed::XStorage > xSubTarget =
                                    xTarget->openStorageElement( aSubElements[nInd], embed::ElementModes::WRITE );
                                if ( !xSubTarget.is() )
                                    throw uno::RuntimeException();
                                xSubStorage->copyToStorage( xSubTarget );
                            }
                        }
                    }
                }
            }
        }
    }
    catch( uno::Exception& )
    {
        bResult = sal_False;
        // TODO/LATER: a specific error could be provided
    }

    return bResult;
}

sal_Bool SfxObjectShell::GenerateAndStoreThumbnail( sal_Bool bEncrypted,
                                                    sal_Bool bSigned,
                                                    sal_Bool bIsTemplate,
                                                    const uno::Reference< embed::XStorage >& xStor )
{
    sal_Bool bResult = sal_False;

    try {
        uno::Reference< embed::XStorage > xThumbnailStor =
                                        xStor->openStorageElement( ::rtl::OUString::createFromAscii( "Thumbnails" ),
                                                                    embed::ElementModes::READWRITE );
        if ( xThumbnailStor.is() )
        {
            uno::Reference< io::XStream > xStream = xThumbnailStor->openStreamElement(
                                                        ::rtl::OUString::createFromAscii( "thumbnail.png" ),
                                                        embed::ElementModes::READWRITE );

            if ( xStream.is() && WriteThumbnail( bEncrypted, bSigned, bIsTemplate, xStream ) )
            {
                uno::Reference< embed::XTransactedObject > xTransact( xThumbnailStor, uno::UNO_QUERY_THROW );
                xTransact->commit();
                bResult = sal_True;
            }
        }
    }
    catch( uno::Exception& )
    {
    }

    return bResult;
}

sal_Bool SfxObjectShell::WriteThumbnail( sal_Bool bEncrypted,
                                         sal_Bool bSigned,
                                         sal_Bool bIsTemplate,
                                         const uno::Reference< io::XStream >& xStream )
{
    sal_Bool bResult = sal_False;

    if ( xStream.is() )
    {
        try {
            uno::Reference< io::XTruncate > xTruncate( xStream->getOutputStream(), uno::UNO_QUERY_THROW );
            xTruncate->truncate();

            if ( bEncrypted )
            {
                sal_uInt16 nResID = GraphicHelper::getThumbnailReplacementIDByFactoryName_Impl(
                                        ::rtl::OUString::createFromAscii( GetFactory().GetShortName() ),
                                        bIsTemplate );
                if ( nResID )
                {
                    if ( !bSigned )
                    {
                        bResult = GraphicHelper::getThumbnailReplacement_Impl( nResID, xStream );
                    }
                    else
                    {
                        // retrieve the bitmap and write a signature bitmap over it
                        SfxResId aResId( nResID );
                        BitmapEx aThumbBitmap( aResId );
                        bResult = GraphicHelper::getSignedThumbnailFormatFromBitmap_Impl( aThumbBitmap, xStream );
                    }
                }
            }
            else
            {
                GDIMetaFile* pMetaFile = GetPreviewMetaFile( sal_False );
                if ( pMetaFile )
                {
                    bResult = GraphicHelper::getThumbnailFormatFromGDI_Impl( pMetaFile, bSigned, xStream );
                    delete pMetaFile;
                }
            }
        }
        catch( uno::Exception& )
        {}
    }

    return bResult;
}

