/*************************************************************************
 *
 *  $RCSfile: dlgedobj.hxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: tbe $ $Date: 2002-04-24 14:48:23 $
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

#ifndef _BASCTL_DLGEDOBJ_HXX
#define _BASCTL_DLGEDOBJ_HXX

#ifndef _SVDOUNO_HXX
#include <svx/svdouno.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERLISTENER_HPP_
#include <com/sun/star/container/XContainerListener.hpp>
#endif

#include <vector>

class DlgEdForm;

//============================================================================
// DlgEdObj
//============================================================================

class DlgEdObj: public SdrUnoObj
{
    friend class DlgEditor;
    friend class DlgEdFactory;
    friend class DlgEdPropListenerImpl;
    friend class DlgEdForm;

private:
    sal_Bool        bIsListening;
    DlgEdForm*      pDlgEdForm;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener> m_xPropertyChangeListener;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener> m_xContainerListener;

protected:
    DlgEdObj();
    DlgEdObj(const ::rtl::OUString& rModelName);
    DlgEdObj(const ::rtl::OUString& rModelName,
             const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rxSFac);

    virtual void     WriteData(SvStream& rOut) const;                           // not working yet
    virtual void     ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);      // not working yet

    virtual void NbcMove( const Size& rSize );
    virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
    virtual FASTBOOL EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd);

    DECL_LINK(OnCreate, void* );

    void StartListening();
    void EndListening(sal_Bool bRemoveListener = sal_True);
    sal_Bool    isListening() const { return bIsListening; }

public:
    TYPEINFO();

    virtual ~DlgEdObj();
    virtual void SetPage(SdrPage* pNewPage);

    virtual void SetDlgEdForm( DlgEdForm* pForm ) { pDlgEdForm = pForm; }
    virtual DlgEdForm* GetDlgEdForm() const { return pDlgEdForm; }

    virtual sal_uInt32 GetObjInventor() const;
    virtual sal_uInt16 GetObjIdentifier() const;

    virtual SdrObject*  Clone() const;                                          // not working yet
    virtual SdrObject*  Clone(SdrPage* pPage, SdrModel* pModel) const;          // not working yet
    virtual void        operator= (const SdrObject& rObj);                      // not working yet
    virtual void clonedFrom(const DlgEdObj* _pSource);                          // not working yet

    virtual ::rtl::OUString GetServiceName() const;
    virtual ::rtl::OUString GetDefaultName() const;
    virtual ::rtl::OUString GetUniqueName() const;

    virtual sal_Int32   GetStep() const;
    virtual void        UpdateStep();

    virtual void SetDefaults();
    virtual void SetRectFromProps();
    virtual void SetPropsFromRect();

    virtual void SAL_CALL NameChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL TabIndexChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException);

    // PropertyChangeListener
    virtual void SAL_CALL _propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw(::com::sun::star::uno::RuntimeException);

    // ContainerListener
    virtual void SAL_CALL _elementInserted( const ::com::sun::star::container::ContainerEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL _elementReplaced( const ::com::sun::star::container::ContainerEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL _elementRemoved( const ::com::sun::star::container::ContainerEvent& Event ) throw(::com::sun::star::uno::RuntimeException);

    virtual SdrObject* CheckHit(const Point& rPnt,USHORT nTol,const SetOfByte*) const;
};


//============================================================================
// DlgEdForm
//============================================================================

class DlgEdForm: public DlgEdObj
{
    friend class DlgEditor;
    friend class DlgEdFactory;

private:
    DlgEditor* pDlgEditor;
    ::std::vector<DlgEdObj*> pChilds;

protected:
    DlgEdForm(const ::rtl::OUString& rModelName);
    DlgEdForm(const ::rtl::OUString& rModelName,
              const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rxSFac);
    DlgEdForm();

    virtual void NbcMove( const Size& rSize );
    virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
    virtual FASTBOOL EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd);

public:
    TYPEINFO();

    virtual ~DlgEdForm();

    virtual void SetDlgEditor( DlgEditor* pEditor ) { pDlgEditor = pEditor; }
    virtual DlgEditor* GetDlgEditor() const { return pDlgEditor; }

    virtual void AddChild( DlgEdObj* pDlgEdObj );
    virtual void RemoveChild( DlgEdObj* pDlgEdObj );
    virtual ::std::vector<DlgEdObj*> GetChilds() const { return pChilds; }

    virtual void UpdateStep();

    virtual void SetRectFromProps();
    virtual void SetPropsFromRect();

    virtual void SortByTabIndex();

    virtual SdrObject* CheckHit(const Point& rPnt,USHORT nTol,const SetOfByte*) const;
};

#endif // _BASCTL_DLGEDOBJ_HXX

