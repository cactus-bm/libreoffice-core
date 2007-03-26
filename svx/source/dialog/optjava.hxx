/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: optjava.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: ihi $ $Date: 2007-03-26 12:07:15 $
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
#ifndef _SVX_OPTJAVA_HXX
#define _SVX_OPTJAVA_HXX

// include ---------------------------------------------------------------

#include <vector>

#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_TIMER_HXX
#include <vcl/timer.hxx>
#endif
#ifndef _SFXTABDLG_HXX
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SVX_SIMPTABL_HXX
#include "simptabl.hxx"
#endif
#ifndef _COM_SUN_STAR_UI_XFOLDERPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFolderPicker.hpp>
#endif
#ifndef _SVTOOLS_DIALOGCLOSEDLISTENER_HXX
#include <svtools/dialogclosedlistener.hxx>
#endif
#include "radiobtnbox.hxx"

// forward ---------------------------------------------------------------

typedef struct _JavaInfo JavaInfo;

class   SvxJavaParameterDlg;
class   SvxJavaClassPathDlg;

// class SvxJavaTable ----------------------------------------------------

class SvxJavaTable : public SvxSimpleTable
{
    using SvxSimpleTable::SetTabs;
private:
    Point               m_aCurMousePoint;

protected:
    virtual void        SetTabs();
    virtual void        MouseButtonUp( const MouseEvent& _rMEvt );
    virtual void        KeyInput( const KeyEvent& rKEvt );

public:
    SvxJavaTable( Window* _pParent, const ResId& _rId );
    ~SvxJavaTable();


    inline Point        GetCurMousePoint() { return m_aCurMousePoint; }
};

// class SvxJavaOptionsPage ----------------------------------------------

class SvxJavaOptionsPage : public SfxTabPage
{
private:
    FixedLine                   m_aJavaLine;
    CheckBox                    m_aJavaEnableCB;
    FixedText                   m_aJavaFoundLabel;
    svx::SvxRadioButtonListBox  m_aJavaList;
    FixedText                   m_aJavaPathText;
    PushButton                  m_aAddBtn;
    PushButton                  m_aParameterBtn;
    PushButton                  m_aClassPathBtn;

    SvxJavaParameterDlg*    m_pParamDlg;
    SvxJavaClassPathDlg*    m_pPathDlg;

    JavaInfo**              m_parJavaInfo;
    rtl_uString**           m_parParameters;
    rtl_uString*            m_pClassPath;
    sal_Int32               m_nInfoSize;
    sal_Int32               m_nParamSize;
    String                  m_sInstallText;
    String                  m_sAccessibilityText;
    String                  m_sAddDialogText;
    Timer                   m_aResetTimer;

    ::std::vector< JavaInfo* >
                            m_aAddedInfos;

    ::com::sun::star::uno::Reference< ::svt::DialogClosedListener > xDialogListener;
    ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFolderPicker > xFolderPicker;

    DECL_LINK(              EnableHdl_Impl, CheckBox * );
    DECL_LINK(              CheckHdl_Impl, SvxSimpleTable * );
    DECL_LINK(              SelectHdl_Impl, SvxSimpleTable * );
    DECL_LINK(              AddHdl_Impl, PushButton * );
    DECL_LINK(              ParameterHdl_Impl, PushButton * );
    DECL_LINK(              ClassPathHdl_Impl, PushButton * );
    DECL_LINK(              ResetHdl_Impl, Timer * );

    DECL_LINK(              StartFolderPickerHdl, void * );
    DECL_LINK(              DialogClosedHdl, ::com::sun::star::ui::dialogs::DialogClosedEvent* );

    void                    ClearJavaInfo();
    void                    ClearJavaList();
    void                    LoadJREs();
    void                    AddJRE( JavaInfo* _pInfo );
    void                    HandleCheckEntry( SvLBoxEntry* _pEntry );
    void                    AddFolder( const ::rtl::OUString& _rFolder );

public:
    SvxJavaOptionsPage( Window* pParent, const SfxItemSet& rSet );
    ~SvxJavaOptionsPage();

    static SfxTabPage*      Create( Window* pParent, const SfxItemSet& rSet );

    virtual BOOL            FillItemSet( SfxItemSet& rSet );
    virtual void            Reset( const SfxItemSet& rSet );
    virtual void            FillUserData();
};

// class SvxJavaParameterDlg ---------------------------------------------

class SvxJavaParameterDlg : public ModalDialog
{
private:
    FixedText               m_aParameterLabel;
    Edit                    m_aParameterEdit;
    PushButton              m_aAssignBtn;

    FixedText               m_aAssignedLabel;
    ListBox                 m_aAssignedList;
    FixedText               m_aExampleText;
    PushButton              m_aRemoveBtn;

    FixedLine               m_aButtonsLine;
    OKButton                m_aOKBtn;
    CancelButton            m_aCancelBtn;
    HelpButton              m_aHelpBtn;

    DECL_LINK(              ModifyHdl_Impl, Edit * );
    DECL_LINK(              AssignHdl_Impl, PushButton * );
    DECL_LINK(              SelectHdl_Impl, ListBox * );
    DECL_LINK(              DblClickHdl_Impl, ListBox * );
    DECL_LINK(              RemoveHdl_Impl, PushButton * );

    inline void             EnableRemoveButton()
                                { m_aRemoveBtn.Enable(
                                    m_aAssignedList.GetSelectEntryPos()
                                    != LISTBOX_ENTRY_NOTFOUND ); }


public:
    SvxJavaParameterDlg( Window* pParent );
    ~SvxJavaParameterDlg();

    virtual short           Execute();

    ::com::sun::star::uno::Sequence< ::rtl::OUString > GetParameters() const;
    void SetParameters( ::com::sun::star::uno::Sequence< ::rtl::OUString >& rParams );
};

// class SvxJavaClassPathDlg ---------------------------------------------

class SvxJavaClassPathDlg : public ModalDialog
{
private:
    FixedText               m_aPathLabel;
    ListBox                 m_aPathList;
    PushButton              m_aAddArchiveBtn;
    PushButton              m_aAddPathBtn;
    PushButton              m_aRemoveBtn;

    FixedLine               m_aButtonsLine;
    OKButton                m_aOKBtn;
    CancelButton            m_aCancelBtn;
    HelpButton              m_aHelpBtn;

    String                  m_sOldPath;

    DECL_LINK(              AddArchiveHdl_Impl, PushButton * );
    DECL_LINK(              AddPathHdl_Impl, PushButton * );
    DECL_LINK(              RemoveHdl_Impl, PushButton * );
    DECL_LINK(              SelectHdl_Impl, ListBox * );

    bool                    IsPathDuplicate( const String& _rPath );
    inline void             EnableRemoveButton()
                                { m_aRemoveBtn.Enable(
                                    m_aPathList.GetSelectEntryPos() != LISTBOX_ENTRY_NOTFOUND ); }

public:
    SvxJavaClassPathDlg( Window* pParent );
    ~SvxJavaClassPathDlg();

    inline const String&    GetOldPath() const { return m_sOldPath; }
    inline void             SetFocus() { m_aPathList.GrabFocus(); }

    String                  GetClassPath() const;
    void                    SetClassPath( const String& _rPath );
};

#endif // #ifndef _SVX_OPTJAVA_HXX

