/*************************************************************************
 *
 *  $RCSfile: cuifmsearch.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 16:31:30 $
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

#ifdef SVX_DLLIMPLEMENTATION
#undef SVX_DLLIMPLEMENTATION
#endif

#ifndef _SVX_FMRESIDS_HRC
#include "fmresids.hrc"
#endif // _SVX_FMRESIDS_HRC

#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif

#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif


#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif

#ifndef _SHL_HXX
#include <tools/shl.hxx>
#endif

#ifndef _SVX_DIALMGR_HXX //autogen
#include <dialmgr.hxx>
#endif

//CHINA001 #ifndef _SVX_SRCHXTRA_HXX
//CHINA001 #include "srchxtra.hxx"
//CHINA001 #endif // _SVX_SRCHXTRA_HXX
#include <sfx2/tabdlg.hxx> //add by CHINA001 for SfxItemSet
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _SVX_DIALOGS_HRC
#include "dialogs.hrc"
#endif

#ifndef _SFXFILEREC_HXX //autogen
#include <svtools/filerec.hxx>
#endif

#ifndef _FMSRCIMP_HXX
#include "fmsrcimp.hxx"
#endif //  _FMSRCIMP_HXX

#ifndef _FMSRCCF_HXX_
#include "fmsrccfg.hxx"
#endif // _FMSRCCF_HXX_

#ifndef _FMSEARCH_HRC
#include "fmsearch.hrc"
#endif // _FMSEARCH_HRC

#ifndef _CUI_FMSEARCH_HXX
#include "cuifmsearch.hxx"
#endif // _CUI_FMSEARCH_HXX

#ifndef _SVX_SRCHDLG_HXX
#include "srchdlg.hxx"
#endif

#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include <svtools/cjkoptions.hxx>
#endif

#ifndef _COM_SUN_STAR_I18N_TRANSLITERATIONMODULES_HPP_
#include <com/sun/star/i18n/TransliterationModules.hpp>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#include "svxdlg.hxx" //CHINA001

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::i18n;
using namespace ::svxform;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::util;



//========================================================================
#define INIT_MEMBERS()  \
     m_flSearchFor              (this, ResId(FL_SEARCHFOR))         \
    ,m_rbSearchForText          (this, ResId(RB_SEARCHFORTEXT))     \
    ,m_rbSearchForNull          (this, ResId(RB_SEARCHFORNULL))     \
    ,m_rbSearchForNotNull       (this, ResId(RB_SEARCHFORNOTNULL))  \
    ,m_cmbSearchText            (this, ResId(CMB_SEARCHTEXT))       \
    ,m_flWhere                  (this, ResId(FL_WHERE))             \
    ,m_ftForm                   (this, ResId(FT_FORM))              \
    ,m_lbForm                   (this, ResId(LB_FORM))              \
    ,m_rbAllFields              (this, ResId(RB_ALLFIELDS))         \
    ,m_rbSingleField            (this, ResId(RB_SINGLEFIELD))       \
    ,m_lbField                  (this, ResId(LB_FIELD))             \
    ,m_flOptions                (this, ResId(FL_OPTIONS))           \
    ,m_ftPosition               (this, ResId(FT_POSITION))          \
    ,m_lbPosition               (this, ResId(LB_POSITION))          \
    ,m_cbUseFormat              (this, ResId(CB_USEFORMATTER))      \
    ,m_cbCase                   (this, ResId(CB_CASE))              \
    ,m_cbBackwards              (this, ResId(CB_BACKWARD))          \
    ,m_cbStartOver              (this, ResId(CB_STARTOVER))         \
    ,m_cbWildCard               (this, ResId(CB_WILDCARD))          \
    ,m_cbRegular                (this, ResId(CB_REGULAR))           \
    ,m_cbApprox                 (this, ResId(CB_APPROX))            \
    ,m_pbApproxSettings         (this, ResId(PB_APPROXSETTINGS))    \
    ,m_aHalfFullFormsCJK        (this, ResId(CB_HALFFULLFORMS))     \
    ,m_aSoundsLikeCJK           (this, ResId(CB_SOUNDSLIKECJK))     \
    ,m_aSoundsLikeCJKSettings   (this, ResId(PB_SOUNDSLIKESETTINGS))    \
    ,m_flState                  (this, ResId(FL_STATE))             \
    ,m_ftRecordLabel            (this, ResId(FT_RECORDLABEL))       \
    ,m_ftRecord                 (this, ResId(FT_RECORD))            \
    ,m_ftHint                   (this, ResId(FT_HINT))              \
    ,m_pbSearchAgain            (this, ResId(PB_SEARCH))            \
    ,m_pbClose                  (this, ResId(1))                    \
    ,m_pbHelp                   (this, ResId(1))                    \
    ,m_pConfig( NULL )                                              \
    ,m_pPreSearchFocus( NULL )

//------------------------------------------------------------------------
FmSearchDialog::FmSearchDialog(Window* pParent, const Reference< XResultSet >& xCursor, const UniString& strVisibleFields,
    const UniString& sInitialText, const Reference< XNumberFormatsSupplier >& xFormatSupplier, FMSEARCH_MODE eMode)
    :ModalDialog(pParent, SVX_RES(RID_SVXDLG_SEARCHFORM))
    ,INIT_MEMBERS()
{
    // hier muss ich die ListBox fuer die Kontextauswahl entfernen :
    sal_Int32 nUpper = m_lbForm.GetPosPixel().Y();
    sal_Int32 nDifference = m_rbAllFields.GetPosPixel().Y() - nUpper;

    // einige Controls muessen nach oben verschoben werden
    Control* pFieldsToMove[] = { &m_rbAllFields, &m_rbSingleField, &m_lbField, &m_flOptions, &m_ftPosition, &m_lbPosition,
            &m_cbUseFormat, &m_cbCase, &m_cbBackwards, &m_cbStartOver, &m_cbWildCard, &m_cbRegular, &m_cbApprox,
            &m_pbApproxSettings, &m_aHalfFullFormsCJK, &m_aSoundsLikeCJK, &m_aSoundsLikeCJKSettings,
            &m_flState, &m_ftRecordLabel, &m_ftRecord, &m_ftHint };
    implMoveControls(pFieldsToMove, sizeof(pFieldsToMove)/sizeof(pFieldsToMove[0]), nDifference, &m_flWhere);

    Point pt = m_rbAllFields.GetPosPixel();
    pt.X() = m_ftForm.GetPosPixel().X();
    m_rbAllFields.SetPosPixel( pt );
    pt = m_rbSingleField.GetPosPixel();
    pt.X() = m_ftForm.GetPosPixel().X();
    m_rbSingleField.SetPosPixel( pt );

    // und die beiden ueberfluessigen muessen weg
    m_ftForm.Hide();
    m_lbForm.Hide();

    m_pSearchEngine = new FmSearchEngine(
        ::comphelper::getProcessServiceFactory(), xCursor, strVisibleFields, xFormatSupplier, eMode );
    initCommon( xCursor );
    Init(strVisibleFields, sInitialText);
}

//------------------------------------------------------------------------
void FmSearchDialog::initCommon( const Reference< XResultSet >& _rxCursor )
{
    // init the engine
    DBG_ASSERT( m_pSearchEngine, "FmSearchDialog::initCommon: have no engine!" );
    m_pSearchEngine->SetProgressHandler(LINK(this, FmSearchDialog, OnSearchProgress));

    // some layout changes according to available CJK options
    SvtCJKOptions aCJKOptions;
    if (!aCJKOptions.IsJapaneseFindEnabled())
    {
        sal_Int32 nUpper = m_pbApproxSettings.GetPosPixel().Y();
        sal_Int32 nDifference = m_aSoundsLikeCJKSettings.GetPosPixel().Y() - nUpper;

        // hide the options for the japanese search
        Control* pFieldsToMove[] = { &m_flState, &m_ftRecordLabel, &m_ftRecord, &m_ftHint };
        implMoveControls(pFieldsToMove, sizeof(pFieldsToMove)/sizeof(pFieldsToMove[0]), nDifference, &m_flOptions);

        m_aSoundsLikeCJK.Hide();
        m_aSoundsLikeCJKSettings.Hide();
    }

    if (!aCJKOptions.IsCJKFontEnabled())
    {
        m_aHalfFullFormsCJK.Hide();

        // never ignore the width (ignoring is expensive) if the option is not available at all
        // 04.12.2001 - 91973 - fs@openoffice.org
        m_pSearchEngine->SetIgnoreWidthCJK( sal_False );
    }

    // some initial record texts
    m_ftRecord.SetText( String::CreateFromInt32( _rxCursor->getRow() ) );
    m_pbClose.SetHelpText(String());
}

//------------------------------------------------------------------------
FmSearchDialog::FmSearchDialog(Window* pParent, const UniString& sInitialText, const UniString& strContexts, sal_Int16 nInitialContext,
    const Link& lnkContextSupplier, FMSEARCH_MODE eMode)
    :ModalDialog(pParent, SVX_RES(RID_SVXDLG_SEARCHFORM))
    ,INIT_MEMBERS()
    ,m_lnkContextSupplier(lnkContextSupplier)
{
    DBG_ASSERT(m_lnkContextSupplier.IsSet(), "FmSearchDialog::FmSearchDialog : have no ContextSupplier !");

    // erst mal die Informationen fuer den initialen Kontext
    FmSearchContext fmscInitial;
    fmscInitial.nContext = nInitialContext;
    m_lnkContextSupplier.Call(&fmscInitial);
    DBG_ASSERT(fmscInitial.xCursor.is(), "FmSearchDialog::FmSearchDialog : invalid data supplied by ContextSupplier !");
    DBG_ASSERT(fmscInitial.strUsedFields.GetTokenCount(';') == (xub_StrLen)fmscInitial.arrFields.size(),
        "FmSearchDialog::FmSearchDialog : invalid data supplied by ContextSupplied !");
#if (OSL_DEBUG_LEVEL > 1) || DBG_UTIL
    for (sal_Int32 i=0; i<(sal_Int32)fmscInitial.arrFields.size(); ++i)
        DBG_ASSERT(fmscInitial.arrFields.at(i).is(), "FmSearchDialog::FmSearchDialog : invalid data supplied by ContextSupplier !");
#endif // (OSL_DEBUG_LEVEL > 1) || DBG_UTIL

    for (xub_StrLen j=0; j<strContexts.GetTokenCount(';'); ++j)
    {
        m_arrContextFields.push_back(String());
        m_lbForm.InsertEntry(strContexts.GetToken(j, ';'));
    }
    m_lbForm.SelectEntryPos(nInitialContext);

    m_lbForm.SetSelectHdl(LINK(this, FmSearchDialog, OnContextSelection));

    if (m_arrContextFields.size() == 1)
    {   // remove the context selection listbox and rearrange the controls accordingly
        sal_Int32 nUpper = m_lbForm.GetPosPixel().Y();
        sal_Int32 nDifference = m_rbAllFields.GetPosPixel().Y() - nUpper;

        // move all controls below the affected ones up
        Control* pFieldsToMove[] = { &m_rbAllFields, &m_rbSingleField, &m_lbField, &m_flOptions, &m_ftPosition, &m_lbPosition,
                &m_cbUseFormat, &m_cbCase, &m_cbBackwards, &m_cbStartOver, &m_cbWildCard, &m_cbRegular, &m_cbApprox,
                &m_pbApproxSettings, &m_aHalfFullFormsCJK, &m_aSoundsLikeCJK, &m_aSoundsLikeCJKSettings,
                &m_flState, &m_ftRecordLabel, &m_ftRecord, &m_ftHint };

        implMoveControls(pFieldsToMove, sizeof(pFieldsToMove)/sizeof(pFieldsToMove[0]), nDifference, &m_flWhere);

        Point pt = m_rbAllFields.GetPosPixel();
        pt.X() = m_ftForm.GetPosPixel().X();
        m_rbAllFields.SetPosPixel( pt );
        pt = m_rbSingleField.GetPosPixel();
        pt.X() = m_ftForm.GetPosPixel().X();
        m_rbSingleField.SetPosPixel( pt );

        // hide dispensable controls
        m_ftForm.Hide();
        m_lbForm.Hide();
    }

    m_pSearchEngine = new FmSearchEngine(
        ::comphelper::getProcessServiceFactory(), fmscInitial.xCursor, fmscInitial.strUsedFields, fmscInitial.arrFields, eMode );
    initCommon( fmscInitial.xCursor );

    if (fmscInitial.sFieldDisplayNames.Len() != 0)
    {   // use the display names if supplied
        DBG_ASSERT(fmscInitial.sFieldDisplayNames.GetTokenCount() == fmscInitial.strUsedFields.GetTokenCount(),
            "FmSearchDialog::FmSearchDialog : invalid initial context description !");
        Init(fmscInitial.sFieldDisplayNames, sInitialText);
    }
    else
        Init(fmscInitial.strUsedFields, sInitialText);
}

//------------------------------------------------------------------------
void FmSearchDialog::implMoveControls(Control** _ppControls, sal_Int32 _nControls, sal_Int32 _nUp, Control* _pToResize)
{
    for (sal_Int32 i=0; i<_nControls; ++i)
    {
        Point pt = _ppControls[i]->GetPosPixel();
        pt.Y() -= _nUp;
        _ppControls[i]->SetPosPixel(pt);
    }

    // resize myself
    Size sz = GetSizePixel();
    sz.Height() -= _nUp;
    SetSizePixel(sz);
}

//------------------------------------------------------------------------
FmSearchDialog::~FmSearchDialog()
{
    if (m_aDelayedPaint.IsActive())
        m_aDelayedPaint.Stop();

    SaveParams();

    if (m_pConfig)
    {
        delete m_pConfig;
        m_pConfig = NULL;
    }

    delete m_pSearchEngine;
}

//------------------------------------------------------------------------
void FmSearchDialog::Init(const UniString& strVisibleFields, const UniString& sInitialText)
{
    // die Initialisierung all der Controls
    m_rbSearchForText.SetClickHdl(LINK(this, FmSearchDialog, OnClickedFieldRadios));
    m_rbSearchForNull.SetClickHdl(LINK(this, FmSearchDialog, OnClickedFieldRadios));
    m_rbSearchForNotNull.SetClickHdl(LINK(this, FmSearchDialog, OnClickedFieldRadios));

    m_rbAllFields.SetClickHdl(LINK(this, FmSearchDialog, OnClickedFieldRadios));
    m_rbSingleField.SetClickHdl(LINK(this, FmSearchDialog, OnClickedFieldRadios));

    m_pbSearchAgain.SetClickHdl(LINK(this, FmSearchDialog, OnClickedSearchAgain));
    m_pbApproxSettings.SetClickHdl(LINK(this, FmSearchDialog, OnClickedSpecialSettings));
    m_aSoundsLikeCJKSettings.SetClickHdl(LINK(this, FmSearchDialog, OnClickedSpecialSettings));

    m_lbPosition.SetSelectHdl(LINK(this, FmSearchDialog, OnPositionSelected));
    m_lbField.SetSelectHdl(LINK(this, FmSearchDialog, OnFieldSelected));

    m_cmbSearchText.SetModifyHdl(LINK(this, FmSearchDialog, OnSearchTextModified));
    m_cmbSearchText.EnableAutocomplete(sal_False);

    m_cbUseFormat.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_cbBackwards.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_cbStartOver.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_cbCase.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_cbWildCard.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_cbRegular.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_cbApprox.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_aHalfFullFormsCJK.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));
    m_aSoundsLikeCJK.SetToggleHdl(LINK(this, FmSearchDialog, OnCheckBoxToggled));

    // die Listboxen fuellen
    // die Methoden des Feldvergleiches
    UniString strListBoxEntries(SVX_RES(RID_STR_RECORDSEARCH_METHODS));
    xub_StrLen i;
    for (i=0; i<strListBoxEntries.GetTokenCount(); ++i)
        m_lbPosition.InsertEntry(strListBoxEntries.GetToken(i));
    m_lbPosition.SelectEntryPos(MATCHING_ANYWHERE);

    // die Feld-Listbox
    for (i=0; i<strVisibleFields.GetTokenCount(';'); ++i)
        m_lbField.InsertEntry(strVisibleFields.GetToken(i, ';'));


    m_pConfig = new FmSearchConfigItem;
    LoadParams();

    m_cmbSearchText.SetText(sInitialText);
    // wenn die Edit-Zeile den Text veraendert hat (weil er zum Beispiel Steuerzeichen enthielt, wie das bei Memofeldern der Fall
    // sein kann), nehme ich einen leeren UniString
    UniString sRealSetText = m_cmbSearchText.GetText();
    if (!sRealSetText.Equals(sInitialText))
        m_cmbSearchText.SetText(UniString());
    LINK(this, FmSearchDialog, OnSearchTextModified).Call(&m_cmbSearchText);

    // initial die ganzen UI-Elemente fuer die Suche an
    m_aDelayedPaint.SetTimeoutHdl(LINK(this, FmSearchDialog, OnDelayedPaint));
    m_aDelayedPaint.SetTimeout(500);
    EnableSearchUI(sal_True);

    if ( m_rbSearchForText.IsChecked() )
        m_cmbSearchText.GrabFocus();

    FreeResource();
}

//------------------------------------------------------------------------
sal_Bool FmSearchDialog::Close()
{
    // Wenn der Close-Button disabled ist und man im Dialog ESC drueckt, dann wird irgendwo vom Frame trotzdem Close aufgerufen,
    // was ich allerdings nicht will, wenn ich gerade mitten in einer (eventuell in einem extra Thread laufenden) Suche bin
    if (!m_pbClose.IsEnabled())
        return sal_False;
    return ModalDialog::Close();
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnClickedFieldRadios, Button*, pButton)
{
    if ((pButton == &m_rbSearchForText) || (pButton == &m_rbSearchForNull) || (pButton == &m_rbSearchForNotNull))
    {
        EnableSearchForDependees(sal_True);
    }
    else
        // die Feldlistbox entsprechend en- oder disablen
        if (pButton == &m_rbSingleField)
        {
            m_lbField.Enable();
            m_pSearchEngine->RebuildUsedFields(m_lbField.GetSelectEntryPos());
        }
        else
        {
            m_lbField.Disable();
            m_pSearchEngine->RebuildUsedFields(-1);
        }

    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnClickedSearchAgain, Button*, pButton)
{
    if (m_pbClose.IsEnabled())
    {   // der Button hat die Funktion 'Suchen'
        UniString strThisRoundText = m_cmbSearchText.GetText();
        // zur History dazu
        m_cmbSearchText.RemoveEntry(strThisRoundText);
        m_cmbSearchText.InsertEntry(strThisRoundText, 0);
            // das Remove/Insert stellt a) sicher, dass der UniString nicht zweimal auftaucht, b), dass die zuletzt gesuchten Strings am
            // Anfang stehen
        // und die Listenlaenge beschraenken
        while (m_cmbSearchText.GetEntryCount() > MAX_HISTORY_ENTRIES)
            m_cmbSearchText.RemoveEntry(m_cmbSearchText.GetEntryCount()-1);

        // den 'Ueberlauf'-Hint rausnehmen
        m_ftHint.SetText(UniString());
        m_ftHint.Invalidate();

        if (m_cbStartOver.IsChecked())
        {
            m_cbStartOver.Check(sal_False);
            EnableSearchUI(sal_False);
            if (m_rbSearchForText.IsChecked())
                m_pSearchEngine->StartOver(strThisRoundText);
            else
                m_pSearchEngine->StartOverSpecial(m_rbSearchForNull.IsChecked());
        }
        else
        {
            EnableSearchUI(sal_False);
            if (m_rbSearchForText.IsChecked())
                m_pSearchEngine->SearchNext(strThisRoundText);
            else
                m_pSearchEngine->SearchNextSpecial(m_rbSearchForNull.IsChecked());
        }
    }
    else
    {   // der Button hat die Fukntion 'Abbrechen'
        DBG_ASSERT(m_pSearchEngine->GetSearchMode() != SM_BRUTE, "FmSearchDialog, OnClickedSearchAgain : falscher Modus !");
            // der CancelButton wird normalerweise nur disabled, wenn ich in einem Thread oder mit Reschedule arbeite
        m_pSearchEngine->CancelSearch();
            // mein ProgressHandler wird gerufen, wenn es wirklich zu Ende ist, das hier war nur eine Anforderung
    }
    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnClickedSpecialSettings, Button*, pButton )
{
    if (&m_pbApproxSettings == pButton)
    {
        //CHINA001 SvxSearchSimilarityDialog dlgLevSettings(this, m_pSearchEngine->GetLevRelaxed(), m_pSearchEngine->GetLevOther(),
        //CHINA001  m_pSearchEngine->GetLevShorter(), m_pSearchEngine->GetLevLonger());
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if(pFact)
        {
            AbstractSvxSearchSimilarityDialog* dlgLevSettings = pFact->CreateSvxSearchSimilarityDialog( this, m_pSearchEngine->GetLevRelaxed(), m_pSearchEngine->GetLevOther(),
                            m_pSearchEngine->GetLevShorter(), m_pSearchEngine->GetLevLonger(), ResId(RID_SVXDLG_SEARCHATTR) );
            DBG_ASSERT(dlgLevSettings, "Dialogdiet fail!");//CHINA001
            if (dlgLevSettings->Execute() == RET_OK) //CHINA001 if (dlgLevSettings.Execute() == RET_OK)
            {
                m_pSearchEngine->SetLevRelaxed(dlgLevSettings->IsRelaxed()); //CHINA001 m_pSearchEngine->SetLevRelaxed(dlgLevSettings.IsRelaxed());
                m_pSearchEngine->SetLevOther(dlgLevSettings->GetOther()); //CHINA001 m_pSearchEngine->SetLevOther(dlgLevSettings.GetOther());
                m_pSearchEngine->SetLevShorter(dlgLevSettings->GetShorter()); //CHINA001 m_pSearchEngine->SetLevShorter(dlgLevSettings.GetShorter());
                m_pSearchEngine->SetLevLonger(dlgLevSettings->GetLonger()); //CHINA001 m_pSearchEngine->SetLevLonger(dlgLevSettings.GetLonger());
            }
            delete dlgLevSettings; //add by CHINA001
        }
    }
    else if (&m_aSoundsLikeCJKSettings == pButton)
    {
        SfxItemSet aSet( SFX_APP()->GetPool() );
        //CHINA001 SvxJSearchOptionsDialog aDlg( this, aSet, RID_SVXPAGE_JSEARCH_OPTIONS, m_pSearchEngine->GetTransliterationFlags() );
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if(pFact)
        {
            AbstractSvxJSearchOptionsDialog* aDlg = pFact->CreateSvxJSearchOptionsDialog( this, aSet, RID_SVXPAGE_JSEARCH_OPTIONS, m_pSearchEngine->GetTransliterationFlags(), ResId(RID_SVXPAGE_JSEARCH_OPTIONS) );
            DBG_ASSERT(aDlg, "Dialogdiet fail!");//CHINA001
            aDlg->Execute(); //CHINA001 aDlg.Execute();


            INT32 nFlags = aDlg->GetTransliterationFlags(); //CHINA001 INT32 nFlags = aDlg.GetTransliterationFlags();
            m_pSearchEngine->SetTransliterationFlags(nFlags);

            m_cbCase.Check(m_pSearchEngine->GetCaseSensitive());
            OnCheckBoxToggled( &m_cbCase );
            m_aHalfFullFormsCJK.Check( !m_pSearchEngine->GetIgnoreWidthCJK() );
            OnCheckBoxToggled( &m_aHalfFullFormsCJK );
            delete aDlg; //add for CHINA001
        }
    }

    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnSearchTextModified, ComboBox*, pEdit)
{
    if ((m_cmbSearchText.GetText().Len() != 0) || !m_rbSearchForText.IsChecked())
        m_pbSearchAgain.Enable();
    else
        m_pbSearchAgain.Disable();

    m_pSearchEngine->InvalidatePreviousLoc();
    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnPositionSelected, ListBox*, pBox)
{
    DBG_ASSERT(pBox->GetSelectEntryCount() == 1, "FmSearchDialog::OnMethodSelected : unerwartet : nicht genau ein Eintrag selektiert !");

    m_pSearchEngine->SetPosition(m_lbPosition.GetSelectEntryPos());
    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnFieldSelected, ListBox*, pBox)
{
    DBG_ASSERT(pBox->GetSelectEntryCount() == 1, "FmSearchDialog::OnFieldSelected : unerwartet : nicht genau ein Eintrag selektiert !");

    m_pSearchEngine->RebuildUsedFields(m_rbAllFields.IsChecked() ? -1 : (sal_Int16)m_lbField.GetSelectEntryPos());
        // ruft auch m_pSearchEngine->InvalidatePreviousLoc auf

    sal_Int16 nCurrentContext = m_lbForm.GetSelectEntryPos();
    if (nCurrentContext != LISTBOX_ENTRY_NOTFOUND)
        m_arrContextFields[nCurrentContext] = UniString(m_lbField.GetSelectEntry());
    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnCheckBoxToggled, CheckBox*, pBox)
{
    sal_Bool bChecked = pBox->IsChecked();

    // Formatter oder case -> an die Engine weiterreichen
    if (pBox == &m_cbUseFormat)
        m_pSearchEngine->SetFormatterUsing(bChecked);
    else if (pBox == &m_cbCase)
        m_pSearchEngine->SetCaseSensitive(bChecked);
    // Richtung -> weiterreichen und Checkbox-Text fuer StartOver neu setzen
    else if (pBox == &m_cbBackwards)
    {
        UniString strButtonText(SVX_RES(RID_STR_RECORDSEARCH_BUTTONS));
        m_cbStartOver.SetText(strButtonText.GetToken(bChecked ? 1 : 0));

        m_pSearchEngine->SetDirection(!bChecked);
    }
    // Aehnlichkeitssuche oder regulaerer Ausdruck
    else if ((pBox == &m_cbApprox) || (pBox == &m_cbRegular) || (pBox == &m_cbWildCard))
    {
        // die beiden jeweils anderen Boxes disablen oder enablen
        CheckBox* pBoxes[] = { &m_cbWildCard, &m_cbRegular, &m_cbApprox };
        for (int i=0; i<sizeof(pBoxes)/sizeof(CheckBox*); ++i)
            if (pBoxes[i] != pBox)
                if (bChecked)
                    pBoxes[i]->Disable();
                else
                    pBoxes[i]->Enable();

        // an die Engine weiterreichen
        m_pSearchEngine->SetWildcard(m_cbWildCard.IsEnabled() ? m_cbWildCard.IsChecked() : sal_False);
        m_pSearchEngine->SetRegular(m_cbRegular.IsEnabled() ? m_cbRegular.IsChecked() : sal_False);
        m_pSearchEngine->SetLevenshtein(m_cbApprox.IsEnabled() ? m_cbApprox.IsChecked() : sal_False);
            // (Boxes, die disabled sind, muessen als sal_False an die Engine gehen)

        // die Position-Listbox anpassen (ist bei Wildcard-Suche nicht erlaubt)
        if (pBox == &m_cbWildCard)
            if (bChecked)
            {
                m_ftPosition.Disable();
                m_lbPosition.Disable();
            }
            else
            {
                m_ftPosition.Enable();
                m_lbPosition.Enable();
            }

        // und den Button fuer die Aehnlichkeitssuche
        if (pBox == &m_cbApprox)
            if (bChecked)
                m_pbApproxSettings.Enable();
            else
                m_pbApproxSettings.Disable();
    }
    else if (pBox == &m_aHalfFullFormsCJK)
    {
        // forward to the search engine
        m_pSearchEngine->SetIgnoreWidthCJK( !bChecked );
    }
    else if (pBox == &m_aSoundsLikeCJK)
    {
        m_aSoundsLikeCJKSettings.Enable(bChecked);

        // two other buttons which depend on this one
        sal_Bool bEnable =  (   m_rbSearchForText.IsChecked()
                            &&  !m_aSoundsLikeCJK.IsChecked()
                            )
                         || !SvtCJKOptions().IsJapaneseFindEnabled();
        m_cbCase.Enable(bEnable);
        m_aHalfFullFormsCJK.Enable(bEnable);

        // forward to the search engine
        m_pSearchEngine->SetTransliteration( bChecked );
    }

    return 0;
}

//------------------------------------------------------------------------
void FmSearchDialog::InitContext(sal_Int16 nContext)
{
    FmSearchContext fmscContext;
    fmscContext.nContext = nContext;

    sal_uInt32 nResult = m_lnkContextSupplier.Call(&fmscContext);
    DBG_ASSERT(nResult > 0, "FmSearchDialog::InitContext : ContextSupplier didn't give me any controls !");

    // packen wir zuerst die Feld-Namen in die entsprechende Listbox
    m_lbField.Clear();

    if (fmscContext.sFieldDisplayNames.Len() != 0)
    {
        // use the display names if supplied
        DBG_ASSERT(fmscContext.sFieldDisplayNames.GetTokenCount() == fmscContext.strUsedFields.GetTokenCount(),
            "FmSearchDialog::InitContext : invalid context description supplied !");
        for (xub_StrLen i=0; i<fmscContext.sFieldDisplayNames.GetTokenCount(); ++i)
            m_lbField.InsertEntry(fmscContext.sFieldDisplayNames.GetToken(i));
    }
    else
        // else use the field names
        for (xub_StrLen i=0; i<fmscContext.strUsedFields.GetTokenCount(); ++i)
            m_lbField.InsertEntry(fmscContext.strUsedFields.GetToken(i));

    if (nContext < (sal_Int32)m_arrContextFields.size() && m_arrContextFields[nContext].Len())
    {
        m_lbField.SelectEntry(m_arrContextFields[nContext]);
    }
    else
    {
        m_lbField.SelectEntryPos(0);
        if (m_rbSingleField.IsChecked() && (m_lbField.GetEntryCount() > 1))
            m_lbField.GrabFocus();
    }

    // dann geben wir der Engine Bescheid
    m_pSearchEngine->SwitchToContext(fmscContext.xCursor, fmscContext.strUsedFields, fmscContext.arrFields,
        m_rbAllFields.IsChecked() ? -1 : 0);

    // und die Position des neuen Cursors anzeigen
    m_ftRecord.SetText(String::CreateFromInt32(fmscContext.xCursor->getRow()));
}

//------------------------------------------------------------------------
IMPL_LINK( FmSearchDialog, OnContextSelection, ListBox*, pBox)
{
    InitContext(pBox->GetSelectEntryPos());
    return 0L;
}

//------------------------------------------------------------------------
void FmSearchDialog::EnableSearchUI(sal_Bool bEnable)
{
    // wenn die Controls disabled werden sollen, schalte ich mal eben kurz ihr Paint aus und verzoegert wieder an
    if (!bEnable)
        EnableControlPaint(sal_False);
    else
    {   // beim Enablen teste ich, ob der Timer fuer das delayed paint aktiv ist und stoppe ihn wenn noetig
        if (m_aDelayedPaint.IsActive())
            m_aDelayedPaint.Stop();
    }
    // (das ganze geht unten noch weiter)
    // diese kleine Verrenkung fuehrt hoffentlich dazu, dass es nicht flackert, wenn man die SearchUI schnell hintereinander
    // aus- und wieder einschaltet (wie das bei einem kurzen Suchvorgang zwangslaeufig der Fall ist)

    if ( !bEnable )
    {
        // if one of my children has the focus, remember it
        // 104332 - 2002-10-17 - fs@openoffice.org
        Window* pFocusWindow = Application::GetFocusWindow( );
        if ( pFocusWindow && IsChild( pFocusWindow ) )
            m_pPreSearchFocus = pFocusWindow;
        else
            m_pPreSearchFocus = NULL;
    }

    // der Suchen-Button hat einen Doppelfunktion, seinen Text entsprechend anpassen
    UniString strButtonText(SVX_RES(RID_STR_RECORDSEARCH_BUTTONS));
    m_pbSearchAgain.SetText(strButtonText.GetToken(bEnable ? 2 : 3));

    // jetzt Controls en- oder disablen
    if (m_pSearchEngine->GetSearchMode() != SM_BRUTE)
    {
        m_flSearchFor.Enable        (bEnable);
        m_rbSearchForText.Enable    (bEnable);
        m_rbSearchForNull.Enable    (bEnable);
        m_rbSearchForNotNull.Enable (bEnable);
        m_flWhere.Enable            (bEnable);
        m_ftForm.Enable             (bEnable);
        m_lbForm.Enable             (bEnable);
        m_rbAllFields.Enable        (bEnable);
        m_rbSingleField.Enable      (bEnable);
        m_lbField.Enable            (bEnable && m_rbSingleField.IsChecked());
        m_flOptions.Enable          (bEnable);
        m_cbBackwards.Enable        (bEnable);
        m_cbStartOver.Enable        (bEnable);
        m_pbClose.Enable            (bEnable);
        EnableSearchForDependees    (bEnable);

        if ( !bEnable )
        {   // this means we're preparing for starting a search
            // In this case, EnableSearchForDependees disabled the search button
            // But as we're about to use it for cancelling the search, we really need to enable it, again
            // 07.12.2001 - 95246 - fs@openoffice.org
            m_pbSearchAgain.Enable( sal_True );
        }
    }

    // und den Rest fuer das delayed paint
    if (!bEnable)
        m_aDelayedPaint.Start();
    else
        EnableControlPaint(sal_True);

    if ( bEnable )
    {   // restore focus
        // 104332 - 2002-10-17 - fs@openoffice.org
        if ( m_pPreSearchFocus )
        {
            m_pPreSearchFocus->GrabFocus();
            if ( WINDOW_EDIT == m_pPreSearchFocus->GetType() )
            {
                Edit* pEdit = static_cast< Edit* >( m_pPreSearchFocus );
                pEdit->SetSelection( Selection( 0, pEdit->GetText().Len() ) );
            }
        }
        m_pPreSearchFocus = NULL;
    }

}

//------------------------------------------------------------------------
void FmSearchDialog::EnableSearchForDependees(sal_Bool bEnable)
{
    sal_Bool bSearchingForText = m_rbSearchForText.IsChecked();
    m_pbSearchAgain.Enable(bEnable && (!bSearchingForText || (m_cmbSearchText.GetText().Len() != 0)));

    bEnable = bEnable && bSearchingForText;

    sal_Bool bEnableRedundants = !m_aSoundsLikeCJK.IsChecked() || !SvtCJKOptions().IsJapaneseFindEnabled();

    m_cmbSearchText.Enable          (bEnable);
    m_ftPosition.Enable             (bEnable && !m_cbWildCard.IsChecked());
    m_cbWildCard.Enable             (bEnable && !m_cbRegular.IsChecked() && !m_cbApprox.IsChecked());
    m_cbRegular.Enable              (bEnable && !m_cbWildCard.IsChecked() && !m_cbApprox.IsChecked());
    m_cbApprox.Enable               (bEnable && !m_cbWildCard.IsChecked() && !m_cbRegular.IsChecked());
    m_pbApproxSettings.Enable       (bEnable && m_cbApprox.IsChecked());
    m_aHalfFullFormsCJK.Enable      (bEnable && bEnableRedundants);
    m_aSoundsLikeCJK.Enable         (bEnable);
    m_aSoundsLikeCJKSettings.Enable (bEnable && m_aSoundsLikeCJK.IsChecked());
    m_lbPosition.Enable             (bEnable && !m_cbWildCard.IsChecked());
    m_cbUseFormat.Enable            (bEnable);
    m_cbCase.Enable                 (bEnable && bEnableRedundants);
}

//------------------------------------------------------------------------
void FmSearchDialog::EnableControlPaint(sal_Bool bEnable)
{
    Control* pAffectedControls[] = { &m_flSearchFor, &m_rbSearchForText, &m_cmbSearchText, &m_rbSearchForNull, &m_rbSearchForNotNull,
        &m_rbSearchForText, &m_flWhere, &m_rbAllFields, &m_rbSingleField, &m_lbField, &m_flOptions, &m_ftPosition, &m_lbPosition,
        &m_cbUseFormat, &m_cbCase, &m_cbBackwards, &m_cbStartOver, &m_cbWildCard, &m_cbRegular, &m_cbApprox, &m_pbApproxSettings,
        &m_pbSearchAgain, &m_pbClose };

    if (!bEnable)
        for (sal_Int16 i=0; i<sizeof(pAffectedControls)/sizeof(pAffectedControls[0]); ++i)
        {
            pAffectedControls[i]->SetUpdateMode(bEnable);
            pAffectedControls[i]->EnablePaint(bEnable);
        }
    else
        for (sal_Int16 i=0; i<sizeof(pAffectedControls)/sizeof(pAffectedControls[0]); ++i)
        {
            pAffectedControls[i]->EnablePaint(bEnable);
            pAffectedControls[i]->SetUpdateMode(bEnable);
        }
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnDelayedPaint, void*, EMPTYARG)
{
    EnableControlPaint(sal_True);
    return 0L;
}

//------------------------------------------------------------------------
void FmSearchDialog::OnFound(const ::com::sun::star::uno::Any& aCursorPos, sal_Int16 nFieldPos)
{
    FmFoundRecordInformation friInfo;
    friInfo.nContext = m_lbForm.GetSelectEntryPos();
        // wenn ich keine Suche in Kontexten mache, steht hier was ungueltiges drin, aber dann ist es auch egal
    friInfo.aPosition = aCursorPos;
    if (m_rbAllFields.IsChecked())
        friInfo.nFieldPos = nFieldPos;
    else
        friInfo.nFieldPos = m_lbField.GetSelectEntryPos();
        // das setzt natuerlich voraus, dass ich wirklich in dem Feld gesucht habe, dass in der Listbox ausgewaehlt ist,
        // genau das wird auch in RebuildUsedFields sichergestellt

    // dem Handler Bescheid sagen
    sal_Int16 nResult = (sal_Int16)m_lnkFoundHandler.Call(&friInfo);

    // und wieder Focus auf mich
    if (nResult & FM_SEARCH_GETFOCUS_ASYNC)
        Application::PostUserEvent(LINK(this, FmSearchDialog, AsyncGrabFocus));
    else
        m_cmbSearchText.GrabFocus();
}

//------------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, AsyncGrabFocus, void*, EMPTYTAG)
{
    m_cmbSearchText.GrabFocus();
    return 0;
}

//------------------------------------------------------------------------
IMPL_LINK(FmSearchDialog, OnSearchProgress, FmSearchProgress*, pProgress)
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );
        // diese eine Methode Thread-sicher machen (das ist ein Overkill, die ganze restliche Applikation dafuer zu blockieren,
        // aber im Augenblick haben wir kein anderes Sicherheitskonpzept)

    switch (pProgress->aSearchState)
    {
        case FmSearchProgress::STATE_PROGRESS:
            if (pProgress->bOverflow)
            {
                UniString strHint = SVX_RESSTR(RID_STR_SEARCH_OVERFLOW).GetToken(m_cbBackwards.IsChecked() ? 1 : 0);
                m_ftHint.SetText(strHint);
                m_ftHint.Invalidate();
            }

            m_ftRecord.SetText(String::CreateFromInt32(1 + pProgress->nCurrentRecord));
            m_ftRecord.Invalidate();
            break;

        case FmSearchProgress::STATE_PROGRESS_COUNTING:
            m_ftHint.SetText(SVX_RESSTR(RID_STR_SEARCH_COUNTING));
            m_ftHint.Invalidate();

            m_ftRecord.SetText(String::CreateFromInt32(pProgress->nCurrentRecord));
            m_ftRecord.Invalidate();
            break;

        case FmSearchProgress::STATE_SUCCESSFULL:
            OnFound(pProgress->aBookmark, (sal_Int16)pProgress->nFieldIndex);
            EnableSearchUI(sal_True);
            break;

        case FmSearchProgress::STATE_ERROR:
        case FmSearchProgress::STATE_NOTHINGFOUND:
        {
            sal_uInt16 nErrorId = (FmSearchProgress::STATE_ERROR == pProgress->aSearchState)
                ? RID_SVXERR_SEARCH_GENERAL_ERROR
                : RID_SVXERR_SEARCH_NORECORD;
            ErrorBox(this, SVX_RES(nErrorId)).Execute();
        }
            // KEIN break !
        case FmSearchProgress::STATE_CANCELED:
            EnableSearchUI(sal_True);
            if (m_lnkCanceledNotFoundHdl.IsSet())
            {
                FmFoundRecordInformation friInfo;
                friInfo.nContext = m_lbForm.GetSelectEntryPos();
                    // wenn ich keine Suche in Kontexten mache, steht hier was ungueltiges drin, aber dann ist es auch egal
                friInfo.aPosition = pProgress->aBookmark;
                m_lnkCanceledNotFoundHdl.Call(&friInfo);
            }
            break;
    }

    m_ftRecord.SetText(String::CreateFromInt32(1 + pProgress->nCurrentRecord));

    return 0L;
}

//------------------------------------------------------------------------
void FmSearchDialog::LoadParams()
{
    FmSearchParams aParams(m_pConfig->getParams());

    const ::rtl::OUString* pHistory     =                   aParams.aHistory.getConstArray();
    const ::rtl::OUString* pHistoryEnd  =   pHistory    +   aParams.aHistory.getLength();
    for (; pHistory != pHistoryEnd; ++pHistory)
        m_cmbSearchText.InsertEntry( *pHistory );

    // die Einstellungen nehme ich an meinen UI-Elementen vor und rufe dann einfach den entsprechenden Change-Handler auf,
    // dadurch werden die Daten an die SearchEngine weitergereicht und alle abhaengigen Enstellungen vorgenommen

    // aktuelles Feld
    sal_uInt16 nInitialField = m_lbField.GetEntryPos( String( aParams.sSingleSearchField ) );
    if (nInitialField == COMBOBOX_ENTRY_NOTFOUND)
        nInitialField = 0;
    m_lbField.SelectEntryPos(nInitialField);
    LINK(this, FmSearchDialog, OnFieldSelected).Call(&m_lbField);
    // alle/einzelnes Feld (NACH dem Selektieren des Feldes, da OnClickedFieldRadios dort einen gueltigen Eintrag erwartet)
    if (aParams.bAllFields)
    {
        m_rbSingleField.Check(sal_False);
        m_rbAllFields.Check(sal_True);
        LINK(this, FmSearchDialog, OnClickedFieldRadios).Call(&m_rbAllFields);
        // OnClickedFieldRadios ruft auch um RebuildUsedFields
    }
    else
    {
        m_rbAllFields.Check(sal_False);
        m_rbSingleField.Check(sal_True);
        LINK(this, FmSearchDialog, OnClickedFieldRadios).Call(&m_rbSingleField);
    }

    // Position im Feld
    m_lbPosition.SelectEntryPos(aParams.nPosition);
    LINK(this, FmSearchDialog, OnPositionSelected).Call(&m_lbPosition);

    // Feld-Formatierung/Case-Sensitivitaet/Richtung
    m_cbUseFormat.Check(aParams.bUseFormatter);
    m_cbCase.Check( aParams.isCaseSensitive() );
    m_cbBackwards.Check(aParams.bBackwards);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_cbUseFormat);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_cbCase);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_cbBackwards);

    m_aHalfFullFormsCJK.Check( !aParams.isIgnoreWidthCJK( ) );  // BEWARE: this checkbox has a inverse semantics!
    m_aSoundsLikeCJK.Check( aParams.bSoundsLikeCJK );
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_aHalfFullFormsCJK);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_aSoundsLikeCJK);

    // die drei Checkboxen fuer spezielle Sucharten
    // erst mal alle ruecksetzen
    m_cbWildCard.Check(sal_False);
    m_cbRegular.Check(sal_False);
    m_cbApprox.Check(sal_False);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_cbWildCard);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_cbRegular);
    LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(&m_cbApprox);

    // dann die richtige setzen
    CheckBox* pToCheck = NULL;
    if (aParams.bWildcard)
        pToCheck = &m_cbWildCard;
    if (aParams.bRegular)
        pToCheck = &m_cbRegular;
    if (aParams.bApproxSearch)
        pToCheck = &m_cbApprox;
    if (aParams.bSoundsLikeCJK)
        pToCheck = &m_aSoundsLikeCJK;
    if (pToCheck)
    {
        pToCheck->Check(sal_True);
        LINK(this, FmSearchDialog, OnCheckBoxToggled).Call(pToCheck);
    }

    // die Levenshtein-Parameter direkt an der SearchEngine setzen
    m_pSearchEngine->SetLevRelaxed(aParams.bLevRelaxed);
    m_pSearchEngine->SetLevOther(aParams.nLevOther);
    m_pSearchEngine->SetLevShorter(aParams.nLevShorter);
    m_pSearchEngine->SetLevLonger(aParams.nLevLonger);

    m_pSearchEngine->SetTransliterationFlags( aParams.getTransliterationFlags( ) );

    m_rbSearchForText.Check(sal_False);
    m_rbSearchForNull.Check(sal_False);
    m_rbSearchForNotNull.Check(sal_False);
    switch (aParams.nSearchForType)
    {
        case 1: m_rbSearchForNull.Check(sal_True); break;
        case 2: m_rbSearchForNotNull.Check(sal_True); break;
        default: m_rbSearchForText.Check(sal_True); break;
    }
    LINK(this, FmSearchDialog, OnClickedFieldRadios).Call(&m_rbSearchForText);
}

//------------------------------------------------------------------------
void FmSearchDialog::SaveParams() const
{
    if (!m_pConfig)
        return;

    FmSearchParams aCurrentSettings;

    aCurrentSettings.aHistory.realloc( m_cmbSearchText.GetEntryCount() );
    ::rtl::OUString* pHistory = aCurrentSettings.aHistory.getArray();
    for (sal_uInt16 i=0; i<m_cmbSearchText.GetEntryCount(); ++i, ++pHistory)
        *pHistory = m_cmbSearchText.GetEntry(i);

    aCurrentSettings.sSingleSearchField         = m_lbField.GetSelectEntry();
    aCurrentSettings.bAllFields                 = m_rbAllFields.IsChecked();
    aCurrentSettings.nPosition                  = m_pSearchEngine->GetPosition();
    aCurrentSettings.bUseFormatter              = m_pSearchEngine->GetFormatterUsing();
    aCurrentSettings.setCaseSensitive           ( m_pSearchEngine->GetCaseSensitive() );
    aCurrentSettings.bBackwards                 = !m_pSearchEngine->GetDirection();
    aCurrentSettings.bWildcard                  = m_pSearchEngine->GetWildcard();
    aCurrentSettings.bRegular                   = m_pSearchEngine->GetRegular();
    aCurrentSettings.bApproxSearch              = m_pSearchEngine->GetLevenshtein();
    aCurrentSettings.bLevRelaxed                = m_pSearchEngine->GetLevRelaxed();
    aCurrentSettings.nLevOther                  = m_pSearchEngine->GetLevOther();
    aCurrentSettings.nLevShorter                = m_pSearchEngine->GetLevShorter();
    aCurrentSettings.nLevLonger                 = m_pSearchEngine->GetLevLonger();

    aCurrentSettings.bSoundsLikeCJK             = m_pSearchEngine->GetTransliteration();
    aCurrentSettings.setTransliterationFlags    ( m_pSearchEngine->GetTransliterationFlags() );

    if (m_rbSearchForNull.IsChecked())
        aCurrentSettings.nSearchForType = 1;
    else if (m_rbSearchForNotNull.IsChecked())
        aCurrentSettings.nSearchForType = 2;
    else
        aCurrentSettings.nSearchForType = 0;

    m_pConfig->setParams( aCurrentSettings );
}

