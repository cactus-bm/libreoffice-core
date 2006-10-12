/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: htmlcfg.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-10-12 13:00:03 $
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
#include "precompiled_svx.hxx"

#include "htmlcfg.hxx"

#ifndef _PARHTML_HXX //autogen
#include <svtools/parhtml.hxx>
#endif
#ifndef _SFX_BINDINGS_HXX //autogen
#include <sfx2/bindings.hxx>
#endif

#include <sfx2/viewfrm.hxx>

#include "svxids.hrc"
#include "txencbox.hxx"

// -----------------------------------------------------------------------
#define HTMLCFG_UNKNOWN_TAGS            0x01
//#define HTMLCFG_STYLE_SHEETS          0x02
//#define HTMLCFG_NETSCAPE3             0x04
#define HTMLCFG_STAR_BASIC              0x08
#define HTMLCFG_LOCAL_GRF               0x10
#define HTMLCFG_PRINT_LAYOUT_EXTENSION  0x20
#define HTMLCFG_IGNORE_FONT_FAMILY      0x40
#define HTMLCFG_IS_BASIC_WARNING        0x80
#define HTMLCFG_NUMBERS_ENGLISH_US      0x100

using namespace utl;
using namespace rtl;
using namespace com::sun::star::uno;

static SvxHtmlOptions* pOptions = 0;

#define C2U(cChar) OUString::createFromAscii(cChar)
/* -----------------------------23.11.00 11:39--------------------------------

 ---------------------------------------------------------------------------*/
struct HtmlOptions_Impl
{
    sal_Int32   nFlags;
    sal_Int32   nExportMode;
    sal_Int32   aFontSizeArr[HTML_FONT_COUNT];
    sal_Int32   eEncoding;
    sal_Bool    bIsEncodingDefault;

    HtmlOptions_Impl() :
        nFlags(HTMLCFG_LOCAL_GRF|HTMLCFG_IS_BASIC_WARNING),
        nExportMode(HTML_CFG_NS40),
        eEncoding( gsl_getSystemTextEncoding() ),
        bIsEncodingDefault(sal_True)
    {
        aFontSizeArr[0] = HTMLFONTSZ1_DFLT;
        aFontSizeArr[1] = HTMLFONTSZ2_DFLT;
        aFontSizeArr[2] = HTMLFONTSZ3_DFLT;
        aFontSizeArr[3] = HTMLFONTSZ4_DFLT;
        aFontSizeArr[4] = HTMLFONTSZ5_DFLT;
        aFontSizeArr[5] = HTMLFONTSZ6_DFLT;
        aFontSizeArr[6] = HTMLFONTSZ7_DFLT;
    }
};

/* -----------------------------23.11.00 11:39--------------------------------

 ---------------------------------------------------------------------------*/
const Sequence<OUString>& SvxHtmlOptions::GetPropertyNames()
{
    static Sequence<OUString> aNames;
    if(!aNames.getLength())
    {
        static const char* aPropNames[] =
        {
            "Import/UnknownTag",                    //  0
            "Import/FontSetting",                   //  1
            "Import/FontSize/Size_1",               //  2
            "Import/FontSize/Size_2",               //  3
            "Import/FontSize/Size_3",               //  4
            "Import/FontSize/Size_4",               //  5
            "Import/FontSize/Size_5",               //  6
            "Import/FontSize/Size_6",               //  7
            "Import/FontSize/Size_7",               //  8
            "Export/Browser",                       //  9
            "Export/Basic",                         //  0
            "Export/PrintLayout",                   // 11
            "Export/LocalGraphic",                  // 12
            "Export/Warning",                       // 13
            "Export/Encoding",                      // 14
            "Import/NumbersEnglishUS"               // 15
        };
        const int nCount = sizeof(aPropNames) / sizeof(aPropNames[0]);
        aNames.realloc(nCount);
        OUString* pNames = aNames.getArray();
        for(int i = 0; i < nCount; i++)
            pNames[i] = C2U(aPropNames[i]);
    }
    return aNames;
}
// -----------------------------------------------------------------------
SvxHtmlOptions::SvxHtmlOptions() :
    ConfigItem(C2U("Office.Common/Filter/HTML"))
{
    pImp = new HtmlOptions_Impl;
    const Sequence<OUString>& aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed")
    if(aValues.getLength() == aNames.getLength())
    {
        pImp->nFlags = 0;
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case  0:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_UNKNOWN_TAGS;
                    break;//"Import/UnknownTag",
                    case  1:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_IGNORE_FONT_FAMILY;
                    break;//"Import/FontSetting",
                    case  2: pValues[nProp] >>= pImp->aFontSizeArr[0]; break;//"Import/FontSize/Size_1",
                    case  3: pValues[nProp] >>= pImp->aFontSizeArr[1]; break;//"Import/FontSize/Size_2",
                    case  4: pValues[nProp] >>= pImp->aFontSizeArr[2]; break;//"Import/FontSize/Size_3",
                    case  5: pValues[nProp] >>= pImp->aFontSizeArr[3]; break;//"Import/FontSize/Size_4",
                    case  6: pValues[nProp] >>= pImp->aFontSizeArr[4]; break;//"Import/FontSize/Size_5",
                    case  7: pValues[nProp] >>= pImp->aFontSizeArr[5]; break;//"Import/FontSize/Size_6",
                    case  8: pValues[nProp] >>= pImp->aFontSizeArr[6]; break;//"Import/FontSize/Size_7",
                    case  9://"Export/Browser",
                        {
                            sal_Int32 nExpMode;
//                          pValues[nProp] >>= pImp->nExportMode;
                            pValues[nProp] >>= nExpMode;
                            switch( nExpMode )
                            {
                                case 0:     nExpMode = HTML_CFG_HTML32;     break;
                                case 1:     nExpMode = HTML_CFG_MSIE_40;    break;
//                              case 2:     nExpMode = HTML_CFG_NS30;       break;  depricated
                                case 3:     nExpMode = HTML_CFG_WRITER;     break;
                                case 4:     nExpMode = HTML_CFG_NS40;       break;
                                case 5:     nExpMode = HTML_CFG_MSIE_40_OLD;break;
                                default:    nExpMode = HTML_CFG_NS40;       break;
                            }

                            pImp->nExportMode = nExpMode;
                        }
                        break;
                    case 10:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_STAR_BASIC;
                    break;//"Export/Basic",
                    case 11:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_PRINT_LAYOUT_EXTENSION;
                    break;//"Export/PrintLayout",
                    case 12:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_LOCAL_GRF;
                    break;//"Export/LocalGraphic",
                    case 13:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_IS_BASIC_WARNING;
                    break;//"Export/Warning"

                    case 14: pValues[nProp] >>= pImp->eEncoding;
                             pImp->bIsEncodingDefault = sal_False;
                    break;//"Export/Encoding"

                    case 15:
                        if(*(sal_Bool*)pValues[nProp].getValue())
                            pImp->nFlags |= HTMLCFG_NUMBERS_ENGLISH_US;
                    break;//"Import/NumbersEnglishUS"
                }
            }
        }
    }
}

// -----------------------------------------------------------------------
SvxHtmlOptions::~SvxHtmlOptions()
{
    delete pImp;
}
// -----------------------------------------------------------------------
void    SvxHtmlOptions::Commit()
{
    const Sequence<OUString>& aNames = GetPropertyNames();

//  const OUString* pNames = aNames.getConstArray();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

//  const Type& rType = ::getBooleanCppuType();
    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        sal_Bool bSet = sal_False;
        switch(nProp)
        {
            case  0: bSet = 0 != (pImp->nFlags & HTMLCFG_UNKNOWN_TAGS);break;//"Import/UnknownTag",
            case  1: bSet = 0 != (pImp->nFlags & HTMLCFG_IGNORE_FONT_FAMILY);break;//"Import/FontSetting",
            case  2: pValues[nProp] <<= pImp->aFontSizeArr[0];break;//"Import/FontSize/Size_1",
            case  3: pValues[nProp] <<= pImp->aFontSizeArr[1];break;//"Import/FontSize/Size_2",
            case  4: pValues[nProp] <<= pImp->aFontSizeArr[2];break;//"Import/FontSize/Size_3",
            case  5: pValues[nProp] <<= pImp->aFontSizeArr[3];break;//"Import/FontSize/Size_4",
            case  6: pValues[nProp] <<= pImp->aFontSizeArr[4];break;//"Import/FontSize/Size_5",
            case  7: pValues[nProp] <<= pImp->aFontSizeArr[5];break;//"Import/FontSize/Size_6",
            case  8: pValues[nProp] <<= pImp->aFontSizeArr[6];break;//"Import/FontSize/Size_7",
            case  9:                //"Export/Browser",
                {
                    sal_Int32 nExpMode = pImp->nExportMode;

                    switch( nExpMode )
                    {
                        case HTML_CFG_HTML32:       nExpMode = 0;   break;
                        case HTML_CFG_MSIE_40:      nExpMode = 1;   break;
//                      case HTML_CFG_NS30:         nExpMode = 2;   break;  depricated
                        case HTML_CFG_WRITER:       nExpMode = 3;   break;
                        case HTML_CFG_NS40:         nExpMode = 4;   break;
                        case HTML_CFG_MSIE_40_OLD:  nExpMode = 5;   break;
                        default:                    nExpMode = 4;   break;  // NS40
                    }

                    pValues[nProp] <<= nExpMode;
                    break;
                }
            case 10: bSet = 0 != (pImp->nFlags & HTMLCFG_STAR_BASIC);break;//"Export/Basic",
            case 11: bSet = 0 != (pImp->nFlags & HTMLCFG_PRINT_LAYOUT_EXTENSION);break;//"Export/PrintLayout",
            case 12: bSet = 0 != (pImp->nFlags & HTMLCFG_LOCAL_GRF);break;//"Export/LocalGraphic",
            case 13: bSet = 0 != (pImp->nFlags & HTMLCFG_IS_BASIC_WARNING);break;//"Export/Warning"
            case 14:
                if(!pImp->bIsEncodingDefault)
                    pValues[nProp] <<= pImp->eEncoding;
                break;//"Export/Encoding",
            case 15: bSet = 0 != (pImp->nFlags & HTMLCFG_NUMBERS_ENGLISH_US);break;//"Import/NumbersEnglishUS"
        }
        if(nProp < 2 || ( nProp > 9 && nProp < 14 ) || nProp == 15)
            pValues[nProp].setValue(&bSet, ::getCppuBooleanType());
    }
    PutProperties(aNames, aValues);
}
// -----------------------------------------------------------------------
USHORT  SvxHtmlOptions::GetFontSize(USHORT nPos) const
{
    if(nPos < HTML_FONT_COUNT)
        return (USHORT)pImp->aFontSizeArr[nPos];
    return 0;
}
// -----------------------------------------------------------------------
void SvxHtmlOptions::SetFontSize(USHORT nPos, USHORT nSize)
{
    if(nPos < HTML_FONT_COUNT)
    {
        pImp->aFontSizeArr[nPos] = nSize;
        SetModified();
    }
}

// -----------------------------------------------------------------------

// -----------------------------------------------------------------------


BOOL SvxHtmlOptions::IsImportUnknown() const
{
    return 0 != (pImp->nFlags & HTMLCFG_UNKNOWN_TAGS) ;
}

// -----------------------------------------------------------------------


void SvxHtmlOptions::SetImportUnknown(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |= HTMLCFG_UNKNOWN_TAGS;
    else
        pImp->nFlags &= ~HTMLCFG_UNKNOWN_TAGS;
    SetModified();
}

// -----------------------------------------------------------------------


USHORT  SvxHtmlOptions::GetExportMode() const
{
    return (USHORT)pImp->nExportMode;
}

// -----------------------------------------------------------------------


void SvxHtmlOptions::SetExportMode(USHORT nSet)
{
    if(nSet <= HTML_CFG_MAX )
    {
        pImp->nExportMode = nSet;
        SetModified();
        // Invalidierung, falls blinkender Text erlaubt/verboten wurde
        SfxViewFrame* pViewFrame = SfxViewFrame::Current();
        if(pViewFrame)
            pViewFrame->GetBindings().Invalidate(SID_DRAW_TEXT_MARQUEE);
    }
}

// -----------------------------------------------------------------------


BOOL SvxHtmlOptions::IsStarBasic() const
{
    return 0 != (pImp->nFlags & HTMLCFG_STAR_BASIC) ;
}

// -----------------------------------------------------------------------


void SvxHtmlOptions::SetStarBasic(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |=  HTMLCFG_STAR_BASIC;
    else
        pImp->nFlags &= ~HTMLCFG_STAR_BASIC;
    SetModified();
}

/*-----------------14.02.97 08.34-------------------

--------------------------------------------------*/

BOOL SvxHtmlOptions::IsSaveGraphicsLocal() const
{
    return 0 != (pImp->nFlags & HTMLCFG_LOCAL_GRF) ;
}
/*-----------------14.02.97 08.34-------------------

--------------------------------------------------*/
void SvxHtmlOptions::SetSaveGraphicsLocal(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |=  HTMLCFG_LOCAL_GRF;
    else
        pImp->nFlags &= ~HTMLCFG_LOCAL_GRF;
    SetModified();
}

/*-----------------10/21/97 08:34am-----------------

--------------------------------------------------*/

BOOL    SvxHtmlOptions::IsPrintLayoutExtension() const
{
    BOOL bRet = 0 != (pImp->nFlags & HTMLCFG_PRINT_LAYOUT_EXTENSION);
    switch( pImp->nExportMode )
    {
        case HTML_CFG_MSIE_40:
        case HTML_CFG_NS40  :
        case HTML_CFG_WRITER :
        break;
        default:
            bRet = FALSE;
    }
    return bRet;
}
/*-----------------10/21/97 08:34am-----------------

--------------------------------------------------*/
void    SvxHtmlOptions::SetPrintLayoutExtension(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |=  HTMLCFG_PRINT_LAYOUT_EXTENSION;
    else
        pImp->nFlags &= ~HTMLCFG_PRINT_LAYOUT_EXTENSION;
    SetModified();
}

/*-----------------10.07.98 10.02-------------------

--------------------------------------------------*/

BOOL SvxHtmlOptions::IsIgnoreFontFamily() const
{
    return 0 != (pImp->nFlags & HTMLCFG_IGNORE_FONT_FAMILY) ;
}
/*-----------------10.07.98 10.02-------------------

--------------------------------------------------*/
void SvxHtmlOptions::SetIgnoreFontFamily(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |=  HTMLCFG_IGNORE_FONT_FAMILY;
    else
        pImp->nFlags &= ~HTMLCFG_IGNORE_FONT_FAMILY;
    SetModified();
}
/* -----------------05.02.99 09:03-------------------
 *
 * --------------------------------------------------*/
BOOL SvxHtmlOptions::IsStarBasicWarning() const
{
    return 0 != (pImp->nFlags & HTMLCFG_IS_BASIC_WARNING) ;
}
/* -----------------05.02.99 09:03-------------------
 *
 * --------------------------------------------------*/
void SvxHtmlOptions::SetStarBasicWarning(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |=  HTMLCFG_IS_BASIC_WARNING;
    else
        pImp->nFlags &= ~HTMLCFG_IS_BASIC_WARNING;
    SetModified();
}

/*-----------------19.02.2001 18:40-----------------
 *
 * --------------------------------------------------*/
rtl_TextEncoding SvxHtmlOptions::GetTextEncoding() const
{
    rtl_TextEncoding eRet;
    if(pImp->bIsEncodingDefault)
        eRet = SvxTextEncodingBox::GetBestMimeEncoding();
    else
        eRet = (rtl_TextEncoding)pImp->eEncoding;
    return eRet;
}

/*-----------------19.02.2001 18:40-----------------
 *
 * --------------------------------------------------*/
void SvxHtmlOptions::SetTextEncoding( rtl_TextEncoding eEnc )
{
    pImp->eEncoding = eEnc;
    pImp->bIsEncodingDefault = sal_False;
    SetModified();
}
/* -----------------------------15.08.2001 12:01------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SvxHtmlOptions::IsDefaultTextEncoding() const
{
    return pImp->bIsEncodingDefault;
}

SvxHtmlOptions* SvxHtmlOptions::Get()
{
    if ( !pOptions )
        pOptions = new SvxHtmlOptions;
    return pOptions;
}


/* ---------------------- 2006-06-07T21:02+0200 ---------------------- */
BOOL SvxHtmlOptions::IsNumbersEnglishUS() const
{
    return 0 != (pImp->nFlags & HTMLCFG_NUMBERS_ENGLISH_US) ;
}


/* ---------------------- 2006-06-07T21:02+0200 ---------------------- */
void SvxHtmlOptions::SetNumbersEnglishUS(BOOL bSet)
{
    if(bSet)
        pImp->nFlags |=  HTMLCFG_NUMBERS_ENGLISH_US;
    else
        pImp->nFlags &= ~HTMLCFG_NUMBERS_ENGLISH_US;
    SetModified();
}
