/*************************************************************************
 *
 *  $RCSfile: hyprdlg.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-17 18:07:50 $
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
#ifndef _SVX_DLG_HYPERDLG_HXX
#define _SVX_DLG_HYPERDLG_HXX

#ifndef _SV_COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _SV_MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
#ifndef _SFXCTRLITEM_HXX //autogen
#include <sfx2/ctrlitem.hxx>
#endif
#ifndef _SVX_SRCHCFG_HXX
#include "srchcfg.hxx"
#endif

#ifndef _SFXTBXCTRL_HXX
#include <sfx2/tbxctrl.hxx>
#endif

class SvxHyperlinkDlg;
class SfxViewFrame;

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

class HyperCombo : public ComboBox
{
    SvxHyperlinkDlg *pDlg;
    long            nMaxWidth;
    long            nMinWidth;
    long            nRatio;

    virtual long Notify( NotifyEvent& rNEvt );
    virtual long PreNotify( NotifyEvent& rNEvt );

public:
    HyperCombo( SvxHyperlinkDlg* pDialog, const ResId& rResId );

    inline void SetRatio( long nR ) { nRatio = nR; }
    inline long GetRatio()          { return nRatio; }
    long        CalcResizeWidth( long nW );
    inline long GetResizeWidth()    { return (nMaxWidth - nMinWidth); }
    void        DoResize( long nW );
};

class HyperFixedText : public FixedInfo
{
    SvxHyperlinkDlg *pDlg;

public:
    HyperFixedText( SvxHyperlinkDlg* pDialog, const ResId& rResId );

};

/*--------------------------------------------------------------------
    Beschreibung: Dialog zum Einf�gen/�ndern eines Hyperlink
 --------------------------------------------------------------------*/

class SvxHyperlinkDlg : public ToolBox, public SfxControllerItem
{
    friend class HyperCombo;
    friend class HyperFixedText;
private:
    SfxStatusForwarder  aForwarder;
    SfxStatusForwarder  aHyperlinkDlgForward;
    HyperCombo          aNameCB;
    HyperFixedText      aUrlFT;
    HyperCombo          aUrlCB;

    SvxSearchConfig     aSearchConfig;

    String              sAddress;
    String              sExplorer;
    String              sOldName;
    String              sSearchTitle;
    PopupMenu           aLinkPopup;
    PopupMenu           *pTargetMenu;
    Timer               aTimer;
    BOOL                bNoDoc;
    BOOL                bSend;
    BOOL                bHasOldName;
    long                nMaxWidth;
    long                nMinWidth;
    long                nMaxHeight;
    BOOL                bHtmlMode;

    SfxImageManager*    mpManager;

    virtual void    StateChanged( USHORT nSID, SfxItemState eState, const SfxPoolItem* pState );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );

    // DockingWindow
    virtual void Resize();
    virtual void Resizing(Size& rSize);

    // Drag&Drop
    BOOL         GetDragData(USHORT nItem, ULONG nDDFormatId, String& rBuffer);

    DECL_LINK(TimeHdl, Timer *);
    DECL_LINK(TBClickHdl, ToolBox *);
    DECL_LINK(TBSelectHdl, ToolBox *);
    DECL_LINK(TBDeactivateHdl, ToolBox *);
    DECL_LINK(LinkPopupSelectHdl, Menu *);
    DECL_LINK(SearchPopupSelectHdl, Menu *);
    DECL_LINK(ComboSelectHdl, ComboBox *);
    DECL_LINK(ComboModifyHdl, ComboBox *);
    DECL_LINK(BookmarkFoundHdl, String *);

    void    OpenDoc( const String& rURL, SfxViewFrame* pViewFrame );
    void    EnableLink();
    void    SendToApp(USHORT nType);
    void    AddToHistory(const String& rName, const String& rURL);
    void    TargetMenu(const String& rSelEntry, BOOL bExecute);
    String  GetSelTarget();

    void    SetImages();
public:
    SvxHyperlinkDlg(SfxBindings *pBindings, Window* pWindow);
    ~SvxHyperlinkDlg();
};


#endif // _SVX_DLG_HYPERDLG_HXX

