/*************************************************************************
 *
 *  $RCSfile: xexptran.cxx,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *  last change: $Author: hr $ $Date: 2000-09-18 17:07:03 $
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

#pragma hdrstop

#ifndef _XEXPTRANSFORM_HXX
#include "xexptran.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star;

//////////////////////////////////////////////////////////////////////////////
// possible object types

#define IMP_SDXMLEXP_OBJ_ROTATE         0x0000
#define IMP_SDXMLEXP_OBJ_SCALE          0x0001
#define IMP_SDXMLEXP_OBJ_TRANSLATE      0x0002
#define IMP_SDXMLEXP_OBJ_SKEWX          0x0003
#define IMP_SDXMLEXP_OBJ_SKEWY          0x0004
#define IMP_SDXMLEXP_OBJ_MATRIX         0x0005

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// parsing help functions, no class
void Imp_SkipSpaces(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen)
{
    while(rPos < nLen
        && sal_Unicode(' ') == rStr.GetChar(rPos))
        rPos++;
}

void Imp_SkipSpacesAndOpeningBraces(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen)
{
    while(rPos < nLen
        && (sal_Unicode(' ') == rStr.GetChar(rPos) || sal_Unicode('(') == rStr.GetChar(rPos)))
        rPos++;
}

void Imp_SkipSpacesAndCommas(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen)
{
    while(rPos < nLen
        && (sal_Unicode(' ') == rStr.GetChar(rPos) || sal_Unicode(',') == rStr.GetChar(rPos)))
        rPos++;
}

void Imp_SkipSpacesAndClosingBraces(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen)
{
    while(rPos < nLen
        && (sal_Unicode(' ') == rStr.GetChar(rPos) || sal_Unicode(')') == rStr.GetChar(rPos)))
        rPos++;
}

sal_Bool Imp_IsOnNumberChar(const UniString& rStr, const xub_StrLen nPos,
    BOOL bSignIsNumber = TRUE, BOOL bPointIsNumber = FALSE)
{
    sal_Unicode aChar(rStr.GetChar(nPos));

    if((sal_Unicode('0') <= aChar && sal_Unicode('9') >= aChar)
        || (bSignIsNumber && (sal_Unicode('-') == aChar || sal_Unicode('+') == aChar))
        || (bPointIsNumber && sal_Unicode('.') == aChar)
    )
        return TRUE;
    return FALSE;
}

void Imp_SkipNumber(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen,
    BOOL bSignIsNumber = TRUE, BOOL bPointIsNumber = FALSE)
{
    sal_Bool bFirstSign(TRUE);

    while(rPos < nLen && Imp_IsOnNumberChar(rStr, rPos, bSignIsNumber && bFirstSign, bPointIsNumber))
    {
        rPos++;
        bFirstSign = FALSE;
    }
}

double Imp_GetNumberChar(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen,
    const SvXMLUnitConverter& rConv, double fRetval,
    BOOL bSignIsNumber = TRUE, BOOL bPointIsNumber = FALSE)
{
    UniString aNumberStr;
    sal_Int32 nNum(0L);
    sal_Bool bFirstSign(TRUE);

    while(rPos < nLen && Imp_IsOnNumberChar(rStr, rPos, bSignIsNumber && bFirstSign, !bPointIsNumber))
    {
        aNumberStr.Append(rStr.GetChar(rPos++));
        bFirstSign = FALSE;
    }

    if(aNumberStr.Len())
    {
        rConv.convertNumber(nNum, OUString(aNumberStr));
        fRetval = (double)nNum;
    }

    if(rPos < nLen && bPointIsNumber && sal_Unicode('.') == rStr.GetChar(rPos))
    {
        UniString aPostStr;
        rPos++;

        while(rPos < nLen && Imp_IsOnNumberChar(rStr, rPos, FALSE, bPointIsNumber))
            aPostStr.Append(rStr.GetChar(rPos++));

        aPostStr.EraseTrailingChars(sal_Unicode('0'));

        xub_StrLen nStrLen(aPostStr.Len());
        if(nStrLen)
        {
            nNum = 0L;
            rConv.convertNumber(nNum, OUString(aPostStr));
            fRetval += (double)nNum / pow(10.0, (double)nStrLen);
        }
    }

    return fRetval;
}

sal_Int32 Imp_GetNumberChar(const UniString& rStr, xub_StrLen& rPos, const xub_StrLen nLen,
    const SvXMLUnitConverter& rConv, sal_Int32 nRetval,
    BOOL bSignIsNumber = TRUE, BOOL bPointIsNumber = FALSE)
{
    return (sal_Int32)Imp_GetNumberChar(rStr,rPos, nLen, rConv,
        (double)nRetval, bSignIsNumber, bPointIsNumber);
}

void Imp_PutNumberChar(UniString& rStr, const SvXMLUnitConverter& rConv, double fValue)
{
    double fNum;
    double fFrac = modf(fValue, &fNum);
    sal_Int32 nNum(fNum);
    OUStringBuffer sStringBuffer;

    rConv.convertNumber(sStringBuffer, nNum);
    rStr += UniString(sStringBuffer.makeStringAndClear());

    if(fFrac != 0.0)
    {
        rStr += sal_Unicode('.');
        nNum = sal_Int32(fFrac * 1000000001.0);
        rConv.convertNumber(sStringBuffer, nNum);
        rStr += UniString(sStringBuffer.makeStringAndClear());
        rStr.EraseTrailingChars(sal_Unicode('0'));
    }
}

void Imp_PutNumberChar(UniString& rStr, const SvXMLUnitConverter& rConv, sal_Int32 nValue)
{
    OUStringBuffer sStringBuffer;
    rConv.convertNumber(sStringBuffer, nValue);
    rStr += UniString(sStringBuffer.makeStringAndClear());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// base class of all objects

struct ImpSdXMLExpTransObjBase
{
    sal_uInt16                  mnType;
    ImpSdXMLExpTransObjBase(sal_uInt16 nType)
    :   mnType(nType) {}
};

//////////////////////////////////////////////////////////////////////////////
// classes of objects, different sizes

struct ImpSdXMLExpTransObjRotate : public ImpSdXMLExpTransObjBase
{
    double                      mfRotate;
    ImpSdXMLExpTransObjRotate(double fVal)
    :   ImpSdXMLExpTransObjBase(IMP_SDXMLEXP_OBJ_ROTATE), mfRotate(fVal) {}
};
struct ImpSdXMLExpTransObjScale : public ImpSdXMLExpTransObjBase
{
    Vector2D                    maScale;
    ImpSdXMLExpTransObjScale(const Vector2D& rNew)
    :   ImpSdXMLExpTransObjBase(IMP_SDXMLEXP_OBJ_SCALE), maScale(rNew) {}
};
struct ImpSdXMLExpTransObjTranslate : public ImpSdXMLExpTransObjBase
{
    Vector2D                    maTranslate;
    ImpSdXMLExpTransObjTranslate(const Vector2D& rNew)
    :   ImpSdXMLExpTransObjBase(IMP_SDXMLEXP_OBJ_TRANSLATE), maTranslate(rNew) {}
};
struct ImpSdXMLExpTransObjSkewX : public ImpSdXMLExpTransObjBase
{
    double                      mfSkewX;
    ImpSdXMLExpTransObjSkewX(double fVal)
    :   ImpSdXMLExpTransObjBase(IMP_SDXMLEXP_OBJ_SKEWX), mfSkewX(fVal) {}
};
struct ImpSdXMLExpTransObjSkewY : public ImpSdXMLExpTransObjBase
{
    double                      mfSkewY;
    ImpSdXMLExpTransObjSkewY(double fVal)
    :   ImpSdXMLExpTransObjBase(IMP_SDXMLEXP_OBJ_SKEWY), mfSkewY(fVal) {}
};
struct ImpSdXMLExpTransObjMatrix : public ImpSdXMLExpTransObjBase
{
    Matrix3D                    maMatrix;
    ImpSdXMLExpTransObjMatrix(const Matrix3D& rNew)
    :   ImpSdXMLExpTransObjBase(IMP_SDXMLEXP_OBJ_MATRIX), maMatrix(rNew) {}
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// delete alle entries in list

void SdXMLImExTransform::EmptyList()
{
    while(maList.Count())
    {
        ImpSdXMLExpTransObjBase* pObj = maList.Remove(maList.Count() - 1);
        switch(pObj->mnType)
        {
            case IMP_SDXMLEXP_OBJ_ROTATE    : delete (ImpSdXMLExpTransObjRotate*)pObj; break;
            case IMP_SDXMLEXP_OBJ_SCALE     : delete (ImpSdXMLExpTransObjScale*)pObj; break;
            case IMP_SDXMLEXP_OBJ_TRANSLATE : delete (ImpSdXMLExpTransObjTranslate*)pObj; break;
            case IMP_SDXMLEXP_OBJ_SKEWX     : delete (ImpSdXMLExpTransObjSkewX*)pObj; break;
            case IMP_SDXMLEXP_OBJ_SKEWY     : delete (ImpSdXMLExpTransObjSkewY*)pObj; break;
            case IMP_SDXMLEXP_OBJ_MATRIX    : delete (ImpSdXMLExpTransObjMatrix*)pObj; break;
            default : DBG_ERROR("SdXMLImExTransform: impossible entry!"); break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// add members

void SdXMLImExTransform::AddRotate(double fNew)
{
    if(fNew != 0.0)
        maList.Insert(new ImpSdXMLExpTransObjRotate(fNew), LIST_APPEND);
}

void SdXMLImExTransform::AddScale(const Vector2D& rNew)
{
    if(rNew.X() != 0.0 || rNew.Y() != 0.0)
        maList.Insert(new ImpSdXMLExpTransObjScale(rNew), LIST_APPEND);
}

void SdXMLImExTransform::AddTranslate(const Vector2D& rNew)
{
    if(rNew.X() != 0.0 || rNew.Y() != 0.0)
        maList.Insert(new ImpSdXMLExpTransObjTranslate(rNew), LIST_APPEND);
}

void SdXMLImExTransform::AddSkewX(double fNew)
{
    if(fNew != 0.0)
        maList.Insert(new ImpSdXMLExpTransObjSkewX(fNew), LIST_APPEND);
}

void SdXMLImExTransform::AddSkewY(double fNew)
{
    if(fNew != 0.0)
        maList.Insert(new ImpSdXMLExpTransObjSkewY(fNew), LIST_APPEND);
}

void SdXMLImExTransform::AddMatrix(const Matrix3D& rNew)
{
    maList.Insert(new ImpSdXMLExpTransObjMatrix(rNew), LIST_APPEND);
}

//////////////////////////////////////////////////////////////////////////////
// gen string for export
const OUString& SdXMLImExTransform::GetExportString(const SvXMLUnitConverter& rConv)
{
    UniString aNewString;

    for(sal_uInt32 a(0L); a < maList.Count(); a++)
    {
        ImpSdXMLExpTransObjBase* pObj = maList.GetObject(a);
        switch(pObj->mnType)
        {
            case IMP_SDXMLEXP_OBJ_ROTATE    :
            {
                aNewString += UniString::CreateFromAscii("rotate (");
                Imp_PutNumberChar(aNewString, rConv, ((ImpSdXMLExpTransObjRotate*)pObj)->mfRotate);
                aNewString += sal_Unicode(')');
                break;
            }
            case IMP_SDXMLEXP_OBJ_SCALE     :
            {
                aNewString += UniString::CreateFromAscii("scale ()");
                break;
            }
            case IMP_SDXMLEXP_OBJ_TRANSLATE :
            {
                aNewString += UniString::CreateFromAscii("translate ()");
                break;
            }
            case IMP_SDXMLEXP_OBJ_SKEWX     :
            {
                aNewString += UniString::CreateFromAscii("skewX ()");
                break;
            }
            case IMP_SDXMLEXP_OBJ_SKEWY     :
            {
                aNewString += UniString::CreateFromAscii("skewY ()");
                break;
            }
            case IMP_SDXMLEXP_OBJ_MATRIX    :
            {
                aNewString += UniString::CreateFromAscii("matrix ()");
                break;
            }
            default : DBG_ERROR("SdXMLImExTransform: impossible entry!"); break;
        }

        // if not the last entry, add one space to next tag
        if(a+1 != maList.Count())
            aNewString += sal_Unicode(' ');
    }

    // fill string form UniString
    msString = OUString(aNewString);

    return msString;
}

//////////////////////////////////////////////////////////////////////////////
// for Import: constructor with string, parses it and generates entries
SdXMLImExTransform::SdXMLImExTransform(const OUString& rNew, const SvXMLUnitConverter& rConv)
:   msString(rNew)
{
    if(msString.getLength())
    {
        const UniString aStr(msString.getStr(), msString.getLength());
        const xub_StrLen nLen(aStr.Len());
        xub_StrLen nPos(0), nPos2;

        while(nPos < nLen)
        {
            // skip spaces
            Imp_SkipSpaces(aStr, nPos, nLen);

            // look for tag
            if(nPos < nLen)
            {
                if((nPos2 = aStr.SearchAscii("rotate", nPos)) != STRING_NOTFOUND)
                {
                    UniString aNumberStr;
                    double fValue(0.0);

                    nPos = nPos2 + 6;
                    Imp_SkipSpacesAndOpeningBraces(aStr, nPos, nLen);
                    fValue = Imp_GetNumberChar(aStr, nPos, nLen, rConv, fValue, TRUE, TRUE);
                    if(fValue != 0.0)
                        maList.Insert(new ImpSdXMLExpTransObjRotate(fValue), LIST_APPEND);

                    Imp_SkipSpacesAndClosingBraces(aStr, nPos, nLen);
                }
                else if((nPos2 = aStr.SearchAscii("scale", nPos)) != STRING_NOTFOUND)
                {
                    nPos = nPos2 + 5;
                }
                else if((nPos2 = aStr.SearchAscii("translate", nPos)) != STRING_NOTFOUND)
                {
                    nPos = nPos2 + 9;
                }
                else if((nPos2 = aStr.SearchAscii("skewX", nPos)) != STRING_NOTFOUND)
                {
                    nPos = nPos2 + 5;
                }
                else if((nPos2 = aStr.SearchAscii("skewY", nPos)) != STRING_NOTFOUND)
                {
                    nPos = nPos2 + 5;
                }
                else if((nPos2 = aStr.SearchAscii("matrix", nPos)) != STRING_NOTFOUND)
                {
                    nPos = nPos2 + 6;
                }
                else
                    nPos++;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// access from outside
ImpSdXMLExpTransObjBase* SdXMLImExTransform::FindObject(sal_uInt16 nType, sal_uInt32 nInd)
{
    while(nInd < maList.Count() && maList.GetObject(nInd)->mnType != nType)
        nInd++;
    if(nInd < maList.Count())
        return maList.GetObject(nInd);
    return 0L;
}

sal_Bool SdXMLImExTransform::FindRotate(double& rNew, sal_uInt32 nInd)
{
    ImpSdXMLExpTransObjBase* pObj = FindObject(IMP_SDXMLEXP_OBJ_ROTATE, nInd);
    if(pObj)
        rNew = ((ImpSdXMLExpTransObjRotate*)pObj)->mfRotate;
    return (pObj != 0L);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SdXMLImExViewBox::SdXMLImExViewBox(sal_Int32 nX, sal_Int32 nY, sal_Int32 nW, sal_Int32 nH)
:   mnX( nX ),
    mnY( nY ),
    mnW( nW ),
    mnH( nH )
{
}

SdXMLImExViewBox::SdXMLImExViewBox(const OUString& rNew, const SvXMLUnitConverter& rConv)
:   mnX( 0L ),
    mnY( 0L ),
    mnW( 1000L ),
    mnH( 1000L ),
    msString(rNew)
{
    if(msString.getLength())
    {
        const UniString aStr(msString.getStr(), msString.getLength());
        const xub_StrLen nLen(aStr.Len());
        xub_StrLen nPos(0);

        // skip starting spaces
        Imp_SkipSpaces(aStr, nPos, nLen);

        // get mX
        mnX = Imp_GetNumberChar(aStr, nPos, nLen, rConv, mnX, TRUE, TRUE);

        // skip spaces and commas
        Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

        // get mY
        mnY = Imp_GetNumberChar(aStr, nPos, nLen, rConv, mnY, TRUE, TRUE);

        // skip spaces and commas
        Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

        // get mW
        mnW = Imp_GetNumberChar(aStr, nPos, nLen, rConv, mnW, TRUE, TRUE);

        // skip spaces and commas
        Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

        // get mH
        mnH = Imp_GetNumberChar(aStr, nPos, nLen, rConv, mnH, TRUE, TRUE);
    }
}

const OUString& SdXMLImExViewBox::GetExportString(const SvXMLUnitConverter& rConv)
{
    UniString aNewString;

    Imp_PutNumberChar(aNewString, rConv, mnX);
    aNewString += sal_Unicode(' ');

    Imp_PutNumberChar(aNewString, rConv, mnY);
    aNewString += sal_Unicode(' ');

    Imp_PutNumberChar(aNewString, rConv, mnW);
    aNewString += sal_Unicode(' ');

    Imp_PutNumberChar(aNewString, rConv, mnH);

    // set new string
    msString = OUString(aNewString);

    return msString;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SdXMLImExPointsElement::SdXMLImExPointsElement(drawing::PointSequence* pPoints,
    const SdXMLImExViewBox& rViewBox,
    const awt::Point& rObjectPos,
    const awt::Size& rObjectSize,
    const SvXMLUnitConverter& rConv)
:   maPoly( 0L )
{
    // add polygon to string
    UniString aNewString;
    sal_Int32 nCnt(pPoints->getLength());
    awt::Point* pArray = pPoints->getArray();

    // last point same? Ignore it.
    if((pArray->X == (pArray + (nCnt - 1))->X)
        && (pArray->Y == (pArray + (nCnt - 1))->Y))
        nCnt--;

    // object size and ViewBox size different?
    sal_Bool bScale(rObjectSize.Width != rViewBox.GetWidth()
        || rObjectSize.Height != rViewBox.GetHeight());
    sal_Bool bTranslate(rViewBox.GetX() != 0L || rViewBox.GetY() != 0L);

    for(sal_Int32 a(0L); a < nCnt; a++)
    {
        // prepare coordinates
        sal_Int32 nX( pArray->X - rObjectPos.X );
        sal_Int32 nY( pArray->Y - rObjectPos.Y );

        if(bScale)
        {
            nX = (nX * rViewBox.GetWidth()) / rObjectSize.Width;
            nY = (nY * rViewBox.GetHeight()) / rObjectSize.Height;
        }

        if(bTranslate)
        {
            nX += rViewBox.GetX();
            nY += rViewBox.GetY();
        }

        // X and comma
        Imp_PutNumberChar(aNewString, rConv, nX);
        aNewString += sal_Unicode(',');

        // Y and space (not for last)
        Imp_PutNumberChar(aNewString, rConv, nY);
        if(a + 1 != nCnt)
            aNewString += sal_Unicode(' ');

        // next point
        pArray++;
    }

    // set new string
    msString = OUString(aNewString);
}

SdXMLImExPointsElement::SdXMLImExPointsElement(const OUString& rNew,
    const SdXMLImExViewBox& rViewBox,
    const awt::Point& rObjectPos,
    const awt::Size& rObjectSize,
    const SvXMLUnitConverter& rConv)
:   msString( rNew ),
    maPoly( 0L )
{
    // convert string to polygon
    const UniString aStr(msString.getStr(), msString.getLength());
    const xub_StrLen nLen(aStr.Len());
    xub_StrLen nPos(0);
    sal_Int32 nNumPoints(0L);

    // skip starting spaces
    Imp_SkipSpaces(aStr, nPos, nLen);

    // count points in first loop
    while(nPos < nLen)
    {
        // skip number
        Imp_SkipNumber(aStr, nPos, nLen, TRUE, TRUE);

        // skip spaces and commas
        Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

        // get mY
        Imp_SkipNumber(aStr, nPos, nLen, TRUE, TRUE);

        // skip spaces and commas
        Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

        // one more point
        nNumPoints++;
    }

    // second loop
    if(nNumPoints)
    {
        nPos = 0L;
        maPoly.realloc(1L);
        drawing::PointSequence* pOuterSequence = maPoly.getArray();
        pOuterSequence->realloc(nNumPoints);
        awt::Point* pInnerSequence = pOuterSequence->getArray();

        // object size and ViewBox size different?
        sal_Bool bScale(rObjectSize.Width != rViewBox.GetWidth()
            || rObjectSize.Height != rViewBox.GetHeight());
        sal_Bool bTranslate(rViewBox.GetX() != 0L || rViewBox.GetY() != 0L);

        // skip starting spaces
        Imp_SkipSpaces(aStr, nPos, nLen);

        while(nPos < nLen)
        {
            // prepare new parameter pair
            sal_Int32 nX(0L);
            sal_Int32 nY(0L);

            // get mX
            nX = Imp_GetNumberChar(aStr, nPos, nLen, rConv, nX, TRUE, TRUE);

            // skip spaces and commas
            Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

            // get mY
            nY = Imp_GetNumberChar(aStr, nPos, nLen, rConv, nY, TRUE, TRUE);

            // skip spaces and commas
            Imp_SkipSpacesAndCommas(aStr, nPos, nLen);

            // prepare parameters
            if(bTranslate)
            {
                nX -= rViewBox.GetX();
                nY -= rViewBox.GetY();
            }

            if(bScale)
            {
                nX = (nX * rObjectSize.Width) / rViewBox.GetWidth();
                nY = (nY * rObjectSize.Height) / rViewBox.GetHeight();
            }

            nX += rObjectPos.X;
            nY += rObjectPos.Y;

            // add new point
            *pInnerSequence = awt::Point( nX, nY );
            pInnerSequence++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SdXMLImExSvgDElement::SdXMLImExSvgDElement(const SdXMLImExViewBox& rViewBox)
:   mrViewBox( rViewBox ),
    mbIsClosed( FALSE ),
    mbIsCurve( FALSE ),
    mnLastX( 0L ),
    mnLastY( 0L ),
    maPoly( 0L ),
    maFlag( 0L )
{
}

void Imp_GetPrevPos(awt::Point*& pPrevPos1,
    drawing::PolygonFlags& aPrevFlag1,
    const BOOL bClosed, awt::Point* pPoints,
    drawing::PolygonFlags* pFlags, const sal_Int32 nPos,
    const sal_Int32 nCnt, const sal_Int32 nAdd)
{
    if(bClosed)
    {
        pPrevPos1 = pPoints + ((nPos + nCnt - nAdd) % nCnt);
        aPrevFlag1 = *(pFlags + ((nPos + nCnt - nAdd) % nCnt));
    }
    else if(nPos > (nAdd - 1))
    {
        pPrevPos1 = pPoints + (nPos - nAdd);
        aPrevFlag1 = *(pFlags + (nPos - nAdd));
    }
    else
        pPrevPos1 = 0L;
}

void Imp_PrepareCoorExport(sal_Int32& nX, sal_Int32& nY,
    const awt::Point* pPointArray, const awt::Point& rObjectPos,
    const awt::Size& rObjectSize, const SdXMLImExViewBox& mrViewBox,
    const BOOL bScale, const BOOL bTranslate)
{
    nX = pPointArray->X - rObjectPos.X;
    nY = pPointArray->Y - rObjectPos.Y;

    if(bScale)
    {
        nX = (nX * mrViewBox.GetWidth()) / rObjectSize.Width;
        nY = (nY * mrViewBox.GetHeight()) / rObjectSize.Height;
    }

    if(bTranslate)
    {
        nX += mrViewBox.GetX();
        nY += mrViewBox.GetY();
    }
}

void Imp_PutNumberCharWithSpace(UniString& rStr, const SvXMLUnitConverter& rConv, double fValue)
{
    const xub_StrLen aLen(rStr.Len());
    if(aLen)
        if(Imp_IsOnNumberChar(rStr, aLen - 1, FALSE, TRUE) && fValue >= 0.0)
            rStr += sal_Unicode(' ');
    Imp_PutNumberChar(rStr, rConv, fValue);
}

void Imp_PutNumberCharWithSpace(UniString& rStr, const SvXMLUnitConverter& rConv, sal_Int32 nValue)
{
    const xub_StrLen aLen(rStr.Len());
    if(aLen)
        if(Imp_IsOnNumberChar(rStr, aLen - 1, FALSE, TRUE) && nValue >= 0)
            rStr += sal_Unicode(' ');
    Imp_PutNumberChar(rStr, rConv, nValue);
}

void SdXMLImExSvgDElement::AddPolygon(
    drawing::PointSequence* pPoints,
    drawing::FlagSequence* pFlags,
    const awt::Point& rObjectPos,
    const awt::Size& rObjectSize,
    const SvXMLUnitConverter& rConv,
    sal_Bool bClosed, sal_Bool bRelative)
{
    // add polygon to string
    UniString aNewString(msString);
    sal_Int32 nCnt(pPoints->getLength());
    sal_Unicode aLastCommand;
    awt::Point* pPointArray = pPoints->getArray();
    drawing::PolygonFlags* pFlagArray = (pFlags) ? pFlags->getArray() : 0L;

    // object size and ViewBox size different?
    sal_Bool bScale(rObjectSize.Width != mrViewBox.GetWidth()
        || rObjectSize.Height != mrViewBox.GetHeight());
    sal_Bool bTranslate(mrViewBox.GetX() != 0L || mrViewBox.GetY() != 0L);

    // last point same? Ignore it, but ONLY when there are NO control
    // points. In that case the last point might be double, but this
    // is necessary. nCnt might NOT be changed to guarantee ring access
    // to single points!
    if(!pFlags
        && (pPointArray->X == (pPointArray + (nCnt - 1))->X)
        && (pPointArray->Y == (pPointArray + (nCnt - 1))->Y))
        nCnt--;

    // bezier poly, handle curves
    BOOL bDidWriteStart(FALSE);

    for(sal_Int32 a(0L); a < nCnt; a++)
    {
        if(!pFlags || drawing::PolygonFlags_CONTROL != *pFlagArray)
        {
            BOOL bDidWriteAsCurve(FALSE);

            if(bDidWriteStart)
            {
                if(pFlags)
                {
                    // real curve point, get previous to see if it's a control point
                    awt::Point* pPrevPos1;
                    drawing::PolygonFlags aPrevFlag1;

                    Imp_GetPrevPos(pPrevPos1, aPrevFlag1, bClosed, pPoints->getArray(),
                        pFlags->getArray(), a, nCnt, 1);

                    if(pPrevPos1 && drawing::PolygonFlags_CONTROL == aPrevFlag1)
                    {
                        // get previous2 to see if it's a control point, too
                        awt::Point* pPrevPos2;
                        drawing::PolygonFlags aPrevFlag2;

                        Imp_GetPrevPos(pPrevPos2, aPrevFlag2, bClosed, pPoints->getArray(),
                            pFlags->getArray(), a, nCnt, 2);

                        if(pPrevPos2 && drawing::PolygonFlags_CONTROL == aPrevFlag2)
                        {
                            // get previous3 to see if it's a curve point and if,
                            // if it is fully symmetric or not
                            awt::Point* pPrevPos3;
                            drawing::PolygonFlags aPrevFlag3;

                            Imp_GetPrevPos(pPrevPos3, aPrevFlag3, bClosed, pPoints->getArray(),
                                pFlags->getArray(), a, nCnt, 3);

                            if(pPrevPos3)
                            {
                                sal_Bool bUseSWord(FALSE);

                                if(drawing::PolygonFlags_SYMMETRIC == aPrevFlag3)
                                {
                                    // get previous4 to see if it's a control point
                                    awt::Point* pPrevPos4;
                                    drawing::PolygonFlags aPrevFlag4;

                                    Imp_GetPrevPos(pPrevPos4, aPrevFlag4, bClosed, pPoints->getArray(),
                                        pFlags->getArray(), a, nCnt, 4);

                                    if(drawing::PolygonFlags_CONTROL == aPrevFlag4)
                                    {
                                        // okay, prevPos3 is symmetric (c2) and prevPos4
                                        // is existing control point, the 's' statement can be used
                                        bUseSWord = TRUE;
                                    }
                                }

                                // prepare coordinates
                                sal_Int32 nX, nY, nX2, nY2;

                                Imp_PrepareCoorExport(nX, nY, pPointArray, rObjectPos, rObjectSize,
                                    mrViewBox, bScale, bTranslate);
                                Imp_PrepareCoorExport(nX2, nY2, pPrevPos1, rObjectPos, rObjectSize,
                                    mrViewBox, bScale, bTranslate);

                                if(bUseSWord)
                                {
                                    // write a shorthand/smooth curveto entry (S)
                                    if(bRelative)
                                    {
                                        if(aLastCommand != sal_Unicode('s'))
                                            aNewString += sal_Unicode('s');

                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX2 - mnLastX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY2 - mnLastY);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX - mnLastX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY - mnLastY);

                                        aLastCommand = sal_Unicode('s');
                                    }
                                    else
                                    {
                                        if(aLastCommand != sal_Unicode('S'))
                                            aNewString += sal_Unicode('S');

                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX2);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY2);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY);

                                        aLastCommand = sal_Unicode('S');
                                    }
                                }
                                else
                                {
                                    // prepare coordinates
                                    sal_Int32 nX1, nY1;

                                    Imp_PrepareCoorExport(nX1, nY1, pPrevPos2, rObjectPos, rObjectSize,
                                        mrViewBox, bScale, bTranslate);

                                    // write a curveto entry (C)
                                    if(bRelative)
                                    {
                                        if(aLastCommand != sal_Unicode('c'))
                                            aNewString += sal_Unicode('c');

                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX1 - mnLastX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY1 - mnLastY);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX2 - mnLastX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY2 - mnLastY);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX - mnLastX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY - mnLastY);

                                        aLastCommand = sal_Unicode('c');
                                    }
                                    else
                                    {
                                        if(aLastCommand != sal_Unicode('C'))
                                            aNewString += sal_Unicode('C');

                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX1);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY1);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX2);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY2);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX);
                                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY);

                                        aLastCommand = sal_Unicode('C');
                                    }
                                }

                                // remember that current point IS written
                                bDidWriteAsCurve = TRUE;

                                // remember new last position
                                mnLastX = nX;
                                mnLastY = nY;
                            }
                        }
                    }
                }
            }

            if(!bDidWriteAsCurve)
            {
                // current point not yet written, prepare coordinates
                sal_Int32 nX, nY;

                Imp_PrepareCoorExport(nX, nY, pPointArray, rObjectPos, rObjectSize,
                    mrViewBox, bScale, bTranslate);

                if(bDidWriteStart)
                {
                    // write as normal point
                    if(mnLastX == nX)
                    {
                        if(bRelative)
                        {
                            if(aLastCommand != sal_Unicode('v'))
                                aNewString += sal_Unicode('v');

                            Imp_PutNumberCharWithSpace(aNewString, rConv, nY - mnLastY);

                            aLastCommand = sal_Unicode('v');
                        }
                        else
                        {
                            if(aLastCommand != sal_Unicode('V'))
                                aNewString += sal_Unicode('V');

                            Imp_PutNumberCharWithSpace(aNewString, rConv, nY);

                            aLastCommand = sal_Unicode('V');
                        }
                    }
                    else if(mnLastY == nY)
                    {
                        if(bRelative)
                        {
                            if(aLastCommand != sal_Unicode('h'))
                                aNewString += sal_Unicode('h');

                            Imp_PutNumberCharWithSpace(aNewString, rConv, nX - mnLastX);

                            aLastCommand = sal_Unicode('h');
                        }
                        else
                        {
                            if(aLastCommand != sal_Unicode('H'))
                                aNewString += sal_Unicode('H');

                            Imp_PutNumberCharWithSpace(aNewString, rConv, nX);

                            aLastCommand = sal_Unicode('H');
                        }
                    }
                    else
                    {
                        if(bRelative)
                        {
                            if(aLastCommand != sal_Unicode('l'))
                                aNewString += sal_Unicode('l');

                            Imp_PutNumberCharWithSpace(aNewString, rConv, nX - mnLastX);
                            Imp_PutNumberCharWithSpace(aNewString, rConv, nY - mnLastY);

                            aLastCommand = sal_Unicode('l');
                        }
                        else
                        {
                            if(aLastCommand != sal_Unicode('L'))
                                aNewString += sal_Unicode('L');

                            Imp_PutNumberCharWithSpace(aNewString, rConv, nX);
                            Imp_PutNumberCharWithSpace(aNewString, rConv, nY);

                            aLastCommand = sal_Unicode('L');
                        }
                    }
                }
                else
                {
                    // write as start point
                    if(bRelative)
                    {
                        aNewString += sal_Unicode('m');

                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX - mnLastX);
                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY - mnLastY);

                        aLastCommand = sal_Unicode('l');
                    }
                    else
                    {
                        aNewString += sal_Unicode('M');

                        Imp_PutNumberCharWithSpace(aNewString, rConv, nX);
                        Imp_PutNumberCharWithSpace(aNewString, rConv, nY);

                        aLastCommand = sal_Unicode('L');
                    }

                    // remember start written
                    bDidWriteStart = TRUE;
                }

                // remember new last position
                mnLastX = nX;
                mnLastY = nY;
            }
        }

        // next point
        pPointArray++;
        pFlagArray++;
    }

    // close path if closed poly
    if(bClosed)
    {
        if(bRelative)
            aNewString += sal_Unicode('z');
        else
            aNewString += sal_Unicode('Z');
    }

    // set new string
    msString = OUString(aNewString);
}

void Imp_SkipNumberAndSpacesAndCommas(const UniString& rStr, xub_StrLen& rPos,
    const xub_StrLen nLen, BOOL bSignIsNumber = TRUE, BOOL bPointIsNumber = FALSE)
{
    Imp_SkipNumber(rStr, rPos, nLen, bSignIsNumber, bPointIsNumber);
    Imp_SkipSpacesAndCommas(rStr, rPos, nLen);
}

sal_Int32 Imp_ImportNumberAndSpaces(
    sal_Int32 nRetval, const UniString& rStr, xub_StrLen& rPos,
    const xub_StrLen nLen, const SvXMLUnitConverter& rConv,
    BOOL bSignIsNumber = TRUE, BOOL bPointIsNumber = FALSE)
{
    nRetval = Imp_GetNumberChar(rStr, rPos, nLen, rConv, nRetval, bSignIsNumber, bPointIsNumber);
    Imp_SkipSpacesAndCommas(rStr, rPos, nLen);
    return nRetval;
}

void Imp_PrepareCoorImport(sal_Int32& nX, sal_Int32& nY,
    const awt::Point& rObjectPos, const awt::Size& rObjectSize,
    const SdXMLImExViewBox& rViewBox, const BOOL bScale, const BOOL bTranslate)
{
    if(bTranslate)
    {
        nX -= rViewBox.GetX();
        nY -= rViewBox.GetY();
    }

    if(bScale)
    {
        nX = (nX * rObjectSize.Width) / rViewBox.GetWidth();
        nY = (nY * rObjectSize.Height) / rViewBox.GetHeight();
    }

    nX += rObjectPos.X;
    nY += rObjectPos.Y;
}

void Imp_AddExportPoints(sal_Int32 nX, sal_Int32 nY,
    awt::Point* pPoints, drawing::PolygonFlags* pFlags,
    const sal_Int32 nInnerIndex,
    drawing::PolygonFlags eFlag)
{
    if(pPoints)
        pPoints[nInnerIndex] = awt::Point( nX, nY );

    if(pFlags)
        pFlags[nInnerIndex] = eFlag;
}

void Imp_CalcVectorValues(Vector2D& aVec1, Vector2D& aVec2, sal_Bool& bSameLength, sal_Bool& bSameDirection)
{
    sal_Int32 nLen1 = (sal_Int32)(aVec1.GetLength() + 0.5);
    sal_Int32 nLen2 = (sal_Int32)(aVec2.GetLength() + 0.5);
    aVec1.Normalize();
    aVec2.Normalize();
    aVec1 += aVec2;
    sal_Int32 nLen3 = (sal_Int32)((aVec1.GetLength() * ((nLen1 + nLen2) / 2.0))+ 0.5);

    bSameLength = (abs(nLen1 - nLen2) < 3);
    bSameDirection = (nLen3 < 3);
}

SdXMLImExSvgDElement::SdXMLImExSvgDElement(const OUString& rNew,
    const SdXMLImExViewBox& rViewBox,
    const awt::Point& rObjectPos,
    const awt::Size& rObjectSize,
    const SvXMLUnitConverter& rConv)
:   msString( rNew ),
    mrViewBox( rViewBox ),
    mbIsClosed( FALSE ),
    mbIsCurve( FALSE ),
    mnLastX( 0L ),
    mnLastY( 0L ),
    maPoly( 0L ),
    maFlag( 0L )
{
    // convert string to polygon
    const UniString aStr(msString.getStr(), msString.getLength());
    const xub_StrLen nLen(aStr.Len());
    xub_StrLen nPos(0);
    sal_Int32 nNumPolys(0L);
    sal_Bool bQuadraticBeziers(FALSE);
    sal_Bool bEllipticalArc(FALSE);

    // object size and ViewBox size different?
    sal_Bool bScale(rObjectSize.Width != mrViewBox.GetWidth()
        || rObjectSize.Height != mrViewBox.GetHeight());
    sal_Bool bTranslate(mrViewBox.GetX() != 0L || mrViewBox.GetY() != 0L);

    // first loop: count polys and get flags
    Imp_SkipSpaces(aStr, nPos, nLen);
    while(nPos < nLen)
    {
        switch(aStr.GetChar(nPos++))
        {
            case 'Z' :
            case 'z' :
            {
                mbIsClosed = TRUE;
                break;
            }
            case 'M' :
            case 'm' :
            {
                nNumPolys++;
                break;
            }
            case 'S' :
            case 's' :
            case 'C' :
            case 'c' :
            {
                mbIsCurve = TRUE;
                break;
            }
            case 'L' :
            case 'l' :
            case 'H' :
            case 'h' :
            case 'V' :
            case 'v' :
            {
                // normal, interpreted values. All okay.
                break;
            }
            case 'Q' :
            case 'q' :
            case 'T' :
            case 't' :
            {
                bQuadraticBeziers = TRUE;
                break;
            }
            case 'A' :
            case 'a' :
            {
                bEllipticalArc = TRUE;
                break;
            }
        }
    }

    if(bQuadraticBeziers || bEllipticalArc)
        DBG_ERROR("XMLIMP: non-interpreted tags in svg:d element!");

    if(nNumPolys)
    {
        // alloc arrays
        maPoly.realloc(nNumPolys);
        if(IsCurve())
            maFlag.realloc(nNumPolys);

        // get outer sequences
        drawing::PointSequence* pOuterSequence = maPoly.getArray();
        drawing::FlagSequence* pOuterFlags = (IsCurve()) ? maFlag.getArray() : 0L;

        // prepare new loop, count
        sal_uInt32 nPointCount(0L);
        nPos = 0;
        Imp_SkipSpaces(aStr, nPos, nLen);
        while(nPos < nLen)
        {
            switch(aStr.GetChar(nPos))
            {
                case 'z' :
                case 'Z' :
                {
                    nPos++;
                    Imp_SkipSpaces(aStr, nPos, nLen);
                    break;
                }
                case 'm' :
                case 'M' :
                {
                    // new poly starts, alloc current poly (if points are used)
                    if(nPointCount)
                    {
                        pOuterSequence->realloc(nPointCount);
                        pOuterSequence++;
                        if(pOuterFlags)
                        {
                            pOuterFlags->realloc(nPointCount);
                            pOuterFlags++;
                        }
                        nPointCount = 0L;
                    }
                    // NO break, continue in next case
                }
                case 'L' :
                case 'l' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        nPointCount++;
                    }
                    break;
                }
                case 'H' :
                case 'h' :
                case 'V' :
                case 'v' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        nPointCount++;
                    }
                    break;
                }
                case 'S' :
                case 's' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        nPointCount += 3;
                    }
                    break;
                }
                case 'C' :
                case 'c' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        Imp_SkipNumberAndSpacesAndCommas(aStr, nPos, nLen, TRUE, TRUE);
                        nPointCount += 3;
                    }
                    break;
                }
                // more cases...
            }
        }

        // alloc last poly (when points used)
        if(nPointCount)
        {
            pOuterSequence->realloc(nPointCount);
            pOuterSequence++;
            if(pOuterFlags)
            {
                pOuterFlags->realloc(nPointCount);
                pOuterFlags++;
            }
        }

        // set pointers back
        pOuterSequence = maPoly.getArray();
        pOuterFlags = (IsCurve()) ? maFlag.getArray() : 0L;
        awt::Point* pInnerSequence = 0L;
        drawing::PolygonFlags* pInnerFlags = 0L;
        sal_uInt32 nInnerIndex(0L);

        // prepare new loop, read points
        nPos = 0;
        Imp_SkipSpaces(aStr, nPos, nLen);

        while(nPos < nLen)
        {
            BOOL bRelative(FALSE);

            switch(aStr.GetChar(nPos))
            {
                case 'z' :
                case 'Z' :
                {
                    nPos++;
                    Imp_SkipSpaces(aStr, nPos, nLen);
                    break;
                }

                case 'm' :
                {
                    bRelative = TRUE;
                }
                case 'M' :
                {
                    // next poly
                    pInnerSequence = pOuterSequence->getArray();
                    pOuterSequence++;

                    if(pOuterFlags)
                    {
                        pInnerFlags = pOuterFlags->getArray();
                        pOuterFlags++;
                    }

                    nInnerIndex = 0L;

                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        sal_Int32 nX(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));

                        if(bRelative)
                        {
                            nX += mnLastX;
                            nY += mnLastY;
                        }

                        // set last position
                        mnLastX = nX;
                        mnLastY = nY;

                        // calc transform and add point and flag
                        Imp_PrepareCoorImport(nX, nY, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_AddExportPoints(nX, nY, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_NORMAL);
                    }
                    break;
                }

                case 'l' :
                {
                    bRelative = TRUE;
                }
                case 'L' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        sal_Int32 nX(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));

                        if(bRelative)
                        {
                            nX += mnLastX;
                            nY += mnLastY;
                        }

                        // set last position
                        mnLastX = nX;
                        mnLastY = nY;

                        // calc transform and add point and flag
                        Imp_PrepareCoorImport(nX, nY, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_AddExportPoints(nX, nY, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_NORMAL);
                    }
                    break;
                }

                case 'h' :
                {
                    bRelative = TRUE;
                }
                case 'H' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        sal_Int32 nX(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY(mnLastY);

                        if(bRelative)
                            nX += mnLastX;

                        // set last position
                        mnLastX = nX;

                        // calc transform and add point and flag
                        Imp_PrepareCoorImport(nX, nY, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_AddExportPoints(nX, nY, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_NORMAL);
                    }
                    break;
                }

                case 'v' :
                {
                    bRelative = TRUE;
                }
                case 'V' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        sal_Int32 nX(mnLastX);
                        sal_Int32 nY(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));

                        if(bRelative)
                            nY += mnLastY;

                        // set last position
                        mnLastY = nY;

                        // calc transform and add point and flag
                        Imp_PrepareCoorImport(nX, nY, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_AddExportPoints(nX, nY, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_NORMAL);
                    }
                    break;
                }

                case 's' :
                {
                    bRelative = TRUE;
                }
                case 'S' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        sal_Int32 nX1;
                        sal_Int32 nY1;
                        sal_Int32 nX2(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY2(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nX(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));

                        if(bRelative)
                        {
                            nX2 += mnLastX;
                            nY2 += mnLastY;
                            nX += mnLastX;
                            nY += mnLastY;
                        }

                        // set last position
                        mnLastX = nX;
                        mnLastY = nY;

                        // calc transform for new points
                        Imp_PrepareCoorImport(nX2, nY2, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_PrepareCoorImport(nX, nY, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);

                        // one more thing is known: the previous real point is PolygonFlags_SYMMETRIC
                        // and the Point X1,Y1 can be constructed by mirroring the point before it.
                        nX1 = nX2;
                        nY1 = nY2;
                        if(nInnerIndex)
                        {
                            awt::Point aPPrev1 = pInnerSequence[nInnerIndex - 1];
                            drawing::PolygonFlags aFPrev1 = pInnerFlags[nInnerIndex - 1];

                            if(nInnerIndex > 1)
                            {
                                awt::Point aPPrev2 = pInnerSequence[nInnerIndex - 2];
                                nX1 = aPPrev1.X -(aPPrev2.X - aPPrev1.X);
                                nY1 = aPPrev1.Y -(aPPrev2.Y - aPPrev1.Y);
                            }

                            // set curve point to symmetric
                            if(drawing::PolygonFlags_SYMMETRIC != aFPrev1)
                                pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_SYMMETRIC;
                        }

                        // add calculated control point
                        Imp_AddExportPoints(nX1, nY1, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_CONTROL);

                        // add new points and set flags
                        Imp_AddExportPoints(nX2, nY2, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_CONTROL);
                        Imp_AddExportPoints(nX, nY, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_SMOOTH);
                    }
                    break;
                }

                case 'c' :
                {
                    bRelative = TRUE;
                }
                case 'C' :
                {
                    nPos++;
                    while(nPos < nLen && Imp_IsOnNumberChar(aStr, nPos, TRUE, TRUE))
                    {
                        Imp_SkipSpaces(aStr, nPos, nLen);
                        sal_Int32 nX1(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY1(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nX2(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY2(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nX(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));
                        sal_Int32 nY(Imp_ImportNumberAndSpaces(0L, aStr, nPos, nLen, rConv, TRUE, TRUE));

                        if(bRelative)
                        {
                            nX1 += mnLastX;
                            nY1 += mnLastY;
                            nX2 += mnLastX;
                            nY2 += mnLastY;
                            nX += mnLastX;
                            nY += mnLastY;
                        }

                        // set last position
                        mnLastX = nX;
                        mnLastY = nY;

                        // calc transform for new points
                        Imp_PrepareCoorImport(nX1, nY1, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_PrepareCoorImport(nX2, nY2, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);
                        Imp_PrepareCoorImport(nX, nY, rObjectPos, rObjectSize, mrViewBox, bScale, bTranslate);

                        // calc information
                        if(nInnerIndex)
                        {
                            awt::Point aPPrev1 = pInnerSequence[nInnerIndex - 1];
                            drawing::PolygonFlags aFPrev1 = pInnerFlags[nInnerIndex - 1];

                            if(nInnerIndex > 1)
                            {
                                awt::Point aPPrev2 = pInnerSequence[nInnerIndex - 2];
                                drawing::PolygonFlags aFPrev2 = pInnerFlags[nInnerIndex - 2];
                                Vector2D aVec1(aPPrev2.X - aPPrev1.X, aPPrev2.Y - aPPrev1.Y);
                                Vector2D aVec2(nX1 - aPPrev1.X, nY1 - aPPrev1.Y);
                                sal_Bool bSameLength(FALSE);
                                sal_Bool bSameDirection(FALSE);

                                // get vector values
                                Imp_CalcVectorValues(aVec1, aVec2, bSameLength, bSameDirection);

                                if(drawing::PolygonFlags_CONTROL == aFPrev2)
                                {
                                    // point before is a control point

                                    if(bSameDirection)
                                    {
                                        if(bSameLength)
                                        {
                                            // set to PolygonFlags_SYMMETRIC
                                            pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_SYMMETRIC;
                                        }
                                        else
                                        {
                                            // set to PolygonFlags_SMOOTH
                                            pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_SMOOTH;
                                        }
                                    }
                                    else
                                    {
                                        // set to PolygonFlags_NORMAL
                                        pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_NORMAL;
                                    }
                                }
                                else
                                {
                                    // point before is a simple curve point
                                    if(bSameDirection)
                                    {
                                        // set to PolygonFlags_SMOOTH
                                        pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_SMOOTH;
                                    }
                                    else
                                    {
                                        // set to PolygonFlags_NORMAL
                                        pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_NORMAL;
                                    }
                                }
                            }
                            else
                            {
                                // no point before starpoint, set type to PolygonFlags_NORMAL
                                pInnerFlags[nInnerIndex - 1] = drawing::PolygonFlags_NORMAL;
                            }
                        }

                        // add new points and set flags
                        Imp_AddExportPoints(nX1, nY1, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_CONTROL);
                        Imp_AddExportPoints(nX2, nY2, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_CONTROL);
                        Imp_AddExportPoints(nX, nY, pInnerSequence, pInnerFlags, nInnerIndex++, drawing::PolygonFlags_SMOOTH);
                    }
                    break;
                }
                // more cases...
            }
        }
    }
}


