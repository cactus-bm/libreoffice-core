/*************************************************************************
 *
 *  $RCSfile: newhelp.hxx,v $
 *
 *  $Revision: 1.35 $
 *
 *  last change: $Author: pb $ $Date: 2001-10-29 07:36:38 $
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
#ifndef INCLUDED_SFX_NEWHELP_HXX
#define INCLUDED_SFX_NEWHELP_HXX

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTATUSLISTENER_HPP_
#include <com/sun/star/frame/XStatusListener.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif

namespace com { namespace sun { namespace star { namespace frame { class XFrame; } } } };
namespace com { namespace sun { namespace star { namespace awt { class XWindow; } } } };

#include <vcl/window.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/tabpage.hxx>
#include <vcl/splitwin.hxx>
#include <vcl/tabctrl.hxx>
#include <vcl/combobox.hxx>
#include <vcl/fixed.hxx>
#include <vcl/button.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/dialog.hxx>
#include <svtools/svtreebx.hxx>

// class OpenStatusListener_Impl -----------------------------------------

class OpenStatusListener_Impl : public ::cppu::WeakImplHelper1< ::com::sun::star::frame::XStatusListener >
{
private:
    sal_Bool    m_bFinished;
    sal_Bool    m_bSuccess;
    String      m_aURL;
    Link        m_aOpenLink;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >
                m_xDispatch;

public:
    OpenStatusListener_Impl() : m_bFinished( FALSE ), m_bSuccess( FALSE ) {}

    virtual void SAL_CALL   statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL   disposing( const ::com::sun::star::lang::EventObject& Source ) throw(::com::sun::star::uno::RuntimeException);

    void                    AddListener( ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >& xDispatch,
                                         const ::com::sun::star::util::URL& aURL );

    inline sal_Bool         IsFinished() const { return m_bFinished; }
    inline sal_Bool         IsSuccessful() const { return m_bSuccess; }
    inline const String&    GetURL() const { return m_aURL; }
    inline void             SetOpenHdl( const Link& rLink ) { m_aOpenLink = rLink; }
};

// ContentListBox_Impl ---------------------------------------------------

class ContentListBox_Impl : public SvTreeListBox
{
private:
    Image           aOpenBookImage;
    Image           aClosedBookImage;
    Image           aOpenChapterImage;
    Image           aClosedChapterImage;
    Image           aDocumentImage;

    void            InitRoot();
    void            ClearChildren( SvLBoxEntry* pParent );

public:
    ContentListBox_Impl( Window* pParent, const ResId& rResId );
    ~ContentListBox_Impl();


    virtual void    RequestingChilds( SvLBoxEntry* pParent );
    virtual long    Notify( NotifyEvent& rNEvt );

    void            SetOpenHdl( const Link& rLink ) { SetDoubleClickHdl( rLink ); }
    String          GetSelectEntry() const;
};

// class ContentTabPage_Impl ---------------------------------------------

class ContentTabPage_Impl : public TabPage
{
private:
    ContentListBox_Impl aContentBox;

public:
    ContentTabPage_Impl( Window* pParent );

    virtual void    Resize();

    void            SetOpenHdl( const Link& rLink ) { aContentBox.SetOpenHdl( rLink ); }
    String          GetSelectEntry() const { return aContentBox.GetSelectEntry(); }
    void            SetFocusOnBox() { aContentBox.GrabFocus(); }

    virtual void    ActivatePage();
};

// class IndexTabPage_Impl -----------------------------------------------

class IndexBox_Impl : public ComboBox
{
public:
    IndexBox_Impl( Window* pParent, const ResId& rResId );

    virtual void        UserDraw( const UserDrawEvent& rUDEvt );
    virtual long        Notify( NotifyEvent& rNEvt );

    void                SelectExecutableEntry();
};

class IndexTabPage_Impl : public TabPage
{
private:
    FixedText           aExpressionFT;
    IndexBox_Impl       aIndexCB;
    PushButton          aOpenBtn;

    Timer               aFactoryTimer;
    Timer               aKeywordTimer;
    Link                aKeywordLink;

    String              sFactory;
    String              sKeyword;

    long                nMinWidth;
    sal_Bool            bIsActivated;

    void                InitializeIndex();
    void                ClearIndex();

    DECL_LINK(          OpenHdl, PushButton* );
    DECL_LINK(          TimeoutHdl, Timer* );

public:
    IndexTabPage_Impl( Window* pParent );
    ~IndexTabPage_Impl();

    virtual void        Resize();
    virtual void        ActivatePage();

    void                SetDoubleClickHdl( const Link& rLink );
    void                SetFactory( const String& rFactory );
    String              GetFactory() const { return sFactory; }
    String              GetSelectEntry() const;
    void                SetFocusOnBox() { aIndexCB.GrabFocus(); }
    sal_Bool            HasFocusOnEdit() const { return aIndexCB.HasChildPathFocus(); }

    void                SetKeywordHdl( const Link& rLink ) { aKeywordLink = rLink; }
    void                SetKeyword( const String& rKeyword );
    sal_Bool            HasKeyword() const;
    void                OpenKeyword();
};

// class SearchTabPage_Impl ----------------------------------------------

class SearchBox_Impl : public ComboBox
{
private:
    Link                aSearchLink;

public:
    SearchBox_Impl( Window* pParent, const ResId& rResId ) :
        ComboBox( pParent, rResId ) { SetDropDownLineCount( 5 ); }

    virtual long        PreNotify( NotifyEvent& rNEvt );
    virtual void        Select();

    void                SetSearchLink( const Link& rLink ) { aSearchLink = rLink; }
};

class SearchResultsBox_Impl : public ListBox
{
public:
    SearchResultsBox_Impl( Window* pParent, const ResId& rResId ) : ListBox( pParent, rResId ) {}

    virtual long    Notify( NotifyEvent& rNEvt );
};

class SearchTabPage_Impl : public TabPage
{
private:
    FixedText               aSearchFT;
    SearchBox_Impl          aSearchED;
    PushButton              aSearchBtn;
    CheckBox                aFullWordsCB;
    CheckBox                aScopeCB;
    SearchResultsBox_Impl   aResultsLB;
    PushButton              aOpenBtn;

    Size                    aMinSize;
    String                  aFactory;

    void                ClearSearchResults();
    void                RememberSearchText( const String& rSearchText );

    DECL_LINK(          SearchHdl, PushButton* );
    DECL_LINK(          OpenHdl, PushButton* );

public:
    SearchTabPage_Impl( Window* pParent );
    ~SearchTabPage_Impl();

    virtual void        Resize();

    void                SetDoubleClickHdl( const Link& rLink );
    void                SetFactory( const String& rFactory ) { aFactory = rFactory; }
    String              GetSelectEntry() const;
    void                ClearPage();
    void                SetFocusOnBox() { aResultsLB.GrabFocus(); }
    sal_Bool            HasFocusOnEdit() const { return aSearchED.HasChildPathFocus(); }
    virtual void        ActivatePage();
    String              GetSearchText() const { return aSearchED.GetText(); }
    sal_Bool            OpenKeyword( const String& rKeyword );
};

// class BookmarksTabPage_Impl -------------------------------------------

class BookmarksBox_Impl : public ListBox
{
private:
    void                DoAction( USHORT nAction );

public:
    BookmarksBox_Impl( Window* pParent, const ResId& rResId );
    ~BookmarksBox_Impl();

    virtual long        Notify( NotifyEvent& rNEvt );
};

class BookmarksTabPage_Impl : public TabPage
{
private:
    FixedText           aBookmarksFT;
    BookmarksBox_Impl   aBookmarksBox;
    PushButton          aBookmarksPB;

    long                nMinWidth;

    DECL_LINK(          OpenHdl, PushButton* );

public:
    BookmarksTabPage_Impl( Window* pParent );

    virtual void        Resize();

    void                SetDoubleClickHdl( const Link& rLink );
    String              GetSelectEntry() const;
    void                AddBookmarks( const String& rTitle, const String& rURL );
    void                SetFocusOnBox() { aBookmarksBox.GrabFocus(); }
    virtual void        ActivatePage();
};

// class SfxHelpIndexWindow_Impl -----------------------------------------

class SfxHelpWindow_Impl;

class SfxHelpIndexWindow_Impl : public Window
{
private:
    ListBox             aActiveLB;
    FixedLine           aActiveLine;

    TabControl          aTabCtrl;
    Timer               aInitTimer;

    Link                aSelectFactoryLink;
    String              sKeyword;

    SfxHelpWindow_Impl*     pParentWin;

    ContentTabPage_Impl*    pCPage;
    IndexTabPage_Impl*      pIPage;
    SearchTabPage_Impl*     pSPage;
    BookmarksTabPage_Impl*  pBPage;

    long                nMinWidth;

    void                Initialize();
    void                SetActiveFactory();

    DECL_LINK(          ActivatePageHdl, TabControl* );
    DECL_LINK(          SelectHdl, ListBox* );
    DECL_LINK(          InitHdl, Timer* );
    DECL_LINK(          KeywordHdl, IndexTabPage_Impl* );

public:
    SfxHelpIndexWindow_Impl( SfxHelpWindow_Impl* pParent );
    ~SfxHelpIndexWindow_Impl();

    virtual void        Resize();

    void                SetDoubleClickHdl( const Link& rLink );
    void                SetSelectFactoryHdl( const Link& rLink ) { aSelectFactoryLink = rLink; }
    void                SetFactory( const String& rFactory, sal_Bool bActive );
    String              GetFactory() const { return pIPage->GetFactory(); }
    String              GetSelectEntry() const;
    void                AddBookmarks( const String& rTitle, const String& rURL );
    String              GetActiveFactoryTitle() const { return aActiveLB.GetSelectEntry(); }
    void                UpdateTabControl() { aTabCtrl.Invalidate(); }
    void                ClearSearchPage();
    void                GrabFocusBack();
    sal_Bool            HasFocusOnEdit() const;
    String              GetSearchText() const;
    void                OpenKeyword( const String& rKeyword );
};

// class TextWin_Impl ----------------------------------------------------

class TextWin_Impl : public DockingWindow
{
public:
                            TextWin_Impl( Window* pParent );
    virtual                 ~TextWin_Impl();

    virtual long            Notify( NotifyEvent& rNEvt );
};

// class SfxHelpTextWindow_Impl ------------------------------------------

class SfxHelpWindow_Impl;
class SfxHelpTextWindow_Impl : public Window
{
private:
    ToolBox                 aToolBox;
    Timer                   aSelectTimer;

    SfxHelpWindow_Impl*     pHelpWin;
    Window*                 pTextWin;
    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XFrame >
                            xFrame;
    sal_Bool                bIsDebug;
    sal_Bool                bIsIndexOn;
    sal_Bool                bIsInClose;

    String                  aIndexOnText;
    String                  aIndexOffText;
    String                  aSearchText;
    Image                   aIndexOnImage;
    Image                   aIndexOffImage;

    sal_Bool                HasSelection() const;

    DECL_LINK(              SelectHdl, Timer* );

public:
    SfxHelpTextWindow_Impl( SfxHelpWindow_Impl* pParent );
    ~SfxHelpTextWindow_Impl();

    virtual void            Resize();
    virtual long            PreNotify( NotifyEvent& rNEvt );

    void                    SetSelectHdl( const Link& rLink ) { aToolBox.SetSelectHdl( rLink ); }

    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XFrame >
                            getFrame() const { return xFrame; }
    void                    ToggleIndex( sal_Bool bOn );
    void                    SelectSearchText( const String& rSearchText );

    virtual void            GetFocus();

    inline ToolBox&         GetToolBox() { return aToolBox; }
};

// class SfxHelpWindow_Impl ----------------------------------------------

class HelpInterceptor_Impl;
class HelpListener_Impl;
class SfxHelpWindow_Impl : public SplitWindow
{
private:
friend class SfxHelpIndexWindow_Impl;

    ::com::sun::star::uno::Reference < ::com::sun::star::awt::XWindow >
                                xWindow;
    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XStatusListener >
                                xOpenListener;

    SfxHelpIndexWindow_Impl*    pIndexWin;
    SfxHelpTextWindow_Impl*     pTextWin;
    HelpInterceptor_Impl*       pHelpInterceptor;
    HelpListener_Impl*          pHelpListener;

    sal_Int32           nExpandWidth;
    sal_Int32           nCollapseWidth;
    sal_Int32           nHeight;
    long                nIndexSize;
    long                nTextSize;
    sal_Bool            bIndex;

    String              sTitle;
    String              sKeyword;

    virtual void        Resize();
    virtual void        Split();

    void                MakeLayout();
    void                InitSizes();
    void                LoadConfig();
    void                SaveConfig();
    void                ShowStartPage();

    DECL_LINK(          SelectHdl, ToolBox* );
    DECL_LINK(          OpenHdl, SfxHelpIndexWindow_Impl* );
    DECL_LINK(          SelectFactoryHdl, SfxHelpIndexWindow_Impl* );
    DECL_LINK(          ChangeHdl, HelpListener_Impl* );
    DECL_LINK(          OpenDoneHdl, OpenStatusListener_Impl* );

public:
    SfxHelpWindow_Impl( const ::com::sun::star::uno::Reference < ::com::sun::star::frame::XFrame >& rFrame,
                        Window* pParent, WinBits nBits );
    ~SfxHelpWindow_Impl();

    virtual long        PreNotify( NotifyEvent& rNEvt );

    void                setContainerWindow(
                            ::com::sun::star::uno::Reference < ::com::sun::star::awt::XWindow > xWin );
    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XFrame >
                        getTextFrame() const { return pTextWin->getFrame(); }

    void                SetFactory( const String& rFactory );
    void                SetHelpURL( const String& rURL );
    void                DoAction( USHORT nActionId );

    void                UpdateToolbox();
    void                AddURLListener( const ::com::sun::star::util::URL& aURL,
                                        ::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatch > xDisp );
    void                OpenKeyword( const String& rKeyword ) { pIndexWin->OpenKeyword( rKeyword ); }
    String              GetFactory() const { return pIndexWin->GetFactory(); }

    sal_Bool            HasHistoryPredecessor() const;      // forward to interceptor
    sal_Bool            HasHistorySuccessor() const;        // forward to interceptor
};

class SfxAddHelpBookmarkDialog_Impl : public ModalDialog
{
private:
    FixedText       aTitleFT;
    Edit            aTitleED;
    OKButton        aOKBtn;
    CancelButton    aEscBtn;
    HelpButton      aHelpBtn;

public:
    SfxAddHelpBookmarkDialog_Impl( Window* pParent, sal_Bool bRename = sal_True );
    ~SfxAddHelpBookmarkDialog_Impl();

    void            SetTitle( const String& rTitle );
    String          GetTitle() const { return aTitleED.GetText(); }
};

#endif // #ifndef INCLUDED_SFX_NEWHELP_HXX

