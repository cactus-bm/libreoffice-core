/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: areasave.hxx,v $
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

#ifndef SC_AREASAVE_HXX
#define SC_AREASAVE_HXX

#include "collect.hxx"
#include "global.hxx"
#include "address.hxx"

class ScDocument;
class ScAreaLink;


class ScAreaLinkSaver : public DataObject
{
private:
    String      aFileName;
    String      aFilterName;
    String      aOptions;
    String      aSourceArea;
    ScRange     aDestArea;
    ULONG       nRefresh;

public:
                ScAreaLinkSaver( const ScAreaLink& rSource );
                ScAreaLinkSaver( const ScAreaLinkSaver& rCopy );
    virtual     ~ScAreaLinkSaver();

    virtual DataObject* Clone() const;

    BOOL        IsEqual( const ScAreaLink& rCompare ) const;
    BOOL        IsEqualSource( const ScAreaLink& rCompare ) const;

    void        WriteToLink( ScAreaLink& rLink ) const;
    void        InsertNewLink( ScDocument* pDoc ) const;
};


class ScAreaLinkSaveCollection : public Collection
{
public:
                ScAreaLinkSaveCollection();
                ScAreaLinkSaveCollection( const ScAreaLinkSaveCollection& rCopy );
    virtual     ~ScAreaLinkSaveCollection();

    virtual DataObject* Clone() const;

    ScAreaLinkSaver*    operator[](USHORT nIndex) const {return (ScAreaLinkSaver*)At(nIndex);}

    BOOL        IsEqual( const ScDocument* pDoc ) const;
    void        Restore( ScDocument* pDoc ) const;

    // returns NULL if empty
    static ScAreaLinkSaveCollection* CreateFromDoc( const ScDocument* pDoc );
};


#endif

