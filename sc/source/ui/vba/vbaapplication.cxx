/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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
#include <stdio.h>


#include<com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include<com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include<ooo/vba/excel/XlCalculation.hpp>
#include <com/sun/star/sheet/XCellRangeReferrer.hpp>
#include <com/sun/star/sheet/XCalculatable.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <ooo/vba/excel/XlMousePointer.hpp>
#include <com/sun/star/sheet/XNamedRanges.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>

#include "vbaapplication.hxx"
#include "vbaworkbooks.hxx"
#include "vbaworkbook.hxx"
#include "vbaworksheets.hxx"
#include "vbarange.hxx"
#include "vbawsfunction.hxx"
#include "vbadialogs.hxx"
#include "vbawindow.hxx"
#include "vbawindows.hxx"
#include "vbaglobals.hxx"
#include "tabvwsh.hxx"
#include "gridwin.hxx"
#include "vbanames.hxx"
#include <vbahelper/vbashape.hxx>
#include "vbatextboxshape.hxx"
#include "vbaassistant.hxx"
#include "sc.hrc"

#include <osl/file.hxx>

#include <sfx2/request.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/app.hxx>

#include <toolkit/awt/vclxwindow.hxx>
#include <toolkit/helper/vclunohelper.hxx>

#include <tools/diagnose_ex.h>

#include <docuno.hxx>

#include <basic/sbx.hxx>
#include <basic/sbstar.hxx>
#include <basic/sbuno.hxx>
#include <basic/sbmeth.hxx>

#include "convuno.hxx"
#include "cellsuno.hxx"
#include "docsh.hxx"
#include <vbahelper/helperdecl.hxx>
#include "excelvbahelper.hxx"


using namespace ::ooo::vba;
using namespace ::com::sun::star;

// Enable our own join detection for Intersection and Union
// should be more efficient than using ScRangeList::Join ( because
// we already are testing the same things )

#define OWN_JOIN 1

// #TODO is this defined somewhere else?
#if ( defined UNX ) || ( defined OS2 ) //unix
#define FILE_PATH_SEPERATOR "/"
#else // windows
#define FILE_PATH_SEPERATOR "\\"
#endif

uno::Any sbxToUnoValue( SbxVariable* pVar );

class ActiveWorkbook : public ScVbaWorkbook
{
protected:
    virtual uno::Reference< frame::XModel > getModel()
    {
        return getCurrentExcelDoc(mxContext);
    }
public:
    ActiveWorkbook( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext) : ScVbaWorkbook(  xParent, xContext ){}
};

ScVbaApplication::ScVbaApplication( const uno::Reference<uno::XComponentContext >& xContext ) :
    ScVbaApplication_BASE( xContext ),
    m_xCalculation( excel::XlCalculation::xlCalculationAutomatic ),
    m_xDisplayAlerts( sal_True)
{
}

ScVbaApplication::~ScVbaApplication()
{
}

SfxObjectShell* ScVbaApplication::GetDocShell( const uno::Reference< frame::XModel >& xModel ) throw (uno::RuntimeException)
{
    return static_cast< SfxObjectShell* >( excel::getDocShell( xModel ) );
}

::rtl::OUString SAL_CALL
ScVbaApplication::getExactName( const ::rtl::OUString& aApproximateName ) throw (uno::RuntimeException)
{
    uno::Reference< beans::XExactName > xWSF( new ScVbaWSFunction( this, mxContext ) );
    return xWSF->getExactName( aApproximateName );
}

uno::Reference< beans::XIntrospectionAccess > SAL_CALL
ScVbaApplication::getIntrospection() throw(css::uno::RuntimeException)
{
    uno::Reference< script::XInvocation > xWSF( new ScVbaWSFunction( this, mxContext ) );
    return xWSF->getIntrospection();
}

uno::Any SAL_CALL
ScVbaApplication::invoke( const ::rtl::OUString& FunctionName, const uno::Sequence< uno::Any >& Params, uno::Sequence< sal_Int16 >& OutParamIndex, uno::Sequence< uno::Any >& OutParam) throw(lang::IllegalArgumentException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
    /*  When calling the functions directly at the Application object, no runtime
        errors are thrown, but the error is inserted into the return value. */
    uno::Any aAny;
    try
    {
        uno::Reference< script::XInvocation > xWSF( new ScVbaWSFunction( this, mxContext ) );
        aAny = xWSF->invoke( FunctionName, Params, OutParamIndex, OutParam );
    }
    catch( uno::Exception& )
    {
        aAny <<= script::BasicErrorException( ::rtl::OUString(), uno::Reference< uno::XInterface >(), 1000, ::rtl::OUString() );
    }
    return aAny;
}

void SAL_CALL
ScVbaApplication::setValue( const ::rtl::OUString& PropertyName, const uno::Any& Value ) throw(beans::UnknownPropertyException, script::CannotConvertException, reflection::InvocationTargetException, uno::RuntimeException)
{
    uno::Reference< script::XInvocation > xWSF( new ScVbaWSFunction( this, mxContext ) );
    xWSF->setValue( PropertyName, Value );
}

uno::Any SAL_CALL
ScVbaApplication::getValue( const ::rtl::OUString& PropertyName ) throw(beans::UnknownPropertyException, uno::RuntimeException)
{
    uno::Reference< script::XInvocation > xWSF( new ScVbaWSFunction( this, mxContext ) );
    return xWSF->getValue( PropertyName );
}

sal_Bool SAL_CALL
ScVbaApplication::hasMethod( const ::rtl::OUString& Name ) throw(uno::RuntimeException)
{
    uno::Reference< script::XInvocation > xWSF( new ScVbaWSFunction( this, mxContext ) );
    return xWSF->hasMethod( Name );
}

sal_Bool SAL_CALL
ScVbaApplication::hasProperty( const ::rtl::OUString& Name ) throw(uno::RuntimeException)
{
    uno::Reference< script::XInvocation > xWSF( new ScVbaWSFunction( this, mxContext ) );
    return xWSF->hasProperty( Name );
}

uno::Reference< excel::XWorkbook >
ScVbaApplication::getActiveWorkbook() throw (uno::RuntimeException)
{
    return new ActiveWorkbook( this, mxContext );
}

uno::Reference< excel::XWorkbook > SAL_CALL
ScVbaApplication::getThisWorkbook() throw (uno::RuntimeException)
{
    uno::Reference< frame::XModel > xModel = getThisExcelDoc(mxContext);
    if( !xModel.is() )
        return uno::Reference< excel::XWorkbook >();

    ScVbaWorkbook *pWb = new ScVbaWorkbook( this, mxContext, xModel );
    return uno::Reference< excel::XWorkbook > (pWb);
}

uno::Reference< XAssistant > SAL_CALL
ScVbaApplication::getAssistant() throw (uno::RuntimeException)
{
    return uno::Reference< XAssistant >( new ScVbaAssistant( this, mxContext ) );
}

uno::Any SAL_CALL
ScVbaApplication::getSelection() throw (uno::RuntimeException)
{
    OSL_TRACE("** ScVbaApplication::getSelection() ** ");
    uno::Reference< frame::XModel > xModel( getCurrentDocument() );
    uno::Reference< lang::XServiceInfo > xServiceInfo( xModel->getCurrentSelection(), uno::UNO_QUERY_THROW );
    rtl::OUString sImpementaionName = xServiceInfo->getImplementationName();
    if( sImpementaionName.equalsIgnoreAsciiCaseAscii("com.sun.star.drawing.SvxShapeCollection") )
    {
        uno::Reference< drawing::XShapes > xShapes( xModel->getCurrentSelection(), uno::UNO_QUERY_THROW );
        uno::Reference< container::XIndexAccess > xIndexAccess( xShapes, uno::UNO_QUERY_THROW );
        uno::Reference< drawing::XShape > xShape( xIndexAccess->getByIndex(0), uno::UNO_QUERY_THROW );
    // if ScVbaShape::getType( xShape ) == office::MsoShapeType::msoAutoShape
    // and the uno object implements the com.sun.star.drawing.Text service
    // return a textboxshape object
    if ( ScVbaShape::getType( xShape ) == office::MsoShapeType::msoAutoShape )
    {
        uno::Reference< lang::XServiceInfo > xShapeServiceInfo( xShape, uno::UNO_QUERY_THROW );
        if ( xShapeServiceInfo->supportsService( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.Text" ) ) )  )
        {
                return uno::makeAny( uno::Reference< msforms::XTextBoxShape >(new ScVbaTextBoxShape( mxContext, xShape, xShapes, xModel ) ) );
        }
    }
        return uno::makeAny( uno::Reference< msforms::XShape >(new ScVbaShape( this, mxContext, xShape, xShapes, xModel, ScVbaShape::getType( xShape ) ) ) );
    }
    else if( xServiceInfo->supportsService( rtl::OUString::createFromAscii("com.sun.star.sheet.SheetCellRange")) ||
             xServiceInfo->supportsService( rtl::OUString::createFromAscii("com.sun.star.sheet.SheetCellRanges")))
    {
        uno::Reference< table::XCellRange > xRange( getCurrentDocument()->getCurrentSelection(), ::uno::UNO_QUERY);
        if ( !xRange.is() )
        {
            uno::Reference< sheet::XSheetCellRangeContainer > xRanges( getCurrentDocument()->getCurrentSelection(), ::uno::UNO_QUERY);
            if ( xRanges.is() )
                return uno::makeAny( uno::Reference< excel::XRange >( new ScVbaRange( this, mxContext, xRanges ) ) );

        }
        return uno::makeAny( uno::Reference< excel::XRange >(new ScVbaRange( this, mxContext, xRange ) ) );
    }
    else
    {
        throw uno::RuntimeException( sImpementaionName + rtl::OUString::createFromAscii(" not suported"), uno::Reference< uno::XInterface >() );
    }
}

uno::Reference< excel::XRange >
ScVbaApplication::getActiveCell() throw (uno::RuntimeException )
{
    uno::Reference< sheet::XSpreadsheetView > xView( getCurrentDocument()->getCurrentController(), uno::UNO_QUERY_THROW );
    uno::Reference< table::XCellRange > xRange( xView->getActiveSheet(), ::uno::UNO_QUERY_THROW);
    ScTabViewShell* pViewShell = excel::getCurrentBestViewShell(mxContext);
    if ( !pViewShell )
        throw uno::RuntimeException( rtl::OUString::createFromAscii("No ViewShell available"), uno::Reference< uno::XInterface >() );
    ScViewData* pTabView = pViewShell->GetViewData();
    if ( !pTabView )
        throw uno::RuntimeException( rtl::OUString::createFromAscii("No ViewData available"), uno::Reference< uno::XInterface >() );

    sal_Int32 nCursorX = pTabView->GetCurX();
    sal_Int32 nCursorY = pTabView->GetCurY();

    return  new ScVbaRange( this, mxContext, xRange->getCellRangeByPosition( nCursorX, nCursorY, nCursorX, nCursorY ) );
}

uno::Any SAL_CALL
ScVbaApplication::Workbooks( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xWorkBooks( new ScVbaWorkbooks( this, mxContext ) );
    if (  aIndex.getValueTypeClass() == uno::TypeClass_VOID )
    {
        // void then somebody did Workbooks.something in vba
        return uno::Any( xWorkBooks );
    }

    return uno::Any ( xWorkBooks->Item( aIndex, uno::Any() ) );
}

uno::Any SAL_CALL
ScVbaApplication::Worksheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
    uno::Reference< excel::XWorkbook > xWorkbook( getActiveWorkbook(), uno::UNO_QUERY );
        uno::Any result;
    if ( xWorkbook.is() )
        result  = xWorkbook->Worksheets( aIndex );

    else
        // Fixme - check if this is reasonable/desired behavior
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
            "No ActiveWorkBook available" ), uno::Reference< uno::XInterface >() );

    return result;
}

uno::Any SAL_CALL
ScVbaApplication::WorksheetFunction( ) throw (::com::sun::star::uno::RuntimeException)
{
    return uno::makeAny( uno::Reference< script::XInvocation >( new ScVbaWSFunction( this, mxContext ) ) );
}

uno::Any SAL_CALL
ScVbaApplication::Evaluate( const ::rtl::OUString& Name ) throw (uno::RuntimeException)
{
    // #TODO Evaluate allows other things to be evaluated, e.g. functions
    // I think ( like SIN(3) etc. ) need to investigate that
    // named Ranges also? e.g. [MyRange] if so need a list of named ranges
    uno::Any aVoid;
    return uno::Any( getActiveWorkbook()->getActiveSheet()->Range( uno::Any( Name ), aVoid ) );
}

uno::Any
ScVbaApplication::Dialogs( const uno::Any &aIndex ) throw (uno::RuntimeException)
{
    uno::Reference< excel::XDialogs > xDialogs( new ScVbaDialogs( uno::Reference< XHelperInterface >( this ), mxContext, getCurrentDocument() ) );
    if( !aIndex.hasValue() )
        return uno::Any( xDialogs );
    return uno::Any( xDialogs->Item( aIndex ) );
}

uno::Reference< excel::XWindow > SAL_CALL
ScVbaApplication::getActiveWindow() throw (uno::RuntimeException)
{
    // #FIXME sofar can't determine Parent
    uno::Reference< frame::XModel > xModel = getCurrentDocument();
    ScVbaWindow* pWin = new ScVbaWindow( uno::Reference< XHelperInterface >(), mxContext, xModel );
    uno::Reference< excel::XWindow > xWin( pWin );
    return xWin;
}

uno::Any SAL_CALL
ScVbaApplication::getCutCopyMode() throw (uno::RuntimeException)
{
    //# FIXME TODO, implementation
    uno::Any result;
    result <<= sal_False;
    return result;
}

void SAL_CALL
ScVbaApplication::setCutCopyMode( const uno::Any& /*_cutcopymode*/ ) throw (uno::RuntimeException)
{
    //# FIXME TODO, implementation
}

uno::Any SAL_CALL
ScVbaApplication::getStatusBar() throw (uno::RuntimeException)
{
    return uno::makeAny( !getDisplayStatusBar() );
}

void SAL_CALL
ScVbaApplication::setStatusBar( const uno::Any& _statusbar ) throw (uno::RuntimeException)
{
    rtl::OUString sText;
    sal_Bool bDefault = sal_False;
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< task::XStatusIndicatorSupplier > xStatusIndicatorSupplier( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    uno::Reference< task::XStatusIndicator > xStatusIndicator( xStatusIndicatorSupplier->getStatusIndicator(), uno::UNO_QUERY_THROW );
    if( _statusbar >>= sText )
    {
        setDisplayStatusBar( sal_True );
        xStatusIndicator->start( sText, 100 );
        //xStatusIndicator->setText( sText );
    }
    else if( _statusbar >>= bDefault )
    {
        if( bDefault == sal_False )
        {
            xStatusIndicator->end();
            setDisplayStatusBar( sal_True );
        }
    }
    else
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "Invalid prarameter. It should be a string or False" ),
            uno::Reference< uno::XInterface >() );
}

::sal_Int32 SAL_CALL
ScVbaApplication::getCalculation() throw (uno::RuntimeException)
{
    uno::Reference<sheet::XCalculatable> xCalc(getCurrentDocument(), uno::UNO_QUERY_THROW);
    if(xCalc->isAutomaticCalculationEnabled())
        return excel::XlCalculation::xlCalculationAutomatic;
    else
        return excel::XlCalculation::xlCalculationManual;
}

void SAL_CALL
ScVbaApplication::setCalculation( ::sal_Int32 _calculation ) throw (uno::RuntimeException)
{
    uno::Reference< sheet::XCalculatable > xCalc(getCurrentDocument(), uno::UNO_QUERY_THROW);
    switch(_calculation)
    {
        case excel::XlCalculation::xlCalculationManual:
            xCalc->enableAutomaticCalculation(sal_False);
            break;
        case excel::XlCalculation::xlCalculationAutomatic:
        case excel::XlCalculation::xlCalculationSemiautomatic:
            xCalc->enableAutomaticCalculation(sal_True);
            break;
    }
}

uno::Any SAL_CALL
ScVbaApplication::Windows( const uno::Any& aIndex  ) throw (uno::RuntimeException)
{
    uno::Reference< excel::XWindows >  xWindows( new ScVbaWindows( this, mxContext ) );
    if ( aIndex.getValueTypeClass() == uno::TypeClass_VOID )
        return uno::Any( xWindows );
    return uno::Any( xWindows->Item( aIndex, uno::Any() ) );
}
void SAL_CALL
ScVbaApplication::wait( double time ) throw (uno::RuntimeException)
{
    StarBASIC* pBasic = SFX_APP()->GetBasic();
    SFX_APP()->EnterBasicCall();
    SbxArrayRef aArgs = new SbxArray;
    SbxVariableRef aRef = new SbxVariable;
    aRef->PutDouble( time );
    aArgs->Put(  aRef, 1 );
    SbMethod* pMeth = (SbMethod*)pBasic->GetRtl()->Find( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("WaitUntil") ), SbxCLASS_METHOD );

    if ( pMeth )
    {
        pMeth->SetParameters( aArgs );
        SbxVariableRef refTemp = pMeth;
        // forces a broadcast
        SbxVariableRef pNew = new  SbxMethod( *((SbxMethod*)pMeth));
    }
    SFX_APP()->LeaveBasicCall();

}

uno::Any SAL_CALL
ScVbaApplication::Range( const uno::Any& Cell1, const uno::Any& Cell2 ) throw (uno::RuntimeException)
{
    uno::Reference< excel::XRange > xVbRange = ScVbaRange::ApplicationRange( mxContext, Cell1, Cell2 );
    return uno::makeAny( xVbRange );
}

uno::Any SAL_CALL
ScVbaApplication::Names( const css::uno::Any& aIndex ) throw ( uno::RuntimeException )
{
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xPropertySet( xModel, uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XNamedRanges > xNamedRanges( xPropertySet->getPropertyValue( rtl::OUString::createFromAscii("NamedRanges")) , uno::UNO_QUERY_THROW );
    css::uno::Reference< excel::XNames > xNames ( new ScVbaNames( this , mxContext , xNamedRanges , xModel ) );
    if (  aIndex.getValueTypeClass() == uno::TypeClass_VOID )
    {
        return uno::Any( xNames );
}
    return uno::Any( xNames->Item( aIndex, uno::Any() ) );
}


uno::Reference< excel::XWorksheet > SAL_CALL
ScVbaApplication::getActiveSheet() throw (uno::RuntimeException)
{
    uno::Reference< excel::XWorksheet > result;
    uno::Reference< excel::XWorkbook > xWorkbook( getActiveWorkbook(), uno::UNO_QUERY );
    if ( xWorkbook.is() )
    {
        uno::Reference< excel::XWorksheet > xWorksheet(
            xWorkbook->getActiveSheet(), uno::UNO_QUERY );
        if ( xWorksheet.is() )
        {
            result = xWorksheet;
        }
    }

    if ( !result.is() )
    {
        // Fixme - check if this is reasonable/desired behavior
        throw uno::RuntimeException( rtl::OUString::createFromAscii(
            "No activeSheet available" ), uno::Reference< uno::XInterface >() );
    }
    return result;

}

/*******************************************************************************
 *  In msdn:
 *  Reference   Optional Variant. The destination. Can be a Range
 *  object, a string that contains a cell reference in R1C1-style notation,
 *  or a string that contains a Visual Basic procedure name.
 *  Scroll   Optional Variant. True to scrol, False to not scroll through
 *  the window. The default is False.
 *  Parser is split to three parts, Range, R1C1 string and procedure name.
 *  by test excel, it seems Scroll no effect. ???
*******************************************************************************/
void SAL_CALL
ScVbaApplication::GoTo( const uno::Any& Reference, const uno::Any& Scroll ) throw (uno::RuntimeException)
{
    //test Scroll is a boolean
    sal_Bool bScroll = sal_False;
    //R1C1-style string or a string of procedure name.

    if( Scroll.hasValue() )
    {
        sal_Bool aScroll = sal_False;
        if( Scroll >>= aScroll )
        {
            bScroll = aScroll;
        }
        else
            throw uno::RuntimeException( rtl::OUString::createFromAscii( "sencond parameter should be boolean" ),
                    uno::Reference< uno::XInterface >() );
    }

    rtl::OUString sRangeName;
    if( Reference >>= sRangeName )
    {
        uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
        uno::Reference< sheet::XSpreadsheetView > xSpreadsheet(
                xModel->getCurrentController(), uno::UNO_QUERY_THROW );
        uno::Reference< sheet::XSpreadsheet > xDoc = xSpreadsheet->getActiveSheet();

        ScTabViewShell* pShell = excel::getCurrentBestViewShell( mxContext );
        ScGridWindow* gridWindow = (ScGridWindow*)pShell->GetWindow();
        try
        {
            // FIXME: pass proper Worksheet parent
            uno::Reference< excel::XRange > xVbaSheetRange = ScVbaRange::getRangeObjectForName(
                uno::Reference< XHelperInterface >(), mxContext, sRangeName,
                excel::getDocShell( xModel ), formula::FormulaGrammar::CONV_XL_R1C1 );

            if( bScroll )
            {
                xVbaSheetRange->Select();
                uno::Reference< excel::XWindow >  xWindow = getActiveWindow();
                ScSplitPos eWhich = pShell->GetViewData()->GetActivePart();
                sal_Int32 nValueX = pShell->GetViewData()->GetPosX(WhichH(eWhich));
                sal_Int32 nValueY = pShell->GetViewData()->GetPosY(WhichV(eWhich));
                xWindow->SmallScroll( uno::makeAny( (sal_Int16)(xVbaSheetRange->getRow() - 1) ),
                         uno::makeAny( (sal_Int16)nValueY ),
                         uno::makeAny( (sal_Int16)(xVbaSheetRange->getColumn() - 1)  ),
                         uno::makeAny( (sal_Int16)nValueX ) );
                gridWindow->GrabFocus();
            }
            else
            {
                xVbaSheetRange->Select();
                gridWindow->GrabFocus();
            }
        }
        catch( uno::RuntimeException )
        {
            //maybe this should be a procedure name
            //TODO for procedure name
            //browse::XBrowseNodeFactory is a singlton. OUString::createFromAscii( "/singletons/com.sun.star.script.browse.theBrowseNodeFactory")
            //and the createView( browse::BrowseNodeFactoryViewTypes::MACROSELECTOR ) to get a root browse::XBrowseNode.
            //for query XInvocation interface.
            //but how to directly get the XInvocation?
            throw uno::RuntimeException( rtl::OUString::createFromAscii( "invalid reference for range name, it should be procedure name" ),
                    uno::Reference< uno::XInterface >() );
        }
        return;
    }
    uno::Reference< excel::XRange > xRange;
    if( Reference >>= xRange )
    {
        uno::Reference< excel::XRange > xVbaRange( Reference, uno::UNO_QUERY );
        ScTabViewShell* pShell = excel::getCurrentBestViewShell( mxContext );
        ScGridWindow* gridWindow = (ScGridWindow*)pShell->GetWindow();
        if ( xVbaRange.is() )
        {
            //TODO bScroll should be using, In this time, it doesenot have effection
            if( bScroll )
            {
                xVbaRange->Select();
                uno::Reference< excel::XWindow >  xWindow = getActiveWindow();
                ScSplitPos eWhich = pShell->GetViewData()->GetActivePart();
                sal_Int32 nValueX = pShell->GetViewData()->GetPosX(WhichH(eWhich));
                sal_Int32 nValueY = pShell->GetViewData()->GetPosY(WhichV(eWhich));
                xWindow->SmallScroll( uno::makeAny( (sal_Int16)(xVbaRange->getRow() - 1) ),
                         uno::makeAny( (sal_Int16)nValueY ),
                         uno::makeAny( (sal_Int16)(xVbaRange->getColumn() - 1)  ),
                         uno::makeAny( (sal_Int16)nValueX ) );
                gridWindow->GrabFocus();
            }
            else
            {
                xVbaRange->Select();
                gridWindow->GrabFocus();
            }
        }
        return;
    }
    throw uno::RuntimeException( rtl::OUString::createFromAscii( "invalid reference or name" ),
            uno::Reference< uno::XInterface >() );
}

sal_Int32 SAL_CALL
ScVbaApplication::getCursor() throw (uno::RuntimeException)
{
    sal_Int32 nPointerStyle =  getPointerStyle(getCurrentDocument());

    switch( nPointerStyle )
    {
        case POINTER_ARROW:
            return excel::XlMousePointer::xlNorthwestArrow;
        case POINTER_NULL:
            return excel::XlMousePointer::xlDefault;
        case POINTER_WAIT:
            return excel::XlMousePointer::xlWait;
        case POINTER_TEXT:
            return excel::XlMousePointer::xlIBeam;
        default:
            return excel::XlMousePointer::xlDefault;
    }
}

void SAL_CALL
ScVbaApplication::setCursor( sal_Int32 _cursor ) throw (uno::RuntimeException)
{
    try
    {
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
        switch( _cursor )
        {
            case excel::XlMousePointer::xlNorthwestArrow:
            {
                const Pointer& rPointer( POINTER_ARROW );
                setCursorHelper( xModel, rPointer, sal_False );
                break;
            }
            case excel::XlMousePointer::xlWait:
            case excel::XlMousePointer::xlIBeam:
            {
                const Pointer& rPointer( static_cast< PointerStyle >( _cursor ) );
                //It will set the edit window, toobar and statusbar's mouse pointer.
                setCursorHelper( xModel, rPointer, sal_True );
                break;
            }
            case excel::XlMousePointer::xlDefault:
            {
                const Pointer& rPointer( POINTER_NULL );
                setCursorHelper( xModel, rPointer, sal_False );
                break;
            }
            default:
                throw uno::RuntimeException( rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM("Unknown value for Cursor pointer")), uno::Reference< uno::XInterface >() );
                // TODO: isn't this a flaw in the API? It should be allowed to throw an
                // IllegalArgumentException, or so
        }
    }
    catch( const uno::Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
}

// #TODO perhaps we should switch the return type depending of the filter
// type, e.g. return Calc for Calc and Excel if its an imported doc
rtl::OUString SAL_CALL
ScVbaApplication::getName() throw (uno::RuntimeException)
{
    static rtl::OUString appName( RTL_CONSTASCII_USTRINGPARAM("Microsoft Excel" ) );
    return appName;
}

// #TODO #FIXME get/setDisplayAlerts are just stub impl
// here just the status of the switch is set
// the function that throws an error message needs to
// evaluate this switch in order to know whether it has to disable the
// error message thrown by OpenOffice

void SAL_CALL
ScVbaApplication::setDisplayAlerts(sal_Bool displayAlerts) throw (uno::RuntimeException)
{
    m_xDisplayAlerts = displayAlerts;
}

sal_Bool SAL_CALL
ScVbaApplication::getDisplayAlerts() throw (uno::RuntimeException)
{
    return m_xDisplayAlerts;
}
void SAL_CALL
ScVbaApplication::Calculate() throw(  script::BasicErrorException , uno::RuntimeException )
{
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XCalculatable > xCalculatable( getCurrentDocument(), uno::UNO_QUERY_THROW );
    xCalculatable->calculateAll();
}

uno::Reference< beans::XPropertySet > lcl_getPathSettingsService( const uno::Reference< uno::XComponentContext >& xContext ) throw ( uno::RuntimeException )
{
    static uno::Reference< beans::XPropertySet >  xPathSettings;
    if ( !xPathSettings.is() )
    {
        uno::Reference< lang::XMultiComponentFactory > xSMgr( xContext->getServiceManager(), uno::UNO_QUERY_THROW );
        xPathSettings.set( xSMgr->createInstanceWithContext(::rtl::OUString::createFromAscii("com.sun.star.util.PathSettings"), xContext), uno::UNO_QUERY_THROW );
    }
    return xPathSettings;
}
rtl::OUString ScVbaApplication::getOfficePath( const rtl::OUString& _sPathType ) throw ( uno::RuntimeException )
{
    rtl::OUString sRetPath;
    uno::Reference< beans::XPropertySet > xProps = lcl_getPathSettingsService( mxContext );
    try
    {
        rtl::OUString sUrl;
         xProps->getPropertyValue( _sPathType ) >>= sUrl;

        // if its a list of paths then use the last one
        sal_Int32 nIndex =  sUrl.lastIndexOf( ';' ) ;
        if ( nIndex > 0 )
            sUrl = sUrl.copy( nIndex + 1 );
        ::osl::File::getSystemPathFromFileURL( sUrl, sRetPath );
    }
    catch (uno::Exception&)
    {
        DebugHelper::exception(SbERR_METHOD_FAILED, rtl::OUString());
    }
    return sRetPath;
}
void SAL_CALL
ScVbaApplication::setDefaultFilePath( const ::rtl::OUString& DefaultFilePath ) throw (script::BasicErrorException, uno::RuntimeException)
{
    uno::Reference< beans::XPropertySet > xProps = lcl_getPathSettingsService( mxContext );
    rtl::OUString aURL;
    osl::FileBase::getFileURLFromSystemPath( DefaultFilePath, aURL );
    xProps->setPropertyValue(  rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Work")), uno::makeAny( aURL ) );


}

::rtl::OUString SAL_CALL
ScVbaApplication::getDefaultFilePath(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
    return getOfficePath( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Work")));
}

::rtl::OUString SAL_CALL
ScVbaApplication::LibraryPath(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
    return getOfficePath( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Basic")));
}

::rtl::OUString SAL_CALL
ScVbaApplication::TemplatesPath(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
    return getOfficePath( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Template")));
}

::rtl::OUString SAL_CALL
ScVbaApplication::PathSeparator(  ) throw (script::BasicErrorException, uno::RuntimeException)
{
    static rtl::OUString sPathSep( RTL_CONSTASCII_USTRINGPARAM( FILE_PATH_SEPERATOR ) );
    return sPathSep;
}

typedef std::list< ScRange > Ranges;
typedef std::list< ScRangeList > RangesList;

void lcl_addRangesToVec( RangesList& vRanges, const uno::Any& aArg ) throw ( script::BasicErrorException, uno::RuntimeException )
{
    ScRangeList theRanges;
    uno::Reference< excel::XRange > xRange( aArg, uno::UNO_QUERY_THROW );
    uno::Reference< XCollection > xCol( xRange->Areas( uno::Any() ), uno::UNO_QUERY_THROW );
    sal_Int32 nCount = xCol->getCount();
    for( sal_Int32 i = 1; i <= nCount; ++i )
    {
        uno::Reference< excel::XRange > xAreaRange( xCol->Item( uno::makeAny( sal_Int32(i) ), uno::Any() ), uno::UNO_QUERY_THROW );
        uno::Reference< sheet::XCellRangeAddressable > xAddressable( xAreaRange->getCellRange(), uno::UNO_QUERY_THROW );
        table::CellRangeAddress addr = xAddressable->getRangeAddress();
        ScRange refRange;
        ScUnoConversion::FillScRange( refRange, addr );
        theRanges.Append( refRange );
    }
    vRanges.push_back( theRanges );
}

void lcl_addRangeToVec( Ranges& vRanges, const uno::Any& aArg ) throw ( script::BasicErrorException, uno::RuntimeException )
{
    uno::Reference< excel::XRange > xRange( aArg, uno::UNO_QUERY_THROW );
    uno::Reference< XCollection > xCol( xRange->Areas( uno::Any() ), uno::UNO_QUERY_THROW );
    sal_Int32 nCount = xCol->getCount();
    for( sal_Int32 i = 1; i <= nCount; ++i )
    {
        uno::Reference< excel::XRange > xAreaRange( xCol->Item( uno::makeAny( sal_Int32(i) ), uno::Any() ), uno::UNO_QUERY_THROW );
        uno::Reference< sheet::XCellRangeAddressable > xAddressable( xAreaRange->getCellRange(), uno::UNO_QUERY_THROW );
        table::CellRangeAddress addr = xAddressable->getRangeAddress();
        ScRange refRange;
        ScUnoConversion::FillScRange( refRange, addr );
        vRanges.push_back( refRange );
    }
}

bool lcl_canJoin( ScRange& r1, ScRange& r2 )
{
    bool bCanJoin = false;
    SCCOL startEndColDiff = r2.aStart.Col() - r1.aEnd.Col();
    SCROW startEndRowDiff = r2.aStart.Row() - r1.aEnd.Row();
    SCCOL startColDiff = r2.aStart.Col() - r1.aStart.Col();
    SCCOL endColDiff = r2.aEnd.Col() - r1.aEnd.Col();
    SCROW startRowDiff =  r2.aStart.Row() - r1.aStart.Row();
    SCROW endRowDiff = r2.aEnd.Row() - r1.aEnd.Row();
    if ( ( startRowDiff == endRowDiff ) && startRowDiff == 0 && startColDiff >=0 && endColDiff > 0 && ( startEndColDiff <= 1 && startEndColDiff >= -r1.aEnd.Col() ) )
        bCanJoin = true;
    else if ( ( startColDiff == endColDiff ) && startColDiff == 0 && startRowDiff >= 0 && endRowDiff > 0 && ( startEndRowDiff <= 1 && startEndRowDiff  >= -r1.aEnd.Row() ) )
        bCanJoin = true;
#ifdef DEBUG
    String sr1;
    String sr2;
    r1.Format( sr1, SCA_VALID ) ;
    r2.Format( sr2, SCA_VALID ) ;
    OSL_TRACE(" canJoin address %s with %s %s ( startRowDiff(%d), endRowDiff(%d), startColDiff(%d) endColDiff(%d) startEndRowDiff(%d), startEndColDiff(%d) ",
        rtl::OUStringToOString( sr1, RTL_TEXTENCODING_UTF8 ).getStr(),
        rtl::OUStringToOString( sr2, RTL_TEXTENCODING_UTF8 ).getStr(), bCanJoin ? "true" : "false", startRowDiff, endRowDiff, startColDiff, endColDiff, startEndRowDiff, startEndColDiff );
#endif
    return bCanJoin;
}
// strips out ranges that contain other ranges, also
// if the borders of the intersecting ranges are alligned
// then the the range is extended to the larger
// e.g. Range("A4:D10"), Range("B4:E10") would be combined
// to Range("A4:E10")
void lcl_strip_containedRanges( Ranges& vRanges )
{
    // get rid of ranges that are surrounded by other ranges
    Ranges::iterator it_outer = vRanges.begin();
    while( it_outer != vRanges.end() )
    {
        bool it_outer_erased = false;   // true = it_outer erased from vRanges
        Ranges::iterator it_inner = vRanges.begin();
        /*  Exit the inner loop if outer iterator has been erased in its last
            iteration (this means it has been joined to last it_inner, or that
            the it_inner contains it completely). The inner loop will restart
            with next element of the outer loop, and all elements (from the
            beginning of the list) will be checked against that new element. */
        while( !it_outer_erased && (it_inner != vRanges.end()) )
        {
            bool it_inner_erased = false;   // true = it_inner erased from vRanges
            if ( it_outer != it_inner )
            {
#ifdef DEBUG
                String r1;
                String r2;
                it_outer->Format( r1, SCA_VALID ) ;
                it_inner->Format( r2, SCA_VALID ) ;
                OSL_TRACE( "try strip/join address %s with %s ",
                    rtl::OUStringToOString( r1, RTL_TEXTENCODING_UTF8 ).getStr(),
                    rtl::OUStringToOString( r2, RTL_TEXTENCODING_UTF8 ).getStr() );
#endif
                if ( it_outer->In( *it_inner ) )
                {
                    it_inner = vRanges.erase( it_inner );
                    it_inner_erased = true;
                }
                else if ( it_inner->In( *it_outer ) )
                {
                    it_outer = vRanges.erase( it_outer );
                    it_outer_erased = true;
                }
#ifndef OWN_JOIN
                else if ( (*it_inner).aStart.Row() == (*it_outer).aStart.Row()
                        && (*it_inner).aEnd.Row() == (*it_outer).aEnd.Row() )
                {
                    it_outer->ExtendTo( *it_inner );
                    it_inner = vRanges.erase( it_inner );
                    it_inner_erased = true;
                }
#else
                else if ( lcl_canJoin( *it_outer, *it_inner ) )
                {
                    it_outer->ExtendTo( *it_inner );
                    it_inner = vRanges.erase( it_inner );
                    it_inner_erased = true;
                }
                else if ( lcl_canJoin( *it_inner, *it_outer) )
                {
                    it_inner->ExtendTo( *it_outer );
                    it_outer = vRanges.erase( it_outer );
                    it_outer_erased = true;
                }
#endif
            }
            /*  If it_inner has not been erased from vRanges, continue inner
                loop with next element. Otherwise, it_inner already points to
                the next element (return value of list::erase()). */
            if( !it_inner_erased )
                ++it_inner;
        }
        /*  If it_outer has not been erased from vRanges, continue outer loop
            with next element. Otherwise, it_outer already points to the next
            element (return value of list::erase()). */
        if( !it_outer_erased )
            ++it_outer;
    }

}

Ranges
lcl_intersectionImpl( ScRangeList& rl1, ScRangeList& rl2 )
{
    Ranges intersections;
    for ( USHORT x = 0 ; x < rl1.Count(); ++x )
    {
        for ( USHORT y = 0 ; y < rl2.Count(); ++y )
        {
#ifdef DEBUG
            String r1;
            String r2;
            rl1.GetObject( x )->Format( r1, SCA_VALID ) ;
            rl2.GetObject( y )->Format( r2, SCA_VALID ) ;
            OSL_TRACE( "comparing address %s with %s ",
                rtl::OUStringToOString( r1, RTL_TEXTENCODING_UTF8 ).getStr(),
                rtl::OUStringToOString( r2, RTL_TEXTENCODING_UTF8 ).getStr() );
#endif
            if( rl1.GetObject( x )->Intersects( *rl2.GetObject( y ) ) )
            {
                ScRange aIntersection = ScRange( Max( rl1.GetObject( x )->aStart.Col(), rl2.GetObject( y )->aStart.Col() ),
                    Max( rl1.GetObject( x )->aStart.Row(), rl2.GetObject( y )->aStart.Row() ),
                    Max( rl1.GetObject( x )->aStart.Tab(), rl2.GetObject( y )->aStart.Tab() ),
                    Min( rl1.GetObject( x )->aEnd.Col(), rl2.GetObject( y )->aEnd.Col() ),
                    Min( rl1.GetObject( x )->aEnd.Row(), rl2.GetObject( y )->aEnd.Row() ),
                        Min( rl1.GetObject( x )->aEnd.Tab(), rl2.GetObject( y )->aEnd.Tab() ) );
                intersections.push_back( aIntersection );
            }
        }
    }
    lcl_strip_containedRanges( intersections );
    return intersections;
}

// Intersection of a set of ranges ( where each range is represented by a ScRangeList e.g.
// any range can be a multi-area range )
// An intersection is performed between each range in the set of ranges.
// The resulting set of intersections is then processed to strip out any
// intersections that contain other intersections ( and also ranges that directly line up
// are joined ) ( see lcl_strip_containedRanges )
RangesList lcl_intersections( RangesList& vRanges )
{
    RangesList intersections;
    RangesList::iterator it = vRanges.begin();
    while( it != vRanges.end() )
    {
        Ranges intermediateList;
        for( RangesList::iterator it_inner = vRanges.begin(); it_inner != vRanges.end(); ++it_inner )
        {
            if ( it != it_inner )
            {
                Ranges ranges = lcl_intersectionImpl( *it, *it_inner );
                for ( Ranges::iterator range_it = ranges.begin(); range_it != ranges.end(); ++range_it )
                    intermediateList.push_back( *range_it );
            }
        }
        it = vRanges.erase( it ); // remove it so we don't include it in the next pass.
        // 'it' is removed uncontidionally from vRanges, so the while loop will terminate

        ScRangeList argIntersect;
        lcl_strip_containedRanges( intermediateList );

        for( Ranges::iterator it_inter = intermediateList.begin(); it_inter != intermediateList.end(); ++it_inter )
#ifndef OWN_JOIN
            argIntersect.Join( *it_inter );
#else
            argIntersect.Append( *it_inter );
#endif

        intersections.push_back( argIntersect );
    }
    return intersections;
}

uno::Reference< excel::XRange > SAL_CALL
ScVbaApplication::Intersect( const uno::Reference< excel::XRange >& Arg1, const uno::Reference< excel::XRange >& Arg2, const uno::Any& Arg3, const uno::Any& Arg4, const uno::Any& Arg5, const uno::Any& Arg6, const uno::Any& Arg7, const uno::Any& Arg8, const uno::Any& Arg9, const uno::Any& Arg10, const uno::Any& Arg11, const uno::Any& Arg12, const uno::Any& Arg13, const uno::Any& Arg14, const uno::Any& Arg15, const uno::Any& Arg16, const uno::Any& Arg17, const uno::Any& Arg18, const uno::Any& Arg19, const uno::Any& Arg20, const uno::Any& Arg21, const uno::Any& Arg22, const uno::Any& Arg23, const uno::Any& Arg24, const uno::Any& Arg25, const uno::Any& Arg26, const uno::Any& Arg27, const uno::Any& Arg28, const uno::Any& Arg29, const uno::Any& Arg30 ) throw (script::BasicErrorException, uno::RuntimeException)
{
    if ( !Arg1.is() || !Arg2.is() )
        DebugHelper::exception(SbERR_BAD_PARAMETER, rtl::OUString() );

    RangesList vRanges;
    lcl_addRangesToVec( vRanges, uno::makeAny( Arg1 ) );
    lcl_addRangesToVec( vRanges, uno::makeAny( Arg2 ) );

    if ( Arg3.hasValue() )
        lcl_addRangesToVec( vRanges, Arg3 );
    if ( Arg4.hasValue() )
        lcl_addRangesToVec( vRanges, Arg4 );
    if ( Arg5.hasValue() )
        lcl_addRangesToVec( vRanges, Arg5 );
    if ( Arg6.hasValue() )
        lcl_addRangesToVec( vRanges, Arg6 );
    if ( Arg7.hasValue() )
        lcl_addRangesToVec( vRanges, Arg7 );
    if ( Arg8.hasValue() )
        lcl_addRangesToVec( vRanges, Arg8 );
    if ( Arg9.hasValue() )
        lcl_addRangesToVec( vRanges, Arg9 );
    if ( Arg10.hasValue() )
        lcl_addRangesToVec( vRanges, Arg10 );
    if ( Arg11.hasValue() )
        lcl_addRangesToVec( vRanges, Arg11 );
    if ( Arg12.hasValue() )
        lcl_addRangesToVec( vRanges, Arg12 );
    if ( Arg13.hasValue() )
        lcl_addRangesToVec( vRanges, Arg13 );
    if ( Arg14.hasValue() )
        lcl_addRangesToVec( vRanges, Arg14 );
    if ( Arg15.hasValue() )
        lcl_addRangesToVec( vRanges, Arg15 );
    if ( Arg16.hasValue() )
        lcl_addRangesToVec( vRanges, Arg16 );
    if ( Arg17.hasValue() )
        lcl_addRangesToVec( vRanges, Arg17 );
    if ( Arg18.hasValue() )
        lcl_addRangesToVec( vRanges, Arg18 );
    if ( Arg19.hasValue() )
        lcl_addRangesToVec( vRanges, Arg19 );
    if ( Arg20.hasValue() )
        lcl_addRangesToVec( vRanges, Arg20 );
    if ( Arg21.hasValue() )
        lcl_addRangesToVec( vRanges, Arg21 );
    if ( Arg22.hasValue() )
        lcl_addRangesToVec( vRanges, Arg22 );
    if ( Arg23.hasValue() )
        lcl_addRangesToVec( vRanges, Arg23 );
    if ( Arg24.hasValue() )
        lcl_addRangesToVec( vRanges, Arg24 );
    if ( Arg25.hasValue() )
        lcl_addRangesToVec( vRanges, Arg25 );
    if ( Arg26.hasValue() )
        lcl_addRangesToVec( vRanges, Arg26 );
    if ( Arg27.hasValue() )
        lcl_addRangesToVec( vRanges, Arg27 );
    if ( Arg28.hasValue() )
        lcl_addRangesToVec( vRanges, Arg28 );
    if ( Arg29.hasValue() )
        lcl_addRangesToVec( vRanges, Arg29 );
    if ( Arg30.hasValue() )
        lcl_addRangesToVec( vRanges, Arg30 );

    uno::Reference< excel::XRange > xRefRange;

    ScRangeList aCellRanges;
    // first pass - gets the set of all possible interections of Arg1..ArgN
    RangesList intersections = lcl_intersections( vRanges );
    // second pass - gets the intersections of the intersections ( don't ask, but this
    // is what seems to happen )
    if ( intersections.size() > 1)
        intersections = lcl_intersections( intersections );
    for( RangesList::iterator it = intersections.begin(); it != intersections.end(); ++it )
    {
        for ( USHORT x = 0 ; x < it->Count(); ++x )
#ifndef OWN_JOIN
            aCellRanges.Join( *it->GetObject(x) );
#else
            aCellRanges.Append( *it->GetObject(x) );
#endif
    }

    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    ScDocShell* pDocShell = excel::getDocShell( xModel );
    if ( aCellRanges.Count() == 1 )
    {
        xRefRange = new ScVbaRange( uno::Reference< XHelperInterface >(), mxContext, new ScCellRangeObj( pDocShell, *aCellRanges.First() ) );
    }
    else if ( aCellRanges.Count() > 1 )
    {
        uno::Reference< sheet::XSheetCellRangeContainer > xRanges( new ScCellRangesObj( pDocShell, aCellRanges ) );
        xRefRange = new ScVbaRange( uno::Reference< XHelperInterface >(), mxContext, xRanges );

    }
    return xRefRange;
}

uno::Reference< excel::XRange > SAL_CALL
ScVbaApplication::Union( const uno::Reference< excel::XRange >& Arg1, const uno::Reference< excel::XRange >& Arg2, const uno::Any& Arg3, const uno::Any& Arg4, const uno::Any& Arg5, const uno::Any& Arg6, const uno::Any& Arg7, const uno::Any& Arg8, const uno::Any& Arg9, const uno::Any& Arg10, const uno::Any& Arg11, const uno::Any& Arg12, const uno::Any& Arg13, const uno::Any& Arg14, const uno::Any& Arg15, const uno::Any& Arg16, const uno::Any& Arg17, const uno::Any& Arg18, const uno::Any& Arg19, const uno::Any& Arg20, const uno::Any& Arg21, const uno::Any& Arg22, const uno::Any& Arg23, const uno::Any& Arg24, const uno::Any& Arg25, const uno::Any& Arg26, const uno::Any& Arg27, const uno::Any& Arg28, const uno::Any& Arg29, const uno::Any& Arg30 ) throw (script::BasicErrorException, uno::RuntimeException)
{
    if ( !Arg1.is() || !Arg2.is() )
        DebugHelper::exception(SbERR_BAD_PARAMETER, rtl::OUString() );

    uno::Reference< excel::XRange > xRange;
    Ranges vRanges;
    lcl_addRangeToVec( vRanges, uno::makeAny( Arg1 ) );
    lcl_addRangeToVec( vRanges, uno::makeAny( Arg2 ) );

    if ( Arg3.hasValue() )
        lcl_addRangeToVec( vRanges, Arg3 );
    if ( Arg4.hasValue() )
        lcl_addRangeToVec( vRanges, Arg4 );
    if ( Arg5.hasValue() )
        lcl_addRangeToVec( vRanges, Arg5 );
    if ( Arg6.hasValue() )
        lcl_addRangeToVec( vRanges, Arg6 );
    if ( Arg7.hasValue() )
        lcl_addRangeToVec( vRanges, Arg7 );
    if ( Arg8.hasValue() )
        lcl_addRangeToVec( vRanges, Arg8 );
    if ( Arg9.hasValue() )
        lcl_addRangeToVec( vRanges, Arg9 );
    if ( Arg10.hasValue() )
        lcl_addRangeToVec( vRanges, Arg10 );
    if ( Arg11.hasValue() )
        lcl_addRangeToVec( vRanges, Arg11 );
    if ( Arg12.hasValue() )
        lcl_addRangeToVec( vRanges, Arg12 );
    if ( Arg13.hasValue() )
        lcl_addRangeToVec( vRanges, Arg13 );
    if ( Arg14.hasValue() )
        lcl_addRangeToVec( vRanges, Arg14 );
    if ( Arg15.hasValue() )
        lcl_addRangeToVec( vRanges, Arg15 );
    if ( Arg16.hasValue() )
        lcl_addRangeToVec( vRanges, Arg16 );
    if ( Arg17.hasValue() )
        lcl_addRangeToVec( vRanges, Arg17 );
    if ( Arg18.hasValue() )
        lcl_addRangeToVec( vRanges, Arg18 );
    if ( Arg19.hasValue() )
        lcl_addRangeToVec( vRanges, Arg19 );
    if ( Arg20.hasValue() )
        lcl_addRangeToVec( vRanges, Arg20 );
    if ( Arg21.hasValue() )
        lcl_addRangeToVec( vRanges, Arg21 );
    if ( Arg22.hasValue() )
        lcl_addRangeToVec( vRanges, Arg22 );
    if ( Arg23.hasValue() )
        lcl_addRangeToVec( vRanges, Arg23 );
    if ( Arg24.hasValue() )
        lcl_addRangeToVec( vRanges, Arg24 );
    if ( Arg25.hasValue() )
        lcl_addRangeToVec( vRanges, Arg25 );
    if ( Arg26.hasValue() )
        lcl_addRangeToVec( vRanges, Arg26 );
    if ( Arg27.hasValue() )
        lcl_addRangeToVec( vRanges, Arg27 );
    if ( Arg28.hasValue() )
        lcl_addRangeToVec( vRanges, Arg28 );
    if ( Arg29.hasValue() )
        lcl_addRangeToVec( vRanges, Arg29 );
    if ( Arg30.hasValue() )
        lcl_addRangeToVec( vRanges, Arg30 );

    ScRangeList aCellRanges;
    lcl_strip_containedRanges( vRanges );

    for( Ranges::iterator it = vRanges.begin(); it != vRanges.end(); ++it )
        aCellRanges.Append( *it );

    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    ScDocShell* pDocShell = excel::getDocShell( xModel );
    if ( aCellRanges.Count() == 1 )
    {
    // normal range
        xRange = new ScVbaRange( uno::Reference< XHelperInterface >(), mxContext, new ScCellRangeObj( pDocShell, *aCellRanges.First() ) );
    }
    else if ( aCellRanges.Count() > 1 ) // Multi-Area
    {
        uno::Reference< sheet::XSheetCellRangeContainer > xRanges( new ScCellRangesObj( pDocShell, aCellRanges ) );
        xRange = new ScVbaRange( uno::Reference< XHelperInterface >(), mxContext, xRanges );
    }

    // #FIXME need proper (WorkSheet) parent
    return xRange;
}

void
ScVbaApplication::Volatile( const uno::Any& aVolatile )  throw ( uno::RuntimeException )
{
    sal_Bool bVolatile = sal_True;
    aVolatile >>= bVolatile;
    return;
}

void SAL_CALL
ScVbaApplication::DoEvents() throw ( uno::RuntimeException )
{
}
::sal_Bool SAL_CALL
ScVbaApplication::getDisplayFormulaBar() throw ( css::uno::RuntimeException )
{
    sal_Bool bRes = sal_False;
    ScTabViewShell* pViewShell = excel::getCurrentBestViewShell( mxContext );
    if ( pViewShell )
    {
        SfxBoolItem sfxFormBar( FID_TOGGLEINPUTLINE);
        SfxAllItemSet reqList(  SFX_APP()->GetPool() );
        reqList.Put( sfxFormBar );

        pViewShell->GetState( reqList );
        const SfxPoolItem *pItem=0;
        if ( reqList.GetItemState( FID_TOGGLEINPUTLINE, sal_False, &pItem ) == SFX_ITEM_SET )
            bRes =   ((SfxBoolItem*)pItem)->GetValue();
    }
    return bRes;
}

void SAL_CALL
ScVbaApplication::setDisplayFormulaBar( ::sal_Bool _displayformulabar ) throw ( css::uno::RuntimeException )
{
    ScTabViewShell* pViewShell = excel::getCurrentBestViewShell( mxContext );
    if ( pViewShell && ( _displayformulabar !=  getDisplayFormulaBar() ) )
    {
        SfxBoolItem sfxFormBar( FID_TOGGLEINPUTLINE, _displayformulabar);
        SfxAllItemSet reqList(  SFX_APP()->GetPool() );
        SfxRequest aReq( FID_TOGGLEINPUTLINE, 0, reqList );
        pViewShell->Execute( aReq );
    }
}

uno::Any SAL_CALL
ScVbaApplication::Caller( const uno::Any& /*aIndex*/ ) throw ( uno::RuntimeException )
{
    StarBASIC* pBasic = SFX_APP()->GetBasic();
    SFX_APP()->EnterBasicCall();
    SbMethod* pMeth = (SbMethod*)pBasic->GetRtl()->Find( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("FuncCaller") ), SbxCLASS_METHOD );
    uno::Any aRet;
    if ( pMeth )
    {
        SbxVariableRef refTemp = pMeth;
        // forces a broadcast
        SbxVariableRef pNew = new  SbxMethod( *((SbxMethod*)pMeth));
                OSL_TRACE("pNew has type %d and string value %s", pNew->GetType(), rtl::OUStringToOString( pNew->GetString(), RTL_TEXTENCODING_UTF8 ).getStr() );
        aRet = sbxToUnoValue( pNew );
    }
    SFX_APP()->LeaveBasicCall();
    return aRet;
}

uno::Reference< frame::XModel >
ScVbaApplication::getCurrentDocument() throw (css::uno::RuntimeException)
{
    return getCurrentExcelDoc(mxContext);
}

rtl::OUString&
ScVbaApplication::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaApplication") );
    return sImplName;
}

uno::Sequence< rtl::OUString >
ScVbaApplication::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.Application" ) );
    }
    return aServiceNames;
}

namespace application
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<ScVbaApplication, sdecl::with_args<false> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "ScVbaApplication",
    "ooo.vba.excel.Application" );
}
