/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dxfreprd.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 02:56:13 $
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

#ifndef _DXFREPRD_HXX
#define _DXFREPRD_HXX

#ifndef _DXFBLKRD_HXX
#include <dxfblkrd.hxx>
#endif

#ifndef _DXFTBLRD_HXX
#include <dxftblrd.hxx>
#endif


//----------------------------------------------------------------------------
//--------------------Nebensachen---------------------------------------------
//----------------------------------------------------------------------------

//-------------------Eine 3D-Min/Max-Box--------------------------------------

class DXFBoundingBox {
public:
    BOOL bEmpty;
    double fMinX;
    double fMinY;
    double fMinZ;
    double fMaxX;
    double fMaxY;
    double fMaxZ;

    DXFBoundingBox() { bEmpty=TRUE; }
    void Union(const DXFVector & rVector);
};


//-------------------Die (konstante) Palette fuer DXF-------------------------

class DXFPalette {

public:

    DXFPalette();
    ~DXFPalette();

    BYTE GetRed(BYTE nIndex) const;
    BYTE GetGreen(BYTE nIndex) const;
    BYTE GetBlue(BYTE nIndex) const;

private:
    BYTE * pRed;
    BYTE * pGreen;
    BYTE * pBlue;
    void SetColor(BYTE nIndex, BYTE nRed, BYTE nGreen, BYTE nBlue);
};


//----------------------------------------------------------------------------
//-----------------DXF Datei lesen und repraesentieren------------------------
//----------------------------------------------------------------------------

class DXFRepresentation {

public:

    DXFPalette aPalette;
        // Die immer gleiche DXF-Farb-Palette

    DXFBoundingBox aBoundingBox;
        // Ist gleich den AutoCAD-Variablen EXTMIN, EXTMAX sofern in DXF-Datei
        // vorhanden, anderenfalls wird die BoundingBox berechnet (in Read()).

    DXFTables aTables;
        // Die Tabellen der DXF-Datei

    DXFBlocks aBlocks;
        // Die Bloecke der DXF-Datei

    DXFEntities aEntities;
        // Die Entities (aus der Entities-Section) der DXF-Datei

    rtl_TextEncoding mEnc;

    DXFRepresentation();
    ~DXFRepresentation();

        rtl_TextEncoding getTextEncoding() const;
        void setTextEncoding(rtl_TextEncoding aEnc);

    BOOL Read(SvStream & rIStream,
              PFilterCallback pCallback, void * pCallerData,
              USHORT nMinPercent, USHORT nMaxPercent);
        // Liesst die komplette DXF-Datei ein.

private:

    void ReadHeader(DXFGroupReader & rDGR);
    void CalcBoundingBox(const DXFEntities & rEntities,
                         DXFBoundingBox & rBox);
};

//----------------------------------------------------------------------------
//-------------------inlines--------------------------------------------------
//----------------------------------------------------------------------------

inline BYTE DXFPalette::GetRed(BYTE nIndex) const { return pRed[nIndex]; }
inline BYTE DXFPalette::GetGreen(BYTE nIndex) const { return pGreen[nIndex]; }
inline BYTE DXFPalette::GetBlue(BYTE nIndex) const { return pBlue[nIndex]; }
inline rtl_TextEncoding DXFRepresentation::getTextEncoding() const { return mEnc; }
inline void DXFRepresentation::setTextEncoding(rtl_TextEncoding aEnc) { mEnc = aEnc; }


#endif


