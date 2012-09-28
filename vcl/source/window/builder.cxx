/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *        Caolán McNamara <caolanm@redhat.com> (Red Hat, Inc.)
 * Portions created by the Initial Developer are Copyright (C) 2012 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Caolán McNamara <caolanm@redhat.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <vcl/builder.hxx>
#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/edit.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <vcl/layout.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/tabctrl.hxx>
#include <vcl/tabpage.hxx>
#include <window.h>

VclBuilder::VclBuilder(Window *pParent, rtl::OUString sUri, rtl::OString sID)
    : m_sID(sID)
    , m_pParent(pParent)
{
    xmlreader::XmlReader reader(sUri);

    handleChild(pParent, reader);

    //Set radiobutton groups when everything has been imported
    for (std::vector<RadioButtonGroupMap>::iterator aI = m_aGroupMaps.begin(),
         aEnd = m_aGroupMaps.end(); aI != aEnd; ++aI)
    {
        RadioButton *pOne = static_cast<RadioButton*>(get_by_name(aI->m_sID));
        RadioButton *pOther = static_cast<RadioButton*>(get_by_name(aI->m_sValue));
        SAL_WARN_IF(!pOne || !pOther, "vcl", "missing member of radiobutton group");
        if (pOne && pOther)
            pOne->group(*pOther);
    }
    //drop maps now
    std::vector<RadioButtonGroupMap>().swap(m_aGroupMaps);

    //Set ComboBox models when everything has been imported
    for (std::vector<ComboBoxModelMap>::iterator aI = m_aModelMaps.begin(),
         aEnd = m_aModelMaps.end(); aI != aEnd; ++aI)
    {
        ListBox *pTarget = static_cast<ListBox*>(get_by_name(aI->m_sID));
        ListStore *pStore = static_cast<ListStore*>(get_model_by_name(aI->m_sValue));
        SAL_WARN_IF(!pTarget || !pStore, "vcl", "missing elements of combobox/liststore");
        if (pTarget && pStore)
            mungemodel(*pTarget, *pStore);
    }
    //drop maps now
    std::vector<ComboBoxModelMap>().swap(m_aModelMaps);
    for (std::vector<ModelAndId>::iterator aI = m_aModels.begin(),
         aEnd = m_aModels.end(); aI != aEnd; ++aI)
    {
        delete aI->m_pModel;
    }
    std::vector<ModelAndId>().swap(m_aModels);

    //auto-show (really necessary ?, maybe drop it when complete)
    for (std::vector<WinAndId>::iterator aI = m_aChildren.begin(),
         aEnd = m_aChildren.end(); aI != aEnd; ++aI)
    {
        Window *pWindow = aI->m_pWindow;
        if (pWindow)
        {
            pWindow->Show();
        }
    }
}

VclBuilder::~VclBuilder()
{
    for (std::vector<WinAndId>::reverse_iterator aI = m_aChildren.rbegin(),
         aEnd = m_aChildren.rend(); aI != aEnd; ++aI)
    {
        if (aI->m_bOwned)
            delete aI->m_pWindow;
    }
}

namespace
{
    bool extractOrientation(VclBuilder::stringmap &rMap)
    {
        bool bVertical = false;
        VclBuilder::stringmap::iterator aFind = rMap.find(rtl::OString(RTL_CONSTASCII_STRINGPARAM("orientation")));
        if (aFind != rMap.end())
        {
            bVertical = aFind->second.equalsIgnoreAsciiCaseL(RTL_CONSTASCII_STRINGPARAM("vertical"));
            rMap.erase(aFind);
        }
        return bVertical;
    }

    Window * extractStockAndBuildButton(Window *pParent, VclBuilder::stringmap &rMap)
    {
        WinBits nBits = WB_CENTER|WB_VCENTER|WB_3DLOOK;

        bool bIsStock = false;
        VclBuilder::stringmap::iterator aFind = rMap.find(rtl::OString(RTL_CONSTASCII_STRINGPARAM("use-stock")));
        if (aFind != rMap.end())
        {
            bIsStock = toBool(aFind->second);
            rMap.erase(aFind);
        }

        Window *pWindow = NULL;

        if (bIsStock)
        {
            rtl::OString sType;
            aFind = rMap.find(rtl::OString(RTL_CONSTASCII_STRINGPARAM("label")));
            if (aFind != rMap.end())
            {
                sType = aFind->second;
                rMap.erase(aFind);
            }

            if (sType.equalsL(RTL_CONSTASCII_STRINGPARAM("gtk-ok")))
                pWindow = new OKButton(pParent, nBits);
            else if (sType.equalsL(RTL_CONSTASCII_STRINGPARAM("gtk-cancel")))
                pWindow = new CancelButton(pParent, nBits);
            else if (sType.equalsL(RTL_CONSTASCII_STRINGPARAM("gtk-help")))
                pWindow = new HelpButton(pParent, nBits);
            else
                fprintf(stderr, "unknown stock type %s\n", sType.getStr());
        }

        if (!pWindow)
            pWindow = new PushButton(pParent, nBits);
        return pWindow;
    }
}

bool VclBuilder::extractGroup(const rtl::OString &id, stringmap &rMap)
{
    VclBuilder::stringmap::iterator aFind = rMap.find(rtl::OString(RTL_CONSTASCII_STRINGPARAM("group")));
    if (aFind != rMap.end())
    {
        m_aGroupMaps.push_back(RadioButtonGroupMap(id, aFind->second));
        rMap.erase(aFind);
        return true;
    }
    return false;
}

bool VclBuilder::extractModel(const rtl::OString &id, stringmap &rMap)
{
    VclBuilder::stringmap::iterator aFind = rMap.find(rtl::OString(RTL_CONSTASCII_STRINGPARAM("model")));
    if (aFind != rMap.end())
    {
        m_aModelMaps.push_back(ComboBoxModelMap(id, aFind->second));
        rMap.erase(aFind);
        return true;
    }
    return false;
}

Window *VclBuilder::makeObject(Window *pParent, const rtl::OString &name, const rtl::OString &id, stringmap &rMap)
{
    if (!m_aParentTypes.empty() && m_aParentTypes.top().equalsL(RTL_CONSTASCII_STRINGPARAM("GtkNotebook")))
    {
        //We have to add a page
        TabControl *pTabControl = static_cast<TabControl*>(pParent);
        TabPage* pPage = new TabPage(pTabControl);
        m_aChildren.push_back(WinAndId(rtl::OString(), pPage));

        //And give the page one container as a child to make it a layout enabled
        //tab page
        VclBin* pContainer = new VclBin(pPage);
        m_aChildren.push_back(WinAndId(rtl::OString(), pContainer));
        pParent = pContainer;

        //keep it simple and make pageid == position
        sal_uInt16 nNewPageId = pTabControl->GetPageCount()+1;
        pTabControl->InsertPage(nNewPageId, rtl::OUString());
        pTabControl->SetTabPage(nNewPageId, pPage);
        pTabControl->SetCurPageId(nNewPageId);
    }

    Window *pWindow = NULL;
    if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkDialog")))
        pWindow = new Dialog(pParent, WB_SIZEMOVE|WB_3DLOOK|WB_CLOSEABLE);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkBox")))
    {
        if (extractOrientation(rMap))
            pWindow = new VclVBox(pParent);
        else
            pWindow = new VclHBox(pParent);
    }
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkButtonBox")))
    {
        if (extractOrientation(rMap))
            pWindow = new VclVButtonBox(pParent);
        else
            pWindow = new VclHButtonBox(pParent);
    }
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkGrid")))
        pWindow = new VclGrid(pParent);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkFrame")))
        pWindow = new VclFrame(pParent);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkAlignment")))
        pWindow = new VclAlignment(pParent);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkButton")))
        pWindow = extractStockAndBuildButton(pParent, rMap);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkRadioButton")))
    {
        extractGroup(id, rMap);
        pWindow = new RadioButton(pParent, WB_CENTER|WB_VCENTER|WB_3DLOOK);
    }
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkCheckButton")))
        pWindow = new CheckBox(pParent, WB_CENTER|WB_VCENTER|WB_3DLOOK);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkSpinButton")))
        pWindow = new MetricField(pParent, WB_RIGHT|WB_SPIN|WB_BORDER|WB_3DLOOK);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkComboBox")))
    {
        extractModel(id, rMap);
        pWindow = new ListBox(pParent, WB_LEFT|WB_DROPDOWN|WB_VCENTER|WB_3DLOOK);
    }
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkLabel")))
        pWindow = new FixedText(pParent, WB_CENTER|WB_VCENTER|WB_3DLOOK);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkEntry")))
        pWindow = new Edit(pParent, WB_LEFT|WB_VCENTER|WB_BORDER|WB_3DLOOK);
    else if (name.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkNotebook")))
        pWindow = new TabControl(pParent, WB_STDTABCONTROL|WB_3DLOOK);
    else
        fprintf(stderr, "TO-DO, implement %s\n", name.getStr());
    if (pWindow)
    {
        fprintf(stderr, "for %s, created %p child of %p (%p/%p/%p)\n", name.getStr(), pWindow, pParent, pWindow->mpWindowImpl->mpParent, pWindow->mpWindowImpl->mpRealParent, pWindow->mpWindowImpl->mpBorderWindow);
        m_aChildren.push_back(WinAndId(id, pWindow));
    }
    return pWindow;
}

Window *VclBuilder::insertObject(Window *pParent, const rtl::OString &rClass, const rtl::OString &rID, stringmap &rMap)
{
    Window *pCurrentChild = NULL;

    if (!m_sID.isEmpty() && rID.equals(m_sID))
    {
        pCurrentChild = m_pParent;
        //toplevels default to resizable
        if (pCurrentChild->IsDialog())
            pCurrentChild->SetStyle(pCurrentChild->GetStyle() | WB_SIZEMOVE | WB_3DLOOK);
    }
    else
    {
        pCurrentChild = makeObject(pParent, rClass, rID, rMap);
        if (!pCurrentChild)
            fprintf(stderr, "missing object!\n");
    }

    if (pCurrentChild)
    {
        m_aParentTypes.push(rClass);
        for (stringmap::iterator aI = rMap.begin(), aEnd = rMap.end(); aI != aEnd; ++aI)
        {
            const rtl::OString &rKey = aI->first;
            const rtl::OString &rValue = aI->second;
            pCurrentChild->set_property(rKey, rValue);
        }
    }

    rMap.clear();

    if (!pCurrentChild)
    {
        fprintf(stderr, "missing object!\n");
        pCurrentChild = m_aChildren.empty() ? pParent : m_aChildren.back().m_pWindow;
    }
    return pCurrentChild;
}

sal_uInt16 VclBuilder::getPositionWithinParent(Window &rWindow)
{
    if (rWindow.mpWindowImpl->mpParent != rWindow.mpWindowImpl->mpRealParent)
    {
        assert(rWindow.mpWindowImpl->mpBorderWindow ==
            rWindow.mpWindowImpl->mpParent);
        assert(rWindow.mpWindowImpl->mpBorderWindow->mpParent ==
            rWindow.mpWindowImpl->mpRealParent);
        return getPositionWithinParent(*rWindow.mpWindowImpl->mpBorderWindow);
    }

    assert(rWindow.GetParent() == rWindow.GetRealParent());

    sal_uInt16 nPosition = 0;
    Window* pChild = rWindow.GetParent()->mpWindowImpl->mpFirstChild;
    while (pChild)
    {
        if (pChild == &rWindow)
            break;
        pChild = pChild->mpWindowImpl->mpNext;
        ++nPosition;
    }
    return nPosition;
}

void VclBuilder::reorderWithinParent(Window &rWindow, sal_uInt16 nNewPosition)
{
    if (rWindow.mpWindowImpl->mpParent != rWindow.mpWindowImpl->mpRealParent)
    {
        assert(rWindow.mpWindowImpl->mpBorderWindow ==
            rWindow.mpWindowImpl->mpParent);
        assert(rWindow.mpWindowImpl->mpBorderWindow->mpParent ==
            rWindow.mpWindowImpl->mpRealParent);
        reorderWithinParent(*rWindow.mpWindowImpl->mpBorderWindow, nNewPosition);
        return;
    }
    rWindow.reorderWithinParent(nNewPosition);
}

void VclBuilder::handleTabChild(Window *pParent, xmlreader::XmlReader &reader)
{
    int nLevel = 1;
    stringmap aProperties;
    while(1)
    {
        xmlreader::Span name;
        int nsId;

        xmlreader::XmlReader::Result res = reader.nextItem(
            xmlreader::XmlReader::TEXT_NONE, &name, &nsId);

        if (res == xmlreader::XmlReader::RESULT_BEGIN)
        {
            ++nLevel;
            if (name.equals(RTL_CONSTASCII_STRINGPARAM("property")))
                collectProperty(reader, aProperties);
        }

        if (res == xmlreader::XmlReader::RESULT_END)
            --nLevel;

        if (!nLevel)
            break;

        if (res == xmlreader::XmlReader::RESULT_DONE)
            break;
    }

    VclBuilder::stringmap::iterator aFind = aProperties.find(rtl::OString(RTL_CONSTASCII_STRINGPARAM("label")));
    if (aFind != aProperties.end())
    {
        TabControl *pTabControl = static_cast<TabControl*>(pParent);
        pTabControl->SetPageText(pTabControl->GetCurPageId(), rtl::OStringToOUString(aFind->second, RTL_TEXTENCODING_UTF8));
    }
}

void VclBuilder::handleChild(Window *pParent, xmlreader::XmlReader &reader)
{
    Window *pCurrentChild = NULL;

    xmlreader::Span name;
    int nsId;
    rtl::OString sType;

    while (reader.nextAttribute(&nsId, &name))
    {
        if (name.equals(RTL_CONSTASCII_STRINGPARAM("type")))
        {
            name = reader.getAttributeValue(false);
            sType = rtl::OString(name.begin, name.length);
        }
    }

    if (sType.equalsL(RTL_CONSTASCII_STRINGPARAM("tab")))
    {
        handleTabChild(pParent, reader);
        return;
    }

    int nLevel = 1;
    while(1)
    {
        xmlreader::XmlReader::Result res = reader.nextItem(
            xmlreader::XmlReader::TEXT_NONE, &name, &nsId);

        if (res == xmlreader::XmlReader::RESULT_BEGIN)
        {
            if (name.equals(RTL_CONSTASCII_STRINGPARAM("object")))
            {
                pCurrentChild = handleObject(pParent, reader);

                if (pCurrentChild)
                {
                    //Select the first page if its a notebook
                    if (!m_aParentTypes.empty() && m_aParentTypes.top().equalsL(RTL_CONSTASCII_STRINGPARAM("GtkNotebook")))
                    {
                        TabControl *pTabControl = static_cast<TabControl*>(pCurrentChild);
                        pTabControl->SetCurPageId(1);

                        //To-Do add reorder capability to the TabControl
                    }
                    else
                    {
                        //To-Do make reorder a virtual in Window, move this foo
                        //there and see above

                        rtl::OString sPosition(RTL_CONSTASCII_STRINGPARAM("position"));
                        std::vector<Window*> aChilds;
                        for (Window* pChild = pCurrentChild->GetWindow(WINDOW_FIRSTCHILD); pChild;
                            pChild = pChild->GetWindow(WINDOW_NEXT))
                        {
                            aChilds.push_back(pChild);
                        }

                        for (size_t i = 0; i < aChilds.size(); ++i)
                        {
                            sal_uInt16 nPosition = aChilds[i]->getWidgetProperty<sal_uInt16>(sPosition, 0xFFFF);
                            if (nPosition == 0xFFFF)
                                continue;
                            reorderWithinParent(*aChilds[i], nPosition);
                        }

#if TODO
//sort by ltr ttb
                        rtl::OString sLeftAttach(RTL_CONSTASCII_STRINGPARAM("left-attach"));
                        rtl::OString sTopAttach(RTL_CONSTASCII_STRINGPARAM("top-attach"));
                        for (size_t i = 0; i < aChilds.size(); ++i)
                        {
                            sal_uInt16 nPosition = aChilds[i]->getWidgetProperty<sal_uInt16>(sPosition, 0xFFFF);
                            if (nPosition == 0xFFFF)
                                continue;
                            reorderWithinParent(*aChilds[i], nPosition);
                        }
#endif
                    }

                    if (!m_aParentTypes.empty())
                        m_aParentTypes.pop();
                }
            }
            else if (name.equals(RTL_CONSTASCII_STRINGPARAM("packing")))
            {
                handlePacking(pCurrentChild, reader);
            }
            else
                ++nLevel;
        }

        if (res == xmlreader::XmlReader::RESULT_END)
            --nLevel;

        if (!nLevel)
            break;

        if (res == xmlreader::XmlReader::RESULT_DONE)
            break;
    }
}

void VclBuilder::handleListStore(xmlreader::XmlReader &reader, const rtl::OString &rID)
{
    m_aModels.push_back(ModelAndId(rID, new ListStore));

    int nLevel = 1;

    while(1)
    {
        xmlreader::Span name;
        int nsId;

        xmlreader::XmlReader::Result res = reader.nextItem(
            xmlreader::XmlReader::TEXT_NONE, &name, &nsId);

        if (res == xmlreader::XmlReader::RESULT_DONE)
            break;

        if (res == xmlreader::XmlReader::RESULT_BEGIN)
        {
            ++nLevel;
            if (name.equals(RTL_CONSTASCII_STRINGPARAM("col")))
            {
                reader.nextItem(
                    xmlreader::XmlReader::TEXT_NORMALIZED, &name, &nsId);
                rtl::OString sValue(name.begin, name.length);
                m_aModels.back().m_pModel->m_aEntries.push_back(sValue);
            }
        }

        if (res == xmlreader::XmlReader::RESULT_END)
        {
            --nLevel;
        }

        if (!nLevel)
            break;
    }
}

Window* VclBuilder::handleObject(Window *pParent, xmlreader::XmlReader &reader)
{
    rtl::OString sClass;
    rtl::OString sID;

    xmlreader::Span name;
    int nsId;

    while (reader.nextAttribute(&nsId, &name))
    {
        if (name.equals(RTL_CONSTASCII_STRINGPARAM("class")))
        {
            name = reader.getAttributeValue(false);
            sClass = rtl::OString(name.begin, name.length);
        }
        else if (name.equals(RTL_CONSTASCII_STRINGPARAM("id")))
        {
            name = reader.getAttributeValue(false);
            sID = rtl::OString(name.begin, name.length);
        }

    }

    if (sClass.equalsL(RTL_CONSTASCII_STRINGPARAM("GtkListStore")))
    {
        handleListStore(reader, sID);
        return NULL;
    }

    int nLevel = 1;

    stringmap aProperties;

    Window *pCurrentChild = NULL;
    while(1)
    {
        xmlreader::XmlReader::Result res = reader.nextItem(
            xmlreader::XmlReader::TEXT_NONE, &name, &nsId);

        if (res == xmlreader::XmlReader::RESULT_DONE)
            break;

        if (res == xmlreader::XmlReader::RESULT_BEGIN)
        {
            if (name.equals(RTL_CONSTASCII_STRINGPARAM("child")))
            {
                if (!pCurrentChild)
                    pCurrentChild = insertObject(pParent, sClass, sID, aProperties);
                handleChild(pCurrentChild, reader);
            }
            else
            {
                ++nLevel;
                if (name.equals(RTL_CONSTASCII_STRINGPARAM("property")))
                    collectProperty(reader, aProperties);
            }
        }

        if (res == xmlreader::XmlReader::RESULT_END)
        {
            --nLevel;
        }

        if (!nLevel)
            break;
    }

    if (!pCurrentChild)
        pCurrentChild = insertObject(pParent, sClass, sID, aProperties);

    return pCurrentChild;
}

void VclBuilder::handlePacking(Window *pCurrent, xmlreader::XmlReader &reader)
{
    xmlreader::Span name;
    int nsId;

    int nLevel = 1;

    while(1)
    {
        xmlreader::XmlReader::Result res = reader.nextItem(
            xmlreader::XmlReader::TEXT_NONE, &name, &nsId);

        if (res == xmlreader::XmlReader::RESULT_DONE)
            break;

        if (res == xmlreader::XmlReader::RESULT_BEGIN)
        {
            ++nLevel;
            if (name.equals(RTL_CONSTASCII_STRINGPARAM("property")))
                applyPackingProperty(pCurrent, reader);
        }

        if (res == xmlreader::XmlReader::RESULT_END)
        {
            --nLevel;
        }

        if (!nLevel)
            break;
    }
}

void VclBuilder::applyPackingProperty(Window *pCurrent,
    xmlreader::XmlReader &reader)
{
    xmlreader::Span name;
    int nsId;

    if (!pCurrent)
        return;

    while (reader.nextAttribute(&nsId, &name))
    {
        if (name.equals(RTL_CONSTASCII_STRINGPARAM("name")))
        {
            name = reader.getAttributeValue(false);
            rtl::OString sKey(name.begin, name.length);
            sKey = sKey.replace('_', '-');
            reader.nextItem(
                xmlreader::XmlReader::TEXT_NORMALIZED, &name, &nsId);
            rtl::OString sValue(name.begin, name.length);

            if ( sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("expand")) ||
                 sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("fill")) )
            {
                bool bTrue = (sValue[0] == 't' || sValue[0] == 'T' || sValue[0] == '1');
                pCurrent->setChildProperty(sKey, bTrue);
            }
            else if (sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("position")))
            {
                pCurrent->setChildProperty(sKey, static_cast<sal_uInt16>(sValue.toInt32()));
            }
            else if (sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("pack-type")))
            {
                sal_Int32 nPackType = (sValue[0] == 'e' || sValue[0] == 'e') ? VCL_PACK_END : VCL_PACK_START;
                pCurrent->setChildProperty(sKey, nPackType);
            }
            else if (
                      sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("left-attach")) ||
                      sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("top-attach")) ||
                      sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("width")) ||
                      sKey.equalsL(RTL_CONSTASCII_STRINGPARAM("height"))
                    )
            {
                pCurrent->setChildProperty(sKey, sValue.toInt32());
            }
            else
                fprintf(stderr, "unknown packing %s\n", sKey.getStr());
        }
    }
}

void VclBuilder::collectProperty(xmlreader::XmlReader &reader, stringmap &rMap)
{
    xmlreader::Span name;
    int nsId;

    while (reader.nextAttribute(&nsId, &name))
    {
        if (name.equals(RTL_CONSTASCII_STRINGPARAM("name")))
        {
            name = reader.getAttributeValue(false);
            rtl::OString sProperty(name.begin, name.length);
            sProperty = sProperty.replace('_', '-');
            reader.nextItem(
                xmlreader::XmlReader::TEXT_NORMALIZED, &name, &nsId);
            rtl::OString sValue(name.begin, name.length);
            //replace '_' with '-' except for property values that
            //refer to widget ids themselves. TO-DO, drop conversion
            //and just use foo_bar properties throughout
            if (sProperty.equalsL(RTL_CONSTASCII_STRINGPARAM("group")))
                rMap[sProperty] = sValue;
            else
                rMap[sProperty] = sValue.replace('_', '-');
        }
    }
}

Window *VclBuilder::get_widget_root()
{
    return m_aChildren.empty() ? NULL : m_aChildren[0].m_pWindow;
}

Window *VclBuilder::get_by_name(rtl::OString sID)
{
    for (std::vector<WinAndId>::iterator aI = m_aChildren.begin(),
         aEnd = m_aChildren.end(); aI != aEnd; ++aI)
    {
        if (aI->m_sID.equals(sID))
            return aI->m_pWindow;
    }

    return NULL;
}

VclBuilder::ListStore *VclBuilder::get_model_by_name(rtl::OString sID)
{
    for (std::vector<ModelAndId>::iterator aI = m_aModels.begin(),
         aEnd = m_aModels.end(); aI != aEnd; ++aI)
    {
        if (aI->m_sID.equals(sID))
            return aI->m_pModel;
    }

    return NULL;
}

void VclBuilder::swapGuts(Window &rOrig, Window &rReplacement)
{
#if 1
    if (rOrig.mpWindowImpl->mpBorderWindow)
        fprintf(stderr, "problem one\n");

    sal_uInt16 nPosition = getPositionWithinParent(rOrig);

    if (rReplacement.mpWindowImpl->mpBorderWindow)
        fprintf(stderr, "problem two\n");

    rReplacement.take_properties(rOrig);

    reorderWithinParent(rReplacement, nPosition);

    assert(nPosition == getPositionWithinParent(rReplacement));
#else
    //rReplacement is intended to be not inserted into
    //anything yet
    assert(!rReplacement.mpWindowImpl->mpParent);

    rReplacement.ImplInit(rOrig.GetParent(), rOrig.GetStyle(), NULL);
    assert(rReplacement.GetParent() == rOrig.GetParent());

    rReplacement.ImplRemoveWindow(false);

    Window *pParent = rOrig.mpWindowImpl->mpParent;
    Window *pOrigsNext = rOrig.mpWindowImpl->mpNext;
    Window *pOrigsPrev = rOrig.mpWindowImpl->mpPrev;
    std::swap(rOrig.mpWindowImpl, rReplacement.mpWindowImpl);
    std::swap(rOrig.m_aWidgetProperties, rReplacement.m_aWidgetProperties);
    assert(rReplacement.mpWindowImpl->mpPrev == pOrigsPrev);
    assert(rReplacement.mpWindowImpl->mpNext == pOrigsNext);
    if (pOrigsNext)
        pOrigsNext->mpWindowImpl->mpPrev = &rReplacement;
    else
        pParent->mpWindowImpl->mpLastChild = &rReplacement;
    if (pOrigsPrev)
        pOrigsPrev->mpWindowImpl->mpNext = &rReplacement;
    else
        pParent->mpWindowImpl->mpFirstChild = &rReplacement;
    assert(!rOrig.mpWindowImpl->mpNext && !rOrig.mpWindowImpl->mpPrev);

    //now put this at the end of the window list
    rOrig.ImplInsertWindow(pParent);
#endif
    fprintf(stderr, "swapped %p for %p %p/%p/%p\n", &rReplacement, &rOrig, rReplacement.mpWindowImpl->mpParent, rReplacement.mpWindowImpl->mpRealParent, rReplacement.mpWindowImpl->mpBorderWindow);
}

bool VclBuilder::replace(rtl::OString sID, Window &rReplacement)
{
    for (std::vector<WinAndId>::iterator aI = m_aChildren.begin(),
         aEnd = m_aChildren.end(); aI != aEnd; ++aI)
    {
        if (aI->m_sID.equals(sID))
        {
            Window *pOrig = aI->m_pWindow;
            swapGuts(*pOrig, rReplacement);
            delete pOrig;

            aI->m_pWindow = &rReplacement;
            aI->m_bOwned = false;
            return true;
        }
    }
    fprintf(stderr, "no sign of %s\n", sID.getStr());
    return false;
}

void VclBuilder::mungemodel(ListBox &rTarget, ListStore &rStore)
{
    for (std::vector<rtl::OString>::iterator aI = rStore.m_aEntries.begin(), aEnd = rStore.m_aEntries.end();
        aI != aEnd; ++aI)
    {
        rTarget.InsertEntry(rtl::OStringToOUString(*aI, RTL_TEXTENCODING_UTF8));
    }
    if (!rStore.m_aEntries.empty())
        rTarget.SelectEntryPos(0);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
