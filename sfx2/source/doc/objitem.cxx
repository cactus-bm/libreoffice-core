/*************************************************************************
 *
 *  $RCSfile: objitem.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: kz $ $Date: 2004-08-31 14:57:32 $
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

#ifndef GCC
#pragma hdrstop
#endif

#include "objsh.hxx"
//#include "objshimp.hxx"
#include "objitem.hxx"

#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

//====================================================================

TYPEINIT1_AUTOFACTORY(SfxObjectShellItem,SfxPoolItem)
TYPEINIT1_AUTOFACTORY(SfxObjectItem,SfxPoolItem)

//=========================================================================

int SfxObjectShellItem::operator==( const SfxPoolItem &rItem ) const
{
     return PTR_CAST(SfxObjectShellItem, &rItem)->pObjSh == pObjSh;
}

//--------------------------------------------------------------------

String SfxObjectShellItem::GetValueText() const
{
    return String();
}

//--------------------------------------------------------------------

SfxPoolItem* SfxObjectShellItem::Clone( SfxItemPool *) const
{
    return new SfxObjectShellItem( Which(), pObjSh );
}

//--------------------------------------------------------------------

sal_Bool SfxObjectShellItem::QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId ) const
{
    if ( pObjSh )
    {
        // This item MUST provide a model. Please don't change this, there are UNO-based
        // implementations which need it!!
        rVal <<= pObjSh->GetModel();
    }
    else
    {
        rVal <<= ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >();
    }
    return TRUE;
}

//--------------------------------------------------------------------

sal_Bool SfxObjectShellItem::PutValue( const com::sun::star::uno::Any& rVal, BYTE nMemberId )
{
    // This item MUST have a model. Please don't change this, there are UNO-based
    // implementations which need it!!
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > xModel;

    if ( rVal >>= xModel )
    {
        if ( xModel.is() )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::lang::XUnoTunnel > xTunnel(
                xModel, ::com::sun::star::uno::UNO_QUERY );
            if ( xTunnel.is() )
            {
                ::com::sun::star::uno::Sequence < sal_Int8 > aSeq( (sal_Int8*) SvGlobalName( SFX_GLOBAL_CLASSID ).GetBytes(), 16 );
                sal_Int64 nHandle = xTunnel->getSomething( aSeq );
                if ( nHandle )
                {
                    pObjSh = (SfxObjectShell*)(sal_Int32*)nHandle;
                    return TRUE;
                }
            }
        }

        pObjSh = 0;
        return TRUE;
    }

    return FALSE;
}

//=========================================================================

SfxObjectItem::SfxObjectItem( USHORT nWhich, SfxShell *pSh )
:   SfxPoolItem( nWhich ),
    _pSh( pSh )
{}

//--------------------------------------------------------------------

int SfxObjectItem::operator==( const SfxPoolItem &rItem ) const
{
     SfxObjectItem *pOther = PTR_CAST(SfxObjectItem, &rItem);
     return pOther->_pSh == _pSh;
}

//--------------------------------------------------------------------

SfxPoolItem* SfxObjectItem::Clone( SfxItemPool *) const
{
    return new SfxObjectItem( Which(), _pSh );
}
