/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: protocols.h,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 23:59:52 $
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

/*TODO outline this implementation :-) */

#ifndef __FRAMEWORK_PROTOCOLS_H_
#define __FRAMEWORK_PROTOCOLS_H_

//_________________________________________________________________________________________________________________
//  includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_MACROS_GENERIC_HXX_
#include <macros/generic.hxx>
#endif

//_________________________________________________________________________________________________________________
//  namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_______________________________________________________________________
/**
    some protocols must be checked during loading or dispatching URLs manually
    It can be neccessary to decide, if a URL represent a non visible content or
    a real visible component.
 */

#define SPECIALPROTOCOL_PRIVATE           DECLARE_ASCII("private:"       )       // indicates a loadable content in general!
#define SPECIALPROTOCOL_PRIVATE_OBJECT    DECLARE_ASCII("private:object" )       // indicates loading of components using a model directly
#define SPECIALPROTOCOL_PRIVATE_STREAM    DECLARE_ASCII("private:stream" )       // indicates loading of components using a stream only
#define SPECIALPROTOCOL_PRIVATE_FACTORY   DECLARE_ASCII("private:factory")       // indicates creation of empty documents
#define SPECIALPROTOCOL_SLOT              DECLARE_ASCII("slot:"          )       // internal protocol of the sfx project for generic dispatch funtionality
#define SPECIALPROTOCOL_UNO               DECLARE_ASCII(".uno:"          )       // external representation of the slot protocol using names instead of id's
#define SPECIALPROTOCOL_MACRO             DECLARE_ASCII("macro:"         )       // special sfx protocol to execute macros
#define SPECIALPROTOCOL_SERVICE           DECLARE_ASCII("service:"       )       // generic way to start uno services during dispatch
#define SPECIALPROTOCOL_MAILTO            DECLARE_ASCII("mailto:"        )       // for sending mails
#define SPECIALPROTOCOL_NEWS              DECLARE_ASCII("news:"          )       // for sending news

class ProtocolCheck
{
    public:

    //_______________________________________________________________________
    /**
        enums for well known protocols
     */
    enum EProtocol
    {
        E_UNKNOWN_PROTOCOL  ,
        E_PRIVATE           ,
        E_PRIVATE_OBJECT    ,
        E_PRIVATE_STREAM    ,
        E_PRIVATE_FACTORY   ,
        E_SLOT              ,
        E_UNO               ,
        E_MACRO             ,
        E_SERVICE           ,
        E_MAILTO            ,
        E_NEWS
    };

    //_______________________________________________________________________
    /**
        it checks, if the given URL string match one of the well known protocols.
        It returns the right enum value.
        Protocols are defined above ...
     */
    static EProtocol specifyProtocol( const ::rtl::OUString& sURL )
    {
        // because "private:" is part of e.g. "private:object" too ...
        // we must check it before all other ones!!!
        if (sURL.compareTo(SPECIALPROTOCOL_PRIVATE,SPECIALPROTOCOL_PRIVATE.getLength()) == 0)
            return E_PRIVATE;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_PRIVATE_OBJECT,SPECIALPROTOCOL_PRIVATE_OBJECT.getLength()) == 0)
            return E_PRIVATE_OBJECT;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_PRIVATE_STREAM,SPECIALPROTOCOL_PRIVATE_STREAM.getLength()) == 0)
            return E_PRIVATE_STREAM;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_PRIVATE_FACTORY,SPECIALPROTOCOL_PRIVATE_FACTORY.getLength()) == 0)
            return E_PRIVATE_FACTORY;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_SLOT,SPECIALPROTOCOL_SLOT.getLength()) == 0)
            return E_SLOT;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_UNO,SPECIALPROTOCOL_UNO.getLength()) == 0)
            return E_UNO;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_MACRO,SPECIALPROTOCOL_MACRO.getLength()) == 0)
            return E_MACRO;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_SERVICE,SPECIALPROTOCOL_SERVICE.getLength()) == 0)
            return E_SERVICE;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_MAILTO,SPECIALPROTOCOL_MAILTO.getLength()) == 0)
            return E_MAILTO;
        else
        if (sURL.compareTo(SPECIALPROTOCOL_NEWS,SPECIALPROTOCOL_NEWS.getLength()) == 0)
            return E_NEWS;
        else
            return E_UNKNOWN_PROTOCOL;
    }

    //_______________________________________________________________________
    /**
        it checks if given URL match the required protocol only
        It should be used instead of specifyProtocol() if only this question
        is interesting to perform the code. We must not check for all possible protocols here...
     */
    static sal_Bool isProtocol( const ::rtl::OUString& sURL, EProtocol eRequired )
    {
        switch(eRequired)
        {
            case E_PRIVATE           : return (sURL.compareTo(SPECIALPROTOCOL_PRIVATE        ,SPECIALPROTOCOL_PRIVATE.getLength()        ) == 0); break;
            case E_PRIVATE_OBJECT    : return (sURL.compareTo(SPECIALPROTOCOL_PRIVATE_OBJECT ,SPECIALPROTOCOL_PRIVATE_OBJECT.getLength() ) == 0); break;
            case E_PRIVATE_STREAM    : return (sURL.compareTo(SPECIALPROTOCOL_PRIVATE_STREAM ,SPECIALPROTOCOL_PRIVATE_STREAM.getLength() ) == 0); break;
            case E_PRIVATE_FACTORY   : return (sURL.compareTo(SPECIALPROTOCOL_PRIVATE_FACTORY,SPECIALPROTOCOL_PRIVATE_FACTORY.getLength()) == 0); break;
            case E_SLOT              : return (sURL.compareTo(SPECIALPROTOCOL_SLOT           ,SPECIALPROTOCOL_SLOT.getLength()           ) == 0); break;
            case E_UNO               : return (sURL.compareTo(SPECIALPROTOCOL_UNO            ,SPECIALPROTOCOL_UNO.getLength()            ) == 0); break;
            case E_MACRO             : return (sURL.compareTo(SPECIALPROTOCOL_MACRO          ,SPECIALPROTOCOL_MACRO.getLength()          ) == 0); break;
            case E_SERVICE           : return (sURL.compareTo(SPECIALPROTOCOL_SERVICE        ,SPECIALPROTOCOL_SERVICE.getLength()        ) == 0); break;
            case E_MAILTO            : return (sURL.compareTo(SPECIALPROTOCOL_MAILTO         ,SPECIALPROTOCOL_MAILTO.getLength()         ) == 0); break;
            case E_NEWS              : return (sURL.compareTo(SPECIALPROTOCOL_NEWS           ,SPECIALPROTOCOL_NEWS.getLength()           ) == 0); break;
        }
        return sal_False;
    }
};

} // namespace framework

#endif // #ifndef __FRAMEWORK_PROTOCOLS_H_
