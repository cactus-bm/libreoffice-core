/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: BlindtextCreator.java,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2006-04-07 12:34:54 $
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
package com.sun.star.wizards.db;

import com.sun.star.wizards.common.JavaTools;

/**
 *
 * @author  bc93774
 */
public class BlindtextCreator {

    public static final String BlindText =
        "Ut wisi enim ad minim veniam, quis nostrud exerci tation "
            + "ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor "
            + "in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at "
            + "vero et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore "
            + "te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy "
            + "nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, "
            + "quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. "
            + "Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum "
            + "dolore eu feugiat nulla facilisis at vero et accumsan et iusto odio dignissim qui blandit praesent "
            + "luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Nam liber tempor cum soluta nobis "
            + "eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum.";

    public static String adjustBlindTextlength(String FieldTitle, int FieldWidth, boolean bIsCurLandscape, boolean bIsGroupTable, String[] RecordFieldNames) {
        String BlindTextString = "";
        if (bIsGroupTable)
            return getBlindTextString(FieldTitle, FieldWidth, FieldWidth);
        int MaxFieldCount = getMaxFieldCount(bIsCurLandscape);
        if (RecordFieldNames.length <= 2 * MaxFieldCount) {
            if (RecordFieldNames.length <= MaxFieldCount)
                BlindTextString = getBlindTextString(FieldTitle, FieldWidth, FieldWidth);
            else
                BlindTextString = getBlindTextString(FieldTitle, FieldWidth, (int) (0.5 * FieldWidth));
        } else
            BlindTextString = getBlindTextString(FieldTitle, FieldWidth, (int) 1.1 * FieldTitle.length());
        return BlindTextString;
    }

    public static String getBlindTextString(String FieldTitle, int FieldWidth, int MaxWidth) {
        String[] BlindTextArray = JavaTools.ArrayoutofString(BlindText, " ");
        String PartBlindText = BlindTextArray[0];
        String NewPartBlindText;
        int MaxHeaderWidth;
        int iWidth = FieldWidth;
        int Titlelength = (int) 1.1 * FieldTitle.length(); // We assume that the TableHeading is bold

        if (Titlelength > PartBlindText.length())
            MaxHeaderWidth = Titlelength;
        else
            MaxHeaderWidth = PartBlindText.length();
        if (MaxHeaderWidth > MaxWidth)
            MaxWidth = MaxHeaderWidth;

        int i = 1;
        do {
            NewPartBlindText = PartBlindText + " " + BlindTextArray[i];
            if (NewPartBlindText.length() < MaxWidth) {
                PartBlindText = NewPartBlindText;
                i += 1;
            }
        } while (NewPartBlindText.length() < MaxWidth);
        return PartBlindText;
    }

    private static int getMaxFieldCount(boolean bIsCurLandscape) {
        if (bIsCurLandscape == true)
            return 5;
        else
            return 3;
    }

}
