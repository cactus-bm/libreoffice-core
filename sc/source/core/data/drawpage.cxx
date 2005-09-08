/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: drawpage.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 18:24:57 $
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

#ifdef PCH
#include "core_pch.hxx"
#endif

#pragma hdrstop

// INCLUDE ---------------------------------------------------------------

#include <sfx2/objsh.hxx>

#include "drawpage.hxx"
#include "drwlayer.hxx"
#include "document.hxx"
#include "pageuno.hxx"

// STATIC DATA -----------------------------------------------------------

// -----------------------------------------------------------------------

ScDrawPage::ScDrawPage(ScDrawLayer& rNewModel, StarBASIC* pBasic, BOOL bMasterPage) :
    FmFormPage(rNewModel, pBasic, bMasterPage)
{
    SetSize( Size( LONG_MAX, LONG_MAX ) );
}

// -----------------------------------------------------------------------

__EXPORT ScDrawPage::~ScDrawPage()
{
}

// -----------------------------------------------------------------------

void __EXPORT ScDrawPage::RequestBasic()
{
    ScDrawLayer* pLayer = (ScDrawLayer*)GetModel();
    ScDocument* pDoc = pLayer->GetDocument();
    SfxObjectShell* pDocSh = pDoc ? pDoc->GetDocumentShell() : NULL;
    if (pDocSh)
        SetBasic(pDocSh->GetBasic());
}

// -----------------------------------------------------------------------

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ScDrawPage::createUnoPage()
{
    return static_cast<cppu::OWeakObject*>( new ScPageObj( this ) );
}


