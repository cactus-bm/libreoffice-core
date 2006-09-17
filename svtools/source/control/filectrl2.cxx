/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: filectrl2.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 14:37:57 $
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
#include "precompiled_svtools.hxx"

// this file contains code from filectrl.cxx which needs to be compiled with enabled exception hanling

#ifndef _SV_FILECTRL_HXX
#include <filectrl.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef  _COM_SUN_STAR_UI_DIALOGS_XFILEPICKER_HPP_
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#endif

#ifndef _VCL_UNOHELP_HXX
#include <vcl/unohelp.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _OSL_FILE_H_
#include <osl/file.h>
#endif

#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ui;

void FileControl::ImplBrowseFile( )
{
    try
    {
        XubString aNewText;

        const ::rtl::OUString sServiceName = ::rtl::OUString::createFromAscii( "com.sun.star.ui.dialogs.FilePicker" );

        Reference< XMultiServiceFactory > xMSF = vcl::unohelper::GetMultiServiceFactory();
        Reference < dialogs::XFilePicker > xFilePicker( xMSF->createInstance( sServiceName ), UNO_QUERY );
        if ( xFilePicker.is() )
        {
            // transform the system notation text into a file URL
            ::rtl::OUString sSystemNotation = GetText(), sFileURL;
            oslFileError nError = osl_getFileURLFromSystemPath( sSystemNotation.pData, &sFileURL.pData );
            if ( nError == osl_File_E_INVAL )
                sFileURL = GetText();   // #97709# Maybe URL is already a file URL...

            //#90430# Check if URL is really a file URL
            ::rtl::OUString aTmp;
            if ( osl_getSystemPathFromFileURL( sFileURL.pData, &aTmp.pData ) == osl_File_E_None )
            {
                // initially set this directory
                xFilePicker->setDisplayDirectory( sFileURL );
            }

            if ( xFilePicker.is() && xFilePicker->execute() )
            {
                Sequence < rtl::OUString > aPathSeq = xFilePicker->getFiles();

                if ( aPathSeq.getLength() )
                {
                    aNewText = aPathSeq[0];
                    INetURLObject aObj( aNewText );
                    if ( aObj.GetProtocol() == INET_PROT_FILE )
                        aNewText = aObj.PathToFileName();
                    SetText( aNewText );
                    maEdit.GetModifyHdl().Call( &maEdit );
                }
            }
        }
        else
            ShowServiceNotAvailableError( this, sServiceName, sal_True );
    }
    catch( const Exception& )
    {
        DBG_ERROR( "FileControl::ImplBrowseFile: caught an exception while executing the file picker!" );
    }
}

