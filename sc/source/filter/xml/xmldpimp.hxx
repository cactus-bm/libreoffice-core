/*************************************************************************
 *
 *  $RCSfile: xmldpimp.hxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-29 12:53:55 $
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
#ifndef SC_XMLDPIMP_HXX
#define SC_XMLDPIMP_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <xmloff/xmlictxt.hxx>
#endif
#ifndef _XMLOFF_XMLIMP_HXX
#include <xmloff/xmlimp.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDREFERENCE_HPP_
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDSORTINFO_HPP_
#include <com/sun/star/sheet/DataPilotFieldSortInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDAUTOSHOWINFO_HPP_
#include <com/sun/star/sheet/DataPilotFieldAutoShowInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAPILOTFIELDLAYOUTINFO_HPP_
#include <com/sun/star/sheet/DataPilotFieldLayoutInfo.hpp>
#endif

#include "global.hxx"
#include "dpobject.hxx"
#include "dpsave.hxx"

class ScXMLImport;
class ScDPSaveNumGroupDimension;
class ScDPSaveGroupDimension;

enum ScMySourceType
{
    SQL,
    TABLE,
    QUERY,
    SERVICE,
    CELLRANGE
};

class ScXMLDataPilotTablesContext : public SvXMLImportContext
{

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotTablesContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList);

    virtual ~ScXMLDataPilotTablesContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDataPilotTableContext : public SvXMLImportContext
{
    ScDocument*     pDoc;
    ScDPObject*     pDPObject;
    ScDPSaveData*   pDPSave;
    ScDPDimensionSaveData* pDPDimSaveData;
    rtl::OUString   sDataPilotTableName;
    rtl::OUString   sApplicationData;
    rtl::OUString   sGrandTotal;
    rtl::OUString   sDatabaseName;
    rtl::OUString   sSourceObject;
    rtl::OUString   sServiceName;
    rtl::OUString   sServiceSourceName;
    rtl::OUString   sServiceSourceObject;
    rtl::OUString   sServiceUsername;
    rtl::OUString   sServicePassword;
    rtl::OUString   sButtons;
    ScRange         aSourceCellRangeAddress;
    ScRange         aTargetRangeAddress;
    ScRange         aFilterSourceRange;
    ScAddress       aFilterOutputPosition;
    ScQueryParam    aSourceQueryParam;
    ScMySourceType  nSourceType;
    sal_Bool        bIsNative;
    sal_Bool        bIgnoreEmptyRows;
    sal_Bool        bIdentifyCategories;
    sal_Bool        bUseRegularExpression;
    sal_Bool        bIsCaseSensitive;
    sal_Bool        bSkipDuplicates;
    sal_Bool        bFilterCopyOutputData;
    sal_Bool        bTargetRangeAddress;
    sal_Bool        bSourceCellRange;
    sal_Bool        bShowFilter;
    sal_Bool        bDrillDown;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotTableContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList);

    virtual ~ScXMLDataPilotTableContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();

    void SetDatabaseName(const rtl::OUString& sValue) { sDatabaseName = sValue; }
    void SetSourceObject(const rtl::OUString& sValue) { sSourceObject = sValue; }
    void SetNative(const sal_Bool bValue) { bIsNative = bValue; }
    void SetServiceName(const rtl::OUString& sValue) { sServiceName = sValue; }
    void SetServiceSourceName(const rtl::OUString& sValue) { sServiceSourceName = sValue; }
    void SetServiceSourceObject(const rtl::OUString& sValue) { sServiceSourceObject = sValue; }
    void SetServiceUsername(const rtl::OUString& sValue) { sServiceUsername = sValue; }
    void SetServicePassword(const rtl::OUString& sValue) { sServicePassword = sValue; }
    void SetSourceCellRangeAddress(const ScRange& aValue) { aSourceCellRangeAddress = aValue; bSourceCellRange = sal_True; }
    void SetSourceQueryParam(const ScQueryParam& aValue) { aSourceQueryParam = aValue; }
//  void SetFilterUseRegularExpressions(const sal_Bool bValue) { aSourceQueryParam.bRegExp = bValue; }
    void SetFilterOutputPosition(const ScAddress& aValue) { aFilterOutputPosition = aValue; }
    void SetFilterCopyOutputData(const sal_Bool bValue) { bFilterCopyOutputData = bValue; }
    void SetFilterSourceRange(const ScRange& aValue) { aFilterSourceRange = aValue; }
//  void SetFilterIsCaseSensitive(const sal_Bool bValue) { aSourceQueryParam.bCaseSens = bValue; }
//  void SetFilterSkipDuplicates(const sal_Bool bValue) { aSourceQueryParam.bDuplicate = !bValue; }
    void AddDimension(ScDPSaveDimension* pDim);
    void AddGroupDim(const ScDPSaveNumGroupDimension& aNumGroupDim);
    void AddGroupDim(const ScDPSaveGroupDimension& aGroupDim);
    void SetButtons();
};

class ScXMLDPSourceSQLContext : public SvXMLImportContext
{
    ScXMLDataPilotTableContext* pDataPilotTable;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDPSourceSQLContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotTableContext* pDataPilotTable);

    virtual ~ScXMLDPSourceSQLContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDPSourceTableContext : public SvXMLImportContext
{
    ScXMLDataPilotTableContext* pDataPilotTable;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDPSourceTableContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotTableContext* pDataPilotTable);

    virtual ~ScXMLDPSourceTableContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDPSourceQueryContext : public SvXMLImportContext
{
    ScXMLDataPilotTableContext* pDataPilotTable;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDPSourceQueryContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotTableContext* pDataPilotTable);

    virtual ~ScXMLDPSourceQueryContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLSourceServiceContext : public SvXMLImportContext
{
    ScXMLDataPilotTableContext* pDataPilotTable;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLSourceServiceContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotTableContext* pDataPilotTable);

    virtual ~ScXMLSourceServiceContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLSourceCellRangeContext : public SvXMLImportContext
{
    ScXMLDataPilotTableContext* pDataPilotTable;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLSourceCellRangeContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotTableContext* pDataPilotTable);

    virtual ~ScXMLSourceCellRangeContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

struct ScXMLDataPilotGroup
{
    ::std::vector<rtl::OUString> aMembers;
    rtl::OUString aName;
};

class ScXMLDataPilotFieldContext : public SvXMLImportContext
{
    ScXMLDataPilotTableContext* pDataPilotTable;
    ScDPSaveDimension*          pDim;

    ::std::vector<ScXMLDataPilotGroup> aGroups;
    rtl::OUString               sGroupSource;
    rtl::OUString               sSelectedPage;
    rtl::OUString               sName;
    double                      fStart;
    double                      fEnd;
    double                      fStep;
    sal_Int32                   nUsedHierarchy;
    sal_Int32                   nGroupPart;
    sal_Int16                   nFunction;
    sal_Int16                   nOrientation;
    sal_Bool                    bShowEmpty;
    sal_Bool                    bSelectedPage;
    sal_Bool                    bIsGroupField;
    sal_Bool                    bDateValue;
    sal_Bool                    bAutoStart;
    sal_Bool                    bAutoEnd;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotFieldContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotTableContext* pDataPilotTable);

    virtual ~ScXMLDataPilotFieldContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();

    void SetShowEmpty(const sal_Bool bValue) { if (pDim) pDim->SetShowEmpty(bValue); }
    void SetSubTotals(const sal_uInt16* pFunctions, const sal_Int16 nCount) { if(pDim) pDim->SetSubTotals(nCount, pFunctions); }
    void AddMember(ScDPSaveMember* pMember) { if (pDim) pDim->AddMember(pMember); }
    void SetFieldReference(const com::sun::star::sheet::DataPilotFieldReference& aRef) { if (pDim) pDim->SetReferenceValue(&aRef); }
    void SetAutoShowInfo(const com::sun::star::sheet::DataPilotFieldAutoShowInfo& aInfo) { if (pDim) pDim->SetAutoShowInfo(&aInfo); }
    void SetSortInfo(const com::sun::star::sheet::DataPilotFieldSortInfo& aInfo) { if (pDim) pDim->SetSortInfo(&aInfo); }
    void SetLayoutInfo(const com::sun::star::sheet::DataPilotFieldLayoutInfo& aInfo) { if (pDim) pDim->SetLayoutInfo(&aInfo); }
    void SetGrouping(const rtl::OUString& rGroupSource, const double& rStart, const double& rEnd, const double& rStep,
        sal_Int32 nPart, sal_Bool bDate, sal_Bool bAutoSt, sal_Bool bAutoE)
    {
        bIsGroupField = sal_True;
        sGroupSource = rGroupSource;
        fStart = rStart;
        fEnd = rEnd;
        fStep = rStep;
        nGroupPart = nPart;
        bDateValue = bDate;
        bAutoStart = bAutoSt;
        bAutoEnd = bAutoE;
    }
    void AddGroup(const ::std::vector<rtl::OUString>& rMembers, const rtl::OUString& rName);
};

class ScXMLDataPilotFieldReferenceContext : public SvXMLImportContext
{
    com::sun::star::sheet::DataPilotFieldReference aReference;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotFieldReferenceContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotFieldReferenceContext();
};

class ScXMLDataPilotLevelContext : public SvXMLImportContext
{
    ScXMLDataPilotFieldContext* pDataPilotField;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotLevelContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotLevelContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDataPilotDisplayInfoContext : public SvXMLImportContext
{
    com::sun::star::sheet::DataPilotFieldAutoShowInfo aInfo;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotDisplayInfoContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotDisplayInfoContext();
};

class ScXMLDataPilotSortInfoContext : public SvXMLImportContext
{
    com::sun::star::sheet::DataPilotFieldSortInfo aInfo;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotSortInfoContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotSortInfoContext();
};

class ScXMLDataPilotLayoutInfoContext : public SvXMLImportContext
{
    com::sun::star::sheet::DataPilotFieldLayoutInfo aInfo;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotLayoutInfoContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotLayoutInfoContext();
};

class ScXMLDataPilotSubTotalsContext : public SvXMLImportContext
{
    ScXMLDataPilotFieldContext* pDataPilotField;

    sal_Int16   nFunctionCount;
    sal_uInt16* pFunctions;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotFieldContext* GetDataPilotField() { return pDataPilotField; }

    ScXMLDataPilotSubTotalsContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotSubTotalsContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
    void AddFunction(sal_Int16 nFunction);
};

class ScXMLDataPilotSubTotalContext : public SvXMLImportContext
{
    ScXMLDataPilotSubTotalsContext* pDataPilotSubTotals;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotSubTotalContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotSubTotalsContext* pDataPilotSubTotals);

    virtual ~ScXMLDataPilotSubTotalContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDataPilotMembersContext : public SvXMLImportContext
{
    ScXMLDataPilotFieldContext* pDataPilotField;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotMembersContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotMembersContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDataPilotMemberContext : public SvXMLImportContext
{
    ScXMLDataPilotFieldContext* pDataPilotField;

    rtl::OUString sName;
    sal_Bool    bDisplay;
    sal_Bool    bDisplayDetails;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotMemberContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotMemberContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDataPilotGroupsContext : public SvXMLImportContext
{
    ScXMLDataPilotFieldContext* pDataPilotField;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotGroupsContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotGroupsContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

class ScXMLDataPilotGroupContext : public SvXMLImportContext
{
    ScXMLDataPilotFieldContext* pDataPilotField;

    rtl::OUString sName;
    ::std::vector<rtl::OUString> aMembers;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotGroupContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotFieldContext* pDataPilotField);

    virtual ~ScXMLDataPilotGroupContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();

    void AddMember(const rtl::OUString& sMember) { aMembers.push_back(sMember); }
};

class ScXMLDataPilotGroupMemberContext : public SvXMLImportContext
{
    ScXMLDataPilotGroupContext* pDataPilotGroup;

    rtl::OUString sName;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

    ScXMLDataPilotGroupMemberContext( ScXMLImport& rImport, USHORT nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ScXMLDataPilotGroupContext* pDataPilotGroup);

    virtual ~ScXMLDataPilotGroupMemberContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

#endif

