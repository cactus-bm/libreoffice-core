/*************************************************************************
 *
 *  $RCSfile: hyperdlg.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: sj $ $Date: 2002-02-19 16:09:15 $
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

#ifndef _SVX_TAB_HYPERLINK_HXX
#define _SVX_TAB_HYPERLINK_HXX

// include ---------------------------------------------------------------

#ifndef _SVX_HLNKITEM_HXX
#include <hlnkitem.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX
#include <sfx2/childwin.hxx>
#endif
#ifndef _SFXCTRLITEM_HXX
#include <sfx2/ctrlitem.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX
#include <sfx2/bindings.hxx>
#endif

#ifndef _SV_IMAGE_HXX
#include <vcl/image.hxx>
#endif

#include "iconcdlg.hxx"

/*************************************************************************
|*
|* Hyperlink-Dialog
|*
\************************************************************************/

class SvxHpLinkDlg;
class SvxHlinkCtrl : public SfxControllerItem
{
private :
    SvxHpLinkDlg *pParent;

    SfxStatusForwarder aOnlineForwarder;
    SfxStatusForwarder aRdOnlyForwarder;

public :
    SvxHlinkCtrl( USHORT nId, SfxBindings & rBindings, SvxHpLinkDlg* pDlg);

    virtual void    StateChanged( USHORT nSID, SfxItemState eState,
                                const SfxPoolItem* pState );
};

/*************************************************************************
|*
|* Hyperlink-Dialog
|*
\************************************************************************/

class SvxHlinkDlgWrapper : public SfxChildWindow
{
public:
    SvxHlinkDlgWrapper( Window*pParent, USHORT nId,
                        SfxBindings* pBindings, SfxChildWinInfo* pInfo );

    SFX_DECL_CHILDWINDOW(SvxHlinkDlgWrapper);
};

/*************************************************************************
|*
|* Hyperlink-Dialog
|*
\************************************************************************/

class SvxHpLinkDlg : public IconChoiceDialog
{
private:
    SvxHlinkCtrl        maCtrl;         // Controler
    SfxBindings*        mpBindings;
    SfxItemSet*         mpItemSet;

    sal_Bool            mbDummy1    : 1;
    sal_Bool            mbDummy2    : 1;
    sal_Bool            mbDummy3    : 1;
    sal_Bool            mbDummy4    : 1;
    sal_Bool            mbDummy5    : 1;
    sal_Bool            mbGrabFocus : 1;
    sal_Bool            mbReadOnly  : 1;
    sal_Bool            mbIsHTMLDoc : 1;

    void*               mpDummy1;
    void*               mpDummy2;

    DECL_LINK (ClickApplyHdl_Impl, void * );
    DECL_LINK (ClickCloseHdl_Impl, void * );

protected:
    virtual BOOL    Close();
    virtual void    Move();

public:
    SvxHpLinkDlg (Window* pParent, SfxBindings* pBindings );
    ~SvxHpLinkDlg ();

    USHORT SetPage( SvxHyperlinkItem* pItem );
    void   EnableInetBrowse( sal_Bool bEnable = sal_True );
    void   SetReadOnlyMode( sal_Bool bReadOnly = sal_False );
    const BOOL IsHTMLDoc() const { return mbIsHTMLDoc; }

    SfxBindings*   GetBindings() const { return mpBindings; };
    SfxDispatcher* GetDispatcher() const { return mpBindings->GetDispatcher(); }
};


#endif // _SVX_TAB_HYPERLINK_HXX
