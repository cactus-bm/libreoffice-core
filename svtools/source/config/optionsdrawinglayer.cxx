/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: optionsdrawinglayer.cxx,v $
 * $Revision: 1.11 $
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
#ifdef _MSC_VER
#pragma hdrstop
#endif

//_________________________________________________________________________________________________________________
//  includes
//_________________________________________________________________________________________________________________

#include <svtools/optionsdrawinglayer.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

//_________________________________________________________________________________________________________________
//  namespaces
//_________________________________________________________________________________________________________________

using namespace ::utl                   ;
using namespace ::rtl                   ;
using namespace ::osl                   ;
using namespace ::com::sun::star::uno   ;

//_________________________________________________________________________________________________________________
//  const
//_________________________________________________________________________________________________________________

#define ROOTNODE_START                  OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/Drawinglayer"   ))
#define DEFAULT_OVERLAYBUFFER           sal_True
#define DEFAULT_PAINTBUFFER             sal_True
#define DEFAULT_STRIPE_COLOR_A          0
#define DEFAULT_STRIPE_COLOR_B          16581375
#define DEFAULT_STRIPE_LENGTH           4

// #i73602#
// #i74769#, #i75172# : Change default for Calc and Writer to True
#define DEFAULT_OVERLAYBUFFER_CALC          sal_True
#define DEFAULT_OVERLAYBUFFER_WRITER        sal_True
#define DEFAULT_OVERLAYBUFFER_DRAWIMPRESS   sal_True

// #i74769#, #i75172#
#define DEFAULT_PAINTBUFFER_CALC            sal_True
#define DEFAULT_PAINTBUFFER_WRITER          sal_True
#define DEFAULT_PAINTBUFFER_DRAWIMPRESS     sal_True

// #i4219#
#define DEFAULT_MAXIMUMPAPERWIDTH           300
#define DEFAULT_MAXIMUMPAPERHEIGHT          300
#define DEFAULT_MAXIMUMPAPERLEFTMARGIN      9999
#define DEFAULT_MAXIMUMPAPERRIGHTMARGIN     9999
#define DEFAULT_MAXIMUMPAPERTOPMARGIN       9999
#define DEFAULT_MAXIMUMPAPERBOTTOMMARGIN    9999

#define PROPERTYNAME_OVERLAYBUFFER      OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer"    ))
#define PROPERTYNAME_PAINTBUFFER        OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer"      ))
#define PROPERTYNAME_STRIPE_COLOR_A     OUString(RTL_CONSTASCII_USTRINGPARAM("StripeColorA"     ))
#define PROPERTYNAME_STRIPE_COLOR_B     OUString(RTL_CONSTASCII_USTRINGPARAM("StripeColorB"     ))
#define PROPERTYNAME_STRIPE_LENGTH      OUString(RTL_CONSTASCII_USTRINGPARAM("StripeLength"     ))

// #i73602#
#define PROPERTYNAME_OVERLAYBUFFER_CALC         OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer_Calc"))
#define PROPERTYNAME_OVERLAYBUFFER_WRITER       OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer_Writer"))
#define PROPERTYNAME_OVERLAYBUFFER_DRAWIMPRESS  OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer_DrawImpress"))

// #i74769#, #i75172#
#define PROPERTYNAME_PAINTBUFFER_CALC           OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer_Calc"))
#define PROPERTYNAME_PAINTBUFFER_WRITER         OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer_Writer"))
#define PROPERTYNAME_PAINTBUFFER_DRAWIMPRESS    OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer_DrawImpress"))

// #i4219#
#define PROPERTYNAME_MAXIMUMPAPERWIDTH OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperWidth"))
#define PROPERTYNAME_MAXIMUMPAPERHEIGHT OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperHeight"))
#define PROPERTYNAME_MAXIMUMPAPERLEFTMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperLeftMargin"))
#define PROPERTYNAME_MAXIMUMPAPERRIGHTMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperRightMargin"))
#define PROPERTYNAME_MAXIMUMPAPERTOPMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperTopMargin"))
#define PROPERTYNAME_MAXIMUMPAPERBOTTOMMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperBottomMargin"))

#define PROPERTYHANDLE_OVERLAYBUFFER                0
#define PROPERTYHANDLE_PAINTBUFFER                  1
#define PROPERTYHANDLE_STRIPE_COLOR_A               2
#define PROPERTYHANDLE_STRIPE_COLOR_B               3
#define PROPERTYHANDLE_STRIPE_LENGTH                4

// #i73602#
#define PROPERTYHANDLE_OVERLAYBUFFER_CALC           5
#define PROPERTYHANDLE_OVERLAYBUFFER_WRITER         6
#define PROPERTYHANDLE_OVERLAYBUFFER_DRAWIMPRESS    7

// #i74769#, #i75172#
#define PROPERTYHANDLE_PAINTBUFFER_CALC             8
#define PROPERTYHANDLE_PAINTBUFFER_WRITER           9
#define PROPERTYHANDLE_PAINTBUFFER_DRAWIMPRESS      10

// #i4219#
#define PROPERTYHANDLE_MAXIMUMPAPERWIDTH            11
#define PROPERTYHANDLE_MAXIMUMPAPERHEIGHT           12
#define PROPERTYHANDLE_MAXIMUMPAPERLEFTMARGIN       13
#define PROPERTYHANDLE_MAXIMUMPAPERRIGHTMARGIN      14
#define PROPERTYHANDLE_MAXIMUMPAPERTOPMARGIN        15
#define PROPERTYHANDLE_MAXIMUMPAPERBOTTOMMARGIN     16

#define PROPERTYCOUNT                               17

class SvtOptionsDrawinglayer_Impl : public ConfigItem
{
public:

//---------------------------------------------------------------------------------------------------------
//  constructor / destructor
//---------------------------------------------------------------------------------------------------------

     SvtOptionsDrawinglayer_Impl();
    ~SvtOptionsDrawinglayer_Impl();

//---------------------------------------------------------------------------------------------------------
//  overloaded methods of baseclass
//---------------------------------------------------------------------------------------------------------

    virtual void Commit();

//---------------------------------------------------------------------------------------------------------
//  public interface
//---------------------------------------------------------------------------------------------------------

    sal_Bool    IsOverlayBuffer() const;
    sal_Bool    IsPaintBuffer() const;
    Color       GetStripeColorA() const;
    Color       GetStripeColorB() const;
    sal_uInt16  GetStripeLength() const;

    // #i73602#
    sal_Bool    IsOverlayBuffer_Calc() const;
    sal_Bool    IsOverlayBuffer_Writer() const;
    sal_Bool    IsOverlayBuffer_DrawImpress() const;

    // #i74769#, #i75172#
    sal_Bool    IsPaintBuffer_Calc() const;
    sal_Bool    IsPaintBuffer_Writer() const;
    sal_Bool    IsPaintBuffer_DrawImpress() const;

    void        SetOverlayBuffer( sal_Bool bState );
    void        SetPaintBuffer( sal_Bool bState );
    void        SetStripeColorA( Color aColor );
    void        SetStripeColorB( Color aColor );
    void        SetStripeLength( sal_uInt16 nLength );

    // #i73602#
    void        SetOverlayBuffer_Calc( sal_Bool bState );
    void        SetOverlayBuffer_Writer( sal_Bool bState );
    void        SetOverlayBuffer_DrawImpress( sal_Bool bState );

    // #i74769#, #i75172#
    void        SetPaintBuffer_Calc( sal_Bool bState );
    void        SetPaintBuffer_Writer( sal_Bool bState );
    void        SetPaintBuffer_DrawImpress( sal_Bool bState );

    // #i4219#
    sal_uInt32 GetMaximumPaperWidth() const;
    sal_uInt32 GetMaximumPaperHeight() const;
    sal_uInt32 GetMaximumPaperLeftMargin() const;
    sal_uInt32 GetMaximumPaperRightMargin() const;
    sal_uInt32 GetMaximumPaperTopMargin() const;
    sal_uInt32 GetMaximumPaperBottomMargin() const;

    void SetMaximumPaperWidth(sal_uInt32 nNew);
    void SetMaximumPaperHeight(sal_uInt32 nNew);
    void SetMaximumPaperLeftMargin(sal_uInt32 nNew);
    void SetMaximumPaperRightMargin(sal_uInt32 nNew);
    void SetMaximumPaperTopMargin(sal_uInt32 nNew);
    void SetMaximumPaperBottomMargin(sal_uInt32 nNew);

//-------------------------------------------------------------------------------------------------------------
//  private methods
//-------------------------------------------------------------------------------------------------------------

private:

    static Sequence< OUString > impl_GetPropertyNames();

//-------------------------------------------------------------------------------------------------------------
//  private member
//-------------------------------------------------------------------------------------------------------------

private:

        sal_Bool    m_bOverlayBuffer;
        sal_Bool    m_bPaintBuffer;
        Color       m_bStripeColorA;
        Color       m_bStripeColorB;
        sal_uInt16  m_nStripeLength;

        // #i73602#
        sal_Bool    m_bOverlayBuffer_Calc;
        sal_Bool    m_bOverlayBuffer_Writer;
        sal_Bool    m_bOverlayBuffer_DrawImpress;

        // #i74769#, #i75172#
        sal_Bool    m_bPaintBuffer_Calc;
        sal_Bool    m_bPaintBuffer_Writer;
        sal_Bool    m_bPaintBuffer_DrawImpress;

        // #i4219#
        sal_uInt32  m_nMaximumPaperWidth;
        sal_uInt32  m_nMaximumPaperHeight;
        sal_uInt32  m_nMaximumPaperLeftMargin;
        sal_uInt32  m_nMaximumPaperRightMargin;
        sal_uInt32  m_nMaximumPaperTopMargin;
        sal_uInt32  m_nMaximumPaperBottomMargin;
};

//_________________________________________________________________________________________________________________
//  definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//  constructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl() :
    ConfigItem( ROOTNODE_START  ),
    m_bOverlayBuffer( DEFAULT_OVERLAYBUFFER ),
    m_bPaintBuffer( DEFAULT_PAINTBUFFER ),
    m_bStripeColorA(Color(DEFAULT_STRIPE_COLOR_A)),
    m_bStripeColorB(Color(DEFAULT_STRIPE_COLOR_B)),
    m_nStripeLength(DEFAULT_STRIPE_LENGTH),

    // #i73602#
    m_bOverlayBuffer_Calc( DEFAULT_OVERLAYBUFFER_CALC ),
    m_bOverlayBuffer_Writer( DEFAULT_OVERLAYBUFFER_WRITER ),
    m_bOverlayBuffer_DrawImpress( DEFAULT_OVERLAYBUFFER_DRAWIMPRESS ),

    // #i74769#, #i75172#
    m_bPaintBuffer_Calc( DEFAULT_PAINTBUFFER_CALC ),
    m_bPaintBuffer_Writer( DEFAULT_PAINTBUFFER_WRITER ),
    m_bPaintBuffer_DrawImpress( DEFAULT_PAINTBUFFER_DRAWIMPRESS ),

    // #i4219#
    m_nMaximumPaperWidth(DEFAULT_MAXIMUMPAPERWIDTH),
    m_nMaximumPaperHeight(DEFAULT_MAXIMUMPAPERHEIGHT),
    m_nMaximumPaperLeftMargin(DEFAULT_MAXIMUMPAPERLEFTMARGIN),
    m_nMaximumPaperRightMargin(DEFAULT_MAXIMUMPAPERRIGHTMARGIN),
    m_nMaximumPaperTopMargin(DEFAULT_MAXIMUMPAPERTOPMARGIN),
    m_nMaximumPaperBottomMargin(DEFAULT_MAXIMUMPAPERBOTTOMMARGIN)
{
    Sequence< OUString >    seqNames( impl_GetPropertyNames() );
    Sequence< Any >         seqValues   = GetProperties( seqNames ) ;

    DBG_ASSERT( !(seqNames.getLength()!=seqValues.getLength()), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nI miss some values of configuration keys!\n" );

    // Copy values from list in right order to ouer internal member.
    sal_Int32 nPropertyCount = seqValues.getLength();
    sal_Int32 nProperty = 0;

    for( nProperty=0; nProperty<nPropertyCount; ++nProperty )
    {
        DBG_ASSERT( !(seqValues[nProperty].hasValue()==sal_False), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nInvalid property value for property detected!\n" );

        switch( nProperty )
        {
            case PROPERTYHANDLE_OVERLAYBUFFER:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer\"?" );
                seqValues[nProperty] >>= m_bOverlayBuffer;
            }
            break;

            case PROPERTYHANDLE_PAINTBUFFER:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer\"?" );
                seqValues[nProperty] >>= m_bPaintBuffer;
            }
            break;

            case PROPERTYHANDLE_STRIPE_COLOR_A:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\StripeColorA\"?" );
                sal_Int32 nValue = 0;
                seqValues[nProperty] >>= nValue;
                m_bStripeColorA = nValue;
            }
            break;

            case PROPERTYHANDLE_STRIPE_COLOR_B:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\StripeColorB\"?" );
                sal_Int32 nValue = 0;
                seqValues[nProperty] >>= nValue;
                m_bStripeColorB = nValue;
            }
            break;

            case PROPERTYHANDLE_STRIPE_LENGTH:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_SHORT), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\StripeLength\"?" );
                seqValues[nProperty] >>= m_nStripeLength;
            }
            break;

            // #i73602#
            case PROPERTYHANDLE_OVERLAYBUFFER_CALC:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer_Calc\"?" );
                seqValues[nProperty] >>= m_bOverlayBuffer_Calc;
            }
            break;

            case PROPERTYHANDLE_OVERLAYBUFFER_WRITER:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer_Writer\"?" );
                seqValues[nProperty] >>= m_bOverlayBuffer_Writer;
            }
            break;

            case PROPERTYHANDLE_OVERLAYBUFFER_DRAWIMPRESS:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer_DrawImpress\"?" );
                seqValues[nProperty] >>= m_bOverlayBuffer_DrawImpress;
            }
            break;

            // #i74769#, #i75172#
            case PROPERTYHANDLE_PAINTBUFFER_CALC:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer_Calc\"?" );
                seqValues[nProperty] >>= m_bPaintBuffer_Calc;
            }
            break;

            case PROPERTYHANDLE_PAINTBUFFER_WRITER:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer_Writer\"?" );
                seqValues[nProperty] >>= m_bPaintBuffer_Writer;
            }
            break;

            case PROPERTYHANDLE_PAINTBUFFER_DRAWIMPRESS:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer_DrawImpress\"?" );
                seqValues[nProperty] >>= m_bPaintBuffer_DrawImpress;
            }
            break;

            // #i4219#
            case PROPERTYHANDLE_MAXIMUMPAPERWIDTH:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperWidth\"?" );
                seqValues[nProperty] >>= m_nMaximumPaperWidth;
            }
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERHEIGHT:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperHeight\"?" );
                seqValues[nProperty] >>= m_nMaximumPaperHeight;
            }
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERLEFTMARGIN:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperLeftMargin\"?" );
                seqValues[nProperty] >>= m_nMaximumPaperLeftMargin;
            }
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERRIGHTMARGIN:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperRightMargin\"?" );
                seqValues[nProperty] >>= m_nMaximumPaperRightMargin;
            }
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERTOPMARGIN:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperTopMargin\"?" );
                seqValues[nProperty] >>= m_nMaximumPaperTopMargin;
            }
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERBOTTOMMARGIN:
            {
                DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperBottomMargin\"?" );
                seqValues[nProperty] >>= m_nMaximumPaperBottomMargin;
            }
            break;
        }
    }
}

//*****************************************************************************************************************
//  destructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer_Impl::~SvtOptionsDrawinglayer_Impl()
{
    if( IsModified() )
        Commit();
}

//*****************************************************************************************************************
//  Commit
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::Commit()
{
    Sequence< OUString >    aSeqNames( impl_GetPropertyNames() );
    Sequence< Any >         aSeqValues( aSeqNames.getLength() );

    for( sal_Int32 nProperty = 0, nCount = aSeqNames.getLength(); nProperty < nCount; ++nProperty )
    {
        switch( nProperty )
        {
            case PROPERTYHANDLE_OVERLAYBUFFER:
                aSeqValues[nProperty] <<= m_bOverlayBuffer;
            break;

            case PROPERTYHANDLE_PAINTBUFFER:
                aSeqValues[nProperty] <<= m_bPaintBuffer;
            break;

            case PROPERTYHANDLE_STRIPE_COLOR_A:
                aSeqValues[nProperty] <<= m_bStripeColorA.GetColor();
            break;

            case PROPERTYHANDLE_STRIPE_COLOR_B:
                aSeqValues[nProperty] <<= m_bStripeColorB.GetColor();
            break;

            case PROPERTYHANDLE_STRIPE_LENGTH:
                aSeqValues[nProperty] <<= m_nStripeLength;
            break;

            // #i73602#
            case PROPERTYHANDLE_OVERLAYBUFFER_CALC:
                aSeqValues[nProperty] <<= m_bOverlayBuffer_Calc;
            break;

            case PROPERTYHANDLE_OVERLAYBUFFER_WRITER:
                aSeqValues[nProperty] <<= m_bOverlayBuffer_Writer;
            break;

            case PROPERTYHANDLE_OVERLAYBUFFER_DRAWIMPRESS:
                aSeqValues[nProperty] <<= m_bOverlayBuffer_DrawImpress;
            break;

            // #i74769#, #i75172#
            case PROPERTYHANDLE_PAINTBUFFER_CALC:
                aSeqValues[nProperty] <<= m_bPaintBuffer_Calc;
            break;

            case PROPERTYHANDLE_PAINTBUFFER_WRITER:
                aSeqValues[nProperty] <<= m_bPaintBuffer_Writer;
            break;

            case PROPERTYHANDLE_PAINTBUFFER_DRAWIMPRESS:
                aSeqValues[nProperty] <<= m_bPaintBuffer_DrawImpress;
            break;

            // #i4219#
            case PROPERTYHANDLE_MAXIMUMPAPERWIDTH:
                aSeqValues[nProperty] <<= m_nMaximumPaperWidth;
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERHEIGHT:
                aSeqValues[nProperty] <<= m_nMaximumPaperHeight;
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERLEFTMARGIN:
                aSeqValues[nProperty] <<= m_nMaximumPaperLeftMargin;
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERRIGHTMARGIN:
                aSeqValues[nProperty] <<= m_nMaximumPaperRightMargin;
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERTOPMARGIN:
                aSeqValues[nProperty] <<= m_nMaximumPaperTopMargin;
            break;

            case PROPERTYHANDLE_MAXIMUMPAPERBOTTOMMARGIN:
                aSeqValues[nProperty] <<= m_nMaximumPaperBottomMargin;
            break;
        }
    }

    PutProperties( aSeqNames, aSeqValues );
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer() const
{
    return m_bOverlayBuffer;
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer() const
{
    return m_bPaintBuffer;
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer_Impl::GetStripeColorA() const
{
    return m_bStripeColorA;
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer_Impl::GetStripeColorB() const
{
    return m_bStripeColorB;
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
sal_uInt16 SvtOptionsDrawinglayer_Impl::GetStripeLength() const
{
    return m_nStripeLength;
}

// #i73602#
sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer_Calc() const
{
    return m_bOverlayBuffer_Calc;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer_Writer() const
{
    return m_bOverlayBuffer_Writer;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer_DrawImpress() const
{
    return m_bOverlayBuffer_DrawImpress;
}

// #i74769#, #i75172#
sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer_Calc() const
{
    return m_bPaintBuffer_Calc;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer_Writer() const
{
    return m_bPaintBuffer_Writer;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer_DrawImpress() const
{
    return m_bPaintBuffer_DrawImpress;
}

// #i4219#
sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperWidth() const
{
    return m_nMaximumPaperWidth;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperHeight() const
{
    return m_nMaximumPaperHeight;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperLeftMargin() const
{
    return m_nMaximumPaperLeftMargin;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperRightMargin() const
{
    return m_nMaximumPaperRightMargin;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperTopMargin() const
{
    return m_nMaximumPaperTopMargin;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperBottomMargin() const
{
    return m_nMaximumPaperBottomMargin;
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer( sal_Bool bState )
{
    if(m_bOverlayBuffer != bState)
    {
        m_bOverlayBuffer = bState;
        SetModified();
    }
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetPaintBuffer( sal_Bool bState )
{
    if(m_bPaintBuffer != bState)
    {
        m_bPaintBuffer = bState;
        SetModified();
    }
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetStripeColorA( Color aColor )
{
    if(m_bStripeColorA != aColor)
    {
        m_bStripeColorA = aColor;
        SetModified();
    }
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetStripeColorB( Color aColor )
{
    if(m_bStripeColorB != aColor)
    {
        m_bStripeColorB = aColor;
        SetModified();
    }
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetStripeLength( sal_uInt16 nLength )
{
    if(m_nStripeLength != nLength)
    {
        m_nStripeLength = nLength;
        SetModified();
    }
}

// #i73602#
void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer_Calc( sal_Bool bState )
{
    if(m_bOverlayBuffer_Calc != bState)
    {
        m_bOverlayBuffer_Calc = bState;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer_Writer( sal_Bool bState )
{
    if(m_bOverlayBuffer_Writer != bState)
    {
        m_bOverlayBuffer_Writer = bState;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer_DrawImpress( sal_Bool bState )
{
    if(m_bOverlayBuffer_DrawImpress != bState)
    {
        m_bOverlayBuffer_DrawImpress = bState;
        SetModified();
    }
}

// #i74769#, #i75172#
void SvtOptionsDrawinglayer_Impl::SetPaintBuffer_Calc( sal_Bool bState )
{
    if(m_bPaintBuffer_Calc != bState)
    {
        m_bPaintBuffer_Calc = bState;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetPaintBuffer_Writer( sal_Bool bState )
{
    if(m_bPaintBuffer_Writer != bState)
    {
        m_bPaintBuffer_Writer = bState;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetPaintBuffer_DrawImpress( sal_Bool bState )
{
    if(m_bPaintBuffer_DrawImpress != bState)
    {
        m_bPaintBuffer_DrawImpress = bState;
        SetModified();
    }
}

// #i4219#
void SvtOptionsDrawinglayer_Impl::SetMaximumPaperWidth( sal_uInt32 nNew )
{
    if(m_nMaximumPaperWidth != nNew)
    {
        m_nMaximumPaperWidth = nNew;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperHeight( sal_uInt32 nNew )
{
    if(m_nMaximumPaperHeight != nNew)
    {
        m_nMaximumPaperHeight = nNew;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperLeftMargin( sal_uInt32 nNew )
{
    if(m_nMaximumPaperLeftMargin != nNew)
    {
        m_nMaximumPaperLeftMargin = nNew;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperRightMargin( sal_uInt32 nNew )
{
    if(m_nMaximumPaperRightMargin != nNew)
    {
        m_nMaximumPaperRightMargin = nNew;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperTopMargin( sal_uInt32 nNew )
{
    if(m_nMaximumPaperTopMargin != nNew)
    {
        m_nMaximumPaperTopMargin = nNew;
        SetModified();
    }
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperBottomMargin( sal_uInt32 nNew )
{
    if(m_nMaximumPaperBottomMargin != nNew)
    {
        m_nMaximumPaperBottomMargin = nNew;
        SetModified();
    }
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
Sequence< OUString > SvtOptionsDrawinglayer_Impl::impl_GetPropertyNames()
{
    // Build static list of configuration key names.
    static const OUString pProperties[] =
    {
        PROPERTYNAME_OVERLAYBUFFER      ,
        PROPERTYNAME_PAINTBUFFER        ,
        PROPERTYNAME_STRIPE_COLOR_A     ,
        PROPERTYNAME_STRIPE_COLOR_B     ,
        PROPERTYNAME_STRIPE_LENGTH      ,

        // #i73602#
        PROPERTYNAME_OVERLAYBUFFER_CALC,
        PROPERTYNAME_OVERLAYBUFFER_WRITER,
        PROPERTYNAME_OVERLAYBUFFER_DRAWIMPRESS,

        // #i74769#, #i75172#
        PROPERTYNAME_PAINTBUFFER_CALC,
        PROPERTYNAME_PAINTBUFFER_WRITER,
        PROPERTYNAME_PAINTBUFFER_DRAWIMPRESS,

        // #i4219#
        PROPERTYNAME_MAXIMUMPAPERWIDTH,
        PROPERTYNAME_MAXIMUMPAPERHEIGHT,
        PROPERTYNAME_MAXIMUMPAPERLEFTMARGIN,
        PROPERTYNAME_MAXIMUMPAPERRIGHTMARGIN,
        PROPERTYNAME_MAXIMUMPAPERTOPMARGIN,
        PROPERTYNAME_MAXIMUMPAPERBOTTOMMARGIN
    };

    // Initialize return sequence with these list ...
    static const Sequence< OUString > seqPropertyNames( pProperties, PROPERTYCOUNT );
    // ... and return it.
    return seqPropertyNames;
}

//*****************************************************************************************************************
//  initialize static member
//  DON'T DO IT IN YOUR HEADER!
//  see definition for further informations
//*****************************************************************************************************************
SvtOptionsDrawinglayer_Impl* SvtOptionsDrawinglayer::m_pDataContainer = NULL;
sal_Int32 SvtOptionsDrawinglayer::m_nRefCount = 0;

//*****************************************************************************************************************
//  constructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer::SvtOptionsDrawinglayer()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
    // Increase ouer refcount ...
    ++m_nRefCount;
    // ... and initialize ouer data container only if it not already!
    if( m_pDataContainer == NULL )
    {
        m_pDataContainer = new SvtOptionsDrawinglayer_Impl();
    }
}

//*****************************************************************************************************************
//  destructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer::~SvtOptionsDrawinglayer()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetOwnStaticMutex() );
    // Decrease ouer refcount.
    --m_nRefCount;
    // If last instance was deleted ...
    // we must destroy ouer static data container!
    if( m_nRefCount <= 0 )
    {
        delete m_pDataContainer;
        m_pDataContainer = NULL;
    }
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsOverlayBuffer();
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsPaintBuffer();
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer::GetStripeColorA() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetStripeColorA();
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer::GetStripeColorB() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetStripeColorB();
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
sal_uInt16 SvtOptionsDrawinglayer::GetStripeLength() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetStripeLength();
}

// #i73602#
sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer_Calc() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsOverlayBuffer_Calc();
}

sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer_Writer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsOverlayBuffer_Writer();
}

sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer_DrawImpress() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsOverlayBuffer_DrawImpress();
}

// #i74769#, #i75172#
sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer_Calc() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsPaintBuffer_Calc();
}

sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer_Writer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsPaintBuffer_Writer();
}

sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer_DrawImpress() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->IsPaintBuffer_DrawImpress();
}

// #i4219#
sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperWidth() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMaximumPaperWidth();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperHeight() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMaximumPaperHeight();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperLeftMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMaximumPaperLeftMargin();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperRightMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMaximumPaperRightMargin();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperTopMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMaximumPaperTopMargin();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperBottomMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    return m_pDataContainer->GetMaximumPaperBottomMargin();
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetOverlayBuffer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetOverlayBuffer( bState );
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetPaintBuffer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetPaintBuffer( bState );
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetStripeColorA( Color aColor )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetStripeColorA( aColor );
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetStripeColorB( Color aColor )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetStripeColorB( aColor );
}

//*****************************************************************************************************************
//  public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetStripeLength( sal_uInt16 nLength )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetStripeLength( nLength );
}

// #i73602#
void SvtOptionsDrawinglayer::SetOverlayBuffer_Calc( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetOverlayBuffer_Calc( bState );
}

void SvtOptionsDrawinglayer::SetOverlayBuffer_Writer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetOverlayBuffer_Writer( bState );
}

void SvtOptionsDrawinglayer::SetOverlayBuffer_DrawImpress( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetOverlayBuffer_DrawImpress( bState );
}

// #i74769#, #i75172#
void SvtOptionsDrawinglayer::SetPaintBuffer_Calc( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetPaintBuffer_Calc( bState );
}

void SvtOptionsDrawinglayer::SetPaintBuffer_Writer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetPaintBuffer_Writer( bState );
}

void SvtOptionsDrawinglayer::SetPaintBuffer_DrawImpress( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetPaintBuffer_DrawImpress( bState );
}

// #i4219#
void SvtOptionsDrawinglayer::SetMaximumPaperWidth( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetMaximumPaperWidth( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperHeight( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetMaximumPaperHeight( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperLeftMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetMaximumPaperLeftMargin( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperRightMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetMaximumPaperRightMargin( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperTopMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetMaximumPaperTopMargin( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperBottomMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
    m_pDataContainer->SetMaximumPaperBottomMargin( nNew );
}

//*****************************************************************************************************************
//  private method
//*****************************************************************************************************************
Mutex& SvtOptionsDrawinglayer::GetOwnStaticMutex()
{
    // Initialize static mutex only for one time!
    static Mutex* pMutex = NULL;
    // If these method first called (Mutex not already exist!) ...
    if( pMutex == NULL )
    {
        // ... we must create a new one. Protect follow code with the global mutex -
        // It must be - we create a static variable!
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        // We must check our pointer again - because it can be that another instance of ouer class will be fastr then these!
        if( pMutex == NULL )
        {
            // Create the new mutex and set it for return on static variable.
            static Mutex aMutex;
            pMutex = &aMutex;
        }
    }
    // Return new created or already existing mutex object.
    return *pMutex;
}

// eof

