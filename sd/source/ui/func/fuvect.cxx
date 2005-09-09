/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuvect.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 04:53:37 $
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


#include "fuvect.hxx"

#ifndef _TL_POLY_HXX
#include <tools/poly.hxx>
#endif
#ifndef _SVDOPATH_HXX //autogen
#include <svx/svdograf.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _SVX_SVDOGRAF_HXX //autogen
#include <svx/svdograf.hxx>
#endif
#ifndef _SVX_SVDEDTV_HXX //autogen
#include <svx/svdedtv.hxx>
#endif

#pragma hdrstop

#ifndef SD_VIEW_HXX
#include "View.hxx"
#endif
#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif
#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif
#include "strings.hrc"
#include "sdresid.hxx"
//CHINA001 #include "vectdlg.hxx"
#include "sdabstdlg.hxx" //CHINA001
#include "vectdlg.hrc" //CHINA001
namespace sd {

TYPEINIT1( FuVectorize, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuVectorize::FuVectorize (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq)
    : FuPoor (pViewSh, pWin, pView, pDoc, rReq)
{
    const SdrMarkList& rMarkList = pView->GetMarkedObjectList();

    if( rMarkList.GetMarkCount() == 1 )
    {
        SdrObject* pObj = rMarkList.GetMark( 0 )->GetObj();

        if( pObj && pObj->ISA( SdrGrafObj ) )
        {
            //CHINA001 SdVectorizeDlg aDlg(pWin, ( (SdrGrafObj*) pObj )->GetGraphic().GetBitmap(), pDocSh );
            SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();//CHINA001
            DBG_ASSERT(pFact, "SdAbstractDialogFactory fail!");//CHINA001
            AbstractSdVectorizeDlg* pDlg = pFact->CreateSdVectorizeDlg(ResId( DLG_VECTORIZE ), pWin, ( (SdrGrafObj*) pObj )->GetGraphic().GetBitmap(), pDocSh );
            DBG_ASSERT(pDlg, "Dialogdiet fail!");//CHINA001
            if( pDlg->Execute() == RET_OK ) //CHINA001 if( aDlg.Execute() == RET_OK )
            {
                const GDIMetaFile&  rMtf = pDlg->GetGDIMetaFile(); //CHINA001 const GDIMetaFile&    rMtf = aDlg.GetGDIMetaFile();
                SdrPageView*        pPageView = pView->GetPageViewPvNum( 0 );

                if( pPageView && rMtf.GetActionCount() )
                {
                    SdrGrafObj* pVectObj = (SdrGrafObj*) pObj->Clone();
                    String      aStr( pView->GetDescriptionOfMarkedObjects() );

                    aStr.Append( sal_Unicode(' ') );
                    aStr.Append( String( SdResId( STR_UNDO_VECTORIZE ) ) );
                    pView->BegUndo( aStr );
                    pVectObj->SetGraphic( rMtf );
                    pView->ReplaceObject( pObj, *pPageView, pVectObj );
                    pView->EndUndo();
                }
            }
            delete pDlg; //add by CHINA001
        }
    }
}

} // end of namespace sd
