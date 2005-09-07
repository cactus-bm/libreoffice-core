/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dataedit.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 21:14:51 $
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

#ifndef DATAEDIT_HXX
#define DATAEDIT_HXX

#ifndef _SV_MENU_HXX
#include <vcl/menu.hxx>
#endif
#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _TEXTDATA_HXX //autogen
#include <svtools/textdata.hxx>
#endif

class String;
class Font;

// Find, Load und Save m�ssen implementiert werden,
// die anderen m�ssen in MemberType existieren
#define DATA_FUNC_DEF( MemberName, MemberType )                                 \
public:                                                                         \
    MemberType MemberName;                                                      \
    BOOL Find( const String& rStr );                                            \
    BOOL Load( const String& rStr );                                            \
    BOOL Save( const String& rStr );                                            \
                                                                                \
    void GrabFocus(){ MemberName.GrabFocus(); }                                 \
    void Show(){ MemberName.Show(); }                                           \
    void SetPosPixel( const Point& rNewPos ){ MemberName.SetPosPixel(rNewPos); }\
    void SetSizePixel( const Size& rNewSize ){ MemberName.SetSizePixel(rNewSize); } \
    Size GetSizePixel(){ return MemberName.GetSizePixel(); }                    \
    Point GetPosPixel(){ return MemberName.GetPosPixel(); }                     \
    void Update(){ MemberName.Update(); }                                       \
    void SetFont( const Font& rNewFont ){ MemberName.SetFont(rNewFont); }       \
                                                                                \
    void Delete();                                                              \
    void Cut();                                                                 \
    void Copy();                                                                \
    void Paste();                                                               \
    void Undo();                                                                \
    void Redo();                                                                \
    String GetText() const;                                                     \
    void SetText( const String& rStr );                                         \
    BOOL HasText() const;                                                       \
    String GetSelected();                                                       \
    TextSelection GetSelection() const;                                         \
    void SetSelection( const TextSelection& rSelection );                       \
    USHORT GetLineNr() const;                                                   \
    void ReplaceSelected( const String& rStr );                                 \
    BOOL IsModified();                                                          \
    void SetModifyHdl( Link l );


class DataEdit
{
public:
    virtual ~DataEdit(){}

    virtual void Delete()=0;
    virtual void Cut()=0;
    virtual void Copy()=0;
    virtual void Paste()=0;

    virtual void Undo()=0;
    virtual void Redo()=0;

    virtual BOOL Find( const String& )=0;           // Text suchen & markieren
    virtual BOOL Load( const String& )=0;           // Text aus Datei laden
    virtual BOOL Save( const String& )=0;           // Text in Datei speichern
    virtual String GetSelected()=0;
    virtual void GrabFocus()=0;
    virtual TextSelection GetSelection() const=0;
    virtual void SetSelection( const TextSelection& rSelection )=0;
    virtual USHORT GetLineNr() const=0;
    virtual String GetText() const=0;
    virtual void SetText( const String& rStr )=0;
    virtual BOOL HasText() const=0;             // damit vermeiden wir GetText.Len()
    virtual void ReplaceSelected( const String& rStr )=0;
    virtual BOOL IsModified()=0;
    virtual void SetModifyHdl( Link )=0;
    virtual void Show()=0;
    virtual void SetPosPixel( const Point& rNewPos )=0;
    virtual void SetSizePixel( const Size& rNewSize )=0;
    virtual Size GetSizePixel()=0;
    virtual Point GetPosPixel()=0;
    virtual void Update()=0;
    virtual void SetFont( const Font& rNewFont )=0;

    virtual void BuildKontextMenu( PopupMenu *&pMenu ){;}   // Wer will kann hier eigene Eintr�ge hinzuf�gen
};

#endif
