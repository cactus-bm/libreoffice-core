/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: pfiledlg.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:17:50 $
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
#ifndef _SVX_PFILEDLG_HXX
#define _SVX_PFILEDLG_HXX

// include ---------------------------------------------------------------

#ifndef _FILEDLGHELPER_HXX
#include <sfx2/filedlghelper.hxx>
#endif

#ifndef INCLUDED_SVXDLLAPI_H
#include "svx/svxdllapi.h"
#endif

/*************************************************************************
|*
|* Filedialog to insert Plugin-Fileformats
|*
\************************************************************************/

class SVX_DLLPUBLIC SvxPluginFileDlg
{
private:
    sfx2::FileDialogHelper          maFileDlg;

public:
    // with nKind = SID_INSERT_SOUND    or
    //      nKind = SID_INSERT_VIDEO
    SvxPluginFileDlg (Window *pParent, USHORT nKind );
    ~SvxPluginFileDlg ();

    ErrCode                  Execute();
    String                   GetPath() const;

    static BOOL IsAvailable (USHORT nKind);

    // setting HelpId and/or context of FileDialogHelper
    void                    SetDialogHelpId( const sal_Int32 nHelpId );
    void                    SetContext( sfx2::FileDialogHelper::Context eNewContext );
};

#endif // _SVX_PFILEDLG_HXX


