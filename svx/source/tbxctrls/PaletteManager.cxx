/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <memory>
#include <svx/PaletteManager.hxx>

#include <comphelper/propertyvalue.hxx>
#include <tools/urlobj.hxx>
#include <osl/file.hxx>
#include <unotools/pathoptions.hxx>
#include <sfx2/objsh.hxx>
#include <svx/drawitem.hxx>
#include <svx/strings.hrc>
#include <svx/svxids.hrc>
#include <svx/dialmgr.hxx>
#include <tbxcolorupdate.hxx>
#include <vcl/svapp.hxx>
#include <vcl/settings.hxx>
#include <comphelper/sequence.hxx>
#include <stack>
#include <set>
#include <officecfg/Office/Common.hxx>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/util/URLTransformer.hpp>

#include <palettes.hxx>

namespace
{
// Luminance modulation for the 6 effect presets.
// 10000 is the default.
sal_Int16 g_aLumMods[] = { 10000, 2000, 4000, 6000, 7500, 5000 };

// Luminance offset for the 6 effect presets.
// 0 is the default.
sal_Int16 g_aLumOffs[] = { 0, 8000, 6000, 4000, 0, 0 };
}

PaletteManager::PaletteManager() :
    mnMaxRecentColors(Application::GetSettings().GetStyleSettings().GetColorValueSetColumnCount()),
    mnNumOfPalettes(3),
    mnCurrentPalette(0),
    mnColorCount(0),
    mpBtnUpdater(nullptr),
    maColorSelectFunction(PaletteManager::DispatchColorCommand)
{
    SfxObjectShell* pDocSh = SfxObjectShell::Current();
    if(pDocSh)
    {
        const SfxPoolItem* pItem = nullptr;
        if( nullptr != ( pItem = pDocSh->GetItem(SID_COLOR_TABLE) ) )
            pColorList = static_cast<const SvxColorListItem*>(pItem)->GetColorList();
    }
    if(!pColorList.is())
        pColorList = XColorList::CreateStdColorList();
    LoadPalettes();
    mnNumOfPalettes += m_Palettes.size();

}

PaletteManager::PaletteManager(const PaletteManager* pClone)
    : mnMaxRecentColors(pClone->mnMaxRecentColors)
    , mnNumOfPalettes(pClone->mnNumOfPalettes)
    , mnCurrentPalette(pClone->mnCurrentPalette)
    , mnColorCount(pClone->mnColorCount)
    , mpBtnUpdater(nullptr)
    , pColorList(pClone->pColorList)
    , maRecentColors(pClone->maRecentColors)
    , maColorSelectFunction(PaletteManager::DispatchColorCommand)
{
    for (const auto& a : pClone->m_Palettes)
        m_Palettes.emplace_back(a->Clone());
}

PaletteManager* PaletteManager::Clone() const
{
    return new PaletteManager(this);
}

PaletteManager::~PaletteManager()
{
}

void PaletteManager::LoadPalettes()
{
    m_Palettes.clear();
    OUString aPalPaths = SvtPathOptions().GetPalettePath();

    std::stack<OUString> aDirs;
    sal_Int32 nIndex = 0;
    do
    {
        aDirs.push(aPalPaths.getToken(0, ';', nIndex));
    }
    while (nIndex >= 0);

    std::set<OUString> aNames;
    //try all entries palette path list user first, then
    //system, ignoring duplicate file names
    while (!aDirs.empty())
    {
        OUString aPalPath = aDirs.top();
        aDirs.pop();

        osl::Directory aDir(aPalPath);
        osl::DirectoryItem aDirItem;
        osl::FileStatus aFileStat( osl_FileStatus_Mask_FileName |
                                   osl_FileStatus_Mask_FileURL  |
                                   osl_FileStatus_Mask_Type     );
        if( aDir.open() == osl::FileBase::E_None )
        {
            while( aDir.getNextItem(aDirItem) == osl::FileBase::E_None )
            {
                aDirItem.getFileStatus(aFileStat);
                if(aFileStat.isRegular() || aFileStat.isLink())
                {
                    OUString aFName = aFileStat.getFileName();
                    INetURLObject aURLObj( aFileStat.getFileURL() );
                    OUString aFNameWithoutExt = aURLObj.GetBase();
                    if (aNames.find(aFName) == aNames.end())
                    {
                        std::unique_ptr<Palette> pPalette;
                        if( aFName.endsWithIgnoreAsciiCase(".gpl") )
                            pPalette.reset(new PaletteGPL(aFileStat.getFileURL(), aFNameWithoutExt));
                        else if( aFName.endsWithIgnoreAsciiCase(".soc") )
                            pPalette.reset(new PaletteSOC(aFileStat.getFileURL(), aFNameWithoutExt));
                        else if ( aFName.endsWithIgnoreAsciiCase(".ase") )
                            pPalette.reset(new PaletteASE(aFileStat.getFileURL(), aFNameWithoutExt));

                        if( pPalette && pPalette->IsValid() )
                            m_Palettes.push_back( std::move(pPalette) );
                        aNames.insert(aFNameWithoutExt);
                    }
                }
            }
        }
    }
}

bool PaletteManager::IsThemePaletteSelected() const
{
    return mnCurrentPalette == mnNumOfPalettes - 2;
}

void PaletteManager::GetThemeIndexLumModOff(sal_uInt16 nItemId, sal_Int16& rThemeIndex,
                                            sal_Int16& rLumMod, sal_Int16& rLumOff)
{
    // Each column is the same color with different effects.
    rThemeIndex = nItemId % 12;

    // Each row is the same effect with different colors.
    rLumMod = g_aLumMods[nItemId / 12];
    rLumOff = g_aLumOffs[nItemId / 12];
}

void PaletteManager::ReloadColorSet(SvxColorValueSet &rColorSet)
{
    if( mnCurrentPalette == 0)
    {
        rColorSet.Clear();
        css::uno::Sequence< sal_Int32 > CustomColorList( officecfg::Office::Common::UserColors::CustomColor::get() );
        css::uno::Sequence< OUString > CustomColorNameList( officecfg::Office::Common::UserColors::CustomColorName::get() );
        int nIx = 1;
        for (int i = 0; i < CustomColorList.getLength(); ++i)
        {
            Color aColor(ColorTransparency, CustomColorList[i]);
            rColorSet.InsertItem(nIx, aColor, CustomColorNameList[i]);
            ++nIx;
        }
    }
    else if (IsThemePaletteSelected())
    {
        SfxObjectShell* pObjectShell = SfxObjectShell::Current();
        if (pObjectShell)
        {
            std::vector<Color> aColors = pObjectShell->GetThemeColors();
            mnColorCount = aColors.size();
            rColorSet.Clear();
            if (aColors.size() >= 12)
            {
                std::vector<OUString> aEffectNames = {
                    SvxResId(RID_SVXSTR_THEME_EFFECT1),  SvxResId(RID_SVXSTR_THEME_EFFECT2),
                    SvxResId(RID_SVXSTR_THEME_EFFECT3),  SvxResId(RID_SVXSTR_THEME_EFFECT4),
                    SvxResId(RID_SVXSTR_THEME_EFFECT5),
                };

                std::vector<OUString> aColorNames = {
                    SvxResId(RID_SVXSTR_THEME_COLOR1),  SvxResId(RID_SVXSTR_THEME_COLOR2),
                    SvxResId(RID_SVXSTR_THEME_COLOR3),  SvxResId(RID_SVXSTR_THEME_COLOR4),
                    SvxResId(RID_SVXSTR_THEME_COLOR5),  SvxResId(RID_SVXSTR_THEME_COLOR6),
                    SvxResId(RID_SVXSTR_THEME_COLOR7),  SvxResId(RID_SVXSTR_THEME_COLOR8),
                    SvxResId(RID_SVXSTR_THEME_COLOR9),  SvxResId(RID_SVXSTR_THEME_COLOR10),
                    SvxResId(RID_SVXSTR_THEME_COLOR11), SvxResId(RID_SVXSTR_THEME_COLOR12),
                };

                sal_uInt16 nItemId = 0;
                // Each row is one effect type (no effect + each type).
                for (size_t nEffect = 0; nEffect < aEffectNames.size() + 1; ++nEffect)
                {
                    // Each column is one color type.
                    for (size_t nColor = 0; nColor < aColorNames.size(); ++nColor)
                    {
                        Color aColor = aColors[nColor];
                        aColor.ApplyLumModOff(g_aLumMods[nEffect], g_aLumOffs[nEffect]);
                        OUString aColorName;
                        if (nEffect == 0)
                        {
                            aColorName = aColorNames[nColor];
                        }
                        else
                        {
                            aColorName = aEffectNames[nEffect - 1].replaceAll("%1", aColorNames[nColor]);
                        }
                        rColorSet.InsertItem(nItemId++, aColor, aColorName);
                    }
                }
            }
        }
    }
    else if( mnCurrentPalette == mnNumOfPalettes - 1 )
    {
        // Add doc colors to palette
        SfxObjectShell* pDocSh = SfxObjectShell::Current();
        if (pDocSh)
        {
            std::set<Color> aColors = pDocSh->GetDocColors();
            mnColorCount = aColors.size();
            rColorSet.Clear();
            rColorSet.addEntriesForColorSet(aColors, Concat2View(SvxResId( RID_SVXSTR_DOC_COLOR_PREFIX ) + " ") );
        }
    }
    else
    {
        m_Palettes[mnCurrentPalette - 1]->LoadColorSet( rColorSet );
        mnColorCount = rColorSet.GetItemCount();
    }
}

void PaletteManager::ReloadRecentColorSet(SvxColorValueSet& rColorSet)
{
    maRecentColors.clear();
    rColorSet.Clear();
    css::uno::Sequence< sal_Int32 > Colorlist(officecfg::Office::Common::UserColors::RecentColor::get());
    css::uno::Sequence< OUString > ColorNamelist(officecfg::Office::Common::UserColors::RecentColorName::get());
    int nIx = 1;
    const bool bHasColorNames = Colorlist.getLength() == ColorNamelist.getLength();
    for (int i = 0; i < Colorlist.getLength(); ++i)
    {
        Color aColor(ColorTransparency, Colorlist[i]);
        OUString sColorName = bHasColorNames ? ColorNamelist[i] : ("#" + aColor.AsRGBHexString().toAsciiUpperCase());
        maRecentColors.emplace_back(aColor, sColorName);
        rColorSet.InsertItem(nIx, aColor, sColorName);
        ++nIx;
    }
}

std::vector<OUString> PaletteManager::GetPaletteList()
{
    std::vector<OUString> aPaletteNames
    {
        SvxResId( RID_SVXSTR_CUSTOM_PAL )
    };
    for (auto const& it : m_Palettes)
    {
        aPaletteNames.push_back( (*it).GetName() );
    }
    aPaletteNames.push_back(SvxResId(RID_SVXSTR_THEME_COLORS));
    aPaletteNames.push_back( SvxResId ( RID_SVXSTR_DOC_COLORS ) );

    return aPaletteNames;
}

void PaletteManager::SetPalette( sal_Int32 nPos )
{
    mnCurrentPalette = nPos;
    if( nPos != mnNumOfPalettes - 1 && nPos != 0)
    {
        pColorList = XPropertyList::AsColorList(
                            XPropertyList::CreatePropertyListFromURL(
                            XPropertyListType::Color, GetSelectedPalettePath()));
        auto name = GetPaletteName(); // may change pColorList
        pColorList->SetName(name);
        if(pColorList->Load())
        {
            SfxObjectShell* pShell = SfxObjectShell::Current();
            if (pShell != nullptr)
            {
                SvxColorListItem aColorItem(pColorList, SID_COLOR_TABLE);
                pShell->PutItem( aColorItem );
            }
        }
    }
    OUString aPaletteName(officecfg::Office::Common::UserColors::PaletteName::get());
    if (aPaletteName != GetPaletteName())
    {
        std::shared_ptr<comphelper::ConfigurationChanges> batch(comphelper::ConfigurationChanges::create());
        officecfg::Office::Common::UserColors::PaletteName::set(GetPaletteName(), batch);
        batch->commit();
    }
}

sal_Int32 PaletteManager::GetPalette() const
{
    return mnCurrentPalette;
}

OUString PaletteManager::GetPaletteName()
{
    std::vector<OUString> aNames(GetPaletteList());
    if(mnCurrentPalette != mnNumOfPalettes - 1 && mnCurrentPalette != 0)
    {
        SfxObjectShell* pDocSh = SfxObjectShell::Current();
        if(pDocSh)
        {
            const SfxPoolItem* pItem = nullptr;
            if( nullptr != ( pItem = pDocSh->GetItem(SID_COLOR_TABLE) ) )
                pColorList = static_cast<const SvxColorListItem*>(pItem)->GetColorList();
        }
    }
    return aNames[mnCurrentPalette];
}

OUString PaletteManager::GetSelectedPalettePath()
{
    if (mnCurrentPalette < m_Palettes.size() && mnCurrentPalette != 0)
        return m_Palettes[mnCurrentPalette - 1]->GetPath();
    else
        return OUString();
}

tools::Long PaletteManager::GetColorCount() const
{
    return mnColorCount;
}

tools::Long PaletteManager::GetRecentColorCount() const
{
    return maRecentColors.size();
}

void PaletteManager::AddRecentColor(const Color& rRecentColor, const OUString& rName, bool bFront)
{
    auto itColor = std::find_if(maRecentColors.begin(),
                                maRecentColors.end(),
                                [rRecentColor] (const NamedColor &a) { return a.first == rRecentColor; });
    // if recent color to be added is already in list, remove it
    if( itColor != maRecentColors.end() )
        maRecentColors.erase( itColor );

    if (maRecentColors.size() == mnMaxRecentColors)
        maRecentColors.pop_back();
    if (bFront)
        maRecentColors.push_front(std::make_pair(rRecentColor, rName));
    else
        maRecentColors.emplace_back(rRecentColor, rName);
    css::uno::Sequence< sal_Int32 > aColorList(maRecentColors.size());
    auto aColorListRange = asNonConstRange(aColorList);
    css::uno::Sequence< OUString > aColorNameList(maRecentColors.size());
    auto aColorNameListRange = asNonConstRange(aColorNameList);
    for (size_t i = 0; i < maRecentColors.size(); ++i)
    {
        aColorListRange[i] = static_cast<sal_Int32>(maRecentColors[i].first);
        aColorNameListRange[i] = maRecentColors[i].second;
    }
    std::shared_ptr<comphelper::ConfigurationChanges> batch(comphelper::ConfigurationChanges::create());
    officecfg::Office::Common::UserColors::RecentColor::set(aColorList, batch);
    officecfg::Office::Common::UserColors::RecentColorName::set(aColorNameList, batch);
    batch->commit();
}

void PaletteManager::SetBtnUpdater(svx::ToolboxButtonColorUpdaterBase* pBtnUpdater)
{
    mpBtnUpdater = pBtnUpdater;
}

void PaletteManager::SetColorSelectFunction(const ColorSelectFunction& aColorSelectFunction)
{
    maColorSelectFunction = aColorSelectFunction;
}

void PaletteManager::PopupColorPicker(weld::Window* pParent, const OUString& aCommand, const Color& rInitialColor)
{
    // The calling object goes away during aColorDlg.Execute(), so we must copy this
    OUString aCommandCopy = aCommand;
    m_pColorDlg = std::make_unique<SvColorDialog>();
    m_pColorDlg->SetColor(rInitialColor);
    m_pColorDlg->SetMode(svtools::ColorPickerMode::Modify);
    m_pColorDlg->ExecuteAsync(pParent, [this, aCommandCopy] (sal_Int32 nResult) {
        if (nResult == RET_OK)
        {
            Color aLastColor = m_pColorDlg->GetColor();
            OUString sColorName = "#" + aLastColor.AsRGBHexString().toAsciiUpperCase();
            NamedColor aNamedColor = std::make_pair(aLastColor, sColorName);
            if (mpBtnUpdater)
                mpBtnUpdater->Update(aNamedColor);
            AddRecentColor(aLastColor, sColorName);
            maColorSelectFunction(aCommandCopy, svx::NamedThemedColor::FromNamedColor(aNamedColor));
        }
    });
}

void PaletteManager::DispatchColorCommand(const OUString& aCommand, const svx::NamedThemedColor& rColor)
{
    using namespace css::uno;
    using namespace css::frame;
    using namespace css::beans;
    using namespace css::util;

    Reference<XComponentContext> xContext(comphelper::getProcessComponentContext());
    Reference<XDesktop2> xDesktop = Desktop::create(xContext);
    Reference<XFrame> xFrame(xDesktop->getCurrentFrame());
    Reference<XDispatchProvider> xDispatchProvider(xFrame, UNO_QUERY);
    if (!xDispatchProvider.is())
        return;

    INetURLObject aObj( aCommand );

    std::vector<PropertyValue> aArgs{
        comphelper::makePropertyValue(aObj.GetURLPath(), sal_Int32(rColor.m_aColor)),
    };
    if (rColor.m_nThemeIndex != -1)
    {
        aArgs.push_back(comphelper::makePropertyValue("ColorThemeIndex", rColor.m_nThemeIndex));
        aArgs.push_back(comphelper::makePropertyValue("ColorLumMod", rColor.m_nLumMod));
        aArgs.push_back(comphelper::makePropertyValue("ColorLumOff", rColor.m_nLumOff));
    }

    URL aTargetURL;
    aTargetURL.Complete = aCommand;
    Reference<XURLTransformer> xURLTransformer(URLTransformer::create(comphelper::getProcessComponentContext()));
    xURLTransformer->parseStrict(aTargetURL);

    Reference<XDispatch> xDispatch = xDispatchProvider->queryDispatch(aTargetURL, OUString(), 0);
    if (xDispatch.is())
    {
        xDispatch->dispatch(aTargetURL, comphelper::containerToSequence(aArgs));
        if (xFrame->getContainerWindow().is())
            xFrame->getContainerWindow()->setFocus();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
