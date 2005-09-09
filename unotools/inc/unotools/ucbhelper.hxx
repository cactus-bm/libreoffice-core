/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ucbhelper.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 09:39:22 $
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
#ifndef INCLUDED_UNOTOOLSDLLAPI_H
#include "unotools/unotoolsdllapi.h"
#endif

#ifndef _UNOTOOLS_UCBHELPER_HXX
#define _UNOTOOLS_UCBHELPER_HXX

// include ---------------------------------------------------------------

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef  _COM_SUN_STAR_UCB_NAMECLASH_HPP_
#include <com/sun/star/ucb/NameClash.hpp>
#endif

#include <tools/string.hxx>

namespace ucb
{
    class Content;
};

namespace utl
{
    class UNOTOOLS_DLLPUBLIC UCBContentHelper
    {
    private:
        static sal_Bool             Transfer_Impl( const String& rSource, const String& rDest, sal_Bool bMoveData,
                                                        sal_Int32 nNameClash );

    public:
        static sal_Bool             IsDocument( const String& rContent );
        static sal_Bool             IsFolder( const String& rContent );
        static sal_Bool             GetTitle( const String& rContent, String& rTitle );
        static sal_Bool             Kill( const String& rContent );

        static ::com::sun::star::uno::Any GetProperty( const String& rURL, const ::rtl::OUString& rName );

        static ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    GetFolderContents( const String& rFolder, sal_Bool bFolder, sal_Bool bSorted = sal_False );
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > GetResultSet( const String& rURL );

        static sal_Bool             CopyTo( const String& rSource, const String& rDest );
        static sal_Bool             MoveTo( const String& rSource, const String& rDest, sal_Int32 nNameClash = com::sun::star::ucb::NameClash::ERROR );

        static sal_Bool             CanMakeFolder( const String& rFolder );
        static sal_Bool             MakeFolder( const String& rFolder, sal_Bool bNewOnly = sal_False );
        static sal_Bool             MakeFolder( ::ucb::Content& rParent,
                                                const String& rTitle,
                                                ::ucb::Content& rNewFolder,
                                                sal_Bool bNewOnly = sal_False );
        static sal_Bool             HasParentFolder( const String& rFolder );

        static ULONG                GetSize( const String& rContent );
        static sal_Bool             IsYounger( const String& rIsYoung, const String& rIsOlder );

        static sal_Bool             Exists( const String& rContent );
        static sal_Bool             FindInPath( const String& rPath, const String& rName, String& rFile, char cDelim = ';', BOOL bAllowWildCards = TRUE );
        static sal_Bool             Find( const String& rFolder, const String& rName, String& rFile, BOOL bAllowWildCards = FALSE );
    };
};

#endif


