/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dialogs.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 21:15:25 $
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

#ifndef _DIALOGS_HXX
#define _DIALOGS_HXX

#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_TABDLG_HXX //autogen
#include <vcl/tabdlg.hxx>
#endif
#ifndef _SV_TABCTRL_HXX //autogen
#include <vcl/tabctrl.hxx>
#endif
#ifndef _SV_TABPAGE_HXX //autogen
#include <vcl/tabpage.hxx>
#endif
#ifndef _CONFIG_HXX
#include <tools/config.hxx>
#endif
#ifndef _TOOLS_LIST_HXX
#include <tools/list.hxx>
#endif
#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_FLOATWIN_HXX //autogen
#include <vcl/floatwin.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _CTRLTOOL_HXX
#include <svtools/ctrltool.hxx>
#endif
#ifndef _CTRLBOX_HXX
#include <svtools/ctrlbox.hxx>
#endif

class SbxVariable;

#ifndef _BASIC_TTRESHLP_HXX
#include "ttstrhlp.hxx"
#endif

class AboutDialog : public ModalDialog {
    FixedText a1,a4,aVersionString;
    OKButton  aOk;
public:
    AboutDialog (Window*, const ResId&);
};

class FindDialog : public ModalDialog {
    FixedText       aFT1;
    Edit            aFind;
    OKButton        aOk;
    CancelButton    aCancel;
    String*         pFind;
    DECL_LINK( ButtonClick, Button * );
public:
    FindDialog (Window*, const ResId&, String&);
};

class ReplaceDialog : public ModalDialog {
    FixedText       aFT1;
    FixedText       aFT2;
    Edit            aFind;
    Edit            aReplace;
    OKButton        aOk;
    CancelButton    aCancel;
    String*         pFind;
    String*         pReplace;
    DECL_LINK( ButtonClick, Button * );
public:
    ReplaceDialog (Window*, const ResId&, String&, String&);
};

////////////////////////////////////////////////////////////////////

class ConfEdit : public PushButton
{
    FixedText aText;
    ByteString aKeyName;
    Edit aEdit;
public:
    ConfEdit( Window* pParent, USHORT nResText, USHORT nResEdit, USHORT nResButton, const ByteString& aKN, Config &aConf );
    void Save( Config &aConf );
    void Reload( Config &aConf );
    void Click();
};


class OptionsDialog : public ModelessDialog
{
private:
    TabControl      aTabCtrl;

    OKButton aOK;
    CancelButton aCancel;
    DECL_LINK( OKClick, Button * );

    Config aConfig;

public:
    OptionsDialog( Window* pParent, const ResId& );
    ~OptionsDialog();
    virtual BOOL    Close();


    DECL_LINK( ActivatePageHdl, TabControl * );
};

class ProfileOptions : public TabPage
{
    FixedLine aFlProfile;
    ComboBox aCbProfile;
    PushButton aPbNewProfile;
    PushButton aPbDelProfile;

    FixedLine aDirs;
    ConfEdit aLog;
    ConfEdit aBasis;
    ConfEdit aHID;

    CheckBox aAutoReload;
    CheckBox aAutoSave;
    CheckBox aStopOnSyntaxError;

    Config &rConf;

    void LoadData();

    DECL_LINK( Select, ComboBox* );
    DECL_LINK( DelProfile, Button* );
    DECL_LINK( NewProfile, Button* );
    DECL_LINK( CheckButtonsHdl, ComboBox* );

    void ReloadProfile();
    void Save();

public:
    ProfileOptions( Window*, Config &rConfig );
    void Save( Config &rConfig );
};

class MiscOptions : public TabPage
{
    FixedLine aFLCommunication;
    FixedText aFTHost;
    Edit aEDHost;
    FixedText aFTTTPort;
    NumericField aNFTTPort;
    FixedText aFTUNOPort;
    NumericField aNFUNOPort;
    FixedLine aOther;
    FixedText aTimeoutText;
    TimeField aServerTimeout;
    FixedText aFTLRU;
    NumericField aTFMaxLRU;

public:
    MiscOptions( Window*, Config &aConfig );
    void Save( Config &aConfig );
};

class FontOptions : public TabPage
{
    FixedText aFTFontName;
    FontNameBox aFontName;
    FixedText aFTStyle;
    FontStyleBox aFontStyle;
    FixedText aFTSize;
    FontSizeBox aFontSize;
    FixedText aFTPreview;

    FontList aFontList;

    DECL_LINK( FontNameChanged, void* );
    DECL_LINK( FontStyleChanged, void* );
    DECL_LINK( FontSizeChanged, void* );

    void UpdatePreview();

public:
    FontOptions( Window*, Config &aConfig );
    void Save( Config &aConfig );
};


DECLARE_LIST( StringList, String * );
#define C_KEY_ALLE          CByteString("All")
#define C_KEY_AKTUELL       CByteString("Current")
#define C_KEY_DELETE        CByteString("Deleted Groups")

class GenericOptions : public TabPage
{
    FixedLine aFlArea;
    ComboBox aCbArea;
    PushButton aPbNewArea;
    PushButton aPbDelArea;

    FixedLine aFlValue;
    ComboBox aCbValue;
    PushButton aPbNewValue;
    PushButton aPbDelValue;

    Config &aConf;
    ByteString aLastGroupName;

    String ReadKey( const ByteString &aGroup, const ByteString &aKey );

    StringList* GetAllGroups();
    void LoadData();

    DECL_LINK( LoadGroup, ComboBox* );
    DECL_LINK( DelGroup, Button* );
    DECL_LINK( NewGroup, Button* );
    DECL_LINK( DelValue, Button* );
    DECL_LINK( NewValue, Button* );
    DECL_LINK( CheckButtonsHdl, ComboBox* );

public:
    GenericOptions( Window*, Config &aConfig );
    ~GenericOptions();
    void Save( Config &aConfig );
};


struct WinInfoRec;
class SplitWindow;

class DisplayHidDlg : public FloatingWindow
{
protected:
    ToolBox aTbConf;
    FixedText aFtControls;
    MultiListBox aMlbControls;
    FixedText aFtSlots;
    MultiListBox aMlbSlots;
    PushButton aPbKopieren;
    PushButton aPbBenennen;
    PushButton aPbSelectAll;
    OKButton aOKClose;

    DockingWindow* pControls;
    DockingWindow* pSlots;
    SplitWindow *pSplit;

    ULONG nDisplayMode;

    DECL_LINK( Select, void* );
    DECL_LINK( SelectAll, PushButton* );
    DECL_LINK( CopyToClipboard, void* );

public:
    DisplayHidDlg( Window * pParent );
    virtual ~DisplayHidDlg();

    virtual void Resize();

    void AddData( WinInfoRec* pWinInfo );

};


class VarEditDialog : public ModelessDialog
{
protected:
    FixedText aFixedTextRID_FT_NAME;
    FixedText aFixedTextRID_FT_CONTENT;
    FixedText aFixedTextRID_FT_NEW_CONTENT;
    FixedText aFixedTextRID_FT_NAME_VALUE;
    FixedText aFixedTextRID_FT_CONTENT_VALUE;

    RadioButton aRadioButtonRID_RB_NEW_BOOL_T;
    RadioButton aRadioButtonRID_RB_NEW_BOOL_F;
    NumericField aNumericFieldRID_NF_NEW_INTEGER;
    NumericField aNumericFieldRID_NF_NEW_LONG;
    Edit aEditRID_ED_NEW_STRING;

    OKButton aOKButtonRID_OK;
    CancelButton aCancelButtonRID_CANCEL;

    SbxVariable *pVar;

    DECL_LINK( OKClick, Button * );

//  BOOL bCompare = FALSE;
//  String aCompareString;

public:
    VarEditDialog( Window * pParent, SbxVariable *pPVar );
};


#endif
