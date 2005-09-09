/*************************************************************************
*
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TextDocument.java,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:41:56 $
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
 ************************************************************************/package com.sun.star.wizards.text;
import java.util.Calendar;
import java.util.GregorianCalendar;

import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.XNameAccess;
import com.sun.star.document.XDocumentInfo;
import com.sun.star.document.XDocumentInfoSupplier;
import com.sun.star.frame.XController;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XFramesSupplier;
import com.sun.star.frame.XModel;
import com.sun.star.frame.XTerminateListener;
import com.sun.star.frame.XStorable;
import com.sun.star.i18n.NumberFormatIndex;
import com.sun.star.awt.Rectangle;
import com.sun.star.awt.Size;
import com.sun.star.awt.XWindow;
import com.sun.star.awt.XWindowPeer;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.PropertyVetoException;
import com.sun.star.lang.Locale;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;

import com.sun.star.style.XStyle;
import com.sun.star.style.XStyleFamiliesSupplier;
import com.sun.star.task.XStatusIndicatorFactory;
import com.sun.star.text.XPageCursor;
import com.sun.star.text.XSimpleText;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextViewCursor;
import com.sun.star.text.XTextViewCursorSupplier;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.DateTime;
import com.sun.star.util.XModifiable;
import com.sun.star.util.XNumberFormatsSupplier;
import com.sun.star.util.XRefreshable;
import com.sun.star.wizards.common.Configuration;
import com.sun.star.wizards.common.Desktop;
import com.sun.star.wizards.common.Helper;
import com.sun.star.wizards.common.JavaTools;
import com.sun.star.wizards.common.Helper.DateUtils;
import com.sun.star.wizards.document.OfficeDocument;

public class TextDocument {
    public XComponent xComponent;
    public com.sun.star.text.XTextDocument xTextDocument;
    public com.sun.star.util.XNumberFormats NumberFormats;
    public com.sun.star.document.XDocumentInfo xDocInfo;
    public com.sun.star.task.XStatusIndicator xProgressBar;
    public com.sun.star.frame.XFrame xFrame;
    public XText xText;
    public XMultiServiceFactory xMSFDoc;
    public XMultiServiceFactory xMSF;
    public com.sun.star.util.XNumberFormatsSupplier xNumberFormatsSupplier;
    public com.sun.star.awt.XWindowPeer xWindowPeer;
    public int PageWidth;
    public int ScaleWidth;
    public Size DocSize;
    public com.sun.star.awt.Rectangle PosSize;
    public com.sun.star.lang.Locale CharLocale;
    public XStorable xStorable;

    //creates an instance of TextDocument and creates a frame
    public TextDocument(XMultiServiceFactory xMSF, XTerminateListener listener) {
        this.xMSF = xMSF;
        XDesktop xDesktop = Desktop.getDesktop(xMSF);
        xFrame = OfficeDocument.createNewFrame(xMSF, listener);
    }

    //creates an instance of TextDocument and creates a frame with an empty document
    public TextDocument(XMultiServiceFactory xMSF, boolean bshowStatusIndicator, boolean bgetCurrentFrame, XTerminateListener listener) {
        this.xMSF = xMSF;
        XDesktop xDesktop = Desktop.getDesktop(xMSF);

        if (bgetCurrentFrame) {
            XFramesSupplier xFrameSupplier = (XFramesSupplier) UnoRuntime.queryInterface(XFramesSupplier.class, xDesktop);
            xFrame = xFrameSupplier.getActiveFrame();
            xComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, xFrame.getController().getModel());
            xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, xComponent);
        } else {
            xFrame = OfficeDocument.createNewFrame(xMSF, listener);
            PropertyValue[] xEmptyArgs = new PropertyValue[0];
            Object oDoc = OfficeDocument.load(xFrame, "private:factory/swriter", "_self", xEmptyArgs);
            xTextDocument = (XTextDocument) oDoc;
            xComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, xTextDocument);
        }
        XWindow xWindow = xFrame.getComponentWindow();
        //PosSize = xFrame.getComponentWindow().getPosSize();

        if (bshowStatusIndicator) {
            XStatusIndicatorFactory xStatusIndicatorFactory = (XStatusIndicatorFactory) UnoRuntime.queryInterface(XStatusIndicatorFactory.class, xFrame);
            xProgressBar = xStatusIndicatorFactory.createStatusIndicator();
            xProgressBar.start("", 100);
            xProgressBar.setValue(5);
        }
        xWindowPeer = (XWindowPeer) UnoRuntime.queryInterface(XWindowPeer.class, xFrame.getComponentWindow());
        xMSFDoc = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
        xNumberFormatsSupplier = (XNumberFormatsSupplier) UnoRuntime.queryInterface(XNumberFormatsSupplier.class, xTextDocument);
        XDocumentInfoSupplier xDocInfoSuppl = (XDocumentInfoSupplier) UnoRuntime.queryInterface(XDocumentInfoSupplier.class, xTextDocument);
        xDocInfo = xDocInfoSuppl.getDocumentInfo();
        CharLocale = (Locale) Helper.getUnoStructValue((Object) xComponent, "CharLocale");
        xStorable = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
        xText = xTextDocument.getText();
    }


    //creates an instance of TextDocument and creates a frame with an empty document
    public TextDocument(XMultiServiceFactory xMSF,XTextDocument _textDocument, boolean bshowStatusIndicator) {
        this.xMSF = xMSF;
        XDesktop xDesktop = Desktop.getDesktop(xMSF);

                xFrame = _textDocument.getCurrentController().getFrame();
                xComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, _textDocument);
                xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, xComponent);
        XWindow xWindow = xFrame.getComponentWindow();
        //PosSize = xFrame.getComponentWindow().getPosSize();

        if (bshowStatusIndicator) {
            XStatusIndicatorFactory xStatusIndicatorFactory = (XStatusIndicatorFactory) UnoRuntime.queryInterface(XStatusIndicatorFactory.class, xFrame);
            xProgressBar = xStatusIndicatorFactory.createStatusIndicator();
            xProgressBar.start("", 100);
            xProgressBar.setValue(5);
        }
        xWindowPeer = (XWindowPeer) UnoRuntime.queryInterface(XWindowPeer.class, xFrame.getComponentWindow());
        xMSFDoc = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
        xNumberFormatsSupplier = (XNumberFormatsSupplier) UnoRuntime.queryInterface(XNumberFormatsSupplier.class, xTextDocument);

        XDocumentInfoSupplier xDocInfoSuppl = (XDocumentInfoSupplier) UnoRuntime.queryInterface(XDocumentInfoSupplier.class, xTextDocument);
        xDocInfo = xDocInfoSuppl.getDocumentInfo();
        CharLocale = (Locale) Helper.getUnoStructValue((Object) xComponent, "CharLocale");
        xText = xTextDocument.getText();
    }


    public XTextDocument loadAsPreview(String sDefaultTemplate, boolean asTemplate) {
        PropertyValue loadValues[] = new PropertyValue[3];
        //      open document in the Preview mode
        loadValues[0] = new PropertyValue();
        loadValues[0].Name = "ReadOnly";
        loadValues[0].Value = Boolean.TRUE;
        loadValues[1] = new PropertyValue();
        loadValues[1].Name = "AsTemplate";
        loadValues[1].Value = asTemplate ? Boolean.TRUE : Boolean.FALSE;
        loadValues[2] = new PropertyValue();
        loadValues[2].Name = "Preview";
        loadValues[2].Value = Boolean.TRUE;

        //set the preview document to non-modified mode in order to avoid the 'do u want to save' box
        if (xTextDocument != null){
            try {
                XModifiable xModi = (XModifiable) UnoRuntime.queryInterface(XModifiable.class, xTextDocument);
                xModi.setModified(false);
            } catch (PropertyVetoException e1) {
                // TODO Auto-generated catch block
                e1.printStackTrace();
            }
        }
        Object oDoc = OfficeDocument.load(xFrame, sDefaultTemplate, "_self", loadValues);
        xTextDocument = (com.sun.star.text.XTextDocument) oDoc;
        DocSize = getPageSize();
        xMSFDoc = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);

        ViewHandler myViewHandler = new ViewHandler(xMSFDoc, xTextDocument);
        try {
            myViewHandler.setViewSetting("ZoomType", new Short(com.sun.star.view.DocumentZoomType.ENTIRE_PAGE));
        } catch (Exception e) {
            e.printStackTrace();
        }

        TextFieldHandler myFieldHandler = new TextFieldHandler(xMSF, xTextDocument);
        myFieldHandler.updateDocInfoFields();

        return xTextDocument;

    }

    public Size getPageSize() {
        try {
            XStyleFamiliesSupplier xStyleFamiliesSupplier = (XStyleFamiliesSupplier) com.sun.star.uno.UnoRuntime.queryInterface(XStyleFamiliesSupplier.class, xTextDocument);
            com.sun.star.container.XNameAccess xNameAccess = null;
            xNameAccess = xStyleFamiliesSupplier.getStyleFamilies();
            com.sun.star.container.XNameContainer xPageStyleCollection = null;
            xPageStyleCollection = (com.sun.star.container.XNameContainer) UnoRuntime.queryInterface(com.sun.star.container.XNameContainer.class, xNameAccess.getByName("PageStyles"));
            XStyle xPageStyle = (XStyle) UnoRuntime.queryInterface(XStyle.class, xPageStyleCollection.getByName("First Page"));
            return (Size) Helper.getUnoPropertyValue(xPageStyle, "Size");

        } catch (Exception exception) {
            exception.printStackTrace(System.out);
            return null;
        }
    }

    //creates an instance of TextDocument and creates a frame and loads a document
    public TextDocument(XMultiServiceFactory xMSF, String URL, PropertyValue[] xArgs, XTerminateListener listener) {
        this.xMSF = xMSF;
        XDesktop xDesktop = Desktop.getDesktop(xMSF);

        xFrame = OfficeDocument.createNewFrame(xMSF,listener);
        Object oDoc = OfficeDocument.load(xFrame, URL, "_self", xArgs);
        xTextDocument = (XTextDocument) oDoc;
        xComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, xTextDocument);
        XWindow xWindow = xFrame.getComponentWindow();

        xWindowPeer = (XWindowPeer) UnoRuntime.queryInterface(XWindowPeer.class, xFrame.getComponentWindow());
        xMSFDoc = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
        xNumberFormatsSupplier = (XNumberFormatsSupplier) UnoRuntime.queryInterface(XNumberFormatsSupplier.class, xTextDocument);

        XDocumentInfoSupplier xDocInfoSuppl = (XDocumentInfoSupplier) UnoRuntime.queryInterface(XDocumentInfoSupplier.class, xTextDocument);
        xDocInfo = xDocInfoSuppl.getDocumentInfo();
        CharLocale = (Locale) Helper.getUnoStructValue((Object) xComponent, "CharLocale");
    }


    public static XTextCursor createTextCursor(Object oCursorContainer) {
        XSimpleText xText = (XSimpleText) UnoRuntime.queryInterface(XSimpleText.class, oCursorContainer);
        XTextCursor xTextCursor = xText.createTextCursor();
        return xTextCursor;
    }

    // Todo: This method is  unsecure because the last index is not necessarily the last section

    // Todo: This Routine should be  modified, because I cannot rely on the last Table in the document to be the last in the TextTables sequence
    // to make it really safe you must acquire the Tablenames before the insertion and after the insertion of the new Table. By comparing the
    // two sequences of tablenames you can find out the tablename of the last inserted Table

    // Todo: This method is  unsecure because the last index is not necessarily the last section

    public int getCharWidth(String ScaleString) {
        int iScale = 200;
        xTextDocument.lockControllers();
        int iScaleLen = ScaleString.length();
        com.sun.star.text.XTextCursor xTextCursor = createTextCursor(xTextDocument.getText());
        xTextCursor.gotoStart(false);
        com.sun.star.wizards.common.Helper.setUnoPropertyValue(xTextCursor, "PageDescName", "First Page");
        xTextCursor.setString(ScaleString);
        XTextViewCursorSupplier xViewCursor = (XTextViewCursorSupplier) UnoRuntime.queryInterface(XTextViewCursorSupplier.class, xTextDocument.getCurrentController());
        XTextViewCursor xTextViewCursor = xViewCursor.getViewCursor();
        xTextViewCursor.gotoStart(false);
        int iFirstPos = xTextViewCursor.getPosition().X;
        xTextViewCursor.gotoEnd(false);
        int iLastPos = xTextViewCursor.getPosition().X;
        iScale = (iLastPos - iFirstPos) / iScaleLen;
        xTextCursor.gotoStart(false);
        xTextCursor.gotoEnd(true);
        xTextCursor.setString("");
        xTextDocument.unlockControllers();
        return iScale;
    }

    public void unlockallControllers() {
        while (xTextDocument.hasControllersLocked() == true) {
            xTextDocument.unlockControllers();
        }
    }

    public void refresh () {
        XRefreshable xRefreshable = (XRefreshable) UnoRuntime.queryInterface(XRefreshable.class, xTextDocument);
        xRefreshable.refresh();
    }

    /**
     * This method sets the Author of a Wizard-generated template correctly
     * and adds a explanatory sentence to the template description.
     * @param WizardName The name of the Wizard.
     * @param TemplateDescription The old Description which is being appended with another sentence.
     * @return void.
     */
    public void setWizardTemplateDocInfo(String WizardName, String TemplateDescription) {
        try {
            Object uD = Configuration.getConfigurationRoot(xMSF, "/org.openoffice.UserProfile/Data", false);
            XNameAccess xNA = (XNameAccess) UnoRuntime.queryInterface(XNameAccess.class, uD);
            Object gn = xNA.getByName("givenname");
            Object sn = xNA.getByName("sn");
            String fullname = (String)gn + " " + (String)sn;

            Calendar cal = new GregorianCalendar();
            int year = cal.get(Calendar.YEAR);
            int month = cal.get(Calendar.MONTH);
            int day = cal.get(Calendar.DAY_OF_MONTH);
            DateTime currentDate = new DateTime();
            currentDate.Day = (short)day;
            currentDate.Month = (short)month;
            currentDate.Year = (short)year;
            DateUtils du = new DateUtils(xMSF, this.xTextDocument);
            int ff = du.getFormat( NumberFormatIndex.DATE_SYS_DDMMYY );
            String myDate = du.format(ff, currentDate);

            XDocumentInfoSupplier xDocInfoSuppl = (XDocumentInfoSupplier) UnoRuntime.queryInterface(XDocumentInfoSupplier.class, xTextDocument);
            XDocumentInfo xDocInfo = xDocInfoSuppl.getDocumentInfo();
            Helper.setUnoPropertyValue(xDocInfo, "Author", fullname);
            Helper.setUnoPropertyValue(xDocInfo, "ModifiedBy", fullname);
            String description = (String)Helper.getUnoPropertyValue(xDocInfo, "Description");
            description = description + " " + TemplateDescription;
            description = JavaTools.replaceSubString(description, WizardName, "<wizard_name>");
            description = JavaTools.replaceSubString(description, myDate, "<current_date>");
            Helper.setUnoPropertyValue(xDocInfo, "Description", description);
        } catch (NoSuchElementException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (WrappedTargetException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }



    /**
     * removes an arbitrary Object which supports the  'XTextContent' interface
     * @param oTextContent
     * @return
     */
    public boolean removeTextContent(Object oTextContent){
        try {
            XTextContent xTextContent = (XTextContent) UnoRuntime.queryInterface(XTextContent.class, oTextContent);
            xText.removeTextContent(xTextContent);
            return true;
        } catch (NoSuchElementException e) {
            e.printStackTrace(System.out);
            return false;
        }
    }

    /**
     * Apparently there is no other way to get the
     * page count of a text document other than using a cursor and
     * making it jump to the last page...
     * @param model the document model.
     * @return the page count of the document.
     */
    public static int getPageCount(Object model) {
        XModel xModel = (XModel)UnoRuntime.queryInterface(XModel.class, model);
        XController xController = xModel.getCurrentController();
        XTextViewCursorSupplier xTextVCS = (XTextViewCursorSupplier)
            UnoRuntime.queryInterface(XTextViewCursorSupplier.class,xController);
        XTextViewCursor xTextVC = xTextVCS.getViewCursor();
        XPageCursor xPC = (XPageCursor) UnoRuntime.queryInterface(XPageCursor.class,xTextVC);
        xPC.jumpToLastPage();
        return xPC.getPage();
    }

    /* Possible Values for "OptionString" are: "LoadCellStyles", "LoadTextStyles", "LoadFrameStyles",
                           "LoadPageStyles", "LoadNumberingStyles", "OverwriteStyles" */
}
