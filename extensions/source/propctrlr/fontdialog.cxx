/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fontdialog.cxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: rt $ $Date: 2006-07-26 07:55:57 $
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

#ifndef _SFXSIDS_HRC
#include <sfx2/sfxsids.hrc>
#endif

#ifndef _EXTENSIONS_PROPCTRLR_FONTDIALOG_HXX_
#include "fontdialog.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_PROPRESID_HRC_
#include "formresid.hrc"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_MODULEPRC_HXX_
#include "modulepcr.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_FORMLOCALID_HRC_
#include "formlocalid.hrc"
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTDESCRIPTOR_HPP_
#include <com/sun/star/awt/FontDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTWEIGHT_HPP_
#include <com/sun/star/awt/FontWeight.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTUNDERLINE_HPP_
#include <com/sun/star/awt/FontUnderline.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTSTRIKEOUT_HPP_
#include <com/sun/star/awt/FontStrikeout.hpp>
#endif
#ifndef _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_
#include "formstrings.hxx"
#endif

#ifndef _EXTENSIONS_PROPCTRLR_FONTITEMIDS_HXX_
#include "fontitemids.hxx"
#endif
#define ITEMID_FONT             CFID_FONT
#define ITEMID_FONTHEIGHT       CFID_HEIGHT
#define ITEMID_WEIGHT           CFID_WEIGHT
#define ITEMID_POSTURE          CFID_POSTURE
#define ITEMID_LANGUAGE         CFID_LANGUAGE
#define ITEMID_UNDERLINE        CFID_UNDERLINE
#define ITEMID_CROSSEDOUT       CFID_STRIKEOUT
#define ITEMID_WORDLINEMODE     CFID_WORDLINEMODE
#define ITEMID_COLOR            CFID_CHARCOLOR
#define ITEMID_CHARRELIEF       CFID_RELIEF
#define ITEMID_EMPHASISMARK     CFID_EMPHASIS
#define ITEMID_CASEMAP          CFID_CASEMAP
#define ITEMID_CONTOUR          CFID_CONTOUR
#define ITEMID_SHADOWED         CFID_SHADOWED
#define ITEMID_FONTLIST         CFID_FONTLIST

#ifndef _SVX_CHARRELIEFITEM_HXX
#include <svx/charreliefitem.hxx>
#endif
#ifndef _SVX_EMPHITEM_HXX
#include <svx/emphitem.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX
#include <svx/fontitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX
#include <svx/fhgtitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX
#include <svx/postitem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX
#include <svx/wghtitem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX
#include <svx/udlnitem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX
#include <svx/crsditem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX
#include <svx/colritem.hxx>
#endif
#ifndef _SVX_FLSTITEM_HXX
#include <svx/flstitem.hxx>
#endif
#ifndef _SVX_LANGITEM_HXX
#include <svx/langitem.hxx>
#endif
#ifndef _SVX_WRLMITEM_HXX
#include <svx/wrlmitem.hxx>
#endif
#ifndef _SVX_CMAPITEM_HXX
#include <svx/cmapitem.hxx>
#endif
#ifndef _SVX_ITEM_HXX
#include <svx/cntritem.hxx>
#endif
#ifndef _SVX_SHDDITEM_HXX
#include <svx/shdditem.hxx>
#endif
#ifndef _SVX_FLSTITEM_HXX
#include <svx/flstitem.hxx>
#endif
#ifndef _CTRLTOOL_HXX
#include <svtools/ctrltool.hxx>
#endif
#include <tools/diagnose_ex.h>
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc> //CHINA001
#endif
#include <svx/svxdlg.hxx> //CHINA001
#include <svx/dialogs.hrc> //CHINA001
#include <svx/flagsdef.hxx>
//............................................................................
namespace pcr
{
//............................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;

    //========================================================================
    //= OFontPropertyExtractor
    //========================================================================
    class OFontPropertyExtractor
    {
    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                    m_xPropValueAccess;
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyState >
                    m_xPropStateAccess;

    public:
        OFontPropertyExtractor( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >&
            _rxProps );

    public:
        sal_Bool        getCheckFontProperty(const ::rtl::OUString& _rPropName, ::com::sun::star::uno::Any& _rValue);
        ::rtl::OUString getStringFontProperty(const ::rtl::OUString& _rPropName, const ::rtl::OUString& _rDefault);
        sal_Int16       getInt16FontProperty(const ::rtl::OUString& _rPropName, const sal_Int16 _nDefault);
        sal_Int32       getInt32FontProperty(const ::rtl::OUString& _rPropName, const sal_Int32 _nDefault);
        float           getFloatFontProperty(const ::rtl::OUString& _rPropName, const float _nDefault);

        void            invalidateItem(
                            const ::rtl::OUString& _rPropName,
                            sal_uInt16 _nItemId,
                            SfxItemSet& _rSet,
                            sal_Bool _bForceInvalidation = sal_False);
    };

    //------------------------------------------------------------------------
    OFontPropertyExtractor::OFontPropertyExtractor(const Reference< XPropertySet >& _rxProps)
        :m_xPropValueAccess(_rxProps)
        ,m_xPropStateAccess(_rxProps, UNO_QUERY)
    {
        OSL_ENSURE(m_xPropValueAccess.is(), "OFontPropertyExtractor::OFontPropertyExtractor: invalid property set!");
    }

    //------------------------------------------------------------------------
    sal_Bool OFontPropertyExtractor::getCheckFontProperty(const ::rtl::OUString& _rPropName, Any& _rValue)
    {
        _rValue = m_xPropValueAccess->getPropertyValue(_rPropName);
        if (m_xPropStateAccess.is())
            return PropertyState_DEFAULT_VALUE == m_xPropStateAccess->getPropertyState(_rPropName);

        return sal_False;
    }

    //------------------------------------------------------------------------
    ::rtl::OUString OFontPropertyExtractor::getStringFontProperty(const ::rtl::OUString& _rPropName, const ::rtl::OUString& _rDefault)
    {
        Any aValue;
        if (getCheckFontProperty(_rPropName, aValue))
            return _rDefault;

        return ::comphelper::getString(aValue);
    }

    //------------------------------------------------------------------------
    sal_Int16 OFontPropertyExtractor::getInt16FontProperty(const ::rtl::OUString& _rPropName, const sal_Int16 _nDefault)
    {
        Any aValue;
        if (getCheckFontProperty(_rPropName, aValue))
            return _nDefault;

        sal_Int32 nValue(_nDefault);
        ::cppu::enum2int(nValue, aValue);
        return (sal_Int16)nValue;
    }

    //------------------------------------------------------------------------
    sal_Int32 OFontPropertyExtractor::getInt32FontProperty(const ::rtl::OUString& _rPropName, const sal_Int32 _nDefault)
    {
        Any aValue;
        if (getCheckFontProperty(_rPropName, aValue))
            return _nDefault;

        sal_Int32 nValue(_nDefault);
        ::cppu::enum2int(nValue, aValue);
        return nValue;
    }

    //------------------------------------------------------------------------
    float OFontPropertyExtractor::getFloatFontProperty(const ::rtl::OUString& _rPropName, const float _nDefault)
    {
        Any aValue;
        if (getCheckFontProperty(_rPropName, aValue))
            return _nDefault;

        return ::comphelper::getFloat(aValue);
    }

    //------------------------------------------------------------------------
    void OFontPropertyExtractor::invalidateItem(const ::rtl::OUString& _rPropName, sal_uInt16 _nItemId, SfxItemSet& _rSet, sal_Bool _bForceInvalidation)
    {
        if  (   _bForceInvalidation
            ||  (   m_xPropStateAccess.is()
                && (PropertyState_AMBIGUOUS_VALUE == m_xPropStateAccess->getPropertyState(_rPropName))
                )
            )
            _rSet.InvalidateItem(_nItemId);
    }

    //========================================================================
    //= ControlCharacterDialog
    //========================================================================
    //------------------------------------------------------------------------
    ControlCharacterDialog::ControlCharacterDialog(Window* _pParent, const SfxItemSet& _rCoreSet)
        :SfxTabDialog(_pParent, PcrRes(RID_TABDLG_FONTDIALOG), &_rCoreSet)
    {
        FreeResource();
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        DBG_ASSERT(pFact, "CreateFactory fail!");
        AddTabPage(TABPAGE_CHARACTERS, pFact->GetTabPageCreatorFunc(RID_SVXPAGE_CHAR_NAME), 0 );
        AddTabPage(TABPAGE_CHARACTERS_EXT, pFact->GetTabPageCreatorFunc(RID_SVXPAGE_CHAR_EFFECTS), 0 );
    }

    //------------------------------------------------------------------------
    ControlCharacterDialog::~ControlCharacterDialog()
    {
    }

    //------------------------------------------------------------------------
    void ControlCharacterDialog::translatePropertiesToItems(const Reference< XPropertySet >& _rxModel, SfxItemSet* _pSet)
    {
        OSL_ENSURE(_pSet && _rxModel.is(), "ControlCharacterDialog::translatePropertiesToItems: invalid arguments!");
        if (!_pSet || !_rxModel.is())
            return;

        try
        {
            OFontPropertyExtractor aPropExtractor(_rxModel);

            // some items, which may be in default state, have to be filled with non-void information
            Font aDefaultVCLFont = Application::GetDefaultDevice()->GetSettings().GetStyleSettings().GetAppFont();
            ::com::sun::star::awt::FontDescriptor aDefaultFont = VCLUnoHelper::CreateFontDescriptor(aDefaultVCLFont);

            // get the current properties
            ::rtl::OUString aFontName       = aPropExtractor.getStringFontProperty(PROPERTY_FONT_NAME, aDefaultFont.Name);
            ::rtl::OUString aFontStyleName  = aPropExtractor.getStringFontProperty(PROPERTY_FONT_STYLENAME, aDefaultFont.StyleName);
            sal_Int16   nFontFamily         = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_FAMILY, aDefaultFont.Family);
            sal_Int16   nFontCharset        = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_CHARSET, aDefaultFont.CharSet);
            float   nFontHeight             = aPropExtractor.getFloatFontProperty(PROPERTY_FONT_HEIGHT, (float)aDefaultFont.Height);
            float   nFontWeight             = aPropExtractor.getFloatFontProperty(PROPERTY_FONT_WEIGHT, aDefaultFont.Weight);
            sal_Int16 nFontSlant            = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_SLANT, (sal_Int16)aDefaultFont.Slant);
            sal_Int16 nFontUnderline        = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_UNDERLINE, aDefaultFont.Underline);
            sal_Int16 nFontStrikeout        = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_STRIKEOUT, aDefaultFont.Strikeout);

            sal_Int32 nTextLineColor        = aPropExtractor.getInt32FontProperty(PROPERTY_TEXTLINECOLOR, COL_AUTO);
            sal_Int16 nFontRelief           = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_RELIEF, (sal_Int16)aDefaultVCLFont.GetRelief());
            sal_Int16 nFontEmphasisMark     = aPropExtractor.getInt16FontProperty(PROPERTY_FONT_EMPHASIS_MARK, aDefaultVCLFont.GetEmphasisMark());

            Any aValue;
            sal_Bool bWordLineMode          = aPropExtractor.getCheckFontProperty(PROPERTY_WORDLINEMODE, aValue) ? aDefaultFont.WordLineMode : ::cppu::any2bool(aValue);
            sal_Int32 nColor32              = aPropExtractor.getInt32FontProperty(PROPERTY_TEXTCOLOR, 0);

            // build SfxItems with the values
            SvxFontItem aFontItem((FontFamily)nFontFamily, aFontName, aFontStyleName, PITCH_DONTKNOW, nFontCharset, CFID_FONT);

            nFontHeight = (float)OutputDevice::LogicToLogic(Size(0, (sal_Int32)nFontHeight), MAP_POINT, MAP_TWIP).Height();
            SvxFontHeightItem aSvxFontHeightItem((sal_uInt32)nFontHeight,100,CFID_HEIGHT);

            FontWeight      eWeight=VCLUnoHelper::ConvertFontWeight(nFontWeight);
            FontItalic      eItalic=(FontItalic)nFontSlant;
            FontUnderline   eUnderline=(FontUnderline)nFontUnderline;
            FontStrikeout   eStrikeout=(FontStrikeout)nFontStrikeout;

            SvxWeightItem       aWeightItem(eWeight,CFID_WEIGHT);
            SvxPostureItem      aPostureItem(eItalic,CFID_POSTURE);

            SvxCrossedOutItem   aCrossedOutItem(eStrikeout,CFID_STRIKEOUT);
            SvxWordLineModeItem aWordLineModeItem(bWordLineMode, CFID_WORDLINEMODE);

            SvxUnderlineItem    aUnderlineItem(eUnderline,CFID_UNDERLINE);
            aUnderlineItem.SetColor(Color(nTextLineColor));

            SvxColorItem aSvxColorItem(nColor32,CFID_CHARCOLOR);
            SvxLanguageItem aLanguageItem(Application::GetSettings().GetUILanguage(), CFID_LANGUAGE);

            // the 2 CJK props
            SvxCharReliefItem aFontReliefItem((FontRelief)nFontRelief);
            SvxEmphasisMarkItem aEmphasisMarkitem((FontEmphasisMark)nFontEmphasisMark);

            _pSet->Put(aFontItem, CFID_FONT);
            _pSet->Put(aSvxFontHeightItem,CFID_HEIGHT);
            _pSet->Put(aWeightItem, CFID_WEIGHT);
            _pSet->Put(aPostureItem, CFID_POSTURE);
            _pSet->Put(aLanguageItem, CFID_LANGUAGE);
            _pSet->Put(aUnderlineItem,CFID_UNDERLINE);
            _pSet->Put(aCrossedOutItem,CFID_STRIKEOUT);
            _pSet->Put(aWordLineModeItem, CFID_WORDLINEMODE);
            _pSet->Put(aSvxColorItem, CFID_CHARCOLOR);
            _pSet->Put(aFontReliefItem, CFID_RELIEF);
            _pSet->Put(aEmphasisMarkitem, CFID_EMPHASIS);

            aPropExtractor.invalidateItem(PROPERTY_FONT_NAME, CFID_FONT, *_pSet);
            aPropExtractor.invalidateItem(PROPERTY_FONT_HEIGHT, CFID_HEIGHT, *_pSet);
            aPropExtractor.invalidateItem(PROPERTY_FONT_WEIGHT, CFID_WEIGHT, *_pSet, ::com::sun::star::awt::FontWeight::DONTKNOW == nFontWeight);
            aPropExtractor.invalidateItem(PROPERTY_FONT_SLANT, CFID_POSTURE, *_pSet, ::com::sun::star::awt::FontSlant_DONTKNOW == nFontSlant);
            aPropExtractor.invalidateItem(PROPERTY_FONT_UNDERLINE, CFID_UNDERLINE, *_pSet, ::com::sun::star::awt::FontUnderline::DONTKNOW == nFontUnderline);
            aPropExtractor.invalidateItem(PROPERTY_FONT_STRIKEOUT, CFID_STRIKEOUT, *_pSet, ::com::sun::star::awt::FontStrikeout::DONTKNOW == nFontStrikeout);
            aPropExtractor.invalidateItem(PROPERTY_WORDLINEMODE, CFID_WORDLINEMODE, *_pSet);
            aPropExtractor.invalidateItem(PROPERTY_TEXTCOLOR, CFID_CHARCOLOR, *_pSet);
            aPropExtractor.invalidateItem(PROPERTY_FONT_RELIEF, CFID_RELIEF, *_pSet);
            aPropExtractor.invalidateItem(PROPERTY_FONT_EMPHASIS_MARK, CFID_EMPHASIS, *_pSet);
        }
        catch (Exception&)
        {
            DBG_ERROR("ControlCharacterDialog::translatePropertiesToItems: caught an exception!")
        }

        _pSet->DisableItem(SID_ATTR_CHAR_CJK_FONT);
        _pSet->DisableItem(SID_ATTR_CHAR_CJK_FONTHEIGHT);
        _pSet->DisableItem(SID_ATTR_CHAR_CJK_LANGUAGE);
        _pSet->DisableItem(SID_ATTR_CHAR_CJK_POSTURE);
        _pSet->DisableItem(SID_ATTR_CHAR_CJK_WEIGHT);

        _pSet->DisableItem(SID_ATTR_CHAR_CASEMAP);
        _pSet->DisableItem(SID_ATTR_CHAR_CONTOUR);
        _pSet->DisableItem(SID_ATTR_CHAR_SHADOWED);

    }

    //------------------------------------------------------------------------
    namespace
    {
        void lcl_pushBackPropertyValue( Sequence< NamedValue >& _out_properties, const ::rtl::OUString& _name, const Any& _value )
        {
            _out_properties.realloc( _out_properties.getLength() + 1 );
            _out_properties[ _out_properties.getLength() - 1 ] = NamedValue( _name, _value );
        }
    }

    //------------------------------------------------------------------------
    void ControlCharacterDialog::translateItemsToProperties( const SfxItemSet& _rSet, Sequence< NamedValue >& _out_properties )
    {
        _out_properties.realloc( 0 );

        try
        {
            // --------------------------
            // font name
            SfxItemState eState = _rSet.GetItemState(CFID_FONT);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxFontItem& rFontItem =
                    static_cast<const SvxFontItem&>(_rSet.Get(CFID_FONT));

                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_NAME     , makeAny(::rtl::OUString(rFontItem.GetFamilyName())));
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_STYLENAME, makeAny(::rtl::OUString(rFontItem.GetStyleName())));
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_FAMILY   , makeAny((sal_Int16)rFontItem.GetFamily()));
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_CHARSET  , makeAny((sal_Int16)rFontItem.GetCharSet()));
            }

            // --------------------------
            // font height
            eState = _rSet.GetItemState(CFID_HEIGHT);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxFontHeightItem& rSvxFontHeightItem =
                    static_cast<const SvxFontHeightItem&>(_rSet.Get(CFID_HEIGHT));

                float nHeight = (float)OutputDevice::LogicToLogic(Size(0, rSvxFontHeightItem.GetHeight()), MAP_TWIP, MAP_POINT).Height();
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_HEIGHT,makeAny(nHeight));

            }

            // --------------------------
            // font weight
            eState = _rSet.GetItemState(CFID_WEIGHT);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxWeightItem& rWeightItem =
                    static_cast<const SvxWeightItem&>(_rSet.Get(CFID_WEIGHT));

                float nWeight = VCLUnoHelper::ConvertFontWeight( rWeightItem.GetWeight());
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_WEIGHT,makeAny(nWeight));
            }

            // --------------------------
            // font slant
            eState = _rSet.GetItemState(CFID_POSTURE);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxPostureItem& rPostureItem =
                    static_cast<const SvxPostureItem&>(_rSet.Get(CFID_POSTURE));

                ::com::sun::star::awt::FontSlant eSlant = (::com::sun::star::awt::FontSlant)rPostureItem.GetPosture();
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_SLANT, makeAny((sal_Int16)eSlant));
            }

            // --------------------------
            // font underline
            eState = _rSet.GetItemState(CFID_UNDERLINE);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxUnderlineItem& rUnderlineItem =
                    static_cast<const SvxUnderlineItem&>(_rSet.Get(CFID_UNDERLINE));

                sal_Int16 nUnderline = (sal_Int16)rUnderlineItem.GetUnderline();
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_UNDERLINE,makeAny(nUnderline));

                // the text line color is transported in this item, too
                sal_Int32 nColor = rUnderlineItem.GetColor().GetColor();

                Any aUnoColor;
                if (COL_AUTO != nColor)
                    aUnoColor <<= (sal_Int32)nColor;

                lcl_pushBackPropertyValue( _out_properties, PROPERTY_TEXTLINECOLOR, aUnoColor );
            }

            // --------------------------
            // font strikeout
            eState = _rSet.GetItemState(CFID_STRIKEOUT);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxCrossedOutItem& rCrossedOutItem =
                    static_cast<const SvxCrossedOutItem&>(_rSet.Get(CFID_STRIKEOUT));

                sal_Int16 nStrikeout = (sal_Int16)rCrossedOutItem.GetStrikeout();
                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_STRIKEOUT,makeAny(nStrikeout));
            }


            // --------------------------
            // font wordline mode
            eState = _rSet.GetItemState(CFID_WORDLINEMODE);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxWordLineModeItem& rWordLineModeItem =
                    static_cast<const SvxWordLineModeItem&>(_rSet.Get(CFID_WORDLINEMODE));

                lcl_pushBackPropertyValue( _out_properties, PROPERTY_WORDLINEMODE, ::cppu::bool2any(rWordLineModeItem.GetValue()));
            }


            // --------------------------
            // text color
            eState = _rSet.GetItemState(CFID_CHARCOLOR);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxColorItem& rColorItem =
                    static_cast<const SvxColorItem&>(_rSet.Get(CFID_CHARCOLOR));

                sal_Int32 nColor = rColorItem.GetValue().GetColor();

                Any aUnoColor;
                if (COL_AUTO != nColor)
                    aUnoColor <<= (sal_Int32)nColor;

                lcl_pushBackPropertyValue( _out_properties, PROPERTY_TEXTCOLOR, aUnoColor );
            }

            // --------------------------
            // font relief
            eState = _rSet.GetItemState(CFID_RELIEF);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxCharReliefItem& rReliefItem =
                    static_cast<const SvxCharReliefItem&>(_rSet.Get(CFID_RELIEF));

                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_RELIEF, makeAny((sal_Int16)rReliefItem.GetValue()) );
            }

            // --------------------------
            // font emphasis mark
            eState = _rSet.GetItemState(CFID_EMPHASIS);

            if ( eState == SFX_ITEM_SET )
            {
                const SvxEmphasisMarkItem& rEmphMarkItem =
                    static_cast<const SvxEmphasisMarkItem&>(_rSet.Get(CFID_EMPHASIS));

                lcl_pushBackPropertyValue( _out_properties, PROPERTY_FONT_EMPHASIS_MARK, makeAny((sal_Int16)rEmphMarkItem.GetEmphasisMark()) );
            }
        }
        catch (const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //------------------------------------------------------------------------
    void ControlCharacterDialog::translateItemsToProperties( const SfxItemSet& _rSet, const Reference< XPropertySet >& _rxModel)
    {
        OSL_ENSURE( _rxModel.is(), "ControlCharacterDialog::translateItemsToProperties: invalid arguments!" );
        if ( !_rxModel.is())
            return;

        Sequence< NamedValue > aPropertyValues;
        translateItemsToProperties( _rSet, aPropertyValues );
        try
        {
            const NamedValue* propertyValue = aPropertyValues.getConstArray();
            const NamedValue* propertyValueEnd = propertyValue + aPropertyValues.getLength();
            for ( ; propertyValue != propertyValueEnd; ++propertyValue )
                _rxModel->setPropertyValue( propertyValue->Name, propertyValue->Value );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //------------------------------------------------------------------------
    SfxItemSet* ControlCharacterDialog::createItemSet(SfxItemSet*& _rpSet, SfxItemPool*& _rpPool, SfxPoolItem**& _rppDefaults)
    {
        // just to be sure ....
        _rpSet = NULL;
        _rpPool = NULL;
        _rppDefaults = NULL;

        // create and initialize the defaults
        _rppDefaults = new SfxPoolItem*[CFID_LAST_ITEM_ID - CFID_FIRST_ITEM_ID + 1];

        Font aDefaultVCLFont = Application::GetDefaultDevice()->GetSettings().GetStyleSettings().GetAppFont();

        SfxPoolItem** pCounter = _rppDefaults;  // want to modify this without affecting the out param _rppDefaults
        *pCounter++ = new SvxFontItem(aDefaultVCLFont.GetFamily(), aDefaultVCLFont.GetName(), aDefaultVCLFont.GetStyleName(), aDefaultVCLFont.GetPitch(), aDefaultVCLFont.GetCharSet(), CFID_FONT);
        *pCounter++ = new SvxFontHeightItem(aDefaultVCLFont.GetHeight(), 100, CFID_HEIGHT);
        *pCounter++ = new SvxWeightItem(aDefaultVCLFont.GetWeight(), CFID_WEIGHT);
        *pCounter++ = new SvxPostureItem(aDefaultVCLFont.GetItalic(), CFID_POSTURE);
        *pCounter++ = new SvxLanguageItem(Application::GetSettings().GetUILanguage(), CFID_LANGUAGE);
        *pCounter++ = new SvxUnderlineItem(aDefaultVCLFont.GetUnderline(), CFID_UNDERLINE);
        *pCounter++ = new SvxCrossedOutItem(aDefaultVCLFont.GetStrikeout(), CFID_STRIKEOUT);
        *pCounter++ = new SvxWordLineModeItem(aDefaultVCLFont.IsWordLineMode(), CFID_WORDLINEMODE);
        *pCounter++ = new SvxColorItem(aDefaultVCLFont.GetColor(), CFID_CHARCOLOR);
        *pCounter++ = new SvxCharReliefItem(aDefaultVCLFont.GetRelief(), CFID_RELIEF);
        *pCounter++ = new SvxEmphasisMarkItem(aDefaultVCLFont.GetEmphasisMark(), CFID_EMPHASIS);

        *pCounter++ = new SvxFontItem(aDefaultVCLFont.GetFamily(), aDefaultVCLFont.GetName(), aDefaultVCLFont.GetStyleName(), aDefaultVCLFont.GetPitch(), aDefaultVCLFont.GetCharSet(), CFID_CJK_FONT);
        *pCounter++ = new SvxFontHeightItem(aDefaultVCLFont.GetHeight(), 100, CFID_CJK_HEIGHT);
        *pCounter++ = new SvxWeightItem(aDefaultVCLFont.GetWeight(), CFID_CJK_WEIGHT);
        *pCounter++ = new SvxPostureItem(aDefaultVCLFont.GetItalic(), CFID_CJK_POSTURE);
        *pCounter++ = new SvxLanguageItem(Application::GetSettings().GetUILanguage(), CFID_CJK_LANGUAGE);

        *pCounter++ = new SvxCaseMapItem(SVX_CASEMAP_NOT_MAPPED, CFID_CASEMAP);
        *pCounter++ = new SvxContourItem(sal_False, CFID_CONTOUR);
        *pCounter++ = new SvxShadowedItem(sal_False, CFID_SHADOWED);

        *pCounter++ = new SvxFontListItem (new FontList(Application::GetDefaultDevice()), CFID_FONTLIST);

        // create the pool
        static SfxItemInfo __READONLY_DATA aItemInfos[CFID_LAST_ITEM_ID - CFID_FIRST_ITEM_ID + 1] =
        {
            { SID_ATTR_CHAR_FONT,               0 },
            { SID_ATTR_CHAR_FONTHEIGHT,         0 },
            { SID_ATTR_CHAR_WEIGHT,             0 },
            { SID_ATTR_CHAR_POSTURE,            0 },
            { SID_ATTR_CHAR_LANGUAGE,           0 },
            { SID_ATTR_CHAR_UNDERLINE,          0 },
            { SID_ATTR_CHAR_STRIKEOUT,          0 },
            { SID_ATTR_CHAR_WORDLINEMODE,       0 },
            { SID_ATTR_CHAR_COLOR,              0 },
            { SID_ATTR_CHAR_RELIEF,             0 },
            { SID_ATTR_CHAR_EMPHASISMARK,       0 },
            { 0,                                0 },
            { 0,                                0 },
            { 0,                                0 },
            { 0,                                0 },
            { 0,                                0 },
            { 0,                                0 },
            { 0,                                0 },
            { 0,                                0 },
            { SID_ATTR_CHAR_FONTLIST,           0 }
        };

        _rpPool = new SfxItemPool(String::CreateFromAscii("PCRControlFontItemPool"), CFID_FIRST_ITEM_ID, CFID_LAST_ITEM_ID,
            aItemInfos, _rppDefaults);
        _rpPool->FreezeIdRanges();

        // and, finally, the set
        _rpSet = new SfxItemSet(*_rpPool, sal_True);

        return _rpSet;
    }

    //-------------------------------------------------------------------------
    void ControlCharacterDialog::destroyItemSet(SfxItemSet*& _rpSet, SfxItemPool*& _rpPool, SfxPoolItem**& _rppDefaults)
    {
        // from the pool, get and remember the font list (needs to be deleted)
        const SvxFontListItem& rFontListItem = static_cast<const SvxFontListItem&>(_rpPool->GetDefaultItem(CFID_FONTLIST));
        const FontList* pFontList = rFontListItem.GetFontList();

        // _first_ delete the set (refering the pool)
        if (_rpSet)
        {
            delete _rpSet;
            _rpSet = NULL;
        }

        // delete the pool
        if (_rpPool)
        {
            _rpPool->ReleaseDefaults(sal_True);
                // the "true" means delete the items, too
            delete _rpPool;
            _rpPool = NULL;
        }

        // reset the defaults ptr
        _rppDefaults = NULL;
            // no need to explicitly delete the defaults, this has been done by the ReleaseDefaults

        delete pFontList;
    }

    //------------------------------------------------------------------------
    void ControlCharacterDialog::PageCreated( sal_uInt16 _nId, SfxTabPage& _rPage )
    {
        SfxAllItemSet aSet(*(GetInputSetImpl()->GetPool()));
        switch ( _nId )
        {
            case TABPAGE_CHARACTERS:
//CHINA001              static_cast<SvxCharNamePage&>(_rPage).SetFontList(
//CHINA001              static_cast<const SvxFontListItem&>(GetInputSetImpl()->Get(CFID_FONTLIST))
//CHINA001              );
//CHINA001              static_cast<SvxCharNamePage&>(_rPage).DisableControls( DISABLE_HIDE_LANGUAGE );

//CHINA001              SvxFontListItem aFontListItem( static_cast<const SvxFontListItem&>(GetInputSetImpl()->Get(CFID_FONTLIST) ));
//CHINA001              aSet.Put ( SvxFontListItem( aFontListItem.GetFontList(), SID_ATTR_CHAR_FONTLIST));

//              aSet.Put (SfxUInt16Item(SID_CFID_FONTLIST,CFID_FONTLIST));
                aSet.Put (SvxFontListItem(static_cast<const SvxFontListItem&>(GetInputSetImpl()->Get(CFID_FONTLIST))));
                aSet.Put (SfxUInt16Item(SID_DISABLE_CTL,DISABLE_HIDE_LANGUAGE));
                _rPage.PageCreated(aSet);
                break;
        }
    }

//............................................................................
}   // namespace pcr
//............................................................................

