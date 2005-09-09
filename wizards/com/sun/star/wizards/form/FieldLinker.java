/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: FieldLinker.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:30:38 $
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
package com.sun.star.wizards.form;

import java.beans.PropertyChangeEvent;

import com.sun.star.awt.ItemEvent;
import com.sun.star.awt.VclWindowPeerAttribute;
import com.sun.star.awt.XFixedText;
import com.sun.star.awt.XListBox;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.Exception;
import com.sun.star.wizards.common.Helper;
import com.sun.star.wizards.common.JavaTools;
import com.sun.star.wizards.common.SystemDialog;
import com.sun.star.wizards.db.CommandMetaData;
import com.sun.star.wizards.ui.UnoDialog;
import com.sun.star.wizards.ui.WizardDialog;
import com.sun.star.wizards.ui.UIConsts;
import com.sun.star.wizards.ui.DBLimitedFieldSelection;


public class FieldLinker extends DBLimitedFieldSelection{
    XFixedText[] lblSlaveFields;
    XFixedText[] lblMasterFields;
    XListBox[] lstSlaveFields;
    XListBox[] lstMasterFields;
    final int SOMASTERINDEX = 1;
    final int SOSLAVEINDEX = 0;
    int SOFIRSTLINKLST = 0;
    int SOSECLINKLST = 1;
    int SOTHIRDLINKLST = 2;
    int SOFOURTHLINKLST = 3;
    int[] SOLINKLST = null;
    String[] sSlaveListHeader;
    String[] sMasterListHeader; //CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 40);
    String sSlaveHidString;
    String sMasterHidString;
    Integer IListBoxPosX;



    public FieldLinker(WizardDialog _CurUnoDialog, int iStep, int iCompPosX, int iCompPosY, int iCompWidth, int _firsthelpid) {
        super(_CurUnoDialog, iStep, iCompPosX, iCompPosY, iCompWidth, _firsthelpid);
    }


    protected void insertControlGroup(int i){
    try {
        if (i == 0){
            lblSlaveFields = new XFixedText[rowcount];
            lblMasterFields = new XFixedText[rowcount];
            lstSlaveFields = new XListBox[rowcount];
            lstMasterFields = new XListBox[rowcount];
            SOFIRSTLINKLST = 0;
            SOSECLINKLST = 1;
            SOTHIRDLINKLST = 2;
            SOFOURTHLINKLST = 3;
            IListBoxPosX = new Integer(iCompPosX + 6);
            sSlaveListHeader = CurUnoDialog.oResource.getResArray(UIConsts.RID_FORM + 20, 4); //new String[rowcount];""; //CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 40);
            sMasterListHeader = CurUnoDialog.oResource.getResArray(UIConsts.RID_FORM + 24, 4);// new String[rowcount];""; //CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 40);
            SOLINKLST = new int[] { SOFIRSTLINKLST, SOSECLINKLST, SOTHIRDLINKLST, SOFOURTHLINKLST };
        }
        sSlaveHidString = "HID:" + Integer.toString(FirstHelpIndex + (i * 2));
        sMasterHidString = "HID:" + Integer.toString(FirstHelpIndex + (i * 2) + 1);
        boolean bDoEnable = (i < 2);
        lblSlaveFields[i] = CurUnoDialog.insertLabel("lblSlaveFieldLink" + new Integer(i + 1).toString(),
                                new String[] {"Enabled", "Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
                                new Object[] {new Boolean(bDoEnable), new Integer(8), sSlaveListHeader[i], new Integer(97), new Integer(iCurPosY), IStep, new Short(curtabindex++), new Integer(97)} );
        lstSlaveFields[i] = CurUnoDialog.insertListBox("lstSlaveFieldLink" + new Integer(i + 1).toString(), SOLINKLST[i],  null, new ItemListenerImpl(),
                                new String[] {"Dropdown", "Enabled",  "Height", "HelpURL", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
                                new Object[] { Boolean.TRUE, new Boolean(bDoEnable), UIConsts.INTEGER_12, sSlaveHidString,  new Integer(97), new Integer(iCurPosY+10), IStep, new Short(curtabindex++), new Integer(97)});

        lblMasterFields[i] = CurUnoDialog.insertLabel("lblMasterFieldLink" + new Integer(i + 1).toString(),
                                new String[] {"Enabled", "Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
                                new Object[] {new Boolean(bDoEnable), new Integer(8),sMasterListHeader[i], new Integer(206), new Integer(iCurPosY), IStep, new Short(curtabindex++), new Integer(97)});

        lstMasterFields[i] = CurUnoDialog.insertListBox("lstMasterFieldLink" + new Integer(i + 1).toString(), SOLINKLST[i], null, new ItemListenerImpl(),
                                new String[] {"Dropdown", "Enabled", "Height", "HelpURL", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
                                new Object[] { Boolean.TRUE, new Boolean(bDoEnable), UIConsts.INTEGER_12, sMasterHidString, new Integer(206), new Integer(iCurPosY+10), IStep, new Short(curtabindex++), new Integer(97)});
        iCurPosY = iCurPosY + 38;
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    protected void enableNextControlRow(int curindex) {
        setMaxSelIndex();
        boolean bDoEnable = ((lstSlaveFields[curindex].getSelectedItemPos() > 0) && (lstMasterFields[curindex].getSelectedItemPos() > 0));
        if (!bDoEnable)
            moveupSelectedItems(curindex, bDoEnable);
        else
            toggleControlRow(curindex + 1, true);
    }


    protected void setMaxSelIndex(){
        MaxSelIndex = -1;
        for (int i = 0; i < rowcount; i++) {
            if ((lstSlaveFields[i].getSelectedItemPos() > 0) && (lstMasterFields[i].getSelectedItemPos() > 0))
                MaxSelIndex += 1;
        }
    }


    protected void toggleControlRow(int i, boolean bDoEnable){
        if (i < rowcount) {
            Helper.setUnoPropertyValue(UnoDialog.getModel(lblSlaveFields[i]), "Enabled", new Boolean(bDoEnable));
            Helper.setUnoPropertyValue(UnoDialog.getModel(lstSlaveFields[i]), "Enabled", new Boolean(bDoEnable));
            Helper.setUnoPropertyValue(UnoDialog.getModel(lblMasterFields[i]), "Enabled", new Boolean(bDoEnable));
            Helper.setUnoPropertyValue(UnoDialog.getModel(lstMasterFields[i]), "Enabled", new Boolean(bDoEnable));
            if (bDoEnable == false){
                Helper.setUnoPropertyValue(UnoDialog.getModel(lstSlaveFields[i]), "SelectedItems", new short[] {0});
                Helper.setUnoPropertyValue(UnoDialog.getModel(lstMasterFields[i]), "SelectedItems", new short[] {0});
            }
        }
    }


    protected void updateFromNextControlRow(int curindex){
        short iNextMasterItemPos = lstMasterFields[curindex + 1].getSelectedItemPos();
        short iNextSlaveItemPos = lstSlaveFields[curindex + 1].getSelectedItemPos();

        if ((iNextMasterItemPos != 0) && (iNextSlaveItemPos != 0)) {
            Helper.setUnoPropertyValue(UnoDialog.getModel(lstMasterFields[curindex]), "SelectedItems", new short[] { iNextMasterItemPos });
            Helper.setUnoPropertyValue(UnoDialog.getModel(lstSlaveFields[curindex]), "SelectedItems", new short[] { iNextSlaveItemPos });

            Helper.setUnoPropertyValue(UnoDialog.getModel(lstMasterFields[curindex+1]), "SelectedItems", new short[] {0});
            Helper.setUnoPropertyValue(UnoDialog.getModel(lstSlaveFields[curindex+1]), "SelectedItems", new short[] {0});
            toggleControlRow(curindex, true);
        }
    }


    public void initialize(String[] _AllMasterFieldNames, String[] _AllSlaveFieldNames, String[][] _LinkFieldNames) {
        short[] MasterSelList = null;
        short[] SlaveSelList = null;
        String[] MasterLinkNames = JavaTools.ArrayOutOfMultiDimArray(_LinkFieldNames, SOMASTERINDEX);
        String[] SlaveLinkNames = JavaTools.ArrayOutOfMultiDimArray(_LinkFieldNames, SOSLAVEINDEX);
        String[] ViewMasterFieldNames = addNoneFieldItemToList(_AllMasterFieldNames);
        String[] ViewSlaveFieldNames = addNoneFieldItemToList(_AllSlaveFieldNames);
        for (int i = 0; i < super.rowcount; i++) {
            super.initializeListBox(lstMasterFields[i], ViewMasterFieldNames, MasterLinkNames, i);
            super.initializeListBox(lstSlaveFields[i], ViewSlaveFieldNames, SlaveLinkNames, i);
            if (_LinkFieldNames != null)
                toggleControlRow(i, (i <= _LinkFieldNames.length));
            else
                toggleControlRow(i, i == 0);
        }
    }

    public String[][] getLinkFieldNames(CommandMetaData _oCommandMetaData, String _sreferencedtablename){
        return _oCommandMetaData.getKeyColumns(_sreferencedtablename);
    }

    /**
     * @return the LinkFieldnames of the joins. When no LinkFieldNames were selected the returned Array is empty.
     * When Joins were assigned duplicate a null value is returned
     *
     */
    public String[][] getLinkFieldNames() {
        String sLinkFieldsAreDuplicate = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 19);
        setMaxSelIndex();
        String[][] LinkFieldNames = new String[2][MaxSelIndex + 1];
        for (int i = 0; i <= MaxSelIndex; i++) {
            LinkFieldNames[0][i] = lstSlaveFields[i].getSelectedItem();
            LinkFieldNames[1][i] = lstMasterFields[i].getSelectedItem();
        }
        int iduplicate = JavaTools.getDuplicateFieldIndex(LinkFieldNames);
        if (iduplicate != -1) {
            String sLocLinkFieldsAreDuplicate = JavaTools.replaceSubString(sLinkFieldsAreDuplicate, LinkFieldNames[0][iduplicate], "<FIELDNAME1>");
            sLocLinkFieldsAreDuplicate = JavaTools.replaceSubString(sLocLinkFieldsAreDuplicate, LinkFieldNames[1][iduplicate], "<FIELDNAME2>");
            CurUnoDialog.setCurrentStep(FormWizard.SOFIELDLINKERPAGE);
            CurUnoDialog.enableNavigationButtons(true, true, true);
            CurUnoDialog.showMessageBox("WarningBox", VclWindowPeerAttribute.OK, sLocLinkFieldsAreDuplicate);
            CurUnoDialog.setFocus("lstSlaveFieldLink" + (iduplicate + 1));
            return null;
        } else
            return LinkFieldNames;
    }



    public void enable(boolean _bdoenable){
        CurUnoDialog.setStepEnabled(IStep.intValue(), _bdoenable);
    }

    class ItemListenerImpl implements com.sun.star.awt.XItemListener {

        public void itemStateChanged(ItemEvent EventObject) {
            int ikey = CurUnoDialog.getControlKey(EventObject.Source, CurUnoDialog.ControlList);
            enableNextControlRow(ikey);
        }

        public void disposing(com.sun.star.lang.EventObject eventObject) {
        }
    }
}
