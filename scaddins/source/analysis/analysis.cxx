/*************************************************************************
 *
 *  $RCSfile: analysis.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: gt $ $Date: 2001-06-18 13:00:32 $
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

#include "analysis.hxx"
#include "analysishelper.hxx"

#include <cppuhelper/factory.hxx>
#include <osl/diagnose.h>
#include <rtl/ustrbuf.hxx>
#include <tools/solmath.hxx>
#include <string.h>

#include <tools/resmgr.hxx>
#include <tools/rcid.h>
#include <tools/isolang.hxx>
#include "analysis.hrc"


#define ADDIN_SERVICE               "com.sun.star.sheet.AddIn"
#define MY_SERVICE                  "com.sun.star.sheet.addin.Analysis"
#define MY_IMPLNAME                 "com.sun.star.sheet.addin.AnalysisImpl"


//------------------------------------------------------------------
//
//  entry points for service registration / instantiation
//
//------------------------------------------------------------------

extern "C" {


void SAL_CALL component_getImplementationEnvironment( const sal_Char** ppEnvTypeName, uno_Environment** ppEnv )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}


sal_Bool SAL_CALL component_writeInfo( void* pServiceManager, registry::XRegistryKey* pRegistryKey )
{
    if( pRegistryKey )
    {
        try
        {
            STRING                          aImpl = STRFROMASCII( "/" );
            aImpl += AnalysisAddIn::getImplementationName_Static();
            aImpl += STRFROMASCII( "/UNO/SERVICES" );

            REF( registry::XRegistryKey )   xNewKey(
                reinterpret_cast< registry::XRegistryKey* >( pRegistryKey )->createKey( aImpl ) );

            SEQ( STRING )                   aSequ = AnalysisAddIn::getSupportedServiceNames_Static();
            const STRING*                   pArray = aSequ.getConstArray();

            for( sal_Int32 i = 0 ; i < aSequ.getLength() ; i++ )
                xNewKey->createKey( pArray[ i ] );

            return sal_True;
        }
        catch( registry::InvalidRegistryException& )
        {
            OSL_ENSURE( sal_False, "### InvalidRegistryException!" );
        }
    }
    return sal_False;
}


void* SAL_CALL component_getFactory( const sal_Char* pImplName, void* pServiceManager, void* pRegistryKey )
{
    void*                                   pRet = 0;

    if( pServiceManager && STRING::createFromAscii( pImplName ) == AnalysisAddIn::getImplementationName_Static() )
    {
        REF( lang::XSingleServiceFactory )  xFactory( cppu::createOneInstanceFactory(
                reinterpret_cast< lang::XMultiServiceFactory* >( pServiceManager ),
                AnalysisAddIn::getImplementationName_Static(),
                AnalysisAddIn_CreateInstance,
                AnalysisAddIn::getSupportedServiceNames_Static() ) );

        if( xFactory.is() )
        {
            xFactory->acquire();
            pRet = xFactory.get();
        }
    }

    return pRet;
}


}   // extern C




//------------------------------------------------------------------------
//
//  "normal" service implementation
//
//------------------------------------------------------------------------


ResMgr& AnalysisAddIn::GetResMgr( void ) THROWDEF_RTE
{
    if( !pResMgr )
        THROW_RTE;

    return *pResMgr;
}


STRING AnalysisAddIn::GetDisplFuncStr( sal_uInt16 nFuncNum ) THROWDEF_RTE
{
    return String( AnalysisRscStrLoader( RID_ANALYSIS_FUNCTION_NAMES, nFuncNum, GetResMgr() ).GetString() );
}


class AnalysisResourcePublisher : public Resource
{
public:
                    AnalysisResourcePublisher( const AnalysisResId& rId ) : Resource( rId ) {}
    BOOL            IsAvailableRes( const ResId& rId ) const { return Resource::IsAvailableRes( rId ); }
    void            FreeResource() { Resource::FreeResource(); }
};


class AnalysisFuncRes : public Resource
{
public:
    AnalysisFuncRes( ResId& rRes, ResMgr& rResMgr, sal_uInt16 nInd, STRING& rRet );
};


AnalysisFuncRes::AnalysisFuncRes( ResId& rRes, ResMgr& rResMgr, sal_uInt16 nInd, STRING& rRet ) : Resource( rRes )
{
    rRet = String( AnalysisResId( nInd, rResMgr ) );

    FreeResource();
}


STRING AnalysisAddIn::GetFuncDescrStr( sal_uInt16 nResId, sal_uInt16 nStrIndex ) THROWDEF_RTE
{
    STRING                      aRet;
    AnalysisResourcePublisher   aResPubl( AnalysisResId( RID_ANALYSIS_FUNCTION_DESCRIPTIONS, GetResMgr() ) );
    AnalysisResId               aRes( nResId, GetResMgr() );
    aRes.SetRT( RSC_RESOURCE );
    if( aResPubl.IsAvailableRes( aRes ) )
    {
        AnalysisFuncRes         aSubRes( aRes, GetResMgr(), nStrIndex, aRet );
    }

    aResPubl.FreeResource();

    return aRet;
}


AnalysisAddIn::AnalysisAddIn()
{
    pResMgr = ResMgr::CreateResMgr( "analysis633", ConvertIsoNamesToLanguage( aFuncLoc.Language, aFuncLoc.Country ) );

    if( pResMgr )
        pFD = new FuncDataList( *pResMgr );
    else
        pFD = NULL;

    pFactDoubles = NULL;
    pCDL = NULL;
}


AnalysisAddIn::~AnalysisAddIn()
{
    if( pFD )
        delete pFD;

    if( pFactDoubles )
        delete pFactDoubles;

    if( pCDL )
        delete pCDL;

    if( pResMgr )
        delete pResMgr;
}


#define MAXFACTDOUBLE   300

double AnalysisAddIn::FactDouble( sal_Int32 nNum ) THROWDEF_RTE_IAE
{
    if( nNum < 0 || nNum > MAXFACTDOUBLE )
        THROW_IAE;

    if( !pFactDoubles )
    {
        pFactDoubles = new double[ MAXFACTDOUBLE + 1 ];

        pFactDoubles[ 0 ] = 1.0;    // by default

        double      fOdd = 1.0;
        double      fEven = 2.0;

        pFactDoubles[ 1 ] = fOdd;
        pFactDoubles[ 2 ] = fEven;

        sal_Bool    bOdd = sal_True;

        for( sal_uInt16 nCnt = 3 ; nCnt <= MAXFACTDOUBLE ; nCnt++ )
        {
            if( bOdd )
            {
                fOdd *= nCnt;
                pFactDoubles[ nCnt ] = fOdd;
            }
            else
            {
                fEven *= nCnt;
                pFactDoubles[ nCnt ] = fEven;
            }

            bOdd = !bOdd;

        }
    }

    return pFactDoubles[ nNum ];
}


STRING AnalysisAddIn::getImplementationName_Static()
{
    return STRFROMASCII( MY_IMPLNAME );
}


SEQ( STRING ) AnalysisAddIn::getSupportedServiceNames_Static()
{
    SEQ( STRING )   aRet(2);
    STRING*         pArray = aRet.getArray();
    pArray[0] = STRFROMASCII( ADDIN_SERVICE );
    pArray[1] = STRFROMASCII( MY_SERVICE );
    return aRet;
}


REF( uno::XInterface ) SAL_CALL AnalysisAddIn_CreateInstance( const REF( lang::XMultiServiceFactory )& )
{
    static REF( uno::XInterface )   xInst = ( cppu::OWeakObject* ) new AnalysisAddIn();
    return xInst;
}


// XServiceName

STRING SAL_CALL AnalysisAddIn::getServiceName() THROWDEF_RTE
{
    // name of specific AddIn service
    return STRFROMASCII( MY_SERVICE );
}


// XServiceInfo

STRING SAL_CALL AnalysisAddIn::getImplementationName() THROWDEF_RTE
{
    return getImplementationName_Static();
}


sal_Bool SAL_CALL AnalysisAddIn::supportsService( const STRING& aName ) THROWDEF_RTE
{
    return aName.compareToAscii( ADDIN_SERVICE ) == 0 || aName.compareToAscii( MY_SERVICE ) == 0;
}


SEQ( STRING ) SAL_CALL AnalysisAddIn::getSupportedServiceNames() THROWDEF_RTE
{
    return getSupportedServiceNames_Static();
}


// XLocalizable

void SAL_CALL AnalysisAddIn::setLocale( const lang::Locale& eLocale ) THROWDEF_RTE
{
    aFuncLoc = eLocale;
}

lang::Locale SAL_CALL AnalysisAddIn::getLocale() THROWDEF_RTE
{
    return aFuncLoc;
}


// XAddIn

STRING SAL_CALL AnalysisAddIn::getProgrammaticFuntionName( const STRING& aDisplayName ) THROWDEF_RTE
{
    //  not used by calc
    //  (but should be implemented for other uses of the AddIn service)

    return STRING();
}


STRING SAL_CALL AnalysisAddIn::getDisplayFunctionName( const STRING& aProgrammaticName ) THROWDEF_RTE
{
    STRING          aRet;

    const FuncData* p = pFD->Get( aProgrammaticName );
    if( p )
    {
        aRet = GetDisplFuncStr( p->GetUINameID() );
        if( p->IsDouble() )
            aRet += STRFROMANSI( "_ADD" );
    }
    else
    {
        aRet = STRFROMANSI( "UNKNOWNFUNC_" );
        aRet += aProgrammaticName;
    }

    return aRet;
}


STRING SAL_CALL AnalysisAddIn::getFunctionDescription( const STRING& aProgrammaticName ) THROWDEF_RTE
{
    STRING          aRet;

    const FuncData* p = pFD->Get( aProgrammaticName );
    if( p )
        aRet = GetFuncDescrStr( p->GetDescrID(), 1 );

    return aRet;
}


STRING SAL_CALL AnalysisAddIn::getDisplayArgumentName( const STRING& aName, sal_Int32 nArg ) THROWDEF_RTE
{
    STRING          aRet;

    const FuncData* p = pFD->Get( aName );
    if( p && nArg <= 0xFFFF )
    {
        sal_uInt16  nStr = p->GetStrIndex( sal_uInt16( nArg ) );
        if( nStr /*&& nStr < 4*/ )
            aRet = GetFuncDescrStr( p->GetDescrID(), nStr );
        else
            aRet = STRFROMANSI( "internal" );
    }

    return aRet;
}


STRING SAL_CALL AnalysisAddIn::getArgumentDescription( const STRING& aName, sal_Int32 nArg ) THROWDEF_RTE
{
    STRING          aRet;

    const FuncData* p = pFD->Get( aName );
    if( p && nArg <= 0xFFFF )
    {
        sal_uInt16  nStr = p->GetStrIndex( sal_uInt16( nArg ) );
        if( nStr /*&& nStr < 4*/ )
            aRet = GetFuncDescrStr( p->GetDescrID(), nStr + 1 );
        else
            aRet = STRFROMANSI( "for internal use only" );
    }

    return aRet;
}


static const char*  pDefCatName = "Add-In";


STRING SAL_CALL AnalysisAddIn::getProgrammaticCategoryName( const STRING& aName ) THROWDEF_RTE
{
    //  return non-translated strings
//  return STRFROMASCII( "Add-In" );
    const FuncData*     p = pFD->Get( aName );
    STRING              aRet;
    if( p )
    {
        const sal_Char* pStr;

        switch( p->GetCategory() )
        {
            case FDCat_DateTime:    pStr = "Date&Time";         break;
            case FDCat_Finance:     pStr = "Financial";         break;
            case FDCat_Inf:         pStr = "Information";       break;
            case FDCat_Math:        pStr = "Mathematical";      break;
            case FDCat_Tech:        pStr = "Technical";         break;
            default:
                                    pStr = pDefCatName;         break;
        }

        aRet = STRFROMASCII( pStr );
    }
    else
        aRet = STRFROMASCII( pDefCatName );

    return aRet;
}


STRING SAL_CALL AnalysisAddIn::getDisplayCategoryName( const STRING& aProgrammaticFunctionName ) THROWDEF_RTE
{
    //  return translated strings, not used for predefined categories
//  return STRFROMASCII( "Add-In" );
    const FuncData*     p = pFD->Get( aProgrammaticFunctionName );
    STRING              aRet;
    if( p )
    {
        const sal_Char* pStr;

        switch( p->GetCategory() )
        {
            case FDCat_DateTime:    pStr = "Date&Time";         break;
            case FDCat_Finance:     pStr = "Financial";         break;
            case FDCat_Inf:         pStr = "Information";       break;
            case FDCat_Math:        pStr = "Mathematical";      break;
            case FDCat_Tech:        pStr = "Technical";         break;
            default:
                                    pStr = pDefCatName;         break;
        }

        aRet = STRFROMASCII( pStr );
    }
    else
        aRet = STRFROMASCII( pDefCatName );

    return aRet;
}


SEQofLocName SAL_CALL AnalysisAddIn::getCompatibilityNames( const STRING& aProgrammaticName ) THROWDEF_RTE
{
    const FuncData*             p = pFD->Get( aProgrammaticName );

    if( !p )
        return SEQofLocName( 0 );

    const StringList&           r = p->GetCompNameList();
    sal_uInt32                  nCount = r.Count();

    SEQofLocName                aRet( nCount );

    CSS::sheet::LocalizedName*  pArray = aRet.getArray();
    CSS::sheet::LocalizedName   aLocName;
    aLocName.Locale = aFuncLoc;

    for( sal_uInt32 n = 0 ; n < nCount ; n++ )
    {
        aLocName.Name = *r.Get( n );
        pArray[ n ] = aLocName;
    }

    return aRet;
}


// XAnalysis

/*double SAL_CALL AnalysisAddIn::get_Test( constREFXPS&,
    sal_Int32 nMode, double f1, double f2, double f3 ) THROWDEF_RTE
{
    return _Test( nMode, f1, f2, f3 );
}*/


/**
 * Workday
 */

sal_Int32 SAL_CALL AnalysisAddIn::getWorkday( constREFXPS& xOptions,
    sal_Int32 nDate, sal_Int32 nDays, const ANY& aHDay ) THROWDEF_RTE
{
    if( !nDays )
        return nDate;

    sal_Int32                   nNullDate = GetNullDate( xOptions );

    SortedIndividualInt32List   aSrtLst;

    aSrtLst.InsertHolidayList( aHDay, nNullDate, sal_False );

    sal_Int32                   nActDate = nDate + nNullDate;

    if( nDays > 0 )
    {
        while( nDays )
        {
            nActDate++;

            if( GetDayOfWeek( nActDate ) < 5 )
            {
                if( !aSrtLst.Find( nActDate ) )
                    nDays--;
            }
            else
                nActDate++;     // jump over weekend
        }
    }
    else
    {
        while( nDays )
        {
            nActDate--;

            if( GetDayOfWeek( nActDate ) < 5 )
            {
                if( !aSrtLst.Find( nActDate ) )
                    nDays--;
            }
            else
                nActDate--;     // jump over weekend
        }
    }

    return nActDate - nNullDate;
}


/**
 * Yearfrac
 */

double SAL_CALL AnalysisAddIn::getYearfrac( constREFXPS& xOpt,
    sal_Int32 nStartDate, sal_Int32 nEndDate, const ANY& rMode ) THROWDEF_RTE_IAE
{
    return GetYearFrac( xOpt, nStartDate, nEndDate, GetOptBase( rMode ) );
}


sal_Int32 SAL_CALL AnalysisAddIn::getEdate( constREFXPS& xOpt, sal_Int32 nStartDate, sal_Int32 nMonths ) THROWDEF_RTE
{
    sal_Int32   nNullDate = GetNullDate( xOpt );
    sal_Int32   nDate = nStartDate + nNullDate;
    sal_uInt16  nDay, nMonth, nYear;
    DaysToDate( nDate, nDay, nMonth, nYear );

    nYear += nMonths / 12;
    nMonth += nMonths % 12;

    sal_uInt16  nDaysInMonth = DaysInMonth( nMonth, nYear );
    if( nDay > nDaysInMonth )
        nDay = nDaysInMonth;

    return DateToDays( nDay, nMonth, nYear ) - nNullDate;
}


sal_Int32 SAL_CALL AnalysisAddIn::getWeeknum( constREFXPS& xOpt, sal_Int32 nDate, sal_Int32 nMode ) THROWDEF_RTE
{
    nDate += GetNullDate( xOpt );

    sal_uInt16  nDay, nMonth, nYear;
    DaysToDate( nDate, nDay, nMonth, nYear );

    sal_Int32   nFirstInYear = DateToDays( 1, 1, nYear );
    sal_uInt16  nFirstDayInYear = GetDayOfWeek( nFirstInYear );

    return ( nDate - nFirstInYear + ( ( nMode == 1 )? ( nFirstDayInYear + 1 ) % 7 : nFirstDayInYear ) ) / 7 + 1;
}


sal_Int32 SAL_CALL AnalysisAddIn::getEomonth( constREFXPS& xOpt, sal_Int32 nDate, sal_Int32 nMonths ) THROWDEF_RTE
{
    sal_Int32   nNullDate = GetNullDate( xOpt );
    nDate += nNullDate;
    sal_uInt16  nDay, nMonth, nYear;
    DaysToDate( nDate, nDay, nMonth, nYear );

    sal_Int32   nNewMonth = nMonth + nMonths;

    if( nNewMonth > 12 )
    {
        nYear += sal_uInt16( nNewMonth / 12 );
        nNewMonth %= 12;
    }
    else if( nNewMonth < 1 )
    {
        nNewMonth = -nNewMonth;
        nYear += sal_uInt16( nNewMonth / 12 );
        nYear--;
        nNewMonth %= 12;
        nNewMonth = 12 - nNewMonth;
    }

    return DateToDays( DaysInMonth( sal_uInt16( nNewMonth ), nYear ), sal_uInt16( nNewMonth ), nYear ) - nNullDate;
}


sal_Int32 SAL_CALL AnalysisAddIn::getNetworkdays( constREFXPS& xOpt,
        sal_Int32 nStartDate, sal_Int32 nEndDate, const ANY& aHDay ) THROWDEF_RTE
{
    sal_Int32                   nNullDate = GetNullDate( xOpt );

    SortedIndividualInt32List   aSrtLst;

    aSrtLst.InsertHolidayList( aHDay, nNullDate, sal_False );

    sal_Int32                   nActDate = nStartDate + nNullDate;
    sal_Int32                   nStopDate = nEndDate + nNullDate;
    sal_Int32                   nCnt = 0;

    if( nActDate <= nStopDate )
    {
        while( nActDate <= nStopDate )
        {
            if( GetDayOfWeek( nActDate ) < 5 && !aSrtLst.Find( nActDate ) )
                nCnt++;

            nActDate++;
        }
    }
    else
    {
        while( nActDate >= nStopDate )
        {
            if( GetDayOfWeek( nActDate ) < 5 && !aSrtLst.Find( nActDate ) )
                nCnt--;

            nActDate--;
        }
    }

    return nCnt;
}


sal_Int32 SAL_CALL AnalysisAddIn::getIseven( sal_Int32 nVal ) THROWDEF_RTE
{
    return ( nVal & 0x00000001 )? 0 : 1;
}


sal_Int32 SAL_CALL AnalysisAddIn::getIsodd( sal_Int32 nVal ) THROWDEF_RTE
{
    return ( nVal & 0x00000001 )? 1 : 0;
}


double SAL_CALL AnalysisAddIn::getMultinomial( const SEQSEQ( sal_Int32 )& aV ) THROWDEF_RTE_IAE
{
    sal_Int32       n1, n2;
    sal_Int32       nE1 = aV.getLength();
    sal_Int32       nE2;
    sal_Int32       nZ = 0;
    double          fN = 1.0;

    for( n1 = 0 ; n1 < nE1 ; n1++ )
    {
        const SEQ( sal_Int32 )& rList = aV[ n1 ];
        nE2 = rList.getLength();
        const sal_Int32*    pList = rList.getConstArray();

        for( n2 = 0 ; n2 < nE2 ; n2++ )
        {
            sal_Int32       n = pList[ n2 ];

            if( n < 0 || n > 170 )
                THROW_IAE;
            else if( n > 0 )
            {
                nZ += n;
                fN *= Fak( n );
            }
        }
    }

    if( nZ <= 170 )
        return Fak( nZ ) / fN;
    else
        THROW_IAE;
}


double SAL_CALL AnalysisAddIn::getSeriessum( double fX, double fN, double fM, const SEQSEQ( double )& aCoeffList ) THROWDEF_RTE
{
    double                          fRet = 0.0;

    if( fX != 0.0 )
    {
        sal_Int32       n1, n2;
        sal_Int32       nE1 = aCoeffList.getLength();
        sal_Int32       nE2;
        sal_Int32       nZ = 0;

        for( n1 = 0 ; n1 < nE1 ; n1++ )
        {
            const SEQ( double )&    rList = aCoeffList[ n1 ];
            nE2 = rList.getLength();
            const double*           pList = rList.getConstArray();

            for( n2 = 0 ; n2 < nE2 ; n2++ )
            {
                fRet += pList[ n2 ] * pow( fX, fN );

                fN += fM;
            }
        }
    }

    return fRet;
}


double SAL_CALL AnalysisAddIn::getQuotient( double fNum, double fDenum ) THROWDEF_RTE
{
    return SolarMath::ApproxFloor( fNum / fDenum );
}


double SAL_CALL AnalysisAddIn::getMround( double fNum, double fMult ) THROWDEF_RTE
{
    if( fMult == 0.0 )
        return fMult;

    return fMult * SolarMath::Round( fNum / fMult );
}


double SAL_CALL AnalysisAddIn::getSqrtpi( double fNum ) THROWDEF_RTE
{
    return sqrt( fNum * PI );
}


double SAL_CALL AnalysisAddIn::getRandbetween( double fMin, double fMax ) THROWDEF_RTE_IAE
{
    const SolarMathRoundingMode     eRM = SolarMathRoundUp;
    fMin = SolarMath::Round( fMin, 0, eRM );
    fMax = SolarMath::Round( fMax, 0, eRM );

    if( fMin > fMax )
        THROW_IAE;

    // fMax -> range
    fMax -= fMin;
    fMax /= double( RAND_MAX );

    return SolarMath::Round( fMin + fMax * double( rand() ) );
}


double SAL_CALL AnalysisAddIn::getGcd( const SEQSEQ( double )& aVLst, const SEQ( uno::Any )& aOptVLst ) THROWDEF_RTE_IAE
{
    ChkDoubleList1  aValList;

    aValList.Append( aVLst );
    aValList.Append( aOptVLst );

    if( aValList.Count() == 0 )
        return 0.0;

    const double*   p = aValList.First();
    double          f = *p;

    p = aValList.Next();

    while( p )
    {
        f = GetGcd( *p, f );
        p = aValList.Next();
    }

    return f;
}


double SAL_CALL AnalysisAddIn::getLcm( const SEQSEQ( ANY )& aVLst, const SEQ( uno::Any )& aOptVLst ) THROWDEF_RTE_IAE
{
    ChkDoubleList2  aValList;

    aValList.Append( aVLst );
    aValList.Append( aOptVLst );

    if( aValList.Count() == 0 )
        return 0.0;

    const double*   p = aValList.First();
    double          f = *p;

    if( f == 0.0 )
        return f;

    p = aValList.Next();

    while( p )
    {
        double      fTmp = *p;
        if( f == 0.0 )
            return f;
        else
            f = fTmp * f / GetGcd( fTmp, f );
        p = aValList.Next();
    }

    return f;
}


double SAL_CALL AnalysisAddIn::getBesseli( double fNum, sal_Int32 nOrder ) THROWDEF_RTE_IAE
{
    return Bessel( fNum, nOrder, sal_True );
}


double SAL_CALL AnalysisAddIn::getBesselj( double fNum, sal_Int32 nOrder ) THROWDEF_RTE_IAE
{
    return Bessel( fNum, nOrder, sal_False );
}


double SAL_CALL AnalysisAddIn::getBesselk( double fNum, sal_Int32 nOrder ) THROWDEF_RTE_IAE
{
    THROW_IAE;

    if( nOrder < 0 )
        THROW_IAE;

    return Besselk( fNum, nOrder );
}


double SAL_CALL AnalysisAddIn::getBessely( double fNum, sal_Int32 nOrder ) THROWDEF_RTE_IAE
{
    THROW_IAE;

    if( nOrder < 0 )
        THROW_IAE;
    return 0.0; // temporary solution ;-)
}


#define _P      10                  // max. number of places
#define _MIN2   -512                // min. val for binary numbers
#define _MAX2   511                 // min. val for binary numbers
#define _MIN8   -536870912          // min. val for octal numbers
#define _MAX8   536870911           // max. val for octal numbers
#define _MIN16  -1099511627776      // min. val for hexadecimal numbers
#define _MAX16  1099511627775       // max. val for hexadecimal numbers
#define GETPLACES()                 GetOpt(rPlaces,sal_Int32(-(2^31)))
#define DOUBLECONV(from,to)         ConvertFromDec(sal_Int64(ConvertToDec(aNum,from,_P)),_MIN##to,_MAX##to,to,GETPLACES(),_P)


STRING SAL_CALL AnalysisAddIn::getBin2Oct( const STRING& aNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return DOUBLECONV( 2, 8 );
}


double SAL_CALL AnalysisAddIn::getBin2Dec( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return ConvertToDec( aNum, 2, _P );
}


STRING SAL_CALL AnalysisAddIn::getBin2Hex( const STRING& aNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return DOUBLECONV( 2, 16 );
}


STRING SAL_CALL AnalysisAddIn::getOct2Bin( const STRING& aNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return DOUBLECONV( 8, 2 );
}


double SAL_CALL AnalysisAddIn::getOct2Dec( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return ConvertToDec( aNum, 8, _P );
}


STRING SAL_CALL AnalysisAddIn::getOct2Hex( const STRING& aNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return DOUBLECONV( 8, 16 );
}


STRING SAL_CALL AnalysisAddIn::getDec2Bin( sal_Int32 nNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return ConvertFromDec( nNum, _MIN2, _MAX2, 2, GETPLACES(), _P );
}


STRING SAL_CALL AnalysisAddIn::getDec2Oct( sal_Int32 nNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return ConvertFromDec( nNum, _MIN8, _MAX8, 8, GETPLACES(), _P );
}


STRING SAL_CALL AnalysisAddIn::getDec2Hex( double fNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return ConvertFromDec( sal_Int64( fNum ), _MIN16, _MAX16, 16, GETPLACES(), _P );
}


STRING SAL_CALL AnalysisAddIn::getHex2Bin( const STRING& aNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return DOUBLECONV( 16, 2 );
}


double SAL_CALL AnalysisAddIn::getHex2Dec( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return ConvertToDec( aNum, 16, _P );
}


STRING SAL_CALL AnalysisAddIn::getHex2Oct( const STRING& aNum, const ANY& rPlaces ) THROWDEF_RTE_IAE
{
    return DOUBLECONV( 16, 8 );
}


sal_Int32 SAL_CALL AnalysisAddIn::getDelta( double fNum1, const ANY& rNum2 ) THROWDEF_RTE
{
#ifdef DEBUG
    return ( fNum1 == GetOpt( rNum2, 0.0 ) )? 1 : 0;
#else
    return fNum1 == GetOpt( rNum2, 0.0 );
#endif
}


double SAL_CALL AnalysisAddIn::getErf( double fLL, const ANY& rUL ) THROWDEF_RTE
{
    switch( rUL.getValueTypeClass() )
    {
        case uno::TypeClass_VOID:
            return Erf( fLL );
            break;
        case uno::TypeClass_DOUBLE:
            double  fUL = *( double* ) rUL.getValue();
            return Erf( fUL ) - Erf( fLL );
            break;
    }

    THROW_IAE;

    return 0.0;
}


double SAL_CALL AnalysisAddIn::getErfc( double f ) THROWDEF_RTE
{
    return 1.0 - Erf( f );
}


sal_Int32 SAL_CALL AnalysisAddIn::getGestep( double fNum, const ANY& rStep ) THROWDEF_RTE
{
#ifdef DEBUG
    return ( fNum >= GetOpt( rStep, 0.0 ) )? 1 : 0;
#else
    return fNum >= GetOpt( rStep, 0.0 );
#endif
}


double SAL_CALL AnalysisAddIn::getFactdouble( sal_Int32 nNum ) THROWDEF_RTE_IAE
{
    return FactDouble( nNum );
}


double SAL_CALL AnalysisAddIn::getImabs( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return Complex( aNum ).Abs();
}


double SAL_CALL AnalysisAddIn::getImaginary( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return Complex( aNum ).Imag();
}


STRING SAL_CALL AnalysisAddIn::getImpower( const STRING& aNum, double f ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Power( f );

    return z.GetString();
}


double SAL_CALL AnalysisAddIn::getImargument( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return Complex( aNum ).Arg();
}


STRING SAL_CALL AnalysisAddIn::getImcos( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Cos();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImdiv( const STRING& aDivid, const STRING& aDivis ) THROWDEF_RTE_IAE
{
    Complex     z( aDivid );

    z.Div( Complex( aDivis ) );

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImexp( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Exp();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImconjugate( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Conjugate();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImln( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Ln();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImlog10( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Log10();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImlog2( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Log2();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImproduct( const STRING& aNum1, const SEQ( uno::Any )& aNL ) THROWDEF_RTE_IAE
{
    Complex         z( aNum1 );

    ComplexList     aNumList;

    aNumList.Append( aNL );

    for( const Complex* p = aNumList.First(); p ; p = aNumList.Next() )
        z.Mult( *p );

    return z.GetString();
}


double SAL_CALL AnalysisAddIn::getImreal( const STRING& aNum ) THROWDEF_RTE_IAE
{
    return Complex( aNum ).Real();
}


STRING SAL_CALL AnalysisAddIn::getImsin( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Sin();

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImsub( const STRING& aNum1, const STRING& aNum2 ) THROWDEF_RTE_IAE
{
    Complex     z( aNum1 );

    z.Sub( Complex( aNum2 ) );

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImsum( const STRING& aNum1, const SEQ( CSS::uno::Any )& aFollowingPars ) THROWDEF_RTE_IAE
{
    Complex         z( aNum1 );

    ComplexList     z_list;

    z_list.Append( aFollowingPars );

    for( const Complex* p = z_list.First() ; p ; p = z_list.Next() )
        z.Add( *p );

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getImsqrt( const STRING& aNum ) THROWDEF_RTE_IAE
{
    Complex     z( aNum );

    z.Power( 0.5 );

    return z.GetString();
}


STRING SAL_CALL AnalysisAddIn::getComplex( double fR, double fI, const ANY& rSuff ) THROWDEF_RTE_IAE
{
    sal_Bool    bi;

    switch( rSuff.getValueTypeClass() )
    {
        case uno::TypeClass_VOID:
            bi = sal_True;
            break;
        case uno::TypeClass_STRING:
            {
            const STRING*   pSuff = ( const STRING* ) rSuff.getValue();
            bi = pSuff->compareToAscii( "i" ) == 0 || pSuff->getLength() == 0;
            if( !bi && pSuff->compareToAscii( "j" ) != 0 )
                THROW_IAE;
            }
            break;
        default:
            THROW_IAE;
    }

    return Complex( fR,fI ).GetString( bi );
}


double SAL_CALL AnalysisAddIn::getConvert( double f, const STRING& aFU, const STRING& aTU ) THROWDEF_RTE_IAE
{
    if( !pCDL )
        pCDL = new ConvertDataList();

    return pCDL->Convert( f, aFU, aTU );
}


