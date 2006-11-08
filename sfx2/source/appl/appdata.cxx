/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: appdata.cxx,v $
 *
 *  $Revision: 1.24 $
 *
 *  last change: $Author: kz $ $Date: 2006-11-08 11:57:01 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sfx2.hxx"
#ifndef _CACHESTR_HXX //autogen
#include <tools/cachestr.hxx>
#endif
#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif
#ifndef _INETSTRM_HXX //autogen
#include <svtools/inetstrm.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif

#define _SVSTDARR_STRINGS
#include <svtools/svstdarr.hxx>
#include <vos/mutex.hxx>

#include <vcl/menu.hxx>

#ifndef _LOGINERR_HXX
#include <svtools/loginerr.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _DATETIMEITEM_HXX //autogen
#include <svtools/dateitem.hxx>
#endif
#ifndef _SV_MENU_HXX
#include <vcl/menu.hxx>
#endif
#ifndef _SV_WRKWIN_HXX
#include <vcl/wrkwin.hxx>
#endif
#include "comphelper/processfactory.hxx"

#include "viewfrm.hxx"
#include "appdata.hxx"
#include "dispatch.hxx"
#include "event.hxx"
#include "sfxtypes.hxx"
#include "doctempl.hxx"
#include "arrdecl.hxx"
#include "docfac.hxx"
#include "docfile.hxx"
#include "request.hxx"
#include "referers.hxx"
#include "app.hrc"
#include "sfxresid.hxx"
#include "objshimp.hxx"
#include "appuno.hxx"
#include "imestatuswindow.hxx"
#include "appbaslib.hxx"

#include <basic/basicmanagerrepository.hxx>
#include <basic/basmgr.hxx>

using ::basic::BasicManagerRepository;
using ::basic::BasicManagerCreationListener;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::frame::XModel;

class SfxBasicManagerCreationListener : public ::basic::BasicManagerCreationListener
{
private:
    SfxAppData_Impl& m_rAppData;

public:
    SfxBasicManagerCreationListener( SfxAppData_Impl& _rAppData ) :m_rAppData( _rAppData ) { }

    virtual void onBasicManagerCreated( const Reference< XModel >& _rxForDocument, BasicManager& _rBasicManager );
};

void SfxBasicManagerCreationListener::onBasicManagerCreated( const Reference< XModel >& _rxForDocument, BasicManager& _rBasicManager )
{
    if ( _rxForDocument == NULL )
        m_rAppData.OnApplicationBasicManagerCreated( _rBasicManager );
}

SfxAppData_Impl::SfxAppData_Impl( SfxApplication* pApp ) :
        pDdeService( 0 ),
        pDocTopics( 0 ),
        pTriggerTopic(0),
        pDdeService2(0),
        pFactArr(0),
        pTopFrames( new SfxFrameArr_Impl ),
        pInitLinkList(0),
        pMatcher( 0 ),
        pCancelMgr( 0 ),
        pLabelResMgr( 0 ),
        pAppDispatch(NULL),
        pTemplates( 0 ),
        pPool(0),
        pEventConfig(0),
        pDisabledSlotList( 0 ),
        pSecureURLs(0),
        pMiscConfig(0),
        pSaveOptions( 0 ),
        pUndoOptions( 0 ),
        pHelpOptions( 0 ),
        pThisDocument(0),
        pProgress(0),
        pTemplateCommon( 0 ),
        nDocModalMode(0),
        nAutoTabPageId(0),
        nBasicCallLevel(0),
        nRescheduleLocks(0),
        nInReschedule(0),
        nAsynchronCalls(0),
        m_xImeStatusWindow(new sfx2::appl::ImeStatusWindow(
                               *pApp, comphelper::getProcessServiceFactory()))
    , pTbxCtrlFac(0)
    , pStbCtrlFac(0)
    , pViewFrames(0)
    , pObjShells(0)
    , pSfxResManager(0)
    , pOfaResMgr(0)
    , pSimpleResManager(0)
    , pBasicManager( new SfxBasicManagerHolder )
    , pBasMgrListener( new SfxBasicManagerCreationListener( *this ) )
    , pViewFrame( 0 )
    , pSlotPool( 0 )
    , pResMgr( 0 )
    , pAppDispat( 0 )
    , pInterfaces( 0 )
    , nDocNo(0)
    , nInterfaces( 0 )
    , bDowning( sal_True ),
        bInQuit(sal_False),
        bInvalidateOnUnlock(sal_False)
{
    BasicManagerRepository::registerCreationListener( *pBasMgrListener );
}

SfxAppData_Impl::~SfxAppData_Impl()
{
    DeInitDDE();
    delete pTopFrames;
    delete pCancelMgr;
    delete pSecureURLs;
    delete pBasicManager;

    BasicManagerRepository::revokeCreationListener( *pBasMgrListener );
    delete pBasMgrListener;
}

IMPL_STATIC_LINK( SfxAppData_Impl, CreateDocumentTemplates, void*, EMPTYARG)
{
    pThis->GetDocumentTemplates();
    return 0;
}

void SfxAppData_Impl::UpdateApplicationSettings( sal_Bool bDontHide )
{
    AllSettings aAllSet = Application::GetSettings();
    StyleSettings aStyleSet = aAllSet.GetStyleSettings();
    sal_uInt32 nStyleOptions = aStyleSet.GetOptions();
    if ( bDontHide )
        nStyleOptions &= ~STYLE_OPTION_HIDEDISABLED;
    else
        nStyleOptions |= STYLE_OPTION_HIDEDISABLED;
    aStyleSet.SetOptions( nStyleOptions );
    aAllSet.SetStyleSettings( aStyleSet );
    Application::SetSettings( aAllSet );
}

SfxDocumentTemplates* SfxAppData_Impl::GetDocumentTemplates()
{
    if ( !pTemplates )
        pTemplates = new SfxDocumentTemplates;
    else
        pTemplates->ReInitFromComponent();
    return pTemplates;
}

void SfxAppData_Impl::OnApplicationBasicManagerCreated( BasicManager& _rBasicManager )
{
    pBasicManager->reset( &_rBasicManager );

    // global constants, additionally to the ones already added by createApplicationBasicManager:
    // ThisComponent
    Reference< XModel > xThisComponent;
    SfxObjectShell* pDoc = SfxObjectShell::Current();
    if ( pDoc )
        xThisComponent = pDoc->GetModel();
    _rBasicManager.InsertGlobalUNOConstant( "ThisComponent", makeAny( xThisComponent ) );
    pThisDocument = pDoc;
}
