/*************************************************************************
 *
 *  $RCSfile: browserline.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2004-09-08 17:50:06 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _EXTENSIONS_PROPCTRLR_BROWSERLINE_HXX_
#include "browserline.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_BRWCONTROLLISTENER_HXX_
#include "brwcontrollistener.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

//............................................................................
namespace pcr
{
//............................................................................

    //==================================================================
    //= OBrowserLine
    //==================================================================
    DBG_NAME(OBrowserLine)
    //------------------------------------------------------------------

    OBrowserLine::OBrowserLine( Window* pParent)
            :m_aFtTitle(pParent)
            ,m_pData(NULL)
            ,m_pBrowseButton(NULL)
            ,m_pBrowserControl(NULL)
            ,m_bIsLocked(sal_False)
            ,m_bIsHyperlink(sal_False)
            ,m_nNameWidth(0)
            ,m_pTheParent(pParent)
            ,m_bNeedsRepaint(sal_True)
            ,m_eControlType(BCT_UNDEFINED)
    {
        DBG_CTOR(OBrowserLine,NULL);
        m_aFtTitle.Show();
    }
    //------------------------------------------------------------------
    OBrowserLine::~OBrowserLine()
    {
        if(m_pBrowseButton)
        {
            m_pBrowseButton->Hide();
            delete m_pBrowseButton;
            m_pBrowseButton=NULL;
        }
        DBG_DTOR(OBrowserLine,NULL);
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetHelpId(sal_uInt32 nHelpId,sal_uInt32 nBtUniqueId)
    {
        if(m_pBrowserControl)
            m_pBrowserControl->SetCtrHelpId(nHelpId);

        if(m_pBrowseButton)
        {
            m_pBrowseButton->SetHelpId(nHelpId);
            m_pBrowseButton->SetUniqueId(nBtUniqueId);
        }
    }

    //------------------------------------------------------------------
    void OBrowserLine::setControl(IBrowserControl* pXControl)
    {
        m_pBrowserControl=pXControl;
        m_pBrowserControl->ShowCtr();
        Resize();
    }

    //------------------------------------------------------------------
    IBrowserControl* OBrowserLine::getControl()
    {
        return m_pBrowserControl;
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetSizePixel(Size aSize)
    {
        m_aOutputSize = aSize;
        Resize();
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetPosPixel(Point aPosPoint)
    {
        m_aLinePos=aPosPoint;
        Point aPos(m_aLinePos);
        aPos.Y()+=2;

        Point aTitlePos(m_aLinePos);
        aTitlePos.Y()+=8;
        Point aCtrPos(aPos);
        Point aBtnPos(aPos);

        if(m_pBrowserControl)
        {
            aCtrPos.X()=m_pBrowserControl->GetCtrPos().X();
        }
        if(m_pBrowseButton)
        {
            aBtnPos.X()=m_pBrowseButton->GetPosPixel().X();
        }
        m_aFtTitle.SetPosPixel(aTitlePos);

        if(m_pBrowserControl)
        {
            m_pBrowserControl->SetCtrPos(aCtrPos);
        }

        if(m_pBrowseButton)
        {
            m_pBrowseButton->SetPosPixel(aBtnPos);
        }
    }

    //------------------------------------------------------------------
    Window* OBrowserLine::GetRefWindow()
    {
        Window* pRefWindow=&m_aFtTitle;

        if(m_pBrowseButton)
        {
            pRefWindow=(Window*)m_pBrowseButton;
        }
        else if(m_pBrowserControl)
        {
            pRefWindow=m_pBrowserControl->GetMe();
        }
        return pRefWindow;
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetTabOrder(Window* pRefWindow, sal_uInt16 nFlags )
    {
        m_aFtTitle.SetZOrder(pRefWindow,nFlags);
        if(m_pBrowserControl)
        {
            m_pBrowserControl->
                SetTabOrder((Window*)&m_aFtTitle,WINDOW_ZORDER_BEHIND);
        }

        if(m_pBrowseButton)
        {
            m_pBrowseButton->SetZOrder(m_pBrowserControl->GetMe(),
                            WINDOW_ZORDER_BEHIND);
        }
    }

    //------------------------------------------------------------------
    sal_Bool OBrowserLine::GrabFocus()
    {
        sal_Bool bRes=sal_False;

        if(m_pBrowserControl &&
            m_pBrowserControl->GetMe()->IsEnabled())
        {
            m_pBrowserControl->GetMe()->GrabFocus();
            bRes=sal_True;
        }
        else if(m_pBrowseButton && m_pBrowseButton->IsEnabled())
        {
            m_pBrowseButton->GrabFocus();
            bRes=sal_True;
        }
        return bRes;
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetPosSizePixel(Point aPosPoint,Size aSize)
    {
        if(m_aLinePos!=aPosPoint && m_aOutputSize!=aSize)
        {
            SetControlPosSize(aPosPoint,aSize);
        }
        else
        {
            m_aOutputSize=aSize;
            Resize();
        }
    }

    //------------------------------------------------------------------
    Size OBrowserLine::GetSizePixel()
    {
        return m_aOutputSize;
    }

    //------------------------------------------------------------------
    void OBrowserLine::Enable( sal_Bool _bDoEnable )
    {
        m_aFtTitle.Enable( _bDoEnable );
        if ( m_pBrowserControl )
            m_pBrowserControl->EnableControl( _bDoEnable );
        if ( m_pBrowseButton )
            m_pBrowseButton->Enable( _bDoEnable );
    }

    //------------------------------------------------------------------
    void OBrowserLine::Show(sal_Bool bFlag)
    {
        m_aFtTitle.Show(bFlag);
        if(m_pBrowserControl)
        {
            m_pBrowserControl->ShowCtr(bFlag);
        }
        if(m_pBrowseButton)
        {
            m_pBrowseButton->Show(bFlag);
        }
    }

    //------------------------------------------------------------------
    void OBrowserLine::Hide()
    {
        Show(sal_False);
    }

    //------------------------------------------------------------------
    sal_Bool OBrowserLine::IsVisible()
    {
        return m_aFtTitle.IsVisible();
    }

    //------------------------------------------------------------------
    void OBrowserLine::Invalidate()
    {
        m_aFtTitle.Invalidate();
        if(m_pBrowserControl)
        {
            m_pBrowserControl->InvalidateCtr();
        }
        if(m_pBrowseButton)
        {
            m_pBrowseButton->Invalidate();
        }
    }

    //------------------------------------------------------------------
    void OBrowserLine::Update()
    {
        m_aFtTitle.Update();
        if(m_pBrowserControl)
        {
            m_pBrowserControl->UpdateCtr();
        }
        if(m_pBrowseButton)
        {
            m_pBrowseButton->Update();
        }
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetData(void *pPtr)
    {
        m_pData=pPtr;
    }

    //------------------------------------------------------------------
    void* OBrowserLine::GetData()
    {
        return m_pData;
    }
    //------------------------------------------------------------------
    void OBrowserLine::SetControlPosSize(Point aPos,Size aSize)
    {
        m_aLinePos=aPos;
        m_aOutputSize=aSize;
        Size a2Size(aSize);

        aSize.Width()=m_nNameWidth-3;
        a2Size.Width()-=m_nNameWidth;
        a2Size.Height() -=2;

        aPos.Y()+=2;
        Point aTitlePos(m_aLinePos);
        aTitlePos.Y()+=8;

        m_aFtTitle.SetPosSizePixel(aTitlePos,aSize);
        sal_uInt16 nBrowseButtonWidth=0;

        nBrowseButtonWidth=(sal_uInt16)aSize.Height()-4;

        a2Size.Width()=a2Size.Width()-nBrowseButtonWidth;

        aPos.X()+=aSize.Width();

        if(m_pBrowserControl)
        {
            a2Size.Height()=m_pBrowserControl->GetCtrSize().Height();
            m_pBrowserControl->SetCtrPos(aPos);
            m_pBrowserControl->SetCtrSize(a2Size);
        }

        if ( m_pBrowseButton )
        {
            long nx=m_aOutputSize.Width()-nBrowseButtonWidth;
            if(nx>aPos.X())
            {
                aPos.X()=nx;
            }
            aSize.Width()=nBrowseButtonWidth-3;
            aSize.Height()=nBrowseButtonWidth-2;
            m_pBrowseButton->SetPosSizePixel(aPos,aSize);
        }
    }

    //------------------------------------------------------------------
    void OBrowserLine::Resize()
    {
        Size aSize(m_aOutputSize);
        Size a2Size(aSize);

        aSize.Width()=m_nNameWidth-3;
        a2Size.Width()-=m_nNameWidth;
        a2Size.Height() -=2;

        Point aPos(m_aLinePos);
        aPos.Y()+=2;
        Point aTitlePos(m_aLinePos);
        aTitlePos.Y()+=8;

        sal_uInt16 nBrowseButtonWidth=0;

        nBrowseButtonWidth=(sal_uInt16)aSize.Height()-4;

        a2Size.Width()=a2Size.Width()-nBrowseButtonWidth;

        aPos.X()+=aSize.Width();

        if(m_pBrowserControl)
        {
            a2Size.Height() = m_pBrowserControl->GetCtrSize().Height();
            m_pBrowserControl->SetCtrSize(a2Size);
        }

        if ( m_pBrowseButton )
        {
            long nx=m_aOutputSize.Width()-nBrowseButtonWidth;
            if(nx>aPos.X())
            {
                aPos.X()=nx;
            }
            aSize.Width()=nBrowseButtonWidth-3;
            aSize.Height()=nBrowseButtonWidth-2;
            m_pBrowseButton->SetPosSizePixel(aPos,aSize);
        }
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetNeedsRepaint(sal_Bool bFlag)
    {
        m_bNeedsRepaint=bFlag;
    }

    //------------------------------------------------------------------
    sal_Bool OBrowserLine::NeedsRepaint()
    {
        return m_bNeedsRepaint;
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetTitle(const XubString& rString )
    {
        String aText(rString);
        // #99102# --------------
        m_aFtTitle.SetText(aText);
        if ( m_pBrowserControl )
            m_pBrowserControl->GetMe()->SetAccessibleName( rString );
        if ( m_pBrowseButton )
            m_pBrowseButton->SetAccessibleName( rString );
        FullFillTitleString();
    }

    // #99102# ---------------------------------------------------------
    void OBrowserLine::FullFillTitleString()
    {
        if( m_pTheParent )
        {
            String aText = m_aFtTitle.GetText();
            while( m_pTheParent->GetTextWidth( aText ) < m_nNameWidth )
                aText.AppendAscii("...........");
            m_aFtTitle.SetText(aText);
        }
    }

    //------------------------------------------------------------------
    XubString OBrowserLine::GetTitle() const
    {
        return m_aFtTitle.GetText();
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetKindOfControl(BrowserControlType eKOC)
    {
        m_eControlType=eKOC;
    }

    //------------------------------------------------------------------
    BrowserControlType OBrowserLine::GetKindOfControl()
    {
        return m_eControlType;
    }

    //------------------------------------------------------------------
    void OBrowserLine::EnableInputControls( bool _bEnableInput, bool _bEnableBrowseButton )
    {
        if ( m_pBrowseButton )
            m_pBrowseButton->Enable( _bEnableBrowseButton );
        if ( m_pBrowserControl )
            m_pBrowserControl->EnableControl( _bEnableInput );
        m_aFtTitle.Enable( _bEnableInput );
    }

    //------------------------------------------------------------------
    void OBrowserLine::ShowBrowseButton()
    {
        if ( !m_pBrowseButton )
        {
            delete m_pBrowseButton;
            m_pBrowseButton=new PushButton( m_pTheParent, WB_NOPOINTERFOCUS );
            m_pBrowseButton->SetGetFocusHdl( LINK( this, OBrowserLine, GetFocusHdl ) );
            m_pBrowseButton->SetData( (void*)this );
            m_pBrowseButton->SetText( String::CreateFromAscii( "..." ) );
        }
        m_pBrowseButton->Show();
        Resize();
    }

    //------------------------------------------------------------------
    void OBrowserLine::HideBrowseButton()
    {
        if ( m_pBrowseButton )
        {
            m_pBrowseButton->Hide();
            delete m_pBrowseButton;
            m_pBrowseButton=NULL;
        }
        Resize();
    }

    //------------------------------------------------------------------
    void OBrowserLine::Locked(sal_Bool nFlag)
    {
        m_bIsLocked=nFlag;
    }

    //------------------------------------------------------------------
    sal_Bool OBrowserLine::IsLocked()
    {
        return m_bIsLocked;
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetTitleWidth(sal_uInt16 nWidth)
    {
        if (m_nNameWidth != nWidth+10)
        {
            m_nNameWidth = nWidth+10;
            SetControlPosSize(m_aLinePos,m_aOutputSize);
        }
        // #99102# ---------
        FullFillTitleString();
    }

    //------------------------------------------------------------------
    void OBrowserLine::SetClickHdl(const Link& rLink)
    {
        if(m_pBrowseButton)
            m_pBrowseButton->SetClickHdl(rLink );
    }

    //------------------------------------------------------------------
    IMPL_LINK( OBrowserLine, GetFocusHdl, PushButton*, pPB )
    {
        if(m_pBrowserControl)
        {
            IBrowserControlListener* pListener = m_pBrowserControl->getListener();
            if (pListener)
                pListener->GetFocus(m_pBrowserControl);
        }
        return 0;
    }
//............................................................................
} // namespace pcr
//............................................................................

