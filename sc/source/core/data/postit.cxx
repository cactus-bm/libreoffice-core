/*************************************************************************
 *
 *  $RCSfile: postit.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-11-09 17:56:14 $
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


#ifdef PCH
#include "core_pch.hxx"
#endif

#pragma hdrstop

// INCLUDE ---------------------------------------------------------------

#ifndef SC_POSTIT_HXX
#include "postit.hxx"
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif
#ifndef _SXCECITM_HXX
#include <svx/sxcecitm.hxx>
#endif
#ifndef _EDITOBJ_HXX
#include <svx/editobj.hxx>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_EDITUTIL_HXX
#include "editutil.hxx"
#endif
#ifndef SC_SCATTR_HXX
#include "attrib.hxx"
#endif
#ifndef SC_DETFUNC_HXX
#include "detfunc.hxx"
#endif
#ifndef SC_SCDOCPOL_HXX
#include "docpool.hxx"
#endif
#ifndef SC_SCPATATR_HXX
#include "patattr.hxx"
#endif
#ifndef SC_DRWLAYER_HXX
#include "drwlayer.hxx"
#endif
#ifndef SC_DOCUNO_HXX
#include "docuno.hxx"
#endif
#ifndef _SVDPAGE_HXX
#include <svx/svdpage.hxx>
#endif
#ifndef _SVX_XFLCLIT_HXX
#include <svx/xflclit.hxx>
#endif
#ifndef _SVX_XLNSTCIT_HXX
#include <svx/xlnstcit.hxx>
#endif
#ifndef _SVX_XLNSTIT_HXX
#include <svx/xlnstit.hxx>
#endif
#ifndef _SVX_XLNSWIT_HXX
#include <svx/xlnstwit.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif

namespace com { namespace sun { namespace star { namespace frame { class XModel; } } } }

using ::com::sun::star::uno::Reference;
using ::com::sun::star::frame::XModel;

//========================================================================
// class ScPostIt
//========================================================================

ScPostIt::ScPostIt(ScDocument* pDoc):
    mpDoc(pDoc),
    mbShown(FALSE),
    maItemSet(pDoc->GetNoteItemPool(), SDRATTR_START,  SDRATTR_END, EE_ITEMS_START, EE_ITEMS_END, 0,0)
{
    // this 'default' ctor  does not set the Author & Date stamp.
    // maRectangle is not initialised as it can be tested using IsEmpty().
}

ScPostIt::ScPostIt( const String& rText, ScDocument* pDoc ):
    mpDoc(pDoc),
    mbShown(FALSE),
    maItemSet(pDoc->GetNoteItemPool(), SDRATTR_START,  SDRATTR_END, EE_ITEMS_START, EE_ITEMS_END, 0,0)
{
    // maRectangle is not initialised as it can be tested using IsEmpty().
    SetText( rText);
    AutoStamp( );
}

ScPostIt::ScPostIt( const EditTextObject* pTextObj, ScDocument* pDoc ):
    mpDoc ( pDoc ),
    mbShown(FALSE),
    maItemSet(pDoc->GetNoteItemPool(), SDRATTR_START,  SDRATTR_END, EE_ITEMS_START, EE_ITEMS_END, 0,0)
{
    // maRectangle is not initialised as it can be tested using IsEmpty().
    SetEditTextObject( pTextObj);
    AutoStamp( );
}

ScPostIt::ScPostIt( const ScPostIt& rNote, ScDocument* pDoc ):
    mpDoc ( pDoc ),
    maItemSet(pDoc->GetNoteItemPool(), SDRATTR_START,  SDRATTR_END, EE_ITEMS_START, EE_ITEMS_END, 0,0)
{
    SetEditTextObject( rNote.mpEditObj.get());
    maStrDate   = rNote.maStrDate;
    maStrAuthor = rNote.maStrAuthor;
    mbShown = rNote.mbShown;
    maRectangle = rNote.maRectangle;
    maItemSet.PutExtended(rNote.maItemSet,SFX_ITEM_DONTCARE, SFX_ITEM_DEFAULT);
}

ScPostIt::~ScPostIt()
{
}

const ScPostIt& ScPostIt::operator=( const ScPostIt& rCpy )
{
    mpDoc       = rCpy.mpDoc;
    SetEditTextObject( rCpy.mpEditObj.get());
    maStrDate   = rCpy.maStrDate;
    maStrAuthor = rCpy.maStrAuthor;
    mbShown = rCpy.mbShown;
    maRectangle = rCpy.maRectangle;
    maItemSet.PutExtended(rCpy.maItemSet,SFX_ITEM_DONTCARE, SFX_ITEM_DEFAULT);

    return *this;
}

// Support existing functionality - create simple text string from the
// EditTextObject.
String ScPostIt::GetText() const
{
    String aText;
    const EditTextObject* pEditText;
    if(mpDoc && ((pEditText = GetEditTextObject()) != 0))
    {
        ScNoteEditEngine& rEE = mpDoc->GetNoteEngine();
        rEE.SetText( *pEditText );
        aText = rEE.GetText( LINEEND_LF );
        aText.SearchAndReplaceAll(CHAR_CR, ' ');
    }
    return aText;
}

void ScPostIt::SetEditTextObject( const EditTextObject* pTextObj)
{
    if(pTextObj && mpDoc)
    {
        ScNoteEditEngine& rEE = mpDoc->GetNoteEngine();
        rEE.SetText( *pTextObj );
        mpEditObj.reset(rEE.CreateTextObject());
    }
    else
        mpEditObj.reset();
}

// Support existing functionality - create EditTextObject from a simple
// text string
void ScPostIt::SetText( const String& rText)
{
    if(mpDoc && rText.Len())
    {
        ScNoteEditEngine& rEE = mpDoc->GetNoteEngine();
        rEE.SetText( rText );
        mpEditObj.reset(rEE.CreateTextObject());
    }
    else
        mpEditObj.reset();

}

void ScPostIt::AutoStamp( )
{
    maStrDate   = ScGlobal::pLocaleData->getDate( Date() );

    SvtUserOptions aUserOpt;
    maStrAuthor = aUserOpt.GetID();
}

BOOL ScPostIt::IsEmpty() const
{
    return (!mpEditObj.get());
}

Rectangle ScPostIt::DefaultRectangle(const ScAddress& rPos) const
{
    BOOL bNegativePage = mpDoc->IsNegativePage( rPos.Tab() );
    long nPageSign = bNegativePage ? -1 : 1;

    SCCOL nNextCol = rPos.Col()+1;
    const ScMergeAttr* pMerge = (const ScMergeAttr*) mpDoc->GetAttr( rPos.Col(), rPos.Row(), rPos.Tab(), ATTR_MERGE );
    if ( pMerge->GetColMerge() > 1 )
        nNextCol = rPos.Col() + pMerge->GetColMerge();

    Point aRectPos = ScDetectiveFunc(mpDoc, rPos.Tab()).GetDrawPos( nNextCol, rPos.Row(), FALSE );

    aRectPos.X() += 600 * nPageSign;
    aRectPos.Y() -= 1500;
    if ( aRectPos.Y() < 0 ) aRectPos.Y() = 0;

    String aText = GetText();
    long nDefWidth = ( aText.Len() > SC_NOTE_SMALLTEXT ) ? 5800 : 2900;
    Size aRectSize( nDefWidth, 1800 );

    long nMaxWidth = aRectSize.Width();

    if ( bNegativePage )
        aRectPos.X() -= aRectSize.Width();
    else
        if ( aRectPos.X() < 0 ) aRectPos.X() = 0;

    return Rectangle(aRectPos, aRectSize);
}

SfxItemSet ScPostIt::DefaultItemSet() const
{
    SfxItemSet  aCaptionSet( mpDoc->GetNoteItemPool(), SDRATTR_START,  SDRATTR_END, EE_ITEMS_START, EE_ITEMS_END, 0,0);

    XPolygon aTriangle(4);
    aTriangle[0].X()=10; aTriangle[0].Y()= 0;
    aTriangle[1].X()= 0; aTriangle[1].Y()=30;
    aTriangle[2].X()=20; aTriangle[2].Y()=30;
    aTriangle[3].X()=10; aTriangle[3].Y()= 0;   // #99319# line end polygon must be closed

    aCaptionSet.Put( XLineStartItem( EMPTY_STRING, aTriangle ) );
    aCaptionSet.Put( XLineStartWidthItem( 200 ) );
    aCaptionSet.Put( XLineStartCenterItem( FALSE ) );
    aCaptionSet.Put( XFillStyleItem( XFILL_SOLID ) );
    Color aYellow( ScDetectiveFunc::GetCommentColor() );
    aCaptionSet.Put( XFillColorItem( String(), aYellow ) );

    //  shadow
    //  SdrShadowItem has FALSE, instead the shadow is set for the rectangle
    //  only with SetSpecialTextBoxShadow when the object is created
    //  (item must be set to adjust objects from older files)
    aCaptionSet.Put( SdrShadowItem( FALSE ) );
    aCaptionSet.Put( SdrShadowXDistItem( 100 ) );
    aCaptionSet.Put( SdrShadowYDistItem( 100 ) );

    //  text attributes
    aCaptionSet.Put( SdrTextLeftDistItem( 100 ) );
    aCaptionSet.Put( SdrTextRightDistItem( 100 ) );
    aCaptionSet.Put( SdrTextUpperDistItem( 100 ) );
    aCaptionSet.Put( SdrTextLowerDistItem( 100 ) );

    //  #78943# do use the default cell style, so the user has a chance to
    //  modify the font for the annotations
    ((const ScPatternAttr&)mpDoc->GetPool()->GetDefaultItem(ATTR_PATTERN)).
    FillEditItemSet( &aCaptionSet );

    // support the best position for the tail connector now that
    // that notes can be resized and repositioned.
    aCaptionSet.Put( SdrCaptionEscDirItem( SDRCAPT_ESCBESTFIT) );

    return aCaptionSet;
}

void ScPostIt::SetItemSet(const SfxItemSet& rItemSet)
{
    maItemSet.PutExtended(rItemSet, SFX_ITEM_DONTCARE, SFX_ITEM_DEFAULT);
}

void ScPostIt::InsertObject(SdrCaptionObj* pObj, ScDocument& rDoc, SCTAB nTab) const
{
    SdrPage* pPage = NULL;
    ScDrawLayer* pDrawModel = rDoc.GetDrawLayer();
    // Copying note cells between documents is one user scenario where the
    // DrawLayer may not exist. Use the ScModelObj() to create
    // and ensure there is a DrawLayer.
    if(!pDrawModel)
    {
        ScModelObj* pScModelObj = NULL;
        SfxObjectShell* pDocShell = rDoc.GetDocumentShell();
        if(pDocShell)
            pScModelObj = ScModelObj::getImplementation( Reference< XModel >( pDocShell->GetModel() ) );
        if(pScModelObj)
            pDrawModel= pScModelObj->MakeDrawLayer();
        if(pDrawModel)
            pPage = pDrawModel->GetPage(nTab);
    }
    else
        pPage = pDrawModel->GetPage(nTab);

    if(pPage && pObj)
        pPage->InsertObject(pObj);
}

void ScPostIt::RemoveObject(SdrCaptionObj* pObj, ScDocument& rDoc, SCTAB nTab) const
{
    SdrPage* pPage = NULL;
    ScDrawLayer* pDrawModel = rDoc.GetDrawLayer();
    if(pDrawModel)
        pPage = pDrawModel->GetPage(nTab);
    if(pPage && pObj)
        pPage->RemoveObject(pObj->GetOrdNum());
}

//========================================================================
