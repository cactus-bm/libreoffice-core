/*************************************************************************
 *
 *  $RCSfile: PropertyName.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Date: 2004-11-02 11:23:52 $
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

package convwatch;

public interface PropertyName
{
    final public static String INPUT_PATH =                 "DOC_COMPARATOR_INPUT_PATH";
    final public static String OUTPUT_PATH =                "DOC_COMPARATOR_OUTPUT_PATH";
    final public static String DIFF_PATH =                  "DOC_COMPARATOR_DIFF_PATH";
    final public static String REFERENCE_PATH =             "DOC_COMPARATOR_REFERENCE_PATH";
    final public static String REFERENCE_INPUT_PATH =       "DOC_COMPARATOR_REFERENCE_INPUT_PATH";
    final public static String REFERENCE_TYPE =             "DOC_COMPARATOR_REFERENCE_CREATOR_TYPE";
    final public static String PRINTER_NAME =               "DOC_COMPARATOR_PRINTER_NAME";
    final public static String DEFAULT_XML_FORMAT_APP =     "DOC_COMPARATOR_DEFAULT_XML_FORMAT_APP";
    final public static String INCLUDE_SUBDIRS =            "DOC_COMPARATOR_INCLUDE_SUBDIRS";
    final public static String PRINT_MAX_PAGE =             "DOC_COMPARATOR_PRINT_MAX_PAGE";
    final public static String PRINT_ONLY_PAGE =            "DOC_COMPARATOR_PRINT_ONLY_PAGE";
    final public static String GFX_OUTPUT_DPI_RESOLUTION =  "DOC_COMPARATOR_GFX_OUTPUT_DPI_RESOLUTION";
    final public static String OVERWRITE_REFERENCE =        "DOC_COMPARATOR_OVERWRITE_REFERENCE";
}
