/*************************************************************************
 *
 *  $RCSfile: fileview.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: pb $ $Date: 2001-05-07 10:28:03 $
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
#ifndef _SVT_FILEVIEW_HXX
#define _SVT_FILEVIEW_HXX

#include <vcl/ctrl.hxx>
#include <vcl/image.hxx>

// class SvtFileView -----------------------------------------------------

class ViewTabListBox_Impl;
class SvLBoxEntry;

class SvtFileView : public Control
{
private:
    ViewTabListBox_Impl*    mpView;
    sal_Bool                mbOnlyFolder;
    sal_Bool                mbAutoResize;
    String                  maViewURL;
    String                  maAllFilter;
    String                  maCurrentFilter;
    Image                   maFolderImage;
    Link                    maOpenDoneLink;

    void                    OpenFolder();

public:
    SvtFileView( Window* pParent, const ResId& rResId, sal_Bool bOnlyFolder, sal_Bool bMultiSelection );
    ~SvtFileView();

    const String&           GetViewURL() const { return maViewURL; }
    String                  GetURL( SvLBoxEntry* pEntry ) const;
    String                  GetCurrentURL() const;

    void                    CreateNewFolder( const String& rNewFolder );
    sal_Bool                HasPreviousLevel( String& rParentURL ) const;
    sal_Bool                PreviousLevel( String& rNewURL );

    void                    SetHelpId( sal_uInt32 nHelpId );
    void                    SetSizePixel( const Size& rNewSize );
    void                    SetPosSizePixel( const Point& rNewPos, const Size& rNewSize );
    void                    Initialize( const String& rURL, const String& rFilter );
    void                    ExecuteFilter( const String& rFilter );
    void                    SetNoSelection();
    void                    SetFocusInView();
    void                    ResetCursor();

    void                    SetSelectHdl( const Link& rHdl );
    void                    SetDoubleClickHdl( const Link& rHdl );
    void                    SetOpenDoneHdl( const Link& rHdl ) { maOpenDoneLink = rHdl; }

    ULONG                   GetSelectionCount() const;
    SvLBoxEntry*            FirstSelected() const;
    SvLBoxEntry*            NextSelected( SvLBoxEntry* pEntry ) const;
    void                    EnableAutoResize();
};

// struct SvtContentEntry ------------------------------------------------

struct SvtContentEntry
{
    sal_Bool    mbIsFolder;
    UniString   maURL;

    SvtContentEntry( const UniString& rURL, sal_Bool bIsFolder ) :
        mbIsFolder( bIsFolder ), maURL( rURL ) {}
};

#endif // _SVT_FILEVIEW_HXX

