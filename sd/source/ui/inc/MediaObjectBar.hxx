/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MediaObjectBar.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: kz $ $Date: 2006-12-12 17:34:52 $
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

#ifndef SD_MEDIA_OBJECT_BAR_HXX
#define SD_MEDIA_OBJECT_BAR_HXX

#ifndef _SFXMODULE_HXX //autogen
#include <sfx2/module.hxx>
#endif
#ifndef _SFX_SHELL_HXX //autogen
#include <sfx2/shell.hxx>
#endif
#ifndef _SD_GLOB_HXX
#include "glob.hxx"
#endif

namespace sd {

class View;
class ViewShell;


class MediaObjectBar
    : public SfxShell
{
public:
    TYPEINFO();
    SFX_DECL_INTERFACE( SD_IF_SDDRAWMEDIAOBJECTBAR )

    MediaObjectBar (ViewShell* pSdViewShell, ::sd::View* pSdView);
    virtual ~MediaObjectBar (void);

    void            GetState( SfxItemSet& rSet );
    void            Execute( SfxRequest& rReq );

protected:
    ::sd::View* mpView;
    ViewShell*  mpViewSh;
};

} // end of namespace sd

#endif
