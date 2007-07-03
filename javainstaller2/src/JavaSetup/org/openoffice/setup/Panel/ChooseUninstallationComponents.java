/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ChooseUninstallationComponents.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-03 11:56:02 $
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

package org.openoffice.setup.Panel;

import org.openoffice.setup.PanelHelper.PanelLabel;
import org.openoffice.setup.PanelHelper.PanelTitle;
import org.openoffice.setup.PanelHelper.TreeNodeRenderer;
import org.openoffice.setup.ResourceManager;
import org.openoffice.setup.SetupData.DisplayPackageDescription;
import org.openoffice.setup.SetupData.SetupDataProvider;
import java.awt.BorderLayout;
import java.awt.Insets;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;


public class ChooseUninstallationComponents extends JPanel implements MouseListener, KeyListener, TreeSelectionListener {

    private JTree componentTree;
    private PanelLabel descriptionLabel;
    private PanelLabel sizeLabel;

    private String sizeString;

    public ChooseUninstallationComponents() {

        setLayout(new BorderLayout());
        setBorder(new EmptyBorder(new Insets(10, 10, 10, 10)));

        String titleText    = ResourceManager.getString("String_ChooseComponents1");
        String subtitleText = ResourceManager.getString("String_ChooseUninstallationComponents2");
        PanelTitle titleBox = new PanelTitle(titleText, subtitleText, 2, 40);
        // PanelTitle titleBox = new PanelTitle(titleText, subtitleText);
        titleBox.addVerticalStrut(20);
        add(titleBox, BorderLayout.NORTH);

        DefaultMutableTreeNode root = SetupDataProvider.createTree();

        componentTree = new JTree(root);
        componentTree.setShowsRootHandles(true);
        componentTree.setRootVisible(false);
        componentTree.setVisibleRowCount(3);
        componentTree.setCellRenderer(new TreeNodeRenderer());
        componentTree.addMouseListener( this );
        componentTree.addKeyListener( this );
        componentTree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        componentTree.addTreeSelectionListener(this);

        String BorderTitle = ResourceManager.getString("String_ChooseComponents3");
        TitledBorder PanelBorder = BorderFactory.createTitledBorder(BorderTitle);

        BorderLayout PanelLayout = new BorderLayout();
        PanelLayout.setHgap(20);
        JPanel DescriptionPanel = new JPanel();
        DescriptionPanel.setBorder(PanelBorder);
        DescriptionPanel.setLayout(PanelLayout);

        String DescriptionText = "";
        descriptionLabel = new PanelLabel(DescriptionText, 3, 20);
        sizeString = ResourceManager.getString("String_ChooseComponents4");
        sizeLabel = new PanelLabel(sizeString, 1, 5);

        DescriptionPanel.add(descriptionLabel, BorderLayout.CENTER);
        DescriptionPanel.add(sizeLabel, BorderLayout.EAST);

        add(new JScrollPane(componentTree), BorderLayout.CENTER);
        add(DescriptionPanel, BorderLayout.SOUTH);
    }

    private void updateNode(DefaultMutableTreeNode node) {
        if (node != null) {
            DisplayPackageDescription nodeInfo = (DisplayPackageDescription)node.getUserObject();
            nodeInfo.toggleState(node);

            DefaultTreeModel model = (DefaultTreeModel)componentTree.getModel();
            model.nodeChanged(node);

            descriptionLabel.setText(nodeInfo.getDescription());
            sizeLabel.setText(sizeString + nodeInfo.getSize());
        }
    }

    /**
     * Implement the MouseListener Interface
     */
    public void mouseClicked(MouseEvent event)  {
    }
    public void mouseEntered(MouseEvent event)  {
    }
    public void mouseExited(MouseEvent event)   {
    }
    public void mousePressed(MouseEvent event)  {
        TreePath selPath = componentTree.getPathForLocation( event.getX(), event.getY() );
        if ((selPath != null) && (componentTree.getPathBounds(selPath).getX() + 20 >= event.getX())) {
            updateNode((DefaultMutableTreeNode)selPath.getLastPathComponent());
        }
    }
    public void mouseReleased(MouseEvent e) {
    }

    /**
     * Implement the KeyListener Interface
     */
    public void keyPressed(KeyEvent event)  {
    }
    public void keyReleased(KeyEvent event) {
    }
    public void keyTyped(KeyEvent event)    {
        if ( event.getKeyChar() == ' ' ) {
            TreePath selPath = componentTree.getAnchorSelectionPath();
            if ( selPath != null ) {
                updateNode((DefaultMutableTreeNode)selPath.getLastPathComponent());
            }
        }
    }

    /**
     * Implement the TreeSelectionListener Interface.
     */
    public void valueChanged(TreeSelectionEvent event) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode)componentTree.getLastSelectedPathComponent();
        if (node == null) {
            descriptionLabel.setText("");
            sizeLabel.setText("");
        } else {
            DisplayPackageDescription nodeInfo = (DisplayPackageDescription)node.getUserObject();

            nodeInfo.updateSize(node); // important to set default values for nodes
            DefaultTreeModel model = (DefaultTreeModel)componentTree.getModel();
            model.nodeChanged(node);

            descriptionLabel.setText(nodeInfo.getDescription());
            sizeLabel.setText(sizeString + nodeInfo.getSize());
        }
    }
}
