/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: parawin.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:43:02 $
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

#ifndef SC_PARAWIN_HXX
#define SC_PARAWIN_HXX

#ifndef SC_FUNCUTL_HXX
#include "funcutl.hxx"
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"       // ScAddress
#endif

#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef _SVEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif

#ifndef _SV_TABPAGE_HXX //autogen
#include <vcl/tabpage.hxx>
#endif

#ifndef _SVSTDARR_STRINGS

#define _SVSTDARR_STRINGS
#include <svtools/svstdarr.hxx>

#endif

#ifndef _SV_TABCTRL_HXX //autogen
#include <vcl/tabctrl.hxx>
#endif

class ScFuncDesc;

//============================================================================
#define NOT_FOUND 0xffff
//============================================================================

class ScParaWin : public TabPage
{
private:

        Link            aScrollLink;
        Link            aFxLink;
        Link            aArgModifiedLink;

        const ScFuncDesc*   pFuncDesc;
        ScAnyRefDlg*    pMyParent;
        USHORT          nArgs;
        Font            aFntBold;
        Font            aFntLight;

        FixedInfo       aFtEditDesc;
        FixedText       aFtArgName;
        FixedInfo       aFtArgDesc;

        ImageButton     aBtnFx1;
        FixedText       aFtArg1;
        ArgEdit         aEdArg1;
        ScRefButton     aRefBtn1;
        ImageButton     aBtnFx2;
        FixedText       aFtArg2;
        ArgEdit         aEdArg2;
        ScRefButton     aRefBtn2;
        ImageButton     aBtnFx3;
        FixedText       aFtArg3;
        ArgEdit         aEdArg3;
        ScRefButton     aRefBtn3;
        ImageButton     aBtnFx4;
        FixedText       aFtArg4;
        ArgEdit         aEdArg4;
        ScRefButton     aRefBtn4;
        ScrollBar       aSlider;
        BOOL            bRefMode;

        USHORT          nEdFocus;
        USHORT          nActiveLine;

        ArgInput        aArgInput[4];
        String          aDefaultString;
        SvStrings       aParaArray;
        DECL_LINK( ScrollHdl, ScrollBar* );
        DECL_LINK( ModifyHdl, ArgInput* );
        DECL_LINK( GetEdFocusHdl, ArgInput* );
        DECL_LINK( GetFxFocusHdl, ArgInput* );
        DECL_LINK( GetFxHdl, ArgInput* );

protected:

        virtual void    SliderMoved();
        virtual void    ArgumentModified();
        virtual void    FxClick();

        void            InitArgInput( USHORT nPos, FixedText& rFtArg, ImageButton& rBtnFx,
                                        ArgEdit& rEdArg, ScRefButton& rRefBtn);

        void            DelParaArray();
        void            SetArgumentDesc(const String& aText);
        void            SetArgumentText(const String& aText);


        void            SetArgName      (USHORT no,const String &aArg);
        void            SetArgNameFont  (USHORT no,const Font&);
        void            SetArgVal       (USHORT no,const String &aArg);

        void            HideParaLine(USHORT no);
        void            ShowParaLine(USHORT no);
        void            UpdateArgDesc( USHORT nArg );
        void            UpdateArgInput( USHORT nOffset, USHORT i );

public:
                        ScParaWin(ScAnyRefDlg* pParent,Point aPos);
                        ~ScParaWin();

        void            SetFunctionDesc(const ScFuncDesc* pFDesc);
        void            SetArgCount(USHORT nArgs, USHORT nOffset);
        void            SetEditDesc(const String& aText);
        void            UpdateParas();
        void            ClearAll();

        BOOL            IsRefMode() {return bRefMode;}
        void            SetRefMode(BOOL bFlag) {bRefMode=bFlag;}

        USHORT          GetActiveLine();
        void            SetActiveLine(USHORT no);
        ScRefEdit*      GetActiveEdit();
        String          GetActiveArgName();

        String          GetArgument(USHORT no);
        void            SetArgument(USHORT no, const String& aString);
        void            SetArgumentFonts(const Font&aBoldFont,const Font&aLightFont);

        void            SetEdFocus(USHORT nEditLine); //Sichtbare Editzeilen
        USHORT          GetSliderPos();
        void            SetSliderPos(USHORT nSliderPos);

        void            SetScrollHdl( const Link& rLink ) { aScrollLink = rLink; }
        const Link&     GetScrollHdl() const { return aScrollLink; }

        void            SetArgModifiedHdl( const Link& rLink ) { aArgModifiedLink = rLink; }
        const Link&     GetArgModifiedHdl() const { return aArgModifiedLink; }

        void            SetFxHdl( const Link& rLink ) { aFxLink = rLink; }
        const Link&     GetFxHdl() const { return aFxLink; }
};





#endif // SC_PARAWIN_HXX

