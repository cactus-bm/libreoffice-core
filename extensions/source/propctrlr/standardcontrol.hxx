/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: standardcontrol.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: vg $ $Date: 2006-03-14 11:33:05 $
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

#ifndef _EXTENSIONS_PROPCTRLR_STANDARDCONTROL_HXX_
#define _EXTENSIONS_PROPCTRLR_STANDARDCONTROL_HXX_

#ifndef _EXTENSIONS_PROPCTRLR_COMMONCONTROL_HXX_
#include "commoncontrol.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PCRCOMMON_HXX_
#include "pcrcommon.hxx"
#endif

/** === begin UNO includes === **/
#ifndef _COM_SUN_STAR_INSPECTION_XNUMERICCONTROL_HPP_
#include <com/sun/star/inspection/XNumericControl.hpp>
#endif
#ifndef _COM_SUN_STAR_INSPECTION_XSTRINGLISTCONTROL_HPP_
#include <com/sun/star/inspection/XStringListControl.hpp>
#endif
#ifndef _COM_SUN_STAR_INSPECTION_XHYPERLINKCONTROL_HPP_
#include <com/sun/star/inspection/XHyperlinkControl.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
/** === end UNO includes === **/

#ifndef _SV_FIELD_HXX
#include <vcl/field.hxx>
#endif
#ifndef _LONGCURR_HXX
#include <vcl/longcurr.hxx>
#endif
#ifndef _CTRLBOX_HXX
#include <svtools/ctrlbox.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_COMBOBOX_HXX
#include <vcl/combobox.hxx>
#endif
#ifndef _CALENDAR_HXX
#include <svtools/calendar.hxx>
#endif
#ifndef _FMTFIELD_HXX_
#include <svtools/fmtfield.hxx>
#endif

#include <set>

class PushButton;
class MultiLineEdit;
//............................................................................
namespace pcr
{
//............................................................................

    //========================================================================
    //= ListLikeControlWithModifyHandler
    //========================================================================
    /** Very small helper class which adds a SetModifyHdl to a ListBox-derived class,
        thus giving this class the same API (as far as the CommonBehaviourControl is concerned)
        as all other windows.
    */
    template< class LISTBOX_WINDOW >
    class ListLikeControlWithModifyHandler : public ControlWindow< LISTBOX_WINDOW >
    {
    protected:
        typedef ControlWindow< LISTBOX_WINDOW >  ListBoxType;
    public:
        ListLikeControlWithModifyHandler( Window* _pParent, WinBits _nStyle )
            :ListBoxType( _pParent, _nStyle )
        {
        }

        void SetModifyHdl( const Link& _rLink ) { ListBoxType::SetSelectHdl( _rLink ); }
    };

    //========================================================================
    //= OTimeControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XPropertyControl, ControlWindow< TimeField > > OTimeControl_Base;
    class OTimeControl : public OTimeControl_Base
    {
    public:
        OTimeControl( Window* pParent, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);
    };

    //========================================================================
    //= ODateControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XPropertyControl, ControlWindow< CalendarField > > ODateControl_Base;
    class ODateControl : public ODateControl_Base
    {
    public:
        ODateControl( Window* pParent, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);
    };

    //========================================================================
    //= OEditControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XPropertyControl, ControlWindow< Edit > > OEditControl_Base;
    class OEditControl : public OEditControl_Base
    {
    protected:
        sal_Bool m_bIsPassword : 1;

    public:
        OEditControl( Window* _pParent, sal_Bool _bPassWord, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);

    protected:
        virtual void modified();
    };

    //========================================================================
    //= ODateTimeControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XPropertyControl, ControlWindow< FormattedField > > ODateTimeControl_Base;
    class ODateTimeControl : public ODateTimeControl_Base
    {
    public:
        ODateTimeControl( Window* pParent,WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);
    };

    //========================================================================
    //= HyperlinkInput
    //========================================================================
    class HyperlinkInput : public Edit
    {
    private:
        Point   m_aMouseButtonDownPos;
        Link    m_aClickHandler;

    public:
        HyperlinkInput( Window* _pParent, WinBits _nWinStyle );

        /** sets the handler which will (asynchronously, with locked SolarMutex) be called
            when the hyperlink has been clicked by the user
        */
        void        SetClickHdl( const Link& _rHdl ) { m_aClickHandler = _rHdl; }
        const Link& GetClickHdl( ) const { return m_aClickHandler; }

    protected:
        virtual void        MouseMove( const MouseEvent& rMEvt );
        virtual void        MouseButtonDown( const MouseEvent& rMEvt );
        virtual void        MouseButtonUp( const MouseEvent& rMEvt );
        virtual void        Tracking( const TrackingEvent& rTEvt );

    private:
        void    impl_checkEndClick( const MouseEvent rMEvt );
        bool    impl_textHitTest( const Point& _rWindowPos );
    };

    //========================================================================
    //= OHyperlinkControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XHyperlinkControl, ControlWindow< HyperlinkInput > > OHyperlinkControl_Base;
    class OHyperlinkControl : public OHyperlinkControl_Base
    {
    private:
        ::cppu::OInterfaceContainerHelper   m_aActionListeners;

    public:
        OHyperlinkControl( Window* _pParent, WinBits _nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);

        // XHyperlinkControl
        virtual void SAL_CALL addActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& listener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeActionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& listener ) throw (::com::sun::star::uno::RuntimeException);

    protected:
        // XComponent
        virtual void SAL_CALL disposing();

    protected:
        DECL_LINK( OnHyperlinkClicked, void* );
    };

    //========================================================================
    //= CustomConvertibleNumericField
    //========================================================================
    class CustomConvertibleNumericField : public ControlWindow< MetricField >
    {
        typedef ControlWindow< MetricField > BaseClass;

    public:
        CustomConvertibleNumericField( Window* _pParent, WinBits _nStyle )
            :BaseClass( _pParent, _nStyle )
        {
        }

        long GetLastValue() const { return mnLastValue; }
    };

    //========================================================================
    //= ONumericControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XNumericControl, CustomConvertibleNumericField > ONumericControl_Base;
    class ONumericControl : public ONumericControl_Base
    {
    private:
        FieldUnit   m_eValueUnit;
        sal_Int16   m_nFieldToUNOValueFactor;

    public:
        ONumericControl( Window* pParent, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);

        // XNumericControl
        virtual ::sal_Int16 SAL_CALL getDecimalDigits() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setDecimalDigits( ::sal_Int16 _decimaldigits ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::beans::Optional< double > SAL_CALL getMinValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setMinValue( const ::com::sun::star::beans::Optional< double >& _minvalue ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::beans::Optional< double > SAL_CALL getMaxValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setMaxValue( const ::com::sun::star::beans::Optional< double >& _maxvalue ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int16 SAL_CALL getDisplayUnit() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setDisplayUnit( ::sal_Int16 _displayunit ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int16 SAL_CALL getValueUnit() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValueUnit( ::sal_Int16 _valueunit ) throw (::com::sun::star::uno::RuntimeException);

    private:
        /** converts an API value (<code>double</code>, as passed into <code>set[Max|Min|]Value) into
            a <code>long</code> value which can be passed to our NumericField.

            The conversion respects our decimal digits as well as our value factor (<member>m_nFieldToUNOValueFactor</member>).
        */
        long    impl_apiValueToFieldValue_nothrow( double _nApiValue ) const;

        /** converts a control value, as obtained from our Numeric field, into a value which can passed
            to outer callers via our UNO API.
        */
        double  impl_fieldValueToApiValue_nothrow( long _nFieldValue ) const;
    };

    //========================================================================
    //= OColorControl
    //========================================================================
    typedef CommonBehaviourControl  <   ::com::sun::star::inspection::XStringListControl
                                    ,   ListLikeControlWithModifyHandler< ColorListBox >
                                    >   OColorControl_Base;
    class OColorControl : public OColorControl_Base
    {
    private:
        ::std::set< ::rtl::OUString >   m_aNonColorEntries;

    public:
        OColorControl( Window* pParent, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);

        // XStringListControl
        virtual void SAL_CALL clearList(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL prependListEntry( const ::rtl::OUString& NewEntry ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL appendListEntry( const ::rtl::OUString& NewEntry ) throw (::com::sun::star::uno::RuntimeException);

    protected:
        virtual void modified();
    };

    //========================================================================
    //= OListboxControl
    //========================================================================
    typedef CommonBehaviourControl  <   ::com::sun::star::inspection::XStringListControl
                                    ,   ListLikeControlWithModifyHandler< ListBox >
                                    >   OListboxControl_Base;
    class OListboxControl : public OListboxControl_Base
    {
    public:
        OListboxControl( Window* pParent, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);

        // XStringListControl
        virtual void SAL_CALL clearList(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL prependListEntry( const ::rtl::OUString& NewEntry ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL appendListEntry( const ::rtl::OUString& NewEntry ) throw (::com::sun::star::uno::RuntimeException);

    protected:
        virtual void modified();
    };

    //========================================================================
    //= OComboboxControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XStringListControl, ControlWindow< ComboBox > > OComboboxControl_Base;
    class OComboboxControl : public OComboboxControl_Base
    {
    public:
        OComboboxControl( Window* pParent, WinBits nWinStyle );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);

        // XStringListControl
        virtual void SAL_CALL clearList(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL prependListEntry( const ::rtl::OUString& NewEntry ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL appendListEntry( const ::rtl::OUString& NewEntry ) throw (::com::sun::star::uno::RuntimeException);

    };

    //========================================================================
    //= DropDownEditControl
    //========================================================================
    enum MultiLineOperationMode
    {
        eStringList,
        eMultiLineText
    };
    //========================================================================
    //= DropDownEditControl
    //========================================================================
    class OMultilineFloatingEdit;
    typedef ControlWindow< Edit > DropDownEditControl_Base;
    /** an Edit field which can be used as ControlWindow, and has a drop-down button
    */
    class DropDownEditControl : public DropDownEditControl_Base
    {
    private:
        OMultilineFloatingEdit*             m_pFloatingEdit;
        MultiLineEdit*                      m_pImplEdit;
        PushButton*                         m_pDropdownButton;
        MultiLineOperationMode              m_nOperationMode;
        sal_Bool                            m_bDropdown : 1;

    public:
        DropDownEditControl( Window* _pParent, WinBits _nStyle );
        ~DropDownEditControl();

        void setOperationMode( MultiLineOperationMode _eMode ) { m_nOperationMode = _eMode; }
        MultiLineOperationMode getOperationMode() const { return m_nOperationMode; }

        void            SetTextValue( const ::rtl::OUString& _rText );
        ::rtl::OUString GetTextValue() const;

        void            SetStringListValue( const StlSyntaxSequence< ::rtl::OUString >& _rStrings );
        StlSyntaxSequence< ::rtl::OUString >
                        GetStringListValue() const;

        // ControlWindow overridables
        virtual void setControlHelper( ControlHelper& _rControlHelper );

    protected:
        // Window overridables
        virtual long    PreNotify( NotifyEvent& rNEvt );
        virtual void    Resize();

    protected:
        long            FindPos(long nSinglePos);

    private:
        DECL_LINK( ReturnHdl, OMultilineFloatingEdit* );
        DECL_LINK( DropDownHdl, PushButton* );

        sal_Bool ShowDropDown( sal_Bool bShow );
    };

    //========================================================================
    //= OMultilineEditControl
    //========================================================================
    typedef CommonBehaviourControl< ::com::sun::star::inspection::XPropertyControl, DropDownEditControl > OMultilineEditControl_Base;
    class OMultilineEditControl : public OMultilineEditControl_Base
    {
    public:
        OMultilineEditControl( Window* pParent, MultiLineOperationMode _eMode, WinBits nWinStyle  );

        // XPropertyControl
        virtual ::com::sun::star::uno::Any SAL_CALL getValue() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setValue( const ::com::sun::star::uno::Any& _value ) throw (::com::sun::star::beans::IllegalTypeException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Type SAL_CALL getValueType() throw (::com::sun::star::uno::RuntimeException);
    };

//............................................................................
} // namespace pcr
//............................................................................

#endif // _EXTENSIONS_PROPCTRLR_STANDARDCONTROL_HXX_

