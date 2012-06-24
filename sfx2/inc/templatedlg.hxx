/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright 2012 LibreOffice contributors.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TEMPLATEDLG_HXX
#define TEMPLATEDLG_HXX

#include <set>

#include <vcl/dialog.hxx>
#include <vcl/button.hxx>

class Edit;
class TemplateFolderView;
class ThumbnailViewItem;
class ToolBox;

namespace com {
    namespace sun { namespace star { namespace frame {
        class XComponentLoader;
    }   }   }
}

class SfxTemplateManagerDlg : public ModalDialog
{
public:

    SfxTemplateManagerDlg (Window *parent = NULL);

    ~SfxTemplateManagerDlg ();

    DECL_LINK(ViewAllHdl, void*);
    DECL_LINK(ViewDocsHdl, void*);
    DECL_LINK(ViewPresentsHdl, void*);
    DECL_LINK(ViewSheetsHdl, void*);
    DECL_LINK(ViewDrawsHdl, void*);

private:

    virtual void MouseButtonDown( const MouseEvent& rMEvt );

    DECL_LINK(OnClickSelectionMode, ImageButton*);

    DECL_LINK(TBXViewHdl, void*);
    DECL_LINK(TBXActionHdl, void*);
    DECL_LINK(TBXTemplateHdl, void*);

    DECL_LINK(TVFolderStateHdl, const ThumbnailViewItem*);
    DECL_LINK(TVTemplateStateHdl, const ThumbnailViewItem*);

    void OnTemplateCreate ();
    void OnTemplateImport ();
    void OnTemplateSearch ();
    void OnTemplateAction ();
    void OnTemplateEdit ();
    void OnTemplateProperties ();
    void OnTemplateMove ();
    void OnTemplateDelete ();

private:

    PushButton aButtonAll;
    PushButton aButtonDocs;
    PushButton aButtonPresents;
    PushButton aButtonSheets;
    PushButton aButtonDraws;
    ImageButton maButtonSelMode;

    Edit *mpSearchEdit;
    ToolBox *mpViewBar;
    ToolBox *mpActionBar;
    ToolBox *mpTemplateBar;
    TemplateFolderView *maView;

    int mnSelectionCount;
    std::set<const ThumbnailViewItem*> maSelTemplates;
    std::set<const ThumbnailViewItem*> maSelFolders;

    com::sun::star::uno::Reference< com::sun::star::frame::XComponentLoader > mxDesktop;
};

#endif // TEMPLATEDLG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
