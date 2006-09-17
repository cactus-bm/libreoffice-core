/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TypeGeneration.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 17:17:28 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_comphelper.hxx"
#ifndef _COMPHELPER_TYPEGENERATION_HXX
#include <comphelper/TypeGeneration.hxx>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYCONTAINER_HPP_
#include <com/sun/star/script/XLibraryContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_LINESPACINGMODE_HPP_
#include <com/sun/star/style/LineSpacingMode.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHADJUST_HPP_
#include <com/sun/star/style/ParagraphAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_DROPCAPFORMAT_HPP_
#include <com/sun/star/style/DropCapFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_LINESPACING_HPP_
#include <com/sun/star/style/LineSpacing.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_TABSTOP_HPP_
#include <com/sun/star/style/TabStop.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_TABALIGN_HPP_
#include <com/sun/star/style/TabAlign.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_PAGESTYLELAYOUT_HPP_
#include <com/sun/star/style/PageStyleLayout.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_BREAKTYPE_HPP_
#include <com/sun/star/style/BreakType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XMODULE_HPP_
#include <com/sun/star/text/XModule.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTRANGEMOVER_HPP_
#include <com/sun/star/text/XTextRangeMover.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XFOOTNOTESSETTINGSSUPPLIER_HPP_
#include <com/sun/star/text/XFootnotesSettingsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XFOOTNOTE_HPP_
#include <com/sun/star/text/XFootnote.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XFOOTNOTESSUPPLIER_HPP_
#include <com/sun/star/text/XFootnotesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XENDNOTESSUPPLIER_HPP_
#include <com/sun/star/text/XEndnotesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XENDNOTESSETTINGSSUPPLIER_HPP_
#include <com/sun/star/text/XEndnotesSettingsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_FOOTNOTENUMBERING_HPP_
#include <com/sun/star/text/FootnoteNumbering.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTSECTIONSSUPPLIER_HPP_
#include <com/sun/star/text/XTextSectionsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTSECTION_HPP_
#include <com/sun/star/text/XTextSection.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_SECTIONFILELINK_HPP_
#include <com/sun/star/text/SectionFileLink.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_RELORIENTATION_HPP_
#include <com/sun/star/text/RelOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_VERTORIENTATION_HPP_
#include <com/sun/star/text/VertOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_HORIZONTALADJUST_HPP_
#include <com/sun/star/text/HorizontalAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_DOCUMENTSTATISTIC_HPP_
#include <com/sun/star/text/DocumentStatistic.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_HORIORIENTATION_HPP_
#include <com/sun/star/text/HoriOrientation.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_HORIORIENTATIONFORMAT_HPP_
#include <com/sun/star/text/HoriOrientationFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_NOTEPRINTMODE_HPP_
#include <com/sun/star/text/NotePrintMode.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_SIZETYPE_HPP_
#include <com/sun/star/text/SizeType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_VERTORIENTATIONFORMAT_HPP_
#include <com/sun/star/text/VertOrientationFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_WRAPTEXTMODE_HPP_
#include <com/sun/star/text/WrapTextMode.hpp>
#endif
//undef to prevent error (from sfx2/docfile.cxx)
#undef SEQUENCE
#ifndef _COM_SUN_STAR_TEXT_SETVARIABLETYPE_HPP_
#include <com/sun/star/text/SetVariableType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELDSSUPPLIER_HPP_
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_USERDATAPART_HPP_
#include <com/sun/star/text/UserDataPart.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_CHAPTERFORMAT_HPP_
#include <com/sun/star/text/ChapterFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELD_HPP_
#include <com/sun/star/text/XTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_PLACEHOLDERTYPE_HPP_
#include <com/sun/star/text/PlaceholderType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEMPLATEDISPLAYFORMAT_HPP_
#include <com/sun/star/text/TemplateDisplayFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_USERFIELDFORMAT_HPP_
#include <com/sun/star/text/UserFieldFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_PAGENUMBERTYPE_HPP_
#include <com/sun/star/text/PageNumberType.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_REFERENCEFIELDPART_HPP_
#include <com/sun/star/text/ReferenceFieldPart.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_FILENAMEDISPLAYFORMAT_HPP_
#include <com/sun/star/text/FilenameDisplayFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XDEPENDENTTEXTFIELD_HPP_
#include <com/sun/star/text/XDependentTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_GRAPHICCROP_HPP_
#include <com/sun/star/text/GraphicCrop.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTGRAPHICOBJECTSSUPPLIER_HPP_
#include <com/sun/star/text/XTextGraphicObjectsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTTABLECURSOR_HPP_
#include <com/sun/star/text/XTextTableCursor.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTTABLESSUPPLIER_HPP_
#include <com/sun/star/text/XTextTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TABLECOLUMNSEPARATOR_HPP_
#include <com/sun/star/text/TableColumnSeparator.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTTABLE_HPP_
#include <com/sun/star/text/XTextTable.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XDOCUMENTINDEXMARK_HPP_
#include <com/sun/star/text/XDocumentIndexMark.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XDOCUMENTINDEXESSUPPLIER_HPP_
#include <com/sun/star/text/XDocumentIndexesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XDOCUMENTINDEX_HPP_
#include <com/sun/star/text/XDocumentIndex.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTCOLUMNS_HPP_
#include <com/sun/star/text/XTextColumns.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEXTCOLUMNSEQUENCE_HPP_
#include <com/sun/star/text/TextColumnSequence.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFRAMESSUPPLIER_HPP_
#include <com/sun/star/text/XTextFramesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFRAME_HPP_
#include <com/sun/star/text/XTextFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_
#include <com/sun/star/beans/PropertyValues.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATE_HPP_
#include <com/sun/star/beans/PropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XMULTIPROPERTYSET_HPP_
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XFASTPROPERTYSET_HPP_
#include <com/sun/star/beans/XFastPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XVETOABLECHANGELISTENER_HPP_
#include <com/sun/star/beans/XVetoableChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATECHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyStateChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PropertyAttribute_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTIESCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertiesChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCHANGELISTENER_HPP_
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYACCESS_HPP_
#include <com/sun/star/beans/XPropertyAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYCONTAINER_HPP_
#include <com/sun/star/beans/XPropertyContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_NAMEDVALUE_HPP_
#include <com/sun/star/beans/NamedValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYSTATECHANGEEVENT_HPP_
#include <com/sun/star/beans/PropertyStateChangeEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYCHANGEEVENT_HPP_
#include <com/sun/star/beans/PropertyChangeEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_TEXTCONTENTANCHORTYPE_HPP_
#include <com/sun/star/text/TextContentAnchorType.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTSLANT_HPP_
#include <com/sun/star/awt/FontSlant.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XBITMAP_HPP_
#include <com/sun/star/awt/XBitmap.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_GRAPHICLOCATION_HPP_
#include <com/sun/star/style/GraphicLocation.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_VERTICALALIGNMENT_HPP_
#include <com/sun/star/style/VerticalAlignment.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_SHADOWFORMAT_HPP_
#include <com/sun/star/table/ShadowFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_BORDERLINE_HPP_
#include <com/sun/star/table/BorderLine.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_TABLEBORDER_HPP_
#include <com/sun/star/table/TableBorder.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POINTSEQUENCESEQUENCE_HPP_
#include <com/sun/star/drawing/PointSequenceSequence.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XFORBIDDENCHARACTERS_HPP_
#include <com/sun/star/i18n/XForbiddenCharacters.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_COLORMODE_HPP_
#include <com/sun/star/drawing/ColorMode.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATE_HPP_
#include <com/sun/star/util/Date.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
// --> OD 2004-08-09 #i28749#
#ifndef _COM_SUN_STAR_DRAWING_HOMOGENMATRIX3_HPP_
#include <com/sun/star/drawing/HomogenMatrix3.hpp>
#endif
// <--

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::i18n;
using namespace ::comphelper;

namespace comphelper
{
    void GenerateCppuType (
        CppuTypes eType, const com::sun::star::uno::Type*& pType )
    {
        switch( eType )
        {
            case CPPUTYPE_BOOLEAN:      pType = &::getBooleanCppuType();    break;
            case CPPUTYPE_INT8:         pType = &::getCppuType( (sal_Int8*)0 ); break;
            case CPPUTYPE_INT16:        pType = &::getCppuType( (sal_Int16*)0 );    break;
            case CPPUTYPE_INT32:        pType = &::getCppuType( (sal_Int32*)0 );    break;

            case CPPUTYPE_DOUBLE:       pType = &::getCppuType( (double*)0 ); break;
            case CPPUTYPE_FLOAT:        pType = &::getCppuType( (float*)0 );    break;
            case CPPUTYPE_OUSTRING:     pType = &::getCppuType( (OUString*)0 ); break;
            case CPPUTYPE_FONTSLANT:    pType = &::getCppuType( (FontSlant*)0 );    break;
            case CPPUTYPE_LOCALE:       pType = &::getCppuType( (Locale*)0 );   break;
            case CPPUTYPE_PROPERTYVALUE:pType = &::getCppuType( (Sequence<PropertyValue>*)0 );  break;
            case CPPUTYPE_PROPERTYVALUES:   pType = &::getCppuType( (Sequence<PropertyValues>*)0 ); break;
            case CPPUTYPE_BORDERLINE:   pType = &::getCppuType( (table::BorderLine*)0 );    break;
            case CPPUTYPE_BREAK:        pType = &::getCppuType( (style::BreakType*)0 ); break;
            case CPPUTYPE_GRAPHICLOC:   pType = &::getCppuType( (style::GraphicLocation*)0 );   break;
            case CPPUTYPE_DROPCAPFMT:   pType = &::getCppuType( (style::DropCapFormat*)0 ); break;
            case CPPUTYPE_LINESPACE:    pType = &::getCppuType( (style::LineSpacing*)0 );   break;

            case CPPUTYPE_AWTSIZE:      pType = &::getCppuType( (awt::Size*)0 );    break;
            case CPPUTYPE_SHADOWFMT:    pType = &::getCppuType( (table::ShadowFormat*)0 );  break;
            case CPPUTYPE_TBLCOLSEP:    pType = &::getCppuType( (Sequence<text::TableColumnSeparator>*)0 ); break;
            case CPPUTYPE_PNTSEQSEQ:    pType = &::getCppuType( (PointSequenceSequence*)0 );    break;
            case CPPUTYPE_DOCIDXMRK:    pType = &::getCppuType( (Sequence< Reference< XDocumentIndexMark > >*)0 );  break;
            case CPPUTYPE_SEQINT8:      pType = &::getCppuType( (Sequence<sal_Int8>*)0 );   break;
            case CPPUTYPE_SEQTABSTOP:   pType = &::getCppuType( (Sequence<style::TabStop>*)0 ); break;
            case CPPUTYPE_SEQANCHORTYPE:pType = &::getCppuType( (Sequence<text::TextContentAnchorType>*)0 ); break;
            case CPPUTYPE_SEQDEPTXTFLD: pType = &::getCppuType( (Sequence<Reference<XDependentTextField> >*)0); break;
            case CPPUTYPE_TXTCNTANCHOR: pType = &::getCppuType( (text::TextContentAnchorType*)0 );  break;
            case CPPUTYPE_WRAPTXTMODE:  pType = &::getCppuType( (text::WrapTextMode*)0 );   break;

            case CPPUTYPE_COLORMODE:    pType = &::getCppuType( (drawing::ColorMode*)0 ); break;
            case CPPUTYPE_PAGESTYLELAY: pType = &::getCppuType( (style::PageStyleLayout*)0 ); break;
            case CPPUTYPE_VERTALIGN:    pType = &::getCppuType( (style::VerticalAlignment*)0 ); break;
            case CPPUTYPE_TABLEBORDER:  pType = &::getCppuType( (table::TableBorder*)0 ); break;
            case CPPUTYPE_GRFCROP:      pType = &::getCppuType( (text::GraphicCrop*)0 ); break;
            case CPPUTYPE_SECTFILELNK:  pType = &::getCppuType( (text::SectionFileLink*)0 ); break;
            case CPPUTYPE_PAGENUMTYPE:  pType = &::getCppuType( (text::PageNumberType*)0); break;
            case CPPUTYPE_DATETIME:     pType = &::getCppuType( (util::DateTime*)0 ); break;
            case CPPUTYPE_DATE:         pType = &::getCppuType( (util::Date*)0 ); break;

            case CPPUTYPE_REFINTERFACE: pType = &::getCppuType( (Reference<XInterface>*)0 );    break;
            case CPPUTYPE_REFIDXREPL:   pType = &::getCppuType( (Reference<container::XIndexReplace>*)0 );  break;
            case CPPUTYPE_REFNAMECNT:   pType = &::getCppuType( (Reference<container::XNameContainer>*)0 ); break;
            case CPPUTYPE_REFTEXTFRAME: pType = &::getCppuType( (Reference<text::XTextFrame>*)0 );  break;
            case CPPUTYPE_REFTEXTSECTION:   pType = &::getCppuType( (Reference<text::XTextSection>*)0 );    break;
            case CPPUTYPE_REFFOOTNOTE:  pType = &::getCppuType( (Reference<text::XFootnote>*)0 );   break;
            case CPPUTYPE_REFTEXT:      pType = &::getCppuType( (Reference<text::XText>*)0 );   break;
            case CPPUTYPE_REFTEXTCOL:   pType = &::getCppuType( (Reference<text::XTextColumns>*)0 );    break;
            case CPPUTYPE_REFFORBCHARS: pType = &::getCppuType( (Reference<XForbiddenCharacters>*)0 ); break;
            case CPPUTYPE_REFIDXCNTNR:  pType = &::getCppuType( (Reference<XIndexContainer>*)0 ); break;
            case CPPUTYPE_REFTEXTCNTNT: pType = &::getCppuType( (Reference<XTextContent>*)0 ); break;
            case CPPUTYPE_REFBITMAP:    pType = &::getCppuType( (Reference<awt::XBitmap>*)0 ); break;
            case CPPUTYPE_REFNMREPLACE: pType = &::getCppuType( (Reference<container::XNameReplace>*)0 ); break;
            case CPPUTYPE_REFCELL:      pType = &::getCppuType( (Reference<table::XCell>*)0 ); break;
            case CPPUTYPE_REFDOCINDEX:  pType = &::getCppuType( (Reference<text::XDocumentIndex>*)0 ); break;
            case CPPUTYPE_REFDOCIDXMRK: pType = &::getCppuType( (Reference<text::XDocumentIndexMark>*)0 ); break;
            case CPPUTYPE_REFTXTFIELD:  pType = &::getCppuType( (Reference<text::XTextField>*)0 ); break;
            case CPPUTYPE_REFTXTRANGE:  pType = &::getCppuType( (Reference<text::XTextRange>*)0 ); break;
            case CPPUTYPE_REFTXTTABLE:  pType = &::getCppuType( (Reference<text::XTextTable>*)0 ); break;
            case CPPUTYPE_AWTPOINT:     pType = &::getCppuType( (awt::Point*)0 );    break;
            case CPPUTYPE_REFLIBCONTAINER:  pType = &::getCppuType( (Reference< script::XLibraryContainer >*)0);    break;
            case CPPUTYPE_OUSTRINGS:    pType = &::getCppuType( (Sequence< ::rtl::OUString >*)0);    break;
            case CPPUTYPE_SEQANY:           pType = &::getCppuType( (Sequence< uno::Any >*)0);    break;
            case CPPUTYPE_REFRESULTSET:     pType = &::getCppuType( (Reference< sdbc::XResultSet >*)0);    break;
            case CPPUTYPE_REFCONNECTION:    pType = &::getCppuType( (Reference< sdbc::XConnection >*)0);    break;
            case CPPUTYPE_REFMODEL:         pType = &::getCppuType( (Reference< frame::XModel >*)0);    break;
            case CPPUTYPE_REFCOMPONENT:     pType = &::getCppuType( (Reference< lang::XComponent >*)0 ); break;
            // --> OD 2004-08-09 #i28749#
            case CPPUTYPE_TRANSFORMATIONINHORIL2R:
            {
                pType = &::getCppuType( (drawing::HomogenMatrix3*)0 );
            }
            break;
            // <--
            case CPPUTYPE_SEQNAMEDVALUE:    pType = &::getCppuType( (Sequence<beans::NamedValue>*)0 ); break;

            default:
                OSL_ASSERT( "Unknown CPPU type" );
        }
    }
}
