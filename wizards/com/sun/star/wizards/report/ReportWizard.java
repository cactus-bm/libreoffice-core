/*************************************************************************
 *
 *  $RCSfile: ReportWizard.java,v $
 *
 *  $Revision: 1.63 $
 *
 *  last change: $Author: hr $ $Date: 2005-04-06 10:18:35 $
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
 */
package com.sun.star.wizards.report;

import java.util.Vector;

import com.sun.star.awt.TextEvent;
import com.sun.star.awt.XTextListener;
import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.EventObject;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.CommandType;
import com.sun.star.uno.AnyConverter;
import com.sun.star.wizards.common.*;
import com.sun.star.wizards.document.OfficeDocument;
import com.sun.star.wizards.ui.*;
import com.sun.star.wizards.db.*;


public class ReportWizard extends WizardDialog implements XTextListener, XCompletion{
    XMultiServiceFactory xMSF;
    QueryMetaData CurDBMetaData;
    FieldSelection CurGroupFieldSelection;
    SortingComponent CurSortingComponent;
    UnoDialog CurUnoProgressDialog;
    TitlesComponent CurTitlesComponent;
    CommandFieldSelection CurDBCommandFieldSelection;
    GroupFieldHandler CurGroupFieldHandler;
    ReportLayouter CurReportLayouter;
    Finalizer CurReportFinalizer;
    public static String ReportPath;
    PropertyValue[] DBGPROPERTYVALUE;
    String sCommandName = "";
    int nCommandType = -1;
    int nReportMode = Finalizer.SOCREATEDOCUMENT;
    String sReportName = "";

    public static final String SOREPORTFORMNAME = "ReportSource";
    final int SOSELGROUPLST = 33;
    final int SOTXTCOLTITLE = 48;
    final int SOTITLESCROLLBAR = 49;

    public static final int SONULLPAGE = 0;
    public static final int SOMAINPAGE = 1;
    public static final int SOTITLEPAGE = 2;
    public static final int SOGROUPPAGE = 3;
    public static final int SOSORTPAGE = 4;
    public static final int SOTEMPLATEPAGE = 5;
    public static final int SOSTOREPAGE = 6;

    ReportDocument CurReportDocument;

    static String sMsgWizardName;
    static String slblFields;
    static String slblSelFields;
    static String sShowBinaryFields;

    static String sGroupings;
    String[] WizardHeaderText = new String[6];
    static String[] WizardTitle = new String[6];
    static String sWriterFilterName;
    static String slstDatabasesDefaultText;
    static String slstTablesDefaultText;
    static String sMsgErrorOccured;
    static String sMsgSavingImpossible;
    static String sMsgNoConnection;
    // Progress display relevant Strings
    static String slblColumnTitles;
    static String slblColumnNames;
    static String sMsgNoConnectionforDataimport;
    static String sMsgQueryCreationImpossible;
    public static String sMsgFilePathInvalid;
    static String slblTables;
    public static String sBlindTextNote;
    public static boolean bCloseDocument;



    public ReportWizard(XMultiServiceFactory xMSF) {
        super(xMSF, 34320);
        super.addResourceHandler("Report Wizard", "dbw");
        if (getReportResources( false) == true){
            Helper.setUnoPropertyValues(xDialogModel,
            new String[] { "Height","Moveable","Name","PositionX","PositionY","Step","TabIndex","Title","Width"},
            new Object[] { new Integer(210),Boolean.TRUE, "DialogReport", new Integer(102),new Integer(41),new Integer(1), new Short((short)0), sMsgWizardName, new Integer(310)}  );
            drawNaviBar();
            setRightPaneHeaders(this.WizardHeaderText);
        }
    }


    protected void enterStep(int nOldStep, int nNewStep){
        if ((nOldStep >= SOTEMPLATEPAGE) && (nNewStep < SOTEMPLATEPAGE)){
            CurReportDocument.oTextSectionHandler.removeTextSectionbyName("RecordSection");
            CurReportDocument.oTextTableHandler.removeTextTablebyName("Tbl_RecordSection");
        }
        switch (nNewStep){
            case SOMAINPAGE:
                CurDBCommandFieldSelection.setModified(false);
                break;
            case SOTITLEPAGE:
                CurTitlesComponent.initialize(CurReportDocument.CurDBMetaData.FieldNames, CurReportDocument.CurDBMetaData.FieldTitleSet);
                break;
            case SOGROUPPAGE:
                CurGroupFieldHandler.initialize();
                break;
            case SOSORTPAGE:
                CurSortingComponent.initialize(CurReportDocument.CurDBMetaData.FieldNames, CurReportDocument.CurDBMetaData.SortFieldNames);
                CurSortingComponent.setReadOnlyUntil(CurReportDocument.CurDBMetaData.GroupFieldNames.length, false);
                break;
            case SOTEMPLATEPAGE:
                break;
            case SOSTOREPAGE:
                //TODO initialize with suitable PathName
                CurReportFinalizer.initialize(CurReportDocument.CurDBMetaData);
                break;
             default:
             break;
        }
    }

    protected void leaveStep(int nOldStep, int nNewStep){

         switch (nOldStep){
            case SOMAINPAGE:
                CurReportDocument.CurDBMetaData.setFieldNames(CurDBCommandFieldSelection.getSelectedFieldNames());
                CurReportDocument.CurDBMetaData.setAllIncludedFieldNames(false);
                CurReportDocument.CurDBMetaData.setFieldColumns(true);
                if (CurDBCommandFieldSelection.isModified()){
                    CurReportDocument.oTextSectionHandler.removeAllTextSections(false);
                    CurReportDocument.oTextTableHandler.removeAllTextTables();
                    CurReportDocument.DBColumnsVector = new Vector();
                    CurReportDocument.CurDBMetaData.setGroupFieldNames(new String[]{});
                    CurGroupFieldHandler.removeGroupFieldNames();
                 }
                break;
            case SOTITLEPAGE:
                CurReportDocument.CurDBMetaData.FieldTitleSet = CurTitlesComponent.getFieldTitles();
                break;
            case SOGROUPPAGE:
                CurGroupFieldHandler.getGroupFieldNames(CurReportDocument.CurDBMetaData);
                CurReportDocument.CurDBMetaData.prependSortFieldNames(CurReportDocument.CurDBMetaData.GroupFieldNames);
                break;
            case SOSORTPAGE:
                CurReportDocument.CurDBMetaData.SortFieldNames = CurSortingComponent.getSortFieldNames();
                super.enablefromStep(SOTEMPLATEPAGE, true);
                break;
            case SOTEMPLATEPAGE:
                break;
            case SOSTOREPAGE:
                break;
             default:
             break;
          }
        if ((nOldStep < SOTEMPLATEPAGE) && (super.getNewStep() >= SOTEMPLATEPAGE)){
            CurReportDocument.CurDBMetaData.setRecordFieldNames();
            CurReportLayouter.initialize(ReportPath + "/cnt-default.ott");
        }
    }


    private boolean executeQuery(){
        boolean bQueryCreated = false;
        if (this.CurDBCommandFieldSelection.getSelectedCommandType() == CommandType.TABLE)
            bQueryCreated = CurDBMetaData.oSQLQueryComposer.setQueryCommand(sMsgWizardName, this.xWindow, false, false);
        else{
            try {
                DBMetaData.CommandObject oCommand = CurDBMetaData.getQueryByName(CurDBCommandFieldSelection.getSelectedCommandName());
                String sCommand = (String) oCommand.xPropertySet.getPropertyValue("Command");
                bQueryCreated = (!sCommand.equals(""));
                CurDBMetaData.oSQLQueryComposer.xQueryAnalyzer.setQuery(sCommand);
                CurDBMetaData.oSQLQueryComposer.prependSortingCriteria();
                CurDBMetaData.Command = CurDBMetaData.oSQLQueryComposer.getQuery();
            } catch (Exception e) {
                e.printStackTrace(System.out);
            }
        }
        if (bQueryCreated){
            CurDBMetaData.Command = CurDBMetaData.oSQLQueryComposer.getQuery();
            CurReportLayouter.initialize(ReportPath + "/cnt-default.ott");
        }
        else
            super.vetoableChange(null);
        return bQueryCreated;
    }



    public static void main(String args[]) {
    String ConnectStr = "uno:socket,host=localhost,port=8100;urp,negotiate=0,forcesynchronous=1;StarOffice.NamingService";   //localhost  ;Lo-1.Germany.sun.com; 10.16.65.155
    try {
        XMultiServiceFactory xLocMSF = com.sun.star.wizards.common.Desktop.connect(ConnectStr);
        ReportWizard CurReportWizard = new ReportWizard(xLocMSF);
        if(xLocMSF != null){
            System.out.println("Connected to "+ ConnectStr);
            PropertyValue[] curproperties = new PropertyValue[3];
            curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///C:/Documents and Settings/bc93774.EHAM02-DEV/My Documents/MyDocAssign.odb"); //baseLocation ); "DataSourceName", "db1");
            curproperties[0] = Properties.createProperty("DataSourceName", "Bibliography");
            curproperties[1] = Properties.createProperty("CommandType", new Integer(CommandType.QUERY));
            curproperties[2] = Properties.createProperty("Command", "Query1");
            CurReportWizard.startReportWizard(xLocMSF, curproperties);
        }
    }
    catch(Exception exception){
        exception.printStackTrace(System.out);
    }}


    public void buildSteps(){
        boolean bEnabled;
        CurReportDocument.xProgressBar.setValue(30);
        CurDBCommandFieldSelection = new CommandFieldSelection(this, CurReportDocument.CurDBMetaData, 100, slblFields, slblSelFields,  slblTables, true, 34330);
        CurDBCommandFieldSelection.addFieldSelectionListener(new FieldSelectionListener());
        insertLabel("lblBinaryFields",
            new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "Width"},
            new Object[] {new Integer(16), sShowBinaryFields, new Integer(95), new Integer(162), new Integer(1), new Integer(210)});
        CurReportDocument.xProgressBar.setValue(40);
        CurTitlesComponent = new TitlesComponent(this, SOTITLEPAGE, 97, 37, 210, 7, slblColumnNames, slblColumnTitles, 34381);
        CurTitlesComponent.addTextListener(this);
        CurReportDocument.xProgressBar.setValue(50);
        CurGroupFieldHandler = new GroupFieldHandler(CurReportDocument, this);
        CurReportDocument.xProgressBar.setValue(60);
        CurSortingComponent = new SortingComponent(this, SOSORTPAGE, 95, 30, 210, 34346);
        CurReportDocument.xProgressBar.setValue(70);
        CurReportLayouter = new ReportLayouter(CurReportDocument, this);
        CurReportDocument.xProgressBar.setValue(80);
        CurReportFinalizer = new Finalizer(CurReportDocument, this);
        CurReportDocument.xProgressBar.setValue(100);
        bCloseDocument = true;
        CurReportDocument.xProgressBar.end();
        enableNavigationButtons(false, false, false);
    }


    public void finishWizard(){
        int a = 0;
        int ncurStep = getCurrentStep();
        if ((switchToStep(ncurStep, SOSTOREPAGE)) || (ncurStep == SOSTOREPAGE)){
            if (this.executeQuery()){
                if (CurReportFinalizer.finish()){
                    nReportMode = CurReportFinalizer.getReportOpenMode();
                    sReportName = CurReportFinalizer.getStoreName();
                    xDialog.endExecute();
                }
            }
        }
    }


    public void cancelWizard(){
        xDialog.endExecute();
    }


    public void insertQueryRelatedSteps(){
        setRMItemLabels(oResource, UIConsts.RID_QUERY + 80);
        addRoadmap();
        int i = 0;
        i = insertRoadmapItem(0, true, oResource.getResText(UIConsts.RID_QUERY + 80), SOMAINPAGE);
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_REPORT + 68), SOTITLEPAGE);
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_REPORT + 11), SOGROUPPAGE);
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_REPORT + 12),  SOSORTPAGE);       // Orderby is always supported
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_REPORT + 13), SOTEMPLATEPAGE);
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_REPORT + 14), SOSTOREPAGE);
        setRoadmapInteractive(false);
        setRoadmapComplete(true);
        setCurrentRoadmapItemID((short) 1);
    }


    public void startReportWizard(XMultiServiceFactory _xMSF, PropertyValue[] CurPropertyValue){
    try{
        this.xMSF = _xMSF;
        // Check general availability of office paths
        ReportPath = FileAccess.getOfficePath(xMSF, "Template","share");
        ReportPath = FileAccess.combinePaths(xMSF, ReportPath, "/wizard/report");
        if (ReportPath.equals(""))
            return;
        DBGPROPERTYVALUE = CurPropertyValue;
        CurReportDocument =  new ReportDocument(xMSF, true, oResource);
        CurDBMetaData = CurReportDocument.CurDBMetaData;
        if (CurDBMetaData.getConnection(CurPropertyValue)){
            CurReportDocument.xProgressBar.setValue(20);
            CurReportDocument.oTextStyleHandler.loadStyleTemplates(ReportPath + "/stl-default.ott", "LoadPageStyles");
            CurDBMetaData.oSQLQueryComposer = new SQLQueryComposer(CurReportDocument.CurDBMetaData);
            buildSteps();
            this.CurDBCommandFieldSelection.preselectCommand(CurPropertyValue, false);
            createWindowPeer(CurReportDocument.xWindowPeer);
            CurDBMetaData.setWindowPeer(this.xControl.getPeer());
    //      setAutoMnemonic("lblDialogHeader", false);
            insertQueryRelatedSteps();
            short RetValue = executeDialog(CurReportDocument.xFrame.getComponentWindow().getPosSize());
            boolean bdisposeDialog = true;
            switch (RetValue){
                case 0:                         // via Cancelbutton or via sourceCode with "endExecute"
                    this.xComponent.dispose();
                    if (bCloseDocument == true){
                        OfficeDocument.dispose(xMSF, CurReportDocument.xComponent);
                        return;
                    }
                    if ((nReportMode == Finalizer.SOCREATETEMPLATE) || (nReportMode == Finalizer.SOUSETEMPLATE)) {
                        bdisposeDialog = false;
                        CurReportDocument.CurDBMetaData.addReportDocument(CurReportDocument.xComponent, true);
                        boolean bOpenInDesign = (nReportMode == Finalizer.SOCREATETEMPLATE);
                        CurDBMetaData.openReportDocument(sReportName, true, bOpenInDesign);
                    }
                    else {
                        bdisposeDialog = false;
                        Dataimport CurDataimport = new Dataimport(xMSF);
                        CurDataimport.CurReportDocument = CurReportDocument;
                        CurDataimport.showProgressDisplay(xMSF, false);
                        importReportData(xMSF, CurDataimport);
                        CurDBMetaData.openReportDocument(sReportName, false, false);
                    }
                    return;
                case 1:
                    if (bdisposeDialog == true)
                        CurReportDocument.unlockallControllers();
                    break;
            }
        }
        CurDBMetaData.dispose();
    }
    catch(java.lang.Exception jexception ){
        jexception.printStackTrace(System.out);
    }}


    public void importReportData(final XMultiServiceFactory xMSF, final Dataimport CurDataimport){
        boolean bDocisStored = false;
        try{
            if (CurReportDocument.CurDBMetaData.executeCommand(sMsgQueryCreationImpossible + (char) 13 + sMsgEndAutopilot, false))
                CurDataimport.insertDatabaseDatatoReportDocument(xMSF);
                if (CurReportFinalizer.getReportOpenMode() == Finalizer.SOCREATEDOCUMENT){
                    bDocisStored = CurReportDocument.CurDBMetaData.storeDatabaseDocumentToTempPath(CurReportDocument.xComponent, CurReportFinalizer.getStoreName());
                }
            }
        catch (com.sun.star.wizards.common.InvalidQueryException queryexception){
        }
        CurDataimport.xComponent.dispose();
        if (bDocisStored)
            CurReportDocument.CurDBMetaData.addReportDocument(CurReportDocument.xComponent, false);
        CurReportDocument.CurDBMetaData.dispose();
        }


    public boolean getReportResources(boolean bgetProgressResourcesOnly){
        sMsgWizardName = super.oResource.getResText(UIConsts.RID_REPORT);
        if (bgetProgressResourcesOnly == false){
            sShowBinaryFields = oResource.getResText(UIConsts.RID_REPORT + 60);
            slstDatabasesDefaultText = oResource.getResText(UIConsts.RID_DB_COMMON + 37);
            slstTablesDefaultText = oResource.getResText(UIConsts.RID_DB_COMMON + 38);
            sMsgErrorOccured = oResource.getResText(UIConsts.RID_DB_COMMON + 6);
            slblTables = oResource.getResText(UIConsts.RID_FORM + 6);
            slblFields = oResource.getResText(UIConsts.RID_FORM + 12);
            slblSelFields = oResource.getResText(UIConsts.RID_REPORT + 9);
            WizardHeaderText[0] = oResource.getResText(UIConsts.RID_REPORT + 28);
            WizardHeaderText[1] = oResource.getResText(UIConsts.RID_REPORT + 69);
            WizardHeaderText[2] = oResource.getResText(UIConsts.RID_REPORT + 29);
            WizardHeaderText[3] = oResource.getResText(UIConsts.RID_REPORT + 30);
            WizardHeaderText[4] = oResource.getResText(UIConsts.RID_REPORT + 31);
            WizardHeaderText[5] = oResource.getResText(UIConsts.RID_REPORT + 32);
            sMsgSavingImpossible = oResource.getResText(UIConsts.RID_DB_COMMON + 30);
        }
        sMsgFilePathInvalid = oResource.getResText(UIConsts.RID_DB_COMMON + 36);
        slblColumnTitles = oResource.getResText(UIConsts.RID_REPORT + 70);
        slblColumnNames = oResource.getResText(UIConsts.RID_REPORT + 71);
        sBlindTextNote = oResource.getResText(UIConsts.RID_REPORT + 75);
        sBlindTextNote = JavaTools.replaceSubString( sBlindTextNote, String.valueOf((char)13), "<BR>");
        return true;
    }


    public void enableRoadmapItems(boolean _bEnabled ){
    try{
        Object oRoadmapItem = null;
        int CurStep = AnyConverter.toInt(Helper.getUnoPropertyValue(xDialogModel, "Step"));
        boolean bEnabled = false;
        int CurItemID;
        for (int i = 0; i < getRMItemCount(); i++){
            oRoadmapItem = this.xIndexContRoadmap.getByIndex(i);
            CurItemID = AnyConverter.toInt(Helper.getUnoPropertyValue(oRoadmapItem, "ID"));
            if (CurItemID > CurStep)
                bEnabled = _bEnabled;
            else
                bEnabled = true;
            Helper.setUnoPropertyValue(oRoadmapItem , "Enabled", new Boolean(bEnabled));
        }
    }
    catch( com.sun.star.uno.Exception exception ){
        exception.printStackTrace(System.out);
    }}



    private void enableWizardSteps(String[] NewItems){
        boolean bEnabled = NewItems.length > 0;
        setControlProperty("btnWizardNext", "Enabled", new Boolean(bEnabled));
        setControlProperty("btnWizardFinish", "Enabled", new Boolean(bEnabled));
        enableRoadmapItems(bEnabled);   // Note: Performancewise this could be improved
    }


    public void textChanged(TextEvent xTextEvent) {
    try {
        Object otitlemodel = UnoDialog.getModel(xTextEvent.Source);
        String sfieldtitle = (String) Helper.getUnoPropertyValue(otitlemodel, "Text");
        String fieldname = this.CurTitlesComponent.getFieldNameByTitleControl(otitlemodel);
        CurReportDocument.oTextFieldHandler.changeUserFieldContent(fieldname, sfieldtitle);
    } catch (Exception exception) {
        exception.printStackTrace(System.out);
    }}

    public void disposing(EventObject EventObject) {
    }


    public void setmodified(int _ndialogpage, Object ooldValue, Object onewValue) {
        switch(_ndialogpage){
            case SOMAINPAGE:
                break;
            default:
                break;
        }
    }


    private void toggleSortingPage(){
        int nCommandType = this.CurDBCommandFieldSelection.getSelectedCommandType();
        super.setStepEnabled(SOSORTPAGE, (nCommandType == CommandType.TABLE));
    }


    public class FieldSelectionListener implements com.sun.star.wizards.ui.XFieldSelectionListener{
         protected int ID;

         public int getID(){
             return ID;
         }

         public void setID(String sIncSuffix){
             ID = 1;
             if (sIncSuffix != null){
                if ((!sIncSuffix.equals("")) && (!sIncSuffix.equals("_"))) {
                    String sID = JavaTools.ArrayoutofString(sIncSuffix, "_")[1];
                    ID = Integer.parseInt(sID);
                    int a = 0;
                 }
             }
         }

         public void shiftFromLeftToRight(String[] SelItems, String[] NewItems) {
             if (ID == 1){
                 CurDBCommandFieldSelection.setModified(true);
                enableWizardSteps(NewItems);
                toggleSortingPage();
             }
             else{
                 boolean bEnabled = (CurGroupFieldSelection.getSelectedFieldNames().length > 0);
                 Helper.setUnoPropertyValue(getRoadmapItemByID(SOGROUPPAGE), "Enabled", new Boolean(bEnabled));
             }
         }


         public void shiftFromRightToLeft(String[] SelItems, String[] NewItems ) {
             // TODO When the ListFieldbox is refilled only fields of the current Command may be merged into the Listbox
             if (ID == 1){
                enableWizardSteps(NewItems);
                CurDBCommandFieldSelection.setModified(true);
             }
             else{
                 boolean bEnabled = (CurGroupFieldSelection.getSelectedFieldNames().length > 0);
                 Helper.setUnoPropertyValue(getRoadmapItemByID(SOGROUPPAGE), "Enabled", new Boolean(bEnabled));
             }
         }

         public void moveItemDown(String item){
         }

         public void moveItemUp(String item){
         }
     }
}
