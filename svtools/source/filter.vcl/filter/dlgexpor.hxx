/*************************************************************************
 *
 *  $RCSfile: dlgexpor.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: sj $ $Date: 2002-07-16 09:34:28 $
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


#ifndef _DLGEXPOR_HXX_
#define _DLGEXPOR_HXX_

#include "fltcall.hxx"
#include <vcl/dialog.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <vcl/lstbox.hxx>

/*************************************************************************
|*
|* Dialog zum Einstellen von Filteroptionen bei Pixelformaten
|*
\************************************************************************/

class FilterConfigItem;
class DlgExportPix : public ModalDialog
{
private:

    FltCallDialogParameter& rFltCallPara;

    OKButton            aBtnOK;
    CancelButton        aBtnCancel;
    HelpButton          aBtnHelp;

    ListBox             aLbColors;
    CheckBox            aCbxRLE;
    FixedLine           aGrpColors;

    RadioButton         aRbOriginal;
    RadioButton         aRbRes;
    RadioButton         aRbSize;
    FixedText           aFtSizeX;
    MetricField         aMtfSizeX;
    FixedText           aFtSizeY;
    MetricField         aMtfSizeY;
    FixedLine           aGrpMode;
    ComboBox            aCbbRes;

    FilterConfigItem*   pConfigItem;
    ResMgr*             pMgr;

    String              aExt;

                        DECL_LINK( OK, void* p );
                        DECL_LINK( ClickRbOriginal,void* p );
                        DECL_LINK( ClickRbRes,void* p );
                        DECL_LINK( ClickRbSize,void* p );
                        DECL_LINK( SelectLbColors, void* p );

public:
                        DlgExportPix( FltCallDialogParameter& rPara );
                        ~DlgExportPix();
};


/*************************************************************************
|*
|* Dialog zum Einstellen von Filteroptionen bei Vektorformaten
|*
\************************************************************************/
class DlgExportVec : public ModalDialog
{
private:

    FltCallDialogParameter& rFltCallPara;

    OKButton            aBtnOK;
    CancelButton        aBtnCancel;
    HelpButton          aBtnHelp;

    RadioButton         aRbOriginal;
    RadioButton         aRbSize;
    FixedLine           aGrpMode;

    FixedText           aFtSizeX;
    MetricField         aMtfSizeX;
    FixedText           aFtSizeY;
    MetricField         aMtfSizeY;
    FixedLine           aGrpSize;

    FilterConfigItem*   pConfigItem;
    ResMgr*             pMgr;

    String              aExt;

    DECL_LINK( OK, void* p );
    DECL_LINK( ClickRbOriginal,void* p );
    DECL_LINK( ClickRbSize,void* p );

public:
            DlgExportVec( FltCallDialogParameter& rPara );
            ~DlgExportVec();
};

#endif // _DLGEXPOR_HXX_

