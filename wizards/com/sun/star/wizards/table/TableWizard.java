/*************************************************************************
*
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TableWizard.java,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-07 12:54:55 $
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
package com.sun.star.wizards.table;

import java.util.Calendar;
import java.util.Hashtable;

import com.sun.star.awt.TextEvent;
import com.sun.star.awt.VclWindowPeerAttribute;
import com.sun.star.awt.XTextListener;
import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.EventObject;
import com.sun.star.lang.XInitialization;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdbc.SQLException;
import com.sun.star.task.XJobExecutor;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.wizards.common.*;
import com.sun.star.wizards.db.TableDescriptor;
import com.sun.star.wizards.ui.*;


public class TableWizard extends WizardDialog implements XTextListener, XCompletion{

    static String slblFields;
    static String slblSelFields;
    Finalizer curFinalizer;
    ScenarioSelector curScenarioSelector;
    FieldFormatter curFieldFormatter;
    PrimaryKeyHandler curPrimaryKeyHandler;
    String sMsgWizardName = "";
    public Hashtable fielditems;
    int wizardmode;
    String tablename;
    String serrToManyFields;
    String serrTableNameexists;
    String scomposedtablename;
    TableDescriptor curTableDescriptor;
    public static final int SONULLPAGE = 0;
    public static final int SOMAINPAGE = 1;
    public static final int SOFIELDSFORMATPAGE = 2;
    public static final int SOPRIMARYKEYPAGE = 3;
    public static final int SOFINALPAGE = 4;
    private String sMsgColumnAlreadyExists = "";

    String WizardHeaderText[] = new String[8];

    public TableWizard(XMultiServiceFactory xMSF) {
        super(xMSF, 41200);
        super.addResourceHandler("TableWizard", "dbw");
        String sTitle = oResource.getResText(UIConsts.RID_TABLE + 1);
        Helper.setUnoPropertyValues(xDialogModel,
            new String[] { "Height","Moveable","Name","PositionX","PositionY","Step","TabIndex","Title","Width"},
            new Object[] { new Integer(210),Boolean.TRUE, "DialogTable", new Integer(102),new Integer(41),new Integer(1), new Short((short)0), sTitle, new Integer(310)}  );
        drawNaviBar();
        fielditems = new Hashtable();
        //TODO if reportResouces cannot be gotten dispose officedocument
        if (getTableResources() == true)
            setRightPaneHeaders(oResource, UIConsts.RID_TABLE + 8, 4);
    }


    protected void leaveStep(int nOldStep, int nNewStep){
         switch (nOldStep){
            case SOMAINPAGE:
                curScenarioSelector.addColumnsToDescriptor();
                break;
            case SOFIELDSFORMATPAGE:
                curFieldFormatter.updateColumnofColumnDescriptor();
                String[] sfieldnames = curFieldFormatter.getFieldNames();
                super.setStepEnabled(this.SOFIELDSFORMATPAGE, sfieldnames.length > 0);
                curScenarioSelector.setSelectedFieldNames(sfieldnames);
                break;
            case SOPRIMARYKEYPAGE:
                break;
            case SOFINALPAGE:
                break;
            default:
                break;
          }
    }


    protected void enterStep(int nOldStep, int nNewStep) {
        switch (nNewStep){
            case SOMAINPAGE:
                break;
            case SOFIELDSFORMATPAGE:
                curFieldFormatter.initialize(curTableDescriptor, this.curScenarioSelector.getSelectedFieldNames());
                break;
            case SOPRIMARYKEYPAGE:
                curPrimaryKeyHandler.initialize();
                break;
            case SOFINALPAGE:
                curFinalizer.initialize(curScenarioSelector.getFirstTableName());
                break;
            default:
                break;
        }
    }


    /* (non-Javadoc)
     * @see com.sun.star.wizards.ui.XCompletion#iscompleted(int)
     */
    public boolean iscompleted(int _ndialogpage) {
        switch (_ndialogpage){
            case SOMAINPAGE:
                return curScenarioSelector.iscompleted();
            case SOFIELDSFORMATPAGE:
                return this.curFieldFormatter.iscompleted();
            case SOPRIMARYKEYPAGE:
                if (curPrimaryKeyHandler != null)
                    return this.curPrimaryKeyHandler.iscompleted();
            case SOFINALPAGE:
                return this.curFinalizer.iscompleted();
            default:
                return false;
        }
    }


    /* (non-Javadoc)
     * @see com.sun.star.wizards.ui.XCompletion#setcompleted(int, boolean)
     */
    public void setcompleted(int _ndialogpage, boolean _biscompleted) {
        boolean bScenarioiscompleted = _biscompleted;
        boolean bFieldFormatsiscompleted = _biscompleted;
        boolean bPrimaryKeysiscompleted = _biscompleted;
        boolean bFinalPageiscompleted = _biscompleted;
        if (_ndialogpage == SOMAINPAGE)
            curFinalizer.initialize(curScenarioSelector.getFirstTableName());
        else
            bScenarioiscompleted = iscompleted(SOMAINPAGE);

        if (_ndialogpage != TableWizard.SOFIELDSFORMATPAGE){
            bFieldFormatsiscompleted = iscompleted(SOFIELDSFORMATPAGE);
            if (!bFieldFormatsiscompleted)                              // it might be that the Fieldformatter has not yet been initialized
                bFieldFormatsiscompleted = bScenarioiscompleted;        // in this case query the scenarioselector
        }
        if (_ndialogpage != TableWizard.SOPRIMARYKEYPAGE && (this.curPrimaryKeyHandler != null))
            bPrimaryKeysiscompleted = iscompleted(SOPRIMARYKEYPAGE);
        if (_ndialogpage != TableWizard.SOFINALPAGE)
            bFinalPageiscompleted = iscompleted(SOFINALPAGE);           // Basically the finalpage is always enabled
        if (bScenarioiscompleted){
            super.setStepEnabled(SOFIELDSFORMATPAGE, true);
            super.setStepEnabled(SOPRIMARYKEYPAGE, true);
            if (bPrimaryKeysiscompleted){
                super.enablefromStep(SOFINALPAGE, true);
                super.enableFinishButton(bFinalPageiscompleted);
            }
            else{
                super.enablefromStep(SOFINALPAGE, false);
                enableNextButton(false);
            }
        }
        else
            if (_ndialogpage == SOFIELDSFORMATPAGE)
                super.enablefromStep(super.getCurrentStep()+1, iscompleted(SOFIELDSFORMATPAGE));
            else
                super.enablefromStep(super.getCurrentStep()+1, false);
    }


    public static void main(String args[]) {
    String ConnectStr = "uno:socket,host=localhost,port=8100;urp,negotiate=0,forcesynchronous=1;StarOffice.NamingService";      //localhost  ;Lo-1.Germany.sun.com; 10.16.65.155
    PropertyValue[] curproperties = null;
    try {
        XMultiServiceFactory xLocMSF = com.sun.star.wizards.common.Desktop.connect(ConnectStr);
        TableWizard CurTableWizard = new TableWizard(xLocMSF);
        if(xLocMSF != null){
            System.out.println("Connected to "+ ConnectStr);
            curproperties = new PropertyValue[1];
//          curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///C:/Documents and Settings/bc93774.EHAM02-DEV/My Documents/MySQL-JDBC-Database.odb"); //MyDBase; Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb ; Mydbwizard2DocAssign.odb; NewAccessDatabase, MyDocAssign baseLocation ); "DataSourceName", "db1");
            curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///C:/Documents and Settings/bc93774.EHAM02-DEV/My Documents/MyHSQL Database.odb"); //MyDBase; Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb ; Mydbwizard2DocAssign.odb; NewAccessDatabase, MyDocAssign baseLocation ); "DataSourceName", "db1");*//           /--/curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///x:/bc/MyHSQL Database.odb"); //MyDBase; Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb ; Mydbwizard2DocAssign.odb; NewAccessDatabase,  baseLocation ); "DataSourceName", "db1");*//          curproperties[0] = Properties.createProperty("DataSourceName", "Bibliography");*                        //          curproperties[0] = Properties.createProperty("DataSourceName", "Bibliography"); //Bibliography*         CurTableWizard.startTableWizard(xLocMSF, curproperties);
            curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///x:/bc/MyNewHSQLDatabase.odb"); //Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb MyDBase.odb; Mydbwizard2DocAssign.odb; NewAccessDatabase, MyDocAssign baseLocation ); "DataSourceName", "db1");
            curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///x:/bc/Mydbwizardpp3TestDatabase.odb"); //Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb MyDBase.odb; Mydbwizard2DocAssign.odb; NewAccessDatabase, MyDocAssign baseLocation ); "DataSourceName", "db1");

//          curproperties[0] = Properties.createProperty("DataSourceName", "Bibliography");  //file:///C:/Documents and Settings/bc93774.EHAM02-DEV/My Documents/myjapanesehsqldatasourceMyDocAssign.odb"); //MyDBase; Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb ; Mydbwizard2DocAssign.odb; NewAccessDatabase, MyDocAssign baseLocation ); "DataSourceName", "db1");*//          /--/curproperties[0] = Properties.createProperty("DatabaseLocation", "file:///x:/bc/MyHSQL Database.odb"); //MyDBase; Mydbwizard2DocAssign.odb; MyDBase.odb, Mydbwizard2DocAssign.odb ; Mydbwizard2DocAssign.odb; NewAccessDatabase,  baseLocation ); "DataSourceName", "db1");*//          curproperties[0] = Properties.createProperty("DataSourceName", "Bibliography");*                        //          curproperties[0] = Properties.createProperty("DataSourceName", "Bibliography"); //Bibliography*         CurTableWizard.startTableWizard(xLocMSF, curproperties);
            CurTableWizard.startTableWizard(xLocMSF, curproperties);
        }
    }
    catch(Exception exception){
        exception.printStackTrace(System.out);
    }}


    public void buildSteps(){
        curScenarioSelector = new ScenarioSelector(this, this.curTableDescriptor, slblFields, slblSelFields);
        curFieldFormatter = new FieldFormatter(this, curTableDescriptor );
        if (this.curTableDescriptor.supportsCoreSQLGrammar())
            curPrimaryKeyHandler = new PrimaryKeyHandler(this, curTableDescriptor);
        curFinalizer = new Finalizer(this, curTableDescriptor);
        enableNavigationButtons(false, false, false);
    }


    public boolean createTable(){
        boolean bIsSuccessfull = true;
        boolean bTableCreated = false;
        String schemaname = curFinalizer.getSchemaName();
        String catalogname = curFinalizer.getCatalogName();
        if (curTableDescriptor.supportsCoreSQLGrammar()){
            String[] keyfieldnames = curPrimaryKeyHandler.getPrimaryKeyFields(curTableDescriptor);
            if (keyfieldnames != null){
                if (keyfieldnames.length > 0){
                    boolean bIsAutoIncrement = curPrimaryKeyHandler.isAutoIncremented();
                    bIsSuccessfull = curTableDescriptor.createTable(catalogname, schemaname, tablename, keyfieldnames, bIsAutoIncrement, curScenarioSelector.getSelectedFieldNames());
                    bTableCreated = true;
                }
            }
        }
        if (!bTableCreated){
            bIsSuccessfull = curTableDescriptor.createTable(catalogname, schemaname, tablename, curScenarioSelector.getSelectedFieldNames());
        }
        if ((!bIsSuccessfull) && (curPrimaryKeyHandler.isAutomaticMode())){
           curTableDescriptor.dropColumnbyName(curPrimaryKeyHandler.getAutomaticFieldName());
        }
        return bIsSuccessfull;
    }


    public void finishWizard(){
        super.switchToStep(super.getCurrentStep(), SOFINALPAGE);
        tablename = curFinalizer.getTableName(curScenarioSelector.getFirstTableName());
        scomposedtablename = curFinalizer.getComposedTableName(tablename);
        if (this.curTableDescriptor.isSQL92CheckEnabled())
            Desktop.removeSpecialCharacters(curTableDescriptor.xMSF, Configuration.getOfficeLocale(this.curTableDescriptor.xMSF), tablename);
        if (tablename != ""){
            if (!curTableDescriptor.hasTableByName(scomposedtablename)){
                wizardmode = curFinalizer.finish();
                if (createTable()){
                    if (wizardmode == Finalizer.MODIFYTABLEMODE)
                        curTableDescriptor.switchtoDesignmode(curTableDescriptor.getComposedTableName(), com.sun.star.sdb.CommandType.TABLE);
                    else if (wizardmode == Finalizer.WORKWITHTABLEMODE)
                        curTableDescriptor.switchtoDataViewmode(curTableDescriptor.getComposedTableName(), com.sun.star.sdb.CommandType.TABLE);
                    super.xDialog.endExecute();
                }
            }
            else{
                String smessage = JavaTools.replaceSubString(serrTableNameexists, tablename, "%TABLENAME");
                super.showMessageBox("WarningBox", com.sun.star.awt.VclWindowPeerAttribute.OK, smessage );
                curFinalizer.setFocusToTableNameControl();
            }
        }
    }


    private void callFormWizard(){
    try {
        Object oFormWizard = this.xMSF.createInstance("com.sun.star.wizards.form.CallFormWizard");
        PropertyValue[] aProperties  = new PropertyValue[4];
        aProperties[0] = Properties.createProperty("ActiveConnection", curTableDescriptor.DBConnection);
        aProperties[1] = Properties.createProperty("DataSource", curTableDescriptor.xDataSource);
        aProperties[2] = Properties.createProperty("CommandType", new Integer(CommandType.TABLE));
        aProperties[3] = Properties.createProperty("Command", scomposedtablename);
        XInitialization xInitialization = (XInitialization) UnoRuntime.queryInterface(XInitialization.class, oFormWizard);
        xInitialization.initialize(aProperties);
        XJobExecutor xJobExecutor = (XJobExecutor) UnoRuntime.queryInterface(XJobExecutor.class, oFormWizard);
        xJobExecutor.trigger("start");
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    public void cancelWizard() {
        xDialog.endExecute();
    }


    public void insertFormRelatedSteps(){
        addRoadmap();
        int i = 0;
        i = insertRoadmapItem(0, true, oResource.getResText(UIConsts.RID_TABLE + 2), SOMAINPAGE);
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_TABLE + 3), SOFIELDSFORMATPAGE);
        if (this.curTableDescriptor.supportsCoreSQLGrammar())
            i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_TABLE + 4), SOPRIMARYKEYPAGE);
        i = insertRoadmapItem(i, false, oResource.getResText(UIConsts.RID_TABLE + 5),  SOFINALPAGE);        // Orderby is always supported
        setRoadmapInteractive(true);
        setRoadmapComplete(true);
        setCurrentRoadmapItemID((short) 1);
    }


    public void startTableWizard(XMultiServiceFactory _xMSF, PropertyValue[] CurPropertyValue){
    try{
        curTableDescriptor = new TableDescriptor(xMSF, super.xWindow, this.sMsgColumnAlreadyExists);
        if (curTableDescriptor.getConnection(CurPropertyValue)){
            buildSteps();
            createWindowPeer();
            curTableDescriptor.setWindowPeer(this.xControl.getPeer());
    //      setAutoMnemonic("lblDialogHeader", false);
            insertFormRelatedSteps();
            short RetValue = executeDialog();
            boolean bdisposeDialog = true;
            xComponent.dispose();
            switch (RetValue){
                case 0:                         // via Cancelbutton or via sourceCode with "endExecute"
                    if (wizardmode == Finalizer.STARTFORMWIZARDMODE)
                        callFormWizard();
                    break;
                case 1:

                    break;
            }
        }
    }
    catch(java.lang.Exception jexception ){
        jexception.printStackTrace(System.out);
    }}


    public boolean getTableResources(){
        sMsgWizardName = super.oResource.getResText(UIConsts.RID_TABLE+1);
        slblFields = oResource.getResText(UIConsts.RID_TABLE + 19);
        slblSelFields =  oResource.getResText(UIConsts.RID_TABLE + 25);
        serrToManyFields = oResource.getResText(UIConsts.RID_TABLE + 47);
        serrTableNameexists = oResource.getResText(UIConsts.RID_TABLE + 48);
        sMsgColumnAlreadyExists = oResource.getResText(UIConsts.RID_TABLE + 51);
        return true;
    }


    private void toggleWizardSteps(int _startStep, boolean _benable){
        super.setStepEnabled(SOFIELDSFORMATPAGE, _benable);
        super.setStepEnabled(SOPRIMARYKEYPAGE, _benable);
        super.setStepEnabled(SOFINALPAGE, _benable);
        setControlProperty("btnWizardNext", "Enabled", new Boolean(_benable));
        setControlProperty("btnWizardFinish", "Enabled", new Boolean(_benable));
    }


    public boolean verifyfieldcount( int _icount ){
    try{
        int maxfieldcount = curTableDescriptor.getMaxColumnsInTable();
        if (_icount >=  (maxfieldcount - 1)){   // keep one column as reserve for the automaticcally created key
            String smessage = serrToManyFields;
            smessage = JavaTools.replaceSubString(smessage, String.valueOf(maxfieldcount), "%COUNT");
            showMessageBox("ErrorBox", VclWindowPeerAttribute.OK, smessage);
            return false;
        }
    } catch (SQLException e) {
        e.printStackTrace(System.out);
    }
    return true;
    }


    /* (non-Javadoc)
     * @see com.sun.star.awt.XTextListener#textChanged(com.sun.star.awt.TextEvent)
     */
    public void textChanged(TextEvent aTextEvent) {
        if (this.curTableDescriptor.isSQL92CheckEnabled()){
            Object otextcomponent = UnoDialog.getModel(aTextEvent.Source);
            String sName = (String) Helper.getUnoPropertyValue(otextcomponent, "Text");
            sName = Desktop.removeSpecialCharacters(curTableDescriptor.xMSF, Configuration.getOfficeLocale(curTableDescriptor.xMSF), sName);
            Helper.setUnoPropertyValue(otextcomponent, "Text", sName);
        }
    }

}
