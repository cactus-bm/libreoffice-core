/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: controlwizard.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:27:13 $
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

#ifndef _EXTENSIONS_DBP_CONTROLWIZARD_HXX
#define _EXTENSIONS_DBP_CONTROLWIZARD_HXX

#ifndef _SVTOOLS_WIZARDMACHINE_HXX_
#include <svtools/wizardmachine.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_EDIT_HXX
#include <vcl/edit.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_COMBOBOX_HXX
#include <vcl/combobox.hxx>
#endif
#ifndef _EXTENSIONS_DBP_DBPTYPES_HXX_
#include "dbptypes.hxx"
#endif
#ifndef _EXTENSIONS_DBP_DBPRESID_HRC_
#include "dbpresid.hrc"
#endif
#ifndef _EXTENSIONS_COMPONENT_MODULE_HXX_
#include "componentmodule.hxx"
#endif
#ifndef _EXTENSIONS_DBP_WIZARDCONTEXT_HXX_
#include "wizardcontext.hxx"
#endif

class ResId;
//.........................................................................
namespace dbp
{
//.........................................................................

    //=====================================================================
    //= OControlWizardSettings
    //=====================================================================
    struct OControlWizardSettings
    {
        String      sControlLabel;
    };

    //=====================================================================
    //= OControlWizardPage
    //=====================================================================
    class OControlWizard;
    typedef ::svt::OWizardPage OControlWizardPage_Base;
    class OControlWizardPage : public OControlWizardPage_Base
    {
    protected:
        FixedLine*      m_pFormSettingsSeparator;
        FixedText*      m_pFormDatasourceLabel;
        FixedText*      m_pFormDatasource;
        FixedText*      m_pFormContentTypeLabel;
        FixedText*      m_pFormContentType;
        FixedText*      m_pFormTableLabel;
        FixedText*      m_pFormTable;

    protected:
        OControlWizard*                 getDialog();
        const OControlWizard*           getDialog() const;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                                        getServiceFactory();
        const OControlWizardContext&    getContext();
        sal_Bool                        updateContext();
        void                            setFormConnection(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConn, sal_Bool _bAutoDispose = sal_True );
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >
                                        getFormConnection() const;

    public:
        OControlWizardPage( OControlWizard* _pParent, const ResId& _rResId );
        ~OControlWizardPage();

    protected:
        void fillListBox(
            ListBox& _rList,
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rItems,
            sal_Bool _bClear = sal_True);
        void fillListBox(
            ComboBox& _rList,
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rItems,
            sal_Bool _bClear = sal_True);

    protected:
        void enableFormDatasourceDisplay();
        void adjustControlForNoDSDisplay(Control* _pControl, sal_Bool bConstLowerDistance = sal_False);

    protected:
        // OWizardPage overridables
        virtual void        initializePage();
    };

    struct OAccessRegulator;
    //=====================================================================
    //= OControlWizard
    //=====================================================================
    typedef ::svt::OWizardMachine OControlWizard_Base;
    class OControlWizard : public OControlWizard_Base
    {
    private:
        OControlWizardContext   m_aContext;

    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                    m_xORB;

    public:
        OControlWizard(
            Window* _pParent,
            const ResId& _rId,
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObjectModel,
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );
        ~OControlWizard();

        // make the some base class methods public
        sal_Bool    travelNext() { return OControlWizard_Base::travelNext(); }

    public:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
            getServiceFactory() const { return m_xORB; }

        const OControlWizardContext&    getContext() const { return m_aContext; }
        sal_Bool                        updateContext(const OAccessRegulator&);
        void                            setFormConnection(const OAccessRegulator&, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConn, sal_Bool _bAutoDispose = sal_True );
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >
                                        getFormConnection(const OAccessRegulator&) const;

        /** returns the com.sun.star.sdb.InteractionHandler
            @param  _pWindow    The window will be used when an error message has to be shown.
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler > getInteractionHandler(Window* _pWindow) const;

    protected:
        // initialize the derivees settings (which have to be derived from OControlWizardSettings)
        // with some common data extracted from the control model
        void initControlSettings(OControlWizardSettings* _pSettings);
        // commit the control-relevant settings
        void commitControlSettings(OControlWizardSettings* _pSettings);

        sal_Bool needDatasourceSelection();

        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >
                                        getFormConnection() const;

        virtual sal_Bool approveControl(sal_Int16 _nClassId) = 0;

        // ModalDialog overridables
        virtual short   Execute();

    private:
        sal_Bool initContext();

        void implGetDSContext();
        void implDetermineForm();
        void implDeterminePage();
        void implDetermineShape();

        // made private. Not to be used by derived (or external) classes
        virtual void ActivatePage();
    };

//.........................................................................
}   // namespace dbp
//.........................................................................

#endif // _EXTENSIONS_DBP_CONTROLWIZARD_HXX

