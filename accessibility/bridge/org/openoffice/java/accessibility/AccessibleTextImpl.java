/*************************************************************************
 *
 *  $RCSfile: AccessibleTextImpl.java,v $
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
package org.openoffice.java.accessibility;

import com.sun.star.accessibility.*;
import com.sun.star.awt.*;
import com.sun.star.style.*;
import com.sun.star.uno.*;

import org.openoffice.java.accessibility.logging.*;

import javax.accessibility.AccessibleContext;
import javax.accessibility.AccessibleText;

import javax.swing.text.StyleConstants;


/** The GenericAccessibleEditableText mapps the calls to the java AccessibleEditableText
 *  interface to the corresponding methods of the UNO XAccessibleEditableText interface.
 */
public class AccessibleTextImpl implements javax.accessibility.AccessibleText {
    final static double toPointFactor = 1 / ((7 / 10) + 34.5);
    static String[] attributeList = {
        "ParaAdjust", "CharBackColor", "CharWeight", "ParaFirstLineIndent",
        "CharFontPitch", "CharHeight", "CharColor", "CharPosture",
        "ParaLeftMargin", "ParaLineSpacing", "ParaTopMargin", "ParaBottomMargin",
        "CharStrikeout", "CharEscapement", "ParaTabStops", "CharUnderline"
    };
    XAccessibleText unoObject;
    private javax.swing.text.TabSet tabSet = null;
    private javax.swing.text.TabStop[] tabStops = null;
    private static Type TextSegmentType = new Type(TextSegment.class);

    /** Creates new GenericAccessibleEditableText object */
    public AccessibleTextImpl(XAccessibleText xAccessibleText) {

        if (Build.PRODUCT) {
            unoObject = xAccessibleText;
        } else {
            String property = System.getProperty("AccessBridgeLogging");
            if ((property != null) && (property.indexOf("text") != -1)) {
                unoObject = new XAccessibleTextLog(xAccessibleText);
            } else {
                unoObject = xAccessibleText;
            }
        }
    }

    public static javax.accessibility.AccessibleText get(com.sun.star.uno.XInterface unoObject) {
        try {
            XAccessibleText unoAccessibleText = (XAccessibleText)
                UnoRuntime.queryInterface(XAccessibleText.class, unoObject);
            if (unoAccessibleText != null) {
                return new AccessibleTextImpl(unoAccessibleText);
            }
        } catch (com.sun.star.uno.RuntimeException e) {
        }
        return null;
    }

    protected static short toTextType(int part) {
        short type = 0;

        switch (part) {
            case AccessibleText.CHARACTER:
                type = AccessibleTextType.CHARACTER;

                break;

            case AccessibleText.WORD:
                type = AccessibleTextType.WORD;

                break;

            case AccessibleText.SENTENCE:
                type = AccessibleTextType.SENTENCE;

                break;

            case 4:
                type = AccessibleTextType.LINE;

                break;

            case 5:
                type = AccessibleTextType.ATTRIBUTE_RUN;

                break;

            default:
                break;
        }

        return type;
    }

    protected static Object convertTextSegment(Object any) {
        try {
            if (AnyConverter.isObject(any)) {
                TextSegment ts = (TextSegment)
                    AnyConverter.toObject(TextSegmentType, any);
                if (ts != null) {
                    // Since there is nothing like a "range" object in the JAA yet,
                    // the Object[3] is a private negotiation with the JABG
                    Object[] array = { new Integer(ts.SegmentStart),
                        new Integer(ts.SegmentEnd), ts.SegmentText };
                    return array;
                }
            }
        } catch (com.sun.star.lang.IllegalArgumentException e) {
        }

        return null;
    }

    /** Returns the string after a given index */
    public String getAfterIndex(int part, int index) {
        short type = toTextType(part);

        try {
            TextSegment ts = unoObject.getTextBehindIndex(index, type);

            // the office returns an empty string when asking for the word at
            // the position of a blank
            if ((part == AccessibleText.WORD) &&
                    (ts.SegmentText.length() == 0)) {
                // FIXME: should potentially return more than one blank, but
                // since this will be fixed in the office at some time ..
                return " ";
            }

            return ts.SegmentText;
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            // Workaround for #104847#
            if ((type == AccessibleTextType.LINE) && (index > 0) && (getCharCount() == index)) {
                return getAfterIndex(part, index - 1);
            }
        } catch (com.sun.star.lang.IllegalArgumentException e) {
        }

        return null;
    }

    /** Returns the zero-based offset of the caret */
    public int getCaretPosition() {
        try {
            return unoObject.getCaretPosition();
        } catch (com.sun.star.uno.RuntimeException e) {
            return -1;
        }
    }

    /** Returns the start offset within the selected text */
    public int getSelectionStart() {
        try {
            int index = unoObject.getSelectionStart();

            if (index == -1) {
                index = getCaretPosition();
            }

            return index;
        } catch (com.sun.star.uno.RuntimeException e) {
            return -1;
        }
    }

    protected void setAttribute(javax.swing.text.MutableAttributeSet as,
        com.sun.star.beans.PropertyValue property) {
        try {
            // Map alignment attribute
            if (property.Name.equals("ParaAdjust")) {
                ParagraphAdjust adjust = null;

                if (property.Value instanceof ParagraphAdjust) {
                    adjust = (ParagraphAdjust) property.Value;
                } else if (property.Value instanceof Any) {
                    adjust = (ParagraphAdjust) AnyConverter.toObject(new Type(
                                ParagraphAdjust.class), property.Value);
                } else {
                    adjust = ParagraphAdjust.fromInt(AnyConverter.toInt(
                                property.Value));
                }

                if (adjust != null) {
                    if (adjust.equals(ParagraphAdjust.LEFT)) {
                        StyleConstants.setAlignment(as,
                            StyleConstants.ALIGN_LEFT);
                    } else if (adjust.equals(ParagraphAdjust.RIGHT)) {
                        StyleConstants.setAlignment(as,
                            StyleConstants.ALIGN_RIGHT);
                    } else if (adjust.equals(ParagraphAdjust.CENTER)) {
                        StyleConstants.setAlignment(as,
                            StyleConstants.ALIGN_CENTER);
                    } else if (adjust.equals(ParagraphAdjust.BLOCK) ||
                            adjust.equals(ParagraphAdjust.STRETCH)) {
                        StyleConstants.setAlignment(as,
                            StyleConstants.ALIGN_JUSTIFIED);
                    }
                } else if (Build.DEBUG) {
                    System.err.println(
                        "Invalid property value for key ParaAdjust: " +
                        property.Value.getClass().getName());
                }

                // Map background color
            } else if (property.Name.equals("CharBackColor")) {
                StyleConstants.setBackground(as,
                    new java.awt.Color(AnyConverter.toInt(property.Value)));

                // FIXME: BidiLevel
                // Set bold attribute
            } else if (property.Name.equals("CharWeight")) {
                boolean isBold = AnyConverter.toFloat(property.Value) > 125;
                StyleConstants.setBold(as, isBold);

                // FIXME: Java 1.4 ComponentAttribute, ComponentElementName, ComposedTextAttribute
                // Set FirstLineIndent attribute
            } else if (property.Name.equals("ParaFirstLineIndent")) {
                StyleConstants.setFirstLineIndent(as,
                    (float) (toPointFactor * AnyConverter.toInt(property.Value)));

                // Set font family attribute
            } else if (property.Name.equals("CharFontPitch")) {
                if (AnyConverter.toShort(property.Value) == 2) {
                    StyleConstants.setFontFamily(as, "Proportional");
                }

                // Set font size attribute
            } else if (property.Name.equals("CharHeight")) {
                StyleConstants.setFontSize(as,
                    (int) AnyConverter.toFloat(property.Value));

                // Map foreground color
            } else if (property.Name.equals("CharColor")) {
                StyleConstants.setForeground(as,
                    new java.awt.Color(AnyConverter.toInt(property.Value)));

                // FIXME: IconAttribute, IconElementName
                // Set italic attribute
            } else if (property.Name.equals("CharPosture")) {
                FontSlant fs = null;

                if (property.Value instanceof FontSlant) {
                    fs = (FontSlant) property.Value;
                } else if (property.Value instanceof Any) {
                    fs = (FontSlant) AnyConverter.toObject(new Type(
                                FontSlant.class), property.Value);
                }

                if (fs != null) {
                    StyleConstants.setItalic(as, FontSlant.ITALIC.equals(fs));
                }

                // Set left indent attribute
            } else if (property.Name.equals("ParaLeftMargin")) {
                StyleConstants.setLeftIndent(as,
                    (float) (toPointFactor * AnyConverter.toInt(property.Value)));

                // Set right indent attribute
            } else if (property.Name.equals("ParaRightMargin")) {
                StyleConstants.setRightIndent(as,
                    (float) (toPointFactor * AnyConverter.toInt(property.Value)));
            }
            // Set line spacing attribute
            else if (property.Name.equals("ParaLineSpacing")) {
                LineSpacing ls = null;

                if (property.Value instanceof LineSpacing) {
                    ls = (LineSpacing) property.Value;
                } else if (property.Value instanceof Any) {
                    ls = (LineSpacing) AnyConverter.toObject(new Type(
                                LineSpacing.class), property.Value);
                }

                if (ls != null) {
                    StyleConstants.setLineSpacing(as,
                        (float) (toPointFactor * ls.Height));
                }
            }
            // FIXME: Java 1.4 NameAttribute, Orientation, ResolveAttribute
            // Set space above attribute
            else if (property.Name.equals("ParaTopMargin")) {
                StyleConstants.setSpaceAbove(as,
                    (float) (toPointFactor * AnyConverter.toInt(property.Value)));
            }
            // Set space below attribute
            else if (property.Name.equals("ParaBottomMargin")) {
                StyleConstants.setSpaceBelow(as,
                    (float) (toPointFactor * AnyConverter.toInt(property.Value)));

                // Set strike through attribute
            } else if (property.Name.equals("CharStrikeout")) {
                boolean isStrikeThrough = (FontStrikeout.NONE != AnyConverter.toShort(property.Value));
                StyleConstants.setStrikeThrough(as, isStrikeThrough);

                // Set sub-/superscript attribute
            } else if (property.Name.equals("CharEscapement")) {
                short value = AnyConverter.toShort(property.Value);

                if (value > 0) {
                    StyleConstants.setSuperscript(as, true);
                } else if (value < 0) {
                    StyleConstants.setSubscript(as, true);
                }

                // Set tabset attribute
            } else if (property.Name.equals("ParaTabStops")) {
                TabStop[] unoTabStops = (TabStop[]) AnyConverter.toArray(property.Value);
                javax.swing.text.TabStop[] tabStops = new javax.swing.text.TabStop[unoTabStops.length];

                for (int index2 = 0; index2 < unoTabStops.length; index2++) {
                    float pos = (float) (toPointFactor * unoTabStops[index2].Position);

                    if (unoTabStops[index2].Alignment.equals(TabAlign.LEFT)) {
                        tabStops[index2] = new javax.swing.text.TabStop(pos,
                                javax.swing.text.TabStop.ALIGN_LEFT,
                                javax.swing.text.TabStop.LEAD_NONE);
                    } else if (unoTabStops[index2].Alignment.equals(
                                TabAlign.CENTER)) {
                        tabStops[index2] = new javax.swing.text.TabStop(pos,
                                javax.swing.text.TabStop.ALIGN_CENTER,
                                javax.swing.text.TabStop.LEAD_NONE);
                    } else if (unoTabStops[index2].Alignment.equals(
                                TabAlign.RIGHT)) {
                        tabStops[index2] = new javax.swing.text.TabStop(pos,
                                javax.swing.text.TabStop.ALIGN_RIGHT,
                                javax.swing.text.TabStop.LEAD_NONE);
                    } else if (unoTabStops[index2].Alignment.equals(
                                TabAlign.DECIMAL)) {
                        tabStops[index2] = new javax.swing.text.TabStop(pos,
                                javax.swing.text.TabStop.ALIGN_DECIMAL,
                                javax.swing.text.TabStop.LEAD_NONE);
                    } else {
                        tabStops[index2] = new javax.swing.text.TabStop(pos);
                    }
                }

                // Re-use tabSet object if possible to make AttributeSet.equals work
                if ((this.tabSet == null) ||
                        !java.util.Arrays.equals(tabStops, this.tabStops)) {
                    this.tabStops = tabStops;
                    this.tabSet = new javax.swing.text.TabSet(tabStops);
                }

                StyleConstants.setTabSet(as, this.tabSet);

                // Set underline attribute
            } else if (property.Name.equals("CharUnderline")) {
                boolean isUnderline = (FontUnderline.NONE != AnyConverter.toShort(property.Value));
                StyleConstants.setUnderline(as, isUnderline);
            }
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            if (Build.DEBUG) {
                System.err.println("*** ERROR *** " + e.getClass().getName() +
                    " caught for property " + property.Name + ": " +
                    e.getMessage());
                System.err.println("              value is of type " +
                    property.Value.getClass().getName());
            }
        }
    }

    /** Returns the AttributSet for a given character at a given index */
    public javax.swing.text.AttributeSet getCharacterAttribute(int index) {
        try {
            com.sun.star.beans.PropertyValue[] propertyValues = unoObject.getCharacterAttributes(index,
                    attributeList);
            javax.swing.text.SimpleAttributeSet attributeSet = new javax.swing.text.SimpleAttributeSet();

            if (null != propertyValues) {
                for (int i = 0; i < propertyValues.length; i++) {
                    setAttribute(attributeSet, propertyValues[i]);
                }
            }

            return attributeSet;
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            if ((index > 0) && (getCharCount() == index)) {
                return getCharacterAttribute(index - 1);
            }
            return null;
        }
    }

    /** Given a point in local coordinates, return the zero-based index of the character under that point */
    public int getIndexAtPoint(java.awt.Point point) {
        try {
            return unoObject.getIndexAtPoint(new Point(point.x, point.y));
        } catch (com.sun.star.uno.RuntimeException e) {
            return -1;
        }
    }

    /** Returns the end offset within the selected text */
    public int getSelectionEnd() {
        try {
            int index = unoObject.getSelectionEnd();

            if (index == -1) {
                index = getCaretPosition();
            }

            return index;
        } catch (com.sun.star.uno.RuntimeException e) {
            return -1;
        }
    }

    /** Returns the string before a given index */
    public java.lang.String getBeforeIndex(int part, int index) {
        short type = toTextType(part);

        try {
            // the office returns an empty string when asking for the word at
            // the position of a blank
            TextSegment ts = unoObject.getTextBeforeIndex(index, type);

            if ((part == AccessibleText.WORD) &&
                    (ts.SegmentText.length() == 0)) {
                // FIXME: should potentially return more than one blank, but
                // since this will be fixed in the office at some time ..
                return " ";
            }

            return ts.SegmentText;
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            // Workaround for #104847#
            if ((type == AccessibleTextType.LINE) && (index > 0) && (getCharCount() == index)) {
                return getBeforeIndex(part, index - 1);
            }
        } catch (com.sun.star.lang.IllegalArgumentException e) {
        }

        return null;
    }

    /** Returns the string at a given index */
    public java.lang.String getAtIndex(int part, int index) {
        short type = toTextType(part);

        try {
            // the office returns an empty string when asking for the word at
            // the position of a blank
            TextSegment ts = unoObject.getTextAtIndex(index, type);

            if ((part == AccessibleText.WORD) &&
                    (ts.SegmentText.length() == 0)) {
                // FIXME: should potentially return more than one blank, but
                // since this will be fixed in the office at some time ..
                return " ";
            }

            return ts.SegmentText;
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            // Workaround for #104847#
            if ((type == AccessibleTextType.LINE) && (index > 0) && (getCharCount() == index)) {
                return getAtIndex(part, index - 1);
            }
        } catch (com.sun.star.lang.IllegalArgumentException e) {
        }

        return null;
    }

    /** Returns the number of characters (valid indicies) */
    public int getCharCount() {
        try {
            return unoObject.getCharacterCount();
        } catch (com.sun.star.uno.RuntimeException e) {
        }

        return 0;
    }

    /** Returns the portion of the text that is selected */
    public java.lang.String getSelectedText() {
        try {
            return unoObject.getSelectedText();
        } catch (com.sun.star.uno.RuntimeException e) {
        }

        return null;
    }

    /** Determines the bounding box of the character at the given index into the string */
    public java.awt.Rectangle getCharacterBounds(int index) {
        try {
            Rectangle unoRect = unoObject.getCharacterBounds(index);
            return new java.awt.Rectangle(unoRect.X, unoRect.Y, unoRect.Width, unoRect.Height);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            if ((index > 0) && (getCharCount() == index)) {
                return getCharacterBounds(index - 1);
            }
        } catch (com.sun.star.uno.RuntimeException e) {
        }

        return new java.awt.Rectangle();
    }
}
