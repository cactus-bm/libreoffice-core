/*************************************************************************
 *
 *  $RCSfile: misccfg.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 20:47:30 $
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

#ifndef _ZFORLIST_HXX //autogen
#include "zforlist.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#include "misccfg.hxx"

#define DEFAULT_TAB 2000

#define DEF_INCH    2540L
#define DEF_RELTWIP 1440L

using namespace rtl;
using namespace com::sun::star::uno;

#define C2U(cChar) OUString::createFromAscii(cChar)
/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/
SfxMiscCfg::SfxMiscCfg() :
    ConfigItem(C2U("Office.Common") ),
    nYear2000( SvNumberFormatter::GetYear2000Default() ),
    bNotFound (FALSE),
    bPaperSize(FALSE),
    bPaperOrientation (FALSE)
{
    Load();
}
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
SfxMiscCfg::~SfxMiscCfg()
{
}
/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SfxMiscCfg::SetNotFoundWarning( BOOL bSet)
{
    if(bNotFound != bSet)
        SetModified();
    bNotFound = bSet;
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SfxMiscCfg::SetPaperSizeWarning( BOOL bSet)
{
    if(bPaperSize != bSet)
        SetModified();
    bPaperSize = bSet;
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/
void SfxMiscCfg::SetPaperOrientationWarning( BOOL bSet)
{
    if(bPaperOrientation != bSet)
        SetModified();
    bPaperOrientation = bSet;
}
/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SfxMiscCfg::SetYear2000( sal_Int32 nSet )
{
    if(nYear2000 != nSet)
        SetModified();
    nYear2000 = nSet;
}
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
const Sequence<OUString>& SfxMiscCfg::GetPropertyNames()
{
    static Sequence<OUString> aNames;
    if(!aNames.getLength())
    {
        static const char* aPropNames[] =
        {
               "Print/Warning/PaperSize",               //  0
               "Print/Warning/PaperOrientation",        //  1
               "Print/Warning/NotFound",                //  2
            "DateFormat/TwoDigitYear",              //  3
        };
        const int nCount = 4;
        aNames.realloc(nCount);
        OUString* pNames = aNames.getArray();
        for(int i = 0; i < nCount; i++)
            pNames[i] = OUString::createFromAscii(aPropNames[i]);
    }
    return aNames;
}
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
void SfxMiscCfg::Load()
{
    const Sequence<OUString>& aNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(aNames);
    EnableNotification(aNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed")
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case  0: bPaperSize        = *(sal_Bool*)pValues[nProp].getValue(); break;      //"Print/Warning/PaperSize",
                    case  1: bPaperOrientation = *(sal_Bool*)pValues[nProp].getValue();  break;     //"Print/Warning/PaperOrientation",
                    case  2: bNotFound         = *(sal_Bool*)pValues[nProp].getValue()  ;  break;   //"Print/Warning/NotFound",
                    case  3: pValues[nProp] >>= nYear2000;break;                                    //"DateFormat/TwoDigitYear",
                }
            }
        }
    }
}
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
void SfxMiscCfg::Notify( const com::sun::star::uno::Sequence<rtl::OUString>& aPropertyNames)
{
    Load();
}
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
void SfxMiscCfg::Commit()
{
    const Sequence<OUString>& aNames = GetPropertyNames();
    const OUString* pNames = aNames.getConstArray();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    const Type& rType = ::getBooleanCppuType();
    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case  0: pValues[nProp].setValue(&bPaperSize, rType);break;  //"Print/Warning/PaperSize",
            case  1: pValues[nProp].setValue(&bPaperOrientation, rType);break;     //"Print/Warning/PaperOrientation",
            case  2: pValues[nProp].setValue(&bNotFound, rType);break;   //"Print/Warning/NotFound",
            case  3: pValues[nProp] <<= nYear2000;break;                 //"DateFormat/TwoDigitYear",
        }
    }
    PutProperties(aNames, aValues);
}

