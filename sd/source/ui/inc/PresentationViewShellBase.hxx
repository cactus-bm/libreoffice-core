/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: PresentationViewShellBase.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-21 17:24:43 $
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

#ifndef SD_PRESENTATION_VIEW_SHELL_BASE_HXX
#define SD_PRESENTATION_VIEW_SHELL_BASE_HXX

#include "ViewShellBase.hxx"


namespace sd {

/** This class exists to be able to register another factory that
    creates the view shell for the presentation.
*/
class PresentationViewShellBase
    : public ViewShellBase
{
public:
    TYPEINFO();
    SFX_DECL_VIEWFACTORY(PresentationViewShellBase);

    /** This constructor is used by the view factory of the SFX
        macros.
    */
    PresentationViewShellBase (SfxViewFrame *pFrame, SfxViewShell* pOldShell);
    virtual ~PresentationViewShellBase (void);

protected:

    /** The ViewTabBar is not supported so this factory method always
        returns <NULL/>.
    */
    virtual ViewTabBar* CreateViewTabBar (void);
};

} // end of namespace sd

#endif
