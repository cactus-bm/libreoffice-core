/*************************************************************************
 *
 *  $RCSfile: Aolevariant.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-18 08:31:17 $
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
#ifndef _CONNECTIVITY_ADO_AOLEVARIANT_HXX_
#define _CONNECTIVITY_ADO_AOLEVARIANT_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#include <oaidl.h>
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace util
            {
                struct Date;
                struct Time;
                struct DateTime;
            }
        }
    }
}
namespace connectivity
{
    namespace ado
    {
        class OLEString
        {
            BSTR m_sStr;
        public:
            OLEString();
            OLEString(const BSTR& _sBStr);
            OLEString(const ::rtl::OUString& _sBStr);
            ~OLEString();
            OLEString& operator=(const ::rtl::OUString& _rSrc);
            OLEString& operator=(const BSTR& _rSrc);
            operator ::rtl::OUString() const;
            operator BSTR() const;
            BSTR* operator &();
            sal_Int32 length() const;
        };

        class OLEVariant    :   public ::tagVARIANT
        {
        public:
            OLEVariant();
            OLEVariant(const VARIANT& varSrc);
            OLEVariant(const OLEVariant& varSrc)    ;
            OLEVariant(sal_Bool x)  ;
            OLEVariant(sal_Int8 n)  ;
            OLEVariant(sal_Int16 n) ;
            OLEVariant(sal_Int32 n) ;
            OLEVariant(sal_Int64 x) ;

            OLEVariant(const rtl::OUString& us) ;
            ~OLEVariant()                   ;
            OLEVariant(const ::com::sun::star::util::Date& x );
            OLEVariant(const ::com::sun::star::util::Time& x );
            OLEVariant(const ::com::sun::star::util::DateTime& x );
            OLEVariant(const float &x);
            OLEVariant(const double &x);
            OLEVariant(IDispatch* pDispInterface);
            OLEVariant(const ::com::sun::star::uno::Sequence< sal_Int8 >& x);
            OLEVariant& operator=(const OLEVariant& varSrc);
            // Assign a const VARIANT& (::VariantCopy handles everything)
            //
            OLEVariant& operator=(const tagVARIANT& varSrc);
            // Assign a const VARIANT* (::VariantCopy handles everything)
            //
            OLEVariant& operator=(const VARIANT* pSrc);
            void setByte(sal_uInt8 n)               ;
            void setInt16(sal_Int16 n)              ;
            void setInt32(sal_Int32 n)              ;
            void setFloat(float f)                  ;
            void setDouble(double d)                ;
            void setDate(DATE d)                    ;
            void setChar(unsigned char a)           ;
            void setCurrency(double aCur)           ;
            void setBool(sal_Bool b)                ;
            void setString(const rtl::OUString& us) ;
            void setNoArg()                         ;

            void setIDispatch(IDispatch* pDispInterface);
            void setNull()   ;
            void setEmpty()  ;

            void setUI1SAFEARRAYPtr(SAFEARRAY* pSafeAr);
            void setArray(SAFEARRAY* pSafeArray, VARTYPE vtType);
            sal_Bool isNull() const  ;
            sal_Bool isEmpty() const ;

            VARTYPE getType() const ;
            void ChangeType(VARTYPE vartype, const OLEVariant* pSrc);


            operator ::rtl::OUString() const;

            operator sal_Bool()     const { return getBool();   }
            operator sal_Int8()     const { return getInt8();   }
            operator sal_Int16()    const { return getInt16();  }
            operator sal_Int32()    const { return getInt32();  }
            operator float()        const { return getFloat();  }
            operator double()       const { return getDouble(); }

            operator ::com::sun::star::uno::Sequence< sal_Int8 >() const;
            operator ::com::sun::star::util::Date() const ;
            operator ::com::sun::star::util::Time() const ;
            operator ::com::sun::star::util::DateTime()const ;
            ::rtl::OUString getString()     const;
            sal_Bool        getBool()       const;
            IUnknown*       getIUnknown()   const;
            IDispatch*      getIDispatch()  const;
            sal_uInt8       getByte()       const;
            sal_Int16       getInt16()      const;
            sal_Int8        getInt8()       const;
            sal_Int32       getInt32()      const;
            sal_uInt32      getUInt32()     const;
            float           getFloat()      const;
            double          getDouble()     const;
            double          getDate()       const;
            CY              getCurrency()   const;
            SAFEARRAY*      getUI1SAFEARRAYPtr() const;

            static VARIANT_BOOL VariantBool(sal_Bool bEinBoolean);

        private:
            void CHS();

            void set(double n);

        };
    }
}

#endif // _CONNECTIVITY_ADO_AOLEVARIANT_HXX_

