/*************************************************************************
 *
 *  $RCSfile: dlgsnap.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2004-05-10 15:50:06 $
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


#ifndef _SD_DLGSNAP_HXX
#define _SD_DLGSNAP_HXX


#ifndef _IMAGEBTN_HXX //autogen
#include <vcl/imagebtn.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FRACT_HXX //autogen
#include <tools/fract.hxx>
#endif
#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif
#include "sdenumdef.hxx" //CHINA001
/************************************************************************/

class SfxItemSet;
class ::sd::View;
//move to sdenumdef.hxx
//CHINA001 #define RET_SNAP_DELETE 111
//CHINA001 enum SnapKind { SK_HORIZONTAL, SK_VERTICAL, SK_POINT };


/*************************************************************************
|*
|* Dialog zum Einstellen von Fanglinien und -punkten
|*
\************************************************************************/

class SdSnapLineDlg : public ModalDialog
{
private:
    FixedLine           aFlPos;
    FixedText           aFtX;
    MetricField         aMtrFldX;
    FixedText           aFtY;
    MetricField         aMtrFldY;
    FixedLine           aFlDir;
    ImageRadioButton    aRbPoint;
    ImageRadioButton    aRbVert;
    ImageRadioButton    aRbHorz;
    OKButton            aBtnOK;
    CancelButton        aBtnCancel;
    HelpButton          aBtnHelp;
    PushButton          aBtnDelete;
    long                nXValue;
    long                nYValue;
    FieldUnit           eUIUnit;
    Fraction            aUIScale;

    DECL_LINK( ClickHdl, Button * );

public:
    SdSnapLineDlg(Window* pWindow, const SfxItemSet& rInAttrs, ::sd::View* pView);

    void GetAttr(SfxItemSet& rOutAttrs);

    void HideRadioGroup();
    void HideDeleteBtn() { aBtnDelete.Hide(); }
    void SetInputFields(BOOL bEnableX, BOOL bEnableY);
};



#endif      // _SD_DLGSNAP_HXX
