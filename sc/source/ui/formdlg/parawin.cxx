/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: parawin.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-07 11:20:29 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"



//----------------------------------------------------------------------------

#include "scitems.hxx"
#include <sfx2/dispatch.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/objsh.hxx>
#include <svtools/zforlist.hxx>

#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif

#include "parawin.hxx"
#include "parawin.hrc"
#include "formdata.hxx"
#include "globstr.hrc"
#include "scresid.hxx"
#include "reffact.hxx"
#include "document.hxx"
#include "cell.hxx"
#include "scmod.hxx"
#include "inputhdl.hxx"
#include "tabvwsh.hxx"
#include "appoptio.hxx"
#include "compiler.hxx"
#include "funcdesc.hxx"

//============================================================================

ScParaWin::ScParaWin(ScAnyRefDlg* pParent,Point aPos):
    TabPage         (pParent,ScResId(RID_SCTAB_PARAMETER)),
    pFuncDesc       ( NULL ),
    aFtEditDesc     ( this, ScResId( FT_EDITDESC ) ),
    aFtArgName      ( this, ScResId( FT_PARNAME ) ),
    aFtArgDesc      ( this, ScResId( FT_PARDESC ) ),

    aBtnFx1         ( this, ScResId( BTN_FX1 ) ),
    aFtArg1         ( this, ScResId( FT_ARG1 ) ),
    aEdArg1         ( this, ScResId( ED_ARG1 ) ),
    aRefBtn1        ( this, ScResId( RB_ARG1 ) ),
    aBtnFx2         ( this, ScResId( BTN_FX2 ) ),
    aFtArg2         ( this, ScResId( FT_ARG2 ) ),
    aEdArg2         ( this, ScResId( ED_ARG2 ) ),
    aRefBtn2        ( this, ScResId( RB_ARG2 ) ),
    aBtnFx3         ( this, ScResId( BTN_FX3 ) ),
    aFtArg3         ( this, ScResId( FT_ARG3 ) ),
    aEdArg3         ( this, ScResId( ED_ARG3 ) ),
    aRefBtn3        ( this, ScResId( RB_ARG3 ) ),
    aBtnFx4         ( this, ScResId( BTN_FX4 ) ),
    aFtArg4         ( this, ScResId( FT_ARG4 ) ),
    aEdArg4         ( this, ScResId( ED_ARG4 ) ),
    aRefBtn4        ( this, ScResId( RB_ARG4 ) ),
    aSlider         ( this, ScResId( WND_SLIDER ) ),
    bRefMode        (FALSE)
{
    Image aFxHC( ScResId( IMG_FX_H ) );
    FreeResource();
    aDefaultString=aFtEditDesc.GetText();
    pMyParent=pParent;
    SetPosPixel(aPos);
    nEdFocus=NOT_FOUND;
    nActiveLine=0;
    Size aSize = aSlider.GetSizePixel();
    aSize.Width() = GetSettings().GetStyleSettings().GetScrollBarSize();
    aSlider.SetSizePixel( aSize );
    aSlider.SetEndScrollHdl( LINK( this, ScParaWin, ScrollHdl ) );
    aSlider.SetScrollHdl( LINK( this, ScParaWin, ScrollHdl ) );

    aBtnFx1.SetModeImage( aFxHC, BMP_COLOR_HIGHCONTRAST );
    aBtnFx2.SetModeImage( aFxHC, BMP_COLOR_HIGHCONTRAST );
    aBtnFx3.SetModeImage( aFxHC, BMP_COLOR_HIGHCONTRAST );
    aBtnFx4.SetModeImage( aFxHC, BMP_COLOR_HIGHCONTRAST );

    InitArgInput( 0, aFtArg1, aBtnFx1, aEdArg1, aRefBtn1);
    InitArgInput( 1, aFtArg2, aBtnFx2, aEdArg2, aRefBtn2);
    InitArgInput( 2, aFtArg3, aBtnFx3, aEdArg3, aRefBtn3);
    InitArgInput( 3, aFtArg4, aBtnFx4, aEdArg4, aRefBtn4);
    ClearAll();
}

void ScParaWin::UpdateArgDesc( USHORT nArg )
{
    if (nArg==NOT_FOUND) return;

    if ( nArgs > 4 )
        nArg = sal::static_int_cast<USHORT>( nArg + GetSliderPos() );
        //@ nArg += (USHORT)aSlider.GetThumbPos();

    if ( (nArgs > 0) && (nArg<nArgs) )
    {
        String  aArgDesc;
        String  aArgName;

        SetArgumentDesc( EMPTY_STRING );
        SetArgumentText( EMPTY_STRING );

        if ( nArgs < VAR_ARGS )
        {
            USHORT nRealArg = aVisibleArgMapping[nArg];
            aArgDesc  = *(pFuncDesc->ppDefArgDescs[nRealArg]);
            aArgName  = *(pFuncDesc->ppDefArgNames[nRealArg]);
            aArgName += ' ';
            aArgName += ScGlobal::GetRscString(
                    (pFuncDesc->pDefArgFlags[nRealArg].bOptional) ?
                    STR_OPTIONAL : STR_REQUIRED );
        }
        else
        {
            USHORT nFix = nArgs - VAR_ARGS;
            USHORT nPos = ( nArg < nFix ? nArg : nFix );
            USHORT nRealArg = (nPos < aVisibleArgMapping.size() ?
                    aVisibleArgMapping[nPos] : aVisibleArgMapping.back());
            aArgDesc  = *(pFuncDesc->ppDefArgDescs[nRealArg]);
            aArgName  = *(pFuncDesc->ppDefArgNames[nRealArg]);
            if ( nArg >= nFix )
                aArgName += String::CreateFromInt32(nArg-nFix+1);
            aArgName += ' ';
            aArgName += ScGlobal::GetRscString(
                (nArg > nFix || pFuncDesc->pDefArgFlags[nRealArg].bOptional) ?
                STR_OPTIONAL : STR_REQUIRED );
        }

        SetArgumentDesc(aArgDesc);
        SetArgumentText(aArgName);
    }
}

void ScParaWin::UpdateArgInput( USHORT nOffset, USHORT i )
{
    USHORT nArg = nOffset + i;
    if ( nArgs < VAR_ARGS)
    {
        if(nArg<nArgs)
        {
            USHORT nRealArg = aVisibleArgMapping[nArg];
            SetArgNameFont  (i,(pFuncDesc->pDefArgFlags[nRealArg].bOptional)
                                            ? aFntLight : aFntBold );
            SetArgName      (i,*(pFuncDesc->ppDefArgNames[nRealArg]));
        }
    }
    else
    {
        USHORT nFix = nArgs - VAR_ARGS;
        USHORT nPos = ( nArg < nFix ? nArg : nFix );
        USHORT nRealArg = (nPos < aVisibleArgMapping.size() ?
                aVisibleArgMapping[nPos] : aVisibleArgMapping.back());
        SetArgNameFont( i,
                (nArg > nFix || pFuncDesc->pDefArgFlags[nRealArg].bOptional) ?
                aFntLight : aFntBold );
        if ( nArg >= nFix )
        {
            String aArgName( *(pFuncDesc->ppDefArgNames[nRealArg]) );
            aArgName += String::CreateFromInt32(nArg-nFix+1);
            SetArgName( i, aArgName );
        }
        else
            SetArgName( i, *(pFuncDesc->ppDefArgNames[nRealArg]) );
    }
    if(nArg<nArgs) SetArgVal(i,*(aParaArray[nArg]));
    //@ aArgInput[i].SetArgVal( *(pArgArr[nOffset+i]) );

}

ScParaWin::~ScParaWin()
{
    DelParaArray();

    // #i66422# if the focus changes during destruction of the controls,
    // don't call the focus handlers
    Link aEmptyLink;
    aBtnFx1.SetGetFocusHdl( aEmptyLink );
    aBtnFx2.SetGetFocusHdl( aEmptyLink );
    aBtnFx3.SetGetFocusHdl( aEmptyLink );
    aBtnFx4.SetGetFocusHdl( aEmptyLink );
}

USHORT ScParaWin::GetActiveLine()
{
    return nActiveLine;
}

void ScParaWin::SetActiveLine(USHORT no)
{
    if(no<nArgs)
    {
        long nOffset = GetSliderPos();
        nActiveLine=no;
        long nNewEdPos=(long)nActiveLine-nOffset;
        if(nNewEdPos<0 || nNewEdPos>3)
        {
            nOffset+=nNewEdPos;
            SetSliderPos((USHORT) nOffset);
            nOffset=GetSliderPos();
        }
        nEdFocus=no-(USHORT)nOffset;
        UpdateArgDesc( nEdFocus );
    }
}

ScRefEdit* ScParaWin::GetActiveEdit()
{
    if(nArgs>0 && nEdFocus!=NOT_FOUND)
    {
        return aArgInput[nEdFocus].GetArgEdPtr();
    }
    else
    {
        return NULL;
    }
}


String ScParaWin::GetArgument(USHORT no)
{
    String aStr;
    if(no<aParaArray.Count())
    {
        aStr=*aParaArray[no];
        if(no==nActiveLine && aStr.Len()==0)
            aStr+=' ';
    }
    return aStr;
}

String  ScParaWin::GetActiveArgName()
{
    String aStr;
    if(nArgs>0 && nEdFocus!=NOT_FOUND)
    {
        aStr=aArgInput[nEdFocus].GetArgName();
    }
    return aStr;
}


void ScParaWin::SetArgument(USHORT no, const String& aString)
{
    if(no<aParaArray.Count())
    {
            *aParaArray[no]=aString;
            aParaArray[no]->EraseLeadingChars();
    }
}

void ScParaWin::DelParaArray()
{
    for(int i=0;i<aParaArray.Count();i++)
    {
        String* pStr=aParaArray[sal::static_int_cast<USHORT>(i)];
        delete pStr;
    }
    aParaArray.Remove(0,aParaArray.Count());

}

void ScParaWin::SetArgumentFonts(const Font&aBoldFont,const Font&aLightFont)
{
    aFntBold=aBoldFont;
    aFntLight=aLightFont;
}

void ScParaWin::SetFunctionDesc(const ScFuncDesc* pFDesc)
{
    pFuncDesc=pFDesc;

    SetArgumentDesc( EMPTY_STRING );
    SetArgumentText( EMPTY_STRING );
    SetEditDesc( EMPTY_STRING );
    nArgs = 0;
    if ( pFuncDesc!=NULL)
    {
        if(pFuncDesc->pFuncDesc != NULL)
        {
            SetEditDesc(*(pFuncDesc->pFuncDesc));
        }
        else
        {
            SetEditDesc(aDefaultString);
        }
        long nHelpId = pFuncDesc->nHelpId;
        nArgs = pFuncDesc->GetSuppressedArgCount();
        aVisibleArgMapping = pFuncDesc->GetVisibleArgMapping();
        aSlider.Hide();
        SetHelpId( nHelpId );
        aEdArg1.SetHelpId( nHelpId );
        aEdArg2.SetHelpId( nHelpId );
        aEdArg3.SetHelpId( nHelpId );
        aEdArg4.SetHelpId( nHelpId );

        //  Unique-IDs muessen gleich bleiben fuer Automatisierung
        SetUniqueId( HID_SC_FAP_PAGE );
        aEdArg1.SetUniqueId( HID_SC_FAP_EDIT1 );
        aEdArg2.SetUniqueId( HID_SC_FAP_EDIT2 );
        aEdArg3.SetUniqueId( HID_SC_FAP_EDIT3 );
        aEdArg4.SetUniqueId( HID_SC_FAP_EDIT4 );
        SetActiveLine(0);
    }
    else
    {
        nActiveLine=0;
    }

}

void ScParaWin::SetArgumentText(const String& aText)
{
    aFtArgName.SetText(aText);
}

void ScParaWin::SetArgumentDesc(const String& aText)
{
    aFtArgDesc.SetText(aText);
}

void ScParaWin::SetEditDesc(const String& aText)
{
    aFtEditDesc.SetText(aText);
}

void ScParaWin::SetArgName(USHORT no,const String& aText)
{
    aArgInput[no].SetArgName(aText);
}

void ScParaWin::SetArgNameFont(USHORT no,const Font& aFont)
{
    aArgInput[no].SetArgNameFont(aFont);
}

void ScParaWin::SetArgVal(USHORT no,const String& aText)
{
    aArgInput[no].SetArgVal(aText);
}

void ScParaWin::HideParaLine(USHORT no)
{
    aArgInput[no].Hide();
}

void ScParaWin::ShowParaLine(USHORT no)
{
    aArgInput[no].Show();
}

void ScParaWin::SetEdFocus(USHORT no)
{
    UpdateArgDesc(no);
    if(no<4 && no<aParaArray.Count())
        aArgInput[no].GetArgEdPtr()->GrabFocus();
}


void ScParaWin::InitArgInput( USHORT nPos, FixedText& rFtArg, ImageButton& rBtnFx,
                        ArgEdit& rEdArg, ScRefButton& rRefBtn)
{

    rRefBtn.SetReferences(pMyParent,&rEdArg);
    rEdArg.SetRefDialog(pMyParent);

    aArgInput[nPos].InitArgInput (&rFtArg,&rBtnFx,&rEdArg,&rRefBtn);

    aArgInput[nPos].Hide();

    aArgInput[nPos].SetFxClickHdl   ( LINK( this, ScParaWin, GetFxHdl ) );
    aArgInput[nPos].SetFxFocusHdl   ( LINK( this, ScParaWin, GetFxFocusHdl ) );
    aArgInput[nPos].SetEdFocusHdl   ( LINK( this, ScParaWin, GetEdFocusHdl ) );
    aArgInput[nPos].SetEdModifyHdl  ( LINK( this, ScParaWin, ModifyHdl ) );
}

void ScParaWin::ClearAll()
{
    SetFunctionDesc(NULL);
    SetArgumentOffset(0);
}

void ScParaWin::SetArgumentOffset(USHORT nOffset)
{
    DelParaArray();
    aSlider.SetThumbPos(0);
    int i;
    for(i=0;i<nArgs;i++)
        aParaArray.Insert( new String, aParaArray.Count() );

    if ( nArgs > 0 )
    {
        for ( i=0; i<4 && i<nArgs; i++ )
        {
            String aString;
            aArgInput[i].SetArgVal(aString);
            aArgInput[i].GetArgEdPtr()->Init(
                (i==0)               ? (ArgEdit *)NULL : aArgInput[i-1].GetArgEdPtr(),
                (i==3 || i==nArgs-1) ? (ArgEdit *)NULL : aArgInput[i+1].GetArgEdPtr(),
                                       aSlider, nArgs );
        }
    }

    if ( nArgs < 5 )
    {
        aSlider.Hide();
    }
    else
    {
        //aSlider.SetEndScrollHdl( LINK( this, ScFormulaDlg, ScrollHdl ) );
        aSlider.SetPageSize( 4 );
        aSlider.SetVisibleSize( 4 );
        aSlider.SetLineSize( 1 );
        aSlider.SetRange( Range( 0, nArgs ) );
        aSlider.SetThumbPos( nOffset );
        aSlider.Show();
    }

    UpdateParas();
}

void ScParaWin::UpdateParas()
{
    USHORT i;
    USHORT nOffset = GetSliderPos();

    if ( nArgs > 0 )
    {
        for ( i=0; (i<nArgs) && (i<4); i++ )
        {
            UpdateArgInput( nOffset, i );
            ShowParaLine(i);
        }
    }

    for ( i=nArgs; i<4; i++ ) HideParaLine(i);
}


USHORT ScParaWin::GetSliderPos()
{
    return (USHORT) aSlider.GetThumbPos();
}

void ScParaWin::SetSliderPos(USHORT nSliderPos)
{
    USHORT nOffset = GetSliderPos();

    if(aSlider.IsVisible() && nOffset!=nSliderPos)
    {
        aSlider.SetThumbPos(nSliderPos);
        for ( USHORT i=0; i<4; i++ )
        {
            UpdateArgInput( nSliderPos, i );
        }
    }
}

void ScParaWin::SliderMoved()
{
    USHORT nOffset = GetSliderPos();

    for ( USHORT i=0; i<4; i++ )
    {
        UpdateArgInput( nOffset, i );
    }
    if(nEdFocus!=NOT_FOUND)
    {
        UpdateArgDesc( nEdFocus );
        aArgInput[nEdFocus].SetArgSelection(Selection(0,SELECTION_MAX ));
        nActiveLine=nEdFocus+nOffset;
        ArgumentModified();
    }
    aScrollLink.Call(this);
}

void ScParaWin::ArgumentModified()
{
    aArgModifiedLink.Call(this);
}

void ScParaWin::FxClick()
{
    aFxLink.Call(this);
}


IMPL_LINK( ScParaWin, GetFxHdl, ArgInput*, pPtr )
{
    USHORT nOffset = GetSliderPos();
    nEdFocus=NOT_FOUND;
    for ( USHORT nPos=0; nPos<5;nPos++)
    {
        if(pPtr == &aArgInput[nPos])
        {
            nEdFocus=nPos;
            break;
        }
    }

    if(nEdFocus!=NOT_FOUND)
    {
        aArgInput[nEdFocus].SetArgSelection(Selection(0,SELECTION_MAX ));
        nActiveLine=nEdFocus+nOffset;
        FxClick();
    }
    return 0;
}

IMPL_LINK( ScParaWin, GetFxFocusHdl, ArgInput*, pPtr )
{
    USHORT nOffset = GetSliderPos();
    nEdFocus=NOT_FOUND;
    for ( USHORT nPos=0; nPos<5;nPos++)
    {
        if(pPtr == &aArgInput[nPos])
        {
            nEdFocus=nPos;
            break;
        }
    }

    if(nEdFocus!=NOT_FOUND)
    {
        aArgInput[nEdFocus].SetArgSelection(Selection(0,SELECTION_MAX ));
        UpdateArgDesc( nEdFocus );
        nActiveLine=nEdFocus+nOffset;
    }
    return 0;
}



IMPL_LINK( ScParaWin, GetEdFocusHdl, ArgInput*, pPtr )
{
    USHORT nOffset = GetSliderPos();
    nEdFocus=NOT_FOUND;
    for ( USHORT nPos=0; nPos<5;nPos++)
    {
        if(pPtr == &aArgInput[nPos])
        {
            nEdFocus=nPos;
            break;
        }
    }

    if(nEdFocus!=NOT_FOUND)
    {
        aArgInput[nEdFocus].SetArgSelection(Selection(0,SELECTION_MAX ));
        UpdateArgDesc( nEdFocus );
        nActiveLine=nEdFocus+nOffset;
        ArgumentModified();
    }

    return 0;
}


IMPL_LINK( ScParaWin, ScrollHdl, ScrollBar*, EMPTYARG )
{
    SliderMoved();

    return 0;
}

IMPL_LINK( ScParaWin, ModifyHdl, ArgInput*, pPtr )
{
    USHORT nOffset = GetSliderPos();
    nEdFocus=NOT_FOUND;
    for ( USHORT nPos=0; nPos<5;nPos++)
    {
        if(pPtr == &aArgInput[nPos])
        {
            nEdFocus=nPos;
            break;
        }
    }
    if(nEdFocus!=NOT_FOUND)
    {
        *(aParaArray[nEdFocus+nOffset])=aArgInput[nEdFocus].GetArgVal();
        UpdateArgDesc( nEdFocus);
        nActiveLine=nEdFocus+nOffset;
    }

    ArgumentModified();
    return 0;
}



