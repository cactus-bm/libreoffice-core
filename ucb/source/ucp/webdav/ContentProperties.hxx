/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ContentProperties.hxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: kz $ $Date: 2007-06-19 16:12:03 $
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

#ifndef _WEBDAV_UCP_CONTENTPROPERTIES_HXX
#define _WEBDAV_UCP_CONTENTPROPERTIES_HXX

#include <memory>
#include <vector>
#include <hash_map>

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

namespace com { namespace sun { namespace star { namespace beans {
    struct Property;
} } } }

namespace webdav_ucp
{

struct DAVResource;

//=========================================================================

struct equalString
{
  bool operator()( const rtl::OUString& s1, const rtl::OUString& s2 ) const
  {
      return !!( s1 == s2 );
  }
};

struct hashString
{
    size_t operator()( const rtl::OUString & rName ) const
    {
        return rName.hashCode();
    }
};

//=========================================================================
//
// PropertyValueMap.
//
//=========================================================================

class PropertyValue
{
private:
    ::com::sun::star::uno::Any m_aValue;
    bool                       m_bIsCaseSensitive;

public:
    PropertyValue()
    : m_bIsCaseSensitive( true ) {}

    PropertyValue( const ::com::sun::star::uno::Any & rValue,
                   bool bIsCaseSensitive )
    : m_aValue( rValue),
      m_bIsCaseSensitive( bIsCaseSensitive ) {}

    bool isCaseSensitive() const { return m_bIsCaseSensitive; }
    const ::com::sun::star::uno::Any & value() const { return m_aValue; }

};

typedef std::hash_map
<
    rtl::OUString,
    PropertyValue,
    hashString,
    equalString
>
PropertyValueMap;

struct DAVResource;

class ContentProperties
{
public:
      ContentProperties( const DAVResource& rResource );

    // Mini props for transient contents.
      ContentProperties( const rtl::OUString & rTitle, sal_Bool bFolder );

    // Micro props for non-existing contents.
    ContentProperties( const rtl::OUString & rTitle );

    ContentProperties( const ContentProperties & rOther );

    bool contains( const rtl::OUString & rName ) const;

    const com::sun::star::uno::Any &
    getValue( const rtl::OUString & rName ) const;

    // Maps the UCB property names contained in rProps with their DAV property
    // counterparts, if possible. All unmappable properties will be included
    // unchanged in resulting vector unless bIncludeUnmatched is set to false.
    // The vector filles by this method can directly be handed over to
    // DAVResourceAccess::PROPFIND. The result from PROPFIND
    // (vector< DAVResource >) can be used to create a ContentProperties
    // instance which can map DAV properties back to UCB properties.
    static void UCBNamesToDAVNames( const com::sun::star::uno::Sequence<
                                        com::sun::star::beans::Property > &
                                            rProps,
                                    std::vector< rtl::OUString > & resources,
                                    bool bIncludeUnmatched = true );

    // Maps the UCB property names contained in rProps with their HTTP header
    // counterparts, if possible. All unmappable properties will be included
    // unchanged in resulting vector unless bIncludeUnmatched is set to false.
    // The vector filles by this method can directly be handed over to
    // DAVResourceAccess::HEAD. The result from HEAD (vector< DAVResource >)
    // can be used to create a ContentProperties instance which can map header
    // names back to UCB properties.
    static void UCBNamesToHTTPNames( const com::sun::star::uno::Sequence<
                                        com::sun::star::beans::Property > &
                                            rProps,
                                    std::vector< rtl::OUString > & resources,
                                    bool bIncludeUnmatched = true );

    // Returns a list of HTTP header names that can be mapped to UCB property
    // names.
    static void getMappableHTTPHeaders(
         std::vector< rtl::OUString > & rHeaderNames );

    // return true, if all properties contained in rProps are contained in
    // this ContentProperties instance. Otherwiese, false will be returned.
    // rNamesNotContained contain the missing names.
    bool containsAllNames(
                    const com::sun::star::uno::Sequence<
                        com::sun::star::beans::Property >& rProps,
                    std::vector< rtl::OUString > & rNamesNotContained ) const;

    // adds all properties described by rProps that are actually contained in
    // rContentProps to this instance. In case of duplicates the value
    // already contained in this will left unchanged.
    void addProperties( const std::vector< rtl::OUString > & rProps,
                        const ContentProperties & rContentProps );

    // overwrites probably existing entry.
    void addProperty( const rtl::OUString & rName,
                      const com::sun::star::uno::Any & rValue,
                      bool bIsCaseSensitive );

    bool isTrailingSlash() const { return m_bTrailingSlash; }

    const rtl::OUString & getEscapedTitle() const { return m_aEscapedTitle; }

    // Not good to expose implementation details, but this is actually an
    // internal class.
    const std::auto_ptr< PropertyValueMap > & getProperties() const
    { return m_xProps; }

private:
    ::rtl::OUString m_aEscapedTitle;  // escaped Title
    std::auto_ptr< PropertyValueMap > m_xProps;
    bool m_bTrailingSlash;

    static com::sun::star::uno::Any m_aEmptyAny;

    ContentProperties & operator=( const ContentProperties & ); // n.i.

    const PropertyValue * get( const rtl::OUString & rName ) const;
};

}

#endif /* !_WEBDAV_UCP_CONTENTPROPERTIES_HXX */
