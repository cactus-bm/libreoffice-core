/*************************************************************************
 *
 *  $RCSfile: srchdlg.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: tl $ $Date: 2001-02-19 11:16:57 $
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
#ifndef _SVX_SRCHDLG_HXX
#define _SVX_SRCHDLG_HXX

// include ---------------------------------------------------------------


#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <sfx2/childwin.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <svtools/svarray.hxx>
#endif
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>

#ifndef _SVEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif

// defines ---------------------------------------------------------------

#define SEARCH_OPTIONS_SEARCH       ((USHORT)0x0001)
#define SEARCH_OPTIONS_SEARCH_ALL   ((USHORT)0x0002)
#define SEARCH_OPTIONS_REPLACE      ((USHORT)0x0004)
#define SEARCH_OPTIONS_REPLACE_ALL  ((USHORT)0x0008)
#define SEARCH_OPTIONS_WHOLE_WORDS  ((USHORT)0x0010)
#define SEARCH_OPTIONS_BACKWARDS    ((USHORT)0x0020)
#define SEARCH_OPTIONS_REG_EXP      ((USHORT)0x0040)
#define SEARCH_OPTIONS_EXACT        ((USHORT)0x0080)
#define SEARCH_OPTIONS_SELECTION    ((USHORT)0x0100)
#define SEARCH_OPTIONS_FAMILIES     ((USHORT)0x0200)
#define SEARCH_OPTIONS_FORMAT       ((USHORT)0x0400)
#define SEARCH_OPTIONS_MORE         ((USHORT)0x0800)
#define SEARCH_OPTIONS_SIMILARITY   ((USHORT)0x1000)
#define SEARCH_OPTIONS_CONTENT      ((USHORT)0x2000)

// forward ---------------------------------------------------------------

class SvxSearchItem;
class MoreButton;
class SfxStyleSheetBasePool;
struct SearchDlg_Impl;

#ifndef NO_SVX_SEARCH

// struct SearchAttrItem -------------------------------------------------

struct SearchAttrItem
{
    USHORT          nSlot;
    SfxPoolItem*    pItem;
};

// class SearchAttrItemList ----------------------------------------------

SV_DECL_VARARR(SrchAttrItemList, SearchAttrItem, 8, 8);

class SearchAttrItemList : private SrchAttrItemList
{
public:
    SearchAttrItemList() {}
    SearchAttrItemList( const SearchAttrItemList& rList );
    ~SearchAttrItemList();

    void            Put( const SfxItemSet& rSet );
    SfxItemSet&     Get( SfxItemSet& rSet );
    void            Clear();
    USHORT          Count() const { return SrchAttrItemList::Count(); }
    SearchAttrItem& operator[](USHORT nPos) const
                        { return SrchAttrItemList::operator[]( nPos ); }
    SearchAttrItem& GetObject( USHORT nPos ) const
                        { return SrchAttrItemList::GetObject( nPos ); }

    // der Pointer auf das Item wird nicht kopiert!!! (also nicht l"oschen)
    void Insert( const SearchAttrItem& rItem )
        { SrchAttrItemList::Insert( rItem, SrchAttrItemList::Count() ); }
    // l"oscht die Pointer auf die Items
    void Remove( USHORT nPos, USHORT nLen = 1 );
};

#ifndef SV_NODIALOG

// class SvxSearchDialogWrapper ------------------------------------------

class SvxSearchDialogWrapper : public SfxChildWindow
{
public:
    SvxSearchDialogWrapper( Window*pParent, USHORT nId,
                            SfxBindings* pBindings, SfxChildWinInfo* pInfo );

    SFX_DECL_CHILDWINDOW(SvxSearchDialogWrapper);
};

// class SvxSearchDialog -------------------------------------------------

/*
    {k:\svx\prototyp\dialog\srchdlg.hxx}

    [Beschreibung]
    In diesem Modeless-Dialog werden die Attribute einer Suche eingestellt
    und damit eine Suche gestartet. Es sind mehrere Sucharten
    ( Suchen, Alle suchen, Ersetzen, Alle ersetzen ) m"oglich.

    [Items]
    <SvxSearchItem><SID_ATTR_SEARCH>
*/

class SvxSearchDialog : public ModelessDialog
{
    friend class SvxSearchController;
    friend class SvxSearchDialogWrapper;

public:
    SvxSearchDialog( Window* pParent, SfxBindings& rBind );
    ~SvxSearchDialog();

    virtual BOOL    Close();

    void            GetSearchItems( SfxItemSet& rSet );
    void            GetReplaceItems( SfxItemSet& rSet );

    const SearchAttrItemList*   GetSearchItemList() const
                                    { return pSearchList; }
    const SearchAttrItemList*   GetReplaceItemList() const
                                    { return pReplaceList; }

    inline BOOL     HasSearchAttributes() const;
    inline BOOL     HasReplaceAttributes() const;

    PushButton&     GetReplaceBtn() { return aReplaceBtn; }

    INT32           GetTransliterationSettings() const;

private:
    FixedText       aSearchText;
    ComboBox        aSearchLB;
    ListBox         aSearchTmplLB;
    FixedInfo       aSearchAttrText;
    MultiLineEdit   aSearchFormatsED;
    GroupBox        aSearchAttrBox;

    FixedText       aReplaceText;
    ComboBox        aReplaceLB;
    ListBox         aReplaceTmplLB;
    FixedInfo       aReplaceAttrText;
    MultiLineEdit   aReplaceFormatsED;
    GroupBox        aReplaceAttrBox;

    PushButton      aSearchAllBtn;
    PushButton      aSearchBtn;
    PushButton      aReplaceAllBtn;
    PushButton      aReplaceBtn;
    PushButton      aAttributeBtn;
    CancelButton    aCloseBtn;
    PushButton      aFormatBtn;
    HelpButton      aHelpBtn;
    PushButton      aNoFormatBtn;
    MoreButton*     pMoreBtn;

    CheckBox        aWordBtn;
    CheckBox        aExactBtn;
    CheckBox        aBackwardsBtn;
    CheckBox        aSelectionBtn;
    CheckBox        aRegExpBtn;
    CheckBox        aLayoutBtn;
                    // "Ahnlichkeitssuche
    CheckBox        aSimilarityBox;
    PushButton      aSimilarityBtn;
    CheckBox        aJapMatchFullHalfWidthCB;
    CheckBox        aJapSoundsLikeCB;
    PushButton      aJapOptionsBtn;
    GroupBox        aOptionsBox;

                    // nur f"ur Calc
    RadioButton     aFormulasBtn;
    RadioButton     aValuesBtn;
    RadioButton     aNotesBtn;
    GroupBox        aSearchBox;

    RadioButton     aRowsBtn;
    RadioButton     aColumnsBtn;
    GroupBox        aSearchDirBox;

    CheckBox        aAllTablesCB;
    GroupBox        aCalcExtrasBox;

    SfxBindings&    rBindings;
    BOOL            bWriter;
    BOOL            bSearch;
    BOOL            bFormat;
    USHORT          nOptions;
    FASTBOOL        bSet;
    FASTBOOL        bReadOnly;
    FASTBOOL        bConstruct;
    ULONG           nModifyFlag;
    String          aStylesStr;
    String          aLayoutStr;
    String          aCalcStr;

    SvStringsDtor   aSearchStrings;
    SvStringsDtor   aReplaceStrings;

    SearchDlg_Impl*         pImpl;
    SearchAttrItemList*     pSearchList;
    SearchAttrItemList*     pReplaceList;
    SvxSearchItem*          pSearchItem;

    SvxSearchController*    pSearchController;
    SvxSearchController*    pOptionsController;
    SvxSearchController*    pFamilyController;
    SvxSearchController*    pSearchSetController;
    SvxSearchController*    pReplaceSetController;

    INT32           nTransliterationSettings;

#ifdef _SVX_SRCHDLG_CXX
    DECL_LINK( ModifyHdl_Impl, ComboBox* pEdit );
    DECL_LINK( FlagHdl_Impl, Button* pBtn );
    DECL_LINK( CommandHdl_Impl, Button* pBtn );
    DECL_LINK( TemplateHdl_Impl, Button* );
    DECL_LINK( FocusHdl_Impl, Control* );
    DECL_LINK( LoseFocusHdl_Impl, Control* );
    DECL_LINK( FormatHdl_Impl, Button* );
    DECL_LINK( NoFormatHdl_Impl, Button* );
    DECL_LINK( AttributeHdl_Impl, Button* );
    DECL_LINK( TimeoutHdl_Impl, Timer* );

    void            InitControls_Impl();
    void            Init_Impl( int bHasItemSet );
    void            InitAttrList_Impl( const SfxItemSet* pSSet,
                                       const SfxItemSet* pRSet );
    void            Remember_Impl( const String &rStr,BOOL bSearch );
    void            PaintAttrText_Impl();
    String&         BuildAttrText_Impl( String& rStr, BOOL bSrchFlag ) const;

    void            TemplatesChanged_Impl( SfxStyleSheetBasePool& rPool );
    void            EnableControls_Impl( const USHORT nFlags );
    void            EnableControl_Impl( Control* pCtrl );
    void            SetItem_Impl( const SvxSearchItem* pItem );

    void            SetModifyFlag_Impl( const Control* pCtrl );
    void            SaveToModule_Impl();

    void            ApplyTransliterationSettings_Impl( INT32 nSettings );
#endif
};

inline BOOL SvxSearchDialog::HasSearchAttributes() const
{
    int bLen = aSearchAttrText.GetText().Len();
    return ( aSearchAttrText.IsEnabled() && bLen );
}

inline BOOL SvxSearchDialog::HasReplaceAttributes() const
{
    int bLen = aReplaceAttrText.GetText().Len();
    return ( aReplaceAttrText.IsEnabled() && bLen );
}

#endif  // SV_NODIALOG
#endif  // NO_SVX_SEARCH


#endif

