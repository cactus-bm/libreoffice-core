/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sdiocmpt.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 03:00:25 $
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

#ifndef _SD_SDIOCMPT_HXX
#define _SD_SDIOCMPT_HXX

//BFS02#ifndef _SVDIO_HXX //autogen
//BFS02#include <svx/svdio.hxx>
//BFS02#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

//BFS02
//////////////////////////////////////////////////////////////////////////////
class SvStream;

class old_SdrDownCompat
{
protected:
    SvStream&                   rStream;
    UINT32                      nSubRecSiz;
    UINT32                      nSubRecPos;
    UINT16                      nMode;
    BOOL                        bOpen;

protected:
    void Read();
    void Write();

public:
    old_SdrDownCompat(SvStream& rNewStream, UINT16 nNewMode);
    ~old_SdrDownCompat();
    void  OpenSubRecord();
    void  CloseSubRecord();
};
//////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_SDDLLAPI_H
#include "sddllapi.h"
#endif

#define SDIOCOMPAT_VERSIONDONTKNOW (UINT16)0xffff

class SD_DLLPUBLIC SdIOCompat : public old_SdrDownCompat
{
private:
    UINT16 nVersion;

public:
                // nNewMode: STREAM_READ oder STREAM_WRITE
                // nVer:     nur beim Schreiben angeben
            SdIOCompat(SvStream& rNewStream, USHORT nNewMode,
                       UINT16 nVer = SDIOCOMPAT_VERSIONDONTKNOW);
            ~SdIOCompat();
    UINT16  GetVersion() const { return nVersion; }
};

#endif      // _SD_SDIOCMPT_HXX


