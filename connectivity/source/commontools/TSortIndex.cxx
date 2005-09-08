/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TSortIndex.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 05:13:14 $
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
#ifndef CONNECTIVITY_TSORTINDEX_HXX
#include "TSortIndex.hxx"
#endif
#include <algorithm>
#include <functional>

using namespace connectivity;
//------------------------------------------------------------------
/// binary_function Functor object for class OSortIndex::TIntValuePairVector::value_type returntype is bool
struct TKeyValueFunc : ::std::binary_function<OSortIndex::TIntValuePairVector::value_type,OSortIndex::TIntValuePairVector::value_type,bool>
{
    OSortIndex* pIndex;

    TKeyValueFunc(OSortIndex* _pIndex) : pIndex(_pIndex)
    {
    }
    // return false if compared values are equal otherwise true
    inline bool operator()(const OSortIndex::TIntValuePairVector::value_type& lhs,const OSortIndex::TIntValuePairVector::value_type& rhs)   const
    {
        const ::std::vector<OKeyType>& aKeyType = pIndex->getKeyType();
        ::std::vector<OKeyType>::const_iterator aIter = aKeyType.begin();
        for (::std::vector<sal_Int16>::size_type i=0;aIter != aKeyType.end(); ++aIter,++i)
        {
            const bool nGreater = (pIndex->getAscending(i) == SQL_ASC) ? false : true;
            const bool nLess = !nGreater;

            // compare depending for type
            switch (*aIter)
            {
                case SQL_ORDERBYKEY_STRING:
                {
                    sal_Int32 nRes = lhs.second->getKeyString(i).compareTo(rhs.second->getKeyString(i));
                    if (nRes < 0)
                        return nLess;
                    else if (nRes > 0)
                        return nGreater;
                }
                break;
                case SQL_ORDERBYKEY_DOUBLE:
                {
                    double d1 = lhs.second->getKeyDouble(i);
                    double d2 = rhs.second->getKeyDouble(i);

                    if (d1 < d2)
                        return nLess;
                    else if (d1 > d2)
                        return nGreater;
                }
                break;
            }
        }

        // know we know that the values are equal
        return false;
    }
};

// -----------------------------------------------------------------------------
::vos::ORef<OKeySet> OSortIndex::CreateKeySet()
{
    Freeze();

    ::vos::ORef<OKeySet> pKeySet = new OKeySet();
    pKeySet->reserve(m_aKeyValues.size());
    ::std::transform(m_aKeyValues.begin()
                    ,m_aKeyValues.end()
                    ,::std::back_inserter(*pKeySet)
                    ,::std::select1st<TIntValuePairVector::value_type>());
    pKeySet->setFrozen();
    return pKeySet;
}
// -----------------------------------------------------------------------------
OSortIndex::OSortIndex( const ::std::vector<OKeyType>& _aKeyType,
                        const ::std::vector<sal_Int16>& _aAscending)
    : m_bFrozen(sal_False)
     ,m_aAscending(_aAscending)
     ,m_aKeyType(_aKeyType)
{
}
//------------------------------------------------------------------
OSortIndex::~OSortIndex()
{
}
//------------------------------------------------------------------
void OSortIndex::AddKeyValue(OKeyValue * pKeyValue)
{
    OSL_ENSURE(pKeyValue,"Can not be null here!");
    if(m_bFrozen)
    {
        m_aKeyValues.push_back(TIntValuePairVector::value_type(pKeyValue->getValue(),NULL));
        delete pKeyValue;
    }
    else
        m_aKeyValues.push_back(TIntValuePairVector::value_type(pKeyValue->getValue(),pKeyValue));
}


//------------------------------------------------------------------
void OSortIndex::Freeze()
{
    OSL_ENSURE(! m_bFrozen,"OSortIndex::Freeze: already frozen!");
    // Sortierung:
    if (m_aKeyType[0] != SQL_ORDERBYKEY_NONE)
        // we will sort ourself when the first keyType say so
        ::std::sort(m_aKeyValues.begin(),m_aKeyValues.end(),TKeyValueFunc(this));

    TIntValuePairVector::iterator aIter = m_aKeyValues.begin();
    for(;aIter != m_aKeyValues.end();++aIter)
    {
        delete aIter->second;
        aIter->second = NULL;
    }

    m_bFrozen = sal_True;
}

//------------------------------------------------------------------
sal_Int32 OSortIndex::GetValue(sal_Int32 nPos) const
{
    OSL_ENSURE(nPos > 0,"OSortIndex::GetValue: nPos == 0");
    OSL_ENSURE(nPos <= m_aKeyValues.size(),"OSortIndex::GetValue: Zugriff ausserhalb der Array-Grenzen");

    if (!m_bFrozen && m_aKeyType[0] != SQL_ORDERBYKEY_NONE)
    {
        OSL_ASSERT("OSortIndex::GetValue: Invalid use of index!");
        return 0;
    }
    return m_aKeyValues[nPos-1].first;
}
// -----------------------------------------------------------------------------
OKeyValue::OKeyValue()
{
}
// -----------------------------------------------------------------------------
OKeyValue::OKeyValue(sal_Int32 nVal)
: m_nValue(nVal)
{
}
// -----------------------------------------------------------------------------
OKeyValue::~OKeyValue()
{
}
// -----------------------------------------------------------------------------
OKeyValue* OKeyValue::createKeyValue(sal_Int32 _nVal)
{
    return new OKeyValue(_nVal);
}
// -----------------------------------------------------------------------------

