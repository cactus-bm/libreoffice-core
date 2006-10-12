/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: chardlg.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 12:06:51 $
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
#ifndef _SVX_CHARDLG_HXX
#define _SVX_CHARDLG_HXX

// include ---------------------------------------------------------------

#ifndef _CTRLBOX_HXX //autogen
#include <svtools/ctrlbox.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _SVX_FNTCTRL_HXX
#include <fntctrl.hxx>
#endif
#include "checklbx.hxx"
#include "langbox.hxx"

// forward ---------------------------------------------------------------

class SvxFontListItem;
class FontList;

// -----------------------------------------------------------------------

#define DISABLE_CASEMAP             ((USHORT)0x0001)
#define DISABLE_WORDLINE            ((USHORT)0x0002)
#define DISABLE_BLINK               ((USHORT)0x0004)
#define DISABLE_UNDERLINE_COLOR     ((USHORT)0x0008)

#define DISABLE_LANGUAGE            ((USHORT)0x0010)
#define DISABLE_HIDE_LANGUAGE       ((USHORT)0x0020)

// class SvxCharBasePage -------------------------------------------------

class SvxCharBasePage : public SfxTabPage
{
protected:
    SvxFontPrevWindow   m_aPreviewWin;
    FixedInfo           m_aFontTypeFT;

    BOOL                m_bPreviewBackgroundToCharacter;

                        SvxCharBasePage( Window* pParent, const ResId& rResIdTabPage, const SfxItemSet&,
                                            USHORT nResIdPrewievWin, USHORT nResIdFontTypeFT );
    virtual             ~SvxCharBasePage();

    virtual void        ActivatePage( const SfxItemSet& rSet );

    void                SetPrevFontSize( const SfxItemSet& rSet, USHORT nSlot, SvxFont& rFont );
    void                SetPrevFont( const SfxItemSet& rSet, USHORT nSlot, SvxFont& rFont );
    void                SetPrevFontStyle( const SfxItemSet& rSet, USHORT nSlotPosture, USHORT nSlotWeight, SvxFont& rFont ); // posture/weight
    void                SetPrevFontWidthScale( const SfxItemSet& rSet );

    void                SetPrevFontEscapement( BYTE nProp, BYTE nEscProp, short nEsc );

    inline SvxFont&     GetPreviewFont();
    inline SvxFont&     GetPreviewCJKFont();
    inline SvxFont&     GetPreviewCTLFont();

    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

};

// class SvxCharNamePage -------------------------------------------------

struct SvxCharNamePage_Impl;

class SvxCharNamePage : public SvxCharBasePage
{
private:
    FixedLine*          m_pWestLine;
    FixedText*          m_pWestFontNameFT;
    FontNameBox*        m_pWestFontNameLB;
    FixedText*          m_pWestFontStyleFT;
    FontStyleBox*       m_pWestFontStyleLB;
    FixedText*          m_pWestFontSizeFT;
    FontSizeBox*        m_pWestFontSizeLB;
    FixedText*          m_pWestFontLanguageFT;
    SvxLanguageBox*     m_pWestFontLanguageLB;

    FixedLine*          m_pEastLine;
    FixedText*          m_pEastFontNameFT;
    FontNameBox*        m_pEastFontNameLB;
    FixedText*          m_pEastFontStyleFT;
    FontStyleBox*       m_pEastFontStyleLB;
    FixedText*          m_pEastFontSizeFT;
    FontSizeBox*        m_pEastFontSizeLB;
    FixedText*          m_pEastFontLanguageFT;
    SvxLanguageBox*     m_pEastFontLanguageLB;

    FixedLine*          m_pCTLLine;
    FixedText*          m_pCTLFontNameFT;
    FontNameBox*        m_pCTLFontNameLB;
    FixedText*          m_pCTLFontStyleFT;
    FontStyleBox*       m_pCTLFontStyleLB;
    FixedText*          m_pCTLFontSizeFT;
    FontSizeBox*        m_pCTLFontSizeLB;
    FixedText*          m_pCTLFontLanguageFT;
    SvxLanguageBox*     m_pCTLFontLanguageLB;

    FixedLine*          m_pColorFL;
    FixedText*          m_pColorFT;
    ColorListBox*       m_pColorLB;

    SvxCharNamePage_Impl*   m_pImpl;

                        SvxCharNamePage( Window* pParent, const SfxItemSet& rSet );

    void                Initialize();
    const FontList*     GetFontList() const;
    void                UpdatePreview_Impl();
    void                FillStyleBox_Impl( const FontNameBox* rBox );
    void                FillSizeBox_Impl( const FontNameBox* rBox );

    enum LanguageGroup
    {
        /** Language for western text.
         */
        Western = 0,

        /** Language for asian text.
         */
        Asian,

        /** Language for ctl text.
         */
        Ctl
    };

    void                Reset_Impl( const SfxItemSet& rSet, LanguageGroup eLangGrp );
    BOOL                FillItemSet_Impl( SfxItemSet& rSet, LanguageGroup eLangGrp );
    void                ResetColor_Impl( const SfxItemSet& rSet );
    BOOL                FillItemSetColor_Impl( SfxItemSet& rSet );

    DECL_LINK(          UpdateHdl_Impl, Timer* );
    DECL_LINK(          FontModifyHdl_Impl, void* );
    DECL_LINK(          ColorBoxSelectHdl_Impl, ColorListBox* );

protected:
    virtual void        ActivatePage( const SfxItemSet& rSet );
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );

public:
                        ~SvxCharNamePage();

    virtual void DeactivatePage();
    virtual void ActivatePage();

    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rSet );
    static USHORT*      GetRanges();

    virtual void        Reset( const SfxItemSet& rSet );
    virtual BOOL        FillItemSet( SfxItemSet& rSet );

    void                SetFontList( const SvxFontListItem& rItem );
    void                EnableRelativeMode();
    void                EnableSearchMode();
    //                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();

    void                DisableControls( USHORT nDisable );
    virtual void        PageCreated (SfxAllItemSet aSet); //add CHINA001
};

// class SvxCharEffectsPage ----------------------------------------------

class SvxCharEffectsPage : public SvxCharBasePage
{
    using TabPage::DeactivatePage;

private:
    FixedText           m_aUnderlineFT;
    ListBox             m_aUnderlineLB;
    FixedText           m_aColorFT;
    ColorListBox        m_aColorLB;
    FixedText           m_aStrikeoutFT;
    ListBox             m_aStrikeoutLB;
    CheckBox            m_aIndividualWordsBtn;
    FixedText           m_aEmphasisFT;
    ListBox             m_aEmphasisLB;
    FixedText           m_aPositionFT;
    ListBox             m_aPositionLB;
    FixedText           m_aFontColorFT;
    ColorListBox        m_aFontColorLB;

    FixedText           m_aEffectsFT;
    SvxCheckListBox     m_aEffectsLB;

    ListBox             m_aEffects2LB;

    FixedText           m_aReliefFT;
    ListBox             m_aReliefLB;

    TriStateBox         m_aOutlineBtn;
    TriStateBox         m_aShadowBtn;
    TriStateBox         m_aBlinkingBtn;
    TriStateBox         m_aHiddenBtn;

    USHORT              m_nHtmlMode;

    String              m_aTransparentColorName;

                        SvxCharEffectsPage( Window* pParent, const SfxItemSet& rSet );

    void                Initialize();
    void                UpdatePreview_Impl();
    void                SetCaseMap_Impl( SvxCaseMap eCaseMap );
    void                ResetColor_Impl( const SfxItemSet& rSet );
    BOOL                FillItemSetColor_Impl( SfxItemSet& rSet );

    DECL_LINK(          SelectHdl_Impl, ListBox* );
    DECL_LINK(          CbClickHdl_Impl, CheckBox* );
    DECL_LINK(          TristClickHdl_Impl, TriStateBox* );
    DECL_LINK(          UpdatePreview_Impl, ListBox* );
    DECL_LINK(          ColorBoxSelectHdl_Impl, ColorListBox* );

protected:
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );

public:
    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rSet );
    static USHORT*      GetRanges();

    virtual void        Reset( const SfxItemSet& rSet );
    virtual BOOL        FillItemSet( SfxItemSet& rSet );

    void                DisableControls( USHORT nDisable );
    void                EnableFlash();
    //                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();
    virtual void        PageCreated (SfxAllItemSet aSet); //add CHINA001
};

// class SvxCharPositionPage ---------------------------------------------

class SvxCharPositionPage : public SvxCharBasePage
{
private:
    FixedLine           m_aPositionLine;
    RadioButton         m_aHighPosBtn;
    RadioButton         m_aNormalPosBtn;
    RadioButton         m_aLowPosBtn;
    FixedText           m_aHighLowFT;
    MetricField         m_aHighLowEdit;
    CheckBox            m_aHighLowRB;
    FixedText           m_aFontSizeFT;
    MetricField         m_aFontSizeEdit;
    FixedLine           m_aRotationScalingFL;
    FixedLine           m_aScalingFL;
    RadioButton         m_a0degRB;
    RadioButton         m_a90degRB;
    RadioButton         m_a270degRB;
    CheckBox            m_aFitToLineCB;
    FixedText           m_aScaleWidthFT;
    MetricField         m_aScaleWidthMF;

    FixedLine           m_aKerningLine;
    ListBox             m_aKerningLB;
    FixedText           m_aKerningFT;
    MetricField         m_aKerningEdit;
    CheckBox            m_aPairKerningBtn;

    short               m_nSuperEsc;
    short               m_nSubEsc;

    UINT16              m_nScaleWidthItemSetVal;
    UINT16              m_nScaleWidthInitialVal;

    BYTE                m_nSuperProp;
    BYTE                m_nSubProp;

                        SvxCharPositionPage( Window* pParent, const SfxItemSet& rSet );

    void                Initialize();
    void                UpdatePreview_Impl( BYTE nProp, BYTE nEscProp, short nEsc );
    void                SetEscapement_Impl( USHORT nEsc );

    DECL_LINK(          PositionHdl_Impl, RadioButton* );
    DECL_LINK(          RotationHdl_Impl, RadioButton* );
    DECL_LINK(          FontModifyHdl_Impl, MetricField* );
    DECL_LINK(          AutoPositionHdl_Impl, CheckBox* );
    DECL_LINK(          FitToLineHdl_Impl, CheckBox* );
    DECL_LINK(          KerningSelectHdl_Impl, ListBox* );
    DECL_LINK(          KerningModifyHdl_Impl, MetricField* );
    DECL_LINK(          PairKerningHdl_Impl, CheckBox* );
    DECL_LINK(          LoseFocusHdl_Impl, MetricField* );
    DECL_LINK(          ScaleWidthModifyHdl_Impl, MetricField* );

protected:
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );
    virtual void        ActivatePage( const SfxItemSet& rSet );

public:
    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rSet );
    static USHORT*      GetRanges();

    virtual void        Reset( const SfxItemSet& rSet );
    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        FillUserData();
    //                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();
    virtual void        PageCreated (SfxAllItemSet aSet); //add CHINA001

    virtual void DeactivatePage();
    virtual void ActivatePage();
};

// class SvxCharTwoLinesPage ---------------------------------------------

class SvxCharTwoLinesPage : public SvxCharBasePage
{
private:
    FixedLine           m_aSwitchOnLine;
    CheckBox            m_aTwoLinesBtn;

    FixedLine           m_aEncloseLine;
    FixedText           m_aStartBracketFT;
    ListBox             m_aStartBracketLB;
    FixedText           m_aEndBracketFT;
    ListBox             m_aEndBracketLB;

                        SvxCharTwoLinesPage( Window* pParent, const SfxItemSet& rSet );

    void                UpdatePreview_Impl();
    void                Initialize();
    void                SelectCharacter( ListBox* pBox );
    void                SetBracket( sal_Unicode cBracket, BOOL bStart );

    DECL_LINK(          TwoLinesHdl_Impl, CheckBox* );
    DECL_LINK(          CharacterMapHdl_Impl, ListBox* );

protected:
    virtual void        ActivatePage( const SfxItemSet& rSet );
    virtual int         DeactivatePage( SfxItemSet* pSet = 0 );

public:
    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rSet );
    static USHORT*      GetRanges();

    virtual void        Reset( const SfxItemSet& rSet );
    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    //                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();
    virtual void        PageCreated (SfxAllItemSet aSet); //add CHINA001

    virtual void DeactivatePage();
    virtual void ActivatePage();
};

#endif // #ifndef _SVX_CHARDLG_HXX

