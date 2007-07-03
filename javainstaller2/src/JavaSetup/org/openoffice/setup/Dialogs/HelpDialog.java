/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: HelpDialog.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 11:53:34 $
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

package org.openoffice.setup.Dialogs;

import org.openoffice.setup.InstallData;
import org.openoffice.setup.ResourceManager;
import org.openoffice.setup.SetupFrame;
import org.openoffice.setup.Util.DialogFocusTraversalPolicy;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionListener;
import java.io.File;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JEditorPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.border.EmptyBorder;

public class HelpDialog extends JDialog implements ActionListener {

    private JButton okButton;
    private JEditorPane editorPane;
    private JScrollPane editorScrollPane;
    private String helpFileName;
    private String helpFileString;

    public HelpDialog(SetupFrame setupFrame) {

        super(setupFrame.getDialog());

        helpFileString = setupFrame.getCurrentPanel().getHelpFileName();
        helpFileName = ResourceManager.getFileName(helpFileString);
        // String dialogName = setupFrame.getCurrentPanel().getName();

        String helpTitle = ResourceManager.getString("String_Help");
        setTitle(helpTitle);
        // setLayout(new java.awt.BorderLayout());
        this.getContentPane().setLayout(new java.awt.BorderLayout());

        JPanel toppanel = new JPanel();
        toppanel.setLayout(new java.awt.BorderLayout());
        toppanel.setBorder(new EmptyBorder(new Insets(5, 10, 5, 10)));

        JPanel buttonpanel = new JPanel();
        buttonpanel.setLayout(new java.awt.FlowLayout());
        buttonpanel.setBorder(new EmptyBorder(new Insets(5, 10, 5, 10)));

        //Create an editor pane.
        editorPane = createEditorPane();
        editorScrollPane = new JScrollPane(editorPane);
        editorScrollPane.setPreferredSize(new Dimension(250, 145));
        editorScrollPane.setBorder(new EmptyBorder(new Insets(5, 10, 5, 10)));

        // String helpTitle1 = null;
        // InstallData data = InstallData.getInstance();
        // if ( data.isInstallationMode() ) {
        //     helpTitle1 = ResourceManager.getString("String_Help_Title_1");
        // } else {
        //     helpTitle1 = ResourceManager.getString("String_Help_Title_1_Uninstallation");
        // }

        // PanelLabel label1 = new PanelLabel(helpTitle1, true);
        // String helpTitle2 = ResourceManager.getString("String_Help_Title_2");
        // PanelLabel label2 = new PanelLabel(helpTitle2);

        String okString = ResourceManager.getString("String_OK");
        okButton = new JButton(okString);
        okButton.setEnabled(true);
        okButton.addActionListener(this);

        JSeparator separator = new JSeparator();

        // toppanel.add(label1, BorderLayout.NORTH);
        // toppanel.add(label2, BorderLayout.CENTER);
        buttonpanel.add(okButton);

        this.getContentPane().add(toppanel, BorderLayout.NORTH);
        this.getContentPane().add(editorScrollPane, BorderLayout.CENTER);
        this.getContentPane().add(buttonpanel, BorderLayout.SOUTH);

        // Setting tab-order and focus on okButton
        DialogFocusTraversalPolicy policy = new DialogFocusTraversalPolicy(new JComponent[] {okButton, editorScrollPane});
        this.setFocusTraversalPolicy(policy);  // set policy
        this.setFocusCycleRoot(true); // enable policy
    }

     private JEditorPane createEditorPane() {
        JEditorPane editorPane = new JEditorPane();
        editorPane.setEditable(false);

        InstallData data = InstallData.getInstance();
        File htmlDirectory = data.getInfoRoot("html");

        if ( htmlDirectory != null) {
            File htmlFile = new File(htmlDirectory, helpFileName);
            if (! htmlFile.exists()) {
                System.err.println("Couldn't find file: " + htmlFile.toString());
            }

            try {
                // System.err.println("URLPath: " + htmlFile.toURL());
                editorPane.setContentType("text/html;charset=utf-8");
                editorPane.setPage(htmlFile.toURL());
            } catch (Exception e) {
                e.printStackTrace();
                System.err.println("Attempted to read a bad URL");
            }
        }
        else {
            System.err.println("Did not find html directory");
        }

        return editorPane;
    }

//    public void setTabForScrollPane() {
//        JScrollBar ScrollBar = editorScrollPane.getVerticalScrollBar();
//        editorPane.setFocusable(true);
//        if ( ScrollBar.isShowing() ) {
//        } else {
//            editorPane.setFocusable(false);
//        }
//    }

    public void actionPerformed (java.awt.event.ActionEvent evt) {
        setVisible(false);
        dispose();
    }

}
