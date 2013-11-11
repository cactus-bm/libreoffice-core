/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include <sfx2/dispatch.hxx>
#include <svl/zforlist.hxx>
#include <svl/undo.hxx>
#include <boost/random.hpp>

#include "formulacell.hxx"
#include "rangelst.hxx"
#include "scitems.hxx"
#include "docsh.hxx"
#include "document.hxx"
#include "uiitems.hxx"
#include "reffact.hxx"
#include "strload.hxx"
#include "random.hxx"
#include "docfunc.hxx"
#include "StatisticsDialogs.hrc"
#include "TableFillingAndNavigationTools.hxx"

#include "DescriptiveStatisticsDialog.hxx"

namespace
{

struct StatisticCalculation {
    sal_Int16   aCalculationNameId;
    const char* aFormula;
};

static const StatisticCalculation lclCalcDefinitions[] =
{
    { STRID_CALC_MEAN,           "=AVERAGE(%RANGE%)" },
    { STRID_CALC_STD_ERROR,      "=SQRT(VAR(%RANGE%)/COUNT(%RANGE%))"},
    { STRID_CALC_MODE,           "=MODE(%RANGE%)"},
    { STRID_CALC_MEDIAN,         "=MEDIAN(%RANGE%)"},
    { STRID_CALC_VARIANCE,       "=VAR(%RANGE%)"},
    { STRID_CALC_STD_DEVIATION,  "=STDEV(%RANGE%)"},
    { STRID_CALC_KURTOSIS,       "=KURT(%RANGE%)"},
    { STRID_CALC_SKEWNESS,       "=SKEW(%RANGE%)"},
    { STRID_CALC_RANGE,          "=MAX(%RANGE%)-MIN(%RANGE%)"},
    { STRID_CALC_MIN,            "=MIN(%RANGE%)"},
    { STRID_CALC_MAX,            "=MAX(%RANGE%)"},
    { STRID_CALC_SUM,            "=SUM(%RANGE%)"},
    { STRID_CALC_COUNT,          "=COUNT(%RANGE%)" },
    { 0,                         NULL }
};

static const OUString strWildcardRange("%RANGE%");

}

ScDescriptiveStatisticsDialog::ScDescriptiveStatisticsDialog(
                    SfxBindings* pSfxBindings, SfxChildWindow* pChildWindow,
                    Window* pParent, ScViewData* pViewData ) :
    ScStatisticsInputOutputDialog(
            pSfxBindings, pChildWindow, pParent, pViewData,
            "DescriptiveStatisticsDialog", "modules/scalc/ui/descriptivestatisticsdialog.ui" )
{}

ScDescriptiveStatisticsDialog::~ScDescriptiveStatisticsDialog()
{}

sal_Bool ScDescriptiveStatisticsDialog::Close()
{
    return DoClose( ScDescriptiveStatisticsDialogWrapper::GetChildWindowId() );
}

void ScDescriptiveStatisticsDialog::CalculateInputAndWriteToOutput( )
{
    OUString aUndo(SC_STRLOAD(RID_STATISTICS_DLGS, STR_DESCRIPTIVE_STATISTICS_UNDO_NAME));
    ScDocShell* pDocShell = mViewData->GetDocShell();
    svl::IUndoManager* pUndoManager = pDocShell->GetUndoManager();
    pUndoManager->EnterListAction( aUndo, aUndo );

    OUString aReferenceString;

    AddressWalkerWriter aOutput(mOutputAddress, pDocShell, mDocument);
    FormulaTemplate aTemplate(mDocument, mAddressDetails);

    SCROW inTab = mInputRange.aStart.Tab();

    for(sal_Int32 i = 0; lclCalcDefinitions[i].aFormula != NULL; i++)
    {
        OUString aLabel(SC_STRLOAD(RID_STATISTICS_DLGS, lclCalcDefinitions[i].aCalculationNameId));
        aOutput.writeString(aLabel);
        aOutput.nextRow();
    }
    aOutput.nextColumn();

    for (SCCOL inCol = mInputRange.aStart.Col(); inCol <= mInputRange.aEnd.Col(); inCol++)
    {
        aOutput.resetRow();

        ScRange aColumnRange(
            ScAddress(inCol, mInputRange.aStart.Row(), inTab),
            ScAddress(inCol, mInputRange.aEnd.Row(), inTab)
        );

        for(sal_Int32 i = 0; lclCalcDefinitions[i].aFormula != NULL; i++)
        {
            aTemplate.setTemplate(lclCalcDefinitions[i].aFormula);
            aTemplate.applyRange(strWildcardRange, aColumnRange);
            aOutput.writeFormula(aTemplate.getTemplate());
            aOutput.nextRow();
        }
        aOutput.nextColumn();
    }

    pUndoManager->LeaveListAction();
    ScRange aOutputRange(aOutput.mMinimumAddress, aOutput.mMaximumAddress);
    pDocShell->PostPaint( aOutputRange, PAINT_GRID );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
