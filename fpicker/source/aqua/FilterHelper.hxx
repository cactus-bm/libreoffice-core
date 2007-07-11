/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: FilterHelper.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: ihi $ $Date: 2007-07-11 10:58:41 $
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

#ifndef _FILTERHELPER_HXX_
#define _FILTERHELPER_HXX_

#ifndef _COM_SUN_STAR_UNO_TYPE_HXX_
#include <com/sun/star/uno/Type.hxx>
#endif

#ifndef INCLUDED_CPPU_UNOTYPE_HXX
#include <cppu/unotype.hxx>
#endif

#ifndef _TYPELIB_TYPECLASS_H_
#include "typelib/typeclass.h"
#endif

#ifndef _TYPELIB_TYPEDESCRIPTION_H_
#include "typelib/typedescription.h"
#endif

#ifndef _COM_SUN_STAR_BEANS_STRINGPAIR_HPP_
#include <com/sun/star/beans/StringPair.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HXX_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HXX_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif

#ifndef _RTL_USTRING_H_
#include <rtl/ustring.hxx>
#endif

#include <list>
#include <vector>

#include <premac.h>
#include <Carbon/Carbon.h>
#include <postmac.h>

typedef ::com::sun::star::beans::StringPair UnoFilterEntry;
typedef ::com::sun::star::uno::Sequence< UnoFilterEntry >   UnoFilterList;  // can be transported more effectively
typedef ::std::list<CFStringRef> CFStringList;
typedef ::std::list<rtl::OUString> OUStringList;

struct FilterEntry
{
protected:
    rtl::OUString       m_sTitle;
    OUStringList        m_sFilterSuffixList;
    UnoFilterList       m_aSubFilters;

public:
    FilterEntry( const rtl::OUString& _rTitle, const OUStringList _rFilter )
    : m_sTitle( _rTitle )
    , m_sFilterSuffixList( _rFilter )
    {
    }

    FilterEntry( const rtl::OUString& _rTitle, const UnoFilterList& _rSubFilters );

    rtl::OUString       getTitle() const { return m_sTitle; }
    OUStringList    getFilterSuffixList() const { return m_sFilterSuffixList; }

    /// determines if the filter has sub filter (i.e., the filter is a filter group in real)
    sal_Bool        hasSubFilters( ) const;

    /** retrieves the filters belonging to the entry
        @return
        the number of sub filters
        */
    sal_Int32       getSubFilters( UnoFilterList& _rSubFilterList );

    // helpers for iterating the sub filters
    const UnoFilterEntry*   beginSubFilters() const { return m_aSubFilters.getConstArray(); }
    const UnoFilterEntry*   endSubFilters() const { return m_aSubFilters.getConstArray() + m_aSubFilters.getLength(); }
};

typedef ::std::vector < FilterEntry >       FilterList;

class FilterHelper {

public:
    FilterHelper();
    virtual ~FilterHelper();

    //XFilterManager delegates
    void SAL_CALL appendFilter( const ::rtl::OUString& aTitle, const ::rtl::OUString& aFilter )
        throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );

    void SAL_CALL setCurrentFilter( const ::rtl::OUString& aTitle )
        throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );

    ::rtl::OUString SAL_CALL getCurrentFilter(  )
        throw( ::com::sun::star::uno::RuntimeException );

    //XFilterGroupManager delegates
    void SAL_CALL appendFilterGroup( const ::rtl::OUString& sGroupTitle, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair >& aFilters )
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);


    //accessor
    FilterList* getFilterList();
    CFStringList* getFilterNames();

    //misc
    void SetCurFilter( const rtl::OUString& rFilter );
    void SetFilterAtIndex(unsigned index);
    OUStringList getCurrentFilterSuffixList();
    int getCurrentFilterIndex();
    void SetFilters();
    sal_Bool SAL_CALL implFilterHandler(AEDesc *theItem, void *info,
                                                void *callBackUD,
                                                NavFilterModes filterMode);

private:
    FilterList *m_pFilterList;
    rtl::OUString m_aCurrentFilter;
    CFStringList *m_pFilterNames;

    int implAddFilter( const rtl::OUString rFilter, const OUStringList rSuffixList);
    int implAddFilterGroup( const rtl::OUString rFilter,
                            const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair>& _rFilters );

    sal_Bool FilterNameExists( const rtl::OUString rTitle );
    sal_Bool FilterNameExists( const UnoFilterList& _rGroupedFilters );

    void ensureFilterList( const ::rtl::OUString& _rInitialCurrentFilter );

    void fillSuffixList(OUStringList& aSuffixList, const ::rtl::OUString& suffixString);

};

#endif //_FILTERHELPER_HXX_
