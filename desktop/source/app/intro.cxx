/*************************************************************************
 *
 *  $RCSfile: intro.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: cd $ $Date: 2001-07-06 15:37:38 $
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

#pragma hdrstop

#include "intro.hxx"

#include <tools/stream.hxx>
#include <tools/urlobj.hxx>
#include <unotools/bootstrap.hxx>
#include <vos/process.hxx>
#include <sfx2/sfxuno.hxx>
#include <sfx2/sfx.hrc>


// -----------------------------------------------------------------------

void IntroWindow_Impl::Init()
{
    Size aSize = aIntroBmp.GetSizePixel();
    SetOutputSizePixel( aSize );
    Size  aScreenSize( GetDesktopRectPixel().GetSize() );
    Size  aWinSize( GetSizePixel() );
    Point aWinPos( ( aScreenSize.Width()  - aWinSize.Width() )  / 2,
                   ( aScreenSize.Height() - aWinSize.Height() ) / 2  );
    SetPosPixel( aWinPos );

    if ( GetColorCount() >= 16 )
    {
        Show();
        Update();
    }
}

// -----------------------------------------------------------------------

IntroWindow_Impl::IntroWindow_Impl( const ResId& aBitmapResId ) :
    WorkWindow( NULL, (WinBits)0 ),
    aIntroBmp()
{
    Hide();

    String          aBmpFileName;
    ::rtl::OUString aProductKey;
    ::rtl::OUString aIniPath;
    ::rtl::OUString aLogo;

    // load bitmap depends on productname ("StarOffice", "StarSuite",...)
    ::utl::BootstrapRetVal nRetVal = ::utl::bootstrap_getProductKeyAndLogo( aProductKey, aLogo, aIniPath );
    sal_Bool        bLogo   = (sal_Bool)aLogo.toInt32();
    if ( nRetVal == ::utl::BOOTSTRAP_OK && bLogo )
    {
        xub_StrLen nIndex = 0;

        aBmpFileName = aProductKey;
        aBmpFileName = aBmpFileName.GetToken( 0, (sal_Unicode)' ', nIndex );
        aBmpFileName += String( DEFINE_CONST_UNICODE("_intro.bmp") );

        ::rtl::OUString aExecutePath;
        ::vos::OStartupInfo().getExecutableFile( aExecutePath );
        sal_uInt32  lastIndex = aExecutePath.lastIndexOf('/');
        if ( lastIndex > 0 )
            aExecutePath = aExecutePath.copy( 0, lastIndex+1 );

        INetURLObject aObj( aExecutePath, INET_PROT_FILE );
        aObj.insertName( aBmpFileName );
        SvFileStream aStrm( aObj.PathToFileName(), STREAM_STD_READ );
        if ( !aStrm.GetError() )
        {
            aStrm >> aIntroBmp;
        }
        else
        {
            aIntroBmp = Bitmap( aBitmapResId );
        }

        Init();
    }
}

// -----------------------------------------------------------------------

IntroWindow_Impl::~IntroWindow_Impl()
{
    Hide();
}

// -----------------------------------------------------------------------

void IntroWindow_Impl::Paint( const Rectangle& )
{
    DrawBitmap( Point(), aIntroBmp );
    Flush();
}

// -----------------------------------------------------------------------

void IntroWindow_Impl::Slide()
{
}
