/*************************************************************************
 *
 *  $RCSfile: svxmsbas.cxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-09 09:39:50 $
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SFX_OBJSH_HXX
#include <sfx2/objsh.hxx>
#endif
#ifndef _SFXAPP_HXX
#include <sfx2/app.hxx>
#endif
#ifndef _BASMGR_HXX
#include <basic/basmgr.hxx>
#endif
#ifndef _SB_SBMOD_HXX
#include <basic/sbmod.hxx>
#endif
#ifndef _SVXERR_HXX
#include <svxerr.hxx>
#endif

#ifndef _SVXMSBAS_HXX
#include <svxmsbas.hxx>
#endif
#ifndef _MSVBASIC_HXX
#include <msvbasic.hxx>
#endif
#ifndef _MSOCXIMEX_HXX
#include <msocximex.hxx>
#endif
#ifndef _SOT_STORINFO_HXX
#include <sot/storinfo.hxx>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLMODEL_HPP_
#include <com/sun/star/awt/XControlModel.hpp>
#endif
using namespace com::sun::star::beans;
using namespace com::sun::star::io;
using namespace com::sun::star::awt;
#include <comphelper/storagehelper.hxx>

#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/script/XLibraryContainer.hpp>
using namespace com::sun::star::container;
using namespace com::sun::star::script;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star;

using rtl::OUString;

int SvxImportMSVBasic::Import( const String& rStorageName,
                                const String &rSubStorageName,
                                BOOL bAsComment, BOOL bStripped )
{
    int nRet = 0;
    if( bImport && ImportCode_Impl( rStorageName, rSubStorageName,
                                    bAsComment, bStripped ))
        nRet |= 1;

    if (bImport)
        ImportForms_Impl(rStorageName, rSubStorageName);

    if( bCopy && CopyStorage_Impl( rStorageName, rSubStorageName ))
        nRet |= 2;

    return nRet;
}

bool SvxImportMSVBasic::ImportForms_Impl(const String& rStorageName,
    const String& rSubStorageName)
{
    SvStorageRef xVBAStg(xRoot->OpenSotStorage(rStorageName,
        STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYALL));
    if (!xVBAStg.Is() || xVBAStg->GetError())
        return false;

    std::vector<String> aUserForms;
    SvStorageInfoList aContents;
    xVBAStg->FillInfoList(&aContents);
    for (USHORT nI = 0; nI < aContents.Count(); ++nI)
    {
          SvStorageInfo& rInfo = aContents.GetObject(nI);
          if (!rInfo.IsStream() && rInfo.GetName() != rSubStorageName)
              aUserForms.push_back(rInfo.GetName());
    }

    if (aUserForms.empty())
        return false;

    bool bRet = true;
    SFX_APP()->EnterBasicCall();
    try
    {
        Reference<XMultiServiceFactory> xSF(comphelper::getProcessServiceFactory());

        Reference<XComponentContext> xContext;
        Reference<XPropertySet> xProps(xSF, UNO_QUERY);
        xProps->getPropertyValue(OUString(RTL_CONSTASCII_USTRINGPARAM("DefaultContext")) ) >>= xContext;


        Reference<XLibraryContainer> xLibContainer = rDocSh.GetDialogContainer();
        DBG_ASSERT( xLibContainer.is(), "No BasicContainer!" );

        String aLibName(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("Standard")));
        Reference<XNameContainer> xLib;
        if (xLibContainer.is())
        {
            if( !xLibContainer->hasByName(aLibName))
                xLibContainer->createLibrary(aLibName);

            Any aLibAny = xLibContainer->getByName( aLibName );
            aLibAny >>= xLib;
        }

        if(xLib.is())
        {
            typedef std::vector<String>::iterator myIter;
            myIter aEnd = aUserForms.end();
            for (myIter aIter = aUserForms.begin(); aIter != aEnd; ++aIter)
            {
                SvStorageRef xForm (xVBAStg->OpenSotStorage(*aIter,
                    STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYALL));

                if (!xForm.Is() || xForm->GetError())
                    continue;

                SvStorageStreamRef xFrame = xForm->OpenSotStream(
                    String(RTL_CONSTASCII_STRINGPARAM("\3VBFrame"),
                        RTL_TEXTENCODING_MS_1252),
                    STREAM_STD_READ | STREAM_NOCREATE);

                if (!xFrame.Is() || xFrame->GetError())
                    continue;

                SvStorageStreamRef xContents = xForm->OpenSotStream(
                    String(RTL_CONSTASCII_STRINGPARAM("o"),
                        RTL_TEXTENCODING_MS_1252),
                    STREAM_STD_READ | STREAM_NOCREATE);

                if (!xContents.Is() || xContents->GetError())
                    continue;

                SvStorageStreamRef xTypes = xForm->OpenSotStream(
                    String(RTL_CONSTASCII_STRINGPARAM("f"),
                        RTL_TEXTENCODING_MS_1252),
                    STREAM_STD_READ | STREAM_NOCREATE);

                if (!xTypes.Is() || xTypes->GetError())
                    continue;

                //<UserForm Name=""><VBFrame></VBFrame>"
                String sData;
                String sLine;
                while(xFrame->ReadByteStringLine(sLine, RTL_TEXTENCODING_MS_1252))
                {
                    sData += sLine;
                    sData += '\n';
                }
                sData.ConvertLineEnd();


                OCX_UserForm aForm(*aIter);
                sal_Bool bOk = aForm.Read(xTypes);
                DBG_ASSERT(bOk, "Had unexpected content, not risking this module");
                if (bOk)
                    aForm.Import(xContents, xSF, xContext, xLib);
            }
        }
    }
    catch(...)
    {
        //bRet = false;
    }
    SFX_APP()->LeaveBasicCall();
    return bRet;
}


BOOL SvxImportMSVBasic::CopyStorage_Impl( const String& rStorageName,
                                         const String& rSubStorageName)
{
    BOOL bValidStg = FALSE;
    {
        SvStorageRef xVBAStg( xRoot->OpenSotStorage( rStorageName,
                                    STREAM_READWRITE | STREAM_NOCREATE |
                                    STREAM_SHARE_DENYALL ));
        if( xVBAStg.Is() && !xVBAStg->GetError() )
        {
            SvStorageRef xVBASubStg( xVBAStg->OpenSotStorage( rSubStorageName,
                                     STREAM_READWRITE | STREAM_NOCREATE |
                                    STREAM_SHARE_DENYALL ));
            if( xVBASubStg.Is() && !xVBASubStg->GetError() )
            {
                // then we will copy these storages into the (temporary) storage of the document
                bValidStg = TRUE;
            }
        }
    }

    if( bValidStg )
    {
        String aDstStgName( GetMSBasicStorageName() );
        SotStorageRef xDst = SotStorage::OpenOLEStorage( rDocSh.GetStorage(), aDstStgName, STREAM_READWRITE | STREAM_TRUNC );
        SotStorageRef xSrc = xRoot->OpenSotStorage( rStorageName, STREAM_STD_READ );

        // TODO/LATER: should we commit the storage?
        xSrc->CopyTo( xDst );
        xDst->Commit();
        ErrCode nError;
        if ( (nError = xDst->GetError() != ERRCODE_NONE) || (nError = xSrc->GetError() != ERRCODE_NONE) )
            xRoot->SetError( nError );
        else
            bValidStg = TRUE;
    }

    return bValidStg;
}

BOOL SvxImportMSVBasic::ImportCode_Impl( const String& rStorageName,
                                        const String &rSubStorageName,
                                        BOOL bAsComment, BOOL bStripped )
{
    BOOL bRet = FALSE;
    VBA_Impl aVBA( *xRoot, bAsComment );
    if( aVBA.Open(rStorageName,rSubStorageName) )
    {
        SFX_APP()->EnterBasicCall();
        Reference<XLibraryContainer> xLibContainer = rDocSh.GetBasicContainer();
        DBG_ASSERT( xLibContainer.is(), "No BasicContainer!" );

        UINT16 nStreamCount = aVBA.GetNoStreams();
        Reference<XNameContainer> xLib;
        if( xLibContainer.is() && nStreamCount )
        {
            String aLibName( String::CreateFromAscii(
                    RTL_CONSTASCII_STRINGPARAM( "Standard" )));
            if( !xLibContainer->hasByName( aLibName ) )
                xLibContainer->createLibrary( aLibName );

            Any aLibAny = xLibContainer->getByName( aLibName );
            aLibAny >>= xLib;
        }
        if( xLib.is() )
        {
            for( UINT16 i=0; i<nStreamCount;i++)
            {
                StringArray aDecompressed = aVBA.Decompress(i);
                ByteString sByteBasic(aVBA.GetStreamName(i),
                    RTL_TEXTENCODING_ASCII_US,
                        (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_UNDERLINE|
                        RTL_UNICODETOTEXT_FLAGS_INVALID_UNDERLINE |
                        RTL_UNICODETOTEXT_FLAGS_PRIVATE_MAPTO0 |
                        RTL_UNICODETOTEXT_FLAGS_NOCOMPOSITE)
                );

                //const String &sBasicModule = aVBA.GetStreamName( i);
                const String sBasicModule(sByteBasic,
                    RTL_TEXTENCODING_ASCII_US);
                /* #117718# expose information regarding type of Module
                * Class, Form or plain 'ould VBA module with a REM statment
                * at the top of the module. Mapping of Module Name
                * to type is performed in  VBA_Impl::Open() method,
                * ( msvbasic.cxx ) by examining the PROJECT stream.
                */

                // using name from aVBA.GetStreamName
                // because the encoding of the same returned
                // is the same as the encoding for the names
                // that are keys in the map used by GetModuleType method
                const String &sOrigVBAModName = aVBA.GetStreamName( i );
                ModuleType mType = aVBA.GetModuleType( sOrigVBAModName );

                rtl::OUString sClassRem(
                    rtl::OUString::createFromAscii("Rem Attribute VBA_ModuleType=") );
                OSL_TRACE("Type for Module %s is %d",
                    ::rtl::OUStringToOString( sOrigVBAModName, RTL_TEXTENCODING_ASCII_US ).pData->buffer, mType );

                rtl::OUString modeTypeComment;

                switch( mType )
                {
                    case Class:
                        modeTypeComment = sClassRem + rtl::OUString::createFromAscii(
                            "VBAClassModule\n" );
                        break;
                    case Form:
                        modeTypeComment = sClassRem + rtl::OUString::createFromAscii(
                            "VBAFormModule\n" );
                        break;
                    case Normal:
                        modeTypeComment = sClassRem + rtl::OUString::createFromAscii(
                            "VBAModule\n" );
                        break;
                    case Unknown:
                        modeTypeComment = sClassRem + rtl::OUString::createFromAscii(
                            "VBAUnknown\n" );
                        break;
                    default:
                        break;
                }

                for(ULONG j=0;j<aDecompressed.GetSize();j++)
                {
                    String sModule(sBasicModule);
                    if (j>0)
                    {
                        sModule.AppendAscii("_Part");
                        sModule += String::CreateFromInt32(j+1);
                    }

                    if (bStripped)
                    {
                        String *pStr = aDecompressed.Get(j);
                        bool bMac = true;
                        xub_StrLen nBegin = pStr->Search('\x0D');
                        if ((STRING_NOTFOUND != nBegin) && (pStr->Len() > 1) && (pStr->GetChar(nBegin+1) == '\x0A'))
                            bMac = false;

                        const char cLineEnd = bMac ? '\x0D' : '\x0A';
                        const String sAttribute(String::CreateFromAscii(
                            bAsComment ? "Rem Attribute" : "Attribute"));
                        while (STRING_NOTFOUND != (nBegin = pStr->Search(sAttribute)))
                        {
                            if ((nBegin) && pStr->GetChar(nBegin-1) != cLineEnd)
                                continue;
                            xub_StrLen nEnd = pStr->Search(cLineEnd ,nBegin);
                            // DR #i26521# catch STRING_NOTFOUND, will loop endless otherwise
                            if( nEnd == STRING_NOTFOUND )
                                pStr->Erase();
                            else
                                pStr->Erase(nBegin, (nEnd-nBegin)+1);
                        }
                    }

                    if( aDecompressed.Get(j)->Len() )
                    {
                        if (bAsComment)
                        {
                            String sTemp( String::CreateFromAscii(
                                    RTL_CONSTASCII_STRINGPARAM( "Sub " )));
                            String sMunge(sModule);
                            //Streams can have spaces in them, but modulenames
                            //cannot !
                            sMunge.SearchAndReplaceAll(' ','_');

                            sTemp += sMunge;
                            sTemp.AppendAscii("\n");
                            aDecompressed.Get(j)->Insert(sTemp,0);
                            aDecompressed.Get(j)->InsertAscii("\nEnd Sub");
                        }

                        ::rtl::OUString aModName( sModule );
                        ::rtl::OUString aSource( *aDecompressed.Get(j) );

                        aSource = modeTypeComment + aSource;

                        Any aSourceAny;
                        aSourceAny <<= aSource;
                        if( xLib->hasByName( aModName ) )
                            xLib->replaceByName( aModName, aSourceAny );
                        else
                            xLib->insertByName( aModName, aSourceAny );

                        bRet = true;
                    }
                }
            }
        }
        SFX_APP()->LeaveBasicCall();
    }
    return bRet;
}

ULONG SvxImportMSVBasic::SaveOrDelMSVBAStorage( BOOL bSaveInto,
                                                const String& rStorageName )
{
    ULONG nRet = ERRCODE_NONE;
    uno::Reference < embed::XStorage > xSrcRoot( rDocSh.GetStorage() );
    String aDstStgName( GetMSBasicStorageName() );
    SotStorageRef xVBAStg( SotStorage::OpenOLEStorage( xSrcRoot, aDstStgName,
                                STREAM_READWRITE | STREAM_NOCREATE | STREAM_SHARE_DENYALL ) );
    if( xVBAStg.Is() && !xVBAStg->GetError() )
    {
        xVBAStg = 0;
        if( bSaveInto )
        {
            BasicManager *pBasicMan = rDocSh.GetBasicManager();
            if( pBasicMan && pBasicMan->IsBasicModified() )
                nRet = ERRCODE_SVX_MODIFIED_VBASIC_STORAGE;

            SotStorageRef xSrc = SotStorage::OpenOLEStorage( xSrcRoot, aDstStgName, STREAM_STD_READ );
            SotStorageRef xDst = xRoot->OpenSotStorage( rStorageName, STREAM_READWRITE | STREAM_TRUNC );
            xSrc->CopyTo( xDst );
            xDst->Commit();
            ErrCode nError;
            if ( (nError = xDst->GetError() != ERRCODE_NONE) || (nError = xSrc->GetError() != ERRCODE_NONE) )
                xRoot->SetError( nError );
        }
    }

    return nRet;
}

// check if the MS-VBA-Storage exists in the RootStorage of the DocShell.
// If it exists, then return the WarningId for losing the information.
ULONG SvxImportMSVBasic::GetSaveWarningOfMSVBAStorage( SfxObjectShell &rDocSh)
{
    uno::Reference < embed::XStorage > xSrcRoot( rDocSh.GetStorage() );
    SvStorageRef xVBAStg( SotStorage::OpenOLEStorage( xSrcRoot, GetMSBasicStorageName(),
                    STREAM_READ | STREAM_NOCREATE | STREAM_SHARE_DENYALL ));
    return ( xVBAStg.Is() && !xVBAStg->GetError() )
                    ? ERRCODE_SVX_VBASIC_STORAGE_EXIST
                    : ERRCODE_NONE;
}

String SvxImportMSVBasic::GetMSBasicStorageName()
{
char __READONLY_DATA sSO_VBAStgName[] = "_MS_VBA_Macros";
    return String::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM(sSO_VBAStgName));
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
