/*************************************************************************
 *
 *  $RCSfile: ImageList.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $  $Date: 2004-05-19 13:05:03 $
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
 */package com.sun.star.wizards.ui;

import com.sun.star.awt.*;
import com.sun.star.lang.EventObject;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.wizards.common.Helper;
import com.sun.star.wizards.common.Renderer;
import com.sun.star.wizards.ui.event.*;

import java.awt.ItemSelectable;
import java.awt.SystemColor;
import java.awt.event.ItemListener;

import javax.swing.ListModel;
import javax.swing.event.ListDataEvent;
import javax.swing.event.ListDataListener;

/**
 *
 * @author  rpiterman
 */
public class ImageList implements ListDataListener, ItemSelectable {

    private XControl imgContainer;
    private XFixedText lblImageText;
    private XFixedText grbxSelectedImage;
    private XButton btnBack;
    private XButton btnNext;
    private XFixedText lblCounter;
    private XControl images[];
    private boolean benabled;

    private Size gap = new Size(4, 4);
    private int cols = 4;
    private int rows = 3;
    private Size imageSize = new Size(20, 20);
    private Size pos;
    private Size selectionGap = new Size(2, 2);
    private boolean showButtons = true;
    private Short step;
    private final static Short NO_BORDER = new Short((short) 0);
    private boolean refreshOverNull = true;
    private int imageTextLines = 1;

    private boolean rowSelect = false;
    public int tabIndex;
    public Boolean scaleImages = Boolean.TRUE;
    public String name = "il";

    private int selected = -1;
    private int pageStart = 0;
    public int helpURL = 0;

    private CommonListener uiEventListener = new CommonListener();
    private ImageRenderer renderer;
    private ListModel listModel;
    public Renderer counterRenderer = new SimpleCounterRenderer();

    private Object dialogModel;

    private ImageKeyListener imageKeyListener;

    private static final Integer BACKGROUND_COLOR = new Integer(SystemColor.window.getRGB() + 16777216);

    private final static Short HIDE_PAGE = new Short((short) 99);
    private final static Integer TRANSPARENT = new Integer(-1);
    private final static int LINE_HEIGHT = 8;

    //private MethodInvocation METHOD_MOUSE_ENTER_IMAGE;
    //private MethodInvocation METHOD_MOUSE_EXIT_IMAGE;
    private MethodInvocation METHOD_MOUSE_PRESSED;

    /** Getter for property imageSize.
     * @return Value of property imageSize.
     *
     */
    public Size getImageSize() {
        return this.imageSize;
    }

    /** Setter for property imageSize.
     * @param imageSize New value of property imageSize.
     *
     */
    public void setImageSize(Size imageSize) {
        this.imageSize = imageSize;
    }

    public void create(UnoDialog2 dialog) {

        dialogModel = dialog.xDialogModel;

        int imageTextHeight = imageTextLines * LINE_HEIGHT;

        PeerConfigHelper peerConfigHelper = new PeerConfigHelper(dialog.xUnoDialog);

        MOVE_SELECTION_VALS[2] = step;

        imgContainer = dialog.insertImage(name + "lblContainer", new String[] { "BackgroundColor", "Border", "Height", "PositionX", "PositionY", "Step", "Width" }, new Object[] { BACKGROUND_COLOR, new Short((short) 1), new Integer((imageSize.Height + gap.Height) * rows + gap.Height + imageTextHeight + 1), new Integer(pos.Width), new Integer(pos.Height), step, new Integer((imageSize.Width + gap.Width) * cols + gap.Width)});

        peerConfigHelper.setPeerProperties(imgContainer, new String[] { "MouseTransparent" }, new Object[] { Boolean.TRUE });

        //XWindow win = (XWindow)UnoRuntime.queryInterface(XWindow.class,lblContainer);
        /*dialog.xWindow.addWindowListener(uiEventListener);
        String dName = (String)Helper.getUnoPropertyValue(dialog.xDialogModel,"Name");

        uiEventListener.add(dName,EventNames.EVENT_WINDOW_SHOWN,"disableContainerMouseEvents",this);
        */
        int selectionWidth = rowSelect ? (imageSize.Width + gap.Width) * cols - gap.Width + (selectionGap.Width * 2) : imageSize.Width + (selectionGap.Width * 2);

        grbxSelectedImage = dialog.insertLabel(name + "_grbxSelected", new String[] { "BackgroundColor", "Border", "Height", "PositionX", "PositionY", "Step", "Tabstop", "Width" }, new Object[] { TRANSPARENT, new Short((short) 1), new Integer(imageSize.Height + (selectionGap.Height * 2)), //height
            new Integer(0), //posx
            new Integer(0), //posy
            step, Boolean.TRUE, new Integer(selectionWidth)});

        XWindow xWindow = (XWindow) UnoRuntime.queryInterface(XWindow.class,grbxSelectedImage);
        xWindow.addMouseListener(new XMouseListener() {

            public void mousePressed(MouseEvent arg0) {
                focus(getImageIndexFor(getSelected()));
            }

            public void mouseReleased(MouseEvent arg0) {}

            public void mouseEntered(MouseEvent arg0) {}

            public void mouseExited(MouseEvent arg0) {}

            public void disposing(EventObject arg0) {}
        });


        final String[] pNames1 = new String[] { "Height", "HelpURL", "PositionX", "PositionY", "Step", "TabIndex", "Tabstop", "Width" };

        lblImageText = dialog.insertLabel(name + "_imageText", pNames1, new Object[] { new Integer(imageTextHeight), "", new Integer(pos.Width + 1), new Integer(pos.Height + (imageSize.Height + gap.Height) * rows + gap.Height), step, new Short((short)0), Boolean.FALSE, new Integer(cols * (imageSize.Width + gap.Width) + gap.Width - 2)});

        final Integer btnSize = new Integer(14);

        if (showButtons) {

            btnBack = dialog.insertButton(name + "_btnBack", "prevPage", this, pNames1, new Object[] { btnSize, "HID:" + helpURL++, new Integer(pos.Width), new Integer(pos.Height + (imageSize.Height + gap.Height) * rows + gap.Height + imageTextHeight + 1), step, new Short((short)(tabIndex + 1)), Boolean.TRUE, btnSize });

            btnNext = dialog.insertButton(name+ "_btnNext", "nextPage", this, pNames1, new Object[] { btnSize, "HID:" + helpURL++, new Integer(pos.Width + (imageSize.Width + gap.Width) * cols + gap.Width - btnSize.intValue() + 1), new Integer(pos.Height + (imageSize.Height + gap.Height) * rows + gap.Height + imageTextHeight + 1), step, new Short((short)(tabIndex + 2)), Boolean.TRUE, btnSize });

            lblCounter = dialog.insertLabel(name + "_lblCounter", pNames1, new Object[] { new Integer(LINE_HEIGHT), "", new Integer(pos.Width + btnSize.intValue() + 1), new Integer(pos.Height + (imageSize.Height + gap.Height) * rows + gap.Height + imageTextHeight + ((btnSize.intValue() - LINE_HEIGHT) / 2)), step, new Short((short)0), Boolean.FALSE, new Integer(cols * (imageSize.Width + gap.Width) + gap.Width - 2 * btnSize.intValue() - 1)});

            Helper.setUnoPropertyValue(getModel(lblCounter), "Align", new Short((short) 1));
            Helper.setUnoPropertyValue(getModel(btnBack),"Label","<");
            Helper.setUnoPropertyValue(getModel(btnNext),"Label",">");


        }

        imageKeyListener = new ImageKeyListener();
        tabIndex_ = new Short((short)tabIndex);

        images = new XControl[rows * cols];

        try {
            //METHOD_MOUSE_ENTER_IMAGE = new MethodInvocation("mouseEnterImage",this,Object.class);
            //METHOD_MOUSE_EXIT_IMAGE = new MethodInvocation("mouseExitImage",this,Object.class);
            METHOD_MOUSE_PRESSED = new MethodInvocation("mousePressed", this, Object.class);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }

        _imageHeight = new Integer(imageSize.Height);
        _imageWidth  = new Integer(imageSize.Width);

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                images[r * cols + c] = createImage(dialog, r, c);

        refreshImages();

        listModel.addListDataListener(this);

    }

    private Integer _imageHeight, _imageWidth;
    private final static String[] IMAGE_PROPS = new String[] { "Border", "BackgroundColor", "Height", "HelpURL", "PositionX", "PositionY", "ScaleImage", "Step", "TabIndex", "Tabstop", "Width" };
    //used for optimization
    private Short tabIndex_;

    private XControl createImage(UnoDialog2 dialog, int row, int col) {
        String imageName = name + "_image" + (row * cols + col);
        XControl image = dialog.insertImage(imageName, IMAGE_PROPS, new Object[] { NO_BORDER, BACKGROUND_COLOR, _imageHeight, "HID:" + helpURL++, new Integer(imagePosX(col)), new Integer(imagePosY(row)), scaleImages, step, tabIndex_ , Boolean.FALSE, _imageWidth });

        XWindow win = (XWindow) UnoRuntime.queryInterface(XWindow.class, image);
        win.addMouseListener(uiEventListener);
        win.addKeyListener(imageKeyListener);
        //uiEventListener.add(imageName,EventNames.EVENT_MOUSE_ENTERED,METHOD_MOUSE_ENTER_IMAGE);
        //uiEventListener.add(imageName,EventNames.EVENT_MOUSE_EXITED,METHOD_MOUSE_EXIT_IMAGE);
        uiEventListener.add(imageName, EventNames.EVENT_MOUSE_PRESSED, METHOD_MOUSE_PRESSED);

        return image;
    }

    private int imagePosX(int col) {
        return pos.Width + col * (imageSize.Width + gap.Width) + gap.Width;
    }

    private int imagePosY(int row) {
        return pos.Height + row * (imageSize.Height + gap.Height) + gap.Height;
    }

    private void refreshImages() {
        if (showButtons)
            refreshCounterText();
        hideSelection();
        if (refreshOverNull)
            for (int i = 0; i < images.length; i++)
                setVisible(images[i], false);
        boolean focusable = true;
        for (int i = 0; i < images.length; i++) {
            Helper.setUnoPropertyValue(images[i].getModel(), "ImageURL", renderer.getImageUrl(getObjectFor(i)));
            Helper.setUnoPropertyValue(images[i].getModel(), "Tabstop" , focusable ? Boolean.TRUE : Boolean.FALSE);
            if (refreshOverNull)
                setVisible(images[i], true);
            focusable = false;
        }
        refreshSelection();
    }

    private void refreshCounterText() {
        Helper.setUnoPropertyValue(getModel(lblCounter), "Label", counterRenderer.render(new Counter(pageStart + 1, pageEnd(), listModel.getSize())));
    }

    private int pageEnd() {
        int i = pageStart + cols * rows;
        if (i > listModel.getSize() - 1)
            return listModel.getSize();
        else
            return i;
    }

    private void refreshSelection() {
        if (selected < pageStart || selected >= (pageStart + rows * cols))
            hideSelection();
        else
            moveSelection(getImageIndexFor(selected));
    }

    private void hideSelection() {
        Helper.setUnoPropertyValue(getModel(grbxSelectedImage), "Step", HIDE_PAGE);
        setVisible(grbxSelectedImage, false);
    }

    private final static String[] MOVE_SELECTION = new String[] { "PositionX", "PositionY", "Step" };
    private Object[] MOVE_SELECTION_VALS = new Object[3];

    /** Utility field holding list of ItemListeners. */
    private transient java.util.ArrayList itemListenerList;

    private void moveSelection(int image) {
        //System.out.println(image);
        setVisible(grbxSelectedImage, false);

        int row = image / cols;
        int col = rowSelect ? 0 : image - (row * cols);

        MOVE_SELECTION_VALS[0] = new Integer(imagePosX(col) - selectionGap.Width);
        MOVE_SELECTION_VALS[1] = new Integer(imagePosY(row) - selectionGap.Height);

        Helper.setUnoPropertyValues(getModel(grbxSelectedImage), MOVE_SELECTION, MOVE_SELECTION_VALS);

        if (((Number) Helper.getUnoPropertyValue(dialogModel, "Step")).shortValue() == step.shortValue())
            setVisible(grbxSelectedImage, true);

        //now focus...
        for (int i = 0; i<images.length; i++)
            if (i != image)
                defocus(i);
            else
                Helper.setUnoPropertyValue(images[image].getModel(),"Tabstop",Boolean.TRUE);
    }


    private void setVisible(Object control, boolean visible) {
        ((XWindow) UnoRuntime.queryInterface(XWindow.class, control)).setVisible(visible);
    }

    /**
     *
     * @param i
     * @return the Object in the list model corresponding to the given image index.
     */
    private Object getObjectFor(int i) {
        int ii = getIndexFor(i);
        if (listModel.getSize() <= ii)
            return null;
        else
            return listModel.getElementAt(ii);
    }

    /**
     *
     * @param i
     * @return the index in the listModel for the given image index.
     */
    private int getIndexFor(int i) {
        return pageStart + i;
    }

    private int getImageIndexFor(int i) {
        return i - pageStart;
    }

    public void contentsChanged(ListDataEvent event) {
        /*//if the content that was changed is in the displayed range, refresh...
        if (  (event.getIndex1() <  getIndexFor(0)) // range is before...
              || event.getIndex0() > getIndexFor( cols*rows - 1)) ; //do nothing
        else
          refreshImages();
          */

        //selected = 0;
        //pageStart = 0;
        //if (event.refreshImages();
    }

    public void intervalAdded(ListDataEvent event) {
        if (event.getIndex0()<=selected) {
            if (event.getIndex1()<=selected)
                  selected += event.getIndex1()- event.getIndex0() + 1;
        }
        if (event.getIndex0() < pageStart || event.getIndex1() < (pageStart + getRows() + getCols()))
            refreshImages();
    }

    public void intervalRemoved(ListDataEvent event) {
        //contentsChanged(event);
    }

    /** Registers ItemListener to receive events.
     * @param listener The listener to register.
     *
     */
    public synchronized void addItemListener(ItemListener listener) {
        if (itemListenerList == null)
            itemListenerList = new java.util.ArrayList();

        itemListenerList.add(listener);
    }

    /** Removes ItemListener from the list of listeners.
     * @param listener The listener to remove.
     *
     */
    public synchronized void removeItemListener(ItemListener listener) {
        if (itemListenerList != null) {
            itemListenerList.remove(listener);
        }
    }

    /** Notifies all registered listeners about the event.
     *
     * @param event The event to be fired
     *
     */
    private void fireItemSelected() {
        java.awt.event.ItemEvent event = new java.awt.event.ItemEvent(this, 0,
            getSelectedObject(), java.awt.event.ItemEvent.SELECTED);
        java.util.ArrayList list;
        synchronized (this) {
            if (itemListenerList == null)
                return;
            list = (java.util.ArrayList) itemListenerList.clone();
        }
        for (int i = 0; i < list.size(); i++) {
            ((java.awt.event.ItemListener) list.get(i)).itemStateChanged(event);
        }
    }

    /**
     * @return
     */
    public int getCols() {
        return cols;
    }

    /**
     * @return
     */
    public Size getGap() {
        return gap;
    }

    /**
     * @return
     */
    public ListModel getListModel() {
        return listModel;
    }

    /**
    * @return
    */
    public Short getStep() {
        return step;
    }

    /**
     * @return
     */
    public int getPageStart() {
        return pageStart;
    }

    /**
     * @return
     */
    public Size getPos() {
        return pos;
    }

    /**
     * @return
     */
    public ImageRenderer getRenderer() {
        return renderer;
    }

    /**
     * @return
     */
    public int getRows() {
        return rows;
    }

    /**
     * @return
     */
    public int getSelected() {
        return selected;
    }

    /**
     * @return
     */
    public Size getSelectionGap() {
        return selectionGap;
    }

    /**
     * @return
     */
    public boolean isShowButtons() {
        return showButtons;
    }

    /**
     * @param i
     */
    public void setCols(int i) {
        cols = i;
    }

    /**
     * @param size
     */
    public void setGap(Size size) {
        gap = size;
    }

    /**
     * @param model
     */
    public void setListModel(ListModel model) {
        listModel = model;
    }

    /**
     * @param short1
     */
    public void setStep(Short short1) {
        step = short1;
    }

    /**
     * @param i
     */
    public void setPageStart(int i) {
        if (i == pageStart)
            return;
        pageStart = i;
        enableButtons();
        refreshImages();
    }

    /**
     * @param size
     */
    public void setPos(Size size) {
        pos = size;
    }

    /**
     * @param renderer
     */
    public void setRenderer(ImageRenderer renderer) {
        this.renderer = renderer;
    }

    /**
     * @param i
     */
    public void setRows(int i) {
        rows = i;
    }

    /**
     * @param i
     */
    public void setSelected(int i) {
        if (rowSelect && (i >= 0) )
            i = (i / cols) * cols;
        if (selected == i)
            return;
        selected = i;
        refreshImageText();
        refreshSelection();
        fireItemSelected();
    }

    public void setSelected(Object object) {
        if (object == null)
            setSelected(-1);
        else
            for (int i = 0; i < getListModel().getSize(); i++)
                if (getListModel().getElementAt(i).equals(object)) {
                    setSelected(i);
                    return;
                }
        setSelected(-1);

    }

    private void refreshImageText() {
        Object item = selected >= 0 ? getListModel().getElementAt(selected) : null;
        Helper.setUnoPropertyValue(getModel(lblImageText), "Label", " " + renderer.render(item));
    }

    /**
     * @param size
     */
    public void setSelectionGap(Size size) {
        selectionGap = size;
    }

    /**
     * @param b
     */
    public void setShowButtons(boolean b) {
        showButtons = b;
    }

    public void nextPage() {
        if (pageStart < getListModel().getSize() - rows * cols) {
            setPageStart(pageStart + rows * cols);
        }
    }

    public void prevPage() {
        if (pageStart == 0)
            return;
        int i = pageStart - rows * cols;
        if (i < 0)
            i = 0;
        setPageStart(i);
    }

    private void enableButtons() {
        enable(btnNext, new Boolean(pageStart + rows * cols < listModel.getSize()));
        enable(btnBack, new Boolean(pageStart > 0));
    }

    private void enable(Object control, Boolean enable) {
        Helper.setUnoPropertyValue(getModel(control), "Enabled", enable);
    }

    private Object getModel(Object control) {
        return ((XControl) UnoRuntime.queryInterface(XControl.class, control)).getModel();
    }

/*
    public void mouseEnterImage(Object event) {
        System.out.println("mouse enter");
        int i = getImageFromEvent(event);
        //TODO what is when the image does not display an image?
        if (getIndexFor(i) != selected)
          setBorder(images[i],imageBorderMO);
    }

    public void mouseExitImage(Object event) {
        //System.out.println("mouse exit");
        int i = getImageFromEvent(event);
        //TODO what is when the image does not display an image?
        if (getIndexFor(i) != selected)
          setBorder(images[i],imageBorder);
    }
*/

    private void setBorder(Object control, Short border) {
        Helper.setUnoPropertyValue(getModel(control), "Border", border);
        //XWindowPeer peer = ((XControl)UnoRuntime.queryInterface(XControl.class,control)).getPeer();
        //peer.invalidate(InvalidateStyle.CHILDREN);
    }

    private int getImageFromEvent(Object event) {
        Object image = ((EventObject) event).Source;
        String controlName = (String) Helper.getUnoPropertyValue(getModel(image), "Name");
        return Integer.valueOf(controlName.substring(6  + name.length())).intValue();

    }

    public void mousePressed(Object event) {
        int image = getImageFromEvent(event);
        int index = getIndexFor(image);
        if (index < listModel.getSize()) {
            focus(image);
            setSelected(index);
        }
    }

    /* (non-Javadoc)
     * @see java.awt.ItemSelectable#getSelectedObjects()
     */
    public Object[] getSelectedObjects() {
        return new Object[] { getListModel().getElementAt(selected)};
    }

    /**
     */
    public static interface ImageRenderer extends Renderer {

        /**
         *
         * @param listItem
         * @return an url for the image of the given item,
         * or an empty string if the given argument is null.
         */
        public String getImageUrl(Object listItem);
    }

    private static class SimpleCounterRenderer implements Renderer {
        public String render(Object counter) {
            return "" + ((Counter) counter).start + ".." + ((Counter) counter).end + "/" + ((Counter) counter).max;
        }
    }

    public static class Counter {
        public int start, end, max;

        public Counter(int start_, int end_, int max_) {
            start = start_;
            end = end_;
            max = max_;
        }
    }

    public Object getSelectedObject() {
        return selected >= 0 ? getListModel().getElementAt(selected) : null;
    }

    public void showSelected() {
        int oldPageStart = pageStart;
        if (selected == -1)
            pageStart += 0;
        else
            pageStart = (selected / images.length) * images.length;
        if (oldPageStart != pageStart) {
            enableButtons();
            refreshImages();
        }
    }

    public void setRowSelect(boolean b) {
        rowSelect = b;
    }

    public boolean isRowSelect() {
        return rowSelect;
    }



    private class ImageKeyListener implements XKeyListener {

        /* (non-Javadoc)
         * @see com.sun.star.awt.XKeyListener#keyPressed(com.sun.star.awt.KeyEvent)
         */
        public void keyPressed(KeyEvent ke) {
            int image = getImageFromEvent(ke);
            int r = image / getCols();
            int c = image - (r * getCols());
            int d = getKeyMove(ke,r,c);
            int newImage = image + d ;
            if (newImage == image)
                return;
            if (isFocusable(newImage))
                changeFocus(image,newImage);
        }

        private boolean isFocusable(int image) {
            return (image >= 0) && (getIndexFor(image) < listModel.getSize());
        }

        private void changeFocus(int oldFocusImage, int newFocusImage) {
            focus(newFocusImage);
            defocus(oldFocusImage);
        }


        private final int getKeyMove(KeyEvent ke, int row, int col) {
            switch (ke.KeyCode) {
                case Key.UP :
                    if (row > 0) return 0 - getCols();
                    break;
                case Key.DOWN :
                    if (row < getRows() - 1) return getCols();
                    break;
                case Key.LEFT :
                    if (col > 0) return -1;
                    break;
                case Key.RIGHT :
                    if (col < getCols() - 1) return 1;
                    break;
                case Key.SPACE :
                    select(ke);
            }
            return 0;
        }

        private void select(KeyEvent ke) {
            setSelected(getIndexFor(getImageFromEvent(ke)));
        }


        public void keyReleased(KeyEvent ke) {}

        public void disposing(EventObject arg0) {}

    }

    private final void focus(int image) {
        Helper.setUnoPropertyValue(images[image].getModel(),"Tabstop",
            Boolean.TRUE );
        XWindow xWindow = (XWindow) UnoRuntime.queryInterface(XWindow.class,images[image]);
        xWindow.setFocus();
    }

    private final void defocus(int image) {
        Helper.setUnoPropertyValue(UnoDialog.getModel(images[image]),"Tabstop",
            Boolean.FALSE);

    }
    /**
     * jump to the given item (display the screen
     * that contains the given item).
     * @param i
     */
    public void display(int i) {
        int is = (getCols() * getRows());
        int ps = (listModel.getSize() / is ) * is ;
        setPageStart(ps);
    }


    /**
     * @return
     */
    public boolean isenabled() {
        return benabled;
    }

    /**
     * @param b
     */
    public void setenabled(boolean b) {
        benabled = b;
    }

}
