/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "condformatuno.hxx"

#include "document.hxx"
#include "conditio.hxx"
#include "colorscale.hxx"
#include "docsh.hxx"
#include "miscuno.hxx"
#include "compiler.hxx"
#include "tokenarray.hxx"

#include "cellsuno.hxx"
#include "convuno.hxx"

#include <vcl/svapp.hxx>
#include <rtl/ustring.hxx>

#include <com/sun/star/sheet/DataBarAxis.hpp>
#include <com/sun/star/sheet/IconSetType.hpp>
#include <com/sun/star/sheet/DataBarAxis.hpp>
#include <com/sun/star/sheet/ConditionFormatOperator.hpp>

namespace {

enum CondFormatProperties
{
    ID,
    CondFormat_Range
};

const SfxItemPropertyMapEntry* getCondFormatPropset()
{
    static const SfxItemPropertyMapEntry aCondFormatPropertyMap_Impl[] =
    {
        {OUString("ID"), ID, cppu::UnoType<sal_Int32>::get(), 0, 0},
        {OUString("Range"), CondFormat_Range, cppu::UnoType<sheet::XSheetCellRanges>::get(), 0, 0},
        {OUString(), 0, css::uno::Type(), 0, 0}
    };
    return aCondFormatPropertyMap_Impl;
}

enum ConditionEntryProperties
{
    StyleName,
    Formula1,
    Formula2,
    Operator
};

const SfxItemPropertyMapEntry* getConditionEntryrPropSet()
{
    static const SfxItemPropertyMapEntry aConditionEntryPropertyMap_Impl[] =
    {
        {OUString("StyleName"), StyleName, cppu::UnoType<OUString>::get(), 0, 0},
        {OUString("Formula1"), Formula1, cppu::UnoType<OUString>::get(), 0, 0},
        {OUString("Formula2"), Formula2, cppu::UnoType<OUString>::get(), 0, 0},
        {OUString("Operator"), Operator, getCppuType(&sheet::ConditionFormatOperator::EQUAL), 0, 0 },
        {OUString(), 0, css::uno::Type(), 0, 0}
    };
    return aConditionEntryPropertyMap_Impl;
}

struct ConditionEntryApiMap
{
    ScConditionMode eMode;
    sal_Int32 nApiMode;
};

ConditionEntryApiMap aConditionEntryMap[] =
{
    {SC_COND_EQUAL, sheet::ConditionFormatOperator::EQUAL},
    {SC_COND_LESS, sheet::ConditionFormatOperator::LESS},
    {SC_COND_GREATER, sheet::ConditionFormatOperator::GREATER},
    {SC_COND_EQLESS, sheet::ConditionFormatOperator::LESS_EQUAL},
    {SC_COND_EQGREATER, sheet::ConditionFormatOperator::GREATER_EQUAL},
    {SC_COND_NOTEQUAL, sheet::ConditionFormatOperator::NOT_EQUAL},
    {SC_COND_BETWEEN, sheet::ConditionFormatOperator::BETWEEN},
    {SC_COND_NOTBETWEEN, sheet::ConditionFormatOperator::NOT_BETWEEN},
    {SC_COND_DUPLICATE, sheet::ConditionFormatOperator::DUPLICATE},
    {SC_COND_NOTDUPLICATE, sheet::ConditionFormatOperator::UNIQUE},
    {SC_COND_DIRECT, sheet::ConditionFormatOperator::EXPRESSION},
    {SC_COND_TOP10, sheet::ConditionFormatOperator::TOP_N_ELEMENTS},
    {SC_COND_BOTTOM10, sheet::ConditionFormatOperator::BOTTOM_N_ELEMENTS},
    {SC_COND_TOP_PERCENT, sheet::ConditionFormatOperator::TOP_N_PERCENT},
    {SC_COND_BOTTOM_PERCENT, sheet::ConditionFormatOperator::BOTTOM_N_PERCENT},
    {SC_COND_ABOVE_AVERAGE, sheet::ConditionFormatOperator::ABOVE_AVERAGE},
    {SC_COND_BELOW_AVERAGE, sheet::ConditionFormatOperator::BELOW_AVERAGE},
    {SC_COND_ABOVE_EQUAL_AVERAGE, sheet::ConditionFormatOperator::ABOVE_EQUAL_AVERAGE},
    {SC_COND_BELOW_EQUAL_AVERAGE, sheet::ConditionFormatOperator::BELOW_EQUAL_AVERAGE},
    {SC_COND_ERROR, sheet::ConditionFormatOperator::ERROR},
    {SC_COND_NOERROR, sheet::ConditionFormatOperator::NO_ERROR},
    {SC_COND_BEGINS_WITH, sheet::ConditionFormatOperator::BEGINS_WITH},
    {SC_COND_ENDS_WITH, sheet::ConditionFormatOperator::ENDS_WITH},
    {SC_COND_CONTAINS_TEXT, sheet::ConditionFormatOperator::CONTAINS},
    {SC_COND_NOT_CONTAINS_TEXT, sheet::ConditionFormatOperator::NOT_CONTAINS},
    {SC_COND_NONE, sheet::ConditionFormatOperator::EQUAL},
};

enum ColorScaleProperties
{
    ColorScaleEntries
};

const SfxItemPropertyMapEntry* getColorScalePropSet()
{
    static const SfxItemPropertyMapEntry aColorScalePropertyMap_Impl[] =
    {
        {OUString("ColorScaleEntries"), ColorScaleEntries, getCppuType((uno::Sequence< sheet::XColorScaleEntry >*)0), 0, 0 },
        {OUString(), 0, css::uno::Type(), 0, 0}
    };
    return aColorScalePropertyMap_Impl;
}

enum DataBarProperties
{
    AxisPosition,
    UseGradient,
    UseNegativeColor,
    DataBar_ShowValue,
    DataBar_Color,
    AxisColor,
    NegativeColor,
    DataBarEntries
};

const SfxItemPropertyMapEntry* getDataBarPropSet()
{
    static const SfxItemPropertyMapEntry aDataBarPropertyMap_Impl[] =
    {
        {OUString("AxisPosition"), AxisPosition, getCppuType(&sheet::DataBarAxis::AXIS_AUTOMATIC), 0, 0 },
        {OUString("UseGradient"), UseGradient, getBooleanCppuType(), 0, 0 },
        {OUString("UseNegativeColor"), UseNegativeColor, getBooleanCppuType(), 0, 0 },
        {OUString("ShowValue"), DataBar_ShowValue, getBooleanCppuType(), 0, 0 },
        {OUString("Color"), DataBar_Color, cppu::UnoType<sal_Int32>::get(), 0, 0},
        {OUString("AxisColor"), AxisColor, cppu::UnoType<sal_Int32>::get(), 0, 0},
        {OUString("NegativeColor"), NegativeColor, cppu::UnoType<sal_Int32>::get(), 0, 0},
        {OUString("DataBarEntries"), DataBarEntries, getCppuType((uno::Sequence< sheet::XDataBarEntry >*)0), 0, 0 },
        {OUString(), 0, css::uno::Type(), 0, 0}
    };
    return aDataBarPropertyMap_Impl;
}

struct DataBarAxisApiMap
{
    databar::ScAxisPosition ePos;
    sal_Int32 nApiPos;
};

DataBarAxisApiMap aDataBarAxisMap[] =
{
    { databar::NONE, sheet::DataBarAxis::AXIS_NONE },
    { databar::AUTOMATIC, sheet::DataBarAxis::AXIS_AUTOMATIC },
    { databar::MIDDLE, sheet::DataBarAxis::AXIS_MIDDLE }
};

enum IconSetProperties
{
    Icons,
    Reverse,
    ShowValue,
    IconSetEntries
};

const SfxItemPropertyMapEntry* getIconSetPropSet()
{
    static const SfxItemPropertyMapEntry aIconSetPropertyMap_Impl[] =
    {
        {OUString("Icons"), Icons, getCppuType(&sheet::IconSetType::ICONSET_3SYMBOLS), 0, 0 },
        {OUString("Reverse"), Reverse, getBooleanCppuType(), 0, 0 },
        {OUString("ShowValue"), ShowValue, getBooleanCppuType(), 0, 0 },
        {OUString("IconSetEntries"), IconSetEntries, getCppuType((uno::Sequence< sheet::XIconSetEntry >*)0), 0, 0 },
        {OUString(), 0, css::uno::Type(), 0, 0}
    };
    return aIconSetPropertyMap_Impl;
}

struct IconSetTypeApiMap
{
    ScIconSetType eType;
    sal_Int32 nApiType;
};

const IconSetTypeApiMap aIconSetApiMap[] =
{
    { IconSet_3Arrows, sheet::IconSetType::ICONSET_3ARROWS },
    { IconSet_3ArrowsGray, sheet::IconSetType::ICONSET_3ARROWS_GRAY },
    { IconSet_3Flags, sheet::IconSetType::ICONSET_3FLAGS },
    { IconSet_3TrafficLights1, sheet::IconSetType::ICONSET_3TRAFFICLIGHTS1 },
    { IconSet_3TrafficLights2, sheet::IconSetType::ICONSET_3TRAFFICLIGHTS2 },
    { IconSet_3Signs, sheet::IconSetType::ICONSET_3SIGNS },
    { IconSet_3Symbols, sheet::IconSetType::ICONSET_3SYMBOLS },
    { IconSet_3Symbols2, sheet::IconSetType::ICONSET_3SYMBOLS2 },
    { IconSet_3Smilies, sheet::IconSetType::ICONSET_3SMILIES },
    { IconSet_3ColorSmilies, sheet::IconSetType::ICONSET_3COLOR_SIMILIES },
    { IconSet_4Arrows, sheet::IconSetType::ICONSET_4ARROWS },
    { IconSet_4ArrowsGray, sheet::IconSetType::ICONSET_4ARROWS_GRAY },
    { IconSet_4Rating, sheet::IconSetType::ICONSET_4RATING },
    { IconSet_4RedToBlack, sheet::IconSetType::ICONSET_4RED_TO_BLACK },
    { IconSet_4TrafficLights, sheet::IconSetType::ICONSET_4TRAFFICLIGHTS },
    { IconSet_5Arrows, sheet::IconSetType::ICONSET_5ARROWS },
    { IconSet_5ArrowsGray, sheet::IconSetType::ICONSET_4ARROWS_GRAY },
    { IconSet_5Ratings, sheet::IconSetType::ICONSET_5RATINGS },
    { IconSet_5Quarters, sheet::IconSetType::ICONSET_5QUARTERS },
};

}

ScCondFormatsObj::ScCondFormatsObj(ScDocShell* pDocShell, SCTAB nTab):
    mnTab(nTab),
    mpDocShell(pDocShell)
{
    pDocShell->GetDocument().AddUnoObject(*this);
}

ScCondFormatsObj::~ScCondFormatsObj()
{
    if (mpDocShell)
        mpDocShell->GetDocument().RemoveUnoObject(*this);
}

void ScCondFormatsObj::Notify(SfxBroadcaster& /*rBC*/, const SfxHint& rHint)
{
    if ( dynamic_cast<const SfxSimpleHint*>(&rHint) &&
            static_cast<const SfxSimpleHint&>(rHint).GetId() == SFX_HINT_DYING )
    {
        mpDocShell = NULL;       // ungueltig geworden
    }
}

sal_Int32 ScCondFormatsObj::addByRange(const uno::Reference< sheet::XConditionalFormat >& xCondFormat,
        const uno::Reference< sheet::XSheetCellRanges >& /*xRanges*/)
    throw(uno::RuntimeException, std::exception)
{
    if (!xCondFormat.is())
        return 0;

    SolarMutexGuard aGuard;
    /*
    ScCondFormatObj* pFormatObj = ScCondFormatObj::getImplementation(xCondFormat);
    ScConditionalFormat* pFormat = pFormatObj->getCoreObject();
    mpFormatList->InsertNew(pFormat);
    */
    return 0;
}

void ScCondFormatsObj::removeByID(const sal_Int32 nID)
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    ScConditionalFormatList* pFormatList = getCoreObject();;
    pFormatList->erase(nID);
}

uno::Sequence<uno::Reference<sheet::XConditionalFormat> > ScCondFormatsObj::getConditionalFormats()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    ScConditionalFormatList* pFormatList = getCoreObject();;
    size_t n = pFormatList->size();
    uno::Sequence<uno::Reference<sheet::XConditionalFormat> > aCondFormats(n);
    sal_Int32 i = 0;
    for (ScConditionalFormatList::const_iterator itr = pFormatList->begin(); itr != pFormatList->end(); ++itr, ++i) {
        uno::Reference<sheet::XConditionalFormat> xCondFormat(new ScCondFormatObj(mpDocShell, this, itr->GetKey()));
        aCondFormats[i] = xCondFormat;
    }

    return aCondFormats;
}

sal_Int32 ScCondFormatsObj::getLength()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    ScConditionalFormatList* pFormatList = getCoreObject();;
    return pFormatList->size();;
}

ScConditionalFormatList* ScCondFormatsObj::getCoreObject()
{
    if (!mpDocShell)
        throw uno::RuntimeException();

    ScConditionalFormatList* pList = mpDocShell->GetDocument().GetCondFormList(mnTab);
    if (!pList)
        throw uno::RuntimeException();

    return pList;
}

namespace {

uno::Reference<beans::XPropertySet> createConditionEntry(const ScFormatEntry* pEntry,
        rtl::Reference<ScCondFormatObj> xParent)
{
    switch (pEntry->GetType())
    {
        case condformat::CONDITION:
            return new ScConditionEntryObj(xParent);
        break;
        case condformat::COLORSCALE:
            return new ScColorScaleFormatObj(xParent);
        break;
        case condformat::DATABAR:
            return new ScDataBarFormatObj(xParent);
        break;
        case condformat::ICONSET:
            return new ScIconSetFormatObj(xParent);
        break;
        case condformat::DATE:
        break;
        default:
        break;
    }
    return uno::Reference<beans::XPropertySet>();
}

}

ScCondFormatObj::ScCondFormatObj(ScDocShell* pDocShell, rtl::Reference<ScCondFormatsObj> xCondFormats,
        sal_Int32 nKey):
    mxCondFormatList(xCondFormats),
    mpDocShell(pDocShell),
    maPropSet(getCondFormatPropset()),
    mnKey(nKey)
{
}

ScCondFormatObj::~ScCondFormatObj()
{
}

ScConditionalFormat* ScCondFormatObj::getCoreObject()
{
    ScConditionalFormatList* pList = mxCondFormatList->getCoreObject();
    ScConditionalFormat* pFormat = pList->GetFormat(mnKey);
    if (!pFormat)
        throw uno::RuntimeException();

    return pFormat;
}

ScDocShell* ScCondFormatObj::getDocShell()
{
    return mpDocShell;
}

void ScCondFormatObj::addEntry(const uno::Reference<sheet::XConditionEntry>& /*xEntry*/)
    throw(uno::RuntimeException, std::exception)
{
}

void ScCondFormatObj::removeByIndex(const sal_Int32 nIndex)
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    if (getCoreObject()->size() >= size_t(nIndex))
        throw lang::IllegalArgumentException();

    getCoreObject()->RemoveEntry(nIndex);
}

uno::Type ScCondFormatObj::getElementType()
    throw(uno::RuntimeException, std::exception)
{
    return cppu::UnoType<beans::XPropertySet>::get();
}

sal_Bool ScCondFormatObj::hasElements()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    ScConditionalFormat* pFormat = getCoreObject();
    return !pFormat->IsEmpty();
}

sal_Int32 ScCondFormatObj::getCount()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    ScConditionalFormat* pFormat = getCoreObject();

    return pFormat->size();
}

uno::Any ScCondFormatObj::getByIndex(sal_Int32 nIndex)
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    if (getCoreObject()->size() >= size_t(nIndex))
        throw lang::IllegalArgumentException();

    const ScFormatEntry* pEntry = getCoreObject()->GetEntry(nIndex);
    uno::Reference<beans::XPropertySet> xCondEntry =
        createConditionEntry(pEntry, this);
    uno::Any aAny;
    aAny <<= xCondEntry;
    return aAny;
}

uno::Reference<beans::XPropertySetInfo> SAL_CALL ScCondFormatObj::getPropertySetInfo()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    static uno::Reference<beans::XPropertySetInfo> aRef(
        new SfxItemPropertySetInfo( maPropSet.getPropertyMap()));
    return aRef;
}

void SAL_CALL ScCondFormatObj::setPropertyValue(
                        const OUString& aPropertyName, const uno::Any& aValue )
                throw(beans::UnknownPropertyException, beans::PropertyVetoException,
                        lang::IllegalArgumentException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    switch(pEntry->nWID)
    {
        case ID:
            throw lang::IllegalArgumentException();
        break;
        case CondFormat_Range:
        {
            uno::Reference<sheet::XSheetCellRanges> xRange;
            if (aValue >>= xRange)
            {
                ScConditionalFormat* pFormat = getCoreObject();
                uno::Sequence<table::CellRangeAddress> aRanges =
                    xRange->getRangeAddresses();
                ScRangeList aTargetRange;
                for (size_t i = 0, n = aRanges.getLength(); i < n; ++i)
                {
                    ScRange aRange;
                    ScUnoConversion::FillScRange(aRange, aRanges[i]);
                    aTargetRange.Join(aRange);
                }
                pFormat->SetRange(aTargetRange);
            }
        }
        break;
        default:
            SAL_WARN("sc", "unknown property");
    }
}

uno::Any SAL_CALL ScCondFormatObj::getPropertyValue( const OUString& aPropertyName )
                throw(beans::UnknownPropertyException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    uno::Any aAny;
    switch(pEntry->nWID)
    {
        case ID:
            aAny <<= sal_Int32(getCoreObject()->GetKey());
        break;
        case CondFormat_Range:
        {
            const ScRangeList& rRange = getCoreObject()->GetRange();
            uno::Reference<sheet::XSheetCellRanges> xRange;
            xRange.set(new ScCellRangesObj(mpDocShell, rRange));
            aAny <<= xRange;
        }
        break;
        default:
            SAL_WARN("sc", "unknown property");
    }
    return aAny;
}

void SAL_CALL ScCondFormatObj::addPropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScCondFormatObj::removePropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScCondFormatObj::addVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScCondFormatObj::removeVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

ScConditionEntryObj::ScConditionEntryObj(rtl::Reference<ScCondFormatObj> xParent):
    mpDocShell(xParent->getDocShell()),
    mxParent(xParent),
    maPropSet(getConditionEntryrPropSet())
{
}

ScConditionEntryObj::~ScConditionEntryObj()
{
}

ScCondFormatEntry* ScConditionEntryObj::getCoreObject()
{
    return NULL;
}

sal_Int32 ScConditionEntryObj::getType()
    throw(uno::RuntimeException, std::exception)
{
    return 0;
}

uno::Reference<beans::XPropertySetInfo> SAL_CALL ScConditionEntryObj::getPropertySetInfo()
    throw(uno::RuntimeException, std::exception)
{
    static uno::Reference<beans::XPropertySetInfo> aRef(
        new SfxItemPropertySetInfo( maPropSet.getPropertyMap() ));
    return aRef;
}

void SAL_CALL ScConditionEntryObj::setPropertyValue(
                        const OUString& aPropertyName, const uno::Any& aValue )
                throw(beans::UnknownPropertyException, beans::PropertyVetoException,
                        lang::IllegalArgumentException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    switch(pEntry->nWID)
    {
        case StyleName:
        {
            OUString aStyleName;
            if ((aValue >>= aStyleName) && !aStyleName.isEmpty())
                getCoreObject()->UpdateStyleName(aStyleName);
        }
        break;
        case Formula1:
        {
            OUString aFormula;
            if ((aValue >>= aFormula) && !aFormula.isEmpty())
            {
                ScCompiler aComp(&mpDocShell->GetDocument(), getCoreObject()->GetSrcPos());
                boost::scoped_ptr<ScTokenArray> pArr(aComp.CompileString(aFormula));
                getCoreObject()->SetFormula1(*pArr);
            }
        }
        break;
        case Formula2:
        {
            OUString aFormula;
            if ((aValue >>= aFormula) && !aFormula.isEmpty())
            {
                ScCompiler aComp(&mpDocShell->GetDocument(), getCoreObject()->GetSrcPos());
                boost::scoped_ptr<ScTokenArray> pArr(aComp.CompileString(aFormula));
                getCoreObject()->SetFormula2(*pArr);
            }
        }
        break;
        case Operator:
        {
            sal_Int32 nVal;
            if (aValue >>= nVal)
            {
                for (size_t i = 0; i < SAL_N_ELEMENTS(aConditionEntryMap); ++i)
                {
                    if (aConditionEntryMap[i].nApiMode == nVal)
                    {
                        getCoreObject()->SetOperation(aConditionEntryMap[i].eMode);
                        break;
                    }
                }
            }
        }
        break;
        default:
            SAL_WARN("sc", "unsupported property");
    }
}

uno::Any SAL_CALL ScConditionEntryObj::getPropertyValue( const OUString& aPropertyName )
                throw(beans::UnknownPropertyException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    uno::Any aAny;
    switch(pEntry->nWID)
    {
        case StyleName:
            aAny <<= getCoreObject()->GetStyle();
        break;
        case Formula1:
        {
            ScAddress aCursor = getCoreObject()->GetSrcPos();
            OUString aFormula = getCoreObject()->GetExpression(aCursor, 0);
            aAny <<= aFormula;
        }
        break;
        case Formula2:
        {
            ScAddress aCursor = getCoreObject()->GetSrcPos();
            OUString aFormula = getCoreObject()->GetExpression(aCursor, 1);
            aAny <<= aFormula;
        }
        break;
        case Operator:
        {
            ScConditionMode eMode = getCoreObject()->GetOperation();
            for (size_t i = 0; i < SAL_N_ELEMENTS(aConditionEntryMap); ++i)
            {
                if (aConditionEntryMap[i].eMode == eMode)
                {
                    aAny <<= aConditionEntryMap[i].nApiMode;
                    break;

                }
            }
        }
        break;
        default:
            SAL_WARN("sc", "unsupported property");
    }
    return aAny;
}

void SAL_CALL ScConditionEntryObj::addPropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScConditionEntryObj::removePropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScConditionEntryObj::addVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScConditionEntryObj::removeVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

ScColorScaleFormatObj::ScColorScaleFormatObj(rtl::Reference<ScCondFormatObj> xParent):
    mpDocShell(xParent->getDocShell()),
    mxParent(xParent),
    maPropSet(getColorScalePropSet())
{
}

ScColorScaleFormatObj::~ScColorScaleFormatObj()
{
}

ScColorScaleFormat* ScColorScaleFormatObj::getCoreObject()
{
    return NULL;
}

uno::Reference<beans::XPropertySetInfo> SAL_CALL ScColorScaleFormatObj::getPropertySetInfo()
    throw(uno::RuntimeException, std::exception)
{
    static uno::Reference<beans::XPropertySetInfo> aRef(
        new SfxItemPropertySetInfo( maPropSet.getPropertyMap() ));
    return aRef;
}

void SAL_CALL ScColorScaleFormatObj::setPropertyValue(
                        const OUString& aPropertyName, const uno::Any& /*aValue*/ )
                throw(beans::UnknownPropertyException, beans::PropertyVetoException,
                        lang::IllegalArgumentException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    switch(pEntry->nWID)
    {
        case ColorScaleEntries:
        break;
        default:
            SAL_WARN("sc", "unknown property");
    }
}

uno::Any SAL_CALL ScColorScaleFormatObj::getPropertyValue( const OUString& aPropertyName )
                throw(beans::UnknownPropertyException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    uno::Any aAny;

    switch(pEntry->nWID)
    {
        case ColorScaleEntries:
        break;
        default:
            SAL_WARN("sc", "unknown property");
    }

    return aAny;
}

void SAL_CALL ScColorScaleFormatObj::addPropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScColorScaleFormatObj::removePropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScColorScaleFormatObj::addVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScColorScaleFormatObj::removeVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

ScDataBarFormatObj::ScDataBarFormatObj(rtl::Reference<ScCondFormatObj> xParent):
    mpDocShell(xParent->getDocShell()),
    mxParent(xParent),
    maPropSet(getDataBarPropSet())
{
}

ScDataBarFormatObj::~ScDataBarFormatObj()
{
}

ScDataBarFormat* ScDataBarFormatObj::getCoreObject()
{
    return NULL;
}

uno::Reference<beans::XPropertySetInfo> SAL_CALL ScDataBarFormatObj::getPropertySetInfo()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    static uno::Reference<beans::XPropertySetInfo> aRef(
        new SfxItemPropertySetInfo( maPropSet.getPropertyMap() ));
    return aRef;
}

void SAL_CALL ScDataBarFormatObj::setPropertyValue(
                        const OUString& aPropertyName, const uno::Any& aValue )
                throw(beans::UnknownPropertyException, beans::PropertyVetoException,
                        lang::IllegalArgumentException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    switch(pEntry->nWID)
    {
        case AxisPosition:
        {
            sal_Int32 nVal;
            if (aValue >>= nVal)
            {
                for (size_t i = 0; i < SAL_N_ELEMENTS(aDataBarAxisMap); ++i)
                {
                    if (aDataBarAxisMap[i].nApiPos == nVal)
                    {
                        getCoreObject()->GetDataBarData()->meAxisPosition =
                            aDataBarAxisMap[i].ePos;
                        break;
                    }
                }
            }
        }
        break;
        case UseGradient:
        {
            bool bUseGradient = true;
            if (aValue >>= bUseGradient)
            {
                getCoreObject()->GetDataBarData()->mbGradient = bUseGradient;
            }
        }
        break;
        case UseNegativeColor:
        {
            bool bUseNegativeColor = false;
            if (aValue >>= bUseNegativeColor)
            {
                getCoreObject()->GetDataBarData()->mbNeg = bUseNegativeColor;
                if (bUseNegativeColor && !getCoreObject()->GetDataBarData()->mpNegativeColor)
                {
                    getCoreObject()->GetDataBarData()->mpNegativeColor.reset(new Color(COL_AUTO));
                }
            }
        }
        break;
        case DataBar_ShowValue:
        {
            bool bShowValue = true;
            if (aValue >>= bShowValue)
            {
                getCoreObject()->GetDataBarData()->mbOnlyBar = !bShowValue;
            }
        }
        break;
        case DataBar_Color:
        {
            sal_Int32 nColor = COL_AUTO;
            if (aValue >>= nColor)
            {
                getCoreObject()->GetDataBarData()->maPositiveColor.SetColor(nColor);
            }
        }
        break;
        case AxisColor:
        {
            sal_Int32 nAxisColor = COL_AUTO;
            if (aValue >>= nAxisColor)
            {
                getCoreObject()->GetDataBarData()->maAxisColor.SetColor(nAxisColor);
            }
        }
        break;
        case NegativeColor:
        {
            sal_Int32 nNegativeColor = COL_AUTO;
            if ((aValue >>= nNegativeColor) && getCoreObject()->GetDataBarData()->mbNeg)
            {
                getCoreObject()->GetDataBarData()->mpNegativeColor->SetColor(nNegativeColor);
            }
            else
                throw lang::IllegalArgumentException();
        }
        break;
        case DataBarEntries:
        {
        }
        break;
    }
}

uno::Any SAL_CALL ScDataBarFormatObj::getPropertyValue( const OUString& aPropertyName )
                throw(beans::UnknownPropertyException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    uno::Any aAny;
    switch(pEntry->nWID)
    {
        case AxisPosition:
        {
            databar::ScAxisPosition ePos = getCoreObject()->GetDataBarData()->meAxisPosition;
            sal_Int32 nApiPos = sheet::DataBarAxis::AXIS_NONE;
            for (size_t i = 0; i < SAL_N_ELEMENTS(aDataBarAxisMap); ++i)
            {
                if (aDataBarAxisMap[i].ePos == ePos)
                {
                    nApiPos = aDataBarAxisMap[i].nApiPos;
                }
            }

            aAny <<= nApiPos;
        }
        break;
        case UseGradient:
        {
           aAny <<= getCoreObject()->GetDataBarData()->mbGradient;
        }
        break;
        case UseNegativeColor:
        {
           aAny <<= getCoreObject()->GetDataBarData()->mbNeg;
        }
        break;
        case DataBar_ShowValue:
        {
            aAny <<= !getCoreObject()->GetDataBarData()->mbOnlyBar;
        }
        break;
        case DataBar_Color:
        {
            sal_Int32 nColor = getCoreObject()->GetDataBarData()->maPositiveColor.GetColor();
            aAny <<= nColor;
        }
        break;
        case AxisColor:
        {
            sal_Int32 nAxisColor = getCoreObject()->GetDataBarData()->maAxisColor.GetColor();
            aAny <<= nAxisColor;
        }
        break;
        case NegativeColor:
        {
            if (getCoreObject()->GetDataBarData()->mbNeg && getCoreObject()->GetDataBarData()->mpNegativeColor)
            {
                sal_Int32 nNegativeColor = getCoreObject()->GetDataBarData()->mpNegativeColor->GetColor();
                aAny <<= nNegativeColor;
            }
        }
        break;
        case DataBarEntries:
        {
        }
        break;
    }
    return aAny;
}

void SAL_CALL ScDataBarFormatObj::addPropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScDataBarFormatObj::removePropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScDataBarFormatObj::addVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScDataBarFormatObj::removeVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

ScIconSetFormatObj::ScIconSetFormatObj(rtl::Reference<ScCondFormatObj> xParent):
    mpDocShell(xParent->getDocShell()),
    mxParent(xParent),
    maPropSet(getIconSetPropSet())
{
}

ScIconSetFormatObj::~ScIconSetFormatObj()
{
}

ScIconSetFormat* ScIconSetFormatObj::getCoreObject()
{
    return NULL;
}

uno::Reference<beans::XPropertySetInfo> SAL_CALL ScIconSetFormatObj::getPropertySetInfo()
    throw(uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;
    static uno::Reference<beans::XPropertySetInfo> aRef(
        new SfxItemPropertySetInfo( maPropSet.getPropertyMap() ));
    return aRef;
}

void SAL_CALL ScIconSetFormatObj::setPropertyValue(
                        const OUString& aPropertyName, const uno::Any& aValue )
                throw(beans::UnknownPropertyException, beans::PropertyVetoException,
                        lang::IllegalArgumentException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    switch(pEntry->nWID)
    {
        case ShowValue:
        {
            bool bShowValue = true;
            aValue >>= bShowValue;
            getCoreObject()->GetIconSetData()->mbShowValue = bShowValue;
        }
        break;
        case Reverse:
        {
            bool bReverse = false;
            aValue >>= bReverse;
            getCoreObject()->GetIconSetData()->mbReverse = bReverse;
        }
        break;
        case Icons:
        {
            sal_Int32 nApiType = -1;
            aValue >>= nApiType;
            ScIconSetType eType = IconSet_3Arrows;
            bool bFound = false;
            for (size_t i = 0; i < SAL_N_ELEMENTS(aIconSetApiMap); ++i)
            {
                if (aIconSetApiMap[i].nApiType == nApiType)
                {
                    eType = aIconSetApiMap[i].eType;
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                throw lang::IllegalArgumentException();
            }

            getCoreObject()->GetIconSetData()->eIconSetType = eType;
        }
        break;
        case IconSetEntries:
        break;
        default:
        break;
    }
}

uno::Any SAL_CALL ScIconSetFormatObj::getPropertyValue( const OUString& aPropertyName )
                throw(beans::UnknownPropertyException, lang::WrappedTargetException,
                        uno::RuntimeException, std::exception)
{
    SolarMutexGuard aGuard;

    const SfxItemPropertyMap& rPropertyMap = maPropSet.getPropertyMap();     // from derived class
    const SfxItemPropertySimpleEntry* pEntry = rPropertyMap.getByName( aPropertyName );
    if ( !pEntry )
        throw beans::UnknownPropertyException();

    uno::Any aAny;

    switch(pEntry->nWID)
    {
        case ShowValue:
            aAny <<= getCoreObject()->GetIconSetData()->mbShowValue;
        break;
        case Reverse:
            aAny <<= getCoreObject()->GetIconSetData()->mbReverse;
        break;
        case Icons:
        {
            ScIconSetType eType = getCoreObject()->GetIconSetData()->eIconSetType;
            for (size_t i = 0; i < SAL_N_ELEMENTS(aIconSetApiMap); ++i)
            {
                if (aIconSetApiMap[i].eType == eType)
                {
                    aAny <<= aIconSetApiMap[i].nApiType;
                    break;
                }
            }
        }
        break;
        case IconSetEntries:
        {
            uno::Sequence< sheet::XIconSetEntry > aEntries(getCoreObject()->size());
            for (auto it = getCoreObject()->begin(), itEnd = getCoreObject()->end(); it != itEnd; ++it)
            {
                //aEntries.operator[] = ;
            }
        }
        break;
        default:
            SAL_WARN("sc", "unknown property");
    }
    return aAny;
}

void SAL_CALL ScIconSetFormatObj::addPropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScIconSetFormatObj::removePropertyChangeListener( const OUString& /* aPropertyName */,
                            const uno::Reference<beans::XPropertyChangeListener>& /* aListener */)
                            throw(beans::UnknownPropertyException,
                                    lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScIconSetFormatObj::addVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

void SAL_CALL ScIconSetFormatObj::removeVetoableChangeListener( const OUString&,
                            const uno::Reference<beans::XVetoableChangeListener>&)
                            throw(beans::UnknownPropertyException,
                                lang::WrappedTargetException, uno::RuntimeException, std::exception)
{
    SAL_WARN("sc", "not implemented");
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
