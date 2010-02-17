/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

package org.openoffice.setup.Panel;

import org.openoffice.setup.PanelHelper.PanelLabel;
import org.openoffice.setup.PanelHelper.PanelTitle;
import org.openoffice.setup.ResourceManager;
import org.openoffice.setup.SetupActionListener;
import java.awt.BorderLayout;
import java.awt.ComponentOrientation;
import java.awt.Dimension;
import java.awt.Insets;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import org.openoffice.setup.InstallData;

public class UninstallationCompleted extends JPanel {

    public static final String ACTION_DETAILS   = "ActionDetails";
    public static final int BUTTON_DETAILS  = 5;
    private JButton mDetailsButton;
    private PanelLabel varLabel;
    private PanelTitle titleBox;

    public UninstallationCompleted() {

        InstallData data = InstallData.getInstance();

        setLayout(new java.awt.BorderLayout());
        setBorder(new EmptyBorder(new Insets(10, 10, 10, 10)));

        String titleText = ResourceManager.getString("String_UninstallationCompleted1");
        titleBox = new PanelTitle(titleText);
        add(titleBox, BorderLayout.NORTH);

        JPanel contentPanel = new JPanel();
        contentPanel.setLayout(new java.awt.BorderLayout());
        if ( data.useRtl() ) { contentPanel.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT); }

        String dialogText = ResourceManager.getString("String_UninstallationCompleted2");
        varLabel = new PanelLabel(dialogText, true);
        String text2 = ResourceManager.getString("String_InstallationCompleted3");
        PanelLabel label2 = new PanelLabel(text2);
        if ( data.useRtl() ) { label2.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT); }

        mDetailsButton = new JButton();
        String buttonText = ResourceManager.getString("String_InstallationCompleted_Button");
        mDetailsButton.setText(buttonText);
        mDetailsButton.setEnabled(true);
        if ( data.useRtl() ) { mDetailsButton.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT); }

        JPanel ButtonPanel = new JPanel();
        ButtonPanel.setLayout(new BorderLayout());
        ButtonPanel.setBorder(new EmptyBorder(new Insets(5, 10, 5, 10)));
        ButtonPanel.setPreferredSize(new Dimension(120, 44));
        ButtonPanel.add(mDetailsButton, BorderLayout.NORTH);
        if ( data.useRtl() ) { ButtonPanel.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT); }

        contentPanel.add(varLabel, BorderLayout.NORTH);
        contentPanel.add(ButtonPanel, BorderLayout.EAST);
        contentPanel.add(label2, BorderLayout.CENTER);

        add(contentPanel, BorderLayout.CENTER);
    }

    public void setDetailsButtonActionCommand(String actionCommand) {
        mDetailsButton.setActionCommand(actionCommand);
    }

    public void addDetailsButtonActionListener(SetupActionListener actionListener) {
        mDetailsButton.addActionListener(actionListener);
    }

    public void setTitleText(String s) {
        titleBox.setTitle(s);
    }

    public void setDialogText(String text) {
        varLabel.setText(text);
    }

}
