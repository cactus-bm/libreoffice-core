/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: delcodlg.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:20:48 $
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

#ifndef SC_DELCODLG_HXX
#define SC_DELCODLG_HXX

#ifndef _SV_DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/imagebtn.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif

//------------------------------------------------------------------------

class ScDeleteContentsDlg : public ModalDialog
{
private:
    FixedLine       aFlFrame;
    CheckBox        aBtnDelAll;
    CheckBox        aBtnDelStrings;
    CheckBox        aBtnDelNumbers;
    CheckBox        aBtnDelDateTime;
    CheckBox        aBtnDelFormulas;
    CheckBox        aBtnDelNotes;
    CheckBox        aBtnDelAttrs;
    CheckBox        aBtnDelObjects;
    OKButton        aBtnOk;
    CancelButton    aBtnCancel;
    HelpButton      aBtnHelp;

    BOOL            bObjectsDisabled;

    static BOOL     bPreviousAllCheck;
    static USHORT   nPreviousChecks;

    void DisableChecks( BOOL bDelAllChecked = TRUE );
    DECL_LINK( DelAllHdl, void * );

public:
            ScDeleteContentsDlg( Window* pParent,
                                 USHORT  nCheckDefaults = 0 );
            ~ScDeleteContentsDlg();
    void    DisableObjects();

    USHORT  GetDelContentsCmdBits() const;
};


#endif // SC_DELCODLG_HXX


