/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fmshell.cxx,v $
 *
 *  $Revision: 1.68 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 05:09:08 $
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
#include "precompiled_svx.hxx"

#ifndef _SVX_FMVWIMP_HXX
#include "fmvwimp.hxx"
#endif

#ifndef _SVX_FMSHELL_HXX
#include "fmshell.hxx"
#endif
#ifndef _SVX_FMTOOLS_HXX
#include "fmtools.hxx"
#endif
#ifndef _SVX_FMSERVS_HXX
#include "fmservs.hxx"
#endif
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef _SVX_FMUNOPGE_HXX
#include "fmpgeimp.hxx"
#endif
#ifndef _SVX_FMITEMS_HXX
#include "fmitems.hxx"
#endif
#ifndef _SVX_FMUNDO_HXX
#include "fmundo.hxx"
#endif

#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif

#ifndef _COM_SUN_STAR_FORM_XLOADABLE_HPP_
#include <com/sun/star/form/XLoadable.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_SDDB_PRIVILEGE_HPP_
#include <com/sun/star/sdbcx/Privilege.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XMULTIPROPERTYSET_HPP_
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XFASTPROPERTYSET_HPP_
#include <com/sun/star/beans/XFastPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XEXECUTABLEDIALOG_HPP_
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLERMODEL_HPP_
#include <com/sun/star/awt/XTabControllerModel.hpp>
#endif

#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif

#ifndef _SV_WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif

#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif

#ifndef _SFX_WHITER_HXX //autogen
#include <svtools/whiter.hxx>
#endif

#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif

#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXVISIBILITYITEM_HXX
#include <svtools/visitem.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_MODULEOPTIONS_HXX
#include <svtools/moduleoptions.hxx>
#endif
#ifndef _SFXOBJFACE_HXX //autogen
#include <sfx2/objface.hxx>
#endif

#ifndef _SFXREQUEST_HXX //autogen
#include <sfx2/request.hxx>
#endif

#ifndef _SFXDISPATCH_HXX //autogen
#include <sfx2/dispatch.hxx>
#endif

#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif

#ifndef _SVDOBJ_HXX //autogen
#include "svdobj.hxx"
#endif

#ifndef _SVX_FMPAGE_HXX
#include "fmpage.hxx"
#endif

#ifndef _SVDITER_HXX //autogen
#include "svditer.hxx"
#endif

#ifndef _SVX_FMOBJ_HXX
#include "fmobj.hxx"
#endif

#ifndef _SVX_SVXIDS_HRC
#include "svxids.hrc"
#endif

#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif

#ifndef _SVX_FMEXCH_HXX
#include "fmexch.hxx"
#endif

#ifndef _SVX_FMGLOB_HXX
#include "fmglob.hxx"
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SVDPAGE_HXX //autogen
#include <svdpage.hxx>
#endif

#ifndef _SVX_FMMODEL_HXX
#include "fmmodel.hxx"
#endif

#ifndef _SVX_DIALMGR_HXX
#include "dialmgr.hxx"
#endif

#ifndef _SVX_FMSHIMP_HXX
#include "fmshimp.hxx"
#endif

#ifndef _SVDPAGV_HXX //autogen
#include "svdpagv.hxx"
#endif

#ifndef _SFX_OBJITEM_HXX //autogen
#include <sfx2/objitem.hxx>
#endif

#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif

#ifndef _SV_SOUND_HXX //autogen
#include <vcl/sound.hxx>
#endif

#ifndef _SVX_FMEXPL_HXX
#include "fmexpl.hxx"
#endif
#ifndef SVX_FORMCONTROLLING_HXX
#include "formcontrolling.hxx"
#endif

#ifndef _NUMUNO_HXX //autogen
#include <svtools/numuno.hxx>
#endif

#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef SVX_SOURCE_INC_FMDOCUMENTCLASSIFICATION_HXX
#include "fmdocumentclassification.hxx"
#endif
#ifndef SVX_SOURCE_INC_FORMTOOLBARS_HXX
#include "formtoolbars.hxx"
#endif

#include "svxdlg.hxx" //CHINA001
#include <dialogs.hrc> //CHINA001

#define HANDLE_SQL_ERRORS( action, successflag, context, message )          \
    try                                                                     \
    {                                                                       \
        successflag = sal_False;                                                \
        action;                                                             \
        successflag = sal_True;                                                 \
    }                                                                       \
    catch(::com::sun::star::sdbc::SQLException& e)                                                  \
    {                                                                       \
        ::com::sun::star::sdb::SQLContext eExtendedInfo =                                           \
        GetImpl()->prependContextInfo(e, Reference< XInterface > (), context, ::rtl::OUString());              \
        displayException(eExtendedInfo);                                    \
    }                                                                       \
    catch(Exception&)                                                           \
    {                                                                       \
        DBG_ERROR(message);                                                 \
    }                                                                       \


#define DO_SAFE_WITH_ERROR( action, message ) try { action; } catch(Exception&) { DBG_ERROR(message); }

#define FmFormShell
#include "svxslots.hxx"

#ifndef _SVX_SVXIDS_HRC
#include <svxids.hrc>
#endif

#ifndef _SVX_TBXFORM_HXX
#include "tbxform.hxx"
#endif

#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

// wird fuer Invalidate verwendet -> mitpflegen
// aufsteigend sortieren !!!!!!
sal_uInt16 ControllerSlotMap[] =    // slots des Controllers
{
    SID_FM_CONFIG,
    SID_FM_PUSHBUTTON,
    SID_FM_RADIOBUTTON,
    SID_FM_CHECKBOX,
    SID_FM_FIXEDTEXT,
    SID_FM_GROUPBOX,
    SID_FM_EDIT,
    SID_FM_LISTBOX,
    SID_FM_COMBOBOX,
    SID_FM_DBGRID,
    SID_FM_IMAGEBUTTON,
    SID_FM_FILECONTROL,
    SID_FM_NAVIGATIONBAR,
    SID_FM_CTL_PROPERTIES,
    SID_FM_PROPERTIES,
    SID_FM_TAB_DIALOG,
    SID_FM_ADD_FIELD,
    SID_FM_DESIGN_MODE,
    SID_FM_SHOW_FMEXPLORER,
    SID_FM_SHOW_PROPERTIES,
    SID_FM_FMEXPLORER_CONTROL,
    SID_FM_DATEFIELD,
    SID_FM_TIMEFIELD,
    SID_FM_NUMERICFIELD,
    SID_FM_CURRENCYFIELD,
    SID_FM_PATTERNFIELD,
    SID_FM_OPEN_READONLY,
    SID_FM_IMAGECONTROL,
    SID_FM_USE_WIZARDS,
    SID_FM_FORMATTEDFIELD,
    SID_FM_FILTER_NAVIGATOR,
    SID_FM_AUTOCONTROLFOCUS,
    SID_FM_SCROLLBAR,
    SID_FM_SPINBUTTON,
    SID_FM_SHOW_DATANAVIGATOR,
    SID_FM_DATANAVIGATOR_CONTROL,

    0
};

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::frame;
using namespace ::svxform;

//========================================================================
//------------------------------------------------------------------------
sal_Bool IsFormComponent( const SdrObject& rObj )
{
    //////////////////////////////////////////////////////////////////////
    // Gruppenobjekte rekursiv pruefen
    if( rObj.IsGroupObject() )
    {
        SdrObject* pObj;
        SdrObjListIter aIter( *rObj.GetSubList() );

        while( aIter.IsMore() )
        {
            pObj = aIter.Next();

            if( !IsFormComponent(*pObj) )
                return sal_False;
        }
    }

    //////////////////////////////////////////////////////////////////
    // ...ansonsten Pruefen, ob SdrObj eine FormComponent ist
    else
    {
        if( !rObj.ISA(SdrUnoObj) )
            return sal_False;

        Reference< ::com::sun::star::form::XFormComponent >  xFormComponent(((SdrUnoObj*)&rObj)->GetUnoControlModel(), UNO_QUERY);
        if (!xFormComponent.is())
            return sal_False;
    }

    return sal_True;
}

//------------------------------------------------------------------------
sal_Bool IsFormComponentList( const SdrMarkList& rMarkList )
{
    sal_uInt32 nMarkCount = rMarkList.GetMarkCount();

    if( nMarkCount==0 )
        return sal_False;

    SdrObject* pObj;
    for( sal_uInt32 i=0; i<nMarkCount; ++i )
    {
        pObj = rMarkList.GetMark(i)->GetMarkedSdrObj();
        if( !IsFormComponent(*pObj) )
            return sal_False;
    }

    return sal_True;
}

//------------------------------------------------------------------------
sal_Bool IsFormComponentList( const SdrObjList& rSdrObjList )
{
    SdrObject* pSdrObj;


    SdrObjListIter aIter( rSdrObjList );
    while( aIter.IsMore() )
    {
        pSdrObj = aIter.Next();

        //////////////////////////////////////////////////////////////////
        // Gruppenobjekte rekursiv pruefen
        if( pSdrObj->IsGroupObject() )
        {
            if( !IsFormComponentList(*pSdrObj->GetSubList()) )
                return sal_False;
        }

        //////////////////////////////////////////////////////////////////
        // ...ansonsten Pruefen, ob SdrObj eine FormComponent ist
        else
        {
            if (!pSdrObj->ISA(SdrUnoObj))
                return sal_False;

            Reference< ::com::sun::star::form::XFormComponent >  xFormComponent(((SdrUnoObj*)pSdrObj)->GetUnoControlModel(), UNO_QUERY);
            if (!xFormComponent.is())
                return sal_False;
        }
    }

    return sal_True;
}

#if SUPD>500
//========================================================================
// class FmDesignModeChangedHint
//========================================================================
TYPEINIT1( FmDesignModeChangedHint, SfxHint );

//------------------------------------------------------------------------
FmDesignModeChangedHint::FmDesignModeChangedHint( sal_Bool bDesMode )
    :m_bDesignMode( bDesMode )
{
}

//------------------------------------------------------------------------
FmDesignModeChangedHint::~FmDesignModeChangedHint()
{
}
#endif

//========================================================================
const sal_uInt32 FM_UI_FEATURE_SHOW_DATABASEBAR         = 0x00000001;
const sal_uInt32 FM_UI_FEATURE_SHOW_FIELD               = 0x00000002;
const sal_uInt32 FM_UI_FEATURE_SHOW_PROPERTIES          = 0x00000004;
const sal_uInt32 FM_UI_FEATURE_SHOW_EXPLORER            = 0x00000008;
const sal_uInt32 FM_UI_FEATURE_SHOW_FILTERBAR           = 0x00000010;
const sal_uInt32 FM_UI_FEATURE_SHOW_FILTERNAVIGATOR     = 0x00000020;
const sal_uInt32 FM_UI_FEATURE_SHOW_TEXT_CONTROL_BAR    = 0x00000040;
const sal_uInt32 FM_UI_FEATURE_TB_CONTROLS              = 0x00000080;
const sal_uInt32 FM_UI_FEATURE_TB_MORECONTROLS          = 0x00000100;
const sal_uInt32 FM_UI_FEATURE_TB_FORMDESIGN            = 0x00000200;
const sal_uInt32 FM_UI_FEATURE_SHOW_DATANAVIGATOR       = 0x00000400;

SFX_IMPL_INTERFACE(FmFormShell, SfxShell, SVX_RES(RID_STR_FORMSHELL))
{
    SFX_FEATURED_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_NAVIGATION|SFX_VISIBILITY_STANDARD|SFX_VISIBILITY_READONLYDOC,
        SVX_RES(RID_SVXTBX_FORM_NAVIGATION),
        FM_UI_FEATURE_SHOW_DATABASEBAR );

    SFX_FEATURED_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_NAVIGATION|SFX_VISIBILITY_STANDARD|SFX_VISIBILITY_READONLYDOC,
        SVX_RES(RID_SVXTBX_FORM_FILTER),
        FM_UI_FEATURE_SHOW_FILTERBAR );

    SFX_FEATURED_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT | SFX_VISIBILITY_STANDARD | SFX_VISIBILITY_READONLYDOC,
        SVX_RES( RID_SVXTBX_TEXT_CONTROL_ATTRIBUTES ),
        FM_UI_FEATURE_SHOW_TEXT_CONTROL_BAR );

    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_FM_ADD_FIELD, FM_UI_FEATURE_SHOW_FIELD);
    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_FM_SHOW_PROPERTIES, FM_UI_FEATURE_SHOW_PROPERTIES);
    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_FM_SHOW_FMEXPLORER, FM_UI_FEATURE_SHOW_EXPLORER);
    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_FM_FILTER_NAVIGATOR, FM_UI_FEATURE_SHOW_FILTERNAVIGATOR);
    SFX_FEATURED_CHILDWINDOW_REGISTRATION(SID_FM_SHOW_DATANAVIGATOR, FM_UI_FEATURE_SHOW_DATANAVIGATOR);

    SFX_FEATURED_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT | SFX_VISIBILITY_STANDARD,
        SVX_RES( RID_SVXTBX_CONTROLS ),
        FM_UI_FEATURE_TB_CONTROLS );

    SFX_FEATURED_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT | SFX_VISIBILITY_STANDARD,
        SVX_RES( RID_SVXTBX_MORECONTROLS ),
        FM_UI_FEATURE_TB_MORECONTROLS );

    SFX_FEATURED_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT | SFX_VISIBILITY_STANDARD,
        SVX_RES( RID_SVXTBX_FORMDESIGN ),
        FM_UI_FEATURE_TB_FORMDESIGN );
}

//========================================================================
TYPEINIT1(FmFormShell,SfxShell)

//------------------------------------------------------------------------
FmFormShell::FmFormShell( SfxViewShell* _pParent, FmFormView* pView )
            :SfxShell(_pParent)
            ,m_pImpl(new FmXFormShell(this, _pParent->GetViewFrame()))
            ,m_pFormView( pView )
            ,m_pFormModel( NULL )
            ,m_pParentShell(_pParent)
            ,m_nLastSlot( 0 )
            ,m_bDesignMode( sal_True )
            ,m_bHasForms(sal_False)
{
    m_pImpl->acquire();
    SetPool( &SFX_APP()->GetPool() );
    SetName( String::CreateFromAscii( "Form" ) );

    SetView(m_pFormView);
}

//------------------------------------------------------------------------
FmFormShell::~FmFormShell()
{
    if (m_pFormView)
        GetImpl()->ResetForms();

    m_pImpl->dispose();
    m_pImpl->release();

    if (m_pFormView)
    {
        m_pFormView->SetFormShell(NULL);
        m_pFormView  = NULL;
    }


    // reset the model after disposing the shell !
    m_pFormModel = NULL;
}

//------------------------------------------------------------------------
void FmFormShell::NotifyMarkListChanged(FmFormView* pWhichView)
{
    FmNavViewMarksChanged aChangeNotification(pWhichView);
    Broadcast(aChangeNotification);
}

//------------------------------------------------------------------------
sal_uInt16 FmFormShell::PrepareClose(sal_Bool bUI, sal_Bool bForBrowsing)
{
    if ( GetImpl()->didPrepareClose() )
        // we already did a PrepareClose for the current modifications of the current form
        // 2002-11-12 #104702# - fs@openoffice.org
        return sal_True;

    sal_Bool bResult = sal_True;
    // Save the data records, not in DesignMode and FilterMode
    if (!m_bDesignMode && !GetImpl()->isInFilterMode() &&
        m_pFormView && m_pFormView->GetActualOutDev() &&
        m_pFormView->GetActualOutDev()->GetOutDevType() == OUTDEV_WINDOW)
    {
        if (GetImpl()->HasAnyPendingCursorAction())
            GetImpl()->CancelAnyPendingCursorAction();

        SdrPageView* pCurPageView = m_pFormView->GetPageViewPvNum(0);

        // sal_uInt16 nPos = pCurPageView ? pCurPageView->GetWinList().Find((OutputDevice*)m_pFormView->GetActualOutDev()) : SDRPAGEVIEWWIN_NOTFOUND;
        SdrPageViewWindow* pWindow = pCurPageView ? pCurPageView->FindWindow(*((OutputDevice*)m_pFormView->GetActualOutDev())) : 0L;

        if(pWindow)
        {
            // Zunaechst werden die aktuellen Inhalte der Controls gespeichert
            // Wenn alles glatt gelaufen ist, werden die modifizierten Datensaetze gespeichert
            if ( GetImpl()->getActiveController().is() )
            {
                const ::svx::ControllerFeatures& rController = GetImpl()->getActiveControllerFeatures();
                if ( rController->commitCurrentControl() )
            {
                    sal_Bool bModified = rController->isModifiedRecord();

                if ( bModified && bUI )
                {
                    QueryBox aQry(NULL, SVX_RES(RID_QRY_SAVEMODIFIED));
                    if (bForBrowsing)
                        aQry.AddButton(ResId(RID_STR_NEW_TASK), RET_NEWTASK,
                            BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON);

                    switch (aQry.Execute())
                    {
                        case RET_NO:
                            bModified = sal_False;
                            GetImpl()->didPrepareClose( sal_True );
                            break;

                        case RET_CANCEL:
                            return sal_False;

                        case RET_NEWTASK:
                            return RET_NEWTASK;
                    }

                        if ( bModified )
                            bResult = rController->commitCurrentRecord( );
                }
            }
        }
    }
    }
    return bResult;
}

//------------------------------------------------------------------------
void FmFormShell::SetDesignMode(sal_Bool bDesign)
{
    if (m_pFormView)
    {
        if (!bDesign)
            m_nLastSlot = SID_FM_DESIGN_MODE;

        GetImpl()->SetDesignMode(bDesign);
        // mein m_bDesignMode wird auch von der Impl gesetzt ...
    }
    else
    {
        m_bHasForms = sal_False;
        m_bDesignMode = bDesign;
        UIFeatureChanged();
    }

    GetViewShell()->GetViewFrame()->GetBindings().Invalidate(ControllerSlotMap);
}

//------------------------------------------------------------------------
sal_Bool FmFormShell::HasUIFeature( sal_uInt32 nFeature )
{
    sal_Bool bResult = sal_False;
    if ((nFeature & FM_UI_FEATURE_SHOW_DATABASEBAR) == FM_UI_FEATURE_SHOW_DATABASEBAR)
    {
        // nur wenn auch formulare verfuegbar
        bResult = !m_bDesignMode && GetImpl()->hasDatabaseBar() && !GetImpl()->isInFilterMode();
    }
    else if ((nFeature & FM_UI_FEATURE_SHOW_FILTERBAR) == FM_UI_FEATURE_SHOW_FILTERBAR)
    {
        // nur wenn auch formulare verfuegbar
        bResult = !m_bDesignMode && GetImpl()->hasDatabaseBar() && GetImpl()->isInFilterMode();
    }
    else if ((nFeature & FM_UI_FEATURE_SHOW_FILTERNAVIGATOR) == FM_UI_FEATURE_SHOW_FILTERNAVIGATOR)
    {
        bResult = !m_bDesignMode && GetImpl()->hasDatabaseBar() && GetImpl()->isInFilterMode();
    }
    else if ((nFeature & FM_UI_FEATURE_SHOW_FIELD) == FM_UI_FEATURE_SHOW_FIELD)
    {
        bResult = m_bDesignMode && m_pFormView && m_bHasForms;
    }
    else if ((nFeature & FM_UI_FEATURE_SHOW_PROPERTIES) == FM_UI_FEATURE_SHOW_PROPERTIES)
    {
        bResult = m_bDesignMode && m_pFormView && m_bHasForms;
    }
    else if ((nFeature & FM_UI_FEATURE_SHOW_EXPLORER) == FM_UI_FEATURE_SHOW_EXPLORER)
    {
        bResult = m_bDesignMode; // OJ #101593# && m_pFormView && m_bHasForms;
    }
    else if ( ( nFeature & FM_UI_FEATURE_SHOW_TEXT_CONTROL_BAR ) == FM_UI_FEATURE_SHOW_TEXT_CONTROL_BAR )
    {
        bResult = m_pImpl->IsActiveControl( true );
    }
    else if ((nFeature & FM_UI_FEATURE_SHOW_DATANAVIGATOR) == FM_UI_FEATURE_SHOW_DATANAVIGATOR)
    {
        bResult = GetImpl()->isEnhancedForm();
    }
    else if (  ( ( nFeature & FM_UI_FEATURE_TB_CONTROLS ) == FM_UI_FEATURE_TB_CONTROLS )
            || ( ( nFeature & FM_UI_FEATURE_TB_MORECONTROLS ) == FM_UI_FEATURE_TB_MORECONTROLS )
            || ( ( nFeature & FM_UI_FEATURE_TB_FORMDESIGN ) == FM_UI_FEATURE_TB_FORMDESIGN )
            )
    {
        bResult = sal_True;
    }

    return bResult;
}

//------------------------------------------------------------------------
void FmFormShell::Execute(SfxRequest &rReq)
{
    sal_uInt16 nSlot = rReq.GetSlot();

    //////////////////////////////////////////////////////////////////////
    // MasterSlot setzen
    switch( nSlot )
    {
        case SID_FM_PUSHBUTTON:
        case SID_FM_RADIOBUTTON:
        case SID_FM_CHECKBOX:
        case SID_FM_FIXEDTEXT:
        case SID_FM_GROUPBOX:
        case SID_FM_LISTBOX:
        case SID_FM_COMBOBOX:
        case SID_FM_NAVIGATIONBAR:
        case SID_FM_EDIT:
        case SID_FM_DBGRID:
        case SID_FM_IMAGEBUTTON:
        case SID_FM_IMAGECONTROL:
        case SID_FM_FILECONTROL:
        case SID_FM_DATEFIELD:
        case SID_FM_TIMEFIELD:
        case SID_FM_NUMERICFIELD:
        case SID_FM_CURRENCYFIELD:
        case SID_FM_PATTERNFIELD:
        case SID_FM_FORMATTEDFIELD:
        case SID_FM_SCROLLBAR:
        case SID_FM_SPINBUTTON:
            m_nLastSlot = nSlot;
            GetViewShell()->GetViewFrame()->GetBindings().Invalidate( SID_FM_CONFIG );
            break;
        case SID_FM_DESIGN_MODE:    // gibt es was zu tun?
        {
            // if we are moving our data source cursor currently ....
            if (GetImpl()->HasAnyPendingCursorAction())
                // ... cancel this
                GetImpl()->CancelAnyPendingCursorAction();

            SFX_REQUEST_ARG(rReq, pDesignItem, SfxBoolItem, nSlot, sal_False);
            if (!pDesignItem || pDesignItem->GetValue() != m_bDesignMode)
            {
                m_nLastSlot = nSlot;
                GetViewShell()->GetViewFrame()->GetBindings().Invalidate( SID_FM_CONFIG );
            }
            else
            {
                rReq.Done();
                return;
            }
        }   break;
    }

    //////////////////////////////////////////////////////////////////////
    // Identifier und Inventor des Uno-Controls setzen
    sal_uInt16 nIdentifier = 0;
    switch( nSlot )
    {
        case SID_FM_CHECKBOX:
            nIdentifier = OBJ_FM_CHECKBOX;
            break;
        case SID_FM_PUSHBUTTON:
            nIdentifier = OBJ_FM_BUTTON;
            break;
        case SID_FM_FIXEDTEXT:
            nIdentifier = OBJ_FM_FIXEDTEXT;
            break;
        case SID_FM_LISTBOX:
            nIdentifier = OBJ_FM_LISTBOX;
            break;
        case SID_FM_EDIT:
            nIdentifier = OBJ_FM_EDIT;
            break;
        case SID_FM_RADIOBUTTON:
            nIdentifier = OBJ_FM_RADIOBUTTON;
            break;
        case SID_FM_GROUPBOX:
            nIdentifier = OBJ_FM_GROUPBOX;
            break;
        case SID_FM_COMBOBOX:
            nIdentifier = OBJ_FM_COMBOBOX;
            break;
        case SID_FM_NAVIGATIONBAR:
            nIdentifier = OBJ_FM_NAVIGATIONBAR;
            break;
        case SID_FM_DBGRID:
            nIdentifier = OBJ_FM_GRID;
            break;
        case SID_FM_IMAGEBUTTON:
            nIdentifier = OBJ_FM_IMAGEBUTTON;
            break;
        case SID_FM_IMAGECONTROL:
            nIdentifier = OBJ_FM_IMAGECONTROL;
            break;
        case SID_FM_FILECONTROL:
            nIdentifier = OBJ_FM_FILECONTROL;
            break;
        case SID_FM_DATEFIELD:
            nIdentifier = OBJ_FM_DATEFIELD;
            break;
        case SID_FM_TIMEFIELD:
            nIdentifier = OBJ_FM_TIMEFIELD;
            break;
        case SID_FM_NUMERICFIELD:
            nIdentifier = OBJ_FM_NUMERICFIELD;
            break;
        case SID_FM_CURRENCYFIELD:
            nIdentifier = OBJ_FM_CURRENCYFIELD;
            break;
        case SID_FM_PATTERNFIELD:
            nIdentifier = OBJ_FM_PATTERNFIELD;
            break;
        case SID_FM_FORMATTEDFIELD:
            nIdentifier = OBJ_FM_FORMATTEDFIELD;
            break;
        case SID_FM_SCROLLBAR:
            nIdentifier = OBJ_FM_SCROLLBAR;
            break;
        case SID_FM_SPINBUTTON:
            nIdentifier = OBJ_FM_SPINBUTTON;
            break;
    }

    switch ( nSlot )
    {
        case SID_FM_CHECKBOX:
        case SID_FM_PUSHBUTTON:
        case SID_FM_FIXEDTEXT:
        case SID_FM_LISTBOX:
        case SID_FM_EDIT:
        case SID_FM_RADIOBUTTON:
        case SID_FM_COMBOBOX:
        case SID_FM_NAVIGATIONBAR:
        case SID_FM_GROUPBOX:
        case SID_FM_DBGRID:
        case SID_FM_IMAGEBUTTON:
        case SID_FM_IMAGECONTROL:
        case SID_FM_FILECONTROL:
        case SID_FM_DATEFIELD:
        case SID_FM_TIMEFIELD:
        case SID_FM_NUMERICFIELD:
        case SID_FM_CURRENCYFIELD:
        case SID_FM_PATTERNFIELD:
        case SID_FM_FORMATTEDFIELD:
        case SID_FM_SCROLLBAR:
        case SID_FM_SPINBUTTON:
        {
            SFX_REQUEST_ARG( rReq, pGrabFocusItem, SfxBoolItem, SID_FM_GRABCONTROLFOCUS, sal_False );
            if ( pGrabFocusItem && pGrabFocusItem->GetValue() )
            {   // see below
                SfxViewShell* pShell = GetViewShell();
                Window* pShellWnd = pShell ? pShell->GetWindow() : NULL;
                if ( pShellWnd )
                    pShellWnd->GrabFocus();
                break;
            }

            SfxUInt16Item aIdentifierItem( SID_FM_CONTROL_IDENTIFIER, nIdentifier );
            SfxUInt32Item aInventorItem( SID_FM_CONTROL_INVENTOR, FmFormInventor );
            const SfxPoolItem* pArgs[] =
            {
                &aIdentifierItem, &aInventorItem, NULL
            };
            const SfxPoolItem* pInternalArgs[] =
            {
                NULL
            };

            GetViewShell()->GetViewFrame()->GetDispatcher()->Execute( SID_FM_CREATE_CONTROL, SFX_CALLMODE_ASYNCHRON,
                                      pArgs, rReq.GetModifier(), pInternalArgs );

            if ( rReq.GetModifier() & KEY_MOD1 )
            {
                //  #99013# if selected with control key, return focus to current view
                // do this asynchron, so that the creation can be finished first
                // reusing the SID_FM_GRABCONTROLFOCUS is somewhat hacky ... which it wouldn't if it would have another
                // name, so I do not really have a big problem with this ....
                SfxBoolItem aGrabFocusIndicatorItem( SID_FM_GRABCONTROLFOCUS, sal_True );
                GetViewShell()->GetViewFrame()->GetDispatcher()->Execute( nSlot, SFX_CALLMODE_ASYNCHRON,
                                          &aGrabFocusIndicatorItem, NULL );
            }

            rReq.Done();
        }   break;
    }

    // Individuelle Aktionen
    switch( nSlot )
    {
        case SID_FM_MORE_CONTROLS:
        case SID_FM_FORM_DESIGN_TOOLS:
        {
            FormToolboxes aToolboxAccess( GetImpl()->getHostFrame(), GetImpl()->getDocumentType() );
            aToolboxAccess.toggleToolbox( nSlot );
            rReq.Done();
        }
        break;

        case SID_FM_GRABCONTROLFOCUS:
        {
            FmFormView* pFormView = GetFormView();
            if ( pFormView )
                pFormView->GrabFirstControlFocus( );
        }
        break;

        case SID_FM_VIEW_AS_GRID:
            GetImpl()->CreateExternalView();
            break;
        case SID_FM_CONVERTTO_EDIT          :
        case SID_FM_CONVERTTO_BUTTON            :
        case SID_FM_CONVERTTO_FIXEDTEXT     :
        case SID_FM_CONVERTTO_LISTBOX       :
        case SID_FM_CONVERTTO_CHECKBOX      :
        case SID_FM_CONVERTTO_RADIOBUTTON   :
        case SID_FM_CONVERTTO_GROUPBOX      :
        case SID_FM_CONVERTTO_COMBOBOX      :
        case SID_FM_CONVERTTO_IMAGEBUTTON   :
        case SID_FM_CONVERTTO_FILECONTROL   :
        case SID_FM_CONVERTTO_DATE          :
        case SID_FM_CONVERTTO_TIME          :
        case SID_FM_CONVERTTO_NUMERIC       :
        case SID_FM_CONVERTTO_CURRENCY      :
        case SID_FM_CONVERTTO_PATTERN       :
        case SID_FM_CONVERTTO_IMAGECONTROL  :
        case SID_FM_CONVERTTO_FORMATTED     :
        case SID_FM_CONVERTTO_SCROLLBAR     :
        case SID_FM_CONVERTTO_SPINBUTTON    :
        case SID_FM_CONVERTTO_NAVIGATIONBAR :
            GetImpl()->executeControlConversionSlot( nSlot );
            // nach dem Konvertieren die Selektion neu bestimmern, da sich ja das selektierte Objekt
            // geaendert hat
            GetImpl()->SetSelection(GetFormView()->GetMarkedObjectList());
            break;
        case SID_FM_LEAVE_CREATE:
            m_nLastSlot = 0;
            GetViewShell()->GetViewFrame()->GetBindings().Invalidate( SID_FM_CONFIG );
            rReq.Done();
            break;
        case SID_FM_SHOW_PROPERTY_BROWSER:
        {
            SFX_REQUEST_ARG( rReq, pShowItem, SfxBoolItem, SID_FM_SHOW_PROPERTIES, sal_False );
            sal_Bool bShow = sal_True;
            if ( pShowItem )
                bShow = pShowItem->GetValue();
            GetImpl()->ShowSelectionProperties( bShow );

            rReq.Done();
        } break;

        case SID_FM_PROPERTIES:
        {
            // PropertyBrowser anzeigen
            SFX_REQUEST_ARG(rReq, pShowItem, SfxBoolItem, nSlot, sal_False);
            sal_Bool bShow = pShowItem ? pShowItem->GetValue() : sal_True;

            InterfaceBag aOnlyTheForm;
            aOnlyTheForm.insert( Reference< XInterface >( GetImpl()->getCurrentForm(), UNO_QUERY ) );
            GetImpl()->setCurrentSelection( aOnlyTheForm );

            GetImpl()->ShowSelectionProperties( bShow );

            rReq.Done();
        }   break;

        case SID_FM_CTL_PROPERTIES:
        {
            SFX_REQUEST_ARG(rReq, pShowItem, SfxBoolItem, nSlot, sal_False);
            sal_Bool bShow = pShowItem ? pShowItem->GetValue() : sal_True;

            OSL_ENSURE( GetImpl()->onlyControlsAreMarked(), "FmFormShell::Execute: ControlProperties should be disabled!" );
            if ( bShow )
                GetImpl()->selectLastMarkedControls();
            GetImpl()->ShowSelectionProperties( bShow );

            rReq.Done();
        }   break;
        case SID_FM_SHOW_PROPERTIES:
        case SID_FM_ADD_FIELD:
        case SID_FM_FILTER_NAVIGATOR:
        case SID_FM_SHOW_DATANAVIGATOR :
        {
            GetViewShell()->GetViewFrame()->ChildWindowExecute(rReq);
            rReq.Done();
        }   break;
        case SID_FM_SHOW_FMEXPLORER:
        {
            if (!m_pFormView)   // setzen der ::com::sun::star::sdbcx::View Forcieren
                GetViewShell()->GetViewFrame()->GetDispatcher()->Execute(SID_CREATE_SW_DRAWVIEW);

            GetViewShell()->GetViewFrame()->ChildWindowExecute(rReq);
            rReq.Done();
        }
        break;

        case SID_FM_TAB_DIALOG:
        {
            GetImpl()->ExecuteTabOrderDialog( Reference< XTabControllerModel >( GetImpl()->getCurrentForm(), UNO_QUERY ) );
            rReq.Done();
        }
        break;

        case SID_FM_DESIGN_MODE:
        {
            SFX_REQUEST_ARG(rReq, pDesignItem, SfxBoolItem, nSlot, sal_False);
            if (!pDesignItem || pDesignItem->GetValue() != m_bDesignMode)
            {
                FmFormModel* pModel = GetFormModel();
                if (pModel)
                {   // fuer die Zeit des Uebergangs das Undo-Environment ausschalten, das sichert, dass man dort auch nicht-transiente
                    // Properties mal eben aendern kann (sollte allerdings mit Vorsicht genossen und beim Rueckschalten des Modes
                    // auch immer wieder rueckgaegig gemacht werden. Ein Beispiel ist das Setzen der maximalen Text-Laenge durch das
                    // OEditModel an seinem Control.)
                    pModel->GetUndoEnv().Lock();
                }

                // dann die eigentliche Umschaltung
                if (m_bDesignMode || PrepareClose(sal_True))
                {
                    SetDesignMode(!m_bDesignMode );
                    rReq.Done();
                }
                // und mein Undo-Environment wieder an
                if (pModel)
                    pModel->GetUndoEnv().UnLock();
            }
        }   break;
        case SID_FM_AUTOCONTROLFOCUS:
        {
            FmFormModel* pModel = GetFormModel();
            DBG_ASSERT(pModel, "FmFormShell::Execute : invalid call !");
                // should have been disabled in GetState if we don't have a FormModel
            pModel->SetAutoControlFocus( !pModel->GetAutoControlFocus() );
            GetViewShell()->GetViewFrame()->GetBindings().Invalidate(SID_FM_AUTOCONTROLFOCUS);
        }
        break;
        case SID_FM_OPEN_READONLY:
        {
            FmFormModel* pModel = GetFormModel();
            DBG_ASSERT(pModel, "FmFormShell::Execute : invalid call !");
                // should have been disabled in GetState if we don't have a FormModel
            pModel->SetOpenInDesignMode( !pModel->GetOpenInDesignMode() );
            GetViewShell()->GetViewFrame()->GetBindings().Invalidate(SID_FM_OPEN_READONLY);
        }
        break;
        case SID_FM_USE_WIZARDS:
        {
            GetImpl()->SetWizardUsing(!GetImpl()->GetWizardUsing());
            GetViewShell()->GetViewFrame()->GetBindings().Invalidate(SID_FM_USE_WIZARDS);
        }
        break;
        case SID_FM_SEARCH:
        {
            const ::svx::ControllerFeatures& rController = GetImpl()->getActiveControllerFeatures();
            if ( rController->commitCurrentControl() && rController->commitCurrentRecord() )
                GetImpl()->ExecuteSearch();
            rReq.Done();
        } break;

        // first, prev, next, last, and absolute affect the nav controller, not the
        // active controller
        case SID_FM_RECORD_FIRST:
        case SID_FM_RECORD_PREV:
        case SID_FM_RECORD_NEXT:
        case SID_FM_RECORD_LAST:
        case SID_FM_RECORD_NEW:
        {
            const ::svx::ControllerFeatures& rController = GetImpl()->getNavControllerFeatures();
            rController->execute( nSlot );
            rReq.Done();
            }
        break;

        case SID_FM_REFRESH:
        case SID_FM_RECORD_DELETE:
        case SID_FM_RECORD_UNDO:
        case SID_FM_RECORD_SAVE:
        case SID_FM_REMOVE_FILTER_SORT:
        case SID_FM_SORTDOWN:
        case SID_FM_SORTUP:
        case SID_FM_AUTOFILTER:
        case SID_FM_ORDERCRIT:
        case SID_FM_FORM_FILTERED:
            // these are the slots whose implementations are already moved in the impl class
            GetImpl()->ExecuteFormSlot( nSlot, GetImpl()->getActiveForm(), GetImpl()->getActiveController() );
            rReq.Done();
            break;

        case SID_FM_RECORD_ABSOLUTE:
        {
            const ::svx::ControllerFeatures& rController = GetImpl()->getNavControllerFeatures();
            sal_Int32 nRecord = -1;

            const SfxItemSet* pArgs = rReq.GetArgs();
            if ( pArgs )
            {
                const SfxPoolItem* pItem;
                if ( ( pArgs->GetItemState( FN_PARAM_1, sal_True, &pItem ) ) == SFX_ITEM_SET )
                {
                    const SfxInt32Item* pTypedItem = PTR_CAST( SfxInt32Item, pItem );
                    if ( pTypedItem )
                        nRecord = Max( pTypedItem->GetValue(), sal_Int32(0) );
                }
            }
            else
            {
                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                DBG_ASSERT( pFact, "no dialog factory!" );
                if ( pFact )
                {
                    ::std::auto_ptr< AbstractFmInputRecordNoDialog > dlg( pFact->CreateFmInputRecordNoDialog( NULL, ResId( RID_SVX_DLG_INPUTRECORDNO ) ) );
                    DBG_ASSERT( dlg.get(), "Dialogdiet fail!" );
                    dlg->SetValue( rController->getCursor()->getRow() );
                    if ( dlg->Execute() == RET_OK )
                        nRecord = dlg->GetValue();

                    rReq.AppendItem( SfxInt32Item( FN_PARAM_1, nRecord ) );
                }
            }

            if ( nRecord != -1 )
                rController->execute( nSlot, ::rtl::OUString::createFromAscii( "Position" ), makeAny( (sal_Int32)nRecord ) );

            rReq.Done();
        }   break;
        case SID_FM_FILTER_EXECUTE:
        case SID_FM_FILTER_EXIT:
        {
            sal_Bool bCancelled = ( SID_FM_FILTER_EXIT == nSlot );
            sal_Bool bReopenNavigator = sal_False;

            if ( !bCancelled )
            {
                // if the filter navigator is still open, we need to close it, so it can possibly
                // commit it's most recent changes
                if ( GetViewShell() && GetViewShell()->GetViewFrame() )
                    if ( GetViewShell()->GetViewFrame()->HasChildWindow( SID_FM_FILTER_NAVIGATOR ) )
                    {
                        GetViewShell()->GetViewFrame()->ToggleChildWindow( SID_FM_FILTER_NAVIGATOR );
                        bReopenNavigator = sal_True;
                    }

                Reference< XFormController >  xController( GetImpl()->getActiveController() );

                if  (   GetViewShell()->GetViewFrame()->HasChildWindow( SID_FM_FILTER_NAVIGATOR )
                        // closing the window was denied, for instance because of a invalid criterion

                    ||  (   xController.is()
                        &&  !GetImpl()->getActiveControllerFeatures()->commitCurrentControl( )
                        )
                        // committing the controller was denied
                    )
                {
                    rReq.Done();
                    break;
                }
            }

            GetImpl()->stopFiltering( !bCancelled );
            rReq.Done();

            if ( bReopenNavigator )
                // we closed the navigator only to implicitly commit it (as we do not have another
                // direct wire to it), but to the user, it should look it it was always open
                GetViewShell()->GetViewFrame()->ToggleChildWindow( SID_FM_FILTER_NAVIGATOR );
        }
        break;

        case SID_FM_FILTER_START:
        {
            GetImpl()->startFiltering();
            rReq.Done();
        }   break;
    }
}

//------------------------------------------------------------------------
void FmFormShell::GetState(SfxItemSet &rSet)
{
    SfxWhichIter aIter( rSet );
    sal_uInt16 nWhich = aIter.FirstWhich();
    while ( nWhich )
    {
        switch( nWhich )
        {
            case SID_FM_MORE_CONTROLS:
            case SID_FM_FORM_DESIGN_TOOLS:
            {
                FormToolboxes aToolboxAccess( GetImpl()->getHostFrame(), GetImpl()->getDocumentType() );
                rSet.Put( SfxBoolItem( nWhich, aToolboxAccess.isToolboxVisible( nWhich ) ) );
            }
            break;

            case SID_FM_FILTER_EXECUTE:
            case SID_FM_FILTER_EXIT:
                if (!GetImpl()->isInFilterMode())
                    rSet.DisableItem( nWhich );
                break;

            case SID_FM_USE_WIZARDS:
                if  ( GetImpl()->isEnhancedForm()
                    ||  !SvtModuleOptions().IsModuleInstalled( SvtModuleOptions::E_SDATABASE )
                    )
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
                else if (!m_bDesignMode || !GetFormModel())
                    rSet.DisableItem( nWhich );
                else
                    rSet.Put( SfxBoolItem(nWhich, GetImpl()->GetWizardUsing() ) );
                break;
            case SID_FM_AUTOCONTROLFOCUS:
                if (!m_bDesignMode || !GetFormModel())
                    rSet.DisableItem( nWhich );
                else
                    rSet.Put( SfxBoolItem(nWhich, GetFormModel()->GetAutoControlFocus() ) );
                break;
            case SID_FM_OPEN_READONLY:
                if (!m_bDesignMode || !GetFormModel())
                    rSet.DisableItem( nWhich );
                else
                    rSet.Put( SfxBoolItem(nWhich, GetFormModel()->GetOpenInDesignMode() ) );
                break;

            case SID_FM_NAVIGATIONBAR:
            case SID_FM_DBGRID:
                if ( !SvtModuleOptions().IsModuleInstalled( SvtModuleOptions::E_SDATABASE ) )
                {
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
                    break;
                }
                // NO break!

            case SID_FM_SCROLLBAR:
            case SID_FM_IMAGECONTROL:
            case SID_FM_FILECONTROL:
            case SID_FM_CURRENCYFIELD:
            case SID_FM_PATTERNFIELD:
                if ( GetImpl()->isEnhancedForm() )
                {
                    // in XForms mode, several controls are disabled:
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
                    break;
                }
                // NO break here!
            case SID_FM_IMAGEBUTTON:
            case SID_FM_RADIOBUTTON:
            case SID_FM_COMBOBOX:
            case SID_FM_GROUPBOX:
            case SID_FM_CHECKBOX:
            case SID_FM_PUSHBUTTON:
            case SID_FM_FIXEDTEXT:
            case SID_FM_LISTBOX:
            case SID_FM_EDIT:
            case SID_FM_DATEFIELD:
            case SID_FM_TIMEFIELD:
            case SID_FM_NUMERICFIELD:
            case SID_FM_FORMATTEDFIELD:
            case SID_FM_SPINBUTTON:
                if (!m_bDesignMode)
                    rSet.DisableItem( nWhich );
                else
                {
                    sal_Bool bLayerLocked = sal_False;
                    if (m_pFormView)
                    {
                        // Ist der ::com::sun::star::drawing::Layer gelocked, so m�ssen die Slots disabled werden. #36897
                        SdrPageView* pPV = m_pFormView->GetPageViewPvNum(0);
                        if (pPV != NULL)
                            bLayerLocked = pPV->IsLayerLocked(m_pFormView->GetActiveLayer());
                    }
                    if (bLayerLocked)
                        rSet.DisableItem( nWhich );
                    else
                        rSet.Put( SfxBoolItem(nWhich, (nWhich==m_nLastSlot)) );
                }
                break;
            case SID_FM_FILTER_NAVIGATOR_CONTROL:
            {
                if (GetImpl()->isInFilterMode())
                    rSet.Put(SfxObjectItem(nWhich, this));
                else
                    rSet.Put(SfxObjectItem(nWhich));
            }   break;
            case SID_FM_FIELDS_CONTROL:
            case SID_FM_PROPERTY_CONTROL:
            {
                if (!m_bDesignMode || !m_pFormView || !m_bHasForms)
                    rSet.Put(SfxObjectItem(nWhich));
                else
                    rSet.Put(SfxObjectItem(nWhich, this));

            }   break;
            case SID_FM_FMEXPLORER_CONTROL:
            case SID_FM_DATANAVIGATOR_CONTROL :
            {
                if (!m_bDesignMode || !m_pFormView)
                    rSet.Put(SfxObjectItem(nWhich));
                else
                    rSet.Put(SfxObjectItem(nWhich, this));

            }   break;
            case SID_FM_ADD_FIELD:
            case SID_FM_SHOW_FMEXPLORER:
            case SID_FM_SHOW_PROPERTIES:
            case SID_FM_FILTER_NAVIGATOR:
            case SID_FM_SHOW_DATANAVIGATOR:
            {
                if ( GetViewShell()->GetViewFrame()->KnowsChildWindow(nWhich) )
                    rSet.Put( SfxBoolItem( nWhich, GetViewShell()->GetViewFrame()->HasChildWindow(nWhich)) );
                else
                    rSet.DisableItem(nWhich);

                // special case for data navigator: hide if not in XML form document
                if ( nWhich == SID_FM_SHOW_DATANAVIGATOR && !GetImpl()->isEnhancedForm() )
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
            }   break;

            case SID_FM_SHOW_PROPERTY_BROWSER:
            {
                rSet.Put(SfxBoolItem(GetImpl()->IsPropBrwOpen()));
            }
            break;

            case SID_FM_CTL_PROPERTIES:
            {
                // der Impl eventuell die Moeglichjkeit geben, ihre an der aktuellen MarkList ausgerichteten Objekte
                // auf den neuesten Stand zu bringen
                if (GetImpl()->IsSelectionUpdatePending())
                    GetImpl()->ForceUpdateSelection(sal_False);

                if ( !m_pFormView || !m_bDesignMode || !GetImpl()->onlyControlsAreMarked() )
                    rSet.DisableItem( nWhich );
                else
                {
                    sal_Bool bChecked  = GetImpl()->IsPropBrwOpen() && !GetImpl()->isSolelySelected( GetImpl()->getCurrentForm() );
                        // if the property browser is open, and only controls are marked, and the current selection
                        // does not consist of only the current form, then the current selection is the (composition of)
                        // the currently marked controls
                    rSet.Put( SfxBoolItem( nWhich, bChecked ) );
                }
            }   break;

            case SID_FM_PROPERTIES:
            {
                // der Impl eventuell die Moeglichjkeit geben, ihre an der aktuellen MarkList ausgerichteten Objekte
                // auf den neuesten Stand zu bringen
                if (GetImpl()->IsSelectionUpdatePending())
                    GetImpl()->ForceUpdateSelection(sal_False);

                if ( GetImpl()->isEnhancedForm() )
                {
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
                    break;
                }

                if ( !m_pFormView || !m_bDesignMode || !GetImpl()->getCurrentForm().is() )
                    rSet.DisableItem( nWhich );
                else
                {
                    sal_Bool bChecked = GetImpl()->IsPropBrwOpen() && GetImpl()->isSolelySelected( GetImpl()->getCurrentForm() );
                    rSet.Put(SfxBoolItem(nWhich, bChecked));
                }
            }   break;
            case SID_FM_TAB_DIALOG:
                // der Impl eventuell die Moeglichjkeit geben, ihre an der aktuellen MarkList ausgerichteten Objekte
                // auf den neuesten Stand zu bringen
                if (GetImpl()->IsSelectionUpdatePending())
                    GetImpl()->ForceUpdateSelection(sal_False);

                if (!m_pFormView || !m_bDesignMode || !GetImpl()->getCurrentForm().is() )
                    rSet.DisableItem( nWhich );
                break;
            case SID_FM_CONFIG:
                rSet.Put(SfxUInt16Item(nWhich, m_nLastSlot));
                break;
            case SID_FM_DESIGN_MODE:
                if ( GetImpl()->isEnhancedForm() )
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
                else if (!m_pFormView || GetImpl()->IsReadonlyDoc() )
                    rSet.DisableItem( nWhich );
                else
                    rSet.Put( SfxBoolItem(nWhich, m_bDesignMode) );
                break;
            case SID_FM_SEARCH:
            case SID_FM_RECORD_FIRST:
            case SID_FM_RECORD_NEXT:
            case SID_FM_RECORD_PREV:
            case SID_FM_RECORD_LAST:
            case SID_FM_RECORD_NEW:
            case SID_FM_RECORD_DELETE:
            case SID_FM_RECORD_ABSOLUTE:
            case SID_FM_RECORD_TOTAL:
            case SID_FM_RECORD_SAVE:
            case SID_FM_RECORD_UNDO:
            case SID_FM_FORM_FILTERED:
            case SID_FM_REMOVE_FILTER_SORT:
            case SID_FM_SORTUP:
            case SID_FM_SORTDOWN:
            case SID_FM_ORDERCRIT:
            case SID_FM_FILTER_START:
            case SID_FM_AUTOFILTER:
            case SID_FM_REFRESH:
            case SID_FM_VIEW_AS_GRID:
                GetFormState(rSet,nWhich);
                break;

            case SID_FM_CHANGECONTROLTYPE:
            {
                if ( !m_pFormView || !m_bDesignMode )
                    rSet.DisableItem( nWhich );
                else
                {
                    if ( !GetImpl()->canConvertCurrentSelectionToControl( OBJ_FM_FIXEDTEXT ) )
                        // if it cannot be converted to a fixed text, it is no single control
                        rSet.DisableItem( nWhich );
                }
            } break;

            case SID_FM_CONVERTTO_FILECONTROL   :
            case SID_FM_CONVERTTO_CURRENCY      :
            case SID_FM_CONVERTTO_PATTERN       :
            case SID_FM_CONVERTTO_IMAGECONTROL  :
            case SID_FM_CONVERTTO_SCROLLBAR     :
            case SID_FM_CONVERTTO_NAVIGATIONBAR :
                if ( GetImpl()->isEnhancedForm() )
                {
                    rSet.Put( SfxVisibilityItem( nWhich, sal_False ) );
                    break;
                }
                // NO break here!

            case SID_FM_CONVERTTO_IMAGEBUTTON   :
            case SID_FM_CONVERTTO_EDIT          :
            case SID_FM_CONVERTTO_BUTTON        :
            case SID_FM_CONVERTTO_FIXEDTEXT     :
            case SID_FM_CONVERTTO_LISTBOX       :
            case SID_FM_CONVERTTO_CHECKBOX      :
            case SID_FM_CONVERTTO_RADIOBUTTON   :
            case SID_FM_CONVERTTO_GROUPBOX      :
            case SID_FM_CONVERTTO_COMBOBOX      :
            case SID_FM_CONVERTTO_DATE          :
            case SID_FM_CONVERTTO_TIME          :
            case SID_FM_CONVERTTO_NUMERIC       :
            case SID_FM_CONVERTTO_FORMATTED     :
            case SID_FM_CONVERTTO_SPINBUTTON    :
            {
                if ( !m_pFormView || !m_bDesignMode || !GetImpl()->canConvertCurrentSelectionToControl( nWhich ) )
                    rSet.DisableItem( nWhich );
                else
                {
                    rSet.Put( SfxBoolItem( nWhich, sal_False ) );
                    // just to have a defined state (available and not checked)
                }
            }
            break;
        }
        nWhich = aIter.NextWhich();
    }
}

//------------------------------------------------------------------------
void FmFormShell::GetFormState(SfxItemSet &rSet, sal_uInt16 nWhich)
{
    if  (   !GetImpl()->getNavController().is()
        ||  !isRowSetAlive(GetImpl()->getNavController()->getModel())
        ||  !m_pFormView
        ||  m_bDesignMode
        ||  !GetImpl()->getActiveForm().is()
        ||  GetImpl()->isInFilterMode()
        ||  (   GetImpl()->HasPendingCursorAction(GetImpl()->getNavController())
            &&  (SID_FM_RECORD_TOTAL != nWhich)
            )
        )
        rSet.DisableItem(nWhich);
    else
    {
        sal_Bool bEnable = sal_False;
        try
        {
            switch (nWhich)
            {
            case SID_FM_VIEW_AS_GRID:
                if (GetImpl()->getHostFrame().is() && GetImpl()->getNavController().is())
                {
                    bEnable = sal_True;
                    sal_Bool bDisplayingCurrent =
                        GetImpl()->getInternalForm(
                            Reference< XForm >( GetImpl()->getNavController()->getModel(), UNO_QUERY )
                        ) == GetImpl()->getExternallyDisplayedForm();
                    rSet.Put(SfxBoolItem(nWhich, bDisplayingCurrent));
                }
                break;

            case SID_FM_SEARCH:
            {
                Reference< ::com::sun::star::beans::XPropertySet >  xNavSet(GetImpl()->getActiveForm(), UNO_QUERY);
                sal_Int32 nCount = ::comphelper::getINT32(xNavSet->getPropertyValue(FM_PROP_ROWCOUNT));
                bEnable = nCount != 0;
            }   break;
            case SID_FM_RECORD_ABSOLUTE:
            case SID_FM_RECORD_TOTAL:
            {
                ::svx::ControllerFeatureState aState;
                GetImpl()->getNavControllerFeatures()->getState( nWhich, aState );
                if ( SID_FM_RECORD_ABSOLUTE == nWhich )
                {
                    sal_Int32 nPosition = 0;
                    aState.aState >>= nPosition;
                    rSet.Put( SfxInt32Item( nWhich, nPosition ) );
                }
                else if ( SID_FM_RECORD_TOTAL == nWhich )
                {
                    ::rtl::OUString sTotalCount;
                    aState.aState >>= sTotalCount;
                    rSet.Put( SfxStringItem( nWhich, sTotalCount ) );
                }
                bEnable = aState.bEnabled;
            }
            break;

            case SID_FM_RECORD_NEXT:
            case SID_FM_RECORD_NEW:
            case SID_FM_RECORD_FIRST:
            case SID_FM_RECORD_PREV:
            case SID_FM_RECORD_LAST:
                // delegate
                bEnable = GetImpl()->getNavControllerFeatures()->getSimpleState( nWhich );
                break;

            case SID_FM_RECORD_SAVE:
            case SID_FM_RECORD_UNDO:
            case SID_FM_RECORD_DELETE:
            case SID_FM_REFRESH:
            case SID_FM_REMOVE_FILTER_SORT:
            case SID_FM_SORTUP:
            case SID_FM_SORTDOWN:
            case SID_FM_AUTOFILTER:
            case SID_FM_ORDERCRIT:
                // delegate
                bEnable = GetImpl()->getActiveControllerFeatures()->getSimpleState( nWhich );
                break;

            case SID_FM_FORM_FILTERED:
            {
                ::svx::ControllerFeatureState aState;
                GetImpl()->getActiveControllerFeatures()->getState( nWhich, aState );

                bEnable = aState.bEnabled;
                rSet.Put( SfxBoolItem( nWhich, ::comphelper::getBOOL( aState.aState ) ) );
            }
            break;

            case SID_FM_FILTER_START:
                if ( GetImpl()->getActiveControllerFeatures()->isParsable() )
                    bEnable = !GetImpl()->getActiveControllerFeatures()->isInsertOnlyForm();
                break;
            }
        }
        catch( const Exception& )
        {
            DBG_ERROR( "FmFormShell::GetFormState: caught an exception while determining the state!" );
        }
        if (!bEnable)
            rSet.DisableItem(nWhich);
    }
}

//------------------------------------------------------------------------
FmFormPage* FmFormShell::GetCurPage() const
{
    FmFormPage* pP = NULL;
    if (m_pFormView && m_pFormView->GetPageViewPvNum(0))
        pP = PTR_CAST(FmFormPage,m_pFormView->GetPageViewPvNum(0)->GetPage());
    return pP;
}

//------------------------------------------------------------------------
void FmFormShell::SetView(FmFormView* pView)
{
    if (m_pFormView)
    {
        m_pFormView->SetFormShell( NULL );
        GetImpl()->ResetForms();
        m_pFormModel = NULL;
    }
    if (pView)
    {
        m_pFormView = pView;
        m_pFormView->SetFormShell( this );
        m_pFormModel = (FmFormModel*)m_pFormView->GetModel();

        SetDesignMode(pView->IsDesignMode());

        // We activate our view if we are activated ourself, but sometimes the Activate precedes the SetView.
        // But here we know both the view and our activation state so we at least are able to pass the latter
        // to the former.
        // FS - 30.06.99 - 67308
        if ( IsActive() )
            GetImpl()->viewActivated( m_pFormView );
    }
    else
    {
        m_pFormView = NULL;
    }
}

//------------------------------------------------------------------------
void FmFormShell::DetermineForms(sal_Bool bInvalidate)
{
    // Existieren Formulare auf der aktuellen Page
    sal_Bool bForms = GetImpl()->hasForms();
    if (bForms != m_bHasForms)
    {
        m_bHasForms = bForms;
        if (bInvalidate)
            UIFeatureChanged();
    }
}

//------------------------------------------------------------------------
sal_Bool FmFormShell::GetY2KState(sal_uInt16& nReturn)
{
    return GetImpl()->GetY2KState(nReturn);
}

//------------------------------------------------------------------------
void FmFormShell::SetY2KState(sal_uInt16 n)
{
    GetImpl()->SetY2KState(n);
}

//------------------------------------------------------------------------
void FmFormShell::Activate(sal_Bool bMDI)
{
    SfxShell::Activate(bMDI);

    GetImpl()->viewActivated( m_pFormView, sal_True );
}

//------------------------------------------------------------------------
void FmFormShell::Deactivate(sal_Bool bMDI)
{
    SfxShell::Deactivate(bMDI);

    GetImpl()->viewDeactivated( m_pFormView, sal_False );
}

//------------------------------------------------------------------------
void FmFormShell::ExecuteTextAttribute( SfxRequest& _rReq )
{
    m_pImpl->ExecuteTextAttribute( _rReq );
}

//------------------------------------------------------------------------
void FmFormShell::GetTextAttributeState( SfxItemSet& _rSet )
{
    m_pImpl->GetTextAttributeState( _rSet );
}

//------------------------------------------------------------------------
bool FmFormShell::IsActiveControl() const
{
    return m_pImpl->IsActiveControl();
}

//------------------------------------------------------------------------
void FmFormShell::ForgetActiveControl()
{
    m_pImpl->ForgetActiveControl();
}

//------------------------------------------------------------------------
void FmFormShell::SetControlActivationHandler( const Link& _rHdl )
{
    m_pImpl->SetControlActivationHandler( _rHdl );
}
