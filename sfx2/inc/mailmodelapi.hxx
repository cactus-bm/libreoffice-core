/*************************************************************************
 *
 *  $RCSfile: mailmodelapi.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2005-01-21 17:32:38 $
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
#ifndef INCLUDED_SFX_MAILMODELAPI_HXX
#define INCLUDED_SFX_MAILMODELAPI_HXX

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
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
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > m_xCurrentFrame;

    String              maFromAddress;
    String              maSubject;
    MailPriority        mePriority;

    sal_Bool            mbLoadDone;

    void                ClearList( AddressList_Impl* pList );
    void                MakeValueList( AddressList_Impl* pList, String& rValueList );
    SaveResult          SaveDocument( const ::rtl::OUString& _sAttachmentTitle
                                    , const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& _xModel
                                    , String& rFileName);
    SaveResult          SaveDocAsPDF( const ::rtl::OUString& _sAttachmentTitle
                                    , const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& _xModel
                                    , String& rFileName);

    DECL_LINK( DoneHdl, void* );

public:
    enum SendMailResult
    {
        SEND_MAIL_OK,
        SEND_MAIL_CANCELLED,
        SEND_MAIL_ERROR
    };

    SfxMailModel( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& _xFrame);
    ~SfxMailModel();

    void                AddAddress( const String& rAddress, AddressRole eRole );
    void                SetFromAddress( const String& rAddress )            { maFromAddress = rAddress; }
    void                SetSubject( const String& rSubject )                { maSubject = rSubject; }
    void                SetPriority( MailPriority ePrio )                   { mePriority = ePrio; }

    /** attaches a document to the current attachment list, can be called more than once.
    *   at the moment there will be a dialog for export executed for every model which is going to be attached.
    *
    * \param _eMailDocType
        The doc type to export. PDF will be at the moment only a direct export (no dialog).
    * \param _xModel
        The current model to attach
    * \param _sAttachmentTitle
        The title which will be used as attachment title
    * \return @see error code
    */
    SendMailResult      AttachDocument(   MailDocType _eMailDocType
                                        , const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& _xModel
                                        , const ::rtl::OUString& _sAttachmentTitle = ::rtl::OUString());

    sal_Int32           GetCount() const { return maAttachedDocuments.size();  }
    sal_Bool            IsEmpty() const  { return maAttachedDocuments.empty(); }

    SendMailResult      Send( );
};

#endif // INCLUDED_SFX_MAILMODELAPI_HXX


