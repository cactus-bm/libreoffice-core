/*************************************************************************
 *
 *  $RCSfile: fmundo.cxx,v $
 *
 *  $Revision: 1.29 $
 *
 *  last change: $Author: obo $ $Date: 2005-03-18 10:01:04 $
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

#ifndef _SVX_FMUNDO_HXX
#include "fmundo.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_UTIL_XMODIFYBROADCASTER_HPP_
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORMCONTROLLER_HPP_
#include <com/sun/star/form/XFormController.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINER_HPP_
#include <com/sun/star/container/XContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERLISTENER_HPP_
#include <com/sun/star/container/XContainerListener.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XEVENTATTACHERMANAGER_HPP_
#include <com/sun/star/script/XEventAttacherManager.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_BINDING_XBINDABLEVALUE_HPP_
#include <com/sun/star/form/binding/XBindableValue.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_BINDING_XLISTENTRYSINK_HPP_
#include <com/sun/star/form/binding/XListEntrySink.hpp>
#endif
/** === end UNO includes === **/

#ifndef _FM_FMMODEL_HXX
#include "fmmodel.hxx"
#endif
#ifndef _SVX_FMTOOLS_HXX
#include "fmtools.hxx"
#endif
#ifndef _SVX_FMPAGE_HXX //autogen
#include "fmpage.hxx"
#endif
#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif
#ifndef _SVX_DIALMGR_HXX
#include "dialmgr.hxx"
#endif
#ifndef _SVX_FMUNOPGE_HXX
#include "fmpgeimp.hxx"
#endif
#ifndef SVX_DBTOOLSCLIENT_HXX
#include "dbtoolsclient.hxx"
#endif

#ifndef _SFXMACITEM_HXX //autogen
#include <svtools/macitem.hxx>
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SBXCLASS_HXX //autogen
#include <svtools/sbx.hxx>
#endif

#ifndef _SB_SBUNO_HXX
#include <basic/sbuno.hxx>
#endif

#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif

#ifndef _SFXDOCFILE_HXX //autogen
#include <sfx2/docfile.hxx>
#endif

#ifndef _SFXAPP_HXX //autogen
#include <sfx2/app.hxx>
#endif

#ifndef _SFX_HRC
#include <sfx2/sfx.hrc>
#endif

#ifndef _SFXEVENT_HXX //autogen
#include <sfx2/event.hxx>
#endif

#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif

#ifndef _SVDITER_HXX //autogen
#include "svditer.hxx"
#endif

#ifndef _SVX_FMOBJ_HXX
#include "fmobj.hxx"
#endif

#ifndef _OSL_MUTEX_HXX_ //autogen
#include <osl/mutex.hxx>
#endif

#ifndef _SVX_FMGLOB_HXX
#include "fmglob.hxx"
#endif
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::script;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::form::binding;
using namespace ::svxform;

//------------------------------------------------------------------------------
// some helper structs for caching property infos
//------------------------------------------------------------------------------
struct PropertyInfo
{
    BOOL    bIsTransientOrReadOnly  : 1;    // the property is transient or read-only, thus we need no undo action for it
    BOOL    bIsValueProperty        : 1;    // the property is the special value property, thus it may be handled
                                            // as if it's transient or persistent
};

struct PropertySetInfo
{
    DECLARE_STL_USTRINGACCESS_MAP(PropertyInfo, AllProperties);

    AllProperties   aProps;                 // all properties of this set which we know so far
    BOOL            bHasEmptyControlSource; // sal_True -> the set has a DataField property, and the current value is an empty string
                                            // sal_False -> the set has _no_ such property or it's value isn't empty
};

BOOL operator < (const Reference< XPropertySet >& lhs,
                 const Reference< XPropertySet >& rhs)
{
    return lhs.get() < rhs.get();
}

DECLARE_STL_STDKEY_MAP(Reference< XPropertySet >, PropertySetInfo, PropertySetInfoCache);

//------------------------------------------------------------------------------

String static_STR_UNDO_PROPERTY;
//------------------------------------------------------------------------------
DBG_NAME(FmXUndoEnvironment);
//------------------------------------------------------------------------------
FmXUndoEnvironment::FmXUndoEnvironment(FmFormModel& _rModel)
                   :rModel(_rModel)
                   ,m_Locks( 0 )
                   ,bReadOnly(sal_False)
                   ,m_pPropertySetCache(NULL)
{
    DBG_CTOR(FmXUndoEnvironment,NULL);
}

//------------------------------------------------------------------------------
FmXUndoEnvironment::~FmXUndoEnvironment()
{
    DBG_DTOR(FmXUndoEnvironment,NULL);
    if (m_pPropertySetCache)
        delete static_cast<PropertySetInfoCache*>(m_pPropertySetCache);
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::Clear()
{
    Lock();
    sal_uInt16 nCount = rModel.GetPageCount();
    sal_uInt16 i;
    for (i = 0; i < nCount; i++)
    {
        Reference< XInterface >  xInt(((FmFormPage*)rModel.GetPage(i))->GetForms());
        RemoveElement(xInt);
    }

    nCount = rModel.GetMasterPageCount();
    for (i = 0; i < nCount; i++)
    {
        Reference< XInterface >  xInt(((FmFormPage*)rModel.GetMasterPage(i))->GetForms());
        RemoveElement(xInt);
    }
    UnLock();

    EndListening(*rModel.GetObjectShell());
    if (IsListening(rModel))
        EndListening(rModel);
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::ModeChanged()
{
    if (bReadOnly != (rModel.GetObjectShell()->IsReadOnly() || rModel.GetObjectShell()->IsReadOnlyUI()))
    {
        bReadOnly = !bReadOnly;

        sal_uInt16 nCount = rModel.GetPageCount();
        sal_uInt16 i;
        for (i = 0; i < nCount; i++)
        {
            Reference< XInterface >  xInt(((FmFormPage*)rModel.GetPage(i))->GetForms());
            TogglePropertyListening(xInt);
        }

        nCount = rModel.GetMasterPageCount();
        for (i = 0; i < nCount; i++)
        {
            Reference< XInterface >  xInt(((FmFormPage*)rModel.GetMasterPage(i))->GetForms());
            TogglePropertyListening(xInt);
        }

        if (!bReadOnly)
            StartListening(rModel);
        else
            EndListening(rModel);
    }
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    if (rHint.ISA(SdrHint))
    {
        SdrHint* pSdrHint = (SdrHint*)&rHint;
        switch( pSdrHint->GetKind() )
        {
            case HINT_OBJINSERTED:
            {
                SdrObject* pSdrObj = (SdrObject*)pSdrHint->GetObject();
                Inserted( pSdrObj );
            }   break;
            case HINT_OBJREMOVED:
            {
                SdrObject* pSdrObj = (SdrObject*)pSdrHint->GetObject();
                Removed( pSdrObj );
            }
            break;
        }
    }
    else if (rHint.ISA(SfxSimpleHint))
    {
        switch ( ((SfxSimpleHint&)rHint).GetId() )
        {
            case SFX_HINT_DYING:
                Clear();
                break;
            case SFX_HINT_MODECHANGED:
                ModeChanged();
                break;
        }
    }
    else if (rHint.ISA(SfxEventHint))
    {
        switch (((SfxEventHint&)rHint).GetEventId())
        {
        case SFX_EVENT_CREATEDOC:
            case SFX_EVENT_OPENDOC:
                ModeChanged();
                break;
        }
    }

}

//------------------------------------------------------------------
void FmXUndoEnvironment::Inserted(SdrObject* pObj)
{
    if (bReadOnly)
        return;

    if (pObj->GetObjInventor() == FmFormInventor)
    {
        FmFormObj* pFormObj = PTR_CAST(FmFormObj, pObj);
        Inserted( pFormObj );
    }
    else if (pObj->IsGroupObject())
    {
        SdrObjListIter aIter(*pObj->GetSubList());
        while (aIter.IsMore())
        {
            SdrObject* pObj = aIter.Next();
            Inserted(pObj);
        }
    }
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::Inserted(FmFormObj* pObj)
{
    DBG_ASSERT( pObj, "FmXUndoEnvironment::Inserted: invalid object!" );
    if ( !pObj )
        return;

    // ist das Control noch einer Form zugeordnet
    Reference< XInterface >  xModel = pObj->GetUnoControlModel();
    Reference< XFormComponent >  xContent(xModel, UNO_QUERY);
    if (xContent.is() && pObj->GetPage())
    {
        // Komponente gehoert noch keiner Form an
        if (!xContent->getParent().is())
        {
            // Einfuegen in den Parent falls noetig
            Reference< XIndexContainer >  xParent = pObj->GetParent();
            // Suchen des Form in der aktuellen Page
            Reference< XIndexContainer >  xForm;
            Reference< XInterface >  xIface(xParent, UNO_QUERY);
            Reference< XIndexAccess >  xForms(((FmFormPage*)pObj->GetPage())->GetForms(), UNO_QUERY);;

            if (searchElement(xForms, xIface))
                xForm = xParent;
            else
            {
                Reference< XForm >  xTemp = ((FmFormPage*)pObj->GetPage())->GetImpl()->placeInFormComponentHierarchy(xContent);
                xForm = Reference< XIndexContainer > (xTemp, UNO_QUERY);
            }

            // Position des Elements
            sal_Int32 nPos = xForm->getCount();
            if ((XIndexContainer*)xForm.get() == (XIndexContainer*)xParent.get())
            {
                if (nPos > pObj->GetPos())
                    nPos = xForm->getCount();
            }

            xForm->insertByIndex(nPos, makeAny(xContent));

            Reference< XEventAttacherManager >  xManager(xForm, UNO_QUERY);
            if (xManager.is())
                xManager->registerScriptEvents(nPos, pObj->GetEvents());
        }

        // FormObject zuruecksetzen
        pObj->SetObjEnv(Reference< XIndexContainer > ());
    }
}

//------------------------------------------------------------------
void FmXUndoEnvironment::Removed(SdrObject* pObj)
{
    if (bReadOnly)
        return;

    if (pObj->GetObjInventor() == FmFormInventor)
    {
        FmFormObj* pFormObj = PTR_CAST(FmFormObj, pObj);
        Removed(pFormObj);
    }
    else if (pObj->IsGroupObject())
    {
        SdrObjListIter aIter(*pObj->GetSubList());
        while (aIter.IsMore())
        {
            SdrObject* pObj = aIter.Next();
            Removed(pObj);
        }
    }
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::Removed(FmFormObj* pObj)
{
    DBG_ASSERT( pObj, "FmXUndoEnvironment::Removed: invalid object!" );
    if ( !pObj )
        return;

    // ist das Control noch einer Form zugeordnet
    Reference< XInterface >  xModel = pObj->GetUnoControlModel();
    Reference< XFormComponent >  xContent(xModel, UNO_QUERY);
    if (xContent.is())
    {
        // das Object wird aus einer Liste herausgenommen
        // existiert ein Vater wird das Object beim beim Vater entfernt und
        // am FormObject gemerkt!

        // wird das Object wieder eingefuegt und ein Parent existiert, so wird dieser
        // Parent wiederum gesetzt
        Reference< XIndexContainer >  xForm(xContent->getParent(), UNO_QUERY);
        if (xForm.is())
        {
            Reference< XIndexAccess >  xIndexAccess((XIndexContainer*)xForm.get());
            // Feststellen an welcher Position sich das Kind befunden hat
            sal_Int32 nPos = getElementPos(xIndexAccess, xContent);
            if (nPos >= 0)
            {
                Sequence< ScriptEventDescriptor > aEvts;
                Reference< XEventAttacherManager >  xManager(xForm, UNO_QUERY);
                if (xManager.is())
                    aEvts = xManager->getScriptEvents(nPos);

                try
                {
                    pObj->SetObjEnv(xForm, nPos, aEvts);
                    xForm->removeByIndex(nPos);
                }
                catch(Exception&)
                {
                }

            }
        }
    }
}

//  XEventListener
//------------------------------------------------------------------------------
void SAL_CALL FmXUndoEnvironment::disposing(const EventObject& e) throw( RuntimeException )
{
    // check if it's an object we have cached informations about
    if (m_pPropertySetCache)
    {
        Reference< XPropertySet > xSourceSet(e.Source, UNO_QUERY);
        if (xSourceSet.is())
        {
            PropertySetInfoCache* pCache = static_cast<PropertySetInfoCache*>(m_pPropertySetCache);
            PropertySetInfoCacheIterator aSetPos = pCache->find(xSourceSet);
            if (aSetPos != pCache->end())
                pCache->erase(aSetPos);
        }
    }
}

// XPropertyChangeListener
//------------------------------------------------------------------------------
void SAL_CALL FmXUndoEnvironment::propertyChange(const PropertyChangeEvent& evt) throw(::com::sun::star::uno::RuntimeException)
{
    ::osl::ClearableMutexGuard aGuard( m_aMutex );

    if (!IsLocked())
    {
        Reference< XPropertySet >  xSet(evt.Source, UNO_QUERY);
        if (!xSet.is())
            return;

        // if it's a "default value" property of a control model, set the according "value" property
        static const sal_Char* pDefaultValueProperties[] = {
            FM_PROP_DEFAULT_TEXT, FM_PROP_DEFAULTCHECKED, FM_PROP_DEFAULT_DATE, FM_PROP_DEFAULT_TIME,
            FM_PROP_DEFAULT_VALUE, FM_PROP_DEFAULT_SELECT_SEQ, FM_PROP_EFFECTIVE_DEFAULT
        };
        const ::rtl::OUString aValueProperties[] = {
            FM_PROP_TEXT, FM_PROP_STATE, FM_PROP_DATE, FM_PROP_TIME,
            FM_PROP_VALUE, FM_PROP_SELECT_SEQ, FM_PROP_EFFECTIVE_VALUE
        };
        sal_Int32 nDefaultValueProps = sizeof(pDefaultValueProperties)/sizeof(pDefaultValueProperties[0]);
        OSL_ENSURE(sizeof(aValueProperties)/sizeof(aValueProperties[0]) == nDefaultValueProps,
            "FmXUndoEnvironment::propertyChange: inconsistence!");
        for (sal_Int32 i=0; i<nDefaultValueProps; ++i)
        {
            if (0 == evt.PropertyName.compareToAscii(pDefaultValueProperties[i]))
            {
                try
                {
                    xSet->setPropertyValue(aValueProperties[i], evt.NewValue);
                }
                catch(const Exception&)
                {
                    OSL_ENSURE(sal_False, "FmXUndoEnvironment::propertyChange: could not adjust the value property!");
                }
            }
        }

        // no Undo for transient and readonly props. But unfortunately "transient" is not only that the
        // "transient" flag is set for the property in question, instead is is somewhat more complex
        // Transience criterions are:
        // - the "transient" flag is set for the property
        // - OR the control has a non-empty COntrolSource property, i.e. is intended to be bound
        //   to a database column. Note that it doesn't matter here whether the control actually
        //   *is* bound to a column
        // - OR the control is bound to an external value via XBindableValue/XValueBinding

        if (!m_pPropertySetCache)
            m_pPropertySetCache = new PropertySetInfoCache;
        PropertySetInfoCache* pCache = static_cast<PropertySetInfoCache*>(m_pPropertySetCache);

        // let's see if we know something about the set
        PropertySetInfoCacheIterator aSetPos = pCache->find(xSet);
        if (aSetPos == pCache->end())
        {
            PropertySetInfo aNewEntry;
            if (!::comphelper::hasProperty(FM_PROP_CONTROLSOURCE, xSet))
            {
                aNewEntry.bHasEmptyControlSource = sal_False;
            }
            else
            {
                try
                {
                    Any aCurrentControlSource = xSet->getPropertyValue(FM_PROP_CONTROLSOURCE);
                    aNewEntry.bHasEmptyControlSource = !aCurrentControlSource.hasValue() || (::comphelper::getString(aCurrentControlSource).getLength() == 0);
                }
                catch(Exception&)
                {
                }
            }
            aSetPos = pCache->insert(PropertySetInfoCache::value_type(xSet,aNewEntry)).first;
            DBG_ASSERT(aSetPos != pCache->end(), "FmXUndoEnvironment::propertyChange : just inserted it ... why it's not there ?");
        }
        else
        {   // is it the DataField property ?
            if (evt.PropertyName.equals(FM_PROP_CONTROLSOURCE))
            {
                aSetPos->second.bHasEmptyControlSource = !evt.NewValue.hasValue() || (::comphelper::getString(evt.NewValue).getLength() == 0);
            }
        }

        // now we have access to the cached info about the set
        // let's see what we know about the property
        PropertySetInfo::AllProperties& rPropInfos = aSetPos->second.aProps;
        PropertySetInfo::AllPropertiesIterator aPropertyPos = rPropInfos.find(evt.PropertyName);
        if (aPropertyPos == rPropInfos.end())
        {   // nothing 'til now ... have to change this ....
            PropertyInfo aNewEntry;

            // the attributes
            INT32 nAttributes = xSet->getPropertySetInfo()->getPropertyByName(evt.PropertyName).Attributes;
            aNewEntry.bIsTransientOrReadOnly = ((nAttributes & PropertyAttribute::READONLY) != 0) || ((nAttributes & PropertyAttribute::TRANSIENT) != 0);

            // check if it is the special "DataFieldProperty"
            aNewEntry.bIsValueProperty = sal_False;
            try
            {
                if (::comphelper::hasProperty(FM_PROP_CONTROLSOURCEPROPERTY, xSet))
                {
                    Any aControlSourceProperty = xSet->getPropertyValue(FM_PROP_CONTROLSOURCEPROPERTY);
                    ::rtl::OUString sControlSourceProperty;
                    aControlSourceProperty >>= sControlSourceProperty;

                    aNewEntry.bIsValueProperty = (sControlSourceProperty.equals(evt.PropertyName));
                }
            }
            catch(Exception&)
            {
            }

            // insert the new entry
            aPropertyPos = rPropInfos.insert(PropertySetInfo::AllProperties::value_type(evt.PropertyName,aNewEntry)).first;
            DBG_ASSERT(aPropertyPos != rPropInfos.end(), "FmXUndoEnvironment::propertyChange : just inserted it ... why it's not there ?");
        }

        // now we have access to the cached info about the property affected
        // and are able to decide wether or not we need an undo action

        bool bAddUndoAction = true;
        // no UNDO for transient/readonly properties
        if ( aPropertyPos->second.bIsTransientOrReadOnly )
            bAddUndoAction = false;

        if ( bAddUndoAction && aPropertyPos->second.bIsValueProperty )
        {
            // no UNDO when the "value" property changes, but the ControlSource is non-empty
            // (in this case the control is intended to be bound to a database column)
            if ( !aSetPos->second.bHasEmptyControlSource )
                bAddUndoAction = false;

            // no UNDO if the control is currently bound to an external value
            if ( bAddUndoAction )
            {
                Reference< XBindableValue > xBindable( evt.Source, UNO_QUERY );
                bAddUndoAction = !xBindable.is() || !xBindable->getValueBinding().is();
            }
        }

        if ( bAddUndoAction && ( evt.PropertyName == FM_PROP_STRINGITEMLIST ) )
        {
            Reference< XListEntrySink > xSink( evt.Source, UNO_QUERY );
            if ( xSink.is() && xSink->getListEntrySource().is() )
                // #i41029# / 2005-01-31 / frank.schoenheit@sun.com
                bAddUndoAction = false;
        }

        if ( bAddUndoAction )
        {
            aGuard.clear();
            // TODO: this is a potential race condition: two threads here could in theory
            // add their undo actions out-of-order

            ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
            rModel.AddUndo(new FmUndoPropertyAction(rModel, evt));
        }
    }
    else
    {
        // if it's the DataField property we may have to adjust our cache
        if (m_pPropertySetCache && evt.PropertyName.equals(FM_PROP_CONTROLSOURCE))
        {
            Reference< XPropertySet >  xSet(evt.Source, UNO_QUERY);
            PropertySetInfoCache* pCache = static_cast<PropertySetInfoCache*>(m_pPropertySetCache);
            PropertySetInfo& rSetInfo = (*pCache)[xSet];
            rSetInfo.bHasEmptyControlSource = !evt.NewValue.hasValue() || (::comphelper::getString(evt.NewValue).getLength() == 0);
        }
    }
}

// XContainerListener
//------------------------------------------------------------------------------
void SAL_CALL FmXUndoEnvironment::elementInserted(const ContainerEvent& evt) throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
    ::osl::MutexGuard aGuard( m_aMutex );

    // neues Object zum lauschen
    Reference< XInterface >  xIface;
    evt.Element >>= xIface;
    OSL_ENSURE(xIface.is(), "FmXUndoEnvironment::elementInserted: invalid container notification!");
    AddElement(xIface);

    implSetModified();
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::implSetModified()
{
    if ( !IsLocked() && rModel.GetObjectShell() )
    {
        rModel.GetObjectShell()->SetModified( sal_True );
    }
}

//------------------------------------------------------------------------------
void SAL_CALL FmXUndoEnvironment::elementReplaced(const ContainerEvent& evt) throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
    ::osl::MutexGuard aGuard( m_aMutex );

    Reference< XInterface >  xIface;
    evt.ReplacedElement >>= xIface;
    OSL_ENSURE(xIface.is(), "FmXUndoEnvironment::elementReplaced: invalid container notification!");
    RemoveElement(xIface);

    evt.Element >>= xIface;
    AddElement(xIface);

    implSetModified();
}

//------------------------------------------------------------------------------
void SAL_CALL FmXUndoEnvironment::elementRemoved(const ContainerEvent& evt) throw(::com::sun::star::uno::RuntimeException)
{
    ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );
    ::osl::MutexGuard aGuard( m_aMutex );

    Reference< XInterface >  xIface( evt.Element, UNO_QUERY );
    OSL_ENSURE(xIface.is(), "FmXUndoEnvironment::elementRemoved: invalid container notification!");
    RemoveElement(xIface);

    implSetModified();
}

//------------------------------------------------------------------------------
void SAL_CALL FmXUndoEnvironment::modified( const EventObject& aEvent ) throw (RuntimeException)
{
    implSetModified();
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::AddForms(const Reference< XNameContainer > & rForms)
{
    Lock();
    Reference< XInterface >  xInt = rForms;
    AddElement(xInt);
    UnLock();
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::RemoveForms(const Reference< XNameContainer > & rForms)
{
    Lock();
    Reference< XInterface >  xInt = rForms;
    RemoveElement(xInt);
    UnLock();
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::TogglePropertyListening(const Reference< XInterface > & Element)
{
    // am Container horchen
    Reference< XIndexContainer >  xContainer(Element, UNO_QUERY);
    if (xContainer.is())
    {
        sal_uInt32 nCount = xContainer->getCount();
        Reference< XInterface >  xIface;
        for (sal_uInt32 i = 0; i < nCount; i++)
        {
            xContainer->getByIndex(i) >>= xIface;
            TogglePropertyListening(xIface);
        }
    }

    Reference< XPropertySet >  xSet(Element, UNO_QUERY);
    if (xSet.is())
    {
        if (!bReadOnly)
            xSet->addPropertyChangeListener( ::rtl::OUString(), this );
        else
            xSet->removePropertyChangeListener( ::rtl::OUString(), this );
    }
}


//------------------------------------------------------------------------------
void FmXUndoEnvironment::switchListening( const Reference< XIndexContainer >& _rxContainer, bool _bStartListening ) SAL_THROW(())
{
    OSL_PRECOND( _rxContainer.is(), "FmXUndoEnvironment::switchListening: invalid container!" );
    if ( !_rxContainer.is() )
        return;

    try
    {
        // if it's an EventAttacherManager, then we need to listen for
        // script events
        Reference< XEventAttacherManager > xManager( _rxContainer, UNO_QUERY );
        if ( xManager.is() )
            if ( _bStartListening )
                xManager->addScriptListener( this );
            else
                xManager->removeScriptListener( this );

        // also handle all children of this element
        sal_uInt32 nCount = _rxContainer->getCount();
        Reference< XInterface > xInterface;
        for ( sal_uInt32 i = 0; i < nCount; ++i )
        {
            _rxContainer->getByIndex( i ) >>= xInterface;
            if ( _bStartListening )
                AddElement( xInterface );
            else
                RemoveElement( xInterface );
        }

        // be notified of any changes in the container elements
        Reference< XContainer > xSimpleContainer( _rxContainer, UNO_QUERY );
        OSL_ENSURE( xSimpleContainer.is(), "FmXUndoEnvironment::switchListening: how are we expected to be notified of changes in the container?" );
        if ( xSimpleContainer.is() )
            if ( _bStartListening )
                xSimpleContainer->addContainerListener( this );
            else
                xSimpleContainer->removeContainerListener( this );
    }
    catch( const Exception& )
    {
        OSL_ENSURE( sal_False, "FmXUndoEnvironment::switchListening: caught an exception!" );
    }
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::switchListening( const Reference< XInterface >& _rxObject, bool _bStartListening ) SAL_THROW(())
{
    OSL_PRECOND( _rxObject.is(), "FmXUndoEnvironment::switchListening: how should I listen at a NULL object?" );

    try
    {
        if ( !bReadOnly )
        {
            Reference< XPropertySet > xProps( _rxObject, UNO_QUERY );
            if ( xProps.is() )
                if ( _bStartListening )
                    xProps->addPropertyChangeListener( ::rtl::OUString(), this );
                else
                    xProps->removePropertyChangeListener( ::rtl::OUString(), this );
        }

        Reference< XModifyBroadcaster > xBroadcaster( _rxObject, UNO_QUERY );
        if ( xBroadcaster.is() )
            if ( _bStartListening )
                xBroadcaster->addModifyListener( this );
            else
                xBroadcaster->removeModifyListener( this );
    }
    catch( const Exception& )
    {
        OSL_ENSURE( sal_False, "FmXUndoEnvironment::switchListening: caught an exception!" );
    }
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::AddElement(const Reference< XInterface >& _rxElement )
{
    // am Container horchen
    Reference< XIndexContainer > xContainer( _rxElement, UNO_QUERY );
    if ( xContainer.is() )
        switchListening( xContainer, true );

    switchListening( _rxElement, true );
}

//------------------------------------------------------------------------------
void FmXUndoEnvironment::RemoveElement(const Reference< XInterface >& _rxElement)
{
    switchListening( _rxElement, false );

    if (!bReadOnly)
    {
        // reset the ActiveConnection if the form is to be removed. This will (should) free the resources
        // associated with this connection
        // 86299 - 05/02/2001 - frank.schoenheit@germany.sun.com
        Reference< XForm > xForm( _rxElement, UNO_QUERY );
        Reference< XPropertySet > xFormProperties( xForm, UNO_QUERY );
        if ( xFormProperties.is() )
            if ( !::svxform::OStaticDataAccessTools().isEmbeddedInDatabase( _rxElement ) )
                // (if there is a connection in the context of the component, setting
                // a new connection would be vetoed, anyway)
                // #i34196# - 2004-09-21 - fs@openoffice.org
                xFormProperties->setPropertyValue( FM_PROP_ACTIVE_CONNECTION, Any() );
    }

    Reference< XIndexContainer > xContainer( _rxElement, UNO_QUERY );
    if ( xContainer.is() )
        switchListening( xContainer, false );
}


// XScriptListener
void FmXUndoEnvironment::firing_Impl( const ScriptEvent& evt, Any *pSyncRet )
{
    OSL_TRACE( "in FmXUndoEnvironment::firing_Impl");
    ::vos::OClearableGuard aSolarGuard( Application::GetSolarMutex() );

    SfxObjectShellRef xObjSh = rModel.GetObjectShell();
    if( !xObjSh.Is() )
        return;

    {
        Reference< XInterface >  xThis;
        evt.Helper >>= xThis;

        aSolarGuard.clear();
        if (xThis.is())
        {
            ::rtl::OUString sScriptType = evt.ScriptType;
            ::rtl::OUString sScriptCode = evt.ScriptCode;
            Sequence< Any > aArguments = evt.Arguments;

            ::rtl::OUString sMacroLocation;

            // the object shell still want's the script in the old format where neither "document" nor "application" is prepended
            if ( 0 == sScriptType.compareToAscii( "StarBasic" ) )
            {   // it's a starbasic script
                sal_Int32 nPrefixLen = sScriptCode.indexOf( ':' );
                DBG_ASSERT( 0 <= nPrefixLen, "FmXUndoEnvironment::firing_Impl: Basic script name in old format encountered!" );

                if ( 0 <= nPrefixLen )
                {
                    // and it has such a prefix
                    sMacroLocation = sScriptCode.copy( 0, nPrefixLen );
                    DBG_ASSERT( 0 == sMacroLocation.compareToAscii( "document" )
                            ||  0 == sMacroLocation.compareToAscii( "application" ),
                            "FmXUndoEnvironment::firing_Impl: invalid (unknown) prefix!" );

                    // strip the prefix: the SfxObjectShell::CallScript knows nothing about such prefixes
                    sScriptCode = sScriptCode.copy( nPrefixLen + 1 );

                    // (On the medium run, we should migrate to the mechanism where scripts are executed via
                    // XDispatch (or whatever they are planning to use). But at the moment this mechanism is not implemented at all ...)
                }

                if ( sMacroLocation.getLength() )
                {   // we have a macro in the "new" runtime format (fully described)
                    xObjSh->CallStarBasicScript( sScriptCode, sMacroLocation, static_cast< void* >( &aArguments ), pSyncRet );
                }
                else
                {   // we have a script in the old format
                    xObjSh->CallScript( sScriptType, sScriptCode, xThis, static_cast< void* >( &aArguments ), pSyncRet );
                }

            }
            else
            {

                Any aRet;

                ::com::sun::star::uno::Sequence< sal_Int16 > aOutArgsIndex;
                ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > aOutArgs;
                xObjSh->CallXScript( sScriptCode,  aArguments, aRet,
                                    aOutArgsIndex, aOutArgs);
            }

        }

    }

    // Objectshells are not thread safe, so guard the destruction
    {
        ::vos::OGuard aSolarGuard( Application::GetSolarMutex() );
        xObjSh = NULL;
    }
    OSL_TRACE( "leaving FmXUndoEnvironment::firing_Impl");
}

void SAL_CALL FmXUndoEnvironment::firing(const ScriptEvent& evt) throw(::com::sun::star::uno::RuntimeException)
{
    firing_Impl( evt );
}

//------------------------------------------------------------------------------
Any SAL_CALL FmXUndoEnvironment::approveFiring(const ScriptEvent& evt) throw(::com::sun::star::reflection::InvocationTargetException, ::com::sun::star::uno::RuntimeException)
{
    Any aRet;
    firing_Impl( evt, &aRet );
    return aRet;
}

//------------------------------------------------------------------------------
FmUndoPropertyAction::FmUndoPropertyAction(FmFormModel& rNewMod, const PropertyChangeEvent& evt)
                     :SdrUndoAction(rNewMod)
                     ,aPropertyName(evt.PropertyName)
                     ,aNewValue(evt.NewValue)
                     ,aOldValue(evt.OldValue)
                     ,xObj(evt.Source, UNO_QUERY)
{
    if (rNewMod.GetObjectShell())
        rNewMod.GetObjectShell()->SetModified(sal_True);
    if(static_STR_UNDO_PROPERTY.Len() == 0)
        static_STR_UNDO_PROPERTY = SVX_RES(RID_STR_UNDO_PROPERTY);
}


//------------------------------------------------------------------------------
void FmUndoPropertyAction::Undo()
{
    FmXUndoEnvironment& rEnv = ((FmFormModel&)rMod).GetUndoEnv();

    if (xObj.is() && !rEnv.IsLocked())
    {
        rEnv.Lock();
        try
        {
            xObj->setPropertyValue( aPropertyName, aOldValue );
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FmUndoPropertyAction::Undo: caught an exception!" );
        }
        rEnv.UnLock();
    }
}

//------------------------------------------------------------------------------
void FmUndoPropertyAction::Redo()
{
    FmXUndoEnvironment& rEnv = ((FmFormModel&)rMod).GetUndoEnv();

    if (xObj.is() && !rEnv.IsLocked())
    {
        rEnv.Lock();
        try
        {
            xObj->setPropertyValue( aPropertyName, aNewValue );
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FmUndoPropertyAction::Redo: caught an exception!" );
        }
        rEnv.UnLock();
    }
}

//------------------------------------------------------------------------------
String FmUndoPropertyAction::GetComment() const
{
    String aStr(static_STR_UNDO_PROPERTY);

    aStr.SearchAndReplace( '#', aPropertyName );
    return aStr;
}


DBG_NAME(FmUndoContainerAction);
//------------------------------------------------------------------------------
FmUndoContainerAction::FmUndoContainerAction(FmFormModel& rMod,
                                             Action _eAction,
                                             const Reference< XIndexContainer > & xCont,
                                             const Reference< XInterface > & xElem,
                                             sal_Int32 nIdx)
                      :SdrUndoAction(rMod)
                      ,m_eAction( _eAction )
                      ,m_xContainer( xCont )
                      ,m_nIndex( nIdx )
{
    OSL_ENSURE( nIdx >= 0, "FmUndoContainerAction::FmUndoContainerAction: invalid index!" );
        // some old code suggested this could be a valid argument. However, this code was
        // buggy, and it *seemed* that nobody used it - so it was removed.

    DBG_CTOR(FmUndoContainerAction,NULL);
    if ( xCont.is() && xElem.is() )
    {
        // normalize
        m_xElement = m_xElement.query( xElem );
        switch ( m_eAction )
        {
        case Removed:
            if (m_nIndex >= 0)
            {
                Reference< XEventAttacherManager >  xManager( xCont, UNO_QUERY );
                if ( xManager.is() )
                    m_aEvents = xManager->getScriptEvents(m_nIndex);
            }
            else
                m_xElement = NULL;

            // we now own the element
            m_xOwnElement = m_xElement;
            break;
        }
    }
}

//------------------------------------------------------------------------------
FmUndoContainerAction::~FmUndoContainerAction()
{
    // if we own the object ....
    Reference< XComponent > xComp( m_xOwnElement, UNO_QUERY );
    if ( xComp.is() )
    {
        // and the object does not have a parent
        Reference< XChild >  xChild( m_xOwnElement, UNO_QUERY );
        if ( xChild.is() && !xChild->getParent().is() )
            // -> dispose it
            xComp->dispose();
    }
    DBG_DTOR(FmUndoContainerAction,NULL);
}

//------------------------------------------------------------------------------
void FmUndoContainerAction::implReInsert( ) SAL_THROW( ( Exception ) )
{
    if ( m_xContainer->getCount() >= m_nIndex )
    {
        // insert the element
        Any aVal;
        if ( m_xContainer->getElementType() == ::getCppuType( static_cast< const Reference< XFormComponent >* >( NULL ) ) )
        {
            aVal <<= Reference< XFormComponent >( m_xElement, UNO_QUERY );
        }
        else
        {
            aVal <<= Reference< XForm >( m_xElement, UNO_QUERY );
        }
        m_xContainer->insertByIndex( m_nIndex, aVal );

        OSL_ENSURE( getElementPos( m_xContainer.get(), m_xElement ) == m_nIndex, "FmUndoContainerAction::implReInsert: insertion did not work!" );

        // register the events
        Reference< XEventAttacherManager >  xManager( m_xContainer, UNO_QUERY );
        if ( xManager.is() )
            xManager->registerScriptEvents( m_nIndex, m_aEvents );

        // we don't own the object anymore
        m_xOwnElement = NULL;
    }
}

//------------------------------------------------------------------------------
void FmUndoContainerAction::implReRemove( ) SAL_THROW( ( Exception ) )
{
    Reference< XInterface > xElement;
    if ( ( m_nIndex >= 0 ) && ( m_nIndex < m_xContainer->getCount() ) )
        m_xContainer->getByIndex( m_nIndex ) >>= xElement;

    if ( xElement != m_xElement )
    {
        // the indexes in the container changed. Okay, so go the long way and
        // manually determine the index
        m_nIndex = getElementPos( m_xContainer.get(), m_xElement );
        if ( m_nIndex != -1 )
            xElement = m_xElement;
    }

    OSL_ENSURE( xElement == m_xElement, "FmUndoContainerAction::implReRemove: cannot find the element which I'm responsible for!" );
    if ( xElement == m_xElement )
    {
        Reference< XEventAttacherManager >  xManager( m_xContainer, UNO_QUERY );
        if ( xManager.is() )
            m_aEvents = xManager->getScriptEvents( m_nIndex );
        m_xContainer->removeByIndex( m_nIndex );
        // from now on, we own this object
        m_xOwnElement = m_xElement;
    }
}

//------------------------------------------------------------------------------
void FmUndoContainerAction::Undo()
{
    FmXUndoEnvironment& rEnv = static_cast< FmFormModel& >( rMod ).GetUndoEnv();

    if ( m_xContainer.is() && !rEnv.IsLocked() && m_xElement.is() )
    {
        rEnv.Lock();
        try
        {
            switch ( m_eAction )
            {
            case Inserted:
                implReRemove();
                break;

            case Removed:
                implReInsert();
                break;
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FmUndoContainerAction::Undo: caught an exception!" );
        }
        rEnv.UnLock();
    }
}

//------------------------------------------------------------------------------
void FmUndoContainerAction::Redo()
{
    FmXUndoEnvironment& rEnv = static_cast< FmFormModel& >( rMod ).GetUndoEnv();
    if ( m_xContainer.is() && !rEnv.IsLocked() && m_xElement.is() )
    {
        rEnv.Lock();
        try
        {
            switch ( m_eAction )
            {
            case Inserted:
                implReInsert();
                break;

            case Removed:
                implReRemove();
                break;
            }
        }
        catch( const Exception& )
        {
            OSL_ENSURE( sal_False, "FmUndoContainerAction::Redo: caught an exception!" );
        }
        rEnv.UnLock();
    }
}

//------------------------------------------------------------------------------
FmUndoModelReplaceAction::FmUndoModelReplaceAction(FmFormModel& _rMod, SdrUnoObj* _pObject, const Reference< XControlModel > & _xReplaced)
    :SdrUndoAction(_rMod)
    ,m_xReplaced(_xReplaced)
    ,m_pObject(_pObject)
{
}

//------------------------------------------------------------------------------
FmUndoModelReplaceAction::~FmUndoModelReplaceAction()
{
    // dispose our element if nobody else is responsible for
    Reference< XComponent >  xComp(m_xReplaced, UNO_QUERY);
    if (xComp.is())
    {
        Reference< XChild >  xChild(m_xReplaced, UNO_QUERY);
        if (!xChild.is() || !xChild->getParent().is())
            xComp->dispose();
    }
}

//------------------------------------------------------------------------------
void FmUndoModelReplaceAction::Undo()
{
    try
    {
        Reference< XControlModel > xCurrentModel( m_pObject->GetUnoControlModel() );

        // replace the model within the parent
        Reference< XChild > xCurrentAsChild( xCurrentModel, UNO_QUERY );
        Reference< XNameContainer > xCurrentsParent;
        if ( xCurrentAsChild.is() )
            xCurrentsParent = xCurrentsParent.query( xCurrentAsChild->getParent() );
        DBG_ASSERT( xCurrentsParent.is(), "FmUndoModelReplaceAction::Undo: invalid current model!" );

        if ( xCurrentsParent.is() )
        {
            // the form container works with FormComponents
            Reference< XFormComponent > xComponent( m_xReplaced, UNO_QUERY );
            DBG_ASSERT( xComponent.is(), "FmUndoModelReplaceAction::Undo: the new model is no form component !" );

            Reference< XPropertySet > xCurrentAsSet( xCurrentModel, UNO_QUERY );
            DBG_ASSERT( ::comphelper::hasProperty(FM_PROP_NAME, xCurrentAsSet ), "FmUndoModelReplaceAction::Undo : one of the models is invalid !");

            ::rtl::OUString sName;
            xCurrentAsSet->getPropertyValue( FM_PROP_NAME ) >>= sName;
            xCurrentsParent->replaceByName( sName, makeAny( xComponent ) );

            m_pObject->SetUnoControlModel(m_xReplaced);
            m_pObject->SetChanged();

            m_xReplaced = xCurrentModel;
        }
    }
    catch(Exception&)
    {
        DBG_ERROR("FmUndoModelReplaceAction::Undo : could not replace the model !");
    }
}

//------------------------------------------------------------------------------
String FmUndoModelReplaceAction::GetComment() const
{
    return SVX_RES(RID_STR_UNDO_MODEL_REPLACE);
}




