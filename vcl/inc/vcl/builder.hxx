/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _VCLBUILDER_HXX
#define _VCLBUILDER_HXX

#include <vcl/dllapi.h>
#include <vcl/window.hxx>
#include <xmlreader/xmlreader.hxx>
#include <map>
#include <stack>
#include <vector>

class ListBox;
class NumericFormatter;

class VCL_DLLPUBLIC VclBuilder
{
public:
    typedef std::map<rtl::OString, rtl::OString> stringmap;
    typedef Window* (*customMakeWidget)(Window *pParent, stringmap &rVec);
private:
    struct WinAndId
    {
        rtl::OString m_sID;
        Window *m_pWindow;
        sal_Int32 m_nPosition;
        bool m_bOwned;
        WinAndId(const rtl::OString &rId, Window *pWindow)
            : m_sID(rId)
            , m_pWindow(pWindow)
            , m_nPosition(-1)
            , m_bOwned(true)
        {
        }
    };
    std::vector<WinAndId> m_aChildren;

    struct ListStore
    {
        typedef std::vector<rtl::OString> row;
        std::vector<row> m_aEntries;
    };

    struct ModelAndId
    {
        rtl::OString m_sID;
        ListStore *m_pModel;
        ModelAndId(const rtl::OString &rId, ListStore *pListStore)
            : m_sID(rId)
            , m_pModel(pListStore)
        {
        }
    };

    struct StringPair
    {
        rtl::OString m_sID;
        rtl::OString m_sValue;
        StringPair(const rtl::OString &rId, const rtl::OString &rValue)
            : m_sID(rId)
            , m_sValue(rValue)
        {
        }
    };

    typedef StringPair RadioButtonGroupMap;
    typedef StringPair ComboBoxModelMap;

    ListStore *get_model_by_name(rtl::OString sID);
    static void mungemodel(ListBox &rTarget, ListStore &rStore);

    typedef stringmap Adjustment;

    struct AdjustmentAndId
    {
        rtl::OString m_sID;
        Adjustment m_aAdjustment;
        AdjustmentAndId(const rtl::OString &rId, Adjustment &rAdjustment)
            : m_sID(rId)
        {
            m_aAdjustment.swap(rAdjustment);
        }
    };

    typedef StringPair SpinButtonAdjustmentMap;

    Adjustment *get_adjustment_by_name(rtl::OString sID);
    static void mungeadjustment(NumericFormatter &rTarget, Adjustment &rAdjustment);

    typedef std::map<rtl::OString, rtl::OString> WidgetTranslations;
    typedef std::map<rtl::OString, WidgetTranslations> Translations;

    struct ParserState
    {
        std::vector<RadioButtonGroupMap> m_aGroupMaps;
        std::vector<ComboBoxModelMap> m_aModelMaps;
        std::vector<ModelAndId> m_aModels;
        std::vector<AdjustmentAndId> m_aAdjustments;
        std::vector<SpinButtonAdjustmentMap> m_aAdjustmentMaps;
        Translations m_aTranslations;
    };

    rtl::OString getTranslation(const rtl::OString &rId, const rtl::OString &rProperty) const;

    rtl::OString m_sID;
    rtl::OString m_sHelpRoot;
    Window *m_pParent;
    ParserState *m_pParserState;

    Window *get_by_name(rtl::OString sID);
public:
    VclBuilder(Window *pParent, rtl::OUString sUIRootDir, rtl::OUString sUIFile, rtl::OString sID = rtl::OString());
    ~VclBuilder();
    Window *get_widget_root();
    template <typename T> T* get(T*& ret, rtl::OString sID)
    {
        Window *w = get_by_name(sID);
        ret = static_cast<T*>(w);

#if OSL_DEBUG_LEVEL > 0
        if (w)
        {
            ret = dynamic_cast<T*>(w);
            assert(ret);
        }
#endif

        return ret;
    }
    template <typename T /*=Window if we had c++11*/> T* get(rtl::OString sID)
    {
        Window *w = get_by_name(sID);
        T* ret = static_cast<T*>(w);

#if OSL_DEBUG_LEVEL > 0
        if (w)
        {
            ret = dynamic_cast<T*>(w);
            assert(ret);
        }
#endif

        return ret;
    }

    rtl::OString get_by_window(const Window *pWindow) const;
    //for the purposes of retrofitting this to the existing code
    //look up sID, clone its properties into replacement and
    //splice replacement into the tree instead of it, without
    //taking ownership of it
    bool replace(rtl::OString sID, Window &rReplacement);
private:
    Window *insertObject(Window *pParent, const rtl::OString &rClass, const rtl::OString &rID, stringmap &rVec);
    Window *makeObject(Window *pParent, const rtl::OString &rClass, const rtl::OString &rID, stringmap &rVec);
    bool extractGroup(const rtl::OString &id, stringmap &rVec);
    bool extractModel(const rtl::OString &id, stringmap &rVec);
    bool extractAdjustment(const rtl::OString &id, stringmap &rVec);

    void handleTranslations(xmlreader::XmlReader &reader);

    void handleChild(Window *pParent, xmlreader::XmlReader &reader);
    Window* handleObject(Window *pParent, xmlreader::XmlReader &reader);
    void handlePacking(Window *pCurrent, xmlreader::XmlReader &reader);
    void applyPackingProperty(Window *pCurrent, xmlreader::XmlReader &reader);
    void collectProperty(xmlreader::XmlReader &reader, const rtl::OString &rID, stringmap &rVec);

    void handleListStore(xmlreader::XmlReader &reader, const rtl::OString &rID);
    void handleRow(xmlreader::XmlReader &reader, const rtl::OString &rID, sal_Int32 nRowIndex);
    void handleAdjustment(const rtl::OString &rID, stringmap &rProperties);
    void handleTabChild(Window *pParent, xmlreader::XmlReader &reader);

    sal_Int32 get_window_packing_position(const Window *pWindow) const;
    void set_window_packing_position(const Window *pWindow, sal_Int32 nPosition);

    //Helpers to retrofit all the existing code the the builder
    static void swapGuts(Window &rOrig, Window &rReplacement);
    static sal_uInt16 getPositionWithinParent(Window &rWindow);
    static void reorderWithinParent(Window &rWindow, sal_uInt16 nNewPosition);
};


//allows retro fitting existing dialogs/tabpages that load a resource
//to load a .ui file instead
//
//vcl requires the Window Children of a Parent Window to be destroyed before
//the Parent Window.  VclBuilderContainer owns the VclBuilder which owns the
//Children Window. So the VclBuilderContainer dtor must be called before
//the Parent Window dtor.
//
//i.e.  class Dialog : public SystemWindow, public VclBuilderContainer
//not   class Dialog : public VclBuilderContainer, public SystemWindow
class ResId;

class VCL_DLLPUBLIC VclBuilderContainer
{
protected:
    VclBuilder *m_pUIBuilder;
public:
    VclBuilderContainer();
    virtual ~VclBuilderContainer();
    static rtl::OUString getUIRootDir();
    static VclBuilder* overrideResourceWithUIXML(Window *pWindow, const ResId& rResId);
    static bool replace_buildable(Window *pParent, const ResId& rResId, Window &rReplacement);
    template <typename T> T* get(T*& ret, rtl::OString sID)
    {
        return m_pUIBuilder->get<T>(ret, sID);
    }
    template <typename T /*=Window if we had c++11*/> T* get(rtl::OString sID)
    {
        return m_pUIBuilder->get<T>(sID);
    }
};

/*
 * @return true if rValue is "True", "true", "1", etc.
 */
bool VCL_DLLPUBLIC toBool(const rtl::OString &rValue);

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
