/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: undomanager.cxx,v $
 * $Revision: 1.5 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sd.hxx"
#include <tools/debug.hxx>
#include "undo/undomanager.hxx"

using namespace sd;

UndoManager::UndoManager( USHORT nMaxUndoActionCount /* = 20 */ )
: SfxUndoManager( nMaxUndoActionCount )
, mnListLevel( 0 )
{
}

void UndoManager::EnterListAction(const UniString &rComment, const UniString& rRepeatComment, USHORT nId /* =0 */)
{
    if( !isInUndo() )
    {
        mnListLevel++;
        SfxUndoManager::EnterListAction( rComment, rRepeatComment, nId );
    }
}

void UndoManager::LeaveListAction()
{
    if( !isInUndo() )
    {
        SfxUndoManager::LeaveListAction();
        if( mnListLevel )
        {
            mnListLevel--;
        }
        else
        {
            DBG_ERROR("sd::UndoManager::LeaveListAction(), no open list action!" );
        }
    }
}

void UndoManager::AddUndoAction( SfxUndoAction *pAction, BOOL bTryMerg /* = FALSE */ )
{
    if( !isInUndo() )
    {
        SfxUndoManager::AddUndoAction( pAction, bTryMerg );
    }
    else
    {
        delete pAction;
    }
}


BOOL UndoManager::Undo( USHORT nCount )
{
    ScopeLockGuard aGuard( maIsInUndoLock );
    return SfxUndoManager::Undo( nCount );
}

BOOL UndoManager::Redo( USHORT nCount )
{
    ScopeLockGuard aGuard( maIsInUndoLock );
    return SfxUndoManager::Redo( nCount );
}

