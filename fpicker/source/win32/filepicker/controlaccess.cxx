/*************************************************************************
 *
 *  $RCSfile: controlaccess.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: tra $ $Date: 2002-03-21 07:40:13 $
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

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif

#ifndef _CONTROLACCESS_HXX_
#include "controlaccess.hxx"
#endif

#ifndef _WINIMPLHELPER_HXX_
#include "..\misc\WinImplHelper.hxx"
#endif

#include <systools/win32/user9x.h>

//------------------------------------------------------------
// we are using a table based algorithm to dispatch control
// actions there is one table containing one action table for
// each control class and one action table per control class
// which contains function pointer to control action functions
//------------------------------------------------------------

//------------------------------------------------------------
// namespace directives
//------------------------------------------------------------

using rtl::OUString;

namespace // private
{

    //------------------------------------------------------------
    // table setup
    //------------------------------------------------------------

    CTRL_SETVALUE_FUNCTION_T CheckboxSetValueFunctionTable[] =
    {
        CheckboxSetState
    };
    const size_t SIZE_CHECKBOX_SETVALUE_FUNCTION_TABLE =
        sizeof( CheckboxSetValueFunctionTable ) / sizeof( CTRL_SETVALUE_FUNCTION_T );

    CTRL_GETVALUE_FUNCTION_T CheckboxGetValueFunctionTable[] =
    {
        CheckboxGetState
    };
    const size_t SIZE_CHECKBOX_GETVALUE_FUNCTION_TABLE =
        sizeof( CheckboxGetValueFunctionTable ) / sizeof( CTRL_GETVALUE_FUNCTION_T );

    CTRL_SETVALUE_FUNCTION_T ListboxSetValueFunctionTable[] =
    {
        NULL,
        ListboxAddItem,
        ListboxAddItems,
        ListboxDeleteItem,
        ListboxDeleteItems,
        ListboxSetSelectedItem
    };
    const size_t SIZE_LISTBOX_SETVALUE_FUNCTION_TABLE =
        sizeof( ListboxSetValueFunctionTable ) / sizeof( CTRL_SETVALUE_FUNCTION_T );

    CTRL_GETVALUE_FUNCTION_T ListboxGetValueFunctionTable[] =
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        ListboxGetItems,
        ListboxGetSelectedItem,
        ListboxGetSelectedItemIndex
    };
    const size_t SIZE_LISTBOX_GETVALUE_ACTION_TABLE =
        sizeof( ListboxGetValueFunctionTable ) / sizeof( CTRL_GETVALUE_FUNCTION_T );

    struct _ENTRY
    {
        LPVOID lpFunctionTable;
        size_t TableSize;
    };

    // an array of function tables, one for each control class
    _ENTRY CtrlClassSetValueFunctionTable[] =
    {
        { NULL, 0 },
        { CheckboxSetValueFunctionTable, SIZE_CHECKBOX_SETVALUE_FUNCTION_TABLE },
        { ListboxSetValueFunctionTable, SIZE_LISTBOX_SETVALUE_FUNCTION_TABLE },
        { NULL, 0 }
    };

    // an array of function tables, one for each control class
    _ENTRY CtrlClassGetValueFunctionTable[] =
    {
        { NULL, 0 },
        { CheckboxGetValueFunctionTable, SIZE_CHECKBOX_GETVALUE_FUNCTION_TABLE },
        { ListboxGetValueFunctionTable, SIZE_LISTBOX_GETVALUE_ACTION_TABLE },
        { NULL, 0 }
    };

    //------------------------------------------------------------
    //
    //------------------------------------------------------------

    CTRL_SETVALUE_FUNCTION_T SAL_CALL GetCtrlSetValueFunction(
        CTRL_SETVALUE_FUNCTION_T* aCtrlSetValueFunctionTable, size_t aTableSize, sal_Int16 aCtrlAction )
    {
        if ( !aCtrlSetValueFunctionTable ||
            aCtrlAction < 0 || aCtrlAction >= aTableSize )
            return NULL;

        return aCtrlSetValueFunctionTable[aCtrlAction];
    }

    //------------------------------------------------------------
    //
    //------------------------------------------------------------

    CTRL_GETVALUE_FUNCTION_T SAL_CALL GetCtrlGetValueFunction(
        CTRL_GETVALUE_FUNCTION_T* aCtrlGetValueFunctionTable, size_t aTableSize, sal_Int16 aCtrlAction )
    {
        if ( !aCtrlGetValueFunctionTable ||
            aCtrlAction < 0 || aCtrlAction >= aTableSize )
            return NULL;

        return aCtrlGetValueFunctionTable[aCtrlAction];
    }

    //------------------------------------------------------------
    //
    //------------------------------------------------------------

    inline
    _ENTRY SAL_CALL GetCtrlClassSetValueFunctionTable( CTRL_CLASS aCtrlClass )
    {
        return CtrlClassSetValueFunctionTable[aCtrlClass];
    }

    //------------------------------------------------------------
    //
    //------------------------------------------------------------

    inline
    _ENTRY SAL_CALL GetCtrlClassGetValueFunctionTable( CTRL_CLASS aCtrlClass )
    {
        return CtrlClassGetValueFunctionTable[aCtrlClass];
    }

    int WindowsFileOpenCtrlIds[] =
    {
        0,
        IDOK,
        IDCANCEL,
        cmb1
    };
    const SIZE_WINDOWS_FILEOPEN_CTRL_IDS =
        sizeof( WindowsFileOpenCtrlIds ) / sizeof( int );

}; // end namespace

//------------------------------------------------------------
//
//------------------------------------------------------------

CTRL_SETVALUE_FUNCTION_T SAL_CALL GetCtrlSetValueFunction( CTRL_CLASS aCtrlClass, sal_Int16 aCtrlAction )
{
    _ENTRY aEntry =
        GetCtrlClassSetValueFunctionTable( aCtrlClass );

    return GetCtrlSetValueFunction(
        reinterpret_cast< CTRL_SETVALUE_FUNCTION_T* >( aEntry.lpFunctionTable ),
        aEntry.TableSize,
        aCtrlAction );
}

//------------------------------------------------------------
//
//------------------------------------------------------------

CTRL_GETVALUE_FUNCTION_T SAL_CALL GetCtrlGetValueFunction( CTRL_CLASS aCtrlClass, sal_Int16 aCtrlAction )
{
    _ENTRY aEntry =
        GetCtrlClassGetValueFunctionTable( aCtrlClass );

    return GetCtrlGetValueFunction(
        reinterpret_cast< CTRL_GETVALUE_FUNCTION_T* >( aEntry.lpFunctionTable ),
        aEntry.TableSize,
        aCtrlAction );
}

//------------------------------------------------------------
//
//------------------------------------------------------------

CTRL_CLASS SAL_CALL GetCtrlClass( HWND hwndCtrl )
{
    CTRL_CLASS aCtrlClass = UNKNOWN;
    char aClassName[256];

    int nRet = GetClassNameA(hwndCtrl,aClassName,sizeof(aClassName));
    if (nRet)
    {
        if (0 == _stricmp(aClassName,"button"))
        {
            // button means many things so we have
            // to find out what button it is
            LONG lBtnStyle = GetWindowLong(hwndCtrl,GWL_STYLE);
            if (lBtnStyle & BS_CHECKBOX)
                aCtrlClass = CHECKBOX;
            else if (((lBtnStyle & BS_PUSHBUTTON) == 0) || (lBtnStyle & BS_DEFPUSHBUTTON))
                aCtrlClass = PUSHBUTTON;
        }
        else if (0 == _stricmp(aClassName,"listbox") ||
                  0 == _stricmp(aClassName,"combobox"))
            aCtrlClass = LISTBOX;
    }

    return aCtrlClass;
}

//------------------------------------------------------------
//
//------------------------------------------------------------

int SAL_CALL CommonFilePickerCtrlIdToWinFileOpenCtrlId( sal_Int16 aControlId )
{
    if ( aControlId < 0 || aControlId > SIZE_WINDOWS_FILEOPEN_CTRL_IDS )
        return aControlId;

    return WindowsFileOpenCtrlIds[aControlId];
}
