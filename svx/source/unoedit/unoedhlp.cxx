/*************************************************************************
 *
 *  $RCSfile: unoedhlp.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: thb $ $Date: 2002-02-28 12:25:39 $
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

#pragma hdrstop

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include "unoedhlp.hxx"
#include "editdata.hxx"

//------------------------------------------------------------------------

TYPEINIT1( SvxEditSourceHint, TextHint );

SvxEditSourceHint::SvxEditSourceHint( ULONG _nId ) :
    TextHint( _nId ),
    mnStart( 0 ),
    mnEnd( 0 )
{
}

SvxEditSourceHint::SvxEditSourceHint( ULONG _nId, ULONG nValue, ULONG nStart, ULONG nEnd ) :
    TextHint( _nId, nValue ),
    mnStart( nStart),
    mnEnd( nEnd )
{
}

ULONG SvxEditSourceHint::GetValue() const
{
    return TextHint::GetValue();
}

ULONG SvxEditSourceHint::GetStartValue() const
{
    return mnStart;
}

ULONG SvxEditSourceHint::GetEndValue() const
{
    return mnEnd;
}

void SvxEditSourceHint::SetValue( ULONG n )
{
    TextHint::SetValue( n );
}

void SvxEditSourceHint::SetStartValue( ULONG n )
{
    mnStart = n;
}

void SvxEditSourceHint::SetEndValue( ULONG n )
{
    mnEnd = n;
}

//------------------------------------------------------------------------

SfxHint SvxEditSourceHintTranslator::EENotification2Hint( EENotify* aNotify )
{
    if( aNotify )
    {
        switch( aNotify->eNotificationType )
        {
            case EE_NOTIFY_TEXTMODIFIED:
                return TextHint( TEXT_HINT_MODIFIED, aNotify->nParagraph );

            case EE_NOTIFY_PARAGRAPHINSERTED:
                return TextHint( TEXT_HINT_PARAINSERTED, aNotify->nParagraph );

            case EE_NOTIFY_PARAGRAPHREMOVED:
                return TextHint( TEXT_HINT_PARAREMOVED, aNotify->nParagraph );

            case EE_NOTIFY_PARAGRAPHSMOVED:
                return SvxEditSourceHint( EDITSOURCE_HINT_PARASMOVED, aNotify->nParagraph, aNotify->nParam1, aNotify->nParam2 );

            case EE_NOTIFY_TEXTHEIGHTCHANGED:
                return TextHint( TEXT_HINT_TEXTHEIGHTCHANGED, aNotify->nParagraph );

            case EE_NOTIFY_TEXTVIEWSCROLLED:
                return TextHint( TEXT_HINT_VIEWSCROLLED );

            case EE_NOTIFY_TEXTVIEWSELECTIONCHANGED:
                return SvxEditSourceHint( EDITSOURCE_HINT_SELECTIONCHANGED );

            default:
                DBG_ERROR( "SvxEditSourceHintTranslator::EENotification2Hint unknown notification" );
                break;
        }
    }

    return SfxHint();
}
