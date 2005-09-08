/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fmmodel.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:53:39 $
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

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef SVX_LIGHT
#ifndef _SFX_OBJSH_HXX //autogen
#include <sfx2/objsh.hxx>
#endif
#else
class SfxObjectShell;
#endif

#pragma hdrstop

#ifndef _FM_FMMODEL_HXX
#include "fmmodel.hxx"
#endif

#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif

#ifndef _FM_PAGE_HXX
#include "fmpage.hxx"
#endif
#ifndef _SVX_FMUNDO_HXX
#include "fmundo.hxx"
#endif
#ifndef _SVX_SVDOBJ_HXX
#include "svdobj.hxx"
#endif

using ::com::sun::star::uno::Reference;
using ::com::sun::star::container::XNameContainer;

TYPEINIT1(FmFormModel, SdrModel);

struct FmFormModelImplData
{
    FmXUndoEnvironment*     pUndoEnv;
    sal_Bool                bOpenInDesignIsDefaulted;
    sal_Bool                bMovingPage;

    FmFormModelImplData()
        :pUndoEnv( NULL )
        ,bOpenInDesignIsDefaulted( sal_True )
        ,bMovingPage( sal_False )
    {
    }
};

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
FmFormModel::FmFormModel(SfxItemPool* pPool, SfxObjectShell* pPers)
            :SdrModel(pPool, pPers, LOADREFCOUNTS)
            ,pObjShell(0)
//BFS01         ,bStreamingOldVersion(sal_False)
            ,m_pImpl(NULL)
            ,m_bOpenInDesignMode(sal_False)
            ,m_bAutoControlFocus(sal_False)
{
#ifndef SVX_LIGHT
    m_pImpl = new FmFormModelImplData;
    m_pImpl->pUndoEnv = new FmXUndoEnvironment(*this);
    m_pImpl->pUndoEnv->acquire();
#endif
}

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
FmFormModel::FmFormModel(const XubString& rPath, SfxItemPool* pPool, SfxObjectShell* pPers)
            :SdrModel(rPath, pPool, pPers)
            ,pObjShell(0)
//BFS01         ,bStreamingOldVersion(sal_False)
            ,m_pImpl(NULL)
            ,m_bOpenInDesignMode(sal_False)
            ,m_bAutoControlFocus(sal_False)
{
#ifndef SVX_LIGHT
    m_pImpl = new FmFormModelImplData;
    m_pImpl->pUndoEnv = new FmXUndoEnvironment(*this);
    m_pImpl->pUndoEnv->acquire();
#endif
}

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
FmFormModel::FmFormModel(SfxItemPool* pPool, SfxObjectShell* pPers,
                         FASTBOOL bUseExtColorTable
                         )
            :SdrModel(pPool, pPers, bUseExtColorTable, LOADREFCOUNTS)
            ,pObjShell(0)
//BFS01         ,bStreamingOldVersion(sal_False)
            ,m_pImpl(NULL)
{
#ifndef SVX_LIGHT
    m_pImpl = new FmFormModelImplData;
    m_pImpl->pUndoEnv = new FmXUndoEnvironment(*this);
    m_pImpl->pUndoEnv->acquire();
#endif
}

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
FmFormModel::FmFormModel(const XubString& rPath, SfxItemPool* pPool, SfxObjectShell* pPers,
                         FASTBOOL bUseExtColorTable)
            :SdrModel(rPath, pPool, pPers, bUseExtColorTable, LOADREFCOUNTS)
            ,pObjShell(0)
//BFS01         ,bStreamingOldVersion(sal_False)
            ,m_bOpenInDesignMode(sal_False)
            ,m_bAutoControlFocus(sal_False)
{
#ifndef SVX_LIGHT
    m_pImpl = new FmFormModelImplData;
    m_pImpl->pUndoEnv = new FmXUndoEnvironment(*this);
    m_pImpl->pUndoEnv->acquire();
#endif
}

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/
FmFormModel::~FmFormModel()
{
#ifndef SVX_LIGHT
    if (pObjShell && m_pImpl->pUndoEnv->IsListening(*pObjShell))
        SetObjectShell(NULL);

    ClearUndoBuffer();
    // minimale grenze fuer undos
    SetMaxUndoActionCount(1);

    m_pImpl->pUndoEnv->release();
    delete m_pImpl;

#endif
}

/*************************************************************************
|*
|* Copy-Ctor
|*
\************************************************************************/

FmFormModel::FmFormModel(const FmFormModel&)
{
    DBG_ERROR("FmFormModel: CopyCtor not implemented");
}

/*************************************************************************
|*
|* Operator=
|*
\************************************************************************/
void FmFormModel::operator=(const FmFormModel&)
{
    DBG_ERROR("FmFormModel: operator= not implemented");
}

/*************************************************************************
|*
|* Operator==
|*
\************************************************************************/
FASTBOOL FmFormModel::operator==(const FmFormModel&) const
{
    DBG_ERROR("FmFormModel: operator== not implemented");
    return sal_False;
}


/*************************************************************************
|*
|* Erzeugt eine neue Seite
|*
\************************************************************************/
SdrPage* FmFormModel::AllocPage(FASTBOOL bMasterPage)
{
    return new FmFormPage(*this, NULL, bMasterPage);
}

/*************************************************************************
|*
|* WriteData
|*
\************************************************************************/

//BFS01void FmFormModel::WriteData(SvStream& rOut) const
//BFS01{
//BFS01#ifndef SVX_LIGHT
//BFS01
//BFS01 if( rOut.GetVersion() < SOFFICE_FILEFORMAT_50 )
//BFS01     ((FmFormModel*)this)->bStreamingOldVersion = sal_True;
//BFS01
//BFS01 SdrModel::WriteData( rOut );
//BFS01
//BFS01 //////////////////////////////////////////////////////////////////////
//BFS01 // Speichern der Option OpenInDesignMode
//BFS01 if (!bStreamingOldVersion)
//BFS01 {
//BFS01     SdrDownCompat aModelFormatCompat(rOut,STREAM_WRITE);
//BFS01
//BFS01     sal_uInt8 nTemp = m_bOpenInDesignMode;
//BFS01     rOut << nTemp;
//BFS01
//BFS01     nTemp = m_bAutoControlFocus;
//BFS01     rOut << nTemp;
//BFS01 }
//BFS01
//BFS01 ((FmFormModel*)this)->bStreamingOldVersion = sal_False;
//BFS01
//BFS01#endif
//BFS01}


/*************************************************************************
|*
|* ReadData
|*
\************************************************************************/
//BFS01void FmFormModel::ReadData(const SdrIOHeader& rHead, SvStream& rIn)
//BFS01{
//BFS01 if( rIn.GetVersion() < SOFFICE_FILEFORMAT_50 )
//BFS01     ((FmFormModel*)this)->bStreamingOldVersion = sal_True;
//BFS01
//BFS01 SdrModel::ReadData( rHead, rIn );
//BFS01
//BFS01 //////////////////////////////////////////////////////////////////////
//BFS01 // Lesen der Option OpenInDesignMode
//BFS01 if (!bStreamingOldVersion)
//BFS01 {
//BFS01     SdrDownCompat aCompat(rIn,STREAM_READ);
//BFS01     sal_uInt8 nTemp = 0;
//BFS01     rIn >> nTemp;
//BFS01
//BFS01     implSetOpenInDesignMode( nTemp ? sal_True : sal_False, sal_True );
//BFS01
//BFS01     if (aCompat.GetBytesLeft())
//BFS01     {   // it is a version which already wrote the AutoControlFocus flag
//BFS01         rIn >> nTemp;
//BFS01         m_bAutoControlFocus = nTemp ? sal_True : sal_False;
//BFS01     }
//BFS01 }
//BFS01
//BFS01 ((FmFormModel*)this)->bStreamingOldVersion = sal_False;
//BFS01}


/*************************************************************************
|*
|* InsertPage
|*
\************************************************************************/
void FmFormModel::InsertPage(SdrPage* pPage, sal_uInt16 nPos)
{
#ifndef SVX_LIGHT
    // hack solange Methode intern
    if (pObjShell && !m_pImpl->pUndoEnv->IsListening( *pObjShell ))
        SetObjectShell(pObjShell);
#endif

    SdrModel::InsertPage( pPage, nPos );
}

/*************************************************************************
|*
|* MovePage
|*
\************************************************************************/
void FmFormModel::MovePage( USHORT nPgNum, USHORT nNewPos )
{
#ifndef SVX_LIGHT
    m_pImpl->bMovingPage = sal_True;
        // see InsertPage for this
#endif

    SdrModel::MovePage( nPgNum, nNewPos );

#ifndef SVX_LIGHT
    m_pImpl->bMovingPage = sal_False;
#endif
}

/*************************************************************************
|*
|* RemovePage
|*
\************************************************************************/
SdrPage* FmFormModel::RemovePage(sal_uInt16 nPgNum)
{
    FmFormPage* pPage = (FmFormPage*)SdrModel::RemovePage(nPgNum);

#ifndef SVX_LIGHT
    if (pPage)
    {
        Reference< XNameContainer > xForms( pPage->GetForms( false ) );
        if ( xForms.is() )
            m_pImpl->pUndoEnv->RemoveForms( xForms );
    }
#endif

    return pPage;
}

/*************************************************************************
|*
|* InsertMasterPage
|*
\************************************************************************/
void FmFormModel::InsertMasterPage(SdrPage* pPage, sal_uInt16 nPos)
{
#ifndef SVX_LIGHT
    // hack solange Methode intern
    if (pObjShell && !m_pImpl->pUndoEnv->IsListening( *pObjShell ))
        SetObjectShell(pObjShell);
#endif

    SdrModel::InsertMasterPage(pPage, nPos);
}

/*************************************************************************
|*
|* RemoveMasterPage
|*
\************************************************************************/
SdrPage* FmFormModel::RemoveMasterPage(sal_uInt16 nPgNum)
{
    FmFormPage* pPage = (FmFormPage*)SdrModel::RemoveMasterPage(nPgNum);

#ifndef SVX_LIGHT
    if ( pPage )
    {
        Reference< XNameContainer > xForms( pPage->GetForms( false ) );
        if ( xForms.is() )
            m_pImpl->pUndoEnv->RemoveForms( xForms );
    }
#endif

    return pPage;
}

//------------------------------------------------------------------------
SdrLayerID FmFormModel::GetControlExportLayerId( const SdrObject& rObj ) const
{
    return rObj.GetLayer();
}

//------------------------------------------------------------------------
void FmFormModel::implSetOpenInDesignMode( sal_Bool _bOpenDesignMode, sal_Bool _bForce )
{
    if( ( _bOpenDesignMode != m_bOpenInDesignMode ) || _bForce )
    {
        m_bOpenInDesignMode = _bOpenDesignMode;

        if ( pObjShell )
            pObjShell->SetModified( sal_True );
    }
    // no matter if we really did it or not - from now on, it does not count as defaulted anymore
    m_pImpl->bOpenInDesignIsDefaulted = sal_False;
}

//------------------------------------------------------------------------
void FmFormModel::SetOpenInDesignMode( sal_Bool bOpenDesignMode )
{
#ifndef SVX_LIGHT
    implSetOpenInDesignMode( bOpenDesignMode, sal_False );
#endif
}

#ifndef SVX_LIGHT
//------------------------------------------------------------------------
sal_Bool FmFormModel::OpenInDesignModeIsDefaulted( )
{
    return m_pImpl->bOpenInDesignIsDefaulted;
}
#endif

//------------------------------------------------------------------------
void FmFormModel::SetAutoControlFocus( sal_Bool _bAutoControlFocus )
{
#ifndef SVX_LIGHT
    if( _bAutoControlFocus != m_bAutoControlFocus )
    {
        m_bAutoControlFocus = _bAutoControlFocus;
        pObjShell->SetModified( sal_True );
    }
#endif
}

//------------------------------------------------------------------------
void FmFormModel::SetObjectShell( SfxObjectShell* pShell )
{
#ifndef SVX_LIGHT
    if (pShell == pObjShell)
        return;

    if (pObjShell)
    {
        m_pImpl->pUndoEnv->EndListening( *this );
        m_pImpl->pUndoEnv->EndListening( *pObjShell );
    }

    pObjShell = pShell;

    if (pObjShell)
    {
        m_pImpl->pUndoEnv->SetReadOnly( pObjShell->IsReadOnly() || pObjShell->IsReadOnlyUI(), FmXUndoEnvironment::Accessor() );

        if (!m_pImpl->pUndoEnv->IsReadOnly())
             m_pImpl->pUndoEnv->StartListening(*this);

        m_pImpl->pUndoEnv->StartListening( *pObjShell );
    }
#endif
}

//------------------------------------------------------------------------
FmXUndoEnvironment& FmFormModel::GetUndoEnv()
{
    return *m_pImpl->pUndoEnv;
}
