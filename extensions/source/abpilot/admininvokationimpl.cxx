/*************************************************************************
 *
 *  $RCSfile: admininvokationimpl.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hjs $ $Date: 2004-06-28 17:10:49 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef EXTENSIONS_ABP_ADMININVOKATIONIMPL_HXX
#include "admininvokationimpl.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_UI_DIALOGS_XEXECUTABLEDIALOG_HPP_
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif
#ifndef EXTENSIONS_ABPRESID_HRC
#include "abpresid.hrc"
#endif
#ifndef _EXTENSIONS_COMPONENT_MODULE_HXX_
#include "componentmodule.hxx"
#endif
#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif


//.........................................................................
namespace abp
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::ui::dialogs;

    //=====================================================================
    //= OAdminDialogInvokation
    //=====================================================================
    //---------------------------------------------------------------------
    OAdminDialogInvokation::OAdminDialogInvokation(const Reference< XMultiServiceFactory >& _rxORB, const ::rtl::OUString& _rPreferredName, Window* _pMessageParent)
        :m_xORB(_rxORB)
        ,m_sPreferredName(_rPreferredName)
        ,m_pMessageParent(_pMessageParent)
    {
        DBG_ASSERT(m_xORB.is(), "OAdminDialogInvokation::OAdminDialogInvokation: invalid service factory!");
        DBG_ASSERT(m_sPreferredName.getLength(), "OAdminDialogInvokation::OAdminDialogInvokation: invalid preferred name!");
        DBG_ASSERT(m_pMessageParent, "OAdminDialogInvokation::OAdminDialogInvokation: invalid message parent!");
    }

    //---------------------------------------------------------------------
    sal_Bool OAdminDialogInvokation::invokeAdministration( sal_Bool _bFixedType )
    {
        if (!m_xORB.is())
            return sal_False;

        try
        {
            // the service name of the administration dialog
            static ::rtl::OUString s_sAdministrationServiceName = ::rtl::OUString::createFromAscii("com.sun.star.sdb.DatasourceAdministrationDialog");

            // the parameters for the call
            Sequence< Any > aArguments(4);
            Any* pArguments = aArguments.getArray();

            // the parent window
            Reference< XWindow > xDialogParent = VCLUnoHelper::GetInterface(m_pMessageParent);
            *pArguments++ <<= PropertyValue(::rtl::OUString::createFromAscii("ParentWindow"), -1, makeAny(xDialogParent), PropertyState_DIRECT_VALUE);

            // the mode indicating that we want allow creation of one single data source only
            ::rtl::OUString sDialogMode = ::rtl::OUString::createFromAscii( _bFixedType ? "SingleEditFixedType" : "SingleEdit" );
            *pArguments++ <<= PropertyValue(::rtl::OUString::createFromAscii("Mode"), -1, makeAny( sDialogMode ), PropertyState_DIRECT_VALUE);

            // the title of the dialog
            String sAdminDialogTitle(ModuleRes(RID_STR_ADMINDIALOGTITLE));
            *pArguments++ <<= PropertyValue(::rtl::OUString::createFromAscii("Title"), -1, makeAny(::rtl::OUString(sAdminDialogTitle)), PropertyState_DIRECT_VALUE);

            // the name of the new data source
            *pArguments++ <<= PropertyValue(::rtl::OUString::createFromAscii("InitialSelection"), -1, makeAny(m_sPreferredName), PropertyState_DIRECT_VALUE);

            // create the dialog
            Reference< XExecutableDialog > xDialog;
            {
                // creating the dialog service is potentially expensive (if all the libraries invoked need to be loaded)
                // so we display a wait cursor
                WaitObject aWaitCursor(m_pMessageParent);
                xDialog = Reference< XExecutableDialog >( m_xORB->createInstanceWithArguments( s_sAdministrationServiceName , aArguments ), UNO_QUERY );

                // just for a smoother UI: What the dialog does upon execution, is (amongst other things) creating
                // the DriverManager service
                // If this context has never been accessed before, this may be expensive (it includes loading of
                // at least one library).
                // As this wizard is intended to run on the first office start, it is very likely that the
                // context needs to be freshly created
                // Thus, we access the context here (within the WaitCursor), which means the user sees a waitcursor
                // while his/her office blocks a few seconds ....
                m_xORB->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.sdbc.DriverManager" ) );
            }

            if (xDialog.is())
            {   // execute it
                if (xDialog->execute())
                    return sal_True;
            }
            else
                ShowServiceNotAvailableError(m_pMessageParent, s_sAdministrationServiceName, sal_True);
        }
        catch(const Exception&)
        {
            DBG_ERROR("OAdminDialogInvokation::invokeAdministration: caught an exception while executing the dialog!");
        }
        return sal_False;
    }

//.........................................................................
}   // namespace abp
//.........................................................................

