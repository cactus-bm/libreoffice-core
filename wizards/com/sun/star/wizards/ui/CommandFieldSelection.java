/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: CommandFieldSelection.java,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:46:08 $
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
 ************************************************************************/package com.sun.star.wizards.ui;
import com.sun.star.wizards.common.*;
import com.sun.star.wizards.db.*;
import com.sun.star.awt.XWindow;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdbc.SQLException;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.awt.*;
import com.sun.star.beans.PropertyValue;

public class CommandFieldSelection extends FieldSelection {
    CommandMetaData CurDBMetaData;
    XListBox xTableListBox;
    XFixedText xlblTable;
    String SFILLUPFIELDSLISTBOX = "fillUpFieldsListbox";
    String sTableListBoxName;
    String sTableLabelName;
    String sQueryPrefix;
    String sTablePrefix;
    short iSelPos = -1;
    short iOldSelPos = -1;
    boolean bpreselectCommand = true;
    boolean bgetQueries;
    boolean AppendMode;
    WizardDialog oWizardDialog;

    class ItemListenerImpl implements com.sun.star.awt.XItemListener {

        public void itemStateChanged(com.sun.star.awt.ItemEvent EventObject) {
            short[] SelItems = (short[]) CurUnoDialog.getControlProperty(sTableListBoxName, "SelectedItems");
            if (SelItems.length > 0) {
                iOldSelPos = iSelPos;
                iSelPos = SelItems[0];
                if ((iSelPos > -1) && (iSelPos != iOldSelPos)){
                    if (!AppendMode)
                        oWizardDialog.enablefromStep(IStep.intValue() + 1, false);
                    fillUpFieldsListbox();
                }
            }
        }


        public void disposing(com.sun.star.lang.EventObject eventObject) {
        }
    }


    /**
     * instantiates a CommandFieldSelection with a preselected command
     * @param _CurUnoDialog
     * @param _CurDBMetaData
     * @param iStep
     * @param _iHeight
     * @param _reslblFields
     * @param _reslblSelFields
     * @param _reslblTables
     * @param _bgetQueries
     * @param _ifirstHID
     */
    public CommandFieldSelection(WizardDialog _CurUnoDialog, CommandMetaData _CurDBMetaData, int iStep, int _iHeight, String _reslblFields, String _reslblSelFields, String _reslblTables, boolean _bgetQueries, int _ifirstHID) {
        super(_CurUnoDialog, iStep, 95, 57, 210, _iHeight, _reslblFields, _reslblSelFields, (_ifirstHID + 1), true);
        insertControls(_CurDBMetaData, _bgetQueries, _reslblTables);
        oWizardDialog = (WizardDialog) CurUnoDialog;
    }


    /**
     * instantiates a CommandFieldSelection with a preselected command
     * @param _CurUnoDialog
     * @param _CurDBMetaData
     * @param _iHeight
     * @param _reslblFields
     * @param _reslblSelFields
     * @param _reslblTables
     * @param _bgetQueries
     * @param _ifirstHID
     */
    public CommandFieldSelection(UnoDialog _CurUnoDialog, CommandMetaData _CurDBMetaData, int _iHeight, String _reslblFields, String _reslblSelFields, String _reslblTables, boolean _bgetQueries, int _ifirstHID) {
        super(_CurUnoDialog, 1, 95, 57, 210, _iHeight, _reslblFields, _reslblSelFields, (_ifirstHID + 1), true);
        insertControls(_CurDBMetaData, _bgetQueries, _reslblTables);
        oWizardDialog = (WizardDialog) CurUnoDialog;
    }


    private void insertControls(CommandMetaData _CurDBMetaData, boolean _bgetQueries, String _reslblTables){
        try {
            this.AppendMode = !_bgetQueries;
            this.bgetQueries = _bgetQueries;
            this.CurDBMetaData = _CurDBMetaData;
            toggleListboxControls(Boolean.FALSE);
            sTableLabelName = "lblTables_" + super.sIncSuffix;
            sTableListBoxName = "lstTables_" + super.sIncSuffix;
            sTablePrefix = getTablePrefix();
            sQueryPrefix = getQueryPrefix();
            Integer LabelWidth = new Integer(getListboxWidth().intValue() + 6);
            xlblTable = CurUnoDialog.insertLabel(sTableLabelName,
                        new String[] { "Enabled", "Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width" },
                        new Object[] { Boolean.FALSE, new Integer(8), _reslblTables, new Integer(95), new Integer(27), IStep, new Short((short) 3), LabelWidth });
            xTableListBox = CurUnoDialog.insertListBox(sTableListBoxName, 0, null, new ItemListenerImpl(),
                        new String[] { "Dropdown", "Enabled", "Height", "HelpURL", "LineCount", "PositionX", "PositionY", "Step", "TabIndex", "Width" },
                        new Object[] { Boolean.TRUE, Boolean.FALSE, new Integer(12), "HID:" + (super.FirstHelpIndex-1), new Short("7"), new Integer(95), new Integer(37), IStep, new Short((short) 4), getListboxWidth()});
            XWindow xTableListBoxWindow = (XWindow) UnoRuntime.queryInterface(XWindow.class, xTableListBox);
            fillupCommandListBox();
        } catch (Exception exception) {
            exception.printStackTrace(System.out);
        }
    }

    /**
     * @return Returns the sQueryPrefix.
     */
    public String getQueryPrefix() {
        if (sQueryPrefix == null)
            sQueryPrefix = CurUnoDialog.oResource.getResText(UIConsts.RID_QUERY + 22);
        return sQueryPrefix;
    }


    private String getCommandPrefix(int _nCommandType){
        if (_nCommandType == CommandType.TABLE)
            return getTablePrefix();
        else if (_nCommandType == CommandType.QUERY)
            return getQueryPrefix();
        else return "";
    }


    /**
     * @return Returns the sTablePrefix.
     */
    public String getTablePrefix() {
        if (sTablePrefix == null)
            sTablePrefix = CurUnoDialog.oResource.getResText(UIConsts.RID_QUERY + 21);
        return sTablePrefix;
    }

    private short getselectedItemPos(){
        short[] iSelPoses = ((short[]) Helper.getUnoPropertyValue(UnoDialog.getModel(xTableListBox), "SelectedItems"));
        if (iSelPoses.length > 0)
            return iSelPoses[0];
        else{
            if (this.bpreselectCommand){
                String[] sItemList= ((String[]) Helper.getUnoPropertyValue(UnoDialog.getModel(xTableListBox), "StringItemList"));
                if (sItemList.length > 0)
                    return (short) 0;
            }
            return (short) -1;
        }
    }

    public void fillUpFieldsListbox() {
    try {
        boolean binitialize = false;
        String curCommandName = "";
        //As the peer of the control might not yet exist we have to query the model for the SelectedItems
        String[] sLocList = (String[]) CurUnoDialog.getControlProperty(sTableListBoxName, "StringItemList");
        short iSelPos = getselectedItemPos();
        if (!bgetQueries){
            curCommandName = sLocList[iSelPos];
            CurDBMetaData.setTableByName(curCommandName);
            binitialize = CurDBMetaData.getFieldNamesOfCommand(curCommandName, CommandType.TABLE, AppendMode);
        }
        else {
            if (sLocList[iSelPos].startsWith(sTablePrefix)){
                CurDBMetaData.setCommandType(CommandType.TABLE);
                curCommandName = JavaTools.replaceSubString(sLocList[iSelPos], "", sTablePrefix);
                CurDBMetaData.setTableByName(curCommandName);
                binitialize = CurDBMetaData.getFieldNamesOfCommand(curCommandName, CommandType.TABLE, AppendMode);
            }
            else {
                CurDBMetaData.setCommandType(CommandType.QUERY);
                curCommandName = JavaTools.replaceSubString(sLocList[iSelPos], "", sQueryPrefix);
                CurDBMetaData.setQueryByName(curCommandName);
                binitialize = CurDBMetaData.getFieldNamesOfCommand(curCommandName, CommandType.QUERY, AppendMode);
            }
        }
        if (binitialize){
            CurDBMetaData.setCommandName(curCommandName);
            if (CurDBMetaData.AllFieldNames != null){
                if (CurDBMetaData.AllFieldNames.length > 0) {
                    initialize(CurDBMetaData.AllFieldNames, AppendMode, CurDBMetaData.getMaxColumnsInSelect());
                    return;
                }
            }
        }
        emptyFieldsListBoxes();
    } catch (Exception exception) {
        exception.printStackTrace(System.out);
    }}


    /** returns the selected entry index in the commandListbox
     *
     * @return
     * @throws com.sun.star.wizards.common.TerminateWizardException
     */
    private short fillupCommandListBox() throws com.sun.star.wizards.common.TerminateWizardException {
        short[] iSelArray = new short[0];
        boolean bgetFields = false;
        String[] ContentList = new String[0];
        CurDBMetaData.setCommandNames();
        if (bgetQueries) {
            ContentList = new String[CurDBMetaData.TableNames.length + CurDBMetaData.QueryNames.length];
            System.arraycopy(CurDBMetaData.QueryNames, 0, ContentList, CurDBMetaData.TableNames.length, CurDBMetaData.QueryNames.length);
            ContentList = setPrefixinArray(ContentList, sQueryPrefix, CurDBMetaData.TableNames.length, CurDBMetaData.QueryNames.length);
        } else
            ContentList = new String[CurDBMetaData.TableNames.length];
        System.arraycopy(CurDBMetaData.TableNames, 0, ContentList, 0, CurDBMetaData.TableNames.length);
        if (bgetQueries)
            ContentList = setPrefixinArray(ContentList, sTablePrefix, 0, CurDBMetaData.TableNames.length);
        Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "StringItemList", ContentList);
        short iSelPos = getselectedItemPos();
        if (bpreselectCommand) {
            if (iSelPos > -1) {
                bgetFields = true;
                iSelArray = new short[] { iSelPos };
            }
        } else {
            emptyFieldsListBoxes();
            iSelArray = new short[] {(short) iSelPos };
        }
        Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "SelectedItems", iSelArray);
        toggleCommandListBox(true);
        if (bgetFields)
            fillUpFieldsListbox();
        return iSelPos;
    }

    private String[] setPrefixinArray(String[] _ContentList, String _sprefix, int _startindex, int _nlen) {
        for (int i = _startindex; i < _startindex + _nlen; i++)
            _ContentList[i] = _sprefix + _ContentList[i];
        return _ContentList;
    }


    public void toggleCommandListBox(String[] _NewItems) {
        boolean bdoenable = !(QueryMetaData.getIncludedCommandNames(_NewItems).length >= CurDBMetaData.getMaxTablesInSelect());
        toggleCommandListBox(bdoenable);
    }


    public void toggleCommandListBox(boolean _bdoenable) {
        Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "Enabled", new Boolean(_bdoenable));
        Helper.setUnoPropertyValue(UnoDialog.getModel(xlblTable), "Enabled", new Boolean(_bdoenable));
    }

    public String getSelectedCommandName(){
        String sCommandname = xTableListBox.getSelectedItem();
        if (sCommandname.startsWith(this.sTablePrefix))
            return sCommandname.substring(sTablePrefix.length());
        else if ((sCommandname.startsWith(this.sQueryPrefix)))
            return sCommandname.substring(sQueryPrefix.length());
        else
            return sCommandname;
    }


    public int getSelectedCommandType(){
        String sCommandname = xTableListBox.getSelectedItem();
        if (sCommandname.startsWith(this.sTablePrefix))
            return CommandType.TABLE;
        else
            return CommandType.QUERY;
    }


    public void preselectCommand(PropertyValue[] _aPropertyValue, boolean _bReadOnly){
    try {
        if (Properties.hasPropertyValue(_aPropertyValue, "CommandType")){
            int nCommandType = AnyConverter.toInt(Properties.getPropertyValue(_aPropertyValue, "CommandType"));
            String sCommand = AnyConverter.toString(Properties.getPropertyValue(_aPropertyValue, "Command"));
            if (sCommand != null)
                preselectCommand(sCommand, nCommandType, _bReadOnly );
        }
    } catch (IllegalArgumentException e) {
        e.printStackTrace();
    }}



    public void preselectCommand(String _selitem, int _nCommandType, boolean _bReadOnly){
        if (_selitem.length() > 0){
            String[] sitems = (String[]) Helper.getUnoPropertyValue(UnoDialog.getModel(xTableListBox), "StringItemList");
            String sPrefix = getCommandPrefix(_nCommandType);
            short iselpos = (short) JavaTools.FieldInList(sitems, sPrefix + _selitem);
            if (iselpos > -1)
                Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "SelectedItems", new short[]{iselpos});
            this.fillUpFieldsListbox();
        }
        else
            Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "SelectedItems", new short[]{});
        if (_bReadOnly)
            Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "ReadOnly", new Boolean(_selitem.length() > 0));
    }


    public void preselectCommand(String _selitem, boolean _bReadOnly){
        if (_selitem.length() > 0){
            String[] sitems = (String[]) Helper.getUnoPropertyValue(UnoDialog.getModel(xTableListBox), "StringItemList");
            short iselpos = (short) JavaTools.FieldInList(sitems, getTablePrefix() + _selitem);
            if (iselpos > -1)
                Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "SelectedItems", new short[]{iselpos});
            this.fillUpFieldsListbox();
        }
        else
            Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "SelectedItems", new short[]{});
        if (_bReadOnly)
            Helper.setUnoPropertyValue(UnoDialog.getModel(xTableListBox), "ReadOnly", new Boolean(_selitem.length() > 0));
    }
}
