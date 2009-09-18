/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: tabbgcolordlg.hxx,v $
 * $Revision: 1.0 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef SC_TABBGCOLORDLG_HXX
#define SC_TABBGCOLORDLG_HXX

#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <vcl/imagebtn.hxx>
#include <svtools/valueset.hxx>

//------------------------------------------------------------------------

class ScTabBgColorDlg : public ModalDialog
{
public:
        ScTabBgColorDlg(    Window* pParent,
                            const String& rTitle,
                            const String& rTabBgColorNoColorText,
                            const Color& rDefaultColor,
                            ULONG nHelpId );
        ~ScTabBgColorDlg();

        void GetSelectedColor( Color& rColor ) const;
        //void SelectColor( const Color& rColor2 ) const;
private:
    class ScTabBgColorValueSet : public ValueSet
    {

    public:
            ScTabBgColorValueSet(Control* pParent, const ResId& rResId, ScTabBgColorDlg* pTabBgColorDlg);

            virtual void KeyInput( const KeyEvent& rKEvt );
    private:
        ScTabBgColorDlg* aTabBgColorDlg;
    };
        Control                 aBorderWin;
        ScTabBgColorValueSet    aTabBgColorSet;
        FixedLine               aTabBgColorBox;
        OKButton                aBtnOk;
        CancelButton            aBtnCancel;
        HelpButton              aBtnHelp;
        Color                   aTabBgColor;
        const String            aTabBgColorNoColorText;
        ULONG                   aHelpId;

        void            FillColorValueSets_Impl();

        DECL_LINK( TabBgColorDblClickHdl_Impl, ValueSet* );
        DECL_LINK( TabBgColorOKHdl_Impl, OKButton* pBtn );
};

#endif // SC_TABBGCOLORDLG_HXX
