/*************************************************************************
 *
 *  $RCSfile: services.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: hr $ $Date: 2004-04-13 11:40:25 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _FRM_SERVICES_HXX_
#define _FRM_SERVICES_HXX_

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef FRM_STRINGS_HXX
#include "frm_strings.hxx"
#endif

//.........................................................................
namespace frm
{
//.........................................................................

    FORMS_CONSTASCII_STRING( AWT_CONTROL_TEXTFIELD, "com.sun.star.awt.TextField" );

    FORMS_CONSTASCII_STRING( VCL_CONTROL_EDIT, "stardiv.vcl.control.Edit" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_LISTBOX, "stardiv.vcl.control.ListBox" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_COMBOBOX, "stardiv.vcl.control.ComboBox" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_RADIOBUTTON, "stardiv.vcl.control.RadioButton" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_GROUPBOX, "stardiv.vcl.control.GroupBox" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_FIXEDTEXT, "stardiv.vcl.control.FixedText" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_COMMANDBUTTON, "stardiv.vcl.control.Button" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_CHECKBOX, "stardiv.vcl.control.CheckBox" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_IMAGEBUTTON, "stardiv.vcl.control.ImageButton" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_FILECONTROL, "stardiv.vcl.control.FileControl" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_TIMEFIELD, "stardiv.vcl.control.TimeField" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_DATEFIELD, "stardiv.vcl.control.DateField" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_NUMERICFIELD, "stardiv.vcl.control.NumericField" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_CURRENCYFIELD, "stardiv.vcl.control.CurrencyField" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_PATTERNFIELD, "stardiv.vcl.control.PatternField" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_FORMATTEDFIELD, "stardiv.vcl.control.FormattedField" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_IMAGECONTROL, "stardiv.vcl.control.ImageControl" );

    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_EDIT,             "stardiv.vcl.controlmodel.Edit"              );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_LISTBOX,          "stardiv.vcl.controlmodel.ListBox"           );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_COMBOBOX,         "stardiv.vcl.controlmodel.ComboBox"          );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_RADIOBUTTON,      "stardiv.vcl.controlmodel.RadioButton"       );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_GROUPBOX,         "stardiv.vcl.controlmodel.GroupBox"          );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_FIXEDTEXT,        "stardiv.vcl.controlmodel.FixedText"         );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_COMMANDBUTTON,    "stardiv.vcl.controlmodel.Button"            );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_CHECKBOX,         "stardiv.vcl.controlmodel.CheckBox"          );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_IMAGEBUTTON,      "stardiv.vcl.controlmodel.ImageButton"       );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_FILECONTROL,      "stardiv.vcl.controlmodel.FileControl"       );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_TIMEFIELD,        "stardiv.vcl.controlmodel.TimeField"         );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_DATEFIELD,        "stardiv.vcl.controlmodel.DateField"         );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_NUMERICFIELD,     "stardiv.vcl.controlmodel.NumericField"      );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_CURRENCYFIELD,    "stardiv.vcl.controlmodel.CurrencyField"     );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_PATTERNFIELD,     "stardiv.vcl.controlmodel.PatternField"      );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_FORMATTEDFIELD,   "stardiv.vcl.controlmodel.FormattedField"    );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_IMAGECONTROL,     "stardiv.vcl.controlmodel.ImageControl"      );

    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_SCROLLBAR,        "com.sun.star.awt.UnoControlScrollBarModel"  );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_SCROLLBAR,             "com.sun.star.awt.UnoControlScrollBar"       );
    FORMS_CONSTASCII_STRING( VCL_CONTROLMODEL_SPINBUTTON,       "com.sun.star.awt.UnoControlSpinButtonModel" );
    FORMS_CONSTASCII_STRING( VCL_CONTROL_SPINBUTTON,            "com.sun.star.awt.UnoControlSpinButton"      );

    // -----------------------
    // service names for compatibility
    // -----------------------
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_FORM, "stardiv.one.form.component.Form" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_EDIT, "stardiv.one.form.component.Edit" );           // compatibility
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_TEXTFIELD, "stardiv.one.form.component.TextField" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_LISTBOX, "stardiv.one.form.component.ListBox" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_COMBOBOX, "stardiv.one.form.component.ComboBox" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_RADIOBUTTON, "stardiv.one.form.component.RadioButton" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_GROUPBOX, "stardiv.one.form.component.GroupBox" );       // compatibility
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_FIXEDTEXT, "stardiv.one.form.component.FixedText" );     // compatibility
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_COMMANDBUTTON, "stardiv.one.form.component.CommandButton" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_CHECKBOX, "stardiv.one.form.component.CheckBox" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_GRID, "stardiv.one.form.component.Grid" );           // compatibility
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_GRIDCONTROL, "stardiv.one.form.component.GridControl" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_IMAGEBUTTON, "stardiv.one.form.component.ImageButton" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_FILECONTROL, "stardiv.one.form.component.FileControl" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_TIMEFIELD, "stardiv.one.form.component.TimeField" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_DATEFIELD, "stardiv.one.form.component.DateField" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_NUMERICFIELD, "stardiv.one.form.component.NumericField" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_CURRENCYFIELD, "stardiv.one.form.component.CurrencyField" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_PATTERNFIELD, "stardiv.one.form.component.PatternField" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_HIDDEN, "stardiv.one.form.component.Hidden" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_HIDDENCONTROL, "stardiv.one.form.component.HiddenControl" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_IMAGECONTROL, "stardiv.one.form.component.ImageControl" );
    FORMS_CONSTASCII_STRING( FRM_COMPONENT_FORMATTEDFIELD, "stardiv.one.form.component.FormattedField" );

    FORMS_CONSTASCII_STRING( FRM_CONTROL_EDIT, "stardiv.one.form.control.Edit" );                   // compatibility
    FORMS_CONSTASCII_STRING( FRM_CONTROL_TEXTFIELD, "stardiv.one.form.control.TextField" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_LISTBOX, "stardiv.one.form.control.ListBox" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_COMBOBOX, "stardiv.one.form.control.ComboBox" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_RADIOBUTTON, "stardiv.one.form.control.RadioButton" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_GROUPBOX, "stardiv.one.form.control.GroupBox" );               // compatibility
    FORMS_CONSTASCII_STRING( FRM_CONTROL_FIXEDTEXT, "stardiv.one.form.control.FixedText" );         // compatibility
    FORMS_CONSTASCII_STRING( FRM_CONTROL_COMMANDBUTTON, "stardiv.one.form.control.CommandButton" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_CHECKBOX, "stardiv.one.form.control.CheckBox" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_GRID, "stardiv.one.form.control.Grid" );                   // compatibility
    FORMS_CONSTASCII_STRING( FRM_CONTROL_GRIDCONTROL, "stardiv.one.form.control.GridControl" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_IMAGEBUTTON, "stardiv.one.form.control.ImageButton" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_TIMEFIELD, "stardiv.one.form.control.TimeField" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_DATEFIELD, "stardiv.one.form.control.DateField" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_NUMERICFIELD, "stardiv.one.form.control.NumericField" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_CURRENCYFIELD, "stardiv.one.form.control.CurrencyField" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_PATTERNFIELD, "stardiv.one.form.control.PatternField" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_IMAGECONTROL, "stardiv.one.form.control.ImageControl" );
    FORMS_CONSTASCII_STRING( FRM_CONTROL_FORMATTEDFIELD, "stardiv.one.form.control.FormattedField" );

    // -----------------------
    // new (sun) service names
    // -----------------------
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_FORM,            "com.sun.star.form.component.Form"              );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_HTMLFORM,        "com.sun.star.form.component.HTMLForm"          );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATAFORM,        "com.sun.star.form.component.DataForm"          );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_TEXTFIELD,       "com.sun.star.form.component.TextField"         );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_LISTBOX,         "com.sun.star.form.component.ListBox"           );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_COMBOBOX,        "com.sun.star.form.component.ComboBox"          );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_RADIOBUTTON,     "com.sun.star.form.component.RadioButton"       );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_GROUPBOX,        "com.sun.star.form.component.GroupBox"          );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_FIXEDTEXT,       "com.sun.star.form.component.FixedText"         );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_COMMANDBUTTON,   "com.sun.star.form.component.CommandButton"     );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_CHECKBOX,        "com.sun.star.form.component.CheckBox"          );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_GRIDCONTROL,     "com.sun.star.form.component.GridControl"       );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_IMAGEBUTTON,     "com.sun.star.form.component.ImageButton"       );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_FILECONTROL,     "com.sun.star.form.component.FileControl"       );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_TIMEFIELD,       "com.sun.star.form.component.TimeField"         );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATEFIELD,       "com.sun.star.form.component.DateField"         );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_NUMERICFIELD,    "com.sun.star.form.component.NumericField"      );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_CURRENCYFIELD,   "com.sun.star.form.component.CurrencyField"     );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_PATTERNFIELD,    "com.sun.star.form.component.PatternField"      );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_HIDDENCONTROL,   "com.sun.star.form.component.HiddenControl"     );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_FORMATTEDFIELD,  "com.sun.star.form.component.FormattedField"    );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_SCROLLBAR,       "com.sun.star.form.component.ScrollBar"         );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_SPINBUTTON,      "com.sun.star.form.component.SpinButton"        );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_NAVTOOLBAR,      "com.sun.star.form.component.NavigationToolBar" );

    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_IMAGECONTROL, "com.sun.star.form.component.DatabaseImageControl" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_RADIOBUTTON, "com.sun.star.form.component.DatabaseRadioButton" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_CHECKBOX, "com.sun.star.form.component.DatabaseCheckBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_LISTBOX, "com.sun.star.form.component.DatabaseListBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_COMBOBOX, "com.sun.star.form.component.DatabaseComboBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_FORMATTEDFIELD, "com.sun.star.form.component.DatabaseFormattedField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_TEXTFIELD, "com.sun.star.form.component.DatabaseTextField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_DATEFIELD, "com.sun.star.form.component.DatabaseDateField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_TIMEFIELD, "com.sun.star.form.component.DatabaseTimeField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_NUMERICFIELD, "com.sun.star.form.component.DatabaseNumericField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_CURRENCYFIELD, "com.sun.star.form.component.DatabaseCurrencyField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_COMPONENT_DATABASE_PATTERNFIELD, "com.sun.star.form.component.DatabasePatternField" );

    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_TEXTFIELD, "com.sun.star.form.control.TextField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_LISTBOX, "com.sun.star.form.control.ListBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_COMBOBOX, "com.sun.star.form.control.ComboBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_RADIOBUTTON, "com.sun.star.form.control.RadioButton" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_GROUPBOX, "com.sun.star.form.control.GroupBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_FIXEDTEXT, "com.sun.star.form.control.FixedText" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_COMMANDBUTTON, "com.sun.star.form.control.CommandButton" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_CHECKBOX, "com.sun.star.form.control.CheckBox" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_GRIDCONTROL, "com.sun.star.form.control.GridControl" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_IMAGEBUTTON, "com.sun.star.form.control.ImageButton" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_TIMEFIELD, "com.sun.star.form.control.TimeField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_DATEFIELD, "com.sun.star.form.control.DateField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_NUMERICFIELD, "com.sun.star.form.control.NumericField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_CURRENCYFIELD, "com.sun.star.form.control.CurrencyField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_PATTERNFIELD, "com.sun.star.form.control.PatternField" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_IMAGECONTROL, "com.sun.star.form.control.ImageControl" );
    FORMS_CONSTASCII_STRING( FRM_SUN_CONTROL_FORMATTEDFIELD, "com.sun.star.form.control.FormattedField" );

    FORMS_CONSTASCII_STRING( FRM_SUN_FORMS_COLLECTION, "com.sun.star.form.Forms" );

    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_CHECK_BOX,      "com.sun.star.form.component.binding.BindableDatabaseCheckBox" );
    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_COMBO_BOX,      "com.sun.star.form.component.binding.BindableDatabaseComboBox" );
    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_FORMATTED_FIELD,"com.sun.star.form.component.binding.BindableDatabaseFormattedField" );
    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_LIST_BOX,       "com.sun.star.form.component.binding.BindableDatabaseListBox" );
    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_NUMERIC_FIELD,  "com.sun.star.form.component.binding.BindableDatabaseNumericField" );
    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_RADIO_BUTTON,   "com.sun.star.form.component.binding.BindableDatabaseRadioButton" );
    FORMS_CONSTASCII_STRING( BINDABLE_DATABASE_TEXT_FIELD,     "com.sun.star.form.component.binding.BindableDatabaseTextField" );

    DECLARE_CONSTASCII_USTRING( BINDABLE_CONTROL_MODEL,            "com.sun.star.form.binding.BindableControlModel" );
    DECLARE_CONSTASCII_USTRING( BINDABLE_DATA_AWARE_CONTROL_MODEL, "com.sun.star.form.binding.BindableDataAwareControlModel" );
    DECLARE_CONSTASCII_USTRING( DATA_AWARE_CONTROL_MODEL,          "com.sun.star.form.binding.DataAwareControlModel" );
    DECLARE_CONSTASCII_USTRING( VALIDATABLE_CONTROL_MODEL,         "com.sun.star.form.binding.ValidatableControlModel" );
    DECLARE_CONSTASCII_USTRING( VALIDATABLE_BINDABLE_CONTROL_MODEL,"com.sun.star.form.binding.ValidatableBindableControlModel" );

    // -----------------------
    // common
    // -----------------------
    FORMS_CONSTASCII_STRING( FRM_SUN_FORMCOMPONENT, "com.sun.star.form.FormComponent" );

    // -----------------------
    // misc
    // -----------------------
    FORMS_CONSTASCII_STRING( SRV_AWT_POINTER, "com.sun.star.awt.Pointer" );
    FORMS_CONSTASCII_STRING( FRM_NUMBER_FORMATTER, "com.sun.star.util.NumberFormatter" );
    FORMS_CONSTASCII_STRING( FRM_NUMBER_FORMATS_SUPPLIER, "com.sun.star.util.NumberFormatsSupplier" );

    FORMS_CONSTASCII_STRING( SRV_SDB_ROWSET, "com.sun.star.sdb.RowSet" );
    FORMS_CONSTASCII_STRING( SRV_SDB_CONNECTION, "com.sun.star.sdb.Connection" );


//.........................................................................
}   // namespace frm
//.........................................................................

#endif // _FRM_SERVICES_HXX_

