/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: convert.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:15:43 $
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

#ifndef _CONVERT_HXX
#define _CONVERT_HXX

#include <com/sun/star/uno/Sequence.hxx>
#include <map>

namespace com { namespace sun { namespace star { namespace uno
{
    class Any;
    class Type;
} } } }
namespace rtl { class OUString; }
class ConvertImpl;

namespace xforms
{

struct TypeLess
{
    bool operator()( const com::sun::star::uno::Type& rType1,
                     const com::sun::star::uno::Type& rType2 ) const
    { return rType1.getTypeName() < rType2.getTypeName(); }
};

class Convert
{
    typedef com::sun::star::uno::Type Type_t;
    typedef com::sun::star::uno::Sequence<com::sun::star::uno::Type> Types_t;
    typedef com::sun::star::uno::Any Any_t;

    // hold conversion objects
    typedef rtl::OUString (*fn_toXSD)( const Any_t& );
    typedef Any_t (*fn_toAny)( const rtl::OUString& );
    typedef std::pair<fn_toXSD,fn_toAny> Convert_t;
    typedef std::map<Type_t,Convert_t,TypeLess> Map_t;
    Map_t maMap;

    Convert();
    ~Convert();

    void init();

public:
    /** get/create Singleton class */
    static Convert& get();

    /// can we convert this type?
    bool hasType( const Type_t& );

    /// get list of convertable types
    Types_t getTypes();

    /// convert any to XML representation
    rtl::OUString toXSD( const Any_t& rAny );

    /// convert XML representation to Any of given type
    Any_t toAny( const rtl::OUString&, const Type_t& );

    /** translates the whitespaces in a given string, according
        to a given <type scope="com::sun::star::xsd">WhiteSpaceTreatment</type>.

        @param _rString
            the string to convert
        @param _nWhitespaceTreatment
            a constant from the <type scope="com::sun::star::xsd">WhiteSpaceTreatment</type> group, specifying
            how to handle whitespaces
        @return
            the converted string
    */
    static ::rtl::OUString convertWhitespace(
            const ::rtl::OUString& _rString,
            sal_Int16   _nWhitespaceTreatment
    );

    /** replace all occurences 0x08, 0x0A, 0x0D with 0x20
    */
    static ::rtl::OUString replaceWhitespace( const ::rtl::OUString& _rString );

    /** replace all sequences of 0x08, 0x0A, 0x0D, 0x20 with a single 0x20.
        also strip leading/trailing whitespace.
    */
    static ::rtl::OUString collapseWhitespace( const ::rtl::OUString& _rString );
};

} // namespace xforms

#endif
