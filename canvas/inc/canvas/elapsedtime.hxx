/*************************************************************************
 *
 *  $RCSfile: elapsedtime.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: thb $ $Date: 2004-03-18 10:38:25 $
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

#ifndef _CANVAS_ELAPSEDTIME_HXX
#define _CANVAS_ELAPSEDTIME_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

namespace canvas
{
    namespace tools
    {
        /** Calculate elapsed time
         */
        class ElapsedTime
        {
        public:
            /** Create a new ElapsedTime object

                The moment of construction starts the time
                measurement. That means, a subsequent getElapsedTime()
                call will return the time difference between object
                creation and getElapsedTime() call.
             */
            ElapsedTime();

            /** Reset the time

                The instance of the reset() call starts the time
                measurement from scratch. That means, a subsequent
                getElapsedTime() call will return the time difference
                between reset() and getElapsedTime() call.
             */
            void reset();

            /** Query the elapsed time

                This method returns the elapsed time in seconds
                between either the construction of this object, or the
                last reset() call, if any.

                @return the elapsed time in seconds.
             */
            double getElapsedTime();

        private:
            sal_uInt64  mnStartTime;    // implementation-dependent start time representation
            double      mfTimeFactor;   // correction factor to get the time in seconds from mnStartTime
        };

    }
}

#endif /* _CANVAS_ELAPSEDTIME_HXX */
