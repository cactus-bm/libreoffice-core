/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "automation/stsdllapi.h"

#include <tools/stream.hxx>

#include <automation/commtypes.hxx>
#include "icommstream.hxx"


class STS_DLLPUBLIC SvCommStream : public ICommStream
{
protected:
    SvStream* pStream;
public:

    SvCommStream( SvStream* pIO );
    ~SvCommStream();

    ICommStream&        operator>>( comm_UINT16& rUShort );
    ICommStream&        operator>>( comm_UINT32& rULong );
    ICommStream&        operator>>( comm_BOOL& rChar );

    ICommStream&        operator<<( comm_UINT16 nUShort );
    ICommStream&        operator<<( comm_UINT32 nULong );
    ICommStream&        operator<<( comm_BOOL nChar );

    comm_UINT32     Read( void* pData, comm_UINT32 nSize );
    comm_UINT32     Write( const void* pData, comm_UINT32 nSize );

    comm_BOOL       IsEof() const;
    comm_UINT32     SeekRel( long nPos );
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
