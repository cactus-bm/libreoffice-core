/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: embeddedobjectcontainer.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-10-19 12:47:36 $
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
#ifndef _COMPHELPER_OBJECTCONTAINER_HXX_
#define _COMPHELPER_OBJECTCONTAINER_HXX_

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_EMBED_XEMBEDDEDOBJECT_HPP_
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_XSTORAGE_HPP_
#include <com/sun/star/embed/XStorage.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStrem.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include "comphelper/comphelperdllapi.h"
#endif

#include <rtl/ustring.hxx>

namespace comphelper
{

struct EmbedImpl;
class COMPHELPER_DLLPUBLIC EmbeddedObjectContainer
{
    EmbedImpl*  pImpl;

public:
    // add an embedded object to the container storage
    sal_Bool            StoreEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, ::rtl::OUString&, sal_Bool );

    // add an embedded object that has been imported from the container storage - should only be called by filters!
    void                AddEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, const ::rtl::OUString& );

                        EmbeddedObjectContainer();
                        EmbeddedObjectContainer( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& );
                        ~EmbeddedObjectContainer();

    void                SwitchPersistence( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& );
    sal_Bool            CommitImageSubStorage();
    void                ReleaseImageSubStorage();

    ::rtl::OUString     CreateUniqueObjectName();

    // get a list of object names that have been added so far
    com::sun::star::uno::Sequence < ::rtl::OUString > GetObjectNames();

    // check for existence of objects at all
    sal_Bool            HasEmbeddedObjects();

    // check existence of an object - either by identity or by name
    sal_Bool            HasEmbeddedObject( const ::rtl::OUString& );
    sal_Bool            HasEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& );

    // get the object name of an object - this is the persist name if the object has persistence
    ::rtl::OUString     GetEmbeddedObjectName( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& );

    // retrieve an embedded object by name that either has been added already or is available in the container storage
    ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject > GetEmbeddedObject( const ::rtl::OUString& );

    // create an object from a ClassId
    ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >
                        CreateEmbeddedObject( const com::sun::star::uno::Sequence < sal_Int8 >&, ::rtl::OUString& );

    // insert an embedded object into the container - objects persistant representation will be added to the storage
    sal_Bool            InsertEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, ::rtl::OUString& );

    // load an embedded object from a MediaDescriptor and insert it into the container
    // a new object will be created from the new content and returned
    ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >
                        InsertEmbeddedObject( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, ::rtl::OUString& );

    // create an embedded link based on a MediaDescriptor and insert it into the container
    // a new object will be created from the new content and returned
    ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >
                        InsertEmbeddedLink( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >&, ::rtl::OUString& );

    // create an object from a stream that contains its persistent representation and insert it as usual (usually called from clipboard)
    // a new object will be created from the new content and returned
    ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >
                        InsertEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::io::XInputStream >&, ::rtl::OUString& );

    // copy an embedded object into the storage
    sal_Bool CopyEmbeddedObject( EmbeddedObjectContainer& rSrc, const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, ::rtl::OUString& );

    // copy an embedded object into the storage, open the new copy and return it
    ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject > CopyAndGetEmbeddedObject( EmbeddedObjectContainer& rSrc, const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& xObj, /* TODO const ::rtl::OUString& aOrigName,*/ ::rtl::OUString& rName );

    // move an embedded object from one container to another one
    sal_Bool MoveEmbeddedObject( EmbeddedObjectContainer& rSrc, const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, ::rtl::OUString& );

    // remove an embedded object from the container and from the storage; if object can't be closed
    sal_Bool            RemoveEmbeddedObject( const ::rtl::OUString& rName, sal_Bool bClose=sal_True );
    sal_Bool            RemoveEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, sal_Bool bClose=sal_True );

    // close and remove an embedded object from the container without removing it from the storage
    sal_Bool            CloseEmbeddedObject( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& );

    // move an embedded object to another container (keep the persistent name)
    sal_Bool            MoveEmbeddedObject( const ::rtl::OUString& rName, EmbeddedObjectContainer& );

    // get the stored graphical representation for the object
    com::sun::star::uno::Reference < com::sun::star::io::XInputStream > GetGraphicStream( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&, ::rtl::OUString* pMediaType=0 );

    // get the stored graphical representation by the object name
    com::sun::star::uno::Reference < com::sun::star::io::XInputStream > GetGraphicStream( const ::rtl::OUString& aName, ::rtl::OUString* pMediaType=0 );

    // add a graphical representation for an object
    sal_Bool            InsertGraphicStream( const com::sun::star::uno::Reference < com::sun::star::io::XInputStream >& rStream, const ::rtl::OUString& rObjectName, const ::rtl::OUString& rMediaType );

    // try to add a graphical representation for an object in optimized way ( might fail )
    sal_Bool            InsertGraphicStreamDirectly( const com::sun::star::uno::Reference < com::sun::star::io::XInputStream >& rStream, const ::rtl::OUString& rObjectName, const rtl::OUString& rMediaType );

    // remove a graphical representation for an object
    sal_Bool            RemoveGraphicStream( const ::rtl::OUString& rObjectName );

    // copy the graphical representation from different container
    sal_Bool            TryToCopyGraphReplacement( EmbeddedObjectContainer& rSrc,
                                                    const ::rtl::OUString& aOrigName,
                                                    const ::rtl::OUString& aTargetName );

    void                CloseEmbeddedObjects();
};

};

#endif

