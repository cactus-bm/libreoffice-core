/*************************************************************************
 *
 *  $RCSfile: propbrw.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: tbe $ $Date: 2001-10-08 12:00:58 $
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
 *  Source License Version 1.1 (the License); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an AS IS basis,
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


#ifndef _BASCTL_PROPBRW_HXX
#include "propbrw.hxx"
#endif

#ifndef _BASCTL_DLGEDOBJ_HXX
#include "dlgedobj.hxx"
#endif

#include "basidesh.hxx"

#ifndef _IDERID_HXX
#include <iderid.hxx>
#endif

#ifndef _BASCTL_DLGRESID_HRC
#include <dlgresid.hrc>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _SFX_BINDINGS_HXX
#include <sfx2/bindings.hxx>
#endif

#ifndef _SFX_CHILDWIN_HXX
#include <sfx2/childwin.hxx>
#endif

#ifndef _SFX_OBJITEM_HXX
#include <sfx2/objitem.hxx>
#endif

#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif

#ifndef _SVDVIEW_HXX //autogen
#include <svx/svdview.hxx>
#endif

#ifndef _SVDOGRP_HXX
#include <svx/svdogrp.hxx>
#endif

#ifndef _SVDPAGE_HXX
#include <svx/svdpage.hxx>
#endif

#ifndef _SVDITER_HXX
#include <svx/svditer.hxx>
#endif

#ifndef _SFXVIEWSH_HXX //autogen
#include <sfx2/viewsh.hxx>
#endif

#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif

#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif

#ifndef _COMPHELPER_COMPOSEDPROPS_HXX_
#include <comphelper/composedprops.hxx>
#endif

#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif


//============================================================================
// PropBrwMgr
//============================================================================

SFX_IMPL_FLOATINGWINDOW(PropBrwMgr, SID_SHOW_BROWSER)

//----------------------------------------------------------------------------

PropBrwMgr::PropBrwMgr( Window *pParent, sal_uInt16 nId,
                        SfxBindings *pBindings, SfxChildWinInfo* pInfo)
              :SfxChildWindow(pParent, nId)
{
    // my UNO representation
    m_xUnoRepresentation = VCLUnoHelper::CreateControlContainer(pParent);

    pWindow = new PropBrw(::comphelper::getProcessServiceFactory(),pBindings, this, pParent);
    eChildAlignment = SFX_ALIGN_NOALIGNMENT;
    ((SfxFloatingWindow*)pWindow)->Initialize( pInfo );

    // set current selection
    SfxViewShell* pShell = SfxViewShell::Current();
    if( pShell )
    {
        if( pShell->IsA( TYPE( BasicIDEShell )) )
            ((PropBrw*)pWindow)->Update( ((BasicIDEShell*)pShell)->GetCurDlgView() );
        else
        {
            SdrView* pDrawView = pShell->GetDrawView();
            if( pDrawView )
                ((PropBrw*)pWindow)->Update( pDrawView );
        }
    }
}

//----------------------------------------------------------------------------

const long STD_WIN_SIZE_X = 300;
const long STD_WIN_SIZE_Y = 350;

const long STD_MIN_SIZE_X = 250;
const long STD_MIN_SIZE_Y = 250;

const long STD_WIN_POS_X = 50;
const long STD_WIN_POS_Y = 50;
const long WIN_BORDER = 2;
const long MIN_WIN_SIZE_X = 50;
const long MIN_WIN_SIZE_Y = 50;

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::comphelper;

//----------------------------------------------------------------------------

//============================================================================
// PropBrw
//============================================================================

DBG_NAME(PropBrw);

//----------------------------------------------------------------------------

PropBrw::PropBrw(const Reference< XMultiServiceFactory >&   _xORB,
                 SfxBindings *pBindings, SfxChildWindow *pMgr, Window* pParent)
          :SfxFloatingWindow(pBindings, pMgr, pParent,WinBits(WB_STDMODELESS|WB_SIZEABLE|WB_3DLOOK|WB_ROLLABLE))
          ,pView( NULL )
          ,m_bInitialStateChange(sal_True)
          ,m_xORB(_xORB)
{
    DBG_CTOR(PropBrw,NULL);

    Size aPropWinSize(STD_WIN_SIZE_X,STD_WIN_SIZE_Y);
    SetMinOutputSizePixel(Size(STD_MIN_SIZE_X,STD_MIN_SIZE_Y));
    SetOutputSizePixel(aPropWinSize);

    try
    {
        // create a frame wrapper for myself
        m_xMeAsFrame = Reference< XFrame >(m_xORB->createInstance(::rtl::OUString::createFromAscii("com.sun.star.frame.Frame")), UNO_QUERY);
        if (m_xMeAsFrame.is())
        {
            m_xMeAsFrame->initialize( VCLUnoHelper::GetInterface ( this ) );
            m_xMeAsFrame->setName(::rtl::OUString::createFromAscii("form property browser"));  // change name!
        }
    }
    catch (Exception&)
    {
        DBG_ERROR("PropBrw::PropBrw: could not create/initialize my frame!");
        m_xMeAsFrame.clear();
    }

    // append our frame
    //Reference < XFramesSupplier > xSup(_xFrame,UNO_QUERY);
    //Reference < XFrames > xFrames = xSup->getFrames();
    //xFrames->append( m_xMeAsFrame );
    // TODO: where to append?

    if (m_xMeAsFrame.is())
    {
        try
        {
            Sequence< Any > aArgs(1);
            aArgs[0] <<= PropertyValue(
                ::rtl::OUString::createFromAscii("ParentWindow"),
                0,
                makeAny(VCLUnoHelper::GetInterface ( this )),
                PropertyState_DIRECT_VALUE
            );
            // create a property browser controller
            static const ::rtl::OUString s_sControllerServiceName = ::rtl::OUString::createFromAscii("com.sun.star.form.PropertyBrowserController");
            m_xBrowserController = Reference< XPropertySet >(
                m_xORB->createInstance(s_sControllerServiceName), UNO_QUERY
            );
            if (!m_xBrowserController.is())
            {
                ShowServiceNotAvailableError(pParent, s_sControllerServiceName, sal_True);
            }
            else
            {
                Reference< XController > xAsXController(m_xBrowserController, UNO_QUERY);
                DBG_ASSERT(xAsXController.is(), "PropBrw::PropBrw: invalid controller object!");
                if (!xAsXController.is())
                {
                    ::comphelper::disposeComponent(m_xBrowserController);
                    m_xBrowserController.clear();
                }
                else
                {
                    xAsXController->attachFrame(m_xMeAsFrame);
                    m_xBrowserComponentWindow = m_xMeAsFrame->getComponentWindow();
                    DBG_ASSERT(m_xBrowserComponentWindow.is(), "PropBrw::PropBrw: attached the controller, but have no component window!");
                }
            }
        }
        catch (Exception&)
        {
            DBG_ERROR("PropBrw::PropBrw: could not create/initialize the browser controller!");
            try
            {
                ::comphelper::disposeComponent(m_xBrowserController);
                ::comphelper::disposeComponent(m_xBrowserComponentWindow);
            }
            catch(Exception&) { }
            m_xBrowserController.clear();
            m_xBrowserComponentWindow.clear();
        }
    }

    Point aPropWinPos = Point( WIN_BORDER, WIN_BORDER );
    aPropWinSize.Width() -= (2*WIN_BORDER);
    aPropWinSize.Height() -= (2*WIN_BORDER);

    if (m_xBrowserComponentWindow.is())
    {
        m_xBrowserComponentWindow->setPosSize(aPropWinPos.X(), aPropWinPos.Y(), aPropWinSize.Width(), aPropWinSize.Height(),
            ::com::sun::star::awt::PosSize::WIDTH | ::com::sun::star::awt::PosSize::HEIGHT |
            ::com::sun::star::awt::PosSize::X | ::com::sun::star::awt::PosSize::Y);
        m_xBrowserComponentWindow->setVisible(sal_True);
    }
}

//----------------------------------------------------------------------------

PropBrw::~PropBrw()
{
    if (m_xBrowserController.is())
        implDetachController();

    DBG_DTOR(PropBrw,NULL);
}

//----------------------------------------------------------------------------

void PropBrw::implDetachController()
{
    implSetNewObject(Reference< XPropertySet >());
    if (m_xMeAsFrame.is())
        m_xMeAsFrame->setComponent(NULL, NULL);
    m_xBrowserController = NULL;
    m_xMeAsFrame = NULL;
}

//----------------------------------------------------------------------------

sal_Bool PropBrw::Close()
{
    implDetachController();

    if( IsRollUp() )
        RollDown();

    sal_Bool bClose = SfxFloatingWindow::Close();

    return bClose;
}

//----------------------------------------------------------------------------

::comphelper::OComposedPropertySet* PropBrw::CreateCompPropSet(const SdrMarkList& _rMarkList)
{
    PropertySetArray aSets;

    sal_uInt32 nMarkCount = _rMarkList.GetMarkCount();

    for(sal_uInt32 i=0;i<nMarkCount;i++)
    {
        SdrObject* pCurrent = _rMarkList.GetMark(i)->GetObj();

        SdrObjListIter* pGroupIterator = NULL;
        if (pCurrent->IsGroupObject())
        {
            pGroupIterator = new SdrObjListIter(*pCurrent->GetSubList());
            pCurrent = pGroupIterator->IsMore() ? pGroupIterator->Next() : NULL;
        }

        while (pCurrent)
        {
            DlgEdObj* pDlgEdObj = PTR_CAST(DlgEdObj, pCurrent);
            if (pDlgEdObj)
            {
                Reference< XPropertySet > xControlModel(pDlgEdObj->GetUnoControlModel(), UNO_QUERY);
                if (xControlModel.is())
                    aSets.push_back(xControlModel);
            }

            // next element
            pCurrent = pGroupIterator && pGroupIterator->IsMore() ? pGroupIterator->Next() : NULL;
        }
        if (pGroupIterator)
            delete pGroupIterator;
    }
    return new ::comphelper::OComposedPropertySet(Sequence< Reference< XPropertySet > >(aSets.begin(), aSets.size()));
}

//----------------------------------------------------------------------------

void PropBrw::implSetNewObject(const Reference< XPropertySet >& _rxObject)
{
    if (m_xBrowserController.is())
    {
        m_xBrowserController->setPropertyValue(
            ::rtl::OUString::createFromAscii("IntrospectedObject"),
            makeAny(_rxObject)
        );

        // set the new title according to the selected object
        SetText( GetHeadlineName(_rxObject) );
    }
}

//----------------------------------------------------------------------------

::rtl::OUString PropBrw::GetHeadlineName( const Reference< XPropertySet >& _rxObject )
{
    ::rtl::OUString aName;
    Reference< lang::XServiceInfo > xServiceInfo( _rxObject, UNO_QUERY );

    if (xServiceInfo.is())    // single selection
    {
        sal_uInt16 nResId = 0;
        aName = ::rtl::OUString(IDEResId(RID_STR_BRWTITLE_PROPERTIES));

        Sequence< ::rtl::OUString > aServiceNames( xServiceInfo->getSupportedServiceNames() );
        DBG_ASSERT( aServiceNames.getLength() == 1 , "PropBrw: aServiceNames.getLength() != 1" );
        ::rtl::OUString const & aServiceName = aServiceNames.getConstArray()[ 0 ];

        if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlDialogModel") ))
        {
            nResId = RID_STR_CLASS_DIALOG;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlButtonModel") ))
        {
            nResId = RID_STR_CLASS_BUTTON;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlRadioButtonModel") ))
        {
            nResId = RID_STR_CLASS_RADIOBUTTON;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlCheckBoxModel") ))
        {
            nResId = RID_STR_CLASS_CHECKBOX;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlListBoxModel") ))
        {
            nResId = RID_STR_CLASS_LISTBOX;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlComboBoxModel") ))
        {
            nResId = RID_STR_CLASS_COMBOBOX;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlGroupBoxModel") ))
        {
            nResId = RID_STR_CLASS_GROUPBOX;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlEditModel") ))
        {
            nResId = RID_STR_CLASS_EDIT;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlFixedTextModel") ))
        {
            nResId = RID_STR_CLASS_FIXEDTEXT;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlImageControlModel") ))
        {
            nResId = RID_STR_CLASS_IMAGECONTROL;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlProgressBarModel") ))
        {
            nResId = RID_STR_CLASS_PROGRESSBAR;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlScrollBarModel") ))
        {
            nResId = RID_STR_CLASS_SCROLLBAR;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlFixedLineModel") ))
        {
            nResId = RID_STR_CLASS_FIXEDLINE;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlDateFieldModel") ))
        {
            nResId = RID_STR_CLASS_DATEFIELD;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlTimeFieldModel") ))
        {
            nResId = RID_STR_CLASS_TIMEFIELD;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlNumericFieldModel") ))
        {
            nResId = RID_STR_CLASS_NUMERICFIELD;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlCurrencyFieldModel") ))
        {
            nResId = RID_STR_CLASS_CURRENCYFIELD;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlFormattedFieldModel") ))
        {
            nResId = RID_STR_CLASS_FORMATTEDFIELD;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlPatternFieldModel") ))
        {
            nResId = RID_STR_CLASS_PATTERNFIELD;
        }
        else if (aServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("com.sun.star.awt.UnoControlFileControlModel") ))
        {
            nResId = RID_STR_CLASS_FILECONTROL;
        }
        else
        {
            nResId = RID_STR_CLASS_CONTROL;
        }

        if (nResId)
        {
            aName += ::rtl::OUString( IDEResId(nResId) );
        }
    }
    else if (!_rxObject.is())    // no properties
    {
        aName = ::rtl::OUString(IDEResId(RID_STR_BRWTITLE_NO_PROPERTIES));
    }
    else    // multiselection
    {
        aName = ::rtl::OUString(IDEResId(RID_STR_BRWTITLE_PROPERTIES));
        aName += ::rtl::OUString(IDEResId(RID_STR_BRWTITLE_MULTISELECT));
    }

    return aName;
}

//----------------------------------------------------------------------------

void PropBrw::FillInfo( SfxChildWinInfo& rInfo ) const
{
    rInfo.bVisible = sal_False;
}

//----------------------------------------------------------------------------

void PropBrw::Resize()
{
    SfxFloatingWindow::Resize();

    // adjust size
    Size  aSize = GetOutputSizePixel();
    Size aPropWinSize( aSize );
    aPropWinSize.Width() -= (2*WIN_BORDER);
    aPropWinSize.Height() -= (2*WIN_BORDER);

    if (m_xBrowserComponentWindow.is())
    {
        m_xBrowserComponentWindow->setPosSize(0, 0, aPropWinSize.Width(), aPropWinSize.Height(),
            ::com::sun::star::awt::PosSize::WIDTH | ::com::sun::star::awt::PosSize::HEIGHT);
    }
}

//----------------------------------------------------------------------------

void PropBrw::Update( SdrView* pNewView )
{

    try
    {
        if( pView )
        {
            EndListening( *(pView->GetModel()) );
            pView = NULL;
        }

        if( !pNewView )
            return;
        else
            pView = pNewView;

        // set focus on initialization
        if (m_bInitialStateChange)
        {
            if (m_xBrowserComponentWindow.is())
                m_xBrowserComponentWindow->setFocus();
            m_bInitialStateChange = sal_False;
        }

        DlgEdObj* pDlgEdObj = NULL;
        const SdrMarkList& rMarkList = pView->GetMarkList();
        sal_uInt32 nMarkCount = rMarkList.GetMarkCount();

        if (nMarkCount==1)
        {
            SdrObject *pObj=rMarkList.GetMark(0)->GetObj();

            if( pObj->IsGroupObject() ) // group object
            {
                implSetNewObject( Reference< XPropertySet> (CreateCompPropSet( rMarkList )) );
            }
            else
            {
                pDlgEdObj = PTR_CAST(DlgEdObj, rMarkList.GetMark(0)->GetObj());

                if ( pDlgEdObj ) // single selection
                {
                    implSetNewObject(Reference< XPropertySet >(pDlgEdObj->GetUnoControlModel(), UNO_QUERY));
                }
                else
                    implSetNewObject(Reference< XPropertySet >());
            }
        }
        else if (nMarkCount > 1) // multiple selection
        {
            implSetNewObject( Reference< XPropertySet> (CreateCompPropSet( rMarkList )) );
        }
        else
        {
            EndListening( *(pView->GetModel()) );
            pView = NULL;
            implSetNewObject(Reference< XPropertySet >());
            return;
        }

        StartListening( *(pView->GetModel()) );
    }
    catch (Exception&)
    {
        DBG_ERROR("PropBrw::Update: Exception occured!");
    }
}

//----------------------------------------------------------------------------

