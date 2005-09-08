/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rscrange.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 13:33:22 $
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
#ifndef _RSCRANGE_HXX
#define _RSCRANGE_HXX

#ifndef _RSCALL_H
#include <rscall.h>
#endif
#ifndef _RSCERROR_H
#include <rscerror.h>
#endif
#ifndef _RSCHASH_HXX
#include <rschash.hxx>
#endif
#ifndef _RSCTOP_HXX
#include <rsctop.hxx>
#endif

/******************* R s c R a n g e *************************************/
class RscRange : public RscTop
{
protected:
    struct RscRangeInst {
        sal_uInt16  nValue; // nValue = Ausgangswert - nMin
        BOOL    bDflt;  // Ist Default
    };
    INT32    nMin;   // Minimum des Bereiches
    INT32    nMax;   // Maximum des Bereiches
    sal_uInt32  nSize;
public:
                    RscRange( Atom nId, sal_uInt32 nTypId );
    virtual RSCCLASS_TYPE   GetClassType() const;
    RSCINST         Create( RSCINST * pInst, const RSCINST & rDfltInst, BOOL );
                    // Der zulaessige Bereich wird gesetzt
    ERRTYPE         SetRange( INT32 nMinimum, INT32 nMaximum );
                    // Gibt die Groesse der Klasse in Bytes
    sal_uInt32          Size(){ return nSize; }
                    // Eine Zuweisung an eine Variable
    virtual void    SetToDefault( const RSCINST & rInst )
                    {
                        ((RscRangeInst*)rInst.pData)->bDflt = TRUE;
                    }
    BOOL            IsDefault( const RSCINST & rInst)
                    {
                        return( ((RscRangeInst*)rInst.pData)->bDflt );
                    };
                    // Als Default setzen
    BOOL            IsValueDefault( const RSCINST & rInst, CLASS_DATA pDef );
    ERRTYPE         SetNumber( const RSCINST &, INT32 );
    ERRTYPE         GetNumber( const RSCINST &, INT32 * );
    void            WriteSrc( const RSCINST &, FILE * fOutput,
                              RscTypCont * pTC, sal_uInt32 nTab, const char * );
    ERRTYPE         WriteRc( const RSCINST &, RscWriteRc & aMem,
                             RscTypCont * pTC, sal_uInt32, BOOL bExtra );
    void            WriteRcAccess( FILE * fOutput, RscTypCont * pTC,
                                    const char * );
};

/******************* R s c L o n g R a n g e ******************************/
class RscLongRange : public RscTop
{
protected:
    struct RscLongRangeInst
    {
        INT32    nValue; // nValue = Ausgangswert - nMin
        BOOL    bDflt;  // Ist Default
    };
    INT32    nMin;   // Minimum des Bereiches
    INT32    nMax;   // Maximum des Bereiches
    sal_uInt32  nSize;
public:
                    RscLongRange( Atom nId, sal_uInt32 nTypId );
    virtual RSCCLASS_TYPE   GetClassType() const;
    RSCINST         Create( RSCINST * pInst, const RSCINST & rDfltInst, BOOL );
                    // Der zulaessige Bereich wird gesetzt
    ERRTYPE         SetRange( INT32 nMinimum, INT32 nMaximum );
                    // Gibt die Groesse der Klasse in Bytes
    sal_uInt32          Size(){ return nSize; }
                    // Eine Zuweisung an eine Variable
    virtual void    SetToDefault( const RSCINST & rInst )
                    {
                        ((RscLongRangeInst*)rInst.pData)->bDflt = TRUE;
                    }
    BOOL            IsDefault( const RSCINST & rInst)
                    {
                        return( ((RscLongRangeInst*)rInst.pData)->bDflt );
                    };
                    // Als Default setzen
    BOOL            IsValueDefault( const RSCINST & rInst, CLASS_DATA pDef );
    ERRTYPE         SetNumber( const RSCINST &, INT32 );
    ERRTYPE         GetNumber( const RSCINST &, INT32 * );
    void            WriteSrc( const RSCINST &, FILE * fOutput,
                              RscTypCont * pTC, sal_uInt32 nTab, const char * );
    ERRTYPE         WriteRc( const RSCINST &, RscWriteRc & aMem,
                             RscTypCont * pTC, sal_uInt32, BOOL bExtra );
    void            WriteRcAccess( FILE * fOutput, RscTypCont * pTC,
                                    const char * );

};

/******************* R s c L o n g E n u m R a n g e ******************/
class RscLongEnumRange : public RscLongRange
{
public:
                    RscLongEnumRange( Atom nId, sal_uInt32 nTypId );

    ERRTYPE         SetConst( const RSCINST & rInst, Atom nValueId,
                              INT32 nValue );
};

/******************* R s c I d R a n g e ***********************************/
class RscIdRange : public RscTop
{
    sal_uInt32  nSize;
protected:
    INT32    nMin;   // Minimum des Bereiches
    INT32    nMax;   // Maximum des Bereiches
public:
                    RscIdRange( Atom nId, sal_uInt32 nTypId );
    virtual RSCCLASS_TYPE   GetClassType() const;
                    // Der zulaessige Bereich wird gesetzt
    ERRTYPE         SetRange( INT32 nMinimum, INT32 nMaximum ){
                        nMin = nMinimum;
                        nMax = nMaximum;
                        return ERR_OK;
                    }
    RSCINST         Create( RSCINST * pInst, const RSCINST & rDfltInst, BOOL );
    void            Destroy( const RSCINST & rInst );
    sal_uInt32          Size(){ return nSize; }
    virtual void    SetToDefault( const RSCINST & rInst )
                    {
                        ((RscId*)rInst.pData)->aExp.cUnused = TRUE;
                    }
    BOOL            IsDefault( const RSCINST & rInst)
                    {
                        //cUnused wird fuer Defaultkennung verwendet
                        return ((RscId*)rInst.pData)->aExp.cUnused
                               ? TRUE : FALSE;
                    }
                    // Als Default setzen
    BOOL            IsValueDefault( const RSCINST & rInst, CLASS_DATA pDef );
    ERRTYPE         SetNumber( const RSCINST &, INT32 );
    ERRTYPE         GetNumber( const RSCINST &, INT32 * );
    ERRTYPE         SetRef( const RSCINST &, const RscId & rRscId );
    ERRTYPE         GetRef( const RSCINST & rInst, RscId * );
    void            WriteSrc( const RSCINST &, FILE * fOutput,
                              RscTypCont * pTC, sal_uInt32 nTab, const char * );
    ERRTYPE         WriteRc( const RSCINST &, RscWriteRc & aMem,
                             RscTypCont * pTC, sal_uInt32, BOOL bExtra );
    BOOL            IsConsistent( const RSCINST & rInst, RscInconsList * pList );
    void            WriteRcAccess( FILE * fOutput, RscTypCont * pTC,
                                    const char * );

};

/******************* R s c B o o l ***************************************/
class RscBool : public RscRange
{
public:
                    RscBool( Atom nId, sal_uInt32 nTypId );
    virtual RSCCLASS_TYPE   GetClassType() const;
                    // Der zulaessige Bereich wird gesetzt
    ERRTYPE         SetRange( INT32, INT32 ){
                        return( ERR_UNKNOWN_METHOD );
                    };
    ERRTYPE         SetBool( const RSCINST & rInst, BOOL b ){
                        return( SetNumber( rInst, (INT32)b ) );
                    };
    ERRTYPE         GetBool( const RSCINST & rInst, BOOL * pB){
                        INT32 l;
                        GetNumber( rInst, &l );
                        *pB = (0 != l);
                        return( ERR_OK );
                    };
    void            WriteSrc( const RSCINST &, FILE * fOutput,
                              RscTypCont * pTC, sal_uInt32 nTab, const char * );
    void            WriteRcAccess( FILE * fOutput, RscTypCont * pTC,
                                    const char * );

};

class RscBreakRange : public RscRange {
    INT32    nOutRange;
public:
                    RscBreakRange( Atom nId, sal_uInt32 nTypId );
    void            SetOutRange( INT32 nNumber ){
                        nOutRange = nNumber;
                    }
    RSCINST         Create( RSCINST * pInst, const RSCINST & rDfltInst, BOOL );
    ERRTYPE         SetNumber( const RSCINST &, INT32 );
};

#endif // _RSCRANGE_HXX
