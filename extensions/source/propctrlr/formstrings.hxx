/*************************************************************************
 *
 *  $RCSfile: formstrings.hxx,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: fs $ $Date: 2001-11-08 11:37:43 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_
#define _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_

#ifndef _EXTENSIONS_FORMSCTRLR_STRINGDEFINE_HXX_
#include "stringdefine.hxx"
#endif

//............................................................................
namespace pcr
{
//............................................................................

    // properties
    PCR_CONSTASCII_STRING( PROPERTY_CLASSID,                "ClassId" );
    PCR_CONSTASCII_STRING( PROPERTY_CONTROLLABEL,           "LabelControl");
    PCR_CONSTASCII_STRING( PROPERTY_LABEL,                  "Label");
    PCR_CONSTASCII_STRING( PROPERTY_TABINDEX,               "TabIndex");
    PCR_CONSTASCII_STRING( PROPERTY_TAG,                    "Tag");
    PCR_CONSTASCII_STRING( PROPERTY_NAME,                   "Name");
    PCR_CONSTASCII_STRING( PROPERTY_VALUE,                  "Value");
    PCR_CONSTASCII_STRING( PROPERTY_TEXT,                   "Text");
    PCR_CONSTASCII_STRING( PROPERTY_NAVIGATION,             "NavigationBarMode");
    PCR_CONSTASCII_STRING( PROPERTY_CYCLE,                  "Cycle");
    PCR_CONSTASCII_STRING( PROPERTY_CONTROLSOURCE,          "DataField");
    PCR_CONSTASCII_STRING( PROPERTY_ENABLED,                "Enabled");
    PCR_CONSTASCII_STRING( PROPERTY_READONLY,               "ReadOnly");
    PCR_CONSTASCII_STRING( PROPERTY_ISREADONLY,             "IsReadOnly");
    PCR_CONSTASCII_STRING( PROPERTY_FILTER_CRITERIA,        "Filter");
    PCR_CONSTASCII_STRING( PROPERTY_WIDTH,                  "Width");
    PCR_CONSTASCII_STRING( PROPERTY_MULTILINE,              "MultiLine");
    PCR_CONSTASCII_STRING( PROPERTY_TARGET_URL,             "TargetURL");
    PCR_CONSTASCII_STRING( PROPERTY_TARGET_FRAME,           "TargetFrame");
    PCR_CONSTASCII_STRING( PROPERTY_MAXTEXTLEN,             "MaxTextLen");
    PCR_CONSTASCII_STRING( PROPERTY_EDITMASK,               "EditMask");
    PCR_CONSTASCII_STRING( PROPERTY_SPIN,                   "Spin");
    PCR_CONSTASCII_STRING( PROPERTY_TRISTATE,               "TriState");
    PCR_CONSTASCII_STRING( PROPERTY_HIDDEN_VALUE,           "HiddenValue");
    PCR_CONSTASCII_STRING( PROPERTY_BUTTONTYPE,             "ButtonType");
    PCR_CONSTASCII_STRING( PROPERTY_STRINGITEMLIST,         "StringItemList");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULT_TEXT,           "DefaultText");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULTCHECKED,         "DefaultState");
    PCR_CONSTASCII_STRING( PROPERTY_FORMATKEY,              "FormatKey");
    PCR_CONSTASCII_STRING( PROPERTY_FORMATSSUPPLIER,        "FormatsSupplier");
    PCR_CONSTASCII_STRING( PROPERTY_SUBMIT_ACTION,          "SubmitAction");
    PCR_CONSTASCII_STRING( PROPERTY_SUBMIT_TARGET,          "SubmitTarget");
    PCR_CONSTASCII_STRING( PROPERTY_SUBMIT_METHOD,          "SubmitMethod");
    PCR_CONSTASCII_STRING( PROPERTY_SUBMIT_ENCODING,        "SubmitEncoding");
    PCR_CONSTASCII_STRING( PROPERTY_IMAGE_URL,              "ImageURL");
    PCR_CONSTASCII_STRING( PROPERTY_EMPTY_IS_NULL,          "ConvertEmptyToNull");
    PCR_CONSTASCII_STRING( PROPERTY_LISTSOURCETYPE,         "ListSourceType");
    PCR_CONSTASCII_STRING( PROPERTY_LISTSOURCE,             "ListSource");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULT_SELECT_SEQ,     "DefaultSelection");
    PCR_CONSTASCII_STRING( PROPERTY_MULTISELECTION,         "MultiSelection");
    PCR_CONSTASCII_STRING( PROPERTY_ALIGN,                  "Align");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULT_DATE,           "DefaultDate");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULT_TIME,           "DefaultTime");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULT_VALUE,          "DefaultValue");
    PCR_CONSTASCII_STRING( PROPERTY_DECIMAL_ACCURACY,       "DecimalAccuracy");
    PCR_CONSTASCII_STRING( PROPERTY_REFVALUE,               "RefValue");
    PCR_CONSTASCII_STRING( PROPERTY_VALUEMIN,               "ValueMin");
    PCR_CONSTASCII_STRING( PROPERTY_VALUEMAX,               "ValueMax");
    PCR_CONSTASCII_STRING( PROPERTY_STRICTFORMAT,           "StrictFormat");
    PCR_CONSTASCII_STRING( PROPERTY_ALLOWADDITIONS,         "AllowInserts");
    PCR_CONSTASCII_STRING( PROPERTY_ALLOWEDITS,             "AllowUpdates");
    PCR_CONSTASCII_STRING( PROPERTY_ALLOWDELETIONS,         "AllowDeletes");
    PCR_CONSTASCII_STRING( PROPERTY_MASTERFIELDS,           "MasterFields");
    PCR_CONSTASCII_STRING( PROPERTY_LITERALMASK,            "LiteralMask");
    PCR_CONSTASCII_STRING( PROPERTY_VALUESTEP,              "ValueStep");
    PCR_CONSTASCII_STRING( PROPERTY_SHOWTHOUSANDSEP,        "ShowThousandsSeparator");
    PCR_CONSTASCII_STRING( PROPERTY_CURRENCYSYMBOL,         "CurrencySymbol");
    PCR_CONSTASCII_STRING( PROPERTY_DATEFORMAT,             "DateFormat");
    PCR_CONSTASCII_STRING( PROPERTY_DATEMIN,                "DateMin");
    PCR_CONSTASCII_STRING( PROPERTY_DATEMAX,                "DateMax");
    PCR_CONSTASCII_STRING( PROPERTY_TIMEFORMAT,             "TimeFormat");
    PCR_CONSTASCII_STRING( PROPERTY_TIMEMIN,                "TimeMin");
    PCR_CONSTASCII_STRING( PROPERTY_TIMEMAX,                "TimeMax");
    PCR_CONSTASCII_STRING( PROPERTY_LINECOUNT,              "LineCount");
    PCR_CONSTASCII_STRING( PROPERTY_BOUNDCOLUMN,            "BoundColumn");
    PCR_CONSTASCII_STRING( PROPERTY_BACKGROUNDCOLOR,        "BackgroundColor");
    PCR_CONSTASCII_STRING( PROPERTY_FILLCOLOR,              "FillColor");
    PCR_CONSTASCII_STRING( PROPERTY_TEXTCOLOR,              "TextColor");
    PCR_CONSTASCII_STRING( PROPERTY_LINECOLOR,              "LineColor");
    PCR_CONSTASCII_STRING( PROPERTY_BORDER,                 "Border");
    PCR_CONSTASCII_STRING( PROPERTY_DROPDOWN,               "Dropdown");
    PCR_CONSTASCII_STRING( PROPERTY_MULTI,                  "Multi");
    PCR_CONSTASCII_STRING( PROPERTY_HSCROLL,                "HScroll");
    PCR_CONSTASCII_STRING( PROPERTY_VSCROLL,                "VScroll");
    PCR_CONSTASCII_STRING( PROPERTY_TABSTOP,                "Tabstop");
    PCR_CONSTASCII_STRING( PROPERTY_AUTOCOMPLETE,           "Autocomplete");
    PCR_CONSTASCII_STRING( PROPERTY_HARDLINEBREAKS,         "HardLineBreaks");
    PCR_CONSTASCII_STRING( PROPERTY_PRINTABLE,              "Printable");
    PCR_CONSTASCII_STRING( PROPERTY_ECHO_CHAR,              "EchoChar");
    PCR_CONSTASCII_STRING( PROPERTY_ROWHEIGHT,              "RowHeight");
    PCR_CONSTASCII_STRING( PROPERTY_HELPTEXT,               "HelpText");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_NAME,              "FontName");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_STYLENAME,         "FontStyleName");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_FAMILY,            "FontFamily");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_CHARSET,           "FontCharset");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_HEIGHT,            "FontHeight");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_WEIGHT,            "FontWeight");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_SLANT,             "FontSlant");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_UNDERLINE,         "FontUnderline");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_STRIKEOUT,         "FontStrikeout");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_RELIEF,            "FontRelief");
    PCR_CONSTASCII_STRING( PROPERTY_FONT_EMPHASIS_MARK,     "FontEmphasisMark");
    PCR_CONSTASCII_STRING( PROPERTY_TEXTLINECOLOR,          "TextLineColor");
    PCR_CONSTASCII_STRING( PROPERTY_HELPURL,                "HelpURL");
    PCR_CONSTASCII_STRING( PROPERTY_RECORDMARKER,           "HasRecordMarker");
    PCR_CONSTASCII_STRING( PROPERTY_EFFECTIVE_DEFAULT,      "EffectiveDefault");
    PCR_CONSTASCII_STRING( PROPERTY_EFFECTIVE_MIN,          "EffectiveMin");
    PCR_CONSTASCII_STRING( PROPERTY_EFFECTIVE_MAX,          "EffectiveMax");
    PCR_CONSTASCII_STRING( PROPERTY_FILTERPROPOSAL,         "UseFilterValueProposal");
    PCR_CONSTASCII_STRING( PROPERTY_CURRSYM_POSITION,       "PrependCurrencySymbol");
    PCR_CONSTASCII_STRING( PROPERTY_COMMAND,                "Command");
    PCR_CONSTASCII_STRING( PROPERTY_COMMANDTYPE,            "CommandType");
    PCR_CONSTASCII_STRING( PROPERTY_INSERTONLY,             "IgnoreResult");
    PCR_CONSTASCII_STRING( PROPERTY_ESCAPE_PROCESSING,      "EscapeProcessing");
    PCR_CONSTASCII_STRING( PROPERTY_TITLE,                  "Title");
    PCR_CONSTASCII_STRING( PROPERTY_SORT,                   "Order");
    PCR_CONSTASCII_STRING( PROPERTY_DATASOURCE,             "DataSourceName");
    PCR_CONSTASCII_STRING( PROPERTY_DETAILFIELDS,           "DetailFields");
    PCR_CONSTASCII_STRING( PROPERTY_DEFAULTBUTTON,          "DefaultButton");
    PCR_CONSTASCII_STRING( PROPERTY_LISTINDEX,              "ListIndex");
    PCR_CONSTASCII_STRING( PROPERTY_HEIGHT,                 "Height");
    PCR_CONSTASCII_STRING( PROPERTY_HASNAVIGATION,          "HasNavigationBar");
    PCR_CONSTASCII_STRING( PROPERTY_POSITIONX,              "PositionX");
    PCR_CONSTASCII_STRING( PROPERTY_POSITIONY,              "PositionY");
    PCR_CONSTASCII_STRING( PROPERTY_STEP,                   "Step");
    PCR_CONSTASCII_STRING( PROPERTY_WORDLINEMODE,           "FontWordLineMode");
    PCR_CONSTASCII_STRING( PROPERTY_PROGRESSVALUE,          "ProgressValue");
    PCR_CONSTASCII_STRING( PROPERTY_PROGRESSVALUE_MIN,      "ProgressValueMin");
    PCR_CONSTASCII_STRING( PROPERTY_PROGRESSVALUE_MAX,      "ProgressValueMax");
    PCR_CONSTASCII_STRING( PROPERTY_SCROLLVALUE,            "ScrollValue");
    PCR_CONSTASCII_STRING( PROPERTY_SCROLLVALUE_MAX,        "ScrollValueMax");
    PCR_CONSTASCII_STRING( PROPERTY_LINEINCREMENT,          "LineIncrement");
    PCR_CONSTASCII_STRING( PROPERTY_BLOCKINCREMENT,         "BlockIncrement");
    PCR_CONSTASCII_STRING( PROPERTY_VISIBLESIZE,            "VisibleSize");
    PCR_CONSTASCII_STRING( PROPERTY_ORIENTATION,            "Orientation");
    PCR_CONSTASCII_STRING( PROPERTY_IMAGEALIGN,             "ImageAlign");
    PCR_CONSTASCII_STRING( PROPERTY_ACTIVE_CONNECTION,      "ActiveConnection");
    PCR_CONSTASCII_STRING( PROPERTY_DATE,                   "Date");
    PCR_CONSTASCII_STRING( PROPERTY_STATE,                  "State");
    PCR_CONSTASCII_STRING( PROPERTY_TIME,                   "Time");
    PCR_CONSTASCII_STRING( PROPERTY_SCALEIMAGE,             "ScaleImage");
    PCR_CONSTASCII_STRING( PROPERTY_PUSHBUTTONTYPE,         "PushButtonType");

    // services
    PCR_CONSTASCII_STRING( SERVICE_COMPONENT_GROUPBOX,      "com.sun.star.form.component.GroupBox");
    PCR_CONSTASCII_STRING( SERVICE_COMPONENT_FIXEDTEXT,     "com.sun.star.form.component.FixedText");
    PCR_CONSTASCII_STRING( SERVICE_COMPONENT_FORMATTEDFIELD,"com.sun.star.form.component.FormattedField");
    PCR_CONSTASCII_STRING( SERVICE_DATABASE_CONTEXT,        "com.sun.star.sdb.DatabaseContext");

//............................................................................
} // namespace pcr
//............................................................................

#endif // _EXTENSIONS_FORMSCTRLR_FORMSTRINGS_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.13  2001/10/19 12:58:51  tbe
 *  #92755# Assign Standard Values for Basic Controls in Designmode
 *
 *  Revision 1.12  2001/08/07 08:46:10  fs
 *  #87690# +ACTIVE_CONNECTION
 *
 *  Revision 1.11  2001/08/06 14:51:43  fs
 *  #87690# PROPERTY_URL/PROPERTY_PASSWORD / removed some unused strings
 *
 *  Revision 1.10  2001/06/06 10:37:16  fs
 *  #86837# +PROPERTY_IMAGEALIGN
 *
 *  Revision 1.9  2001/06/06 08:14:59  fs
 *  #86096# +FontRelief/FontEmphasisMark/TextLineColor
 *
 *  Revision 1.8  2001/05/02 12:43:12  tbe
 *  added scrollbar properties
 *
 *  Revision 1.7  2001/04/26 09:16:34  tbe
 *  added progressbar properties
 *
 *  Revision 1.6  2001/03/21 15:40:37  fs
 *  #82696# +FontWordLineMode
 *
 *  Revision 1.5  2001/03/07 14:37:34  tbe
 *  added step property
 *
 *  Revision 1.4  2001/02/22 09:32:45  tbe
 *  added properties for dialog controls
 *
 *  Revision 1.3  2001/02/06 10:18:10  fs
 *  #83479# reintroduced the HasNavigationBar property
 *
 *  Revision 1.2  2001/01/17 08:44:07  fs
 *  #82726# removed an obsolete string
 *
 *  Revision 1.1  2001/01/12 11:29:10  fs
 *  initial checkin - outsourced the form property browser
 *
 *
 *  Revision 1.0 08.01.01 14:17:16  fs
 ************************************************************************/

