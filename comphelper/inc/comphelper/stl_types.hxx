/*************************************************************************
 *
 *  $RCSfile: stl_types.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: pl $ $Date: 2001-05-10 12:21:01 $
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
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _COMPHELPER_STLTYPES_HXX_
#define _COMPHELPER_STLTYPES_HXX_

#if !defined(__SGI_STL_VECTOR_H) || !defined(__SGI_STL_MAP_H) || !defined(__SGI_STL_MULTIMAP_H)

#include <vector>
#include <map>
#include <hash_map>
#include <stack>
#include <set>
#include <math.h> // prevent conflict between exception and std::exception

using namespace std;

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

//... namespace comphelper ................................................
namespace comphelper
{
//.........................................................................

//========================================================================
// comparisation functions

//------------------------------------------------------------------------
struct UStringLess : public binary_function< ::rtl::OUString, ::rtl::OUString, bool>
{
    bool operator() (const ::rtl::OUString& x, const ::rtl::OUString& y) const { return x < y ? true : false;}      // construct prevents a MSVC6 warning
};
//------------------------------------------------------------------------
struct UStringMixLess : public binary_function< ::rtl::OUString, ::rtl::OUString, bool>
{
    bool m_bCaseSensitive;
public:
    UStringMixLess(bool bCaseSensitive = true):m_bCaseSensitive(bCaseSensitive){}
    bool operator() (const ::rtl::OUString& x, const ::rtl::OUString& y) const
    {
        if (m_bCaseSensitive)
            return rtl_ustr_compare(x.getStr(), y.getStr()) < 0 ? true : false;
        else
            return rtl_ustr_compareIgnoreAsciiCase(x.getStr(), y.getStr()) < 0 ? true : false;
    }

    bool isCaseSensitive() const {return m_bCaseSensitive;}
};
//------------------------------------------------------------------------
struct UStringEqual
{
    sal_Bool operator() (const ::rtl::OUString& lhs, const ::rtl::OUString& rhs) const { return lhs.equals( rhs );}
};

//------------------------------------------------------------------------
struct UStringIEqual
{
    sal_Bool operator() (const ::rtl::OUString& lhs, const ::rtl::OUString& rhs) const { return lhs.equalsIgnoreAsciiCase( rhs );}
};

//------------------------------------------------------------------------
struct UStringHash
{
    size_t operator() (const ::rtl::OUString& rStr) const {return rStr.hashCode();}
};

//------------------------------------------------------------------------
class UStringMixEqual
{
    sal_Bool m_bCaseSensitive;

public:
    UStringMixEqual(sal_Bool bCaseSensitive = sal_True):m_bCaseSensitive(bCaseSensitive){}
    sal_Bool operator() (const ::rtl::OUString& lhs, const ::rtl::OUString& rhs) const
    {
        return m_bCaseSensitive ? lhs.equals( rhs ) : lhs.equalsIgnoreAsciiCase( rhs );
    }
    sal_Bool isCaseSensitive() const {return m_bCaseSensitive;}
};

//------------------------------------------------------------------------
class UStringMixHash
{
    sal_Bool m_bCaseSensitive;

public:
    UStringMixHash(sal_Bool bCaseSensitive = sal_True):m_bCaseSensitive(bCaseSensitive){}
    size_t operator() (const ::rtl::OUString& rStr) const
    {
        return m_bCaseSensitive ? rStr.hashCode() : rStr.toAsciiUpperCase().hashCode();
    }
    sal_Bool isCaseSensitive() const {return m_bCaseSensitive;}
};

//.........................................................................
}
//... namespace comphelper ................................................

//==================================================================
// consistently defining stl-types
//==================================================================

#define DECLARE_STL_ITERATORS(classname)                            \
    typedef classname::iterator         classname##Iterator;        \
    typedef classname::const_iterator   Const##classname##Iterator  \

#define DECLARE_STL_MAP(keytype, valuetype, comparefct, classname)  \
    typedef std::map< keytype, valuetype, comparefct >  classname;  \
    DECLARE_STL_ITERATORS(classname)                                \

#define DECLARE_STL_STDKEY_MAP(keytype, valuetype, classname)               \
    DECLARE_STL_MAP(keytype, valuetype, std::less< keytype >, classname)    \

#define DECLARE_STL_VECTOR(valuetyp, classname)     \
    typedef std::vector< valuetyp >     classname;  \
    DECLARE_STL_ITERATORS(classname)                \

#define DECLARE_STL_USTRINGACCESS_MAP(valuetype, classname)                 \
    DECLARE_STL_MAP(::rtl::OUString, valuetype, ::comphelper::UStringLess, classname)   \

#define DECLARE_STL_STDKEY_SET(valuetype, classname)    \
    typedef ::std::set< valuetype > classname;          \
    DECLARE_STL_ITERATORS(classname)                    \

#define DECLARE_STL_SET(valuetype, comparefct, classname)               \
    typedef ::std::set< valuetype, comparefct > classname;  \
    DECLARE_STL_ITERATORS(classname)                        \

#endif

#endif  // _COMPHELPER_STLTYPES_HXX_

