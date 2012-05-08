/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/text/FilenameDisplayFormat.hpp>
#include <com/sun/star/lang/NoSupportException.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <vcl/svapp.hxx>
#include <osl/mutex.hxx>

#include <editeng/eeitem.hxx>
#include <editeng/flditem.hxx>
#include <editeng/measfld.hxx>
#include <editeng/unofield.hxx>
#include <editeng/unotext.hxx>
#include <comphelper/serviceinfohelper.hxx>
#include <comphelper/servicehelper.hxx>

#include "editeng/unonames.hxx"

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star;

#define QUERYINT( xint ) \
    if( rType == ::getCppuType((const uno::Reference< xint >*)0) ) \
        aAny <<= uno::Reference< xint >(this)


#define WID_DATE    0
#define WID_BOOL1   1
#define WID_BOOL2   2
#define WID_INT32   3
#define WID_INT16   4
#define WID_STRING1 5
#define WID_STRING2 6
#define WID_STRING3 7

const sal_Int32 UNKNOWN_FIELD = -1;

inline bool isValidFieldId(sal_Int32 nId)
{
    return nId >= 0;
}

class SvxUnoFieldData_Impl
{
public:
    sal_Bool    mbBoolean1;
    sal_Bool    mbBoolean2;
    sal_Int32   mnInt32;
    sal_Int16   mnInt16;
    OUString    msString1;
    OUString    msString2;
    OUString    msString3;
    util::DateTime maDateTime;

    OUString    msPresentation;
};

const SfxItemPropertySet* ImplGetFieldItemPropertySet( sal_Int32 mnId )
{
    static SfxItemPropertyMapEntry aExDateTimeFieldPropertyMap_Impl[] =
    {
        { MAP_CHAR_LEN("DateTime"),         WID_DATE,       &::getCppuType((const util::DateTime*)0),       0, 0 },
        { MAP_CHAR_LEN("IsFixed"),          WID_BOOL1,      &::getBooleanCppuType(),                0, 0 },
        { MAP_CHAR_LEN("IsDate"),           WID_BOOL2,      &::getBooleanCppuType(),                0, 0 },
        { MAP_CHAR_LEN("NumberFormat"),     WID_INT32,      &::getCppuType((const sal_Int16*)0),    0, 0 },
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aExDateTimeFieldPropertySet_Impl(aExDateTimeFieldPropertyMap_Impl);

    static SfxItemPropertyMapEntry aDateTimeFieldPropertyMap_Impl[] =
    {
        { MAP_CHAR_LEN("IsDate"),           WID_BOOL2,      &::getBooleanCppuType(),                0, 0 },
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aDateTimeFieldPropertySet_Impl(aDateTimeFieldPropertyMap_Impl);

    static SfxItemPropertyMapEntry aUrlFieldPropertyMap_Impl[] =
    {

        { MAP_CHAR_LEN("Format"),           WID_INT16,      &::getCppuType((const sal_Int16*)0),    0, 0 },
        { MAP_CHAR_LEN("Representation"),   WID_STRING1,    &::getCppuType((const OUString*)0),     0, 0 },
        { MAP_CHAR_LEN("TargetFrame"),      WID_STRING2,    &::getCppuType((const OUString*)0),     0, 0 },
        { MAP_CHAR_LEN("URL"),              WID_STRING3,    &::getCppuType((const OUString*)0),     0, 0 },
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aUrlFieldPropertySet_Impl(aUrlFieldPropertyMap_Impl);

    static SfxItemPropertyMapEntry aEmptyPropertyMap_Impl[] =
    {
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aEmptyPropertySet_Impl(aEmptyPropertyMap_Impl);

    static SfxItemPropertyMapEntry aExtFileFieldPropertyMap_Impl[] =
    {
        { MAP_CHAR_LEN("IsFixed"),              WID_BOOL1,  &::getBooleanCppuType(),                0, 0 },
        { MAP_CHAR_LEN("FileFormat"),           WID_INT16,  &::getCppuType((const sal_Int16*)0),    0, 0 },
        { MAP_CHAR_LEN("CurrentPresentation"),  WID_STRING1,&::getCppuType((const OUString*)0),     0, 0 },
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aExtFileFieldPropertySet_Impl(aExtFileFieldPropertyMap_Impl);

    static SfxItemPropertyMapEntry aAuthorFieldPropertyMap_Impl[] =
    {
        { MAP_CHAR_LEN("IsFixed"),              WID_BOOL1,  &::getBooleanCppuType(),                0, 0 },
        { MAP_CHAR_LEN("CurrentPresentation"),  WID_STRING1,&::getCppuType((const OUString*)0),     0, 0 },
        { MAP_CHAR_LEN("Content"),              WID_STRING2,&::getCppuType((const OUString*)0),     0, 0 },
        { MAP_CHAR_LEN("AuthorFormat"),         WID_INT16,  &::getCppuType((const sal_Int16*)0),    0, 0 },
        { MAP_CHAR_LEN("FullName"),             WID_BOOL2,  &::getBooleanCppuType(),                0, 0 },
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aAuthorFieldPropertySet_Impl(aAuthorFieldPropertyMap_Impl);

    static SfxItemPropertyMapEntry aMeasureFieldPropertyMap_Impl[] =
    {
        { MAP_CHAR_LEN("Kind"),                 WID_INT16,  &::getCppuType((const sal_Int16*)0),    0, 0 },
        {0,0,0,0,0,0}
    };
    static SfxItemPropertySet aMeasureFieldPropertySet_Impl(aMeasureFieldPropertyMap_Impl);

    switch( mnId )
    {
    case text::textfield::Type::EXTENDED_DATE:
    case text::textfield::Type::EXTENDED_TIME:
        return &aExDateTimeFieldPropertySet_Impl;
    case text::textfield::Type::URL:
        return &aUrlFieldPropertySet_Impl;
    case text::textfield::Type::DATE:
    case text::textfield::Type::TIME:
        return &aDateTimeFieldPropertySet_Impl;
    case text::textfield::Type::EXTENDED_FILE:
        return &aExtFileFieldPropertySet_Impl;
    case text::textfield::Type::AUTHOR:
        return &aAuthorFieldPropertySet_Impl;
    case text::textfield::Type::MEASURE:
        return &aMeasureFieldPropertySet_Impl;
    default:
        return &aEmptyPropertySet_Impl;
    }
}

static sal_Char const* aFieldItemNameMap_Impl[] =
{
    "Date",
    "URL",
    "Page",
    "Pages",
    "Time",
    "File",
    "Table",
    "ExtTime",
    "ExtFile",
    "Author",
    "Measure",
    "ExtDate",
    "Header",
    "Footer",
    "DateTime"
};

/* conversion routines */

static sal_Int16 getFileNameDisplayFormat( SvxFileFormat nFormat )
{
    switch( nFormat )
    {
    case SVXFILEFORMAT_NAME_EXT:    return text::FilenameDisplayFormat::NAME_AND_EXT;
    case SVXFILEFORMAT_FULLPATH:    return text::FilenameDisplayFormat::FULL;
    case SVXFILEFORMAT_PATH:    return text::FilenameDisplayFormat::PATH;
//  case SVXFILEFORMAT_NAME:
    default: return text::FilenameDisplayFormat::NAME;
    }
}

static SvxFileFormat setFileNameDisplayFormat( sal_Int16 nFormat )
{
    switch( nFormat )
    {
    case text::FilenameDisplayFormat::FULL: return SVXFILEFORMAT_FULLPATH;
    case text::FilenameDisplayFormat::PATH: return SVXFILEFORMAT_PATH;
    case text::FilenameDisplayFormat::NAME: return SVXFILEFORMAT_NAME;
//  case text::FilenameDisplayFormat::NAME_AND_EXT:
    default:
        return SVXFILEFORMAT_NAME_EXT;
    }
}

static util::DateTime getDate( sal_uLong nDate )
{
    util::DateTime aDate;
    memset( &aDate, 0, sizeof( util::DateTime ) );

    Date aTempDate( nDate );

    aDate.Day = aTempDate.GetDay();
    aDate.Month = aTempDate.GetMonth();
    aDate.Year = aTempDate.GetYear();

    return aDate;
}

inline Date setDate( util::DateTime& rDate )
{
    return Date( rDate.Day, rDate.Month, rDate.Year );
}

static util::DateTime getTime( long nTime )
{
    util::DateTime aTime;
    memset( &aTime, 0, sizeof( util::DateTime ) );

    Time aTempTime( nTime );

    aTime.HundredthSeconds = aTempTime.Get100Sec();
    aTime.Seconds = aTempTime.GetSec();
    aTime.Minutes = aTempTime.GetMin();
    aTime.Hours = aTempTime.GetHour();

    return aTime;
}

inline Time setTime( util::DateTime& rDate )
{
    return Time( rDate.Hours, rDate.Minutes, rDate.Seconds, rDate.HundredthSeconds  );
}

// ====================================================================
// class SvxUnoTextField
// ====================================================================
UNO3_GETIMPLEMENTATION_IMPL( SvxUnoTextField );

SvxUnoTextField::SvxUnoTextField( sal_Int32 nServiceId ) throw()
:   OComponentHelper( getMutex() )
,   mpPropSet(NULL)
,   mnServiceId(nServiceId)
,   mpImpl( new SvxUnoFieldData_Impl )
{
    mpPropSet = ImplGetFieldItemPropertySet(mnServiceId);

    memset( &(mpImpl->maDateTime), 0, sizeof( util::DateTime ) );

    switch( nServiceId )
    {
    case text::textfield::Type::EXTENDED_DATE:
    case text::textfield::Type::DATE:
        mpImpl->mbBoolean2 = sal_True;
        mpImpl->mnInt32 = SVXDATEFORMAT_STDSMALL;
        mpImpl->mbBoolean1 = sal_False;
        break;

    case text::textfield::Type::EXTENDED_TIME:
    case text::textfield::Type::TIME:
        mpImpl->mbBoolean2 = sal_False;
        mpImpl->mbBoolean1 = sal_False;
        mpImpl->mnInt32 = SVXTIMEFORMAT_STANDARD;
        break;

    case text::textfield::Type::URL:
        mpImpl->mnInt16 = SVXURLFORMAT_REPR;
        break;

    case text::textfield::Type::EXTENDED_FILE:
        mpImpl->mbBoolean1 = sal_False;
        mpImpl->mnInt16 = text::FilenameDisplayFormat::FULL;
        break;

    case text::textfield::Type::AUTHOR:
        mpImpl->mnInt16 = SVXAUTHORFORMAT_FULLNAME;
        mpImpl->mbBoolean1 = sal_False;
        mpImpl->mbBoolean2 = sal_True;
        break;

    case text::textfield::Type::MEASURE:
        mpImpl->mnInt16 = SDRMEASUREFIELD_VALUE;
        break;

    default:
        mpImpl->mbBoolean1 = sal_False;
        mpImpl->mbBoolean2 = sal_False;
        mpImpl->mnInt32 = 0;
        mpImpl->mnInt16 = 0;

    }
}

SvxUnoTextField::SvxUnoTextField( uno::Reference< text::XTextRange > xAnchor, const OUString& rPresentation, const SvxFieldData* pData ) throw()
:   OComponentHelper( getMutex() )
,   mxAnchor( xAnchor )
,   mpPropSet(NULL)
,   mnServiceId(UNKNOWN_FIELD)
,   mpImpl( new SvxUnoFieldData_Impl )
{
    DBG_ASSERT(pData, "pFieldData == NULL! [CL]" );

    mpImpl->msPresentation = rPresentation;

    if(pData)
    {
        mnServiceId = GetFieldId(pData);
        DBG_ASSERT(mnServiceId != UNKNOWN_FIELD, "unknown SvxFieldData! [CL]");
        if(mnServiceId != UNKNOWN_FIELD)
        {
            // extract field properties from data class
            switch( mnServiceId )
            {
            case text::textfield::Type::DATE:
            case text::textfield::Type::EXTENDED_DATE:
                {
                    mpImpl->mbBoolean2 = sal_True;
                    // #i35416# for variable date field, don't use invalid "0000-00-00" date,
                    // use current date instead
                    sal_Bool bFixed = ((SvxDateField*)pData)->GetType() == SVXDATETYPE_FIX;
                    mpImpl->maDateTime = getDate( bFixed ?
                                            ((SvxDateField*)pData)->GetFixDate() :
                                            Date( Date::SYSTEM ).GetDate() );
                    mpImpl->mnInt32 = ((SvxDateField*)pData)->GetFormat();
                    mpImpl->mbBoolean1 = bFixed;
                }
                break;

            case text::textfield::Type::TIME:
                mpImpl->mbBoolean2 = sal_False;
                mpImpl->mbBoolean1 = sal_False;
                mpImpl->mnInt32 = SVXTIMEFORMAT_STANDARD;
                break;

            case text::textfield::Type::EXTENDED_TIME:
                mpImpl->mbBoolean2 = sal_False;
                mpImpl->maDateTime = getTime( ((SvxExtTimeField*)pData)->GetFixTime() );
                mpImpl->mbBoolean1 = ((SvxExtTimeField*)pData)->GetType() == SVXTIMETYPE_FIX;
                mpImpl->mnInt32 = ((SvxExtTimeField*)pData)->GetFormat();
                break;

            case text::textfield::Type::URL:
                mpImpl->msString1 = ((SvxURLField*)pData)->GetRepresentation();
                mpImpl->msString2 = ((SvxURLField*)pData)->GetTargetFrame();
                mpImpl->msString3 = ((SvxURLField*)pData)->GetURL();
                mpImpl->mnInt16 = sal::static_int_cast< sal_Int16 >(
                    ((SvxURLField*)pData)->GetFormat());
                break;

            case text::textfield::Type::EXTENDED_FILE:
                mpImpl->msString1 = ((SvxExtFileField*)pData)->GetFile();
                mpImpl->mbBoolean1 = ((SvxExtFileField*)pData)->GetType() == SVXFILETYPE_FIX;
                mpImpl->mnInt16 = getFileNameDisplayFormat(((SvxExtFileField*)pData)->GetFormat());
                break;

            case text::textfield::Type::AUTHOR:
                mpImpl->msString1  = ((SvxAuthorField*)pData)->GetFormatted();
                mpImpl->msString2  = ((SvxAuthorField*)pData)->GetFormatted();
                mpImpl->mnInt16    = sal::static_int_cast< sal_Int16 >(
                    ((SvxAuthorField*)pData)->GetFormat());
                mpImpl->mbBoolean1 = ((SvxAuthorField*)pData)->GetType() == SVXAUTHORTYPE_FIX;
                mpImpl->mbBoolean2 = ((SvxAuthorField*)pData)->GetFormat() != SVXAUTHORFORMAT_SHORTNAME;
                break;

            case text::textfield::Type::MEASURE:
                mpImpl->mnInt16     = sal::static_int_cast< sal_Int16 >(((SdrMeasureField*)pData)->GetMeasureFieldKind());
                break;
            }
        }
    }

    mpPropSet = ImplGetFieldItemPropertySet(mnServiceId);
}

SvxUnoTextField::~SvxUnoTextField() throw()
{
    delete mpImpl;
}

SvxFieldData* SvxUnoTextField::CreateFieldData() const throw()
{
    SvxFieldData* pData = NULL;

    switch( mnServiceId )
    {
    case text::textfield::Type::TIME:
    case text::textfield::Type::EXTENDED_TIME:
    case text::textfield::Type::DATE:
    case text::textfield::Type::EXTENDED_DATE:
    {
        if( mpImpl->mbBoolean2 ) // IsDate?
        {
            Date aDate( setDate( mpImpl->maDateTime ) );
            pData = new SvxDateField( aDate, mpImpl->mbBoolean1?SVXDATETYPE_FIX:SVXDATETYPE_VAR );
            if( mpImpl->mnInt32 >= SVXDATEFORMAT_APPDEFAULT && mpImpl->mnInt32 <= SVXDATEFORMAT_F )
                ((SvxDateField*)pData)->SetFormat( (SvxDateFormat)mpImpl->mnInt32 );
        }
        else
        {
            if( mnServiceId != text::textfield::Type::TIME && mnServiceId != text::textfield::Type::DATE )
            {
                Time aTime( setTime( mpImpl->maDateTime ) );
                pData = new SvxExtTimeField( aTime, mpImpl->mbBoolean1?SVXTIMETYPE_FIX:SVXTIMETYPE_VAR );

                if( mpImpl->mnInt32 >= SVXTIMEFORMAT_APPDEFAULT && mpImpl->mnInt32 <= SVXTIMEFORMAT_AM_HMSH )
                    ((SvxExtTimeField*)pData)->SetFormat( (SvxTimeFormat)mpImpl->mnInt32 );
            }
            else
            {
                pData = new SvxTimeField();
            }
        }

    }
        break;

    case text::textfield::Type::URL:
        pData = new SvxURLField( mpImpl->msString3, mpImpl->msString1, !mpImpl->msString1.isEmpty() ? SVXURLFORMAT_REPR : SVXURLFORMAT_URL );
        ((SvxURLField*)pData)->SetTargetFrame( mpImpl->msString2 );
        if( mpImpl->mnInt16 >= SVXURLFORMAT_APPDEFAULT && mpImpl->mnInt16 <= SVXURLFORMAT_REPR )
            ((SvxURLField*)pData)->SetFormat( (SvxURLFormat)mpImpl->mnInt16 );
        break;

    case text::textfield::Type::PAGE:
        pData = new SvxPageField();
        break;

    case text::textfield::Type::PAGES:
        pData = new SvxPagesField();
        break;

    case text::textfield::Type::FILE:
        pData = new SvxFileField();
        break;

    case text::textfield::Type::TABLE:
        pData = new SvxTableField();
        break;

    case text::textfield::Type::EXTENDED_FILE:
    {
        // #92009# pass fixed attribute to constructor
        pData = new SvxExtFileField( mpImpl->msString1,
                                     mpImpl->mbBoolean1 ? SVXFILETYPE_FIX : SVXFILETYPE_VAR,
                                     setFileNameDisplayFormat(mpImpl->mnInt16 ) );
        break;
    }

    case text::textfield::Type::AUTHOR:
    {
        ::rtl::OUString aContent;
        String aFirstName;
        String aLastName;
        String aEmpty;

        // do we have CurrentPresentation given?
        // mimic behaviour of writer, which means:
        // prefer CurrentPresentation over Content
        // if both are given.
        if( !mpImpl->msString1.isEmpty() )
            aContent = mpImpl->msString1;
        else
            aContent = mpImpl->msString2;

        sal_Int32 nPos = aContent.lastIndexOf( sal_Char(' '), 0 );
        if( nPos > 0 )
        {
            aFirstName = aContent.copy( 0, nPos );
            aLastName = aContent.copy( nPos + 1 );
        }
        else
        {
            aLastName = aContent;
        }

        // #92009# pass fixed attribute to constructor
        pData = new SvxAuthorField( aFirstName, aLastName, aEmpty,
                                    mpImpl->mbBoolean1 ? SVXAUTHORTYPE_FIX : SVXAUTHORTYPE_VAR );

        if( !mpImpl->mbBoolean2 )
        {
            ((SvxAuthorField*)pData)->SetFormat( SVXAUTHORFORMAT_SHORTNAME );
        }
        else if( mpImpl->mnInt16 >= SVXAUTHORFORMAT_FULLNAME || mpImpl->mnInt16 <= SVXAUTHORFORMAT_SHORTNAME )
        {
            ((SvxAuthorField*)pData)->SetFormat( (SvxAuthorFormat) mpImpl->mnInt16 );
        }

        break;
    }

    case text::textfield::Type::MEASURE:
    {
        SdrMeasureFieldKind eKind = SDRMEASUREFIELD_VALUE;
        if( mpImpl->mnInt16 == (sal_Int16)SDRMEASUREFIELD_UNIT || mpImpl->mnInt16 == (sal_Int16)SDRMEASUREFIELD_ROTA90BLANCS )
            eKind = (SdrMeasureFieldKind) mpImpl->mnInt16;
        pData = new SdrMeasureField( eKind);
        break;
    }
    case text::textfield::Type::HEADER:
        pData = new SvxHeaderField();
        break;
    case text::textfield::Type::FOOTER:
        pData = new SvxFooterField();
        break;
    case text::textfield::Type::DATE_TIME:
        pData = new SvxDateTimeField();
        break;
    };

    return pData;
}

// uno::XInterface
uno::Any SAL_CALL SvxUnoTextField::queryAggregation( const uno::Type & rType )
    throw(uno::RuntimeException)
{
    uno::Any aAny;

    QUERYINT( beans::XPropertySet );
    else QUERYINT( text::XTextContent );
    else QUERYINT( text::XTextField );
    else QUERYINT( lang::XServiceInfo );
    else QUERYINT( lang::XUnoTunnel );
    else
        return OComponentHelper::queryAggregation( rType );

    return aAny;
}

// XTypeProvider

uno::Sequence< uno::Type > SAL_CALL SvxUnoTextField::getTypes()
    throw (uno::RuntimeException)
{
    if( maTypeSequence.getLength() == 0 )
    {
        maTypeSequence = OComponentHelper::getTypes();
        sal_Int32 nOldCount = maTypeSequence.getLength();

        maTypeSequence.realloc( nOldCount + 4 ); // !DANGER! keep this updated
        uno::Type* pTypes = &maTypeSequence.getArray()[nOldCount];

        *pTypes++ = ::getCppuType(( const uno::Reference< text::XTextField >*)0);
        *pTypes++ = ::getCppuType(( const uno::Reference< beans::XPropertySet >*)0);
        *pTypes++ = ::getCppuType(( const uno::Reference< lang::XServiceInfo >*)0);
        *pTypes++ = ::getCppuType(( const uno::Reference< lang::XUnoTunnel >*)0);
    }
    return maTypeSequence;
}

namespace
{
    class theSvxUnoTextFieldImplementationId : public rtl::Static< UnoTunnelIdInit, theSvxUnoTextFieldImplementationId > {};
}

uno::Sequence< sal_Int8 > SAL_CALL SvxUnoTextField::getImplementationId()
    throw (uno::RuntimeException)
{
    return theSvxUnoTextFieldImplementationId::get().getSeq();
}

uno::Any SAL_CALL SvxUnoTextField::queryInterface( const uno::Type & rType )
    throw(uno::RuntimeException)
{
    return OComponentHelper::queryInterface(rType);
}

void SAL_CALL SvxUnoTextField::acquire() throw( )
{
    OComponentHelper::acquire();
}

void SAL_CALL SvxUnoTextField::release() throw( )
{
    OComponentHelper::release();
}

// Interface text::XTextField
OUString SAL_CALL SvxUnoTextField::getPresentation( sal_Bool bShowCommand )
    throw(uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    if(bShowCommand)
    {
        DBG_ASSERT(mnServiceId >= 0, "Unknown field type");
        if (isValidFieldId(mnServiceId))
            return OUString::createFromAscii(aFieldItemNameMap_Impl[mnServiceId]);
        else
            return OUString("Unknown");
    }
    else
    {
        return mpImpl->msPresentation;
    }
}

// Interface text::XTextContent
void SAL_CALL SvxUnoTextField::attach( const uno::Reference< text::XTextRange >& xTextRange )
    throw(lang::IllegalArgumentException, uno::RuntimeException)
{
    SvxUnoTextRangeBase* pRange = SvxUnoTextRange::getImplementation( xTextRange );
    if(pRange == NULL)
        throw lang::IllegalArgumentException();

    SvxFieldData* pData = CreateFieldData();
    if( pData )
        pRange->attachField( pData );

    delete pData;
}

uno::Reference< text::XTextRange > SAL_CALL SvxUnoTextField::getAnchor()
    throw(uno::RuntimeException)
{
    return mxAnchor;
}

// lang::XComponent
void SAL_CALL SvxUnoTextField::dispose()
    throw(uno::RuntimeException)
{
    OComponentHelper::dispose();
}

void SAL_CALL SvxUnoTextField::addEventListener( const uno::Reference< lang::XEventListener >& xListener )
    throw(uno::RuntimeException)
{
    OComponentHelper::addEventListener(xListener);
}

void SAL_CALL SvxUnoTextField::removeEventListener( const uno::Reference< lang::XEventListener >& aListener )
    throw(uno::RuntimeException)
{
    OComponentHelper::removeEventListener(aListener);
}


// Interface beans::XPropertySet
uno::Reference< beans::XPropertySetInfo > SAL_CALL SvxUnoTextField::getPropertySetInfo(  )
    throw(uno::RuntimeException)
{
    SolarMutexGuard aGuard;
    return mpPropSet->getPropertySetInfo();
}

void SAL_CALL SvxUnoTextField::setPropertyValue( const OUString& aPropertyName, const uno::Any& aValue )
    throw(beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    if( mpImpl == NULL )
        throw uno::RuntimeException();

    if (aPropertyName == UNO_TC_PROP_ANCHOR)
    {
        aValue >>= mxAnchor;
        return;
    }

    const SfxItemPropertySimpleEntry* pMap = mpPropSet->getPropertyMap().getByName( aPropertyName );
    if ( !pMap )
        throw beans::UnknownPropertyException();

    switch( pMap->nWID )
    {
    case WID_DATE:
        if(aValue >>= mpImpl->maDateTime)
            return;
        break;
    case WID_BOOL1:
        if(aValue >>= mpImpl->mbBoolean1)
            return;
        break;
    case WID_BOOL2:
        if(aValue >>= mpImpl->mbBoolean2)
            return;
        break;
    case WID_INT16:
        if(aValue >>= mpImpl->mnInt16)
            return;
        break;
    case WID_INT32:
        if(aValue >>= mpImpl->mnInt32)
            return;
        break;
    case WID_STRING1:
        if(aValue >>= mpImpl->msString1)
            return;
        break;
    case WID_STRING2:
        if(aValue >>= mpImpl->msString2)
            return;
        break;
    case WID_STRING3:
        if(aValue >>= mpImpl->msString3)
            return;
        break;
    }

    throw lang::IllegalArgumentException();

/*
    case WID_FORMAT:
        {
        sal_Int32 nFormat;

        switch( mnId )
        {
        case text::textfield::Type::DATE:
        {
            SvxDateField* pDate = PTR_CAST( SvxDateField, aFieldItem.GetField() );
            if(pDate)
                pDate->SetFormat( (SvxDateFormat)nFormat );
            break;
        }
        case text::textfield::Type::URL:
        {
            SvxURLField* pURL = PTR_CAST( SvxURLField, aFieldItem.GetField() );
            if(pURL)
                pURL->SetFormat( (SvxURLFormat)nFormat );
            break;
        }
        case text::textfield::Type::EXTENDED_TIME:
        {
            SvxExtTimeField* pTime = PTR_CAST( SvxExtTimeField, aFieldItem.GetField() );
            if(pTime)
                pTime->SetFormat( (SvxTimeFormat)nFormat );
            break;
        }
        case text::textfield::Type::EXTENDED_FILE:
        {
            SvxExtFileField* pFile = PTR_CAST( SvxExtFileField, aFieldItem.GetField() );
            if(pFile)
                pFile->SetFormat( (SvxFileFormat)nFormat );
            break;
        }
        case text::textfield::Type::AUTHOR:
        {
            SvxAuthorField* pAuthor = PTR_CAST( SvxAuthorField, aFieldItem.GetField() );
            if(pAuthor)
                pAuthor->SetFormat( (SvxAuthorFormat)nFormat );
            break;
        }
        default:
            throw beans::UnknownPropertyException();
        }
        }
        break;
    case WID_FIX:
        {
        if( aValue.hasValue() || aValue.getValueType() != ::getCppuBooleanType() )
            throw lang::IllegalArgumentException();
        sal_Bool bFix( *(sal_Bool*)aValue.getValue() );
        switch( mnId )
        {
        case text::textfield::Type::EXTENDED_TIME:
        {
            SvxExtTimeField* pTime = PTR_CAST( SvxExtTimeField, aFieldItem.GetField() );
            if(pTime)
                pTime->SetType( (SvxTimeType)bFix?SVXTIMETYPE_FIX:SVXTIMETYPE_VAR );
            break;
        }
        case text::textfield::Type::DATE:
        {
            SvxDateField* pDate = PTR_CAST( SvxDateField, aFieldItem.GetField() );
            if(pDate)
                pDate->SetType( (SvxDateType)bFix?SVXDATETYPE_FIX:SVXDATETYPE_VAR );
            break;
        }
        case text::textfield::Type::EXTENDED_FILE:
        {
            SvxExtFileField* pFile = PTR_CAST( SvxExtFileField, aFieldItem.GetField() );
            if(pFile)
                pFile->SetType( (SvxFileType)bFix?SVXFILETYPE_FIX:SVXFILETYPE_VAR );
            break;
        }
        case text::textfield::Type::AUTHOR:
        {
            SvxAuthorField* pAuthor = PTR_CAST( SvxAuthorField, aFieldItem.GetField() );
            if(pAuthor)
                pAuthor->SetType( (SvxAuthorType)bFix?SVXAUTHORTYPE_FIX:SVXAUTHORTYPE_VAR );
            break;
        }
        default:
            throw beans::UnknownPropertyException();
        }
        }
        break;
    case WID_PRES:
    case WID_URL:
    case WID_TARGET:
    {
        SvxURLField* pURL = PTR_CAST( SvxURLField, aFieldItem.GetField() );
        if(pURL)
        {
            OUString aUnoStr;
            if(!(aValue >>= aUnoStr))
                throw lang::IllegalArgumentException();

            switch( pMap->nWID )
            {
            case WID_PRES:
                pURL->SetRepresentation( aUnoStr );
                break;
            case WID_URL:
                pURL->SetURL( aUnoStr );
                break;
            case WID_TARGET:
                pURL->SetTargetFrame( aUnoStr );
                break;
            }
        }
        break;
    }
    }

    SfxItemSet aSet = pForwarder->GetAttribs( GetSelection() );
    aSet.Put( aFieldItem );
*/
}

uno::Any SAL_CALL SvxUnoTextField::getPropertyValue( const OUString& PropertyName )
    throw(beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    if (PropertyName == UNO_TC_PROP_ANCHOR)
        return uno::makeAny(mxAnchor);

    uno::Any aValue;

    const SfxItemPropertySimpleEntry* pMap = mpPropSet->getPropertyMap().getByName( PropertyName );
    if ( !pMap )
        throw beans::UnknownPropertyException();

    switch( pMap->nWID )
    {
    case WID_DATE:
        aValue <<= mpImpl->maDateTime;
        break;
    case WID_BOOL1:
        aValue <<= mpImpl->mbBoolean1;
        break;
    case WID_BOOL2:
        aValue <<= mpImpl->mbBoolean2;
        break;
    case WID_INT16:
        aValue <<= mpImpl->mnInt16;
        break;
    case WID_INT32:
        aValue <<= mpImpl->mnInt32;
        break;
    case WID_STRING1:
        aValue <<= mpImpl->msString1;
        break;
    case WID_STRING2:
        aValue <<= mpImpl->msString2;
        break;
    case WID_STRING3:
        aValue <<= mpImpl->msString3;
        break;
    }

    return aValue;

/*
    switch(pMap->nWID)
    {
    case WID_FORMAT:
        switch( mnId )
        {
        case text::textfield::Type::DATE:
        {
            SvxDateField* pDate = PTR_CAST( SvxDateField, pFieldItem->GetField() );
            if(pDate)
                aValue <<= (sal_Int32)pDate->GetFormat();
            break;
        }
        case text::textfield::Type::URL:
        {
            SvxURLField* pURL = PTR_CAST( SvxURLField, pFieldItem->GetField() );
            if(pURL)
                aValue <<= (sal_Int32)pURL->GetFormat();
            break;
        }
        case text::textfield::Type::EXTENDED_TIME:
        {
            SvxExtTimeField* pTime = PTR_CAST( SvxExtTimeField, pFieldItem->GetField() );
            if(pTime)
                aValue <<= (sal_Int32)pTime->GetFormat();
            break;
        }
        case text::textfield::Type::EXTENDED_FILE:
        {
            SvxExtFileField* pFile = PTR_CAST( SvxExtFileField, pFieldItem->GetField() );
            if(pFile)
                aValue <<= (sal_Int32)pFile->GetFormat();
            break;
        }
        case text::textfield::Type::AUTHOR:
        {
            SvxAuthorField* pAuthor = PTR_CAST( SvxAuthorField, pFieldItem->GetField() );
            if(pAuthor)
                aValue <<= (sal_Int32)pAuthor->GetFormat();
            break;
        }
        default:
            throw beans::UnknownPropertyException();
        }
        break;
    case WID_FIX:
        {
            sal_Bool bFix = sal_False;
        switch( mnId )
        {
        case text::textfield::Type::EXTENDED_TIME:
        {
            SvxExtTimeField* pTime = PTR_CAST( SvxExtTimeField, pFieldItem->GetField() );
            if(pTime)
                bFix = pTime->GetType() == SVXTIMETYPE_FIX;
            break;
        }
        case text::textfield::Type::DATE:
        {
            SvxDateField* pDate = PTR_CAST( SvxDateField, pFieldItem->GetField() );
            if(pDate)
                bFix = pDate->GetType() == SVXDATETYPE_FIX;
            break;
        }
        case text::textfield::Type::EXTENDED_FILE:
        {
            SvxExtFileField* pFile = PTR_CAST( SvxExtFileField, pFieldItem->GetField() );
            if(pFile)
                bFix = pFile->GetType() == SVXFILETYPE_FIX;
            break;
        }
        case text::textfield::Type::AUTHOR:
        {
            SvxAuthorField* pAuthor = PTR_CAST( SvxAuthorField, pFieldItem->GetField() );
            if(pAuthor)
                bFix = pAuthor->GetType() == SVXAUTHORTYPE_FIX;
            break;
        }
        default:
            throw beans::UnknownPropertyException();
        }
        aValue.setValue( &bFix, ::getCppuBooleanType() );
        }
        break;
    case WID_PRES:
    case WID_URL:
    case WID_TARGET:
    {
        SvxURLField* pURL = PTR_CAST( SvxURLField, pFieldItem->GetField() );
        if(pURL)
        {
            OUString aStr;
            switch( pMap->nWID )
            {
            case WID_PRES:
                aStr = pURL->GetRepresentation();
                break;
            case WID_URL:
                aStr = pURL->GetURL();
                break;
            case WID_TARGET:
                aStr = pURL->GetTargetFrame();
                break;
            }
            aValue <<= aStr;
        }
        break;
    }
    case WID_FCOLOR:
    case WID_TCOLOR:
    {
        Color* pFColor = NULL;
        Color* pTColor = NULL;
        const ESelection aSel = GetSelection();

        pForwarder->CalcFieldValue( *pFieldItem, aSel.nStartPara, aSel.nStartPos, pTColor, pFColor );

        if( pMap->nWID == WID_FCOLOR )
            aValue <<= (sal_Int32)pFColor->GetColor();
        else
            aValue <<= (sal_Int32)pTColor->GetColor();
        break;

        delete pTColor;
        delete pFColor;
    }
    }
    return aValue;
*/
}

void SAL_CALL SvxUnoTextField::addPropertyChangeListener( const OUString&, const uno::Reference< beans::XPropertyChangeListener >& ) throw(::com::sun::star::beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL SvxUnoTextField::removePropertyChangeListener( const OUString&, const uno::Reference< beans::XPropertyChangeListener >& ) throw(::com::sun::star::beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL SvxUnoTextField::addVetoableChangeListener( const OUString&, const uno::Reference< beans::XVetoableChangeListener >& ) throw(::com::sun::star::beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}
void SAL_CALL SvxUnoTextField::removeVetoableChangeListener( const OUString&, const uno::Reference< beans::XVetoableChangeListener >& ) throw(::com::sun::star::beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException) {}

// OComponentHelper
void SvxUnoTextField::disposing()
{
    // nothing to do
}

sal_Int32 SvxUnoTextField::GetFieldId( const SvxFieldData* pFieldData ) const throw()
{
    if( pFieldData->ISA( SvxURLField ) )
        return text::textfield::Type::URL;
    else if( pFieldData->ISA( SvxPageField ) )
        return text::textfield::Type::PAGE;
    else if( pFieldData->ISA( SvxPagesField ) )
        return text::textfield::Type::PAGES;
    else if( pFieldData->ISA( SvxTimeField )    )
        return text::textfield::Type::TIME;
    else if( pFieldData->ISA( SvxFileField )    )
        return text::textfield::Type::FILE;
    else if( pFieldData->ISA( SvxTableField ) )
        return text::textfield::Type::TABLE;
    else if( pFieldData->ISA( SvxExtTimeField ) )
        return text::textfield::Type::EXTENDED_TIME;
    else if( pFieldData->ISA( SvxExtFileField ) )
        return text::textfield::Type::EXTENDED_FILE;
    else if( pFieldData->ISA( SvxAuthorField ) )
        return text::textfield::Type::AUTHOR;
    else if( pFieldData->ISA( SvxDateField ) )
        return text::textfield::Type::EXTENDED_DATE;
    else if( pFieldData->ISA( SdrMeasureField ) )
        return text::textfield::Type::MEASURE;
    else if( pFieldData->ISA( SvxHeaderField ) )
        return text::textfield::Type::HEADER;
    else if( pFieldData->ISA( SvxFooterField ) )
        return text::textfield::Type::FOOTER;
    else if( pFieldData->ISA( SvxDateTimeField ) )
        return text::textfield::Type::DATE_TIME;

    return UNKNOWN_FIELD;
}

// lang::XServiceInfo
OUString SAL_CALL SvxUnoTextField::getImplementationName() throw(uno::RuntimeException)
{
    return OUString(RTL_CONSTASCII_USTRINGPARAM("SvxUnoTextField"));
}

static const sal_Char* pOldServiceNames[] =
{
    "com.sun.star.text.TextField.DateTime",
    "com.sun.star.text.TextField.URL",
    "com.sun.star.text.TextField.PageNumber",
    "com.sun.star.text.TextField.PageCount",
    "com.sun.star.text.TextField.DateTime",
    "com.sun.star.text.TextField.DocInfo.Title",    // SvxFileField is used for title
    "com.sun.star.text.TextField.SheetName",
    "com.sun.star.text.TextField.DateTime",
    "com.sun.star.text.TextField.FileName",
    "com.sun.star.text.TextField.Author",
    "com.sun.star.text.TextField.Measure",
    "com.sun.star.text.TextField.DateTime",
    "com.sun.star.presentation.TextField.Header",
    "com.sun.star.presentation.TextField.Footer",
    "com.sun.star.presentation.TextField.DateTime"
};

static const sal_Char* pNewServiceNames[] =
{
    "com.sun.star.text.textfield.DateTime",
    "com.sun.star.text.textfield.URL",
    "com.sun.star.text.textfield.PageNumber",
    "com.sun.star.text.textfield.PageCount",
    "com.sun.star.text.textfield.DateTime",
    "com.sun.star.text.textfield.docinfo.Title",    // SvxFileField is used for title
    "com.sun.star.text.textfield.SheetName",
    "com.sun.star.text.textfield.DateTime",
    "com.sun.star.text.textfield.FileName",
    "com.sun.star.text.textfield.Author",
    "com.sun.star.text.textfield.Measure",
    "com.sun.star.text.textfield.DateTime",
    "com.sun.star.presentation.textfield.Header",
    "com.sun.star.presentation.textfield.Footer",
    "com.sun.star.presentation.textfield.DateTime"
};

uno::Sequence< OUString > SAL_CALL SvxUnoTextField::getSupportedServiceNames()
    throw(uno::RuntimeException)
{
    uno::Sequence< OUString > aSeq( 4 );
    OUString* pServices = aSeq.getArray();
    pServices[0] = OUString::createFromAscii( pNewServiceNames[mnServiceId] );
    pServices[1] = OUString::createFromAscii( pOldServiceNames[mnServiceId] );
    pServices[2] = OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.TextContent" )),
    pServices[3] = OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.TextField" ));

    return aSeq;
}

sal_Bool SAL_CALL SvxUnoTextField::supportsService( const OUString& ServiceName ) throw( uno::RuntimeException )
{
    return comphelper::ServiceInfoHelper::supportsService( ServiceName, getSupportedServiceNames() );
}

uno::Reference< uno::XInterface > SAL_CALL SvxUnoTextCreateTextField( const ::rtl::OUString& ServiceSpecifier ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    uno::Reference< uno::XInterface > xRet;

    const OUString aTextFieldPrexit( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.textfield.") );

    // #i93308# up to OOo 3.2 we used this wrong namespace name with the capital T & F. This is
    // fixed since OOo 3.2 but for compatibility we will still provide support for the wrong notation.
    const OUString aTextFieldPrexit2( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextField.") );

    if( (ServiceSpecifier.compareTo( aTextFieldPrexit, aTextFieldPrexit.getLength() ) == 0) ||
        (ServiceSpecifier.compareTo( aTextFieldPrexit2, aTextFieldPrexit2.getLength() ) == 0) )
    {
        OUString aFieldType( ServiceSpecifier.copy( aTextFieldPrexit.getLength() ) );

        sal_Int32 nId = UNKNOWN_FIELD;

        if ( aFieldType == "DateTime" )
        {
            nId = text::textfield::Type::DATE;
        }
        else if ( aFieldType == "URL" )
        {
            nId = text::textfield::Type::URL;
        }
        else if ( aFieldType == "PageNumber" )
        {
            nId = text::textfield::Type::PAGE;
        }
        else if ( aFieldType == "PageCount" )
        {
            nId = text::textfield::Type::PAGES;
        }
        else if ( aFieldType == "SheetName" )
        {
            nId = text::textfield::Type::TABLE;
        }
        else if ( aFieldType == "FileName" )
        {
            nId = text::textfield::Type::EXTENDED_FILE;
        }
        else if (aFieldType.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM("docinfo.Title") ) ||
                 aFieldType.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM("DocInfo.Title") ) )
        {
            nId = text::textfield::Type::FILE;
        }
        else if ( aFieldType == "Author" )
        {
            nId = text::textfield::Type::AUTHOR;
        }
        else if ( aFieldType == "Measure" )
        {
            nId = text::textfield::Type::MEASURE;
        }

        if (nId != UNKNOWN_FIELD)
            xRet = (::cppu::OWeakObject * )new SvxUnoTextField( nId );
    }

    return xRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
