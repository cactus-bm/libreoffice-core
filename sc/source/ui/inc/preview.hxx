/*************************************************************************
 *
 *  $RCSfile: preview.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: nn $ $Date: 2002-02-27 19:34:18 $
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

#ifndef SC_PREVIEW_HXX
#define SC_PREVIEW_HXX


#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#include "printfun.hxx"     // ScPrintState

class ScDocShell;
class ScPreviewShell;
class FmFormView;

class ScPreview : public Window
{
private:
                                        // eingestellt:
    long            nPageNo;            // Seite im Dokument
    USHORT          nZoom;              // eingestellter Zoom
    Point           aOffset;            // positiv

                                        // berechnet:
    BOOL            bValid;             // folgende Werte gueltig
    USHORT          nTabCount;
    USHORT          nTabsTested;        // fuer wieviele Tabellen ist nPages gueltig?
    long            nPages[MAXTAB+1];
    long            nFirstAttr[MAXTAB+1];
    USHORT          nTab;               // Tabelle
    long            nTabPage;           // Seite von Tabelle
    long            nTabStart;          // erste Seite der Tabelle (wirklich)
    long            nDisplayStart;      // dito, relativ zum Anfang der Zaehlung
    Date            aDate;
    Time            aTime;
    long            nTotalPages;
    Size            aPageSize;          // fuer GetOptimalZoom
    BOOL            bStateValid;
    BOOL            bLocationValid;
    ScPrintState    aState;
    ScPreviewLocationData* pLocationData;   // stores table layout for accessibility API
    FmFormView*     pDrawView;

                                        // intern:
    BOOL            bInPaint;
    BOOL            bInGetState;
    ScDocShell*     pDocShell;
    ScPreviewShell* pViewShell;


    void    TestLastPage();
    void    CalcPages( USHORT nToWhichTab );
    void    RecalcPages();
    void    UpdateDrawView();
    void    DoPrint( ScPreviewLocationData* pFillLocation );

    void    InvalidateLocationData();

protected:
    virtual void    Paint( const Rectangle& rRect );
    virtual void    Command( const CommandEvent& rCEvt );
    virtual void    KeyInput( const KeyEvent& rKEvt );

    virtual ::com::sun::star::uno::Reference< ::drafts::com::sun::star::accessibility::XAccessible > CreateAccessible();

public:
            ScPreview( Window* pParent, ScDocShell* pDocSh, ScPreviewShell* pViewSh );
            ~ScPreview();

    virtual void DataChanged( const DataChangedEvent& rDCEvt );

    void    DataChanged(BOOL bNewTime = FALSE);             // statt Invalidate rufen
    void    DoInvalidate();

    void    SetXOffset( long nX );
    void    SetYOffset( long nY );
    void    SetZoom(USHORT nNewZoom);
    void    SetPageNo( long nPage );

    const ScPreviewLocationData& GetLocationData();

    String  GetPosString();

    long    GetPageNo() const   { return nPageNo; }
    USHORT  GetZoom() const     { return nZoom; }
    Point   GetOffset() const   { return aOffset; }

    USHORT  GetTab()            { if (!bValid) { CalcPages(0); RecalcPages(); } return nTab; }
    long    GetTotalPages()     { if (!bValid) { CalcPages(0); RecalcPages(); } return nTotalPages; }

    BOOL    AllTested() const   { return bValid && nTabsTested >= nTabCount; }

    USHORT  GetOptimalZoom(BOOL bWidthOnly);
    long    GetFirstPage(USHORT nTab);

    void    CalcAll()           { CalcPages(MAXTAB); }
    void    SetInGetState(BOOL bSet) { bInGetState = bSet; }

    DECL_STATIC_LINK( ScPreview, InvalidateHdl, void* );
    static void StaticInvalidate();
};



#endif
