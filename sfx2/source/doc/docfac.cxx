/*************************************************************************
 *
 *  $RCSfile: docfac.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: mba $ $Date: 2002-07-09 14:54:44 $
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

#ifndef _COM_SUN_STAR_REGISTRY_MERGECONFLICTEXCEPTION_HPP_
#include <com/sun/star/registry/MergeConflictException.hpp>
#endif
#ifndef _COM_SUN_STAR_REGISTRY_XSIMPLEREGISTRY_HPP_
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _EMBOBJ_HXX //autogen
#include <so3/embobj.hxx>
#endif

#include <vcl/config.hxx>
#include <svtools/pathoptions.hxx>
#include <svtools/moduleoptions.hxx>
#include <tools/urlobj.hxx>
#include <unotools/ucbstreamhelper.hxx>

#pragma hdrstop

#include "sfx.hrc"
#include "docfilt.hxx"
#include "docfac.hxx"
#include "viewfac.hxx"
#include "fltfnc.hxx"
#include "appdata.hxx"
#include "arrdecl.hxx"
#include "app.hxx"
#include "module.hxx"
#include "mnumgr.hxx"
#include "accmgr.hxx"
#include <sfxresid.hxx>
#include <sfxuno.hxx>
#include "doc.hrc"

//========================================================================
DECL_PTRARRAY( SfxViewFactoryArr_Impl, SfxViewFactory*, 2, 2 );

typedef SfxViewFactory* SfxViewFactoryPtr;
SV_DECL_PTRARR_DEL(SfxGlobalViewFactoryArr_Impl, SfxViewFactoryPtr, 2, 2);
SV_IMPL_PTRARR(SfxGlobalViewFactoryArr_Impl, SfxViewFactoryPtr);
//========================================================================

DBG_NAME(SfxObjectFactory);
TYPEINIT1(SfxObjectFactory,SvFactory);

static SfxObjectFactoryArr_Impl* pObjFac = 0;
static SfxGlobalViewFactoryArr_Impl* pViewFac = 0;

//========================================================================

struct SfxObjectFactory_Impl
{
    SfxViewFactoryArr_Impl      aViewFactoryArr;// Liste von <SfxViewFactory>s
    SfxFilterArr_Impl           aFilterArr;     // Liste von <SFxFilter>n
    ResId*                      pMenuBarResId;
    ResId*                      pPluginMenuBarResId;
    ResId*                      pAccelResId;
    ResId*                      pNameResId;
    String                      aHelpFile;
    String                      aHelpPIFile;
    ::rtl::OUString             aServiceName;
    sal_Bool                    bInitFactoryCalled;
    SfxVoidFunc                 pInitFactory;
    SfxFactoryFilterContainer*  pFilterContainer;
    SfxModule*                  pModule;
    SfxAcceleratorManager*      pAccMgr;
    sal_uInt16                  nImageId;
    sal_Bool                    bOwnsAccel;
    String                      aStandardTemplate;
    sal_Bool                    bTemplateInitialized;
    sal_uInt16                  nCreateNewSlotId;

    SfxObjectFactory_Impl() :
        pMenuBarResId       ( NULL ),
        pPluginMenuBarResId ( NULL ),
        pAccelResId         ( NULL ),
        pNameResId          ( NULL ),
        bInitFactoryCalled  ( sal_False ),
        pInitFactory        ( NULL ),
        pFilterContainer    ( NULL ),
        pModule             ( NULL ),
        pAccMgr             ( NULL ),
        nImageId            ( 0 ),
        bTemplateInitialized( sal_False ),
        nCreateNewSlotId    ( 0 ) {}

    ~SfxObjectFactory_Impl()
    {
        delete pMenuBarResId;
        delete pPluginMenuBarResId;
        delete pAccelResId;
        // Jetzt vom FilterMatcher
        // delete pFilterContainer;
        if ( bOwnsAccel )
            delete pAccMgr;
    }
};

//========================================================================

SfxFactoryFilterContainer* SfxObjectFactory::GetFilterContainer(
    sal_Bool bForceLoad ) const
{
    if( bForceLoad )
        ((SfxObjectFactory *)this)->DoInitFactory();
    return pImpl->pFilterContainer;
}

void SfxObjectFactory::RegisterInitFactory(SfxVoidFunc pFunc)
{
    pImpl->pInitFactory = pFunc;
    DoInitFactory();
}

void SfxObjectFactory::RegisterFilter
(
    const String&       rName,          // Klartext-Name f"ur Anzeige im Dialog
    const String&       rWildcard,      // Default-Wildcards
    SfxFilterFlags      eType,          // SFX_FILTER_...
    sal_uInt32               lFormat,        // Clipboard-Format
    const String&       rMacType,       // 4-Byte-Type f"ur Apple Macintosh
    const String&       rTypeName,      // String-Type f"ur OS/2
    sal_uInt16              nIconId,        // Resource-Id f"ur Icon
    const String&       rMimeType,
    const String&       rUserData       // zum Wiedererkennen oder (frei verwendbar)
)
{
    DBG_CHKTHIS(SfxObjectFactory, 0);
    sal_uInt16 nDemo = SFX_APP()->GetDemoKind();

    SfxFilter* pFilter = new SfxFilter(
        rName, rWildcard, eType, lFormat, rMacType,
        rTypeName, nIconId, rMimeType, pImpl->pFilterContainer, rUserData );
/*  if( nDemo != SFX_DEMOKIND_DEMO && nDemo != SFX_DEMOKIND_INVALID ||
        pFilter->IsOwnFormat() )*/
    // Jetzt immer uebernehmen, da es keine Demo mehr gibt. Sonst bei Ablauf
    // Absturz in sba
    pImpl->pFilterContainer->AddFilter(
        pFilter, pImpl->pFilterContainer->GetFilterCount() );
/*  else
        delete pFilter;*/
}

//--------------------------------------------------------------------

void SfxObjectFactory::DoInitFactory()
{
    if(!pImpl->bInitFactoryCalled)
    {
        pImpl->bInitFactoryCalled = sal_True;
        // FilterContainer Landen
        GetFilterCount( );
        (*pImpl->pInitFactory)();
        String aShortName( String::CreateFromAscii( pShortName ) );
        aShortName.ToLowerAscii();
        if ( aShortName.EqualsAscii( "swriter" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_SW );
        else if ( aShortName.EqualsAscii( "swriter/web" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_SWWEB );
        else if ( aShortName.EqualsAscii( "swriter/globaldocument" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_SWGLOB );
        else if ( aShortName.EqualsAscii( "scalc" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_SC );
        else if ( aShortName.EqualsAscii( "simpress" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_SI );
        else if ( aShortName.EqualsAscii( "sdraw" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_SD );
        else if ( aShortName.EqualsAscii( "message" ) )
            pImpl->pNameResId = new SfxResId( STR_DOCTYPENAME_MESSAGE );

        // There are no filters for "dummy" factory!
        if( pImpl->aServiceName.compareToAscii("dummy") != 0 )
        {
            DBG_ASSERT( pImpl->aServiceName.getLength(), "No service name - no filters!" )
            SfxFilterContainer *pCont = GetFilterContainer();
            pCont->ReadExternalFilters( pImpl->aServiceName );
        }
    }
}

IMPL_LINK_INLINE( SfxObjectFactory, InitFactoryHdl, void*, EMPTYARG,
{
    SFX_APP()->Get_Impl()->aPendingInitFactories.Remove( this );
    DoInitFactory();
    return 0;
}
)

sal_uInt16 SfxObjectFactory::GetFilterCount() const
{
    ((SfxObjectFactory *)this)->DoInitFactory();
    return pImpl->pFilterContainer->GetFilterCount();
}

//--------------------------------------------------------------------

const SfxFilter* SfxObjectFactory::GetFilter(sal_uInt16 i) const
{
    ((SfxObjectFactory *)this)->DoInitFactory();
    return pImpl->pFilterContainer->GetFilter( i );
}

//--------------------------------------------------------------------

SfxObjectShell *SfxObjectFactory::CreateObject(SfxObjectCreateMode eMode) const
{
    DBG_CHKTHIS(SfxObjectFactory, 0);
    return (*fnCreate)(eMode);
}

// -----------------------------------------------------------------------

void SfxObjectFactory::Construct
(
    sal_uInt16      nFactoryId,
    SfxObjectCtor   fnCreateFnc,
    SfxObjectShellFlags nFlagsP,
    const char*     pName
)
{
    DBG_CHKTHIS(SfxObjectFactory, 0);

    nFlags = nFlagsP;
    fnCreate = fnCreateFnc;
    nId = nFactoryId;
//  pIniMgr = 0;
    pShortName = pName;
    pImpl->pFilterContainer = new SfxFactoryFilterContainer(
        String::CreateFromAscii( pName ), *this );
    SFX_APP()->GetFilterMatcher().AddContainer( pImpl->pFilterContainer );
    if( !(nFlagsP & SFXOBJECTSHELL_DONTLOADFILTERS) )
        pImpl->pFilterContainer->LoadFilters( String::CreateFromAscii( pName ) );

    pImpl->aHelpFile = String::CreateFromAscii(pShortName);
    pImpl->aHelpFile.Erase( 8 );
    pImpl->aHelpPIFile = String(pImpl->aHelpFile,0,3);
    pImpl->aHelpPIFile += DEFINE_CONST_UNICODE( "hlppi" );
    pImpl->aHelpFile += DEFINE_CONST_UNICODE( ".hlp" );
    pImpl->aHelpPIFile += DEFINE_CONST_UNICODE( ".hlp" );
    pImpl->bOwnsAccel = sal_False;
}

//--------------------------------------------------------------------

SfxObjectFactory::SfxObjectFactory
(
    const SvGlobalName&     rName,
    const String&           rClassName,
    CreateInstanceType      funcCIT
)
:   SvFactory( rName, rClassName, funcCIT ),
    fnCreate( 0 ),
    nId( 0 ),
//  pIniMgr( 0 ),
    pShortName( 0 ),
    pImpl( new SfxObjectFactory_Impl )
{
    DBG_CTOR(SfxObjectFactory, 0);

    SfxApplication* pApp = SFX_APP();
    RegisterObjectFactory_Impl(*this);
    pApp->InsertLateInitHdl( LINK( this, SfxObjectFactory, InitFactoryHdl ) );
    pApp->Get_Impl()->aPendingInitFactories.Insert( this );
}

//--------------------------------------------------------------------

SfxObjectFactory::~SfxObjectFactory()
{
    DBG_DTOR(SfxObjectFactory, 0);

    const sal_uInt16 nCount = pImpl->aFilterArr.Count();
    for ( sal_uInt16 i = 0; i < nCount; ++i )
        delete pImpl->aFilterArr[i];
    delete pImpl->pNameResId;
    delete pImpl;
}

void SfxObjectFactory::RemoveAll_Impl()
{
    GetObjFacArray_Impl();
    for( USHORT n=0; n<pObjFac->Count(); )
    {
        SfxObjectFactoryPtr pFac = pObjFac->GetObject(n);
        pObjFac->Remove( n );
        delete pFac;
    }

    DELETEZ( pViewFac );
}

//--------------------------------------------------------------------

void SfxObjectFactory::RegisterViewFactory
(
    SfxViewFactory &rFactory
)
{
    sal_uInt16 nPos;
    for ( nPos = 0;
          nPos < pImpl->aViewFactoryArr.Count() &&
          pImpl->aViewFactoryArr[nPos]->GetOrdinal() <= rFactory.GetOrdinal();
          ++nPos )
    /* empty loop */;
    SfxViewFactoryPtr pFact = &rFactory;
    pImpl->aViewFactoryArr.Insert( nPos, pFact );

    if ( !pViewFac )
        pViewFac = new SfxGlobalViewFactoryArr_Impl;

    if ( !pViewFac->GetPos( pFact ) )
        pViewFac->Insert( pFact, nPos );
}

//--------------------------------------------------------------------

sal_uInt16 SfxObjectFactory::GetViewFactoryCount() const
{
    return pImpl->aViewFactoryArr.Count();
}

//--------------------------------------------------------------------

SfxViewFactory& SfxObjectFactory::GetViewFactory(sal_uInt16 i) const
{
    return *pImpl->aViewFactoryArr[i];
}

//--------------------------------------------------------------------

void SfxObjectFactory::RegisterMenuBar( const ResId& rId )
{
    delete pImpl->pMenuBarResId;
    pImpl->pMenuBarResId = new ResId( rId );
}

//--------------------------------------------------------------------

void SfxObjectFactory::RegisterPluginMenuBar( const ResId& rId )
{
    delete pImpl->pPluginMenuBarResId;
    pImpl->pPluginMenuBarResId = new ResId( rId );
}

//--------------------------------------------------------------------

const ResId* SfxObjectFactory::GetMenuBarId() const
{
    return pImpl->pMenuBarResId;
}

//--------------------------------------------------------------------

const ResId* SfxObjectFactory::GetPluginMenuBarId() const
{
    return pImpl->pPluginMenuBarResId;
}

//--------------------------------------------------------------------

const ResId* SfxObjectFactory::GetAccelId() const
{
    return pImpl->pAccelResId;
}

//--------------------------------------------------------------------

void SfxObjectFactory::RegisterAccel( const ResId& rId )
{
    DBG_ASSERT( !pImpl->pAccelResId, "SfxObjectFactory: double registration of Accel" );
    pImpl->pAccelResId = new ResId(rId);
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------

void SfxObjectFactory::RegisterHelpFile( const String& rString )
{
    pImpl->aHelpFile = rString;
}

//--------------------------------------------------------------------

const String& SfxObjectFactory::GetHelpFile() const
{
    return pImpl->aHelpFile;
}

//--------------------------------------------------------------------

void SfxObjectFactory::RegisterHelpPIFile( const String& rString )
{
    pImpl->aHelpPIFile = rString;
}

//--------------------------------------------------------------------

const String& SfxObjectFactory::GetHelpPIFile() const
{
    return pImpl->aHelpPIFile;
}

SfxModule* SfxObjectFactory::GetModule() const
{
    return pImpl->pModule;
}

SfxAcceleratorManager* SfxObjectFactory::GetAccMgr_Impl()
{
    if ( !pImpl->pAccMgr && pImpl->pAccelResId )
    {
        // factories in the same module may share their accelerators
        SfxApplication *pApp = SFX_APP();
        SfxObjectFactoryArr_Impl& rArr = GetObjFacArray_Impl();
        sal_uInt32 nCount = rArr.Count();
        const ResId& rMyId = *GetAccelId();
        for ( sal_uInt32 n=0; n<nCount; n++ )
        {
            SfxObjectFactory *pFact = rArr[(sal_uInt16)n];
            if ( pFact == this )
                break;
            const ResId *pId = pFact->pImpl->pAccelResId;
            if ( pId &&
                pId->GetId() == rMyId.GetId() &&
                pId->GetResMgr() == rMyId.GetResMgr() )
            {
                pImpl->pAccMgr = pFact->GetAccMgr_Impl();
                return pImpl->pAccMgr;
            }
        }

        // create accelerator manager
        pImpl->pAccMgr = new SfxAcceleratorManager( rMyId, SFX_APP()->GetConfigManager_Impl() );
        pImpl->bOwnsAccel = sal_True;
    }

    return pImpl->pAccMgr;
}

void SfxObjectFactory::SetModule_Impl( SfxModule *pMod )
{
    pImpl->pModule = pMod;
}

void SfxObjectFactory::SetExplorerImageId( sal_uInt16 nImageId )
{
    pImpl->nImageId = nImageId;
}

sal_uInt16 SfxObjectFactory::GetExplorerImageId() const
{
    return pImpl->nImageId;
}

void SfxObjectFactory::SetStandardTemplate( const String& rFactoryURL, const String& rTemplate )
{
    const SfxObjectFactory* pFactory = SfxObjectFactory::GetFactory( rFactoryURL );
    if ( pFactory )
    {
        ((SfxObjectFactory*)pFactory)->pImpl->aStandardTemplate = rTemplate;
        SvtModuleOptions aModOpt;
        SvtModuleOptions::EFactory eFac = SvtModuleOptions::E_WRITER;
        if ( SvtModuleOptions::ClassifyFactoryByName( pFactory->GetDocumentServiceName(), eFac ) )
            aModOpt.SetFactoryStandardTemplate( eFac, rTemplate );
    }
}

const String& SfxObjectFactory::GetStandardTemplate() const
{
    if ( !pImpl->bTemplateInitialized )
    {
        pImpl->bTemplateInitialized = sal_True;
        SvtModuleOptions aModOpt;
        SvtModuleOptions::EFactory eFac = SvtModuleOptions::E_WRITER;
        if ( SvtModuleOptions::ClassifyFactoryByName( GetDocumentServiceName(), eFac ) )
            pImpl->aStandardTemplate = aModOpt.GetFactoryStandardTemplate( eFac );
    }

    return pImpl->aStandardTemplate;
}

const SfxObjectFactory* SfxObjectFactory::GetFactory( const String& rFactoryURL )
{
    const SfxObjectFactory* pFactory = 0;
    String aFact( rFactoryURL );
    String aPrefix( DEFINE_CONST_UNICODE( "private:factory/" ) );
    if ( aPrefix.Len() == aFact.Match( aPrefix ) )
        // Aufruf m"oglich mit z.B. "swriter" oder "private:factory/swriter"
        aFact.Erase( 0, aPrefix.Len() );
    sal_uInt16 nPos = aFact.Search( '?' );

    // Etwaige Parameter abschneiden
    aFact.Erase( nPos, aFact.Len() );

    SfxApplication *pApp = SFX_APP();

    // "swriter4" durch "swriter" ersetzen, zum Vergleichen uppercase verwenden
    WildCard aSearchedFac( aFact.EraseAllChars('4').ToUpperAscii() );
    for( sal_uInt16 n = GetObjectFactoryCount_Impl(); !pFactory && n--; )
    {
        pFactory = &GetObjectFactory_Impl( n );
        String aCompareTo = String::CreateFromAscii( pFactory->GetShortName() );
        aCompareTo.ToUpperAscii();
        if( !aSearchedFac.Matches( aCompareTo ) )
            pFactory = 0;
    }

    return pFactory;
}

const SfxFilter* SfxObjectFactory::GetTemplateFilter() const
{
    sal_uInt16 nCount = GetFilterCount();
    for ( sal_uInt16 n=0; n<nCount; n++ )
    {
        const SfxFilter *pFilter = pImpl->pFilterContainer->GetFilter(n);
        if ( pFilter->GetFilterFlags() & SFX_FILTER_TEMPLATE)
            return pFilter;
    }

    return 0;
}

void SfxObjectFactory::SetCreateNewSlotId( sal_uInt16 nId )
{
    pImpl->nCreateNewSlotId = nId;
}

sal_uInt16 SfxObjectFactory::GetCreateNewSlotId() const
{
    return pImpl->nCreateNewSlotId;
}

void SfxObjectFactory::SetDocumentTypeNameResource( const ResId& rId )
{
    DBG_ASSERT( !pImpl->pNameResId, "UI-Namensresource mehrfach gesetzt!" );
    pImpl->pNameResId = new ResId( rId );
}

String SfxObjectFactory::GetDocumentTypeName() const
{
    ((SfxObjectFactory*)this)->DoInitFactory();
    if ( pImpl->pNameResId )
        return String( *pImpl->pNameResId );
    return String();
}

void SfxObjectFactory::SetDocumentServiceName( const ::rtl::OUString& rServiceName )
{
    pImpl->aServiceName = rServiceName;
}

const ::rtl::OUString& SfxObjectFactory::GetDocumentServiceName() const
{
    return pImpl->aServiceName;
}

void SfxObjectFactory::RegisterObjectFactory_Impl( SfxObjectFactory &rFac )
{
    SfxObjectFactory *pFac = &rFac;
    GetObjFacArray_Impl().Insert( pFac, GetObjFacArray_Impl().Count() );
}

//--------------------------------------------------------------------

USHORT SfxObjectFactory::GetObjectFactoryCount_Impl()
{
    return GetObjFacArray_Impl().Count();
}

//--------------------------------------------------------------------

const SfxObjectFactory &SfxObjectFactory::GetObjectFactory_Impl(USHORT i)
{
    return *GetObjFacArray_Impl()[i];
}

const SfxObjectFactory& SfxObjectFactory::GetDefaultFactory()
{
    const SfxObjectFactory &rFactory = GetObjectFactory_Impl(0);
    return rFactory;
}

sal_Bool SfxObjectFactory::HasObjectFactories()
{
    return ( GetObjFacArray_Impl().Count() != 0 );
}

SfxObjectFactoryArr_Impl& SfxObjectFactory::GetObjFacArray_Impl()
{
    if ( !pObjFac )
        pObjFac = new SfxObjectFactoryArr_Impl;
    return *pObjFac;
}

String SfxObjectFactory::GetModuleName() const
{
    SvtModuleOptions::EFactory eFac = SvtModuleOptions::E_WRITER;
    if ( SvtModuleOptions::ClassifyFactoryByName( GetDocumentServiceName(), eFac ) )
        return SvtModuleOptions().GetModuleName( eFac );
    else
        return String();
}
