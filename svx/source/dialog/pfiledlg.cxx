/*************************************************************************
 *
 *  $RCSfile: pfiledlg.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: gt $ $Date: 2002-10-18 13:31:56 $
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

// include ---------------------------------------------------------------

#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif

#ifndef _COM_SUN_STAR_PLUGIN_PLUGINDESCRIPTION_HPP_
#include <com/sun/star/plugin/PluginDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_PLUGIN_XPLUGINMANAGER_HPP_
#include <com/sun/star/plugin/XPluginManager.hpp>
#endif

#include <comphelper/processfactory.hxx>

#include "pfiledlg.hxx"
#include "dialogs.hrc"

#include "dialmgr.hxx"
#include <list>

using namespace ::rtl;
using namespace ::com::sun::star;

sal_Char __READONLY_DATA sAudio[] = "audio";
sal_Char __READONLY_DATA sVideo[] = "video";

/*************************************************************************
|*
|* Filedialog to insert Plugin-Fileformats
|*
\************************************************************************/

ErrCode SvxPluginFileDlg::Execute()
{
    return maFileDlg.Execute();
}

String SvxPluginFileDlg::GetPath() const
{
    return maFileDlg.GetPath();
}

SvxPluginFileDlg::SvxPluginFileDlg (Window *pParent, sal_uInt16 nKind ) :
    maFileDlg(SFXWB_INSERT)
{
    // set title of the dialogwindow
    switch (nKind)
    {
        case SID_INSERT_SOUND :
        {
            maFileDlg.SetTitle(SVX_RESSTR(STR_INSERT_SOUND_TITLE));
        }
        break;
        case SID_INSERT_VIDEO :
        {
            maFileDlg.SetTitle(SVX_RESSTR(STR_INSERT_VIDEO_TITLE));
        }
        break;
    }

    // fill the filterlist of the filedialog with data of installed plugins
    uno::Reference< lang::XMultiServiceFactory >  xMgr( ::comphelper::getProcessServiceFactory() );

    if( xMgr.is() )
    {
        uno::Reference< plugin::XPluginManager > rPluginManager( xMgr->createInstance(
            OUString::createFromAscii( "com.sun.star.plugin.PluginManager" ) ), uno::UNO_QUERY );
        if ( rPluginManager.is() )
        {
            const uno::Sequence<plugin::PluginDescription > aSeq( rPluginManager->getPluginDescriptions() );
            const plugin::PluginDescription* pDescription = aSeq.getConstArray();
            sal_Int32 nAnzahlPlugins = rPluginManager->getPluginDescriptions().getLength();

            std::list< String > aPlugNames;
            std::list< String > aPlugExtensions;
            std::list< String >::iterator j;
            std::list< String >::iterator k;
            std::list< String >::const_iterator end;

            for ( int i = 0; i < nAnzahlPlugins; i++ )
            {
                String aStrPlugMIMEType( pDescription[i].Mimetype );
                String aStrPlugName( pDescription[i].Description );
                String aStrPlugExtension( pDescription[i].Extension );

                aStrPlugMIMEType.ToLowerAscii();
                aStrPlugExtension.ToLowerAscii();

                if ( ( nKind == SID_INSERT_SOUND && aStrPlugMIMEType.SearchAscii ( sAudio ) == 0 ) ||
                     ( nKind == SID_INSERT_VIDEO && aStrPlugMIMEType.SearchAscii ( sVideo ) == 0 ) )
                {
                    // extension already in the filterlist of the filedlg ?
                    sal_Bool bAlreadyExist = sal_False;
                    for ( j = aPlugExtensions.begin(), end = aPlugExtensions.end(); j != end && !bAlreadyExist; ++j )
                    {
                        bAlreadyExist = (j->Search( aStrPlugExtension ) != STRING_NOTFOUND );
                    }

                    if ( !bAlreadyExist )
                    {
                        // filterdescription already there?
                        // (then append the new extension to the existing filter)
                        int nfound = -1;
                         for ( j = aPlugNames.begin(),
                                  k = aPlugExtensions.begin(),
                                  end = aPlugNames.end();
                              j != end && nfound != 0;  )
                        {
                            if ( ( nfound = j->Search( aStrPlugName ) ) == 0 )
                            {
                                if ( aStrPlugExtension.Len() > 0 )
                                    aStrPlugExtension.Insert( sal_Unicode( ';' ) );
                                aStrPlugExtension.Insert( *k );

                                // remove old entry, increment (iterators are invalid thereafter, thus the postincrement)
                                aPlugNames.erase(j++); aPlugExtensions.erase(k++);

                                // update end iterator (which may be invalid, too!)
                                end = aPlugNames.end();
                            }
                            else
                            {
                                // next element
                                ++j; ++k;
                            }
                        }

                        // build filterdescription
                        aStrPlugName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "  (" ) );
                        aStrPlugName.Append( aStrPlugExtension );
                        aStrPlugName.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ")" ) );

                        // use a own description for the video-formate avi, mov and mpeg
                        // the descriptions of these MIME-types are not very meaningful
                        const sal_Char sAVI[] = "*.avi";
                        const sal_Char sMOV[] = "*.mov";
                        const sal_Char sMPG[] = "*.mpg";
                        const sal_Char sMPE[] = "*.mpe";
                        const sal_Char sMPEG[] = "*.mpeg";

                        if ( aStrPlugExtension.EqualsIgnoreCaseAscii( sAVI ) )
                            aStrPlugName = SVX_RESSTR( STR_INSERT_VIDEO_EXTFILTER_AVI );
                        else if ( aStrPlugExtension.EqualsIgnoreCaseAscii( sMOV ) )
                            aStrPlugName = SVX_RESSTR( STR_INSERT_VIDEO_EXTFILTER_MOV );
                        else if ( aStrPlugExtension.SearchAscii( sMPG ) != STRING_NOTFOUND ||
                                  aStrPlugExtension.SearchAscii( sMPE ) != STRING_NOTFOUND ||
                                  aStrPlugExtension.SearchAscii( sMPEG ) != STRING_NOTFOUND )
                            aStrPlugName = SVX_RESSTR(STR_INSERT_VIDEO_EXTFILTER_MPEG);

                        aPlugNames.push_back( aStrPlugName );
                        aPlugExtensions.push_back( aStrPlugExtension );
                    }
                }
            }

            // add filter to dialog
            for ( j = aPlugNames.begin(),
                      k = aPlugExtensions.begin(),
                      end = aPlugNames.end();
                  j != end; ++j, ++k )
            {
                maFileDlg.AddFilter( *j, *k );
            }
        }
    }

    // add the All-Filter
    String aAllFilter( ResId( STR_EXTFILTER_ALL, DIALOG_MGR() ) );
    maFileDlg.AddFilter( aAllFilter, UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "*.*" ) ) );

    // and activate him
    maFileDlg.SetCurrentFilter( aAllFilter );
}

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

SvxPluginFileDlg::~SvxPluginFileDlg()
{
}

/*************************************************************************
|*
|* Plugins available for the the MIME-Typ in nKind
|* (whith nKind = SID_INSERT_SOUND for MIME-Type audio
|*                SID_INSERT_VIDEO for MIME-Type video
|*
\************************************************************************/

#define PFDLG_CHECKED_SOUND     0x0001
#define PFDLG_CHECKED_VIDEO     0x0002
#define PFDLG_FOUND_SOUND       0x0004
#define PFDLG_FOUND_VIDEO       0x0008

sal_Bool SvxPluginFileDlg::IsAvailable (sal_uInt16 nKind)
{
    static sal_uInt16 nCheck = 0;

    if ( nKind == SID_INSERT_SOUND && ( nCheck & PFDLG_CHECKED_SOUND ) )
        return (nCheck & PFDLG_FOUND_SOUND);
    if ( nKind == SID_INSERT_VIDEO && ( nCheck & PFDLG_CHECKED_VIDEO ) )
        return (nCheck & PFDLG_FOUND_VIDEO);

    sal_Bool bFound=sal_False;
    uno::Reference< lang::XMultiServiceFactory >  xMgr( ::comphelper::getProcessServiceFactory() );

    if( xMgr.is() )
    {
        uno::Reference< plugin::XPluginManager >  rPluginManager = uno::Reference< plugin::XPluginManager > ( xMgr->createInstance( OUString::createFromAscii( "com.sun.star.plugin.PluginManager" ) ), uno::UNO_QUERY );
        if( rPluginManager.is() )
        {
            const uno::Sequence<plugin::PluginDescription > aSeq( rPluginManager->getPluginDescriptions() );
            const plugin::PluginDescription* pDescription = aSeq.getConstArray();
            sal_Int32 nAnzahlPlugins = rPluginManager->getPluginDescriptions().getLength();

            for ( sal_uInt16 i = 0; i < nAnzahlPlugins && !bFound; ++i )
            {
                String aStrPlugMIMEType( pDescription[i].Mimetype );
                switch (nKind)
                {
                    case SID_INSERT_SOUND :
                    {
                        nCheck |= PFDLG_CHECKED_SOUND;

                        if( aStrPlugMIMEType.SearchAscii( sAudio ) == 0 )
                        {
                            bFound=sal_True;
                            nCheck |= PFDLG_FOUND_SOUND;
                        }
                    }
                    break;
                    case SID_INSERT_VIDEO :
                    {
                        nCheck |= PFDLG_CHECKED_VIDEO;

                        if (aStrPlugMIMEType.SearchAscii( sVideo ) == 0)
                        {
                            bFound=sal_True;
                            nCheck |= PFDLG_FOUND_VIDEO;
                        }
                    }
                    break;
                }
            }
        }
    }

    return bFound;
}

void SvxPluginFileDlg::SetDialogHelpId( const sal_Int32 _nHelpId )
{
    maFileDlg.SetDialogHelpId( _nHelpId );
}

void SvxPluginFileDlg::SetContext( sfx2::FileDialogHelper::Context _eNewContext )
{
    maFileDlg.SetContext( _eNewContext );
}
