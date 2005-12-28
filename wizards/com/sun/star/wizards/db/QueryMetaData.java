/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: QueryMetaData.java,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2005-12-28 17:17:08 $
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
 ************************************************************************/package com.sun.star.wizards.db;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.beans.PropertyValue;
import com.sun.star.container.XNameAccess;

import java.util.*;
import com.sun.star.lang.Locale;
import com.sun.star.wizards.common.*;

public class QueryMetaData extends CommandMetaData {

    public Vector QueryFields;
    FieldColumn CurFieldColumn;
    public String Command;
    Vector CommandNames;
    public PropertyValue[][] FilterConditions = new PropertyValue[][] {};
    public PropertyValue[][] GroupByFilterConditions = new PropertyValue[][] {};
    public String[] FieldTitles = new String[] {};
    public String[] UniqueAggregateFieldNames = new String[]{};
    public int Type = QueryType.SODETAILQUERY;

    public static interface QueryType {
        final static int SOSUMMARYQUERY = 0;
        final static int SODETAILQUERY = 1;

    }

    public QueryMetaData(XMultiServiceFactory xMSF, Locale CharLocale, NumberFormatter oNumberFormatter) {
        super(xMSF, CharLocale, oNumberFormatter);
        QueryFields = new Vector(0);
    }

    public QueryMetaData(XMultiServiceFactory _xMSF) {
        super(_xMSF);
        QueryFields = new Vector(0);
        //      FieldColumn(CommandMetaData oCommandMetaData, String FieldName, boolean bgetDefaultValue){
    }

    public void setFilterConditions(PropertyValue[][] _FilterConditions) {
        this.FilterConditions = _FilterConditions;
    }

    public PropertyValue[][] getFilterConditions() {
        return this.FilterConditions;
    }

    public void setGroupByFilterConditions(PropertyValue[][] _GroupByFilterConditions) {
        this.GroupByFilterConditions = _GroupByFilterConditions;
    }

    public PropertyValue[][] getGroupByFilterConditions() {
        return this.GroupByFilterConditions;
    }

    public Vector getQueryFields() {
        return QueryFields;
    }

    void removeQueryField() {

    }


    public void setFieldNames(String[] _FieldNames, XNameAccess _xColumns) {
        int FieldCount = _FieldNames.length;
        FieldNames = new String[FieldCount];
        //      FieldTitles = new String[FieldCount];
        QueryFields.removeAllElements();
        for (int i = 0; i < FieldCount; i++) {
            CurFieldColumn = new FieldColumn(this,_xColumns, _FieldNames[i] );
            QueryFields.add(QueryFields.size(), CurFieldColumn);
            FieldNames[i] = _FieldNames[i];
        }
    }


    public void setFieldNames(String[] _FieldNames){ //, String _CommandName) {
        int FieldCount = _FieldNames.length;
        FieldNames = new String[FieldCount];
        //      FieldTitles = new String[FieldCount];
        QueryFields.removeAllElements();
        for (int i = 0; i < FieldCount; i++) {
            CurFieldColumn = new FieldColumn(this, _FieldNames[i]); //, _CommandName);
            QueryFields.add(QueryFields.size(), CurFieldColumn);
            FieldNames[i] = _FieldNames[i];
            if (FieldTitleSet != null){
                if (FieldTitleSet.containsKey(FieldNames[i])){
                    CurFieldColumn.AliasName = (String) FieldTitleSet.get(FieldNames[i]);
                    if (CurFieldColumn.AliasName == null){
                        CurFieldColumn.AliasName = _FieldNames[i];
                        FieldTitleSet.put(FieldNames[i], _FieldNames[i]);
                    }

                }
            }
        }
    }

    public String[] getIncludedCommandNames() {
        FieldColumn CurQueryField;
        CommandNames = new Vector(1);
        String CurCommandName;
        int SearchIndex;
        for (int i = 0; i < QueryFields.size(); i++) {
            CurQueryField = (FieldColumn) QueryFields.elementAt(i);
            CurCommandName = CurQueryField.getCommandName();
            if (!CommandNames.contains(CurCommandName))
                CommandNames.addElement(CurCommandName);
        }
        String[] sIncludedCommandNames = new String[CommandNames.size()];
        CommandNames.toArray(sIncludedCommandNames);
        return sIncludedCommandNames;
    }


    public static String[] getIncludedCommandNames(String[] _FieldNames) {
        FieldColumn CurQueryField;
        Vector CommandNames = new Vector(1);
        int SearchIndex;
        for (int i = 0; i < _FieldNames.length; i++) {
            String CurCommandName = "";
            String[] MetaList = JavaTools.ArrayoutofString(_FieldNames[i], ".");
            if (MetaList.length > 1) {
                for (int a = 0; a < MetaList.length - 1; a++)
                    CurCommandName += MetaList[a];
                if (!CommandNames.contains(CurCommandName))
                    CommandNames.addElement(CurCommandName);
            }
        }
        String[] sIncludedCommandNames = new String[CommandNames.size()];
        CommandNames.toArray(sIncludedCommandNames);
        return sIncludedCommandNames;
    }

    public String[] getAllFieldNames() {
        return AllFieldNames;
    }

    public void setAllIncludedFieldNames(boolean _bAppendMode) {
        try {
            this.getIncludedCommandNames();
            if (FieldTitleSet == null)
                FieldTitleSet = new HashMap();
            for (int i = 0; i < CommandNames.size(); i++) {
                CommandObject otable = getTableByName((String) CommandNames.elementAt(i));
                String[] LocFieldNames = otable.xColumns.getElementNames();
                for (int a = 0; a < LocFieldNames.length; a++) {
                    String CurFieldDisplayString = "";
                    if (_bAppendMode)
                        CurFieldDisplayString = ((String) CommandNames.elementAt(i)) + "." + LocFieldNames[a];
                    else
                        CurFieldDisplayString = LocFieldNames[a];
                    if (!FieldTitleSet.containsKey(CurFieldDisplayString))
                        FieldTitleSet.put(CurFieldDisplayString, null);
                }
            }
            FieldTitleSet.keySet().toArray(this.AllFieldNames);
        } catch (Exception exception) {
            exception.printStackTrace(System.out);
        }
    }


    public FieldColumn getFieldColumnByDisplayName(String _DisplayFieldName) {
        FieldColumn CurFieldColumn;
        for (int i = 0; i < QueryFields.size(); i++) {
            CurFieldColumn = (FieldColumn) QueryFields.elementAt(i);
            if (CurFieldColumn.DisplayFieldName.equals(_DisplayFieldName))
                return CurFieldColumn;
        }
        return null;
    }


    public String[] getUniqueAggregateFieldNames(){
        Vector UniqueAggregateFieldVector = new Vector(0);
        for (int i = 0; i < AggregateFieldNames.length; i++) {
            if (!UniqueAggregateFieldVector.contains(AggregateFieldNames[i][0]))
                UniqueAggregateFieldVector.add(AggregateFieldNames[i][0]);
        }
        UniqueAggregateFieldNames = new String[UniqueAggregateFieldVector.size()];
        UniqueAggregateFieldVector.toArray(UniqueAggregateFieldNames);
        return UniqueAggregateFieldNames;
    }


    public boolean hasNumericalFields() {
        FieldColumn CurQueryField = null;
        for (int i = 0; i < QueryFields.size(); i++) {
            CurQueryField = (FieldColumn) QueryFields.elementAt(i);
            if (CurQueryField.bIsNumberFormat)
                return true;
        }
        return false;
    }

    public void setfieldtitles() {
        FieldTitles = new String[FieldNames.length];
        for (int i = 0; i < FieldNames.length; i++) {
            if (this.FieldTitleSet.containsKey(FieldNames[i])) {
                String curvalue = (String) this.FieldTitleSet.get(FieldNames[i]);
                if ((curvalue == null) || (curvalue.equals(FieldNames[i])))
                    curvalue = FieldNames[i];
                else {
                    CurFieldColumn = getFieldColumnByDisplayName(FieldNames[i]);
                    CurFieldColumn.AliasName = (String) curvalue;
                }
                FieldTitles[i] = curvalue;
            }
        }
    }


    public int getAggregateIndex(String _FieldName){
        int iAggregate = -1;
        if (Type == QueryType.SOSUMMARYQUERY)
            iAggregate = JavaTools.FieldInTable(AggregateFieldNames, _FieldName);
        return iAggregate;
    }


    public String getFieldName(String _FieldTitle) {
        int NameIndex = JavaTools.FieldInList(this.FieldTitles, _FieldTitle);
        if (NameIndex > -1)
            return FieldNames[NameIndex];
        else
            return null;
    }

}
