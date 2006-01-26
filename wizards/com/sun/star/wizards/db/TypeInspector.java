/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TypeInspector.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: hr $ $Date: 2006-01-26 17:19:31 $
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
package com.sun.star.wizards.db;

import java.util.Vector;

import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.sdbc.ColumnSearch;
import com.sun.star.sdbc.ColumnValue;
import com.sun.star.sdbc.DataType;
import com.sun.star.sdbc.SQLException;
import com.sun.star.sdbc.XResultSet;
import com.sun.star.sdbc.XRow;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.wizards.common.JavaTools;


public class TypeInspector{
    private String[] sDataTypeNames;
    private int[] nDataTypeInfos;
    private int[] nPrecisionInfos;
    private int[] nNullableInfos;
    private int[] nSearchables;
    private boolean[] bisAutoIncrementableInfos;
    private int[] nMinScaleInfos;
    private int[] nMaxScaleInfos;
    private int[] nNumericFallBackList  = new int[]{DataType.INTEGER, DataType.FLOAT, DataType.REAL, DataType.DOUBLE, DataType.NUMERIC, DataType.DECIMAL};
    final int INVALID = 999999;
    XResultSet xResultSet;

    public class TypeInfo{
        public int nDataType;
        public String sDataTypeName;
        public boolean bisAutoIncrementable;

        public TypeInfo(int _nDataType, String _sDataTypeName, boolean _bisAutoIncrementable){
            nDataType = _nDataType;
            sDataTypeName = _sDataTypeName;
            bisAutoIncrementable = _bisAutoIncrementable;
        }
    }

    public TypeInspector(XResultSet _xResultSet){
    try {
        xResultSet = _xResultSet;
        Vector aTypeNameVector = new Vector();
        Vector aTypeVector = new Vector();
        Vector aNullableVector = new Vector();
        Vector aAutoIncrementVector = new Vector();
        Vector aPrecisionVector = new Vector();
        Vector aMinScaleVector = new Vector();
        Vector aMaxScaleVector = new Vector();
        Vector aSearchableVector = new Vector();
        Integer[] aIntegerDataTypes = null;
//      XResultSet xResultSet = xDBMetaDagetTypeInfo();
        XRow xRow = (XRow) UnoRuntime.queryInterface(XRow.class, xResultSet);
        while (xResultSet.next()){
            aTypeNameVector.addElement(new String(xRow.getString(1)));
            aTypeVector.addElement(new Integer(xRow.getShort(2)));
            aPrecisionVector.addElement(new Integer(xRow.getInt(3)));
            aNullableVector.addElement(new Integer(xRow.getShort(7)));
            aSearchableVector.addElement(new Integer(xRow.getShort(9)));
            aAutoIncrementVector.addElement(new Boolean(xRow.getBoolean(12)));
            aMinScaleVector.addElement(new Integer(xRow.getShort(14)));
            aMaxScaleVector.addElement(new Integer(xRow.getShort(15)));

        }
        sDataTypeNames = new String[aTypeNameVector.size()];
        aTypeNameVector.toArray(sDataTypeNames);
        nDataTypeInfos = JavaTools.IntegerTointList(aTypeVector);
        nNullableInfos = JavaTools.IntegerTointList(aNullableVector);
        nSearchables = JavaTools.IntegerTointList(aSearchableVector);
        bisAutoIncrementableInfos = JavaTools.BooleanTobooleanList(aAutoIncrementVector);
        nPrecisionInfos = JavaTools.IntegerTointList(aPrecisionVector);
        nMinScaleInfos = JavaTools.IntegerTointList(aMinScaleVector);
        nMaxScaleInfos = JavaTools.IntegerTointList(aMaxScaleVector);
    } catch (SQLException e) {
        e.printStackTrace(System.out);
    }}


    public int getScale(XPropertySet _xColPropertySet){
    try {
        int i = getDataTypeIndex(_xColPropertySet, false);
        int nScale = AnyConverter.toInt(_xColPropertySet.getPropertyValue("Scale"));
        if (i == -1)
            return nScale;
        if (nScale > nMaxScaleInfos[i])
            return nMaxScaleInfos[i];
        else if (nScale < nMinScaleInfos[i])
            return nMinScaleInfos[i];
        else
            return nScale;
    } catch (Exception e) {
        e.printStackTrace(System.out);
        return 0;
    }}


    public int getNullability(XPropertySet _xColPropertySet, int _nNullable){
        int i = getDataTypeIndex(_xColPropertySet, false);
        if (i == -1)
            return ColumnValue.NO_NULLS;
        int nNullable = _nNullable;
        if (nNullable == ColumnValue.NULLABLE)
            return nNullableInfos[i];           //probably nullability is not allowed
        return nNullable;
    }


    public int getNullability(XPropertySet _xColPropertySet){
        try {
            int i = getDataTypeIndex(_xColPropertySet, false);
            if (i == -1)
                return ColumnValue.NO_NULLS;
            int nNullable = AnyConverter.toInt(_xColPropertySet.getPropertyValue("IsNullable"));
            if (nNullable == ColumnValue.NULLABLE)
                return nNullableInfos[i];
            return nNullable;
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        return ColumnValue.NO_NULLS;
    }


    public boolean isColumnOrderable(XPropertySet _xColPropertySet){
        int i = getDataTypeIndex(_xColPropertySet, false);
        if (i > -1)
            return (nSearchables[i] != ColumnSearch.NONE);
        else
            return false;
    }


    public int isNullable(XPropertySet _xColPropertySet){
        int i = getDataTypeIndex(_xColPropertySet, false);
        if (i > -1)
            return nNullableInfos[i];
        else
            return ColumnValue.NO_NULLS;
    }


    private int getDataTypeIndex(XPropertySet _xColPropertySet, boolean _bCheckNumericAttributes){
        try {
            int nPrecision = -1;
            int nScale = -1;
            int nDataType = AnyConverter.toInt(_xColPropertySet.getPropertyValue("Type"));
            String sTypeName = AnyConverter.toString(_xColPropertySet.getPropertyValue("TypeName"));
            if (_bCheckNumericAttributes){
                nPrecision = AnyConverter.toInt(_xColPropertySet.getPropertyValue("Precision"));
                nScale = AnyConverter.toInt(_xColPropertySet.getPropertyValue("Scale"));
            }
            boolean bleaveloop = false;
            int startindex = 0;
            while (!bleaveloop){
                int i = JavaTools.FieldInIntTable(nDataTypeInfos, nDataType, startindex);
                startindex = i+1;
                bleaveloop = (i < 0);
                if (!bleaveloop){
                    if (sTypeName.equals(sDataTypeNames[i])){
                        if (_bCheckNumericAttributes){
                            if (nPrecision <= nPrecisionInfos[i]){
                                if ((nScale >= nMinScaleInfos[i]) && (nScale <= nMinScaleInfos[i])){
                                    return i;
                                }
                            }
                        }
                        else
                            return i;
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        return -1;
    }


    public boolean supportsDataType(int _curDataType){
        return (JavaTools.FieldInIntTable(nDataTypeInfos, _curDataType)> -1);
    }


    public int getLastConversionFallbackDataType(){
        if (supportsDataType(DataType.VARCHAR))
            return DataType.VARCHAR;
        else
            return DataType.LONGVARCHAR;
    }


    /**
     * an empty string is returned when no appropriate Typename can be found
     * finds the first TypeName of the passed datatype.
     * @param _curDataType
     * @return
     */
    public String getDefaultTypeName(int _curDataType){
        int i = JavaTools.FieldInIntTable(nDataTypeInfos, _curDataType, 0);
        if (i > -1)
            return sDataTypeNames[i];;
        return "";
    }


    public int getDataType(String _sTypeName){
        int i = JavaTools.FieldInList(sDataTypeNames, _sTypeName);
        if (i > -1)
            return nDataTypeInfos[i];
        else
            return getLastConversionFallbackDataType();
    }


    public int convertDataType(int _curDataType){
        int retDataType = _curDataType;
        if (!supportsDataType(_curDataType)){
            switch(_curDataType)
            {
                case DataType.BIT:
                        retDataType = convertDataType(DataType.BOOLEAN);
                        break;
                case DataType.BOOLEAN:
                        retDataType = convertDataType(DataType.BIT);
                        break;
                case DataType.TINYINT:
                    retDataType = convertDataType(DataType.SMALLINT);
                    break;
                case DataType.SMALLINT:
                    retDataType = convertDataType(DataType.INTEGER);
                    break;
                case DataType.INTEGER:
                    retDataType = convertDataType(DataType.FLOAT);
                    break;
                case DataType.FLOAT:
                    retDataType = convertDataType(DataType.REAL);
                    break;
                case DataType.DATE:
                case DataType.TIME:
                    retDataType = convertDataType(DataType.TIMESTAMP);
                    break;
                case DataType.TIMESTAMP:
                case DataType.REAL:
                case DataType.BIGINT:
                    retDataType = convertDataType(DataType.DOUBLE);
                    break;
                case DataType.DOUBLE:
                    retDataType = convertDataType(DataType.NUMERIC);
                    break;
                case DataType.NUMERIC:
                    retDataType = convertDataType(DataType.DECIMAL);
                    break;
                case DataType.DECIMAL:
                    if (supportsDataType(DataType.DOUBLE))
                        retDataType = convertDataType(DataType.DOUBLE);
                    else if (supportsDataType(DataType.NUMERIC))
                        retDataType = DataType.NUMERIC;
                    else
                        retDataType = getLastConversionFallbackDataType();
                    break;
                case DataType.VARCHAR:
                    retDataType = getLastConversionFallbackDataType();
                    break;
                default:
                    retDataType = getLastConversionFallbackDataType();
            }
        }
        return retDataType;
    }

    public int getAutoIncrementIndex(XPropertySet _xColPropertySet){
    try {
        boolean bleaveloop = false;
        int startindex = 0;
        int curDataType = ((Integer) _xColPropertySet.getPropertyValue("Type")).intValue();
        while (!bleaveloop){
            int i = JavaTools.FieldInIntTable(nDataTypeInfos, curDataType, startindex);
            startindex = i+1;
            bleaveloop = (i == -1);
            if (!bleaveloop){
                if (bisAutoIncrementableInfos[i]){
                    return nDataTypeInfos[i];
                }
            }
        }
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }
    return INVALID;

    }


    public boolean isAutoIncrementable(XPropertySet _xColPropertySet){
        return (getAutoIncrementIndex(_xColPropertySet) != INVALID);
    }



    /** finds the first available DataType that can be used as a primary key in a table.
     * @return The first datatype that also supports Autoincrmentation is taken according to the following list:
     *1) INTEGER
     *2) FLOAT
     *3) REAL
     *4) DOUBLE
     *5) NUMERIC
     *6) DECIMAL         *
     * If no appropriate datatype is found ther first available numeric type after DataType.INTEGER
     * according to the 'convertDataType' method is returned
     */
    /**TODO the fallback order is the same as implemented in the method 'convertDataType'.
     * It's not very elegant to have the same intelligence
     * on several spots in the class!!
     *
     */
    public TypeInfo findAutomaticPrimaryKeyType(){
        int nDataType;
        for (int n = 0; n < this.nNumericFallBackList.length; n++){
            nDataType = nNumericFallBackList[n];
            boolean bleaveloop = false;
            int startindex = 0;
            while (!bleaveloop){
                int i = JavaTools.FieldInIntTable(nDataTypeInfos, nDataType, startindex);
                bleaveloop = (i < 0);
                if (!bleaveloop){
                    if (this.bisAutoIncrementableInfos[i] = true)
                        return new TypeInfo(nDataType, this.sDataTypeNames[i], true);
                }
                else
                    startindex = i+1;
            }
        }
        // As Autoincrementation is not supported for any numeric datatype we take the first available numeric Type;
        nDataType =convertDataType(DataType.INTEGER);
        return new TypeInfo(nDataType, getDefaultTypeName(nDataType), false);
    }
}
