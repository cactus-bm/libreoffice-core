/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TableDescriptor.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:25:46 $
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

import com.sun.star.awt.VclWindowPeerAttribute;
import com.sun.star.beans.Property;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.PropertyVetoException;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.ContainerEvent;
import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.XContainer;
import com.sun.star.container.XContainerListener;
import com.sun.star.container.XHierarchicalNameAccess;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.EventObject;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.IndexOutOfBoundsException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdbc.ColumnValue;
import com.sun.star.sdbc.SQLException;
import com.sun.star.sdbcx.KeyType;
import com.sun.star.sdbcx.XAppend;
import com.sun.star.sdbcx.XColumnsSupplier;
import com.sun.star.sdbcx.XDataDescriptorFactory;
import com.sun.star.sdbcx.XDrop;
import com.sun.star.sdbcx.XKeysSupplier;
import com.sun.star.sdbcx.XTablesSupplier;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.wizards.common.Desktop;
import com.sun.star.wizards.common.JavaTools;
import com.sun.star.wizards.common.Properties;


public class TableDescriptor extends CommandMetaData  implements XContainerListener{
    XDataDescriptorFactory xTableDataDescriptorFactory;
    XPropertySet xPropTableDataDescriptor;
    private XNameAccess xNameAccessColumns;
    private XIndexAccess xIndexAccessKeys;

    public XDataDescriptorFactory xColumnDataDescriptorFactory;
    XContainer xTableContainer;
    XAppend xTableAppend;
    XDrop   xTableDrop;
    private XAppend xKeyAppend;
    private XDrop xKeyDrop;
    private String[] sTableFilters = null;
    private Vector columncontainer;
    private Vector keycolumncontainer;
    public XHierarchicalNameAccess xTableHierarchicalNameAccess;
    private CommandName ComposedTableName;
    private XAppend xKeyColAppend;
    private XPropertySet xKey;
    private boolean bIDFieldisInserted = false;
    private String IDFieldName = "";

        /**
         * @param xMSF
         */
        public TableDescriptor(XMultiServiceFactory xMSF) {
            super(xMSF);
            columncontainer = new Vector();
            keycolumncontainer = new Vector();
        }


        private class ColumnDescriptor{
            String Name;
            XPropertySet xColPropertySet;

            public ColumnDescriptor(XPropertySet _xColPropertySet, String _Name){
                Name = _Name;
                xColPropertySet = _xColPropertySet;
            }

        }

        public boolean getConnection(PropertyValue[] _curPropertyValue){
            if (super.getConnection(_curPropertyValue)){
                XTablesSupplier xDBTables = (XTablesSupplier) UnoRuntime.queryInterface(XTablesSupplier.class, DBConnection);
                xTableNames =  xDBTables.getTables();
                xTableAppend = (XAppend) UnoRuntime.queryInterface(XAppend.class, xTableNames);
                xTableDrop = (XDrop) UnoRuntime.queryInterface(XDrop.class, xTableNames);
                xTableDataDescriptorFactory = (XDataDescriptorFactory) UnoRuntime.queryInterface(XDataDescriptorFactory.class, xTableNames);
                xPropTableDataDescriptor = xTableDataDescriptorFactory.createDataDescriptor();
                XColumnsSupplier xColumnsSupplier = (XColumnsSupplier) UnoRuntime.queryInterface(XColumnsSupplier.class, xPropTableDataDescriptor);
                xNameAccessColumns = xColumnsSupplier.getColumns();
                xColumnDataDescriptorFactory = (XDataDescriptorFactory) UnoRuntime.queryInterface(XDataDescriptorFactory.class, xNameAccessColumns);
                try {
                    createTypeInspector();
                    sTableFilters = (String[]) AnyConverter.toArray(xDataSourcePropertySet.getPropertyValue("TableFilter"));
                } catch (Exception e) {
                    e.printStackTrace(System.out);
                }
                return true;
            }
            else
                return false;
        }


       public boolean createPrimaryKeys(String[] _fieldnames, boolean _bAutoincrementation){
        try {
           XKeysSupplier xKeySupplier = (XKeysSupplier)UnoRuntime.queryInterface(XKeysSupplier.class, xPropTableDataDescriptor);
           xIndexAccessKeys = xKeySupplier.getKeys();
           XDataDescriptorFactory xKeyFac = (XDataDescriptorFactory)UnoRuntime.queryInterface(XDataDescriptorFactory.class,xIndexAccessKeys);
           xKeyDrop = (XDrop) UnoRuntime.queryInterface(XDrop.class, xIndexAccessKeys);
           xKeyAppend = (XAppend)UnoRuntime.queryInterface(XAppend.class, xKeyFac);
           xKey = xKeyFac.createDataDescriptor();
           xKey.setPropertyValue("Type", new Integer(KeyType.PRIMARY));
           XColumnsSupplier xKeyColumSup = (XColumnsSupplier)UnoRuntime.queryInterface(XColumnsSupplier.class, xKey);
           XDataDescriptorFactory xKeyColFac = (XDataDescriptorFactory)UnoRuntime.queryInterface(XDataDescriptorFactory.class,xKeyColumSup.getColumns());
           xKeyColAppend = (XAppend)UnoRuntime.queryInterface(XAppend.class, xKeyColFac);
            if (keycolumncontainer.size() > 0){
                for (int i = (keycolumncontainer.size()-1); i >= 0 ; i--){
                    keycolumncontainer.remove(i);
                }
            }
           for (int i = 0; i < _fieldnames.length; i++){
                  XPropertySet xKeyColPropertySet = xKeyColFac.createDataDescriptor();
                   xKeyColPropertySet.setPropertyValue("Name", _fieldnames[i]);
                   keycolumncontainer.add(xKeyColPropertySet);
                XPropertySet xColPropertySet = null;
                   if (hasByName(_fieldnames[i]))
                       xColPropertySet = getByName(_fieldnames[i]);
                   else
                       xColPropertySet = addPrimaryKeyColumn(_fieldnames[i]);
                   xColPropertySet.setPropertyValue("IsNullable", new Integer(com.sun.star.sdbc.ColumnValue.NO_NULLS));
                   if (_bAutoincrementation){
                       int nDataType  = oTypeInspector.getAutoIncrementIndex(xColPropertySet);
                       if (nDataType != oTypeInspector.INVALID)
                           if (xColPropertySet.getPropertySetInfo().hasPropertyByName("IsAutoIncrement")){
                            xColPropertySet.setPropertyValue("Type", new Integer(nDataType));
                            xColPropertySet.setPropertyValue("IsAutoIncrement", new Boolean(_bAutoincrementation));
                           }
                }
                   modifyColumn(_fieldnames[i], xColPropertySet);
           }
           return true;
        } catch (Exception e) {
            showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
            e.printStackTrace(System.out);
        }
            return false;
        }


        /**
         * creates the table under the passed name
         * @param _tablename is made unique if necessary
         * @return true or false to indicate successful creation or not
         */
        public boolean createTable(String _catalogname, String _schemaname, String _tablename, String[] _fieldnames){
            try {
                XAppend xAppendColumns = (XAppend) UnoRuntime.queryInterface(XAppend.class, xNameAccessColumns);
                for (int i = 0; i < columncontainer.size(); i++){
                    XPropertySet xColPropertySet = getByIndex(i);
                    xAppendColumns.appendByDescriptor(xColPropertySet);
                }
                assignTableProperty("Name", _tablename);
                assignTableProperty("CatalogName", _catalogname);
                assignTableProperty("SchemaName", _schemaname);
                xTableContainer = (XContainer) UnoRuntime.queryInterface(XContainer.class, xTableNames);
                xTableContainer.addContainerListener(this);
                if (keycolumncontainer.size() > 0){
                    for (int i = 0; i < keycolumncontainer.size(); i++){
                        XPropertySet xKeyColPropertySet = (XPropertySet) keycolumncontainer.get(i);
                        xKeyColAppend.appendByDescriptor(xKeyColPropertySet);
                    }
                   xKeyAppend.appendByDescriptor(xKey);
                }
                xTableAppend.appendByDescriptor(xPropTableDataDescriptor);
                return true;
            } catch (Exception e) {
                e.printStackTrace(System.out);
                showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
                try {
                    xPropTableDataDescriptor.setPropertyValue("Name", "");
                    if ((xKeyDrop != null) && (xIndexAccessKeys != null)){
                        int icount = xIndexAccessKeys.getCount();
                        if (icount > 0){
                            for (int i = xIndexAccessKeys.getCount()-1; i >= 0; i--){
                                xKeyDrop.dropByIndex(i);
                            }
                        }
                    }
                    XDrop xColumnDrop = (XDrop) UnoRuntime.queryInterface(XDrop.class, xNameAccessColumns);
                    for (int i = xNameAccessColumns.getElementNames().length - 1; i >= 0; i--)
                        xColumnDrop.dropByIndex(i);
                    if (xTableDrop != null)
                        if (xTableNames.hasByName(_tablename))
                            xTableDrop.dropByName(_tablename);
                    if (bIDFieldisInserted){
                        this.dropColumnbyName(this.IDFieldName);
                        bIDFieldisInserted = false;
                    }
                    return false;
                } catch (Exception e1) {
                    e1.printStackTrace(System.out);
                }
                return false;
            }
        }


        public boolean createTable(String _catalogname, String _schemaname, String _tablename, String[] _keycolumnnames, boolean _bAutoincrementation, String[] _fieldnames){
            if (createPrimaryKeys(_keycolumnnames, _bAutoincrementation))
                return createTable(_catalogname, _schemaname, _tablename, _fieldnames);
            return false;
        }


        private void assignTableProperty(String _spropname, String _svalue){
            if (_svalue != null){
                if (!_svalue.equals("")){
                    try {
                        xPropTableDataDescriptor.setPropertyValue(_spropname, _svalue);
                    } catch (Exception e) {
                        e.printStackTrace(System.out);
                    }
                }
            }
        }


        public boolean modifyColumnName(String _soldname, String _snewname){
        try {
            if (hasByName(_soldname)){
                ColumnDescriptor oColumnDescriptor = this.getColumnDescriptorByName(_soldname);
                oColumnDescriptor.xColPropertySet.setPropertyValue("Name", _snewname);
                oColumnDescriptor.Name = _snewname;
            }
            return true;
        } catch (Exception e) {
            e.printStackTrace(System.out);
            showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
            return false;
        }}


        public boolean modifyColumn(String _sname, String _spropname, Object _oValue){
            try {
                if (this.columncontainer.size() > 0){
                    for (int i = 0; i < columncontainer.size(); i++){
                        ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                        if (oColumnDescriptor.Name.equals(_sname)){
                            oColumnDescriptor.xColPropertySet.setPropertyValue(_spropname, _oValue);
                            if (_spropname.equals("Name"))
                                oColumnDescriptor.Name = (String) _oValue;
                            return true;
                        }
                    }
                }
            } catch (Exception e) {
                e.printStackTrace(System.out);
                showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
            }
            return false;

        }


        public boolean modifyColumn(String _sname, XPropertySet _xColPropertySet){
        try {
            if (this.columncontainer.size() > 0){
                for (int i = 0; i < columncontainer.size(); i++){
                    ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                    if (oColumnDescriptor.Name.equals(_sname)){
                        oColumnDescriptor.xColPropertySet = _xColPropertySet;
                        oColumnDescriptor.Name = (String) _xColPropertySet.getPropertyValue("Name");
                        columncontainer.remove(i);
                        columncontainer.insertElementAt(oColumnDescriptor, i);
                        return true;
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
            showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
        }
        return false;
        }


        public void dropColumnbyName(String _sname){
        try {
            if (columncontainer.size() > 0){
                for (int i = 0; i < columncontainer.size(); i++){
                    ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                    if (oColumnDescriptor != null)
                        if (oColumnDescriptor.Name.equals(_sname))
                            columncontainer.remove(i);
                }
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }}


        public String[] getColumnNames(){
            if (columncontainer.size() > 0){
                try {
                    String[] fieldnames = new String[columncontainer.size()];
                    for (int i = 0; i < columncontainer.size(); i++){
                        ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                        fieldnames[i] = oColumnDescriptor.Name;
                    }
                    return fieldnames;
                } catch (RuntimeException e) {
                    e.printStackTrace(System.out);
                }
            }
            return new String[]{};
        }


        private boolean hasByName(String _fieldname){
            try {
                if (columncontainer.size() > 0){
                    for (int i = 0; i < columncontainer.size(); i++){
                        ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                        if (oColumnDescriptor.Name.equals(_fieldname)){
                            return true;
                        }
                    }
                }
            } catch (RuntimeException e) {
                e.printStackTrace(System.out);
            }
            return false;
        }


        private ColumnDescriptor getColumnDescriptorByName(String _fieldname){
            try {
                if (this.columncontainer.size() > 0){
                    for (int i = 0; i < columncontainer.size(); i++){
                        ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                        if (oColumnDescriptor.Name.equals(_fieldname)){
                            return oColumnDescriptor;
                        }
                    }
                }
            } catch (RuntimeException e) {
                e.printStackTrace(System.out);
            }
            return null;
        }


        public XPropertySet getByName(String _fieldname){
            ColumnDescriptor oColumnDescriptor = getColumnDescriptorByName(_fieldname);
            if (oColumnDescriptor != null)
                return oColumnDescriptor.xColPropertySet;
            else
                return null;
        }


        private XPropertySet getByIndex(int _index){
            try {
                if (columncontainer.size() > _index){
                    ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(_index);
                    return oColumnDescriptor.xColPropertySet;
                }
            } catch (RuntimeException e) {
                e.printStackTrace(System.out);
            }
            return null;
        }


        public XPropertySet clonePropertySet(String _snewname, XPropertySet _xnewPropertySet){
            XPropertySet xRetPropertySet = xColumnDataDescriptorFactory.createDataDescriptor();
            try {
                if (hasByName(_snewname)){
                    Object oColumn = getByName(_snewname);
                    XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, oColumn);
                    Property[] aColProperties = xPropertySet.getPropertySetInfo().getProperties();
                    for (int i = 0; i < aColProperties.length; i++){
                        String sPropName = aColProperties[i].Name;
                        Object oColValue = _xnewPropertySet.getPropertyValue(sPropName);
                        xRetPropertySet.setPropertyValue(sPropName, oColValue);
                    }
                }
            } catch (Exception e) {
                e.printStackTrace(System.out);
            }
            return xRetPropertySet;
        }


        public boolean addColumn(PropertyValue[] _aNewPropertyValues){
        try {
            String sname = (String) Properties.getPropertyValue(_aNewPropertyValues, "Name");
            if (!hasByName(sname)){
                ColumnPropertySet oPropertySet = new ColumnPropertySet(oTypeInspector, xColumnDataDescriptorFactory.createDataDescriptor());
                oPropertySet.assignPropertyValues(_aNewPropertyValues, true);
                ColumnDescriptor oColumnDescriptor = new ColumnDescriptor(oPropertySet.xPropertySet, sname);
                this.columncontainer.add(oColumnDescriptor);
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        return false;
        }


        public boolean addColumn(String _columnname, XPropertySet _xNewColPropertySet){
        try {
            if (!hasByName(_columnname)){
                if (_columnname.equals(""))
                    return false;
                else{
                    ColumnPropertySet oPropertySet = new ColumnPropertySet(oTypeInspector, xColumnDataDescriptorFactory.createDataDescriptor());
                    oPropertySet.assignNewPropertySet(_columnname, _xNewColPropertySet);
                    ColumnDescriptor oColumnDescriptor = new ColumnDescriptor(oPropertySet.xPropertySet, _columnname);
                    columncontainer.add(oColumnDescriptor);
                    return true;
                }
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
            showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
        }
        return false;
        }



        public XPropertySet addPrimaryKeyColumn(String _columnname){
        try {
            if (!hasByName(_columnname)){
                try {
                    XPropertySet xColPropertySet = xColumnDataDescriptorFactory.createDataDescriptor();
                    IDFieldName = Desktop.getUniqueName(getColumnNames(), _columnname, "");
                    xColPropertySet.setPropertyValue("Name", IDFieldName);
                    xColPropertySet.setPropertyValue("Type", new Integer(oTypeInspector.convertDataType(com.sun.star.sdbc.DataType.INTEGER)));
                    ColumnDescriptor oColumnDescriptor = new ColumnDescriptor( xColPropertySet, IDFieldName);
                    this.columncontainer.add(0, oColumnDescriptor);
                    this.bIDFieldisInserted = true;
                    return xColPropertySet;
                } catch (RuntimeException e1) {
                    e1.printStackTrace(System.out);
                }
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
            showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, e.getMessage());
        }
            return null;
        }



       public String[] getNonBinaryFieldNames(){
            Vector NonBinaryFieldNameVector = new Vector();
            try {
                for (int i = 0; i < columncontainer.size(); i++){
                    ColumnDescriptor oColumnDescriptor = (ColumnDescriptor) columncontainer.get(i);
                    XPropertySet xColPropertySet = getByName(oColumnDescriptor.Name);
                    Property[] aProperties = xColPropertySet.getPropertySetInfo().getProperties();
                    int itype;
                    try {
                        itype = AnyConverter.toInt(xColPropertySet.getPropertyValue("Type"));
                        if (!isBinaryDataType(itype))
                            NonBinaryFieldNameVector.addElement(oColumnDescriptor.Name);
                    } catch (Exception e) {
                        e.printStackTrace(System.out);
                    }
                }
            } catch (RuntimeException e) {
                e.printStackTrace(System.out);
            }
            String[] sbinaryfieldnames = new String[NonBinaryFieldNameVector.size()];
            NonBinaryFieldNameVector.toArray(sbinaryfieldnames);
            return sbinaryfieldnames;
        }


           public String getComposedTableName(String _scatalogname, String _sschemaname, String _stablename){
            ComposedTableName = new CommandName(this, _scatalogname, _sschemaname, _stablename, false);
            return ComposedTableName.getComposedName();
           }


           public String getComposedTableName(){
               if (ComposedTableName != null)
                   return this.ComposedTableName.getComposedName();
               else
                   return null;
           }


               /* (non-Javadoc)
         * @see com.sun.star.container.XContainerListener#elementInserted(com.sun.star.container.ContainerEvent)
         */
        public void elementInserted(ContainerEvent arg0) {
        try {
            XPropertySet xTablePropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, arg0.Element);
            String stablename = AnyConverter.toString(xTablePropertySet.getPropertyValue("Name"));
            String sschemaname = AnyConverter.toString(xPropTableDataDescriptor.getPropertyValue("SchemaName"));
            String scatalogname = AnyConverter.toString(xPropTableDataDescriptor.getPropertyValue("CatalogName"));
            ComposedTableName = new CommandName(this, scatalogname, sschemaname, stablename, false);
            appendTableNameToFilter(ComposedTableName.getComposedName());
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }}


        /* (non-Javadoc)
         * @see com.sun.star.container.XContainerListener#elementRemoved(com.sun.star.container.ContainerEvent)
         */
        public void elementRemoved(ContainerEvent arg0) {

        }

        /* (non-Javadoc)
         * @see com.sun.star.container.XContainerListener#elementReplaced(com.sun.star.container.ContainerEvent)
         */
        public void elementReplaced(ContainerEvent arg0) {

        }

        /* (non-Javadoc)
         * @see com.sun.star.lang.XEventListener#disposing(com.sun.star.lang.EventObject)
         */
        public void disposing(EventObject arg0) {

        }

           /**
            * @param _stablename
            * @return
            */
           public boolean appendTableNameToFilter(String _scomposedtablename){
               boolean bhastoinsert = true;
               for (int i = 0; i < sTableFilters.length; i++){
                   if (sTableFilters[i].compareTo("%") > -1){
                       if (sTableFilters[i].endsWith("." + _scomposedtablename))
                           bhastoinsert = false;
                       else if (sTableFilters[i].length() == 1)
                           bhastoinsert = false;
                   }
                   else if (sTableFilters[i].equals(_scomposedtablename))
                       bhastoinsert = false;
                   if (!bhastoinsert)
                       break;
               }
               if (bhastoinsert){
                   String[] sNewTableFilters = new String[sTableFilters.length + 1];
                System.arraycopy(sTableFilters, 0, sNewTableFilters, 0, sTableFilters.length);
                   sNewTableFilters[sTableFilters.length] = _scomposedtablename;
                   sTableFilters = sNewTableFilters;
                   try {
                    xDataSourcePropertySet.setPropertyValue("TableFilter", sTableFilters);
                } catch (Exception e) {
                    e.printStackTrace(System.out);
                    bhastoinsert = false;
                }
               }
               return bhastoinsert;
           }
}

