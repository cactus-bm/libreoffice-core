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


#ifdef PRECOMPILED_HEADERS
#include <sal/config.h>
#include "scitems.hxx"

#include <algorithm>
#include <assert.h>
#include <deque>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <iosfwd>
#include <limits.h>
#include <limits>
#include <list>
#include <math.h>
#include <memory>
#include <new>
#include <cfloat>

#include <boost/bind.hpp>

#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b3dpolygon.hxx>
#include <basegfx/polygon/b3dpolypolygon.hxx>
#include <com/sun/star/uno/Any.h>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Type.hxx>
#include <cppu/macros.hxx>
#include <cppuhelper/weakref.hxx>
#include <cstddef>
#include <cwchar>
#include <float.h>
#include <functional>
#include <com/sun/star/awt/Point.hdl>
#include <com/sun/star/awt/Point.hpp>
#include <com/sun/star/awt/Size.hdl>
#include <com/sun/star/awt/Size.hpp>
#include <com/sun/star/beans/PropertyVetoException.hdl>
#include <com/sun/star/beans/PropertyVetoException.hpp>
#include <com/sun/star/container/ElementExistException.hdl>
#include <com/sun/star/container/ElementExistException.hpp>
#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/container/XElementAccess.hdl>
#include <com/sun/star/container/XElementAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/datatransfer/DataFlavor.hdl>
#include <com/sun/star/datatransfer/dnd/DragGestureEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DragSourceDragEvent.hdl>
#include <com/sun/star/datatransfer/dnd/DragSourceDragEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DragSourceDropEvent.hdl>
#include <com/sun/star/datatransfer/dnd/DragSourceEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetDragEnterEvent.hdl>
#include <com/sun/star/datatransfer/dnd/DropTargetDragEnterEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetDragEvent.hdl>
#include <com/sun/star/datatransfer/dnd/DropTargetDragEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetDropEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetEvent.hdl>
#include <com/sun/star/datatransfer/dnd/DropTargetEvent.hpp>
#include <com/sun/star/datatransfer/dnd/XDragGestureListener.hdl>
#include <com/sun/star/datatransfer/dnd/XDragGestureListener.hpp>
#include <com/sun/star/datatransfer/dnd/XDragSource.hdl>
#include <com/sun/star/datatransfer/dnd/XDragSource.hpp>
#include <com/sun/star/datatransfer/dnd/XDragSourceContext.hdl>
#include <com/sun/star/datatransfer/dnd/XDragSourceContext.hpp>
#include <com/sun/star/datatransfer/dnd/XDragSourceListener.hdl>
#include <com/sun/star/datatransfer/dnd/XDragSourceListener.hpp>
#include <com/sun/star/datatransfer/dnd/XDropTargetDragContext.hpp>
#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hdl>
#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hpp>
#include <com/sun/star/datatransfer/UnsupportedFlavorException.hdl>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/embed/InvalidStorageException.hpp>
#include <com/sun/star/embed/StorageWrappedTargetException.hdl>
#include <com/sun/star/embed/StorageWrappedTargetException.hpp>
#include <com/sun/star/embed/XStorage.hdl>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/io/BufferSizeExceededException.hpp>
#include <com/sun/star/io/IOException.hdl>
#include <com/sun/star/io/NotConnectedException.hdl>
#include <com/sun/star/io/NotConnectedException.hpp>
#include <com/sun/star/io/XInputStream.hdl>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hdl>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XStream.hdl>
#include <com/sun/star/lang/EventObject.hdl>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/lang/WrappedTargetException.hdl>
#include <com/sun/star/lang/WrappedTargetException.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/packages/NoEncryptionException.hdl>
#include <com/sun/star/packages/NoEncryptionException.hpp>
#include <com/sun/star/packages/WrongPasswordException.hdl>
#include <com/sun/star/packages/WrongPasswordException.hpp>
#include <com/sun/star/uno/Exception.hdl>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hdl>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XAdapter.hdl>
#include <com/sun/star/uno/XAdapter.hpp>
#include <com/sun/star/uno/XInterface.hdl>
#include <com/sun/star/uno/XReference.hdl>
#include <com/sun/star/uno/XReference.hpp>
#include <com/sun/star/uno/XWeak.hpp>
#include <osl/endian.h>
#include <osl/interlck.h>
#include <osl/mutex.hxx>
#include <rtl/alloc.h>
#include <rtl/string.h>
#include <rtl/ustrbuf.h>
#include <rtl/ustring.h>
#include <sal/mathconf.h>
#include <sal/types.h>
#include <sot/exchange.hxx>
#include <sot/factory.hxx>
#include <sot/storage.hxx>
#include <svl/brdcst.hxx>
#include <svl/cenumitm.hxx>
#include <svl/cintitem.hxx>
#include <unotools/fltrcfg.hxx>
#include <svl/intitem.hxx>
#include <svl/listener.hxx>
#include <svl/lstner.hxx>
#include <unotools/pathoptions.hxx>
#include <svl/solar.hrc>
#include <unotools/useroptions.hxx>
#include <editeng/editobj.hxx>
#include <editeng/eeitem.hxx>
#include <svx/fmglob.hxx>
#include <editeng/outlobj.hxx>
#include <svx/sdangitm.hxx>
#include <svx/sderitm.hxx>
#include <svx/sdmetitm.hxx>
#include <svx/sdooitm.hxx>
#include <svx/sdprcitm.hxx>
#include <svx/sdrmasterpagedescriptor.hxx>
#include <svx/sdrpageuser.hxx>
#include <svx/sdtaitm.hxx>
#include <svx/svdglue.hxx>
#include <svx/svdlayer.hxx>
#include <svx/svdoattr.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdpool.hxx>
#include <svx/svdtrans.hxx>
#include <svx/svdtypes.hxx>
#include <svx/unoapi.hxx>
#include <svx/volume3d.hxx>
#include <svx/xcolit.hxx>
#include <svx/xenum.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xflasit.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xlnasit.hxx>
#include <svx/xtextit0.hxx>
#include <tools/date.hxx>
#include <tools/datetime.hxx>
#include <tools/errcode.hxx>
#include <tools/errinf.hxx>
#include <tools/gen.hxx>
#include <tools/globname.hxx>
#include <tools/rc.hxx>
#include <tools/rtti.hxx>
#include <tools/solar.h>
#include <tools/string.hxx>
#include <tools/toolsdllapi.h>
#include <tools/weakbase.h>
#include <tools/weakbase.hxx>
#include <typeinfo>
#include <typelib/typeclass.h>
#include <typelib/typedescription.h>
#include <typelib/uik.h>
#include <uno/any2.h>
#include <uno/lbnames.h>
#include <uno/sequence2.h>
#include <unotools/ucbstreamhelper.hxx>

#include <vcl/apptypes.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/dllapi.h>
#include <vcl/dndhelp.hxx>
#include <vcl/edit.hxx>
#include <vcl/field.hxx>
#include <tools/fldunit.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/inputctx.hxx>
#include <vcl/jobset.hxx>
#include <vcl/mapmod.hxx>
#include <vcl/menu.hxx>
#include <vcl/pointr.hxx>
#include <vcl/print.hxx>
#include <vcl/prntypes.hxx>
#include <vcl/ptrstyle.hxx>
#include <vcl/region.hxx>
#include <vcl/salnativewidgets.hxx>
#include <vcl/spinfld.hxx>
#include <vcl/sv.h>
#include <vcl/svapp.hxx>
#include <vcl/vclevent.hxx>
#include <vcl/window.hxx>
#include <vcl/wintypes.hxx>
#include <wchar.h>

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
