/*************************************************************************
 *
 *  $RCSfile: xformsimport.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2004-11-16 10:04:05 $
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
#ifndef _XMLOFF_XFORMSIMPORT_HXX
#define _XMLOFF_XFORMSIMPORT_HXX

#include <tools/solar.h> // for USHORT
#include <com/sun/star/uno/Reference.hxx>

class SvXMLImport;
class SvXMLImportContext;
namespace rtl { class OUString; }
namespace std { template<typename A, typename B> struct pair; }
namespace com { namespace sun { namespace star {
    namespace uno { template<typename T> class Reference; }
    namespace beans { class XPropertySet; }
    namespace frame { class XModel; }
} } }

/** create import context for xforms:model element. */
SvXMLImportContext* createXFormsModelContext(
    SvXMLImport& rImport,
    USHORT nPrefix,
    const rtl::OUString& rLocalName );

/** perform the actual binding of an XForms-binding with the suitable control
 *  @param document which contains the XForms-model(s)
 *  @param pair<XForms binding ID, reference to control>
 */
void bindXFormsValueBinding(
    com::sun::star::uno::Reference<com::sun::star::frame::XModel>,
    std::pair<com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet>,rtl::OUString> );


/** perform the actual binding of an XForms-binding as list source with a list control
 *  @param document which contains the XForms-model(s)
 *  @param pair<XForms binding ID, reference to control>
 */
void bindXFormsListBinding(
    com::sun::star::uno::Reference<com::sun::star::frame::XModel>,
    std::pair<com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet>,rtl::OUString> );

/** perform the actual binding of an XForms submission with the suitable control
 *  @param document which contains the XForms-model(s)
 *  @param pair<XForms submission ID, reference to control>
 */
void bindXFormsSubmission(
    com::sun::star::uno::Reference<com::sun::star::frame::XModel>,
    std::pair<com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet>,rtl::OUString> );


#endif
