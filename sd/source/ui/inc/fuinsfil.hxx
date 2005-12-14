/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fuinsfil.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-12-14 17:12:27 $
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

#ifndef SD_FU_INSERT_FILE_HXX
#define SD_FU_INSERT_FILE_HXX

#ifndef SD_FU_POOR_HXX
#include "fupoor.hxx"
#endif
#include <vector>

class SfxMedium;
struct StyleRequestData;

namespace sd {

class FuInsertFile
    : public FuPoor
{
public:
    TYPEINFO();

    static FunctionReference Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq );
    virtual void DoExecute( SfxRequest& rReq );

    static void     GetSupportedFilterVector( ::std::vector< String >& rFilterVector );

private:
    FuInsertFile (
        ViewShell* pViewSh,
        ::sd::Window* pWin,
        ::sd::View* pView,
        SdDrawDocument* pDoc,
        SfxRequest& rReq);

    String          aLayoutName;    // Layoutname der aktuell eingefuegten Seite
    String          aFilterName;    // gewaehlter Dateifilter
    String          aFile;          // gewaehlter Dateiname

    void            InsTextOrRTFinOlMode(SfxMedium* pMedium);
    BOOL            InsSDDinOlMode(SfxMedium* pMedium);
    void            InsTextOrRTFinDrMode(SfxMedium* pMedium);
    BOOL            InsSDDinDrMode(SfxMedium* pMedium);
};

} // end of namespace sd

#endif
