/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcusfiltersimpl.hxx"
#include "orcusinterface.hxx"
#include "orcusxml.hxx"
#include "document.hxx"

#include "svtools/treelistbox.hxx"

#define __ORCUS_STATIC_LIB
#include <orcus/spreadsheet/import_interface.hpp>
#include <orcus/xml_structure_tree.hpp>
#include <orcus/xml_namespace.hpp>
#include <orcus/orcus_xml.hpp>
#include <orcus/global.hpp>

namespace {

ScOrcusXMLTreeParam::EntryData& setUserDataToEntry(
    SvTreeListEntry& rEntry, ScOrcusXMLTreeParam::UserDataStoreType& rStore, ScOrcusXMLTreeParam::EntryType eType)
{
    rStore.push_back(new ScOrcusXMLTreeParam::EntryData(eType));
    rEntry.SetUserData(&rStore.back());
    return rStore.back();
}

void setEntityNameToUserData(
    ScOrcusXMLTreeParam::EntryData& rEntryData,
    const orcus::xml_structure_tree::entity_name& entity, const orcus::xml_structure_tree::walker& walker)
{
    rEntryData.mnNamespaceID = walker.get_xmlns_index(entity.ns);
}

OUString toString(const orcus::xml_structure_tree::entity_name& entity, const orcus::xml_structure_tree::walker& walker)
{
    OUStringBuffer aBuf;
    if (entity.ns)
    {
        // Namespace exists.  Namespaces are displayed as ns0, ns1, ns2, ....
        size_t index = walker.get_xmlns_index(entity.ns);
        if (index == orcus::xml_structure_tree::walker::index_not_found)
            // This namespace doesn't exist in this context. Something has gone wrong.
            aBuf.append("???");
        else
        {
            OString aName = ScOrcusImportXMLParam::getShortNamespaceName(index);
            aBuf.append(OUString(aName.getStr(), aName.getLength(), RTL_TEXTENCODING_UTF8));
        }

        aBuf.append(':');
    }
    aBuf.append(OUString(entity.name.get(), entity.name.size(), RTL_TEXTENCODING_UTF8));
    return aBuf.makeStringAndClear();
}

void populateTree(
   SvTreeListBox& rTreeCtrl, orcus::xml_structure_tree::walker& rWalker,
   const orcus::xml_structure_tree::entity_name& rElemName, bool bRepeat,
   SvTreeListEntry* pParent, ScOrcusXMLTreeParam& rParam)
{
    SvTreeListEntry* pEntry = rTreeCtrl.InsertEntry(toString(rElemName, rWalker), pParent);
    if (!pEntry)
        // Can this ever happen!?
        return;

    ScOrcusXMLTreeParam::EntryData& rEntryData = setUserDataToEntry(
        *pEntry, rParam.maUserDataStore,
        bRepeat ? ScOrcusXMLTreeParam::ElementRepeat : ScOrcusXMLTreeParam::ElementDefault);

    setEntityNameToUserData(rEntryData, rElemName, rWalker);

    if (bRepeat)
    {
        // Recurring elements use different icon.
        rTreeCtrl.SetExpandedEntryBmp(pEntry, rParam.maImgElementRepeat);
        rTreeCtrl.SetCollapsedEntryBmp(pEntry, rParam.maImgElementRepeat);
    }

    if (pParent)
        rTreeCtrl.Expand(pParent);

    orcus::xml_structure_tree::entity_names_type aNames;

    // Insert attributes.
    rWalker.get_attributes(aNames);
    orcus::xml_structure_tree::entity_names_type::const_iterator it = aNames.begin();
    orcus::xml_structure_tree::entity_names_type::const_iterator itEnd = aNames.end();
    for (; it != itEnd; ++it)
    {
        const orcus::xml_structure_tree::entity_name& rAttrName = *it;
        SvTreeListEntry* pAttr = rTreeCtrl.InsertEntry(toString(rAttrName, rWalker), pEntry);

        if (!pAttr)
            continue;

        ScOrcusXMLTreeParam::EntryData& rAttrData =
            setUserDataToEntry(*pAttr, rParam.maUserDataStore, ScOrcusXMLTreeParam::Attribute);
        setEntityNameToUserData(rAttrData, rAttrName, rWalker);

        rTreeCtrl.SetExpandedEntryBmp(pAttr, rParam.maImgAttribute);
        rTreeCtrl.SetCollapsedEntryBmp(pAttr, rParam.maImgAttribute);
    }
    rTreeCtrl.Expand(pEntry);

    rWalker.get_children(aNames);

    // Non-leaf if it has child elements, leaf otherwise.
    rEntryData.mbLeafNode = aNames.empty();

    // Insert child elements recursively.
    for (it = aNames.begin(), itEnd = aNames.end(); it != itEnd; ++it)
    {
        orcus::xml_structure_tree::element aElem = rWalker.descend(*it);
        populateTree(rTreeCtrl, rWalker, *it, aElem.repeat, pEntry, rParam);
        rWalker.ascend();
    }
}

class TreeUpdateSwitch
{
    SvTreeListBox& mrTreeCtrl;
public:
    TreeUpdateSwitch(SvTreeListBox& rTreeCtrl) : mrTreeCtrl(rTreeCtrl)
    {
        mrTreeCtrl.SetUpdateMode(false);
    }

    ~TreeUpdateSwitch()
    {
        mrTreeCtrl.SetUpdateMode(true);
    }
};

class InsertFieldPath : std::unary_function<OString, void>
{
    orcus::orcus_xml& mrFilter;
public:
    InsertFieldPath(orcus::orcus_xml& rFilter) : mrFilter(rFilter) {}
    void operator() (const OString& rPath)
    {
        mrFilter.append_field_link(rPath.getStr());
    }
};

}

ScOrcusXMLContextImpl::ScOrcusXMLContextImpl(ScDocument& rDoc, const OUString& rPath) :
    ScOrcusXMLContext(), mrDoc(rDoc), maPath(rPath) {}

ScOrcusXMLContextImpl::~ScOrcusXMLContextImpl() {}

bool ScOrcusXMLContextImpl::loadXMLStructure(SvTreeListBox& rTreeCtrl, ScOrcusXMLTreeParam& rParam)
{
    rParam.maUserDataStore.clear();

    OString aSysPath = ScOrcusFiltersImpl::toSystemPath(maPath);
    const char* path = aSysPath.getStr();

    // TODO: Use our own stream loading call instead of one from orcus.
    std::string aStrm;
    orcus::load_file_content(path, aStrm);

    if (aStrm.empty())
        return false;

    orcus::xmlns_context cxt = maNsRepo.create_context();
    orcus::xml_structure_tree aXmlTree(cxt);
    try
    {
        aXmlTree.parse(&aStrm[0], aStrm.size());

        TreeUpdateSwitch aSwitch(rTreeCtrl);
        rTreeCtrl.Clear();
        rTreeCtrl.SetDefaultCollapsedEntryBmp(rParam.maImgElementDefault);
        rTreeCtrl.SetDefaultExpandedEntryBmp(rParam.maImgElementDefault);

        orcus::xml_structure_tree::walker aWalker = aXmlTree.get_walker();

        // Root element.
        orcus::xml_structure_tree::element aElem = aWalker.root();
        populateTree(rTreeCtrl, aWalker, aElem.name, aElem.repeat, NULL, rParam);
    }
    catch (const std::exception&)
    {
        // Parsing of this XML file failed.
        return false;
    }

    return true;
}

namespace {

class SetNamespaceAlias : std::unary_function<size_t, void>
{
    orcus::orcus_xml& mrFilter;
    orcus::xmlns_repository& mrNsRepo;
public:
    SetNamespaceAlias(orcus::orcus_xml& filter, orcus::xmlns_repository& repo) :
        mrFilter(filter), mrNsRepo(repo) {}

    void operator() (size_t index)
    {
        orcus::xmlns_id_t nsid = mrNsRepo.get_identifier(index);
        if (nsid == orcus::XMLNS_UNKNOWN_ID)
            return;

        OString aAlias = ScOrcusImportXMLParam::getShortNamespaceName(index);
        mrFilter.set_namespace_alias(aAlias.getStr(), nsid);
    }
};


}

bool ScOrcusXMLContextImpl::importXML(const ScOrcusImportXMLParam& rParam)
{
    ScOrcusFactory aFactory(mrDoc);
    OString aSysPath = ScOrcusFiltersImpl::toSystemPath(maPath);
    const char* path = aSysPath.getStr();
    try
    {
        orcus::orcus_xml filter(maNsRepo, &aFactory, NULL);

        // Define all used namespaces.
        std::for_each(rParam.maNamespaces.begin(), rParam.maNamespaces.end(), SetNamespaceAlias(filter, maNsRepo));

        {
            // Set cell links.
            ScOrcusImportXMLParam::CellLinksType::const_iterator it = rParam.maCellLinks.begin();
            ScOrcusImportXMLParam::CellLinksType::const_iterator itEnd = rParam.maCellLinks.end();
            for (; it != itEnd; ++it)
            {
                const ScOrcusImportXMLParam::CellLink& rLink = *it;
                OUString aTabName;
                mrDoc.GetName(rLink.maPos.Tab(), aTabName);
                filter.set_cell_link(
                    rLink.maPath.getStr(),
                    rtl::OUStringToOString(aTabName, RTL_TEXTENCODING_UTF8).getStr(),
                    rLink.maPos.Row(), rLink.maPos.Col());
            }
        }

        {
            // Set range links.
            ScOrcusImportXMLParam::RangeLinksType::const_iterator it = rParam.maRangeLinks.begin();
            ScOrcusImportXMLParam::RangeLinksType::const_iterator itEnd = rParam.maRangeLinks.end();
            for (; it != itEnd; ++it)
            {
                const ScOrcusImportXMLParam::RangeLink& rLink = *it;
                OUString aTabName;
                mrDoc.GetName(rLink.maPos.Tab(), aTabName);
                filter.start_range(
                    rtl::OUStringToOString(aTabName, RTL_TEXTENCODING_UTF8).getStr(),
                    rLink.maPos.Row(), rLink.maPos.Col());

                std::for_each(rLink.maFieldPaths.begin(), rLink.maFieldPaths.end(), InsertFieldPath(filter));

                filter.commit_range();
            }
        }

        filter.read_file(path);
    }
    catch (const std::exception&)
    {
        return false;
    }
    return true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
