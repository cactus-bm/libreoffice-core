/*************************************************************************
 *
 *  $RCSfile: fmvwimp.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: fs $ $Date: 2001-07-25 13:43:36 $
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
#pragma hdrstop

#ifndef _SVX_FMCTRLER_HXX
#include <fmctrler.hxx>
#endif

#ifndef _SVX_FMVWIMP_HXX
#include "fmvwimp.hxx"
#endif

#ifndef _COM_SUN_STAR_SDBC_XROWSET_HPP_
#include <com/sun/star/sdbc/XRowSet.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XLOADABLE_HPP_
#include <com/sun/star/form/XLoadable.hpp>
#endif

#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XRESET_HPP_
#include <com/sun/star/form/XReset.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLERMODEL_HPP_
#include <com/sun/star/awt/XTabControllerModel.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLCONTAINER_HPP_
#include <com/sun/star/awt/XControlContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLER_HPP_
#include <com/sun/star/awt/XTabController.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROL_HPP_
#include <com/sun/star/awt/XControl.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _SVX_FMMODEL_HXX
#include <fmmodel.hxx>
#endif

#ifndef _SVX_FMUNDO_HXX
#include "fmundo.hxx"
#endif

#ifndef _SVX_FMPAGE_HXX
#include "fmpage.hxx"
#endif
#ifndef _SVX_FMUNOPGE_HXX
#include "fmpgeimp.hxx"
#endif
#ifndef _SVX_FMVIEW_HXX
#include "fmview.hxx"
#endif
#ifndef _SVX_FMSHELL_HXX
#include "fmshell.hxx"
#endif
#ifndef _SVX_FMSHIMP_HXX
#include "fmshimp.hxx"
#endif
#ifndef _SVX_FMTOOLS_HXX
#include "fmtools.hxx"
#endif
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVDPAGV_HXX
#include "svdpagv.hxx"
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _COMPHELPER_ENUMHELPER_HXX_
#include <comphelper/enumhelper.hxx>
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::lang;
using namespace ::dbtools;
using namespace ::comphelper;
using namespace ::svxform;

DBG_NAME(FmXPageViewWinRec);
//------------------------------------------------------------------------
FmXPageViewWinRec::FmXPageViewWinRec(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&    _xORB,
                                     const SdrPageViewWinRec* pWinRec,
                                   FmXFormView* _pViewImpl)
                 :m_pViewImpl(_pViewImpl)
                 ,m_pWindow( (Window*)pWinRec->GetOutputDevice() )
                 ,m_xORB(_xORB)
{
    DBG_CTOR(FmXPageViewWinRec,NULL);

    // legt fuer jede ::com::sun::star::form ein FormController an
    FmFormPage* pP = NULL;
    if (m_pViewImpl)
        pP = PTR_CAST(FmFormPage,m_pViewImpl->getView()->GetPageViewPvNum(0)->GetPage());

    DBG_ASSERT(pP,"kein Page gefunden");

    if (pP)
    {
        Reference< XIndexAccess >  xForms(pP->GetForms(), UNO_QUERY);
        sal_uInt32 nLength = xForms->getCount();
        Any aElement;
        Reference< XForm >  xForm;
        for (sal_uInt32 i = 0; i < nLength; i++)
        {
            xForms->getByIndex(i) >>= xForm;
            setController(xForm, pWinRec->GetControlContainerRef() );
        }
    }
}
// -----------------------------------------------------------------------------
FmXPageViewWinRec::~FmXPageViewWinRec()
{
    DBG_DTOR(FmXPageViewWinRec,NULL);
}

//------------------------------------------------------------------
void FmXPageViewWinRec::dispose()
{
    for (vector< ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController > >::const_iterator i = m_aControllerList.begin();
            i != m_aControllerList.end(); i++)
    {
        // detaching the events
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XChild >  xChild((*i)->getModel(), ::com::sun::star::uno::UNO_QUERY);
        if (xChild.is())
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::script::XEventAttacherManager >  xEventManager(xChild->getParent(), ::com::sun::star::uno::UNO_QUERY);
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  xIfc(*i, ::com::sun::star::uno::UNO_QUERY);
            xEventManager->detach( i - m_aControllerList.begin(), xIfc );
        }

        // dispose the formcontroller
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >  xComp(*i, ::com::sun::star::uno::UNO_QUERY);
        xComp->dispose();
    }
    m_aControllerList.clear(); // this call deletes the formcontrollers
    m_xORB = NULL;
}


//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXPageViewWinRec::hasElements(void) throw( ::com::sun::star::uno::RuntimeException )
{
    return getCount() != 0;
}

//------------------------------------------------------------------------------
::com::sun::star::uno::Type SAL_CALL  FmXPageViewWinRec::getElementType(void) throw( ::com::sun::star::uno::RuntimeException )
{
    return ::getCppuType((const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController>*)0);
}

// ::com::sun::star::container::XEnumerationAccess
//------------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL FmXPageViewWinRec::createEnumeration(void) throw( ::com::sun::star::uno::RuntimeException )
{
    return new ::comphelper::OEnumerationByIndex(this);
}

// ::com::sun::star::container::XIndexAccess
//------------------------------------------------------------------------------
sal_Int32 SAL_CALL FmXPageViewWinRec::getCount(void) throw( ::com::sun::star::uno::RuntimeException )
{
    return m_aControllerList.size();
}

//------------------------------------------------------------------------------
::com::sun::star::uno::Any SAL_CALL FmXPageViewWinRec::getByIndex(sal_Int32 nIndex) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException )
{
    if (nIndex < 0 ||
        nIndex >= getCount())
        throw ::com::sun::star::lang::IndexOutOfBoundsException();

    ::com::sun::star::uno::Any aElement;
    aElement <<= m_aControllerList[nIndex];
    return aElement;
}

//------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  getControllerSearchChilds( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & xIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel > & xModel)
{
    if (xIndex.is() && xIndex->getCount())
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  xController;
        for (sal_Int32 n = xIndex->getCount(); n--; )
        {
            xIndex->getByIndex(n) >>= xController;
            if ((::com::sun::star::awt::XTabControllerModel*)xModel.get() == (::com::sun::star::awt::XTabControllerModel*)xController->getModel().get())
                return xController;
            else
                return getControllerSearchChilds(::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > (xController, ::com::sun::star::uno::UNO_QUERY), xModel);
        }
    }
    return ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController > ();
}

// Search the according controller
//------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  FmXPageViewWinRec::getController( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm > & xForm )
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel >  xModel(xForm, ::com::sun::star::uno::UNO_QUERY);
    for (::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController > >::const_iterator i = m_aControllerList.begin();
         i != m_aControllerList.end(); i++)
    {
        if ((::com::sun::star::awt::XTabControllerModel*)(*i)->getModel().get() == (::com::sun::star::awt::XTabControllerModel*)xModel.get())
            return *i;

        // the current-round controller isn't the right one. perhaps one of it's children ?
        ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  xChildSearch = getControllerSearchChilds(::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > (*i, ::com::sun::star::uno::UNO_QUERY), xModel);
        if (xChildSearch.is())
            return xChildSearch;
    }
    return ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController > ();
}

//------------------------------------------------------------------------
void FmXPageViewWinRec::setController(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm > & xForm,
                                     const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & xCC,
                                     FmXFormController* pParent, sal_Bool)
{
    DBG_ASSERT(xForm.is(), "Kein Formular angegeben");
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >  xFormCps(xForm, ::com::sun::star::uno::UNO_QUERY);
    if (!xFormCps.is())
        return;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel >  xTabOrder(xForm, ::com::sun::star::uno::UNO_QUERY);

    FmFormPage* pCurrentPage = PTR_CAST(FmFormPage, m_pViewImpl->getView()->GetPageViewPvNum(0)->GetPage());
    DBG_ASSERT(pCurrentPage, "FmXFormController::queryDispatch : could not retrieve my page !");
    String sPageId = pCurrentPage ? pCurrentPage->GetImpl()->GetPageId() : String::CreateFromAscii("no page");
        // this "no page" should result in a empty ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  provided by the controller's external dispatcher

    // Anlegen des Tabcontrollers
    FmXFormController* pController = new FmXFormController(m_xORB,m_pViewImpl->getView(), m_pWindow, sPageId);
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  xController(pController);

    pController->setModel(xTabOrder);
    pController->setContainer(xCC);
    pController->activateTabOrder();
    pController->addActivateListener(m_pViewImpl);

    if (pParent)
        pParent->addChild(pController);
    else
    {
        //  ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  xController(pController);
        m_aControllerList.push_back(xController);

        pController->setParent(*this);

        // attaching the events
        ::com::sun::star::uno::Reference< ::com::sun::star::script::XEventAttacherManager >  xEventManager(xForm->getParent(), ::com::sun::star::uno::UNO_QUERY);
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  xIfc(xController, ::com::sun::star::uno::UNO_QUERY);
        xEventManager->attach(m_aControllerList.size() - 1, xIfc, ::com::sun::star::uno::makeAny(xController) );
    }



    // jetzt die Subforms durchgehen
    sal_uInt32 nLength = xFormCps->getCount();
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm >  xSubForm;
    for (sal_uInt32 i = 0; i < nLength; i++)
    {
        xFormCps->getByIndex(i) >>= xSubForm;
        if (xSubForm.is())
            setController(xSubForm, xCC, pController);
    }
}

//------------------------------------------------------------------------
void FmXPageViewWinRec::updateTabOrder( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl > & xControl,
                                       const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & xCC )
{
    // Das TabControllerModel der ::com::sun::star::form ermitteln, in der das Control
    // enthalten ist ...
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormComponent >  xFormComp(xControl->getModel(), ::com::sun::star::uno::UNO_QUERY);
    if (!xFormComp.is())
        return;

    ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm >  xForm(xFormComp->getParent(), ::com::sun::star::uno::UNO_QUERY);
    if (!xForm.is())
        return;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabController >  xTabCtrl(getController( xForm ), ::com::sun::star::uno::UNO_QUERY);
    // Wenn es fuer dieses Formular noch keinen Tabcontroller gibt,
    // dann einen neuen anlegen
    if (!xTabCtrl.is())
    {
        // ist es ein Unterformular?
        // dann muss ein Tabcontroller fuer den Parent existieren
        // wichtig da ein hierarchischer Aufbau vorliegt
        ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm >  xParentForm(::com::sun::star::uno::Reference< ::com::sun::star::container::XChild > (xForm, ::com::sun::star::uno::UNO_QUERY)->getParent(), ::com::sun::star::uno::UNO_QUERY);
        FmXFormController* pFormController = NULL;
        // zugehoerigen controller suchen
        if (xParentForm.is())
            xTabCtrl = ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabController >(getController(xParentForm), ::com::sun::star::uno::UNO_QUERY);

        if (xTabCtrl.is())
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::lang::XUnoTunnel > xTunnel(xTabCtrl,::com::sun::star::uno::UNO_QUERY);
            DBG_ASSERT(xTunnel.is(), "FmPropController::ChangeFormatProperty : xTunnel is invalid!");
            if(xTunnel.is())
            {
                pFormController = (FmXFormController*)xTunnel->getSomething(FmXFormController::getUnoTunnelImplementationId());
            }
            //  ::comphelper::getImplementation(pFormController, xTunnel);
        }

        // Es gibt noch keinen TabController fuer das Formular, also muss
        // ein neuer angelegt werden.
        setController(xForm, xCC, pFormController);
    }
}

//------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >  FmXPageViewWinRec::getControlContainer() const
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >  xCC;
    if (m_aControllerList.size())
        xCC = m_aControllerList[0]->getContainer();
    return xCC;
}

//------------------------------------------------------------------------
FmXFormView::~FmXFormView()
{
    DBG_ASSERT(m_aWinList.size() == 0, "Liste nicht leer");
    if (m_nEvent)
        Application::RemoveUserEvent(m_nEvent);
    if (m_nErrorMessageEvent)
        Application::RemoveUserEvent(m_nErrorMessageEvent);
    if (m_nAutoFocusEvent)
        Application::RemoveUserEvent(m_nAutoFocusEvent);
}

//      EventListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormView::disposing(const ::com::sun::star::lang::EventObject& Source) throw( ::com::sun::star::uno::RuntimeException )
{
}

// ::com::sun::star::form::XFormControllerListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormView::formActivated(const ::com::sun::star::lang::EventObject& rEvent)
{
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormController >  xController(rEvent.Source, ::com::sun::star::uno::UNO_QUERY);
    // benachrichtigung der Shell
    if (m_pView && m_pView->GetFormShell())
    {
        FmXFormShell* pShImpl =  m_pView->GetFormShell()->GetImpl();
        if (pShImpl)
            pShImpl->setActiveController(xController);
    }
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormView::formDeactivated(const ::com::sun::star::lang::EventObject& rEvent)
{
    // deaktivierung wird nicht registriert
}

// ::com::sun::star::container::XContainerListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormView::elementInserted(const ::com::sun::star::container::ContainerEvent& evt)
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >  xCC(evt.Source, ::com::sun::star::uno::UNO_QUERY);
    if( xCC.is() )
    {
        FmWinRecList::iterator i = findWindow( xCC );

        if ( i != m_aWinList.end() )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >  xControl;
            evt.Element >>= xControl;
            if( xControl.is() )
                (*i)->updateTabOrder( xControl, xCC );
        }
    }
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormView::elementReplaced(const ::com::sun::star::container::ContainerEvent& evt)
{
    elementInserted(evt);
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormView::elementRemoved(const ::com::sun::star::container::ContainerEvent& evt)
{
}

//------------------------------------------------------------------------------
FmWinRecList::const_iterator FmXFormView::findWindow( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & rCC )  const
{
    for (FmWinRecList::const_iterator i = m_aWinList.begin();
            i != m_aWinList.end(); i++)
    {
        if (rCC == (*i)->getControlContainer())
            return i;
    }
    return m_aWinList.end();
}

//------------------------------------------------------------------------------
FmWinRecList::iterator FmXFormView::findWindow( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & rCC )
{
    for (FmWinRecList::iterator i = m_aWinList.begin();
            i != m_aWinList.end(); i++)
    {
        if (rCC == (*i)->getControlContainer())
            return i;
    }
    return m_aWinList.end();
}

//------------------------------------------------------------------------------
void FmXFormView::addWindow(const SdrPageViewWinRec* pRec)
{
    // Wird gerufen, wenn
    // - in den aktiven Modus geschaltet wird
    // - ein Window hinzugefuegt wird, waehrend man im Design-Modus ist
    // - der Control-Container fuer ein Window angelegt wird, waehrend
    //   der aktive Modus eingeschaltet ist.

    // Wenn es noch keinen Control-Container gibt oder am Control-Container
    // noch keine Peer Erzeugt wurde, dann ist es noch zu frueh, um die
    // Tab-Order einzustellen ...
    if( pRec->GetOutputDevice()->GetOutDevType() == OUTDEV_WINDOW  )
    {
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & rCC = pRec->GetControlContainerRef();
        if ( rCC.is() && findWindow( rCC ) == m_aWinList.end())
        {
            FmXPageViewWinRec *pFmRec = new FmXPageViewWinRec(m_xORB,pRec, this);
            pFmRec->acquire();

            m_aWinList.push_back(pFmRec);

            // Am ControlContainer horchen um Aenderungen mitzbekommen
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainer >  xContainer(rCC, ::com::sun::star::uno::UNO_QUERY);
            if (xContainer.is())
                xContainer->addContainerListener(this);
        }
    }
}

//------------------------------------------------------------------------------
void FmXFormView::removeWindow( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & rCC )
{
    // Wird gerufen, wenn
    // - in den Design-Modus geschaltet wird
    // - ein Window geloescht wird, waehrend man im Design-Modus ist
    // - der Control-Container fuer ein Window entfernt wird, waehrend
    //   der aktive Modus eingeschaltet ist.
    FmWinRecList::iterator i = findWindow( rCC );
    if (i != m_aWinList.end())
    {
        // Am ControlContainer horchen um Aenderungen mitzbekommen
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainer >  xContainer(rCC, ::com::sun::star::uno::UNO_QUERY);
        if (xContainer.is())
            xContainer->removeContainerListener(this);

        (*i)->dispose();
        (*i)->release();
        m_aWinList.erase(i);
    }
}

//------------------------------------------------------------------------------
IMPL_LINK(FmXFormView, OnDelayedErrorMessage, void*, EMPTYTAG)
{
    m_nErrorMessageEvent = 0;
    Window* pParentWindow = Application::GetDefDialogParent();
    ErrorBox(pParentWindow, WB_OK, m_sErrorMessage).Execute();
//  ErrorBox(Application::GetDefDialogParent(), WB_OK, m_sErrorMessage).Execute();
    return 0L;
}

//------------------------------------------------------------------------------
IMPL_LINK(FmXFormView, OnActivate, void*, EMPTYTAG)
{
    m_nEvent = 0;

    if (m_pPageViewForActivation)
    {
        FmFormModel* pModel = PTR_CAST(FmFormModel, m_pView->GetModel());
        if (pModel)
            // lock the undo env so the forms can change non-transient properties while loading
                // (without this my doc's modified flag would be set)
            pModel->GetUndoEnv().Lock();

        // Load all forms
        FmFormPage* pPage = (FmFormPage*)m_pPageViewForActivation->GetPage();
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >  xForms(pPage->GetForms(), ::com::sun::star::uno::UNO_QUERY);
        ::com::sun::star::uno::Any aElement;
        ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadable >  xForm;
        for (sal_Int32 i = 0, nCount = xForms->getCount(); i < nCount; i++)
        {
            xForms->getByIndex(i) >>= xForm;
            // a database form must be loaded for
            if (::isLoadable(xForm) && !xForm->isLoaded())
                xForm->load();
        }

        if (pModel && pModel->GetAutoControlFocus())
            LINK(this, FmXFormView, OnAutoFocus).Call(NULL);

        if (pModel)
            // unlock the environment
            pModel->GetUndoEnv().UnLock();
    }

    // setting the controller to activate
    if (m_pView->GetFormShell() && m_pView->GetActualOutDev() && m_pView->GetActualOutDev()->GetOutDevType() == OUTDEV_WINDOW)
    {
        Window* pWindow = (Window*)m_pView->GetActualOutDev();
        FmXPageViewWinRec* pFmRec = m_aWinList.size() ? m_aWinList[0] : NULL;
        for (FmWinRecList::const_iterator i = m_aWinList.begin();
            i != m_aWinList.end(); i++)
        {
            if (pWindow == (*i)->getWindow())
                pFmRec =*i;
        }

        if (pFmRec)
        {
            for (vector< Reference< XFormController > >::const_iterator i = pFmRec->GetList().begin();
                i != pFmRec->GetList().end(); i++)
            {
                const Reference< XFormController > & xController = *i;
                if (xController.is())
                {
                    // Nur bei Datenbankformularen erfolgt eine aktivierung
                    Reference< XRowSet >  xForm(xController->getModel(), UNO_QUERY);
                    if (xForm.is() && getRowsetConnection(xForm).is())
                    {
                        Reference< XPropertySet >  xFormSet(xForm, UNO_QUERY);
                        if (xFormSet.is())
                        {
                            // wenn es eine Datenquelle gibt, dann als aktive ::com::sun::star::form setzen
                            ::rtl::OUString aSource = ::comphelper::getString(xFormSet->getPropertyValue(FM_PROP_COMMAND));
                            if (aSource.getLength())
                            {
                                // benachrichtigung der Shell
                                FmXFormShell* pShImpl =  m_pView->GetFormShell()->GetImpl();
                                if (pShImpl)
                                    pShImpl->setActiveController(xController);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
void FmXFormView::Activate(SdrPageView* pPageView, sal_Bool bSync)
{
    if (m_nEvent)
    {
        Application::RemoveUserEvent(m_nEvent);
        m_nEvent = 0;
    }

    m_pPageViewForActivation = pPageView;
    if (bSync)
    {
        LINK(this,FmXFormView,OnActivate).Call(NULL);
    }
    else
        m_nEvent = Application::PostUserEvent(LINK(this,FmXFormView,OnActivate));
}

//------------------------------------------------------------------------------
void FmXFormView::Deactivate(SdrPageView* pPageView, BOOL bDeactivateController)
{
    if (m_nEvent)
    {
        Application::RemoveUserEvent(m_nEvent);
        m_nEvent = 0;
        m_pPageViewForActivation = NULL;
    }

    FmXFormShell* pShImpl =  m_pView->GetFormShell() ? m_pView->GetFormShell()->GetImpl() : NULL;
    if (pShImpl && bDeactivateController)
        pShImpl->setActiveController();
}

//------------------------------------------------------------------------------
void FmXFormView::AttachControl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl > & rControl, sal_Bool bDetach )
{
    /* wird im fmctrler gemacht */
}

//------------------------------------------------------------------------------
void FmXFormView::AttachControls( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > & rCtrlContainer,
                                  sal_Bool bDetach )
{
}

//------------------------------------------------------------------------------
void FmXFormView::smartControlReset( const Reference< XIndexAccess >& _rxModels )
{
    if (!_rxModels.is())
    {
        DBG_ERROR("FmXFormView::smartControlReset: invalid container!");
        return;
    }

    static const ::rtl::OUString sClassIdPropertyName = FM_PROP_CLASSID;
    static const ::rtl::OUString sBoundFieldPropertyName = FM_PROP_BOUNDFIELD;
    sal_Int32 nCount = _rxModels->getCount();
    Reference< XPropertySet > xCurrent;
    Reference< XPropertySetInfo > xCurrentInfo;
    Reference< XPropertySet > xBoundField;

    for (sal_Int32 i=0; i<nCount; ++i)
    {
        _rxModels->getByIndex(i) >>= xCurrent;
        if (xCurrent.is())
            xCurrentInfo = xCurrent->getPropertySetInfo();
        else
            xCurrentInfo.clear();
        if (!xCurrentInfo.is())
            continue;

        if (xCurrentInfo->hasPropertyByName(sClassIdPropertyName))
        {   // it's a control model

            // check if this control is bound to a living database field
            if (xCurrentInfo->hasPropertyByName(sBoundFieldPropertyName))
                xCurrent->getPropertyValue(sBoundFieldPropertyName) >>= xBoundField;
            else
                xBoundField.clear();

            if (!xBoundField.is())
            {   // no, not valid bound -> reset it
                Reference< XReset > xControlReset(xCurrent, UNO_QUERY);
                if (xControlReset.is())
                    xControlReset->reset();
            }
        }
        else
        {
            Reference< XIndexAccess > xContainer(xCurrent, UNO_QUERY);
            if (xContainer.is())
                smartControlReset(xContainer);
        }
    }
}

//------------------------------------------------------------------------------
FmFormShell* FmXFormView::GetFormShell() const
{
    return m_pView ? m_pView->GetFormShell() : NULL;
}
// -----------------------------------------------------------------------------
void FmXFormView::AutoFocus()
{
    if (m_nAutoFocusEvent)
          Application::RemoveUserEvent(m_nAutoFocusEvent);
    m_nAutoFocusEvent = Application::PostUserEvent(LINK(this, FmXFormView, OnAutoFocus));
}
// -----------------------------------------------------------------------------
IMPL_LINK(FmXFormView, OnAutoFocus, void*, EMPTYTAG)
{
    m_nAutoFocusEvent = 0;

    // go to the first form of our page, examine it's TabController, go to it's first (in terms of the tab order)
    // control, give it the focus

    // get the forms collection of the page we belong to
    FmFormPage* pPage = m_pView ? PTR_CAST(FmFormPage, m_pView->GetPageViewPvNum(0)->GetPage()) : NULL;
    Reference< XIndexAccess > xForms;
    if (pPage)
        xForms = Reference< XIndexAccess >(pPage->GetForms(), UNO_QUERY);
    FmXPageViewWinRec* pViewWinRec = m_aWinList.size() ? m_aWinList[0] : NULL;
    if (pViewWinRec)
    {
        try
        {
            // go for the tab controller of the first form
            sal_Int32 nObjects = xForms->getCount();
            ::com::sun::star::uno::Reference< XForm > xForm;
            if (nObjects)
                ::cppu::extractInterface(xForm, xForms->getByIndex(0));

            Reference< XTabController > xTabControllerModel(pViewWinRec->getController( xForm ), UNO_QUERY);
            // go for the first control of the controller
            Sequence< Reference< XControl > > aControls;
            if (xTabControllerModel.is())
                aControls = xTabControllerModel->getControls();
            Reference< XControl > xFirstControl;
            if (aControls.getLength())
                xFirstControl = aControls[0];

            // set the focus to this first control
            Reference< XWindow > xControlWindow(xFirstControl, UNO_QUERY);
            if (xControlWindow.is())
                xControlWindow->setFocus();

            // ensure that the control is visible
            // 80210 - 12/07/00 - FS
            if (xControlWindow.is() && m_pView->GetActualOutDev() && (OUTDEV_WINDOW == m_pView->GetActualOutDev()->GetOutDevType()))
            {
                const Window* pWindow = static_cast<const Window*>(m_pView->GetActualOutDev());
                ::com::sun::star::awt::Rectangle aRect = xControlWindow->getPosSize();
                ::Rectangle aNonUnoRect(aRect.X, aRect.Y, aRect.X + aRect.Width, aRect.Y + aRect.Height);
                m_pView->MakeVisible(pWindow->PixelToLogic(aNonUnoRect), *const_cast<Window*>(pWindow));
            }
        }
        catch(Exception&)
        {
            DBG_ERROR("FmXFormView::OnAutoFocus: could not activate the first control!");
        }
    }
    return 0L;
}
// -----------------------------------------------------------------------------


