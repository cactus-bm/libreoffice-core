/*************************************************************************
 *
 *  $RCSfile: StyleApplier.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $  $Date: 2005-02-21 13:57:25 $
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
 *  Contributor(s): Berend Cornelius
 *
 */
package com.sun.star.wizards.form;
import com.sun.star.awt.ItemEvent;
import com.sun.star.awt.XListBox;
import com.sun.star.awt.XRadioButton;
import com.sun.star.beans.Property;
import com.sun.star.beans.PropertyVetoException;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.drawing.XControlShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.lang.EventObject;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.IndexOutOfBoundsException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.wizards.common.Configuration;
import com.sun.star.wizards.common.Desktop;
import com.sun.star.wizards.common.FileAccess;
import com.sun.star.wizards.common.Helper;
import com.sun.star.wizards.common.JavaTools;
import com.sun.star.wizards.common.NoValidPathException;
import com.sun.star.wizards.document.Control;
import com.sun.star.wizards.document.DatabaseControl;
import com.sun.star.wizards.document.GridControl;
import com.sun.star.wizards.document.TimeStampControl;
import com.sun.star.wizards.text.TextDocument;
import com.sun.star.wizards.text.TextStyleHandler;
import com.sun.star.wizards.ui.*;
import com.sun.star.wizards.ui.UIConsts;

public class StyleApplier{

    WizardDialog CurUnoDialog;
    XPropertySet xPageStylePropertySet;
    XMultiServiceFactory xMSF;
    short curtabindex;
    XRadioButton optNoBorder;
    XRadioButton opt3DLook;
    XRadioButton optFlat;
    XListBox lstStyles;
    Desktop.OfficePathRetriever curofficepath;
//  String[][] sLayoutFiles;
    FormDocument curFormDocument;
    short iOldLayoutPos;
    int SOLAYOUTLST = 0;
    String SCHANGELAYOUT = "changeLayout";
    String SCHANGEBORDERTYPE = "changeBorderLayouts";
    String[] StyleNames;
    String[] StyleNodeNames;
    String[] FileNames;
    String StylesPath;

    final static int SOBACKGROUNDCOLOR = 0;
    final static int SODBTEXTCOLOR = 1;
    final static int SOLABELTEXTCOLOR = 2;
//  final static int SODBCONTROLBACKGROUNDCOLOR = 3;
    final static int SOLABELBACKGROUNDCOLOR = 4;
    final static int SOBORDERCOLOR = 5;


    public StyleApplier(WizardDialog _CurUnoDialog, FormDocument _curFormDocument)  throws NoValidPathException{
    try {
            this.curFormDocument = _curFormDocument;
            xMSF = curFormDocument.xMSF;
            StylesPath = FileAccess.getOfficePath(xMSF, "Config","");
            StylesPath = FileAccess.combinePaths(xMSF, StylesPath, "/wizard/form/styles");
            TextStyleHandler oTextStyleHandler = new TextStyleHandler(xMSF, curFormDocument.xTextDocument);
            xPageStylePropertySet = oTextStyleHandler.getStyleByName("PageStyles", "Standard");
            this.CurUnoDialog = _CurUnoDialog;
            curtabindex = (short) (FormWizard.SOSTYLEPAGE * 100);
            Integer IStyleStep = new Integer(FormWizard.SOSTYLEPAGE);
            String sPageStyles = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 86);
            String sNoBorder = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 29);
            String s3DLook = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 30);
            String sFlat = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 31);
            String sFieldBorder = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 28);
            XInterface xUcbInterface = (XInterface) _curFormDocument.xMSF.createInstance("com.sun.star.ucb.SimpleFileAccess");
            setStyles();
            short[] SelLayoutPos;
                SelLayoutPos = new short[]{0};

            CurUnoDialog.insertLabel("lblStyles",
                  new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
                  new Object[] {UIConsts.INTEGERS[8], sPageStyles, new Integer(92), new Integer(25),IStyleStep, new Short(curtabindex++), new Integer(90)});

            lstStyles = CurUnoDialog.insertListBox("lstStyles",  null, SCHANGELAYOUT, this,
                  new String[] {"Height", "HelpURL", "PositionX", "PositionY", "SelectedItems", "Step", "StringItemList", "TabIndex", "Width"},
                  new Object[] { new Integer(143),"HID:34471", new Integer(92), new Integer(35), SelLayoutPos, IStyleStep, this.StyleNames, new Short(curtabindex++), new Integer(90)});

            optNoBorder = CurUnoDialog.insertRadioButton("otpNoBorder", SCHANGEBORDERTYPE, this,
                  new String[] {"Height", "HelpURL", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Tag", "Width"},
                  new Object[] {UIConsts.INTEGERS[10],"HID:34472",sNoBorder, new Integer(196), new Integer(39),IStyleStep, new Short(curtabindex++), "0", new Integer(93)});

            opt3DLook = CurUnoDialog.insertRadioButton("otp3DLook", SCHANGEBORDERTYPE, this,
                  new String[] {"Height", "HelpURL", "Label", "PositionX", "PositionY", "State", "Step", "TabIndex", "Tag", "Width"},
                 new Object[] {UIConsts.INTEGERS[10],"HID:34473",s3DLook, new Integer(196), new Integer(53), new Short((short)1),IStyleStep, new Short(curtabindex++), "1", new Integer(93)});

            optFlat = CurUnoDialog.insertRadioButton("otpFlat", SCHANGEBORDERTYPE, this,
                  new String[] {"Height", "HelpURL", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Tag", "Width"},
                  new Object[] {UIConsts.INTEGERS[10],"HID:34474",sFlat, new Integer(196), new Integer(67),IStyleStep, new Short(curtabindex++), "2", new Integer(93)});

            CurUnoDialog.insertFixedLine("lnFieldBorder",
                  new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
                  new Object[] {UIConsts.INTEGERS[8], sFieldBorder, new Integer(192), new Integer(25),IStyleStep, new Short(curtabindex++), new Integer(98)});
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


/*  public void initialize(short _iStyleindex){
        if (_iStyleindex < lstStyles.getItemCount()){
            Helper.setUnoPropertyValue(UnoDialog.getModel(lstStyles), "SelectedItems", new short[]{_iStyleindex});
            applyStyle(true, false);
        }
    }
*/


    private void setStyles(){
    try {
        Object oRootNode = Configuration.getConfigurationRoot(xMSF, "org.openoffice.Office.FormWizard/FormWizard/Styles", false);
        XNameAccess xNameAccess = (XNameAccess) UnoRuntime.queryInterface(XNameAccess.class, oRootNode);
        StyleNodeNames = xNameAccess.getElementNames();
        StyleNames = new String[StyleNodeNames.length];
        FileNames = new String[StyleNodeNames.length];
        for (int i = 0; i < StyleNodeNames.length; i++){
             Object oStyleNode = xNameAccess.getByName(StyleNodeNames[i]);
             StyleNames[i] =  (String) Helper.getUnoPropertyValue(oStyleNode, "Name");
             FileNames[i] =  (String) Helper.getUnoPropertyValue(oStyleNode, "CssHref");
        }
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    private short getStyleIndex(){
    try {
        short[] SelFields = (short[]) AnyConverter.toArray(Helper.getUnoPropertyValue(UnoDialog.getModel(lstStyles), "SelectedItems"));
        if (SelFields != null)
            return SelFields[0];
    } catch (IllegalArgumentException e) {
        e.printStackTrace(System.out);
    }
    return (short) -1;
    }



    public void applyStyle( boolean _bmodifyBackground, boolean _bapplyalways){
        short iStyle = getStyleIndex();
        if ((iStyle != iOldLayoutPos) || _bapplyalways){
            if (iStyle > -1){
                iOldLayoutPos = iStyle;
                String sFileName = FileNames[iStyle]; //Style =  lstStyles.getSelectedItem();
                int[] iStyles = getStyleColors(sFileName);
                applyDBControlProperties(iStyles);
            }
        }
    }

    public void changeLayout(){
        short iPos = lstStyles.getSelectedItemPos();
        if (iPos != iOldLayoutPos){
            iOldLayoutPos = iPos;
            String sFileName = FileNames[iPos]; //Style =  lstStyles.getSelectedItem();
            int[] iStyles = getStyleColors(sFileName);
            applyDBControlProperties(iStyles);
        }
        curFormDocument.unlockallControllers();
    }


/*  public void changeLayout(){
/       curFormDocument.xTextDocument.lockControllers();
        applyStyle(true, false);
        curFormDocument.unlockallControllers();
    }
    */


    public void changeBorderLayouts(){
    try {
        curFormDocument.xTextDocument.lockControllers();
        Short IBorderValue = null;
        if (optNoBorder.getState())
            IBorderValue = new Short((short) 0);
        else if (opt3DLook.getState())
            IBorderValue =  new Short((short) 1);
        else
            IBorderValue =  new Short((short) 2);
        for (int m = 0; m < curFormDocument.oControlForms.size(); m++){
            FormDocument.ControlForm curControlForm = ((FormDocument.ControlForm) curFormDocument.oControlForms.get(m));
            if (curControlForm.getArrangemode() == FormWizard.SOGRID){
                GridControl oGridControl = curControlForm.getGridControl();
                oGridControl.xPropertySet.setPropertyValue("Border", IBorderValue);
            }
            else{
                DatabaseControl[] DBControls = curControlForm.getDatabaseControls();
                for (int n = 0; n < DBControls.length; n++){
                    if (DBControls[n].xServiceInfo.supportsService("com.sun.star.drawing.ShapeCollection")){
                        TimeStampControl oTimeStampControl = (TimeStampControl) DBControls[n];
                        for (int i = 0; i < 2; i++){
                            XPropertySet xPropertySet = oTimeStampControl.getControlofGroupShapeByIndex(i);
                            if (xPropertySet.getPropertySetInfo().hasPropertyByName("Border"))
                                xPropertySet.setPropertyValue("Border", IBorderValue);
                        }
                    }
                    else{
                        if (DBControls[n].xPropertySet.getPropertySetInfo().hasPropertyByName("Border"))
                            DBControls[n].xPropertySet.setPropertyValue("Border", IBorderValue);
                    }
                }
            }
        }
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }
    curFormDocument.unlockallControllers();
    }


    public void disposing(EventObject eventObject) {
    }


    private int getStyleColor(String[] _sDataList, String _sHeader, String _sPropertyDescription){
        int iColor = -1;
        int index = JavaTools.FieldInList(_sDataList, _sHeader);
        if (index > -1){
            String sPropName = "";
            int iStyleColor;
            while (((sPropName.indexOf("}") < 0) && (index < _sDataList.length-1))){
                String scurline = _sDataList[index++];
                if ((scurline.indexOf(_sPropertyDescription)) > 0){
                    if (scurline.indexOf(":") > 0){
                        String[] sPropList = JavaTools.ArrayoutofString(scurline, ":");
                        String sPropValue = sPropList[1];
                        sPropValue.trim();
                        if (sPropValue.indexOf("#") > 0){
                            sPropValue = JavaTools.replaceSubString(sPropValue, "", ";");
                            sPropValue = JavaTools.replaceSubString(sPropValue, "", " ");
                            return Integer.decode(sPropValue).intValue();
                        }
                    }
                }
            }
        }
        return -1;
    }


    private int[] getStyleColors(String _filename){
        String sFilePath = this.StylesPath + "/" + _filename;
        int[] oStylePropList = new int[6];
        String[] sData = FileAccess.getDataFromTextFile(xMSF, sFilePath);
        oStylePropList[SOBACKGROUNDCOLOR] = getStyleColor(sData, ".toctitle {", "background-color:");
        oStylePropList[SODBTEXTCOLOR] = getStyleColor(sData, ".doctitle {", "color:");
        oStylePropList[SOLABELTEXTCOLOR] = getStyleColor(sData, ".toctitle {", "color:");
//      oStylePropList[SODBCONTROLBACKGROUNDCOLOR] = getStyleColor(sData, "body {", "background-color:");
//      oStylePropList[SOLABELBACKGROUNDCOLOR] = getStyleColor(sData, ".toctitle {", "background-color:");
        oStylePropList[SOBORDERCOLOR] = getStyleColor(sData, ".tcolor {", "border-color:");
        return oStylePropList;
    }



    private void setDBControlColors(XPropertySet xPropertySet, int[] _iStyleColors){
        try {
            if (xPropertySet.getPropertySetInfo().hasPropertyByName("TextColor")){
                if (_iStyleColors[SODBTEXTCOLOR] > -1)
                    xPropertySet.setPropertyValue("TextColor", Integer.decode("#00000"));
            }
            if (xPropertySet.getPropertySetInfo().hasPropertyByName("BackgroundColor")){
                xPropertySet.setPropertyValue("BackgroundColor", Integer.decode("#DDDDDD"));
            }
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
    }


    public void applyDBControlProperties(int[] _iStyleColors){
    try {
        for (int m = 0; m < curFormDocument.oControlForms.size(); m++){
            FormDocument.ControlForm curControlForm = ((FormDocument.ControlForm) curFormDocument.oControlForms.get(m));
            if (curControlForm.getArrangemode() == FormWizard.SOGRID){
                if (_iStyleColors[SOLABELTEXTCOLOR] > -1)
                    curControlForm.oGridControl.xPropertySet.setPropertyValue("TextColor", new Integer(_iStyleColors[SODBTEXTCOLOR]));
                curControlForm.oGridControl.xPropertySet.setPropertyValue("BackgroundColor", Integer.decode("#DDDDDD"));
            }
            else{
                DatabaseControl[] DBControls = curControlForm.getDatabaseControls();
                for (int n = 0; n < DBControls.length; n++){
                    if (_iStyleColors[SODBTEXTCOLOR] > -1)
                        if (DBControls[n].xServiceInfo.supportsService("com.sun.star.drawing.ShapeCollection")){
                                TimeStampControl oTimeStampControl = (TimeStampControl) DBControls[n];
                                for (int i = 0; i < 2; i++){
                                    XPropertySet xPropertySet = oTimeStampControl.getControlofGroupShapeByIndex(i);
                                    setDBControlColors(xPropertySet, _iStyleColors);
                                }
                        }
                        else
                            setDBControlColors(DBControls[n].xPropertySet, _iStyleColors);
                }
                Control[] LabelControls = curControlForm.getLabelControls();
                for (int n = 0; n < LabelControls.length; n++){
                    if (_iStyleColors[SOLABELTEXTCOLOR] > -1)
                        LabelControls[n].xPropertySet.setPropertyValue("TextColor", new Integer(_iStyleColors[SOLABELTEXTCOLOR]));
//                  if (_iStyleColors[SOCONTROLBACKGROUNDCOLOR] > -1)
//                      LabelControls[n].xPropertySet.setPropertyValue("BackgroundColor", new Integer(_iStyleColors[SOCONTROLBACKGROUNDCOLOR]));
                }
            }
        }
        xPageStylePropertySet.setPropertyValue("BackColor", new Integer(_iStyleColors[SOBACKGROUNDCOLOR]));
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}
}
