/*************************************************************************
 *
 *  $RCSfile: prntopts.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: cl $ $Date: 2002-07-02 14:09:50 $
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

#ifndef _SD_PRNTOPTS_HXX
#define _SD_PRNTOPTS_HXX

#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif


/*
enum PrintType
{
    PT_DRAWING,
    PT_NOTES,
    PT_HANDOUT,
    PT_OUTLINE
};
*/
class SdModule;
class SdPrintOptions : public SfxTabPage
{
 friend class SdOptionsDlg;
 friend class SdModule;

private:
    FixedLine           aGrpPrint;
    CheckBox            aCbxDraw;
    CheckBox            aCbxNotes;
    CheckBox            aCbxHandout;
    CheckBox            aCbxOutline;

    FixedLine           aSeparator1FL;
    FixedLine           aGrpOutput;
    RadioButton         aRbtColor;
    RadioButton         aRbtGrayscale;
    RadioButton         aRbtBlackWhite;

    FixedLine           aGrpPrintExt;
    CheckBox            aCbxPagename;
    CheckBox            aCbxDate;
    CheckBox            aCbxTime;
    CheckBox            aCbxHiddenPages;

    FixedLine           aSeparator2FL;
    FixedLine           aGrpPageoptions;
    RadioButton         aRbtDefault;
    RadioButton         aRbtPagesize;
    RadioButton         aRbtPagetile;
    RadioButton         aRbtBooklet;
    CheckBox            aCbxFront;
    CheckBox            aCbxBack;

    CheckBox            aCbxPaperbin;

    const SfxItemSet&   rOutAttrs;

    DECL_LINK( ClickCheckboxHdl, CheckBox * );
    DECL_LINK( ClickBookletHdl, CheckBox * );

    void updateControls();

public:
            SdPrintOptions( Window* pParent, const SfxItemSet& rInAttrs);
            ~SdPrintOptions();

    static  SfxTabPage* Create( Window*, const SfxItemSet& );
    static  USHORT*     GetRanges();

    virtual BOOL FillItemSet( SfxItemSet& );
    virtual void Reset( const SfxItemSet & );

    void    SetDrawMode();
};



#endif // _SD_PRNTOPTS_HXX


