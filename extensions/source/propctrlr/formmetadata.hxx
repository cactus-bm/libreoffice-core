/*************************************************************************
 *
 *  $RCSfile: formmetadata.hxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2004-05-07 16:04:14 $
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

#ifndef _EXTENSIONS_PROPCTRLR_FORMMETADATA_HXX_
#define _EXTENSIONS_PROPCTRLR_FORMMETADATA_HXX_

#ifndef _EXTENSIONS_PROPCTRLR_PROPERTYINFO_HXX_
#include "propertyinfo.hxx"
#endif
#ifndef _EXTENSIONS_PROPCTRLR_MODULEPRC_HXX_
#include "modulepcr.hxx"
#endif

//............................................................................
namespace pcr
{
//............................................................................

    struct OPropertyInfoImpl;

    //========================================================================
    //= OFormPropertyInfoService
    //========================================================================
    class OFormPropertyInfoService
                :public IPropertyInfoService
                ,public OModuleResourceClient
    {
    protected:
        static sal_uInt16               s_nCount;
        static OPropertyInfoImpl*       s_pPropertyInfos;
        // TODO: a real structure which allows quick access by name as well as by id

    public:
        sal_Int32               getPropertyId(const String& _rName) const;
        String                  getPropertyTranslation(sal_Int32 _nId) const;
        sal_Int32               getPropertyHelpId(sal_Int32 _nId) const;
        sal_Int16               getPropertyPos(sal_Int32 _nId) const;
        sal_uInt32              getPropertyUIFlags(sal_Int32 _nId) const;
        virtual ::std::vector< String > getPropertyEnumRepresentations(sal_Int32 _nId) const;

    protected:
        static const OPropertyInfoImpl* getPropertyInfo();

        static const OPropertyInfoImpl* getPropertyInfo(const String& _rName);
        static const OPropertyInfoImpl* getPropertyInfo(sal_Int32 _nId);
    };

    //========================================================================
    //= event description
    //========================================================================
    class EventDisplayDescription
    {
    public:
        ::rtl::OUString sName;
        String          sDisplayName;
        sal_Int32       nHelpId;
        sal_Int32       nIndex;

        EventDisplayDescription(sal_Int32 _nId, const ::rtl::OUString& _rName, const UniString& _rDisplayName, sal_Int32 _nHelpId)
            :nIndex(_nId)
            ,sName(_rName)
            ,sDisplayName(_rDisplayName)
            ,nHelpId(_nHelpId)
            {
            }
    };

    //========================================================================
    //= event helpers
    //========================================================================
    EventDisplayDescription* GetEvtTranslation(const ::rtl::OUString& _rName);

    //========================================================================
    //= UI flags (for all browseable properties)
    //========================================================================

#define PROP_FLAG_NONE              0x00000000  // no special flag
#define PROP_FLAG_FORM_VISIBLE      0x00000001  // the property is visible when inspecting a form object
#define PROP_FLAG_DIALOG_VISIBLE    0x00000002  // the property is visible when inspecting a dialog object
#define PROP_FLAG_VIRTUAL_PROP      0x00000004  // the property is a "virtual" property
#define PROP_FLAG_DATA_PROPERTY     0x00000008  // the property is to appear on the "Data" page
#define PROP_FLAG_ACTUATING         0x00000010  // the property is "actuating" - when it changes,
                                                //  dependent properties (their UI, more concrete) need
                                                //  also to be updated
#define PROP_FLAG_ENUM              0x00000020  // the property is some kind of enum property, i.e. its
                                                // value is chose from a fixed list of possible values

    //========================================================================
    //= property ids (for all browseable properties)
    //========================================================================

    #define PROPERTY_ID_NAME                  1
    #define PROPERTY_ID_LABEL                 2
    #define PROPERTY_ID_CONTROLLABEL          3
    #define PROPERTY_ID_MAXTEXTLEN            4
    #define PROPERTY_ID_EDITMASK              5
    #define PROPERTY_ID_LITERALMASK           6
    #define PROPERTY_ID_STRICTFORMAT          7
    #define PROPERTY_ID_ENABLED               8
    #define PROPERTY_ID_READONLY              9
    #define PROPERTY_ID_PRINTABLE            10
    #define PROPERTY_ID_CONTROLSOURCE        11
    #define PROPERTY_ID_TABSTOP              12
    #define PROPERTY_ID_TABINDEX             13
    #define PROPERTY_ID_DATASOURCE           14
    #define PROPERTY_ID_COMMAND              15
    #define PROPERTY_ID_COMMANDTYPE          16
    #define PROPERTY_ID_FILTER               17
    #define PROPERTY_ID_SORT                 18
    #define PROPERTY_ID_INSERTONLY           19
    #define PROPERTY_ID_ALLOWADDITIONS       20
    #define PROPERTY_ID_ALLOWEDITS           21
    #define PROPERTY_ID_ALLOWDELETIONS       22
    #define PROPERTY_ID_NAVIGATION           24
    #define PROPERTY_ID_CYCLE                25
    #define PROPERTY_ID_HIDDEN_VALUE         26
    #define PROPERTY_ID_VALUEMIN             27
    #define PROPERTY_ID_VALUEMAX             28
    #define PROPERTY_ID_VALUESTEP            29
    #define PROPERTY_ID_DEFAULT_VALUE        30
    #define PROPERTY_ID_DECIMAL_ACCURACY     31
    #define PROPERTY_ID_SHOWTHOUSANDSEP      32
    #define PROPERTY_ID_REFVALUE             33
    #define PROPERTY_ID_CURRENCYSYMBOL       34
    #define PROPERTY_ID_CURRSYM_POSITION     35
    #define PROPERTY_ID_DATEMIN              36
    #define PROPERTY_ID_DATEMAX              37
    #define PROPERTY_ID_DATEFORMAT           38
    #define PROPERTY_ID_SELECTEDITEMS        39
    #define PROPERTY_ID_DEFAULT_DATE         40
    #define PROPERTY_ID_TIMEMIN              41
    #define PROPERTY_ID_TIMEMAX              42
    #define PROPERTY_ID_TIMEFORMAT           43
    #define PROPERTY_ID_DEFAULT_TIME         44
    #define PROPERTY_ID_EFFECTIVE_MIN        45
    #define PROPERTY_ID_EFFECTIVE_MAX        46
    #define PROPERTY_ID_EFFECTIVE_DEFAULT    47
    #define PROPERTY_ID_FORMATKEY            48
    #define PROPERTY_ID_CLASSID              50
    #define PROPERTY_ID_HEIGHT               51
    #define PROPERTY_ID_WIDTH                52
    #define PROPERTY_ID_BOUNDCOLUMN          53
    #define PROPERTY_ID_LISTSOURCETYPE       54
    #define PROPERTY_ID_LISTSOURCE           55
    #define PROPERTY_ID_LISTINDEX            56
    #define PROPERTY_ID_STRINGITEMLIST       57
    #define PROPERTY_ID_DEFAULT_TEXT         58
    #define PROPERTY_ID_FONT_NAME            59
    #define PROPERTY_ID_ALIGN                60
    #define PROPERTY_ID_ROWHEIGHT            61
    #define PROPERTY_ID_BACKGROUNDCOLOR      62
    #define PROPERTY_ID_FILLCOLOR            63
    #define PROPERTY_ID_ESCAPE_PROCESSING    64
    #define PROPERTY_ID_LINECOLOR            65
    #define PROPERTY_ID_BORDER               66
    #define PROPERTY_ID_DROPDOWN             67
    #define PROPERTY_ID_AUTOCOMPLETE         68
    #define PROPERTY_ID_LINECOUNT            69
    #define PROPERTY_ID_WORDBREAK            70
    #define PROPERTY_ID_MULTILINE            71
    #define PROPERTY_ID_MULTISELECTION       72
    #define PROPERTY_ID_AUTOLINEBREAK        73
    #define PROPERTY_ID_HSCROLL              74
    #define PROPERTY_ID_VSCROLL              75
    #define PROPERTY_ID_SPIN                 76
    #define PROPERTY_ID_BUTTONTYPE           77
    #define PROPERTY_ID_TARGET_URL           78
    #define PROPERTY_ID_TARGET_FRAME         79
    #define PROPERTY_ID_SUBMIT_ACTION        80
    #define PROPERTY_ID_SUBMIT_TARGET        81
    #define PROPERTY_ID_SUBMIT_METHOD        82
    #define PROPERTY_ID_SUBMIT_ENCODING      83
    #define PROPERTY_ID_DEFAULTCHECKED       84
    #define PROPERTY_ID_DEFAULTBUTTON        85
    #define PROPERTY_ID_IMAGE_URL            86
    #define PROPERTY_ID_DEFAULT_SELECT_SEQ   87
    #define PROPERTY_ID_ECHO_CHAR            88
    #define PROPERTY_ID_EMPTY_IS_NULL        89
    #define PROPERTY_ID_TRISTATE             90
    #define PROPERTY_ID_MASTERFIELDS         91
    #define PROPERTY_ID_DETAILFIELDS         92
    #define PROPERTY_ID_RECORDMARKER         93
    #define PROPERTY_ID_FILTERPROPOSAL       94
    #define PROPERTY_ID_TAG                  95
    #define PROPERTY_ID_HELPTEXT             96
    #define PROPERTY_ID_HELPURL              97
    #define PROPERTY_ID_HASNAVIGATION        98
    #define PROPERTY_ID_POSITIONX            99
    #define PROPERTY_ID_POSITIONY            100
    #define PROPERTY_ID_TITLE                101
    #define PROPERTY_ID_STEP                 102
    #define PROPERTY_ID_PROGRESSVALUE        103
    #define PROPERTY_ID_PROGRESSVALUE_MIN    104
    #define PROPERTY_ID_PROGRESSVALUE_MAX    105
    #define PROPERTY_ID_SCROLLVALUE          106
    #define PROPERTY_ID_SCROLLVALUE_MAX      107
    #define PROPERTY_ID_LINEINCREMENT        108
    #define PROPERTY_ID_BLOCKINCREMENT       109
    #define PROPERTY_ID_VISIBLESIZE          110
    #define PROPERTY_ID_ORIENTATION          111
    #define PROPERTY_ID_IMAGEALIGN           112
    #define PROPERTY_ID_DATE                 113
    #define PROPERTY_ID_STATE                114
    #define PROPERTY_ID_TIME                 115
    #define PROPERTY_ID_VALUE                116
    #define PROPERTY_ID_SCALEIMAGE           117
    #define PROPERTY_ID_PUSHBUTTONTYPE       118
    #define PROPERTY_ID_EFFECTIVE_VALUE      119
    #define PROPERTY_ID_TEXT                 120
    #define PROPERTY_ID_BOUND_CELL           121
    #define PROPERTY_ID_LIST_CELL_RANGE      122
    #define PROPERTY_ID_CELL_EXCHANGE_TYPE   123
    #define PROPERTY_ID_SCROLLVALUE_MIN      124
    #define PROPERTY_ID_DEFAULT_SCROLLVALUE  125
    #define PROPERTY_ID_REPEAT_DELAY         126
    #define PROPERTY_ID_SYMBOLCOLOR          127
    #define PROPERTY_ID_SPINVALUE            128
    #define PROPERTY_ID_SPINVALUE_MIN        129
    #define PROPERTY_ID_SPINVALUE_MAX        130
    #define PROPERTY_ID_DEFAULT_SPINVALUE    131
    #define PROPERTY_ID_SPININCREMENT        132
    #define PROPERTY_ID_REPEAT               133
    #define PROPERTY_ID_SHOW_SCROLLBARS      134
    #define PROPERTY_ID_ICONSIZE             135
    #define PROPERTY_ID_SHOW_POSITION        136
    #define PROPERTY_ID_SHOW_NAVIGATION      137
    #define PROPERTY_ID_SHOW_RECORDACTIONS   138
    #define PROPERTY_ID_SHOW_FILTERSORT      139
    #define PROPERTY_ID_TEXTTYPE             140

//............................................................................
} // namespace pcr
//............................................................................

#endif // _EXTENSIONS_PROPCTRLR_FORMMETADATA_HXX_

