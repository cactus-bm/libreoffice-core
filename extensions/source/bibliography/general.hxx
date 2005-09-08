/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: general.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 19:18:36 $
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

#ifndef _BIB_GENERAL_HXX
#define _BIB_GENERAL_HXX

#ifndef _COM_SUN_STAR_AWT_XFOCUSLISTENER_HPP_
#include <com/sun/star/awt/XFocusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLCONTAINER_HPP_
#include <com/sun/star/awt/XControlContainer.hpp>
#endif
//#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
//#include <com/sun/star/lang/XMultiServiceFactory.hpp>
//#endif
#ifndef _COM_SUN_STAR_FORM_XBOUNDCOMPONENT_HPP_
#include <com/sun/star/form/XBoundComponent.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROWSETLISTENER_HPP_
#include <com/sun/star/sdbc/XRowSetListener.hpp>
#endif

#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef _SVEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif

#ifndef _SV_TABPAGE_HXX //autogen
#include <vcl/tabpage.hxx>
#endif

#ifndef _SV_COMBOBOX_HXX //
#include <vcl/combobox.hxx>
#endif
#ifndef _SV_SCRBAR_HXX
#include <vcl/scrbar.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx> // helper for implementations
#endif
#ifndef _BIBSHORTCUTHANDLER_HXX
#include "bibshortcuthandler.hxx"
#endif


class BibDataManager;
#define TYPE_COUNT 22
#define FIELD_COUNT 31

typedef cppu::WeakAggImplHelper1 < ::com::sun::star::awt::XFocusListener > BibGeneralPageBaseClass;

class BibGeneralPage: public BibGeneralPageBaseClass, public BibTabPage
{
    Window              aControlParentWin;
    FixedText           aIdentifierFT;
    FixedText           aAuthTypeFT;
    FixedText           aYearFT;

    FixedText           aAuthorFT;
    FixedText           aTitleFT;

    FixedText           aPublisherFT;
    FixedText           aAddressFT;
    FixedText           aISBNFT;

    FixedText           aChapterFT;
    FixedText           aPagesFT;
    FixedLine           aFirstFL;

    FixedText           aEditorFT;
    FixedText           aEditionFT;

    FixedText           aBooktitleFT;
    FixedText           aVolumeFT;
    FixedText           aHowpublishedFT;

    FixedText           aOrganizationsFT;
    FixedText           aInstitutionFT;
    FixedText           aSchoolFT;

    FixedText           aReportTypeFT;
    FixedText           aMonthFT;
    FixedLine           aSecondFL;

    FixedText           aJournalFT;
    FixedText           aNumberFT;
    FixedText           aSeriesFT;

    FixedText           aAnnoteFT;
    FixedText           aNoteFT;
    FixedText           aURLFT;
    FixedLine           aThirdFL;

    FixedText           aCustom1FT;
    FixedText           aCustom2FT;
    FixedText           aCustom3FT;
    FixedText           aCustom4FT;
    FixedText           aCustom5FT;

    ScrollBar           aHoriScroll;
    ScrollBar           aVertScroll;

    FixedText*          aFixedTexts[ FIELD_COUNT ];
    sal_Int16           nFT2CtrlMap[ FIELD_COUNT ];

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >
                        aControls[ FIELD_COUNT ];

    Size                aStdSize;
    Point               aBasePos;

    String              aBibTypeArr[ TYPE_COUNT ];
    String              sErrorPrefix;
    String              sTableErrorString;

    String              sTypeColumnName;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >
                        xCtrlContnr;

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                        xMgr;

    ::com::sun::star::uno::Reference< ::com::sun::star::form::XBoundComponent >
                        xCurrentBoundComponent;

    ::com::sun::star::uno::Reference< ::com::sun::star::form::XBoundComponent >
                        xLBModel;

    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener >
                        xPosListener;


    BibDataManager*     pDatMan;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >
                                AddXControl( const String& rName, Point aPos, Size aSize, sal_uInt16 nHelpId,
                                            sal_Int16& rIndex );

    void                        AddControlWithError( const rtl::OUString& rColumnName, const Point& rPos,
                                            const Size& rSize, String& rErrorString, String aColumnUIName,
                                            sal_uInt16 nHelpId, sal_uInt16 nIndexInFTArray );

    void                        AdjustScrollbars();

    DECL_LINK( ScrollHdl, ScrollBar* );

protected:
    virtual void                Resize();
    void                        InitFixedTexts( void );     // create mnemonics and set text an all fixed texts

public:
                                BibGeneralPage(Window* pParent, BibDataManager* pDatMan);
    virtual                     ~BibGeneralPage();

    inline const String&        GetErrorString() const;

    inline const ::com::sun::star::uno::Reference< ::com::sun::star::form::XBoundComponent >&
                                GetTypeListBoxModel() const;
    inline const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >&
                                GetControlContainer() const;

    inline BibDataManager*      GetDataManager();

    void                        CommitActiveControl();

    virtual void SAL_CALL       disposing( const ::com::sun::star::lang::EventObject& Source ) throw( com::sun::star::uno::RuntimeException );

    void SAL_CALL               focusGained( const ::com::sun::star::awt::FocusEvent& e ) throw( com::sun::star::uno::RuntimeException );
    void SAL_CALL               focusLost( const ::com::sun::star::awt::FocusEvent& e ) throw( com::sun::star::uno::RuntimeException );

    void                        RemoveListeners();

    virtual void                GetFocus();

    virtual BOOL                HandleShortCutKey( const KeyEvent& rKeyEvent ); // returns true, if key was handled
};


inline const String& BibGeneralPage::GetErrorString() const
{
    return sTableErrorString;
}

inline const ::com::sun::star::uno::Reference< ::com::sun::star::form::XBoundComponent >&
    BibGeneralPage::GetTypeListBoxModel() const
{
    return xLBModel;
}

inline const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >&
    BibGeneralPage::GetControlContainer() const
{
    return xCtrlContnr;
}

inline BibDataManager* BibGeneralPage::GetDataManager()
{
    return pDatMan;
}

#endif


