/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pfiltdlg.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:43:16 $
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

#ifndef SC_PFILTDLG_HXX
#define SC_PFILTDLG_HXX

#ifndef _SV_HXX
#endif

#ifndef _MOREBTN_HXX //autogen
#include <vcl/morebtn.hxx>
#endif
#ifndef _STDCTRL_HXX //autogen
#include <svtools/stdctrl.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx" // -> ScQueryParam
#endif

#ifndef SC_ADDRESS_HXX
#include "address.hxx"
#endif

//------------------------------------------------------------------

class ScViewData;
class ScDocument;
class ScQueryItem;
class TypedStrCollection;

//==================================================================

class ScPivotFilterDlg : public ModalDialog
{
public:
                    ScPivotFilterDlg( Window* pParent,
                                      const SfxItemSet& rArgSet, SCTAB nSourceTab );
                    ~ScPivotFilterDlg();

    const ScQueryItem&  GetOutputItem();

private:
    FixedLine       aFlCriteria;
    //----------------------------
    ListBox         aLbField1;
    ListBox         aLbCond1;
    ComboBox        aEdVal1;
    //----------------------------
    ListBox         aLbConnect1;
    ListBox         aLbField2;
    ListBox         aLbCond2;
    ComboBox        aEdVal2;
    //----------------------------
    ListBox         aLbConnect2;
    ListBox         aLbField3;
    ListBox         aLbCond3;
    ComboBox        aEdVal3;
    //----------------------------
    FixedText       aFtConnect;
    FixedText       aFtField;
    FixedText       aFtCond;
    FixedText       aFtVal;

    FixedLine       aFlOptions;
    CheckBox        aBtnCase;
    CheckBox        aBtnRegExp;
    CheckBox        aBtnUnique;
    FixedText       aFtDbAreaLabel;
    FixedInfo       aFtDbArea;
    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
    HelpButton      aBtnHelp;
    MoreButton      aBtnMore;
    const String    aStrUndefined;
    const String    aStrNoName;
    const String    aStrNone;
    const String    aStrEmpty;
    const String    aStrNotEmpty;
    const String    aStrRow;
    const String    aStrColumn;

    const USHORT        nWhichQuery;
    const ScQueryParam  theQueryData;
    ScQueryItem*        pOutItem;
    ScViewData*         pViewData;
    ScDocument*         pDoc;
    SCTAB               nSrcTab;

    USHORT              nFieldCount;
    ComboBox*           aValueEdArr[3];
    ListBox*            aFieldLbArr[3];
    ListBox*            aCondLbArr[3];

    TypedStrCollection* pEntryLists[MAXCOLCOUNT];

#ifdef _PFILTDLG_CXX
private:
    void    Init            ( const SfxItemSet& rArgSet );
    void    FillFieldLists  ();
    void    UpdateValueList ( USHORT nList );
    void    ClearValueList  ( USHORT nList );
    USHORT  GetFieldSelPos  ( SCCOL nField );

    // Handler:
    DECL_LINK( LbSelectHdl, ListBox* );
    DECL_LINK( ValModifyHdl, ComboBox* );
    DECL_LINK( CheckBoxHdl,  CheckBox* );
#endif
};


#endif // SC_PFILTDLG_HXX

