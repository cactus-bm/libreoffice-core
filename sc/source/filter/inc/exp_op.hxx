/*************************************************************************
 *
 *  $RCSfile: exp_op.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: obo $ $Date: 2004-08-11 09:04:31 $
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

#ifndef _EXP_OP_HXX
#define _EXP_OP_HXX

#ifndef SC_FILTER_HXX
#include "filter.hxx"
#endif

#ifndef _ROOT_HXX
#include "root.hxx"
#endif

#ifndef SC_XEROOT_HXX
#include "xeroot.hxx"
#endif


class ScDocument;
class ScPatternAttr;
class ScFormulaCell;
class ExcDocument;


class ExportTyp
{
protected:
    SvStream&           aOut;           // Ausgabe-Stream
    ScDocument*         pD;             // Dokument
    CharSet             eZielChar;      // Ziel-Zeichensatz
public:
                        ExportTyp( SvStream& aStream, ScDocument* pDoc, CharSet eDest ):
                            aOut( aStream )
                        {
                            eZielChar = eDest;
                            pD = pDoc;
                        }

    virtual FltError    Write() = 0;
};




class ExportWK1 : public ExportTyp
{
private:
    BYTE                GenFormByte( const ScPatternAttr& );
    void                Bof();
    void                Eof();
    void                Calcmode();
    void                Calcorder();
    void                Split();
    void                Sync();
    void                Dimensions();
    void                Window1();
    void                Colw();
    void                Blank( const UINT16 nC, const UINT16 nR, const ScPatternAttr& );
    void                Number( const UINT16 nC, const UINT16 nR, const double f, const ScPatternAttr& );
    void                Label( const UINT16 nC, const UINT16 nR, const String&, const ScPatternAttr& );
    void                Formula( const UINT16 nC, const UINT16 nR, const ScFormulaCell*, const ScPatternAttr& );
    void                Protect();
    void                Footer();
    void                Header();
    void                Margins();
    void                Labelfmt();
    void                Calccount();
    void                Cursorw12();
    void                WKString( const UINT16 nC, const UINT16 nR, const ScFormulaCell*, const ScPatternAttr& );
    void                Snrange();
    void                Hidcol();
    void                Cpi();
public:

    static  const USHORT    WK1MAXCOL;
    static  const USHORT    WK1MAXROW;

    inline              ExportWK1( SvStream& r, ScDocument* p, CharSet e ) :
                            ExportTyp( r, p, e ) {};

    FltError            Write();
};




class ExportBiff5 : private XclExpRootData, public ExportTyp, public XclExpRoot
{
private:
    ExcDocument*        pExcDoc;

protected:
    RootData*           pExcRoot;

public:
                        ExportBiff5( SfxMedium& rMedium, SvStream&, XclBiff, ScDocument*, CharSet eDest, bool bRelUrl );
    virtual             ~ExportBiff5();
    FltError            Write();
};




class ExportBiff8 : public ExportBiff5
{
public:
                        ExportBiff8( SfxMedium& rMedium, SvStream&, XclBiff, ScDocument*, CharSet eDest, bool bRelUrl );
    virtual             ~ExportBiff8();
};


#endif


