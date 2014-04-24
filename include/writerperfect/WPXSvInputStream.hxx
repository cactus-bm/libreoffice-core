/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_WRITERPERFECT_WPXSVINPUTSTREAM_HXX
#define INCLUDED_WRITERPERFECT_WPXSVINPUTSTREAM_HXX

#include <libwpd-stream/libwpd-stream.h>

#include <com/sun/star/uno/Reference.h>

#include <writerperfect/writerperfectdllapi.h>

namespace com { namespace sun { namespace star { namespace io {
    class XInputStream;
    class XSeekable;
} } } }

namespace writerperfect
{

class WPXSvInputStreamImpl;

class WRITERPERFECT_DLLPUBLIC WPXSvInputStream : public WPXInputStream
{
public:
    WPXSvInputStream( ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > xStream );
    virtual ~WPXSvInputStream();

    virtual bool isStructured();
    virtual unsigned subStreamCount();
    virtual const char * subStreamName(unsigned id);
    virtual bool existsSubStream(const char *name);
    virtual WPXInputStream * getSubStreamByName(const char *name);
    virtual WPXInputStream * getSubStreamById(unsigned id);

    virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead) SAL_OVERRIDE;
    virtual int seek(long offset, WPX_SEEK_TYPE seekType) SAL_OVERRIDE;
    virtual long tell() SAL_OVERRIDE;
    virtual bool isEnd();

    virtual bool isOLEStream() SAL_OVERRIDE;
    virtual WPXInputStream * getDocumentOLEStream(const char *name) SAL_OVERRIDE;
    virtual bool atEOS() SAL_OVERRIDE;

private:
    WPXSvInputStreamImpl *mpImpl;
};

}

#endif // INCLUDED_WRITERPERFECT_WPXSVINPUTSTREAM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
