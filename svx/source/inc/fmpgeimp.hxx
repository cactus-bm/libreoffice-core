/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fmpgeimp.hxx,v $
 * $Revision: 1.19 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _SVX_FMUNOPGE_HXX
#define _SVX_FMUNOPGE_HXX

#include <com/sun/star/sdbc/XDataSource.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/form/XFormComponent.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <tools/list.hxx>
#include <tools/link.hxx>
#include <comphelper/uno3.hxx>

#include "svx/svxdllapi.h"

class SvStream;
class FmFormObj;
class FmFormPage;
class SdrObject;

//FORWARD_DECLARE_INTERFACE(uno,Reference)
FORWARD_DECLARE_INTERFACE(io,XObjectOutputStream)
FORWARD_DECLARE_INTERFACE(io,XObjectInputStream)
FORWARD_DECLARE_INTERFACE(container,XIndexContainer)

class SdrObjList;

DECLARE_LIST(FmObjectList, FmFormObj*)

//==================================================================
// FmFormPageImpl
// lauscht an allen Containern, um festzustellen, wann Objecte
// eingefuegt worden sind und wann diese entfernt wurden
//==================================================================

class SVX_DLLPRIVATE FmFormPageImpl
{
    friend class FmFormPage;
    friend class FmFormObj;
    friend class FmXFormShell;

    ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm>                xCurrentForm;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer>  m_xForms;
    FmFormPage*     pPage;
    Link            m_aFormsCreationHdl;

    sal_Bool        m_bFirstActivation;
    bool            m_bAttemptedFormCreation;

protected:
    FmFormPageImpl(FmFormPage* _pPage);
    FmFormPageImpl(FmFormPage* _pPage, const FmFormPageImpl& rImpl);
    ~FmFormPageImpl();

    void Init();

public:
    //  nur wichtig fuer den DesignMode
    void setCurForm(::com::sun::star::uno::Reference< ::com::sun::star::form::XForm> xForm);
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm> getDefaultForm();

    /** inserts a form component into the form component hierarchy

        If the given component does not yet belong into the form hierarchy, a suitable place for
        it is found, using <member>findFormForDataSource</member>, if possible.

        If no sutiable form is found, a new one is created, and also inserted into the
        hierarchy.
    */
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm> placeInFormComponentHierarchy(
        const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormComponent>& rContent,
        const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource>& rDatabase = ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource>(),
        const ::rtl::OUString& rDBTitle = ::rtl::OUString(),
        const ::rtl::OUString& rCursorSource = ::rtl::OUString(),
        sal_Int32 nCommandType = 0
    );

    // activation handling
    inline  sal_Bool    hasEverBeenActivated( ) const { return !m_bFirstActivation; }
    inline  void        setHasBeenActivated( ) { m_bFirstActivation = sal_False; }

    void        SetFormsCreationHdl( const Link& _rFormsCreationHdl ) { m_aFormsCreationHdl = _rFormsCreationHdl; }
    const Link& GetFormsCreationHdl() const { return m_aFormsCreationHdl; }

protected:
    // lesen und schreiben der Objecte
    void write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& OutStream) const;
    void read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& InStream);

    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer>& getForms( bool _bForceCreate = true );

    void fillList(FmObjectList& rList, const SdrObjList& rObjList, sal_Bool bConnected) const;

    /** finds a form with a given data source signature
        @param rForm
            the form to start the search with. This form, including all possible sub forms,
            will be examined
        @param rDatabase
            the data source which to which the found form must be bound
        @param rCommand
            the desired Command property value of the sought-after form
        @param nCommandType
            the desired CommandType property value of the sought-after form
    */
    ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm> findFormForDataSource(
        const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm>& rForm,
        const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource>& rDatabase,
        const ::rtl::OUString& rCommand,
        sal_Int32 nCommandType
    );

    ::rtl::OUString getDefaultName(
                        sal_Int16 _nClassId,
                        const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm>& _rxControls,
                        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XServiceInfo >& _rxObject
                    ) const;

private:
    /** validates whether <member>xCurrentForm</member> is still valid and to be used

        There are situations where our current form becomes invalid, without us noticing this. Thus,
        every method which accesses <member>xCurrentForm</member> should beforehand validate the current
        form by calling this method.

        If <member>xCurrentForm</member> is not valid anymore, it is reset to <NULL/>.

        @since #i40086#
    */
    void    validateCurForm();

public:

    static UniString getDefaultName(
                        sal_Int16 nClassId,
                        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XServiceInfo >& _rxObject
                    );

    ::rtl::OUString setUniqueName(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormComponent>& xFormComponent, const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm>& xControls);
    ::rtl::OUString getUniqueName(const ::rtl::OUString& rName, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>& xNamedSet) const;


private:
    FmFormPageImpl();                                   // never implemented
    FmFormPageImpl( const FmFormPageImpl& );            // never implemented
    FmFormPageImpl& operator=( const FmFormPageImpl& ); // never implemented
};


#endif // _SVX_FMUNOPGE_HXX

