/*************************************************************************
 *
 *  $RCSfile: unoobjw.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-23 17:12:19 $
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

#include "ole2uno.hxx"

#include <tools/presys.h>
#include <olectl.h>
#include <vector>
#include <list>
#include <hash_map>
#include "comifaces.hxx"
#include <tools/postsys.h>


#include <vos/diagnose.hxx>
#include <vos/refernce.hxx>
#include <tools/debug.hxx>

#ifndef _RTL_USTRING_
#include <rtl/ustring>
#endif

#ifndef _COM_SUN_STAR_BEANS_METHODCONCEPT_HPP_
#include <com/sun/star/beans/MethodConcept.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYCONCEPT_HPP_
#include <com/sun/star/beans/PropertyConcept.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_FAILREASON_HPP_
#include <com/sun/star/script/FailReason.hpp>
#endif
#ifndef _COM_SUN_STAR_REFLECTION_PARAMINFO_HPP_
#include <com/sun/star/reflection/ParamInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_XEXACTNAME_HPP_
#include <com/sun/star/beans/XExactName.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_NOSUCHELEMENTEXCEPTION_HPP_
#include <com/sun/star/container/NoSuchElementException.hpp>
#endif

#include <com/sun/star/beans/XMaterialHolder.hpp>
#include <com/sun/star/script/XInvocation2.hpp>
#include <com/sun/star/script/MemberType.hpp>
#include <com/sun/star/reflection/XIdlReflection.hpp>

#ifndef _TYPELIB_TYPEDESCRIPTION_H_
#include <uno/typedescription.h>
#endif
#ifndef _OSL_INTERLOCK_H_
#include <osl/interlck.h>
#endif
#ifndef _COM_SUN_STAR_UNO_GENFUNC_H_
#include <com/sun/star/uno/genfunc.h>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

#include "comifaces.hxx"
#include "jscriptclasses.hxx"
#include "oleobjw.hxx"
#include "unoobjw.hxx"
#include "servprov.hxx"

using namespace vos;
using namespace std;
using namespace rtl;
using namespace osl;
using namespace cppu;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::script;
using namespace com::sun::star::lang;
using namespace com::sun::star::bridge::ModelDependent;
using namespace com::sun::star::reflection;



#if _MSC_VER < 1200
extern "C" const GUID IID_IDispatchEx;
#endif

namespace ole_adapter
{
hash_map<sal_uInt32, WeakReference<XInterface> > UnoObjToWrapperMap;
static sal_Bool writeBackOutParameter(VARIANTARG* pDest, VARIANT* pSource);
static sal_Bool writeBackOutParameter2( VARIANTARG* pDest, VARIANT* pSource);
static HRESULT mapCannotConvertException( CannotConvertException e, unsigned int * puArgErr);

/*****************************************************************************

    class implementation: InterfaceOleWrapper_Impl

*****************************************************************************/
InterfaceOleWrapper_Impl::InterfaceOleWrapper_Impl( Reference<XMultiServiceFactory>& xFactory,
                                                    sal_uInt8 unoWrapperClass, sal_uInt8 comWrapperClass):
        m_defaultValueType( 0),
        UnoConversionUtilities<InterfaceOleWrapper_Impl>( xFactory, unoWrapperClass, comWrapperClass)
{
}

InterfaceOleWrapper_Impl::~InterfaceOleWrapper_Impl()
{
    MutexGuard guard(getBridgeMutex());
    // remove entries in global map
    IT_Uno it= UnoObjToWrapperMap.find( (sal_uInt32) m_xOrigin.get());
    if(it != UnoObjToWrapperMap.end())
        UnoObjToWrapperMap.erase(it);
}

STDMETHODIMP InterfaceOleWrapper_Impl::QueryInterface(REFIID riid, LPVOID FAR * ppv)
{
    HRESULT ret= S_OK;

    if( !ppv)
        return E_POINTER;

    if(IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppv = (IUnknown*) (IDispatch*) this;
    }
    else if (IsEqualIID(riid, IID_IDispatch))
    {
        AddRef();
        *ppv = (IDispatch*) this;
    }
    else if( IsEqualIID( riid, __uuidof( IUnoObjectWrapper)))
    {
        AddRef();
        *ppv= (IUnoObjectWrapper*) this;
    }
    else
        ret= E_NOINTERFACE;
    return ret;
}

STDMETHODIMP_(ULONG) InterfaceOleWrapper_Impl::AddRef()
{
    acquire();
    // does not need to guard because one should not rely on the return value of
    // AddRef anyway
    return m_refCount;
}

STDMETHODIMP_(ULONG) InterfaceOleWrapper_Impl::Release()
{
    ULONG n= m_refCount;
    release();
    return n - 1;
}

// IUnoObjectWrapper --------------------------------------------------------
STDMETHODIMP InterfaceOleWrapper_Impl::getWrapperXInterface( Reference<XInterface>* pXInt)
{
    *pXInt= Reference<XInterface>( static_cast<XWeak*>( this), UNO_QUERY);
    return pXInt->is() ? S_OK : E_FAIL;
}
STDMETHODIMP InterfaceOleWrapper_Impl::getOriginalUnoObject( Reference<XInterface>* pXInt)
{
    *pXInt= m_xOrigin;
    return m_xOrigin.is() ? S_OK : E_FAIL;
}
STDMETHODIMP  InterfaceOleWrapper_Impl::getOriginalUnoStruct( Any * pStruct)
{
    HRESULT ret= E_FAIL;
    if( !m_xOrigin.is())
    {
        Reference<XMaterialHolder> xMatHolder( m_xInvocation, UNO_QUERY);
        if( xMatHolder.is())
        {
            Any any = xMatHolder->getMaterial();
            if( any.getValueTypeClass() == TypeClass_STRUCT)
            {
                *pStruct= any;
                ret= S_OK;
            }
        }
    }
    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetTypeInfoCount( unsigned int * pctinfo )
{
    return E_NOTIMPL ;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo ** pptinfo)
{
    return E_NOTIMPL;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetIDsOfNames(REFIID riid,
                                                     OLECHAR ** rgszNames,
                                                     unsigned int cNames,
                                                     LCID lcid,
                                                     DISPID * rgdispid )
{

    MutexGuard guard( getBridgeMutex());
    if( ! rgdispid)
        return E_POINTER;
    HRESULT ret = E_UNEXPECTED;
    // ----------------------------------------
    if( ! _wcsicmp( *rgszNames, JSCRIPT_VALUE_FUNC) ||
        ! _wcsicmp( *rgszNames, BRIDGE_VALUE_FUNC))
    {
        *rgdispid= DISPID_JSCRIPT_VALUE_FUNC;
        return S_OK;
    }
    else if( ! _wcsicmp( *rgszNames, GET_STRUCT_FUNC) ||
             ! _wcsicmp( *rgszNames, BRIDGE_GET_STRUCT_FUNC))
    {
        *rgdispid= DISPID_GET_STRUCT_FUNC;
        return S_OK;
    }
    // ----------------------------------------
    if (m_xInvocation.is() && (cNames > 0))
    {
        OUString name(rgszNames[0]);
        NameToIdMap::iterator iter = m_nameToDispIdMap.find(name);

        if (iter == m_nameToDispIdMap.end())
        {
            OUString exactName;

            if (m_xExactName.is())
            {
                exactName = m_xExactName->getExactName(name);
            }
            else
            {
                exactName = name;
            }

            MemberInfo d(0, exactName);

            if (m_xInvocation->hasProperty(exactName))
            {
                d.flags |= DISPATCH_PROPERTYGET;
                d.flags |= DISPATCH_PROPERTYPUT;
                d.flags |= DISPATCH_PROPERTYPUTREF;
            }

            if (m_xInvocation->hasMethod(exactName))
            {
                d.flags |= DISPATCH_METHOD;
            }

            if (d.flags != 0)
            {
                m_MemberInfos.push_back(d);
                iter = m_nameToDispIdMap.insert(NameToIdMap::value_type(exactName, (DISPID)m_MemberInfos.size())).first;

                if (exactName != name)
                {
                    iter = m_nameToDispIdMap.insert(NameToIdMap::value_type(name, (DISPID)m_MemberInfos.size())).first;
                }
            }
        }

        if (iter == m_nameToDispIdMap.end())
        {
            ret = ResultFromScode(DISP_E_UNKNOWNNAME);
        }
        else
        {
            *rgdispid = (*iter).second;
            ret = ResultFromScode(S_OK);
        }
    }

    return ret;
}

// "convertDispparamsArgs" converts VARIANTS to their respecting Any counterparts
// The parameters "id", "wFlags" and "pdispparams" equal those as used in
// IDispatch::Invoke. The function handles special JavaScript
// cases where a VARIANT of type VT_DISPATCH is ambiguous and could represent
// an object, array ( JavaScript Array object), out parameter and in/out ( JavaScript Array object)
//  parameter (JavaScript Array object)
// parameter. Because all those VT_DISPATCH objects need a different conversion
// we have to find out what the object is supposed to be. The function does this
// by either using type information or by help of a specialized ValueObject object.

// A. Type Information
// -----------------------------------------------------------------------------
// With the help of type information the kind of parameter can be exactly determined
// and an appropriate conversion can be choosen. A problem arises if a method expects
// an Any. Then the type info does not tell what the type of the value, that is kept
// by the any, should be. In this situation the decision wheter the param is a
// sequence or an object is made upon the fact if the object has a property "0"
// ( see function "isJScriptArray"). Since this is unsafe it is recommended to use
// the JScript value objects within a JScript script on such an occasion.

// B. JavaScript Value Object ( class JScriptValue )
// -----------------------------------------------------------------------------
// A JScriptValue (ValueObject) object is a COM object in that it implements IDispatch and the
// IJScriptValue object interface. Such objects are provided by all UNO wrapper
// objects used within a JScript script. To obtain an instance one has to call
// "_GetValueObject() or Bridge_GetValueObject()" on an UNO wrapper object (class InterfaceOleWrapper_Impl).
// A value object is appropriately initialized within the script and passed as
// parameter to an UNO object method or property. The convertDispparamsArgs function
// can easily find out that a param is such an object by queriing for the
// IJScriptValue interface. By this interface one the type and kind ( out, in/out)
// can be determined and the right conversion can be applied.
// Using ValueObjects we spare us the effort of aquiring and examining type information
// in order to figure out what the an IDispatch parameter is meant for.

// Normal JScript object parameter can be mixed with JScriptValue object. If an
// VARIANT contains an VT_DISPATCH that is no JScriptValue than the type information
// is used to find out about the reqired type.

// out Parameter in Visual Basic have the type VT_BYREF | VT_VARIANT and if the bridge
// is used from C++ than the type is usually VT_BYREF |VT_xxx. Those parameter are converted
// and we don't go to the trouble of aquiring type information.
HRESULT InterfaceOleWrapper_Impl::convertDispparamsArgs(  DISPID id, unsigned short wFlags, DISPPARAMS* pdispparams, Sequence<Any>& rSeq)
{
    HRESULT hr= S_OK;
    sal_Int32 countArgs= pdispparams->cArgs;
    if( countArgs == 0)
        return S_OK;
    sal_Bool convOk = sal_True;


    // The sequence will hold information whether a param at a certain index has already been converted.
    Sequence< sal_Bool > seqConvertedParams( countArgs);
    rSeq.realloc( countArgs);
    Any*    pParams = rSeq.getArray();

    sal_Bool bTypeInfoNeeded= sal_False;
    Any anyParam;
    sal_Bool bHandled= sal_False;

    // Iterate over all parameters. The aim is to find out if we need to obtain
    // type information for the parameter, which is the case when we receive an
    // VT_DISPATCH or VT_ARRAY argument. Also, ValuObjects are converted in this loop.
    // An VT_DISPATCH can be a ValueObject (class JScriptValue) objects that originated in this bridge.
    // The index in the sequence "seqConvertedParams" matches the index in the
    // DISPPARAMS.rgvarg array. If the value is true then the corresponding VARIANT has been converted
    // and the any has been written to the Sequence that holds the converted values.
    for( int i= 0; i < countArgs; i++)
    {
        if( sal_False == convertValueObject( &pdispparams->rgvarg[i], anyParam, bHandled) )
            return DISP_E_BADVARTYPE;
        if( bHandled)
        {// a param is a ValueObject and could be converted
            pParams[countArgs - i - 1]= anyParam;
            seqConvertedParams[i]= sal_True;
            continue;
        }
        else
        {
            // if the param is no ValueObject although it is an IDispatch then we must
            // set a flag that indicates that we have to examine those objects further.
            // This is also true if the argument is an array
            CComVariant var;
            if( SUCCEEDED( var.ChangeType(VT_DISPATCH, &pdispparams->rgvarg[i]))
                || (pdispparams->rgvarg[i].vt & VT_ARRAY)
                ||  ( pdispparams->rgvarg[i].vt == (VT_VARIANT | VT_BYREF) &&
                      pdispparams->rgvarg[i].pvarVal->vt & VT_ARRAY))
            {
                bTypeInfoNeeded= sal_True;
            }
        }
    }
    try{
        // If there is a VT_DISPATCH  and if it is no JScriptValue object then
        // we need to know if it represents a Sequence, out or in/out parameter
        // or an actual object. Therefore we obtain type information.
        InvocationInfo info;
        sal_Bool bTypesAvailable= sal_False;
        if(  bTypeInfoNeeded)
        {
            if( ! (bTypesAvailable= getInvocationInfoForCall( id, info)))
                return DISP_E_EXCEPTION;
        }
        // Used within the following "for" loop. If the param is an out, in/out parameter in
        // JScript (Array object, with value at index 0) then we
        // extract Array[0] and put the value into varParam. At the end of the loop varParam
        // is converted if it contains a value otherwise the VARIANT from
        // DISPPARAMS is converted.
        CComVariant varParam;

        // In this loop all params are converted except for those which are already converted
        // by convertValueObject ( see above ). The Sequence seqConvertedParams contains information
        // about which params are already converted (convertValueObject). A value in seqConvertedParams
        // at a particular index relates to the VARIANTARGS param at the same index in DISPPARAMS.
        for (int i = 0; convOk && (i < countArgs); i++)
        {
            if( seqConvertedParams[i] == sal_True)
                continue;
            sal_Bool bDoConversion= TRUE;
            varParam.Clear();
            // Check for JScript out and in/out paramsobjects (VT_DISPATCH).
            // To find them out we use typeinformation of the function being called.
            if( pdispparams->rgvarg[i].vt == VT_DISPATCH )
            {
                if( info.eMemberType == MemberType_METHOD && info.aParamModes[ countArgs - i -1 ]  == ParamMode_INOUT)
                {
                    // INOUT-param
                    // Index ( property) "0" contains the actual IN-param. The object is a JScript
                    // Array object.
                    // Get the IN-param at index "0"
                    IDispatch* pdisp= pdispparams->rgvarg[i].pdispVal;

                    OLECHAR* sindex= L"0";
                    DISPID id;
                    DISPPARAMS noParams= {0,0,0,0};
                    if(SUCCEEDED( hr= pdisp->GetIDsOfNames( IID_NULL, &sindex, 1, LOCALE_USER_DEFAULT, &id)))
                        hr= pdisp->Invoke( id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
                            &noParams, &varParam, NULL, NULL);
                    if( FAILED( hr))
                    {
                        varParam.Clear();
                        convOk= sal_False; break;
                        hr= DISP_E_EXCEPTION;
                        break;
                    }
                }
                else if( info.eMemberType == MemberType_METHOD && info.aParamModes[ countArgs - i -1 ] == ParamMode_OUT)
                {
                    // No conversion necessary. The VT_DISPATCH represents
                    // an JScript Array in which the outparam is written on index 0
                    // or it is VT_DISPATCH | VT_BYREF, out param in C
                    bDoConversion= sal_False;
                }
            }
            if( bDoConversion)
            {
                if( varParam.vt == VT_EMPTY) // then it was no in/out parameter
                    varParam= pdispparams->rgvarg[i];

                Any theParam;
                if( bTypesAvailable && info.eMemberType == MemberType_METHOD)
                    convOk= variantToAny2( & varParam,theParam,
                            info.aParamTypes[ countArgs - i - 1]);
                else if( bTypesAvailable && info.eMemberType == MemberType_PROPERTY)
                    convOk= variantToAny2( & varParam,theParam, info.aType);
                else
                    convOk = variantToAny( & varParam, theParam);

                if( convOk)
                    pParams[countArgs - (i + 1)]= theParam;
            }
        }// end for / iterating over all parameters
    }catch( IllegalArgumentException ) // XInvocation2::getInfoForName
    {
        hr= DISP_E_BADVARTYPE;
    }
    catch(...)
    {
        hr= DISP_E_EXCEPTION;
    }
    if( SUCCEEDED( hr) && convOk == sal_False)
        hr= DISP_E_EXCEPTION;
    return hr;
}

sal_Bool  InterfaceOleWrapper_Impl::getInvocationInfoForCall( DISPID id, InvocationInfo& info)
{
    sal_Bool bTypesAvailable= sal_False;

    if( !m_xInvocation.is() )return false;
    Reference<XInvocation2> inv2( m_xInvocation, UNO_QUERY);
    if( inv2.is())
    {
        // We need the name of the property or method to get its type information.
        // The name can be identified through the param "id"
        // that is kept as value in the map m_nameToDispIdMap.
        // Proplem: the Windows JScript engine sometimes changes small letters to capital
        // letters as happens in xidlclass_obj.createObject( var) // in JScript.
        // IDispatch::GetIdsOfNames is then called with "CreateObject" !!!
        // m_nameToDispIdMap can contain several names for one DISPID but only one is
        // the exact one. If there's no m_xExactName and therefore no exact name then
        // there's only one entry in the map.
        typedef NameToIdMap::const_iterator cit;
        OUString sMemberName;

        for(cit ci1= m_nameToDispIdMap.begin(); ci1 != m_nameToDispIdMap.end(); ci1++)
        {
            if( (*ci1).second == id) // iterator is a pair< OUString, DISPID>
            {
                sMemberName= (*ci1).first;
                break;
            }
        }
        // Get information for the current call ( property or method).
        // There could be similar names which only differ in the cases
        // of letters. First we assume that the name which was passed into
        // GetIDsOfNames is correct. If we won't get information with that
        // name then we have the invocation service use the XExactName interface.
        sal_Bool validInfo= sal_True;
        InvocationInfo invInfo;
        try{
            invInfo= inv2->getInfoForName( sMemberName, sal_False);
        }
        catch( IllegalArgumentException )
        {
            validInfo= sal_False;
        }

        if( ! validInfo)
        {
            invInfo= inv2->getInfoForName( sMemberName, sal_True);
        }
        if( invInfo.aName.pData)
        {
            bTypesAvailable= sal_True;
            info= invInfo;
        }
    }
    return bTypesAvailable;
}
// XBridgeSupplier2 ---------------------------------------------------
// only bridges itself ( this instance of InterfaceOleWrapper_Impl)from UNO to IDispatch
// If sourceModelType is UNO than any UNO interface implemented by InterfaceOleWrapper_Impl
// can bridged to IDispatch ( if destModelType == OLE). The IDispatch is
// implemented by this class.
Any SAL_CALL InterfaceOleWrapper_Impl::createBridge(const Any& modelDepObject,
                                const Sequence<sal_Int8>& ProcessId,
                                sal_Int16 sourceModelType,
                                sal_Int16 destModelType)
            throw (IllegalArgumentException, RuntimeException)
{

    Any retAny;
    if( sourceModelType == UNO && destModelType == OLE &&
        modelDepObject.getValueTypeClass() == TypeClass_INTERFACE )
    {
        Reference<XInterface> xInt;
        if( modelDepObject >>= xInt )
        {
            if( xInt == Reference<XInterface>( static_cast<XWeak*>( this), UNO_QUERY))
            {
                VARIANT *pVar= (VARIANT*)CoTaskMemAlloc( sizeof( VARIANT));
                if( pVar)
                {
                    pVar->vt= VT_DISPATCH;
                    pVar->pdispVal= static_cast<IDispatch*>( this);
                    AddRef();

                    retAny<<= reinterpret_cast< sal_uInt32 >( pVar);
                }
            }
        }
    }

    return retAny;
}

// XInterface ------------------------------------------------------
// void SAL_CALL InterfaceOleWrapper_Impl::acquire(  ) throw()
// {
//  AddRef();
// }
// void SAL_CALL InterfaceOleWrapper_Impl::release(  ) throw()
// {
//  Release();
// }


// XInitialization --------------------------------------------------
void SAL_CALL InterfaceOleWrapper_Impl::initialize( const Sequence< Any >& aArguments )
        throw(Exception, RuntimeException)
{
    switch( aArguments.getLength() )
    {
    case 2: // the object wraps an UNO struct
        aArguments[0] >>= m_xInvocation;
        aArguments[1] >>= m_defaultValueType;
        break;
    case 3: // the object wraps an UNO interface
        aArguments[0] >>= m_xInvocation;
        aArguments[1] >>= m_xOrigin;
        aArguments[2] >>= m_defaultValueType;
        break;
    }

    m_xExactName= Reference<XExactName>( m_xInvocation, UNO_QUERY);
}

Reference< XInterface > InterfaceOleWrapper_Impl::createUnoWrapperInstance()
{
    Reference<XWeak> xWeak= static_cast<XWeak*>( new InterfaceOleWrapper_Impl(
                            m_smgr, m_nUnoWrapperClass, m_nComWrapperClass));
    return Reference<XInterface>( xWeak, UNO_QUERY);
}

Reference<XInterface> InterfaceOleWrapper_Impl::createComWrapperInstance()
{
    Reference<XWeak> xWeak= static_cast<XWeak*>( new IUnknownWrapper_Impl(
                            m_smgr, m_nUnoWrapperClass, m_nComWrapperClass));
    return Reference<XInterface>( xWeak, UNO_QUERY);
}



// "getType" is used in convertValueObject to map the string denoting the type
// to an actual Type object.
Type getType( const BSTR type)
{
    Type retType;
    typelib_TypeDescription * pDesc= NULL;
    OUString str( type);
    typelib_typedescription_getByName( &pDesc, str.pData );
    if( pDesc)
    {
        retType= Type( pDesc->pWeakRef );
        typelib_typedescription_release( pDesc);
    }
    return retType;
}

static sal_Bool writeBackOutParameter2( VARIANTARG* pDest, VARIANT* pSource)
{
    sal_Bool ret = sal_False;
    HRESULT hr;

    // Handle JScriptValue objects and JScript out params ( Array object )
    CComVariant varDest( *pDest);

    if( SUCCEEDED( varDest.ChangeType(VT_DISPATCH)))

//      if( (pDest->vt ==  VT_DISPATCH) ||
//          (pDest->vt == (VT_VARIANT | VT_BYREF) &&
//          pDest->pvarVal->vt == VT_DISPATCH) )
    {
        CComPtr<IDispatch> spDispDest(varDest.pdispVal);
//          if( pDest->vt == VT_DISPATCH)
//              spDispDest= pDest->pdispVal;
//          else
//              spDispDest= pDest->pvarVal->pdispVal;

//      CComPtr <IJScriptValueObject> spValueDest;

        // special Handling for a JScriptValue object
        CComQIPtr<IJScriptValueObject> spValueDest(spDispDest);
        if (spValueDest)
        {
            VARIANT_BOOL varBool= VARIANT_FALSE;
            if( SUCCEEDED( hr= spValueDest->IsOutParam( &varBool) )
                && varBool == VARIANT_TRUE  ||
                SUCCEEDED(hr= spValueDest->IsInOutParam( &varBool) )
                && varBool == VARIANT_TRUE )
            {
                if( SUCCEEDED( spValueDest->Set( CComVariant(), *pSource)))
                    ret= sal_True;
            }
        }
        else if (pDest->vt == VT_DISPATCH)// VT_DISPATCH -> JScript out param
        {
            // We use IDispatchEx because its GetDispID function causes the creation
            // of a property if it does not exist already. This is convenient for
            // out parameters in JScript. Then the user must not specify propery "0"
            // explicitly
            CComQIPtr<IDispatchEx> spDispEx( spDispDest);
            if( spDispEx)
            {
                CComBSTR nullProp(L"0");
                DISPID dwDispID;
                if( SUCCEEDED( spDispEx->GetDispID( nullProp, fdexNameEnsure, &dwDispID)))
                {
                    DISPPARAMS dispparams = {NULL, NULL, 1, 1};
                    dispparams.rgvarg = pSource;
                    DISPID dispidPut = DISPID_PROPERTYPUT;
                    dispparams.rgdispidNamedArgs = &dispidPut;

                    if (pSource->vt == VT_UNKNOWN || pSource->vt == VT_DISPATCH ||
                        (pSource->vt & VT_ARRAY) || (pSource->vt & VT_BYREF))
                        hr = spDispEx->InvokeEx(dwDispID, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF,
                                                &dispparams, NULL, NULL, NULL);
                    else
                        hr= spDispEx->InvokeEx(dwDispID, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
                                               &dispparams, NULL, NULL, NULL);
                    if( SUCCEEDED(hr))
                        ret= sal_True;
                }
            }
        }
        else
            ret= writeBackOutParameter( pDest, pSource);
    }
    else // The param can't be a JScript out-parameter ( an Array object), it could be a VBScript
    {   // param. The function checks itself for correct VBScript params
        ret= writeBackOutParameter( pDest, pSource);
    }
    return ret;
}
// VisualBasic Script passes arguments as VT_VARIANT | VT_BYREF be it in or out parameter.
// Thus we are in charge of freeing an eventual value contained by the inner VARIANT
// Please note: VariantCopy doesn't free a VT_BYREF value
// The out parameters are expected to have always a valid type
static sal_Bool writeBackOutParameter(VARIANTARG* pDest, VARIANT* pSource)
{
    HRESULT hr;
    sal_Bool ret = FALSE;
    // Out parameter must be VT_BYREF
    if ((V_VT(pDest) & VT_BYREF) != 0 )
    {
        VARTYPE oleTypeFlags = V_VT(pSource);

        // if caller accept VARIANT as out parameter, any value must be converted
        if (V_VT(pDest) == (VT_VARIANT | VT_BYREF))
        {
            // When the user provides a VARIANT rather then a concrete type
            // we just copy the source to the out, in/out parameter
            // VT_DISPATCH, VT_UNKNOWN, VT_ARRAY, VT_BSTR in the VARIANT that
            // is contained in pDest are released by VariantCopy
            VariantCopy(V_VARIANTREF(pDest), pSource);
            ret = sal_True;
        }
        else
        {
            // variantarg and variant must have same type
              if ((V_VT(pDest) & oleTypeFlags) == oleTypeFlags)
            {
                if ((oleTypeFlags & VT_ARRAY) != 0)
                {
                    // In / Out Param
                    if( *V_ARRAYREF(pDest) != NULL)
                        hr= SafeArrayCopyData( V_ARRAY(pSource), *V_ARRAYREF(pDest));
                    else
                        // Out Param
                        hr= SafeArrayCopy(V_ARRAY(pSource), V_ARRAYREF(pDest)) == NOERROR;
                    if( SUCCEEDED( hr))
                        ret = sal_True;
                }
                else
                {
                    // copy base type
                    switch (V_VT(pSource))
                    {
                    case VT_I2:
                    {
                        *V_I2REF(pDest) = V_I2(pSource);
                        ret = sal_True;
                        break;
                    }
                    case VT_I4:
                        *V_I4REF(pDest) = V_I4(pSource);
                        ret = sal_True;
                        break;
                    case VT_R4:
                        *V_R4REF(pDest) = V_R4(pSource);
                        ret = sal_True;
                        break;
                    case VT_R8:
                        *V_R8REF(pDest) = V_R8(pSource);
                        ret = sal_True;
                        break;
                    case VT_CY:
                        *V_CYREF(pDest) = V_CY(pSource);
                        ret = sal_True;
                        break;
                    case VT_DATE:
                        *V_DATEREF(pDest) = V_DATE(pSource);
                        ret = sal_True;
                        break;
                    case VT_BSTR:
                        SysFreeString( *pDest->pbstrVal);

                        *V_BSTRREF(pDest) = SysAllocString(V_BSTR(pSource));
                        ret = sal_True;
                        break;
                    case VT_DISPATCH:
                        if (*V_DISPATCHREF(pDest) != NULL)
                            (*V_DISPATCHREF(pDest))->Release();

                        *V_DISPATCHREF(pDest) = V_DISPATCH(pSource);

                        if (*V_DISPATCHREF(pDest) != NULL)
                            (*V_DISPATCHREF(pDest))->AddRef();

                        ret = sal_True;
                        break;
                    case VT_ERROR:
                        *V_ERRORREF(pDest) = V_ERROR(pSource);
                        ret = sal_True;
                        break;
                    case VT_BOOL:
                        *V_BOOLREF(pDest) = V_BOOL(pSource);
                        ret = sal_True;
                        break;
                    case VT_UNKNOWN:
                        if (*V_UNKNOWNREF(pDest) != NULL)
                            (*V_UNKNOWNREF(pDest))->Release();

                        *V_UNKNOWNREF(pDest) = V_UNKNOWN(pSource);

                        if (*V_UNKNOWNREF(pDest) != NULL)
                            (*V_UNKNOWNREF(pDest))->AddRef();

                        ret = sal_True;
                        break;
                    case VT_I1:
                        *V_I1REF(pDest) = V_I1(pSource);
                        ret = sal_True;
                        break;
                    case VT_UI1:
                        *V_UI1REF(pDest) = V_UI1(pSource);
                        ret = sal_True;
                        break;
                    case VT_UI2:
                        *V_UI2REF(pDest) = V_UI2(pSource);
                        ret = sal_True;
                        break;
                    case VT_UI4:
                        *V_UI4REF(pDest) = V_UI4(pSource);
                        ret = sal_True;
                        break;
                    case VT_INT:
                        *V_INTREF(pDest) = V_INT(pSource);
                        ret = sal_True;
                        break;
                    case VT_UINT:
                        *V_UINTREF(pDest) = V_UINT(pSource);
                        ret = sal_True;
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                // Handling of special cases
                // Destination and source types are different
                if( pDest->vt == (VT_BSTR | VT_BYREF)
                    && pSource->vt == VT_I2)
                {
                    // When the user provides a String as out our in/out parameter
                    // and the type is char (TypeClass_CHAR) then we convert to a BSTR
                    // instead of VT_I2 as is done otherwise
                    OLECHAR buff[]= {0,0};
                    buff[0]= pSource->iVal;

                    SysFreeString( *pDest->pbstrVal);
                    *pDest->pbstrVal= SysAllocString( buff);
                }
            }
        }
    }
    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::Invoke(DISPID dispidMember,
                                              REFIID riid,
                                              LCID lcid,
                                              unsigned short wFlags,
                                               DISPPARAMS * pdispparams,
                                              VARIANT * pvarResult,
                                              EXCEPINFO * pexcepinfo,
                                               unsigned int * puArgErr )
{
    HRESULT ret = S_OK;

    sal_Bool bHandled= sal_False;
    ret= InvokeGeneral( dispidMember,  wFlags, pdispparams, pvarResult,  pexcepinfo,
                   puArgErr, bHandled);
    if( bHandled)
        return ret;

    if ((dispidMember > 0) && (dispidMember <= m_MemberInfos.size()) && m_xInvocation.is())
    {
        MemberInfo d = m_MemberInfos[dispidMember - 1];
        DWORD flags = wFlags & d.flags;

        if (flags != 0)
        {
            if ((flags & DISPATCH_METHOD) != 0)
            {
                if (pdispparams->cNamedArgs > 0)
                    ret = DISP_E_NONAMEDARGS;
                else
                {
                    Sequence<Any> params;

                    if( SUCCEEDED(ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams , params ) ))
                    {
                        ret= doInvoke(  pdispparams, pvarResult,
                          pexcepinfo, puArgErr, d.name, params );
                    }
                    else
                        ret = DISP_E_BADVARTYPE;
                }
            }
            else if ((flags & DISPATCH_PROPERTYGET) != 0)
            {
                ret=  doGetProperty( pdispparams, pvarResult,
                                    pexcepinfo, d.name);
            }
            else if ((flags & DISPATCH_PROPERTYPUT || flags & DISPATCH_PROPERTYPUTREF) != 0)
            {
                if (pdispparams->cArgs != 1)
                    ret = DISP_E_BADPARAMCOUNT;
                else
                {
                    Sequence<Any> params;
                    ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params );
                    if( SUCCEEDED( ret) && (params.getLength() > 0))
                    {
                        ret= doSetProperty( pdispparams, pvarResult, pexcepinfo, puArgErr, d.name, params);
                    }
                    else
                        ret = DISP_E_BADVARTYPE;
                }
            }
        }
        else
            ret= DISP_E_MEMBERNOTFOUND;
    }
    else
        ret = DISP_E_MEMBERNOTFOUND;

    return ret;
}

HRESULT InterfaceOleWrapper_Impl::doInvoke( DISPPARAMS * pdispparams, VARIANT * pvarResult,
                              EXCEPINFO * pexcepinfo, unsigned int * puArgErr, OUString& name, Sequence<Any>& params)
{

    HRESULT ret= S_OK;
    Sequence<INT16>     outIndex;
    Sequence<Any>   outParams;
    Any                 returnValue;

    if (pdispparams->cNamedArgs > 0)
        return DISP_E_NONAMEDARGS;

    // invoke method and take care of exceptions
    try
    {
        returnValue = m_xInvocation->invoke(name,
                                            params,
                                            outIndex,
                                            outParams);

        // try to write back out parameter
        if (outIndex.getLength() > 0)
        {
             const INT16* pOutIndex = outIndex.getConstArray();
            const Any* pOutParams = outParams.getConstArray();

            for (UINT32 i = 0; i < outIndex.getLength(); i++)
            {
                VARIANT variant;

                VariantInit(&variant);

                // Currently a Sequence is converted to an SafeArray of VARIANTs.
                anyToVariant( &variant, pOutParams[i]);

                // out parameter need special handling if they are VT_DISPATCH
                // and used in JScript
                int outindex= pOutIndex[i];
                writeBackOutParameter2(&(pdispparams->rgvarg[pdispparams->cArgs - 1 - outindex]),
                                      &variant );

                VariantClear(&variant);
            }
        }

        // write back return value
        if ((pvarResult != NULL) && ! anyToVariant(pvarResult, returnValue))
        {
            ret = DISP_E_BADVARTYPE;
        }
    }
    catch(IllegalArgumentException e)
    {
        ret = DISP_E_TYPEMISMATCH;
    }
    catch(CannotConvertException e)
    {
        ret= mapCannotConvertException( e, puArgErr);
    }
    catch(InvocationTargetException e)
    {
        if (pexcepinfo != NULL)
        {
            Any org = e.TargetException;

            pexcepinfo->wCode = UNO_2_OLE_EXCEPTIONCODE;
            pexcepinfo->bstrSource = SysAllocString(L"any ONE component");
            pexcepinfo->bstrDescription = SysAllocString(
                  org.getValueType().getTypeName());
        }
        ret = ResultFromScode(DISP_E_EXCEPTION);
    }
    catch( NoSuchMethodException e)
    {
        ret= DISP_E_MEMBERNOTFOUND;
    }
    catch( ... )
    {
        ret= DISP_E_EXCEPTION;
    }
    return ret;
}

HRESULT InterfaceOleWrapper_Impl::doGetProperty( DISPPARAMS * pdispparams, VARIANT * pvarResult,
                                                EXCEPINFO * pexcepinfo, OUString& name)
{
    HRESULT ret= S_OK;

    Any value;
    try
    {
        Any returnValue = m_xInvocation->getValue( name);
        // write back return value
        if (!anyToVariant(pvarResult, returnValue))     {
            ret = DISP_E_BADVARTYPE;
        }
    }
    catch(UnknownPropertyException e)
    {
        ret = DISP_E_MEMBERNOTFOUND;
    }
    catch( ... )
    {
        ret= DISP_E_EXCEPTION;
    }
    return  ret;
}

HRESULT InterfaceOleWrapper_Impl::doSetProperty( DISPPARAMS * pdispparams, VARIANT * pvarResult,
                                        EXCEPINFO * pexcepinfo, unsigned int * puArgErr, OUString& name, Sequence<Any> params)
{
    HRESULT ret= S_OK;

    try
    {
        m_xInvocation->setValue( name, params.getConstArray()[0]);
    }
    catch(UnknownPropertyException )
    {
        ret = DISP_E_MEMBERNOTFOUND;
    }
    catch(CannotConvertException e)
    {
        ret= mapCannotConvertException( e, puArgErr);
    }
    catch(InvocationTargetException e)
    {
        if (pexcepinfo != NULL)
        {
            Any org = e.TargetException;

            pexcepinfo->wCode = UNO_2_OLE_EXCEPTIONCODE;
            pexcepinfo->bstrSource = SysAllocString(L"any ONE component");
            pexcepinfo->bstrDescription = SysAllocString(
                org.getValueType().getTypeName());
        }
        ret = DISP_E_EXCEPTION;
    }
    catch( ... )
    {
        ret= DISP_E_EXCEPTION;
    }
    return ret;
}

HRESULT InterfaceOleWrapper_Impl::InvokeGeneral( DISPID dispidMember, unsigned short wFlags,
                         DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo,
                         unsigned int * puArgErr, sal_Bool& bHandled)
{
    HRESULT ret= S_OK;
// DISPID_VALUE | The DEFAULT Value is required in JScript when the situation
// is that we put an object into an Array object ( out parameter). We have to return
// IDispatch otherwise the object cannot be accessed from the Script.
    if( dispidMember == DISPID_VALUE && wFlags == DISPATCH_PROPERTYGET
    && m_defaultValueType != VT_EMPTY && pvarResult != NULL)
    {
        bHandled= sal_True;
        if( m_defaultValueType == VT_DISPATCH)
        {
            pvarResult->vt= VT_DISPATCH;
            pvarResult->pdispVal= static_cast<IDispatch*>( this);
            AddRef();
            ret= S_OK;
        }
    }
// ---------
    // function: _GetValueObject
    else if( dispidMember == DISPID_JSCRIPT_VALUE_FUNC)
    {
        bHandled= sal_True;
        if( !pvarResult)
            ret= E_POINTER;
        CComObject< JScriptValue>* pValue;
        if( SUCCEEDED( CComObject<JScriptValue>::CreateInstance( &pValue)))
        {
            pValue->AddRef();
            pvarResult->vt= VT_DISPATCH;
            pvarResult->pdispVal= CComQIPtr<IDispatch>(pValue->GetUnknown());
            ret= S_OK;
        }
        else
            ret= DISP_E_EXCEPTION;
    }
    else if( dispidMember == DISPID_GET_STRUCT_FUNC)
    {
        bHandled= sal_True;
        sal_Bool bStruct= sal_False;


        Reference<XInterface> xIntCore= m_smgr->createInstance( OUString::createFromAscii("com.sun.star.reflection.CoreReflection"));
        Reference<XIdlReflection> xRefl( xIntCore, UNO_QUERY);
        if( xRefl.is() )
        {
            // the first parameter is in DISPPARAMS rgvargs contains the name of the struct.
            CComVariant arg;
            if( pdispparams->cArgs == 1 && SUCCEEDED( arg.ChangeType( VT_BSTR, &pdispparams->rgvarg[0])) )
            {
                Reference<XIdlClass> classStruct= xRefl->forName( arg.bstrVal);
                if( classStruct.is())
                {
                    Any anyStruct;
                    classStruct->createObject( anyStruct);
                    CComVariant var;
                    if( anyToVariant( &var, anyStruct ))
                    {
                        if( var.vt == VT_DISPATCH)
                        {
                            VariantCopy( pvarResult, & var);
                            bStruct= sal_True;
                        }
                    }

                }
            }
        }
        ret= bStruct == sal_True ? S_OK : DISP_E_EXCEPTION;
    }
    return ret;
}




STDMETHODIMP InterfaceOleWrapper_Impl::GetDispID(BSTR bstrName, DWORD grfdex, DISPID __RPC_FAR *pid)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::InvokeEx(
    /* [in] */ DISPID id,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [in] */ DISPPARAMS __RPC_FAR *pdp,
    /* [out] */ VARIANT __RPC_FAR *pvarRes,
    /* [out] */ EXCEPINFO __RPC_FAR *pei,
    /* [unique][in] */ IServiceProvider __RPC_FAR *pspCaller)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}


STDMETHODIMP InterfaceOleWrapper_Impl::DeleteMemberByName(
    /* [in] */ BSTR bstr,
    /* [in] */ DWORD grfdex)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::DeleteMemberByDispID(DISPID id)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetMemberProperties(
    /* [in] */ DISPID id,
    /* [in] */ DWORD grfdexFetch,
    /* [out] */ DWORD __RPC_FAR *pgrfdex)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetMemberName(
    /* [in] */ DISPID id,
    /* [out] */ BSTR __RPC_FAR *pbstrName)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetNextDispID(
    /* [in] */ DWORD grfdex,
    /* [in] */ DISPID id,
    /* [out] */ DISPID __RPC_FAR *pid)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}

STDMETHODIMP InterfaceOleWrapper_Impl::GetNameSpaceParent(
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunk)
{
    HRESULT ret = ResultFromScode(E_NOTIMPL);

    return ret;
}


/*************************************************************************

    UnoObjectWrapperRemoteOpt

*************************************************************************/
UnoObjectWrapperRemoteOpt::UnoObjectWrapperRemoteOpt( Reference<XMultiServiceFactory>& aFactory,
                                                     sal_uInt8 unoWrapperClass, sal_uInt8 comWrapperClass):
InterfaceOleWrapper_Impl( aFactory, unoWrapperClass, comWrapperClass),
m_currentId(1)

{
}
UnoObjectWrapperRemoteOpt::~UnoObjectWrapperRemoteOpt()
{
}

// UnoConversionUtilities
Reference< XInterface > UnoObjectWrapperRemoteOpt::createUnoWrapperInstance()
{
    Reference<XWeak> xWeak= static_cast<XWeak*>( new UnoObjectWrapperRemoteOpt(
                                                 m_smgr, m_nUnoWrapperClass, m_nComWrapperClass));
    return Reference<XInterface>( xWeak, UNO_QUERY);
}

STDMETHODIMP  UnoObjectWrapperRemoteOpt::GetIDsOfNames ( REFIID riid, OLECHAR ** rgszNames, unsigned int cNames,
                                LCID lcid, DISPID * rgdispid )
{
    MutexGuard guard( getBridgeMutex());

    if( ! rgdispid)
        return E_POINTER;
    HRESULT ret = E_UNEXPECTED;
    // ----------------------------------------
    // _GetValueObject
    if( ! wcscmp( *rgszNames, JSCRIPT_VALUE_FUNC))
    {
        *rgdispid= DISPID_JSCRIPT_VALUE_FUNC;
        return S_OK;
    }
    else if( ! wcscmp( *rgszNames, GET_STRUCT_FUNC))
    {
        *rgdispid= DISPID_GET_STRUCT_FUNC;
        return S_OK;
    }

    // ----------------------------------------
    if (m_xInvocation.is() && (cNames > 0))
    {
        OUString name(rgszNames[0]);
        // has this name been determined as "bad"
        BadNameMap::iterator badIter= m_badNameMap.find( name);
        if( badIter == m_badNameMap.end() )
        {
            // name has not been bad before( member exists
            typedef NameToIdMap::iterator ITnames;
            pair< ITnames, bool > pair_id= m_nameToDispIdMap.insert( NameToIdMap::value_type(name, m_currentId++));
            // new ID inserted ?
            if( pair_id.second )
            {// yes, now create MemberInfo and ad to IdToMemberInfoMap
                MemberInfo d(0, name);
                m_idToMemberInfoMap.insert( IdToMemberInfoMap::value_type( m_currentId - 1, d));
            }

            *rgdispid = pair_id.first->second;
            ret = S_OK;
        }
        else
            ret= DISP_E_UNKNOWNNAME;
    }
    return ret;
}

STDMETHODIMP  UnoObjectWrapperRemoteOpt::Invoke ( DISPID dispidMember, REFIID riid, LCID lcid, unsigned short wFlags,
                         DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo,
                         unsigned int * puArgErr )
{
    HRESULT ret = S_OK;

    sal_Bool bHandled= sal_False;
    ret= InvokeGeneral( dispidMember,  wFlags, pdispparams, pvarResult,  pexcepinfo,
                   puArgErr, bHandled);
    if( bHandled)
        return ret;

    if ( dispidMember > 0  && m_xInvocation.is())
    {

        IdToMemberInfoMap::iterator it_MemberInfo= m_idToMemberInfoMap.find( dispidMember);
        if( it_MemberInfo != m_idToMemberInfoMap.end() )
        {
            MemberInfo& info= it_MemberInfo->second;

            Sequence<Any> params; // holds converted any s
            if( ! info.flags )
            { // DISPID called for the first time
                if( wFlags == DISPATCH_METHOD )
                {

                    if( SUCCEEDED(ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params )))
                    {
                        if( FAILED( ret= doInvoke( pdispparams, pvarResult,
                                  pexcepinfo, puArgErr, info.name, params))
                                  && ret == DISP_E_MEMBERNOTFOUND)
                        {
                            // try to get the exact name
                            OUString exactName;
                            if (m_xExactName.is())
                            {
                                exactName = m_xExactName->getExactName( info.name);
                                // invoke again
                                if( exactName.getLength() != 0)
                                {
                                    if( SUCCEEDED( ret= doInvoke( pdispparams, pvarResult,
                                                        pexcepinfo, puArgErr, exactName, params)))
                                      info.name= exactName;
                                }
                            }
                        }
                        if( SUCCEEDED( ret ) )
                            info.flags= DISPATCH_METHOD;

                    } // end if ( convertDispparamsArgs )
                    else // convertDispparamsArgs failed
                        ret= DISP_E_BADVARTYPE;
                } //if( wFlags == DISPATCH_METHOD )

                else if( wFlags == DISPATCH_PROPERTYPUT || wFlags == DISPATCH_PROPERTYPUTREF)
                {
                    if( SUCCEEDED( ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params )))
                    {
                        if( FAILED( ret= doSetProperty( pdispparams, pvarResult,
                                  pexcepinfo, puArgErr, info.name, params))
                                  && ret == DISP_E_MEMBERNOTFOUND)
                        {
                            // try to get the exact name
                            OUString exactName;
                            if (m_xExactName.is())
                            {
                                exactName = m_xExactName->getExactName( info.name);
                                // invoke again
                                if( exactName.getLength() != 0)
                                {
                                    if( SUCCEEDED( ret= doSetProperty( pdispparams, pvarResult,
                                                        pexcepinfo, puArgErr, exactName, params)))
                                      info.name= exactName;
                                }
                            }
                        }
                        if( SUCCEEDED( ret ) )
                            info.flags= DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYGET;
                    } // end if ( convertDispparamsArgs )
                    else
                        ret= DISP_E_BADVARTYPE;

                }

                else if( wFlags == DISPATCH_PROPERTYGET)
                {
                    if( FAILED( ret= doGetProperty( pdispparams, pvarResult,
                                                    pexcepinfo, info.name))
                              && ret == DISP_E_MEMBERNOTFOUND)
                    {
                        // try to get the exact name
                        OUString exactName;
                        if (m_xExactName.is())
                        {
                            exactName = m_xExactName->getExactName( info.name);
                            // invoke again
                            if( exactName.getLength() != 0)
                            {
                                if( SUCCEEDED( ret= doGetProperty( pdispparams, pvarResult,
                                                    pexcepinfo, exactName)))
                                  info.name= exactName;
                            }
                        }
                    }
                    if( SUCCEEDED( ret ) )
                        info.flags= DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT;
                }
                else if( wFlags & DISPATCH_METHOD &&
                    (wFlags & DISPATCH_PROPERTYPUT || wFlags & DISPATCH_PROPERTYPUTREF))
                {

                    OUString exactName;
                    // convert params for DISPATCH_METHOD or DISPATCH_PROPERTYPUT
                    if( SUCCEEDED( ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params )) )
                    {// try first as method
                        if( FAILED( ret= doInvoke( pdispparams, pvarResult,
                                  pexcepinfo, puArgErr, info.name, params))
                                  && ret == DISP_E_MEMBERNOTFOUND)
                        {
                            // try to get the exact name
                            if (m_xExactName.is())
                            {
                                exactName = m_xExactName->getExactName( info.name);
                                // invoke again
                                if( exactName.getLength() != 0)
                                {
                                    if( SUCCEEDED( ret= doInvoke( pdispparams, pvarResult,
                                                        pexcepinfo, puArgErr, exactName, params)))
                                      info.name= exactName;
                                }
                            }
                        }
                        if( SUCCEEDED( ret ) )
                            info.flags= DISPATCH_METHOD;

                        // try as property
                        if( FAILED( ret) && pdispparams->cArgs == 1)
                        {
                            if( FAILED( ret= doSetProperty( pdispparams, pvarResult,
                                      pexcepinfo, puArgErr, info.name, params))
                                      && ret == DISP_E_MEMBERNOTFOUND)
                            {
                                // try to get the exact name
                                if( exactName.getLength() != 0)
                                {
                                    if( SUCCEEDED( ret= doSetProperty( pdispparams, pvarResult,
                                                        pexcepinfo, puArgErr, exactName, params)))
                                      info.name= exactName;
                                }
                            }
                            if( SUCCEEDED( ret ) )
                                info.flags= DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYGET;
                        }
                    } // end if ( convertDispparamsArgs )
                    else
                        ret= DISP_E_BADVARTYPE;
                }
                else if( wFlags & DISPATCH_METHOD && wFlags & DISPATCH_PROPERTYGET)
                {
                    OUString exactName;
                    if( SUCCEEDED(ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params )) )
                    {
                        if( FAILED( ret= doInvoke( pdispparams, pvarResult,
                                  pexcepinfo, puArgErr, info.name, params))
                                  && ret == DISP_E_MEMBERNOTFOUND)
                        {
                            // try to get the exact name
                            if (m_xExactName.is())
                            {
                                exactName = m_xExactName->getExactName( info.name);
                                // invoke again
                                if( exactName.getLength() != 0)
                                {
                                    if( SUCCEEDED( ret= doInvoke( pdispparams, pvarResult,
                                                        pexcepinfo, puArgErr, exactName, params)))
                                      info.name= exactName;
                                }
                            }
                        }
                        if( SUCCEEDED( ret ) )
                            info.flags= DISPATCH_METHOD;

                    } // end if ( convertDispparamsArgs )
                    else
                        ret= DISP_E_BADVARTYPE;

                    // try as property
                    if( FAILED( ret) && pdispparams->cArgs == 1)
                    {
                        if( FAILED( ret= doGetProperty( pdispparams, pvarResult,
                                  pexcepinfo, info.name))
                                  && ret == DISP_E_MEMBERNOTFOUND)
                        {
                            if( exactName.getLength() != 0)
                            {
                                if( SUCCEEDED( ret= doSetProperty( pdispparams, pvarResult,
                                                    pexcepinfo, puArgErr, exactName, params)))
                                  info.name= exactName;
                            }
                        }
                        if( SUCCEEDED( ret ) )
                            info.flags= DISPATCH_PROPERTYGET;
                    }
                }

                // update �nformation about this member
                if( ret == DISP_E_MEMBERNOTFOUND)
                {
                    // Remember the name as not existing
                    // and remove the MemberInfo
                    m_badNameMap[info.name]= sal_False;
                    m_idToMemberInfoMap.erase( it_MemberInfo);
                }
            } // if( ! info.flags )
            else // IdToMemberInfoMap contains a MemberInfo
            {
                if( wFlags & DISPATCH_METHOD && info.flags == DISPATCH_METHOD)
                {
                    if( SUCCEEDED(ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params )) )
                    {
                        ret= doInvoke( pdispparams, pvarResult,
                                  pexcepinfo, puArgErr, info.name, params);
                    }
                    else
                        ret= DISP_E_BADVARTYPE;
                }
                else if( (wFlags & DISPATCH_PROPERTYPUT || wFlags & DISPATCH_PROPERTYPUTREF )  &&
                        info.flags & DISPATCH_PROPERTYPUT)
                {
                    if( SUCCEEDED( ret= convertDispparamsArgs(dispidMember, wFlags, pdispparams, params )) )
                    {
                        ret= doSetProperty( pdispparams, pvarResult,
                                  pexcepinfo, puArgErr, info.name, params);
                    }
                    else
                        ret= DISP_E_BADVARTYPE;
                }
                else if( (wFlags & DISPATCH_PROPERTYGET) && ( info.flags & DISPATCH_PROPERTYGET))
                {
                    ret= doGetProperty( pdispparams, pvarResult,
                              pexcepinfo, info.name);
                }
                else
                {
                    ret= DISP_E_MEMBERNOTFOUND;
                }
            }
        }//     if( it_MemberInfo != m_idToMemberInfoMap.end() )
        else
            ret= DISP_E_MEMBERNOTFOUND;
    }
    return ret;
}

HRESULT UnoObjectWrapperRemoteOpt::methodInvoke( DISPID dispidMember, DISPPARAMS * pdispparams, VARIANT * pvarResult,
                              EXCEPINFO * pexcepinfo, unsigned int * puArgErr, Sequence<Any> params)
{
    return S_OK;
}


// The returned HRESULT is only appropriate for IDispatch::Invoke
static HRESULT mapCannotConvertException( CannotConvertException e, unsigned int * puArgErr)
{
    HRESULT ret;
    sal_Bool bWriteIndex= sal_True;

    switch ( e.Reason)
    {
        case FailReason::OUT_OF_RANGE:
            ret = DISP_E_OVERFLOW;
            break;
        case FailReason::IS_NOT_NUMBER:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::IS_NOT_ENUM:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::IS_NOT_BOOL:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::NO_SUCH_INTERFACE:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::SOURCE_IS_NO_DERIVED_TYPE:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::TYPE_NOT_SUPPORTED:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::INVALID:
            ret = DISP_E_TYPEMISMATCH;
            break;
        case FailReason::NO_DEFAULT_AVAILABLE:
            ret = DISP_E_BADPARAMCOUNT;
            break;
        case FailReason::UNKNOWN:
            ret = E_UNEXPECTED;
            break;
        default:
            ret = E_UNEXPECTED;
            bWriteIndex= sal_False;
            break;
    }

    if( bWriteIndex &&  puArgErr != NULL)
        *puArgErr = e.ArgumentIndex;
    return ret;
}

// The function maps the TypeClass of the any to VARTYPE: If
// the Any contains STRUCT or INTERFACE then the return value
// is VT_DISPATCH. The function is used from o2u_createUnoObjectWrapper
// and the result is put into the constructor of the uno - wrapper
// object. If a client asks the object for DISPID_VALUE and this
// funtion returned VT_DISPATCH then the IDispatch of the same
// object is being returned.
// See InterfaceOleWrapper_Impl::Invoke, InterfaceOleWrapper_Impl::m_defaultValueType
const VARTYPE getVarType( const Any& value)
{
    VARTYPE ret= VT_EMPTY;

    switch ( value.getValueTypeClass())
    {
    case TypeClass_STRUCT: ret= VT_DISPATCH; break;
    case TypeClass_INTERFACE: ret= VT_DISPATCH; break;
    default: break;
    }
    return ret;
}




} // end namespace
