/*************************************************************************
 *
 *  $RCSfile: cmduicollector.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: cd $ $Date: 2004-01-26 16:40:45 $
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

#include <sal/main.h>
#include <osl/file.hxx>
#include <osl/thread.h>
#include <osl/process.h>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/string.hxx>

#include <rsc/rscsfx.hxx>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/servicefactory.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <tools/resmgr.hxx>
#include <tools/rc.h>
#include <tools/isolang.hxx>
#include <vcl/rc.hxx>
#include <vcl/menu.hxx>
#include <vcl/svapp.hxx>

#include <svx/svxids.hrc>
#include <svx/dialogs.hrc>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include <hash_map>
#include <vector>

using namespace rtl;

// Menubars
const unsigned short MENUBAR_GLOBAL             = 261;
const unsigned short MENUBAR_BASIC              = 14851;
const unsigned short MENUBAR_WRITER             = 20016;
const unsigned short MENUBAR_WRITERWEB          = 20019;
const unsigned short MENUBAR_WRITERGLOBAL       = 20029;
const unsigned short MENUBAR_CALC               = 26007;
const unsigned short MENUBAR_DRAW               = 23009;
const unsigned short MENUBAR_CHART              = 20081;
const unsigned short MENUBAR_MATH               = 20814;
const unsigned short MENUBAR_IMPRESS            = 23006;
const unsigned short MENUBAR_BIBLIOGRAPHY       = 15901;
const unsigned short MENUBAR_BACKINGCOMP        = 261;
const unsigned short MENUBAR_DBACCESS_TABLE     = 19210;
const unsigned short MENUBAR_DBACCESS_QUERY     = 19211;
const unsigned short MENUBAR_DBACCESS_RELATION  = 19212;


struct ConvertIDToISO
{
    int         nLangID;
    const char* pISO;
};

const int NUM_LANGUAGES = 27;
//const int NUM_LANGUAGES = 2;

ConvertIDToISO Language_Mapping[] =
{
    { 49, "de"        },
    { 1,  "en-US"     },
    { 96, "ar"        },
    { 47, "no"        },
    { 37, "ca"        },
    { 88, "zh-TW"     },
    { 42, "cs"        },
    { 45, "da"        },
    { 30, "el"        },
    { 34, "es"        },
    { 35, "fi"        },
    { 33, "fr"        },
    { 97, "he"        },
    { 39, "it"        },
    { 81, "ja"        },
    { 82, "ko"        },
    { 31, "nl"        },
    { 48, "pl"        },
    { 55, "pt-BR"     },
    { 7,  "ru"        },
    { 43, "sk"        },
    { 46, "sv"        },
    { 66, "th"        },
    { 90, "tr"        },
    { 91, "hi"        },
    { 86, "zh-CN"     },
    { 3,  "pt"        },
    { 0,  0           }
};

enum MODULES
{
    MODULE_GLOBAL,
    MODULE_WRITER,
    MODULE_CALC,
    MODULE_DRAWIMPRESS,
    MODULE_CHART,
    MODULE_MATH,
    MODULE_BASIC,
    MODULE_BIBLIO,
    MODULE_BACKINGCOMP,
    MODULE_DBACCESS,
    MODULE_COUNT
};

struct Projects
{
    const char* pProjectFolder;
    const char* pResPrefix;
    const char* pCSVPrefix;
    bool        bSlotCommands;
    MODULES     eBelongsTo;
};

Projects ProjectModule_Mapping[] =
{
    { "sfx2"        , "sfx",    "sfx2",     true,   MODULE_GLOBAL         },
    { "svx"         , "svx",    "svx",      true,   MODULE_GLOBAL         },
    { "offmgr"      , "ofa",    "ofa",      true,   MODULE_GLOBAL         },
    { "sw"          , "sw",     "sw",       true,   MODULE_WRITER         },
    { "sd"          , "sd",     "sd",       true,   MODULE_DRAWIMPRESS    },
    { "sc"          , "sc",     "sc",       true,   MODULE_CALC           },
    { "sch"         , "sch",    "sch",      true,   MODULE_CHART          },
    { "starmath"    , "sm",     "starmath", true,   MODULE_MATH           },
    { "basctl"      , "basctl", "bastctl",  true,   MODULE_BASIC          },
    { "extensions"  , "bib",    "",         false,  MODULE_BIBLIO         },
    { "offmgr"      , "ofa",    "",         false,  MODULE_BACKINGCOMP    },
    { "dbaccess"    , "dbu",    "",         false,  MODULE_DBACCESS       },
    { 0             , 0,        "",         false,  MODULE_BASIC          }
};


const char XMLFileExtension[] = ".xcu";
const char* ModuleToXML_Mapping[] =
{
    "GenericCommands",
    "WriterCommands",
    "CalcCommands",
    "DrawImpressCommands",
    "ChartCommands",
    "MathCommands",
    "BasicIDECommands",
    "BibliographyCommands",
    "StartModuleCommands",
    "DbAccessCommands",
    0
};

struct MenuResourceToModule
{
    unsigned short nResId;
    const char*    pXMLPrefix;
    MODULES        eBelongsTo;
};

MenuResourceToModule ResourceModule_Mapping[] =
{
    { MENUBAR_GLOBAL,           "default",      MODULE_GLOBAL       },  // important: To prevent duplicate popup menu entries make sure this is the first entry!!
    { MENUBAR_BASIC,            "basic",        MODULE_BASIC        },
    { MENUBAR_WRITER,           "writer",       MODULE_WRITER       },
    { MENUBAR_WRITERWEB,        "web",          MODULE_WRITER       },
    { MENUBAR_WRITERGLOBAL,     "global",       MODULE_WRITER       },
    { MENUBAR_CALC,             "calc",         MODULE_CALC         },
    { MENUBAR_DRAW,             "draw",         MODULE_DRAWIMPRESS  },
    { MENUBAR_CHART,            "chart",        MODULE_CHART        },
    { MENUBAR_MATH,             "math",         MODULE_MATH         },
    { MENUBAR_IMPRESS,          "impress",      MODULE_DRAWIMPRESS  },
    { MENUBAR_BIBLIOGRAPHY,     "biblio",       MODULE_BIBLIO       },
    { MENUBAR_BACKINGCOMP,      "backing",      MODULE_BACKINGCOMP  },
    { MENUBAR_DBACCESS_TABLE,   "dbtable",      MODULE_DBACCESS     },
    { MENUBAR_DBACCESS_QUERY,   "dbquery",      MODULE_DBACCESS     },
    { MENUBAR_DBACCESS_RELATION,"dbrelation",   MODULE_DBACCESS     },
    { 0,                        "",             MODULE_DBACCESS     }
};

struct CommandLabels
{
    unsigned short  nID;
    bool            bWritten;
    bool            bPopupMenu;
    rtl::OUString   aCommand;
    unsigned long   nModules;
    rtl::OUString   aLabels[NUM_LANGUAGES];

    CommandLabels() :
    nID( 0 ),
    bWritten( false ),
    bPopupMenu( false ),
    nModules( 0 ) {}
};

struct OUStringHashCode
{
    size_t operator()( const ::rtl::OUString& sString ) const
    {
        return sString.hashCode();
    }
};

typedef std::hash_map< int, CommandLabels > CommandIDToLabelsMap;
typedef std::hash_map< OUString, CommandLabels, OUStringHashCode, ::std::equal_to< OUString > > CommandToLabelsMap;

static CommandIDToLabelsMap moduleMapFiles[MODULE_COUNT];
static CommandToLabelsMap   modulePopupMenusCmd[MODULE_COUNT];

bool ExtractVersionNumber( const OUString& rVersion, OUString& rVersionNumber )
{
    bool bCheckNumOnly( false );

    OUStringBuffer aBuf;

    rVersionNumber = OUString();
    for ( int i = 0; i < rVersion.getLength(); i++ )
    {
        if ( rVersion[i] >= sal_Unicode( '0' ) && rVersion[i] <= sal_Unicode( '9' ))
        {
            bCheckNumOnly = true;
            aBuf.append( rVersion[i] );
        }
        else if ( bCheckNumOnly )
            return false;
    }

    rVersionNumber = aBuf.makeStringAndClear();
    return true;
}

bool ReadCSVFile( const OUString& aCVSFileURL, MODULES eModule, const OUString& aProjectName )
{
    osl::File  aCSVFile( aCVSFileURL );

    if ( aCSVFile.open( OpenFlag_Read ) == osl::FileBase::E_None )
    {
        sal_Bool            bEOF;
        ::rtl::ByteSequence aCSVLine;
        OUString            aUnoCmd( RTL_CONSTASCII_USTRINGPARAM( ".uno:" ));

        while ( aCSVFile.isEndOfFile( &bEOF ) == osl::FileBase::E_None && !bEOF )
        {
            aCSVFile.readLine( aCSVLine );

            OString     aLine( (const char *)aCSVLine.getConstArray(), aCSVLine.getLength() );
            OString     aID;
            OString     aAccelState;
            OString     aMenuState;
            OString     aStatusState;
            OString     aToolbarState;
            OString     aCmdName;

            sal_Int32 nIndex = 0;
            aID              = aLine.getToken( 2, ',', nIndex );
            aAccelState      = aLine.getToken( 2, ',', nIndex );
            aMenuState       = aLine.getToken( 0, ',', nIndex );
            aStatusState     = aLine.getToken( 0, ',', nIndex );
            aToolbarState    = aLine.getToken( 0, ',', nIndex );
            aCmdName         = aLine.getToken( 20, ',', nIndex );

            int nID = aID.toInt32();

            if ( nID > 5000 && ( aAccelState.equalsIgnoreAsciiCase( "TRUE" ) ||
                                 aMenuState.equalsIgnoreAsciiCase( "TRUE" ) ||
                                 aStatusState.equalsIgnoreAsciiCase( "TRUE" ) ||
                                 aToolbarState.equalsIgnoreAsciiCase( "TRUE" ) ))
            {
                CommandLabels aCmdLabel;

                aCmdLabel.nID = nID;
                aCmdLabel.aCommand = aUnoCmd;
                aCmdLabel.aCommand += OStringToOUString( aCmdName, RTL_TEXTENCODING_ASCII_US );
                aCmdLabel.nModules |= ( 1 << (unsigned long)( eModule ));
                moduleMapFiles[int(eModule)].insert( CommandIDToLabelsMap::value_type( nID, aCmdLabel ));
                modulePopupMenusCmd[int(eModule)].insert( CommandToLabelsMap::value_type( aCmdLabel.aCommand, aCmdLabel ));
            }
        }

        aCSVFile.close();
    }

    return true;
}

CommandLabels* RetrieveCommandLabelsFromID( unsigned short nId, MODULES eModule )
{
    CommandIDToLabelsMap::iterator pIter = moduleMapFiles[MODULE_GLOBAL].find( nId );
    if ( pIter != moduleMapFiles[MODULE_GLOBAL].end() )
        return &(pIter->second);
    else if ( eModule != MODULE_GLOBAL )
    {
        CommandIDToLabelsMap::iterator pIter = moduleMapFiles[eModule].find( nId );
        if ( pIter != moduleMapFiles[eModule].end() )
            return &(pIter->second);
    }

    return NULL;
}

CommandLabels* RetrieveCommandLabelsFromCommand( const OUString& aCommand, MODULES eModule )
{
    CommandToLabelsMap::iterator pIter = modulePopupMenusCmd[MODULE_GLOBAL].find( aCommand );
    if ( pIter != modulePopupMenusCmd[MODULE_GLOBAL].end() )
        return &(pIter->second);
    else if ( eModule != MODULE_GLOBAL )
    {
        CommandToLabelsMap::iterator pIter = modulePopupMenusCmd[eModule].find( aCommand );
        if ( pIter != modulePopupMenusCmd[eModule].end() )
            return &(pIter->second);
    }

    return NULL;
}

void AddCommandLabelsToIDHashMap( MODULES eModule, const CommandLabels& rNewCmdLabels )
{
    moduleMapFiles[int(eModule)].insert( CommandIDToLabelsMap::value_type( rNewCmdLabels.nID, rNewCmdLabels ));
}

void AddCommandLabelsToCommandHashMap( MODULES eModule, const CommandLabels& rNewCmdLabels )
{
    modulePopupMenusCmd[int(eModule)].insert( CommandToLabelsMap::value_type( rNewCmdLabels.aCommand, rNewCmdLabels ));
}

bool ReadMenuLabels( int nLangIndex, Menu* pMenu, MODULES eModule, bool bHasSlotInfos, OUString aResourceFilePrefix )
{
    OUString aCmd( RTL_CONSTASCII_USTRINGPARAM( ".uno:" ));

    CommandIDToLabelsMap::const_iterator pIter = moduleMapFiles[MODULE_GLOBAL].find( 5510 );
    if ( pIter != moduleMapFiles[MODULE_GLOBAL].end() )
    {
        if (( pIter->second.aLabels[2].getLength() == 0 ) || ( pIter->second.aLabels[2].equalsAscii( "~File" )))
        {
            int a = 1;
        }
    }

    for ( unsigned short n = 0; n < pMenu->GetItemCount(); n++ )
    {
        unsigned short nId = pMenu->GetItemId( n );
        if ( pMenu->GetItemType( n ) != MENUITEM_SEPARATOR )
        {
            OUString aLabel( pMenu->GetItemText( nId ));
            OUString aCommand( pMenu->GetItemCommand( nId ));
            bool     bHasUnoCommand( aCommand.getLength() > 0 && ( aCommand.indexOf( aCmd ) == 0 ));

            if ( pMenu->GetPopupMenu( nId ) != 0 )
            {
                CommandLabels* pCmdLabels = 0;
                pCmdLabels = RetrieveCommandLabelsFromID( nId, eModule );
                if ( !pCmdLabels )
                    pCmdLabels = RetrieveCommandLabelsFromCommand( aCommand, eModule );

                if ( pCmdLabels )
                {
                    pCmdLabels->aLabels[nLangIndex] = aLabel;
                    pCmdLabels->bPopupMenu = true;
                }
                else
                {
                    if ( bHasUnoCommand )
                    {
                        CommandLabels aCmdLabels;

                        aCmdLabels.nID = 0;
                        aCmdLabels.aCommand = aCommand;
                        aCmdLabels.aLabels[nLangIndex] = aLabel;
                        aCmdLabels.bPopupMenu = true;
                        AddCommandLabelsToCommandHashMap( eModule, aCmdLabels );
                    }
                    else
                    {
                        fprintf( stderr, "Popup menu (resource = %s ) with unknown uno-command and id '%s', ID = %d, aCmd = %s\n",
                            OUStringToOString( aResourceFilePrefix, RTL_TEXTENCODING_UTF8 ).getStr(),
                            OUStringToOString( aLabel, RTL_TEXTENCODING_UTF8 ).getStr(),
                            nId,
                            OUStringToOString( aCommand, RTL_TEXTENCODING_ASCII_US ).getStr() );
                    }
                }

                ReadMenuLabels( nLangIndex, pMenu->GetPopupMenu( nId ), eModule, bHasSlotInfos, aResourceFilePrefix );
            }
            else
            {
                if ( bHasSlotInfos )
                {
                    CommandLabels* pCmdLabels = RetrieveCommandLabelsFromID( nId, eModule );
                    if ( !pCmdLabels )
                        pCmdLabels = RetrieveCommandLabelsFromCommand( aCommand, eModule );

                    if ( pCmdLabels )
                        pCmdLabels->aLabels[nLangIndex] = aLabel;
                    else if ( bHasUnoCommand )
                    {
                        CommandLabels aCmdLabels;

                        aCmdLabels.nID = ( nId > 5000 ) ? nId : 0;
                        aCmdLabels.aCommand = aCommand;
                        aCmdLabels.aLabels[nLangIndex] = aLabel;
                        AddCommandLabelsToCommandHashMap( eModule, aCmdLabels );
                        if ( nId > 5000 )
                            AddCommandLabelsToIDHashMap( eModule, aCmdLabels );
                   }
                   else
                   {
                        fprintf( stderr, "Menu item with unknown uno-command and id '%s', ID = %d, aCmd = %s\n",
                            OUStringToOString( aLabel, RTL_TEXTENCODING_UTF8 ).getStr(),
                            nId,
                            OUStringToOString( aCommand, RTL_TEXTENCODING_ASCII_US ).getStr() );
                   }
                }
                else if ( bHasUnoCommand )
                {
                    CommandLabels* pCmdLabels = RetrieveCommandLabelsFromCommand( aCommand, eModule );
                    if ( pCmdLabels )
                        pCmdLabels->aLabels[nLangIndex] = aLabel;
                    else
                    {
                        CommandLabels aCmdLabels;

                        aCmdLabels.nID = 0;
                        aCmdLabels.aCommand = aCommand;
                        aCmdLabels.aLabels[nLangIndex] = aLabel;
                        AddCommandLabelsToCommandHashMap( eModule, aCmdLabels );
                    }
                }
            }
        }
    }

    return true;
}

class SfxSlotInfo : public Resource
{
    OUString aName;
    OUString aHelpText;
public:
    SfxSlotInfo(const ResId &rResId);

    const OUString &GetName() const { return aName; }
    const OUString &GetHelpText() const { return aHelpText; }
};

SfxSlotInfo::SfxSlotInfo( const ResId &rResId ) :
    Resource( rResId.SetRT( RSC_SFX_SLOT_INFO ) )
{
    unsigned short nMask = (unsigned short) ReadShortRes();
    if(nMask & RSC_SFX_SLOT_INFO_SLOTNAME)
    {
        aName = ReadStringRes();
    }
    if(nMask & RSC_SFX_SLOT_INFO_HELPTEXT)
    {
        aHelpText = ReadStringRes();
    }
}

bool ReadSlotInfos( int nLangIndex, ResMgr* pResMgr, MODULES eModule, const OUString& aProjectName )
{
    CommandIDToLabelsMap::const_iterator pTestIter = moduleMapFiles[MODULE_GLOBAL].find( 5510 );
    CommandIDToLabelsMap::iterator pIter = moduleMapFiles[MODULE_GLOBAL].begin();;

    if ( eModule != MODULE_GLOBAL )
    {
        while ( pIter != moduleMapFiles[eModule].end() )
        {
            int nId = pIter->first;
            CommandLabels& rCmdLabels = pIter->second;

            ResId aResId( nId, pResMgr );
            aResId.SetRT( RSC_SFX_SLOT_INFO );

            if ( pResMgr->IsAvailable( aResId ))
            {
                SfxSlotInfo aSlotInfo( aResId );
                OUString    aText( aSlotInfo.GetName() );
                if ( rCmdLabels.aLabels[nLangIndex].getLength() == 0 )
                    rCmdLabels.aLabels[nLangIndex] = aText;
            }
            ++pIter;
        }
    }

    pIter = moduleMapFiles[eModule].begin();
    while ( pIter != moduleMapFiles[eModule].end() )
    {
        int nId = pIter->first;
        CommandLabels& rCmdLabels = pIter->second;

        ResId aResId( nId, pResMgr );
        aResId.SetRT( RSC_SFX_SLOT_INFO );

        if ( pResMgr->IsAvailable( aResId ))
        {
            SfxSlotInfo aSlotInfo( aResId );
            OUString    aText( aSlotInfo.GetName() );
            if ( rCmdLabels.aLabels[nLangIndex].getLength() == 0 )
                rCmdLabels.aLabels[nLangIndex] = aText;
        }
        ++pIter;
    }

    return true;
}

bool ReadResourceFile( int nLangIndex, const OUString& aResourceDirURL, const OUString& aResourceFilePrefix, MODULES eModule, const OUString& aProjectName, bool bHasSlotInfos )
{
    OUString aSysDirPath;
    OString  aResFilePrefix = OUStringToOString( aResourceFilePrefix, RTL_TEXTENCODING_ASCII_US );

    osl::FileBase::getSystemPathFromFileURL( aResourceDirURL, aSysDirPath );

    String aSysDirPathStr( aSysDirPath );

    String aLangString( String::CreateFromAscii( Language_Mapping[nLangIndex].pISO ));
    LanguageType aLangtype = ConvertIsoStringToLanguage( aLangString );
    ResMgr* pResMgr = ResMgr::CreateResMgr( aResFilePrefix.getStr(),
                                            aLangtype,
                                            NULL,
                                            &aSysDirPathStr );
    if ( pResMgr )
    {
        int i = 0;
        while ( ResourceModule_Mapping[i].nResId > 0 )
        {
            if ( ResourceModule_Mapping[i].eBelongsTo == eModule )
            {
                ResId   aResId( ResourceModule_Mapping[i].nResId, pResMgr );
                aResId.SetRT( RSC_MENU );

                if ( pResMgr->IsAvailable( aResId ))
                {
                    MenuBar* pMenuBar = new MenuBar( aResId );
                    ReadMenuLabels( nLangIndex, (Menu *)pMenuBar, eModule, bHasSlotInfos, aResourceFilePrefix );
                    delete pMenuBar;
                }

                if ( bHasSlotInfos )
                    ReadSlotInfos( nLangIndex, pResMgr, eModule, aProjectName );
            }
            ++i;
        }

        delete pResMgr;
    }

    return true;
}

static const char ENCODED_AMPERSAND[]  = "&amp;";
static const char ENCODED_LESS[]       = "&lt;";
static const char ENCODED_GREATER[]    = "&gt;";
static const char ENCODED_QUOTE[]      = "&quot;";
static const char ENCODED_APOS[]       = "&apos;";

struct EncodeChars
{
    char  cChar;
    const char* pEncodedChars;
};

EncodeChars EncodeChar_Map[] =
{
    { '&', ENCODED_AMPERSAND    },
    { '<', ENCODED_LESS         },
    { '>', ENCODED_GREATER      },
    { '"', ENCODED_QUOTE        },
    { '\'', ENCODED_APOS        },
    { ' ', 0                    }
};

// Encodes a string to UTF-8 and uses "Built-in entity reference" to
// to escape character data that is not markup!
OString EncodeString( const OUString& aToEncodeStr )
{
    OString aString = OUStringToOString( aToEncodeStr, RTL_TEXTENCODING_UTF8 );

    int     i = 0;
    bool    bMustCopy( sal_False );
    while ( EncodeChar_Map[i].pEncodedChars != 0 )
    {
        OStringBuffer aBuf;
        bool bEncoded( false );

        sal_Int32 nCurIndex = 0;
        sal_Int32 nIndex    = 0;
        while ( nIndex < aString.getLength() )
        {
            nIndex = aString.indexOf( EncodeChar_Map[i].cChar, nIndex );
            if ( nIndex > 0 )
            {
                bEncoded = true;
                if ( nIndex > nCurIndex )
                    aBuf.append( aString.copy( nCurIndex, nIndex-nCurIndex ));
                aBuf.append( EncodeChar_Map[i].pEncodedChars );
                nCurIndex = nIndex+1;
            }
            else if ( nIndex == 0 )
            {
                bEncoded = true;
                aBuf.append( EncodeChar_Map[i].pEncodedChars );
                nCurIndex = nIndex+1;
            }
            else
            {
                if ( bEncoded && nCurIndex < aString.getLength() )
                    aBuf.append( aString.copy( nCurIndex ));
                break;
            }
            ++nIndex;
        }

        if ( bEncoded )
            aString = aBuf.makeStringAndClear();
        ++i;
    }

    return aString;
}

bool FindAndMarkPopupMenuCmds( const OUString& aCommand, MODULES eExcludeModule )
{
    bool bFound( sal_False );

    for ( int i = 1; i < int( MODULE_COUNT ); i++ )
    {
        if ( eExcludeModule != MODULES( i ))
        {
            CommandToLabelsMap::iterator pCmdIter = modulePopupMenusCmd[i].find( aCommand );
            if ( pCmdIter != modulePopupMenusCmd[i].end() )
            {
                CommandLabels& rCmdLabel = pCmdIter->second;
                rCmdLabel.bWritten = true; // mark as written, this is now a global popup menu command
                bFound = true;
            }
        }
    }

    return bFound;
}

bool FindAndMoveGlobalPopupMenus()
{
    for ( int i = 1; i < int( MODULE_COUNT ); i++ )
    {
        CommandToLabelsMap::iterator pCmdIter = modulePopupMenusCmd[i].begin();
        while ( pCmdIter != modulePopupMenusCmd[i].end() )
        {
            CommandLabels& rCmdLabel = pCmdIter->second;
            if ( rCmdLabel.bPopupMenu )
            {
                bool bDuplicate = FindAndMarkPopupMenuCmds( rCmdLabel.aCommand, MODULES( i ));
                if ( bDuplicate )
                {
                    CommandToLabelsMap::const_iterator pIter = modulePopupMenusCmd[MODULE_GLOBAL].find( rCmdLabel.aCommand );
                    if ( pIter == modulePopupMenusCmd[MODULE_GLOBAL].end() )
                    {
                        // Put command into global ui command list
                        CommandLabels aCmdLabels;

                        aCmdLabels.nID = 0;
                        aCmdLabels.aCommand = rCmdLabel.aCommand;
                        aCmdLabels.bPopupMenu = true;
                        for ( int j = 0; j < NUM_LANGUAGES; j++ )
                            aCmdLabels.aLabels[j] = rCmdLabel.aLabels[j];
                        AddCommandLabelsToCommandHashMap( MODULE_GLOBAL, aCmdLabels );
                        rCmdLabel.bWritten = true;
                    }
                    else
                    {
                        rCmdLabel.bWritten = true;
                    }
                }
            }
            ++pCmdIter;
        }
    }

    return true;
}

static const char XMLStart[]        = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

bool WriteXMLFiles( const OUString& aOutputDirURL)
{
    static const char DocType[]                 = "<!DOCTYPE oor:component-data SYSTEM \"../../../../component-update.dtd\">\n";
    static const char ComponentDataStart[]      = "<oor:component-data oor:name=\"";
    static const char ComponentDataStartEnd[]   = "\" oor:package=\"org.openoffice.Office.UI\" xmlns:oor=\"http://openoffice.org/2001/registry\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n";
    static const char GroupText[]               = "\t<node oor:name=\"UserInterface\">\n";
    static const char SetText[]                 = "\t\t<node oor:name=\"Label\">\n";
    static const char NodeStart[]               = "\t\t\t<node oor:name=\"";
    static const char ReplaceOp[]               = "\" oor:op=\"replace\">\n";
    static const char NodeEnd[]                 = "\t\t\t</node>\n";
    static const char PropNodeStart[]           = "\t\t\t\t<prop oor:name=\"Label\" oor:type=\"xs:string\">\n";
    static const char PropNodeEnd[]             = "\t\t\t\t</prop>\n";
    static const char ValueNodeStart[]          = "\t\t\t\t\t<value xml:lang=\"";
    static const char ValueNodeMid[]            = "\">";
    static const char ValueNodeEnd[]            = "</value>\n";
    static const char ValueNodeEmpty[]          = "\t\t\t\t\t<value/>\n";
    static const char SetTextEnd[]              = "\t\t</node>\n";
    static const char GroupTextEnd[]            = "\t</node>\n";
    static const char ComponentDataEnd[]        = "</oor:component-data>\n";

    // Search popup menu commands that can be moved to the global list as they are used in more than one project
    FindAndMoveGlobalPopupMenus();

    OUString aOutputDirectoryURL( aOutputDirURL );
    if ( aOutputDirectoryURL.getLength() > 0 && aOutputDirectoryURL[aOutputDirectoryURL.getLength()-1] != '/' )
        aOutputDirectoryURL += OUString::createFromAscii( "/" );

    int i = 0;
    while ( ModuleToXML_Mapping[i] != 0 )
    {
        OUString aOutputFileURL( aOutputDirectoryURL );
        aOutputFileURL += OUString::createFromAscii( ModuleToXML_Mapping[i] );
        aOutputFileURL += OUString::createFromAscii( XMLFileExtension );

        osl::File aXMLFile( aOutputFileURL );

        osl::File::RC nRet = aXMLFile.open( OpenFlag_Create|OpenFlag_Write );
        if ( nRet == osl::File::E_EXIST )
        {
            nRet = aXMLFile.open( OpenFlag_Write );
            if ( nRet == osl::File::E_None )
                nRet = aXMLFile.setSize( 0 );
        }

        if ( nRet == osl::FileBase::E_None )
        {
            sal_uInt64 nWritten;

            aXMLFile.write( XMLStart, strlen( XMLStart ), nWritten );
            aXMLFile.write( DocType, strlen( DocType ), nWritten );
            aXMLFile.write( ComponentDataStart, strlen( ComponentDataStart ), nWritten );
            aXMLFile.write( ModuleToXML_Mapping[i], strlen( ModuleToXML_Mapping[i] ), nWritten );
            aXMLFile.write( ComponentDataStartEnd, strlen( ComponentDataStartEnd ), nWritten );
            aXMLFile.write( GroupText, strlen( GroupText ), nWritten );
            aXMLFile.write( SetText, strlen( SetText ), nWritten );

            if (( moduleMapFiles[i].size() > 0      ) ||
                ( modulePopupMenusCmd[i].size() > 0 )    )
            {
                CommandIDToLabelsMap::const_iterator pIter = moduleMapFiles[i].begin();
                while ( pIter != moduleMapFiles[i].end() )
                {
                    const CommandLabels& rCmdLabels = pIter->second;

                    CommandToLabelsMap::iterator pCmdIter = modulePopupMenusCmd[i].find( rCmdLabels.aCommand );
                    if ( pCmdIter != modulePopupMenusCmd[i].end() )
                    {
                        CommandLabels& rCmdLabel = pCmdIter->second;
                        rCmdLabel.bWritten = true;
                    }

                    // we have to use UTF-8 as encoding
                    OString aCmd = OUStringToOString( rCmdLabels.aCommand, RTL_TEXTENCODING_UTF8 );

                    aXMLFile.write( NodeStart, strlen( NodeStart ), nWritten );
                    aXMLFile.write( aCmd.getStr(), aCmd.getLength(), nWritten );
                    aXMLFile.write( ReplaceOp, strlen( ReplaceOp ), nWritten );
                    aXMLFile.write( PropNodeStart, strlen( PropNodeStart ), nWritten );

                    sal_Bool bLabels( sal_False );
                    for ( int j = 0; j < NUM_LANGUAGES; j++ )
                    {
                        OString aLabel;
                        if ( rCmdLabels.aLabels[j].getLength() > 0 )
                        {
                            bLabels = sal_True;

                            // Encode label to the XML rules.
                            aLabel = EncodeString( rCmdLabels.aLabels[j] );
                        }
                        else
                        {
                            CommandToLabelsMap::const_iterator pIter = modulePopupMenusCmd[i].find( rCmdLabels.aCommand );
                            if ( pIter != modulePopupMenusCmd[i].end() )
                            {
                                const CommandLabels& rCmdLabels = pIter->second;
                                if ( rCmdLabels.aLabels[j].getLength() > 0 )
                                {
                                    bLabels = sal_True;
                                    aLabel = EncodeString( rCmdLabels.aLabels[j] );
                                }
                            }
                        }

                        if ( aLabel.getLength() > 0 )
                        {
                            aXMLFile.write( ValueNodeStart, strlen( ValueNodeStart ), nWritten );
                            aXMLFile.write( Language_Mapping[j].pISO, strlen( Language_Mapping[j].pISO ), nWritten );
                            aXMLFile.write( ValueNodeMid, strlen( ValueNodeMid ), nWritten );
                            aXMLFile.write( aLabel.getStr(), aLabel.getLength(), nWritten );
                            aXMLFile.write( ValueNodeEnd, strlen( ValueNodeEnd ), nWritten );
                        }
                    }

                    if ( !bLabels )
                        aXMLFile.write( ValueNodeEmpty, strlen( ValueNodeEmpty ), nWritten );

                    aXMLFile.write( PropNodeEnd, strlen( PropNodeEnd ), nWritten );
                    aXMLFile.write( NodeEnd, strlen( NodeEnd ), nWritten );
                    ++pIter;
                }

                sal_uInt32 nSize = modulePopupMenusCmd[i].size();
                if ( nSize > 0 )
                {
                    CommandToLabelsMap::const_iterator pIter = modulePopupMenusCmd[i].begin();
                    while ( pIter != modulePopupMenusCmd[i].end() )
                    {
                        const CommandLabels& rCmdLabels = pIter->second;
                        if ( !rCmdLabels.bWritten )
                        {
                            OString aCmd( OUStringToOString( rCmdLabels.aCommand, RTL_TEXTENCODING_UTF8 ));
                            aXMLFile.write( NodeStart, strlen( NodeStart ), nWritten );
                            aXMLFile.write( aCmd.getStr(), aCmd.getLength(), nWritten );
                            aXMLFile.write( ReplaceOp, strlen( ReplaceOp ), nWritten );
                            aXMLFile.write( PropNodeStart, strlen( PropNodeStart ), nWritten );

                            sal_Bool bLabels( sal_False );
                            for ( int k = 0; k < NUM_LANGUAGES; k++ )
                            {
                                if ( rCmdLabels.aLabels[k].getLength() > 0 )
                                {
                                    bLabels = sal_True;
                                    OString aLabel( OUStringToOString( rCmdLabels.aLabels[k], RTL_TEXTENCODING_UTF8 ));

                                    aXMLFile.write( ValueNodeStart, strlen( ValueNodeStart ), nWritten );
                                    aXMLFile.write( Language_Mapping[k].pISO, strlen( Language_Mapping[k].pISO ), nWritten );
                                    aXMLFile.write( ValueNodeMid, strlen( ValueNodeMid ), nWritten );
                                    aXMLFile.write( aLabel.getStr(), aLabel.getLength(), nWritten );
                                    aXMLFile.write( ValueNodeEnd, strlen( ValueNodeEnd ), nWritten );
                                }
                            }

                            if ( !bLabels )
                                aXMLFile.write( ValueNodeEmpty, strlen( ValueNodeEmpty ), nWritten );

                            aXMLFile.write( PropNodeEnd, strlen( PropNodeEnd ), nWritten );
                            aXMLFile.write( NodeEnd, strlen( NodeEnd ), nWritten );
                        }

                        ++pIter;
                    }
                }

            }

            aXMLFile.write( SetTextEnd, strlen( SetTextEnd ), nWritten );
            aXMLFile.write( GroupTextEnd, strlen( GroupTextEnd ), nWritten );
            aXMLFile.write( ComponentDataEnd, strlen( ComponentDataEnd ), nWritten );

            aXMLFile.close();
            ++i;
        }
    }

    return true;
}

bool WriteLevel( osl::File& rFile, int nLevel )
{
    const char cTab[] = "\t";

    sal_uInt64 nWritten;
    for ( int i = 0; i < nLevel; i++ )
        rFile.write( cTab, strlen( cTab ), nWritten );

    return true;
}

bool WriteSeparator( osl::File& rFile, int nLevel )
{
    static const char MenuSeparator[] = "<menu:menuseparator/>\n";

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuSeparator, strlen( MenuSeparator ), nWritten );

    return true;
}

bool WriteMenuItem( osl::File& rFile, const OUString& aCmd, const OUString& aHelpID, int nLevel )
{
    static const char MenuItemStart[]   = "<menu:menuitem menu:id=\"";
    static const char MenuItemHelp[]    = "menu:helpid=\"";
    static const char MenuItemLabel[]   = "menu:label=\"";
    static const char MenuItemAttrEnd[] = "\" ";
    static const char MenuItemEnd[]     = "/>\n";

    OString aCmdStr = OUStringToOString( aCmd, RTL_TEXTENCODING_UTF8 );
    OString aHelpId = OUStringToOString( aHelpID, RTL_TEXTENCODING_UTF8 );

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuItemStart, strlen( MenuItemStart ), nWritten );
    rFile.write( aCmdStr.getStr(), aCmdStr.getLength(), nWritten );
    rFile.write( MenuItemAttrEnd, strlen( MenuItemAttrEnd ), nWritten );
    rFile.write( MenuItemHelp, strlen( MenuItemHelp ), nWritten );
    rFile.write( aHelpId.getStr(), aHelpId.getLength(), nWritten );
    rFile.write( MenuItemAttrEnd, strlen( MenuItemAttrEnd ), nWritten );
    rFile.write( MenuItemLabel, strlen( MenuItemLabel ), nWritten );
    rFile.write( MenuItemAttrEnd, strlen( MenuItemAttrEnd ), nWritten );
    rFile.write( MenuItemEnd, strlen( MenuItemEnd ), nWritten );

    return true;
}

bool WritePopupMenuEmpty( osl::File& rFile, int nLevel )
{
    static const char MenuPopupStart[]  = "<menu:menupopup/>\n";

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuPopupStart, strlen( MenuPopupStart ), nWritten );

    return true;
}

bool WritePopupMenuStart( osl::File& rFile, int nLevel )
{
    static const char MenuPopupStart[]  = "<menu:menupopup>\n";

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuPopupStart, strlen( MenuPopupStart ), nWritten );

    return true;
}

bool WritePopupMenuEnd( osl::File& rFile, int nLevel )
{
    static const char MenuPopupEnd[]    = "</menu:menupopup>\n";

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuPopupEnd, strlen( MenuPopupEnd ), nWritten );

    return true;
}

bool WriteMenuStart( osl::File& rFile, const OUString& aCmd, int nLevel )
{
    static const char MenuStart[]       = "<menu:menu menu:id=\"";
    static const char MenuEnd[]         = ">\n";
    static const char MenuAttrEnd[]     = "\" ";
    static const char MenuLabel[]       = "menu:label=\"";

    OString aCmdStr = OUStringToOString( aCmd, RTL_TEXTENCODING_UTF8 );

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuStart, strlen( MenuStart ), nWritten );
    rFile.write( aCmdStr.getStr(), aCmdStr.getLength(), nWritten );
    rFile.write( MenuAttrEnd, strlen( MenuAttrEnd ), nWritten );
    rFile.write( MenuLabel, strlen( MenuLabel ), nWritten );
    rFile.write( MenuAttrEnd, strlen( MenuAttrEnd ), nWritten );
    rFile.write( MenuEnd, strlen( MenuEnd ), nWritten );

    return true;
}

bool WriteMenuEnd( osl::File& rFile, int nLevel )
{
    static const char MenuEnd[]         = "</menu:menu>\n";

    sal_uInt64 nWritten;
    WriteLevel( rFile, nLevel );
    rFile.write( MenuEnd, strlen( MenuEnd ), nWritten );

    return true;
}

bool WriteMenuBarXML( osl::File& rFile, Menu* pMenu, MODULES eModule, int nLevel )
{
   OUString aCmd( RTL_CONSTASCII_USTRINGPARAM( ".uno:" ));

    for ( unsigned short n = 0; n < pMenu->GetItemCount(); n++ )
    {
        unsigned short nId = pMenu->GetItemId( n );
        if ( pMenu->GetItemType( n ) != MENUITEM_SEPARATOR )
        {
            OUString aLabel( pMenu->GetItemText( nId ));
            OUString aCommand( pMenu->GetItemCommand( nId ));
            bool     bHasUnoCommand( aCommand.getLength() > 0 && ( aCommand.indexOf( aCmd ) == 0 ));

            if ( pMenu->GetPopupMenu( nId ) != 0 )
            {
                CommandLabels* pCmdLabels = 0;
                pCmdLabels = RetrieveCommandLabelsFromID( nId, eModule );
                if ( !pCmdLabels )
                    pCmdLabels = RetrieveCommandLabelsFromCommand( aCommand, eModule );

                if ( pCmdLabels )
                    aCommand = pCmdLabels->aCommand;
                else
                    aCommand = OUString( RTL_CONSTASCII_USTRINGPARAM( "???" ));

                WriteMenuStart( rFile, aCommand, nLevel );
                WritePopupMenuStart( rFile, nLevel+1 );
                WriteMenuBarXML( rFile, pMenu->GetPopupMenu( nId ), eModule, nLevel+2 );
                WritePopupMenuEnd( rFile, nLevel+1 );
                WriteMenuEnd( rFile, nLevel );
            }
            else if ( pMenu->IsMenuBar() )
            {
                // write empty popup menu
                WriteMenuStart( rFile, aCommand, nLevel );
                WritePopupMenuEmpty( rFile, nLevel+1 );
                WriteMenuEnd( rFile, nLevel );
            }
            else
            {
                CommandLabels* pCmdLabels = RetrieveCommandLabelsFromID( nId, eModule );
                if ( !pCmdLabels )
                    pCmdLabels = RetrieveCommandLabelsFromCommand( aCommand, eModule );

                if ( pCmdLabels )
                    aCommand = pCmdLabels->aCommand;
                else
                    aCommand = OUString( RTL_CONSTASCII_USTRINGPARAM( "???" ));

                OUString aHelpId = OUString::valueOf( sal_Int32( pMenu->GetHelpId( nId )));
                WriteMenuItem( rFile, aCommand, aHelpId, nLevel );
            }
        }
        else
        {
            WriteSeparator( rFile, nLevel );
        }
    }

    return true;
}

bool ReadResourceWriteMenuBarXML( const OUString& aOutDirURL,
                                  const OUString& aResourceDirURL,
                                  const OUString& aResourceFilePrefix,
                                  MODULES eModule,
                                  const OUString& aProjectName )
{
    static const char MenuBarStart[]    = "<menu:menubar xmlns:menu=\"http://openoffice.org/2001/menu\" menu:id=\"menubar\">\n";
    static const char MenuBarEnd[]      = "</menu:menubar>";

    OUString aSysDirPath;
    OString  aResFilePrefix = OUStringToOString( aResourceFilePrefix, RTL_TEXTENCODING_ASCII_US );

    osl::FileBase::getSystemPathFromFileURL( aResourceDirURL, aSysDirPath );

    String aSysDirPathStr( aSysDirPath );

    String aLangString( String::CreateFromAscii( Language_Mapping[0].pISO ));
    LanguageType aLangtype = ConvertIsoStringToLanguage( aLangString );
    ResMgr* pResMgr = ResMgr::CreateResMgr( aResFilePrefix.getStr(),
                                            aLangtype,
                                            NULL,
                                            &aSysDirPathStr );
    if ( pResMgr )
    {
        OUString aMenuBar( RTL_CONSTASCII_USTRINGPARAM( "menubar" ));
        int i = 0;
        while ( ResourceModule_Mapping[i].nResId > 0 )
        {
            if ( ResourceModule_Mapping[i].eBelongsTo == eModule )
            {
                ResId   aResId( ResourceModule_Mapping[i].nResId, pResMgr );
                aResId.SetRT( RSC_MENU );

                if ( pResMgr->IsAvailable( aResId ))
                {
                    OUString aOutputDirectoryURL( aOutDirURL );
                    if ( aOutputDirectoryURL.getLength() > 0 && aOutputDirectoryURL[aOutputDirectoryURL.getLength()-1] != '/' )
                        aOutputDirectoryURL += OUString::createFromAscii( "/" );

                    OUString aOutputFileURL( aOutputDirectoryURL );
                    aOutputFileURL += OUString::createFromAscii( ResourceModule_Mapping[i].pXMLPrefix );
                    aOutputFileURL += aMenuBar;
                    aOutputFileURL += OUString::createFromAscii( ".xml" );

                    osl::File aXMLFile( aOutputFileURL );
                    osl::File::RC nRet = aXMLFile.open( OpenFlag_Create|OpenFlag_Write );
                    if ( nRet == osl::File::E_EXIST )
                    {
                        nRet = aXMLFile.open( OpenFlag_Write );
                        if ( nRet == osl::File::E_None )
                            nRet = aXMLFile.setSize( 0 );
                    }

                    if ( nRet == osl::FileBase::E_None )
                    {
                        sal_uInt64 nWritten;

                        aXMLFile.write( XMLStart, strlen( XMLStart ), nWritten );
                        aXMLFile.write( MenuBarStart, strlen( MenuBarStart ), nWritten );

                        MenuBar* pMenuBar = new MenuBar( aResId );
                        WriteMenuBarXML( aXMLFile, (Menu *)pMenuBar, eModule, 1 );
                        delete pMenuBar;

                        aXMLFile.write( MenuBarEnd, strlen( MenuBarEnd ), nWritten );
                        aXMLFile.close();
                    }
                }
            }
            ++i;
        }

        delete pResMgr;
    }

    return true;
}

bool Convert( sal_Bool        bUseProduct,
              const OUString& aUseRes,
              const OUString& rVersion,
              const OUString& rOutputDirName,
              const OUString& rPlatformName,
              const std::vector< OUString >& rInDirVector,
              const OUString& rErrOutputFileName )
{
    OUString aWorkDir;

    osl_getProcessWorkingDir( &aWorkDir.pData );

    // Try to find xx*.csv file and put all commands into hash table
    for ( int i = 0; i < (int)rInDirVector.size(); i++ )
    {
        OUString aAbsInDirURL;
        OUString aInDirURL;
        OUString aInDir( rInDirVector[i] );

        osl::FileBase::getFileURLFromSystemPath( aInDir, aInDirURL );
        osl::FileBase::getAbsoluteFileURL( aWorkDir, aInDirURL, aAbsInDirURL );
        osl::Directory aDir( aAbsInDirURL );
        if ( aDir.open() == osl::FileBase::E_None )
        {
            osl::DirectoryItem aItem;
            while ( aDir.getNextItem( aItem ) == osl::FileBase::E_None )
            {
                osl::FileStatus aFileStatus( FileStatusMask_FileName );

                aItem.getFileStatus( aFileStatus );

                int j=0;
                OUString aFileName = aFileStatus.getFileName();

                while ( ProjectModule_Mapping[j].pProjectFolder != 0 &&
                        ProjectModule_Mapping[j].bSlotCommands == true )
                {
                    if ( aFileName.equalsAscii( ProjectModule_Mapping[j].pProjectFolder ))
                    {
                        OUStringBuffer aBuf( aAbsInDirURL );

                        aBuf.appendAscii( "/" );
                        aBuf.append( aFileStatus.getFileName() );
                        aBuf.appendAscii( "/" );
                        aBuf.append( rPlatformName );
                        if ( bUseProduct )
                            aBuf.appendAscii( ".pro" );
                        aBuf.appendAscii( "/misc/xx" );
                        aBuf.appendAscii( ProjectModule_Mapping[j].pCSVPrefix );
                        aBuf.appendAscii( ".csv" );

                        OUString aCSVFileURL( aBuf.makeStringAndClear() );
                        ReadCSVFile( aCSVFileURL, ProjectModule_Mapping[j].eBelongsTo, OUString::createFromAscii( ProjectModule_Mapping[j].pProjectFolder ));
                        break;
                    }
                    j++;
                }
            }

            aDir.close();
        }
    }

    OUString aVersionNumber;
    bool bOk = ExtractVersionNumber( rVersion, aVersionNumber );
    if ( bOk )
    {
        // Try to find the resource files and read in menus and slot infos
        for ( int i = 0; i < (int)rInDirVector.size(); i++ )
        {
            OUString aAbsInDirURL;
            OUString aInDirURL;
            OUString aInDir( rInDirVector[i] );

            osl::FileBase::getFileURLFromSystemPath( aInDir, aInDirURL );
            osl::FileBase::getAbsoluteFileURL( aWorkDir, aInDirURL, aAbsInDirURL );
            osl::Directory aDir( aAbsInDirURL );
            if ( aDir.open() == osl::FileBase::E_None )
            {
                osl::DirectoryItem aItem;
                while ( aDir.getNextItem( aItem ) == osl::FileBase::E_None )
                {
                    osl::FileStatus aFileStatus( FileStatusMask_FileName );

                    aItem.getFileStatus( aFileStatus );

                    int j=0;
                    OUString aFileName = aFileStatus.getFileName();

                    while ( ProjectModule_Mapping[j].pProjectFolder != 0 )
                    {
                        if ( aFileName.equalsAscii( ProjectModule_Mapping[j].pProjectFolder ))
                        {
                            OUStringBuffer aBuf( aAbsInDirURL );

                            aBuf.appendAscii( "/" );
                            aBuf.append( aFileStatus.getFileName() );
                            aBuf.appendAscii( "/common" );
                            if ( bUseProduct )
                                aBuf.appendAscii( ".pro" );
                            aBuf.appendAscii( "/bin/" );
                            if ( aUseRes.equalsAscii( "so" ))
                                aBuf.appendAscii( "so/" );

                            OUString aResDirURL( aBuf.makeStringAndClear() );

                            OUStringBuffer aLangResBuf;
                            aLangResBuf.appendAscii( ProjectModule_Mapping[j].pResPrefix );
                            aLangResBuf.append( aVersionNumber );
                            OUString aLangResPrefix( aLangResBuf.makeStringAndClear() );

                            for ( int k = 0; k < NUM_LANGUAGES; k++ )
                            {
                                char aBuf[8] = {0};

                                ReadResourceFile( k,
                                                  aResDirURL,
                                                  aLangResPrefix,
                                                  ProjectModule_Mapping[j].eBelongsTo,
                                                  OUString::createFromAscii( ProjectModule_Mapping[j].pProjectFolder ),
                                                  ProjectModule_Mapping[j].bSlotCommands );
                            }

                            OUString aOutDirURL;
                            osl::FileBase::getFileURLFromSystemPath( rOutputDirName, aOutDirURL );
                            osl::FileBase::getAbsoluteFileURL( aWorkDir, aOutDirURL, aOutDirURL );

                            ReadResourceWriteMenuBarXML( aOutDirURL, aResDirURL, aLangResPrefix, ProjectModule_Mapping[j].eBelongsTo, OUString::createFromAscii( ProjectModule_Mapping[j].pProjectFolder ));
                        }
                        j++;
                    }
                }

                aDir.close();
            }
        }
    }

    OUString aOutDirURL;
    osl::FileBase::getFileURLFromSystemPath( rOutputDirName, aOutDirURL );
    osl::FileBase::getAbsoluteFileURL( aWorkDir, aOutDirURL, aOutDirURL );

    WriteXMLFiles( aOutDirURL );

    return true;
}

bool GetCommandOption( const ::std::vector< OUString >& rArgs, const OUString& rSwitch, OUString& rParam )
{
    bool        bRet = false;
    OUString    aSwitch( OUString::createFromAscii( "-" ));

    aSwitch += rSwitch;
    for( int i = 0, nCount = rArgs.size(); ( i < nCount ) && !bRet; i++ )
    {
        for( int n = 0; ( n < 2 ) && !bRet; n++ )
        {
            if ( aSwitch.equalsIgnoreAsciiCase( rArgs[ i ] ))
            {
                bRet = true;

                if( i < ( nCount - 1 ) )
                    rParam = rArgs[ i + 1 ];
                else
                    rParam = OUString();
            }
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------

bool GetCommandOptions( const ::std::vector< OUString >& rArgs, const OUString& rSwitch, ::std::vector< OUString >& rParams )
{
    bool bRet = false;

    OUString    aSwitch( OUString::createFromAscii( "-" ));

    aSwitch += rSwitch;
    for( int i = 0, nCount = rArgs.size(); ( i < nCount ); i++ )
    {
        for( int n = 0; ( n < 2 ) && !bRet; n++ )
        {
            if ( aSwitch.equalsIgnoreAsciiCase( rArgs[ i ] ))
            {
                if( i < ( nCount - 1 ) )
                    rParams.push_back( rArgs[ i + 1 ] );
                else
                    rParams.push_back( OUString() );

                break;
            }
        }
    }

    return( rParams.size() > 0 );
}

void ShowUsage()
{
    fprintf( stderr, "Usage: uicmdxml output_dir -r res -v version [-p] -s platform -i input_dir [-i input_dir] [-f errfile]\n" );
    fprintf( stderr, "Options:" );
    fprintf( stderr, "   -r [oo|so]    use resources from ooo, so\n" );
    fprintf( stderr, "   -v            name of the version used, f.e. SRX645, SRC680\n" );
    fprintf( stderr, "   -p            use product version\n" );
    fprintf( stderr, "   -s            name of the system to use, f.e. wntmsci8, unxsols4, unxlngi5\n" );
    fprintf( stderr, "   -i ...        name of directory to be searched for input files [multiple occurence is possible]\n" );
    fprintf( stderr, "   -f            name of file, error output should be written to\n" );
    fprintf( stderr, "Examples:\n" );
    fprintf( stderr, "    uicmdxml /home/out -r so -v SRC680 -p unxlngi5 -i /home/res -f /home/out/log.err\n" );
}

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
// -----------------------------------------------------------------------

// Forward declaration
void Main();

// -----------------------------------------------------------------------

SAL_IMPLEMENT_MAIN()
{
    Reference< XMultiServiceFactory > xMS;

    // for this to work make sure an <appname>.ini file is available, you can just copy soffice.ini
    Reference< XComponentContext > xComponentContext = ::cppu::defaultBootstrap_InitialComponentContext();
    xMS = Reference< XMultiServiceFactory >( xComponentContext->getServiceManager() , UNO_QUERY );

    InitVCL( xMS );
    ::Main();
    DeInitVCL();

    return 0;
}

void Main()
{
    ::std::vector< OUString > aArgs;

    sal_uInt32 nCmds = osl_getCommandArgCount();
    if ( nCmds >= 8 )
    {
        for ( sal_uInt32 i=0; i < nCmds; i++ )
        {
            OUString aArg;
            osl_getCommandArg( i, &aArg.pData );
            aArgs.push_back( aArg );
        }

        ::std::vector< OUString > aInDirVector;
        OUString                  aErrOutputFileName;
        OUString                  aOutputDirName( aArgs[0] );
        OUString                  aPlatformName;
        OUString                  aVersion;
        OUString                  aUseRes;
        bool                      bUseProduct;
        OUString                  aDummy;

        GetCommandOption( aArgs, OUString::createFromAscii( "v" ), aVersion );
        bUseProduct = GetCommandOption( aArgs, OUString::createFromAscii( "p" ), aDummy );
        GetCommandOption( aArgs, OUString::createFromAscii( "s" ), aPlatformName );
        GetCommandOptions( aArgs, OUString::createFromAscii( "i" ), aInDirVector );
        GetCommandOption( aArgs, OUString::createFromAscii( "f" ), aErrOutputFileName );
        GetCommandOption( aArgs, OUString::createFromAscii( "r" ), aUseRes );

        if ( aVersion.getLength() > 0 &&
             aPlatformName.getLength() > 0 &&
             aUseRes.getLength() > 0 &&
             aInDirVector.size() > 0 )
        {
            Convert( bUseProduct, aUseRes, aVersion, aOutputDirName, aPlatformName, aInDirVector, aErrOutputFileName );
        }
        else
        {
            ShowUsage();
            exit( -1 );
        }
    }
    else
    {
        ShowUsage();
        exit( -1 );
    }
}
