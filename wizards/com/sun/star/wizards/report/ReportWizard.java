 /*************************************************************************
 *
 *  $RCSfile: ReportWizard.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: bc $ $Date: 2002-05-15 15:04:29 $
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

import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XElementAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XEnumeration;
import com.sun.star.container.XNameContainer;

import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.XMultiPropertySet;

import com.sun.star.comp.servicemanager.ServiceManager;

import com.sun.star.connection.XConnector;
import com.sun.star.connection.XConnection;
import com.sun.star.sdb.XCompletedConnection;
import com.sun.star.sdbcx.XColumnsSupplier;
import com.sun.star.sdbc.*;
import com.sun.star.sdb.*;
import com.sun.star.sdbcx.XTablesSupplier;
import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.ucb.XSimpleFileAccess;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.XNamingService;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Any;
import com.sun.star.sheet.*;
import com.sun.star.document.*;
import com.sun.star.table.*;
import com.sun.star.text.*;
import com.sun.star.text.XTextRange;
import com.sun.star.text.XTextDocument;
import com.sun.star.task.XInteractionHandler;

import com.sun.star.util.XNumberFormats;
import com.sun.star.util.XNumberFormatsSupplier;
import com.sun.star.util.NumberFormat;

import com.sun.star.lang.Locale;
import com.sun.star.lang.*;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XComponent;

import com.sun.star.lang.EventObject;
import com.sun.star.script.XInvocation;
import com.sun.star.awt.XListBox;
import com.sun.star.awt.XRadioButton;
import com.sun.star.awt.XControl;
import com.sun.star.awt.XControlModel;
import com.sun.star.awt.XControlContainer;
import com.sun.star.awt.XWindow;
import com.sun.star.awt.XToolkit;
import com.sun.star.awt.XDialog;
import com.sun.star.awt.XButton;
import com.sun.star.awt.XActionListener;
import com.sun.star.awt.XItemListener;
import com.sun.star.awt.ItemEvent;
import com.sun.star.awt.ActionEvent;
import com.sun.star.awt.XMouseListener;
import com.sun.star.awt.XTextComponent;
import com.sun.star.awt.MouseEvent;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XComponentLoader;

import java.io.*;
import java.util.*;


/**
 *
 * @author  bc93774
 * @version
 */
public class ReportWizard {
    static long iStart;
    static XMultiServiceFactory xMSF;
    static Object xListBoxModel;
    static Object oDialogModel;
    static XNameAccess xDlgNameAccess;
    static Hashtable ControlList;
    static XNameContainer xDlgNames;
    static XStatement xStatement;
    static XDatabaseMetaData xDBMetaData;
    static XResultSet xResultSet;
    static XTextTable xTextTable;
    static DBMetaData.CommandMetaData CurDBMetaData;
    static String ReportFolderName;
    static final int SOCMDCANCEL = 1;
    static final int SOCMDHELP = 2;
    static final int SOCMDBACK = 3;
    static final int SOCMDGOON = 4;
    static final int SOCMDMOVESEL = 5;
    static final int SOCMDMOVEALL = 6;
    static final int SOCMDREMOVESEL = 7;
    static final int SOCMDREMOVEALL = 8;
    static final int SOFLDSLST =  9;
    static final int SODBLST =  10;
    static final int SOTBLLST =  11;
    static final int SOSELFLDSLST =  12;
    static final int SOGROUPLST = 13;

    static final int SOFIRSTSORTLST = 14;
    static final int SOSECSORTLST = 15;
    static final int SOTHIRDSORTLST = 16;
    static final int SOFOURTHSORTLST = 17;
    static final int[] SOSORTLST = new int[]{SOFIRSTSORTLST, SOSECSORTLST, SOTHIRDSORTLST, SOFOURTHSORTLST};

    static final int SOFIRSTSORTCMD = 18;
    static final int SOSECSORTCMD = 19;
    static final int SOTHIRDSORTCMD = 20;
    static final int SOFOURTHSORTCMD = 21;
    static final int[] SOSORTCMD = new int[]{SOFIRSTSORTCMD, SOSECSORTCMD, SOTHIRDSORTCMD, SOFOURTHSORTCMD};
    static final int SOCMDGROUPIN = 22;
    static final int SOCMDGROUPOUT = 23;
    static final int SOTXTTITLE = 24;
    static final int SOCONTENTLST = 25;

    static final int SOFIRSTARRANGECMD = 25;
    static final int SOSECARRANGECMD = 26;
    static final int SOTHIRDARRANGECMD = 27;
    static final int SOFOURTHARRANGECMD = 28;
    static final int[] SOARRANGECMD = new int[]{SOFIRSTARRANGECMD, SOSECARRANGECMD, SOTHIRDARRANGECMD, SOFOURTHARRANGECMD};

    static final int SOOPTLANDSCAPE = 29;
    static final int SOOPTPORTRAIT = 30;
    static final int SOLAYOUTLST = 31;

    static int MaxSortIndex = -1;
    static String[] sDatabaseList;
    static String[] TableNames;
    static String[] QueryNames;
    static String[][] LayoutFiles = new String[2][];
    static String[][] ContentFiles = new String[2][];
    static XListBox xDBListBox;
    static XListBox xTableListBox;
    static XListBox xFieldsListBox;
    static XListBox xSelFieldsListBox;
    static XListBox xContentListBox;

    static XListBox[] xSortListBox = new XListBox[4];
    static XListBox xGroupListBox;
    static XListBox xLayoutListBox;
    static XTextComponent xTitleTextBox;
    static XTablesSupplier xDBTables;
    static XNameAccess xTableNames;
    static XNameAccess xQueryNames;
    static String TableName;
    static Object oDBTable;
    static int[] iCommandTypes;
    static int[][] WidthList;
    static boolean bEnableBinaryOptionGroup;
    static String[] OriginalList = new String[]{""};
    static XNameAccess xColumns;
    static XMultiServiceFactory  xMSFDialogModel;
    static XControlContainer xDialogContainer;
    static XDesktop xDesktop;
    static ReportDocument.RepWizardDocument CurReportDocument;
    static java.util.Vector GroupFieldVector;
    static String CurGroupName;
    static String TemplatePath;
    static boolean bGroupByIsSupported;
    static String sMsgWizardName;
    static String scmdReady;
    static String scmdCancel;
    static String scmdBack;
    static String scmdHelp;
    static String scmdGoOn;
    static String slblDatabases;
    static String slblTables;
    static String slblFields;
    static String slblSelFields;
    static String slblDataStructure;
    static String slblPageLayout;
    static String sOrganizeFields;
    static String sSortHeader;
    static String sNoSorting;
    static String sOrientationHeader;
    static String sOrientVertical;
    static String sOrientHorizontal;
    static String sReportTitle;
    static String sSortAscend;
    static String sSortDescend;
    static String[] WizardTitle = new String[5];
    static String sWriterFilterName;


    public ReportWizard() {
    }

    private static int getControlKey(Object EventObject){
          int iKey;
          XControl xControl = (XControl) UnoRuntime.queryInterface(XControl.class, EventObject);
          XControlModel xControlModel = xControl.getModel();
          XPropertySet xPSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xControlModel);
          try{
              String sName = (String) xPSet.getPropertyValue("Name");
              Integer KeyObject = (Integer) ControlList.get(sName);
              iKey = KeyObject.intValue();
          }
          catch( Exception exception )
          {
               System.err.println( exception);
               iKey = 2000;
          }
          return iKey;
    }


    private static void getSelectedDBMetaData()
        throws com.sun.star.container.NoSuchElementException, com.sun.star.beans.UnknownPropertyException,
               com.sun.star.lang.IllegalArgumentException, com.sun.star.lang.WrappedTargetException{
        try{
            short DBIndex = xDBListBox.getSelectedItemPos();
            String sDBName = sDatabaseList[DBIndex];
            if (DBMetaData.getConnection(CurReportDocument, sDBName) == true){
        CurDBMetaData.DataSourceName = sDBName;
                if (DBMetaData.getDBMetaData(CurReportDocument) == true){
                    UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "lstTables", "Enabled", new Boolean(true));
                    UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "lblTables", "Enabled", new Boolean(true));
                    iCommandTypes = DBMetaData.createCommandTypeList();
                    UNODialogs.emptyFieldsListBoxes(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox);
                }
            }
        }
        catch(Exception exception)
        {
            exception.printStackTrace(System.out);
        }
    }


    private static void FillUpFieldsListbox(boolean bgetCommandType){
        try{
    if (xTableListBox.getSelectedItems().length > 0) {
           int CurCommandType;
           String TableName;
           int SelIndex = xTableListBox.getSelectedItemPos();
           if (bgetCommandType){
               CurCommandType = iCommandTypes[SelIndex];
               if (CurCommandType == com.sun.star.sdb.CommandType.QUERY){
                    int QueryIndex = SelIndex - TableNames.length - 1;
                    TableName = QueryNames[QueryIndex];
                    oDBTable = xQueryNames.getByName(TableName);
                }
                else{
                    TableName = TableNames[SelIndex];
                    oDBTable = xTableNames.getByName(TableName);
                }
           }
           XColumnsSupplier xTableCols = (XColumnsSupplier) UnoRuntime.queryInterface(XColumnsSupplier.class, oDBTable);
           xColumns = (XNameAccess) UnoRuntime.queryInterface(XNameAccess.class, xTableCols.getColumns());
           if (DBMetaData.getSpecificFieldNames() > 0)
               UNODialogs.toggleListboxControls(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, new Boolean(true));
           else
               UNODialogs.emptyFieldsListBoxes(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox);
        }
        else
            UNODialogs.emptyFieldsListBoxes(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox);
        }
        catch(Exception exception){
            exception.printStackTrace(System.out);
        }
    }


    static class TextListenerImpl implements com.sun.star.awt.XTextListener{

    public void textChanged(com.sun.star.awt.TextEvent EventObject){
        //XControl xControl = (XControl) UnoRuntime.queryInterface(XControl.class, EventObject);
        String TitleName = xTitleTextBox.getText();
        ReportDocument.updateReportTitle(CurReportDocument, xTitleTextBox);
    }

    public void disposing(com.sun.star.lang.EventObject EventObject) {
        }
    }



   static class ItemListenerImpl implements com.sun.star.awt.XItemListener{

        // XEventListener
       public void itemStateChanged(ItemEvent EventObject ){
       int iPos;
    try{
            int iKey  =  getControlKey(EventObject.Source);
            switch (iKey) {
              case SODBLST:
                 getSelectedDBMetaData();
                 break;
              case SOTBLLST:
                 FillUpFieldsListbox(true);
                 TableName = xTableListBox.getSelectedItem();
                 break;

             case SOFLDSLST:
                UNODialogs.FormSetMoveRights(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, (short)-1, (short)-1);
                break;

             case SOSELFLDSLST:
                UNODialogs.FormSetMoveRights(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, (short)-1, (short)-1);
                break;

             case SOFIRSTSORTLST:
                enableNextSortListBox(0);
                break;

             case SOSECSORTLST:
                enableNextSortListBox(1);
                break;

             case SOTHIRDSORTLST:
                enableNextSortListBox(2);
                break;

             case SOFOURTHSORTLST:
                MaxSortIndex = 3;
                break;

             case SOGROUPLST:
         boolean bDoEnable = ((xGroupListBox.getSelectedItems().length > 0) && (GroupFieldVector.size() < 4));
         UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGroupOut", "Enabled", new Boolean(bDoEnable));

               break;
         case SOCONTENTLST:
        CurReportDocument.ReportTextDocument.lockControllers();
        iPos = xContentListBox.getSelectedItemPos();
        ReportDocument.loadSectionsfromTemplate(CurReportDocument, CurDBMetaData, ContentFiles[0][iPos]);
        ReportDocument.loadStyleTemplates(CurReportDocument, ContentFiles[0][iPos], "LoadTextStyles");
        CurReportDocument.ReportTextDocument.unlockControllers();
           break;

         case SOLAYOUTLST:
        CurReportDocument.ReportTextDocument.lockControllers();
        iPos = xLayoutListBox.getSelectedItemPos();
            boolean bOldIsCurLandscape = ((Boolean) tools.getUNOPropertyValue(CurReportDocument.ReportPageStyle, "IsLandscape")).booleanValue();
        ReportDocument.loadStyleTemplates(CurReportDocument, LayoutFiles[0][iPos], "LoadPageStyles");
        ReportDocument.changePageOrientation(CurReportDocument, bOldIsCurLandscape);
        CurReportDocument.ReportTextDocument.unlockControllers();
        break;

            default:
//               System.err.println( exception);
               break;
           }
        }
        catch( Exception exception ){
            System.err.println( exception);
        }
     }

        public void disposing(EventObject eventObject) {
          System.out.println( getClass().getName() + ".disposing" + eventObject);
        }

    }


    static class ActionListenerImpl implements com.sun.star.awt.XActionListener {

        // XEventListener
        public void disposing(EventObject eventObject) {
        System.out.println( getClass().getName() + ".disposing:" + eventObject);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            try{
                int iKey  =  getControlKey(actionEvent.Source);
                switch (iKey) {
                    case SOFLDSLST:
                       UNODialogs.MoveOrderedSelectedListBox(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, CurDBMetaData.FieldNames,  false);
                       break;

                    case SOCMDMOVESEL:
                        UNODialogs.MoveSelectedListBox(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox);
                        break;

                    case SOCMDMOVEALL:
                        UNODialogs.FormMoveAll(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, CurDBMetaData.FieldNames);
                        break;

                    case SOCMDREMOVESEL:
                        UNODialogs.MoveOrderedSelectedListBox(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, CurDBMetaData.FieldNames, false);
                        break;

                    case SOCMDREMOVEALL:
                        UNODialogs.MoveOrderedSelectedListBox(xDlgNameAccess, xFieldsListBox, xSelFieldsListBox, CurDBMetaData.FieldNames, true);
                        break;

                    case SOCMDGOON:
                        gotoNextStep();
                        break;

                    case SOCMDBACK:
                        gotoPrevStep();
                        break;

                    case SOFIRSTSORTCMD:
                       setSortButtonLabel(0);
                       break;

                    case SOSECSORTCMD:
                       setSortButtonLabel(1);
                       break;

                    case SOTHIRDSORTCMD:
                       setSortButtonLabel(2);
                       break;

                    case SOFOURTHSORTCMD:
                       setSortButtonLabel(3);
                       break;

                    case SOCMDGROUPOUT:
               CurReportDocument.ReportTextDocument.lockControllers();
                       addGroupNametoDocument();
               CurReportDocument.ReportTextDocument.unlockControllers();
                       break;

                    case SOCMDGROUPIN:
                CurReportDocument.ReportTextDocument.lockControllers();
            removeGroupName();
            CurReportDocument.ReportTextDocument.unlockControllers();

                       break;

                    case SOOPTLANDSCAPE:
                        ReportDocument.changePageOrientation(CurReportDocument, true);
                        break;

                    case SOOPTPORTRAIT:
                        ReportDocument.changePageOrientation(CurReportDocument, false);
                        break;
                    default:
//               System.err.println( exception);
                        break;
                }
            }
            catch( Exception exception ){
               exception.printStackTrace(System.out);
            }
        }

    }


    public static void gotoNextStep(){
    try{
        int PageCount = 4;
        int iPage = ((Integer) tools.getUNOPropertyValue(oDialogModel, "Step")).intValue();
        switch (iPage){
            case 1:
                UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdBack", "Enabled", new Boolean(true));
                CurDBMetaData.FieldNames = xSelFieldsListBox.getItems();
                fillSecondStep();
                break;
            case 2:
                String[] GroupFieldNames = new String[GroupFieldVector.size()];
                GroupFieldVector.copyInto(GroupFieldNames);
        CurDBMetaData.GroupFieldNames = GroupFieldNames;
        fillThirdStep();
                break;
            case 3:
                UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGoOn", "Label", scmdReady);
                setUpSortList();
        DBMetaData.setupWidthList();
                CurDBMetaData.ResultSet = DBMetaData.combineSelectStatement(CurDBMetaData.DBConnection, xDBMetaData, TableName, CurDBMetaData);
//      ReportDocument.insertTextArrangement(CurReportDocument, ReportFolderName + "Example.stw", CurDBMetaData, null);
        ReportDocument.setupRecordSection(CurReportDocument, ReportFolderName + "cnt-Default.stw", CurDBMetaData);
        fillFourthStep();
        break;
        case 4:
        ReportDocument.storeReportDocument(xMSF, CurReportDocument, CurDBMetaData);
        XComponent xDBComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, CurDBMetaData.DBConnection);
        xDBComponent.dispose();
        ReportDocument.reconnectToDatabase(xMSF, CurDBMetaData, CurReportDocument);
        default:
                break;
        }
        if (iPage < PageCount)
            tools.setUNOPropertyValues(oDialogModel, new String[]{"Step", "Title"},
                                                     new Object[]{ new Integer(iPage + 1), WizardTitle[iPage]});
    }
    catch( Exception exception ){
        exception.printStackTrace(System.out);
    }}


    public static void addGroupNametoDocument(){
    try{
      CurGroupName = xGroupListBox.getSelectedItem();
      UNODialogs.removeSelectedItems(xGroupListBox);
      GroupFieldVector.addElement(CurGroupName);
      UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGroupOut", "Enabled", new Boolean(xGroupListBox.getSelectedItems().length > 0));
      UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGroupIn", "Enabled", new Boolean(true));
      int GroupCount = GroupFieldVector.size();
      ReportDocument.insertTextSection(CurReportDocument, "GroupField" + GroupCount, ReportFolderName + "cnt-Default.stw");
      ReportDocument.replaceFieldValueInGroupTable(CurReportDocument, "Tbl_GroupField" + GroupCount, CurGroupName, GroupCount-1);
      UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGroupOut", "Enabled", new Boolean(GroupCount < 4));
    }
      catch( Exception exception ){
         exception.printStackTrace(System.out);
    }}


    public static void removeGroupName(){
    try{
      CurGroupName = (String) GroupFieldVector.lastElement();
      GroupFieldVector.removeElement(CurGroupName);
      xGroupListBox.addItem(CurGroupName, (short) xGroupListBox.getItemCount());
      UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGroupOut", "Enabled", new Boolean(xGroupListBox.getSelectedItems().length > 0));
      UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGroupIn", "Enabled", new Boolean(GroupFieldVector.isEmpty() == false));
      ReportDocument.removeLastTextSection(CurReportDocument);
      ReportDocument.removeLastTextTable(CurReportDocument);
    }
      catch( Exception exception ){
         exception.printStackTrace(System.out);
    }}


    public static void gotoPrevStep(){
    try{
        int iPage = ((Integer) tools.getUNOPropertyValue(oDialogModel, "Step")).intValue();
        switch (iPage){
            case 1:
                break;
            case 2:
                break;
            case 3:
        break;
        case 4:
                UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdGoOn", "Label", scmdGoOn);
                fillThirdStep();
                break;
            default:
                break;
        }
        UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdBack", "Enabled", new Boolean(iPage > 2));

        if (iPage > 1)
            tools.setUNOPropertyValues(oDialogModel, new String[]{"Step", "Title"},
                       new Object[]{ new Integer(iPage - 1), WizardTitle[iPage-2]});
    }
      catch( Exception exception ){
         System.err.println( exception);
    }}


    public static void setSortButtonLabel(int iKey){
    try{
        String sTag = (String) UNODialogs.getPropertyOfDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(iKey+1).toString(), "Tag");
        if (sTag.compareTo("ASC") == 0){
            UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(iKey + 1).toString(), "Label", sSortDescend);
            UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(iKey + 1).toString(), "Tag", "DESC");
        }
        else{
            UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(iKey + 1).toString(), "Label", sSortAscend);
            UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(iKey + 1).toString(), "Tag", "ASC");
        }
    }
    catch( Exception exception ){
        System.err.println( exception);
    }}


    public static void setUpSortList(){
    try{
        CurDBMetaData.SortFieldNames = new String[MaxSortIndex+1][2];
        for (int i=0;i<=MaxSortIndex;i++){
            CurDBMetaData.SortFieldNames[i][0] = xSortListBox[i].getSelectedItem();
            CurDBMetaData.SortFieldNames[i][1] = (String) UNODialogs.getPropertyOfDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(i+1).toString(), "Tag");
        }
    }
    catch( Exception exception ){
        System.err.println( exception);
    }}


    public static void enableNextSortListBox(int CurIndex){
    try{
        short iNextItemPos;
        if (CurIndex > MaxSortIndex)
            MaxSortIndex = CurIndex;
        boolean bDoEnable = (xSortListBox[CurIndex].getSelectedItemPos()!= 0);
        if ((bDoEnable == false) && (MaxSortIndex > CurIndex)){
            for (int i= CurIndex; i < MaxSortIndex; i++){
                iNextItemPos = xSortListBox[i+1].getSelectedItemPos();
                if (iNextItemPos != 0){
                    xSortListBox[i].selectItemPos(iNextItemPos,true);
                    xSortListBox[i+1].selectItemPos(iNextItemPos,false);
                    toggleSortListBox(i,true);
                    xSortListBox[i+1].selectItemPos((short)0,true);
                }
            }
            MaxSortIndex = MaxSortIndex - 1;
            if (MaxSortIndex < xSortListBox.length -2)
                toggleSortListBox(MaxSortIndex + 2, false);
        }
        else
            toggleSortListBox(CurIndex+1, bDoEnable);
    }
      catch( Exception exception ){
         System.err.println( exception);
    }}


     public static void toggleSortListBox(int CurIndex, boolean bDoEnable){
     try{
        UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "lstSort_" + new Integer(CurIndex + 1).toString(), "Enabled", new Boolean(bDoEnable));
        UNODialogs.AssignPropertyToDialogControl(xDlgNameAccess, "cmdSort_" + new Integer(CurIndex + 1).toString(), "Enabled", new Boolean(bDoEnable));
        if (bDoEnable == false)
            xSortListBox[CurIndex].selectItemPos((short)0,true);
     }
     catch( Exception exception ){
         System.err.println( exception);
     }}


    public static void fillFourthStep(){
    try{

        InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblTitle",
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Integer(8), new Integer(6), new Integer(41), new Integer(4), new Integer(60), sReportTitle});

    xTitleTextBox = InsertTextField(xMSFDialogModel, xDlgNames, xDialogContainer, "txtTitle", SOTXTTITLE,
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width"},
                            new Object[] {new Integer(12), new Integer(50), new Integer(40), new Integer(4), new Integer(100)});

    InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblLayout",
                new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                new Object[] {new Integer(8), new Integer(6), new Integer(55), new Integer(4), new Integer(109), slblDataStructure});

    ContentFiles = tools.getFolderTitles(xMSF, "cnt");
    xContentListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstContent", SOCONTENTLST,
                    new String[] {"Height", "PositionX", "PositionY", "Step", "StringItemList", "Width"},
                            new Object[] {new Integer(118), new Integer(6), new Integer(66), new Integer(4), ContentFiles[1], new Integer(109)});

    InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblOrientation",
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Integer(8), new Integer(155), new Integer(152), new Integer(4), new Integer(74), sOrientationHeader});

        InsertRadioButton(xMSFDialogModel, xDlgNames, xDialogContainer, "optLandscape", SOOPTLANDSCAPE,
                            new String[] {"Height", "Label", "PositionX", "PositionY", "State", "Step", "Width"},
                            new Object[] {new Integer(10), sOrientHorizontal, new Integer(161), new Integer(164), new Short((short) 1), new Integer(4), new Integer(100)});

        InsertRadioButton(xMSFDialogModel, xDlgNames, xDialogContainer, "optPortrait", SOOPTPORTRAIT,
                            new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "Width"},
                            new Object[] {new Integer(10), sOrientVertical, new Integer(161), new Integer(177), new Integer(4), new Integer(100)});

    InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblLayout",
                new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                new Object[] {new Integer(8), new Integer(155), new Integer(40), new Integer(4), new Integer(109), slblPageLayout});
    LayoutFiles = tools.getFolderTitles(xMSF,"stl");
    xLayoutListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstLayout", SOLAYOUTLST,
                    new String[] {"Height", "PositionX", "PositionY", "Step", "StringItemList", "Width"},
                            new Object[] {new Integer(95), new Integer(155), new Integer(51), new Integer(4), LayoutFiles[1], new Integer(109)});

    }
    catch( Exception exception ){
        exception.printStackTrace(System.out);
    }}


    public static void fillThirdStep(){
    try{
        boolean bDoEnable;
        int YPos = 60;
        int FieldCount = CurDBMetaData.FieldNames.length;
    int SortFieldCount = FieldCount + 1-CurDBMetaData.GroupFieldNames.length;
        String SortFieldNames[] = new String[SortFieldCount];
        SortFieldNames[0] = sNoSorting;
        String CurFieldName;
    int a = 1;
    for (int i = 0; i < FieldCount;i++){
        CurFieldName = CurDBMetaData.FieldNames[i];
        if (tools.FieldInList(CurDBMetaData.GroupFieldNames, CurFieldName) == -1){
        SortFieldNames[a] = CurFieldName;
        a +=1;
        }
    }
    for (int i = 0; i<4; i++){
            bDoEnable = (i == 0);
            xSortListBox[i] = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstSort_" + new Integer(i+1).toString(), SOSORTLST[i],
                           new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "StringItemList", "Width", "Dropdown","LineCount", "Name"},
                           new Object[] {new Boolean(bDoEnable), new Integer(12), new Integer(144), new Integer(YPos), new Integer(3), SortFieldNames, new Integer(92), new Boolean(true), new Short("5"), "lstSort_" + new Integer(i+1)});
        InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdSort_" + new Integer(i+1).toString(), SOSORTCMD[i],
                 new String[] {"Enabled", "Height", "Label", "PositionX", "PositionY", "Step", "Tag", "Width"},
                 new Object[] {new Boolean(bDoEnable), new Integer(14), sSortAscend, new Integer(246), new Integer(YPos-1), new Integer(3), "ASC", new Integer(14)});
            YPos = YPos + 25;

        InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblSort",
                new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "Width"},
                new Object[] {new Integer(8), sSortHeader, new Integer(144), new Integer(40), new Integer(3), new Integer(72)});
    }
    }
    catch( Exception exception ){
        exception.printStackTrace(System.out);
    }}



    public static void fillSecondStep(){
    try{

    xGroupListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstGroup", SOGROUPLST,
                    new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "StringItemList", "Width", "MultiSelection"},
                            new Object[] {new Boolean(bGroupByIsSupported), new Integer(133), new Integer(6), new Integer(51), new Integer(2), CurDBMetaData.FieldNames, new Integer(100), new Boolean(true)});

       InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblGroups",
            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
            new Object[] {new Boolean(bGroupByIsSupported), new Integer(8), new Integer(6), new Integer(40), new Integer(2), new Integer(100), slblDataStructure});
       InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdGroupOut", SOCMDGROUPOUT,
            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
            new Object[] {new Boolean(false), new Integer(14), new Integer(116), new Integer(93), new Integer(2), new Integer(14),">>"});
       InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdGroupIn", SOCMDGROUPIN,
            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
            new Object[] {new Boolean(false), new Integer(14), new Integer(116), new Integer(118), new Integer(2), new Integer(14), "<<"});
            GroupFieldVector = new java.util.Vector(CurDBMetaData.FieldNames.length);
    }
    catch( Exception exception ){
        exception.printStackTrace(System.out);
    }}


    public static void main (String args[]) {

        String[] ConnectStr;
        ConnectStr = new String[1];
// connecting synchronously with the office
//        String connectStr = "uno:socket,host=localhost,port=8100;urp,negotiate=0,forcesynchronous=1;StarOffice.NamingService";
            ConnectStr[0] = "uno:socket,host=localhost,port=8100;urp;StarOffice.ServiceManager";
//        ConnectStr[1] = "uno:socket,host=localhost,port=8100;urp;StarOffice.NamingService;StarOffice.NamingService";     //ServiceManager

        xMSF = null;
        try {
            xMSF = connect( ConnectStr[0] );
        if( xMSF != null )  System.out.println("Connected to "+ ConnectStr );
        startReportWizard(xMSF);
        }
        catch(Exception e) {
            e.printStackTrace(System.out);
            System.exit( 0 );
        }
    }

    public static void startReportWizard(XMultiServiceFactory xMSF){
    try{
    xDesktop = tools.getDesktop( xMSF );
    getReportResources(xMSF);
    CurReportDocument =  new ReportDocument.RepWizardDocument();
    CurDBMetaData = new DBMetaData.CommandMetaData();

    CurReportDocument.ReportTextDocument =  (XTextDocument) tools.createNewDocument(xDesktop, CurReportDocument.oComponent, "swriter");
    ReportDocument.initializeReportDocument(xMSF, CurReportDocument);
        ShowDialog(xMSF, CurReportDocument);
    }
    catch(java.lang.Exception jexception ){
        System.err.println( jexception);
    }
    }

    public static void ShowDialog(XMultiServiceFactory xMSF, ReportDocument.RepWizardDocument CurReportDocument)
        {
            try
            {
                 DBMetaData.InitializeWidthList();

                ControlList = new Hashtable();

/*                XPropertySet xPropertySetMultiComponentFactory = ( XPropertySet ) UnoRuntime.queryInterface( XPropertySet.class, xMultiComponentFactory );

                // Get the default context from the office server.
                Object objectDefaultContext = xPropertySetMultiComponentFactory.getPropertyValue( "DefaultContext" );

                // Query for the interface XComponentContext.
                xcomponentcontext = ( XComponentContext ) UnoRuntime.queryInterface(XComponentContext.class, objectDefaultContext );

                // create a dialog model
                oDialogModel =  xMultiComponentFactory.createInstanceWithContext("com.sun.star.awt.UnoControlDialogModel", xcomponentcontext ); */
        oDialogModel = xMSF.createInstance("com.sun.star.awt.UnoControlDialogModel");
                XMultiPropertySet xMultiPSetDlg = (XMultiPropertySet) UnoRuntime.queryInterface(XMultiPropertySet.class, oDialogModel);
                xMultiPSetDlg.setPropertyValues(new String[] {"Height", "PositionX", "PositionY", "Step", "Title", "Width"},
                                                new Object[] {new Integer(210), new Integer(16), new Integer(28), new Integer(1), WizardTitle[0], new Integer(270)});

                // get service manager from dialog model
        xMSFDialogModel = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, oDialogModel);

                // create dialog control
//                Object objectDialog = xMultiComponentFactory.createInstanceWithContext("com.sun.star.awt.UnoControlDialog", xcomponentcontext );
        Object objectDialog = xMSF.createInstance("com.sun.star.awt.UnoControlDialog");
                // set dialog model at dialog control
                XControl xControl = ( XControl ) UnoRuntime.queryInterface( XControl.class, objectDialog );
                XControlModel xControlModel = ( XControlModel ) UnoRuntime.queryInterface( XControlModel.class, oDialogModel );
                xControl.setModel( xControlModel );

                xDialogContainer = ( XControlContainer ) UnoRuntime.queryInterface( XControlContainer.class, objectDialog );
                xDlgNames = (XNameContainer) UnoRuntime.queryInterface( XNameContainer.class, oDialogModel );
                xDlgNameAccess = (XNameAccess) UnoRuntime.queryInterface(XNameAccess.class, oDialogModel);
            InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdCancel", SOCMDCANCEL,
                            new String[] {"Height", "PositionX", "PositionY", "PushButtonType", "Step", "Width", "Label"},
                            new Object[] {new Integer(14), new Integer(6), new Integer(190), new Short((short)com.sun.star.awt.PushButtonType.CANCEL_value), new Integer(0), new Integer(53), scmdCancel});

        InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdHelp", SOCMDHELP,
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Integer(14), new Integer(63), new Integer(190), new Integer(0), new Integer(53), scmdHelp});

                InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdBack", SOCMDBACK,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(14), new Integer(155), new Integer(190), new Integer(0), new Integer(53), scmdBack});

                InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdGoOn", SOCMDGOON,
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Integer(14), new Integer(211), new Integer(190), new Integer(0), new Integer(53), scmdGoOn});

        InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdMoveSelected", SOCMDMOVESEL,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(14), new Integer(122), new Integer(88), new Integer(1), new Integer(25),"->"});

                InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdMoveAll", SOCMDMOVEALL,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(14), new Integer(122), new Integer(113), new Integer(1), new Integer(25),"=>>"});

        InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdRemoveSelected", SOCMDREMOVESEL,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(14), new Integer(122), new Integer(138), new Integer(1), new Integer(25), "<-"});

                InsertButton(xMSFDialogModel, xDlgNames, xDialogContainer, "cmdRemoveAll", SOCMDREMOVEALL,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(14), new Integer(122), new Integer(163), new Integer(1), new Integer(25), "<<="});

                InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblDatabases",
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Integer(8), new Integer(6), new Integer(40), new Integer(1), new Integer(74), slblDatabases});

        InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblTables",
                            new String[] {"Enabled", "Height", "Label", "PositionX", "PositionY", "Step", "Width"},
                            new Object[] {new Boolean(false), new Integer(8), slblTables, new Integer(122), new Integer(40), new Integer(1), new Integer(72)});

                InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblFields",
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(8), new Integer(6), new Integer(70), new Integer(1), new Integer(109), slblFields});

                InsertControlModel("com.sun.star.awt.UnoControlFixedTextModel", xMSFDialogModel, xDlgNames, "lblSelFields",
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Label"},
                            new Object[] {new Boolean(false), new Integer(8), new Integer(154), new Integer(70), new Integer(1), new Integer(110), slblSelFields});

                InsertControlModel("com.sun.star.awt.UnoControlImageControlModel", xMSFDialogModel, xDlgNames, "imgTheme",
                            new String[] {"Height", "PositionX", "PositionY", "Step", "Width"},
                            new Object[] {new Integer(26), new Integer(6), new Integer(6), new Integer(0), new Integer(258)});

                xDBListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstDatabases", SODBLST,
                            new String[] {"Height", "PositionX", "PositionY", "Step", "StringItemList", "Width", "Dropdown","LineCount", "Name"},
                            new Object[] {new Integer(12), new Integer(6), new Integer(51), new Integer(1), DBMetaData.getDatabaseNames(CurReportDocument), new Integer(110), new Boolean(true), new Short("5"), "lstDatabases"});
        xTableListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstTables", SOTBLLST,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "Dropdown", "LineCount"},
                            new Object[] {new Boolean(false), new Integer(12), new Integer(122), new Integer(51), new Integer(1), new Integer(110), new Boolean(true), new Short("5")});
        xFieldsListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstFields", SOFLDSLST,
                new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "MultiSelection"},
                            new Object[] {new Boolean(false), new Integer(103), new Integer(6), new Integer(81), new Integer(1), new Integer(110), new Boolean(true)});
        xSelFieldsListBox = InsertListbox(xMSFDialogModel, xDlgNames, xDialogContainer, "lstSelFields", SOSELFLDSLST,
                            new String[] {"Enabled", "Height", "PositionX", "PositionY", "Step", "Width", "MultiSelection"},
                            new Object[] {new Boolean(false), new Integer(103), new Integer(154), new Integer(81), new Integer(1), new Integer(110), new Boolean(true)});
                XWindow xWindow = ( XWindow ) UnoRuntime.queryInterface( XWindow.class, objectDialog );
                xWindow.setVisible( false );
                XDialog xDialog = ( XDialog ) UnoRuntime.queryInterface( XDialog.class, objectDialog );

        Object objectToolkit = xMSF.createInstance("com.sun.star.awt.ExtToolkit");

//      Object objectToolkit =  xMultiComponentFactory.createInstanceWithContext("com.sun.star.awt.ExtToolkit", xcomponentcontext);
                XToolkit xToolkit = ( XToolkit ) UnoRuntime.queryInterface( XToolkit.class, objectToolkit );
                xControl.createPeer( xToolkit, null );

//                long iEnd = System.currentTimeMillis();
//                long iTime = (long) (iEnd - iStart);
//                System.out.println("Die Ausf�hrung dauerte " + iTime + " Millisekunden");
                short retvalue = xDialog.execute();
                switch (retvalue){
                    case 0:
                        break;
                    case 1:
                        break;
                }
                // dispose dialog
                XComponent xComponent = ( XComponent ) UnoRuntime.queryInterface( XComponent.class, objectDialog );
                xComponent.dispose();

                System.exit(0);
            }
            catch( Exception exception )
            {
            exception.printStackTrace(System.out);
            }
        }

        public static void InsertButton(XMultiServiceFactory xMSFDialogModel, XNameContainer xDlgNames, XControlContainer xDialogContainer, String sName,
                                        int iControlKey, String[] sProperties, Object[] sValues)
        throws com.sun.star.uno.Exception{
           Object oButtonModel = InsertControlModel("com.sun.star.awt.UnoControlButtonModel", xMSFDialogModel, xDlgNames, sName, sProperties, sValues);
           XPropertySet xPSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, oButtonModel);
           xPSet.setPropertyValue("Name", sName);
           Object objectButton = xDialogContainer.getControl( new String(sName));

           XButton xButton = ( XButton ) UnoRuntime.queryInterface( XButton.class, objectButton );
           xButton.addActionListener( new ActionListenerImpl() );
           Integer ControlKey = new Integer(iControlKey);
           ControlList.put(sName, ControlKey);
        }


        public static XTextComponent InsertTextField(XMultiServiceFactory xMSFDialogModel, XNameContainer xDlgNames, XControlContainer xDialogContainer, String sName,
                                        int iControlKey, String[] sProperties, Object[] sValues)
        throws com.sun.star.uno.Exception{
           Object oButtonModel = InsertControlModel("com.sun.star.awt.UnoControlEditModel", xMSFDialogModel, xDlgNames, sName, sProperties, sValues);
           XPropertySet xPSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, oButtonModel);
           xPSet.setPropertyValue("Name", sName);
           Object oTextField = xDialogContainer.getControl( new String(sName));
           XTextComponent xTextBox = (XTextComponent) UnoRuntime.queryInterface(XTextComponent.class, oTextField);
       xTextBox.addTextListener( new TextListenerImpl());
           Integer ControlKey = new Integer(iControlKey);
           ControlList.put(sName, ControlKey);
       return xTextBox;
        }


        static public XListBox InsertListbox(XMultiServiceFactory xMSFDialogModel, XNameContainer xDlgNames, XControlContainer xDialogContainer, String sName,
                                           int iControlKey, String[] sProperties, Object[] sValues)
        throws com.sun.star.uno.Exception{
            Object xListBoxModel = InsertControlModel("com.sun.star.awt.UnoControlListBoxModel", xMSFDialogModel, xDlgNames, sName, sProperties, sValues);
            XPropertySet xPSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xListBoxModel);
            xPSet.setPropertyValue("Name", sName);
            Object oListBox = xDialogContainer.getControl( new String(sName));
            com.sun.star.awt.XListBox xListBox = (com.sun.star.awt.XListBox) UnoRuntime.queryInterface(com.sun.star.awt.XListBox.class, oListBox);
            xListBox.addItemListener( new ItemListenerImpl());
            xListBox.addActionListener( new ActionListenerImpl() );
            Integer ControlKey = new Integer(iControlKey);
            ControlList.put(sName, ControlKey);
            return xListBox;
       }


       static public XButton InsertRadioButton(XMultiServiceFactory xMSFDialogModel, XNameContainer xDlgNames, XControlContainer xDialogContainer, String sName,
                                           int iControlKey, String[] sProperties, Object[] sValues)
        throws com.sun.star.uno.Exception{
            Object oRadioButtonModel = InsertControlModel("com.sun.star.awt.UnoControlRadioButtonModel", xMSFDialogModel, xDlgNames, sName, sProperties, sValues);
            XPropertySet xPSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, oRadioButtonModel);
            xPSet.setPropertyValue("Name", sName);
            Object oRadioButton = xDialogContainer.getControl( new String(sName));
            com.sun.star.awt.XButton xRadioButton = (com.sun.star.awt.XButton) UnoRuntime.queryInterface(com.sun.star.awt.XButton.class, oRadioButton);
            xRadioButton.addActionListener(new ActionListenerImpl());
            Integer ControlKey = new Integer(iControlKey);
            ControlList.put(sName, ControlKey);
            return xRadioButton;
       }


        public static Object InsertControlModel(String ServiceName, XMultiServiceFactory xMSFDialogModel, XNameContainer xDlgNames, String sName,
                                                String[] sProperties, Object[] sValues)
        throws com.sun.star.uno.Exception{
           Object objectControlModel = xMSFDialogModel.createInstance(ServiceName);
           tools.setUNOPropertyValues(objectControlModel, sProperties, sValues);
//           XMultiPropertySet xMultiPSetLst = (XMultiPropertySet) UnoRuntime.queryInterface(XMultiPropertySet.class, objectControlModel);
//           xMultiPSetLst.setPropertyValues(sProperties, sValues);
       xDlgNames.insertByName(sName, objectControlModel);
       return objectControlModel;
        }



    public static XMultiServiceFactory connect( String connectStr )
    throws com.sun.star.uno.Exception, com.sun.star.uno.RuntimeException, Exception {
        XComponentContext xcomponentcontext = null;
    XMultiComponentFactory xMultiComponentFactory = tools.getMultiComponentFactory();
        // create a connector, so that it can contact the office
        Object  xUrlResolver  = xMultiComponentFactory.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", xcomponentcontext );
        XUnoUrlResolver urlResolver = (XUnoUrlResolver)UnoRuntime.queryInterface( XUnoUrlResolver.class, xUrlResolver );

        Object rInitialObject = urlResolver.resolve( connectStr );
    xMultiComponentFactory = tools.getMultiComponentFactory();

        rInitialObject = urlResolver.resolve("uno:socket,host=localhost,port=8100;urp;StarOffice.NamingService");

        XNamingService rName = (XNamingService)UnoRuntime.queryInterface(XNamingService.class, rInitialObject );

        XMultiServiceFactory xMSF = null;
        if( rName != null ) {
            System.err.println( "got the remote naming service !" );
            Object rXsmgr = rName.getRegisteredObject("StarOffice.ServiceManager" );
            xMSF = (XMultiServiceFactory) UnoRuntime.queryInterface( XMultiServiceFactory.class, rXsmgr );

    }
        return ( xMSF );
    }


    public static void getReportResources(XMultiServiceFactory xMSF){
            final int RID_COMMON = 1000;
        final int RID_FORM = 2200;
        final int RID_REPORT = 2400;

        ReportFolderName = tools.getOfficePath(xMSF, "Template", "share") + "/wizard/report/";
        XInvocation xResInvoke = tools.initResources(xMSF, "ReportWizard","dbw");

        sMsgWizardName = "AutoPilot Report";
        scmdReady = "C~reate";
        scmdCancel = "~Cancel";
        scmdBack = "<< ~Back";
        scmdHelp = "~Help";
        scmdGoOn = "~Next >>";
        slblDatabases = "Data so~urce";
        slblTables = "Tables or queries";
        slblFields = "Ex~isting fields";
        slblSelFields = "Fields in form";
        slblDataStructure = "Layout of the data structure";
        slblPageLayout = "Layout of the page headers";
        sOrganizeFields = "Organize fields";
        sSortHeader = "Additional ~Sorting Criterias";
        sNoSorting = "(none)";
        sOrientationHeader =  "~Alignment";
        sOrientVertical = "~Hochformat";
        sOrientHorizontal = "~Landscape";
        sReportTitle = "~Name of report";
        sSortAscend = "A;Z";
        sSortDescend = "Z;A";
        WizardTitle = new String[4];
        WizardTitle[0] = sMsgWizardName + " - " + "Choose Database";
        WizardTitle[1] = sMsgWizardName + " - " + "Structure";
        WizardTitle[2] = sMsgWizardName + " - " + "Sort Options";
        WizardTitle[3] = sMsgWizardName + " - " + "Style Selection";
        sWriterFilterName = "Writer 6.0";
     }

/*      ReportFolderName = tools.getOfficePath(xMSF, "Template", "share") + "/wizard/report/";
        XInvocation xResInvoke = tools.initResources(xMSF, "ReportWizard","dbw");
        sMsgWizardName = tools.getResText(xResInvoke, RID_REPORT);
        scmdReady = tools.getResText(xResInvoke, RID_COMMON + 0);
        scmdCancel = tools.getResText(xResInvoke, RID_COMMON + 1);
        scmdBack = tools.getResText(xResInvoke, RID_COMMON + 2);
        scmdHelp = tools.getResText(xResInvoke, RID_COMMON + 20);
        sGoOn = tools.getResText(xResInvoke, RID_COMMON + 3);
        slblDatabases = tools.getResText(xResInvoke, RID_FORM + 11);
        slblTables = tools.getResText(xResInvoke, RID_FORM + 6);
        slblFields = tools.getResText(xResInvoke, RID_FORM + 12);
        slblSelFields = tools.getResText(xResInvoke, RID_FORM + 13);
        slblDataStructure = tools.getResText(xResInvoke, RID_REPORT + 14);
        slblPageLayout = tools.getResText(xResInvoke, RID_REPORT + 15);
        sOrganizeFields = tools.getResText(xResInvoke, RID_REPORT + 19);
        sSortHeader = tools.getResText(xResInvoke, RID_REPORT + 20);
        sNoSorting = tools.getResText(xResInvoke, RID_REPORT + 8);
        sOrientationHeader =  tools.getResText(xResInvoke, RID_REPORT + 21);
        sOrientVertical = tools.getResText(xResInvoke, RID_REPORT + 22);
        sOrientHorizontal = tools.getResText(xResInvoke, RID_REPORT + 23);
        sReportTitle = tools.getResText(xResInvoke, RID_REPORT + 32);
        sSortAscend = tools.getResText(xResInvoke, RID_REPORT + 36);
        sSortDescend = tools.getResText(xResInvoke, RID_REPORT + 37);
        WizardTitle = new String[5];
        WizardTitle[1] = sMsgWizardName + " - " + tools.getResText(xResInvoke, RID_FORM + 45);
        WizardTitle[2] = sMsgWizardName + " - " + tools.getResText(xResInvoke, RID_REPORT + 11);
        WizardTitle[3] = sMsgWizardName + " - " + tools.getResText(xResInvoke, RID_REPORT + 12);
        WizardTitle[4] = sMsgWizardName + " - " + tools.getResText(xResInvoke, RID_FORM + 13);
        sWriterFilterName = tools.getResText(xResInvoke, RID_FORM + 70); */
}   