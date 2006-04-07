/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Finalizer.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-07 12:41:55 $
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

import com.sun.star.awt.XRadioButton;
import com.sun.star.awt.XTextComponent;
import com.sun.star.wizards.common.Desktop;
import com.sun.star.wizards.common.JavaTools;
import com.sun.star.wizards.db.DBMetaData;
import com.sun.star.wizards.ui.UIConsts;
import com.sun.star.wizards.ui.*;

/**
 * @author Administrator
 *
 * To change the template for this generated type comment go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
public class Finalizer {
    WizardDialog CurUnoDialog;
    Desktop.OfficePathRetriever curofficepath;
    short curtabindex;
    XRadioButton optModifyForm;
    XRadioButton optWorkWithForm;
    XTextComponent txtFormName;
    FormDocument oFormDocument;

    public Finalizer(WizardDialog _CurUnoDialog) {
        this.CurUnoDialog = _CurUnoDialog;
        curtabindex = (short) (FormWizard.SOSTOREPAGE * 100);

        String slblFormName = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 50);
        String slblProceed = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 51);
        String sWorkWithForm = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 52);
        String sModifyForm = CurUnoDialog.oResource.getResText(UIConsts.RID_FORM + 53);
        CurUnoDialog.insertLabel("lblFormName",
          new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
          new Object[] { UIConsts.INTEGERS[8], slblFormName, new Integer(97), new Integer(25), UIConsts.INTEGERS[8], new Short(curtabindex++), new Integer(111)}
        );
        txtFormName = CurUnoDialog.insertTextField("txtFormName", "toggleFinishButton", this,
          new String[] {"Height", "HelpURL", "PositionX", "PositionY", "Step", "TabIndex", "Text", "Width"},
          new Object[] { UIConsts.INTEGER_12, "HID:34481", new Integer(97), new Integer(35),UIConsts.INTEGERS[8], new Short((short)82),"", new Integer(185)}
        );
        CurUnoDialog.insertLabel("lblProceed",
          new String[] {"Height", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
          new Object[] { UIConsts.INTEGERS[8], slblProceed, new Integer(97), new Integer(62),UIConsts.INTEGERS[8], new Short(curtabindex++), new Integer(154)}
        );
        XRadioButton optWorkWithForm = CurUnoDialog.insertRadioButton("optWorkWithForm", null,
          new String[] {"Height", "HelpURL", "Label", "PositionX", "PositionY", "State", "Step", "TabIndex", "Width"},
          new Object[] { UIConsts.INTEGERS[8], "HID:34482", sWorkWithForm, new Integer(101), new Integer(77), new Short((short)1),UIConsts.INTEGERS[8], new Short(curtabindex++), new Integer(107)}
        );
        optModifyForm = CurUnoDialog.insertRadioButton("optModifyForm", null,
          new String[] {"Height", "HelpURL", "Label", "PositionX", "PositionY", "Step", "TabIndex", "Width"},
          new Object[] { UIConsts.INTEGERS[8], "HID:34483", sModifyForm, new Integer(101), new Integer(89),UIConsts.INTEGERS[8], new Short(curtabindex++), new Integer(107)}
        );
    }

    public void initialize(String _formname, FormDocument _oFormDocument){
        if (oFormDocument == null)
            oFormDocument = _oFormDocument;
        if (txtFormName.getText().length() == 0)
            txtFormName.setText(Desktop.getUniqueName(_oFormDocument.oMainFormDBMetaData.getFormDocuments(),_formname));
    }

    public void toggleFinishButton(){
        CurUnoDialog.enableFinishButton(txtFormName.getText().length() > 0);
    }


    public String getName(){
        return txtFormName.getText();
    }

    public boolean getOpenMode(){
        boolean bOpenMode = optModifyForm.getState() ? true : false;
        return bOpenMode;
    }

    public boolean finish(){
//        if (!oFormDocument.oMainFormDBMetaData.hasFormDocumentByName(sFormName)){
            return oFormDocument.oMainFormDBMetaData.storeDatabaseDocumentToTempPath(this.oFormDocument.xComponent, getName());
//        }
    }
}
