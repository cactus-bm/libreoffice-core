/*************************************************************************
 *
 *  $RCSfile: olinewin.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: dr $ $Date: 2002-08-14 12:23:27 $
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

#ifndef SC_OLINEWIN_HXX
#define SC_OLINEWIN_HXX

#ifndef _SV_HXX
#endif

#include "viewdata.hxx"

class ScOutlineEntry;
class ScOutlineTable;


enum ScOutlineMode { SC_OUTLINE_HOR, SC_OUTLINE_VER };


class ScOutlineWindow : public Window
{
private:
    ScOutlineMode       eMode;
    USHORT              nHeaderSize;
    ScSplitPos          eWhich;
    ScViewData*         pViewData;

    Color               aColor;
    ImageList*          pSymbols;

    BOOL                bHitMode;               // Knopf gedrueckt ?
    USHORT              nHitLevel;
    USHORT              nHitEntry;
    BOOL                bHitHeader;
    Point               aImagePos;
    BOOL                bIsInverted;


    BOOL            GetEntryPos( ScOutlineEntry* pEntry,
                                    long& rFirstEntry, long& rSecondEntry, long& rBitmapEntry,
                                    ScOutlineEntry* pPrevious );
    void            DoFunction( USHORT nLevel, USHORT nEntry, BOOL bHeader );
    BOOL            ButtonHit( const Point& rPos, USHORT& rLevel, USHORT& rEntry, BOOL& rHeader,
                                Point& rImagePos );
    BOOL            LineHit( const Point& rPos, USHORT& rLevel, USHORT& rEntry );

    BOOL            IsFirst(USHORT nPos);

    void            ImplDrawImage( const Point& rPos, sal_uInt16 nId );
    void            ImplDrawBorder( const Point& rPos, bool bPressed );

protected:
    virtual void    Paint( const Rectangle& rRect );

    virtual void    MouseMove( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt );
    virtual void    MouseButtonDown( const MouseEvent& rMEvt );

    void            ImplInitSettings();


public:
            ScOutlineWindow( Window* pParent, ScOutlineMode eNewMode,
                                ScViewData* pData, ScSplitPos eNewWhich );
            ~ScOutlineWindow();

    void    ScrollPixel( long nDiff );

    void    SetHeaderSize( USHORT nNewSize );

    long    GetDepthSize();

    virtual void DataChanged( const DataChangedEvent& rDCEvt );


};



#endif

