/*************************************************************************
 *
 *  $RCSfile: elementexport.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: fs $ $Date: 2000-11-19 15:41:32 $
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

#ifndef _XMLOFF_ELEMENTEXPORT_HXX_
#include "elementexport.hxx"
#endif
#ifndef _XMLOFF_FORMS_STRINGS_HXX_
#include "strings.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
#ifndef _CPPUHELPER_EXTRACT_HXX_
#include <cppuhelper/extract.hxx>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMCOMPONENTTYPE_HPP_
#include <com/sun/star/form/FormComponentType.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _XMLOFF_FORMENUMS_HXX_
#include "formenums.hxx"
#endif
#ifndef _COM_SUN_STAR_FORM_FORMSUBMITENCODING_HPP_
#include <com/sun/star/form/FormSubmitEncoding.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMSUBMITMETHOD_HPP_
#include <com/sun/star/form/FormSubmitMethod.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_NAVIGATIONBARMODE_HPP_
#include <com/sun/star/form/NavigationBarMode.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_TABULATORCYCLE_HPP_
#include <com/sun/star/form/TabulatorCycle.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_FORMBUTTONTYPE_HPP_
#include <com/sun/star/form/FormButtonType.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_LISTSOURCETYPE_HPP_
#include <com/sun/star/form/ListSourceType.hpp>
#endif
#ifndef _SV_WINTYPES_HXX
#include <vcl/wintypes.hxx>     // for check states
#endif


//.........................................................................
namespace xmloff
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::sdb;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::container;

    //=====================================================================
    //= OControlElement
    //=====================================================================
    //---------------------------------------------------------------------
    const sal_Char* OControlElement::getElementName(ElementType _eType)
    {
        switch (_eType)
        {
            case TEXT: return "text";
            case TEXT_AREA: return "text-area";
            case PASSWORD: return "password";
            case FILE: return "file";
            case FORMATTED_TEXT: return "formatted-text";
            case FIXED_TEXT: return "fixed-text";
            case COMBOBOX: return "combobox";
            case LISTBOX: return "listbox";
            case BUTTON: return "button";
            case IMAGE: return "image";
            case CHECKBOX: return "checkbox";
            case RADIO: return "radio";
            case FRAME: return "frame";
            case IMAGE_FRAME: return "image-frame";
            case HIDDEN: return "hidden";
            case GRID: return "grid";
            case COLUMN: return "column";

            default: return "control";
        }
    }

    //=====================================================================
    //= OControlExport
    //=====================================================================
    //---------------------------------------------------------------------
    OControlExport::OControlExport(SvXMLExport& _rContext, IExportImplementation* _pCallback,
        const Reference< XPropertySet >& _rxControl,
        const ::rtl::OUString& _rControlId, const ::rtl::OUString& _rReferringControls)
        :OPropertyExport(_rContext, _rxControl)
        ,m_sControlId(_rControlId)
        ,m_sReferringControls(_rReferringControls)
        ,m_nIncludeCommon(0)
        ,m_nIncludeDatabase(0)
        ,m_nIncludeSpecial(0)
        ,m_nIncludeEvents(0)
        ,m_nClassId(FormComponentType::CONTROL)
        ,m_pXMLElement(NULL)
        ,m_pCallback(_pCallback)
    {
        OSL_ENSURE(m_xProps.is(), "OControlExport::OControlExport: invalid arguments!");
    }

    //---------------------------------------------------------------------
    void OControlExport::doExport()
    {
        // collect some general information about the control
        examine();

        // start the element
        startExportElement();

        // the sub elements (mostly control type dependent)
        exportSubTags();
    }

    //---------------------------------------------------------------------
    OControlExport::~OControlExport()
    {
        delete m_pXMLElement;
            // if we already started the element, this will end it, if not, nothing will happen
    }

    //---------------------------------------------------------------------
    void OControlExport::startExportElement()
    {
        // first add the attributes necessary for the element
        m_rContext.ClearAttrList();

        // common control attributes
        exportCommonControlAttributes();

        // common database attributes
        exportDatabaseAttributes();

        // attributes special to the respective control type
        exportSpecialAttributes();

        // add the style references to the attributes
        implExportStyleReference();

        // TODO: add the event attributes

        m_pXMLElement = new SvXMLElementExport(m_rContext, XML_NAMESPACE_FORM, getElementName(m_eType), sal_True, sal_True);
    }

    //---------------------------------------------------------------------
    void OControlExport::exportSubTags() throw (Exception)
    {
        // the ListSource related properties do not need to be exported in a generic way, exportListSourceAsElements
        // will handle this (if necessary)
        exportedProperty(PROPERTY_STRING_ITEM_LIST);
        exportedProperty(PROPERTY_VALUE_SEQ);
        exportedProperty(PROPERTY_SELECT_SEQ);
        exportedProperty(PROPERTY_DEFAULT_SELECT_SEQ);
        exportedProperty(PROPERTY_LISTSOURCE);

        // for the upcoming exportRemainingProperties:
        // if a control has the LabelControl property, this is not stored with the control itself, but instead with
        // the control which is referenced by this property. As the base class' exportRemainingProperties doesn't
        // know anything about this, we need to prevent that it tries to export this property
        exportedProperty(PROPERTY_CONTROLLABEL);

        // the properties which where not exported 'til now
        exportRemainingProperties();

        // TODO: the script:events sub tags

        // special sub tags for some controls
        switch (m_eType)
        {
            case LISTBOX:
                // a list box description has sub elements: the options
                exportListSourceAsElements();
                break;
            case GRID:
            {   // a grid control requires us to store all columns as sub elements
                OSL_ENSURE(m_pCallback, "OControlExport::exportSubTags: need a callback for the export of GridControls!");
                Reference< XIndexAccess > xColumnContainer(m_xProps, UNO_QUERY);
                OSL_ENSURE(xColumnContainer.is(), "OControlExport::exportSubTags: a grid control which is no IndexAccess?!!");
                if (xColumnContainer.is())
                    m_pCallback->exportCollectionElements(xColumnContainer);
            }
            break;
            case COMBOBOX:
            {   // a combox box description has sub elements: the items
                DBG_CHECK_PROPERTY((const sal_Char*)PROPERTY_STRING_ITEM_LIST, Sequence< ::rtl::OUString >);

                // get the item list
                Sequence< ::rtl::OUString > aListItems;
                m_xProps->getPropertyValue(PROPERTY_STRING_ITEM_LIST) >>= aListItems;
                // loop through it and write the sub elements
                const ::rtl::OUString* pListItems = aListItems.getConstArray();
                for (sal_Int32 i=0; i<aListItems.getLength(); ++i, ++pListItems)
                {
                    m_rContext.ClearAttrList();
                    AddAttribute(
                        getCommonControlAttributeNamespace(CCA_LABEL),
                        getCommonControlAttributeName(CCA_LABEL),
                        *pListItems);
                    SvXMLElementExport aFormElement(m_rContext, XML_NAMESPACE_FORM, "item", sal_True, sal_True);
                }
            }
            break;
        }
    }

    //---------------------------------------------------------------------
    void OControlExport::exportCommonControlAttributes()
    {
        sal_Int32 i=0;

        // I decided to handle all the properties here with some static arrays describing the property-attribute
        // relations. This leads to somewhat ugly code :), but the only alternative I can think of right now
        // would require maps and O(log n) searches, which seems somewhat expensive as this code is used
        // very frequently.

        // the extra indents for the respective blocks are to ensure that there is no copy'n'paste error, using
        // map identifiers from the wrong block

        // the control id
        if (CCA_CONTROL_ID & m_nIncludeCommon)
        {
            OSL_ENSURE(m_sControlId.getLength(), "OControlExport::exportCommonControlAttributes: have no control id for the control!");
            AddAttribute(
                getCommonControlAttributeNamespace(CCA_CONTROL_ID),
                getCommonControlAttributeName(CCA_CONTROL_ID),
                m_sControlId);
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeCommon = m_nIncludeCommon & ~CCA_CONTROL_ID;
        #endif
        }

        // --------------------------------------------------------------------
        // some string properties
        {
            // the attribute ids of all properties which are expected to be of type string
            static sal_Int32 nStringPropertyAttributeIds[] =
            {
                CCA_NAME, CCA_IMAGE_DATA, CCA_LABEL, CCA_TARGET_LOCATION, CCA_TITLE
            };
            // the names of all properties which are expected to be of type string
            static const sal_Char* pStringPropertyNames[] =
            {
                PROPERTY_NAME, PROPERTY_IMAGEURL, PROPERTY_LABEL, PROPERTY_TARGETURL, PROPERTY_TITLE
            };
            OSL_ENSURE( sizeof(pStringPropertyNames)/sizeof(pStringPropertyNames[0]) ==
                        sizeof(nStringPropertyAttributeIds)/sizeof(nStringPropertyAttributeIds[0]),
                        "OControlExport::exportCommonControlAttributes: somebody tampered with the maps (1)!");

            for (i=0; i<sizeof(nStringPropertyAttributeIds)/sizeof(nStringPropertyAttributeIds[0]); ++i)
                if (nStringPropertyAttributeIds[i] & m_nIncludeCommon)
                {
                    exportStringPropertyAttribute(
                        getCommonControlAttributeNamespace(nStringPropertyAttributeIds[i]),
                        getCommonControlAttributeName(nStringPropertyAttributeIds[i]),
                        pStringPropertyNames[i]
                        );
                #ifdef DBG_UTIL
                    //  reset the bit for later checking
                    m_nIncludeCommon = m_nIncludeCommon & ~nStringPropertyAttributeIds[i];
                #endif
                }
        }

        // --------------------------------------------------------------------
        // some boolean properties
        {
            static sal_Int32 nBooleanPropertyAttributeIds[] =
            {   // attribute flags
                CCA_CURRENT_SELECTED, CCA_DISABLED, CCA_DROPDOWN, CCA_PRINTABLE, CCA_READONLY, CCA_SELECTED, CCA_TAB_STOP
            };
            static const sal_Char* pBooleanPropertyNames[] =
            {   // property names
                PROPERTY_STATE, PROPERTY_ENABLED, PROPERTY_DROPDOWN, PROPERTY_PRINTABLE, PROPERTY_READONLY, PROPERTY_DEFAULT_STATE, PROPERTY_TABSTOP
            };
            static sal_Bool nBooleanPropertyAttrDefaults[] =
            {   // attribute defaults
                sal_False, sal_False, sal_False, sal_True, sal_False, sal_False, sal_True
            };
            static sal_Bool nBooleanPropertySemantics[] =
            {   // inverse semantic?
                sal_False, sal_True, sal_False, sal_False, sal_False, sal_False, sal_False
            };
        #ifdef DBG_UTIL
            sal_Int32 nIdCount = sizeof(nBooleanPropertyAttributeIds) / sizeof(nBooleanPropertyAttributeIds[0]);
            sal_Int32 nNameCount = sizeof(pBooleanPropertyNames) / sizeof(pBooleanPropertyNames[0]);
            sal_Int32 nDefaultCount = sizeof(nBooleanPropertyAttrDefaults) / sizeof(nBooleanPropertyAttrDefaults[0]);
            sal_Int32 nSemanticsCount = sizeof(nBooleanPropertySemantics) / sizeof(nBooleanPropertySemantics[0]);
            OSL_ENSURE((nIdCount == nNameCount) && (nNameCount == nDefaultCount) && (nDefaultCount == nSemanticsCount),
                "OControlExport::exportCommonControlAttributes: somebody tampered with the maps (2)!");
        #endif
            for (i=0; i<sizeof(nBooleanPropertyAttributeIds)/sizeof(nBooleanPropertyAttributeIds[0]); ++i)
                if (nBooleanPropertyAttributeIds[i] & m_nIncludeCommon)
                {
                    exportBooleanPropertyAttribute(
                        getCommonControlAttributeNamespace(nBooleanPropertyAttributeIds[i]),
                        getCommonControlAttributeName(nBooleanPropertyAttributeIds[i]),
                        pBooleanPropertyNames[i],
                        nBooleanPropertyAttrDefaults[i],
                        nBooleanPropertySemantics[i]);
        #ifdef DBG_UTIL
                    //  reset the bit for later checking
                    m_nIncludeCommon = m_nIncludeCommon & ~nBooleanPropertyAttributeIds[i];
        #endif
                }
        }


        // --------------------------------------------------------------------
        // some integer properties
        {
            static sal_Int32 nIntegerPropertyAttributeIds[] =
            {   // attribute flags
                CCA_MAX_LENGTH, CCA_SIZE, CCA_TAB_INDEX
            };
            static const sal_Char* pIntegerPropertyNames[] =
            {   // property names
                PROPERTY_MAXTEXTLENGTH, PROPERTY_LINECOUNT, PROPERTY_TABINDEX
            };
            static const sal_Int16 nIntegerPropertyAttrDefaults[] =
            {   // attribute defaults
                0, 5, 0
            };
        #ifdef DBG_UTIL
            sal_Int32 nIdCount = sizeof(nIntegerPropertyAttributeIds) / sizeof(nIntegerPropertyAttributeIds[0]);
            sal_Int32 nNameCount = sizeof(pIntegerPropertyNames) / sizeof(pIntegerPropertyNames[0]);
            sal_Int32 nDefaultCount = sizeof(nIntegerPropertyAttrDefaults) / sizeof(nIntegerPropertyAttrDefaults[0]);
            OSL_ENSURE((nIdCount == nNameCount) && (nNameCount == nDefaultCount),
                "OControlExport::exportCommonControlAttributes: somebody tampered with the maps (3)!");
        #endif
            for (i=0; i<sizeof(nIntegerPropertyAttributeIds)/sizeof(nIntegerPropertyAttributeIds[0]); ++i)
                if (nIntegerPropertyAttributeIds[i] & m_nIncludeCommon)
                {
                    exportInt16PropertyAttribute(
                        getCommonControlAttributeNamespace(nIntegerPropertyAttributeIds[i]),
                        getCommonControlAttributeName(nIntegerPropertyAttributeIds[i]),
                        pIntegerPropertyNames[i],
                        nIntegerPropertyAttrDefaults[i]);
        #ifdef DBG_UTIL
                    //  reset the bit for later checking
                    m_nIncludeCommon = m_nIncludeCommon & ~nIntegerPropertyAttributeIds[i];
        #endif
                }
        }

        // --------------------------------------------------------------------
        // some enum properties
        {
            // (only one a at the moment)
            if (m_nIncludeCommon & CCA_BUTTON_TYPE)
            {
                exportEnumPropertyAttribute(
                    getCommonControlAttributeNamespace(CCA_BUTTON_TYPE),
                    getCommonControlAttributeName(CCA_BUTTON_TYPE),
                    PROPERTY_BUTTONTYPE,
                    OEnumMapper::getEnumMap(OEnumMapper::epButtonType),
                    FormButtonType_PUSH);
        #ifdef DBG_UTIL
                //  reset the bit for later checking
                m_nIncludeCommon = m_nIncludeCommon & ~CCA_BUTTON_TYPE;
        #endif
            }
        }

        // --------------------------------------------------------------------
        // some properties which require a special handling

        // the service name
        if (m_nIncludeCommon & CCA_SERVICE_NAME)
        {
            exportServiceNameAttribute();
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeCommon = m_nIncludeCommon & ~CCA_SERVICE_NAME;
        #endif
        }

        // the target frame
        if (m_nIncludeCommon & CCA_TARGET_FRAME)
        {
            exportTargetFrameAttribute();
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeCommon = m_nIncludeCommon & ~CCA_TARGET_FRAME;
        #endif
        }

        // the for attribute
        // the target frame
        if (m_nIncludeCommon & CCA_FOR)
        {
            if (m_sReferringControls.getLength())
            {   // there is at least one control referring to the one we're handling currently
                AddAttribute(
                    getCommonControlAttributeNamespace(CCA_FOR),
                    getCommonControlAttributeName(CCA_FOR),
                    m_sReferringControls);
            }
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeCommon = m_nIncludeCommon & ~CCA_FOR;
        #endif
        }

        if ((CCA_CURRENT_VALUE | CCA_VALUE) & m_nIncludeCommon)
        {
            const sal_Char* pCurrentValuePropertyName = NULL;
            const sal_Char* pValuePropertyName = NULL;

            // get the property names
            getPropertyNames_ca(pCurrentValuePropertyName, pValuePropertyName);

            static const sal_Char* pCurrentValueAttributeName = getCommonControlAttributeName(CCA_CURRENT_VALUE);
            static const sal_Char* pValueAttributeName = getCommonControlAttributeName(CCA_VALUE);
            static const sal_uInt16 nCurrentValueAttributeNamespaceKey = getCommonControlAttributeNamespace(CCA_CURRENT_VALUE);
            static const sal_uInt16 nValueAttributeNamespaceKey = getCommonControlAttributeNamespace(CCA_VALUE);

            // add the atrtributes if necessary and possible
            if (pCurrentValuePropertyName && (CCA_CURRENT_VALUE & m_nIncludeCommon))
                exportGenericPropertyAttribute(
                    nCurrentValueAttributeNamespaceKey,
                    pCurrentValueAttributeName,
                    pCurrentValuePropertyName);

            if (pValuePropertyName && (CCA_VALUE & m_nIncludeCommon))
                exportGenericPropertyAttribute(
                    nValueAttributeNamespaceKey,
                    pValueAttributeName,
                    pValuePropertyName);

            OSL_ENSURE((NULL == pValuePropertyName) == (0 == (CCA_VALUE & m_nIncludeCommon)),
                "OControlExport::exportCommonControlAttributes: no property found for the value attribute!");
            OSL_ENSURE((NULL == pCurrentValuePropertyName ) == (0 == (CCA_CURRENT_VALUE & m_nIncludeCommon)),
                "OControlExport::exportCommonControlAttributes: no property found for the current-value attribute!");

        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeCommon = m_nIncludeCommon & ~(CCA_CURRENT_VALUE | CCA_VALUE);
        #endif
        }

        OSL_ENSURE(0 == m_nIncludeCommon,
            "OControlExport::exportCommonControlAttributes: forgot some flags!");
            // in the dbg_util version, we should have removed every bit we handled from the mask, so it should
            // be 0 now ...
    }

    //---------------------------------------------------------------------
    void OControlExport::exportDatabaseAttributes()
    {
        // the only string property: DataField
        if (DA_DATA_FIELD & m_nIncludeDatabase)
        {
            exportStringPropertyAttribute(
                getDatabaseAttributeNamespace(DA_DATA_FIELD),
                getDatabaseAttributeName(DA_DATA_FIELD),
                PROPERTY_DATAFIELD);
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeDatabase = m_nIncludeDatabase & ~DA_DATA_FIELD;
        #endif
        }

        // the only int16 property: BoundColumn
        if (DA_BOUND_COLUMN & m_nIncludeDatabase)
        {
            exportInt16PropertyAttribute(
                getDatabaseAttributeNamespace(DA_BOUND_COLUMN),
                getDatabaseAttributeName(DA_BOUND_COLUMN),
                PROPERTY_BOUNDCOLUMN,
                0);
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeDatabase = m_nIncludeDatabase & ~DA_BOUND_COLUMN;
        #endif
        }

        // the only boolean property: ConvertEmptyToNull
        if (DA_CONVERT_EMPTY & m_nIncludeDatabase)
        {
            exportBooleanPropertyAttribute(
                getDatabaseAttributeNamespace(DA_CONVERT_EMPTY),
                getDatabaseAttributeName(DA_CONVERT_EMPTY),
                PROPERTY_EMPTY_IS_NULL,
                sal_False,      // the default
                sal_False       // no inverse sematics
                );
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeDatabase = m_nIncludeDatabase & ~DA_CONVERT_EMPTY;
        #endif
        }

        // the only enum property: ListSourceType
        if (DA_LIST_SOURCE_TYPE & m_nIncludeDatabase)
        {
            exportEnumPropertyAttribute(
                getDatabaseAttributeNamespace(DA_LIST_SOURCE_TYPE),
                getDatabaseAttributeName(DA_LIST_SOURCE_TYPE),
                PROPERTY_LISTSOURCETYPE,
                OEnumMapper::getEnumMap(OEnumMapper::epListSourceType),
                ListSourceType_VALUELIST
                );
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeDatabase = m_nIncludeDatabase & ~DA_LIST_SOURCE_TYPE;
        #endif
        }

        if (m_nIncludeDatabase & DA_LIST_SOURCE)
        {
            exportListSourceAsAttribute();
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeDatabase = m_nIncludeDatabase & ~DA_LIST_SOURCE;
        #endif
        }

        OSL_ENSURE(0 == m_nIncludeDatabase,
            "OControlExport::exportDatabaseAttributes: forgot some flags!");
            // in the dbg_util version, we should have removed every bit we handled from the mask, so it should
            // be 0 now ...
    }

    //---------------------------------------------------------------------
    void OControlExport::exportSpecialAttributes()
    {
        sal_Int32 i=0;

        // ----------------------
        // the boolean properties
        {
            static sal_Int32 nBooleanPropertyAttributeIds[] =
            {   // attribute flags
                SCA_VALIDATION, SCA_MULTI_LINE, SCA_AUTOMATIC_COMPLETION, SCA_MULTIPLE, SCA_DEFAULT_BUTTON, SCA_IS_TRISTATE
            };
            static const sal_Char* pBooleanPropertyNames[] =
            {   // property names
                PROPERTY_STRICTFORMAT, PROPERTY_MULTILINE, PROPERTY_AUTOCOMPLETE, PROPERTY_MULTISELECTION, PROPERTY_DEFAULTBUTTON, PROPERTY_TRISTATE
            };
            static sal_Bool nBooleanPropertyAttrDefaults[] =
            {   // attribute defaults
                sal_False, sal_False, sal_False, sal_False, sal_False, sal_False
            };
            sal_Int32 nIdCount = sizeof(nBooleanPropertyAttributeIds) / sizeof(nBooleanPropertyAttributeIds[0]);
        #ifdef DBG_UTIL
            sal_Int32 nNameCount = sizeof(pBooleanPropertyNames) / sizeof(pBooleanPropertyNames[0]);
            sal_Int32 nDefaultCount = sizeof(nBooleanPropertyAttrDefaults) / sizeof(nBooleanPropertyAttrDefaults[0]);
            OSL_ENSURE((nIdCount == nNameCount) && (nNameCount == nDefaultCount),
                "OControlExport::exportSpecialAttributes: somebody tampered with the maps (1)!");
        #endif
            for (i=0; i<nIdCount; ++i)
                if (nBooleanPropertyAttributeIds[i] & m_nIncludeSpecial)
                {
                    exportBooleanPropertyAttribute(
                        getSpecialAttributeNamespace(nBooleanPropertyAttributeIds[i]),
                        getSpecialAttributeName(nBooleanPropertyAttributeIds[i]),
                        pBooleanPropertyNames[i],
                        nBooleanPropertyAttrDefaults[i],
                        sal_False);     // no inverse semantics at all
            #ifdef DBG_UTIL
                //  reset the bit for later checking
                m_nIncludeSpecial = m_nIncludeSpecial & ~nBooleanPropertyAttributeIds[i];
            #endif
                }
        }

        // -------------------
        // the enum properties
        {
            if (SCA_STATE & m_nIncludeSpecial)
            {
                exportEnumPropertyAttribute(
                    getSpecialAttributeNamespace(SCA_STATE),
                    getSpecialAttributeName(SCA_STATE),
                    PROPERTY_DEFAULT_STATE,
                    OEnumMapper::getEnumMap(OEnumMapper::epCheckState),
                    STATE_NOCHECK);
            #ifdef DBG_UTIL
                //  reset the bit for later checking
                m_nIncludeSpecial = m_nIncludeSpecial & ~SCA_STATE;
            #endif
            }

            if (SCA_CURRENT_STATE & m_nIncludeSpecial)
            {
                exportEnumPropertyAttribute(
                    getSpecialAttributeNamespace(SCA_CURRENT_STATE),
                    getSpecialAttributeName(SCA_CURRENT_STATE),
                    PROPERTY_STATE,
                    OEnumMapper::getEnumMap(OEnumMapper::epCheckState),
                    STATE_NOCHECK);
            #ifdef DBG_UTIL
                //  reset the bit for later checking
                m_nIncludeSpecial = m_nIncludeSpecial & ~SCA_CURRENT_STATE;
            #endif
            }
        }

        // ----------------------------------
        // the EchoChar property needs special handling, cause it's a Int16, but must be stored as one-character-string
        {
            if (SCA_ECHO_CHAR & m_nIncludeSpecial)
            {
                DBG_CHECK_PROPERTY((const sal_Char*)PROPERTY_ECHO_CHAR, sal_Int16);
                sal_Int16 nValue(0);
                m_xProps->getPropertyValue(PROPERTY_ECHO_CHAR) >>= nValue;
                if (nValue)
                {
                    ::rtl::OUString sCharacter(reinterpret_cast<const sal_Unicode*>(&nValue), 1);
                    AddAttribute(
                        getSpecialAttributeNamespace(SCA_ECHO_CHAR),
                        getSpecialAttributeName(SCA_ECHO_CHAR),
                        sCharacter);
                }
                exportedProperty(PROPERTY_ECHO_CHAR);
            #ifdef DBG_UTIL
                //  reset the bit for later checking
                m_nIncludeSpecial = m_nIncludeSpecial & ~SCA_ECHO_CHAR;
            #endif
            }
        }

        if ((SCA_MIN_VALUE | SCA_MAX_VALUE) & m_nIncludeSpecial)
        {
            // need to export the min value and the max value as attributes
            // It depends on the real type (FormComponentType) of the control, which properties hold these
            // values
            const sal_Char* pMinValuePropertyName = NULL;
            const sal_Char* pMaxValuePropertyName = NULL;
            switch (m_nClassId)
            {
                case FormComponentType::DATEFIELD:
                    pMinValuePropertyName = PROPERTY_DATE_MIN;
                    pMaxValuePropertyName = PROPERTY_DATE_MAX;
                    break;
                case FormComponentType::TIMEFIELD:
                    pMinValuePropertyName = PROPERTY_TIME_MIN;
                    pMaxValuePropertyName = PROPERTY_TIME_MAX;
                    break;
                case FormComponentType::NUMERICFIELD:
                case FormComponentType::CURRENCYFIELD:
                    pMinValuePropertyName = PROPERTY_VALUE_MIN;
                    pMaxValuePropertyName = PROPERTY_VALUE_MAX;
                    break;
                case FormComponentType::PATTERNFIELD:
                    // no min/max value for the pattern field
                    break;
                case FormComponentType::TEXTFIELD:
                    pMinValuePropertyName = PROPERTY_EFFECTIVE_MIN;
                    pMaxValuePropertyName = PROPERTY_EFFECTIVE_MAX;
                    break;
                default:
                    OSL_ENSURE(sal_False, "OControlExport::examineListSource_maybeExportAttribute: exporting an unknown control as FormattedText!");
                    break;
            }
            // add the two attributes
            static const sal_Char* pMinValueAttributeName = getSpecialAttributeName(SCA_MIN_VALUE);
            static const sal_Char* pMaxValueAttributeName = getSpecialAttributeName(SCA_MAX_VALUE);
            static const sal_uInt16 nMinValueNamespaceKey = getSpecialAttributeNamespace(SCA_MIN_VALUE);
            static const sal_uInt16 nMaxValueNamespaceKey = getSpecialAttributeNamespace(SCA_MAX_VALUE);

            if (pMinValuePropertyName && (SCA_MIN_VALUE & m_nIncludeSpecial))
                exportGenericPropertyAttribute(
                    nMinValueNamespaceKey,
                    pMinValueAttributeName,
                    pMinValuePropertyName);

            if (pMaxValuePropertyName && (SCA_MAX_VALUE & m_nIncludeSpecial))
                exportGenericPropertyAttribute(
                    nMaxValueNamespaceKey,
                    pMaxValueAttributeName,
                    pMaxValuePropertyName);
        #ifdef DBG_UTIL
            //  reset the bit for later checking
            m_nIncludeSpecial = m_nIncludeSpecial & ~(SCA_MIN_VALUE | SCA_MAX_VALUE);
        #endif
        }

        OSL_ENSURE(0 == m_nIncludeSpecial,
            "OControlExport::exportSpecialAttributes: forgot some flags!");
            // in the dbg_util version, we should have removed every bit we handled from the mask, so it should
            // be 0 now ...
    }

    //---------------------------------------------------------------------
    void OControlExport::exportListSourceAsAttribute()
    {
        // DA_LIST_SOURCE needs some special handling
        DBG_CHECK_PROPERTY_NO_TYPE((const sal_Char*)PROPERTY_LISTSOURCE);

        ::rtl::OUString sListSource;
        Any aListSource = m_xProps->getPropertyValue(PROPERTY_LISTSOURCE);
        if (!(aListSource >>= sListSource))
        {
            Sequence< ::rtl::OUString > aListSourceSequence;
            aListSource >>= aListSourceSequence;
            if (aListSourceSequence.getLength())
                sListSource = aListSourceSequence[0];
        }

        if (sListSource.getLength())
        {   // the ListSource property needs to be exported as attribute, and it is not empty
            AddAttribute(
                getDatabaseAttributeNamespace(DA_LIST_SOURCE),
                getDatabaseAttributeName(DA_LIST_SOURCE),
                sListSource);
        }
    }

    //---------------------------------------------------------------------
    void OControlExport::getSequenceInt16PropertyAsSet(const ::rtl::OUString& _rPropertyName, Int16Set& _rOut)
    {
        Sequence< sal_Int16 > aValueSequence;
        DBG_CHECK_PROPERTY(_rPropertyName, Sequence< sal_Int16 >);
        m_xProps->getPropertyValue(_rPropertyName) >>= aValueSequence;

        const sal_Int16* pValues = aValueSequence.getConstArray();
        for (sal_Int32 i=0; i<aValueSequence.getLength(); ++i, ++pValues)
            _rOut.insert(*pValues);
    }

    //---------------------------------------------------------------------
    void OControlExport::exportListSourceAsElements()
    {
        // the string lists
        Sequence< ::rtl::OUString > aItems, aValues;
        DBG_CHECK_PROPERTY((const sal_Char*)PROPERTY_STRING_ITEM_LIST, Sequence< ::rtl::OUString >);
        m_xProps->getPropertyValue(PROPERTY_STRING_ITEM_LIST) >>= aItems;

        DBG_CHECK_PROPERTY((const sal_Char*)PROPERTY_VALUE_SEQ, Sequence< ::rtl::OUString >);
        m_xProps->getPropertyValue(PROPERTY_VALUE_SEQ) >>= aValues;

        // the selection lists
        Int16Set aSelection, aDefaultSelection;
        getSequenceInt16PropertyAsSet(PROPERTY_SELECT_SEQ, aSelection);
        getSequenceInt16PropertyAsSet(PROPERTY_DEFAULT_SELECT_SEQ, aDefaultSelection);

        // the string for "true" and "false"
        ::rtl::OUString sTrue;
        ::rtl::OUStringBuffer sBuffer;
        m_rContext.GetMM100UnitConverter().convertBool(sBuffer, sal_True);
        sTrue = sBuffer.makeStringAndClear();

        // loop through both lists ('til the maximum of both lengths)
        const ::rtl::OUString* pItems = aItems.getConstArray();
        const ::rtl::OUString* pValues = aValues.getConstArray();
        sal_Int32 nItems = aItems.getLength();
        sal_Int32 nValues = aValues.getLength();
        sal_Int32 nMaxLen = max(nItems, nValues);
        for (sal_Int16 i=0; i<nMaxLen; ++i, ++pItems, ++pValues)
        {
            m_rContext.ClearAttrList();
            if (i < nItems)
                // there is an item at this position
                AddAttribute(
                    getCommonControlAttributeNamespace(CCA_LABEL),
                    getCommonControlAttributeName(CCA_LABEL),
                    *pItems);
            if (i < nValues)
                // there is an value at this position
                AddAttribute(
                    getCommonControlAttributeNamespace(CCA_VALUE),
                    getCommonControlAttributeName(CCA_VALUE),
                    *pValues);
            if (aSelection.end() != aSelection.find(i))
            {   // the item at this position is selected
                AddAttribute(
                    getCommonControlAttributeNamespace(CCA_CURRENT_SELECTED),
                    getCommonControlAttributeName(CCA_CURRENT_SELECTED),
                    sTrue
                    );
            }
            if (aDefaultSelection.end() != aDefaultSelection.find(i))
            {   // the item at this position is selected as default
                AddAttribute(
                    getCommonControlAttributeNamespace(CCA_SELECTED),
                    getCommonControlAttributeName(CCA_SELECTED),
                    sTrue
                    );
            }
            SvXMLElementExport aFormElement(m_rContext, XML_NAMESPACE_FORM, "option", sal_True, sal_True);
        }
    }

    //---------------------------------------------------------------------
    void OControlExport::examine()
    {
        // get the class id to decide which kind of element we need in the XML stream
        m_nClassId = FormComponentType::CONTROL;
        DBG_CHECK_PROPERTY((const sal_Char*)PROPERTY_CLASSID, sal_Int16);
        m_xProps->getPropertyValue(PROPERTY_CLASSID) >>= m_nClassId;
        switch (m_nClassId)
        {
            case FormComponentType::DATEFIELD:
            case FormComponentType::TIMEFIELD:
            case FormComponentType::NUMERICFIELD:
            case FormComponentType::CURRENCYFIELD:
            case FormComponentType::PATTERNFIELD:
                m_eType = FORMATTED_TEXT;
                // NO BREAK
            case FormComponentType::TEXTFIELD:
            {   // it's some kind of edit. To know which type we need further investigation

                if (FORMATTED_TEXT != m_eType)
                {   // not coming from the presious cases which had a class id .ne. TEXTFIELD

                    // check if it's a formatted field
                    if (m_xPropertyInfo->hasPropertyByName(PROPERTY_FORMATKEY))
                    {
                        m_eType = FORMATTED_TEXT;
                    }
                    else
                    {
                        // all other controls are represented by an ordinary edit control, but which XML control type
                        // it is depends on the current values of some properties

                        // if the EchoChar string is not empty, it is a password field
                        sal_Int16 nEchoChar = 0;
                        if (m_xPropertyInfo->hasPropertyByName(PROPERTY_ECHOCHAR))
                            // grid columns do not have this property ....
                            m_xProps->getPropertyValue(PROPERTY_ECHOCHAR) >>= nEchoChar;
                        if (nEchoChar)
                        {
                            m_eType = PASSWORD;
                            m_nIncludeSpecial |= SCA_ECHO_CHAR;
                        }
                        else
                        {
                            // if the MultiLine property is sal_True, it is a TextArea
                            sal_Bool bMultiLine = sal_False;
                            if (m_xPropertyInfo->hasPropertyByName(PROPERTY_MULTILINE))
                                // grid columns do not have this property ....
                                bMultiLine = ::cppu::any2bool(m_xProps->getPropertyValue(PROPERTY_MULTILINE));
                            if (bMultiLine)
                                m_eType = TEXT_AREA;
                            else
                                // the only case left is represented by a Text element
                                m_eType = TEXT;
                        }
                    }
                }

                // attributes which are common to all the four types:
                // common attributes
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED |
                    CCA_PRINTABLE | CCA_TAB_INDEX | CCA_TAB_STOP | CCA_TITLE |
                    CCA_VALUE;
                // database attributes
                m_nIncludeDatabase = DA_DATA_FIELD;
                // event attributes
                m_nIncludeEvents = EA_CONTROL_EVENTS | EA_ON_CHANGE | EA_ON_SELECT;

                // only text and pattern fields have a ConvertEmptyToNull property
                if ((m_nClassId == FormComponentType::TEXTFIELD) || (m_nClassId == FormComponentType::PATTERNFIELD))
                    m_nIncludeDatabase |= DA_CONVERT_EMPTY;

                // all controls but the file control fields have a readonly property
                if (m_nClassId != FormComponentType::FILECONTROL)
                    m_nIncludeCommon |= CCA_READONLY;

                // a text field has a max text len
                if (m_nClassId == FormComponentType::TEXTFIELD)
                    m_nIncludeCommon |= CCA_MAX_LENGTH;

                // for formatted-text XML element allows max and min values and validation
                if (FORMATTED_TEXT == m_eType)
                    m_nIncludeSpecial |= SCA_MAX_VALUE | SCA_MIN_VALUE | SCA_VALIDATION;

                // if it's not a password field, the CurrentValue needs to be stored, too
                if (PASSWORD != m_eType)
                    m_nIncludeCommon |= CCA_CURRENT_VALUE;
            }
            break;

            case FormComponentType::FILECONTROL:
                m_eType = FILE;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_CURRENT_VALUE | CCA_DISABLED |
                    CCA_PRINTABLE | CCA_TAB_INDEX | CCA_TAB_STOP | CCA_TITLE |
                    CCA_VALUE;
                m_nIncludeEvents = EA_CONTROL_EVENTS | EA_ON_CHANGE | EA_ON_SELECT;
                break;

            case FormComponentType::FIXEDTEXT:
                m_eType = FIXED_TEXT;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED | CCA_LABEL |
                    CCA_PRINTABLE | CCA_TITLE | CCA_FOR;
                m_nIncludeSpecial = SCA_MULTI_LINE;
                m_nIncludeEvents = EA_CONTROL_EVENTS;
                break;

            case FormComponentType::COMBOBOX:
                m_eType = COMBOBOX;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_CURRENT_VALUE |
                    CCA_DISABLED | CCA_DROPDOWN | CCA_MAX_LENGTH | CCA_PRINTABLE | CCA_READONLY | CCA_SIZE |
                    CCA_TAB_INDEX | CCA_TAB_STOP | CCA_TITLE | CCA_VALUE;
                m_nIncludeSpecial = SCA_AUTOMATIC_COMPLETION;
                m_nIncludeDatabase = DA_CONVERT_EMPTY | DA_DATA_FIELD | DA_LIST_SOURCE | DA_LIST_SOURCE_TYPE;
                m_nIncludeEvents = EA_CONTROL_EVENTS | EA_ON_CHANGE | EA_ON_SELECT;
                break;

            case FormComponentType::LISTBOX:
                m_eType = LISTBOX;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED | CCA_DROPDOWN |
                    CCA_PRINTABLE | CCA_SIZE | CCA_TAB_INDEX | CCA_TAB_STOP | CCA_TITLE;
                m_nIncludeSpecial = SCA_MULTIPLE;
                m_nIncludeDatabase = DA_BOUND_COLUMN | DA_DATA_FIELD | DA_LIST_SOURCE_TYPE;
                m_nIncludeEvents = EA_CONTROL_EVENTS | EA_ON_CHANGE | EA_ON_CLICK   | EA_ON_DBLCLICK;
                // check if we need to export the ListSource as attribute
                {
                    // for a list box, if the ListSourceType is VALUE_LIST, no ListSource is stored, but instead
                    // a sequence of pairs which is build from the StringItemList and the ValueList
                    ListSourceType eListSourceType = ListSourceType_VALUELIST;
                #ifdef DBG_UTIL
                    sal_Bool bSuccess =
                #endif
                    m_xProps->getPropertyValue(PROPERTY_LISTSOURCETYPE) >>= eListSourceType;
                    OSL_ENSURE(bSuccess, "OControlExport::examineControl: could not retrieve the ListSourceType!");
                    if (ListSourceType_VALUELIST != eListSourceType)
                    {
                        m_nIncludeDatabase |= DA_LIST_SOURCE;
                    }
                }

                break;

            case FormComponentType::COMMANDBUTTON:
                m_eType = BUTTON;
                m_nIncludeCommon |= CCA_TAB_STOP | CCA_LABEL;
                m_nIncludeSpecial = SCA_DEFAULT_BUTTON;
                // NO BREAK !
            case FormComponentType::IMAGEBUTTON:
                if (BUTTON != m_eType)
                    // not coming from the previous case
                    m_eType = IMAGE;
                m_nIncludeCommon |=
                    CCA_NAME | CCA_SERVICE_NAME | CCA_BUTTON_TYPE | CCA_DISABLED |
                    CCA_IMAGE_DATA | CCA_PRINTABLE | CCA_TAB_INDEX | CCA_TARGET_FRAME |
                    CCA_TARGET_LOCATION | CCA_TITLE;
                m_nIncludeEvents = EA_CONTROL_EVENTS | EA_ON_CLICK  | EA_ON_DBLCLICK;
                break;

            case FormComponentType::CHECKBOX:
                m_eType = CHECKBOX;
                m_nIncludeSpecial = SCA_CURRENT_STATE | SCA_IS_TRISTATE | SCA_STATE;
                // NO BREAK !
            case FormComponentType::RADIOBUTTON:
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED | CCA_LABEL |
                    CCA_PRINTABLE | CCA_TAB_INDEX | CCA_TAB_STOP | CCA_TITLE | CCA_VALUE;
                if (CHECKBOX != m_eType)
                {   // not coming from the previous case
                    m_eType = RADIO;
                    m_nIncludeCommon |= CCA_CURRENT_SELECTED | CCA_SELECTED;
                }
                m_nIncludeDatabase = DA_DATA_FIELD;
                m_nIncludeEvents = EA_CONTROL_EVENTS | EA_ON_CHANGE;
                break;

            case FormComponentType::GROUPBOX:
                m_eType = FRAME;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED | CCA_LABEL |
                    CCA_PRINTABLE | CCA_TITLE | CCA_FOR;
                m_nIncludeEvents = EA_CONTROL_EVENTS;
                break;

            case FormComponentType::IMAGECONTROL:
                m_eType = IMAGE_FRAME;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED | CCA_IMAGE_DATA |
                    CCA_PRINTABLE | CCA_READONLY | CCA_TITLE;
                m_nIncludeDatabase = DA_DATA_FIELD;
                m_nIncludeEvents = EA_CONTROL_EVENTS;
                break;

            case FormComponentType::HIDDENCONTROL:
                m_eType = HIDDEN;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_VALUE;
                break;

            case FormComponentType::GRIDCONTROL:
                m_eType = GRID;
                m_nIncludeCommon =
                    CCA_NAME | CCA_SERVICE_NAME | CCA_DISABLED | CCA_PRINTABLE |
                    CCA_TAB_INDEX | CCA_TAB_STOP | CCA_TITLE;
                m_nIncludeEvents = EA_CONTROL_EVENTS;
                break;

            case FormComponentType::CONTROL:
                m_eType = CONTROL;
                // unknown control type
                m_nIncludeCommon = CCA_NAME | CCA_SERVICE_NAME;
                    // at least a name should be there, 'cause without a name the control could never have been
                    // inserted into it's parent container
                    // In addition, the service name is absolutely necessary to create the control upon reading.
                m_nIncludeEvents = EA_CONTROL_EVENTS;
                    // we always should be able to export events - this is not control type dependent
                break;
            default:
                OSL_ENSHURE(sal_False, "OControlExport::examineControl: unknown control type (class id)!");
                break;
        }

        // in general, all control types need to export the control id
        m_nIncludeCommon |= CCA_CONTROL_ID;
    }

    //---------------------------------------------------------------------
    void OControlExport::getPropertyNames_ca(sal_Char const * & _rpCurrentValue,
            sal_Char const * & _rpValue)
    {
        // reset the pointers in case we can't determine the property names
        _rpCurrentValue = _rpValue = NULL;
        switch (m_nClassId)
        {
            case FormComponentType::TEXTFIELD:
                if (FORMATTED_TEXT == m_eType)
                {
                    _rpCurrentValue = PROPERTY_EFFECTIVE_VALUE;
                    _rpValue = PROPERTY_EFFECTIVE_DEFAULT;
                }
                else
                {
                    _rpCurrentValue = PROPERTY_TEXT;
                    _rpValue = PROPERTY_DEFAULT_TEXT;
                }
                break;
            case FormComponentType::DATEFIELD:
                _rpCurrentValue = PROPERTY_DATE;
                _rpValue = PROPERTY_DEFAULT_DATE;
                break;
            case FormComponentType::TIMEFIELD:
                _rpCurrentValue = PROPERTY_TIME;
                _rpValue = PROPERTY_DEFAULT_TIME;
                break;
            case FormComponentType::NUMERICFIELD:
            case FormComponentType::CURRENCYFIELD:
                _rpCurrentValue = PROPERTY_VALUE;
                _rpValue = PROPERTY_DEFAULT_VALUE;
                break;
            case FormComponentType::PATTERNFIELD:
            case FormComponentType::FILECONTROL:
            case FormComponentType::COMBOBOX:
                _rpValue = PROPERTY_DEFAULT_TEXT;
                // NO BREAK!!
            case FormComponentType::COMMANDBUTTON:
                _rpCurrentValue = PROPERTY_TEXT;
                break;
            case FormComponentType::CHECKBOX:
            case FormComponentType::RADIOBUTTON:
                _rpValue = PROPERTY_REFVALUE;
                break;
            case FormComponentType::HIDDENCONTROL:
                _rpValue = PROPERTY_VALUE;
                break;
        }
    }

    //=====================================================================
    //= OColumnExport
    //=====================================================================
    //---------------------------------------------------------------------
    OColumnExport::OColumnExport(SvXMLExport& _rContext, const Reference< XPropertySet >& _rxControl)
        :OControlExport(_rContext, NULL, _rxControl, ::rtl::OUString(), ::rtl::OUString())
        ,m_pColumnXMLElement(NULL)
    {
    }

    //---------------------------------------------------------------------
    OColumnExport::~OColumnExport()
    {
        // delete m_pXMLElement before m_pColumnXMLElement !!
            // TODO: this makes me seriously thinking about why OColumnExport is derived from OControlExport, and
            // not the other way round
        delete m_pXMLElement;
        m_pXMLElement = NULL;

        delete m_pColumnXMLElement;
    }

    //---------------------------------------------------------------------
    void OColumnExport::examine()
    {
        OControlExport::examine();

        // grid columns miss some properties of the controls they're representing
        m_nIncludeCommon &= ~(CCA_SERVICE_NAME | CCA_CONTROL_ID | CCA_FOR | CCA_PRINTABLE | CCA_TAB_INDEX | CCA_TAB_STOP | CCA_LABEL | CCA_NAME);
        m_nIncludeSpecial &= ~(SCA_ECHO_CHAR | SCA_AUTOMATIC_COMPLETION | SCA_MULTIPLE | SCA_MULTI_LINE | SCA_IS_TRISTATE);

        if (FormComponentType::DATEFIELD != m_nClassId)
            // except date fields, no column has the DropDown property
            m_nIncludeCommon &= ~CCA_DROPDOWN;
    }

    //---------------------------------------------------------------------
    void OColumnExport::startExportElement()
    {
        // before the base class can start it's element, start an additional one stating that the following is
        // a grid column

        // the attributes:
        m_rContext.ClearAttrList();

        // the attribute "name"
        exportStringPropertyAttribute(
            getCommonControlAttributeNamespace(CCA_NAME),
            getCommonControlAttributeName(CCA_NAME),
            PROPERTY_NAME
            );

        // the attribute "service name" (which has a slightly different meaning for columns
        DBG_CHECK_PROPERTY((const sal_Char*)PROPERTY_COLUMNSERVICENAME, ::rtl::OUString);
        exportStringPropertyAttribute(
            getCommonControlAttributeNamespace(CCA_SERVICE_NAME),
            getCommonControlAttributeName(CCA_SERVICE_NAME),
            PROPERTY_COLUMNSERVICENAME
            );

        // the attribute "label"
        exportStringPropertyAttribute(
            getCommonControlAttributeNamespace(CCA_LABEL),
            getCommonControlAttributeName(CCA_LABEL),
            PROPERTY_LABEL);

        // start the extra element indicating that we're a column
        m_pColumnXMLElement = new SvXMLElementExport(m_rContext, XML_NAMESPACE_FORM, "column", sal_True, sal_True);

        // let the base class do it's handling
        OControlExport::startExportElement();
    }

    //=====================================================================
    //= OFormExport
    //=====================================================================
    //---------------------------------------------------------------------
    OFormExport::OFormExport(SvXMLExport& _rContext, IExportImplementation* _pCallback, const Reference< XPropertySet >& _rxForm)
        :OPropertyExport(_rContext, _rxForm)
        ,m_pCallback(_pCallback)
        ,m_pXMLElement(NULL)
    {
        OSL_ENSURE(m_xProps.is() && m_pCallback, "OFormExport::OFormExport: invalid arguments!");

        // add the attributes
        m_rContext.ClearAttrList();
        exportAttributes();

        // start the form element
        m_pXMLElement = new SvXMLElementExport(m_rContext, XML_NAMESPACE_FORM, "form", sal_True, sal_True);

        // the properties which where not exported 'til now
        exportRemainingProperties();

        // TODO: the remaining scripts

        // loop through all children
        Reference< XIndexAccess > xCollection(m_xProps, UNO_QUERY);
        OSL_ENSURE(xCollection.is(), "OFormLayerXMLExport::implExportForm: a form which is not an index access? Suspic�ous!");

        if (xCollection.is() && m_pCallback)
            m_pCallback->exportCollectionElements(xCollection);
    }

    //---------------------------------------------------------------------
    OFormExport::~OFormExport()
    {
        delete m_pXMLElement;
            // if we already started the element, this will end it, if not, nothing will happen
    }

    //---------------------------------------------------------------------
    void OFormExport::exportAttributes()
    {
        sal_Int32 i=0;

        // ---------------------
        // the string properties
        {
            static FormAttributes eStringPropertyIds[] =
            {
                faName, faAction, faCommand, faDatasource, faFilter, faOrder
            };
            static const sal_Char* pStringPropertyNames[] =
            {
                PROPERTY_NAME, PROPERTY_TARGETURL, PROPERTY_COMMAND, PROPERTY_DATASOURCENAME, PROPERTY_FILTER, PROPERTY_ORDER
            };
            sal_Int32 nIdCount = sizeof(eStringPropertyIds) / sizeof(eStringPropertyIds[0]);
        #ifdef DBG_UTIL
            sal_Int32 nNameCount = sizeof(pStringPropertyNames) / sizeof(pStringPropertyNames[0]);
            OSL_ENSURE((nIdCount == nNameCount),
                "OFormExport::exportAttributes: somebody tampered with the maps (1)!");
        #endif
            for (i=0; i<nIdCount; ++i)
                exportStringPropertyAttribute(
                    getFormAttributeNamespace(eStringPropertyIds[i]),
                    getFormAttributeName(eStringPropertyIds[i]),
                    pStringPropertyNames[i]);
        }

        // ----------------------
        // the boolean properties
        {
            static FormAttributes eBooleanPropertyIds[] =
            {
                faAllowDeletes, faAllowInserts, faAllowUpdates, faApplyFilter, faEscapeProcessing, faIgnoreResult
            };
            static const sal_Char* pBooleanPropertyNames[] =
            {
                PROPERTY_ALLOWDELETES, PROPERTY_ALLOWINSERTS, PROPERTY_ALLOWUPDATES, PROPERTY_APPLYFILTER, PROPERTY_ESCAPEPROCESSING, PROPERTY_IGNORERESULT
            };
            static sal_Bool bBooleanPropertyAttrDefaults[] =
            {
                sal_True, sal_True, sal_True, sal_False, sal_True, sal_False
            };
            sal_Int32 nIdCount = sizeof(eBooleanPropertyIds) / sizeof(eBooleanPropertyIds[0]);
        #ifdef DBG_UTIL
            sal_Int32 nNameCount = sizeof(pBooleanPropertyNames) / sizeof(pBooleanPropertyNames[0]);
            sal_Int32 nDefaultCount = sizeof(bBooleanPropertyAttrDefaults) / sizeof(bBooleanPropertyAttrDefaults[0]);
            OSL_ENSURE((nIdCount == nNameCount) && (nNameCount == nDefaultCount),
                "OFormExport::exportAttributes: somebody tampered with the maps (2)!");
        #endif
            for (i=0; i<nIdCount; ++i)
                exportBooleanPropertyAttribute(
                    getFormAttributeNamespace(eBooleanPropertyIds[i]),
                    getFormAttributeName(eBooleanPropertyIds[i]),
                    pBooleanPropertyNames[i],
                    bBooleanPropertyAttrDefaults[i],
                    sal_False);
        }

        // -------------------
        // the enum properties
        {
            static FormAttributes eEnumPropertyIds[] =
            {
                faEnctype, faMethod, faCommandType, faNavigationMode, faTabbingCycle
            };
            static const sal_Char* pEnumPropertyNames[] =
            {
                PROPERTY_SUBMIT_ENCODING, PROPERTY_SUBMIT_METHOD, PROPERTY_COMMAND_TYPE, PROPERTY_NAVIGATION, PROPERTY_CYCLE
            };
            static OEnumMapper::EnumProperties eEnumPropertyMaps[] =
            {
                OEnumMapper::epSubmitEncoding, OEnumMapper::epSubmitMethod, OEnumMapper::epCommandType, OEnumMapper::epNavigationType, OEnumMapper::epTabCyle
            };
            static sal_Int32 nEnumPropertyAttrDefaults[] =
            {
                FormSubmitEncoding_URL, FormSubmitMethod_GET, CommandType::COMMAND, NavigationBarMode_NONE, TabulatorCycle_RECORDS
            };
            sal_Int32 nIdCount = sizeof(eEnumPropertyIds) / sizeof(eEnumPropertyIds[0]);
        #ifdef DBG_UTIL
            sal_Int32 nNameCount = sizeof(pEnumPropertyNames) / sizeof(pEnumPropertyNames[0]);
            sal_Int32 nDefaultCount = sizeof(nEnumPropertyAttrDefaults) / sizeof(nEnumPropertyAttrDefaults[0]);
            sal_Int32 nMapCount = sizeof(eEnumPropertyMaps) / sizeof(eEnumPropertyMaps[0]);
            OSL_ENSURE((nIdCount == nNameCount) && (nNameCount == nDefaultCount) && (nDefaultCount == nMapCount),
                "OFormExport::exportAttributes: somebody tampered with the maps (3)!");
        #endif
            for (i=0; i<nIdCount; ++i)
                exportEnumPropertyAttribute(
                    getFormAttributeNamespace(eEnumPropertyIds[i]),
                    getFormAttributeName(eEnumPropertyIds[i]),
                    pEnumPropertyNames[i],
                    OEnumMapper::getEnumMap(eEnumPropertyMaps[i]),
                    nEnumPropertyAttrDefaults[i]);
        }

        // the service name
        exportServiceNameAttribute();
        // the target frame
        exportTargetFrameAttribute();

        // master fields
        exportStringSequenceAttribute(
            getFormAttributeNamespace(faMasterFields),
            getFormAttributeName(faMasterFields),
            PROPERTY_MASTERFIELDS);
        // detail fields
        exportStringSequenceAttribute(
            getFormAttributeNamespace(faDetailFiels),
            getFormAttributeName(faDetailFiels),
            PROPERTY_DETAILFIELDS);


        // TODO: the events EA_RESET, EA_SUBMIT
    }

//.........................................................................
}   // namespace xmloff
//.........................................................................

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2000/11/17 19:01:21  fs
 *  initial checkin - export and/or import the applications form layer
 *
 *
 *  Revision 1.0 13.11.00 18:56:13  fs
 ************************************************************************/

