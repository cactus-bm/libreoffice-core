/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dp_gui.h,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: ihi $ $Date: 2006-12-20 17:47:57 $
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

#if ! defined INCLUDED_DP_GUI_H
#define INCLUDED_DP_GUI_H

#include "dp_misc.h"
#include "dp_gui_cmdenv.h"
#include "dp_gui_modifiablecontext.hxx"
#include "dp_gui_updatability.hxx"
#include "dp_gui.hrc"
#include "rtl/ref.hxx"
#include "osl/thread.hxx"
#include "cppuhelper/implbase2.hxx"
#include "vcl/svapp.hxx"
#include "vcl/dialog.hxx"
#include "vcl/button.hxx"
#include "vcl/fixed.hxx"
#include "salhelper/simplereferenceobject.hxx"
#include "svtools/svtabbx.hxx"
#include "svtools/headbar.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/awt/XWindow.hpp"
#include "com/sun/star/ucb/XContentEventListener.hpp"
#include "com/sun/star/deployment/thePackageManagerFactory.hpp"
#include "com/sun/star/deployment/ui/PackageManagerDialog.hpp"
#include "com/sun/star/frame/XDesktop.hpp"
#include <list>
#include <memory>

namespace dp_gui {

enum PackageState { REGISTERED, NOT_REGISTERED, AMBIGUOUS, NOT_AVAILABLE };

PackageState getPackageState(
    ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> const & xPackage,
    ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment> const & xCmdEnv =
    ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment>() );

//==============================================================================
struct DialogImpl :
        public ModelessDialog,
        public ::cppu::WeakImplHelper2< ::com::sun::star::frame::XTerminateListener,
                                        ::com::sun::star::ucb::XContentEventListener >
{
    static ResId getResId( USHORT id );
    static String getResourceString( USHORT id );

    struct SelectionBoxControl : public Control
    {
        SelectionBoxControl( DialogImpl * dialog )
            : Control( dialog, WB_BORDER | WB_TABSTOP ),
              m_dialog(dialog)
            {}
        long Notify( NotifyEvent & rEvt );

    private:
        DialogImpl * m_dialog;
    };

    struct TreeListBoxImpl : public SvHeaderTabListBox
    {
        typedef ::std::list<
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> > t_nodeList;
        t_nodeList m_nodes;
        SvLBoxEntry * addNode(
            SvLBoxEntry * parentNode,
            String const & displayName,
            ::rtl::OUString const & factoryURL,
            ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager>
            const & xPackageManager,
            ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> const & xPackage,
            ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment> const & xCmdEnv,
            bool sortIn = true );
        SvLBoxEntry * addPackageNode(
            SvLBoxEntry * parentNode,
            ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> const & xPackage,
            ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment> const & xCmdEnv);

        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> m_context;
        DialogImpl * m_dialog;
        SvLBoxEntry * m_currentSelectedEntry;
        bool m_hiContrastMode;
        Timer m_timer;

        String m_strEnabled;
        String m_strDisabled;
        String m_strUnknown;
        String m_strCtxAdd;
        String m_strCtxRemove;
        String m_strCtxEnable;
        String m_strCtxDisable;
        String m_strCtxExport;
        String m_strCtxCheckUpdate;

        Image m_defaultPackage;
        Image m_defaultPackage_hc;
        Image m_defaultPackageBundle;
        Image m_defaultPackageBundle_hc;

        DECL_LINK( TimerHandler, Timer * );
        virtual void SelectHdl();
        virtual void DeselectHdl();
        virtual void MouseMove( MouseEvent const & evt );
        virtual void KeyInput( KeyEvent const & evt );
        virtual void RequestingChilds( SvLBoxEntry * pEntry );
        using SvListView::Expand;
        virtual BOOL Expand( SvLBoxEntry * pParent );
        virtual void DataChanged( DataChangedEvent const & evt );
        virtual PopupMenu * CreateContextMenu(void);
        virtual void ExcecuteContextMenuAction( USHORT nSelectedPopupEntry );

        virtual ~TreeListBoxImpl();
        TreeListBoxImpl(
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> const & context,
            Window * pParent, DialogImpl * dialog );

        SvLBoxEntry * getCurrentSingleSelectedEntry() const;
        bool isFirstLevelChild( SvLBoxEntry * entry ) const;
        ::rtl::OUString getContext( SvLBoxEntry * entry ) const;
        ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> getPackage(
            SvLBoxEntry * entry ) const;
        void select(::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> const &   xPackage);

        ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager> getPackageManager(
            SvLBoxEntry * entry ) const;
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> >
        getSelectedPackages( bool onlyFirstLevel = false ) const;
    };

    class SyncPushButton : public PushButton
    {
    public:
        typedef void (DialogImpl::* t_clickCallback)( USHORT id );
        inline SyncPushButton(
            DialogImpl * dialog, t_clickCallback cb, USHORT id )
            : PushButton( dialog, getResId(id) ),
              m_dialog(dialog), m_clickCallback(cb), m_id(id) {}
        // PushButton
        virtual void Click();
    private:
        DialogImpl * m_dialog;
        t_clickCallback m_clickCallback;
        USHORT m_id;
    };

    class ThreadedPushButton : public SyncPushButton
    {
        oslThread m_thread;
    public:
        virtual ~ThreadedPushButton();
        inline ThreadedPushButton(
            DialogImpl * dialog, t_clickCallback cb, USHORT id )
            : SyncPushButton( dialog, cb, id ), m_thread(0) {}
        // PushButton
        virtual void Click();
    };

    virtual BOOL Close();
    virtual void Resize();
    DECL_LINK( headbar_dragEnd, HeaderBar * );

    // solar thread functions, because of gtk file/folder picker:
    ::com::sun::star::uno::Sequence<rtl::OUString> solarthread_raiseAddPicker(
        ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager>
        const & xPackageManager );
    bool solarthread_raiseExportPickers(
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> >
        const & selection,
        rtl::OUString & rDestFolder, rtl::OUString & rNewTitle,
        sal_Int32 & rNameClashAction );

    void clickClose( USHORT id );
    void clickAdd( USHORT id );
    void clickRemove( USHORT id );
    void clickEnableDisable( USHORT id );
    void clickExport( USHORT id );
    void clickCheckUpdates( USHORT id );
    void installExtensions();

    void updateButtonStates(
        ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment> const & xCmdEnv =
        com::sun::star::uno::Reference<
        com::sun::star::ucb::XCommandEnvironment>() );

    void errbox( ::rtl::OUString const & msg );

    void checkUpdates(bool selected);

    ::rtl::Reference<ModifiableContext> m_modifiableContext;
    const css::uno::Sequence< ::rtl::OUString > m_arExtensions;
    oslThread m_installThread;
    bool m_bAutoInstallFinished;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> m_xComponentContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManagerFactory> m_xPkgMgrFac;
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager> >
        m_packageManagers;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDesktop> m_xDesktop;
    ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XContent> m_xTdocRoot;

    const String m_strAddPackages;
    const String m_strAddingPackages;
    const String m_strRemovingPackages;
    const String m_strEnablingPackages;
    const String m_strDisablingPackages;
    const String m_strExportPackage;
    const String m_strExportPackages;
    const String m_strExportingPackages;

    Size m_buttonSize;
    Size m_relatedSpace;
    Size m_unrelatedSpace;
    Size m_borderLeftTopSpace;
    Size m_borderRightBottomSpace;
    long m_ftFontHeight;
    long m_descriptionYSpace;


    // controls:
    ::std::auto_ptr<FixedText> m_ftPackages;
    ::std::auto_ptr<SelectionBoxControl> m_selectionBox;
    ::std::auto_ptr<HeaderBar> m_headerBar;
    ::std::auto_ptr<TreeListBoxImpl> m_treelb;

    ::std::auto_ptr<ThreadedPushButton> m_addButton;
    ::std::auto_ptr<ThreadedPushButton> m_removeButton;
    ::std::auto_ptr<ThreadedPushButton> m_enableButton;
    ::std::auto_ptr<ThreadedPushButton> m_disableButton;
    ::std::auto_ptr<ThreadedPushButton> m_exportButton;
    ::std::auto_ptr<SyncPushButton> m_checkUpdatesButton;
    ::std::auto_ptr<FixedLine> m_bottomLine;

    ::std::auto_ptr<OKButton> m_closeButton;
    ::std::auto_ptr<HelpButton> m_helpButton;

    ::std::auto_ptr<Updatability> m_updatability;

    DECL_STATIC_LINK( DialogImpl, destroyDialog, void * );
    DECL_LINK( startInstallExtensions, DialogImpl *);
    static ::rtl::Reference<DialogImpl> s_dialog;
    virtual ~DialogImpl();
    DialogImpl(
        Window * pParent,
        ::com::sun::star::uno::Sequence< ::rtl::OUString > const & arExtensions,
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext );
    static ::rtl::Reference<DialogImpl> get(
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext> const & xContext,
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow> const & xParent = 0,
        css::uno::Sequence< ::rtl::OUString > const & arExtensions = 0,
        ::rtl::OUString const & view = ::rtl::OUString() );
    // XEventListener
    virtual void SAL_CALL disposing( ::com::sun::star::lang::EventObject const & evt )
        throw (::com::sun::star::uno::RuntimeException);
    // XContentEventListener
    virtual void SAL_CALL contentEvent( ::com::sun::star::ucb::ContentEvent const & evt )
        throw (::com::sun::star::uno::RuntimeException);
    // XTerminateListener
    virtual void SAL_CALL queryTermination( ::com::sun::star::lang::EventObject const & evt )
        throw (::com::sun::star::frame::TerminationVetoException,
               ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL notifyTermination(
        ::com::sun::star::lang::EventObject const & evt ) throw (::com::sun::star::uno::RuntimeException);
};

class SelectedPackageIterator: public salhelper::SimpleReferenceObject {
public:
    SelectedPackageIterator(DialogImpl::TreeListBoxImpl & list);

    virtual ~SelectedPackageIterator();

    void next(
        ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackage> * package,
        ::com::sun::star::uno::Reference< ::com::sun::star::deployment::XPackageManager> * packageManager);
        // must only be called with Application::GetSolarMutex() locked and
        // while the DialogImpl::TreeListBoxImpl is still alive

private:
    SelectedPackageIterator(SelectedPackageIterator &); // not defined
    void operator =(SelectedPackageIterator &); // not defined

    DialogImpl::TreeListBoxImpl & m_list;
    SvLBoxEntry * m_entry;
};

} // namespace dp_gui

#endif
