/*************************************************************************
 *
 *  $RCSfile: tools.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 16:44:09 $
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

#ifndef _CONNECTIVITY_JAVA_TOOLS_HXX_
#define _CONNECTIVITY_JAVA_TOOLS_HXX_

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef JNI_H
#include <jni.h>
#endif
#ifndef _CONNECTIVITY_JAVA_SQL_SQLEXCEPTION_HXX_
#include "java/sql/SQLException.hxx"
#endif

#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif

#include <com/sun/star/container/XNameAccess.hpp>
#ifndef _COM_SUN_STAR_UTIL_TIME_HPP_
#include <com/sun/star/util/Time.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATE_HPP_
#include <com/sun/star/util/Date.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif


namespace connectivity
{

    jstring convertwchar_tToJavaString(JNIEnv *pEnv,const ::rtl::OUString& _Temp);
    ::rtl::OUString JavaString2String(JNIEnv *pEnv,jstring _Str);
    class java_util_Properties;

    java_util_Properties* createStringPropertyArray(JNIEnv *pEnv,const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);

    template<class T,class JT> ::com::sun::star::uno::Sequence< T > copyArrayAndDelete(JNIEnv *pEnv,jobjectArray _Array, const T*, const JT* )
    {
                ::com::sun::star::uno::Sequence< T > xOut;
        if(_Array)
        {
            jsize  nLen = pEnv->GetArrayLength(_Array);
            xOut.realloc(nLen);
            for(jsize i=0;i<nLen;++i)
            {
                JT xInfo(pEnv,pEnv->GetObjectArrayElement(_Array,i));
                java_lang_Object::ThrowSQLException(pEnv,NULL);
                xOut.getArray()[i] = xInfo;
            }
            pEnv->DeleteLocalRef(_Array);
        }
        return xOut;
    }

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > Map2XNameAccess(JNIEnv *pEnv,jobject _pMap);
    jobject XNameAccess2Map(JNIEnv *pEnv,const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & _rMap);

    /** return if a exception occured
        @param  pEnv
            The native java env
        @param  _bClear
            <TRUE/> if the execption should be cleared
        @return
            <TRUE/> if an exception is occured
    */
    sal_Bool isExceptionOccured(JNIEnv *pEnv,sal_Bool _bClear);
}

#endif // _CONNECTIVITY_JAVA_TOOLS_HXX_

