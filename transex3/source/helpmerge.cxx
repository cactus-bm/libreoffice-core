/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: helpmerge.cxx,v $
 *
 *  $Revision: 1.15 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 08:18:10 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_transex3.hxx"
#include <tools/fsys.hxx>
#include <osl/file.hxx>
// local includes
#include <stdio.h>
#include "helpmerge.hxx"
#include "utf8conv.hxx"
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "rtl/strbuf.hxx"
/*****************************************************************************/
void HelpParser::FillInFallbacks( LangHashMap& rElem_out, ByteString sLangIdx_in ){
/*****************************************************************************/
    static const ByteString ENGLISH_LANGUAGECODE( "en-US" );
    static const ByteString GERMAN_LANGUAGECODE ( "de"    );
    ByteString sCur;
    XMLElement* pTmp     = NULL;
    XMLElement* pTmp2    = NULL;

    sCur = sLangIdx_in;
    ByteString sFallback( sCur );
    GetIsoFallback( sFallback );
    if( (rElem_out.find( sFallback ) != rElem_out.end()) && rElem_out[ sFallback ] != NULL ){
        pTmp2 = rElem_out[ sFallback ];
        pTmp = new XMLElement( *pTmp2 )  ; // Copy
        pTmp->SetPos( pTmp2->GetPos()+1 );
        pTmp->ChangeLanguageTag( String( sLangIdx_in , RTL_TEXTENCODING_ASCII_US) );
        rElem_out[ sLangIdx_in ] = pTmp;
        pTmp2 = NULL;
    }
    else if( (rElem_out.find( ENGLISH_LANGUAGECODE ) != rElem_out.end()) && rElem_out[ ENGLISH_LANGUAGECODE ] != NULL ){// No English
        pTmp2 = rElem_out[ ENGLISH_LANGUAGECODE ];
        pTmp = new XMLElement( *pTmp2 )  ; // Copy
        pTmp->SetPos( pTmp2->GetPos()+1 );
        pTmp->ChangeLanguageTag( String( sLangIdx_in , RTL_TEXTENCODING_ASCII_US) );
        rElem_out[ sCur ] = pTmp;
        pTmp2 = NULL;
    }
    else if( (rElem_out.find( GERMAN_LANGUAGECODE ) != rElem_out.end() ) && rElem_out[ GERMAN_LANGUAGECODE ] != NULL ){// No English
        pTmp2 = rElem_out[ GERMAN_LANGUAGECODE ];
        pTmp = new XMLElement( *pTmp2 ); // Copy
        pTmp->SetPos( pTmp2->GetPos()+1 );
        pTmp->ChangeLanguageTag( String( sLangIdx_in , RTL_TEXTENCODING_ASCII_US ) );
        rElem_out[ sCur ] = pTmp;
        pTmp2 = NULL;
    }else{
        fprintf(stdout,"ERROR: No Fallback found for language %s:\n",sCur.GetBuffer());
        rElem_out[ sCur ]=new XMLElement(); // Use dummy element
    }
}

/*****************************************************************************/
void HelpParser::Dump(XMLHashMap* rElem_in) {
/*****************************************************************************/
    for(XMLHashMap::iterator pos = rElem_in->begin();pos != rElem_in->end(); ++pos){
        Dump(pos->second,pos->first);
    }
}
/*****************************************************************************/
void HelpParser::Dump(LangHashMap* rElem_in,const ByteString sKey_in) {
/*****************************************************************************/
    ByteString x;
    OString y;
    fprintf(stdout,"+------------%s-----------+\n",sKey_in.GetBuffer() );
    for(LangHashMap::iterator posn=rElem_in->begin();posn!=rElem_in->end();++posn){
        x=posn->first;
        y=posn->second->ToOString();
        fprintf(stdout,"key=%s value=%s\n",x.GetBuffer(),y.getStr());
    }
    fprintf(stdout,"+--------------------------+\n");
}

HelpParser::HelpParser( const ByteString &rHelpFile, bool rUTF8 , bool rHasInputList  )
        : sHelpFile( rHelpFile ),
          bUTF8    ( rUTF8     ),
          bHasInputList( rHasInputList )
          {};

/*****************************************************************************/
bool HelpParser::CreateSDF(
/*****************************************************************************/
    const ByteString &rSDFFile_in, const ByteString &rPrj_in,const ByteString &rRoot_in ){
    // GSI File constants
    static const String GSI_SEQUENCE1( String::CreateFromAscii("\t0\thelp\t")   );
    static const String GSI_SEQUENCE2( String::CreateFromAscii("\t\t\t0\t")     );
    static const String GSI_TAB      ( String::CreateFromAscii("\t")            );
    static const String GSI_SEQUENCE4( String::CreateFromAscii("\t\t\t\t")      );
    static const String ret          ( String::CreateFromAscii("\n")            );
    static const String ret_char     ( String::CreateFromAscii("")              );
    static const String tab          ( String::CreateFromAscii("\t")            );
    static const String tab_char     ( String::CreateFromAscii("")              );

    SimpleXMLParser aParser;
    String sUsedTempFile;
    String sXmlFile;

    if( Export::fileHasUTF8ByteOrderMarker( sHelpFile ) ){
        DirEntry aTempFile = Export::GetTempFile();
        DirEntry aSourceFile( String( sHelpFile , RTL_TEXTENCODING_ASCII_US ) );
        aSourceFile.CopyTo( aTempFile , FSYS_ACTION_COPYFILE );
        String sTempFile = aTempFile.GetFull();
        Export::RemoveUTF8ByteOrderMarkerFromFile( ByteString( sTempFile , RTL_TEXTENCODING_ASCII_US ) );
        sUsedTempFile = sTempFile;
        sXmlFile = sTempFile;
    }else{
        sUsedTempFile = String::CreateFromAscii("");
        sXmlFile = String( sHelpFile , RTL_TEXTENCODING_ASCII_US );
    }


    std::auto_ptr <XMLFile> file ( aParser.Execute( sXmlFile ) );

    if(file.get() == NULL){
        printf("%s\n",ByteString(aParser.GetError().sMessage,RTL_TEXTENCODING_ASCII_US).GetBuffer());
        exit(-1);
        //return false;
    }
    file->Extract();
    if( !file->CheckExportStatus() ){
        return true;
    }
    SvFileStream aSDFStream( String( rSDFFile_in, RTL_TEXTENCODING_ASCII_US ),
        STREAM_STD_WRITE | STREAM_TRUNC );

    if ( !aSDFStream.IsOpen()) {
        fprintf(stdout,"Can't open file %s\n",rSDFFile_in.GetBuffer());
        return false;
    }

      DirEntry aEntry( String( sHelpFile, RTL_TEXTENCODING_ASCII_US ));
    aEntry.ToAbs();
    String sFullEntry = aEntry.GetFull();
    aEntry += DirEntry( String( "..", RTL_TEXTENCODING_ASCII_US ));
    aEntry += DirEntry( rRoot_in );
    ByteString sPrjEntry( aEntry.GetFull(), gsl_getSystemTextEncoding());
    ByteString sActFileName(
    sFullEntry.Copy( sPrjEntry.Len() + 1 ), gsl_getSystemTextEncoding());

    sActFileName.SearchAndReplaceAll( "/", "\\" );

    XMLHashMap*  aXMLStrHM   = file->GetStrings();
    LangHashMap* pElem;
    XMLElement*  pXMLElement  = NULL;

    //Dump(aXMLStrHM);

    ByteString sTimeStamp( Export::GetTimeStamp() );
    OUString sOUTimeStamp( sTimeStamp.GetBuffer() , sTimeStamp.Len() , RTL_TEXTENCODING_ASCII_US );

    OUStringBuffer sBuffer;
    const OUString sOUPrj( rPrj_in.GetBuffer() , rPrj_in.Len() , RTL_TEXTENCODING_ASCII_US );
    const OUString sOUActFileName(sActFileName.GetBuffer() , sActFileName.Len() , RTL_TEXTENCODING_ASCII_US );

    Export::InitLanguages( false );
    std::vector<ByteString> aLanguages = Export::GetLanguages();

    std::vector<ByteString> order = file->getOrder();
    std::vector<ByteString>::iterator pos;
    XMLHashMap::iterator posm;

    for( pos = order.begin(); pos != order.end() ; ++pos )
    {
        posm = aXMLStrHM->find( *pos );
        pElem = posm->second;
        ByteString sCur;

        for( unsigned int n = 0; n < aLanguages.size(); n++ )
        {
            sCur = aLanguages[ n ];
            if(pElem->find( sCur )==pElem->end())
            {
                FillInFallbacks( *pElem , sCur );
            }
            pXMLElement = (*pElem)[ sCur ];

            if( pXMLElement != NULL )
            {
                OUString data = pXMLElement->ToOUString();
                   String sTmp = String(data.getStr());
                sTmp.SearchAndReplaceAll(ret,ret_char);    // Remove \n
                sTmp.SearchAndReplaceAll(tab,tab_char);    // Remove \t

                data = OUString( sTmp );
                sBuffer.append( sOUPrj );
                sBuffer.append( GSI_TAB );              //"\t";
                if ( rRoot_in.Len())
                    sBuffer.append( sOUActFileName );
                   sBuffer.append( GSI_SEQUENCE1 );     //"\t0\thelp\t";
                ByteString sID = posm->first;           // ID
                sBuffer.append( OUString( sID.GetBuffer() , sID.Len() , RTL_TEXTENCODING_UTF8 ) );
                sBuffer.append( GSI_TAB ); //"\t";
                   ByteString sOldRef = pXMLElement->GetOldref(); // oldref
                sBuffer.append( OUString(sOldRef.GetBuffer() , sOldRef.Len() , RTL_TEXTENCODING_UTF8 ) );
                   sBuffer.append( GSI_SEQUENCE2 );     //"\t\t\t0\t";
                sBuffer.append( OUString( sCur.GetBuffer() , sCur.Len() , RTL_TEXTENCODING_UTF8 ) );
                   sBuffer.append( GSI_TAB );               //"\t";
                sBuffer.append( data );
                  sBuffer.append( GSI_SEQUENCE4 );      //"\t\t\t\t";
                sBuffer.append( sOUTimeStamp );
                ByteString sOut( sBuffer.makeStringAndClear().getStr() , RTL_TEXTENCODING_UTF8 );
                if( !sCur.EqualsIgnoreCaseAscii("de") ||( sCur.EqualsIgnoreCaseAscii("de") && !Export::isMergingGermanAllowed( rPrj_in ) ) )
                {
                    if( data.getLength() > 0 ) aSDFStream.WriteLine( sOut );
                }
                pXMLElement=NULL;
            }else fprintf(stdout,"\nDBG: NullPointer in HelpParser::CreateSDF , Language %s\n",sCur.GetBuffer() );
        }

    }
    //Dump(aXMLStrHM);
    aSDFStream.Close();

    if( !sUsedTempFile.EqualsIgnoreCaseAscii( "" ) ){
        DirEntry aTempFile( sUsedTempFile );
        aTempFile.Kill();
    }
    return TRUE;
}

bool HelpParser::Merge( const ByteString &rSDFFile, const ByteString &rDestinationFile  ,
        ByteString& sLanguage , MergeDataFile& aMergeDataFile )
{

    (void) rSDFFile;
    bool hasNoError = true;

    SimpleXMLParser aParser;

    String sUsedTempFile;
    String sXmlFile;

    if( Export::fileHasUTF8ByteOrderMarker( sHelpFile ) ){
        DirEntry aTempFile = Export::GetTempFile();
        DirEntry aSourceFile( String( sHelpFile , RTL_TEXTENCODING_ASCII_US ) );
        aSourceFile.CopyTo( aTempFile , FSYS_ACTION_COPYFILE );
        String sTempFile = aTempFile.GetFull();
        Export::RemoveUTF8ByteOrderMarkerFromFile( ByteString( sTempFile , RTL_TEXTENCODING_ASCII_US ) );
        sUsedTempFile = sTempFile;
        sXmlFile = sTempFile;
    }else{
        sUsedTempFile = String::CreateFromAscii("");
        sXmlFile = String( sHelpFile , RTL_TEXTENCODING_ASCII_US );
    }

    OUString sOUHelpFile( sXmlFile );

    XMLFile* xmlfile = ( aParser.Execute( sOUHelpFile ) );
    printf("Dest file %s\n",rDestinationFile.GetBuffer());
    hasNoError = MergeSingleFile( xmlfile , aMergeDataFile , sLanguage , rDestinationFile );
    delete xmlfile;
    if( !sUsedTempFile.EqualsIgnoreCaseAscii( "" ) ){
        DirEntry aTempFile( sUsedTempFile );
        aTempFile.Kill();
    }
    return hasNoError;
}

bool ByteStringEqual( const ByteString& rKey1, const ByteString& rKey2 )  {
    return rKey1.CompareTo( rKey2 )==COMPARE_EQUAL;
};
bool ByteStringLess( const ByteString& rKey1, const ByteString& rKey2 )  {
     return rKey1.CompareTo( rKey2 )==COMPARE_LESS;
}

void HelpParser::parse_languages( std::vector<ByteString>& aLanguages , MergeDataFile& aMergeDataFile ){
    std::vector<ByteString> aTmp;

    const ByteString DE     ("de");
    const ByteString ENUS   ("en-US");
    static const ByteString ALL( "ALL" );

    Export::InitLanguages( false );

    if( Export::sLanguages.EqualsIgnoreCaseAscii( ALL ) )
    {
        aLanguages = aMergeDataFile.GetLanguages();
        aLanguages.push_back( DE );
        aLanguages.push_back( ENUS );

        if( !Export::sForcedLanguages.Equals("") )
        {
            std::vector<ByteString> aFL = Export::GetForcedLanguages();
            std::copy( aFL.begin() ,
                       aFL.end() ,
                       back_inserter( aLanguages )
                     );
            std::sort(   aLanguages.begin() , aLanguages.end() , ByteStringLess );
            std::vector<ByteString>::iterator unique_iter =  std::unique( aLanguages.begin() , aLanguages.end() , ByteStringEqual );
            std::copy( aLanguages.begin() , unique_iter , back_inserter( aTmp ) );
            aLanguages = aTmp;
        }
    }
    else{
        aLanguages = Export::GetLanguages();
    }

}

bool HelpParser::Merge(
    const ByteString &rSDFFile, const ByteString &rPathX , const ByteString &rPathY , bool bISO ,
    const std::vector<ByteString>& aLanguages , MergeDataFile& aMergeDataFile , bool bCreateDir )
{


    (void) rSDFFile ;
    bool hasNoError = true;
    SimpleXMLParser aParser;
    String sUsedTempFile;
    String sXmlFile;

    if( Export::fileHasUTF8ByteOrderMarker( sHelpFile ) )
    {
        DirEntry aTempFile = Export::GetTempFile();
        DirEntry aSourceFile( String( sHelpFile , RTL_TEXTENCODING_ASCII_US ) );
        aSourceFile.CopyTo( aTempFile , FSYS_ACTION_COPYFILE );
        String sTempFile = aTempFile.GetFull();
        Export::RemoveUTF8ByteOrderMarkerFromFile( ByteString( sTempFile , RTL_TEXTENCODING_ASCII_US ) );
        sUsedTempFile = sTempFile;
        sXmlFile = sTempFile;
    }
    else
    {
        sUsedTempFile = String::CreateFromAscii("");
        sXmlFile = String( sHelpFile , RTL_TEXTENCODING_ASCII_US );
    }


    OUString sOUHelpFile( sXmlFile );
    XMLFile* xmlfile = ( aParser.Execute( sOUHelpFile ) );
    xmlfile->Extract();

    if( xmlfile == NULL)
    {
        printf("%s\n",ByteString(aParser.GetError().sMessage,RTL_TEXTENCODING_UTF8).GetBuffer());
        exit(-1);
        //return false;
    }


    ByteString sCur;
    for( unsigned int n = 0; n < aLanguages.size(); n++ ){
        sCur = aLanguages[ n ];

        ByteString sFilepath;
        if( bISO )  sFilepath = GetOutpath( rPathX , sCur , rPathY );
        else        sFilepath = rPathX;
        if( bCreateDir ) MakeDir( sFilepath );

        XMLFile* file = new XMLFile( *xmlfile );
        sFilepath.Append( sHelpFile );
        hasNoError = MergeSingleFile( file , aMergeDataFile , sCur , sFilepath );
        delete file;

        if( !hasNoError ) return false;         // Stop on error
     }

    if( !sUsedTempFile.EqualsIgnoreCaseAscii( "" ) )
    {
        DirEntry aTempFile( sUsedTempFile );
        aTempFile.Kill();
    }
    delete xmlfile;
    return hasNoError;
}

bool HelpParser::MergeSingleFile( XMLFile* file , MergeDataFile& aMergeDataFile , const ByteString& sLanguage ,
                                  ByteString sPath )
{
    file->Extract();

       XMLHashMap*   aXMLStrHM     = file->GetStrings();
    LangHashMap*  aLangHM;
    static  ResData pResData( "","","");
    pResData.sResTyp   = "help";

    ByteString sTmp             = Export::sLanguages;

    sTmp.EraseLeadingAndTrailingChars();

    for(XMLHashMap::iterator pos=aXMLStrHM->begin();pos!=aXMLStrHM->end();++pos)    // Merge every l10n related string
    {

        aLangHM             = pos->second;
        //printf("*********************DUMPING HASHMAP***************************************");
        //Dump( aXMLStrHM );
        //printf("DBG: sHelpFile = %s\n",sHelpFile.GetBuffer() );

        pResData.sGId      =  pos->first;
        pResData.sFilename  =  sHelpFile;

        ProcessHelp( aLangHM , sLanguage, &pResData , aMergeDataFile );
     }

    // Init temp and target file
    ByteString sTempFile;
    ByteString sTargetFile( sPath );

    static const ByteString INPATH = Export::GetEnv( "INPATH" );
    Export::getRandomName( sPath , sTempFile , INPATH );

    // Write in the temp file
    bool hasNoError = file->Write ( sTempFile );
    if( !hasNoError ) return false;

    remove( sTargetFile.GetBuffer() );
    if( rename( sTempFile.GetBuffer() , sTargetFile.GetBuffer() ) != 0 )
    {
        cerr << "ERROR: helpex Can't rename file " << sTempFile.GetBuffer() << " to " << sTargetFile.GetBuffer() << "\n";
        return false;
    }

    return true;
}

ByteString HelpParser::GetOutpath( const ByteString& rPathX , const ByteString& sCur , const ByteString& rPathY ){
    ByteString testpath = rPathX;
    static const ByteString sDelimiter( DirEntry::GetAccessDelimiter(), RTL_TEXTENCODING_ASCII_US );
    testpath.EraseTrailingChars( '/' );
    testpath.EraseTrailingChars( '\\' );
    testpath += sDelimiter;
    testpath += sCur;
    testpath += sDelimiter;
    ByteString sRelativePath( rPathY );
    sRelativePath.EraseLeadingChars( '/' );
    sRelativePath.EraseLeadingChars( '\\' );
    testpath += sRelativePath;
    testpath += sDelimiter;
    return testpath;
}
void HelpParser::MakeDir( const ByteString& sPath ){
    String sPathtmp( sPath , RTL_TEXTENCODING_ASCII_US );
    String sDir( sPathtmp.Copy( 0 , sPathtmp.SearchCharBackward( DirEntry::GetAccessDelimiter().GetBuffer() ) ) );
    DirEntry aDirEntry( sDir );

    ByteString sTDir( sDir , sDir.Len() , RTL_TEXTENCODING_ASCII_US );
    if( aDirEntry.MakeDir() ){  // Errorhandling ?
    //    printf("ERROR: Could NOT create Directory %s\n",sTDir.GetBuffer() );
    //    exit( -1 );
    }
}


/* ProcessHelp Methode: search for en-US entry and replace it with the current language*/
void HelpParser::ProcessHelp( LangHashMap* aLangHM , const ByteString& sCur , ResData *pResData , MergeDataFile& aMergeDataFile ){

    XMLElement*   pXMLElement = NULL;
       PFormEntrys   *pEntrys    = NULL;
    XMLData       *data       = NULL;
    XMLParentNode *parent     = NULL;

    String        sNewdata;
    ByteString sLId;
    ByteString sGId;

    pEntrys = NULL;

#ifdef MERGE_SOURCE_LANGUAGES
    if( true ){                  // Merge en-US!
#else
    if( !sCur.EqualsIgnoreCaseAscii("en-US") ){
#endif
        pXMLElement = (*aLangHM)[ "en-US" ];
        if( pXMLElement == NULL )
        {
            printf("Error: Can't find en-US entry\n");
        }
        if( pXMLElement != NULL )
        {
            parent  = pXMLElement->GetParent();
            sLId    = pXMLElement->GetOldref();
            pResData->sId     =  sLId;

            pEntrys = aMergeDataFile.GetPFormEntrys( pResData );
            if( pEntrys != NULL)
            {
                ByteString sNewText;
                pEntrys->GetText( sNewText, STRING_TYP_TEXT, sCur , true );
                sNewdata = String(  sNewText , RTL_TEXTENCODING_UTF8 );
                if ( sNewdata.Len())
                {
                    if( pXMLElement != NULL )
                    {
                        data   = new XMLData( sNewdata , NULL , true ); // Add new one
                        pXMLElement->RemoveAndDeleteAllChilds();
                        pXMLElement->AddChild( data );
                        aLangHM->erase( sCur );
                    }
                }
            }else if( pResData == NULL ){fprintf(stdout,"Can't find GID=%s LID=%s TYP=%s\n",pResData->sGId.GetBuffer(),pResData->sId.GetBuffer(),pResData->sResTyp.GetBuffer());}
            pXMLElement->ChangeLanguageTag( String( sCur , RTL_TEXTENCODING_ASCII_US) );
        }

    }
}
/* Process() Method merges */
void HelpParser::Process( LangHashMap* aLangHM , const ByteString& sCur , ResData *pResData , MergeDataFile& aMergeDataFile ){

    XMLElement*   pXMLElement = NULL;
       PFormEntrys   *pEntrys    = NULL;
    XMLData       *data       = NULL;
    XMLParentNode *parent     = NULL;
    XMLDefault    *xmldefault = NULL;

    short         curLang     = 0;
    String        sNewdata;
    bool          isFallback  = false;
    ByteString sLId;
    ByteString sGId;

    pEntrys = NULL;

#ifdef MERGE_SOURCE_LANGUAGES
    if( true ){                  // Merge en-US!
#else
    if( !sCur.EqualsIgnoreCaseAscii("en-US") ){
#endif
        pXMLElement = (*aLangHM)[ sCur ];
        if( pXMLElement == NULL )
        {
            FillInFallbacks( *aLangHM , sCur );
            pXMLElement =   ( *aLangHM )[ sCur ];
            isFallback = true;
        }
        if( pXMLElement != NULL )
        {
            parent  = pXMLElement->GetParent();
            sLId    = pXMLElement->GetOldref();
            pResData->sId     =  sLId;

            pEntrys = aMergeDataFile.GetPFormEntrys( pResData );
            if( pEntrys != NULL)
            {
                ByteString sNewText;
                pEntrys->GetText( sNewText, STRING_TYP_TEXT, sCur , true );
                sNewdata = String(  sNewText , RTL_TEXTENCODING_UTF8 );
                if ( sNewdata.Len())
                {
                    printf("Entries found\n");
                    if( pXMLElement != NULL )
                    {
                        data   = new XMLData( sNewdata , NULL , true ); // Add new one
                        if( pXMLElement->ToOUString().compareTo( OUString(data->GetData()) ) != 0 )
                        {
                            pXMLElement->RemoveAndDeleteAllChilds();
                            pXMLElement->AddChild( data );
                        }
                        if( isFallback )
                        {
                            xmldefault = new XMLDefault( String::CreateFromAscii("\n") , NULL );
                            int pos = parent->GetPosition( pXMLElement->GetId() );
                            if( pos != -1 ){
                                parent->AddChild(xmldefault , pos+1 );
                                parent->AddChild(pXMLElement , pos+2 );
                            }
                            else fprintf(stdout,"ERROR: Can't find reference Element of id %s language %d\n",pXMLElement->GetId().GetBuffer(),curLang);
                        }

                        aLangHM->erase( sCur );
                    }
                }
                delete pResData;
            }else if( pResData == NULL ){fprintf(stdout,"Can't find GID=%s LID=%s TYP=%s\n",pResData->sGId.GetBuffer(),pResData->sId.GetBuffer(),pResData->sResTyp.GetBuffer());}
        }

    }
}

