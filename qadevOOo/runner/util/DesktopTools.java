/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: DesktopTools.java,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: ihi $ $Date: 2007-06-04 13:31:42 $
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
package util;

import com.sun.star.awt.Rectangle;
import com.sun.star.awt.WindowDescriptor;
import com.sun.star.awt.XToolkit;
import com.sun.star.awt.XWindowPeer;
import com.sun.star.awt.XTopWindow;
import com.sun.star.beans.PropertyState;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XEnumeration;
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.container.XNameContainer;
import com.sun.star.container.XNameReplace;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.uno.UnoRuntime;

// access the implementations via names
import com.sun.star.uno.XInterface;
import com.sun.star.util.XChangesBatch;
import com.sun.star.util.XCloseable;
import com.sun.star.util.XModifiable;
import com.sun.star.view.XViewSettingsSupplier;
import helper.ConfigHelper;
import java.util.Vector;
import lib.StatusException;


/**
 * contains helper methods for the Desktop
 */
public class DesktopTools {
    /**
     * Queries the XComponentLoader
     *
     * @param xMSF the MultiServiceFactory
     * @return the gained XComponentLoader
    */
    public static XComponentLoader getCLoader(XMultiServiceFactory xMSF) {
        XDesktop oDesktop = (XDesktop) UnoRuntime.queryInterface(
                                    XDesktop.class, createDesktop(xMSF));

        XComponentLoader oCLoader = (XComponentLoader) UnoRuntime.queryInterface(
                                            XComponentLoader.class, oDesktop);

        return oCLoader;
    } // finish getCLoader

    /**
     * Creates an Instance of the Desktop service
     *
     * @param xMSF the MultiServiceFactory
     * @return the gained Object
    */
    public static Object createDesktop(XMultiServiceFactory xMSF) {
        Object oInterface;

        try {
            oInterface = xMSF.createInstance("com.sun.star.comp.framework.Desktop");
        } catch (com.sun.star.uno.Exception e) {
            throw new IllegalArgumentException("Desktop Service not available");
        }

        return oInterface;
    } //finish createDesktop

    /**
     * returns a XEnumeration containing all components containing on the desktop
     * @param xMSF the XMultiServiceFactory
     * @return XEnumeration of all components on the desktop
     */
    public static XEnumeration getAllComponents(XMultiServiceFactory xMSF) {
        XDesktop xDesktop = (XDesktop) UnoRuntime.queryInterface(
                                    XDesktop.class, createDesktop(xMSF));
        return xDesktop.getComponents().createEnumeration();
    }

    /**
     * returns the current component on the desktop
     * @param xMSF the XMultiServiceFactory
     * @return XComponent of the current component on the desktop
     */
    public static XComponent getCurrentComponent(XMultiServiceFactory xMSF) {
        XDesktop xDesktop = (XDesktop) UnoRuntime.queryInterface(
                                    XDesktop.class, createDesktop(xMSF));
        return xDesktop.getCurrentComponent();
    }

    /**
     * returns an object arrary of all open documents
     * @param xMSF the MultiServiceFactory
     * @return returns an Array of document kinds like ["swriter"]
     */
    /**
     * returns an array of all open documents
     * @param xMSF the XMultiSerivceFactory
     * @return returns an array of all open documents
     */
    public static Object[] getAllOpenDocuments(XMultiServiceFactory xMSF) {
        Vector components = new Vector();
        XDesktop xDesktop = (XDesktop) UnoRuntime.queryInterface(
                                    XDesktop.class, createDesktop(xMSF));

        XEnumeration allComp = getAllComponents(xMSF);

        while (allComp.hasMoreElements()){
            try{
                XComponent xComponent = (XComponent) UnoRuntime.queryInterface(
                                       XComponent.class, allComp.nextElement());

                if (getDocumentType(xComponent) != null) components.add(xComponent);

            } catch (com.sun.star.container.NoSuchElementException e) {
            } catch ( com.sun.star.lang.WrappedTargetException e) {}
        }
        return components.toArray();
    }

    /**
     * Returns the document type for the given XComponent of an document
     * @param xComponent the document to query for its type
     * @return possible:
     * <ul>
     * <li>swriter</li>
     * <li>scalc</li>
     * <li>sdraw</li>
     * <li>smath</li>
     * </ul>
     * or <CODE>null</CODE>
     */
    public static String getDocumentType(XComponent xComponent) {
        XServiceInfo sInfo = (XServiceInfo)UnoRuntime.queryInterface(
                XServiceInfo.class, xComponent);

        if (sInfo == null) {
            return "";
        } else if (sInfo.supportsService("com.sun.star.sheet.SpreadsheetDocument")) {
            return "scalc";
        } else if (sInfo.supportsService("com.sun.star.text.TextDocument")) {
            return "swriter";
        } else if (sInfo.supportsService("com.sun.star.drawing.DrawingDocument")) {
            return "sdraw";
        } else if (sInfo.supportsService("com.sun.star.presentation.PresentationDocument")) {
            return "simpress";
        } else if (sInfo.supportsService("com.sun.star.formula.FormulaProperties")) {
            return "smath";
        } else {
            return null;
        }
    }


    /**
     * Opens a new document of a given kind
     * with arguments
     * @return the XComponent Interface of the document
     * @param kind the kind of document to load.<br>
     * possible:
     * <ul>
     * <li>swriter</li>
     * <li>scalc</li>
     * <li>sdaw</li>
     * <li>smath</li>
     * </ul>
     * @param Args arguments which passed to the document to load
     * @param xMSF the MultiServiceFactory
     */
    public static XComponent openNewDoc(XMultiServiceFactory xMSF, String kind,
                                        PropertyValue[] Args) {
        XComponent oDoc = null;

        try {
            oDoc = getCLoader(xMSF)
                       .loadComponentFromURL("private:factory/" + kind,
                                             "_blank", 0, Args);
        } catch (com.sun.star.uno.Exception e) {
            throw new IllegalArgumentException("Document could not be opened");
        }

        return oDoc;
    } //finish openNewDoc

    /**
     * loads a document of from a given url
     * with arguments
     * @return the XComponent Interface of the document
     * @param url the URL of the document to load.
     * @param Args arguments which passed to the document to load
     * @param xMSF the MultiServiceFactory
     */
    public static XComponent loadDoc(XMultiServiceFactory xMSF, String url,
                                     PropertyValue[] Args) {
        XComponent oDoc = null;
        if (Args == null)
        {
            Args = new PropertyValue[0];
        }
        try
        {
            oDoc = getCLoader(xMSF).loadComponentFromURL(url, "_blank", 0, Args);
        }
        catch (com.sun.star.uno.Exception e)
        {
            throw new IllegalArgumentException("Document could not be loaded");
        }

        return oDoc;
    } //finish openNewDoc

    /**
     * closes a given document
     * @param DocumentToClose the document to close
     */
    public static void closeDoc(XInterface DocumentToClose) {
        String kd = System.getProperty("KeepDocument");
        if (kd != null ) {
            System.out.println("The property 'KeepDocument' is set and so the document won't be disposed");
            return;
        }
        XModifiable modified = (XModifiable) UnoRuntime.queryInterface(
                                       XModifiable.class, DocumentToClose);
        XCloseable closer = (XCloseable) UnoRuntime.queryInterface(
                                    XCloseable.class, DocumentToClose);

        try {
            modified.setModified(false);
            closer.close(true);
        } catch (com.sun.star.util.CloseVetoException e) {
            System.out.println("Couldn't close document");
        } catch (com.sun.star.lang.DisposedException e) {
            System.out.println("Couldn't close document");
        } catch (java.lang.NullPointerException e) {
            System.out.println("Couldn't close document");
        } catch (com.sun.star.beans.PropertyVetoException e) {
            System.out.println("Couldn't close document");
        }
    }

    /**
     * Creates a floating XWindow with the size of X=500 Y=100 width=400 height=600
     * @param xMSF the MultiServiceFactory
     * @throws lib.StatusException if it is not possible to create a floating window a lib.StatusException was thrown
     * @return a floating XWindow
     */
    public static XWindowPeer createFloatingWindow(XMultiServiceFactory xMSF)
        throws StatusException{
            return createFloatingWindow(xMSF, 500, 100, 400, 600);
    }
     /**
     * Creates a floating XWindow on the given position and size.
     * @return a floating XWindow
     * @param X the X-Postion of the floating XWindow
     * @param Y the Y-Postion of the floating XWindow
     * @param width the width of the floating XWindow
     * @param height the height of the floating XWindow
     * @param xMSF the MultiServiceFactory
     * @throws lib.StatusException if it is not possible to create a floating window a lib.StatusException was thrown
     */
    public static XWindowPeer createFloatingWindow(XMultiServiceFactory xMSF, int X, int Y, int width, int height)
        throws StatusException{

        XInterface oObj = null;

        try {
            oObj = (XInterface) xMSF.createInstance("com.sun.star.awt.Toolkit");
        } catch (com.sun.star.uno.Exception e) {
            throw new StatusException("Couldn't get toolkit", e);
        }

        XToolkit tk = (XToolkit) UnoRuntime.queryInterface(
                                      XToolkit.class, oObj);

    WindowDescriptor descriptor = new com.sun.star.awt.WindowDescriptor();

        descriptor.Type = com.sun.star.awt.WindowClass.TOP;
        descriptor.WindowServiceName = "modelessdialog";
        descriptor.ParentIndex =  -1;

    Rectangle bounds = new com.sun.star.awt.Rectangle();
    bounds.X = X;
    bounds.Y = Y;
    bounds.Width = width;
    bounds.Height = height;

        descriptor.Bounds = bounds;
        descriptor.WindowAttributes = (com.sun.star.awt.WindowAttribute.BORDER +
                                      com.sun.star.awt.WindowAttribute.MOVEABLE +
                                      com.sun.star.awt.WindowAttribute.SIZEABLE +
                                      com.sun.star.awt.WindowAttribute.CLOSEABLE +
                                      com.sun.star.awt.VclWindowPeerAttribute.CLIPCHILDREN);

        XWindowPeer xWindow = null;

        try{
            xWindow = tk.createWindow( descriptor );
        }catch ( com.sun.star.lang.IllegalArgumentException e){
            throw new StatusException("Could not create window",  e);
        }

        return xWindow;

    }

    /**
     * zoom to have a view over the hole page
     * @param xDoc the document to zoom
     */
    public static void zoomToEntirePage( XInterface xDoc){
        try {
            XModel xMod = (XModel) UnoRuntime.queryInterface(XModel.class, xDoc);
            XInterface oCont = xMod.getCurrentController();
            XViewSettingsSupplier oVSSupp = (XViewSettingsSupplier)
                UnoRuntime.queryInterface(XViewSettingsSupplier.class, oCont);

            XInterface oViewSettings = oVSSupp.getViewSettings();
            XPropertySet oViewProp = (XPropertySet)
                   UnoRuntime.queryInterface(XPropertySet.class, oViewSettings);
            oViewProp.setPropertyValue("ZoomType",
                     new Short(com.sun.star.view.DocumentZoomType.ENTIRE_PAGE));

            utils.shortWait(5000);
        } catch (Exception e){
            System.out.println("Could not zoom to entire page: " + e.toString());
        }

    }


    /**
     * This function docks the Stylist onto the right side of the window.</p>
     * Note:<P>
     * Since the svt.viewoptions cache the view configuration at start up
     * the chage of the docking will be effective at a restart.
     * @param xMSF the XMultiServiceFactory
     */
    public static void dockStylist(XMultiServiceFactory xMSF){
        // prepare Window-Settings
        try {
            ConfigHelper aConfig = new ConfigHelper(xMSF,
                                    "org.openoffice.Office.Views", false);

            // Is node "5539" (slot-id for navigator) available? If not, insert it
            XNameReplace x5539 = aConfig.getOrInsertGroup("Windows", "5539");

            aConfig.updateGroupProperty(
               "Windows",  "5539", "WindowState", "952,180,244,349;1;0,0,0,0;");

            aConfig.insertOrUpdateExtensibleGroupProperty(
               "Windows", "5539", "UserData", "Data","V2,V,0,AL:(5,16,0/0/244/349,244;610)");

            // Is node "SplitWindow2" available? If not, instert it.
            aConfig.getOrInsertGroup("Windows", "SplitWindow2");

            aConfig.insertOrUpdateExtensibleGroupProperty(
               "Windows", "SplitWindow2","UserData", "UserItem","V1,2,1,0,5539");

            aConfig.flush();
            aConfig = null;

        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * This function brings a document to the front.<P>
     * NOTE: it is not possible to change the window order of your Window-Manager!!
     * Only the order of Office documents are changeable.
     * @param xModel the XModel of the document to bring to top
     */
    public static void bringWindowToFromt(XModel xModel){
        XTopWindow xTopWindow =
                (XTopWindow) UnoRuntime.queryInterface(
                XTopWindow.class,
                xModel.getCurrentController().getFrame().getContainerWindow());

        xTopWindow.toFront();
    }
}
