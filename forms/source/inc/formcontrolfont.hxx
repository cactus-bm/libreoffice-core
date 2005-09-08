/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: formcontrolfont.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:56:12 $
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

#ifndef FORMS_SOURCE_COMPONENT_FORMCONTROLFONT_HXX
#define FORMS_SOURCE_COMPONENT_FORMCONTROLFONT_HXX

#ifndef _COM_SUN_STAR_IO_XOBJECTOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XObjectOutputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTDESCRIPTOR_HPP_
#include <com/sun/star/awt/FontDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTY_HPP_
#include <com/sun/star/beans/Property.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_ILLEGALARGUMENTEXCEPTION_HPP_
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#endif

//.........................................................................
namespace frm
{
//.........................................................................

    //=====================================================================
    //= FontControlModel
    //=====================================================================
    class FontControlModel
    {
    private:
        // <properties>
        ::com::sun::star::awt::FontDescriptor   m_aFont;
        sal_Int16                               m_nFontRelief;
        sal_Int16                               m_nFontEmphasis;
        ::com::sun::star::uno::Any              m_aTextLineColor;
        ::com::sun::star::uno::Any              m_aTextColor;
        // </properties>

        bool                                    m_bToolkitCompatibleDefaults;

    protected:
        const ::com::sun::star::awt::FontDescriptor& getFont() const { return m_aFont; }
        void setFont( const ::com::sun::star::awt::FontDescriptor& _rFont ) { m_aFont = _rFont; }

        void        setTextColor( sal_Int32 _nColor ) { m_aTextColor <<= _nColor;       }
        void        clearTextColor( )                 { m_aTextColor.clear();           }
        bool        hasTextColor( ) const             { return m_aTextColor.hasValue(); }
        sal_Int32   getTextColor( ) const;

        void        setTextLineColor( sal_Int32 _nColor ) { m_aTextLineColor <<= _nColor;       }
        void        clearTextLineColor( )                 { m_aTextLineColor.clear();           }
        bool        hasTextLineColor( ) const             { return m_aTextLineColor.hasValue(); }
        sal_Int32   getTextLineColor( ) const;

    protected:
        FontControlModel( bool _bToolkitCompatibleDefaults );
        FontControlModel( const FontControlModel* _pOriginal );

    protected:
        bool     isFontRelatedProperty( sal_Int32 _nPropertyHandle ) const;
        bool     isFontAggregateProperty( sal_Int32 _nPropertyHandle ) const;

        /// appends (!) the description of all font related properties to the given sequence
        void     describeFontRelatedProperties(
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps ) const;

        void     getFastPropertyValue            ( ::com::sun::star::uno::Any& _rValue, sal_Int32 _nHandle ) const;
        sal_Bool convertFastPropertyValue        ( ::com::sun::star::uno::Any& _rConvertedValue, ::com::sun::star::uno::Any& _rOldValue, sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue ) throw( ::com::sun::star::lang::IllegalArgumentException );
        void     setFastPropertyValue_NoBroadcast( sal_Int32 _nHandle, const ::com::sun::star::uno::Any& _rValue ) throw ( ::com::sun::star::uno::Exception );
        ::com::sun::star::uno::Any
                 getPropertyDefaultByHandle      ( sal_Int32 _nHandle ) const;

    private:

    private:
        FontControlModel();                                     // never implemented
        FontControlModel( const FontControlModel& );            // never implemented
        FontControlModel& operator=( const FontControlModel& ); // never implemented
    };

//.........................................................................
}   // namespace frm
//.........................................................................

#endif // FORMS_SOURCE_COMPONENT_FORMCONTROLFONT_HXX
