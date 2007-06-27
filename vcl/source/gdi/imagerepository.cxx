/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: imagerepository.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 20:14:25 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_vcl.hxx"

#ifndef VCL_IMAGEREPOSITORY_HXX
#include <vcl/imagerepository.hxx>
#endif

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _SV_IMPIMAGETREE_H
#include "impimagetree.hxx"
#endif

//........................................................................
namespace vcl
{
//........................................................................

    //====================================================================
    //= ImageRepository
    //====================================================================
    //--------------------------------------------------------------------
    bool ImageRepository::loadImage( const ::rtl::OUString& _rName, BitmapEx& _out_rImage, bool _bSearchLanguageDependent )
    {
        ::rtl::OUString sCurrentSymbolsStyle = Application::GetSettings().GetStyleSettings().GetCurrentSymbolsStyleName();

        ImplImageTreeSingletonRef aImplImageTree;
        return aImplImageTree->loadImage( _rName, sCurrentSymbolsStyle, _out_rImage, _bSearchLanguageDependent );
    }

//........................................................................
} // namespace vcl
//........................................................................

