/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: standardcontrol.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 20:28:33 $
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

#include <memory>

class PushButton;
class MultiLineEdit;
//............................................................................
namespace pcr
{
//............................................................................

    class StringRepresentation;

    //========================================================================
    //= OTimeControl
    //========================================================================
    class OTimeControl : public OCommonBehaviourControl, TimeField
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OTimeControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP);

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;
    };

    //========================================================================
    //= ODateControl
    //========================================================================
    class ODateControl : public OCommonBehaviourControl, CalendarField
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            ODateControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP);

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;
    };

    //========================================================================
    //= OEditControl
    //========================================================================
    class OEditControl : public OCommonBehaviourControl, Edit
    {
    protected:
        sal_Bool                            m_bIsPassword : 1;

    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OEditControl(Window* _pParent, sal_Bool _bPassWord, WinBits nWinStyle = WB_TABSTOP);

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

    protected:
            virtual void modified(Window* _pSource);
    };

    //========================================================================
    //= OCurrencyControl
    //========================================================================
    class OCurrencyControl : public OCommonBehaviourControl, LongCurrencyField
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OCurrencyControl( Window* pParent,sal_uInt16 nDigits,
                                                        WinBits nWinStyle = WB_TABSTOP);

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;
    };

    //========================================================================
    //= ODateTimeControl
    //========================================================================
    class ODateTimeControl : public OCommonBehaviourControl, FormattedField
    {
    private:
        ::std::auto_ptr< StringRepresentation > m_pConverter;

    public:
                                            ODateTimeControl( Window* pParent,sal_uInt16 nDigits, WinBits nWinStyle = WB_TABSTOP );

            virtual void                    SetProperty( const ::rtl::OUString& _rString,sal_Bool _bIsUnknown = sal_False );
            virtual ::rtl::OUString         GetProperty() const;
    };

    //========================================================================
    //= ONumericControl
    //========================================================================
    class ONumericControl : public OCommonBehaviourControl, MetricField
    {
    private:
        FieldUnit   m_eValueUnit;

    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
            ONumericControl( Window* pParent, sal_uInt16 nDigits, WinBits nWinStyle = WB_TABSTOP );

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

            void                            SetMin(sal_Int32 _nMin) { MetricField::SetMin(_nMin); }
            void                            SetMax(sal_Int32 _nMax) { MetricField::SetMax(_nMax); }
            void                            SetFieldUnit( FieldUnit _eUnit ) { MetricField::SetUnit( _eUnit ); }
            void                            SetValueUnit( FieldUnit _eUnit ) { m_eValueUnit = _eUnit; }

    protected:
        MetricField::SetUnit;
        MetricField::SetCustomUnitText;
        MetricField::GetCurUnitText;
        MetricField::SetValue;
    protected:
    inline long  GetLastValue() const { return mnLastValue; }
    };

    //========================================================================
    //= OColorControl
    //========================================================================
    class OColorControl : public OCommonBehaviourControl, ColorListBox
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OColorControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP |WB_DROPDOWN|WB_AUTOSIZE);

            // property value handling
            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

            // list handling
            virtual sal_Bool                HasList();
            virtual void                    ClearList();
            virtual void                    InsertCtrEntry( const ::rtl::OUString& rString, sal_uInt16 nPos = LISTBOX_APPEND );

            virtual void                    SetCtrSize(const Size& rSize);

    protected:
            virtual void modified(Window* _pSource);
    };

    //========================================================================
    //= OListboxControl
    //========================================================================
    class OListboxControl : public OCommonBehaviourControl, ListBox
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OListboxControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP|WB_SORT|WB_DROPDOWN|WB_AUTOSIZE);

            // property value handling
            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

            // list handling
            virtual sal_Bool                HasList();
            virtual void                    ClearList();
            virtual void                    InsertCtrEntry( const ::rtl::OUString& rString, sal_uInt16 nPos = LISTBOX_APPEND );

            virtual void                    SetCtrSize(const Size& rSize);

    protected:
            virtual void modified(Window* _pSource);
    };

    //========================================================================
    //= OComboboxControl
    //========================================================================
    class OComboboxControl : public OCommonBehaviourControl, ComboBox
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );


    public:
                                            OComboboxControl( Window* pParent,
                                                    WinBits nWinStyle = WB_TABSTOP | WB_SORT | WB_DROPDOWN | WB_AUTOSIZE);

            // property value handling
            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

            // list handling
            virtual sal_Bool                HasList();
            virtual void                    ClearList();
            virtual void                    InsertCtrEntry( const ::rtl::OUString& rString, sal_uInt16 nPos = LISTBOX_APPEND );

            virtual void                    SetCtrSize(const Size& rSize);

    };

    //========================================================================
    //= OMultilineEditControl
    //========================================================================
    class OMultilineFloatingEdit;
    class OMultilineEditControl : public OCommonBehaviourControl, Edit
    {
            OMultilineFloatingEdit*             m_pFloatingEdit;
            MultiLineEdit*                      m_pImplEdit;
            PushButton*                         m_pDropdownButton;
            sal_Bool                            m_bDropdown :1;
            sal_Bool                            m_bEdit     :1;

            DECL_LINK(ReturnHdl,OMultilineFloatingEdit*);
            DECL_LINK(DropDownHdl, PushButton*);

    protected:

            virtual long                    PreNotify( NotifyEvent& rNEvt );
            void                            Resize();

            long                            FindPos(long nSinglePos);

    public:
                                            OMultilineEditControl( Window* pParent,sal_Bool bEdit,
                                                        WinBits nWinStyle = WB_TABSTOP | WB_DROPDOWN);

                                            ~OMultilineEditControl();

            virtual sal_Bool                ShowDropDown( sal_Bool bShow );

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

    protected:
            virtual void modified(Window* _pSource);
    };

//............................................................................
} // namespace pcr
//............................................................................

#endif // _EXTENSIONS_PROPCTRLR_STANDARDCONTROL_HXX_

