/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: printopt.hxx,v $
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

#ifndef SC_PRINTOPT_HXX
#define SC_PRINTOPT_HXX

#include <svtools/poolitem.hxx>
#include <unotools/configitem.hxx>
#include "scdllapi.h"

class SC_DLLPUBLIC ScPrintOptions
{
private:
    BOOL    bSkipEmpty;
    BOOL    bAllSheets;

public:
                ScPrintOptions();
                ScPrintOptions( const ScPrintOptions& rCpy );
                ~ScPrintOptions();

    BOOL    GetSkipEmpty() const            { return bSkipEmpty; }
    void    SetSkipEmpty( BOOL bVal )       { bSkipEmpty = bVal; }
    BOOL    GetAllSheets() const            { return bAllSheets; }
    void    SetAllSheets( BOOL bVal )       { bAllSheets = bVal; }

    void    SetDefaults();

    const ScPrintOptions&   operator=  ( const ScPrintOptions& rCpy );
    int                     operator== ( const ScPrintOptions& rOpt ) const;
    int                     operator!= ( const ScPrintOptions& rOpt ) const;
};

//==================================================================
// item for the dialog / options page
//==================================================================

class SC_DLLPUBLIC ScTpPrintItem : public SfxPoolItem
{
public:
                TYPEINFO();
                ScTpPrintItem( USHORT nWhich );
                ScTpPrintItem( USHORT nWhich,
                               const ScPrintOptions& rOpt );
                ScTpPrintItem( const ScTpPrintItem& rItem );
                ~ScTpPrintItem();

    virtual String          GetValueText() const;
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

    const ScPrintOptions&   GetPrintOptions() const { return theOptions; }

private:
    ScPrintOptions theOptions;
};

//==================================================================
// config item
//==================================================================

class ScPrintCfg : public ScPrintOptions, public utl::ConfigItem
{
    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();

public:
            ScPrintCfg();

    void            SetOptions( const ScPrintOptions& rNew );
    void            OptionsChanged();   // after direct access to ScPrintOptions base class

    virtual void    Commit();
};

#endif
