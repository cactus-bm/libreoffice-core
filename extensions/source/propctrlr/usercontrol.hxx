/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: usercontrol.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 20:32:19 $
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

#ifndef _EXTENSIONS_PROPCTRLR_USERCONTROL_HXX_
#define _EXTENSIONS_PROPCTRLR_USERCONTROL_HXX_

#ifndef _EXTENSIONS_PROPCTRLR_COMMONCONTROL_HXX_
#include "commoncontrol.hxx"
#endif
#define _ZFORLIST_DECLARE_TABLE
#ifndef _FMTFIELD_HXX_
#include <svtools/fmtfield.hxx>
#endif
#ifndef SVTOOLS_FILEURLBOX_HXX
#include <svtools/fileurlbox.hxx>
#endif
#ifndef _EXTENSIONS_PROPCTRLR_STANDARDCONTROL_HXX_
#include "standardcontrol.hxx"
#endif

class SvNumberFormatsSupplierObj;

//............................................................................
namespace pcr
{
//............................................................................

    //========================================================================
    //= OFormatDescriptionControl
    //========================================================================
    class OFormatDescriptionControl : public OCommonBehaviourControl, FormattedField
    {
    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OFormatDescriptionControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP);

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

            virtual void                    SetFormatSupplier(const SvNumberFormatsSupplierObj* pSupplier);
    };

    //========================================================================
    //= FormatDescription
    //========================================================================
    struct FormatDescription
    {
        SvNumberFormatsSupplierObj*     pSupplier;
        sal_Int32                       nKey;
    };

    //========================================================================
    //= OFormattedNumericControl
    //========================================================================
    class OFormattedNumericControl : public OCommonBehaviourControl, FormattedField
    {
            sal_Int32                       m_nLastDecimalDigits;

    protected:
            virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
                                            OFormattedNumericControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP);
                                            ~OFormattedNumericControl();

            virtual void                    SetProperty(const ::rtl::OUString &rString,sal_Bool bIsUnknown=sal_False);
            virtual ::rtl::OUString         GetProperty()const;

            virtual void                    SetFormatDescription(const FormatDescription& rDesc);

            // make some FormattedField methods available
            virtual void                    SetDecimalDigits(sal_uInt16 nPrecision) { FormattedField::SetDecimalDigits(nPrecision); m_nLastDecimalDigits = nPrecision; }
            virtual void                    SetDefaultValue(double dDef) { FormattedField::SetDefaultValue(dDef); }
            virtual void                    EnableEmptyField(sal_Bool bEnable) { FormattedField::EnableEmptyField(bEnable); }
            virtual void                    SetThousandsSep(sal_Bool bEnable) { FormattedField::SetThousandsSep(bEnable); }
    };

    //========================================================================
    //= OFileUrlControl
    //========================================================================
    typedef ::svt::FileURLBox OFileUrlControl_Base;
    class OFileUrlControl : public OCommonBehaviourControl, OFileUrlControl_Base
    {
    protected:
        virtual long                    PreNotify( NotifyEvent& rNEvt );

    public:
        OFileUrlControl( Window* pParent, WinBits nWinStyle = WB_TABSTOP);
        ~OFileUrlControl();

        virtual void                    SetProperty( const ::rtl::OUString& _rString, sal_Bool bIsUnknown = sal_False );
        virtual ::rtl::OUString         GetProperty() const;

        void                            SetBaseURL( const String& _rURL ) { OFileUrlControl_Base::SetBaseURL( _rURL ); }
    };

    //========================================================================
    //= TimeDurationInput
    //========================================================================
    class TimeDurationInput : public ONumericControl
    {
    public:
        TimeDurationInput( ::Window* pParent, WinBits nWinStyle = WB_TABSTOP);
        ~TimeDurationInput();

    protected:
        virtual void CustomConvert();
    };

//............................................................................
} // namespace pcr
//............................................................................

#endif // _EXTENSIONS_PROPCTRLR_USERCONTROL_HXX_

