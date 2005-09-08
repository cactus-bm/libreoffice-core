/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: commoncontrol.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 20:06:56 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _EXTENSIONS_PROPCTRLR_COMMONCONTROL_HXX_
#include "commoncontrol.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _EXTENSIONS_PROPCTRLR_BRWCONTROLLISTENER_HXX_
#include "brwcontrollistener.hxx"
#endif
#ifndef _SV_COMBOBOX_HXX
#include <vcl/combobox.hxx>
#endif

//............................................................................
namespace pcr
{
//............................................................................
    //==================================================================
    //= OCommonBehaviourControl
    //==================================================================
    //------------------------------------------------------------------
    OCommonBehaviourControl::OCommonBehaviourControl(Window* _pMeAsWin)
        :m_pListener(NULL)
        ,m_bModified(sal_False)
        ,m_nLine(0)
        ,m_sStandardString(getStandardString())
        ,m_pMeAsWindow(_pMeAsWin)
    {
        DBG_ASSERT(m_pMeAsWindow != NULL, "OCommonBehaviourControl::OCommonBehaviourControl: invalid window!");
    }

    //------------------------------------------------------------------
    void OCommonBehaviourControl::autoSizeWindow()
    {
        ComboBox aComboBox(m_pMeAsWindow, WB_DROPDOWN);
        aComboBox.SetPosSizePixel(Point(0,0), Size(100,100));
        m_pMeAsWindow->SetSizePixel(aComboBox.GetSizePixel());
    }

    //------------------------------------------------------------------
    sal_Bool OCommonBehaviourControl::handlePreNotify(NotifyEvent& rNEvt)
    {
        if (EVENT_KEYINPUT == rNEvt.GetType())
        {
            const KeyCode& aKeyCode = rNEvt.GetKeyEvent()->GetKeyCode();
            sal_uInt16 nKey = aKeyCode.GetCode();

            if (nKey == KEY_RETURN && !aKeyCode.IsShift())
            {
                LoseFocusHdl(m_pMeAsWindow);
                if (m_pListener != NULL)
                    m_pListener->TravelLine(this);
                return sal_True;
            }
        }
        return sal_False;
    }

    //------------------------------------------------------------------
    void OCommonBehaviourControl::CommitModified()
    {
        if (IsModified() && getListener())
            getListener()->Commit(this);
        m_bModified = sal_False;
    }

    //------------------------------------------------------------------
    void OCommonBehaviourControl::modified(Window* _pSource)
    {
        m_bModified = sal_True;
        if (m_pListener != NULL)
            m_pListener->Modified(this);
    }

    //------------------------------------------------------------------
    void OCommonBehaviourControl::getFocus(Window* _pSource)
    {
        if (m_pListener != NULL)
            m_pListener->GetFocus(this);
    }

    //------------------------------------------------------------------
    void OCommonBehaviourControl::commitModified(Window* _pSource)
    {
        CommitModified();
    }

    //------------------------------------------------------------------
    IMPL_LINK( OCommonBehaviourControl, ModifiedHdl, Window*, _pWin )
    {
        modified(_pWin);
        return 0;
    }

    //------------------------------------------------------------------
    IMPL_LINK( OCommonBehaviourControl, GetFocusHdl, Window*, _pWin )
    {
        getFocus(_pWin);
        return 0;
    }

    //------------------------------------------------------------------
    IMPL_LINK( OCommonBehaviourControl, LoseFocusHdl, Window*, _pWin )
    {
        commitModified(_pWin);
        return 0;
    }

//............................................................................
} // namespace pcr
//............................................................................

