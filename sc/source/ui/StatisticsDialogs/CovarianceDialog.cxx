/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "docsh.hxx"
#include "reffact.hxx"

#include "CovarianceDialog.hxx"

namespace
{
    static const OUString strCovarianceLabel("Covariances");
    static const OUString strCovarianceTemplate("=COVAR(%VAR1%; %VAR2%)");
}

ScCovarianceDialog::ScCovarianceDialog(
                        SfxBindings* pSfxBindings, SfxChildWindow* pChildWindow,
                        Window* pParent, ScViewData* pViewData ) :
    ScMatrixComparisonGenerator(
            pSfxBindings, pChildWindow, pParent, pViewData,
            "CovarianceDialog", "modules/scalc/ui/covariancedialog.ui" )
{}

sal_Bool ScCovarianceDialog::Close()
{
    return DoClose( ScCovarianceDialogWrapper::GetChildWindowId() );
}

const OUString& ScCovarianceDialog::getLabel()
{
    return strCovarianceLabel;
}

const OUString& ScCovarianceDialog::getTemplate()
{
    return strCovarianceTemplate;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
