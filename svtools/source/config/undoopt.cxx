/*************************************************************************
 *
 *  $RCSfile: undoopt.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-06-16 10:09:56 $
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

#include "undoopt.hxx"

#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SFXSMPLHINT_HXX
#include <smplhint.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#include <osl/mutex.hxx>

using namespace utl;
using namespace rtl;
using namespace com::sun::star::uno;

static SvtUndoOptions_Impl* pOptions = NULL;
static sal_Int32           nRefCount = 0;

#define STEPS 0

class SvtUndoOptions_Impl : public utl::ConfigItem, public SfxBroadcaster
{
    sal_Int32               nUndoCount;
    Sequence< rtl::OUString > m_aPropertyNames;

public:
                            SvtUndoOptions_Impl();

    virtual void            Notify( const com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
    virtual void            Commit();
    void                    Load();

    void                    SetUndoCount( sal_Int32 n ) { nUndoCount = n; SetModified();  }
    sal_Int32               GetUndoCount() const        { return nUndoCount; }
};

// -----------------------------------------------------------------------

SvtUndoOptions_Impl::SvtUndoOptions_Impl()
    : ConfigItem( OUString::createFromAscii("Office.Common/Undo") )
    , nUndoCount( 20 )
{
    Load();
}

void SvtUndoOptions_Impl::Commit()
{
    OUString* pNames = m_aPropertyNames.getArray();
    Sequence< Any > aValues( m_aPropertyNames.getLength() );
    Any* pValues = aValues.getArray();
    for ( int nProp = 0; nProp < m_aPropertyNames.getLength(); nProp++ )
    {
        switch ( nProp )
        {
            case STEPS :
                pValues[nProp] <<= nUndoCount;
                break;
            default:
                DBG_ERRORFILE( "invalid index to save a path" );
        }
    }

    PutProperties( m_aPropertyNames, aValues );
    //broadcast changes
    Broadcast(SfxSimpleHint(SFX_HINT_UNDO_OPTIONS_CHANGED));
}

// -----------------------------------------------------------------------
void SvtUndoOptions_Impl::Load()
{
    if(!m_aPropertyNames.getLength())
    {
        static const char* aPropNames[] =
        {
            "Steps",
        };

        const int nCount = sizeof( aPropNames ) / sizeof( const char* );
        m_aPropertyNames.realloc(nCount);
        OUString* pNames = m_aPropertyNames.getArray();
        for ( int i = 0; i < nCount; i++ )
            pNames[i] = OUString::createFromAscii( aPropNames[i] );
        EnableNotification( m_aPropertyNames );
    }

    Sequence< Any > aValues = GetProperties( m_aPropertyNames );
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT( aValues.getLength() == m_aPropertyNames.getLength(), "GetProperties failed" );
    if ( aValues.getLength() == m_aPropertyNames.getLength() )
    {
        for ( int nProp = 0; nProp < m_aPropertyNames.getLength(); nProp++ )
        {
            DBG_ASSERT( pValues[nProp].hasValue(), "property value missing" );
            if ( pValues[nProp].hasValue() )
            {
                switch ( nProp )
                {
                    case STEPS :
                    {
                        sal_Int32 nTemp;
                        if ( pValues[nProp] >>= nTemp )
                            nUndoCount = nTemp;
                        else
                            DBG_ERROR( "Wrong Type!" );
                        break;
                    }

                    default:
                        DBG_ERROR( "Wrong Type!" );
                        break;
                }
            }
        }
    }
}
// -----------------------------------------------------------------------
void SvtUndoOptions_Impl::Notify( const Sequence<rtl::OUString>& aPropertyNames )
{
    Load();
    //broadcast changes
    Broadcast(SfxSimpleHint(SFX_HINT_UNDO_OPTIONS_CHANGED));
}
// -----------------------------------------------------------------------
SvtUndoOptions::SvtUndoOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( osl::Mutex::getGlobalMutex() );
    if ( !pOptions )
        pOptions = new SvtUndoOptions_Impl;
    ++nRefCount;
    pImp = pOptions;
    StartListening(*pImp);
}

// -----------------------------------------------------------------------

SvtUndoOptions::~SvtUndoOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
    EndListening(*pImp);
    if ( !--nRefCount )
    {
        if ( pOptions->IsModified() )
            pOptions->Commit();
        DELETEZ( pOptions );
    }
}

void SvtUndoOptions::SetUndoCount( sal_Int32 n )
{
    pImp->SetUndoCount( n );
}

sal_Int32 SvtUndoOptions::GetUndoCount() const
{
    return pImp->GetUndoCount();
}

void SvtUndoOptions::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    vos::OGuard aVclGuard( Application::GetSolarMutex() );
    Broadcast( rHint );
}

