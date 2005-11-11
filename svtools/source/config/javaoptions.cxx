/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: javaoptions.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-11-11 08:51:11 $
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
#ifndef GCC
#pragma hdrstop
#endif

#ifndef _SVTOOLS_JAVAPTIONS_HXX
#include <javaoptions.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#include <rtl/logfile.hxx>


using namespace ::com::sun::star::uno;
using namespace ::rtl;

#define C2U(cChar) OUString::createFromAscii(cChar)
#define CFG_READONLY_DEFAULT    sal_False
/* -----------------------------10.04.01 12:39--------------------------------

 ---------------------------------------------------------------------------*/
class SvtExecAppletsItem_Impl : public utl::ConfigItem
{
    sal_Bool  bExecute;
    sal_Bool  bRO;
public:
    SvtExecAppletsItem_Impl();

    virtual void    Commit();

    sal_Bool IsExecuteApplets() const {return bExecute;}
    void     SetExecuteApplets(sal_Bool bSet);
    sal_Bool IsReadOnly() const {return bRO;}
};
/* -----------------------------10.02.2003 07:46------------------------------

 ---------------------------------------------------------------------------*/
void SvtExecAppletsItem_Impl::SetExecuteApplets(sal_Bool bSet)
{
    OSL_ENSURE(!bRO, "SvtExecAppletsItem_Impl::SetExecuteApplets()\nYou tried to write on a readonly value!\n");
    if (!bRO)
    {
        bExecute = bSet;
        SetModified();
    }
}
/* -----------------------------18.05.01 14:44--------------------------------

 ---------------------------------------------------------------------------*/
SvtExecAppletsItem_Impl::SvtExecAppletsItem_Impl() :
        utl::ConfigItem(C2U("Office.Common/Java/Applet")),
        bRO            (CFG_READONLY_DEFAULT            ),
        bExecute       (sal_False                       )
{
    RTL_LOGFILE_CONTEXT(aLog, "svtools (???) SvtExecAppletsItem_Impl::SvtExecAppletsItem_Impl()");

    Sequence< OUString > aNames(1);
    aNames.getArray()[0] = C2U("Enable");
    Sequence< Any > aValues = GetProperties(aNames);
    Sequence< sal_Bool > aROStates = GetReadOnlyStates(aNames);
    const Any* pValues = aValues.getConstArray();
    const sal_Bool* pROStates = aROStates.getConstArray();
    if(aValues.getLength() && aROStates.getLength() && pValues[0].hasValue())
    {
        bExecute = *(sal_Bool*)pValues[0].getValue();
        bRO = pROStates[0];
    }
}
void    SvtExecAppletsItem_Impl::Commit()
{
    if (bRO)
        return;

    Sequence< OUString > aNames(1);
    aNames.getArray()[0] = C2U("Enable");
    Sequence< Any > aValues(1);
    aValues.getArray()[0].setValue(&bExecute, ::getBooleanCppuType());
    PutProperties(aNames, aValues);
}


struct SvtJavaOptions_Impl
{
    SvtExecAppletsItem_Impl aExecItem;
    Sequence<OUString>      aPropertyNames;
    sal_Bool                bEnabled;
    sal_Bool                bSecurity;
    sal_Int32               nNetAccess;
    rtl::OUString           sUserClassPath;

    sal_Bool                bROEnabled;
    sal_Bool                bROSecurity;
    sal_Bool                bRONetAccess;
    sal_Bool                bROUserClassPath;

    SvtJavaOptions_Impl() :
        aPropertyNames(4),
        bEnabled            (sal_False),
        bSecurity           (sal_False),
        nNetAccess          (0),
        bROEnabled          (CFG_READONLY_DEFAULT),
        bROSecurity         (CFG_READONLY_DEFAULT),
        bRONetAccess        (CFG_READONLY_DEFAULT),
        bROUserClassPath    (CFG_READONLY_DEFAULT)
        {
            OUString* pNames = aPropertyNames.getArray();
            pNames[0] = C2U("Enable");
            pNames[1] = C2U("Security");
            pNames[2] = C2U("NetAccess");
            pNames[3] = C2U("UserClassPath");
        }
};
/* -----------------------------18.05.01 13:28--------------------------------

 ---------------------------------------------------------------------------*/
SvtJavaOptions::SvtJavaOptions() :
    utl::ConfigItem(C2U("Office.Java/VirtualMachine")),
    pImpl(new SvtJavaOptions_Impl)
{
    RTL_LOGFILE_CONTEXT(aLog, "svtools (???) SvtJavaOptions::SvtJavaOptions()");

    Sequence< Any > aValues = GetProperties(pImpl->aPropertyNames);
    Sequence< sal_Bool > aROStates = GetReadOnlyStates(pImpl->aPropertyNames);
    const Any* pValues = aValues.getConstArray();
    const sal_Bool* pROStates = aROStates.getConstArray();
    if ( aValues.getLength() == pImpl->aPropertyNames.getLength() && aROStates.getLength() == pImpl->aPropertyNames.getLength() )
    {
        for ( int nProp = 0; nProp < pImpl->aPropertyNames.getLength(); nProp++ )
        {
            if( pValues[nProp].hasValue() )
            {
                switch ( nProp )
                {
                    case 0: pImpl->bEnabled = *(sal_Bool*)pValues[nProp].getValue(); break;
                    case 1: pImpl->bSecurity = *(sal_Bool*)pValues[nProp].getValue();break;
                    case 2: pValues[nProp] >>= pImpl->nNetAccess; break;
                    case 3: pValues[nProp] >>= pImpl->sUserClassPath; break;
                }
            }
        }
        pImpl->bROEnabled = pROStates[0];
        pImpl->bROSecurity = pROStates[1];
        pImpl->bRONetAccess = pROStates[2];
        pImpl->bROUserClassPath = pROStates[3];
    }
}
/* -----------------------------18.05.01 13:28--------------------------------

 ---------------------------------------------------------------------------*/
SvtJavaOptions::~SvtJavaOptions()
{
    delete pImpl;
}
/*-- 18.05.01 13:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
void    SvtJavaOptions::Commit()
{
    pImpl->aExecItem.Commit();

    sal_Int32 nOrgCount = pImpl->aPropertyNames.getLength();
    Sequence< OUString > aNames(nOrgCount);
    Sequence< Any > aValues(nOrgCount);
    sal_Int32 nRealCount = 0;

    const Type& rType = ::getBooleanCppuType();
    for(int nProp = 0; nProp < nOrgCount; nProp++)
    {
        switch(nProp)
        {
            case  0:
            {
                if (!pImpl->bROEnabled)
                {
                    aValues[nRealCount].setValue(&pImpl->bEnabled, rType);
                    aNames[nRealCount] = pImpl->aPropertyNames[nProp];
                    ++nRealCount;
                }
            }
            break;
            case  1:
            {
                if (!pImpl->bROSecurity)
                {
                    aValues[nRealCount].setValue(&pImpl->bSecurity, rType);
                    aNames[nRealCount] = pImpl->aPropertyNames[nProp];
                    ++nRealCount;
                }
            }
            break;
            case  2:
            {
                if (!pImpl->bRONetAccess)
                {
                    aValues[nRealCount] <<= pImpl->nNetAccess;
                    aNames[nRealCount] = pImpl->aPropertyNames[nProp];
                    ++nRealCount;
                }
            }
            break;
            case  3:
            {
                if (!pImpl->bROUserClassPath)
                {
                    aValues[nRealCount] <<= pImpl->sUserClassPath;
                    aNames[nRealCount] = pImpl->aPropertyNames[nProp];
                    ++nRealCount;
                }
            }
            break;
        }
    }
    aValues.realloc(nRealCount);
    aNames.realloc(nRealCount);
    PutProperties(aNames,aValues);
}
/*-- 18.05.01 13:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool        SvtJavaOptions::IsEnabled() const
{
    return pImpl->bEnabled;
}
/*-- 18.05.01 13:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool        SvtJavaOptions::IsSecurity()const
{
    return pImpl->bSecurity;
}
/*-- 18.05.01 13:28:35---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32       SvtJavaOptions::GetNetAccess() const
{
    return pImpl->nNetAccess;
}
/*-- 18.05.01 13:28:36---------------------------------------------------

  -----------------------------------------------------------------------*/
rtl::OUString&  SvtJavaOptions::GetUserClassPath()const
{
    return pImpl->sUserClassPath;
}
/*-- 18.05.01 13:28:37---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtJavaOptions::SetEnabled(sal_Bool bSet)
{
    OSL_ENSURE(!pImpl->bROEnabled, "SvtJavaOptions::SetEnabled()\nYou tried to write on a readonly value!\n");
    if(!pImpl->bROEnabled && pImpl->bEnabled != bSet)
    {
        pImpl->bEnabled = bSet;
        SetModified();
    }
}
/*-- 18.05.01 13:28:38---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtJavaOptions::SetSecurity(sal_Bool bSet)
{
    OSL_ENSURE(!pImpl->bROSecurity, "SvtJavaOptions::SetSecurity()\nYou tried to write on a readonly value!\n");
    if(!pImpl->bROSecurity && pImpl->bSecurity != bSet)
    {
        pImpl->bSecurity = bSet;
        SetModified();
    }
}
/*-- 18.05.01 13:28:38---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtJavaOptions::SetNetAccess(sal_Int32 nSet)
{
    OSL_ENSURE(!pImpl->bRONetAccess, "SvtJavaOptions::SetNetAccess()\nYou tried to write on a readonly value!\n");
    if(!pImpl->bRONetAccess && pImpl->nNetAccess != nSet)
    {
        pImpl->nNetAccess = nSet;
        SetModified();
    }
}
/*-- 18.05.01 13:28:38---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtJavaOptions::SetUserClassPath(const rtl::OUString& rSet)
{
    OSL_ENSURE(!pImpl->bROUserClassPath, "SvtJavaOptions::SetUserClassPath()\nYou tried to write on a readonly value!\n");
    if(!pImpl->bROUserClassPath && pImpl->sUserClassPath != rSet)
    {
        pImpl->sUserClassPath = rSet;
        SetModified();
    }
}

/*-- 18.05.01 14:34:32---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Bool        SvtJavaOptions::IsExecuteApplets() const
{
    return pImpl->aExecItem.IsExecuteApplets();
}
/*-- 18.05.01 14:34:32---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtJavaOptions::SetExecuteApplets(sal_Bool bSet)
{
    if(!pImpl->aExecItem.IsReadOnly() && pImpl->aExecItem.IsExecuteApplets() != bSet)
    {
        pImpl->aExecItem.SetExecuteApplets(bSet);
        SetModified();
    }
}
/*--10.02.2003 08:40---------------------------------------------------

-----------------------------------------------------------------------*/
sal_Bool SvtJavaOptions::IsReadOnly( EOption eOption ) const
{
    sal_Bool bRO = sal_True;
    switch(eOption)
    {
        case E_ENABLED :
            bRO = pImpl->bROEnabled;
            break;
        case E_SECURITY :
            bRO = pImpl->bROSecurity;
            break;
        case E_NETACCESS :
            bRO = pImpl->bRONetAccess;
            break;
        case E_USERCLASSPATH :
            bRO = pImpl->bROUserClassPath;
            break;
        case E_EXECUTEAPPLETS :
            bRO = pImpl->aExecItem.IsReadOnly();
            break;
    }
    return bRO;
}
