/*************************************************************************
 *
 *  $RCSfile: frmload.cxx,v $
 *
 *  $Revision: 1.49 $
 *
 *  last change: $Author: mba $ $Date: 2002-05-27 14:18:23 $
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

#include "frmload.hxx"

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_WRAPPEDTARGETRUNTIMEEXCEPTION_HPP_
#include <com/sun/star/lang/WrappedTargetRuntimeException.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_COMMANDABORTEDEXCEPTION_HPP_
#include <com/sun/star/ucb/CommandAbortedException.hpp>
#endif

#ifndef __FRAMEWORK_DISPATCH_INTERACTION_HXX_
#include <framework/interaction.hxx>
#endif

#ifndef _TOOLKIT_UNOHLP_HXX
#include <toolkit/helper/vclunohelper.hxx>
#endif

#include <rtl/ustring.h>
#include <rtl/logfile.hxx>
#include <svtools/itemset.hxx>
#include <vcl/window.hxx>
#include <svtools/eitem.hxx>
#include <svtools/stritem.hxx>
#include <tools/urlobj.hxx>
#include <vos/mutex.hxx>
#include <svtools/sfxecode.hxx>
#include <svtools/ehdl.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

#include "app.hxx"
#include "request.hxx"
#include "sfxsids.hrc"
#include "dispatch.hxx"
#include "sfxuno.hxx"
#include "viewfrm.hxx"
#include "topfrm.hxx"
#include "frame.hxx"
#include "docfac.hxx"
#include "fcontnr.hxx"
#include "loadenv.hxx"
#include "docfile.hxx"
#include "docfilt.hxx"

Reference< XInterface > SAL_CALL SfxFrameLoaderFactory::createInstance(void) throw(Exception, RuntimeException)
{
    Reference < XFrameLoader > xLoader( pCreateFunction( xSMgr ), UNO_QUERY );
    SfxFrameLoader* pLoader = (SfxFrameLoader*) xLoader.get();
    pLoader->SetFilterName( aImplementationName );
    return xLoader;
}

Reference< XInterface > SAL_CALL SfxFrameLoaderFactory::createInstanceWithArguments(const Sequence<Any>& Arguments) throw(Exception, RuntimeException)
{
    return createInstance();
}

::rtl::OUString SAL_CALL SfxFrameLoaderFactory::getImplementationName() throw(RuntimeException)
{
    return aImplementationName;
}

sal_Bool SAL_CALL SfxFrameLoaderFactory::supportsService(const ::rtl::OUString& ServiceName) throw(RuntimeException)
{
    if ( ServiceName.compareToAscii("com.sun.star.frame.FrameLoader") == COMPARE_EQUAL )
        return sal_True;
    else
        return sal_False;
}

Sequence< ::rtl::OUString > SAL_CALL SfxFrameLoaderFactory::getSupportedServiceNames(void) throw(RuntimeException)
{
    Sequence< ::rtl::OUString > aRet(1);
    *aRet.getArray() = ::rtl::OUString::createFromAscii("com.sun.star.frame.FrameLoader");
    return aRet;
}

SfxFrameLoader::SfxFrameLoader( const REFERENCE < ::com::sun::star::lang::XMultiServiceFactory >& xFactory )
    : pMatcher( 0 )
    , pLoader( 0 )
    , bLoadDone( sal_False )
    , bLoadState( sal_False )
{
}

SfxFrameLoader::~SfxFrameLoader()
{
    if ( pLoader )
        pLoader->ReleaseRef();
    delete pMatcher;
}

sal_Bool SAL_CALL SfxFrameLoader::load( const Sequence< PropertyValue >& rArgs, const Reference< XFrame >& rFrame ) throw( RuntimeException )
{
    // this methods assumes that the filter is detected before, usually by calling the detect() method below
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    RTL_LOGFILE_CONTEXT( aLog, "sfx2 (mb93783) ::SfxFrameLoader::load" );

    String rURL;
    String aTypeName;
    sal_uInt32 nPropertyCount = rArgs.getLength();
    sal_Bool bReadOnlyTest = sal_False;
    for( sal_uInt32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
    {
        if( rArgs[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("FileName")) )
        {
            ::rtl::OUString sTemp;
            rArgs[nProperty].Value >>= sTemp;
            rURL = sTemp;
        }
        if( rArgs[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("TypeName")) )
        {
            // the name of the file type detected so far (or provided by the client code)
            ::rtl::OUString sTemp;
            rArgs[nProperty].Value >>= sTemp;
            aTypeName = sTemp;
        }
        if( rArgs[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("FilterName")) )
        {
            // the name of the desired filter, usually to prevent us from using the default
            // filter for the detected type
            ::rtl::OUString sTemp;
            rArgs[nProperty].Value >>= sTemp;
            aFilterName = sTemp;
        }
        if( rArgs[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("ReadOnly")) )
            rArgs[nProperty].Value >>= bReadOnlyTest;
    }

    const SfxFilter*  pFilter  = NULL;
    SfxFilterMatcher& rMatcher = SFX_APP()->GetFilterMatcher();
    if ( !aFilterName.Len() )
    {
        // no filter detection made, give a warning for the developer
        DBG_ERROR("load called without calling detect before!");
        if ( !aTypeName.Len() )
            return sal_False;

        // try to find a filter with SFX filter detection using the typename
        SfxFilterFlags    nMust    = SFX_FILTER_IMPORT;
        SfxFilterFlags    nDont    = SFX_FILTER_NOTINSTALLED;
        pFilter = rMatcher.GetFilter4EA( aTypeName, nMust, nDont );
        if ( !pFilter )
            return sal_False;
        else
            // use filter names without prefix
            aFilterName = pFilter->GetFilterName();
    }

    xFrame = rFrame;

    // Achtung: beim Abr�umen der Objekte kann die SfxApp destruiert werden, vorher noch Deinitialize_Impl rufen
    SfxApplication* pApp = SFX_APP();

    SfxAllItemSet aSet( pApp->GetPool() );
    TransformParameters( SID_OPENDOC, rArgs, aSet );

    SFX_ITEMSET_ARG( &aSet, pRefererItem, SfxStringItem, SID_REFERER, FALSE );
    if ( !pRefererItem )
        aSet.Put( SfxStringItem( SID_REFERER, String() ) );

    SfxFrame* pFrame=0;
    for ( pFrame = SfxFrame::GetFirst(); pFrame; pFrame = SfxFrame::GetNext( *pFrame ) )
    {
        if ( pFrame->GetFrameInterface() == xFrame )
            break;
    }

    if ( !pFrame )
        pFrame = SfxTopFrame::Create( rFrame );

    // check for the URL pattern of our factory URLs
    BOOL bFactoryURL = FALSE;
    const SfxObjectFactory* pFactory = 0;
    String aPrefix = String::CreateFromAscii( "private:factory/" );
    String aFact( rURL );
    if ( aPrefix.Len() == aFact.Match( aPrefix ) )
    {
        // it's a factory URL
        aFact.Erase( 0, aPrefix.Len() );

        // look for parameters
        String aParam;
        USHORT nPos = aFact.Search( '?' );
        if ( nPos != STRING_NOTFOUND )
        {
            // currently only the "slot" parameter is supported
            USHORT nParamPos = aFact.Search( String::CreateFromAscii("slot="), nPos );
            if ( nParamPos != STRING_NOTFOUND )
                aParam = aFact.Copy( nParamPos+5, aFact.Len() );
            aFact.Erase( nPos, aFact.Len() );
        }

        WildCard aSearchedFac( aFact.EraseAllChars('4').ToUpperAscii() );
        for( USHORT n = SfxObjectFactory::GetObjectFactoryCount_Impl(); !pFactory && n--; )
        {
            pFactory = &SfxObjectFactory::GetObjectFactory_Impl( n );
            if( !aSearchedFac.Matches( String::CreateFromAscii( pFactory->GetShortName() ).ToUpperAscii() ) )
                pFactory = 0;
        }

        if ( pFactory )
        {
            INetURLObject aObj( rURL );
            if ( aParam.Len() )
            {
                // slots are executed on the module shell
                sal_uInt16 nSlotId = (sal_uInt16) aParam.ToInt32();
                SfxModule* pMod = pFactory->GetModule()->Load();
                SfxRequest aReq( nSlotId, SFX_CALLMODE_SYNCHRON, pMod->GetPool() );
                aReq.AppendItem( SfxStringItem ( SID_FILE_NAME, rURL ) );
                aReq.AppendItem( SfxFrameItem ( SID_DOCFRAME, pFrame ) );

                // the next parameter is especially needed for Impress, because they want to know
                // wether their slot is called directly (like from Autopilot menue) or as part
                // of a "NewDocument" request
                aReq.AppendItem( SfxBoolItem ( SID_NEWDOCDIRECT, TRUE ) );

                const SfxPoolItem* pRet = pMod->ExecuteSlot( aReq );
                if ( pRet )
                {
                    bLoadState = sal_True;
                }
                else
                {
                    SfxObjectShell* pDoc = pFrame->GetCurrentDocument();
                    if ( !pDoc )
                    {
                        Reference < XFrame > aXFrame;
                        pFrame->SetFrameInterface_Impl( aXFrame );
                        pFrame->DoClose();
                    }

                    bLoadState = sal_False;
                }

                return bLoadState;
            }

            String aPathName( aObj.GetMainURL() );
            if( pFactory->GetStandardTemplate().Len() )
            {
                // standard template set -> load it "AsTemplate"
                aSet.Put( SfxStringItem ( SID_FILE_NAME, pFactory->GetStandardTemplate() ) );
                aSet.Put( SfxBoolItem( SID_TEMPLATE, sal_True ) );
            }
            else
            {
                // execute "NewDocument" request
                SfxViewShell *pView = pFrame->GetCurrentViewFrame() ? pFrame->GetCurrentViewFrame()->GetViewShell() : NULL;
                SfxRequest aReq( SID_NEWDOCDIRECT, SFX_CALLMODE_SYNCHRON, aSet );
                aReq.AppendItem( SfxFrameItem( SID_DOCFRAME, pFrame ) );
                aReq.AppendItem( SfxStringItem( SID_NEWDOCDIRECT, String::CreateFromAscii(pFactory->GetShortName()) ) );
                const SfxPoolItem* pRet = pApp->ExecuteSlot( aReq );
                if ( pFrame->GetCurrentViewFrame() && pView != pFrame->GetCurrentViewFrame()->GetViewShell() )
                {
                    bLoadState = sal_True;
                }
                else if ( xListener.is() )
                {
                    if ( !pFrame->GetCurrentDocument() )
                    {
                        Reference < XFrame > aXFrame;
                        pFrame->SetFrameInterface_Impl( aXFrame );
                        pFrame->DoClose();
                    }

                    bLoadState = sal_False;
                }

                xFrame = Reference < XFrame >();
                return bLoadState;
            }
        }
    }
    else
    {
        // load the desired file
        aSet.Put( SfxStringItem ( SID_FILE_NAME, rURL ) );
    }

    aSet.Put( SfxFrameItem( SID_DOCFRAME, pFrame ) );
    aSet.Put( SfxStringItem( SID_FILTER_NAME, aFilterName ) );

    // create LoadEnvironment and set link for callback when it is finished
    pLoader = LoadEnvironment_Impl::Create( aSet );
    pLoader->AddRef();
    pLoader->SetDoneLink( LINK( this, SfxFrameLoader, LoadDone_Impl ) );

    if ( !pFactory )
    {
        pFilter = rMatcher.GetFilter4FilterName( aFilterName );
        SfxFactoryFilterContainer* pCont = pFilter ? (SfxFactoryFilterContainer*) pFilter->GetFilterContainer() : NULL;
        if ( pCont )
            pFactory = &pCont->GetFactory();
    }

    if ( pFactory )
    {
        bLoadDone = sal_False;
        pMatcher = new SfxFilterMatcher( pFactory->GetFilterContainer() );
        pLoader->SetFilterMatcher( pMatcher );
        pLoader->Start();

        // wait for callback
        while( bLoadDone == sal_False )
            Application::Yield();
    }

    return bLoadState;
}

void SfxFrameLoader::cancel() throw( RUNTIME_EXCEPTION )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( pLoader )
        pLoader->CancelTransfers();
    bLoadDone = sal_True;
}

IMPL_LINK( SfxFrameLoader, LoadDone_Impl, void*, pVoid )
{
    DBG_ASSERT( pLoader, "No Loader created, but LoadDone ?!" );

    if ( pLoader->GetError() )
    {
        SfxFrame* pFrame = pLoader->GetFrame();
        if ( pFrame && !pFrame->GetCurrentDocument() )
        {
            ::vos::OGuard aGuard( Application::GetSolarMutex() );
            Reference < XFrame > aXFrame;
            pFrame->SetFrameInterface_Impl( aXFrame );
            pFrame->DoClose();
        }

        bLoadDone  = sal_True ;
        bLoadState = sal_False;
    }
    else
    {
        bLoadDone  = sal_True;
        bLoadState = sal_True;
    }

    xFrame = Reference < XFrame >();
    xListener = Reference < XLoadEventListener >();
    return NULL;
}

SfxObjectFactory& SfxFrameLoader_Impl::GetFactory()
{
    DBG_ERROR("Useless method!");
    SfxObjectFactory* pFactory = 0;
    return *pFactory;
}

::rtl::OUString SAL_CALL SfxFrameLoader::detect( ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lDescriptor ) throw( ::com::sun::star::uno::RuntimeException )
{
    // This method detects (or verifies) the file type of the content to load. It also detects the filter, because SFX
    // detects type through filters, so detecting the filter later in the load method would be double work.
    // It can detect all types for that an internal filter is known, even if the preselected filter is an external one.
    // Internal filters should be exactly as described in the configuration, but they may also have filter names with a
    // module prefix as in the old SFX-API, this is handled inside this method; but all filter names that are inserted
    // into the media descriptor by this method will be without prefix
    // examples for supported formats for filter names:
    //  with module prefix  : "swriter: StarWriter 5.0"
    //  without             : "StarWriter 5.0"

    RTL_LOGFILE_CONTEXT( aLog, "sfx2 (mb93783) ::SfxFrameLoader::detect" );

    Reference < XInputStream > xStream;
    Reference < XInteractionHandler > xInteraction;
    String aURL;
    ::rtl::OUString sTemp;
    rtl::OUString aTypeName;            // a name describing the type (from MediaDescriptor, usually from flat detection)
    String aPreselectedFilterName;      // a name describing the filter to use (from MediaDescriptor, usually from UI action)
    const SfxFilter* pFilter = NULL, *pExternalFilter = NULL;

    // opening as template is done when a parameter tells to do so and a template filter can be detected
    // (otherwise no valid filter would be found) or if the detected filter is a template filter and
    // there is no parameter that forbids to open as template
    sal_Bool bOpenAsTemplate = sal_False;
    sal_Bool bWasReadOnly = sal_False, bReadOnly = sal_False;

    // now some parameters that can already be in the array, but may be overwritten or new inserted here
    // remember their indices in the case new values must be added to the array
    sal_Int32 nPropertyCount = lDescriptor.getLength();
    sal_Int32 nIndexOfFilterName = -1;
    sal_Int32 nIndexOfInputStream = -1;
    sal_Int32 nIndexOfReadOnlyFlag = -1;
    sal_Int32 nIndexOfTemplateFlag = -1;
    for( sal_Int32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
    {
        // extract properties
        if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("FileName")) )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aURL = sTemp;
        }
        else if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("TypeName")) )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aTypeName = sTemp;
        }
        else if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("FilterName")) )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aPreselectedFilterName = sTemp;

            // if the preselected filter name is not correct, it must be erased after detection
            // remember index of property to get access to it later
            nIndexOfFilterName = nProperty;
        }
        else if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("InputStream")) )
            nIndexOfInputStream = nProperty;
        else if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("ReadOnly")) )
            nIndexOfReadOnlyFlag = nProperty;
        else if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("AsTemplate")) )
        {
            lDescriptor[nProperty].Value >>= bOpenAsTemplate;
            nIndexOfTemplateFlag = nProperty;
        }
        else if( lDescriptor[nProperty].Name == OUString(RTL_CONSTASCII_USTRINGPARAM("InteractionHandler")) )
            lDescriptor[nProperty].Value >>= xInteraction;
    }

    // can't check the type for external filters, so set the "dont" flag accordingly
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    SfxFilterFlags nMust = SFX_FILTER_IMPORT, nDont = SFX_FILTER_NOTINSTALLED;
    SfxFilterMatcher& rMatcher = SFX_APP()->GetFilterMatcher();
    if ( aPreselectedFilterName.Len() )
    {
        // the preselected filter name itself may be valid SFX filter name
        // the method "GetFilter" supports both formats, so there is no need to deal with module prefixes here
        pFilter = rMatcher.GetFilter( aPreselectedFilterName );
        if ( pFilter )
        {
            if ( pFilter->GetFilterFlags() & SFX_FILTER_STARONEFILTER )
            {
                // external filters can't be detected with this service ( only their type may be used )
                pExternalFilter = pFilter;
            }
        }
        else
        {
            // the preselected filter does not belong to a valid type, so forget it
            aPreselectedFilterName.Erase();
        }
    }

    if ( !pFilter && aTypeName.getLength() )
    {
        // now try the type from the shallow detection or extracted from the preselected filter,
        // though the filter itself was not valid
        // look for a filter registered for the desired type that is not an external one
        // this will be the default filter for that type
        pFilter = rMatcher.GetFilter4EA( aTypeName, nMust, SFX_FILTER_NOTINSTALLED );
        if ( pFilter && pFilter->GetFilterFlags() & SFX_FILTER_STARONEFILTER )
        {
            // external filters can't be detected with this service ( only their type may be used )
            pExternalFilter = pFilter;
        }
    }

    String aPrefix = String::CreateFromAscii( "private:factory/" );
    if( aURL.Match( aPrefix ) == aPrefix.Len() )
    {
        // private:factory URLs are used to create new documents, so nothing must be detected
        // use the result of the shallow detection
        if ( pFilter )
        {
            // use format without prefix
            aFilterName = pFilter->GetFilterName();
            if ( nIndexOfFilterName != -1 )
                lDescriptor[nIndexOfFilterName].Value <<= ::rtl::OUString( aFilterName );
            else
            {
                lDescriptor.realloc( nPropertyCount + 1 );
                lDescriptor[nPropertyCount].Name = ::rtl::OUString::createFromAscii("FilterName");
                lDescriptor[nPropertyCount].Value <<= ::rtl::OUString( aFilterName );
                nPropertyCount++;
            }
        }
        else
            DBG_ERROR( "Illegal type for factory URL!" );
        return aTypeName;
    }
    else if ( !pExternalFilter )
    {
        // Try a SFX filter detection
        // This is only possible for non-external filters!
        SfxErrorContext aCtx( ERRCTX_SFX_OPENDOC, aURL );
        const SfxFilter* pOldFilter = pFilter;

        // transform parameters into an SfxItemSet
        SfxApplication* pApp = SFX_APP();
        SfxAllItemSet *pSet = new SfxAllItemSet( pApp->GetPool() );
        TransformParameters( SID_OPENDOC, lDescriptor, *pSet );
        SFX_ITEMSET_ARG( pSet, pItem, SfxBoolItem, SID_DOC_READONLY, FALSE );

        bWasReadOnly = pItem && pItem->GetValue();

        // ctor of SfxMedium uses owner transition of ItemSet
        SfxMedium aMedium( aURL, bWasReadOnly ? STREAM_STD_READ : STREAM_STD_READWRITE, FALSE, NULL, pSet );
        aMedium.UseInteractionHandler( TRUE );
        aMedium.SetInteractionHandler( xInteraction );

        BOOL bIsStorage = aMedium.IsStorage();
        if ( aMedium.GetErrorCode() == ERRCODE_NONE )
        {
            // maybe that IsStorage() already created an error!
            if ( bIsStorage )
                aMedium.GetStorage();
            else
                aMedium.GetInStream();

            // special filters that can or must(!) be detected inside the medium without further investigation
            // f.e. disk spanned jar files
            if ( aMedium.GetFilter() )
                pFilter = aMedium.GetFilter();

            // remember input stream and put it into the descriptor later
            xStream = aMedium.GetInputStream();
            bReadOnly = aMedium.IsReadOnly();
        }

        if ( aMedium.GetErrorCode() == ERRCODE_NONE && !aMedium.GetFilter() )
        {
            // check the filter detected so far (if any)
            ErrCode nErr = ERRCODE_ABORT;
            if ( pFilter )
            {
                // type or filter name matched to a valid filter name detectable with this service
                const SfxFilter* pOldFilter = pFilter;
                SfxFilterFlags nFlags = pFilter->GetFilterFlags();
                if ( ( nFlags & nMust ) == nMust && ( nFlags & nDont ) == 0 )
                {
                    // some detection methods need the filter name in the ItemSet
                    pSet->Put( SfxStringItem( SID_FILTER_NAME, pFilter->GetFilterName() ) );
                    nErr = pFilter->GetFilterContainer()->GetFilter4Content( aMedium, &pFilter, nMust, nDont );
                    if ( pOldFilter == pFilter && nErr != ERRCODE_NONE )
                        pFilter = NULL;
                }
                else
                    // filterflags don't fit
                    pFilter = NULL;

                if ( !pFilter )
                {
                    // the filter exists, but deep detection regrets it ( or filter flags don't match )
                    if ( aPreselectedFilterName.Len() )
                    {
                        // the filter we just tried was the preselected filter
                        // it doesn't fit, so erase the filter name from media descriptor
                        lDescriptor[nIndexOfFilterName].Value <<= ::rtl::OUString();

                        // try the typename instead ( if any )
                        if ( aTypeName.getLength() )
                        {
                            pFilter = rMatcher.GetFilter4EA( aTypeName, nMust, nDont );
                            if ( pFilter && pFilter->GetFilterFlags() & SFX_FILTER_STARONEFILTER )
                            {
                                // external filters can't be detected with this service ( only their type may be used )
                                pExternalFilter = pFilter;
                            }

                            if ( !pExternalFilter )
                            {
                                // no deep detection for external filters
                                if ( pFilter == pOldFilter )
                                    // filter was already checked
                                    pFilter = NULL;
                                else
                                    pOldFilter = pFilter;

                                if ( pFilter )
                                {
                                    SfxFilterFlags nFlags = pFilter->GetFilterFlags();
                                    if ( ( nFlags & nMust ) == nMust && ( nFlags & nDont ) == 0 )
                                    {
                                        // some detection methods need the filter name in the ItemSet
                                        pSet->Put( SfxStringItem( SID_FILTER_NAME, pFilter->GetFilterName() ) );
                                        nErr = pFilter->GetFilterContainer()->GetFilter4Content( aMedium, &pFilter );
                                        if ( pOldFilter == pFilter && nErr != ERRCODE_NONE )
                                            pFilter = NULL;
                                    }
                                    else
                                        // filterflags don't fit
                                        pFilter = NULL;
                                }
                            }
                        }
                    }
                }
                else if ( aPreselectedFilterName.Len() && nErr == ERRCODE_SFX_CONSULTUSER )
                {
                    // preselected filter could not be detected, but the detection function is
                    // known to be a little bit "fuzzy"
                    // ask user for his decision between two detected filters
                    if( !xInteraction.is() )
                    {
                        // no interaction => pFilter wins
                        nErr = ERRCODE_NONE;
                        aMedium.SetError(nErr);
                    }
                    else
                    {
                        ::rtl::OUString aSelectedFilter( aPreselectedFilterName );  // name of pOldFilter
                        ::rtl::OUString aDetectedFilter( pFilter->GetName()     );

                        ::framework::RequestAmbigousFilter* pRequest = new ::framework::RequestAmbigousFilter( aURL, aSelectedFilter, aDetectedFilter );
                        ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionRequest > xRequest( static_cast< ::com::sun::star::task::XInteractionRequest* >(pRequest), ::com::sun::star::uno::UNO_QUERY );

                        xInteraction->handle( xRequest );
                        if( pRequest->isAbort() )
                        {
                            nErr = ERRCODE_ABORT;
                            aMedium.SetError(nErr);
                        }
                        else
                        {
                            if( pRequest->getFilter() == aSelectedFilter )
                            {
                                nErr = ERRCODE_NONE;
                                aMedium.SetError(nErr);
                                pFilter = pOldFilter;
                            }
                        }
                    }
                }
            }

            // No error while reading from medium ?
            if ( !pFilter && aMedium.GetErrorCode() == ERRCODE_NONE )
            {
                // file extension ".vor" is ambigious, so perhaps the flat detection gave the wrong
                // template filter that was discarded in the deep detection
                // searching the filter for the clipboard Id usually gives the document filter, not
                // the template filter, so set the  "must" flags accordingly
                if ( INetURLObject( aURL ).GetExtension().CompareToAscii("vor") == COMPARE_EQUAL )
                    nMust |= SFX_FILTER_TEMPLATEPATH;

                // no filter found until now
                if ( bIsStorage )
                {
                    // try simplest file lookup: clipboard format in storage
                    SvStorageRef aStor = aMedium.GetStorage();
                    if ( aStor.Is() )
                        pFilter = rMatcher.GetFilter4ClipBoardId( aStor->GetFormat(), nMust, nDont );
                }

                // no filter found : try everything possible
                if ( aMedium.GetErrorCode() == ERRCODE_NONE && ( !pFilter || !pFilter->IsOwnFormat() ) )
                    nErr = rMatcher.GetFilter4Content( aMedium, &pFilter, nMust, nDont );
            }
        }

        if ( aMedium.GetErrorCode() != ERRCODE_NONE )
        {
            // when access to medium gives an error, the filter can't be valid
            pFilter = NULL;
            ::com::sun::star::lang::WrappedTargetRuntimeException exPacked;
            ::com::sun::star::ucb::CommandAbortedException        exAbort ;
            exPacked.TargetException <<= exAbort;
            throw exPacked;
        }

        // may be - w4w filter dont close the stream
        // so we should do that here.
        if( !aMedium.IsOpen() )
            xStream = NULL;
    }

    if ( pFilter )
    {
        // successful detection, get the filter name (without prefix)
        aTypeName = pFilter->GetTypeName();
        aFilterName = pFilter->GetFilterName();
        if ( nIndexOfFilterName != -1 )
            // convert to format with factory ( makes load more easy to implement )
            lDescriptor[nIndexOfFilterName].Value <<= ::rtl::OUString( aFilterName );
        else
        {
            lDescriptor.realloc( nPropertyCount + 1 );
            lDescriptor[nPropertyCount].Name = ::rtl::OUString::createFromAscii("FilterName");
            lDescriptor[nPropertyCount].Value <<= ::rtl::OUString( aFilterName );
            nPropertyCount++;
        }

        if ( pFilter->IsOwnTemplateFormat() && nIndexOfTemplateFlag == -1 )
        {
            lDescriptor.realloc( nPropertyCount + 1 );
            lDescriptor[nPropertyCount].Name = ::rtl::OUString::createFromAscii("AsTemplate");
            lDescriptor[nPropertyCount].Value <<= sal_True;
            nPropertyCount++;
        }
    }
    else
    {
        aFilterName.Erase();
        aTypeName = ::rtl::OUString();
    }

    if ( nIndexOfInputStream == -1 && xStream.is() )
    {
        // if input stream wasn't part of the descriptor, now it should be, otherwise the content would be opend twice
        lDescriptor.realloc( nPropertyCount + 1 );
        lDescriptor[nPropertyCount].Name = ::rtl::OUString::createFromAscii("InputStream");
        lDescriptor[nPropertyCount].Value <<= xStream;
        nPropertyCount++;
    }

    if ( bReadOnly != bWasReadOnly )
    {
        if ( nIndexOfReadOnlyFlag == -1 )
        {
            lDescriptor.realloc( nPropertyCount + 1 );
            lDescriptor[nPropertyCount].Name = ::rtl::OUString::createFromAscii("ReadOnly");
            lDescriptor[nPropertyCount].Value <<= bReadOnly;
            nPropertyCount++;
        }
        else
            lDescriptor[nIndexOfReadOnlyFlag].Value <<= bReadOnly;
    }

    return aTypeName;
}

SFX_IMPL_XINTERFACE_0( SfxFrameLoader_Impl, SfxFrameLoader )
SFX_IMPL_SINGLEFACTORY( SfxFrameLoader_Impl )

/* XServiceInfo */
UNOOUSTRING SAL_CALL SfxFrameLoader_Impl::getImplementationName() throw( UNORUNTIMEEXCEPTION )
{
    return impl_getStaticImplementationName();
}
                                                                                                                                \
/* XServiceInfo */
sal_Bool SAL_CALL SfxFrameLoader_Impl::supportsService( const UNOOUSTRING& sServiceName ) throw( UNORUNTIMEEXCEPTION )
{
    UNOSEQUENCE< UNOOUSTRING >  seqServiceNames =   getSupportedServiceNames();
    const UNOOUSTRING*          pArray          =   seqServiceNames.getConstArray();
    for ( sal_Int32 nCounter=0; nCounter<seqServiceNames.getLength(); nCounter++ )
    {
        if ( pArray[nCounter] == sServiceName )
        {
            return sal_True ;
        }
    }
    return sal_False ;
}

/* XServiceInfo */
UNOSEQUENCE< UNOOUSTRING > SAL_CALL SfxFrameLoader_Impl::getSupportedServiceNames() throw( UNORUNTIMEEXCEPTION )
{
    return impl_getStaticSupportedServiceNames();
}

/* Helper for XServiceInfo */
UNOSEQUENCE< UNOOUSTRING > SfxFrameLoader_Impl::impl_getStaticSupportedServiceNames()
{
    UNOMUTEXGUARD aGuard( UNOMUTEX::getGlobalMutex() );
    UNOSEQUENCE< UNOOUSTRING > seqServiceNames( 2 );
    seqServiceNames.getArray() [0] = UNOOUSTRING::createFromAscii( "com.sun.star.frame.FrameLoader" );
    seqServiceNames.getArray() [1] = UNOOUSTRING::createFromAscii( "com.sun.star.frame.ExtendedFilterDetect" );
    return seqServiceNames ;
}

/* Helper for XServiceInfo */
UNOOUSTRING SfxFrameLoader_Impl::impl_getStaticImplementationName()
{
    return UNOOUSTRING::createFromAscii( "com.sun.star.comp.office.FrameLoader" );
}

/* Helper for registry */
UNOREFERENCE< UNOXINTERFACE > SAL_CALL SfxFrameLoader_Impl::impl_createInstance( const UNOREFERENCE< UNOXMULTISERVICEFACTORY >& xServiceManager ) throw( UNOEXCEPTION )
{
    return UNOREFERENCE< UNOXINTERFACE >( *new SfxFrameLoader_Impl( xServiceManager ) );
}

SfxFrameLoader_Impl::SfxFrameLoader_Impl( const Reference < XMultiServiceFactory >& xFactory )
    : SfxFrameLoader( xFactory )
{
}


