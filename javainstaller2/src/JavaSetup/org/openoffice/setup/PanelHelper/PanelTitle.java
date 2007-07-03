/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PanelTitle.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 11:58:24 $
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

package org.openoffice.setup.PanelHelper;

import java.awt.FlowLayout;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;

public class PanelTitle extends Box {

    private JLabel TitleLabel;

    public PanelTitle() {
         super(BoxLayout.PAGE_AXIS);
    }

    public PanelTitle(String title, String subtitle, int rows, int columns) {
        super(BoxLayout.PAGE_AXIS);
        init(title, subtitle, rows, columns);
    }

    public PanelTitle(String title, String subtitle) {
        super(BoxLayout.PAGE_AXIS);
        init(title, subtitle, 0, 0);
    }

    public PanelTitle(String title) {
        super (BoxLayout.PAGE_AXIS);
        init(title, null, 0, 0);
    }

    public void addVerticalStrut(int strut) {
        add(createVerticalStrut(strut));
    }

    public void setTitle(String title) {
        TitleLabel.setText(title);
    }

    // public void setSubtitle(String subtitle) {
    //     SubtitleLabel.setText(subtitle);
    // }

    private void init(String title, String subtitle, int rows, int columns) {

        TitleLabel = new JLabel(title);
        TitleLabel.setFocusable(false);
        JPanel TitlePanel = new JPanel();
        TitlePanel.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
        TitlePanel.add(TitleLabel);

        add(createVerticalStrut(10));
        add(TitlePanel);
        add(createVerticalStrut(10));
        add(new JSeparator());
        add(createVerticalStrut(20));

        if (subtitle != null) {
            PanelLabel SubtitleLabel = null;
            if ( rows > 0 ) {
                SubtitleLabel = new PanelLabel(subtitle, rows, columns );
            } else {
                SubtitleLabel = new PanelLabel(subtitle);
            }
            SubtitleLabel.setFocusable(false);
            // PanelLabel SubtitleLabel = new PanelLabel(subtitle, true);
            JPanel SubtitlePanel = new JPanel();
            SubtitlePanel.setLayout(new FlowLayout(FlowLayout.LEFT, 0, 0));
            SubtitlePanel.add(SubtitleLabel);

            add(SubtitlePanel);
        }
    }
}