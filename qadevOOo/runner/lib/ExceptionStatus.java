/*************************************************************************
 *
 *  $RCSfile: ExceptionStatus.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change:$Date: 2003-01-27 16:27:43 $
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

package lib;

/**
 * The class implements Status behaviour for exception runstate Status objects.
 */
class ExceptionStatus extends Status {
    /**
     * Contains the exception which caused the abnormal activty termination.
     */
    private final Throwable exception;

    /**
     * Creates an instance of Status object with EXCEPTION runstate.
     *
     * @param t the exception an activity terminated with.
     */
    ExceptionStatus( Throwable t ) {
        exception = t;
    }

    /**
     * @return state of the Status: FAILED.
     */
    public int getState() {
        return FAILED;
    }

    /**
     * @return the run state of the Status: EXCEPTION.
     */
    public int getRunState() {
        return EXCEPTION;
    }

    /**
     * @return a string describing run state of the Status.
     */
    public String getRunStateString() {
        return exception.toString();
    }

    /**
     * Compares this Status with obj.
     *
     * @return <tt>true</tt> if obj is an ExceptionStatus instance and has
     * a similar exception.
     */
    public boolean equals(Object obj) {
        if (obj == null || !(obj instanceof ExceptionStatus)) {
            return false;
        }

        ExceptionStatus other = (ExceptionStatus)obj;

        if (this.exception.getClass() != other.exception.getClass()) {
            return false;
        }

        String thisMess = this.exception.getMessage();
        String otherMess = other.exception.getMessage();

        return (thisMess == otherMess)
                || (thisMess != null && thisMess.equals(otherMess));
    }
}