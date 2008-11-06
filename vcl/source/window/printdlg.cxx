/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: printdlg.cxx,v $
 * $Revision: 1.1.2.7 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "precompiled_vcl.hxx"

#include "vcl/print.hxx"
#include "vcl/prndlg.hxx"
#include "vcl/dialog.hxx"
#include "vcl/button.hxx"
#include "vcl/svdata.hxx"
#include "vcl/svids.hrc"
#include "vcl/wall.hxx"
#include "vcl/jobset.h"

#include "rtl/ustrbuf.hxx"

#include "com/sun/star/awt/Size.hpp"

using namespace vcl;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;

PrintDialog::PrintPreviewWindow::PrintPreviewWindow( Window* i_pParent, const ResId& i_rId )
    : Window( i_pParent, i_rId )
{
}

PrintDialog::PrintPreviewWindow::~PrintPreviewWindow()
{
}

void PrintDialog::PrintPreviewWindow::Paint( const Rectangle& i_rRect )
{
    Window::Paint( i_rRect );

    SetFillColor( Color( COL_WHITE ) );
    SetLineColor();
    DrawRect( Rectangle( Point( 0, 0 ), GetSizePixel() ));
    Push();
    SetMapMode( MAP_100TH_MM );
    maMtf.WindStart();
    maMtf.Play( this, Point( 0, 0 ), PixelToLogic( GetSizePixel() ) );
    Pop();
}

void PrintDialog::PrintPreviewWindow::setPreview( const GDIMetaFile& i_rNewPreview )
{
    maMtf = i_rNewPreview;
    Invalidate();
}

PrintDialog::PrinterTabPage::PrinterTabPage( Window* i_pParent, const ResId& rResId )
    : TabPage( i_pParent, rResId )
    , maPrinters( this, VclResId( SV_PRINT_PRINTERS) )
    , maSetupButton( this, VclResId( SV_PRINT_PRT_SETUP ) )
    , maType( this, VclResId( SV_PRINT_PRT_TYPE ) )
    , maTypeText( this, VclResId( SV_PRINT_PRT_TYPE_TXT ) )
    , maStatus( this, VclResId( SV_PRINT_PRT_STATUS ) )
    , maStatusText(this, VclResId( SV_PRINT_PRT_STATUS_TXT ) )
    , maLocation( this, VclResId( SV_PRINT_PRT_LOCATION ) )
    , maLocText( this, VclResId( SV_PRINT_PRT_LOCATION_TXT ) )
    , maComment( this, VclResId( SV_PRINT_PRT_COMMENT ) )
    , maCommentText( this, VclResId( SV_PRINT_PRT_COMMENT_TXT ) )
    , maToFileBox( this, VclResId( SV_PRINT_PRT_TOFILE ) )
{
    FreeResource();
}

PrintDialog::PrinterTabPage::~PrinterTabPage()
{
}

PrintDialog::JobTabPage::JobTabPage( Window* i_pParent, const ResId& rResId )
    : TabPage( i_pParent, rResId )
    , maPrintRange( this, VclResId( SV_PRINT_RANGE ) )
    , maAllButton( this, VclResId( SV_PRINT_ALL ) )
    , maPagesButton( this, VclResId( SV_PRINT_PAGERANGE ) )
    , maSelectionButton( this, VclResId( SV_PRINT_SELECTION ) )
    , maPagesEdit( this, VclResId( SV_PRINT_PAGERANGE_EDIT ) )
    , maCopies( this, VclResId( SV_PRINT_COPIES ) )
    , maCopyCount( this, VclResId( SV_PRINT_COPYCOUNT ) )
    , maCopyCountField( this, VclResId( SV_PRINT_COPYCOUNT_FIELD ) )
    , maCollateBox( this, VclResId( SV_PRINT_COLLATE ) )
    , maCollateImage( this, VclResId( SV_PRINT_COLLATE_IMAGE ) )
    , maCollateImg( VclResId( SV_PRINT_COLLATE_IMG ) )
    , maCollateHCImg( VclResId( SV_PRINT_COLLATE_HC_IMG ) )
    , maNoCollateImg( VclResId( SV_PRINT_NOCOLLATE_IMG ) )
    , maNoCollateHCImg( VclResId( SV_PRINT_NOCOLLATE_HC_IMG ) )
{
    FreeResource();
}

PrintDialog::JobTabPage::~JobTabPage()
{
}

PrintDialog::PrintDialog( Window* i_pParent, const boost::shared_ptr<PrinterListener>& i_rListener )
    : ModalDialog( i_pParent, VclResId( SV_DLG_PRINT ) )
    , maOKButton( this, VclResId( SV_PRINT_OK ) )
    , maCancelButton( this, VclResId( SV_PRINT_CANCEL ) )
    , maPreviewWindow( this, VclResId( SV_PRINT_PAGE_PREVIEW ) )
    , maPageText( this, VclResId( SV_PRINT_PAGE_TXT ) )
    , maPageScrollbar( this, VclResId( SV_PRINT_PAGE_SCROLL ) )
    , maTabCtrl( this, VclResId( SV_PRINT_TABCTRL ) )
    , maPrinterPage( &maTabCtrl, VclResId( SV_PRINT_TAB_PRINTER ) )
    , maJobPage( &maTabCtrl, VclResId( SV_PRINT_TAB_JOB ) )
    , maButtonLine( this, VclResId( SV_PRINT_BUTTONLINE ) )
    , maPListener( i_rListener )
    , mnCurPage( 0 )
    , mnCachedPages( 0 )
{
    FreeResource();

    // insert the tab pages
    maTabCtrl.InsertPage( SV_PRINT_PAGE_PREVIEW, maPrinterPage.GetText() );
    maTabCtrl.SetTabPage( SV_PRINT_PAGE_PREVIEW, &maPrinterPage );
    maTabCtrl.InsertPage( SV_PRINT_TAB_JOB, maJobPage.GetText() );
    maTabCtrl.SetTabPage( SV_PRINT_TAB_JOB, &maJobPage );

    maPageStr = maPageText.GetText();
    // save space for the preview window
    maPreviewSpace = Rectangle( maPreviewWindow.GetPosPixel(), maPreviewWindow.GetSizePixel() );
    // get the first page
    preparePreview();

    // set up the scrollbar for the preview pages
    maPageScrollbar.SetScrollHdl( LINK( this, PrintDialog, ScrollHdl ) );
    maPageScrollbar.SetEndScrollHdl( LINK( this, PrintDialog, ScrollEndHdl ) );
    maPageScrollbar.EnableDrag( TRUE );

    // fill printer listbox
    const std::vector< rtl::OUString >& rQueues( Printer::GetPrinterQueues() );
    for( std::vector< rtl::OUString >::const_iterator it = rQueues.begin();
         it != rQueues.end(); ++it )
    {
        maPrinterPage.maPrinters.InsertEntry( *it );
    }
    // select current printer
    if( maPrinterPage.maPrinters.GetEntryPos( maPListener->getPrinter()->GetName() ) != LISTBOX_ENTRY_NOTFOUND )
        maPrinterPage.maPrinters.SelectEntry( maPListener->getPrinter()->GetName() );
    else
    {
        // fall back to default printer
        maPrinterPage.maPrinters.SelectEntry( Printer::GetDefaultPrinterName() );
        maPListener->setPrinter( boost::shared_ptr<Printer>( new Printer( Printer::GetDefaultPrinterName() ) ) );
    }
    // update the text fields for the printer
    updatePrinterText();

    // set a select handler
    maPrinterPage.maPrinters.SetSelectHdl( LINK( this, PrintDialog, SelectHdl ) );

    // setup page range edit
    rtl::OUStringBuffer aBuf( 16 );
    aBuf.append( sal_Unicode('1') );
    if( mnCachedPages > 1 )
    {
        aBuf.append( sal_Unicode('-') );
        aBuf.append( mnCachedPages );
    }
    maJobPage.maPagesEdit.SetText( aBuf.makeStringAndClear() );

    // setup click handler on the various buttons
    maJobPage.maCollateBox.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maAllButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maSelectionButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maJobPage.maPagesButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );
    maPrinterPage.maSetupButton.SetClickHdl( LINK( this, PrintDialog, ClickHdl ) );

    // setup modify hdl
    maJobPage.maCopyCountField.SetModifyHdl( LINK( this, PrintDialog, ModifyHdl ) );

    // setup optional UI options set by application
    setupOptionalUI();

    // set change handler for UI options
    maPListener->setOptionChangeHdl( LINK( this, PrintDialog, UIOptionsChanged ) );

    // setup dependencies
    checkControlDependencies();
}

PrintDialog::~PrintDialog()
{
    while( ! maControls.empty() )
    {
        delete maControls.front();
        maControls.pop_front();
    }
}

bool PrintDialog::isPrintToFile()
{
    return maPrinterPage.maToFileBox.IsChecked();
}

int PrintDialog::getCopyCount()
{
    return static_cast<int>(maJobPage.maCopyCountField.GetValue());
}

bool PrintDialog::isCollate()
{
    return maJobPage.maCopyCountField.GetValue() > 1 ? maJobPage.maCollateBox.IsChecked() : FALSE;
}

MultiSelection PrintDialog::getPageSelection()
{
    if( maJobPage.maPagesButton.IsChecked() )
        return MultiSelection( maJobPage.maPagesEdit.GetText() );
    else if( maJobPage.maAllButton.IsChecked() )
    {
        MultiSelection aSel( Range( 1, maPListener->getPageCount() ) );
        aSel.SelectAll();
        return aSel;
    }
    DBG_ERROR( "NYI: Selection" );
    return MultiSelection();
}

void PrintDialog::setupOptionalUI()
{
    Window* pCurParent = 0;
    long nCurY = 0;
    USHORT nOptPageId = 9;
    MapMode aFontMapMode( MAP_APPFONT );

    Size aTabSize = maTabCtrl.GetSizePixel();
    const Sequence< PropertyValue >& rOptions( maPListener->getUIOptions() );
    for( int i = 0; i < rOptions.getLength(); i++ )
    {
        Sequence< beans::PropertyValue > aOptProp;
        rOptions[i].Value >>= aOptProp;

        // extract ui element
        bool bEnabled = true;
        rtl::OUString aCtrlType;
        rtl::OUString aText;
        rtl::OUString aPropertyName;
        Sequence< rtl::OUString > aChoices;

        for( int n = 0; n < aOptProp.getLength(); n++ )
        {
            const beans::PropertyValue& rEntry( aOptProp[ n ] );
            if( rEntry.Name.equalsAscii( "Text" ) )
            {
                rEntry.Value >>= aText;
            }
            else if( rEntry.Name.equalsAscii( "ControlType" ) )
            {
                rEntry.Value >>= aCtrlType;
            }
            else if( rEntry.Name.equalsAscii( "Choices" ) )
            {
                rEntry.Value >>= aChoices;
            }
            else if( rEntry.Name.equalsAscii( "Property" ) )
            {
                PropertyValue aVal;
                rEntry.Value >>= aVal;
                aPropertyName = aVal.Name;
            }
            else if( rEntry.Name.equalsAscii( "Enabled" ) )
            {
                sal_Bool bValue = sal_True;
                rEntry.Value >>= bValue;
                bEnabled = bValue;
            }
        }

        if( aCtrlType.equalsAscii( "Group" ) ||
            aCtrlType.equalsAscii( "Subgroup" ) ||
            aCtrlType.equalsAscii( "Radio" ) ||
            aCtrlType.equalsAscii( "List" )  ||
            aCtrlType.equalsAscii( "Bool" ) )
        {
            if( aCtrlType.equalsAscii( "Group" ) || ! pCurParent )
            {
                // add new tab page
                TabPage* pNewGroup = new TabPage( &maTabCtrl );
                maControls.push_front( pNewGroup );
                pCurParent = pNewGroup;
                nCurY = 5;
                pNewGroup->SetText( aText );
                maTabCtrl.InsertPage( ++nOptPageId, aText );
                maTabCtrl.SetTabPage( nOptPageId, pNewGroup );
            }

            if( aCtrlType.equalsAscii( "Subgroup" ) && pCurParent )
            {
                FixedLine* pNewSub = new FixedLine( pCurParent );
                maControls.push_front( pNewSub );
                pNewSub->SetText( aText );
                nCurY += 4;
                Size aPixelSize( aTabSize );
                aPixelSize.Width() /= 2;
                aPixelSize.Height() = pCurParent->GetTextHeight() + 4;
                pNewSub->SetPosSizePixel( pNewSub->LogicToPixel( Point( 5, nCurY ), aFontMapMode ),
                                          aPixelSize );
                pNewSub->Show();
                nCurY += 12;
            }
            else if( aCtrlType.equalsAscii( "Bool" ) && pCurParent )
            {
                // add a check box
                CheckBox* pNewBox = new CheckBox( pCurParent );
                maControls.push_front( pNewBox );
                pNewBox->SetText( aText );

                // FIXME: measure text
                pNewBox->SetPosSizePixel( pNewBox->LogicToPixel( Point( 5, nCurY ), aFontMapMode ),
                                          pNewBox->LogicToPixel( Size( 100, 10 ), aFontMapMode ) );
                nCurY += 12;

                pNewBox->Show();
                sal_Bool bVal = sal_False;
                PropertyValue* pVal = maPListener->getValue( aPropertyName );
                if( pVal )
                    pVal->Value >>= bVal;
                pNewBox->Check( bVal );
                pNewBox->Enable( maPListener->isUIOptionEnabled( aPropertyName ) && pVal != NULL );

                maPropertyToWindowMap.insert( std::pair< rtl::OUString, Window* >( aPropertyName, pNewBox ) );
            }
            else if( aCtrlType.equalsAscii( "Radio" ) && pCurParent )
            {
                long nXPos = 5;

                if( aText.getLength() )
                {
                    // add a FixedText:
                    FixedText* pHeading = new FixedText( pCurParent );
                    maControls.push_front( pHeading );
                    pHeading->SetText( aText );
                    Size aPixelSize( pHeading->LogicToPixel( Size( 10, 10 ), aFontMapMode ) );
                    aPixelSize.Width() = aTabSize.Width() - aPixelSize.Width();
                    pHeading->SetPosSizePixel( pHeading->LogicToPixel( Point( nXPos, nCurY ), aFontMapMode ),
                                               aPixelSize );
                    pHeading->Show();

                    nXPos = 15;
                    nCurY += 12;
                }

                // iterate options
                rtl::OUString aSelectVal;
                PropertyValue* pVal = maPListener->getValue( aPropertyName );
                if( pVal && pVal->Value.hasValue() )
                    pVal->Value >>= aSelectVal;
                for( sal_Int32 m = 0; m < aChoices.getLength(); m++ )
                {
                    RadioButton* pBtn = new RadioButton( pCurParent, m == 0 ? WB_GROUP : 0 );
                    maControls.push_front( pBtn );
                    pBtn->SetText( aChoices[m] );
                    pBtn->Check( aChoices[m] == aSelectVal );
                    Size aPixelSize( pBtn->LogicToPixel( Size( 10 + nXPos, 12 ), aFontMapMode ) );
                    aPixelSize.Width() = aTabSize.Width() - aPixelSize.Width();
                    pBtn->SetPosSizePixel( pBtn->LogicToPixel( Point( 15, nCurY ), aFontMapMode ),
                                           aPixelSize );
                    pBtn->Enable( maPListener->isUIOptionEnabled( aPropertyName ) );
                    pBtn->Show();
                    maPropertyToWindowMap.insert( std::pair< rtl::OUString, Window* >( aPropertyName, pBtn ) );
                    nCurY += 12;
                }
            }
            else if( aCtrlType.equalsAscii( "List" ) && pCurParent )
            {
                // add a FixedText:
                FixedText* pHeading = new FixedText( pCurParent );
                maControls.push_front( pHeading );
                pHeading->SetText( aText );
                Size aPixelSize( pHeading->LogicToPixel( Size( 10, 10 ), aFontMapMode ) );
                aPixelSize.Width() += pHeading->GetTextWidth( aText );
                pHeading->SetPosSizePixel( pHeading->LogicToPixel( Point( 5, nCurY ), aFontMapMode ),
                                           aPixelSize );
                pHeading->Show();

                ListBox* pList = new ListBox( pCurParent, WB_DROPDOWN | WB_BORDER );
                maControls.push_front( pList );

                // iterate options
                long nMaxTextWidth = 0;
                for( sal_Int32 m = 0; m < aChoices.getLength(); m++ )
                {
                    pList->InsertEntry( aChoices[m] );
                    long nEntryWidth = pList->GetTextWidth( aChoices[m] );
                    if( nEntryWidth > nMaxTextWidth )
                        nMaxTextWidth = nEntryWidth;
                }
                nMaxTextWidth += 30;
                rtl::OUString aSelectVal;
                PropertyValue* pVal = maPListener->getValue( aPropertyName );
                if( pVal && pVal->Value.hasValue() )
                    pVal->Value >>= aSelectVal;
                pList->SelectEntry( aSelectVal );

                aPixelSize = Size( pList->LogicToPixel( Size( 25, 12 ), aFontMapMode ) );
                aPixelSize.Width() = nMaxTextWidth;
                aPixelSize.Height() *= aChoices.getLength() > 15 ? 15 : aChoices.getLength();

                Point aListPos;
                bool bDoAlign = false;
                if( nMaxTextWidth + aPixelSize.Width() < aTabSize.Width() - 10 )
                {
                    aListPos      = pHeading->GetPosPixel();
                    aListPos.X() += pHeading->GetSizePixel().Width() + 5;

                    // align heading and list box
                    bDoAlign = true;
                }
                else
                {
                    nCurY += 12;
                    aListPos = pCurParent->LogicToPixel( Point( 15, nCurY ), aFontMapMode );
                }

                pList->SetPosSizePixel( aListPos, aPixelSize );
                pList->Enable( maPListener->isUIOptionEnabled( aPropertyName ) );
                pList->Show();

                maPropertyToWindowMap.insert( std::pair< rtl::OUString, Window* >( aPropertyName, pList ) );
                nCurY += 16;

                if( bDoAlign )
                {
                    Point aPos = pHeading->GetPosPixel();
                    Size aSize = pHeading->GetSizePixel();
                    aPos.Y() += (pList->GetSizePixel().Height() - aSize.Height())/2;
                    pHeading->SetPosSizePixel( aPos, aSize );
                }
            }
        }
        else
        {
            DBG_ERROR( "Unsupported UI option" );
        }
    }
}

void PrintDialog::checkControlDependencies()
{
    if( maJobPage.maCopyCountField.GetValue() > 1 )
        maJobPage.maCollateBox.Enable( TRUE );
    else
        maJobPage.maCollateBox.Enable( FALSE );

    maJobPage.maPagesEdit.Enable( maJobPage.maPagesButton.IsChecked() );
    Image aImg( maJobPage.maCollateBox.IsChecked() ? maJobPage.maCollateImg : maJobPage.maNoCollateImg );
    if( GetSettings().GetStyleSettings().GetFieldColor().IsDark() )
        aImg = maJobPage.maCollateBox.IsChecked() ? maJobPage.maCollateHCImg : maJobPage.maNoCollateHCImg;

    // adjust position and size of image
    maJobPage.maCollateImage.SetSizePixel( aImg.GetSizePixel() );
    Point aPos( maJobPage.maCollateImage.GetPosPixel() );
    aPos.Y() = maJobPage.maCollateBox.GetPosPixel().Y();
    aPos.Y() -= (aImg.GetSizePixel().Height() - maJobPage.maCollateBox.GetSizePixel().Height())/2;
    maJobPage.maCollateImage.SetPosPixel( aPos );
    maJobPage.maCollateImage.SetImage( aImg );

    // enable setup button only for printers that can be setup
    maPrinterPage.maSetupButton.Enable( maPListener->getPrinter()->HasSupport( SUPPORT_SETUPDIALOG ) );
}

void PrintDialog::updatePrinterText()
{
    const QueueInfo* pInfo = Printer::GetQueueInfo( maPrinterPage.maPrinters.GetSelectEntry(), true );
    if( pInfo )
    {
        maPrinterPage.maTypeText.SetText( pInfo->GetDriver() );
        // FIXME: status message
        // maJobPage.maStatusText.SetText();
        maPrinterPage.maLocText.SetText( pInfo->GetLocation() );
        maPrinterPage.maCommentText.SetText( pInfo->GetComment() );
    }
}

static rtl::OUString searchAndReplace( const rtl::OUString& i_rOrig, const char* i_pRepl, sal_Int32 i_nReplLen, sal_Int32 i_nReplacement )
{
    sal_Int32 nPos = i_rOrig.indexOfAsciiL( i_pRepl, i_nReplLen );
    if( nPos != -1 )
    {
        rtl::OUStringBuffer aBuf( i_rOrig.getLength() );
        aBuf.append( i_rOrig.getStr(), nPos );
        aBuf.append( i_nReplacement );
        if( nPos + i_nReplLen < i_rOrig.getLength() )
            aBuf.append( i_rOrig.getStr() + nPos + i_nReplLen );
        return aBuf.makeStringAndClear();
    }
    return i_rOrig;
}

void PrintDialog::setPreviewText( sal_Int32 nSetPage )
{
    rtl::OUString aNewText( searchAndReplace( maPageStr, "%p", 2, nSetPage+1 ) );
    aNewText = searchAndReplace( aNewText, "%n", 2, mnCachedPages );
    maPageText.SetText( aNewText );
}

void PrintDialog::preparePreview()
{
    // page range may have changed depending on options
    sal_Int32 nPages = maPListener->getPageCount();
    mnCachedPages = nPages;

    if( mnCurPage >= nPages )
        mnCurPage = nPages-1;

    setPreviewText( mnCurPage );

    maPageScrollbar.SetRange( Range( 0, nPages-1 ) );
    maPageScrollbar.SetThumbPos( mnCurPage );

    boost::shared_ptr<Printer> aPrt( maPListener->getPrinter() );


    const MapMode aMapMode( MAP_100TH_MM );
    aPrt->Push();
    aPrt->SetMapMode( aMapMode );

    Size aPageSize( aPrt->GetPaperSize() );
    Sequence< PropertyValue > aPageParms( maPListener->getPageParameters( mnCurPage ) );
    for( sal_Int32 nProperty = 0, nPropertyCount = aPageParms.getLength(); nProperty < nPropertyCount; ++nProperty )
    {
        if( aPageParms[ nProperty ].Name.equalsIgnoreAsciiCaseAscii( "PageSize" ) )
        {
            awt::Size aSize;
            aPageParms[ nProperty ].Value >>= aSize;
            aPageSize.Width() = aSize.Width;
            aPageSize.Height() = aSize.Height;
        }
    }

    aPrt->EnableOutput( FALSE );

    GDIMetaFile aMtf;
    aMtf.SetPrefSize( aPageSize );
    aMtf.SetPrefMapMode( aMapMode );
    aMtf.Record( &(*aPrt) );

    maPListener->printPage( mnCurPage );

    aMtf.Stop();
    aMtf.WindStart();
    aPrt->Pop();

    Size aPreviewSize;
    Point aPreviewPos = maPreviewSpace.TopLeft();
    const long nW = maPreviewSpace.GetSize().Width();
    const long nH = maPreviewSpace.GetSize().Height();
    if( aPageSize.Width() > aPageSize.Height() )
    {
        aPreviewSize = Size( nW, nW * aPageSize.Height() / aPageSize.Width() );
        aPreviewPos.Y() += (maPreviewSpace.GetHeight() - aPreviewSize.Height())/2;
    }
    else
    {
        aPreviewSize = Size( nH * aPageSize.Width() / aPageSize.Height(), nH );
        aPreviewPos.X() += (maPreviewSpace.GetWidth() - aPreviewSize.Width())/2;
    }
    maPreviewWindow.SetPosSizePixel( aPreviewPos, aPreviewSize );
    const Size aLogicSize( maPreviewWindow.PixelToLogic( maPreviewWindow.GetSizePixel(), MapMode( MAP_100TH_MM ) ) );
    aMtf.Scale( double(aLogicSize.Width())/double(aPageSize.Width()),
                double(aLogicSize.Height())/double(aPageSize.Height()) );
    maPreviewWindow.setPreview( aMtf );
}

IMPL_LINK( PrintDialog, ScrollHdl, ScrollBar*, pScrBar )
{
    if( pScrBar == &maPageScrollbar )
    {
        sal_Int32 nNewPage = static_cast<sal_Int32>( maPageScrollbar.GetThumbPos() );
        setPreviewText( nNewPage );
    }
    return 0;
}

IMPL_LINK( PrintDialog, ScrollEndHdl, ScrollBar*, pScrBar )
{
    if( pScrBar == &maPageScrollbar )
    {
        sal_Int32 nNewPage = static_cast<sal_Int32>( maPageScrollbar.GetThumbPos() );
        if( nNewPage != mnCurPage )
        {
            mnCurPage = nNewPage;
            preparePreview();
        }
    }
    return 0;
}

IMPL_LINK( PrintDialog, SelectHdl, ListBox*, pBox )
{
    if( pBox == &maPrinterPage.maPrinters )
    {
        // set new printer
        maPListener->setPrinter( boost::shared_ptr<Printer>( new Printer( maPrinterPage.maPrinters.GetSelectEntry() ) ) );
        // update text fields
        updatePrinterText();
    }
    return 0;
}

IMPL_LINK( PrintDialog, ClickHdl, Button*, pButton )
{
    if( pButton == &maPrinterPage.maSetupButton )
    {
        maPListener->getPrinter()->Setup( this );
    }
    checkControlDependencies();
    return 0;
}

IMPL_LINK( PrintDialog, ModifyHdl, Edit*, EMPTYARG )
{
    checkControlDependencies();
    return 0;
}

IMPL_LINK( PrintDialog, UIOptionsChanged, void*, i_pOption )
{
    PropertyValue* pVal = maPListener->getValue( *reinterpret_cast< rtl::OUString* >(i_pOption) );
    if( pVal )
    {
        std::pair< std::multimap< rtl::OUString, Window* >::iterator,
                   std::multimap< rtl::OUString, Window* >::iterator > aWindows =
            maPropertyToWindowMap.equal_range( pVal->Name );
        for( std::multimap< rtl::OUString, Window* >::const_iterator it =
             aWindows.first; it != aWindows.second; ++it )
        {
            it->second->Enable( maPListener->isUIOptionEnabled( pVal->Name ) );
        }
    }
    return 0;
}

PropertyValue* PrintDialog::getValueForWindow( Window* i_pWindow ) const
{
    PropertyValue* pVal = NULL;
    std::map< Window*, rtl::OUString >::const_iterator it = maControlToPropertyMap.find( i_pWindow );
    if( it != maControlToPropertyMap.end() )
    {
        pVal = maPListener->getValue( it->second );
        DBG_ASSERT( pVal, "property value not found" );
    }
    else
    {
        DBG_ERROR( "changed control not in property map" );
    }
    return pVal;
}

IMPL_LINK( PrintDialog, UIOption_CheckHdl, CheckBox*, i_pBox )
{
    PropertyValue* pVal = getValueForWindow( i_pBox );
    if( pVal )
    {
        sal_Bool bVal = i_pBox->IsChecked();
        pVal->Value <<= bVal;

        // update preview and page settings
        preparePreview();
    }
    return 0;
}

IMPL_LINK( PrintDialog, UIOption_RadioHdl, RadioButton*, i_pBtn )
{
    PropertyValue* pVal = getValueForWindow( i_pBtn );
    if( pVal )
    {
        rtl::OUString aVal( i_pBtn->GetText() );;
        pVal->Value <<= aVal;

        // update preview and page settings
        preparePreview();
    }
    return 0;
}

IMPL_LINK( PrintDialog, UIOption_SelectHdl, ListBox*, i_pBox )
{
    PropertyValue* pVal = getValueForWindow( i_pBox );
    if( pVal )
    {
        rtl::OUString aVal( i_pBox->GetSelectEntry() );;
        pVal->Value <<= aVal;

        // update preview and page settings
        preparePreview();
    }
    return 0;
}

