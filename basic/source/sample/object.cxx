/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: object.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 14:27:22 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_basic.hxx"

#ifndef _ERRCODE_HXX //autogen
#include <tools/errcode.hxx>
#endif
#ifndef _SBX_SBXOBJECT_HXX //autogen
#include <basic/sbxobj.hxx>
#endif
#ifndef _SBXCLASS_HXX //autogen
#include <basic/sbx.hxx>
#endif
#ifndef __SBX_SBXVARIABLE_HXX //autogen
#include <basic/sbxvar.hxx>
#endif
#ifndef _MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif

#include "object.hxx"
#include "collelem.hxx"

// Das Sample-Objekt hat folgende Elemente:
// 1) Properties:
//    Name      der Name
//    Value     ein double-Wert, beide bereits als Default drin
// 2) Methoden:
//    Create    Erzeugen eines neuen Unterelements
//    Display   Ausgabe eines Textes
//    Square    Argument * Argument
//    Event     Aufruf eines Basic-Eventhandlers
// 3) Unterobjekte:
//    Per Create() kann ein neues Unterelement eingerichtet werden,
//    das indiziert werden kann, falls mehrere Objekte gleichen Namens
//    existieren.
// Diese Implementation ist ein Beispiel fuer eine tabellengesteuerte
// Version, die sehr viele Elemente enthalten kann. Die Elemente werden
// je nach Bedarf aus der Tabelle in das Objekt uebernommen.
// Die Collection findet sich in COLLECTN.*, die in der Collection
// enthaltenen Objekte in COLLELEM.*

// Das Sample-Objekt wird in ..\app\mybasic.cxx wie folgt in StarBASIC
// eingebaut:

// MyBasic::MyBasic() : StarBASIC()
// {
//      AddFactory( new SampleObjectFac() );
// }

// Das nArgs-Feld eines Tabelleneintrags ist wie folgt verschluesselt:

#define _ARGSMASK   0x00FF  // Bis zu 255 Argumente
#define _RWMASK     0x0F00  // Maske fuer R/W-Bits
#define _TYPEMASK   0xF000  // Maske fuer den Typ des Eintrags

#define _READ       0x0100  // kann gelesen werden
#define _BWRITE     0x0200  // kann as Lvalue verwendet werden
#define _LVALUE     _BWRITE  // kann as Lvalue verwendet werden
#define _READWRITE  0x0300  // beides
#define _OPT        0x0400  // TRUE: optionaler Parameter
#define _METHOD     0x1000  // Masken-Bit fuer eine Methode
#define _PROPERTY   0x2000  // Masken-Bit fuer eine Property
#define _COLL       0x4000  // Masken-Bit fuer eine Collection
                            // Kombination von oberen Bits:
#define _FUNCTION   0x1100  // Maske fuer Function
#define _LFUNCTION  0x1300  // Maske fuer Function, die auch als Lvalue geht
#define _ROPROP     0x2100  // Maske Read Only-Property
#define _WOPROP     0x2200  // Maske Write Only-Property
#define _RWPROP     0x2300  // Maske Read/Write-Property
#define _COLLPROP   0x4100  // Maske Read-Collection-Element

#define COLLNAME    "Elements"  // Name der Collection, hier mal hart verdrahtet

SampleObject::Methods SampleObject::aMethods[] = {
// Eine Sample-Methode (der Returnwert ist SbxNULL)
{ "Display", SbxEMPTY, &SampleObject::Display, 1 | _FUNCTION },
    // Ein Named Parameter
    { "message", SbxSTRING, NULL, 0 },
// Eine Sample-Funktion
{ "Square", SbxDOUBLE, &SampleObject::Square, 1 | _FUNCTION },
    // Ein Named Parameter
    { "value", SbxDOUBLE, NULL, 0 },
//  Basic-Callback
{ "Event", SbxEMPTY, &SampleObject::Event, 1 | _FUNCTION },
    // Ein Named Parameter
    { "event", SbxSTRING, NULL, 0 },
//  Element erzeugen
{ "Create", SbxEMPTY, &SampleObject::Create, 1 | _FUNCTION },
    // Ein Named Parameter
    { "name", SbxSTRING, NULL, 0 },

{ NULL, SbxNULL, NULL, -1 }};  // Tabellenende

SampleObject::SampleObject( const String& rClass ) : SbxObject( rClass )
{
    SetName( String( RTL_CONSTASCII_USTRINGPARAM("Sample") ) );
    PutDouble( 1.0 );   // Startwert fuer Value
}

// Suche nach einem Element:
// Hier wird linear durch die Methodentabelle gegangen, bis eine
// passende Methode gefunden wurde.
// Wenn die Methode/Property nicht gefunden wurde, nur NULL ohne
// Fehlercode zurueckliefern, da so auch eine ganze Chain von
// Objekten nach der Methode/Property befragt werden kann.

SbxVariable* SampleObject::Find( const String& rName, SbxClassType t )
{
    // Ist das Element bereits vorhanden?
    SbxVariable* pRes = SbxObject::Find( rName, t );
    if( !pRes && t != SbxCLASS_OBJECT )
    {
        // sonst suchen
        Methods* p = aMethods;
        short nIndex = 0;
        BOOL bFound = FALSE;
        while( p->nArgs != -1 )
        {
            if( rName.EqualsIgnoreCaseAscii( p->pName ) )
            {
                bFound = TRUE; break;
            }
            nIndex += ( p->nArgs & _ARGSMASK ) + 1;
            p = aMethods + nIndex;
        }
        if( bFound )
        {
            // Args-Felder isolieren:
            short nAccess = ( p->nArgs & _RWMASK ) >> 8;
            short nType   = ( p->nArgs & _TYPEMASK );
            String aName_ = String::CreateFromAscii( p->pName );
            SbxClassType eCT = SbxCLASS_OBJECT;
            if( nType & _PROPERTY )
                eCT = SbxCLASS_PROPERTY;
            else if( nType & _METHOD )
                eCT = SbxCLASS_METHOD;
            pRes = Make( aName_, eCT, p->eType );
            // Wir setzen den Array-Index + 1, da ja noch andere
            // Standard-Properties existieren, die auch aktiviert
            // werden muessen.
            pRes->SetUserData( nIndex + 1 );
            pRes->SetFlags( nAccess );
        }
    }
    return pRes;
}

// Aktivierung eines Elements oder Anfordern eines Infoblocks

void SampleObject::SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCT,
                             const SfxHint& rHint, const TypeId& rHT )
{
    const SbxHint* pHint = PTR_CAST(SbxHint,&rHint);
    if( pHint )
    {
        SbxVariable* pVar = pHint->GetVar();
        SbxArray* pPar_ = pVar->GetParameters();
        USHORT nIndex = (USHORT) pVar->GetUserData();
        // kein Index: weiterreichen!
        if( nIndex )
        {
            ULONG t = pHint->GetId();
            if( t == SBX_HINT_INFOWANTED )
                pVar->SetInfo( GetInfo( (short) pVar->GetUserData() ) );
            else
            {
                BOOL bWrite = FALSE;
                if( t == SBX_HINT_DATACHANGED )
                    bWrite = TRUE;
                if( t == SBX_HINT_DATAWANTED || bWrite )
                {
                    // Parameter-Test fuer Methoden:
                    USHORT nPar = aMethods[ --nIndex ].nArgs & 0x00FF;
                    // Element 0 ist der Returnwert
                    if( ( !pPar_ && nPar )
                     || ( pPar_->Count() != nPar+1 ) )
                        SetError( SbxERR_WRONG_ARGS );
                    // Alles klar, man kann den Call ausfuehren
                    else
                    {
                        (this->*(aMethods[ nIndex ].pFunc))( pVar, pPar_, bWrite );
                    }
                }
            }
        }
        SbxObject::SFX_NOTIFY( rBC, rBCT, rHint, rHT );
    }
}

// Zusammenbau der Infostruktur fuer einzelne Elemente

SbxInfo* SampleObject::GetInfo( short nIdx )
{
    Methods* p = &aMethods[ nIdx ];
    // Wenn mal eine Hilfedatei zur Verfuegung steht:
    // SbxInfo* pInfo_ = new SbxInfo( Hilfedateiname, p->nHelpId );
    SbxInfo* pInfo_ = new SbxInfo;
    short nPar = p->nArgs & _ARGSMASK;
    for( short i = 0; i < nPar; i++ )
    {
        p++;
        String aName_ = String::CreateFromAscii( p->pName );
        USHORT nFlags_ = ( p->nArgs >> 8 ) & 0x03;
        if( p->nArgs & _OPT )
            nFlags_ |= SBX_OPTIONAL;
        pInfo_->AddParam( aName_, p->eType, nFlags_ );
    }
    return pInfo_;
}

////////////////////////////////////////////////////////////////////////////

// Properties und Methoden legen beim Get (bPut = FALSE) den Returnwert
// im Element 0 des Argv ab; beim Put (bPut = TRUE) wird der Wert aus
// Element 0 gespeichert.

// Die Methoden:

void SampleObject::Display( SbxVariable*, SbxArray* pPar_, BOOL )
{
    // GetString() loest u.U. auch einen Error aus!
    String s( pPar_->Get( 1 )->GetString() );
    if( !IsError() )
        InfoBox( NULL, s ).Execute();
}

void SampleObject::Square( SbxVariable* pVar, SbxArray* pPar_, BOOL )
{
    double n = pPar_->Get( 1 )->GetDouble();
    pVar->PutDouble( n * n );
}

// Callback nach BASIC:

void SampleObject::Event( SbxVariable*, SbxArray* pPar_, BOOL )
{
    Call( pPar_->Get( 1 )->GetString(), NULL );
}

// Neues Element anlegen

void SampleObject::Create( SbxVariable* pVar, SbxArray* pPar_, BOOL )
{
    pVar->PutObject(
        MakeObject( pPar_->Get( 1 )->GetString(), String( RTL_CONSTASCII_USTRINGPARAM("SampleElement") ) ) );
}

// Die Factory legt unsere beiden Objekte an.

SbxObject* SampleObjectFac::CreateObject( const String& rClass )
{
    if( rClass.EqualsIgnoreCaseAscii( "SampleObject" ) )
        return new SampleObject( rClass );
    if( rClass.EqualsIgnoreCaseAscii( "SampleElement" ) )
        return new SampleElement( rClass );
    return NULL;
}

