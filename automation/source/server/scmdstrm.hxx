/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: scmdstrm.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: hr $ $Date: 2006-06-20 00:24:09 $
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
#ifndef _SCMDSTRM_HXX
#define _SCMDSTRM_HXX

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#include "cmdbasestream.hxx"

class SvStream;
class SfxPoolItem;
class String;
class ICommStream;

class SCmdStream: public CmdBaseStream
{
    SvStream *pSammel;

public:
    SCmdStream( SvStream *pIn );
    ~SCmdStream();

    using CmdBaseStream::Read;
    void Read ( comm_USHORT &nNr ){CmdBaseStream::Read ( nNr );}
    void Read ( comm_ULONG &nNr ){CmdBaseStream::Read ( nNr );}
//  void Read ( comm_UniChar* &aString, comm_USHORT &nLenInChars ){CmdBaseStream::Read ( aString, nLenInChars );}
    void Read ( comm_BOOL &bBool ){CmdBaseStream::Read ( bBool );}
//  new
    void Read ( String &aString );
    void Read ( SfxPoolItem *&pItem );
    void Read ( ::com::sun::star::beans::PropertyValue &rItem );

    virtual void Read (String* &pString);
};

#endif
