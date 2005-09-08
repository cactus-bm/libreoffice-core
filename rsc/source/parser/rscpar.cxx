/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rscpar.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 13:51:12 $
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
/****************** I N C L U D E S **************************************/
#pragma hdrstop

// C and C++ Includes.
#include <string.h>

#ifndef _RSCPAR_HXX
#include <rscpar.hxx>
#endif
#ifndef _RSCDB_HXX
#include <rscdb.hxx>
#endif

/****************** R s c F i l e I n s t ********************************/
/****************** C O D E **********************************************/
/*************************************************************************
|*
|*    RscFileInst::Init()
|*
|*    Beschreibung
|*    Ersterstellung    MM 05.11.91
|*    Letzte Aenderung  MM 17.02.93
|*
*************************************************************************/
void RscFileInst::Init()
{
    nLineNo = 0;
    nLineBufLen = 256;
    pLine = (char *)rtl_allocateMemory( nLineBufLen );
    *pLine = '\0';
    nScanPos = 0;
    cLastChar = '\0';
    bEof = FALSE;
};

/*************************************************************************
|*
|*    RscFileInst::RscFileInst()
|*
|*    Beschreibung
|*    Ersterstellung    MM 06.06.91
|*    Letzte Aenderung  MM 06.06.91
|*
*************************************************************************/
RscFileInst::RscFileInst( RscTypCont * pTC, ULONG lIndexSrc,
                          ULONG lFIndex, FILE * fFile )
{
    pTypCont = pTC;
    Init();

    lFileIndex = lFIndex;
    lSrcIndex = lIndexSrc;
    fInputFile = fFile;

    //Status: Zeiger am Ende des Lesepuffers
    nInputPos = nInputEndPos = nInputBufLen = READBUFFER_MAX;
    pInput    = (char *)rtl_allocateMemory( nInputBufLen );
}

RscFileInst::RscFileInst( RscTypCont * pTC, ULONG lIndexSrc,
                          ULONG lFIndex, const ByteString& rBuf )
{
    pTypCont     = pTC;
    Init();
    lFileIndex   = lFIndex;
    lSrcIndex    = lIndexSrc;
    fInputFile   = NULL;
    nInputPos    = 0;
    nInputEndPos = rBuf.Len();

    // Muss groesser sein wegen Eingabeende bei nInputBufLen < nInputEndPos
    nInputBufLen = nInputEndPos +1;
    pInput       = (char *)rtl_allocateMemory( nInputBufLen +100 );
    memcpy( pInput, rBuf.GetBuffer(), nInputEndPos );
}

/*************************************************************************
|*
|*    RscFileInst::~RscFileInst()
|*
|*    Beschreibung
|*    Ersterstellung    MM 06.06.91
|*    Letzte Aenderung  MM 06.06.91
|*
*************************************************************************/
RscFileInst::~RscFileInst(){
    if( pInput )
        rtl_freeMemory( pInput );
    if( pLine )
        rtl_freeMemory( pLine );
}

/*************************************************************************
|*
|*    RscFileInst::GetChar()
|*
|*    Beschreibung
|*    Ersterstellung    MM 01.06.91
|*    Letzte Aenderung  MM 09.08.91
|*
*************************************************************************/
int RscFileInst::GetChar()
{
    if( pLine[ nScanPos ] )
        return( pLine[ nScanPos++ ] );
    else if( nInputPos >= nInputEndPos && nInputEndPos != nInputBufLen )
    {
        // Dateiende
        bEof = TRUE;
        return 0;
    }
    else
    {
        GetNewLine();
        return( '\n' );
    }
}

/*************************************************************************
|*
|*    RscFileInst::GetNewLine()
|*
|*    Beschreibung
|*    Ersterstellung    MM 06.06.91
|*    Letzte Aenderung  MM 06.06.91
|*
*************************************************************************/
void RscFileInst::GetNewLine()
{
    nLineNo++;
    nScanPos = 0;

    //laeuft bis Dateiende
    sal_uInt32 nLen = 0;
    while( (nInputPos < nInputEndPos) || (nInputEndPos == nInputBufLen) )
    {
        if( (nInputPos >= nInputEndPos) && fInputFile )
        {
            nInputEndPos = fread( pInput, 1, nInputBufLen, fInputFile );
            nInputPos = 0;
        }

        while( nInputPos < nInputEndPos )
        {
            //immer eine Zeile lesen
            if( nLen >= nLineBufLen )
            {
                nLineBufLen += 256;
                // einen dazu fuer '\0'
                pLine = (char*)rtl_reallocateMemory( pLine, nLineBufLen +1 );
            }

            // cr lf, lf cr, lf oder cr wird '\0'
            if( pInput[ nInputPos ] == '\n' ){
                nInputPos++;
                if( cLastChar != '\r' ){
                    cLastChar = '\n';
                    pLine[ nLen++ ] = '\0';
                    goto END;
                }
            }
            else if( pInput[ nInputPos ] == '\r' ){
                nInputPos++;
                if( cLastChar != '\n' ){
                    cLastChar = '\r';
                    pLine[ nLen++ ] = '\0';
                    goto END;
                }
            }
            else
            {
                pLine[ nLen++ ] = pInput[ nInputPos++ ];
                if( nLen > 2 )
                {
                    if( pLine[nLen-3] == (char)0xef &&
                        pLine[nLen-2] == (char)0xbb &&
                        pLine[nLen-1] == (char)0xbf )
                    {
                        nLen -= 3;
                    }
                }
            }
        };
    };

    // Abbruch ueber EOF
    pLine[ nLen ] = '\0';

END:
    if( pTypCont->pEH->GetListFile() ){
        char buf[ 10 ];

        sprintf( buf, "%5d ", (int)GetLineNo() );
        pTypCont->pEH->LstOut( buf );
        pTypCont->pEH->LstOut( GetLine() );
        pTypCont->pEH->LstOut( "\n" );
    }
}

/*************************************************************************
|*
|*    RscFileInst::SetError()
|*
|*    Beschreibung
|*    Ersterstellung    MM 05.11.91
|*    Letzte Aenderung  MM 05.11.91
|*
*************************************************************************/
void RscFileInst::SetError( ERRTYPE aError )
{
    if( aError.IsOk() )
    {
        aFirstError = aError;
        nErrorLine  = GetLineNo();
        nErrorPos   = GetScanPos() -1;
    };
};
