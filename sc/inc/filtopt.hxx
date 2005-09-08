/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: filtopt.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:40:23 $
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

#ifndef SC_FILTOPT_HXX
#define SC_FILTOPT_HXX


#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif


//==================================================================
// filter options
//==================================================================

class ScFilterOptions : public utl::ConfigItem
{
    BOOL        bWK3Flag;
    double      fExcelColScale;
    double      fExcelRowScale;

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();

public:
            ScFilterOptions();

    virtual void    Notify( const com::sun::star::uno::Sequence<rtl::OUString>& aPropertyNames );
    virtual void    Commit();

    BOOL        GetWK3Flag() const          { return bWK3Flag; }
    double      GetExcelColScale() const    { return fExcelColScale; }
    double      GetExcelRowScale() const    { return fExcelRowScale; }

    //  values are never modified by office
};


#endif

