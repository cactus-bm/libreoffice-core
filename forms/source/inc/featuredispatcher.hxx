/*************************************************************************
 *
 *  $RCSfile: featuredispatcher.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-04-13 11:16:58 $
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

#ifndef FORMS_SOLAR_DISPATCHER_HXX
#define FORMS_SOLAR_DISPATCHER_HXX

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

//.........................................................................
namespace frm
{
//.........................................................................

    //=========================================================================
    //= IFeatureDispatcher
    //=========================================================================
    class IFeatureDispatcher
    {
    public:
        /** dispatches a feature

            @param _nFeatureId
                the id of the feature to dispatch
        */
        virtual void    dispatch( sal_Int32 _nFeatureId ) const = 0;

        /** dispatches a feature, with an additional named parameter

            @param _nFeatureId
                the id of the feature to dispatch

            @param _pParamAsciiName
                the Ascii name of the parameter of the dispatch call

            @param _rParamValue
                the value of the parameter of the dispatch call
        */
        virtual void dispatchWithArgument(
                sal_Int32 _nFeatureId,
                const sal_Char* _pParamName,
                const ::com::sun::star::uno::Any& _rParamValue
             ) const = 0;

        /** checks whether a given feature is enabled
        */
        virtual bool    isEnabled( sal_Int32 _nFeatureId ) const = 0;

        /** returns the boolean state of a feature

            Certain features may support more status information than only the enabled/disabled
            state. The type of such additional information is fixed relative to a given feature, but
            may differ between different features.

            This method allows retrieving status information about features which have an additional
            boolean information associated with it.
        */
        virtual bool    getBooleanState( sal_Int32 _nFeatureId ) const = 0;

        /** returns the string state of a feature

            Certain features may support more status information than only the enabled/disabled
            state. The type of such additional information is fixed relative to a given feature, but
            may differ between different features.

            This method allows retrieving status information about features which have an additional
            string information associated with it.
        */
        virtual ::rtl::OUString getStringState( sal_Int32 _nFeatureId ) const = 0;

        /** returns the integer state of a feature

            Certain features may support more status information than only the enabled/disabled
            state. The type of such additional information is fixed relative to a given feature, but
            may differ between different features.

            This method allows retrieving status information about features which have an additional
            integer information associated with it.
        */
        virtual sal_Int32       getIntegerState( sal_Int32 _nFeatureId ) const = 0;
    };

//.........................................................................
}   // namespace frm
//.........................................................................

#endif // FORMS_SOLAR_DISPATCHER_HXX
