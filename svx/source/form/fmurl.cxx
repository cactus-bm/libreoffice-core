/*************************************************************************
 *
 *  $RCSfile: fmurl.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: hr $ $Date: 2004-04-13 10:59:53 $
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

#ifndef _SVX_FMURL_HXX
#include "fmurl.hxx"
#endif


namespace svxform
{

    IMPLEMENT_CONSTASCII_USTRING(FMURL_FORMSLOTS_PREFIX,   ".uno:FormController/");

    IMPLEMENT_CONSTASCII_USTRING(FMURL_FORM_POSITION,      ".uno:FormController/positionForm"  );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_FORM_RECORDCOUNT,   ".uno:FormController/RecordCount"  );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_MOVEFIRST,   ".uno:FormController/moveToFirst"  );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_MOVEPREV,    ".uno:FormController/moveToPrev"   );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_MOVENEXT,    ".uno:FormController/moveToNext"   );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_MOVELAST,    ".uno:FormController/moveToLast"   );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_MOVETONEW,   ".uno:FormController/moveToNew"    );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_UNDO,        ".uno:FormController/undoRecord"   );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_SAVE,        ".uno:FormController/saveRecord"   );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_RECORD_DELETE,      ".uno:FormController/deleteRecord" );
    IMPLEMENT_CONSTASCII_USTRING(FMURL_FORM_REFRESH,       ".uno:FormController/refreshForm"  );

    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_SORT_UP,      ".uno:FormController/sortUp"       );
    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_SORT_DOWN,    ".uno:FormController/sortDown"     );
    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_SORT,         ".uno:FormController/sort"         );
    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_AUTO_FILTER,  ".uno:FormController/autoFilter"   );
    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_FILTER,       ".uno:FormController/filter"       );
    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_APPLY_FILTER, ".uno:FormController/applyFilter"  );
    IMPLEMENT_CONSTASCII_USTRING( FMURL_FORM_REMOVE_FILTER,".uno:FormController/removeFilterOrder" );

    IMPLEMENT_CONSTASCII_USTRING(FMURL_CONFIRM_DELETION,".uno:FormSlots/ConfirmDeletion");

    IMPLEMENT_CONSTASCII_USTRING(FMURL_COMPONENT_FORMGRIDVIEW,".component:DB/FormGridView");
    IMPLEMENT_CONSTASCII_USTRING(FMURL_GRIDVIEW_CLEARVIEW,".uno:FormSlots/ClearView");
    IMPLEMENT_CONSTASCII_USTRING(FMURL_GRIDVIEW_ADDCOLUMN,".uno:FormSlots/AddGridColumn");
    IMPLEMENT_CONSTASCII_USTRING(FMURL_GRIDVIEW_ATTACHTOFORM,".uno:FormSlots/AttachToForm");

    IMPLEMENT_CONSTASCII_USTRING(FMARG_ATTACHTO_MASTERFORM,"MasterForm");
    IMPLEMENT_CONSTASCII_USTRING(FMARG_ADDCOL_COLUMNTYPE,"ColumnType");
    IMPLEMENT_CONSTASCII_USTRING(FMARG_ADDCOL_COLUMNPOS,"ColumnPosition");

}   // namespace svxform
