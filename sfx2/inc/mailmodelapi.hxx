/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mailmodelapi.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: obo $ $Date: 2006-03-27 09:29:51 $
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
#ifndef INCLUDED_SFX_MAILMODEL_HXX
#define INCLUDED_SFX_MAILMODEL_HXX

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef INCLUDED_SFX2_DLLAPI_H
#include "sfx2/dllapi.h"
#endif

// class SfxMailModel_Impl -----------------------------------------------

class AddressList_Impl;

class SFX2_DLLPUBLIC SfxMailModel
{
public:
    enum MailPriority
    {
        PRIO_HIGHEST,
        PRIO_HIGH,
        PRIO_NORMAL,
        PRIO_LOW,
        PRIO_LOWEST
    };

    enum AddressRole
    {
        ROLE_TO,
        ROLE_CC,
        ROLE_BCC
    };

    enum MailDocType
    {
        TYPE_SELF,
        TYPE_ASPDF
    };

private:
    enum SaveResult
    {
        SAVE_SUCCESSFULL,
        SAVE_CANCELLED,
        SAVE_ERROR
    };

    ::std::vector< ::rtl::OUString > maAttachedDocuments;
    AddressList_Impl*   mpToList;
    AddressList_Impl*   mpCcList;
    AddressList_Impl*   mpBccList;
    String              maFromAddress;
    String              maSubject;
    MailPriority        mePriority;

    sal_Bool            mbLoadDone;

    void                ClearList( AddressList_Impl* pList );
    void                MakeValueList( AddressList_Impl* pList, String& rValueList );
    SaveResult          SaveDocumentAsFormat( const rtl::OUString& aSaveFileName,
                                              const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xFrameOrModel,
                                              const rtl::OUString& rType,
                                              rtl::OUString& rFileNamePath );

    DECL_LINK( DoneHdl, void* );

public:
    enum SendMailResult
    {
        SEND_MAIL_OK,
        SEND_MAIL_CANCELLED,
        SEND_MAIL_ERROR
    };

    SfxMailModel();
    ~SfxMailModel();

    void                AddAddress( const String& rAddress, AddressRole eRole );
    void                SetFromAddress( const String& rAddress )    { maFromAddress = rAddress; }
    void                SetSubject( const String& rSubject )        { maSubject = rSubject; }
    void                SetPriority( MailPriority ePrio )           { mePriority = ePrio; }

    /** attaches a document to the current attachment list, can be called more than once.
    *   at the moment there will be a dialog for export executed for every model which is going to be attached.
    *
    * \param sDocumentType
        The doc type to export. PDF will be at the moment only a direct export (no dialog).
    * \param xModel
        The current model to attach
    * \param sAttachmentTitle
        The title which will be used as attachment title
    * \return @see error code
    */
    SendMailResult      AttachDocument( const ::rtl::OUString& sDocumentType,
                                        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xFrameOrModel,
                                        const ::rtl::OUString& sAttachmentTitle );

    SendMailResult      SaveAndSend( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame,
                                     const rtl::OUString& rType );
    SendMailResult      Send( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame );

    sal_Int32           GetCount() const;
    sal_Bool            IsEmpty() const;
};

BOOL CreateFromAddress_Impl( String& rFrom );

#endif // INCLUDED_SFX_MAILMODEL_HXX
