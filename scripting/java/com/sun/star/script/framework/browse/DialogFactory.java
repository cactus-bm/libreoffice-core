/*************************************************************************
 *
 *  $RCSfile: DialogFactory.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: svesik $ $Date: 2004-04-19 23:01:54 $
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
 *
 ************************************************************************/

package com.sun.star.script.framework.browse;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.EventObject;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameContainer;

import com.sun.star.awt.*;

import com.sun.star.script.framework.log.LogUtils;

public class DialogFactory
{
    private static DialogFactory factory;
    private XComponentContext xComponentContext;

    // singleton
      private DialogFactory(XComponentContext xComponentContext)
    {
        this.xComponentContext = xComponentContext;
        factory = this;
      }

    public static void createDialogFactory(XComponentContext xComponentContext)
    {
        if (factory == null)
        {
            synchronized(DialogFactory.class)
            {
                if (factory == null)
                {
                    factory = new DialogFactory(xComponentContext);
                }
            }
        }
    }

    public static DialogFactory getDialogFactory()
        throws java.lang.Exception
    {
        if (factory == null)
        {
            throw new java.lang.Exception("DialogFactory not initialized");
        }
        return factory;
    }

    public boolean showConfirmDialog(String title, String prompt)
    {
        final XDialog xDialog;
        try
        {
            xDialog = createConfirmDialog(title, prompt);
        }
        catch (com.sun.star.uno.Exception e)
        {
            return false;
        }

        // add an action listener to the button controls
        XControlContainer controls = (XControlContainer)
            UnoRuntime.queryInterface(XControlContainer.class, xDialog);

        XButton okButton = (XButton) UnoRuntime.queryInterface(
            XButton.class, controls.getControl("Ok"));
        okButton.setActionCommand("Ok");

        XButton cancelButton = (XButton) UnoRuntime.queryInterface(
            XButton.class, controls.getControl("Cancel"));
        cancelButton.setActionCommand("Cancel");

        final ResultHolder resultHolder = new ResultHolder();

        com.sun.star.awt.XActionListener listener =
            new com.sun.star.awt.XActionListener()
            {
                public void actionPerformed(com.sun.star.awt.ActionEvent e) {
                    if (e.ActionCommand.equals("Cancel"))
                    {
                        resultHolder.setResult(Boolean.FALSE);
                        xDialog.endExecute();
                    }
                    else
                    {
                        resultHolder.setResult(Boolean.TRUE);
                        xDialog.endExecute();
                    }
                }

                public void disposing(EventObject o) {
                    // does nothing
                }
            };

        okButton.addActionListener(listener);
        cancelButton.addActionListener(listener);

        xDialog.execute();

        Boolean result = (Boolean)resultHolder.getResult();

        return result == null ? false : result.booleanValue();
    }

    public String showInputDialog(String title, String prompt)
    {
        final XDialog xDialog;
        try
        {
            xDialog = createInputDialog(title, prompt);
        }
        catch (com.sun.star.uno.Exception e)
        {
            return null;
        }

        // add an action listener to the button controls
        XControlContainer controls = (XControlContainer)
            UnoRuntime.queryInterface(XControlContainer.class, xDialog);

        XButton okButton = (XButton) UnoRuntime.queryInterface(
            XButton.class, controls.getControl("Ok"));
        okButton.setActionCommand("Ok");

        XButton cancelButton = (XButton) UnoRuntime.queryInterface(
            XButton.class, controls.getControl("Cancel"));
        cancelButton.setActionCommand("Cancel");

        final XTextComponent textField = (XTextComponent)
            UnoRuntime.queryInterface(
                XTextComponent.class, controls.getControl("NameField"));

        final ResultHolder resultHolder = new ResultHolder();

        com.sun.star.awt.XActionListener listener =
            new com.sun.star.awt.XActionListener()
            {
                public void actionPerformed(com.sun.star.awt.ActionEvent e) {
                    if (e.ActionCommand.equals("Cancel"))
                    {
                        resultHolder.setResult(null);
                        xDialog.endExecute();
                    }
                    else
                    {
                        resultHolder.setResult(textField.getText());
                        xDialog.endExecute();
                    }
                }

                public void disposing(EventObject o) {
                    // does nothing
                }
            };

        okButton.addActionListener(listener);
        cancelButton.addActionListener(listener);

        xDialog.execute();

        return (String)resultHolder.getResult();
    }

    private XDialog createConfirmDialog(String title, String prompt)
        throws com.sun.star.uno.Exception
    {
        if (title == null || title.equals(""))
        {
            title = "Scripting Framework";
        }

        if (prompt == null || prompt.equals(""))
        {
            prompt = "Enter name";
        }

        // get the service manager from the component context
        XMultiComponentFactory xMultiComponentFactory =
            xComponentContext.getServiceManager();

        // create the dialog model and set the properties
        Object dialogModel = xMultiComponentFactory.createInstanceWithContext(
            "com.sun.star.awt.UnoControlDialogModel", xComponentContext);

        XPropertySet props = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, dialogModel);

        props.setPropertyValue("Title", title);
        setDimensions(dialogModel, 100, 100, 157, 37);

        // get the service manager from the dialog model
        XMultiServiceFactory xMultiServiceFactory =
            (XMultiServiceFactory) UnoRuntime.queryInterface(
                XMultiServiceFactory.class, dialogModel);

        // create the label model and set the properties
        Object label = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlFixedTextModel");

        setDimensions(label, 15, 5, 134, 12);

        XPropertySet labelProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, label);
        labelProps.setPropertyValue("Name", "PromptLabel");
        labelProps.setPropertyValue("Label", prompt);

        // create the Run Macro button model and set the properties
        Object okButtonModel = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlButtonModel");

        setDimensions(okButtonModel, 40, 18, 38, 15);

        XPropertySet buttonProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, okButtonModel);
        buttonProps.setPropertyValue("Name", "Ok");
        buttonProps.setPropertyValue("Label", "Ok");

        // create the Dont Run Macro button model and set the properties
        Object cancelButtonModel = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlButtonModel");

        setDimensions(cancelButtonModel, 83, 18, 38, 15);

        buttonProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, cancelButtonModel);
        buttonProps.setPropertyValue("Name", "Cancel");
        buttonProps.setPropertyValue("Label", "Cancel");

        // insert the control models into the dialog model
        XNameContainer xNameCont = (XNameContainer) UnoRuntime.queryInterface(
            XNameContainer.class, dialogModel);

        xNameCont.insertByName("PromptLabel", label);
        xNameCont.insertByName("Ok", okButtonModel);
        xNameCont.insertByName("Cancel", cancelButtonModel);

        // create the dialog control and set the model
        Object dialog = xMultiComponentFactory.createInstanceWithContext(
            "com.sun.star.awt.UnoControlDialog", xComponentContext);
        XControl xControl = (XControl) UnoRuntime.queryInterface(
            XControl.class, dialog);

        XControlModel xControlModel = (XControlModel)
            UnoRuntime.queryInterface(XControlModel.class, dialogModel);
        xControl.setModel(xControlModel);

        // create a peer
        Object toolkit = xMultiComponentFactory.createInstanceWithContext(
            "com.sun.star.awt.ExtToolkit", xComponentContext);
        XToolkit xToolkit = (XToolkit) UnoRuntime.queryInterface(
            XToolkit.class, toolkit);
        XWindow xWindow = (XWindow) UnoRuntime.queryInterface(
            XWindow.class, xControl);
        xWindow.setVisible(false);
        xControl.createPeer(xToolkit, null);

        return (XDialog) UnoRuntime.queryInterface(XDialog.class, dialog);
    }

    private void setDimensions(Object o, int x, int y, int width, int height)
        throws com.sun.star.uno.Exception
    {
        XPropertySet props = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, o);

        props.setPropertyValue("PositionX", new Integer(x));
        props.setPropertyValue("PositionY", new Integer(y));
        props.setPropertyValue("Height", new Integer(height));
        props.setPropertyValue("Width", new Integer(width));
    }

    private XDialog createInputDialog(String title, String prompt)
        throws com.sun.star.uno.Exception
    {
        if (title == null || title.equals(""))
        {
            title = "Scripting Framework";
        }

        if (prompt == null || prompt.equals(""))
        {
            prompt = "Enter name";
        }

        // get the service manager from the component context
        XMultiComponentFactory xMultiComponentFactory =
            xComponentContext.getServiceManager();

        // create the dialog model and set the properties
        Object dialogModel = xMultiComponentFactory.createInstanceWithContext(
            "com.sun.star.awt.UnoControlDialogModel", xComponentContext);

        setDimensions(dialogModel, 100, 100, 157, 58);

        XPropertySet props = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, dialogModel);
        props.setPropertyValue("Title", title);

        // get the service manager from the dialog model
        XMultiServiceFactory xMultiServiceFactory =
            (XMultiServiceFactory) UnoRuntime.queryInterface(
                XMultiServiceFactory.class, dialogModel);

        // create the label model and set the properties
        Object label = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlFixedTextModel");

        setDimensions(label, 15, 5, 134, 12);

        XPropertySet labelProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, label);
        labelProps.setPropertyValue("Name", "PromptLabel");
        labelProps.setPropertyValue("Label", prompt);

        // create the text field
        Object edit = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlEditModel");

        setDimensions(edit, 15, 18, 134, 12);

        XPropertySet editProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, edit);
        editProps.setPropertyValue("Name", "NameField");

        // create the OK button
        Object okButtonModel = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlButtonModel");

        setDimensions(okButtonModel, 40, 39, 38, 15);

        XPropertySet buttonProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, okButtonModel);
        buttonProps.setPropertyValue("Name", "Ok");
        buttonProps.setPropertyValue("Label", "Ok");

        // create the Cancel button
        Object cancelButtonModel = xMultiServiceFactory.createInstance(
            "com.sun.star.awt.UnoControlButtonModel");

        setDimensions(cancelButtonModel, 83, 39, 38, 15);

        buttonProps = (XPropertySet) UnoRuntime.queryInterface(
            XPropertySet.class, cancelButtonModel);
        buttonProps.setPropertyValue("Name", "Cancel");
        buttonProps.setPropertyValue("Label", "Cancel");

        // insert the control models into the dialog model
        XNameContainer xNameCont = (XNameContainer)
            UnoRuntime.queryInterface(XNameContainer.class, dialogModel);

        xNameCont.insertByName("PromptLabel", label);
        xNameCont.insertByName("NameField", edit);
        xNameCont.insertByName("Ok", okButtonModel);
        xNameCont.insertByName("Cancel", cancelButtonModel);

        // create the dialog control and set the model
        Object dialog = xMultiComponentFactory.createInstanceWithContext(
            "com.sun.star.awt.UnoControlDialog", xComponentContext);
        XControl xControl = (XControl) UnoRuntime.queryInterface(
            XControl.class, dialog);

        XControlModel xControlModel = (XControlModel)
            UnoRuntime.queryInterface(XControlModel.class, dialogModel);
        xControl.setModel(xControlModel);

        // create a peer
        Object toolkit = xMultiComponentFactory.createInstanceWithContext(
            "com.sun.star.awt.ExtToolkit", xComponentContext);
        XToolkit xToolkit = (XToolkit) UnoRuntime.queryInterface(
            XToolkit.class, toolkit);
        XWindow xWindow = (XWindow) UnoRuntime.queryInterface(
            XWindow.class, xControl);
        xWindow.setVisible(false);
        xControl.createPeer(xToolkit, null);

        return (XDialog) UnoRuntime.queryInterface(XDialog.class, dialog);
    }

    private static class ResultHolder {
        private Object result = null;

        public Object getResult()
        {
            return result;
        }

        public void setResult(Object result)
        {
            this.result = result;
        }
    }
}
