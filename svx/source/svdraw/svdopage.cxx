/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: svdopage.cxx,v $
 *
 *  $Revision: 1.20 $
 *
 *  last change: $Author: ihi $ $Date: 2006-08-29 14:42:03 $
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

#include "svdopage.hxx"
#include "svdglob.hxx"  // Stringcache
#include "svdstr.hrc"   // Objektname
#include "svdtrans.hxx"
#include "svdetc.hxx"
#include "svdxout.hxx"
#include "svdmodel.hxx"
#include "svdpage.hxx"
#include "svdpagv.hxx"
#include "svdoutl.hxx"

#ifndef INCLUDED_SVTOOLS_COLORCFG_HXX
#include <svtools/colorcfg.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif

#ifndef _SDR_PROPERTIES_PAGEPROPERTIES_HXX
#include <svx/sdr/properties/pageproperties.hxx>
#endif

// #111111#
#ifndef _SDR_CONTACT_VIEWCONTACTOFPAGEOBJ_HXX
#include <svx/sdr/contact/viewcontactofpageobj.hxx>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseProperties section

sdr::properties::BaseProperties* SdrPageObj::CreateObjectSpecificProperties()
{
    return new sdr::properties::PageProperties(*this);
}

//////////////////////////////////////////////////////////////////////////////
// DrawContact section

sdr::contact::ViewContact* SdrPageObj::CreateObjectSpecificViewContact()
{
    return new sdr::contact::ViewContactOfPageObj(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// this method is called form the destructor of the referenced page.
// do all necessary action to forget the page. It is not necessary to call
// RemovePageUser(), that is done form the destructor.
void SdrPageObj::PageInDestruction(const SdrPage& rPage)
{
    if(mpShownPage && mpShownPage == &rPage)
    {
        // #i58769# Do not call ActionChanged() here, because that would
        // lead to the construction of a view contact object for a page that
        // is being destroyed.

        mpShownPage = 0L;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TYPEINIT1(SdrPageObj,SdrObject);

SdrPageObj::SdrPageObj(SdrPage* pNewPage)
:   mpShownPage(pNewPage)
{
    if(mpShownPage)
    {
        mpShownPage->AddPageUser(*this);
    }
}

SdrPageObj::SdrPageObj(const Rectangle& rRect, SdrPage* pNewPage)
:   mpShownPage(pNewPage)
{
    if(mpShownPage)
    {
        mpShownPage->AddPageUser(*this);
    }

    aOutRect = rRect;
}

SdrPageObj::~SdrPageObj()
{
    // #111111#
    if(mpShownPage)
    {
        mpShownPage->RemovePageUser(*this);
    }
}

// #111111#
SdrPage* SdrPageObj::GetReferencedPage() const
{
    return mpShownPage;
}

// #111111#
void SdrPageObj::SetReferencedPage(SdrPage* pNewPage)
{
    if(mpShownPage != pNewPage)
    {
        if(mpShownPage)
        {
            mpShownPage->RemovePageUser(*this);
        }

        mpShownPage = pNewPage;

        if(mpShownPage)
        {
            mpShownPage->AddPageUser(*this);
        }

        SetChanged();
        BroadcastObjectChange();
    }
}

UINT16 SdrPageObj::GetObjIdentifier() const
{
    return UINT16(OBJ_PAGE);
}

void SdrPageObj::TakeObjInfo(SdrObjTransformInfoRec& rInfo) const
{
    rInfo.bRotateFreeAllowed=FALSE;
    rInfo.bRotate90Allowed  =FALSE;
    rInfo.bMirrorFreeAllowed=FALSE;
    rInfo.bMirror45Allowed  =FALSE;
    rInfo.bMirror90Allowed  =FALSE;
    rInfo.bTransparenceAllowed = FALSE;
    rInfo.bGradientAllowed = FALSE;
    rInfo.bShearAllowed     =FALSE;
    rInfo.bEdgeRadiusAllowed=FALSE;
    rInfo.bNoOrthoDesired   =FALSE;
    rInfo.bCanConvToPath    =FALSE;
    rInfo.bCanConvToPoly    =FALSE;
    rInfo.bCanConvToPathLineToArea=FALSE;
    rInfo.bCanConvToPolyLineToArea=FALSE;
}

void SdrPageObj::operator=(const SdrObject& rObj)
{
    SdrObject::operator=(rObj);
    SetReferencedPage(((const SdrPageObj&)rObj).GetReferencedPage());
}

void SdrPageObj::TakeObjNameSingul(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNameSingulPAGE);

    String aName( GetName() );
    if(aName.Len())
    {
        rName += sal_Unicode(' ');
        rName += sal_Unicode('\'');
        rName += aName;
        rName += sal_Unicode('\'');
    }
}

void SdrPageObj::TakeObjNamePlural(XubString& rName) const
{
    rName=ImpGetResStr(STR_ObjNamePluralPAGE);
}

//BFS01void SdrPageObj::WriteData(SvStream& rOut) const
//BFS01{
//BFS01 SdrObject::WriteData(rOut);
//BFS01 SdrDownCompat aCompat(rOut,STREAM_WRITE); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
//BFS01#ifdef DBG_UTIL
//BFS01 aCompat.SetID("SdrPageObj");
//BFS01#endif
//BFS01
//BFS01 // #111111#
//BFS01 if(mpShownPage)
//BFS01 {
//BFS01     rOut << (sal_uInt16)mpShownPage->GetPageNum();
//BFS01 }
//BFS01 else
//BFS01 {
//BFS01     rOut << (sal_uInt16)0;
//BFS01 }
//BFS01}

//BFS01void SdrPageObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
//BFS01{
//BFS01 if (rIn.GetError()!=0) return;
//BFS01 SdrObject::ReadData(rHead,rIn);
//BFS01 SdrDownCompat aCompat(rIn,STREAM_READ); // Fuer Abwaertskompatibilitaet (Lesen neuer Daten mit altem Code)
//BFS01#ifdef DBG_UTIL
//BFS01 aCompat.SetID("SdrPageObj");
//BFS01#endif
//BFS01
//BFS01 // #111111#
//BFS01 sal_uInt16 nPageNum;
//BFS01 rIn >> nPageNum;
//BFS01
//BFS01 if(GetModel())
//BFS01 {
//BFS01     SdrPage* pNewPage = GetModel()->GetPage(nPageNum);
//BFS01     SetReferencedPage(pNewPage);
//BFS01 }
//BFS01}

////////////////////////////////////////////////////////////////////////////////////////////////////
// eof
