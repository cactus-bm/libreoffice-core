/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: HelpLinker.cxx,v $
 * $Revision: 1.16 $
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

#include "HelpCompiler.hxx"

#include <map>

#include <string.h>
#include <limits.h>

#include <libxslt/xslt.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/functions.h>
#include <libxslt/extensions.h>

#include <sal/types.h>
#include <osl/time.h>

#ifdef SYSTEM_EXPAT
#include <expat.h>
#else
#include <expat/xmlparse.h>
#endif

class IndexerPreProcessor
{
private:
    std::string             m_aModuleName;
    fs::path                m_fsIndexBaseDir;
    fs::path                m_fsCaptionFilesDirName;
    fs::path                m_fsContentFilesDirName;

    xsltStylesheetPtr       m_xsltStylesheetPtrCaption;
    xsltStylesheetPtr       m_xsltStylesheetPtrContent;

public:
    IndexerPreProcessor( const std::string& aModuleName, const fs::path& fsIndexBaseDir,
         const fs::path& idxCaptionStylesheet, const fs::path& idxContentStylesheet );
    ~IndexerPreProcessor();

    void processDocument( xmlDocPtr doc, const std::string& EncodedDocPath );
};

IndexerPreProcessor::IndexerPreProcessor
    ( const std::string& aModuleName, const fs::path& fsIndexBaseDir,
      const fs::path& idxCaptionStylesheet, const fs::path& idxContentStylesheet )
        : m_aModuleName( aModuleName )
        , m_fsIndexBaseDir( fsIndexBaseDir )
{
    m_fsCaptionFilesDirName = fsIndexBaseDir / "caption";
    fs::create_directory( m_fsCaptionFilesDirName );

    m_fsContentFilesDirName = fsIndexBaseDir / "content";
    fs::create_directory( m_fsContentFilesDirName );

    m_xsltStylesheetPtrCaption = xsltParseStylesheetFile
        ((const xmlChar *)idxCaptionStylesheet.native_file_string().c_str());
    m_xsltStylesheetPtrContent = xsltParseStylesheetFile
        ((const xmlChar *)idxContentStylesheet.native_file_string().c_str());
}

IndexerPreProcessor::~IndexerPreProcessor()
{
    if( m_xsltStylesheetPtrCaption )
        xsltFreeStylesheet( m_xsltStylesheetPtrCaption );
    if( m_xsltStylesheetPtrContent )
        xsltFreeStylesheet( m_xsltStylesheetPtrContent );
}


std::string getEncodedPath( const std::string& Path )
{
    rtl::OString aOStr_Path( Path.c_str() );
    rtl::OUString aOUStr_Path( rtl::OStringToOUString
        ( aOStr_Path, fs::getThreadTextEncoding() ) );
    rtl::OUString aPathURL;
    osl::File::getFileURLFromSystemPath( aOUStr_Path, aPathURL );
    rtl::OString aOStr_PathURL( rtl::OUStringToOString
        ( aPathURL, fs::getThreadTextEncoding() ) );
    std::string aStdStr_PathURL( aOStr_PathURL.getStr() );
    return aStdStr_PathURL;
}

void IndexerPreProcessor::processDocument
    ( xmlDocPtr doc, const std::string &EncodedDocPath )
{
    std::string aStdStr_EncodedDocPathURL = getEncodedPath( EncodedDocPath );

    xmlDocPtr resCaption = xsltApplyStylesheet( m_xsltStylesheetPtrCaption, doc, NULL );
    xmlNodePtr pResNodeCaption = resCaption->xmlChildrenNode;
    if( pResNodeCaption )
    {
        fs::path fsCaptionPureTextFile_docURL = m_fsCaptionFilesDirName / aStdStr_EncodedDocPathURL;
        std::string aCaptionPureTextFileStr_docURL = fsCaptionPureTextFile_docURL.native_file_string();
        FILE* pFile_docURL = fopen( aCaptionPureTextFileStr_docURL.c_str(), "w" );
        if( pFile_docURL )
        {
            fprintf( pFile_docURL, "%s\n", pResNodeCaption->content );
            fclose( pFile_docURL );
        }
    }
    xmlFreeDoc(resCaption);

    xmlDocPtr resContent = xsltApplyStylesheet( m_xsltStylesheetPtrContent, doc, NULL );
    xmlNodePtr pResNodeContent = resContent->xmlChildrenNode;
    if( pResNodeContent )
    {
        fs::path fsContentPureTextFile_docURL = m_fsContentFilesDirName / aStdStr_EncodedDocPathURL;
        std::string aContentPureTextFileStr_docURL = fsContentPureTextFile_docURL.native_file_string();
        FILE* pFile_docURL = fopen( aContentPureTextFileStr_docURL.c_str(), "w" );
        if( pFile_docURL )
        {
            fprintf( pFile_docURL, "%s\n", pResNodeContent->content );
            fclose( pFile_docURL );
        }
    }
    xmlFreeDoc(resContent);
}

struct Data
{
    std::vector<std::string> _idList;
    typedef std::vector<std::string>::const_iterator cIter;

    void append(const std::string &id)
    {
        _idList.push_back(id);
    }

    std::string getString() const
    {
        std::string ret;
        cIter aEnd = _idList.end();
        for (cIter aIter = _idList.begin(); aIter != aEnd; ++aIter)
            ret += *aIter + ";";
        return ret;
    }
};

class HelpKeyword
{
private:
    typedef std::hash_map<std::string, Data, pref_hash> DataHashtable;
    DataHashtable _hash;

public:
    void insert(const std::string &key, const std::string &id)
    {
        Data &data = _hash[key];
        data.append(id);
    }

    void dump(DB* table)
    {
        DataHashtable::const_iterator aEnd = _hash.end();
        for (DataHashtable::const_iterator aIter = _hash.begin(); aIter != aEnd; ++aIter)
        {
            const std::string &keystr = aIter->first;
            DBT key;
            memset(&key, 0, sizeof(key));
            key.data = const_cast<char*>(keystr.c_str());
            key.size = keystr.length();

            const Data &data = aIter->second;
            std::string str = data.getString();
            DBT value;
            memset(&value, 0, sizeof(value));
            value.data = const_cast<char*>(str.c_str());
            value.size = str.length();

            table->put(table, NULL, &key, &value, 0);
        }
    }
};

class HelpLinker
{
public:
    void main(std::vector<std::string> &args, std::string* pExtensionPath = NULL )
        throw( HelpProcessingException );

    HelpLinker()
        : init(true)
        , m_pIndexerPreProcessor(NULL)
    {}
    ~HelpLinker()
        { delete m_pIndexerPreProcessor; }

private:
    int locCount, totCount;
    Stringtable additionalFiles;
    HashSet helpFiles;
    fs::path sourceRoot;
    fs::path embeddStylesheet;
    fs::path idxCaptionStylesheet;
    fs::path idxContentStylesheet;
    fs::path zipdir;
    fs::path outputFile;
    std::string module;
    std::string lang;
    std::string hid;
    std::string extensionPath;
    bool bExtensionMode;
    fs::path indexDirName;
    Stringtable hidlistTranslation;
    fs::path indexDirParentName;
    bool init;
    IndexerPreProcessor* m_pIndexerPreProcessor;
    void initIndexerPreProcessor();
    void link() throw( HelpProcessingException );
    void addBookmark( DB* dbBase, std::string thishid,
        const std::string& fileB, const std::string& anchorB,
        const std::string& jarfileB, const std::string& titleB );
#if 0
    /**
     * @param outputFile
     * @param module
     * @param lang
     * @param hid
     * @param helpFiles
     * @param additionalFiles
     */

    private HelpURLStreamHandlerFactory urlHandler = null;
#endif
};

namespace URLEncoder
{
    static std::string encode(const std::string &rIn)
    {
        const char *good = "!$&'()*+,-.=@_";
        static const char hex[17] = "0123456789ABCDEF";

        std::string result;
        for (size_t i=0; i < rIn.length(); ++i)
        {
            unsigned char c = rIn[i];
            if (isalnum (c) || strchr (good, c))
                result += c;
            else {
                result += '%';
                result += hex[c >> 4];
                result += hex[c & 0xf];
            }
        }
        return result;
    }
}

void HelpLinker::addBookmark( DB* dbBase, std::string thishid,
        const std::string& fileB, const std::string& anchorB,
        const std::string& jarfileB, const std::string& titleB)
{
    HCDBG(std::cerr << "HelpLinker::addBookmark " << thishid << " " <<
        fileB << " " << anchorB << " " << jarfileB << " " << titleB << std::endl);

    std::string temp = thishid;
    std::transform (temp.begin(), temp.end(), temp.begin(), toupper);
    std::replace(temp.begin(), temp.end(), ':', '_');
    const std::string& translatedHid = hidlistTranslation[temp];
    if (!translatedHid.empty())
        thishid = translatedHid;

    thishid = URLEncoder::encode(thishid);

    DBT key;
    memset(&key, 0, sizeof(key));
    key.data = const_cast<char*>(thishid.c_str());
    key.size = thishid.length();

    int fileLen = fileB.length();
    if (!anchorB.empty())
        fileLen += (1 + anchorB.length());
    int dataLen = 1 + fileLen + 1 + jarfileB.length() + 1 + titleB.length();

    std::vector<unsigned char> dataB(dataLen);
    size_t i = 0;
    dataB[i++] = static_cast<unsigned char>(fileLen);
    for (size_t j = 0; j < fileB.length(); ++j)
        dataB[i++] = fileB[j];
    if (!anchorB.empty())
    {
        dataB[i++] = '#';
        for (size_t j = 0; j < anchorB.length(); ++j)
            dataB[i++] = anchorB[j];
    }
    dataB[i++] = static_cast<unsigned char>(jarfileB.length());
    for (size_t j = 0; j < jarfileB.length(); ++j)
        dataB[i++] = jarfileB[j];

    dataB[i++] = static_cast<unsigned char>(titleB.length());
    for (size_t j = 0; j < titleB.length(); ++j)
        dataB[i++] = titleB[j];

    DBT data;
    memset(&data, 0, sizeof(data));
    data.data = &dataB[0];
    data.size = dataB.size();

    dbBase->put(dbBase, NULL, &key, &data, 0);
}

void HelpLinker::initIndexerPreProcessor()
{
    if( m_pIndexerPreProcessor )
        delete m_pIndexerPreProcessor;
    std::string mod = module;
    std::transform (mod.begin(), mod.end(), mod.begin(), tolower);
    m_pIndexerPreProcessor = new IndexerPreProcessor( mod, indexDirParentName,
         idxCaptionStylesheet, idxContentStylesheet );
}

/**
*
*/
void HelpLinker::link() throw( HelpProcessingException )
{
    bool bIndexForExtension = false;        // TODO

    if( bExtensionMode )
    {
        indexDirParentName = sourceRoot;
    }
    else
    {
        indexDirParentName = zipdir;
        fs::create_directory(indexDirParentName);
    }

#ifdef CMC_DEBUG
    std::cerr << "will not delete tmpdir of " << indexDirParentName.native_file_string().c_str() << std::endl;
#endif

    std::string mod = module;
    std::transform (mod.begin(), mod.end(), mod.begin(), tolower);

    // do the work here
    // continue with introduction of the overall process thing into the
    // here all hzip files will be worked on
    std::string appl = mod;
    if (appl[0] == 's')
        appl = appl.substr(1);

    fs::path helpTextFileName(indexDirParentName / (mod + ".ht"));
    DB* helpText(0);
    db_create(&helpText,0,0);
    helpText->open(helpText, NULL, helpTextFileName.native_file_string().c_str(), NULL, DB_BTREE,
        DB_CREATE | DB_TRUNCATE, 0644);

    fs::path dbBaseFileName(indexDirParentName / (mod + ".db"));
    DB* dbBase(0);
    db_create(&dbBase,0,0);
    dbBase->open(dbBase, NULL, dbBaseFileName.native_file_string().c_str(), NULL, DB_BTREE,
        DB_CREATE | DB_TRUNCATE, 0644);

    fs::path keyWordFileName(indexDirParentName / (mod + ".key"));
    DB* keyWord(0);
    db_create(&keyWord,0,0);
    keyWord->open(keyWord, NULL, keyWordFileName.native_file_string().c_str(), NULL, DB_BTREE,
        DB_CREATE | DB_TRUNCATE, 0644);

    HelpKeyword helpKeyword;

    // catch HelpProcessingException to avoid locking data bases
    try
    {

    std::ifstream fileReader(hid.c_str());
    while (fileReader)
    {
        std::string key;
        fileReader >> key;
        std::transform (key.begin(), key.end(), key.begin(), toupper);
        std::replace(key.begin(), key.end(), ':', '_');
        std::string data;
        fileReader >> data;
        if (!key.empty() && !data.empty())
            hidlistTranslation[key] = data;
    }
    fileReader.close();

    // lastly, initialize the indexBuilder
    if ( (!bExtensionMode || bIndexForExtension) && !helpFiles.empty())
        initIndexerPreProcessor();

    if( !bExtensionMode )
    {
        std::cout << "Making " << outputFile.native_file_string() <<
            " from " << helpFiles.size() << " input files" << std::endl;
    }

    // here we start our loop over the hzip files.
    HashSet::iterator end = helpFiles.end();
    for (HashSet::iterator iter = helpFiles.begin(); iter != end; ++iter)
    {
        std::cout << ".";
        std::cout.flush();

        // process one file
        // streamTable contains the streams in the hzip file
        StreamTable streamTable;
        const std::string &xhpFileName = *iter;

        if (!bExtensionMode && xhpFileName.rfind(".xhp") != xhpFileName.length()-4)
        {
            // only work on .xhp - files
            std::cerr <<
                "ERROR: input list entry '"
                    << xhpFileName
                    << "' has the wrong extension (only files with extension .xhp "
                    << "are accepted)";
            continue;
        }

        fs::path langsourceRoot(sourceRoot);
        fs::path xhpFile;

        if( bExtensionMode )
        {
            // langsourceRoot == sourceRoot for extensions
            std::string xhpFileNameComplete( extensionPath );
            xhpFileNameComplete.append( '/' + xhpFileName );
            xhpFile = fs::path( xhpFileNameComplete );
        }
        else
        {
            langsourceRoot.append('/' + lang + '/');
            xhpFile = fs::path(xhpFileName, fs::native);
        }

        HelpCompiler hc( streamTable, xhpFile, langsourceRoot,
            embeddStylesheet, module, lang, bExtensionMode );

        HCDBG(std::cerr << "before compile of " << xhpFileName << std::endl);
        bool success = hc.compile();
        HCDBG(std::cerr << "after compile of " << xhpFileName << std::endl);

        if (!success && !bExtensionMode)
        {
            std::stringstream aStrStream;
            aStrStream <<
                "\nERROR: compiling help particle '"
                    << xhpFileName
                    << "' for language '"
                    << lang
                    << "' failed!";
            throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
        }

        const std::string documentBaseId = streamTable.document_id;
        std::string documentPath = streamTable.document_path;
        if (documentPath.find("/") == 0)
            documentPath = documentPath.substr(1);

        std::string documentJarfile = streamTable.document_module + ".jar";

        std::string documentTitle = streamTable.document_title;
        if (documentTitle.empty())
            documentTitle = "<notitle>";

#if 0
        std::cout << "for " << xhpFileName << " documentBaseId is " << documentBaseId << "\n";
        std::cout << "for " << xhpFileName << " documentPath is " << documentPath << "\n";
        std::cout << "for " << xhpFileName << " documentJarfile is " << documentJarfile << "\n";
        std::cout << "for " << xhpFileName << " documentPath is " << documentTitle << "\n";
#endif

        const std::string& fileB = documentPath;
        const std::string& jarfileB = documentJarfile;
        std::string& titleB = documentTitle;

        // add once this as its own id.
        addBookmark(dbBase, documentPath, fileB, std::string(), jarfileB, titleB);

        // first the database *.db
        // ByteArrayInputStream bais = null;
        // ObjectInputStream ois = null;

        const HashSet *hidlist = streamTable.appl_hidlist;
        if (!hidlist)
            hidlist = streamTable.default_hidlist;
        if (hidlist && !hidlist->empty())
        {
            // now iterate over all elements of the hidlist
            HashSet::const_iterator aEnd = hidlist->end();
            for (HashSet::const_iterator hidListIter = hidlist->begin();
                hidListIter != aEnd; ++hidListIter)
            {
                std::string thishid = *hidListIter;

                std::string anchorB;
                size_t index = thishid.rfind('#');
                if (index != std::string::npos)
                {
                    anchorB = thishid.substr(1 + index);
                    thishid = thishid.substr(0, index);
                }
                addBookmark(dbBase, thishid, fileB, anchorB, jarfileB, titleB);
            }
        }

        // now the keywords
        const Hashtable *anchorToLL = streamTable.appl_keywords;
        if (!anchorToLL)
            anchorToLL = streamTable.default_keywords;
        if (anchorToLL && !anchorToLL->empty())
        {
            std::string fakedHid = URLEncoder::encode(documentPath);
            Hashtable::const_iterator aEnd = anchorToLL->end();
            for (Hashtable::const_iterator enumer = anchorToLL->begin();
                enumer != aEnd; ++enumer)
            {
                const std::string &anchor = enumer->first;
                addBookmark(dbBase, documentPath, fileB,
                    anchor, jarfileB, titleB);
                std::string totalId = fakedHid + "#" + anchor;
                // std::cerr << hzipFileName << std::endl;
                const LinkedList& ll = enumer->second;
                LinkedList::const_iterator aOtherEnd = ll.end();
                for (LinkedList::const_iterator llIter = ll.begin();
                    llIter != aOtherEnd; ++llIter)
                {
                        helpKeyword.insert(*llIter, totalId);
                }
            }

        }

        // and last the helptexts
        const Stringtable *helpTextHash = streamTable.appl_helptexts;
        if (!helpTextHash)
            helpTextHash = streamTable.default_helptexts;
        if (helpTextHash && !helpTextHash->empty())
        {
            Stringtable::const_iterator aEnd = helpTextHash->end();
            for (Stringtable::const_iterator helpTextIter = helpTextHash->begin();
                helpTextIter != aEnd; ++helpTextIter)
            {
                std::string helpTextId = helpTextIter->first;
                const std::string& helpTextText = helpTextIter->second;

                std::string temp = helpTextId;
                std::transform (temp.begin(), temp.end(), temp.begin(), toupper);
                std::replace(temp.begin(), temp.end(), ':', '_');

                const std::string& tHid = hidlistTranslation[temp];
                if (!tHid.empty())
                    helpTextId = tHid;
                helpTextId = URLEncoder::encode(helpTextId);

                DBT keyDbt;
                memset(&keyDbt, 0, sizeof(keyDbt));
                keyDbt.data = const_cast<char*>(helpTextId.c_str());
                keyDbt.size = helpTextId.length();

                DBT textDbt;
                memset(&textDbt, 0, sizeof(textDbt));
                textDbt.data = const_cast<char*>(helpTextText.c_str());
                textDbt.size = helpTextText.length();
                helpText->put(helpText, NULL, &keyDbt, &textDbt, 0);
            }
        }

        //IndexerPreProcessor
        if( !bExtensionMode || bIndexForExtension )
        {
            // now the indexing
            xmlDocPtr document = streamTable.appl_doc;
            if (!document)
                document = streamTable.default_doc;
            if (document)
            {
                std::string temp = module;
                std::transform (temp.begin(), temp.end(), temp.begin(), tolower);
                m_pIndexerPreProcessor->processDocument(document, URLEncoder::encode(documentPath) );
            }
        }

    } // while loop over hzip files ending
    if( !bExtensionMode )
        std::cout << std::endl;

    }   // try
    catch( HelpProcessingException& )
    {
        // catch HelpProcessingException to avoid locking data bases
        helpText->close(helpText, 0);
        dbBase->close(dbBase, 0);
        keyWord->close(keyWord, 0);
        throw;
    }

    helpText->close(helpText, 0);
    dbBase->close(dbBase, 0);
    helpKeyword.dump(keyWord);
    keyWord->close(keyWord, 0);

    if( !bExtensionMode )
    {
        // New index
        Stringtable::iterator aEnd = additionalFiles.end();
        for (Stringtable::iterator enumer = additionalFiles.begin(); enumer != aEnd;
            ++enumer)
        {
            const std::string &additionalFileName = enumer->second;
            const std::string &additionalFileKey = enumer->first;

            fs::path fsAdditionalFileName( additionalFileName, fs::native );
                std::string aNativeStr = fsAdditionalFileName.native_file_string();
                const char* pStr = aNativeStr.c_str();
                std::cerr << pStr;

            fs::path fsTargetName( indexDirParentName / additionalFileKey );

            fs::copy( fsAdditionalFileName, fsTargetName );
        }
    }

/*
    /////////////////////////////////////////////////////////////////////////
    /// remove temprary directory for index creation
    /////////////////////////////////////////////////////////////////////////
#ifndef CMC_DEBUG
    if( !bExtensionMode )
        fs::remove_all( indexDirParentName );
#endif
*/
}


void HelpLinker::main(std::vector<std::string> &args, std::string* pExtensionPath)
    throw( HelpProcessingException )
{
    bExtensionMode = false;
    if( pExtensionPath && pExtensionPath->length() > 0 )
    {
        helpFiles.clear();
        bExtensionMode = true;
        extensionPath = *pExtensionPath;
        sourceRoot = fs::path(extensionPath);
    }
    if (args.size() > 0 && args[0][0] == '@')
    {
        std::vector<std::string> stringList;
        std::string strBuf;
        std::ifstream fileReader(args[0].substr(1).c_str());

        while (fileReader)
        {
            std::string token;
            fileReader >> token;
            if (!token.empty())
                stringList.push_back(token);
        }
        fileReader.close();

        args = stringList;
    }

    size_t i = 0;

    while (i < args.size())
    {
        if (args[i].compare("-src") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "sourceroot missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            if( !bExtensionMode )
                sourceRoot = fs::path(args[i], fs::native);
        }
        else if (args[i].compare("-sty") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "embeddingStylesheet missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            embeddStylesheet = fs::path(args[i], fs::native);
        }
        else if (args[i].compare("-zipdir") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "idxtemp missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            zipdir = fs::path(args[i], fs::native);
        }
        else if (args[i].compare("-idxcaption") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "idxcaption stylesheet missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            idxCaptionStylesheet = fs::path(args[i], fs::native);
        }
        else if (args[i].compare("-idxcontent") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "idxcontent stylesheet missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            idxContentStylesheet = fs::path(args[i], fs::native);
        }
        else if (args[i].compare("-o") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "outputfilename missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            outputFile = fs::path(args[i], fs::native);
        }
        else if (args[i].compare("-mod") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "module name missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            module = args[i];
        }
        else if (args[i].compare("-lang") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "language name missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            lang = args[i];
        }
        else if (args[i].compare("-hid") == 0)
        {
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "hid list missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            hid = args[i];
        }
        else if (args[i].compare("-add") == 0)
        {
            std::string addFile, addFileUnderPath;
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "pathname missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }

            addFileUnderPath = args[i];
            ++i;
            if (i >= args.size())
            {
                std::stringstream aStrStream;
                aStrStream << "pathname missing" << std::endl;
                throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
            }
            addFile = args[i];
            if (!addFileUnderPath.empty() && !addFile.empty())
                additionalFiles[addFileUnderPath] = addFile;
        }
        else
            helpFiles.push_back(args[i]);
        ++i;
    }

    if (!bExtensionMode && zipdir.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "no index dir given" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (!bExtensionMode && idxCaptionStylesheet.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "no index caption stylesheet given" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (!bExtensionMode && idxContentStylesheet.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "no index content stylesheet given" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (!bExtensionMode && embeddStylesheet.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "no embedding resolving file given" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (sourceRoot.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "no sourceroot given" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (!bExtensionMode && outputFile.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "no output file given" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (module.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "module missing" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (!bExtensionMode && lang.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "language missing" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }
    if (!bExtensionMode && hid.empty())
    {
        std::stringstream aStrStream;
        aStrStream << "hid list missing" << std::endl;
        throw HelpProcessingException( HELPPROCESSING_GENERAL_ERROR, aStrStream.str() );
    }

    link();
}

int main(int argc, char**argv)
{
    sal_uInt32 starttime = osl_getGlobalTimer();
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i)
        args.push_back(std::string(argv[i]));
    try
    {
        HelpLinker* pHelpLinker = new HelpLinker();
        pHelpLinker->main( args );
        delete pHelpLinker;
    }
    catch( const HelpProcessingException& e )
    {
        std::cerr << e.m_aErrorMsg;
        exit(1);
    }
    sal_uInt32 endtime = osl_getGlobalTimer();
    std::cout << "time taken was " << (endtime-starttime)/1000.0 << " seconds" << std::endl;
    return 0;
}

// Variable to set an exception in "C" StructuredXMLErrorFunction
static const HelpProcessingException* GpXMLParsingException = NULL;

extern "C" void StructuredXMLErrorFunction(void *userData, xmlErrorPtr error)
{
    (void)userData;
    (void)error;

    std::string aErrorMsg = error->message;
    std::string aXMLParsingFile;
    if( error->file != NULL )
        aXMLParsingFile = error->file;
    int nXMLParsingLine = error->line;
    HelpProcessingException* pException = new HelpProcessingException( aErrorMsg, aXMLParsingFile, nXMLParsingLine );
    GpXMLParsingException = pException;

    // Reset error handler
    xmlSetStructuredErrorFunc( NULL, NULL );
}

HelpProcessingErrorInfo& HelpProcessingErrorInfo::operator=( const struct HelpProcessingException& e )
{
    m_eErrorClass = e.m_eErrorClass;
    rtl::OString tmpErrorMsg( e.m_aErrorMsg.c_str() );
    m_aErrorMsg = rtl::OStringToOUString( tmpErrorMsg, fs::getThreadTextEncoding() );
    rtl::OString tmpXMLParsingFile( e.m_aXMLParsingFile.c_str() );
    m_aXMLParsingFile = rtl::OStringToOUString( tmpXMLParsingFile, fs::getThreadTextEncoding() );
    m_nXMLParsingLine = e.m_nXMLParsingLine;
    return *this;
}


// Returns true in case of success, false in case of error
HELPLINKER_DLLPUBLIC bool compileExtensionHelp
(
    const rtl::OUString& aExtensionName,
    const rtl::OUString& aExtensionLanguageRoot,
    sal_Int32 nXhpFileCount, const rtl::OUString* pXhpFiles,
    HelpProcessingErrorInfo& o_rHelpProcessingErrorInfo
)
{
    bool bSuccess = true;

    sal_Int32 argc = nXhpFileCount + 3;
    const char** argv = new const char*[argc];
    argv[0] = "";
    argv[1] = "-mod";
    rtl::OString aOExtensionName = rtl::OUStringToOString( aExtensionName, fs::getThreadTextEncoding() );
    argv[2] = aOExtensionName.getStr();

    for( sal_Int32 iXhp = 0 ; iXhp < nXhpFileCount ; ++iXhp )
    {
        rtl::OUString aXhpFile = pXhpFiles[iXhp];

        rtl::OString aOXhpFile = rtl::OUStringToOString( aXhpFile, fs::getThreadTextEncoding() );
        char* pArgStr = new char[aOXhpFile.getLength() + 1];
        strcpy( pArgStr, aOXhpFile.getStr() );
        argv[iXhp + 3] = pArgStr;
    }

    std::vector<std::string> args;
    for( sal_Int32 i = 1; i < argc; ++i )
        args.push_back(std::string( argv[i]) );

    for( sal_Int32 iXhp = 0 ; iXhp < nXhpFileCount ; ++iXhp )
        delete argv[iXhp + 3];
    delete[] argv;

    rtl::OString aOExtensionLanguageRoot = rtl::OUStringToOString( aExtensionLanguageRoot, fs::getThreadTextEncoding() );
    const char* pExtensionPath = aOExtensionLanguageRoot.getStr();
    std::string aStdStrExtensionPath = pExtensionPath;

    // Set error handler
    xmlSetStructuredErrorFunc( NULL, (xmlStructuredErrorFunc)StructuredXMLErrorFunction );
    try
    {
        HelpLinker* pHelpLinker = new HelpLinker();
        pHelpLinker->main( args,&aStdStrExtensionPath );
        delete pHelpLinker;
    }
    catch( const HelpProcessingException& e )
    {
        if( GpXMLParsingException != NULL )
        {
            o_rHelpProcessingErrorInfo = *GpXMLParsingException;
            delete GpXMLParsingException;
            GpXMLParsingException = NULL;
        }
        else
        {
            o_rHelpProcessingErrorInfo = e;
        }
        bSuccess = false;
    }
    // Reset error handler
    xmlSetStructuredErrorFunc( NULL, NULL );

    // i83624: Tree files
    ::rtl::OUString aTreeFileURL = aExtensionLanguageRoot;
    aTreeFileURL += rtl::OUString::createFromAscii( "/help.tree" );
    osl::DirectoryItem aTreeFileItem;
    osl::FileBase::RC rcGet = osl::DirectoryItem::get( aTreeFileURL, aTreeFileItem );
    osl::FileStatus aFileStatus( FileStatusMask_FileSize );
    if( rcGet == osl::FileBase::E_None &&
        aTreeFileItem.getFileStatus( aFileStatus ) == osl::FileBase::E_None &&
        aFileStatus.isValid( FileStatusMask_FileSize ) )
    {
        sal_uInt64 ret, len = aFileStatus.getFileSize();
        char* s = new char[ int(len) ];  // the buffer to hold the installed files
        osl::File aFile( aTreeFileURL );
        aFile.open( OpenFlag_Read );
        aFile.read( s, len, ret );
        aFile.close();

        XML_Parser parser = XML_ParserCreate( 0 );
        int parsed = XML_Parse( parser, s, int( len ), true );

        if( parsed == 0 )
        {
            XML_Error nError = XML_GetErrorCode( parser );
            o_rHelpProcessingErrorInfo.m_eErrorClass = HELPPROCESSING_XMLPARSING_ERROR;
            o_rHelpProcessingErrorInfo.m_aErrorMsg = rtl::OUString::createFromAscii( XML_ErrorString( nError ) );;
            o_rHelpProcessingErrorInfo.m_aXMLParsingFile = aTreeFileURL;
            // CRAHSES!!! o_rHelpProcessingErrorInfo.m_nXMLParsingLine = XML_GetCurrentLineNumber( parser );
            bSuccess = false;
        }

        XML_ParserFree( parser );
        delete[] s;
    }

    return bSuccess;
}

// vnd.sun.star.help://swriter/52821?Language=en-US&System=UNIX
/* vi:set tabstop=4 shiftwidth=4 expandtab: */

