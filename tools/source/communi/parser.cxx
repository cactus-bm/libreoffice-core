/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: parser.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 14:10:13 $
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

#include <stdio.h>
#include <stream.hxx>
#include <fsys.hxx>

#include "iparser.hxx"
#include "geninfo.hxx"



//
// class InformationParser
//

#define cKeyLevelChar '\t'

/*****************************************************************************/
InformationParser::InformationParser( BOOL bReplace )
/*****************************************************************************/
                : pActStream( NULL ),
                nErrorCode( 0 ),
                nErrorLine( 0 ),
                nActLine( 0 ),
                bRecover( FALSE ),
                sOldLine( "" ),
                sErrorText( "" ),
                bReplaceVariables( bReplace ),
                nLevel( 0 ),
                sUPD( "" ),
                sVersion( "" )
{
}

/*****************************************************************************/
InformationParser::~InformationParser()
/*****************************************************************************/
{
}

/*****************************************************************************/
ByteString &InformationParser::ReadLine()
/*****************************************************************************/
{
    ByteString sLine;

    if ( bRecover ) {
        bRecover = FALSE;
    }
    else {
         if ( !pActStream->IsEof()) {
            pActStream->ReadLine( sLine );
            ULONG nStart = 0;
            ULONG nEnd = sLine.Len();
            BOOL bCopy = FALSE;
            while ( nStart < nEnd && ( sLine.GetChar( nStart ) == ' ' || sLine.GetChar( nStart ) == 0x09 ) )
            {
                nStart++;
                bCopy = TRUE;
            }

            while ( nStart < nEnd && ( sLine.GetChar( nEnd-1 ) == ' ' || sLine.GetChar( nEnd-1 ) == 0x09 ) )
            {
                nEnd--;
                bCopy = TRUE;
            }

            if ( bCopy )
                sLine = sLine.Copy( nStart, nEnd - nStart );

            if (( sLine.GetChar( 0 ) == '#' ) || ( !sLine.Len())) {
                if ( sCurrentComment.Len())
                    sCurrentComment += "\n";
                sCurrentComment += sLine;
                return ReadLine();
            }
            else {
                if ( bReplaceVariables ) {
                    sLine.SearchAndReplaceAll( "%UPD", sUPD );
                    sLine.SearchAndReplaceAll( "%VERSION", sVersion );
                }
            }
        }
        else {
            if ( nLevel ) {
                sLine = "}";
                fprintf( stdout, "Reached EOF parsing %s. Suplying extra '}'\n",ByteString( sStreamName, gsl_getSystemTextEncoding()).GetBuffer() );
    //          nErrorCode = IP_UNEXPECTED_EOF;
    //          nErrorLine = nActLine;
            }
            else
                sLine = "";
        }

        sOldLine = sLine;
        nActLine++;
    }

    return sOldLine;
}

/*****************************************************************************/
GenericInformation *InformationParser::ReadKey(
                                    GenericInformationList *pExistingList )
/*****************************************************************************/
{
    // this method has no error handling yet, but it works very fast.
    // it is used to create whole informations and sub informations in
    // a simple data format in memory, readed in a configuration file with
    // following format:

    /*

    key [value]
    {
        key [value]
        key [value]
        {
            key [value]
            ...
            ...
        }
    }
    key [value]
    ...
    ...

    */

    GenericInformation *pInfo = NULL;

    ByteString sLine( ReadLine());
    ByteString sKey;
    ByteString sValue;
    ByteString sComment( sCurrentComment );
    sCurrentComment = "";

    // key separated from value by tab?
    USHORT nWSPos = sLine.Search( ' ' );
    if ( sLine.Search( '\t' ) < nWSPos ) {
        nWSPos = sLine.Search( '\t' );
        sLine.SearchAndReplace( "\t", " " );
    }

    if ( sLine.GetTokenCount( ' ' ) > 1 ) {
        sKey = sLine.GetToken( 0, ' ' );
        sValue = sLine.Copy( sKey.Len() + 1 );
        while (( sValue.Search( ' ' ) == 0 ) || ( sValue.Search( '\t' ) == 0 )) {
            sValue.Erase( 0, 1 );
        }
    }
    else
        sKey=sLine;

    if ( bReplaceVariables && !nLevel ) {
        sUPD = sKey.Copy( sKey.Len() - 3 );
        sVersion = sKey;
    }

    if ( ReadLine() == "{" ) {
        nLevel++;
        GenericInformationList *pSubList = new GenericInformationList();
        while ( ReadLine() != "}" ) {
            Recover();
            ReadKey( pSubList );
        }
        nLevel--;
        pInfo = new GenericInformation( sKey, sValue,
                        pExistingList, pSubList );
        pInfo->SetComment( sComment );
    }
    else {
        Recover();
        if ( !sKey.Equals( "}" ) && !sKey.Equals( "{" ) )
        {
            pInfo = new GenericInformation( sKey, sValue, pExistingList );
            pInfo->SetComment( sComment );
        }
    }

    return pInfo;
}

/*****************************************************************************/
void InformationParser::Recover()
/*****************************************************************************/
{
    bRecover = TRUE;
}

/*****************************************************************************/
BOOL InformationParser::Save( SvStream &rOutStream,
                  const GenericInformationList *pSaveList,
                  USHORT nLevel, BOOL bStripped )
/*****************************************************************************/
{
    USHORT i;
    ULONG nInfoListCount;
    ByteString sTmpStr;
    GenericInformation *pGenericInfo;
    GenericInformationList *pGenericInfoList;

     static ByteString aKeyLevel;
    aKeyLevel.Expand( nLevel, cKeyLevelChar );

    for ( nInfoListCount = 0; nInfoListCount < pSaveList->Count(); nInfoListCount++) {
        // Key-Value Paare schreiben
        pGenericInfo = pSaveList->GetObject( nInfoListCount );
        sTmpStr = "";
        if ( !bStripped && nLevel )
            sTmpStr.Append( aKeyLevel.GetBuffer(), nLevel );

        if ( !bStripped )
            for ( i = 0; i < pGenericInfo->GetComment().GetTokenCount( '\n' ); i++ ) {
                sTmpStr += pGenericInfo->GetComment().GetToken( i, '\n' );
                sTmpStr += "\n";
                if ( nLevel )
                    sTmpStr.Append( aKeyLevel.GetBuffer(), nLevel );
            }

        sTmpStr += pGenericInfo->GetBuffer();
        sTmpStr += ' ';
        sTmpStr += pGenericInfo->GetValue();
        if ( !rOutStream.WriteLine( sTmpStr ) )
              return FALSE;

        // wenn vorhanden, bearbeite recursive die Sublisten
        if (( pGenericInfoList = pGenericInfo->GetSubList() ) != NULL ) {
              // oeffnende Klammer
              sTmpStr = "";
            if ( !bStripped && nLevel )
                sTmpStr.Append( aKeyLevel.GetBuffer(), nLevel );
              sTmpStr += '{';
              if ( !rOutStream.WriteLine( sTmpStr ) )
                return FALSE;
              // recursiv die sublist abarbeiten
              if ( !Save( rOutStream, pGenericInfoList, nLevel+1, bStripped ) )
                return FALSE;
                  // schliessende Klammer
              sTmpStr = "";
            if ( !bStripped && nLevel )
                sTmpStr.Append( aKeyLevel.GetBuffer(), nLevel );
              sTmpStr += '}';
              if ( !rOutStream.WriteLine( sTmpStr ) )
                return FALSE;
        }
      }
      return TRUE;
}

/*****************************************************************************/
GenericInformationList *InformationParser::Execute(
                                SvStream &rSourceStream,
                                GenericInformationList *pExistingList )
/*****************************************************************************/
{
    GenericInformationList *pList;
    if ( pExistingList )
        pList = pExistingList;
    else
        pList = new GenericInformationList();

    pActStream = &rSourceStream;

    // read all infos out of current file
    while( !rSourceStream.IsEof()) {
        nLevel = 0;
        ReadKey( pList );
    }

    return pList;
}

/*****************************************************************************/
GenericInformationList *InformationParser::Execute( SvMemoryStream &rSourceStream,
                            GenericInformationList *pExistingList )
/*****************************************************************************/
{
      sStreamName = UniString( "Memory", gsl_getSystemTextEncoding());
      return Execute( (SvStream &)rSourceStream, pExistingList );
}

/*****************************************************************************/
GenericInformationList *InformationParser::Execute(
                                SvFileStream &rSourceStream,
                                GenericInformationList *pExistingList )
/*****************************************************************************/
{
    if ( !rSourceStream.IsOpen())
        return NULL;
    sStreamName = rSourceStream.GetFileName();
    return Execute( (SvStream &)rSourceStream, pExistingList );
}

/*****************************************************************************/
GenericInformationList *InformationParser::Execute( UniString &rSourceFile,
                                GenericInformationList *pExistingList )
/*****************************************************************************/
{
    DirEntry aDirEntry( rSourceFile );
    if ( !aDirEntry.Exists())
        return NULL;

    GenericInformationList *pList;
    if ( pExistingList )
        pList = pExistingList;
    else
        pList = new GenericInformationList();

    // reset status
    nErrorCode = 0;
    nErrorLine = 0;
    nActLine = 0;

    SvFileStream aActStream;
    aActStream.Open( rSourceFile, STREAM_READ );
    if( aActStream.GetError())
        return NULL;

    pActStream = &aActStream;
    if ( !Execute( aActStream, pList )) {
        delete pList;
        pList = NULL;
    }

    // close the stream
    aActStream.Close();
     pActStream = NULL;

    if ( !nErrorCode )
        return pList;

    return NULL;
}

/*****************************************************************************/
GenericInformationList *InformationParser::Execute( Dir &rDir,
                                GenericInformationList *pExistingList )
/*****************************************************************************/
{
    GenericInformationList *pList;

    if ( pExistingList )
        pList = pExistingList;
    else
        pList = new GenericInformationList();

    for ( USHORT i = 0; i < rDir.Count(); i++ ) {

        // execute this dir
        UniString sNextFile( rDir[i].GetFull());
        GenericInformationList *pSubList = Execute( sNextFile );

        if ( !pSubList ) {
            // any errors ?
            delete pList;
            return NULL;
        }

        // create new info and insert it into list
        ByteString sFileKey( rDir[i].GetName(), RTL_TEXTENCODING_UTF8 );
        GenericInformation *pInfo = new GenericInformation(
                                            sFileKey,
                                            ByteString( "" ),
                                            pList, pSubList );
    }

    return pList;
}

/*****************************************************************************/
BOOL InformationParser::Save( SvFileStream &rSourceStream,
                  const GenericInformationList *pSaveList )
/*****************************************************************************/
{
    if ( !rSourceStream.IsOpen() || !Save( (SvStream &)rSourceStream, pSaveList, 0, FALSE ))
    {
        printf( "ERROR saving file \"%s\"\n",ByteString( rSourceStream.GetFileName(), gsl_getSystemTextEncoding()).GetBuffer() );
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/
BOOL InformationParser::Save( SvMemoryStream &rSourceStream,
                  const GenericInformationList *pSaveList )
/*****************************************************************************/
{
    Time a;
    BOOL bRet = Save( (SvStream &)rSourceStream, pSaveList, 0, TRUE );
    Time b;
    b = b - a;
    return bRet;
}

/*****************************************************************************/
BOOL InformationParser::Save( const UniString &rSourceFile,
                  const GenericInformationList *pSaveList )
/*****************************************************************************/
{
  SvFileStream *pOutFile = new SvFileStream( rSourceFile, STREAM_STD_WRITE | STREAM_TRUNC );

  if ( !Save( *pOutFile, pSaveList )) {
    delete pOutFile;
    return FALSE;
  }
  delete pOutFile;
  return TRUE;
}

/*****************************************************************************/
USHORT InformationParser::GetErrorCode()
/*****************************************************************************/
{
    return nErrorCode;
}

/*****************************************************************************/
ByteString &InformationParser::GetErrorText()
/*****************************************************************************/
{
  //    sErrorText = pActStream->GetFileName();
    sErrorText = ByteString( sStreamName, gsl_getSystemTextEncoding());
    sErrorText += ByteString( " (" );
    sErrorText += ByteString::CreateFromInt64(nErrorLine);
    sErrorText += ByteString( "): " );

    switch ( nErrorCode ) {
    case IP_NO_ERROR:
        sErrorText += ByteString( "Keine Fehler aufgetereten" );
        break;
    case IP_UNEXPECTED_EOF:
        sErrorText += ByteString( "Ung�ltiges Dateiende!" );
        break;
    }

    return sErrorText;
}


