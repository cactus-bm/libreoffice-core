/*************************************************************************
 *
 *  $RCSfile: FormControlArranger.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: vg $  $Date: 2005-02-21 13:56:38 $
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
 *  Contributor(s): Berend Cornelius
 *
 */
package com.sun.star.wizards.form;
import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.container.XNameContainer;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdbc.DataType;
import com.sun.star.task.XStatusIndicator;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Exception;
import com.sun.star.wizards.common.Resource;
import com.sun.star.wizards.db.*;
import com.sun.star.wizards.document.Control;
import com.sun.star.wizards.document.DatabaseControl;
import com.sun.star.wizards.document.FormHandler;
import com.sun.star.wizards.document.Shape;
import com.sun.star.wizards.document.TimeStampControl;


public class FormControlArranger {
    XNameContainer xFormName;
    XMultiServiceFactory xMSF;
    public DatabaseControl[] DBControlList = null;
    public Control[] LabelControlList = null;
    private XStatusIndicator xProgressBar;
    private FieldColumn[] FieldColumns;
    DatabaseControl curDBControl;
    Control curLabelControl;
    int icurArrangement;
    boolean bIsFirstRun;
    boolean bIsVeryFirstRun;
    boolean bControlsareCreated;
    int cXOffset;
    int cYOffset;
    final int cVertDistance = 200;
    final int cHoriDistance = 300;
    final int cLabelGap = 100;
    final double CMAXREDUCTION = 0.7;
    FormHandler oFormHandler;
    int iReduceWidth;
    int nXTCPos;
    int nYTCPos;
    int nXDBPos;
    int nYDBPos;
    int nTCHeight;
    int nTCWidth;
    int nDBHeight;
    int nDBWidth;
    int nMaxTCWidth;
    int nFormWidth;
    int nFormHeight;
    int nMaxRowY;
    int nSecMaxRowY;
    int nMaxColRightX;
    int a;
    int StartA;
    int nMaxDBYPos = 0;     //the maximum YPosition of a DBControl in the form


    public FormControlArranger(FormHandler _oFormHandler, XNameContainer _xFormName, CommandMetaData oDBMetaData, XStatusIndicator _xProgressBar, Point _StartPoint, Size _FormSize) {
        FieldColumns = oDBMetaData.DBFieldColumns;
        xMSF = oDBMetaData.xMSF;
        xFormName = _xFormName;
        xProgressBar = _xProgressBar;
        LabelControlList = new Control[FieldColumns.length];
        DBControlList = new DatabaseControl[FieldColumns.length];
        oFormHandler = _oFormHandler;
        cXOffset = _StartPoint.X;
        cYOffset = _StartPoint.Y;
        setFormSize(_FormSize);
    }


    // Note: on all Controls except for the checkbox the Label has to be set
    // a bit under the DBControl because its Height is also smaller
    private int getLabelDiffHeight(){
        if (curDBControl != null){
            if (curDBControl.getControlType() == FormHandler.SOCHECKBOX)
                return 0;
        }
        return oFormHandler.getBasicLabelDiffHeight();
    }


    private int getCheckBoxDiffHeight(int LastIndex){
        if ((LastIndex > 0) && (LastIndex < DBControlList.length)){
            if (DBControlList[LastIndex].getControlType() == FormHandler.SOCHECKBOX){
                return (int)((oFormHandler.getDBRefHeight() - DBControlList[LastIndex].getDBHeight())/2);
            }
        }
        return 0;
    }


    private boolean isReducable(int _index){
        boolean bisreducable = false;
        int ntype = this.FieldColumns[_index].FieldType;
        switch(ntype)
        {
            case DataType.TINYINT:
            case DataType.SMALLINT:
            case DataType.INTEGER:
            case DataType.FLOAT:
            case DataType.DATE:
            case DataType.TIME:
            case DataType.TIMESTAMP:
            case DataType.REAL:
            case DataType.DOUBLE:
            case DataType.NUMERIC:
            case DataType.DECIMAL:
            case DataType.BIT:
                bisreducable = false;
                break;
            case DataType.VARCHAR:
                short nTextLen;
                try {
                    nTextLen = AnyConverter.toShort(DBControlList[_index].xPropertySet.getPropertyValue("MaxTextLen"));
                    if ((nTextLen == 0) || (nTextLen > 20))
                        bisreducable = true;
                } catch (Exception e) {
                    e.printStackTrace(System.out);
                }
                break;
            case DataType.BIGINT:
                bisreducable = true;
                break;
            default:
                bisreducable = true;
        }
        if (nTCWidth > 0.9 * CMAXREDUCTION * nDBWidth)
            bisreducable = false;
        return bisreducable;
    }


    private int getControlGroupWidth(){
        if (nDBWidth > nTCWidth)
            return nDBWidth;
        else
            return nTCWidth;
    }


    private void checkJustifiedPosition(int a){
        int nBaseWidth = nFormWidth + cXOffset;
        int nLeftDist = nMaxColRightX - nBaseWidth;
        int nRightDist = nBaseWidth - (DBControlList[a].getPosition().X - this.cHoriDistance);
        if (nLeftDist < 0.5 * nRightDist){
            // Fieldwidths in the line can be made smaller..
            adjustLineWidth(StartA, a, nLeftDist, -1);
            nYTCPos = nMaxRowY + cVertDistance;
            nYDBPos = nYTCPos + nTCHeight;
//          if ((nYDBPos + nDBHeight) > nMaxDBYPos)
//              nMaxDBYPos = nYDBPos + nDBHeight;
            nXTCPos = cXOffset;
            nXDBPos = cXOffset;
            bIsFirstRun = true;
            StartA = a + 1;
        }
        else{
            // FieldWidths in the line can be made wider...
            if (nYDBPos + nDBHeight == nMaxRowY){
                // The last Control was the highest in the row
                nYTCPos = nSecMaxRowY + cVertDistance;
            }
            else
                nYTCPos = nMaxRowY + cVertDistance;
            nYDBPos = nYTCPos + nTCHeight;
            nXDBPos = cXOffset;
            nXTCPos = cXOffset;
            this.LabelControlList[a].setPosition(new Point(cXOffset, nYTCPos));
            this.DBControlList[a].setPosition(new Point(cXOffset, nYDBPos));
            bIsFirstRun = true;
            if (nDBWidth > nTCWidth)
                checkOuterPoints(nXDBPos, nDBWidth, nYDBPos, nDBHeight, true);
            else
                checkOuterPoints(nXDBPos, nTCWidth, nYDBPos, nDBHeight, true);
            nXTCPos = nMaxColRightX + cHoriDistance;
            nXDBPos = nXTCPos;
            adjustLineWidth(StartA, a-1, nRightDist, 1);
            StartA = a;
        }
    }


    private int getCorrWidth(int StartIndex , int EndIndex , int nDist, int Widthfactor ){
    int ShapeCount;
        if (Widthfactor > 0){
            // shapes are made wide
            ShapeCount = EndIndex-StartIndex + 1;
        }
        else{
            // shapes are made more narrow
            ShapeCount = iReduceWidth;
        }
        return (nDist)/ShapeCount;
    }


    /**
     *
     * @param StartIndex
     * @param EndIndex
     * @param nDist
     * @param WidthFactor is either '+1' or '-1' and determines whether the control shapes widths are to be made smaller or larger
     */
    private void adjustLineWidth(int StartIndex , int EndIndex , int nDist, int WidthFactor ){
        int CorrWidth = getCorrWidth(StartIndex, EndIndex, nDist, WidthFactor);
        int iLocTCPosX = cXOffset;
        for (int i = StartIndex; i <= EndIndex; i++){
            int nControlBaseWidth = 0;
            curDBControl = this.DBControlList[i];
            curLabelControl = this.LabelControlList[i];
            if (i != StartIndex){
                curLabelControl.setPosition( new Point(iLocTCPosX, curLabelControl.getPosition().Y));
                curDBControl.setPosition( new Point(iLocTCPosX, curLabelControl.getPosition().Y + nTCHeight));
            }
            if (((curLabelControl.getSize().Width > curLabelControl.getSize().Width)) && (WidthFactor > 0))
                nControlBaseWidth = curLabelControl.getSize().Width;
            else
                nControlBaseWidth = curDBControl.getSize().Width;
            if (FieldColumns[i].FieldType == DataType.TIMESTAMP){
                TimeStampControl oDBTimeStampControl = (TimeStampControl) curDBControl;
                nControlBaseWidth = oDBTimeStampControl.getSize().Width;
                oDBTimeStampControl.setSize( new Size(nControlBaseWidth + WidthFactor * CorrWidth, oDBTimeStampControl.getSize().Height));
            }
            else
                curDBControl.setSize( new Size(nControlBaseWidth + WidthFactor * CorrWidth, curDBControl.getSize().Height));
            iLocTCPosX = curDBControl.getPosition().X + curDBControl.getSize().Width + cHoriDistance;
            if (curLabelControl.getSize().Width > curDBControl.getSize().Width)
                iLocTCPosX = curLabelControl.getPosition().X + curLabelControl.getSize().Width + cHoriDistance;
        }
        iReduceWidth = 0;
    }



    private void checkOuterPoints(int nXPos, int nWidth, int nYPos, int nHeight, boolean bIsDBField){
        int nColRightX;
        if (icurArrangement == FormWizard.SOTOPJUSTIFIED){
            if (bIsDBField){
                // Only at DBControls you can measure the Value of nMaxRowY
                if (bIsFirstRun){
                    nMaxRowY = nYPos + nHeight;
                    nSecMaxRowY = nMaxRowY;
                }
                else{
                    int nRowY = nYPos + nHeight;
                    if (nRowY >= nMaxRowY){
                        int nOldMaxRowY = nMaxRowY;
                        nSecMaxRowY = nOldMaxRowY;
                        nMaxRowY = nRowY;
                    }
                }
            }
        }
        // Find the outer right point
        if (bIsFirstRun){
            nMaxColRightX = nXPos + nWidth;
            bIsFirstRun = false;
        }
        else{
            nColRightX = nXPos + nWidth;
            if (nColRightX > nMaxColRightX){
                nMaxColRightX = nColRightX;
            }
        }
    }


    public void positionControls(int _icurArrangement, Point _aStartPoint,Size _aFormSize, short _iAlign){
    try {
        this.setStartPoint(_aStartPoint);
        icurArrangement = _icurArrangement;
        initializePosSizes();
        initializeControlColumn(-1);
        bIsVeryFirstRun = true;
        nMaxRowY = 0;
        nSecMaxRowY = 0;
        this.nMaxColRightX = 0;
        xProgressBar.start("", FieldColumns.length);
        for (int i = 0; i < FieldColumns.length;  i++){
            insertLabel(i, _iAlign);
            insertDBControl(i);
            bIsVeryFirstRun = false;
            DBControlList[i].setPropertyValue("LabelControl", curLabelControl.xPropertySet);
            resetPosSizes(i);
            xProgressBar.setValue(i+1);
        }
        xProgressBar.end();
        bControlsareCreated = true;
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    public boolean areControlsexisting(){
        if (DBControlList != null)
            if (DBControlList.length > 0)
                return (DBControlList[0] != null);
        return false;
    }


    private void initializeControlColumn(int LastIndex){
        bIsFirstRun = true;
        StartA = LastIndex + 1;
        a = 0;
    }


    private void resetPosSizes(int LastIndex){
        int nYRefPos = nYDBPos;
        switch (icurArrangement){
            case FormWizard.SOCOLUMNARLEFT:
                nYDBPos = nYDBPos  + nDBHeight + cVertDistance + getCheckBoxDiffHeight(LastIndex);
                nYRefPos = nYDBPos;
                if ((nYDBPos > cYOffset + nFormHeight) || (LastIndex == (FieldColumns.length-1))){
                    repositionColumnarLeftControls(LastIndex);
                    nXTCPos = nMaxColRightX + 2 * cHoriDistance;
                    nXDBPos = nXTCPos + this.cLabelGap + nMaxTCWidth;
                    nYDBPos = cYOffset;
                    nYRefPos = nYDBPos;
                    initializeControlColumn(LastIndex);
                }
                else{
                    a = a + 1;
                }
                nYTCPos = nYDBPos + this.getLabelDiffHeight() + getCheckBoxDiffHeight(LastIndex);
                if ((nYRefPos + nDBHeight)  > nMaxDBYPos)
                    nMaxDBYPos = nYRefPos + nDBHeight;

                break;
             case FormWizard.SOCOLUMNARTOP:
                nYTCPos = nYDBPos + nDBHeight + cVertDistance + getCheckBoxDiffHeight(LastIndex);;
                if ((nYTCPos > cYOffset + nFormHeight) || (LastIndex == (FieldColumns.length-1))){
                    nXDBPos = nMaxColRightX + cHoriDistance;
                    nXTCPos = nXDBPos;
                    nYRefPos = nYDBPos;
                    nYDBPos = cYOffset + nTCHeight + cVertDistance;
                    nYTCPos = cYOffset;
                    initializeControlColumn(LastIndex);
                }
                else
                    a = a + 1;
                if ((nYRefPos + nDBHeight + cVertDistance) > nMaxDBYPos)
                    nMaxDBYPos = nYRefPos + nDBHeight + cVertDistance;
                break;

             case FormWizard.SOTOPJUSTIFIED:
                if (this.isReducable(a))
                    iReduceWidth = iReduceWidth + 1;
                if (nMaxColRightX > cXOffset + nFormWidth){
                    int nOldYTCPos = nYTCPos;
                    checkJustifiedPosition(a);
                    nYRefPos = nYDBPos;
                }
                else
                    nXTCPos = nMaxColRightX + cHoriDistance;
                a = a + 1;
                if ((nYRefPos + nDBHeight) > nMaxDBYPos)
                    nMaxDBYPos = nYRefPos + nDBHeight;
                break;
        }
//      if ((nYRefPos + nDBHeight) > nMaxDBYPos)
//          nMaxDBYPos = nYRefPos + nDBHeight;
    }



    private void repositionColumnarLeftControls(int LastIndex ){
        bIsFirstRun = true;
        for (int i = StartA; i <= LastIndex; i++){
            if (i == StartA){
                nXTCPos = LabelControlList[i].getPosition().X;
                nXDBPos = nXTCPos + nMaxTCWidth  + cHoriDistance;
            }
            LabelControlList[i].setSize(new Size(nMaxTCWidth, nTCHeight));
            resetDBShape(DBControlList[i], nXDBPos);
            checkOuterPoints(nXDBPos, nDBWidth, nYDBPos, nDBHeight, true);
        }
    }


    private void resetDBShape(Shape _curDBControl, int iXPos){
        int nYDBPos = _curDBControl.getPosition().Y;
        nDBWidth = _curDBControl.getSize().Width;
        nDBHeight = _curDBControl.getSize().Height;
        _curDBControl.setPosition(new Point(iXPos, nYDBPos));
    }


    private void initializePosSizes(){
        this.nMaxDBYPos = 0;
        nXTCPos = cXOffset;
        nTCWidth = 2000;
        nDBWidth = 2000;
        nDBHeight = oFormHandler.getDBRefHeight();
        nTCHeight = oFormHandler.getLabelHeight();
        iReduceWidth = 0;
        if (icurArrangement == FormWizard.SOCOLUMNARLEFT){
            nYTCPos = cYOffset + this.getLabelDiffHeight();
            nXDBPos = cXOffset + 3050;
            nYDBPos = cYOffset;
        }
        else{
            nXDBPos = cXOffset;
            nYTCPos = cYOffset;
        }
    }


    private void insertLabel(int i, int _iAlign){
    try {
        if (bControlsareCreated){
            LabelControlList[i].setPosition(new Point(nXTCPos, nYTCPos));
            if (icurArrangement != FormWizard.SOCOLUMNARLEFT){
                nTCWidth = LabelControlList[i].getPreferredWidth(FieldColumns[i].FieldTitle);
                LabelControlList[i].setSize(new Size(nTCWidth, nTCHeight));
            }
            else
                nTCWidth = LabelControlList[i].getSize().Width;
        }
        else{
            Point aPoint = new Point(nXTCPos, nYTCPos);
            Size aSize = new Size(nTCWidth,nTCHeight);
            this.LabelControlList[i] = new Control(oFormHandler, xFormName, FormHandler.SOLABEL, FieldColumns[i].FieldName, aPoint, aSize);
            if (bIsVeryFirstRun){
                if (icurArrangement == FormWizard.SOCOLUMNARTOP)
                    nYDBPos = nYTCPos + nTCHeight;
            }
            nTCWidth = LabelControlList[i].getPreferredWidth(FieldColumns[i].FieldTitle);
        }
        curLabelControl = LabelControlList[i];
        if (icurArrangement == FormWizard.SOCOLUMNARLEFT){
            // Note This If Sequence must be called before retrieving the outer Points
            if (bIsFirstRun){
                nMaxTCWidth = nTCWidth;
                bIsFirstRun = false;
            }
            else if (nTCWidth > nMaxTCWidth){
                nMaxTCWidth = nTCWidth;
            }
        }
        checkOuterPoints(nXTCPos, nTCWidth, nYTCPos, nTCHeight, false);
        if ((icurArrangement == FormWizard.SOCOLUMNARTOP) || (icurArrangement == FormWizard.SOTOPJUSTIFIED)){
            nXDBPos = nXTCPos;
            nYDBPos = nYTCPos + nTCHeight;
            curLabelControl.xPropertySet.setPropertyValue("Align", new Short((short)com.sun.star.awt.TextAlign.LEFT));
        }
        else
            curLabelControl.xPropertySet.setPropertyValue("Align", new Short((short)_iAlign));
        if (!bControlsareCreated)
            curLabelControl.setSize(new Size(nTCWidth,nTCHeight));
//      if (CurHelpText != ""){
//          oModel.HelpText = CurHelptext;
//      }

    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    private void insertDBControl(int i ){
    try {
        Point aPoint = new Point(nXDBPos, nYDBPos);
        if (bControlsareCreated)
            DBControlList[i].setPosition(aPoint);
        else{
            if (FieldColumns[i].FieldType == DataType.TIMESTAMP)
                DBControlList[i] = new TimeStampControl(new Resource(xMSF, "FormWizard", "dbw"), oFormHandler, xFormName, FieldColumns[i].FieldName, aPoint);
            else{
                DBControlList[i] = new DatabaseControl(oFormHandler, xFormName, FieldColumns[i].FieldName, FieldColumns[i].FieldType, aPoint);
                if (DBControlList[i].getControlType() == FormHandler.SOCHECKBOX)
                    DBControlList[i].setPropertyValue("Label", "");
            }
        }
        this.curDBControl = DBControlList[i];
        nDBHeight = curDBControl.getDBHeight();
        nDBWidth = curDBControl.getPreferredWidth();
        if (FieldColumns[i].FieldType != DataType.TIMESTAMP)
            curDBControl.setSize(new Size(nDBWidth, nDBHeight));
        if (curDBControl.getControlType() == FormHandler.SOCHECKBOX){
            nYDBPos = nYDBPos +(int)((oFormHandler.getDBRefHeight() - nDBHeight)/2);
            aPoint = new Point(nXDBPos, nYDBPos);
            curDBControl.setPosition(aPoint);
        }
        checkOuterPoints(nXDBPos, nDBWidth, nYDBPos, nDBHeight, true);
    } catch (Exception e) {
        e.printStackTrace(System.out);
    }}


    private int assignFieldLength(int _fieldlength){
        if (_fieldlength >= 65535)
            return  -1;
        else
            return _fieldlength;
    }



    public int getFormHeight(){
        return nMaxDBYPos - this.cYOffset;
    }


    public int getEntryPointY(){
        if (this.icurArrangement == FormWizard.SOCOLUMNARTOP){
            Control curLabelControl = LabelControlList[0];
            return curLabelControl.getPosition().Y;
        }
        else{
            DatabaseControl curDBControl = DBControlList[0];
            return curDBControl.getPosition().Y;
        }
    }


    public void setStartPoint(Point _aPoint){
        this.cXOffset = _aPoint.X;
        this.cYOffset = _aPoint.Y;
    }


    public void adjustYPositions(int _diffY){
        for (int i = 0; i < DBControlList.length; i++){
            Point aPoint = DBControlList[i].getPosition();
            DBControlList[i].setPosition(new Point(aPoint.X, aPoint.Y - _diffY));
            aPoint = this.LabelControlList[i].getPosition();
            LabelControlList[i].setPosition(new Point(aPoint.X, aPoint.Y - _diffY));
        }
        nMaxDBYPos =- _diffY;
        cYOffset =- _diffY;
    }


    public void setFormSize(Size _FormSize){
        nFormHeight = _FormSize.Height;
        nFormWidth = _FormSize.Width;
    }
}
