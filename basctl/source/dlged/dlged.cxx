/*************************************************************************
 *
 *  $RCSfile: dlged.cxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: tbe $ $Date: 2001-03-23 16:06:53 $
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

#ifndef _BASCTL_DLGED_HXX
#include "dlged.hxx"
#endif

#ifndef _BASCTL_DLGEDFUNC_HXX
#include "dlgedfunc.hxx"
#endif

#ifndef _BASCTL_DLGEDFAC_HXX
#include "dlgedfac.hxx"
#endif

#ifndef _BASCTL_DLGEDVIEW_HXX
#include "dlgedview.hxx"
#endif

#ifndef _BASCTL_DLGEDOBJ_HXX
#include "dlgedobj.hxx"
#endif

#ifndef _SV_SCRBAR_HXX
#include <vcl/scrbar.hxx>
#endif

#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif

#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XDIALOG_HPP_
#include <com/sun/star/awt/XDialog.hpp>
#endif

#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINER_HPP_
#include <com/sun/star/script/XLibraryContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLONEABLE_HPP_
#include <com/sun/star/util/XCloneable.hpp>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#include "vcsbxdef.hxx"


using namespace comphelper;
using namespace ::com::sun::star;
using namespace ::rtl;
using namespace ::com::sun::star::uno;

//----------------------------------------------------------------------------

IMPL_LINK( DlgEditor, ClipboardCleared, Clipboard *, EMPTYARG ) // not working yet
{
    if( !bClipPrivate )
        return 0;

    SdrModel** ppClipPrivate = (SdrModel**)GetAppData( SHL_VCED );

    if( *ppClipPrivate )
        return 0;

    delete *ppClipPrivate;
    *ppClipPrivate = NULL;

    bClipPrivate = FALSE;
    return 0;
}

//----------------------------------------------------------------------------

void DlgEditor::ShowDialog()
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >  xMSF = getProcessServiceFactory();

    // create a dialog
    uno::Reference< awt::XControl > xDlg( xMSF->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialog" ) ) ), uno::UNO_QUERY );

    // clone the dialog model
    uno::Reference< util::XCloneable > xC( m_xUnoControlDialogModel, uno::UNO_QUERY );
    uno::Reference< util::XCloneable > xNew = xC->createClone();
    uno::Reference< awt::XControlModel > xDlgMod( xNew, uno::UNO_QUERY );

    // set the model
    xDlg->setModel( xDlgMod );

    // create a peer
    uno::Reference< awt::XToolkit> xToolkit( xMSF->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.ExtToolkit" ) ) ), uno::UNO_QUERY );
    xDlg->createPeer( xToolkit, pWindow->GetComponentInterface() );

    uno::Reference< awt::XDialog > xD( xDlg, uno::UNO_QUERY );
    xD->execute();
}

//----------------------------------------------------------------------------

BOOL DlgEditor::UnmarkDialog()
{
    SdrObject*      pDlgObj = pSdrModel->GetPage(0)->GetObj(0);
    SdrPageView*    pPgView = pSdrView->GetPageViewPvNum(0);

    BOOL bWasMarked = pSdrView->IsObjMarked( pDlgObj );

    if( bWasMarked )
        pSdrView->MarkObj( pDlgObj, pPgView, FALSE );

    return bWasMarked;
}

//----------------------------------------------------------------------------

void DlgEditor::RemarkDialog()
{
    SdrObject*      pDlgObj = pSdrModel->GetPage(0)->GetObj(0);
    SdrPageView*    pPgView = pSdrView->GetPageViewPvNum(0);

    pSdrView->MarkObj( pDlgObj, pPgView, TRUE );
}

//----------------------------------------------------------------------------

DlgEditor::DlgEditor() :
    pHScroll(NULL),
    pVScroll(NULL),
    eMode( VCDLGED_SELECT ), // eActObj( OBJ_DLG_PUSHBUTTON ),
    bFirstDraw(FALSE),
    bGridSnap(FALSE),
    bGridVisible(FALSE),
    bClipPrivate(FALSE),
    bCreateOK(TRUE),
    pSdrView(NULL),
    bDialogModelChanged(FALSE)
{
    pWindow     = NULL;

    pSdrModel = new DlgEdModel;
    pSdrModel->GetItemPool().FreezeIdRanges();
    pSdrModel->SetScaleUnit( MAP_TWIP );

    SdrLayerAdmin& rAdmin = pSdrModel->GetLayerAdmin();
    rAdmin.NewStandardLayer();
    rAdmin.NewLayer( UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "HiddenLayer" ) ) );

    pSdrPage = new DlgEdPage( *pSdrModel );
    pSdrModel->InsertPage( pSdrPage );

    pObjFac = new DlgEdFactory();

    pFunc = new DlgEdFuncSelect( this );

    aClip.SetClearedHdl( LINK( this, DlgEditor, ClipboardCleared ) );

    aPaintTimer.SetTimeout( 1 );
    aPaintTimer.SetTimeoutHdl( LINK( this, DlgEditor, PaintTimeout ) );
}

//----------------------------------------------------------------------------

DlgEditor::~DlgEditor()
{
    ClipboardCleared( NULL );

    delete pObjFac;
    delete pFunc;
    delete pSdrView;
    delete pSdrModel;
}

//----------------------------------------------------------------------------

void DlgEditor::SetWindow( Window* pWindow )
{
    DlgEditor::pWindow = pWindow;
    pWindow->SetMapMode( MapMode( MAP_100TH_MM ) );
    pSdrPage->SetSize( pWindow->PixelToLogic( Size( 1280, 1024 ) ) );

    pSdrView = new DlgEdView( pSdrModel, pWindow );
    pSdrView->ShowPagePgNum( 0, Point() );
    pSdrView->SetLayerVisible( UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "HiddenLayer" ) ), FALSE );
    pSdrView->SetMoveSnapOnlyTopLeft( TRUE );

    Size aGridSize( 100, 100 );  // 100TH_MM
    bGridSnap    = TRUE;
    bGridVisible = TRUE;
    pSdrView->SetGridCoarse( aGridSize );
    pSdrView->SetSnapGrid( aGridSize );
    pSdrView->SetGridSnap( bGridSnap );
    pSdrView->SetGridVisible( FALSE );
    pSdrView->SetDragStripes( FALSE );

    pSdrView->SetDesignMode( TRUE );  // tbe put this somewhere else
}

//----------------------------------------------------------------------------

void DlgEditor::SetScrollBars( ScrollBar* pHS, ScrollBar* pVS )
{
    pHScroll = pHS;
    pVScroll = pVS;

    Size aOutSize = pWindow->GetOutputSize();
    Size aPgSize  = pSdrPage->GetSize();

    pHScroll->SetRange( Range( 0, aPgSize.Width()  ));
    pVScroll->SetRange( Range( 0, aPgSize.Height() ));
    pHScroll->SetVisibleSize( (ULONG)aOutSize.Width() );
    pVScroll->SetVisibleSize( (ULONG)aOutSize.Height() );

    pHScroll->SetLineSize( aOutSize.Width() / 10 );
    pVScroll->SetLineSize( aOutSize.Height() / 10 );
    pHScroll->SetPageSize( aOutSize.Width() / 2 );
    pVScroll->SetPageSize( aOutSize.Height() / 2 );

    DoScroll( pHScroll );
    DoScroll( pVScroll );
}

//----------------------------------------------------------------------------

void DlgEditor::DoScroll( ScrollBar* pActScroll )
{
    if( !pHScroll || !pVScroll )
        return;

    MapMode aMap = pWindow->GetMapMode();
    Point aOrg = aMap.GetOrigin();

    Size  aScrollPos( pHScroll->GetThumbPos(), pVScroll->GetThumbPos() );
    aScrollPos = pWindow->LogicToPixel( aScrollPos );
    aScrollPos = pWindow->PixelToLogic( aScrollPos );

    long  nX   = aScrollPos.Width() + aOrg.X();
    long  nY   = aScrollPos.Height() + aOrg.Y();

    if( !nX && !nY )
        return;

    pWindow->Update();

    Brush aOldBrush( pWindow->GetBackgroundBrush() );
    Brush aBrush( BRUSH_NULL );
    pWindow->SetBackgroundBrush( aBrush );
    pWindow->Scroll( -nX, -nY, SCROLL_NOCHILDREN );
    aMap.SetOrigin( Point( -aScrollPos.Width(), -aScrollPos.Height() ) );
    pWindow->SetMapMode( aMap );
    pWindow->Update();

    pWindow->SetBackgroundBrush( aOldBrush );
}

//----------------------------------------------------------------------------

void DlgEditor::SetDialog( uno::Reference< container::XNameContainer > xUnoControlDialogModel )
{
    /* FOR TEST
    // my dialog model  --  delete this later

    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >  xMSF = getProcessServiceFactory();


    //uno::Reference< awt::XToolkit> xToolkit( xMSF->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.ExtToolkit" ) ) ), uno::UNO_QUERY );

    // Create a DialogModel
    uno::Reference< container::XNameContainer > xC( xMSF->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlDialogModel" ) ) ), uno::UNO_QUERY );
    uno::Reference< lang::XMultiServiceFactory >  xModFact( xC, uno::UNO_QUERY );
    uno::Reference< beans::XPropertySet > xDlgPSet( xC, uno::UNO_QUERY );
    uno::Any aValue;
    aValue <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Test-Dialog" ) );
    xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Title" ) ), aValue );
    aValue <<= (sal_Int32) 0;
    xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
    aValue <<= (sal_Int32) 0;
    xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
    aValue <<= (sal_Int32) 0;
    xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
    aValue <<= (sal_Int32) 0;
    xDlgPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );

    // Create a ButtonModel as a DialogModel substitute
    uno::Reference< awt::XControlModel > xDlg1( xModFact->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlButtonModel" ) ) ), uno::UNO_QUERY );
    uno::Reference< beans::XPropertySet > xPSet( xDlg1, uno::UNO_QUERY );
    //aValue <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Test-Dialog" ) );
    //xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Title" ) ), aValue );
    aValue <<= (sal_Int32) 50;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
    aValue <<= (sal_Int32) 50;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
    aValue <<= (sal_Int32) 150;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
    aValue <<= (sal_Int32) 150;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );

    // Create a ButtonModel
    uno::Reference< awt::XControlModel > xCtrl1( xModFact->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlButtonModel" ) ) ), uno::UNO_QUERY );
    xPSet = uno::Reference< beans::XPropertySet >( xCtrl1, uno::UNO_QUERY );
    aValue <<= (sal_Int32) 10;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
    aValue <<= (sal_Int32) 80;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
    aValue <<= (sal_Int32) 50;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
    aValue <<= (sal_Int32) 20;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );
    aValue <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Button1" ) );
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Name" ) ), aValue );
    aValue <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Test!" ) );
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Label" ) ), aValue );
    uno::Any aAny;
    aAny <<= xCtrl1;
    xC->insertByName( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Button1" ) ), aAny );

     // Create a EditModel
    uno::Reference< awt::XControlModel > xCtrl2( xModFact->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.UnoControlEditModel" ) ) ), uno::UNO_QUERY );
    xPSet = uno::Reference< beans::XPropertySet >( xCtrl2, uno::UNO_QUERY );
    aValue <<= (sal_Int32) 10;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionX" ) ), aValue );
    aValue <<= (sal_Int32) 50;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "PositionY" ) ), aValue );
    aValue <<= (sal_Int32) 50;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Width" ) ), aValue );
    aValue <<= (sal_Int32) 20;
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Height" ) ), aValue );
    aValue <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Edit1" ) );
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Name" ) ), aValue );
    aValue <<= ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Text..." ) );
    xPSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Text" ) ), aValue );
    aAny <<= xCtrl2;
    xC->insertByName( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Edit1" ) ), aAny );

    // end of delete
    */

    // set dialog model
    m_xUnoControlDialogModel = xUnoControlDialogModel;

    // create dialog form
    pDlgEdForm = new DlgEdForm();
    uno::Reference< awt::XControlModel > xDlgMod( m_xUnoControlDialogModel , uno::UNO_QUERY );
    pDlgEdForm->SetUnoControlModel(xDlgMod);
    pDlgEdForm->StartListening();
    pDlgEdForm->SortByTabIndex();       // for backward compatibility
    pDlgEdForm->SetRectFromProps();
    pDlgEdForm->SetDlgEditor( this );
    ((DlgEdPage*)pSdrModel->GetPage(0))->SetDlgEdForm( pDlgEdForm );
    pSdrModel->GetPage(0)->InsertObject( pDlgEdForm );
    pDlgEdForm->SendRepaintBroadcast();

    // create controls
    Reference< ::com::sun::star::container::XNameAccess > xNameAcc( m_xUnoControlDialogModel, UNO_QUERY );
    if ( xNameAcc.is() )
    {
           Sequence< OUString > aNames = xNameAcc->getElementNames();
           const OUString* pNames = aNames.getConstArray();
        sal_uInt32 nCtrls = aNames.getLength();

        for( sal_uInt32 n = 0; n < nCtrls; n++ )
        {
               Any aA = xNameAcc->getByName( pNames[n] );
            Reference< ::com::sun::star::awt::XControlModel > xCtrlModel;
               aA >>= xCtrlModel;
            DlgEdObj* pCtrlObj = new DlgEdObj();
            pCtrlObj->SetDlgEdForm(pDlgEdForm);
            pDlgEdForm->AddChild(pCtrlObj);             // add child to parent form
            pCtrlObj->SetUnoControlModel( xCtrlModel );
            pCtrlObj->StartListening();
            pCtrlObj->SetRectFromProps();
            pCtrlObj->SetChanged();
            pSdrModel->GetPage(0)->InsertObject( pCtrlObj );
            pCtrlObj->UpdateStep();
            pCtrlObj->SendRepaintBroadcast();
        }
    }

    bFirstDraw = TRUE;

    pSdrModel->SetChanged( FALSE );
}

//----------------------------------------------------------------------------

void DlgEditor::MouseButtonDown( const MouseEvent& rMEvt )
{
    if( pWindow )
        pWindow->GrabFocus();
    pFunc->MouseButtonDown( rMEvt );
}

//----------------------------------------------------------------------------

void DlgEditor::MouseButtonUp( const MouseEvent& rMEvt )
{
    BOOL bRet = pFunc->MouseButtonUp( rMEvt );

    if( (eMode == VCDLGED_INSERT) )
        bCreateOK = bRet;
}

//----------------------------------------------------------------------------

void DlgEditor::MouseMove( const MouseEvent& rMEvt )
{
    pFunc->MouseMove( rMEvt );
}

//----------------------------------------------------------------------------

BOOL DlgEditor::KeyInput( const KeyEvent& rKEvt )
{
    return pFunc->KeyInput( rKEvt );
}

//----------------------------------------------------------------------------

void DlgEditor::Paint( const Rectangle& rRect )
{
    aPaintRect = rRect;
    PaintTimeout( &aPaintTimer );
}

//----------------------------------------------------------------------------

IMPL_LINK( DlgEditor, PaintTimeout, Timer *, EMPTYARG )
{
    static int nInPaint = FALSE;
    if( !pSdrView )
        return 0;

    nInPaint = TRUE;

    Size aMacSize;
    if( bFirstDraw &&
        pWindow->IsVisible() &&
        (pWindow->GetOutputSize() != aMacSize) )
    {
        bFirstDraw = FALSE;

        // get property set
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  xPSet(pDlgEdForm->GetUnoControlModel(), ::com::sun::star::uno::UNO_QUERY);

        if ( xPSet.is() )
        {
            // get dialog size from properties
            sal_Int32 nWidth, nHeight;
            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ) ) >>= nWidth;
            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) >>= nHeight;

            if ( nWidth == 0 && nHeight == 0 )
            {
                Size   aSize = pWindow->PixelToLogic( Size( 400, 300 ) );

                // align with grid
                Size  aGridSize = pSdrView->GetSnapGrid();
                aSize.Width()  -= aSize.Width()  % aGridSize.Width();
                aSize.Height() -= aSize.Height() % aGridSize.Height();

                Point  aPos;
                Size   aOutSize = pWindow->GetOutputSize();
                aPos.X() = (aOutSize.Width()>>1)  -  (aSize.Width()>>1);
                aPos.Y() = (aOutSize.Height()>>1) -  (aSize.Height()>>1);

                // align with grid
                aPos.X() -= aPos.X() % aGridSize.Width();
                aPos.Y() -= aPos.Y() % aGridSize.Height();

                // don't put in the corner
                Point aMinPos = pWindow->PixelToLogic( Point( 30, 20 ) );
                if( (aPos.X() < aMinPos.X()) || (aPos.Y() < aMinPos.Y()) )
                {
                    aPos = aMinPos;
                    aPos.X() -= aPos.X() % aGridSize.Width();
                    aPos.Y() -= aPos.Y() % aGridSize.Height();
                }

                // set dialog position and size
                pDlgEdForm->SetSnapRect( Rectangle( aPos, aSize ) );
                pDlgEdForm->SetPropsFromRect();

                // set position and size of controls
                ULONG nObjCount;
                if ( pSdrPage && ( ( nObjCount = pSdrPage->GetObjCount() ) > 0 ) )
                {
                    for ( ULONG i = 0 ; i < nObjCount ; i++ )
                    {
                        SdrObject* pObj = pSdrPage->GetObj(i);
                        DlgEdObj* pDlgEdObj = PTR_CAST(DlgEdObj, pObj);
                        if ( pDlgEdObj && !pDlgEdObj->ISA(DlgEdForm) )
                            pDlgEdObj->SetRectFromProps();
                    }
                }
            }
        }
    }

    pSdrView->InitRedraw( pWindow, Region( aPaintRect ) );

    nInPaint = FALSE;

    DBG_ASSERT(pWindow,"Window not set");
    return 0;
}

//----------------------------------------------------------------------------

void DlgEditor::SetMode( VCDlgMode eNewMode )
{
    if( eMode != eNewMode )
    {
        if( pFunc )
            delete pFunc;
    }

    eMode = eNewMode;
    if( eMode == VCDLGED_INSERT )
        pFunc = new DlgEdFuncInsert( this );
    else
        pFunc = new DlgEdFuncSelect( this );

    if( eMode == VCDLGED_TEST )
        ShowDialog();
}

//----------------------------------------------------------------------------

void DlgEditor::SetInsertObj( USHORT eObj )
{
    eActObj = eObj;

    if( pSdrView )
        pSdrView->SetCurrentObj( eActObj, VCSbxInventor );
}

//----------------------------------------------------------------------------

USHORT DlgEditor::GetInsertObj() const
{
    return eActObj;
}

//----------------------------------------------------------------------------

void DlgEditor::Cut()   // not working yet
{
    if( !pSdrView->HasMarkedObj() )
        return;

    pSdrView->BrkAction();

    BOOL bDlgMarked = UnmarkDialog();

    SdrModel* pMarked = pSdrView->GetAllMarkedModel();

    aClip.Clear();
    aClip.CopyPrivateData( pMarked );

    SdrModel** ppClipPrivate = (SdrModel**)GetAppData( SHL_VCED );
    *ppClipPrivate = pMarked;
    bClipPrivate = TRUE;

    Delete();

    if( bDlgMarked )
        RemarkDialog();
}

//----------------------------------------------------------------------------

void DlgEditor::Copy()  // not working yet
{
    if( !pSdrView->HasMarkedObj() )
        return;

    pSdrView->BrkAction();

    BOOL bDlgMarked = UnmarkDialog();

    SdrModel* pMarked = pSdrView->GetAllMarkedModel();

    aClip.Clear();
    aClip.CopyPrivateData( pMarked );

    SdrModel** ppClipPrivate = (SdrModel**)GetAppData( SHL_VCED );
    *ppClipPrivate = pMarked;
    bClipPrivate   = TRUE;

    if( bDlgMarked )
        RemarkDialog();
}

//----------------------------------------------------------------------------

void DlgEditor::Paste() // not working yet
{
    SdrModel** ppClipPrivate = (SdrModel**)GetAppData( SHL_VCED );

    pSdrView->BrkAction();

    if( !*ppClipPrivate )
        return;

    static Point aDefPoint;
    /*
    Rectangle aRect( aDefPoint,
                     pSbxForm->GetSize() );
    pSdrView->Paste( **ppClipPrivate, aRect.Center() );
    */
}

//----------------------------------------------------------------------------

void DlgEditor::Delete()
{
    if( !pSdrView->HasMarkedObj() )
        return;

    // Sicherstellen, dass nicht ein Dialog geloescht wird. So machts man
    // richtig (inkompat.,ab 369): SdrView::CheckPossibilities() ueberladen
    ULONG nMark = pSdrView->GetMarkList().GetMarkCount();

    for( ULONG i = 0; i < nMark; i++ )
    {
        if(pSdrView->GetMarkList().GetMark(i)->GetObj()->ISA(DlgEdForm))
            return;
    }

    // remove control models of marked objects from dialog model
    for( i = 0; i < nMark; i++ )
    {
        SdrObject* pObj = pSdrView->GetMarkList().GetMark(i)->GetObj();
        DlgEdObj* pDlgEdObj = PTR_CAST(DlgEdObj, pObj);

        if (pDlgEdObj)
        {
            // get name from property
            ::rtl::OUString aName;
            uno::Reference< beans::XPropertySet >  xPSet(pDlgEdObj->GetUnoControlModel(), uno::UNO_QUERY);
            if (xPSet.is())
            {
                xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Name" ) ) ) >>= aName;
            }

            // remove control from dialog model
            Reference< ::com::sun::star::container::XNameAccess > xNameAcc(pDlgEdObj->GetDlgEdForm()->GetUnoControlModel(), UNO_QUERY );
            if ( xNameAcc.is() && xNameAcc->hasByName(aName) )
            {
                Reference< ::com::sun::star::container::XNameContainer > xCont(xNameAcc, UNO_QUERY );
                if ( xCont.is() )
                {
                    xCont->removeByName( aName );
                }
            }

            // remove child from parent form
            pDlgEdForm->RemoveChild( pDlgEdObj );
        }
    }

    // update tabindex
    pDlgEdForm->SortByTabIndex();

    pSdrView->BrkAction();

    BOOL bDlgMarked = UnmarkDialog();

    pSdrView->DeleteMarked();

    if( bDlgMarked )
        RemarkDialog();
}

//----------------------------------------------------------------------------

BOOL DlgEditor::IsModified() const
{
    return pSdrModel->IsChanged() || bDialogModelChanged;
}

//----------------------------------------------------------------------------

void DlgEditor::ClearModifyFlag()
{
    pSdrModel->SetChanged( FALSE );
    bDialogModelChanged = FALSE;
}

//----------------------------------------------------------------------------

#define LMARGPRN        1700
#define RMARGPRN         900
#define TMARGPRN        2000
#define BMARGPRN        1000
#define BORDERPRN       300

//----------------------------------------------------------------------------

void lcl_PrintHeader( Printer* pPrinter, const String& rTitle ) // not working yet
{
    short nLeftMargin   = LMARGPRN;
    Size aSz = pPrinter->GetOutputSize();
    short nBorder = BORDERPRN;

    Pen aOldPen( pPrinter->GetPen() );
    Brush aOldBrush( pPrinter->GetFillInBrush() );
    Font aOldFont( pPrinter->GetFont() );

    pPrinter->SetPen( Pen( (PenStyle)PEN_SOLID ) );
    pPrinter->SetFillInBrush( Brush( BRUSH_NULL ) );

    Font aFont( aOldFont );
    aFont.SetWeight( WEIGHT_BOLD );
    aFont.SetAlign( ALIGN_BOTTOM );
    pPrinter->SetFont( aFont );

    long nFontHeight = pPrinter->GetTextHeight();

    // 1.Border => Strich, 2+3 Border = Freiraum.
    long nYTop = TMARGPRN-3*nBorder-nFontHeight;

    long nXLeft = nLeftMargin-nBorder;
    long nXRight = aSz.Width()-RMARGPRN+nBorder;

    pPrinter->DrawRect( Rectangle(
        Point( nXLeft, nYTop ),
        Size( nXRight-nXLeft, aSz.Height() - nYTop - BMARGPRN + nBorder ) ) );

    long nY = TMARGPRN-2*nBorder;
    Point aPos( nLeftMargin, nY );
    pPrinter->DrawText( aPos, rTitle );

    nY = TMARGPRN-nBorder;

    pPrinter->DrawLine( Point( nXLeft, nY ), Point( nXRight, nY ) );

    pPrinter->SetPen( aOldPen );
    pPrinter->SetFont( aOldFont );
    pPrinter->SetFillInBrush( aOldBrush );
}

//----------------------------------------------------------------------------

void DlgEditor::PrintData( Printer* pPrinter, const String& rTitle )    // not working yet
{
    if( pSdrView )
    {
        MapMode aOldMap( pPrinter->GetMapMode());
        Font aOldFont( pPrinter->GetFont() );

        MapMode aMap( MAP_100TH_MM );
        pPrinter->SetMapMode( aMap );
        Font aFont;
#ifdef OS2
        aFont.SetName( System::GetStandardFont( STDFONT_SWISS ).GetName() );
#endif
        aFont.SetAlign( ALIGN_BOTTOM );
        aFont.SetSize( Size( 0, 360 ));
        pPrinter->SetFont( aFont );

        Size aPaperSz = pPrinter->GetOutputSize();
        aPaperSz.Width() -= (LMARGPRN+RMARGPRN);
        aPaperSz.Height() -= (TMARGPRN+BMARGPRN);

        pPrinter->StartPage();

        lcl_PrintHeader( pPrinter, rTitle );

        Bitmap aDlg;
#ifdef OS2
        Bitmap* pDlg = new Bitmap;
        //pSbxForm->SnapShot( *pDlg );
        SvMemoryStream* pStrm = new SvMemoryStream;
        *pStrm << *pDlg;
        delete pDlg;
        pStrm->Seek(0);
        *pStrm >> aDlg;
        delete pStrm;
#else
        //pSbxForm->SnapShot( aDlg );
#endif
        Size aBmpSz( pPrinter->PixelToLogic( aDlg.GetSizePixel() ) );
        double nPaperSzWidth = aPaperSz.Width();
        double nPaperSzHeight = aPaperSz.Height();
        double nBmpSzWidth = aBmpSz.Width();
        double nBmpSzHeight = aBmpSz.Height();
        double nScaleX = (nPaperSzWidth / nBmpSzWidth );
        double nScaleY = (nPaperSzHeight / nBmpSzHeight );

        Size aOutputSz;
        if( nBmpSzHeight * nScaleX <= nPaperSzHeight )
        {
            aOutputSz.Width() = (long)(((double)nBmpSzWidth) * nScaleX);
            aOutputSz.Height() = (long)(((double)nBmpSzHeight) * nScaleX);
        }
        else
        {
            aOutputSz.Width() = (long)(((double)nBmpSzWidth) * nScaleY);
            aOutputSz.Height() = (long)(((double)nBmpSzHeight) * nScaleY);
        }

        Point aPosOffs(
            (aPaperSz.Width() / 2) - (aOutputSz.Width() / 2),
            (aPaperSz.Height()/ 2) - (aOutputSz.Height() / 2));

        aPosOffs.X() += LMARGPRN;
        aPosOffs.Y() += TMARGPRN;

        pPrinter->DrawBitmap( aPosOffs, aOutputSz, aDlg );

        pPrinter->EndPage();

        pPrinter->SetMapMode( aOldMap );
        pPrinter->SetFont( aOldFont );
    }
}

//----------------------------------------------------------------------------
