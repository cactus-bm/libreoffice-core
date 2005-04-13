/*************************************************************************
 *
 *  $RCSfile: ucbhelper.hxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 12:28:50 $
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


