/*************************************************************************
 *
 *  $RCSfile: fmctrler.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: fs $ $Date: 2001-05-29 14:15:08 $
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

#define USE_PROPERTY_HELPER

#ifndef _SVX_FMCTRLER_HXX
#include "fmctrler.hxx"
#endif
#ifndef _SVX_FMTOOLS_HXX
#include "fmtools.hxx"
#endif
#ifndef _SVX_FMPROP_HRC
#include "fmprop.hrc"
#endif
#ifndef _SVX_FMURL_HXX
#include "fmurl.hxx"
#endif
#ifndef _SVX_FMSHELL_HXX
#include "fmshell.hxx"
#endif

#ifndef _COM_SUN_STAR_SDB_ROWCHANGEACTION_HPP_
#include <com/sun/star/sdb/RowChangeAction.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_COLUMNVALUE_HPP_
#include <com/sun/star/sdbc/ColumnValue.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XLOADABLE_HPP_
#include <com/sun/star/form/XLoadable.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XCOMBOBOX_HPP_
#include <com/sun/star/awt/XComboBox.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCHECKBOX_HPP_
#include <com/sun/star/awt/XCheckBox.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XLISTBOX_HPP_
#include <com/sun/star/awt/XListBox.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FOCUSCHANGEREASON_HPP_
#include <com/sun/star/awt/FocusChangeReason.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif


#ifndef _COM_SUN_STAR_FORM_XRESET_HPP_
#include <com/sun/star/form/XReset.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_TABULATORCYCLE_HPP_
#include <com/sun/star/form/TabulatorCycle.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XBOUNDCONTROL_HPP_
#include <com/sun/star/form/XBoundControl.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCONTROLLER_HPP_
#include <com/sun/star/frame/XController.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XVCLWINDOWPEER_HPP_
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#endif

#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif

#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif

#ifndef _SVDPAGV_HXX //autogen
#include <svdpagv.hxx>
#endif

#ifndef _SVX_FMSHIMP_HXX
#include <fmshimp.hxx>
#endif

#ifndef _SVX_FMVIEW_HXX
#include <fmview.hxx>
#endif

#ifndef _SVX_FMFILTER_HXX
#include "fmfilter.hxx"
#endif

#ifndef _SVX_DBERRBOX_HXX
#include "dbmsgbox.hxx"
#endif

#ifndef _SV_MSGBOX_HXX //autogen wg. RET_YES
#include <vcl/msgbox.hxx>
#endif

#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif

#ifndef _SFXVIEWSH_HXX
#include <sfx2/viewsh.hxx>
#endif
#ifndef _SFXVIEWFRM_HXX
#include <sfx2/viewfrm.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX
#include <sfx2/bindings.hxx>
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SVX_DIALMGR_HXX
#include "dialmgr.hxx"
#endif

#ifndef _SVX_FMPROP_HXX
#include "fmprop.hxx"
#endif
#ifndef _SVX_FMSERVS_HXX
#include "fmservs.hxx"
#endif

#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_AGGREGATION_HXX_
#include <comphelper/propagg.hxx>
#endif
#ifndef _COMPHELPER_ENUMHELPER_HXX_
#include <comphelper/enumhelper.hxx>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include <connectivity/dbconversion.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _CPPUHELPER_QUERYINTERFACE_HXX_
#include <cppuhelper/queryinterface.hxx>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _TOOLKIT_UNOHLP_HXX
#include <toolkit/helper/vclunohelper.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _COMPHELPER_INTERACTION_HXX_
#include <comphelper/interaction.hxx>
#endif
#ifndef _COM_SUN_STAR_SDB_XINTERACTIONSUPPLYPARAMETERS_HPP_
#include <com/sun/star/sdb/XInteractionSupplyParameters.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_PARAMETERSREQUEST_HPP_
#include <com/sun/star/sdb/ParametersRequest.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::beans;
using namespace ::comphelper;
using namespace ::dbtools;
using namespace ::svxform;
using namespace ::connectivity;

extern sal_uInt16 AutoSlotMap[];

//==================================================================
// FmXAutoControl
//==================================================================
class OParameterContinuation : public OInteraction< XInteractionSupplyParameters >
{
    Sequence< PropertyValue >       m_aValues;

public:
    OParameterContinuation() { }

    Sequence< PropertyValue >   getValues() const { return m_aValues; }

// XInteractionSupplyParameters
    virtual void SAL_CALL setParameters( const Sequence< PropertyValue >& _rValues ) throw(RuntimeException);
};

//------------------------------------------------------------------
void SAL_CALL OParameterContinuation::setParameters( const Sequence< PropertyValue >& _rValues ) throw(RuntimeException)
{
    m_aValues = _rValues;
}

//==================================================================
// FmXAutoControl
//==================================================================
struct FmFieldInfo
{
    rtl::OUString       aFieldName;
    Reference< XPropertySet >   xField;
    Reference< ::com::sun::star::awt::XTextComponent >  xText;

    FmFieldInfo(const Reference< XPropertySet >& _xField, const Reference< ::com::sun::star::awt::XTextComponent >& _xText)
        :xField(_xField)
        ,xText(_xText)
    {xField->getPropertyValue(FM_PROP_NAME) >>= aFieldName;}
};

//==================================================================
// FmXAutoControl
//==================================================================
class FmXAutoControl: public UnoControl

{
    //  friend Reflection* ::getCppuType((const FmXAutoControl*)0);
    friend Reference< XInterface > SAL_CALL FmXAutoControl_NewInstance_Impl();

public:
    FmXAutoControl(){}

// automatisch auskommentiert - [getImplementation] - Wird von OWeakObject nicht weiter unterst�tzt!
//  virtual void* getImplementation(Reflection * pRef);

    virtual ::rtl::OUString GetComponentServiceName() {return ::rtl::OUString::createFromAscii("Edit");}
    virtual void SAL_CALL createPeer( const Reference< ::com::sun::star::awt::XToolkit > & rxToolkit, const Reference< ::com::sun::star::awt::XWindowPeer >  & rParentPeer );

protected:
    virtual void ImplSetPeerProperty( const ::rtl::OUString& rPropName, const Any& rVal );
};

//------------------------------------------------------------------------------
void FmXAutoControl::createPeer( const Reference< ::com::sun::star::awt::XToolkit > & rxToolkit, const Reference< ::com::sun::star::awt::XWindowPeer >  & rParentPeer )
{
    UnoControl::createPeer( rxToolkit, rParentPeer );

    Reference< ::com::sun::star::awt::XTextComponent >  xText(mxPeer , UNO_QUERY);
    if (xText.is())
    {
        xText->setText(::rtl::OUString(SVX_RES(RID_STR_AUTOFIELD)));
        xText->setEditable(sal_False);
    }
}

//------------------------------------------------------------------------------
void FmXAutoControl::ImplSetPeerProperty( const ::rtl::OUString& rPropName, const Any& rVal )
{
    // these properties are ignored
    if (rPropName == FM_PROP_TEXT)
        return;

    UnoControl::ImplSetPeerProperty( rPropName, rVal );
}

//------------------------------------------------------------------------------
IMPL_LINK(FmXFormController, OnTimeout, void*, EMPTYTAG)
{
    activateTabOrder();
    return 1;
}

/*************************************************************************/

//------------------------------------------------------------------
Reference< XInterface > SAL_CALL
    FmXFormController_NewInstance_Impl(const Reference< ::com::sun::star::lang::XMultiServiceFactory > & _rxORB)
{
    return *(new FmXFormController(_rxORB));
}

//------------------------------------------------------------------
namespace fmctrlr
{
    const ::rtl::OUString& getDataModeIdentifier()
    {
        static ::rtl::OUString s_sDataModeIdentifier = DATA_MODE;
        return s_sDataModeIdentifier;
    }
}
using namespace fmctrlr;

//------------------------------------------------------------------
FmXFormController::FmXFormController(const Reference< ::com::sun::star::lang::XMultiServiceFactory > & _rxORB,
                                     FmFormView* _pView, Window* _pWindow, const UniString& _sDispatchPrefix)
                  :FmXFormController_BASE1(m_aMutex)
                  ,OPropertySetHelper(FmXFormController_BASE1::rBHelper)
                  ,m_xORB(_rxORB)
                  ,m_aActivateListeners(m_aMutex)
                  ,m_aModifyListeners(m_aMutex)
                  ,m_aErrorListeners(m_aMutex)
                  ,m_aDeleteListeners(m_aMutex)
                  ,m_aRowSetApproveListeners(m_aMutex)
                  ,m_aParameterListeners(m_aMutex)
                  ,m_pView(_pView)
                  ,m_pWindow(_pWindow)
                  ,m_bDBConnection(sal_False)
                  ,m_bCycle(sal_False)
                  ,m_bCanUpdate(sal_False)
                  ,m_bCanInsert(sal_False)
                  ,m_bModified(sal_False)
                  ,m_nLoadEvent( 0 )
                  ,m_bCommitLock(sal_False)
                  ,m_bCurrentRecordModified(sal_False)
                  ,m_bCurrentRecordNew(sal_False)
                  ,m_bLocked(sal_False)
                  ,m_aMode(getDataModeIdentifier())
                  ,m_bFiltering(sal_False)
                  ,m_bAttachEvents(sal_True)
                  ,m_bDetachEvents(sal_True)
                  ,m_nCurrentFilterPosition(0)
                  ,m_sDispatchPrefix(_sDispatchPrefix)
                  ,m_nUpdateDispatcherEvent(0)
                  ,m_nToggleEvent(0)
                  ,m_aParser(_rxORB)
{
    ::comphelper::increment(m_refCount);
    {
        m_xAggregate = Reference< XAggregation > (m_xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.awt.TabController")), UNO_QUERY);
        DBG_ASSERT(m_xAggregate.is(), "FmXFormController::FmXFormController : could not create my aggregate !");
        m_xTabController = Reference< ::com::sun::star::awt::XTabController > (m_xAggregate, UNO_QUERY);
    }
    if ( m_xAggregate.is() )
        m_xAggregate->setDelegator(*this);

    ::comphelper::decrement(m_refCount);

    m_aInsertTimer.SetTimeout(500);
    m_aInsertTimer.SetTimeoutHdl(LINK(this,FmXFormController,OnTimeout));
}

//------------------------------------------------------------------
FmXFormController::~FmXFormController()
{
    if( m_nLoadEvent )
        Application::RemoveUserEvent( m_nLoadEvent );

    if (m_nToggleEvent)
        Application::RemoveUserEvent( m_nToggleEvent );

    if (m_nUpdateDispatcherEvent)
        Application::RemoveUserEvent(m_nUpdateDispatcherEvent);

    if (m_aInsertTimer.IsActive())
        m_aInsertTimer.Stop();

    // Freigeben der Aggregation
    if (m_xAggregate.is())
    {
        Reference< XInterface >  xInt;
        m_xAggregate->setDelegator(xInt);
    }
}
// -----------------------------------------------------------------------------
using namespace ::cppu;
using namespace ::osl;

void SAL_CALL FmXFormController::acquire()
{
    FmXFormController_BASE1::acquire();
}
// -----------------------------------------------------------------------------
void SAL_CALL FmXFormController::release()
{
    FmXFormController_BASE1::release();
}
// -----------------------------------------------------------------------------
// XUnoTunnel
Sequence< sal_Int8 > FmXFormController::getUnoTunnelImplementationId()
{
    static OImplementationId * pId = 0;
    if (! pId)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! pId)
        {
            static OImplementationId aId;
            pId = &aId;
        }
    }
    return pId->getImplementationId();
}
//------------------------------------------------------------------------------
sal_Int64 SAL_CALL FmXFormController::getSomething(Sequence<sal_Int8> const& rId)throw( RuntimeException )
{
    if (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
        return (sal_Int64)this;

    return sal_Int64();
}

//------------------------------------------------------------------------------
Sequence< Type > SAL_CALL FmXFormController::getTypes(  ) throw(RuntimeException)
{
    return comphelper::concatSequences(FmXFormController_BASE1::getTypes(),FmXFormController_BASE2::getTypes(),FmXFormController_BASE3::getTypes());
}
//------------------------------------------------------------------
Any SAL_CALL FmXFormController::queryAggregation( const Type& _rType ) throw(RuntimeException)
{
    Any aRet = FmXFormController_BASE1::queryAggregation(_rType);
    if(!aRet.hasValue())
    {
        aRet = OPropertySetHelper::queryInterface(_rType);
        if(!aRet.hasValue())
        {
            aRet = FmXFormController_BASE2::queryInterface(_rType);
            if(!aRet.hasValue())
                aRet = FmXFormController_BASE3::queryInterface(_rType);
        }
    }
    if(aRet.hasValue())
        return aRet;
    return m_xAggregate->queryAggregation(_rType);
}

// ::com::sun::star::lang::XServiceInfo
//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException )
{
    Sequence< ::rtl::OUString> aSNL(static_getSupportedServiceNames());
    const ::rtl::OUString * pArray = aSNL.getConstArray();
    for( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
        if( pArray[i] == ServiceName )
            return sal_True;
    return sal_False;
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL FmXFormController::getImplementationName() throw( RuntimeException )
{
    return ::rtl::OUString::createFromAscii("com.sun.star.form.FmXFormController");
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString> SAL_CALL FmXFormController::getSupportedServiceNames(void) throw( RuntimeException )
{
    return static_getSupportedServiceNames();
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::approveReset(const ::com::sun::star::lang::EventObject& rEvent) throw( RuntimeException )
{
    return sal_True;
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::resetted(const ::com::sun::star::lang::EventObject& rEvent) throw( RuntimeException )
{
    ::osl::MutexGuard aGuard(m_aMutex);
    if (getCurrentControl().is() &&  (getCurrentControl()->getModel() == rEvent.Source))
        m_bModified = sal_False;
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString> FmXFormController::static_getSupportedServiceNames(void)
{
    static Sequence< ::rtl::OUString> aServices;
    if (!aServices.getLength())
    {
        aServices.realloc(2);
        aServices.getArray()[0] = ::rtl::OUString::createFromAscii("com.sun.star.form.FormController");
        aServices.getArray()[1] = ::rtl::OUString::createFromAscii("com.sun.star.awt.control.TabController");
    }
    return aServices;
}

//------------------------------------------------------------------------------
void FmXFormController::setCurrentFilterPosition(sal_Int32 nPos)
{
    DBG_ASSERT(nPos < (sal_Int32)m_aFilters.size(), "Invalid Position");

    if (nPos != m_nCurrentFilterPosition)
    {
        m_nCurrentFilterPosition = nPos;

        // reset the text for all controls
        for (FmFilterControls::const_iterator iter = m_aFilterControls.begin();
             iter != m_aFilterControls.end(); iter++)
                 (*iter).first->setText(rtl::OUString());

        if (nPos != -1)
        {
            // set the text for all filters
            FmFilterRow& rRow = m_aFilters[m_nCurrentFilterPosition];
            for (FmFilterRow::const_iterator iter2 = rRow.begin();
                 iter2 != rRow.end(); iter2++)
            {
                (*iter2).first->setText((*iter2).second);
            }
        }
    }
}

// OPropertySetHelper
//------------------------------------------------------------------------------
sal_Bool FmXFormController::convertFastPropertyValue( Any & rConvertedValue, Any & rOldValue,
                                            sal_Int32 nHandle, const Any& rValue )
                throw( ::com::sun::star::lang::IllegalArgumentException )
{
    return sal_False;
}

//------------------------------------------------------------------------------
void FmXFormController::setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const Any& rValue )
{
}

//------------------------------------------------------------------------------
void FmXFormController::getFastPropertyValue( Any& rValue, sal_Int32 nHandle ) const
{
    switch (nHandle)
    {
        case FM_ATTR_FILTER_CRITERIA:
        {
            ::rtl::OUString aFilter;
            Reference< ::com::sun::star::sdbc::XConnection> xConnection(::dbtools::getConnection(Reference< ::com::sun::star::sdbc::XRowSet>(m_xModelAsIndex, UNO_QUERY)));
            if (xConnection.is())
            {
                Reference< ::com::sun::star::sdbc::XDatabaseMetaData> xMetaData(xConnection->getMetaData());
                Reference< ::com::sun::star::util::XNumberFormatsSupplier> xFormatSupplier( ::dbtools::getNumberFormats(xConnection, sal_True));
                Reference< ::com::sun::star::util::XNumberFormatter> xFormatter(m_xORB
                                ->createInstance(::rtl::OUString::createFromAscii("com.sun.star.util.NumberFormatter")), UNO_QUERY);
                xFormatter->attachNumberFormatsSupplier(xFormatSupplier);

                Reference< ::com::sun::star::sdbcx::XColumnsSupplier> xSupplyCols(m_xModelAsIndex, UNO_QUERY);
                Reference< ::com::sun::star::container::XNameAccess> xFields(xSupplyCols->getColumns(), UNO_QUERY);

                ::rtl::OUString aQuote( xMetaData->getIdentifierQuoteString() );

                    // now add the filter rows
                for (FmFilterRows::const_iterator i = m_aFilters.begin(); i != m_aFilters.end(); i++)
                {
                    ::rtl::OUString aTest, aErrorMsg, aCriteria;
                    const FmFilterRow& rRow = *i;

                    if (!rRow.empty())
                    {
                        if (aFilter.getLength())
                            aFilter += ::rtl::OUString::createFromAscii(" OR ");

                        aFilter += ::rtl::OUString::createFromAscii("(");
                        for (FmFilterRow::const_iterator j = rRow.begin(); j != rRow.end(); j++)
                        {
                            // get the field of the controls map
                            Reference< ::com::sun::star::awt::XTextComponent > xText = (*j).first;
                            Reference< XPropertySet> xField = (*m_aFilterControls.find(xText)).second;
                            DBG_ASSERT(xField.is(), "NoField found");
                            if (j != rRow.begin())
                                aFilter += ::rtl::OUString::createFromAscii(" AND ");

                            aTest = (const sal_Unicode*)(*j).second;
                            aErrorMsg = ::rtl::OUString();
                            OSQLParseNode* pParseNode = const_cast<OSQLParser*>(&m_aParser)->predicateTree(aErrorMsg, aTest, xFormatter, xField);

                            if (pParseNode)
                            {
                                aCriteria = ::rtl::OUString();
                                pParseNode->parseNodeToStr(aCriteria,xMetaData);
                                aFilter += aCriteria;
                            }
                        }
                        aFilter += ::rtl::OUString::createFromAscii(")");
                    }
                }
            }
            rValue <<= ::rtl::OUString(aFilter);
        }   break;
    }
}

//------------------------------------------------------------------------------
Reference< XPropertySetInfo >  FmXFormController::getPropertySetInfo()
{
    static Reference< XPropertySetInfo >  xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

//------------------------------------------------------------------------------
#define DECL_PROP_CORE(varname, type) \
pDesc[nPos++] = Property(FM_PROP_##varname, FM_ATTR_##varname, ::getCppuType((const type*)0),


#define DECL_PROP1(varname, type, attrib1)  \
    DECL_PROP_CORE(varname, type) PropertyAttribute::##attrib1)

//------------------------------------------------------------------------------
void FmXFormController::fillProperties(
        Sequence< Property >& /* [out] */ _rProps,
        Sequence< Property >& /* [out] */ _rAggregateProps
        ) const
{
    _rProps.realloc(1);
    sal_Int32 nPos = 0;
    Property* pDesc = _rProps.getArray();
    DECL_PROP1(FILTER_CRITERIA, rtl::OUString, READONLY);
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& FmXFormController::getInfoHelper()
{
    return *getArrayHelper();
}

// ::com::sun::star::container::XElementAccess
//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::hasElements(void) throw( RuntimeException )
{
    return m_aChilds.size() != 0;
}

//------------------------------------------------------------------------------
Type SAL_CALL  FmXFormController::getElementType(void) throw( RuntimeException )
{
    return ::getCppuType((const Reference< ::com::sun::star::form::XFormController>*)0);

}

// ::com::sun::star::container::XEnumerationAccess
//------------------------------------------------------------------------------
Reference< ::com::sun::star::container::XEnumeration > SAL_CALL  FmXFormController::createEnumeration(void) throw( RuntimeException )
{
    ::osl::MutexGuard aGuard( m_aMutex );
    return new ::comphelper::OEnumerationByIndex(this);
}

// ::com::sun::star::container::XIndexAccess
//------------------------------------------------------------------------------
sal_Int32 SAL_CALL FmXFormController::getCount(void) throw( RuntimeException )
{
    return m_aChilds.size();
}

//------------------------------------------------------------------------------
Any SAL_CALL FmXFormController::getByIndex(sal_Int32 Index) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, RuntimeException )
{
    if (Index < 0 ||
        Index >= (sal_Int32)m_aChilds.size())
        throw ::com::sun::star::lang::IndexOutOfBoundsException();

    return makeAny(m_aChilds[Index]);
    //  , ::getCppuType((const ::com::sun::star::form::XFormController*)0));
}

//-----------------------------------------------------------------------------
void FmXFormController::addChild(FmXFormController* pChild)
{
    Reference< ::com::sun::star::form::XFormController >  xController(pChild);
    m_aChilds.push_back(xController);
    pChild->setParent(static_cast< ::com::sun::star::form::XFormController* >(this));

    Reference< ::com::sun::star::form::XFormComponent >  xForm(pChild->getModel(), UNO_QUERY);

    // search the position of the model within the form
    sal_uInt32 nPos = m_xModelAsIndex->getCount();
    Reference< ::com::sun::star::form::XFormComponent > xTemp;
    for( ; nPos; )
    {
        m_xModelAsIndex->getByIndex(--nPos) >>= xTemp;
        if ((::com::sun::star::form::XFormComponent*)xForm.get() == (::com::sun::star::form::XFormComponent*)xTemp.get())
        {
            Reference< XInterface >  xIfc(xController, UNO_QUERY);
            m_xModelAsManager->attach( nPos, xIfc, makeAny( xController) );
            break;
        }
    }
}

//  EventListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::disposing(const ::com::sun::star::lang::EventObject& e) throw( RuntimeException )
{
    // Ist der Container disposed worden
    Reference< ::com::sun::star::awt::XControlContainer >  xContainer(e.Source, UNO_QUERY);
    if (xContainer.is())
    {
        setContainer(Reference< ::com::sun::star::awt::XControlContainer > ());
    }
    else
    {
        // ist ein Control disposed worden
        Reference< ::com::sun::star::awt::XControl >  xControl(e.Source, UNO_QUERY);
        if (xControl.is())
        {
            if (getContainer().is())
                removeControl(xControl);
        }
    }
}

// OComponentHelper
//-----------------------------------------------------------------------------
void FmXFormController::disposing(void)
{
    ::com::sun::star::lang::EventObject aEvt(static_cast< ::com::sun::star::form::XFormController* >(this));
    // noch aktiv ?
    if (m_xActiveControl.is())
    {
        NOTIFY_LISTENERS(m_aActivateListeners, ::com::sun::star::form::XFormControllerListener, formDeactivated, aEvt);
    }

    m_aActivateListeners.disposeAndClear(aEvt);
    m_aModifyListeners.disposeAndClear(aEvt);
    m_aErrorListeners.disposeAndClear(aEvt);
    m_aDeleteListeners.disposeAndClear(aEvt);
    m_aRowSetApproveListeners.disposeAndClear(aEvt);
    m_aParameterListeners.disposeAndClear(aEvt);

    m_aFilterControls.clear();
    m_aFilters.clear();

    ::osl::MutexGuard aGuard( m_aMutex );
    m_xActiveControl  = m_xCurrentControl = NULL;

    for (FmFormControllers::const_iterator i = m_aChilds.begin();
        i != m_aChilds.end(); i++)
    {
        // search the position of the model within the form
        Reference< ::com::sun::star::form::XFormComponent >  xForm((*i)->getModel(), UNO_QUERY);
        sal_uInt32 nPos = m_xModelAsIndex->getCount();
        Reference< ::com::sun::star::form::XFormComponent > xTemp;
        for( ; nPos; )
        {

            m_xModelAsIndex->getByIndex(--nPos) >>= xTemp;
            if ((::com::sun::star::form::XFormComponent*)xForm.get() == (::com::sun::star::form::XFormComponent*)xTemp.get())
            {
                Reference< XInterface >  xIfc(*i, UNO_QUERY);
                m_xModelAsManager->detach( nPos, xIfc);
                break;
            }
        }

        Reference< ::com::sun::star::lang::XComponent > (*i, UNO_QUERY)->dispose();
    }
    m_aChilds.clear();

    if (m_bDBConnection)
        unload();

    setContainer(Reference< ::com::sun::star::awt::XControlContainer > ());
    setModel(Reference< ::com::sun::star::awt::XTabControllerModel > ());
    setParent(Reference< XInterface > ());

    ::comphelper::disposeComponent(m_xComposer);

    // the following members shouldn't be set to NULL
    // Freigeben der Aggregation
    if (m_xAggregate.is())
    {
        Reference< XInterface >  xInt;
        m_xAggregate->setDelegator(xInt);
    }
    m_xAggregate        = NULL;
    m_xTabController    = NULL;
    m_xORB              = NULL;

    m_bDBConnection = sal_False;
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::propertyChange(const PropertyChangeEvent& evt)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    sal_Bool bModifiedChanged = (evt.PropertyName == FM_PROP_ISMODIFIED);
    sal_Bool bNewChanged = (evt.PropertyName == FM_PROP_ISNEW);
    if (bModifiedChanged || bNewChanged)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if (bModifiedChanged)
            m_bCurrentRecordModified = ::comphelper::getBOOL(evt.NewValue);
        else
            m_bCurrentRecordNew = ::comphelper::getBOOL(evt.NewValue);

        // toggle the locking
        if (m_bLocked != determineLockState())
        {
            m_bLocked = !m_bLocked;
            setLocks();
            if (isListeningForChanges())
                startListening();
            else
                stopListening();
        }

        if (bNewChanged && m_pView)
        {
            if (m_nToggleEvent)
                Application::RemoveUserEvent( m_nToggleEvent );
            m_nToggleEvent = Application::PostUserEvent( LINK(this, FmXFormController,
                                                    OnToggleAutoFields) );
        }

        if (!m_bCurrentRecordModified)
            m_bModified = sal_False;
    }
}

//------------------------------------------------------------------------------
void FmXFormController::toggleAutoFields(sal_Bool bAutoFields)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!m_pView)
    {
        DBG_ERROR("FmXFormController::toggleAutoFields : you can't call toggleAutoFields when no view exists !");
        return;
    }


    // Austauschen der Kontrols fuer das aktuelle Formular
    Sequence < Reference< ::com::sun::star::awt::XControl >  > aControls(m_aControls);
    const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();
    SdrPageView* pCurPageView = m_pView->GetPageViewPvNum(0);

    sal_uInt16 nPos = pCurPageView ? pCurPageView->GetWinList().Find((OutputDevice*)m_pView->GetActualOutDev()) : SDRPAGEVIEWWIN_NOTFOUND;
    if (nPos == SDRPAGEVIEWWIN_NOTFOUND)
        return;

    // the control we have to activate after replacement
    Reference< ::com::sun::star::awt::XControl >  xNewActiveControl;
    if (bAutoFields)
    {
        // as we don't want new controls to be attached to the scripting environment
        // we change attach flags
        m_bAttachEvents = sal_False;
        for (sal_Int32 i = aControls.getLength(); i > 0;)
        {
            const Reference< ::com::sun::star::awt::XControl > & xControl = pControls[--i];
            if (xControl.is())
            {
                Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
                if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
                {
                    // does the model use a bound field ?
                    Reference< XPropertySet >  xField;
                    xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;

                    // is it a autofield?
                    if (xField.is() && ::comphelper::hasProperty(FM_PROP_AUTOINCREMENT, xField) &&
                        ::comphelper::getBOOL(xField->getPropertyValue(FM_PROP_AUTOINCREMENT)))
                    {
                        const SdrPageViewWinRec& rWR = pCurPageView->GetWinList()[nPos];
                        const SdrUnoControlList& rControlList = rWR.GetControlList();
                        sal_uInt16 nCtrlNum = rControlList.Find(xControl);
                        if (nCtrlNum != SDRUNOCONTROL_NOTFOUND)
                        {
                            // ok create an autocontrol
                            SdrUnoControlRec& rControlRec = (SdrUnoControlRec&)rControlList[nCtrlNum];
                            FmXAutoControl* pAutoControl = new FmXAutoControl();
                            Reference< ::com::sun::star::awt::XControl >  xNewControl(pAutoControl);

                            // setting the focus if the current control
                            // is the active one
                            if ((::com::sun::star::awt::XControl*)m_xActiveControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                            {
                                xNewActiveControl = xNewControl;
                                m_xActiveControl = m_xCurrentControl = NULL;
                            }
                            else if ((::com::sun::star::awt::XControl*)m_xCurrentControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                                m_xCurrentControl = xNewControl;

                            rControlRec.ReplaceControl(xNewControl);
                        }
                    }
                }
            }
        }
        m_bAttachEvents = sal_True;
    }
    else
    {
        m_bDetachEvents = sal_False;
        for (sal_Int32 i = aControls.getLength(); i > 0;)
        {
            const Reference< ::com::sun::star::awt::XControl > & xControl = pControls[--i];
            if (xControl.is())
            {
                Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
                if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
                {
                    // does the model use a bound field ?
                    Reference< XPropertySet >  xField;
                    xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;

                    // is it a autofield?
                    if (xField.is() && ::comphelper::hasProperty(FM_PROP_AUTOINCREMENT, xField) &&
                        ::comphelper::getBOOL(xField->getPropertyValue(FM_PROP_AUTOINCREMENT)))
                    {
                        const SdrPageViewWinRec& rWR = pCurPageView->GetWinList()[nPos];
                        const SdrUnoControlList& rControlList = rWR.GetControlList();
                        sal_uInt16 nCtrlNum = rControlList.Find(xControl);
                        if (nCtrlNum != SDRUNOCONTROL_NOTFOUND)
                        {
                            // Zuruecksetzen des Controls auf das Defaultcontrol
                            SdrUnoControlRec& rControlRec = (SdrUnoControlRec&)rControlList[nCtrlNum];
                            ::rtl::OUString aServiceName = ::comphelper::getString(xSet->getPropertyValue(FM_PROP_DEFAULTCONTROL));
                            Reference< ::com::sun::star::awt::XControl >  xNewControl(m_xORB->createInstance(aServiceName), UNO_QUERY);

                            // setting the focus if the current control
                            // is the active one
                            if ((::com::sun::star::awt::XControl*)m_xActiveControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                            {
                                xNewActiveControl = xNewControl;
                                m_xActiveControl = m_xCurrentControl = NULL;
                            }
                            else if ((::com::sun::star::awt::XControl*)m_xCurrentControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                                m_xCurrentControl = xNewControl;

                            rControlRec.ReplaceControl(xNewControl);
                        }
                    }
                }
            }
        }
        m_bDetachEvents = sal_True;
    }

    // set the focus async if possible
    // setting the focus to the replacing control
    Reference< ::com::sun::star::awt::XWindow >  xWindow(xNewActiveControl, UNO_QUERY);
    if (xWindow.is())
        xWindow->setFocus();
}

//------------------------------------------------------------------------------
IMPL_LINK(FmXFormController, OnToggleAutoFields, void*, EMPTYARG)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_nToggleEvent = 0;
    toggleAutoFields(m_bCurrentRecordNew);
    return 1L;
}

// ::com::sun::star::awt::XTextListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::textChanged(const ::com::sun::star::awt::TextEvent& e)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (m_bFiltering)
    {
        Reference< ::com::sun::star::awt::XTextComponent >  xText(e.Source,UNO_QUERY);
        ::rtl::OUString aText = xText->getText();

        // Suchen der aktuellen Row
        FmFilterRow& rRow = m_aFilters[m_nCurrentFilterPosition];

        // do we have a new filter
        if (aText.getLength())
            rRow[xText] = aText;
        else
        {
            // do we have the control in the row
            FmFilterRow::iterator iter = rRow.find(xText);
            // erase the entry out of the row
            if (iter != rRow.end())
                rRow.erase(iter);
        }
    }
    else if (!m_bModified)
        onModify( e.Source );
}

// ::com::sun::star::awt::XItemListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::itemStateChanged(const ::com::sun::star::awt::ItemEvent& rEvent)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!m_bModified)
        onModify( rEvent.Source );
}

// XModificationBroadcaster
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::addModifyListener(const Reference< ::com::sun::star::util::XModifyListener > & l)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aModifyListeners.addInterface( l );
}

//------------------------------------------------------------------------------
void FmXFormController::removeModifyListener(const Reference< ::com::sun::star::util::XModifyListener > & l)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aModifyListeners.removeInterface( l );
}

// XModificationListener
//------------------------------------------------------------------------------
void FmXFormController::modified(const EventObject& rEvent)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!m_bModified)
        onModify( rEvent.Source );
}

//------------------------------------------------------------------------------
void FmXFormController::onModify( const Reference< XInterface >& _rxControl )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!m_bModified)
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        m_bModified = sal_True;

    }

    Reference< XControl > xSourceControl(_rxControl, UNO_QUERY);
    if  (xSourceControl.get() != m_xCurrentControl.get())
    {   // let this control grab the focus
        // (this case may happen if somebody moves the scroll wheel of the mouse over a control
        // which does not have the focus)
        // 85511 - 29.05.2001 - frank.schoenheit@germany.sun.com
        Reference< XWindow > xControlWindow(_rxControl, UNO_QUERY);
        if (xControlWindow.is())
            xControlWindow->setFocus();
    }

    EventObject aEvt(static_cast<cppu::OWeakObject*>(this));
    NOTIFY_LISTENERS(m_aModifyListeners, XModifyListener, modified, aEvt);
}

//------------------------------------------------------------------------------
sal_Bool FmXFormController::determineLockState() const
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // a.) in filter mode we are always locked
    // b.) if we have no valid model or our model (a result set) is not alive -> we're locked
    // c.) if we are inserting everything is OK and we are not locked
    // d.) if are not updatable or on invalid position
    Reference< ::com::sun::star::sdbc::XResultSet >  xResultSet(m_xModelAsIndex, UNO_QUERY);
    if (m_bFiltering || !xResultSet.is() || !isRowSetAlive(xResultSet))
        return sal_True;
    else
        return (m_bCanInsert && m_bCurrentRecordNew) ? sal_False
        :  xResultSet->isBeforeFirst() || xResultSet->isAfterLast() || xResultSet->rowDeleted() || !m_bCanUpdate;
}

//  FocusListener
//------------------------------------------------------------------------------
void FmXFormController::focusGained(const ::com::sun::star::awt::FocusEvent& e)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    Reference< ::com::sun::star::awt::XControl >  xControl(e.Source, UNO_QUERY);

    if (m_bDBConnection)
    {
        // do we need to keep the locking of the commit
        // we hold the lock as long as the control differs from the current
        // otherwhise we disabled the lock
        m_bCommitLock = m_bCommitLock && (::com::sun::star::awt::XControl*)xControl.get() != (::com::sun::star::awt::XControl*)m_xCurrentControl.get();
        if (m_bCommitLock)
            return;

        // when do we have to commit a value to form or a filter
        // a.) if the current value is modified
        // b.) there must be a current control
        // c.) and it must be different from the new focus owning control or
        // d.) the focus is moving around (so we have only one control)

        if ((m_bModified || m_bFiltering) &&
            m_xCurrentControl.is() &&
            ((xControl.get() != m_xCurrentControl.get()) ||
             ((e.FocusFlags & ::com::sun::star::awt::FocusChangeReason::AROUND) && (m_bCycle || m_bFiltering)))
            )
        {
            // check the old control if the content is ok
#if DEBUG || DBG_UTIL
            Reference< ::com::sun::star::form::XBoundControl >  xLockingTest(m_xCurrentControl, UNO_QUERY);
            sal_Bool bControlIsLocked = xLockingTest.is() && xLockingTest->getLock();
            DBG_ASSERT(!bControlIsLocked, "FmXFormController::focusLost : I'm modified and the current control is locked ? How this ?");
            // normalerweise sollte ein gelocktes Control nicht modified sein, also muss wohl mein bModified aus einem anderen Kontext
            // gesetzt worden sein, was ich nicht verstehen wuerde ...
#endif
            DBG_ASSERT(m_xCurrentControl.is(), "kein CurrentControl gesetzt");
            // zunaechst das Control fragen ob es das IFace unterstuetzt
            Reference< ::com::sun::star::form::XBoundComponent >  xBound(m_xCurrentControl, UNO_QUERY);
            if (!xBound.is() && m_xCurrentControl.is())
                xBound  = Reference< ::com::sun::star::form::XBoundComponent > (m_xCurrentControl->getModel(), UNO_QUERY);

            // lock if we lose the focus during commit
            m_bCommitLock = sal_True;

            // Commit nicht erfolgreich, Focus zuruecksetzen
            if (xBound.is() && !xBound->commit())
            {
                // the commit failed and we don't commit again until the current control
                // which couldn't be commit gains the focus again
                Reference< ::com::sun::star::awt::XWindow >  xWindow(m_xCurrentControl, UNO_QUERY);
                if (xWindow.is())
                    xWindow->setFocus();
                return;
            }
            else
            {
                m_bModified = sal_False;
                m_bCommitLock = sal_False;
            }
        }

        if (!m_bFiltering && m_bCycle && (e.FocusFlags & ::com::sun::star::awt::FocusChangeReason::AROUND) && m_xCurrentControl.is())
        {
            if (e.FocusFlags & ::com::sun::star::awt::FocusChangeReason::FORWARD)
                FmXFormShell::CanMoveRight((::com::sun::star::form::XFormController*)this) && FmXFormShell::MoveRight((::com::sun::star::form::XFormController*)this);
            else // backward
                FmXFormShell::CanMoveLeft((::com::sun::star::form::XFormController*)this) && FmXFormShell::MoveLeft((::com::sun::star::form::XFormController*)this);
        }
    }

    // Immer noch ein und dasselbe Control
    if  (   (m_xActiveControl.get() == xControl.get())
        &&  (xControl.get() == m_xCurrentControl.get())
        )
    {
        DBG_ASSERT(m_xCurrentControl.is(), "Kein CurrentControl selektiert");
        return;
    }

    sal_Bool bActivated = !m_xActiveControl.is() && xControl.is();

    m_xActiveControl  = xControl;
    m_xCurrentControl = xControl;

    DBG_ASSERT(m_xCurrentControl.is(), "Kein CurrentControl selektiert");

    if (bActivated)
        onActivate();

    if (m_bDBConnection && !m_bFiltering && m_pView)
        m_pView->GetFormShell()->GetViewShell()->GetViewFrame()->GetBindings().Invalidate(AutoSlotMap);

    if (m_xCurrentControl.is())
    {
        // Control erhaelt Focus, dann eventuell in den sichtbaren Bereich
        Reference< ::com::sun::star::awt::XWindow >  xWindow(xControl, UNO_QUERY);
        if (xWindow.is() && m_pView && m_pWindow)
        {
            ::com::sun::star::awt::Rectangle aRect = xWindow->getPosSize();
            ::Rectangle aNewRect(aRect.X,aRect.Y,aRect.X+aRect.Width,aRect.Y+aRect.Height);
            aNewRect = m_pWindow->PixelToLogic(aNewRect);
            m_pView->MakeVisible(aNewRect, *m_pWindow);
        }
    }
}

//------------------------------------------------------------------------------
void FmXFormController::onActivate()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // benachrichtigen, dass form activiert
    ::com::sun::star::lang::EventObject aEvt;
    aEvt.Source = *this;
    NOTIFY_LISTENERS(m_aActivateListeners, ::com::sun::star::form::XFormControllerListener, formActivated, aEvt);

    // verschicken ob modified
    if (m_bModified)
    {
        NOTIFY_LISTENERS(m_aModifyListeners, ::com::sun::star::util::XModifyListener, modified, aEvt);
    }
}

//------------------------------------------------------------------------------
void FmXFormController::focusLost(const ::com::sun::star::awt::FocusEvent& e)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    Reference< ::com::sun::star::awt::XControl >  xControl(e.Source, UNO_QUERY);
    Reference< ::com::sun::star::awt::XWindowPeer >  xNext(e.NextFocus, UNO_QUERY);
    Reference< ::com::sun::star::awt::XControl >  xNextControl = isInList(xNext);
    if (!xNextControl.is())
    {
        m_xActiveControl = NULL;
        ::com::sun::star::lang::EventObject aEvt;
        aEvt.Source = *this;
        NOTIFY_LISTENERS(m_aActivateListeners, ::com::sun::star::form::XFormControllerListener, formDeactivated, aEvt);
    }
}

//------------------------------------------------------------------------------
void FmXFormController::setModel(const Reference< ::com::sun::star::awt::XTabControllerModel > & Model)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::setModel : invalid aggregate !");

    // disconnect from the old model
    if (m_xModelAsIndex.is())
    {
        if (m_bDBConnection)
        {
            // we are currently working on the model
            ::com::sun::star::lang::EventObject aEvt(m_xModelAsIndex);
            unloaded(aEvt);
        }

        Reference< ::com::sun::star::form::XLoadable >  xForm(m_xModelAsIndex, UNO_QUERY);
        if (xForm.is())
            xForm->removeLoadListener(this);

        Reference< ::com::sun::star::sdb::XSQLErrorBroadcaster >  xBroadcaster(m_xModelAsIndex, UNO_QUERY);
        if (xBroadcaster.is())
            xBroadcaster->removeSQLErrorListener(this);

        Reference< ::com::sun::star::form::XDatabaseParameterBroadcaster >  xParamBroadcaster(m_xModelAsIndex, UNO_QUERY);
        if (xParamBroadcaster.is())
            xParamBroadcaster->removeParameterListener(this);
    }

    // set the new model wait for the load event
    if (m_xTabController.is())
        m_xTabController->setModel(Model);
    m_xModelAsIndex = Reference< ::com::sun::star::container::XIndexAccess > (Model, UNO_QUERY);
    m_xModelAsManager = Reference< ::com::sun::star::script::XEventAttacherManager > (Model, UNO_QUERY);

    // only if both ifaces exit, the controller will work successful
    if (!m_xModelAsIndex.is() || !m_xModelAsManager.is())
    {
        m_xModelAsManager = NULL;
        m_xModelAsIndex = NULL;
    }

    if (m_xModelAsIndex.is())
    {
        // adding load and ui interaction listeners
        Reference< ::com::sun::star::form::XLoadable >  xForm(Model, UNO_QUERY);
        if (xForm.is())
            xForm->addLoadListener(this);

        Reference< ::com::sun::star::sdb::XSQLErrorBroadcaster >  xBroadcaster(Model, UNO_QUERY);
        if (xBroadcaster.is())
            xBroadcaster->addSQLErrorListener(this);

        Reference< ::com::sun::star::form::XDatabaseParameterBroadcaster >  xParamBroadcaster(Model, UNO_QUERY);
        if (xParamBroadcaster.is())
            xParamBroadcaster->addParameterListener(this);

        // well, is the database already loaded?
        // then we have to simulate a load event
        Reference< ::com::sun::star::form::XLoadable >  xCursor(m_xModelAsIndex, UNO_QUERY);
        if (xCursor.is() && xCursor->isLoaded())
        {
            ::com::sun::star::lang::EventObject aEvt(xCursor);
            loaded(aEvt);
        }
    }
}

//------------------------------------------------------------------------------
Reference< ::com::sun::star::awt::XTabControllerModel >  FmXFormController::getModel()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::getModel : invalid aggregate !");
    if (!m_xTabController.is())
        return Reference< ::com::sun::star::awt::XTabControllerModel > ();
    return m_xTabController->getModel();
}

//------------------------------------------------------------------------------
void FmXFormController::addToEventAttacher(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // anmelden beim Eventattacher
    Reference< ::com::sun::star::form::XFormComponent >  xComp(xControl->getModel(), UNO_QUERY);
    if (xComp.is() && m_xModelAsIndex.is())
    {
        // Und die Position des ControlModel darin suchen
        sal_uInt32 nPos = m_xModelAsIndex->getCount();
        Reference< ::com::sun::star::form::XFormComponent > xTemp;
        for( ; nPos; )
        {
            m_xModelAsIndex->getByIndex(--nPos) >>= xTemp;
            if ((::com::sun::star::form::XFormComponent*)xComp.get() == (::com::sun::star::form::XFormComponent*)xTemp.get())
            {
                Reference< XInterface >  xIfc(xControl, UNO_QUERY);
                m_xModelAsManager->attach( nPos, xIfc, makeAny(xControl) );
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
void FmXFormController::removeFromEventAttacher(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // abmelden beim Eventattacher
    Reference< ::com::sun::star::form::XFormComponent >  xComp(xControl->getModel(), UNO_QUERY);
    if (xComp.is() && xComp->getParent().is() && m_xModelAsIndex.is())
    {
        // Und die Position des ControlModel darin suchen
        sal_uInt32 nPos = m_xModelAsIndex->getCount();
        Reference< ::com::sun::star::form::XFormComponent > xTemp;
        for( ; nPos; )
        {
            m_xModelAsIndex->getByIndex(--nPos) >>= xTemp;
            if ((::com::sun::star::form::XFormComponent*)xComp.get() == (::com::sun::star::form::XFormComponent*)xTemp.get())
            {
                Reference< XInterface >  xIfc(xControl, UNO_QUERY);
                m_xModelAsManager->detach( nPos, xIfc );
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
void FmXFormController::setContainer(const Reference< ::com::sun::star::awt::XControlContainer > & xContainer)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    Reference< ::com::sun::star::awt::XTabControllerModel >  xTabModel(getModel());
    DBG_ASSERT(xTabModel.is() || !xContainer.is(), "No Model defined");
        // if we have a new container we need a model
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::setContainer : invalid aggregate !");

    ::osl::MutexGuard aGuard( m_aMutex );
    Reference< ::com::sun::star::container::XContainer >  xCurrentContainer;
    if (m_xTabController.is())
        xCurrentContainer = Reference< ::com::sun::star::container::XContainer > (m_xTabController->getContainer(), UNO_QUERY);
    if (xCurrentContainer.is())
    {
        xCurrentContainer->removeContainerListener(this);

        if (m_aInsertTimer.IsActive())
            m_aInsertTimer.Stop();

        // bei den Controls abmelden
        sal_Int32 nCount = m_aControls.getLength();
        const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();

        // clear the filter map
        for (FmFilterControls::const_iterator iter = m_aFilterControls.begin();
             iter != m_aFilterControls.end(); iter++)
            (*iter).first->removeTextListener(this);

        m_aFilterControls.clear();

        // einsammeln der Controls
        for (sal_Int32 i = 0; i < nCount; ++i, ++pControls)
        {
            // bei jedem Control als FocusListener anmelden
            Reference< ::com::sun::star::awt::XWindow >  xWindow(*pControls, UNO_QUERY);
            if (xWindow.is())
            {
                xWindow->removeFocusListener(this);

                // abmelden beim Eventattacher
                removeFromEventAttacher(*pControls);
            }

            Reference< ::com::sun::star::frame::XDispatchProviderInterception >  xInterception(*pControls, UNO_QUERY);
            if (xInterception.is())
                deleteInterceptor(xInterception);

            if (pControls->is())
            {
                Reference< ::com::sun::star::form::XReset >  xControlReset((*pControls)->getModel(), UNO_QUERY);
                if (xControlReset.is())
                    xControlReset->removeResetListener((::com::sun::star::form::XResetListener*)this);
            }
        }

        // Datenbank spezifische Dinge vornehmen
        if (m_bDBConnection && isListeningForChanges())
            stopListening();

        m_aControls = Sequence< Reference< ::com::sun::star::awt::XControl > >();
    }

    if (m_xTabController.is())
        m_xTabController->setContainer(xContainer);

    // Welche Controls gehoeren zum Container ?
    if (xContainer.is() && xTabModel.is())
    {
        Sequence< Reference< ::com::sun::star::awt::XControlModel > > aModels = xTabModel->getControlModels();
        const Reference< ::com::sun::star::awt::XControlModel > * pModels = aModels.getConstArray();
        Sequence< Reference< ::com::sun::star::awt::XControl > > xCtrls = xContainer->getControls();

        sal_Int32 nCount = aModels.getLength();
        m_aControls = Sequence< Reference< ::com::sun::star::awt::XControl > >( nCount );
        Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getArray();

        // einsammeln der Controls
        sal_Int32 i, j;
        for (i = 0, j = 0; i < nCount; i++ )
        {
            Reference< ::com::sun::star::awt::XControlModel >  xCtrlModel = pModels[i];
            // Zum Model passendes Control suchen
            Reference< ::com::sun::star::awt::XControl >  xCtrl = findControl( xCtrls, xCtrlModel );
            if (xCtrl.is())
            {
                pControls[j++] = xCtrl;

                // bei jedem Control als FocusListener anmelden
                Reference< ::com::sun::star::awt::XWindow >  xWindow(xCtrl, UNO_QUERY);
                if (xWindow.is())
                {
                    xWindow->addFocusListener(this);
                    // anmelden beim Eventattacher
                    addToEventAttacher(xCtrl);
                }

                // add a dispatch interceptor to the control (if supported)
                Reference< ::com::sun::star::frame::XDispatchProviderInterception >  xInterception(xCtrl, UNO_QUERY);
                if (xInterception.is())
                    createInterceptor(xInterception);

                // we want to know about the reset of the the model of our controls
                // (for correctly resetting m_bModified)
                Reference< ::com::sun::star::form::XReset >  xControlReset(xCtrl->getModel(), UNO_QUERY);
                if (xControlReset.is())
                    xControlReset->addResetListener((::com::sun::star::form::XResetListener*)this);
            }
        }

        // nicht fuer jedes Model ein Control
        if (j != i)
            m_aControls.realloc(j);

        // am Container horchen
        Reference< ::com::sun::star::container::XContainer >  xNewContainer(xContainer, UNO_QUERY);
        if (xNewContainer.is())
            xNewContainer->addContainerListener(this);

        // Datenbank spezifische Dinge vornehmen
        if (m_bDBConnection)
        {
            m_bLocked = determineLockState();
            setLocks();
            if (!isLocked())
                startListening();
        }
    }
    // befinden sich die Controls in der richtigen Reihenfolge
    m_bControlsSorted = sal_True;
}

//------------------------------------------------------------------------------
Reference< ::com::sun::star::awt::XControlContainer >  FmXFormController::getContainer()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::getContainer : invalid aggregate !");
    if (!m_xTabController.is())
        return Reference< ::com::sun::star::awt::XControlContainer > ();
    return m_xTabController->getContainer();
}

//------------------------------------------------------------------------------
Sequence< Reference< ::com::sun::star::awt::XControl > > FmXFormController::getControls(void)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    if (!m_bControlsSorted)
    {
        Reference< ::com::sun::star::awt::XTabControllerModel >  xModel = getModel();
        if (!xModel.is())
            return m_aControls;

        Sequence< Reference< ::com::sun::star::awt::XControlModel > > aControlModels = xModel->getControlModels();
        const Reference< ::com::sun::star::awt::XControlModel > * pModels = aControlModels.getConstArray();
        sal_Int32 nModels = aControlModels.getLength();

        Sequence< Reference< ::com::sun::star::awt::XControl > > aNewControls(nModels);

        Reference< ::com::sun::star::awt::XControl > * pControls = aNewControls.getArray();
        Reference< ::com::sun::star::awt::XControlModel >  xCtrlModel;
        Reference< ::com::sun::star::awt::XControl >  xCtrl;

        // Umsortieren der Controls entsprechend der TabReihenfolge
        for (sal_Int32 i = 0, j = 0; i < nModels; i++)
        {
            xCtrlModel = pModels[i];

            // Zum Model passendes Control suchen
            xCtrl = findControl(m_aControls, xCtrlModel);
            if (xCtrl.is())
                pControls[j++] = xCtrl;
        }
        m_aControls = aNewControls;
        m_bControlsSorted = sal_True;
    }
    return m_aControls;
}

//------------------------------------------------------------------------------
void FmXFormController::autoTabOrder()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::autoTabOrder : invalid aggregate !");
    if (m_xTabController.is())
        m_xTabController->autoTabOrder();
}

//------------------------------------------------------------------------------
void FmXFormController::activateTabOrder()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::activateTabOrder : invalid aggregate !");
    if (m_xTabController.is())
        m_xTabController->activateTabOrder();
}

//------------------------------------------------------------------------------
void FmXFormController::setControlLock(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    sal_Bool bLocked = isLocked();

    // es wird gelockt
    // a.) wenn der ganze Datensatz gesperrt ist
    // b.) wenn das zugehoerige Feld gespeert ist
    Reference< ::com::sun::star::form::XBoundControl >  xBound(xControl, UNO_QUERY);
    if (xBound.is() && ((bLocked && bLocked != xBound->getLock() ||
                         !bLocked)))    // beim entlocken immer einzelne Felder ueberpr�fen
    {
        // gibt es eine Datenquelle
        Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
        if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
        {
            // wie sieht mit den Properties ReadOnly und Enable aus
            sal_Bool bTouch = sal_True;
            if (::comphelper::hasProperty(FM_PROP_ENABLED, xSet))
                bTouch = ::comphelper::getBOOL(xSet->getPropertyValue(FM_PROP_ENABLED));
            if (::comphelper::hasProperty(FM_PROP_READONLY, xSet))
                bTouch = !::comphelper::getBOOL(xSet->getPropertyValue(FM_PROP_READONLY));

            if (bTouch)
            {
                Reference< XPropertySet >  xField;
                xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;
                if (xField.is())
                {
                    if (bLocked)
                        xBound->setLock(bLocked);
                    else
                    {
                        try
                        {
                            Any aVal = xField->getPropertyValue(FM_PROP_ISREADONLY);
                            if (aVal.hasValue() && ::comphelper::getBOOL(aVal))
                                xBound->setLock(sal_True);
                            else
                                xBound->setLock(bLocked);
                        }
                        catch(...)
                        {
                        }

                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void FmXFormController::setLocks()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // alle Controls, die mit einer Datenquelle verbunden sind locken/unlocken
    sal_Int32 nLength = m_aControls.getLength();
    const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();
    for (sal_Int32 i = 0; i < nLength; i++)
        setControlLock(pControls[i]);
}

//------------------------------------------------------------------------------
void FmXFormController::startControlListening(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // jetzt anmelden bei gebundenen feldern
    sal_Bool bModifyListening = sal_False;
    Reference< ::com::sun::star::form::XBoundComponent >  xBound(xControl, UNO_QUERY);
    if (xBound.is()) // gebundene Controls,
        bModifyListening = sal_True;
    else
    {
        xBound = Reference< ::com::sun::star::form::XBoundComponent > (xControl->getModel(), UNO_QUERY);

        // gibt es eine Datenquelle
        Reference< XPropertySet >  xSet(xBound, UNO_QUERY);
        if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
        {
            Reference< XPropertySet >  xField;
            xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;
            bModifyListening = (xField.is());
        }
    }

    // kuenstliches while
    while (bModifyListening)
    {
        Reference< ::com::sun::star::util::XModifyBroadcaster >  xMod(xControl, UNO_QUERY);
        if (xMod.is())
        {
            xMod->addModifyListener(this);
            break;
        }

        // alle die Text um vorzeitig ein modified zu erkennen
        Reference< ::com::sun::star::awt::XTextComponent >  xText(xControl, UNO_QUERY);
        if (xText.is())
        {
            xText->addTextListener(this);
            break;
        }

        Reference< ::com::sun::star::awt::XCheckBox >  xBox(xControl, UNO_QUERY);
        if (xBox.is())
        {
            xBox->addItemListener(this);
            break;
        }

        Reference< ::com::sun::star::awt::XComboBox >  xCbBox(xControl, UNO_QUERY);
        if (xCbBox.is())
        {
            xCbBox->addItemListener(this);
            break;
        }

        Reference< ::com::sun::star::awt::XListBox >  xListBox(xControl, UNO_QUERY);
        if (xListBox.is())
        {
            xListBox->addItemListener(this);
            break;
        }
        break;
    }
}

//------------------------------------------------------------------------------
void FmXFormController::stopControlListening(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
//  Reference< ::com::sun::star::data::XDatabaseCursor >  xCursor(getModel(), UNO_QUERY);
    sal_Bool bModifyListening = sal_False;

    Reference< ::com::sun::star::form::XBoundComponent >  xBound(xControl, UNO_QUERY);
    if (xBound.is()) // gebundene Controls,
        bModifyListening = sal_True;
    else
    {
        xBound = Reference< ::com::sun::star::form::XBoundComponent > (xControl->getModel(), UNO_QUERY);
        // gibt es eine Datenquelle
        Reference< XPropertySet >  xSet(xBound, UNO_QUERY);
        if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
        {
            Reference< XPropertySet >  xField;
            xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;
            bModifyListening = (xField.is());
        }
    }

    // kuenstliches while
    while (bModifyListening)
    {
        Reference< ::com::sun::star::util::XModifyBroadcaster >  xMod(xControl, UNO_QUERY);
        if (xMod.is())
        {
            xMod->removeModifyListener(this);
            break;
        }
        // alle die Text um vorzeitig ein modified zu erkennen
        Reference< ::com::sun::star::awt::XTextComponent >  xText(xControl, UNO_QUERY);
        if (xText.is())
        {
            xText->removeTextListener(this);
            break;
        }

        Reference< ::com::sun::star::awt::XCheckBox >  xBox(xControl, UNO_QUERY);
        if (xBox.is())
        {
            xBox->removeItemListener(this);
            break;
        }

        Reference< ::com::sun::star::awt::XComboBox >  xCbBox(xControl, UNO_QUERY);
        if (xCbBox.is())
        {
            xCbBox->removeItemListener(this);
            break;
        }

        Reference< ::com::sun::star::awt::XListBox >  xListBox(xControl, UNO_QUERY);
        if (xListBox.is())
        {
            xListBox->removeItemListener(this);
            break;
        }
        break;
    }
}

//------------------------------------------------------------------------------
void FmXFormController::startListening()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_bModified  = sal_False;

    // jetzt anmelden bei gebundenen feldern
    const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();
//  Reference< ::com::sun::star::data::XDatabaseCursor >  xCursor(getModel(), UNO_QUERY);
    for (sal_Int32 i = 0; i < m_aControls.getLength(); i++ )
        startControlListening(pControls[i]);
}

//------------------------------------------------------------------------------
void FmXFormController::stopListening()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_bModified  = sal_False;

    // jetzt anmelden bei gebundenen feldern
    const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();
    for (sal_Int32 i = 0; i < m_aControls.getLength(); i++ )
        stopControlListening(pControls[i]);
}


//------------------------------------------------------------------------------
Reference< ::com::sun::star::awt::XControl >  FmXFormController::findControl(Sequence< Reference< ::com::sun::star::awt::XControl > >& rCtrls, const Reference< ::com::sun::star::awt::XControlModel > & xCtrlModel ) const
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    DBG_ASSERT( xCtrlModel.is(), "findControl - welches ?!" );

    const Reference< ::com::sun::star::awt::XControl > * pCtrls = rCtrls.getConstArray();
    Reference< ::com::sun::star::awt::XControlModel >  xModel;
    for ( sal_Int32 i = 0, nCount = rCtrls.getLength(); i < nCount; i++ )
    {
        // #66449# Speicherueberschreiber durch folgende Zeile
        // Reference< ::com::sun::star::awt::XControlModel >  xModel(pCtrls[i].is() ? pCtrls[i]->getModel() : Reference< ::com::sun::star::awt::XControlModel > ());
        if (pCtrls[i].is())
        {
            xModel = pCtrls[i]->getModel();
            if ((::com::sun::star::awt::XControlModel*)xModel.get() == (::com::sun::star::awt::XControlModel*)xCtrlModel.get())
            {
                Reference< ::com::sun::star::awt::XControl >  xCtrl( pCtrls[i] );
                ::comphelper::removeElementAt(rCtrls, i);
                return xCtrl;
            }
        }
    }
    return Reference< ::com::sun::star::awt::XControl > ();
}

//------------------------------------------------------------------------------
void FmXFormController::insertControl(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_bControlsSorted = sal_False;
    m_aControls.realloc(m_aControls.getLength() + 1);
    m_aControls.getArray()[m_aControls.getLength() - 1] = xControl;

    Reference< ::com::sun::star::awt::XWindow >  xWindow(xControl, UNO_QUERY);
    if (xWindow.is())
    {
        xWindow->addFocusListener(this);
        // register at the Eventattacher but not in filter mode
        if (m_bAttachEvents)
            addToEventAttacher(xControl);
    }

    Reference< ::com::sun::star::frame::XDispatchProviderInterception >  xInterception(xControl, UNO_QUERY);
    if (xInterception.is())
        createInterceptor(xInterception);

    if (xControl.is())
    {
        // we want to know about the reset of the the model of our controls
        // (for correctly resetting m_bModified)
        Reference< ::com::sun::star::form::XReset >  xControlReset(xControl->getModel(), UNO_QUERY);
        if (xControlReset.is())
            xControlReset->addResetListener((::com::sun::star::form::XResetListener*)this);
    }

    if (m_bDBConnection && !m_bFiltering)
        setControlLock(xControl);

    if (isListeningForChanges() && m_bAttachEvents)
        startControlListening(xControl);
}

//------------------------------------------------------------------------------
void FmXFormController::removeControl(const Reference< ::com::sun::star::awt::XControl > & xControl)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    const Reference< ::com::sun::star::awt::XControl > * pCtrls = m_aControls.getArray();
    sal_Int32 nCount = m_aControls.getLength();
    for ( sal_uInt32 i = 0; i < (sal_uInt32)nCount; i++ )
    {
        if ((::com::sun::star::awt::XControl*)xControl.get() == (::com::sun::star::awt::XControl*)pCtrls[i].get())
        {
            ::comphelper::removeElementAt(m_aControls, i);
            break;
        }
    }

    if (m_aFilterControls.size())
    {
        Reference< ::com::sun::star::awt::XTextComponent >  xText(xControl, UNO_QUERY);
        FmFilterControls::iterator iter = m_aFilterControls.find(xText);
        if (iter != m_aFilterControls.end())
            m_aFilterControls.erase(iter);
    }

    Reference< ::com::sun::star::awt::XWindow >  xWindow(xControl, UNO_QUERY);
    if (xWindow.is())
    {
        xWindow->removeFocusListener(this);
        if (m_bDetachEvents)
            removeFromEventAttacher(xControl);
    }

    Reference< ::com::sun::star::frame::XDispatchProviderInterception >  xInterception(xControl, UNO_QUERY);
    if (xInterception.is())
        deleteInterceptor(xInterception);

    if (xControl.is())
    {
        Reference< ::com::sun::star::form::XReset >  xControlReset(xControl->getModel(), UNO_QUERY);
        if (xControlReset.is())
            xControlReset->removeResetListener((::com::sun::star::form::XResetListener*)this);
    }

    if (isListeningForChanges() && m_bDetachEvents)
        stopControlListening(xControl);
}

//------------------------------------------------------------------------------
IMPL_LINK(FmXFormController, OnUpdateDispatchers, void*, EMPTYARG)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_nUpdateDispatcherEvent = 0;
    DBG_ASSERT(m_aControlDispatchInterceptors.size(), "FmXFormController::OnUpdateDispatchers : not intercepting any control dispatches !");

    Reference< ::com::sun::star::frame::XModel >  xFrameModel = getXModel(getModel());
    Reference< ::com::sun::star::frame::XController >  xFrameController = xFrameModel.is() ? xFrameModel->getCurrentController() : Reference< ::com::sun::star::frame::XController > ();
    Reference< ::com::sun::star::frame::XFrame >  xFrame = xFrameController.is() ? xFrameController->getFrame() : Reference< ::com::sun::star::frame::XFrame > ();

    if (!xFrame.is())
    {   // same as in interceptedQueryDispatch, try again
        m_nUpdateDispatcherEvent = Application::PostUserEvent(LINK(this, FmXFormController, OnUpdateDispatchers));
        return 0L;
    }

    // go through all controls we're intercepting and re-register as interceptor
    // (this should cause them to request the dispatches again)
    Interceptors aTemp = m_aControlDispatchInterceptors;
    for (   ConstInterceptorsIterator aIter = aTemp.begin();
            aIter < aTemp.end();
            ++aIter
        )
    {
        FmXDispatchInterceptorImpl* pInterceptor = *aIter;
        pInterceptor->acquire();
        Reference< ::com::sun::star::frame::XDispatchProviderInterception >  xIntercepted = pInterceptor->getIntercepted();
        deleteInterceptor(xIntercepted);
        createInterceptor(xIntercepted);
        pInterceptor->release();
    }

    return 0L;
}

// ::com::sun::star::form::XLoadListener
//------------------------------------------------------------------------------
void FmXFormController::loaded(const ::com::sun::star::lang::EventObject& rEvent)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    Reference< ::com::sun::star::sdbc::XRowSet >  xForm(rEvent.Source, UNO_QUERY);
    // do we have a connected data source
    if (xForm.is() && ::dbtools::getConnection(xForm).is())
    {
        Reference< XPropertySet >  xSet(xForm, UNO_QUERY);
        if (xSet.is())
        {
            Any aVal        = xSet->getPropertyValue(FM_PROP_CYCLE);
            sal_Int32 aVal2;
            ::cppu::enum2int(aVal2,aVal);
            m_bCycle        = !aVal.hasValue() || aVal2 == ::com::sun::star::form::TabulatorCycle_RECORDS;
            m_bCanUpdate    = ::dbtools::canUpdate(xSet);
            m_bCanInsert    = ::dbtools::canInsert(xSet);
            m_bCurrentRecordModified = ::comphelper::getBOOL(xSet->getPropertyValue(FM_PROP_ISMODIFIED));
            m_bCurrentRecordNew      = ::comphelper::getBOOL(xSet->getPropertyValue(FM_PROP_ISNEW));
            if (m_bCanInsert || m_bCanUpdate)   // modificationen sind moeglich
            {
                xSet->addPropertyChangeListener(FM_PROP_ISNEW, this);
                xSet->addPropertyChangeListener(FM_PROP_ISMODIFIED, this);

                // set the Listener for UI interaction
                Reference< ::com::sun::star::sdb::XRowSetApproveBroadcaster >  xApprove(xForm, UNO_QUERY);
                if (xApprove.is())
                    xApprove->addRowSetApproveListener(this);
            }

            // set the locks for the current controls
            if (getContainer().is())
            {
                if (m_nLoadEvent)
                    Application::RemoveUserEvent( m_nLoadEvent );
                m_nLoadEvent = Application::PostUserEvent( LINK(this, FmXFormController,
                                            OnLoad) );
            }
        }
        else
        {
            m_bCanInsert = m_bCanUpdate = m_bCycle = sal_False;
            m_bCurrentRecordModified = sal_False;
            m_bCurrentRecordNew = sal_False;
            m_bLocked = sal_False;
        }
        m_bDBConnection = sal_True;
    }
    else
    {
        m_bDBConnection = sal_False;
        m_bCanInsert = m_bCanUpdate = m_bCycle = sal_False;
        m_bCurrentRecordModified = sal_False;
        m_bCurrentRecordNew = sal_False;
        m_bLocked = sal_False;
    }
}

//------------------------------------------------------------------------------
IMPL_LINK(FmXFormController, OnLoad, void*, EMPTYARG)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_nLoadEvent = 0;
    m_bLocked = determineLockState();

    setLocks();

    if (!m_bLocked)
        startListening();

    // just one exception toggle the auto values
    if (m_bCurrentRecordNew && m_pView)
        toggleAutoFields(sal_True);

    return 1L;
}

//------------------------------------------------------------------------------
void FmXFormController::unloaded(const ::com::sun::star::lang::EventObject& rEvent)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // not interested in ...
}

//------------------------------------------------------------------------------
void FmXFormController::reloading(const ::com::sun::star::lang::EventObject& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // do the same like in unloading
    // just one exception toggle the auto values
    if (m_nToggleEvent)
    {
        Application::RemoveUserEvent( m_nToggleEvent );
        m_nToggleEvent = 0;
    }
    unload();
}

//------------------------------------------------------------------------------
void FmXFormController::reloaded(const ::com::sun::star::lang::EventObject& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    loaded(aEvent);
}

//------------------------------------------------------------------------------
void FmXFormController::unloading(const ::com::sun::star::lang::EventObject& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    unload();
}

//------------------------------------------------------------------------------
void FmXFormController::unload() throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    if (m_nLoadEvent)
    {
        Application::RemoveUserEvent( m_nLoadEvent );
        m_nLoadEvent = 0;
    }

    // be sure not to have autofields
    if (m_bCurrentRecordNew && m_pView)
        toggleAutoFields(sal_False);

    if (m_bDBConnection && isListeningForChanges())
        stopListening();

    Reference< XPropertySet >  xSet(m_xModelAsIndex, UNO_QUERY);
    if (m_bDBConnection && xSet.is())
    {
        if (m_bCanInsert || m_bCanUpdate)
        {
            xSet->removePropertyChangeListener(FM_PROP_ISNEW, this);
            xSet->removePropertyChangeListener(FM_PROP_ISMODIFIED, this);

            // reset the Listener for UI interaction
            Reference< ::com::sun::star::sdb::XRowSetApproveBroadcaster >  xApprove(xSet, UNO_QUERY);
            if (xApprove.is())
                xApprove->removeRowSetApproveListener(this);
        }
    }
    m_bDBConnection = sal_False;
    m_bCanInsert = m_bCanUpdate = m_bCycle = sal_False;
    m_bCurrentRecordModified = m_bCurrentRecordNew = m_bLocked = sal_False;
}

// com::sun::star::sdbc::XRowSetListener
//------------------------------------------------------------------------------
void FmXFormController::cursorMoved(const ::com::sun::star::lang::EventObject& event) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // toggle the locking ?
    if (m_bLocked != determineLockState())
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        m_bLocked = !m_bLocked;
        setLocks();
        if (isListeningForChanges())
            startListening();
        else
            stopListening();
    }
}

//------------------------------------------------------------------------------
void FmXFormController::rowChanged(const ::com::sun::star::lang::EventObject& event) throw( RuntimeException )
{
    // not interested in ...
}
//------------------------------------------------------------------------------
void FmXFormController::rowSetChanged(const ::com::sun::star::lang::EventObject& event) throw( RuntimeException )
{
    // not interested in ...
}


// ::com::sun::star::container::XContainerListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::elementInserted(const ::com::sun::star::container::ContainerEvent& evt)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    Reference< ::com::sun::star::awt::XControl >  xControl;
    evt.Element >>= xControl;
    if (!xControl.is())
        return;

    ::osl::MutexGuard aGuard( m_aMutex );
    Reference< ::com::sun::star::form::XFormComponent >  xModel(xControl->getModel(), UNO_QUERY);
    if (xModel.is() && m_xModelAsIndex == xModel->getParent())
    {
        insertControl(xControl);

        if (m_aInsertTimer.IsActive())
            m_aInsertTimer.Stop();

        m_aInsertTimer.Start();
    }
    // are we in filtermode and a ::com::sun::star::util::XModeSelector has inserted an element
    else if (m_bFiltering && Reference< ::com::sun::star::util::XModeSelector > (evt.Source, UNO_QUERY).is())
    {
        xModel = Reference< ::com::sun::star::form::XFormComponent > (evt.Source, UNO_QUERY);
        if (xModel.is() && m_xModelAsIndex == xModel->getParent())
        {
            Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
            if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
            {
                // does the model use a bound field ?
                Reference< XPropertySet >  xField;
                xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;

                Reference< ::com::sun::star::awt::XTextComponent >  xText(xControl, UNO_QUERY);
                // may we filter the field?
                if (xText.is() && xField.is() && ::comphelper::hasProperty(FM_PROP_SEARCHABLE, xField) &&
                    ::comphelper::getBOOL(xField->getPropertyValue(FM_PROP_SEARCHABLE)))
                {
                    m_aFilterControls[xText] = xField;
                    xText->addTextListener(this);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::elementReplaced(const ::com::sun::star::container::ContainerEvent& evt)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );

    // Remove the control
    Reference< ::com::sun::star::awt::XControl >  xReplControl;
    evt.ReplacedElement >>= xReplControl;
    Reference< ::com::sun::star::form::XFormComponent >  xModel(xReplControl->getModel(), UNO_QUERY);
    if (xModel.is() && m_xModelAsIndex == xModel->getParent())
        removeControl(xReplControl);
    else if (m_aFilterControls.size())
    {
        Reference< ::com::sun::star::awt::XTextComponent >  xText(xReplControl, UNO_QUERY);
        FmFilterControls::iterator iter = m_aFilterControls.find(xText);
        if (iter != m_aFilterControls.end())
            m_aFilterControls.erase(iter);
    }

    // Add the new one
    elementInserted(evt);
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::elementRemoved(const ::com::sun::star::container::ContainerEvent& evt)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );

    Reference< ::com::sun::star::awt::XControl >  xControl;
    evt.Element >>= xControl;
    if (!xControl.is())
        return;

    Reference< ::com::sun::star::form::XFormComponent >  xModel(xControl->getModel(), UNO_QUERY);
    if (xModel.is() && m_xModelAsIndex == xModel->getParent())
    {
        removeControl(xControl);
        // TabOrder nicht neu berechnen, da das intern schon funktionieren mu�!
    }
    else if (m_aFilterControls.size())
    {
        Reference< ::com::sun::star::awt::XTextComponent >  xText(xControl, UNO_QUERY);
        FmFilterControls::iterator iter = m_aFilterControls.find(xText);
        if (iter != m_aFilterControls.end())
            m_aFilterControls.erase(iter);
    }
}

//------------------------------------------------------------------------------
Reference< ::com::sun::star::awt::XControl >  FmXFormController::isInList(const Reference< ::com::sun::star::awt::XWindowPeer > & xPeer) const
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    const Reference< ::com::sun::star::awt::XControl > * pCtrls = m_aControls.getConstArray();

    sal_uInt32 nCtrls = m_aControls.getLength();
    for ( sal_uInt32 n = 0; n < nCtrls && xPeer.is(); n++ )
    {
        if (pCtrls[n].is())
        {
            Reference< ::com::sun::star::awt::XVclWindowPeer >  xCtrlPeer(pCtrls[n]->getPeer(), UNO_QUERY);
            if ((::com::sun::star::awt::XWindowPeer*)xCtrlPeer.get() == (::com::sun::star::awt::XWindowPeer*)xPeer.get() || xCtrlPeer->isChild(xPeer))
                return pCtrls[n];
        }
    }
    return Reference< ::com::sun::star::awt::XControl > ();
}

//------------------------------------------------------------------------------
void FmXFormController::activateFirst()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::activateFirst : invalid aggregate !");
    if (m_xTabController.is())
        m_xTabController->activateFirst();
}

//------------------------------------------------------------------------------
void FmXFormController::activateLast()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::osl::MutexGuard aGuard( m_aMutex );
    DBG_ASSERT(m_xTabController.is(), "FmXFormController::activateLast : invalid aggregate !");
    if (m_xTabController.is())
        m_xTabController->activateLast();
}

// ::com::sun::star::form::XFormController
//------------------------------------------------------------------------------
Reference< XControl> SAL_CALL FmXFormController::getCurrentControl(void)
{
    return m_xCurrentControl;
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::addActivateListener(const Reference< XFormControllerListener > & l)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aActivateListeners.addInterface(l);
}
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::removeActivateListener(const Reference< XFormControllerListener > & l)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aActivateListeners.removeInterface(l);
}

//------------------------------------------------------------------------------
void FmXFormController::setFilter(vector<FmFieldInfo>& rFieldInfos)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // create the composer
    Reference< ::com::sun::star::sdbc::XRowSet >  xForm(m_xModelAsIndex, UNO_QUERY);
    Reference< ::com::sun::star::sdbc::XConnection >  xConnection(::dbtools::getConnection(xForm));
    if (xForm.is())
    {
        Reference< ::com::sun::star::sdb::XSQLQueryComposerFactory >  xFactory(xConnection, UNO_QUERY);
        if (xFactory.is())
        {
            m_xComposer = xFactory->createQueryComposer();
            try
            {
                Reference< XPropertySet >  xSet(xForm, UNO_QUERY);
                ::rtl::OUString aStatement  = ::comphelper::getString(xSet->getPropertyValue(FM_PROP_ACTIVECOMMAND));
                ::rtl::OUString aFilter     = ::comphelper::getString(xSet->getPropertyValue(FM_PROP_FILTER_CRITERIA));
                m_xComposer->setQuery(aStatement);
                m_xComposer->setFilter(aFilter);
            }
            catch(...)
            {
                DBG_ERROR("Exception occured!");
            }
        }
    }

    if (m_xComposer.is())
    {
        Sequence < PropertyValue> aLevel;
        Sequence< Sequence < PropertyValue > > aFilterRows = m_xComposer->getStructuredFilter();

        // ok, we recieve the list of filters as sequence of fieldnames, value
        // now we have to transform the fieldname into UI names, that could be a label of the field or
        // a aliasname or the fieldname itself

        // first adjust the field names if necessary
        Reference< ::com::sun::star::container::XNameAccess> xQueryColumns = Reference< ::com::sun::star::sdbcx::XColumnsSupplier >
                                            (m_xComposer, UNO_QUERY)->getColumns();

        for (vector<FmFieldInfo>::iterator iter = rFieldInfos.begin();
            iter != rFieldInfos.end(); iter++)
        {
            try
            {
                xQueryColumns->getByName((*iter).aFieldName) >>= (*iter).xField;
                (*iter).xField->getPropertyValue(FM_PROP_REALNAME) >>= (*iter).aFieldName;
            }
            catch (...)
            {
            }
        }

        // now transfer the filters into Value/TextComponent pairs
        ::comphelper::UStringMixEqual aCompare(xConnection->getMetaData()->storesMixedCaseQuotedIdentifiers());

        // retrieving the filter
        const Sequence < PropertyValue >* pRow = aFilterRows.getConstArray();
        for (sal_Int32 i = 0, nLen = aFilterRows.getLength(); i < nLen; ++i)
        {
            FmFilterRow aRow;

            // search a field for the given name
            const PropertyValue* pRefValues = pRow[i].getConstArray();
            for (sal_Int32 j = 0, nLen1 = pRow[i].getLength(); j < nLen1; j++)
            {
                // look for the text component
                Reference< XPropertySet > xField;
                try
                {
                    Reference< XPropertySet > xSet;
                    ::rtl::OUString aRealName;

                    // first look with the given name
                    if (xQueryColumns->hasByName(pRefValues[j].Name))
                    {
                        xQueryColumns->getByName(pRefValues[j].Name) >>= xSet;

                        // get the RealName
                        xSet->getPropertyValue(::rtl::OUString::createFromAscii("RealName")) >>= aRealName;

                        // compare the condition field name and the RealName
                        if (aCompare(aRealName, pRefValues[j].Name))
                            xField = xSet;
                    }
                    if (!xField.is())
                    {
                        // no we have to check every column to find the realname
                        Reference< ::com::sun::star::container::XIndexAccess > xColumnsByIndex(xQueryColumns, UNO_QUERY);
                        for (sal_Int32 n = 0, nCount = xColumnsByIndex->getCount(); n < nCount; n++)
                        {
                            xColumnsByIndex->getByIndex(n) >>= xSet;
                            xSet->getPropertyValue(::rtl::OUString::createFromAscii("RealName")) >>= aRealName;
                            if (aCompare(aRealName, pRefValues[j].Name))
                            {
                                // get the column by its alias
                                xField = xSet;
                                break;
                            }
                        }
                    }
                    if (!xField.is())
                        continue;
                }
                catch (...)
                {
                    continue;
                }

                // find the text component
                for (vector<FmFieldInfo>::iterator iter = rFieldInfos.begin();
                    iter != rFieldInfos.end(); iter++)
                {
                    // we found the field so insert a new entry to the filter row
                    if ((*iter).xField == xField)
                    {
                        // do we already have the control ?
                        if (aRow.find((*iter).xText) != aRow.end())
                        {
                            ::rtl::OUString aCompText = aRow[(*iter).xText];
                            aCompText += ::rtl::OUString::createFromAscii(" ");
                            ::rtl::OString aVal = m_aParser.getContext().getIntlKeywordAscii(OParseContext::KEY_AND);
                            aCompText += ::rtl::OUString(aVal.getStr(),aVal.getLength(),RTL_TEXTENCODING_ASCII_US);
                            aCompText += ::rtl::OUString::createFromAscii(" ");
                            aCompText += ::comphelper::getString(pRefValues[j].Value);
                            aRow[(*iter).xText] = aCompText;
                        }
                        else
                            aRow[(*iter).xText] = ::comphelper::getString(pRefValues[j].Value);
                        break;
                    }
                }
            }

            if (aRow.empty())
                continue;

            m_aFilters.push_back(aRow);
        }
    }

    // now set the filter controls
    for (vector<FmFieldInfo>::iterator iter = rFieldInfos.begin();
         iter != rFieldInfos.end(); iter++)
    {
        m_aFilterControls[(*iter).xText] = (*iter).xField;
    }

    // add an empty row
    m_aFilters.push_back(FmFilterRow());
}

//------------------------------------------------------------------------------
void FmXFormController::startFiltering()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!m_pView)
    {
        DBG_ERROR("FmXFormController::startFiltering : you can't filter if you created me as service !");
        return;
    }

    // stop listening for controls
    if (isListeningForChanges())
        stopListening();

    m_bFiltering = sal_True;

    // as we don't want new controls to be attached to the scripting environment
    // we change attach flags
    m_bAttachEvents = sal_False;

    // Austauschen der Kontrols fuer das aktuelle Formular
    Sequence < Reference< ::com::sun::star::awt::XControl >  > aControls(m_aControls);
    const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();
    SdrPageView* pCurPageView = m_pView->GetPageViewPvNum(0);

    // the control we have to activate after replacement
    Reference< ::com::sun::star::awt::XControl >  xNewActiveControl;
    Reference< ::com::sun::star::sdbc::XConnection >  xConnection(::dbtools::getConnection(Reference< ::com::sun::star::sdbc::XRowSet > (m_xModelAsIndex, UNO_QUERY)));
    Reference< ::com::sun::star::sdbc::XDatabaseMetaData >  xMetaData(xConnection->getMetaData());
    Reference< ::com::sun::star::util::XNumberFormatsSupplier >  xFormatSupplier = ::dbtools::getNumberFormats(xConnection, sal_True);
    Reference< ::com::sun::star::util::XNumberFormatter >  xFormatter(m_xORB
                        ->createInstance(::rtl::OUString::createFromAscii("com.sun.star.util.NumberFormatter")), UNO_QUERY);
    xFormatter->attachNumberFormatsSupplier(xFormatSupplier);

    // structure for storing the field info
    vector<FmFieldInfo> aFieldInfos;

    sal_uInt16 nPos = pCurPageView ? pCurPageView->GetWinList().Find((OutputDevice*)m_pView->GetActualOutDev()) : SDRPAGEVIEWWIN_NOTFOUND;
    if (nPos != SDRPAGEVIEWWIN_NOTFOUND)
    {
        for (sal_Int32 i = aControls.getLength(); i > 0;)
        {
            const Reference< ::com::sun::star::awt::XControl > & xControl = pControls[--i];
            if (xControl.is())
            {
                // no events for the control anymore
                removeFromEventAttacher(xControl);

                // do we have a mode selector
                Reference< ::com::sun::star::util::XModeSelector >  xSelector(xControl, UNO_QUERY);
                if (xSelector.is())
                {
                    xSelector->setMode(FILTER_MODE);

                    // listening for new controls of the selector
                    Reference< ::com::sun::star::container::XContainer >  xContainer(xSelector, UNO_QUERY);
                    if (xContainer.is())
                        xContainer->addContainerListener(this);

                    Reference< ::com::sun::star::container::XEnumerationAccess >  xElementAccess(xSelector, UNO_QUERY);
                    if (xElementAccess.is())
                    {
                        Reference< ::com::sun::star::container::XEnumeration >  xEnumeration(xElementAccess->createEnumeration());
                        Reference< ::com::sun::star::awt::XControl >  xControl;
                        while (xEnumeration->hasMoreElements())
                        {
                            xEnumeration->nextElement() >>= xControl;
                            if (xControl.is())
                            {
                                Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
                                if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
                                {
                                    // does the model use a bound field ?
                                    Reference< XPropertySet >  xField;
                                    xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;

                                    Reference< ::com::sun::star::awt::XTextComponent >  xText(xControl, UNO_QUERY);
                                    // may we filter the field?
                                    if (xText.is() && xField.is() && ::comphelper::hasProperty(FM_PROP_SEARCHABLE, xField) &&
                                        ::comphelper::getBOOL(xField->getPropertyValue(FM_PROP_SEARCHABLE)))
                                    {
                                        aFieldInfos.push_back(FmFieldInfo(xField, xText));
                                        xText->addTextListener(this);
                                    }
                                }
                            }
                        }
                    }
                    continue;
                }

                Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
                if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
                {
                    // does the model use a bound field ?
                    Any aVal = xSet->getPropertyValue(FM_PROP_BOUNDFIELD);
                    Reference< XPropertySet >  xField;
                    aVal >>= xField;

                    // may we filter the field?

                    if (xField.is() && ::comphelper::hasProperty(FM_PROP_SEARCHABLE, xField) &&
                        ::comphelper::getBOOL(xField->getPropertyValue(FM_PROP_SEARCHABLE)))
                    {
                        const SdrPageViewWinRec& rWR = pCurPageView->GetWinList()[nPos];
                        const SdrUnoControlList& rControlList = rWR.GetControlList();
                        sal_uInt16 nCtrlNum = rControlList.Find(xControl);
                        if (nCtrlNum != SDRUNOCONTROL_NOTFOUND)
                        {
                            // Setzen des FilterControls
                            SdrUnoControlRec& rControlRec = (SdrUnoControlRec&)rControlList[nCtrlNum];
                            FmXFilterControl* pFilterControl = new FmXFilterControl(m_xORB,xSet, xField, xMetaData, xFormatter, m_pWindow);
                            Reference< ::com::sun::star::awt::XControl >  xNewControl(pFilterControl);
                            Reference< ::com::sun::star::awt::XTextComponent >  xText(pFilterControl);

                            // merken in der Map
                            aFieldInfos.push_back(FmFieldInfo(xField, xText));
                            xText->addTextListener(this);

                            // setting the focus if the current control
                            // is the active one
                            if ((::com::sun::star::awt::XControl*)m_xActiveControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                            {
                                xNewActiveControl = xNewControl;
                                m_xActiveControl = m_xCurrentControl = NULL;
                            }
                            else if ((::com::sun::star::awt::XControl*)m_xCurrentControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                                m_xCurrentControl = xNewControl;

                            rControlRec.ReplaceControl(xNewControl);
                        }
                    }
                }
                else
                {
                    // abmelden vom EventManager
                }
            }
        }
    }

    // we have all filter controls now, so the next step is to read the filters from the form
    // resolve all aliases and set the current filter to the according structure
    setFilter(aFieldInfos);

    // setting the focus to the replacing control
    Reference< ::com::sun::star::awt::XWindow >  xWindow(xNewActiveControl, UNO_QUERY);
    if (xWindow.is())
        xWindow->setFocus();

    if (m_bCanInsert || m_bCanUpdate)   // modificationen sind moeglich
    {
        Reference< XPropertySet >  xSet(m_xModelAsIndex, UNO_QUERY);
        xSet->removePropertyChangeListener(FM_PROP_ISNEW, this);
        xSet->removePropertyChangeListener(FM_PROP_ISMODIFIED, this);
    }

    // set the text for all filters
    FmFilterRow& rRow = m_aFilters[m_nCurrentFilterPosition];
    for (FmFilterRow::const_iterator iter2 = rRow.begin();
         iter2 != rRow.end(); iter2++)
    {
        (*iter2).first->setText((*iter2).second);
    }

    // lock all controls which are not used for filtering
    m_bLocked = determineLockState();
    setLocks();
    m_bAttachEvents = sal_True;
}

//------------------------------------------------------------------------------
void FmXFormController::stopFiltering()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!m_pView)
    {
        DBG_ERROR("FmXFormController::startFiltering : you can't filter if you created me as service !");
        return;
    }

    m_bFiltering = sal_False;
    m_bDetachEvents = sal_False;

    ::comphelper::disposeComponent(m_xComposer);

    // Austauschen der Kontrols fuer das aktuelle Formular
    Sequence < Reference< ::com::sun::star::awt::XControl >  > aControls(m_aControls);
    const Reference< ::com::sun::star::awt::XControl > * pControls = m_aControls.getConstArray();
    SdrPageView* pCurPageView = m_pView->GetPageViewPvNum(0);
    sal_uInt16 nPos = pCurPageView ? pCurPageView->GetWinList().Find((OutputDevice*)m_pView->GetActualOutDev()) : SDRPAGEVIEWWIN_NOTFOUND;

    // the control we have to activate after replacement
    Reference< ::com::sun::star::awt::XControl >  xNewActiveControl;

    // clear the filter control map
    for (FmFilterControls::const_iterator iter = m_aFilterControls.begin();
         iter != m_aFilterControls.end(); iter++)
         (*iter).first->removeTextListener(this);

    m_aFilterControls.clear();

    if (nPos != SDRPAGEVIEWWIN_NOTFOUND)
    {
        for (sal_Int32 i = aControls.getLength(); i > 0;)
        {
            const Reference< ::com::sun::star::awt::XControl > & xControl = pControls[--i];
            if (xControl.is())
            {
                // now enable eventhandling again
                addToEventAttacher(xControl);

                Reference< ::com::sun::star::util::XModeSelector >  xSelector(xControl, UNO_QUERY);
                if (xSelector.is())
                {
                    xSelector->setMode(DATA_MODE);

                    // listening for new controls of the selector
                    Reference< ::com::sun::star::container::XContainer >  xContainer(xSelector, UNO_QUERY);
                    if (xContainer.is())
                        xContainer->removeContainerListener(this);
                    continue;
                }

                Reference< XPropertySet >  xSet(xControl->getModel(), UNO_QUERY);
                if (xSet.is() && ::comphelper::hasProperty(FM_PROP_BOUNDFIELD, xSet))
                {
                    // does the model use a bound field ?
                    Reference< XPropertySet >  xField;
                    xSet->getPropertyValue(FM_PROP_BOUNDFIELD) >>= xField;

                    // may we filter the field?
                    if (xField.is() && ::comphelper::hasProperty(FM_PROP_SEARCHABLE, xField) &&
                        ::comphelper::getBOOL(xField->getPropertyValue(FM_PROP_SEARCHABLE)))
                    {
                        const SdrPageViewWinRec& rWR = pCurPageView->GetWinList()[nPos];
                        const SdrUnoControlList& rControlList = rWR.GetControlList();
                        sal_uInt16 nCtrlNum = rControlList.Find(xControl);
                        if (nCtrlNum != SDRUNOCONTROL_NOTFOUND)
                        {
                            // Zuruecksetzen des Controls auf das Defaultcontrol
                            SdrUnoControlRec& rControlRec = (SdrUnoControlRec&)rControlList[nCtrlNum];
                            ::rtl::OUString aServiceName = ::comphelper::getString(xSet->getPropertyValue(FM_PROP_DEFAULTCONTROL));
                            Reference< ::com::sun::star::awt::XControl >  xNewControl(m_xORB->createInstance(aServiceName), UNO_QUERY);

                            // setting the focus if the current control
                            // is the active one
                            if ((::com::sun::star::awt::XControl*)m_xActiveControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                            {
                                xNewActiveControl = xNewControl;
                                m_xActiveControl = m_xCurrentControl = NULL;
                            }
                            else if ((::com::sun::star::awt::XControl*)m_xCurrentControl.get() == (::com::sun::star::awt::XControl*)xControl.get())
                                m_xCurrentControl = xNewControl;

                            rControlRec.ReplaceControl(xNewControl);
                        }
                    }
                }
            }
        }
    }

    // setting the focus to the replacing control
    Reference< ::com::sun::star::awt::XWindow >  xWindow(xNewActiveControl, UNO_QUERY);
    if (xWindow.is())
        xWindow->setFocus();

    if (m_bCanInsert || m_bCanUpdate)   // modificationen sind moeglich
    {
        Reference< XPropertySet >  xSet(m_xModelAsIndex, UNO_QUERY);
        xSet->addPropertyChangeListener(FM_PROP_ISNEW, this);
        xSet->addPropertyChangeListener(FM_PROP_ISMODIFIED, this);
    }

    m_bDetachEvents = sal_True;

    m_aFilters.clear();
    m_nCurrentFilterPosition = 0;

    // release the locks if possible
    // lock all controls which are not used for filtering
    m_bLocked = determineLockState();
    setLocks();

    // restart listening for control modifications
    if (isListeningForChanges())
        startListening();
}

// ::com::sun::star::util::XModeSelector
//------------------------------------------------------------------------------
void FmXFormController::setMode(const ::rtl::OUString& Mode) throw( ::com::sun::star::lang::NoSupportException, RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (!supportsMode(Mode))
        throw ::com::sun::star::lang::NoSupportException();

    if (Mode == m_aMode)
        return;

    m_aMode = Mode;

    if (Mode == FILTER_MODE)
        startFiltering();
    else
        stopFiltering();

    for (FmFormControllers::const_iterator i = m_aChilds.begin();
        i != m_aChilds.end(); i++)
    {
        Reference< ::com::sun::star::util::XModeSelector > (*i, UNO_QUERY)->setMode(Mode);
    }
}

//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL FmXFormController::getMode(void) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    return m_aMode;
}

//------------------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL FmXFormController::getSupportedModes(void) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    static Sequence< ::rtl::OUString > aModes;
    if (!aModes.getLength())
    {
        aModes.realloc(2);
        ::rtl::OUString* pModes = aModes.getArray();
        pModes[0] = DATA_MODE;
        pModes[1] = FILTER_MODE;
    }
    return aModes;
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::supportsMode(const ::rtl::OUString& Mode) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    Sequence< ::rtl::OUString > aModes(getSupportedModes());
    const ::rtl::OUString* pModes = aModes.getConstArray();
    for (sal_Int32 i = aModes.getLength(); i > 0; )
    {
        if (pModes[--i] == Mode)
            return sal_True;
    }
    return sal_False;
}

//------------------------------------------------------------------------------
Window* FmXFormController::getDialogParentWindow()
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    Window* pParent = m_pWindow;
    if (!pParent)
    {
        Reference< ::com::sun::star::awt::XControlContainer >  xContainer = getContainer();
        Reference< ::com::sun::star::awt::XControl >  xCtrl(xContainer, UNO_QUERY);
        if (xCtrl.is())
        {
            Reference< ::com::sun::star::awt::XWindowPeer >  xPeer(xCtrl->getPeer(), UNO_QUERY);
            if (xPeer.is())
                pParent = VCLUnoHelper::GetWindow(xPeer);
        }
    }

    return pParent;
}
// ::com::sun::star::sdb::XRowSetApproveListener
//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::approveRowChange(const ::com::sun::star::sdb::RowChangeEvent& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::cppu::OInterfaceIteratorHelper aIter(m_aRowSetApproveListeners);
    sal_Bool bValid = sal_True;
    if (aIter.hasMoreElements())
    {
        ::com::sun::star::sdb::RowChangeEvent aEvt(aEvent);
        aEvt.Source = *this;
        bValid = ((::com::sun::star::sdb::XRowSetApproveListener*)aIter.next())->approveRowChange(aEvt);
    }

    if (bValid && (
        aEvent.Action == ::com::sun::star::sdb::RowChangeAction::INSERT ||
        aEvent.Action == ::com::sun::star::sdb::RowChangeAction::UPDATE))
    {
        if (m_pView)
        {   // we're working for a FormView
            if (!(m_pWindow || (m_pView->GetActualOutDev() == (const OutputDevice*)m_pWindow ||
                !m_pView->GetActualOutDev() && m_pWindow->IsActive())))
                // we're not active
                return sal_True;
        }
        // default handling
        // check Values on NULL and required flag
        Reference< ::com::sun::star::sdbcx::XColumnsSupplier >  xSupplyCols(aEvent.Source, UNO_QUERY);
        Reference< ::com::sun::star::container::XEnumerationAccess >  xEnumAccess;
        if (xSupplyCols.is())
            xEnumAccess = Reference< ::com::sun::star::container::XEnumerationAccess > (xSupplyCols->getColumns(),UNO_QUERY);

        if (!xSupplyCols.is() || !xEnumAccess.is())
        {
            DBG_ERROR("FmXFormController::approveRowChange : source is no columns supplier or columns aren't enumerable !");
            // can't do anything here ...
            return sal_True;
        }

        Reference< ::com::sun::star::container::XEnumeration >  xEnumeration = xEnumAccess->createEnumeration();
        Reference< XPropertySet >  xFieldSet;
        while (xEnumeration->hasMoreElements())
        {
            xEnumeration->nextElement() >>= xFieldSet;
            Reference< ::com::sun::star::sdb::XColumn >  xColumn(xFieldSet, UNO_QUERY);
            if (!xFieldSet.is() || !xColumn.is())
            {
                DBG_ERROR("FmXFormController::approveRowChange : invalid field !");
                continue;
            }

            sal_Bool bRequired = ::comphelper::getINT32(xFieldSet->getPropertyValue(FM_PROP_ISNULLABLE)) == ::com::sun::star::sdbc::ColumnValue::NO_NULLS;
            if (!bRequired)
                continue;

            sal_Bool bAutoIncrement = ::comphelper::getBOOL(xFieldSet->getPropertyValue(FM_PROP_AUTOINCREMENT));
            if (bAutoIncrement)
                continue;

            ::rtl::OUString aFieldName(::comphelper::getString(xFieldSet->getPropertyValue(FM_PROP_NAME)));

            if (!xColumn->getString().getLength() && xColumn->wasNull())
            {
                Sequence< Reference< ::com::sun::star::awt::XControl > > aControls(getControls());
                sal_Int32 nLength = aControls.getLength();
                sal_Int32 i(0);
                const Reference< ::com::sun::star::awt::XControl > * pControls = aControls.getConstArray();
                for (; i < nLength; i++)
                {
                    if (pControls[i].is())
                    {
                        Reference< XPropertySet >  xModel(pControls[i]->getModel(), UNO_QUERY);
                        if (xModel.is())
                        {
                            if (::comphelper::hasProperty(FM_PROP_CONTROLSOURCE, xModel))
                            {
                                ::rtl::OUString aName = ::comphelper::getString(xModel->getPropertyValue(FM_PROP_CONTROLSOURCE));
                                if (aName == aFieldName)    // Control gefunden
                                    break;
                            }
                        }
                    }
                }

                ::vos::OGuard aGuard(Application::GetSolarMutex());

                UniString aTitle(SVX_RES(RID_STR_WRITEERROR));
                UniString aMsg(SVX_RES(RID_ERR_FIELDREQUIRED));
                aMsg.SearchAndReplace('#', aFieldName.getStr());

                SvxDBMsgBox aDlg(getDialogParentWindow(), aTitle, aMsg, WB_OK | WB_DEF_OK,
                                SvxDBMsgBox::Info);
                aDlg.Execute();
                if ( i < nLength)
                {
                    Reference< ::com::sun::star::awt::XWindow >  xWindow(pControls[i], UNO_QUERY);
                    xWindow->setFocus();
                }
                return sal_False;
            }
        }
    }
    return bValid;
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::approveCursorMove(const ::com::sun::star::lang::EventObject& event) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::cppu::OInterfaceIteratorHelper aIter(m_aRowSetApproveListeners);
    if (aIter.hasMoreElements())
    {
        ::com::sun::star::lang::EventObject aEvt(event);
        aEvt.Source = *this;
        return ((::com::sun::star::sdb::XRowSetApproveListener*)aIter.next())->approveCursorMove(aEvt);
    }

    return sal_True;
}

//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::approveRowSetChange(const ::com::sun::star::lang::EventObject& event) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    ::cppu::OInterfaceIteratorHelper aIter(m_aRowSetApproveListeners);
    if (aIter.hasMoreElements())
    {
        ::com::sun::star::lang::EventObject aEvt(event);
        aEvt.Source = *this;
        return ((::com::sun::star::sdb::XRowSetApproveListener*)aIter.next())->approveRowSetChange(aEvt);
    }

    return sal_True;
}

// ::com::sun::star::sdb::XRowSetApproveBroadcaster
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::addRowSetApproveListener(const Reference< ::com::sun::star::sdb::XRowSetApproveListener > & _rxListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aRowSetApproveListeners.addInterface(_rxListener);
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::removeRowSetApproveListener(const Reference< ::com::sun::star::sdb::XRowSetApproveListener > & _rxListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aRowSetApproveListeners.removeInterface(_rxListener);
}

// ::com::sun::star::form::XErrorListener
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::errorOccured(const ::com::sun::star::sdb::SQLErrorEvent& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (m_pView)
    {   // we're working for a FormView
        if (!(m_pWindow || (m_pView->GetActualOutDev() == (const OutputDevice*)m_pWindow ||
            !m_pView->GetActualOutDev() && m_pWindow->IsActive())))
            // we're not active
            return;
    }

    ::cppu::OInterfaceIteratorHelper aIter(m_aErrorListeners);
    if (aIter.hasMoreElements())
    {
        ::com::sun::star::sdb::SQLErrorEvent aEvt(aEvent);
        aEvt.Source = *this;
        ((::com::sun::star::sdb::XSQLErrorListener*)aIter.next())->errorOccured(aEvt);
    }
    else
    {
        displayException(aEvent);
    }
}

// ::com::sun::star::form::XErrorBroadcaster
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::addSQLErrorListener(const Reference< ::com::sun::star::sdb::XSQLErrorListener > & aListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aErrorListeners.addInterface(aListener);
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::removeSQLErrorListener(const Reference< ::com::sun::star::sdb::XSQLErrorListener > & aListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aErrorListeners.removeInterface(aListener);
}

// ::com::sun::star::form::XDatabaseParameterBroadcaster
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::addParameterListener(const Reference< ::com::sun::star::form::XDatabaseParameterListener > & aListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aParameterListeners.addInterface(aListener);
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::removeParameterListener(const Reference< ::com::sun::star::form::XDatabaseParameterListener > & aListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aParameterListeners.removeInterface(aListener);
}

// ::com::sun::star::form::XDatabaseParameterListener
//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::approveParameter(const ::com::sun::star::form::DatabaseParameterEvent& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (m_pView)
    {   // we're working for a FormView
        if (!(m_pWindow || (m_pView->GetActualOutDev() == (const OutputDevice*)m_pWindow ||
            !m_pView->GetActualOutDev() && m_pWindow->IsActive())))
            // we're not active
            return sal_True;
    }

    ::cppu::OInterfaceIteratorHelper aIter(m_aParameterListeners);
    if (aIter.hasMoreElements())
    {
        ::com::sun::star::form::DatabaseParameterEvent aEvt(aEvent);
        aEvt.Source = *this;
        return ((::com::sun::star::form::XDatabaseParameterListener*)aIter.next())->approveParameter(aEvt);
    }
    else
    {
        // default handling: instantiate an interaction handler and let it handle the parameter request
        try
        {
            // two continuations allowed: OK and Cancel
            OParameterContinuation* pParamValues = new OParameterContinuation;
            OInteractionAbort* pAbort = new OInteractionAbort;
            // the request
            ParametersRequest aRequest;
            aRequest.Parameters = aEvent.Parameters;
            aRequest.Connection = getConnection(Reference< XRowSet >(aEvent.Source, UNO_QUERY));
            OInteractionRequest* pParamRequest = new OInteractionRequest(makeAny(aRequest));
            Reference< XInteractionRequest > xParamRequest(pParamRequest);
            // some knittings
            pParamRequest->addContinuation(pParamValues);
            pParamRequest->addContinuation(pAbort);

            // create the handler, let it handle the request
            Reference< XInteractionHandler > xHandler(m_xORB->createInstance(SRV_SDB_INTERACTION_HANDLER), UNO_QUERY);
            if (xHandler.is())
            {
                ::vos::OGuard aGuard(Application::GetSolarMutex());
                xHandler->handle(xParamRequest);
            }

            if (!pParamValues->wasSelected())
                // canceled
                return sal_False;

            // transfer the values into the parameter supplier
            Sequence< PropertyValue > aFinalValues = pParamValues->getValues();
            if (aFinalValues.getLength() != aRequest.Parameters->getCount())
            {
                DBG_ERROR("FmXFormController::approveParameter: the InteractionHandler returned nonsense!");
                return sal_False;
            }
            const PropertyValue* pFinalValues = aFinalValues.getConstArray();
            for (sal_Int32 i=0; i<aFinalValues.getLength(); ++i, ++pFinalValues)
            {
                Reference< XPropertySet > xParam;
                ::cppu::extractInterface(xParam, aRequest.Parameters->getByIndex(i));
                if (xParam.is())
                {
#ifdef DBG_UTIL
                    ::rtl::OUString sName;
                    xParam->getPropertyValue(FM_PROP_NAME) >>= sName;
                    DBG_ASSERT(sName.equals(pFinalValues->Name), "FmXFormController::approveParameter: suspicious value names!");
#endif
                    try { xParam->setPropertyValue(FM_PROP_VALUE, pFinalValues->Value); }
                    catch(Exception&)
                    {
                        DBG_ERROR("FmXFormController::approveParameter: setting one of the properties failed!");
                    }
                }
            }
        }
        catch(Exception&)
        {
            DBG_ERROR("FmXFormController::approveParameter: caught an Exception (tried to let the InteractionHandler handle it)!");
        }
    }
    return sal_True;
}

// ::com::sun::star::form::XConfirmDeleteBroadcaster
//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::addConfirmDeleteListener(const Reference< ::com::sun::star::form::XConfirmDeleteListener > & aListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aDeleteListeners.addInterface(aListener);
}

//------------------------------------------------------------------------------
void SAL_CALL FmXFormController::removeConfirmDeleteListener(const Reference< ::com::sun::star::form::XConfirmDeleteListener > & aListener) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    m_aDeleteListeners.removeInterface(aListener);
}

// ::com::sun::star::form::XConfirmDeleteListener
//------------------------------------------------------------------------------
sal_Bool SAL_CALL FmXFormController::confirmDelete(const ::com::sun::star::sdb::RowChangeEvent& aEvent) throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    if (m_pView)
    {   // we're working for a FormView
        if (!(m_pWindow || (m_pView->GetActualOutDev() == (const OutputDevice*)m_pWindow ||
            !m_pView->GetActualOutDev() && m_pWindow->IsActive())))
            // we're not active
            return sal_True;
    }

    ::cppu::OInterfaceIteratorHelper aIter(m_aDeleteListeners);
    if (aIter.hasMoreElements())
    {
        ::com::sun::star::sdb::RowChangeEvent aEvt(aEvent);
        aEvt.Source = *this;
        return ((::com::sun::star::form::XConfirmDeleteListener*)aIter.next())->confirmDelete(aEvt);
    }
    else
    {
        // default handling
        UniString aTitle;
        sal_Int32 nLength = aEvent.Rows;
        if (nLength > 1)
        {
            aTitle = SVX_RES(RID_STR_DELETECONFIRM_RECORDS);
            aTitle.SearchAndReplace('#', String::CreateFromInt32(nLength));
        }
        else
            aTitle = SVX_RES(RID_STR_DELETECONFIRM_RECORD);

        SvxDBMsgBox aDlg(getDialogParentWindow(), aTitle,
                         SVX_RES(RID_STR_DELETECONFIRM), WB_YES_NO | WB_DEF_NO, SvxDBMsgBox::Warning);

        return aDlg.Execute() == RET_YES;
    }
}

//------------------------------------------------------------------------------
Reference< ::com::sun::star::frame::XDispatch >
FmXFormController::interceptedQueryDispatch(sal_uInt16 _nId, const ::com::sun::star::util::URL& aURL,
                                            const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags)
                                            throw( RuntimeException )
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    Reference< ::com::sun::star::frame::XDispatch >  xReturn;
    // dispatches handled by ourself
    if (aURL.Complete == FMURL_CONFIRM_DELETION)
        xReturn = (::com::sun::star::frame::XDispatch*)this;

    // dispatches of FormSlot-URLs we have to translate
    if (!xReturn.is() && aURL.Complete.indexOf(FMURL_FORMSLOTS_PREFIX) == 0)
    {   // TODO : use an ::com::sun::star::util::URLTransformer to split this ::com::sun::star::util::URL and do a real (safe) check

        // get the frame we're residing in
        Reference< ::com::sun::star::frame::XModel >  xFrameModel = getXModel(getModel());
        Reference< ::com::sun::star::frame::XController >  xFrameController = xFrameModel.is() ? xFrameModel->getCurrentController() : Reference< ::com::sun::star::frame::XController > ();
        Reference< ::com::sun::star::frame::XFrame >  xFrame = xFrameController.is() ? xFrameController->getFrame() : Reference< ::com::sun::star::frame::XFrame > ();

        if (!xFrame.is())
        {
            // bad timing : somebody requested a dispatch, but the model we belong to is not attached to a controller, yet
            // (or the controller isn't attached to a frame). This sometimes happens if a document is opened read-only.
            // As we have no chance to get the "attached" event for the model/controller/frame, we have no chance
            // to refresh the dispatchers later as response to such an event.
            // So we simply try it asynchron until we have that frame.
            if (!m_nUpdateDispatcherEvent)
                m_nUpdateDispatcherEvent = Application::PostUserEvent(LINK(this, FmXFormController, OnUpdateDispatchers));
        }
        else
        {
            // translate the ::com::sun::star::util::URL : the new ::com::sun::star::util::URL is the old one with a mark : the "path" to our model within it's hierarchy
            DBG_ASSERT(aURL.Mark.getLength() == 0, "FmXFormController::interceptedQueryDispatch : the ::com::sun::star::util::URL shouldn't have a mark !");
            ::rtl::OUString sNewMark;
            ::rtl::OUString sCurrentPath;
            // collect the relative indicies of my model, it's parent, it's parent's parent ..., separate them with a '\'
            Reference< ::com::sun::star::container::XChild >  xChild(getModel(), UNO_QUERY);
            Reference< ::com::sun::star::container::XIndexAccess >  xParent;
            if (xChild.is())
                ::comphelper::query_interface(xChild->getParent(), xParent);


            while (xChild.is() && xParent.is())
            {
                sal_Int32 nPos = getElementPos(xParent, xChild);

                sCurrentPath = ::rtl::OUString::valueOf(nPos);
                if (sNewMark.getLength())
                {
                    sCurrentPath += ::rtl::OUString::createFromAscii("\\");
                    sNewMark = sCurrentPath + sNewMark;
                }
                else
                    sNewMark = sCurrentPath;

                ::comphelper::query_interface((Reference<XInterface>)xParent, xChild);

                if (xChild.is())
                    ::comphelper::query_interface(xChild->getParent(), xParent);

            }

            ::com::sun::star::util::URL aNewUrl = aURL;
            // and prepend our dispatch prefix
            ::rtl::OUString aMark = m_sDispatchPrefix;
            aMark += ::rtl::OUString::createFromAscii("\\");
            aMark += sNewMark;

            // assemble the new url
            Reference< ::com::sun::star::util::XURLTransformer >  xTransformer(m_xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer") ), UNO_QUERY);
            if (xTransformer.is())
            {
                xTransformer->parseStrict(aNewUrl);
                aNewUrl.Mark = aMark;
                xTransformer->assemble(aNewUrl);
                DBG_ASSERT(aNewUrl.Complete.getLength(), "FmXFormController::interceptedQueryDispatch : transformation of the ::com::sun::star::util::URL failed !");
            }
            else
            {
                DBG_ERROR("FmXFormController::interceptedQueryDispatch : could not create a com.sun.star.util.URLTransformer !");
                // do it manually ... the service would probably do the same, but to be sure we gave it a chance before doing it ourself
                aNewUrl.Complete += ::rtl::OUString::createFromAscii("#");
                aNewUrl.Complete += aNewUrl.Mark;
            }

            Reference< ::com::sun::star::frame::XDispatchProvider >  xDispProv(xFrame, UNO_QUERY);
            if (xDispProv.is())
                xReturn = xDispProv->queryDispatch(aNewUrl, aTargetFrameName, nSearchFlags);
        }
    }

    // no more to offer
    return xReturn;
}

//------------------------------------------------------------------------------
Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >  FmXFormController::createInterceptor(const Reference< ::com::sun::star::frame::XDispatchProviderInterception > & _xInterception)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
#if DBG_UTIL
    // check if we already have a interceptor for the given object
    for (   ConstInterceptorsIterator aIter = m_aControlDispatchInterceptors.begin();
            aIter != m_aControlDispatchInterceptors.end();
            ++aIter
        )
    {
        if ((*aIter)->getIntercepted() == _xInterception)
            DBG_ERROR("FmXFormController::createInterceptor : we already do intercept this objects dispatches !");
    }
#endif

    ::rtl::OUString sInterceptorScheme(RTL_CONSTASCII_USTRINGPARAM("*"));
    FmXDispatchInterceptorImpl* pInterceptor = new FmXDispatchInterceptorImpl(_xInterception, this, 0, Sequence< ::rtl::OUString >(&sInterceptorScheme, 1));
    pInterceptor->acquire();
    m_aControlDispatchInterceptors.insert(m_aControlDispatchInterceptors.end(), pInterceptor);

    return (::com::sun::star::frame::XDispatchProviderInterceptor*)pInterceptor;
}

//------------------------------------------------------------------------------
void FmXFormController::deleteInterceptor(const Reference< ::com::sun::star::frame::XDispatchProviderInterception > & _xInterception)
{
    OSL_ENSURE(!FmXFormController_BASE1::rBHelper.bDisposed,"FmXFormController: Object already disposed!");
    // search the interceptor responsible for the given object
    InterceptorsIterator aIter;
    for (   aIter = m_aControlDispatchInterceptors.begin();
            aIter != m_aControlDispatchInterceptors.end();
            ++aIter
        )
    {
        if ((*aIter)->getIntercepted() == _xInterception)
            break;
    }
    if (aIter == m_aControlDispatchInterceptors.end())
    {
        DBG_ERROR("FmXFormController::deleteInterceptor : we're not registered for this object !");
        return;
    }

    // log off the interception from it's interception object
    FmXDispatchInterceptorImpl* pInterceptorImpl = *aIter;
    pInterceptorImpl->dispose();
    pInterceptorImpl->release();

    // remove the interceptor from our array
    m_aControlDispatchInterceptors.erase(aIter);
}




