/*************************************************************************
 *
 *  $RCSfile: autocdlg.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 18:15:50 $
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
#ifndef _OFA_AUTOCDLG_HXX
#define _OFA_AUTOCDLG_HXX

#ifndef _SFXTABDLG_HXX
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SVX_LANGTAB_HXX //autogen
#include "langtab.hxx"
#endif
#ifndef _TOOLS_TABLE_HXX //autogen
#include <tools/table.hxx>
#endif
#ifndef _SVX_CHECKLBX_HXX //autogen
#include "checklbx.hxx"
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SVX_LANGBOX_HXX
#include "langbox.hxx"
#endif

class SvxAutoCorrect;
class CharClass;
class CollatorWrapper;

// class OfaAutoCorrDlg --------------------------------------------------

class OfaAutoCorrDlg : public SfxTabDialog
{
    FixedText       aLanguageFT;
    SvxLanguageBox  aLanguageLB;

    DECL_LINK(SelectLanguageHdl, ListBox*);
public:

    OfaAutoCorrDlg(Window* pParent, const SfxItemSet *pSet );

    void    EnableLanguage(BOOL bEnable)
            {   aLanguageFT.Enable(bEnable);
                aLanguageLB.Enable(bEnable);}

};

#ifdef _OFA_AUTOCDLG_CXX

#ifndef _SV_GROUP_HXX
#include <vcl/group.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SVTABBX_HXX
#include <svtools/svtabbx.hxx>
#endif
#ifndef _SVX_SIMPTABL_HXX //autogen
#include "simptabl.hxx"
#endif

class SvStringsISortDtor;

// class OfaACorrCheckListBox ------------------------------------------

class OfaACorrCheckListBox : public SvxSimpleTable
{
    protected:
        virtual void    SetTabs();
        virtual void    HBarClick();
        virtual void    KeyInput( const KeyEvent& rKEvt );

    public:
        OfaACorrCheckListBox(Window* pParent, const ResId& rResId ) :
            SvxSimpleTable( pParent, rResId ){}

        inline void *GetUserData(ULONG nPos) { return GetEntry(nPos)->GetUserData(); }
        inline void SetUserData(ULONG nPos, void *pData ) { GetEntry(nPos)->SetUserData(pData); }
        inline ULONG GetSelectEntryPos() { return GetModel()->GetAbsPos(FirstSelected()); }

        BOOL            IsChecked(ULONG nPos, USHORT nCol = 0);
        void            CheckEntryPos(ULONG nPos, USHORT nCol, BOOL bChecked);
        SvButtonState   GetCheckButtonState( SvLBoxEntry*, USHORT nCol ) const;
        void            SetCheckButtonState( SvLBoxEntry*, USHORT nCol, SvButtonState );
};

// class OfaAutocorrOptionsPage ------------------------------------------


class OfaAutocorrOptionsPage : public SfxTabPage
{

private:
    SvxCheckListBox aCheckLB;

    String      sInput;
    String      sDoubleCaps;
    String      sStartCap;
    String      sBoldUnderline;
    String      sURL;
    String      sNoDblSpaces;
    String      sHalf;
    String      sDash;
    String      sFirst;

public:
                        OfaAutocorrOptionsPage( Window* pParent, const SfxItemSet& rSet );
                        ~OfaAutocorrOptionsPage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& );

};

// class OfaSwAutoFmtOptionsPage ----------------------------------------------------

class OfaSwAutoFmtOptionsPage : public SfxTabPage
{
    OfaACorrCheckListBox    aCheckLB;
    PushButton      aEditPB;
    FixedText       aHeader1Expl;
    FixedText       aHeader2Expl;

    String          sHeader1;
    String          sHeader2;

    String          sDeleteEmptyPara;
    String          sUseReplaceTbl;
    String          sCptlSttWord;
    String          sCptlSttSent;
    String          sTypo;
    String          sUserStyle;
    String          sBullet;
    String          sByInputBullet;
    String          sBoldUnder;
    String          sNoDblSpaces;
    String          sFraction;
    String          sDetectURL;
    String          sDash;
    String          sOrdinal;
    String          sRightMargin;
    String          sNum;
    String          sBorder;
    String          sTable;
    String          sReplaceTemplates;
    String          sDelSpaceAtSttEnd;
    String          sDelSpaceBetweenLines;

    String          sMargin;
    String          sBulletChar;
    String          sByInputBulletChar;
    Bitmap          aChkunBmp;
    Bitmap          aChkchBmp;
    Bitmap          aChkchhiBmp;
    Bitmap          aChkunhiBmp;
    Bitmap          aChktriBmp;
    Bitmap          aChktrihiBmp;

    Font            aBulletFont;
    Font            aByInputBulletFont;
    USHORT          nPercent;

    SvLBoxButtonData*   pCheckButtonData;

        DECL_LINK(SelectHdl, OfaACorrCheckListBox*);
        DECL_LINK(EditHdl, PushButton*);
        SvLBoxEntry* CreateEntry(String& rTxt, USHORT nCol);


        OfaSwAutoFmtOptionsPage( Window* pParent,
                            const SfxItemSet& rSet );
        ~OfaSwAutoFmtOptionsPage();

    public:
        static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);
        virtual BOOL        FillItemSet( SfxItemSet& rSet );
        virtual void        Reset( const SfxItemSet& rSet );
        virtual void        ActivatePage( const SfxItemSet& );
};

// class AutoCorrEdit ----------------------------------------------------

class AutoCorrEdit : public Edit
{
    Link    aActionLink;
    BOOL    bSpaces;

    public:
                    AutoCorrEdit(Window* pParent, const ResId& rResId) :
                        Edit(pParent, rResId), bSpaces(FALSE){}

    void            SetActionHdl( const Link& rLink )
                                { aActionLink = rLink;}

    void            SetSpaces(BOOL bSet)
                                {bSpaces = bSet;}

    virtual void    KeyInput( const KeyEvent& rKEvent );
};

// class OfaAutocorrReplacePage ------------------------------------------

class DoubleStringArray;
typedef DoubleStringArray* DoubleStringArrayPtr;
DECLARE_TABLE(DoubleStringTable, DoubleStringArrayPtr);

class OfaAutocorrReplacePage : public SfxTabPage
{

private:
        CheckBox        aTextOnlyCB;
        FixedText       aShortFT;
        AutoCorrEdit    aShortED;
        FixedText       aReplaceFT;
        AutoCorrEdit    aReplaceED;
        SvTabListBox    aReplaceTLB;
        PushButton      aNewReplacePB;
        PushButton      aDeleteReplacePB;

        String          sModify;
        String          sNew;

        SvxAutoCorrect*         pAutoCorrect;
        SvStringsISortDtor*     pFormatText;
        DoubleStringTable       aDoubleStringTable;
        CollatorWrapper*        pCompareClass;
        CollatorWrapper*        pCompareCaseClass;
        CharClass*              pCharClass;
        LanguageType            eLang;

        BOOL            bHasSelectionText;
        BOOL            bFirstSelect:1;
        BOOL            bReplaceEditChanged:1;
        BOOL            bSWriter:1;

        DECL_LINK(SelectHdl, SvTabListBox*);
        DECL_LINK(NewDelHdl, PushButton*);
        DECL_LINK(ModifyHdl, Edit*);

        void            RefillReplaceBox(BOOL bFromReset, //Box mit neuer Sprache fuellen
                                        LanguageType eOldLanguage,
                                        LanguageType eNewLanguage);

public:
                        OfaAutocorrReplacePage( Window* pParent, const SfxItemSet& rSet );
                        ~OfaAutocorrReplacePage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& );
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );

    void                SetLanguage(LanguageType eSet);
};

// class OfaAutocorrExceptPage ---------------------------------------------

struct StringsArrays;
typedef StringsArrays* StringsArraysPtr;
DECLARE_TABLE(StringsTable, StringsArraysPtr);

class OfaAutocorrExceptPage : public SfxTabPage
{

private:
        FixedLine       aAbbrevFL;
        AutoCorrEdit    aAbbrevED;
        ListBox         aAbbrevLB;
        PushButton      aNewAbbrevPB;
        PushButton      aDelAbbrevPB;
        CheckBox        aAutoAbbrevCB;

        FixedLine       aDoubleCapsFL;
        AutoCorrEdit    aDoubleCapsED;
        ListBox         aDoubleCapsLB;
        PushButton      aNewDoublePB;
        PushButton      aDelDoublePB;
        CheckBox        aAutoCapsCB;

        SvxAutoCorrect* pAutoCorrect;

        StringsTable    aStringsTable;
        CollatorWrapper* pCompareClass;
        LanguageType    eLang;

    DECL_LINK(NewDelHdl, PushButton*);
    DECL_LINK(SelectHdl, ListBox*);
    DECL_LINK(ModifyHdl, Edit*);

    void            RefillReplaceBoxes(BOOL bFromReset, //Box mit neuer Sprache fuellen
                                        LanguageType eOldLanguage,
                                        LanguageType eNewLanguage);
public:
                        OfaAutocorrExceptPage( Window* pParent, const SfxItemSet& rSet );
                        ~OfaAutocorrExceptPage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& );
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );
    void                SetLanguage(LanguageType eSet);

};

// class OfaQuoteTabPage -------------------------------------------------

class OfaQuoteTabPage : public SfxTabPage
{
private:

    FixedLine   aSingleFL;
    CheckBox    aSingleTypoCB;
    FixedText   aSglStartQuoteFT;
    PushButton  aSglStartQuotePB;
    FixedText   aSglStartExFT;
    FixedText   aSglEndQuoteFT;
    PushButton  aSglEndQuotePB;
    FixedText   aSglEndExFT;
    PushButton  aSglStandardPB;

    FixedLine   aDoubleFL;
    CheckBox    aTypoCB;
    FixedText   aStartQuoteFT;
    PushButton  aStartQuotePB;
    FixedText   aDblStartExFT;
    FixedText   aEndQuoteFT;
    PushButton  aEndQuotePB;
    FixedText   aDblEndExFT;
    PushButton  aDblStandardPB;

    String      sStartQuoteDlg;
    String      sEndQuoteDlg;

    String      sStandard;


    sal_Unicode cSglStartQuote;
    sal_Unicode cSglEndQuote;

    sal_Unicode cStartQuote;
    sal_Unicode cEndQuote;

    DECL_LINK( QuoteHdl, PushButton* );
    DECL_LINK( StdQuoteHdl, PushButton* );

    String              ChangeStringExt_Impl( sal_Unicode cChar );

                        OfaQuoteTabPage( Window* pParent, const SfxItemSet& rSet );
public:
                        ~OfaQuoteTabPage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& );
};

// class OfaAutoCompleteTabPage ---------------------------------------------

class OfaAutoCompleteTabPage : public SfxTabPage
{
    class AutoCompleteMultiListBox : public MultiListBox
    {
        OfaAutoCompleteTabPage& rPage;
    public:
        AutoCompleteMultiListBox( OfaAutoCompleteTabPage& rPg,
                                    const ResId& rResId )
            : MultiListBox( &rPg, rResId ), rPage( rPg ) {}

        virtual long PreNotify( NotifyEvent& rNEvt );
    };

    CheckBox        aCBActiv; //Enable word completion
    CheckBox        aCBAppendSpace;//Append space
    CheckBox        aCBAsTip; //Show as tip

    CheckBox        aCBCollect;//Collect words
    CheckBox        aCBKeepList;//...save the list for later use...
    //--removed--CheckBox        aCBEndless;//

    FixedText       aFTExpandKey;
    ListBox         aDCBExpandKey;
    FixedText       aFTMinWordlen;
    NumericField    aNFMinWordlen;
    FixedText       aFTMaxEntries;
    NumericField    aNFMaxEntries;
    AutoCompleteMultiListBox    aLBEntries;
    PushButton                  aPBEntries;
    SvStringsISortDtor*         pAutoCmpltList;
    USHORT                      nAutoCmpltListCnt;

    DECL_LINK( CheckHdl, CheckBox* );

                        OfaAutoCompleteTabPage( Window* pParent,
                                                const SfxItemSet& rSet );
public:
                        virtual ~OfaAutoCompleteTabPage();

    static SfxTabPage*  Create( Window* pParent,
                                const SfxItemSet& rAttrSet);

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
    virtual void        ActivatePage( const SfxItemSet& );

    void CopyToClipboard() const;
    DECL_LINK( DeleteHdl, PushButton* );
};

#endif // _OFA_AUTOCDLG_CXX


#endif //

