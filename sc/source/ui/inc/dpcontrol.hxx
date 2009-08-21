/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: document.hxx,v $
 * $Revision: 1.115.36.9 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef SC_DPCONTROL_HXX
#define SC_DPCONTROL_HXX

#include "rtl/ustring.hxx"
#include "tools/gen.hxx"
#include "tools/fract.hxx"
#include "vcl/floatwin.hxx"
#include "vcl/button.hxx"
#include "vcl/scrbar.hxx"
#include "vcl/timer.hxx"
#include "svx/checklbx.hxx"

#include <boost/shared_ptr.hpp>
#include <memory>
#include <hash_map>

namespace com { namespace sun { namespace star {

    namespace accessibility {
        class XAccessible;
    }

}}}

class OutputDevice;
class Point;
class Size;
class StyleSettings;
class Window;
class ScDocument;
class ScAccessibleFilterMenu;

/**
 * This class takes care of physically drawing field button controls inside
 * data pilot tables.
 */
class ScDPFieldButton
{
public:
    ScDPFieldButton(OutputDevice* pOutDev, const StyleSettings* pStyle, const Fraction* pZoomX = NULL, const Fraction* pZoomY = NULL);
    ~ScDPFieldButton();

    void setText(const ::rtl::OUString& rText);
    void setBoundingBox(const Point& rPos, const Size& rSize);
    void setDrawBaseButton(bool b);
    void setDrawPopupButton(bool b);
    void setHasHiddenMember(bool b);
    void setPopupPressed(bool b);
    void draw();

    void getPopupBoundingBox(Point& rPos, Size& rSize) const;
    bool isPopupButton() const;

private:
    void drawPopupButton();

private:
    Point                   maPos;
    Size                    maSize;
    ::rtl::OUString         maText;
    Fraction                maZoomX;
    Fraction                maZoomY;
    OutputDevice*           mpOutDev;
    const StyleSettings*    mpStyle;
    bool                    mbBaseButton;
    bool                    mbPopupButton;
    bool                    mbHasHiddenMember;
    bool                    mbPopupPressed;
};

// ============================================================================

class ScMenuFloatingWindow : public FloatingWindow
{
public:
    /**
     * Action to perform when an event takes place.  Create a sub-class of
     * this to implement the desired action.
     */
    class Action
    {
    public:
        virtual void execute() = 0;
    };

    class MenuItem : public Window
    {
    public:
        explicit MenuItem(Window* pParent);

    private:
        bool mbSelected:1;
        bool mbEnabled:1;
    };

    explicit ScMenuFloatingWindow(Window* pParent, ScDocument* pDoc, USHORT nMenuStackLevel = 0);
    virtual ~ScMenuFloatingWindow();

    virtual void MouseMove(const MouseEvent& rMEvt);
    virtual void MouseButtonDown(const MouseEvent& rMEvt);
    virtual void MouseButtonUp(const MouseEvent& rMEvt);
    virtual void KeyInput(const KeyEvent& rKEvt);
    virtual void Paint(const Rectangle& rRect);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();

    void addMenuItem(const ::rtl::OUString& rText, bool bEnabled, Action* pAction);
    ScMenuFloatingWindow* addSubMenuItem(const ::rtl::OUString& rText, bool bEnabled);
    void setSelectedMenuItem(size_t nPos, bool bSubMenuTimer = true);
    void selectMenuItem(size_t nPos, bool bSelected, bool bSubMenuTimer);
    void clearSelectedMenuItem();
    ScMenuFloatingWindow* getSubMenuWindow(size_t nPos) const;
    size_t getMenuItemCount() const;
    ::rtl::OUString getMenuItemName(size_t nPos) const;
    bool isMenuItemEnabled(size_t nPos) const;
    bool isMenuItemSelected(size_t nPos) const;

    void setName(const ::rtl::OUString& rName);
    const ::rtl::OUString& getName() const;

protected:

    void drawMenuItem(size_t nPos);
    void drawAllMenuItems();
    const Font& getLabelFont() const;

    void executeMenu(size_t nPos);
    size_t getSelectedMenuPos() const;
    void queueLaunchSubMenu(size_t nPos, ScMenuFloatingWindow* pMenu);
    void queueCloseSubMenu();
    void launchSubMenu(bool bSetMenuPos);
    void endSubMenu();

    void fillMenuItemsToAccessible(ScAccessibleFilterMenu* pAccMenu) const;

    ScDocument* getDoc();

protected:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > mxAccessible;

private:
    struct SubMenuItemData;
    void handleMenuTimeout(SubMenuItemData* pTimer);

    enum NotificationType { SUBMENU_FOCUSED };
    void notify(NotificationType eType);

    void resetMenu(bool bSetMenuPos);
    void resizeToFitMenuItems();
    void highlightMenuItem(size_t nPos, bool bSelected);

    void getMenuItemPosSize(size_t nPos, Point& rPos, Size& rSize) const;
    size_t getEnclosingMenuItem(const Point& rPos) const;

    DECL_LINK( EndPopupHdl, void* );

private:

    struct MenuItemData
    {
        ::rtl::OUString maText;
        bool            mbEnabled;

        ::boost::shared_ptr<Action> mpAction;
        ::boost::shared_ptr<ScMenuFloatingWindow> mpSubMenuWin;

        MenuItemData();
    };

    ::std::vector<MenuItemData>         maMenuItems;

    struct SubMenuItemData
    {
        Timer                   maTimer;
        ScMenuFloatingWindow*   mpSubMenu;
        size_t                  mnMenuPos;

        DECL_LINK( TimeoutHdl, void* );

        SubMenuItemData(ScMenuFloatingWindow* pParent);
        void reset();

    private:
        ScMenuFloatingWindow* mpParent;
    };
    SubMenuItemData   maOpenTimer;
    SubMenuItemData   maCloseTimer;

    Font    maLabelFont;

    // Name of this menu window, taken from the menu item of the parent window
    // that launches it (if this is a sub menu).  If this is a top-level menu
    // window, then this name can be anything.
    ::rtl::OUString maName;

    size_t  mnSelectedMenu;
    size_t  mnClickedMenu;

    ScDocument* mpDoc;

    ScMenuFloatingWindow* mpParentMenu;
    ScMenuFloatingWindow* mpActiveSubMenu;

    bool    mbActionFired;
};

// ============================================================================

/**
 * This class implements a popup window for field button, for quick access
 * of hide-item list, and possibly more stuff related to field options.
 */
class ScDPFieldPopupWindow : public ScMenuFloatingWindow
{
public:
    /**
     * Extended data that the client code may need to store.  Create a
     * sub-class of this and store data there.
     */
    struct ExtendedData {};

    explicit ScDPFieldPopupWindow(Window* pParent, ScDocument* pDoc);
    virtual ~ScDPFieldPopupWindow();

    virtual void MouseMove(const MouseEvent& rMEvt);
    virtual void Paint(const Rectangle& rRect);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();

    void setMemberSize(size_t n);
    void addMember(const ::rtl::OUString& rName, bool bVisible);
    void initMembers();

    const Size& getWindowSize() const;

    void getResult(::std::hash_map< ::rtl::OUString, bool, ::rtl::OUStringHash>& rResult);
    void close(bool bOK);

    /**
     * Set auxiliary data that the client code might need.  Note that this
     * popup window class manages its life time; no explicit deletion of the
     * instance is needed in the client code.
     */
    void setExtendedData(ExtendedData* p);

    /**
     * Get the store auxiliary data, or NULL if no such data is stored.
     */
    ExtendedData* getExtendedData();

    void setOKAction(Action* p);

private:
    struct Member
    {
        ::rtl::OUString maName;
        bool            mbVisible;

        Member();
    };

    class CancelButton : public ::CancelButton
    {
    public:
        CancelButton(ScDPFieldPopupWindow* pParent);

        virtual void Click();

    private:
        ScDPFieldPopupWindow* mpParent;
    };

    enum SectionType {
        WHOLE,                // entire window
        LISTBOX_AREA_OUTER,   // box enclosing the check box items.
        LISTBOX_AREA_INNER,   // box enclosing the check box items.
        SINGLE_BTN_AREA,      // box enclosing the single-action buttons.
        CHECK_TOGGLE_ALL,     // check box for toggling all items.
        BTN_SINGLE_SELECT,
        BTN_SINGLE_UNSELECT,
        BTN_OK,               // OK button
        BTN_CANCEL,           // Cancel button
    };
    void getSectionPosSize(Point& rPos, Size& rSize, SectionType eType) const;

    void setAllMemberState(bool bSet);
    void selectCurrentMemberOnly(bool bSet);

    DECL_LINK( ButtonHdl, Button* );
    DECL_LINK( TriStateHdl, TriStateBox* );
    DECL_LINK( CheckHdl, SvTreeListBox* );

private:
    SvxCheckListBox maChecks;

    TriStateBox     maChkToggleAll;
    ImageButton     maBtnSelectSingle;
    ImageButton     maBtnUnselectSingle;

    OKButton        maBtnOk;
    CancelButton    maBtnCancel;

    ::std::vector<Member>           maMembers;
    ::std::auto_ptr<ExtendedData>   mpExtendedData;
    ::std::auto_ptr<Action>         mpOKAction;

    const Size      maWndSize;  /// hard-coded window size.
    TriState mePrevToggleAllState;
};

#endif
